#ifndef CTHREAD_H
#define CTHREAD_H

#include <QThread>

#ifndef LIBTEST
#include <QtCore/qglobal.h>
#if defined(LIBSHARED_THREAD_EXPORT)
#  define LIBSHARED_THREAD_EXPORT Q_DECL_EXPORT
#else
#  define LIBSHARED_THREAD_EXPORT Q_DECL_IMPORT
#endif
#else
#  define LIBSHARED_THREAD_EXPORT
#endif

#define THREAD_VALUES
#define SHARED_VALUES
#define NORMAL_VALUES



class LIBSHARED_THREAD_EXPORT CThread : public QObject
{
    Q_OBJECT
public:
    /// thread running priority enum
    enum Priority{
        idle,
        lowest,
        below,
        normal,
        above,
        highest,
        critical,
        inherit
    };

    /// thread schedule algorithms enum
    enum ScheduleAlgorithms{
        sched_other = SCHED_OTHER,
        sched_fifo = SCHED_FIFO,
        sched_rr = SCHED_RR
#ifdef __USE_GNU
        ,
        sched_batch = SCHED_BATCH,
        sched_idle = SCHED_IDLE,
        sched_reset_on_fork = SCHED_RESET_ON_FORK
#endif
    };
public:
    explicit CThread(QObject *parent = nullptr);

    ~CThread();

    static void* currentThreadId();
    void* threadId() const;
    static QString describeThread(void *id)
    {
        return "0x" + QString::number((qint64)id, 16);
    }

    void setPriority(Priority priority);
    Priority priority() const;

    bool isFinished() const;
    bool isRunning() const;
    bool isPaused() const;

    void setStackSize(quint32 stackSize);
    quint32 stackSize() const;

    qint64 starttime() const;
    qint64 suspendtime() const;
    qint64 runtime() const;

    qint32 lasterror() const;

    void exit(int retCode = 0);
Q_SIGNALS:
    void finished();
public Q_SLOTS:
    virtual void start(Priority priority = inherit);

    virtual void suspend();

    virtual void resume();

    virtual void kill();

    virtual void wait();
protected:
    virtual void run();

    int exec();
private:
    void action_thread_begin();

    void action_thread_finishing();
#ifdef WIN32
    static __stdcall unsigned run_func(void *params);
#elif linux
    static void* run_func(void *params);
#endif
    class CThreadPrivate * const _d;
    Q_DISABLE_COPY(CThread)
};



class LIBSHARED_THREAD_EXPORT CCyclicThread : public QThread
{
    Q_OBJECT
public:
    explicit CCyclicThread(QObject *parent = nullptr);

    ~CCyclicThread() override;

    /// ??????????????????????????????
    static int threadCount();

    /// ??????????????????????????????????????????
    void setVital(bool bVital);
    bool isVital() const;

    /// ????????????????????????????????????????????????????????????
    void setWorkInterval(quint32 msec);
    quint32 workInterval() const;

Q_SIGNALS:
    /// ????????????????????????????????????
    void threadBegin();

    /// ?????????????????????????????????????????????
    void sigFuncTime(qint64 usedmsec);
public Q_SLOTS:
    /// ???????????????????????????
    virtual void start(Priority = InheritPriority);

    /// ?????????????????????????????????????????????????????????????????????????????????????????????
    virtual void stop();

    /// ?????????????????????????????????????????????????????????QThread::wait()
    virtual void quit(quint32 msec = ULONG_MAX);

    /// ?????????????????????msec?????????????????????
    virtual void kill(quint32 msec = 0);
protected:
    /// ???????????????????????????????????????????????????????????????????????????
    virtual void func() = 0;
private:
    void run() override;

    class CCyclicThreadPrivate * const _d;
    Q_DISABLE_COPY(CCyclicThread)
};
typedef CCyclicThread* CCyclicThreadPTR;


enum LockerType{
    eLocker_mutex = 1,
    eLocker_spin = 2,
    eLocker_semaphore = 4
};



class CLockerPrivate;

class LIBSHARED_THREAD_EXPORT CLocker
{
    Q_DISABLE_COPY(CLocker)
    CLockerPrivate * const _d;
public:
    CLocker(LockerType type = eLocker_spin);
    ~CLocker();

    void lock();

    void release();
};



template<class TYPE> class CMutexObject;
typedef void* PTR;


template<class TYPE>
class LIBSHARED_THREAD_EXPORT CMutexObjectItem
{
    friend class CMutexObject<TYPE>;
    typedef TYPE* TPTR;

    CMutexObject<TYPE> &_obj;

    bool _selfLocking;

    explicit CMutexObjectItem(CMutexObject<TYPE> &parentObject)
        : _obj(parentObject)
    {
        _selfLocking = false;
    }

    /// ??????=?????????
    CMutexObjectItem& operator =(const CMutexObjectItem &)
    {
        QMessageLogger(__FILE__, __FUNCTION__, __LINE__).fatal("CMutexObjectItem???????????????????????????????????????????????????CMutexObject::acquire????????????");
        return *this;
    }
public:
    CMutexObjectItem (const CMutexObjectItem &other) : _obj(other._obj)
    {
        _selfLocking = other._selfLocking;
    }

    ~CMutexObjectItem() { release(); }

    /// ?????????????????????????????????
    TYPE& data();

    /// ?????????????????????????????????
    const TYPE& const_data() const;

    bool isLocked() const { return _obj._locked; }

    bool isSelfLocking() const { return _selfLocking; }

    bool isValid() const { return _obj.isValid(); }

    void release();

    void lock();

    void unlock() { release(); }
};



template<class TYPE>
class LIBSHARED_THREAD_EXPORT CMutexObject : public QObject
{
    Q_DISABLE_COPY(CMutexObject)
    friend class CMutexObjectItem<TYPE>;
    typedef TYPE* TPTR;

    const LockerType _type;
    std::atomic_bool _locked;
    TPTR d;
    CLocker *locker;
public:
    explicit CMutexObject(LockerType type = eLocker_spin, QObject *parent = nullptr);

    explicit CMutexObject(const TYPE& data, LockerType type = eLocker_spin, QObject *parent = nullptr);

    virtual ~CMutexObject();

    /// ?????????????????????
    bool isValid() const { return _type >= eLocker_mutex && _type <= eLocker_semaphore; }

    bool isLocked() const { return _locked; }

    /// ??????????????????
    CMutexObjectItem<TYPE> acquire();

    /// ?????????
    void release() { unlock(); }

    /// ??????????????????
    const TYPE& const_data() const { return *d; }
private:
    void lock();

    void unlock();
};

#ifdef QSTRING_H
typedef CMutexObject<QString> MutexQString;
typedef CMutexObjectItem<QString> MutexQStringItem;
#endif
#ifdef QSTRINGLIST_H
typedef CMutexObject<QStringList> MutexQStrList;
typedef CMutexObjectItem<QStringList> MutexQStrListItem;
#endif


template<class TYPE>
TYPE &CMutexObjectItem<TYPE>::data()
{
    if(!_selfLocking)
        lock();
    return *_obj.d;
}

template<class TYPE>
const TYPE &CMutexObjectItem<TYPE>::const_data() const
{
    return *_obj.d;
}

template<class TYPE>
void CMutexObjectItem<TYPE>::release()
{
    if(!_selfLocking) return;
    _obj.unlock();
    _selfLocking = false;
}

template<class TYPE>
void CMutexObjectItem<TYPE>::lock()
{
    _obj.lock();
    _selfLocking = true;
}

template<class TYPE>
CMutexObject<TYPE>::CMutexObject(LockerType type, QObject *parent)
    : QObject (parent), _type(type), _locked(false), d(new TYPE), locker(new CLocker(type))
{

}

template<class TYPE>
CMutexObject<TYPE>::CMutexObject(const TYPE &data, LockerType type, QObject *parent)
    : QObject (parent), _type(type), _locked(false), d(new TYPE(data)), locker(new CLocker(type))
{

}

template<class TYPE>
CMutexObject<TYPE>::~CMutexObject()
{
    delete locker;
    delete d;
}

template<class TYPE>
CMutexObjectItem<TYPE> CMutexObject<TYPE>::acquire()
{
    return CMutexObjectItem<TYPE>(*this);
}

template<class TYPE>
void CMutexObject<TYPE>::lock()
{
    locker->lock();
    _locked = true;
}

template<class TYPE>
void CMutexObject<TYPE>::unlock()
{
    if(_locked)
    {
        locker->release();
        _locked = false;
    }
}

#endif // CCYCLICTHREAD_H
