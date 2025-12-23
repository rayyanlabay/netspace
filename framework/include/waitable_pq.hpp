/* -----------------------------------------------------------------------------
Description: Waitable thread safe priority queue
Author: HRD31
Reviewer: Itzik
Version:
    v0.2 - Mentor approved
----------------------------------------------------------------------------- */
#ifndef __HRD31_WAITABLE_PQ_HPP__
#define __HRD31_WAITABLE_PQ_HPP__

#include <queue>
#include <mutex>
#include <condition_variable>

namespace hrd31
{

    template <typename T, typename Container = std::vector<T>,
              typename Compare = std::less<typename Container::value_type>>
    class WPQueue
    {
    public:
        explicit WPQueue(const Compare &compare = Compare(), const Container &cont = Container());

        ~WPQueue() = default;
        WPQueue(const WPQueue &other_) = delete;
        WPQueue &operator=(const WPQueue &other_) = delete;

        T Pop();
        void Push(const T &element_);
        size_t Size() const;
        bool IsEmpty() const;

    private:
        std::priority_queue<T, Container, Compare> m_queue;
        mutable std::recursive_mutex m_mutex;
        std::condition_variable_any m_cv;
    };

    template <typename T, typename Container, typename Compare>
    WPQueue<T, Container, Compare>::WPQueue(const Compare &compare, const Container &cont) : m_queue(compare, cont)
    {
    }

    template <typename T, typename Container, typename Compare>
    bool WPQueue<T, Container, Compare>::IsEmpty() const
    {
        std::unique_lock<std::recursive_mutex> lock(m_mutex);
        return m_queue.empty();
    }

    template <typename T, typename Container, typename Compare>
    T WPQueue<T, Container, Compare>::Pop()
    {
        std::unique_lock<std::recursive_mutex> lock(m_mutex);

        m_cv.wait(lock, [this]
                  { return !m_queue.empty(); });

        const T ret = m_queue.top();
        m_queue.pop();

        return ret;
    }

    template <typename T, typename Container, typename Compare>
    void WPQueue<T, Container, Compare>::Push(const T &element_)
    {
        std::unique_lock<std::recursive_mutex> lock(m_mutex);

        m_queue.push(element_);

        m_cv.notify_one();
    }

    template <typename T, typename Container, typename Compare>
    size_t WPQueue<T, Container, Compare>::Size() const
    {
        return m_queue.size();
    }
} // namespace hrd31

#endif //__HRD31_WAITABLE_PQ_HPP__