#include <chrono>
#include <string>
#include <iostream>
#include <fstream>
#include "logger.hpp"

namespace abc
{
    void Logger::SetLogLevel(log_level_t level_)
    {
        m_log_level = level_;
    }

    void Logger::SetFilePath(const std::string &path_to_file_)
    {
        m_stream.open(path_to_file_, std::ios_base::app);
    }

    void Logger::Log(log_level_t level_, const std::string &msg_,
                     const std::string &file_, int line_, bool print_outstream_)
    {
        // if level is bad dont enter
        if (level_ > m_log_level)
        {
            return;
        }

        // get print information ready
        time_t now = time(0);
        std::string time_str = ctime(&now);
        time_str[time_str.size() - 1] = 0;

        std::lock_guard<std::mutex> Lock(m_mutex);
        // according to the flag print, to file, or to both file and outstream
        m_stream << time_str << " | " << msg_ << " | " << file_ << " | " << line_ << std::endl;

        if (print_outstream_)
        {
            std::cout << time_str << " | " << msg_ << " | " << file_ << " | " << line_ << std::endl;
        }
    }

} // namespace abc
