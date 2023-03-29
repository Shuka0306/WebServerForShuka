#ifndef _LOCKER_H__
#define _LOCKER_H__
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <exception>
#include <semaphore.h>
// 线程同步机制封装类

// 互斥锁类
class locker
{
private:
    pthread_mutex_t m_mutex;

public:
    locker()
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    };
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    };
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    // 获取互斥量
    pthread_mutex_t *get()
    {
        return &m_mutex;
    }
};
//条件变量类
class cond
{
private:
    /* data */
    pthread_cond_t m_cond;
public:
    cond(/* args */)
    {
        if(pthread_cond_init(&m_cond,NULL)!=0)
        {
            throw std::exception();
        }
    };
    ~cond()
    {
        pthread_cond_destroy(&m_cond);
    };
     bool wait(pthread_mutex_t *mutex)
    {
        auto ret=pthread_cond_wait(&m_cond,mutex);
        return ret==0;
        
    }
    bool timewait(pthread_mutex_t *mutex,struct timespec t)
    {
        auto ret= pthread_cond_timedwait(&m_cond,mutex,&t);
        return ret==0;
    }
    //一个线程唤醒
    bool signal()
    {
        auto ret=pthread_cond_signal(&m_cond);
        return ret==0;
    }
    //所有线程唤醒
    bool broadcast()
    {
        auto ret=pthread_cond_broadcast(&m_cond);
        return ret==0;
    }


};

//信号量类
class sem{
    public:
    sem()
    {
        if(sem_init(&m_sem,0,0)!=0)
        {
            throw std::exception();
        }
    };
    sem(int num)
    {
        if(sem_init(&m_sem,0,num)!=0)
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        sem_destroy(&m_sem);
    };
    //等到信号量
    bool wait()
    {
        auto ret=sem_wait(&m_sem);
        return ret==0;
    }
    //增加信号量
    bool post()
    {
        auto ret=sem_post(&m_sem);
        return ret==0;
    }
    private:
    sem_t m_sem;
};
#endif