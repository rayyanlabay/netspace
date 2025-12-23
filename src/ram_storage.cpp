
#include <cstddef> /* size_t */

#include "logger.hpp"
#include "ram_storage.hpp"
#include "driver_data.hpp"

namespace abc
{
    RAMStorage::RAMStorage(size_t size_) : m_storage(size_)
    {
    }

    RAMStorage::~RAMStorage() noexcept
    {
    }

    void RAMStorage::Read(std::shared_ptr<DriverData> read_data_) const
    {
        size_t offset = read_data_->m_offset;
        size_t len = read_data_->m_len;

        Logger *logger = Singleton<Logger>::GetInstance();
        logger->Log(Logger::INFO, "transaction: read ", __FILE__, __LINE__, true);
        for (size_t i = 0; i < len; ++i)
        {
            read_data_->m_data[i] = m_storage[offset + i];
        }
    }
    // doesnt throw

    void RAMStorage::Write(std::shared_ptr<DriverData> write_data_)
    {
        size_t offset = write_data_->m_offset;
        size_t len = write_data_->m_len;

        Logger *logger = Singleton<Logger>::GetInstance();
        logger->Log(Logger::INFO, "transaction: write ", __FILE__, __LINE__, true);
        for (size_t i = 0; i < len; ++i)
        {
            m_storage[offset + i] = write_data_->m_data[i];
        }
    }
    // doesnt throw
} // namespace abc
