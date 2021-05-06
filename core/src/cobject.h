#ifndef COBJECT_H
#define COBJECT_H

#ifndef LIBTEST
#include <QtCore/qglobal.h>
#if defined(LIBSHARED_THREAD_EXPORT)
#  define LIBSHARED_COBJECT_EXPORT Q_DECL_EXPORT
#else
#  define LIBSHARED_COBJECT_EXPORT Q_DECL_IMPORT
#endif
#else
#  define LIBSHARED_COBJECT_EXPORT
#endif

#include "cobjectdef.h"

#include <QString>
class CEvent;

class LIBSHARED_COBJECT_EXPORT CObject
{
public:

public:
    explicit CObject(CObject *parent = nullptr);

    explicit CObject(const QString &objectName, CObject *parent = nullptr);

    virtual ~CObject();

    void setObjectName(const QString &objectName);
    QString objectName() const;

    void setParent(CObject *parent);
    CObject* parent() const;

    void *objectThreadId() const;

    virtual bool event(CEvent *event);
private:
    class CObjectData *_d;
};
#endif // COBJECT_H
