#include "ceventloop.h"
#include "cevent.h"
#include "cthread.h"

#include <QDebug>
#include <QQueue>
typedef QQueue<CEvent> EventQueue;
typedef CMutexObject<EventQueue> MutexEvevntQueue;
typedef CMutexObjectItem<EventQueue> MutexEventQueueItem;

#ifdef WIN32
#include "windows.h"
#endif

class CEventLoopPri
{
    MutexEvevntQueue eventQueue;
    CMutexObject<CEvent> curEvent;
    std::atomic_bool exited;
    std::atomic_int exitcode;
#ifdef WIN32
    HANDLE handle = NULL;
#endif
    friend class CEventLoop;
};

CEventLoop::CEventLoop(CObject *parent)
    : CObject(parent), _d(new CEventLoopPri)
{

}

CEventLoop::~CEventLoop()
{
    delete _d;
}

void CEventLoop::sendEvent(const CEvent &event)
{
    _d->curEvent.acquire().data() = event;
}

void CEventLoop::postEvent(const CEvent &event)
{
    _d->eventQueue.acquire().data().append(event);
}

void CEventLoop::processEvent()
{
    CMutexObjectItem<CEvent> item = _d->curEvent.acquire();
    VERIFY_RET(item.data().isLegal());
    CEvent e = item.data();
    item.data().destroy();
    item.release();
    event(&e);
}

int CEventLoop::exec()
{
    void *id = CThread::currentThreadId();
    if(id != objectThreadId())
    {
        qWarning() << QString("cannot call exec in different thread(%1), this is in thread(%2)")
                      .arg(CThread::describeThread(id))
                      .arg(CThread::describeThread(objectThreadId()));
        return -1;
    }

    if(NULL == _d->handle)
    {
        _d->handle = CreateEvent(NULL, TRUE, FALSE, 0);
        if(NULL == _d->handle)
        {
            qWarning() << "Create Event Loop FAILED!";
            return -1;
        }
    }

    while (!_d->exited) {
        MutexEventQueueItem item = _d->eventQueue.acquire();
        if(!item.data().isEmpty())
        {
            _d->curEvent.acquire().data() = item.data().dequeue();
#ifdef WIN32
        SetEvent(_d->handle);
#endif
        }
        item.release();
#ifdef WIN32
        DWORD res = WaitForSingleObject(_d->handle, 1);
        if(WAIT_FAILED != res && WAIT_TIMEOUT != res)
        {
            ResetEvent(_d->handle);
            processEvent();
        }
#endif
    }
    return 0;
}

void CEventLoop::exit(int exitcode)
{
    _d->exitcode = exitcode;
    _d->exited = true;
}

bool CEventLoop::event(CEvent *event)
{

}
