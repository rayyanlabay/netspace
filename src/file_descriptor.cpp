/*
status:

     done:
     exceptions

     todo:
     modern
     include commentry
     general commentry 
*/

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file_descriptor.hpp"
#include "utils.h"

namespace hrd31
{
    void FileDescriptor::CloseFD() // may throw
    {
        if (!*m_count && m_fd != -1)
        {
            CheckFailAndThrow<FDError>(-1 != close(m_fd), "cannot close fd",__FILE__,__LINE__);
            m_fd = -1; // Set to an invalid value to mark it as closed
        }
    }
    // DEBUG_ONLY(this = nullptr);

    FileDescriptor::FileDescriptor(const std::string &path_)
        : m_fd(open(path_.c_str(), O_RDWR)), m_count(new size_t(1))
    {

        CheckFailAndThrow<FDError>(-1 != m_fd, "cannot copy fd",__FILE__,__LINE__);
        *m_count = 1;
    }

    FileDescriptor::FileDescriptor(int fd_) : m_fd(fd_), m_count(new size_t(1))
    {
        *m_count = 1;
    } // may throw bad_alloc

    FileDescriptor::FileDescriptor(const FileDescriptor &other_) : m_fd(other_.m_fd),
                                                                   m_count(other_.m_count)
    {
        ++(*m_count);
    }

    FileDescriptor &FileDescriptor::operator=(const FileDescriptor &other_)
    {
        ++(*other_.m_count);
        --(*m_count);

        try
        {
            CloseFD();
            // may throw, need to catch because we have changed state
        }
        catch (FDError &e)
        {
            // change state back
            --(*other_.m_count);
            ++(*m_count);

            throw;
        }

        m_fd = other_.m_fd;
        return *this;
    }

    FileDescriptor::operator int() const
    {
        return m_fd;
    }

    FileDescriptor::~FileDescriptor()
    {
        try
        {
            CloseFD();
        }
        catch (...)
        {
            // empty on purpose
        }
    }
} // namespace hrd31