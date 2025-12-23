#ifndef __I_INPUT_SRC_HPP__
#define __I_INPUT_SRC_HPP__

namespace abc
{

template <typename KEY, typename ARGS> 
class IInputSrc
{
public:
    explicit IInputSrc() = default;
    virtual ~IInputSrc() = default;
    IInputSrc(const IInputSrc&) = delete;
    IInputSrc& operator=(const IInputSrc&) = delete;

    virtual std::pair<KEY, ARGS> Read() = 0;
    virtual int GetFd() = 0;
};

}//namespace abc
#endif //__I_INPUT_SRC_HPP__