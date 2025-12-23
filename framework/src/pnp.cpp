#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gnu/lib-names.h>

#include <errno.h>
#include <poll.h>
#include <sys/inotify.h>
#include <cstring>

#include <iostream>
#include <string> //std::string
#include <thread> //std::thread
#include <memory> //std::unique_ptr

#include "pnp.hpp"

namespace abc
{

    enum
    {
        MAX_BUF_SIZE = 1024
    };

    PlugNPlay::PlugNPlay(const std::string &path_)
        : m_dispatcher(),
          m_monitor(new DirMonitor(m_dispatcher, path_)),
          m_loader(new SOLoader(m_dispatcher))
    {
    }

    PlugNPlay::~PlugNPlay() noexcept
    {
        // all data members are RAII/primitives
    }

    void PlugNPlay::DirMonitor::ListenerThread(int fd, const std::string &path_)
    {
        char buf[MAX_BUF_SIZE] = {0};

        m_wd = inotify_add_watch(fd, path_.c_str(), IN_CLOSE_WRITE);
        inotify_event *event = (inotify_event *)buf;

        while (0 < read(fd, buf, 256 + sizeof(struct inotify_event)))
        {
            m_dispatcher.NotifyAll(event->name);
        }
    }

    PlugNPlay::DirMonitor::DirMonitor(Dispatcher<std::string> &dispatcher_,
                                      const std::string &path_)
        : m_dir_fd(inotify_init1(IN_NONBLOCK)),
          m_dispatcher(dispatcher_),
          m_async_listener(&PlugNPlay::DirMonitor::ListenerThread, this, m_dir_fd, path_)
    {
    }

    PlugNPlay::DirMonitor::~DirMonitor() noexcept
    {

        // this will cause read to unblock
        inotify_rm_watch(m_dir_fd, m_wd);

        // close inotify descriptor
        close(m_dir_fd);

        // thread will exit and joined
        m_async_listener.join();
    }

    PlugNPlay::SOLoader::SOLoader(Dispatcher<std::string> &dispatcher_)
        : m_call_back(dispatcher_, *this, &PlugNPlay::SOLoader::Load)
    {
        // this will create the call back
        // it will automatically registers to the dispatcher
    }

    PlugNPlay::SOLoader::~SOLoader() noexcept
    {
        // close handles (shared objects)
        for (auto handle : m_handles)
        {
            dlclose(handle);
        }
    }

    // notify
    void PlugNPlay::SOLoader::Load(const std::string &file_name_)
    {
        m_handles.push_back(dlopen(file_name_.c_str(), RTLD_LAZY));
    }

} // namespace abc
