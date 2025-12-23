#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

#include <memory>     // shared ptr
#include <vector>     // vector
#include <thread>     // thread
#include <functional> // std::function
#include <chrono>     // chrono

#include "waitable_pq.hpp"
#include "i_task.hpp"

namespace abc
{
    class ITask;

    class ThreadPool final
    {
    public:
        // DefaultThreadsNum - returns the number of concurrent threads the
        // implementation supports;
        // if the value is not well defined it returns 1.
        explicit ThreadPool(size_t size_ = DefaultThreadsNum());
        ~ThreadPool() noexcept;

        ThreadPool(const ThreadPool &other_) = delete;
        ThreadPool &operator=(const ThreadPool &other_) = delete;

        void AddTask(std::shared_ptr<ITask> task_);
        void SetSize(size_t new_size_);
        void Pause();
        void Resume();
        void Stop();
        void Stop(std::chrono::seconds timeout_);

    private:
        class PauseTask;
        class StopTask;

        using task_ptr = std::shared_ptr<ITask>;
        using Container = std::vector<task_ptr>;
        using Compare = std::function<bool(task_ptr p1_, task_ptr p2_)>;

        static size_t DefaultThreadsNum(); // wrapper for std::thread::hardware_concurrency()
        void WorkerFunc();
        static void TimedStopper(ThreadPool *this_, std::chrono::seconds timeout_);

        std::vector<std::thread> m_pool;

        WPQueue<task_ptr, Container, Compare> m_tasks;
        size_t m_pool_size;
        std::condition_variable m_cv;
        bool m_in_pause;
        bool is_stopeed;
    };

} // namespace abc

#endif //__THREAD_POOL_HPP__