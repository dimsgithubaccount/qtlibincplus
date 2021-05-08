#include "test.h"

#include <QCoreApplication>

#include "net/src/ringbuffer.h"
#include "core/src/cthread.h"
#include "tool/src/logsys.h"

#include <cstring>
#include <list>

#include <QFile>
#include <QDateTime>
#include <QDebug>
#include "mainwindow.h"

#ifdef WIN32
#include "windows.h"
#include "strsafe.h"
#endif

void Print(const QString &a)
{
    qDebug() << QString("%1:%2")
                .arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz: "))
                .arg(a);
}

CTest::CTest(QObject *parent) : QObject(parent)
{

}

void fun1(QList<bool> &a)
{
    QList<bool> empty;
    a.swap(empty);
};

void fun2(std::list<bool> &b)
{
    std::list<bool> empty;
    b.swap(empty);
};

void *thread_func(void *)
{
    Print("thread is running");
    QThread::msleep(3000);
    Print("thread is going to end");
    return nullptr;
}

void CThreadA::func()
{
    Print("prepare set event");
    WINBOOL b = SetEvent(h);
    Print(QString("set event result=%1").arg(b));
    setWorkInterval(1000);
}

void CThreadB::func()
{
//    Print("Wait For event");
    setVital(true);
//    setWorkInterval(0);
    DWORD b = WaitForSingleObject(h, 1);
    ResetEvent(h);
    if(WAIT_FAILED != b && WAIT_TIMEOUT != b)
    {
        Print(QString("Wait Event Ok, type = %1").arg(b));
    }
}

#define rwsize 512
CThread *t;

static inline qint64 floordiv(qint64 a, int b)
{
    return (a - (a < 0 ? b - 1 : 0)) / b;
}

static inline int floordiv(int a, int b)
{
    return (a - (a < 0 ? b - 1 : 0)) / b;
}

int CTest::test(void *param)
{
    Q_UNUSED(param)
    CLogSystem::instance().start();

//    t = new CThread(this);
//    t->start();
//    QThread::msleep(100);
//    qDebug() << QThread::currentThreadId();
//    qDebug() << "-----------------------test-------------------";
//    qDebug() << "last error = " << t->lasterror();
//    qDebug() << "main thread's priority = " << QThread::currentThread()->priority();
//    qDebug() << t << "'s save id = " << t->threadId();
//    qDebug() << "QThread::currentThreadId = " << QThread::currentThreadId();
//    qDebug() << "CThread::currentThreadId = " << t->currentThreadId();
//    qDebug() << "-----------------------test-------------------";

//    MainWindow *w = static_cast<MainWindow*>(param);
//    connect(w, SIGNAL(sigBtnClicked()), this, SLOT(slotTest()));



//    CThread *t1 = new CThread(this);
//    t1->start();
//    QThread::msleep(100);
//    t1->suspend();
//    t1->resume();

#ifdef WIN32
    HANDLE h = CreateEvent(NULL, TRUE, FALSE, NULL);
    CThreadA *at = new CThreadA(this);
    CThreadB *bt = new CThreadB(this);
    at->h = h;
    bt->h = h;
    at->start();
    bt->start();
#endif









//    QDateTime t0 = QDateTime::currentDateTime();
////    QThread::msleep(1);
//    QDateTime t1 = QDateTime::currentDateTime();
//    qDebug() << t0.toString("mm:ss.zzz");
//    qDebug() << t1.toString("mm:ss.zzz");
//    qDebug() << "differ = " << t1.toMSecsSinceEpoch() - t0.toMSecsSinceEpoch();

//    void* id = QThread::currentThreadId();
//    int pol = SCHED_RR;
//    sched_param sp;
//    int min = sched_get_priority_min(pol);
//    int max = sched_get_priority_max(pol);
//    qDebug() << QString("min = %1, max = %2").arg(min).arg(max);
//    sp.sched_priority = (min + max) / 2 + 1;
//    qDebug() << "sched_priority = " << sp.sched_priority;

//    pthread_t tid = 0;
//    qDebug() << "tid = " << tid;
//    int res = pthread_create(&tid, nullptr, thread_func, nullptr);
//    qDebug() << "tid = " << tid;
////    qDebug() << "res = " << res;
//    res = pthread_setschedparam(tid, pol, &sp);
//    if(res != 0)
//    {
//#ifdef WIN32
//        int err = int(GetLastError());
//#elif linux
//        int err = errno;
//#endif
//        qDebug() << "set last error = " << err;
//        qDebug() << "set error res = " << res;
//    }

//    pol = 0;
//    sp.sched_priority = 0;

//    res = pthread_getschedparam(tid, &pol, &sp);
//    if(res != 0)
//    {
//#ifdef WIN32
//        int err = int(GetLastError());
//#elif linux
//        int err = errno;
//#endif
//        qDebug() << "get last error = " << err;
//        qDebug() << "get error res = " << res;
//    }
//    qDebug() << "pol = " << pol;
//    qDebug() << "pri = " << sp.sched_priority;
//    qDebug() << id;

    return 0;

    CharRingBuffer *buffer = new CharRingBuffer(511, 4, this);

    CTestThread *thread1 = new CTestThread(this);   //1 as productor
    CTestThread *thread2 = new CTestThread(this);   //2 as consumer
    thread1->setObjectName("t1");
    thread2->setObjectName("t2");

    thread1->setParams(buffer);
    thread2->setParams(buffer);

//    connect(thread1, SIGNAL(finished()), this, SIGNAL(sigExit()));
//    connect(thread2, SIGNAL(finished()), this, SIGNAL(sigExit()));

    connect(this, SIGNAL(sigMainWindowClosed()), thread2, SLOT(quit()));
    connect(this, SIGNAL(sigMainWindowClosed()), thread1, SLOT(quit()));

    thread1->start();
    thread2->start();

    return 0;
}

void CTest::slotMainWindowClosed()
{
//    CTestThread *thread;
//    thread = findChild<CTestThread*>("t1");
//    if(thread)
//        disconnect(thread, SIGNAL(finished()), this, SIGNAL(sigExit()));
//    thread = findChild<CTestThread*>("t2");
//    if(thread)
//        disconnect(thread, SIGNAL(finished()), this, SIGNAL(sigExit()));
    emit sigMainWindowClosed();
}

void CTest::slotTest()
{
    t->suspend();
}

int CTestThread::idcounter = 0;
int timerid = 0;

CTestThread::CTestThread(QObject *parent) : CCyclicThread(parent), objectid(++idcounter)
{
    setVital(false);
    setWorkInterval(1);
//    if(objectid == 1)
//        timerid = startTimer(1000);
}

CTestThread::~CTestThread()
{
    if(isRunning())
        quit();
    if(objectid == 1 && timerid != 0)
        killTimer(timerid);
}

void CTestThread::timerEvent(QTimerEvent *)
{
//    CharRingBuffer *buffer = static_cast<CharRingBuffer*>(params);
//    qDebug() << QString("the ring-buffer contains %1 items and %2 pieces of memory").arg(buffer->itemCount()).arg(buffer->blockCount());
    QFile file;
    file.open(QIODevice::ReadOnly);
}

void CTestThread::func()
{
    CharRingBuffer *buffer = static_cast<CharRingBuffer*>(params);
    if(objectid == 1)
    {
        static qint64 writetimes = 0, wpows = 0;
        static QFile file;
        if(!file.isOpen())
        {
            file.setFileName(QCoreApplication::applicationDirPath() + "/test/raw");
            qDebug() << file.fileName();
            file.open(QIODevice::ReadOnly);
        }
        if(file.atEnd())
        {
//            file.seek(0);
//            qInfo() << "file at the end";
            if(buffer->itemCount() == 0)
            {
                file.close();
                stop();
                return;
            }
            msleep(1000);
            setVital(false);
            return;
        }
        setVital(true);
        char buf[rwsize];
        int buflen = file.read(buf, rwsize);
        if(buflen != buffer->write(buf, buflen / 2))
            qWarning() << "buffer error action";
        if(++writetimes > INT64_MAX)
        {
            writetimes = 0;
            ++wpows;
        }
    }
    else if(objectid == 2)
    {
        static qint64 readtimes = 0, rpows = 0;
        static char array[rwsize] = { 0 };
        static int len = 0;
        if(buffer->isEmpty())
        {
            setVital(false);
            return;
        }
        setVital(true);
        int readlen = buffer->read(array, rwsize / 2);
        len += readlen;

        static QFile file;
        if(!file.isOpen())
        {
            file.setFileName(QCoreApplication::applicationDirPath() + "/test/new");
            qDebug() << file.fileName();
            file.open(QIODevice::WriteOnly);
        }
        if(readlen > 0)
        {
            file.write(array, readlen);
        }
        else
        {
            qWarning() << "abnormal write action";
        }
        if(len >= 9)
        {
//            if(std::memcmp(array, "123456789", 9) != 0)
//            {
//                qDebug() << QByteArray(array, 9).data();
//            }
//            char arr[32];
//            std::memcpy(arr, array + 9, len - 9);
//            std::memset(array, 0, 32);
//            std::memcpy(array, arr, len - 9);
//            len -= 9;
        }
        if(readlen > 0)
        {
            ++readtimes;
        }
        if(readtimes >= INT64_MAX)
        {
            readtimes = 0;
            ++rpows;
        }
    }
}
