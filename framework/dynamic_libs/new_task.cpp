#include <iostream>
#include "factory.hpp"
#include "singleton.hpp"
#include "i_task.hpp"

class A : public abc::ITask
{
public:
    void Execute() override
    {
        std::cout << "foo " << x << std::endl;
    }

    static std::shared_ptr<A> CreateA(int y)
    {
        std::cout << "created A\n";
        return std::make_shared<A>(y);
    }

    A(int y = 0) : x(y)
    {
        std::cout << "here ctor" << std::endl;
    };

    ~A()
    {
        std::cout << "here" << std::endl;
    }

private:
    int x;
};

using namespace abc;

void __attribute__((constructor)) my_init(void)
{
    Factory<ITask, int, int> *factory = Singleton<Factory<ITask, int, int>>::GetInstance();
    std::cout << "factory: " << factory << std::endl;

    factory->Add(1, A::CreateA);

    std::cout << "/* attirbute ctor */" << std::endl;
}
