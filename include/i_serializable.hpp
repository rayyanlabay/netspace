
#ifndef __I_SERIALIZABLE_HPP__
#define __I_SERIALIZABLE_HPP__

#include <cstddef>
#include <memory>
#include <vector>

#include "driver_data.hpp"

namespace abc
{

class ISerializable
{
public:
    using Buffer = std::shared_ptr<std::vector<std::byte>>;

    explicit ISerializable() = default;
    virtual ~ISerializable() noexcept = default;
    
    ISerializable(const ISerializable& other_) = delete;
    ISerializable& operator=(const ISerializable& other_) = delete;

    virtual std::shared_ptr<DriverData> Deserialize(Buffer ser_buff_) = 0;
    virtual Buffer Serialize(std::shared_ptr<DriverData> data_) = 0;
};

}//namespace abc

#endif //__I_SERIALIZABLE_HPP__