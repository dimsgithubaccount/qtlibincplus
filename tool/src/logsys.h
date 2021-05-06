#ifndef CLOGSYSTEM_H
#define CLOGSYSTEM_H

#include "core/src/cobject.h"

class CLogSystem : CObject
{
    explicit CLogSystem(CObject *parent = nullptr);
public:
    ~CLogSystem();

    static CLogSystem& instance();

    static CLogSystem* instance_ptr() { return &instance(); }

    void start();

    void stop();
private:
    DISABLE_COPY(CLogSystem)
};

#endif // CLOGSYSTEM_H
