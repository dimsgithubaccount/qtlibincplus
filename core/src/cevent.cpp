#include "cevent.h"

CEvent::CEvent(CEvent::Type type)
{
    m_type = type;
}

CEvent::CEvent(const CEvent &other)
{
    m_type = other.m_type;
    m_bAccept = other.m_bAccept;
    m_ref = other.m_ref.value() + 1;
}

CEvent::~CEvent()
{
    if(m_ref.deref())
    {
        destroy();
    }
}

CEvent &CEvent::operator=(const CEvent &other)
{
    m_type = other.m_type;
    m_bAccept = other.m_bAccept;
    m_ref = other.m_ref.value() + 1;
    return *this;
}

int CEvent::registerEventType(int eint)
{
    return eint;
}
