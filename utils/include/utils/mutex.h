/*************************************************************************
    > File Name: mutex.h
    > Author: hsz
    > Desc: for mutex
    > Created Time: 2021年04月25日 星期日 21时24分54秒
 ************************************************************************/

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <stdint.h>
#include <assert.h>
#include <atomic>
#include <functional>

#include <utils/string8.h>
#include <utils/sysdef.h>

#if defined(OS_LINUX)
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

namespace eular {
class NonCopyAble
{
public:
    NonCopyAble() = default;
    ~NonCopyAble() = default;
    NonCopyAble(const NonCopyAble&) = delete;
    NonCopyAble& operator=(const NonCopyAble&) = delete;
};

template<typename MutexType>
class AutoLock : public NonCopyAble
{
public:
    AutoLock(MutexType& mutex) : mMutex(mutex)
    {
        mMutex.lock();
    }
    ~AutoLock()
    {
        mMutex.unlock();
    }
private:
    MutexType& mMutex;
};

typedef enum class __MutexSharedAttr {
    PRIVATE = 0,    // mutex can only be used within the same process
    SHARED = 1      // mutex can be used between processes
} MutexSharedAttr;

class Mutex : public NonCopyAble
{
public:
    Mutex(int32_t type = static_cast<int32_t>(MutexSharedAttr::PRIVATE));
    ~Mutex();

    int32_t lock();
    int32_t trylock();
    void    unlock();

    void setMutexName(const String8 &name);
    const String8 &getMutexName() const { return mName; }

    operator pthread_mutex_t *() { return &mMutex; }
    pthread_mutex_t *mutex() { return &mMutex; }

private:
    friend class Condition;
    mutable pthread_mutex_t mMutex;
    String8 mName;
};

class RecursiveMutex : public NonCopyAble
{
public:
    RecursiveMutex(int32_t type = static_cast<int32_t>(MutexSharedAttr::PRIVATE));
    ~RecursiveMutex();

    int32_t lock();
    int32_t trylock();
    void    unlock();

    void setMutexName(const String8 &name);
    const String8 &getMutexName() const { return mName; }

    operator pthread_mutex_t *() { return &mMutex; }
    pthread_mutex_t *mutex() { return &mMutex; }

private:
    friend class Condition;
    mutable pthread_mutex_t mMutex;
    String8 mName;
};

// 局部写锁
template<typename WRMutexType>
class WRAutoLock 
{
public:
    WRAutoLock(WRMutexType& mtx) : mutex(mtx)
    {
        mutex.wlock();
    }
    ~WRAutoLock()
    {
        mutex.unlock();
    }

private:
    WRMutexType &mutex;
};

// 局部读锁
template<typename RDMutexType>
class RDAutoLock
{
public:
    RDAutoLock(RDMutexType& mtx) : mutex(mtx)
    {
        mutex.rlock();
    }
    ~RDAutoLock()
    {
        mutex.unlock();
    }

private:
    RDMutexType &mutex;
};

// 读写锁, 读共享, 写独享, 读上锁无法写, 写上锁无法读写
class RWMutex : public NonCopyAble {
public:
    typedef RDAutoLock<RWMutex> ReadAutoLock;
    typedef WRAutoLock<RWMutex> WriteAutoLock;

    RWMutex();
    ~RWMutex();
    void rlock();
    void wlock();
    void unlock();

private:
    mutable pthread_rwlock_t mRWMutex;
#ifdef DEBUG
    std::atomic<bool> mReadLocked;
    std::atomic<bool> mWritLocked;
#endif
};

class Sem : public NonCopyAble {
public:
    Sem(const char *semPath, uint8_t val);      // 此种走有名信号量
    Sem(uint8_t valBase);                       // 此种走无名信号量
    Sem(const Sem &) = delete;
    ~Sem();

    bool post();
    bool wait();
    bool trywait();
    bool timedwait(uint32_t ms);

private:
    sem_t  *mSem;       // 信号量
    String8 mFilePath;  // 有名信号量使用
    bool    isNamedSemaphore;
};

struct once_flag
{
    template<typename _Callable, typename... _Args>
    friend void call_once(once_flag& __once, _Callable&& __f, _Args&&... __args);

private:
    pthread_once_t m_once = PTHREAD_ONCE_INIT;

public:
    constexpr once_flag() noexcept = default;
    once_flag(const once_flag&) = delete;
    once_flag& operator=(const once_flag&) = delete;
};

namespace detail {
extern __thread void* __once_callable;
extern __thread void (*__once_call)();
extern "C" void __once_proxy(void);
} // namespace detail

template<typename _Callable, typename... _Args>
void call_once(once_flag& __once, _Callable&& __f, _Args&&... __args)
{
    auto __callable = [&] {
        std::__invoke(std::forward<_Callable>(__f), std::forward<_Args>(__args)...);
    };
    detail::__once_callable = std::addressof(__callable);
    detail::__once_call = []{ (*(decltype(__callable)*)detail::__once_callable)(); };

    int __e = pthread_once(&__once.m_once, &__once_proxy);
    if (__e)
        throw std::runtime_error("");

    __once_callable = nullptr;
    __once_call = nullptr;
}

} // namespace eular

#endif // __MUTEX_H__
