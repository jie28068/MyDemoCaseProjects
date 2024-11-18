#ifndef MUTEX_H
#define MUTEX_H

#include <Windows.h>

class Mutex
{
public:
    inline Mutex()
    {
        InitializeCriticalSection(&_section);
    }

    inline ~Mutex()
    {
        DeleteCriticalSection(&_section);
    }

    inline void lock()
    {
        EnterCriticalSection(&_section);
    }

    inline void unlock()
    {
        LeaveCriticalSection(&_section);
    }

private:
    CRITICAL_SECTION _section;
};

class MutexLock
{
public:
    inline MutexLock(Mutex& mutex)
        :_mutex(mutex)
    {
        _mutex.lock();
    }

    inline ~MutexLock()
    {
        _mutex.unlock();
    }

private:
    Mutex& _mutex;
};

#endif // MUTEX_H
