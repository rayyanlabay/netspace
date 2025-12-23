/* -----------------------------------------------------------------------------
Description:
Author: HRD31
Reviewer: Itzik
Version:
    v0.1 - For mentor approval
    v0.2 - Approved
----------------------------------------------------------------------------- */
#ifndef __HRD31_PLUG_N_PLAY_HPP__
#define __HRD31_PLUG_N_PLAY_HPP__

#include <string> //std::string
#include <thread> //std::thread
#include <memory> //std::unique_ptr

#include "msg_broker.hpp"

namespace hrd31
{

    class PlugNPlay
    {
    public:
        explicit PlugNPlay(const std::string &path_ = "./dynamic_libs/pnp");
        ~PlugNPlay() noexcept;

        PlugNPlay(const PlugNPlay &other_) = delete;
        PlugNPlay &operator=(const PlugNPlay &other_) = delete;

    private:
        class DirMonitor;
        class SOLoader;

        Dispatcher<std::string> m_dispatcher;

        std::unique_ptr<DirMonitor> m_monitor;
        std::unique_ptr<SOLoader> m_loader;
    };

    class PlugNPlay::DirMonitor
    {
    public:
        explicit DirMonitor(Dispatcher<std::string> &dispatcher_,
                            const std::string &path_ = "./dynamic_libs/pnp");
        ~DirMonitor() noexcept;

        DirMonitor(const DirMonitor &other_) = delete;
        DirMonitor &operator=(const DirMonitor &other_) = delete;

    private:
        int m_wd;
        int m_dir_fd;
        Dispatcher<std::string> &m_dispatcher;
        std::thread m_async_listener;
        void ListenerThread(int, const std::string &);
    };

    class PlugNPlay::SOLoader
    {
    public:
        explicit SOLoader(Dispatcher<std::string> &dispatcher_);
        ~SOLoader() noexcept;

        SOLoader(const SOLoader &other_) = delete;
        SOLoader &operator=(const SOLoader &other_) = delete;

    private:
        CallBack<std::string, SOLoader> m_call_back;
        void Load(const std::string &file_name_);
        std::vector<void *> m_handles;
    };

} // namespace hrd31

#endif //__HRD31_PLUG_N_PLAY_HPP__