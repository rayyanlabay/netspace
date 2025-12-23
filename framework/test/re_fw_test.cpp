
#include <iostream> // cout

#include "re_fw.hpp"

using namespace abc;

class MyTask : public IRETask
{
public:
    static std::shared_ptr<MyTask> CreateMyTask(int i)
    {
        return std::make_shared<MyTask>(i);
    }

    void Execute() override
    {
        std::cout << "executing task\n";
    }

    explicit MyTask(int i)
    {
        x = i;
    }

private:
    int x;
};

// req ->
// config task (execute read/write nbd)
// config input (stdin, Read from stdin)

template <typename KEY, typename ARGS>
class MySrc : public IInputSrc<KEY, ARGS>
{
public:
    // key, args
    std::pair<int, int> Read() override
    {
        std::cout << "Read\n";
        return std::make_pair(1, 3);
    }

    int GetFd() override
    {
        std::cout << "here ? \n";
        return STDIN_FILENO;
    }
};

// input src,   MySrc
//                  fd = stdin
//                  read-> return (key = 1, args = 3)

// taskt,       MyTask
//                  key = 1
//                  args = int

int main()
{

    // ctor with default values
    //         // key, args
    std::shared_ptr<MySrc<int, int>> src(new MySrc<int, int>());
    RequestEngine<int, int> re;

    re.ConfigTask(1, MyTask::CreateMyTask);

    re.ConfigInputSrc(src);

    re.Run();
    // must print "3"

    sleep(1);

    re.Stop();

    return 0;
}
