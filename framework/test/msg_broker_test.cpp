#include <string>
#include <iostream>
#include "msg_broker.hpp"

using namespace abc;

class Subscriber
{
public:
    void Print(const std::string &str_)
    {
        std::cout << str_ << std::endl;
    }

    explicit Subscriber(Dispatcher<std::string> &disp_)
    : call_back_(disp_, *this, &Subscriber::Print) {
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
    Dispatcher<std::string> dispatcher;

    Publisher pb(&dispatcher);

    Subscriber sub(dispatcher);
    Subscriber sub1(dispatcher);
    Subscriber sub2(dispatcher);
    Subscriber sub3(dispatcher);

    pb.NotifyAll("test");
    pb.NotifyAll("test2");

    return 0;
}
