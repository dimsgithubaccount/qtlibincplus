#include "cthread.h"
#include <mutex>
#include <atomic>
#include <semaphore.h>
#include <pthread.h>
#ifdef WIN32
#include <process.h>
#include <windows.h>
#elif linux
#include "unistd.h"
#include "sys/syscall.h"
#include "signal.h"
#endif
#define VERIFY_RET(a) if(!(a)) return
#define VERIFY_RETVAL(a, b) if(!(a)) return b

#include <QDateTime>
#include <QDebug>

class CThreadPrivate
{
#ifdef WIN32
    uintptr_t handle;
#elif linux
    pthread_t thread = 0;
#endif
    unsigned long long thread_id = 0;
    int last_error = 0;
    int suspend_count = -1;
    bool bRunning = false;

    CThreadPrivate(){}
    friend class CThread;
};

CThread::CThread(QObject *parent)
    : QObject(parent), _d(new CThreadPrivate)
{

}

CThread::~CThread()
{
    delete _d;
}

void *CThread::currentThreadId()
{
#ifdef WIN32
#elif linux
    pthread_t id = pthread_self();
    return reinterpret_cast<void*>(id);
#endif
}

void *CThread::threadId() const
{
    return reinterpret_cast<void*>(_d->thread_id);
}

bool CThread::isRunning() const
{
#ifdef WIN32
#elif linux
    VERIFY_RETVAL(_d->thread > 0, false);
    int res = pthread_kill(_d->thread, 0);
    if(0 != res)
    {
        _d->bRunning = false;
        _d->suspend_count = -1;
    }
    return 0 == res;
#endif
}

bool CThread::isPaused() const
{
    return _d->suspend_count > 0;
}

qint32 CThread::lasterror() const
{
    return _d->last_error;
}

void CThread::start(CThread::Priority priority)
{
#ifdef WIN32
    _d->handle = _beginthreadex(nullptr, 0, run_func, this, 0, (unsigned*)&_d->thread_id);
    if(0 == _d->handle)
    {
        _d->last_error = ::GetLastError();
        return;
    }
    int priority_value;
    switch (priority) {
    case idle:
        priority_value = THREAD_PRIORITY_IDLE;
        break;
    case lowest:
        priority_value = THREAD_PRIORITY_LOWEST;
        break;
    case below:
        priority_value = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    case normal:
        priority_value = THREAD_PRIORITY_NORMAL;
        break;
    case above:
        priority_value = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    case highest:
        priority_value = THREAD_PRIORITY_HIGHEST;
        break;
    case critical:
        priority_value = THREAD_PRIORITY_TIME_CRITICAL;
        break;
    default:
        HANDLE handle = ::GetCurrentThread();
        priority_value = ::GetThreadPriority(handle);
        break;
    }
    if(0 == SetThreadPriority((HANDLE)_d->handle, priority_value))
    {
        _d->last_error = ::GetLastError();
        return;
    }
#elif linux
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, 1);  //to be continued
    if(0 != ::pthread_create(&_d->thread, &attr, run_func, this))
    {
        _d->last_error = errno;
        qCritical() << QString("create thread failed, errno = %1:%2").arg(_d->last_error).arg(strerror(_d->last_error));
        return;
    }
    _d->thread_id = _d->thread;
    sched_param sp;
    pthread_attr_init(&attr);
    int policy = -1;
    if(0 != pthread_attr_getschedpolicy(&attr, &policy))
    {
        _d->last_error = errno;
        qWarning() << QString("get parent thread's policy failed, errno = %1:%2").arg(_d->last_error).arg(strerror(_d->last_error));
    }
    if(inherit != priority)
    {
        int min = sched_get_priority_min(policy);
        int max = sched_get_priority_max(policy);
        if(max - min > inherit - idle)
        {
            int step = (max - min) / (inherit - idle);
            sp.sched_priority = min + priority * step;
        }
    }
    else
    {
        if(0 != pthread_attr_getschedparam(&attr, &sp))
        {
            _d->last_error = errno;
            qWarning() << QString("get parent thread's priority failed, errno = %1:%2").arg(_d->last_error).arg(strerror(_d->last_error));
            sp.sched_priority = 0;
        }
    }
    if(0 != pthread_attr_setschedparam(&attr, &sp))
    {
        _d->last_error = errno;
        qWarning() << QString("set thread's priority failed, errno = %1:%2").arg(_d->last_error).arg(strerror(_d->last_error));
    }
#endif
}

void CThread::suspend()
{
#ifdef WIN32
    DWORD count = ::SuspendThread((HANDLE)_d->handle);
    if(0 != count - _d->suspend_count++)
        _d->suspend_count = count + 1;
#elif linux
    qWarning() << "in class " << this << " called suspend() is not currently supported on linux system!";
//    pthread_kill(_d->thread, SIGHUP);
#endif
}

void CThread::resume()
{
#ifdef WIN32
    DWORD count = ::ResumeThread((HANDLE)_d->handle);
    if(0 != count - _d->suspend_count--)
        _d->suspend_count = count - 1;
#elif linux
    qWarning() << "in class " << this << " called resume() is not currently supported on linux system!";
#endif
}

void CThread::kill()
{
#ifdef WIN32
#elif linux
    if(0 != pthread_cancel(_d->thread))
    {
        _d->last_error = errno;
        qWarning() << "kill thread = " << this << "& id = " << _d->thread << " failed, due to " << _d->last_error << ":" << strerror(_d->last_error);
    }
#endif
}

void CThread::wait()
{
#ifdef WIN32
//    ::WaitForSingleObjectEx(
#elif linux
#endif
}

#include <QDebug>
void CThread::run()
{

    qDebug() << "-----------------------run-------------------";
    qDebug() << "thread " << this << " is running";
    qDebug() << "QThread::currentThreadId = " << QThread::currentThreadId();
    qDebug() << this << "'s syscall returrns id = " << currentThreadId();
    qDebug() << this << "'s class save id = " << threadId();
    qDebug() << "CThread::currentThreadId = " << currentThreadId();
    qDebug() << "thread " << this << "'s priority = " << QThread::currentThread()->priority();
    qDebug() << "-----------------------run-------------------";
    for (int i = 0; i < 60 * 2; ++i) {
        qDebug() << "thread " << currentThreadId() << " is working, report at " <<QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        QThread::msleep(500);
    }
    qDebug() << "thread finished work";
    exec();
}

int CThread::exec()
{
    return 0;
}

void CThread::action_thread_begin()
{
    _d->bRunning = true;
    _d->suspend_count = 0;
}

void CThread::action_thread_finishing()
{
    _d->bRunning = false;
    emit finished();
}

#ifdef WIN32
unsigned CThread::run_func(void *params)
{
    CThread *thread = static_cast<CThread*>(params);
    thread->action_thread_begin();
    thread->run();
    thread->action_thread_finishing();
    return 0;
}
#elif linux
void *CThread::run_func(void *params)
{
//    sigset_t mask;
//    sigfillset(&mask);
//    pthread_sigmask(SIG_BLOCK, &mask, nullptr);
//    int sig;
//    sigwait(&mask, &sig);
    CThread *thread = static_cast<CThread*>(params);
    thread->run();
    return nullptr;
}
#endif

class CCyclicThreadPrivate
{
    static std::atomic_int m_nThreadCount;

    std::atomic<quint32> m_nWorkRate;

    std::atomic_bool m_bRun;

    std::atomic_bool m_bVIT;

    CCyclicThreadPrivate()
    {
        m_nWorkRate = 0;
        m_bRun = false;
        m_bVIT = false;
    }
    friend class CCyclicThread;
};

std::atomic_int CCyclicThreadPrivate::m_nThreadCount = ATOMIC_VAR_INIT(0);


CCyclicThread::CCyclicThread(QObject *parent)
    : QThread(parent), _d(new CCyclicThreadPrivate)
{

}

CCyclicThread::~CCyclicThread()
{
    delete _d;
}

int CCyclicThread::threadCount()
{
    return CCyclicThreadPrivate::m_nThreadCount;
}

void CCyclicThread::setVital(bool bVital)
{
    _d->m_bVIT = bVital;
}

bool CCyclicThread::isVital() const
{
    return _d->m_bVIT;
}

void CCyclicThread::setWorkInterval(quint32 msec)
{
    _d->m_nWorkRate = msec;
}

quint32 CCyclicThread::workInterval() const
{
    return _d->m_nWorkRate;
}

void CCyclicThread::start(Priority priority)
{
    _d->m_bRun = true;
    QThread::start(priority);
}

void CCyclicThread::stop()
{
    _d->m_bRun = false;
}

void CCyclicThread::quit(quint32 msec)
{
    _d->m_bRun = false;
    QThread::wait(msec);
}

void CCyclicThread::kill(quint32 msec)
{
    _d->m_bRun = false;
    QThread::wait(msec);
    QThread::setTerminationEnabled(true);
    QThread::terminate();
}

void CCyclicThread::run()
{
    emit threadBegin();
    ++_d->m_nThreadCount;
    qint64 t0;
    while(_d->m_bRun)
    {
        t0 = QDateTime::currentMSecsSinceEpoch();
        func();         // 2020/7/9优化记录：先执行后休眠，防止析构时静态变量、成员属性等失效导致段错误
        emit sigFuncTime(QDateTime::currentMSecsSinceEpoch() - t0);
        if(!_d->m_bVIT)
            msleep(_d->m_nWorkRate);
    }
    --_d->m_nThreadCount;
}




class CLockerPrivate{
public:
    LockerType type;
    PTR mutex;
    bool bLocked = false;
};

CLocker::CLocker(LockerType type)
    : _d(new CLockerPrivate)
{
    switch(type)
    {
    case eLocker_mutex:
        _d->mutex = new std::mutex;
        break;
    case eLocker_spin:
        _d->mutex = new std::atomic_flag(false);
        break;
    case eLocker_semaphore:
        _d->mutex = new sem_t;
        sem_init(static_cast<sem_t*>(_d->mutex), 0, 1);
        break;
    }
    _d->type = type;
}

CLocker::~CLocker()
{
    release();
    switch (_d->type) {
    case eLocker_mutex:
        if(_d->mutex) delete static_cast<std::mutex*>(_d->mutex);
        break;
    case eLocker_spin:
        if(_d->mutex) delete static_cast<std::atomic_flag*>(_d->mutex);
        break;
    case eLocker_semaphore:
        if(_d->mutex) delete static_cast<sem_t*>(_d->mutex);
        break;
    }
    delete _d;
}

void CLocker::lock()
{
    switch (_d->type) {
    case eLocker_mutex:
        static_cast<std::mutex*>(_d->mutex)->lock();
        break;
    case eLocker_spin:
        while(static_cast<std::atomic_flag*>(_d->mutex)->test_and_set(std::memory_order_acquire));
        break;
    case eLocker_semaphore:
        sem_wait(static_cast<sem_t*>(_d->mutex));
        break;
    }
    _d->bLocked = true;
}

void CLocker::release()
{
    VERIFY_RET(_d->bLocked);
    switch (_d->type) {
    case eLocker_mutex:
        static_cast<std::mutex*>(_d->mutex)->unlock();
        break;
    case eLocker_spin:
        static_cast<std::atomic_flag*>(_d->mutex)->clear(std::memory_order_release);
        break;
    case eLocker_semaphore:
    default:
        sem_post(static_cast<sem_t*>(_d->mutex));
        break;
    }
    _d->bLocked = false;
}
