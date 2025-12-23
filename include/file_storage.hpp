
#ifndef __FILE_STORAGE_HPP__
#define __FILE_STORAGE_HPP__

#include "i_storage.hpp"
#include "driver_data.hpp"

namespace abc
{
    class FileError : public StorageError
    {
    };

    class FileStorage : public IStorage
    {
    public:
        explicit FileStorage(size_t size_);
        ~FileStorage() noexcept;
        FileStorage(const FileStorage &other_);
        FileStorage &operator=(const FileStorage &other_) = delete;

        void Read(std::shared_ptr<DriverData> read_data_) const override;
        void Write(std::shared_ptr<DriverData> write_data_) override;

    private:
        int m_fd;
        void *m_storage;
    };
} // namespace abc

#endif //__FILE_STORAGE_HPP_