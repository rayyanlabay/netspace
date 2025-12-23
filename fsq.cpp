template <typename T>
class FSQ
{
public:
    FSQ(size_t size_);
    FSQ(const FSQ &) = delete;
    FSQ &operator=(const FSQ &) = delete;
    ~FSQ() noexcept;
    void Write(const T &element_);
    T Read();
    bool IsEmpty();
    bool IsFull();

private:
    T *m_buf;
    size_t m_read_idx;
    size_t m_write_idx; 
    size_t m_size;

    std::recursive_mutex m_reader_mutex;
    std::mutex m_writer_mutex; 
    std::condition_variable_any m_cv;
};

template <typename T>
FSQ<T>::FSQ(size_t size_) : m_buf(new T[size_]), m_read_idx(size_ - 1),
                            m_write_idx(size_ - 1), m_size(size_), m_mutex(), m_cv()
{
}

template <typename T>
FSQ<T>::~FSQ() noexcept {}

template <typename T>
void Write(const T &element_)
{
    std::lock_guard<std::mutex> lock(m_writer_mutex);
    if (!IsFull())
    {
        m_buf[m_write_idx] = element_;
        --m_write_idx;

        if(m_write_idx == -1)
        {
            m_write_idx = m_size -1;
        }
        
        m_cv.notify_one();        
    }
}

template <typename T>
T Read()
{
    std::lock_guard<std::recursive_mutex> lock(m_reader_mutex);
    m_cv.wait(m_reader_mutex, []{!IsEmpty();});
    T& out = m_buf[m_read_idx];
    --m_read_idx;

    if(m_read_idx == -1)
    {
        m_read_idx = m_size - 1;
    }

    return out;
}

template <typename T>
bool IsEmpty()
{
    return m_write_idx == m_read_idx;
}

template <typename T>
bool IsFull()
{
    return ((m_write_idx - 1) % m_size) == (m_read_idx % m_size);
}


// will it work ? 
// lock free ?

// release publish
// atomic add 1


// atomic read 0 < and dec 
// acquire


