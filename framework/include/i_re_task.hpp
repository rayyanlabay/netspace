#ifndef __I_RE_TASK_HPP__
#define __I_RE_TASK_HPP__

#include "i_task.hpp"

namespace abc
{
//note: for using this interface for creating tasks you have to provide
// std::shared_ptr<IRETask> CreateFunc(ARGS) - not member function
//Available priorities: LOW, MEDIUM, HIGH
class IRETask : public ITask
{
public:
    explicit IRETask(Priority priority_ = MEDIUM) : ITask(priority_) {};
    virtual ~IRETask() = default;
    IRETask(const IRETask&) = default;
    IRETask& operator=(const IRETask&) = default;

private:
    virtual void Execute() = 0;
};

}//namespace abc

#endif //__I_RE_TASK_HPP__