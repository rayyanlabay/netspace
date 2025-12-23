#include <iostream>
#include "factory.hpp"

class A
{
public:
    void foo()
    {
        std::cout << x << std::endl;
    }
    static std::shared_ptr<A> CreateA(int y)
    {
        return std::make_shared<A>(y);
    }
    A(int y = 0) : x(y) { std::cout << "here ctor" << std::endl; };
    ~A()
    {
        std::cout << "here" << std::endl;
    }

private:
    int x;
};

using namespace hrd31;

int main()
{
    Factory<A, int, int> *F = Singleton<Factory<A, int, int>>::GetInstance();
    
    F->Add(1, A::CreateA);
    std::shared_ptr<A> a = F->Create(1, 2);
    
    a->foo();
    std::cout << "/* message */" << std::endl;

    return 0;
}
