#include <iostream>
#include <thread>
#include "waitable_pq.hpp"

using namespace abc;

void ThreadFunc(WPQueue<int> *queue, int id)
{
    std::cout << id << " " << queue->Pop() << std::endl;
}

int main(int argc, char const *argv[])
{
    WPQueue<int> queue;
    
    queue.Push(10);
    queue.Push(11);
    queue.Push(12);
    queue.Push(13);

    std::thread t1(ThreadFunc, &queue, 1);
    std::thread t2(ThreadFunc, &queue, 2);
    std::thread t3(ThreadFunc, &queue, 3);
    std::thread t4(ThreadFunc, &queue, 4);
    std::thread t5(ThreadFunc, &queue, 5);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}
// 