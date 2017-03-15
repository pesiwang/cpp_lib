#ifndef ASYNC_CLIENT_H
#define ASYNC_CLIENT_H

#include <string>
#include <sys/epoll.h>
#include "buffer.h"


class AsyncClient;

class AsyncClientCallback {
public:
    virtual void onConnected(AsyncClient* client) = 0;
    virtual ~AsyncClientCallback(){}
};

#ifdef __linux__
class AsyncClient {
public:
    AsyncClient(const std::string& host, unsigned short port, AsyncClientCallback* callback);
    ~AsyncClient();
    
    bool read(vcs::Buffer* buffer);
    bool write(const char* data, size_t count);
    void dispatch();
    void reconnect();

private:
    void _connect();
    void _close();
    
private:
    std::string m_host;
    unsigned short m_port;
    AsyncClientCallback* m_callback = nullptr;

    int m_epollFd   = -1;
    int m_socketFd  = -1;
    bool m_connected = false;
    time_t m_connectBeginTime = 0;

    vcs::Buffer m_readBuffer;
    vcs::Buffer m_writeBuffer;

};
#else
class AsyncClient {
public:
    AsyncClient(const std::string& host, unsigned short port, AsyncClientCallback* callback);
    ~AsyncClient(){};
    
    bool read(vcs::Buffer* buffer){ return false; };
    bool write(const char* data, size_t count){ return false; };
    void dispatch(){};
    void reconnect(){};
};

#endif


#endif


