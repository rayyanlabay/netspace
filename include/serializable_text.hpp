
#ifndef __SERIALIZABLE_TEXT_HPP__
#define __SERIALIZABLE_TEXT_HPP__

#include <cstddef>
#include <memory>
#include <vector>

#include "i_serializable.hpp"
#include "driver_data.hpp"

namespace abc
{

    class SerializableText : public ISerializable
    {
    public:
        using Buffer = std::shared_ptr<std::vector<std::byte>>;

        explicit SerializableText() = default;
        virtual ~SerializableText() noexcept = default;

        SerializableText(const SerializableText &other_) = delete;
        SerializableText &operator=(const SerializableText &other_) = delete;

        std::shared_ptr<DriverData> Deserialize(Buffer ser_buff_) override;
        Buffer Serialize(std::shared_ptr<DriverData> data_) override;
    };

} // namespace abc

#endif //__SERIALIZABLE_TEXT_HPP__