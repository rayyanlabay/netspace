
#include <fcntl.h>
#include "file_storage.hpp"

namespace abc
{

    FileStorage::FileStorage(size_t size_)
    :   m_fd(open("slave_storage", O_CREAT)),
        m_storage(mmap(nullptr, size_, PROT_NONE, MAP_PRIVATE, m_fd, 0))
    {
    }

    FileStorage::~FileStorage() noexcept
    {
        close(m_fd);
    }

    void FileStorage::Read(std::shared_ptr<DriverData> read_data_) const override
    {
        read_data_    
    }

    void FileStorage::Write(std::shared_ptr<DriverData> write_data_) override;

    int m_fd;
    void *m_storage;
} // namespace abc