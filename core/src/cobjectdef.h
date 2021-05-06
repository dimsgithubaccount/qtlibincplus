#ifndef COBJECTDEF_H
#define COBJECTDEF_H

#define DISABLE_COPY(T)     T(const T &other) = delete; \
    T& operator=(const T &other) = delete;
#define VERIFY_RET(a) if(!(a)) return
#define VERIFY_RETVAL(a, b) if(!(a)) return b
#define VERIFY_CNT(a) if(!(a)) continue
#define VERIFY_BRK(a) if(!(a)) break
#define VERIFY_GOTO(a, b) if(!(a)) goto b

#endif // COBJECTDEF_H
