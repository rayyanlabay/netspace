/* -----------------------------------------------------------------------------
Description: Request Engine Frame-Work
Author: HRD31
Reviewer: Itzik
Version:
    v0.1 - For mentor approval
    v0.2 - Mentor approved
    v0.2.1 - class Stop and Pause are changed to PauseTask and StopTask
----------------------------------------------------------------------------- */
#ifndef __HRD31_RE_FW_HPP__
#define __HRD31_RE_FW_HPP__

#include <string> // std::string
#include <memory> //std::shared_ptr, std::make_shared
#include <unordered_map> // std::unordered_map
#include <iostream> // std::cout
#include <stdexcept> // std::runtime_error
#include <utility> 
#include <functional> // std::function
#include <unistd.h> // close

#include <sys/epoll.h> // epoll_create, epoll_ctl, epoll_wait

#include "factory.hpp"
#include "thread_pool.hpp"
#include "pnp.hpp"
#include "i_input_src.hpp"
#include "i_re_task.hpp"

#include "utils.h"

namespace hrd31
{
    enum
    {
        READ_PIPE = 0,
        WRITE_PIPE = 1,
        MAX_EVENTS = 100

    };

    template <typename KEY, typename ARGS>
    class RequestEngine final
    {
    public:
        // DefaultThreadsNum - returns the number of concurrent threads the
        // implementation supports;
        // if the value is not well defined it returns 1.
        explicit RequestEngine(size_t thread_num_ = DefaultThreadsNum(),
                               const std::string &pnp_path_ = "./pnp");
        ~RequestEngine();
        RequestEngine(const RequestEngine &) = delete;
        RequestEngine &operator=(const RequestEngine &) = delete;

        void ConfigInputSrc(std::shared_ptr<IInputSrc<KEY, ARGS>> input_src_);
        using CreateFunc = std::function<std::shared_ptr<IRETask>(ARGS)>;
        void ConfigTask(const KEY &key_, CreateFunc func_);
        void Stop();
        void Run();

    private:
        static size_t DefaultThreadsNum();
        void ReactorThread();
        Factory<IRETask, KEY, ARGS> *m_factory;
        ThreadPool m_thread_pool;
        PlugNPlay m_pnp;
        std::thread m_reactor;
        std::unordered_map<int, std::shared_ptr<IInputSrc<KEY, ARGS>>> m_request_srcs;
        int m_epoll_fd;
        std::vector<int> m_close_epoll_fd;
    };

    template <typename KEY, typename ARGS>
    size_t RequestEngine<KEY, ARGS>::DefaultThreadsNum() // static
    {
        size_t hcsize = std::thread::hardware_concurrency();
        return hcsize == 0 ? 1 : hcsize;
    } // DefaultThreadNum

    static std::vector<int> InitAndAddPipeToEpoll(int epoll_fd_)
    {
        std::vector<int> res(2);
        // create pipe to communicate with the epoll
        CheckFailAndThrow<std::runtime_error>(0 == pipe(res.data()), "pipe error",__FILE__,__LINE__);
        struct epoll_event ev, events[MAX_EVENTS];
        (void)(events);
        // (void)(m_close_epoll_fd);

        // add the closing pipe to epoll
        ev.events = EPOLLIN;
        ev.data.fd = res[READ_PIPE];

        CheckFailAndThrow<std::runtime_error>(
            -1 != epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, res[READ_PIPE], &ev), "epoll ctlf error",__FILE__,__LINE__);

        return res;
    }

    // ctor
    template <typename KEY, typename ARGS>
    RequestEngine<KEY, ARGS>::RequestEngine(size_t thread_num_,
                                            const std::string &pnp_path_)

        : m_factory(Singleton<Factory<IRETask, KEY, ARGS>>::GetInstance()),
          m_thread_pool(thread_num_),
          m_pnp(pnp_path_),
          m_request_srcs(),
          m_epoll_fd(epoll_create1(0)),
          m_close_epoll_fd(InitAndAddPipeToEpoll(m_epoll_fd))
    {
    }

    // dtor
    template <typename KEY, typename ARGS>
    RequestEngine<KEY, ARGS>::~RequestEngine()
    {
        if (m_reactor.joinable())
        {
            m_reactor.join();
        }

        close(m_close_epoll_fd[WRITE_PIPE]);
        close(m_close_epoll_fd[READ_PIPE]);

        close(m_epoll_fd);
    }

    template <typename KEY, typename ARGS>
    void RequestEngine<KEY, ARGS>::ConfigInputSrc(std::shared_ptr<IInputSrc<KEY, ARGS>> input_src_)
    {
        // for each fd push into the hash table where the key is the fd
        int src_fd = input_src_->GetFd();
        m_request_srcs[src_fd] = input_src_;

        // each fd push into the epoll
        struct epoll_event ev, events[MAX_EVENTS];
        (void)(events);
        // add the newly given fd to the epoll
        ev.events = EPOLLIN; //| EPOLLET;
        ev.data.fd = src_fd;
        CheckFailAndThrow<std::runtime_error>(
            -1 != epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, src_fd, &ev), "epoll ctl error",__FILE__,__LINE__);
    }

    template <typename KEY, typename ARGS>
    using CreateFunc = std::function<std::shared_ptr<IRETask>(ARGS)>;
    template <typename KEY, typename ARGS>
    void RequestEngine<KEY, ARGS>::ConfigTask(const KEY &key_, CreateFunc func_)
    {
        m_factory->Add(key_, func_);
    }

    template <typename KEY, typename ARGS>
    void RequestEngine<KEY, ARGS>::Stop()
    {
        char buf[10] = {0};

        CheckFailAndThrow<std::runtime_error>(
            -1 != write(m_close_epoll_fd[WRITE_PIPE], &buf, 1), "write error",__FILE__,__LINE__);

        epoll_event ev;
        for (auto pair : m_request_srcs)
        {
            epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, pair.first, &ev);
            close(pair.first);
        }
    }

    template <typename KEY, typename ARGS>
    void RequestEngine<KEY, ARGS>::ReactorThread()
    {
        int n = 0, nfds = 0;
        bool is_reactor_running = true;
        struct epoll_event ev, events[MAX_EVENTS];
        (void)ev;
        while (is_reactor_running)
        {

            CheckFailAndThrow<std::runtime_error>(-1 != (nfds = epoll_wait(m_epoll_fd, events, MAX_EVENTS, -1)), "epoll wait error",__FILE__,__LINE__);

            for (n = 0; n < nfds; ++n)
            {
                if (events[n].data.fd == m_close_epoll_fd[READ_PIPE])
                {
                    is_reactor_running = false;
                    break;
                }
                else if (events[n].events & EPOLLIN)
                {
                    auto res = m_request_srcs[events[n].data.fd]->Read();
                    m_thread_pool.AddTask(m_factory->Create(res.first, res.second));
                }
            }
        }
    }

    template <typename KEY, typename ARGS>
    void RequestEngine<KEY, ARGS>::Run()
    {
        m_reactor = std::thread(&RequestEngine<KEY, ARGS>::ReactorThread, this);
    }

} // namespace hrd31

#endif //__HRD31_RE_FW_HPP__