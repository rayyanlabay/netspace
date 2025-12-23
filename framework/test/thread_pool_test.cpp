/* 
DESCRIPTION:    thread pool test
AUTHOR:         Rayan
VERSION:        
    v0.1
 */
#include <iostream> // cout
#include <unistd.h> // sleep
#include <chrono> // chrono::seconds
#include <thread> // std thread
#include "thread_pool.hpp"

using namespace hrd31;

class SimpleTask : public ITask
{
public:
    SimpleTask(int i_ = 0) : m_i(i_)
    {
    }

    void Execute() override
    {
        std::cout << "task " << m_i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

private:
    int m_i = 0;
};

#define TEST_NUM 10

int main()
{
    std::vector<std::shared_ptr<SimpleTask>> s;
    
    // add 10 simple tasks
    for (size_t i = 0; i < TEST_NUM; ++i)
    {
       s.emplace_back(new SimpleTask(i));
    }

    // create thread pool of size 4 
    ThreadPool thread_pool(4);

    // add the 10 tasks to the thread pool
    for (size_t i = 0; i < TEST_NUM; ++i)
    {
        thread_pool.AddTask(s[i]);
    }
        
    // pause all tasks
    thread_pool.Pause();

    thread_pool.SetSize(2);
    
    // resume all tasks
    thread_pool.Resume();

    sleep(3);

    // thread_pool.Stop(std::chrono::seconds(0));

    // gracefull stop
    thread_pool.Stop();

    return 0;
}
