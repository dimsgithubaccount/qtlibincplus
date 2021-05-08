#ifndef CTEST_H
#define CTEST_H

#include "core/src/cthread.h"

class CTest : public QObject
{
    Q_OBJECT
public:
    explicit CTest(QObject *parent = nullptr);

    int test(void * = nullptr);
signals:
    void sigMainWindowClosed();

    void sigExit();
protected slots:
    void slotMainWindowClosed();

    void slotTest();
};

class CTestThread : public CCyclicThread
{
    Q_OBJECT
    static int idcounter;
    int objectid;

    void *params;
public:
    explicit CTestThread(QObject *parent = nullptr);

    ~CTestThread();

    void setParams(void *param) { params = param; }

    void timerEvent(QTimerEvent *) override;
Q_SIGNALS:
    void sigQuit();
protected:
    void func() override;
};

class CThreadA : public CCyclicThread
{
    void func() override;
public:
    explicit CThreadA(QObject *p) : CCyclicThread(p) {}

    void *h;
};

class CThreadB : public CCyclicThread
{
    void func() override;
public:
    explicit CThreadB(QObject *p) : CCyclicThread(p) {}

    void *h;
};
#endif // CTEST_H
