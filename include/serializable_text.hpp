/*
Description:
Author: HRD31
Reviewer:
Version:
    v0.1 - For mentor approval
*/

#ifndef __HRD31_SERIALIZABLE_TEXT_HPP__
#define __HRD31_SERIALIZABLE_TEXT_HPP__

#include <cstddef>
#include <memory>
#include <vector>

#include "i_serializable.hpp"
#include "driver_data.hpp"

namespace hrd31
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

} // namespace hrd31

#endif //__HRD31_SERIALIZABLE_TEXT_HPP__