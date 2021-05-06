#ifndef CEVENTLOOP_H
#define CEVENTLOOP_H

#ifndef LIBTEST
#include <QtCore/qglobal.h>
#if defined(LIBSHARED_THREAD_EXPORT)
#  define LIBSHARED_EVENTLOOP_EXPORT Q_DECL_EXPORT
#else
#  define LIBSHARED_EVENTLOOP_EXPORT Q_DECL_IMPORT
#endif
#else
#  define LIBSHARED_EVENTLOOP_EXPORT
#endif

#include "cobject.h"

class CEvent;
class LIBSHARED_EVENTLOOP_EXPORT CEventLoop : public CObject
{
public:
    explicit CEventLoop(CObject *parent = nullptr);

    ~CEventLoop();

    void sendEvent(const CEvent &event);
    void postEvent(const CEvent &event);

    void processEvent();

    int exec();
    void exit(int exitcode = 0);

    bool isRunning() const;
    bool event(CEvent *event) override;
private:
    class CEventLoopPri *_d;
    DISABLE_COPY(CEventLoop)
};

#endif // CEVENTLOOP_H
