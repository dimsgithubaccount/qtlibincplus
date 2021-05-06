#ifndef CEVENT_H
#define CEVENT_H

#ifndef LIBTEST
#include <QtCore/qglobal.h>
#if defined(LIBSHARED_THREAD_EXPORT)
#  define LIBSHARED_EVENT_EXPORT Q_DECL_EXPORT
#else
#  define LIBSHARED_EVENT_EXPORT Q_DECL_IMPORT
#endif
#else
#  define LIBSHARED_EVENT_EXPORT
#endif

#include "crefcount.h"

class LIBSHARED_EVENT_EXPORT CEvent
{
public:
    enum Type
    {
        DEFAULT
    };
public:
    explicit CEvent(Type type = DEFAULT);
    CEvent(const CEvent &other);
    virtual ~CEvent();
    CEvent& operator=(const CEvent &other);
    inline Type type() const { return m_type; }

    inline bool isLegal() const { return DEFAULT != m_type; }
    inline void destroy() { m_type = DEFAULT; }

    inline void setAccepted(bool accepted) { m_bAccept = accepted; }
    inline bool isAccepted() const { return m_bAccept; }

    inline void accept() { m_bAccept = true; }
    inline void ignore() { m_bAccept = false; }

    static int registerEventType(int eint = -1);
private:
    Type m_type;
    bool m_bAccept;
    CRefCount m_ref;
};

#endif // CEVENT_H
