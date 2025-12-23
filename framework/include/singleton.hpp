/*
Description:
Author: HRD31
Reviewer: -
Version:
    v0.1 - For mentor approval
*/
#ifndef __HRD3_SINGLETON_HPP__
#define __HRD3_SINGLETON_HPP__

#include <mutex>
#include <memory>
#include <atomic>

// Use instruction:
// Must define default Ctor
// Default Ctor must be declared in private
// Using class must declare Singleton as a friend
namespace hrd31
{

    template <typename T>
    class Singleton
    {
    public:
        static T *GetInstance();

        Singleton() = delete;
        ~Singleton() = delete; // Desroyer serves as destructor
        Singleton(const Singleton<T> &other_) = delete;
        Singleton<T> &operator=(const Singleton<T> &other_) = delete;

        // unique_ptr so that T it will delete itself
        // atomic, so that read and write to it will be atomic
        static std::atomic<T*> s_instance;

    private:
        static std::mutex s_mutex;
    };

    // static
    template <typename T>
    T *Singleton<T>::GetInstance()
    {
        // load m_instance atomically, relaxed manner (for performance)
        // so that we load a value that was updated by another thread
        T *tmp = s_instance.load(std::memory_order_relaxed);

        // acquire memory that was relased by another thread
        // (only memory threads "publish" is inside the Double Checking)
        std::atomic_thread_fence(std::memory_order_acquire);

        // now we have acquired a valid memory, that has been just released by another thread
        if (tmp == nullptr)
        {
            // this locking will happen only in the start for the initialization step of
            // the s_instance, we could have multiple threads trying to initialize the s_instance
            std::lock_guard<std::mutex> lock(s_mutex);

            // after locking, we load whatever there is in the s_instance, atomically, again relaxed
            tmp = s_instance.load(std::memory_order_relaxed);

            // we check if another thread has passed us and initialized the s_instance
            if (tmp == nullptr)
            {
                tmp = new T();

                // publish memory of this thread
                std::atomic_thread_fence(std::memory_order_release);

                // now we can, atomically, store value in our s_instance, after we have released
                // the memory - so that reading thread of s_instance will read something valid.
                s_instance.store(tmp, std::memory_order_relaxed);
            }
        }

        return tmp;
    }

    template <typename T>
    std::atomic<T*> Singleton<T>::s_instance(nullptr);

    template <typename T>
    std::mutex Singleton<T>::s_mutex;

} // namespace hrd31

#endif /* __HRD3_SINGLETON_HPP__ */