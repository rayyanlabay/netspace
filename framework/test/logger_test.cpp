/* 
DESCRIPTION:    thread pool test
AUTHOR:         Rayan
VERSION:        
    v0.1
 */
#include <thread> // thread
#include <iostream> // ofstream, ostream
#include "singleton.hpp" 
#include "logger.hpp"

using namespace hrd31;

void ThreadFunc1(Logger *log)
{
    for (size_t i = 0; i < 100; ++i)
    {
        log->Log(Logger::ERROR, "thread 1 error", __FILE__, __LINE__);
    }
}

void ThreadFunc2(Logger *log)
{
    for (size_t i = 0; i < 100; ++i)
    {
        log->Log(Logger::DEBUG, "thread 2 info", __FILE__, __LINE__);
    }
}

int main()
{
    // make singleton instance
    Logger *log = Singleton<Logger>::GetInstance();
    log->SetFilePath();
    
    log->SetLogLevel(Logger::INFO);

    std::thread t1(ThreadFunc1, log);
    std::thread t2(ThreadFunc2, log);

    t1.join();
    t2.join();

    return 0;
}
