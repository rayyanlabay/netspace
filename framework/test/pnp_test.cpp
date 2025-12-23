#include <string>
#include <iostream>

#include <unistd.h>

#include "factory.hpp"
#include "msg_broker.hpp"
#include "pnp.hpp"
#include "i_task.hpp"
using namespace abc;

class Subscriber
{
public:
    void Print(const std::string &str_)
    {
        std::cout << str_ << std::endl;
    }

    explicit Subscriber(Dispatcher<std::string> &disp_)
        : call_back_(disp_, *this, &Subscriber::Print)
    {
    }

    CallBack<std::string, Subscriber> call_back_;
};

class Publisher
{
public:
    explicit Publisher(Dispatcher<std::string> *disp_)
    {
        m_disp = disp_;
    }

    void NotifyAll(const std::string &msg_)
    {
        m_disp->NotifyAll(msg_);
    }

    Dispatcher<std::string> *m_disp;
};


int main()
{
    PlugNPlay pnp;

    auto *factory = Singleton<Factory<ITask, int, int>>::GetInstance();
    // (void)factory;
    sleep(5);
    std::shared_ptr<ITask> a = factory->Create(1, 2);
    // std::shared_ptr<ITask> b = factory->Create(2, 2);

    // must print foo
    // a->Execute();

    // // must print bar
    // b->Execute();
    sleep(5);

    // add testing with the thread pool 
    


    return 0;
}
