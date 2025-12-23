
#ifndef __I_DRIVER_COMMUNICATOR_HPP__
#define __I_DRIVER_COMMUNICATOR_HPP__

#include <memory> // shared_ptr
#include <stdexcept> //std::runetime_error 

#include "driver_data.hpp"

namespace abc
{
class DriverError : public std::runtime_error {
  public:
    DriverError(const std::string& str_) : std::runtime_error(str_)
    {}
};

class IDriverCommunicator
{
public:
    explicit IDriverCommunicator() = default;
    virtual ~IDriverCommunicator() noexcept = default;
    IDriverCommunicator(const IDriverCommunicator& other_) = delete;
    IDriverCommunicator& operator=(const IDriverCommunicator& other_) = delete;
    virtual std::shared_ptr<DriverData> ReceiveRequest() = 0;
    virtual void SendReply(std::shared_ptr<DriverData> data_) = 0;
    virtual void Disconnect() = 0;
    virtual int GetRequestFD() const = 0;
};
}//namespace abc

#endif //__I_DRIVER_COMMUNICATOR_HPP__