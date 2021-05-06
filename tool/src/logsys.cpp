#include "logsys.h"
#include "core/src/cthread.h"
#include <iostream>

#include <QQueue>
#include <QDateTime>
#include <qlogging.h>
void outputRedirection(QtMsgType type, const QMessageLogContext &context, const QString &msg);

struct OutputMsg{
    QtMsgType type;
    QString fileName;
    qint32 line;
    QString functionName;
    QString printMsg;

    OutputMsg() {}
    OutputMsg(QtMsgType lpType, const QMessageLogContext &lpContext, const QString &lpPringMsg)
    {
        type = lpType;
        fileName = lpContext.file;
        line = lpContext.line;
        functionName = lpContext.function;
        printMsg = lpPringMsg;
    }
};
typedef QQueue<OutputMsg> OutputMsgQueue;

class COutputThread : public CCyclicThread
{
    CMutexObject<OutputMsgQueue> queue;
public:
    COutputThread();
    ~COutputThread();

    void print(QtMsgType type, const QMessageLogContext &context, const QString &msg);
protected:
    void func() override;
};

static CMutexObject<COutputThread> opt;

void outputRedirection(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString qstrMsgType;
    switch (type) {
    case QtDebugMsg:
        qstrMsgType = "Debug    :";
        break;
    case QtInfoMsg:
        qstrMsgType = "Info     :";
        break;
    case QtWarningMsg:
        qstrMsgType = "Warning  :";
        break;
    case QtCriticalMsg:
        qstrMsgType = "Critical :";
        break;
    case QtFatalMsg:
        qstrMsgType = "Fatal    :";
        abort();
    }
    QString printfMsg = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz | ") + qstrMsgType + msg;
    opt.acquire().data().print(type, context, printfMsg);
}

COutputThread::COutputThread()
    : CCyclicThread()
{
    setWorkInterval(1);
}

COutputThread::~COutputThread()
{
    if(isRunning())
        quit(3000);
}

void COutputThread::print(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    queue.acquire().data().enqueue(OutputMsg(type, context, msg));
    if(!isRunning())
        start();
}

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

void COutputThread::func()
{
    CMutexObjectItem<OutputMsgQueue> item = queue.acquire();
    if(item.data().isEmpty())
    {
        setVital(false);
        return;
    }
    setVital(true);
    OutputMsgQueue omq;
    item.data().swap(omq);
    item.release();

    ///...

    while(!omq.isEmpty())
    {
        OutputMsg om = omq.dequeue();
        if(om.type == QtWarningMsg || om.type == QtCriticalMsg || QtFatalMsg == om.type)
        {
            std::cout << RED << om.printMsg.toLocal8Bit().data() << RESET << std::endl;
        }
        else
        {
            std::cout << om.printMsg.toLocal8Bit().data() << std::endl;
        }
    }
}

CLogSystem::CLogSystem(CObject *parent)
    : CObject(parent)
{

}

CLogSystem::~CLogSystem()
{

}

CLogSystem &CLogSystem::instance()
{
    static CLogSystem _instance;
    return _instance;
}

void CLogSystem::start()
{
    qInstallMessageHandler(outputRedirection);
}

void CLogSystem::stop()
{
    qInstallMessageHandler(nullptr);
    opt.acquire().data().stop();
}
