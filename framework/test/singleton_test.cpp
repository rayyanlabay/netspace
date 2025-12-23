#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <cassert>

#include "singleton.hpp"

class A
{
public:
    A(int x_ = 1) : x(x_){};

    int x;
};

using namespace hrd31;

A *g_b;
void ThreadFunc()
{
    A* a = Singleton<A>::GetInstance();
    std::cout << a << std::endl;
    assert(a == g_b);
}


int main()
{
    g_b = Singleton<A>::GetInstance();
    std::vector<std::thread> threads(10);

    for(int i = 0; i < 10; ++i)
    {
        threads[i] = std::thread(ThreadFunc);    
    }
    
    for(int i = 0; i < 10; ++i)
    {
        threads[i].join();    
    }

    return 0;
}
