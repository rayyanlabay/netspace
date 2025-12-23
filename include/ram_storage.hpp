
#ifndef __RAM_STORAGE_HPP__
#define __RAM_STORAGE_HPP__

#include <cstddef> /* size_t */
#include <vector>
#include <stdexcept>

#include "i_storage.hpp"
#include "driver_data.hpp"

namespace abc
{
class RAMError : public StorageError {};

class RAMStorage : public IStorage
{
public:
    explicit RAMStorage(size_t size_); //may throw bad_alloc
    ~RAMStorage() noexcept;
    RAMStorage(const RAMStorage& other_); //may throw bad_alloc
    RAMStorage& operator=(const RAMStorage& other_) = delete;

    void Read(std::shared_ptr<DriverData> read_data_) const override ; //may throw bad_read, bad_write
    void Write(std::shared_ptr<DriverData> write_data_) override; //may throw bad_write

private:
    std::vector<char> m_storage;
};
}//namespace abc

#endif //__RAM_STORAGE_HPP__