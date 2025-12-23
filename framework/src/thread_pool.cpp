/*
DESCRIPTION:    thread pool implementation
AUTHOR:         Rayan
VERSION:
    v0.1
 */
#include <iostream> // cout
#include "thread_pool.hpp"

namespace hrd31
{
    class ThreadPool::PauseTask : public ITask
    {
    public:
        explicit PauseTask(
            ThreadPool *this_, std::condition_variable &cv_)
            : ITask(ITask::ADMIN), m_current(this_), m_cv(cv_){};

        PauseTask(const PauseTask &) = delete;
        PauseTask &operator=(const PauseTask &) = delete;
        ~PauseTask() = default;

        void Execute() override
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]
                      { return !m_current->m_in_pause; });
        }

    private:
        ThreadPool *m_current;
        std::mutex m_mutex;
        std::condition_variable &m_cv;
    };

    class ThreadPool::StopTask : public ITask
    {
    public:
        explicit StopTask() : ITask(ITask::ADMIN) {}

        StopTask(const StopTask &) = delete;
        StopTask &operator=(const StopTask &) = delete;
        ~StopTask() = default;

        void Execute() override
        {
            throw std::runtime_error("thread stop");
        }

    private:
    };

    // static
    void ThreadPool::WorkerFunc()
    {
        try
        {
            while (1)
            {
                std::shared_ptr<ITask> task = m_tasks.Pop();
                task->Execute();
            }
        }
        catch (std::runtime_error &e)
        {
            // swallow exception
        }
    }

    ThreadPool::ThreadPool(size_t size_)
        : m_pool(),
          m_tasks([](task_ptr p1_, task_ptr p2_)
                  { return *(p1_) < *(p2_); }),
          m_pool_size(size_),
          m_in_pause(false),
          is_stopeed(false)
    {
        for (size_t i = 0; i < size_; ++i)
        {
            m_pool.emplace_back(std::thread(&ThreadPool::WorkerFunc, this));
        }
    }

    ThreadPool::~ThreadPool() noexcept
    {
        if (!is_stopeed)
        {
            Stop();
        }
    }

    void ThreadPool::AddTask(std::shared_ptr<ITask> task_)
    {
        m_tasks.Push(task_);
    }

    void ThreadPool::SetSize(size_t new_size_)
    {
        if (new_size_ < m_pool_size)
        {
            // if in pause and we want decrease size, we just update the size.
            if (!m_in_pause)
            {
                size_t diff = m_pool_size - new_size_;
                while (diff)
                {
                    std::shared_ptr<ThreadPool::PauseTask> pause_task(new PauseTask(this, m_cv));
                    m_tasks.Push(pause_task);
                    --diff;
                }
            }
        }
        else if (new_size_ > m_pool_size)
        {
            // increase thread pool size
            // if we are in pause and we want to increase size, push pause tasks first
            // then increase thread pool
            if (m_in_pause)
            {
                for (size_t i = m_pool_size; i < new_size_; ++i)
                {
                    std::shared_ptr<ThreadPool::PauseTask> pause_task(new PauseTask(this, m_cv));
                    m_tasks.Push(pause_task);
                }
            }

            // if we are not in pause we dont need pause tasks
            // just emplace new threads
            for (size_t i = m_pool_size; i < new_size_; ++i)
            {
                m_pool.emplace_back(std::thread(&ThreadPool::WorkerFunc, this));
            }
        }
        m_pool_size = new_size_;
    }

    void ThreadPool::Resume()
    {

        m_in_pause = false;

        for (size_t i = 0; i < m_pool_size; ++i)
        {
            m_cv.notify_one();
        }
    }

    void ThreadPool::Stop()
    {

        // push stop as thread pool size
        for (size_t i = 0; i < m_pool.size(); ++i)
        {
            std::shared_ptr<ThreadPool::StopTask> stop_task(new StopTask());
            m_tasks.Push(stop_task);
        }

        m_pool_size = m_pool.size();

        // then resume all
        Resume();

        // join threads gracefully
        for (size_t i = 0; i < m_pool.size(); ++i)
        {
            if (m_pool[i].joinable())
            {
                m_pool[i].join();
            }
        }

        is_stopeed = true;
    }

    void ThreadPool::TimedStopper(ThreadPool *this_, std::chrono::seconds timeout_) // static
    {
        // push stop as thread pool size
        for (size_t i = 0; i < this_->m_pool.size(); ++i)
        {
            std::shared_ptr<ThreadPool::StopTask> stop_task(new StopTask());
            this_->m_tasks.Push(stop_task);
        }

        // resume all if they were in pause
        this_->Resume();

        // sleep and give chance to gracefull stopping
        std::this_thread::sleep_for(timeout_);

        // force stop all
        this_->m_pool.clear();
    }

    void ThreadPool::Stop(std::chrono::seconds timeout_)
    {
        std::thread(TimedStopper, this, timeout_);
        is_stopeed = true;
    }

    void ThreadPool::Pause()
    {

        for (size_t i = 0; i < m_pool_size; ++i)
        {
            std::shared_ptr<ThreadPool::PauseTask> pause_task(new PauseTask(this, m_cv));
            m_tasks.Push(pause_task);
        }
        m_in_pause = true;
    }

    size_t ThreadPool::DefaultThreadsNum() // static
    {
        size_t hcsize = std::thread::hardware_concurrency();
        return hcsize == 0 ? 1 : hcsize;
    }

} // namespace hrd31