
#ifndef __FILE_DESC_HPP__
#define __FILE_DESC_HPP__

#include <fcntl.h>
#include <unistd.h>
#include <string>

namespace abc
{    
    class FDError : std::runtime_error
    {
    public:
        explicit FDError(const std::string &str_) : std::runtime_error(str_)
        {
        }
    };


class FileDescriptor
{
public:
    explicit FileDescriptor(int fd_ = -1);
    explicit FileDescriptor(const std::string &str_);

    FileDescriptor(const FileDescriptor &);
    FileDescriptor &operator=(const FileDescriptor &);
    
    ~FileDescriptor();
    
    operator int() const;

    void CloseFD();

private:
    int m_fd;
    size_t *m_count;
};

} // namespace abc
#endif /* __FILE_DESC_HPP__ */