#ifndef CTCPCONNECT_H
#define CTCPCONNECT_H

#include <QObject>

#include <QtCore/qglobal.h>

#ifndef LIBTEST
#if defined(LIBSHARED_NETTCP_EXPORT)
#  define LIBSHARED_NETTCP_EXPORT Q_DECL_EXPORT
#else
#  define LIBSHARED_NETTCP_EXPORT Q_DECL_IMPORT
#endif
#else
#  define LIBSHARED_NETTCP_EXPORT
#endif

#ifdef linux
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"

#ifndef INVALID_SOCKET
#define INVALID_SOCKET int(~0)
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#define SOCKET int
#define closesocket(a) close(a)
#define SOCKADDR_IN sockaddr_in
#define SOCKADDR sockaddr
#define lpSOCKET SOCKET

#elif WIN32
#include "winsock2.h"

#ifdef _WIN64
#define lpSOCKET long long
#else
#define lpSOCKET int
#endif
#endif

class CTcpListener;
class CTcpConnector;
class CTcpReceiver;
class CTcpSender;

class CTcpSenderPrivate;

class LIBSHARED_NETTCP_EXPORT CTcpServer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CTcpServer)
    CTcpSenderPrivate* /*const*/ _d;
public:
    explicit CTcpServer(QObject *parent = nullptr);

    ~CTcpServer();

    CTcpListener* createListener(const QString &bindip, quint16 bindport);
    void destroyListener(const QString &listenip, quint16 listenport);
signals:

};



class CTcpClientPrivate;

class LIBSHARED_NETTCP_EXPORT CTcpClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CTcpClient)
    CTcpClientPrivate* /*const*/ _d;
public:
    explicit CTcpClient(QObject *parent = nullptr);

    ~CTcpClient();


signals:

};


#endif // CTCPCONNECT_H
