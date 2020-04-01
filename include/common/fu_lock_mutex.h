#ifndef _FL_UNCOPYABLE_H_
#define _FL_UNCOPYABLE_H_

#include "fu_uncopyable.h"

//单线程使用的互斥量，多线程请使用事件

#ifdef WIN32

#include <Windows.h>
class L_Mutex:public Uncopyable
{
    friend class Locker;
private:
    ::CRITICAL_SECTION  m_cs; //≤Ÿ◊˜Node ±µƒÀ¯
    
public:
    L_Mutex()  //explicit
    {
        ::InitializeCriticalSection(&m_cs);
    }
    ~L_Mutex()
    {
        ::DeleteCriticalSection(&m_cs);
    }
};

class Locker:public Uncopyable
{
private:
    L_Mutex * m_pmtx;
    
public:
    Locker(L_Mutex * pmtx):m_pmtx(NULL)
    {
        if (pmtx != NULL)
        {
            m_pmtx = pmtx;
            EnterCriticalSection(&m_pmtx->m_cs);
        }
    }
    
    ~Locker()
    {
        if (m_pmtx != NULL)
        {
            LeaveCriticalSection(&m_pmtx->m_cs);
        }
    }
};
#else //ios

#import <pthread.h>

class L_Mutex:public Uncopyable
{
    friend class Locker;
private:
    pthread_mutex_t pLock;
    
public:
    L_Mutex()  //explicit
    {
        pthread_mutex_init(&pLock, NULL);
    }
    ~L_Mutex()
    {
        pthread_mutex_destroy(&pLock);
    }
};

class Locker:public Uncopyable
{
private:
    L_Mutex * m_pmtx;
    
public:
    Locker(L_Mutex * pmtx):m_pmtx(NULL)
    {
        if (pmtx != NULL)
        {
            m_pmtx = pmtx;
            pthread_mutex_lock(&m_pmtx->pLock);
        }
    }
    
    ~Locker()
    {
        if (m_pmtx != NULL)
        {
            pthread_mutex_unlock(&m_pmtx->pLock);
        }
    }
};

#endif

#endif

