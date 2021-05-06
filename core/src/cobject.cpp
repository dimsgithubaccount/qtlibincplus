#include "cobject.h"
#include "cthread.h"
#include "crefcount.h"

#include <QDebug>
#include <QList>
typedef QList<CObject*> CObjectList;

struct CThreadData
{
    void* parentThreadId = 0;
    void* objectThreadId = 0;
    inline QString qstrParentThreadId() const { return CThread::describeThread(parentThreadId); }
    inline QString qstrObjectThreadId() const { return CThread::describeThread(objectThreadId); }
};

struct CObjectData
{
    CThreadData threadData;
    CObject* parentObject = nullptr;
    QString name;
    CObjectList children;
    CRefCount ref;
    CLocker locker;

    void lock() { locker.lock(); }
    void unlock() { locker.release(); }
};

CObject::CObject(CObject *parent) : _d(new CObjectData)
{
    _d->threadData.objectThreadId = CThread::currentThreadId();
    setParent(parent);
}

CObject::CObject(const QString &objectName, CObject *parent)
{
    _d->threadData.objectThreadId = CThread::currentThreadId();
    setObjectName(objectName);
    setParent(parent);
}

CObject::~CObject()
{
    _d->lock();
    CObjectList::const_iterator iter = _d->children.begin();
    while(iter != _d->children.end())
    {
        CObject* child = *iter;
        delete child;
        ++iter;
    }
    _d->unlock();
    delete _d;
}

void CObject::setObjectName(const QString &objectName)
{
    _d->lock();
    _d->name = objectName;
    _d->unlock();
}

QString CObject::objectName() const
{
    return _d->name;
}

void CObject::setParent(CObject *parent)
{
    VERIFY_RET(parent);
    _d->lock();
    if(parent->objectThreadId() != this->objectThreadId())
    {
        qWarning() << QString("cannot set parent object which is in different thread(%1), this is in thread(%2)")
                      .arg(parent->_d->threadData.qstrObjectThreadId())
                      .arg(_d->threadData.qstrObjectThreadId());
        _d->unlock();
        return;
    }
    _d->parentObject = parent;
    parent->_d->children.append(this);
    _d->unlock();
}

CObject *CObject::parent() const
{
    return _d->parentObject;
}

void *CObject::objectThreadId() const
{
    return _d->threadData.objectThreadId;
}

bool CObject::event(CEvent *event)
{

}
