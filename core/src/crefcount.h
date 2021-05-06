#ifndef CREF_H
#define CREF_H

#include <atomic>

class CRefCount
{
    std::atomic_int m_ref;
public:
    CRefCount(int ref = 1) { m_ref = ref; }
    CRefCount(const CRefCount &other) { int v = other.m_ref; m_ref = v; }
    ~CRefCount() {}
    CRefCount& operator=(const CRefCount &other)
    {
        m_ref = other.m_ref + 1;
        return *this;
    }

    inline void ref() { ++m_ref; }
    /// returns true if refrence count is zero, otherwise returns false
    inline bool deref() { if(m_ref < 1) return true; return --m_ref < 1; }
    inline int value() const { return m_ref; }
};

#endif // CREF_H
