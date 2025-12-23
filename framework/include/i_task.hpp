#ifndef __ITASK_HPP__
#define __ITASK_HPP__

#include "thread_pool.hpp"

namespace abc
{

    class ITask
    {
    public:
        enum Priority
        {
            LOW,
            MEDIUM,
            HIGH,
            ADMIN = __INT_MAX__ // a new priority can be added before ADMIN.
            //  ADMIN should always be the last priority for internal use
        };

        explicit ITask(Priority priority_ = MEDIUM);
        ITask(const ITask &other_) = default;
        ITask &operator=(const ITask &other_) = delete;
        virtual ~ITask();

        bool operator<(const ITask &other_) const;

    private:
        friend class ThreadPool;

        virtual void Execute() = 0;

        Priority m_priority;
    };

    inline ITask::ITask(ITask::Priority priority_) : m_priority(priority_) {}
    inline ITask::~ITask(){};
    inline bool ITask::operator<(const ITask &other_) const
    {
        return m_priority < other_.m_priority;
    }
} // namespace abc

#endif //__ITASK_HPP__