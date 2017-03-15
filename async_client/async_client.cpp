#ifdef __linux__
#include <errno.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/epoll.h>
#include "async_client.h"
#include "helper.h"


#define MAX_EVENT_NUM 8
#define EPOLL_WAIT_TIMEOUT 1000  // milliseconds
#define MAX_CONNECT_TIME 10 // seconds


AsyncClient::AsyncClient(const std::string& host, unsigned short port, AsyncClientCallback* callback)
    : m_host(host), m_port(port), m_callback(callback)
{
    m_epollFd = ::epoll_create(MAX_EVENT_NUM);
    if (m_epollFd < 0) {
        std::cerr << "epoll_create fail, errno=" << errno << ",msg=" << strerror(errno) << std::endl;
        exit(-1);
    }

    _connect();
}

AsyncClient::~AsyncClient() {
    _close();

    ::close(m_epollFd);
    m_epollFd = -1;
}

void AsyncClient::_connect() {
    if (m_connectBeginTime == 0) {
        m_connectBeginTime = time(NULL);
    }

    m_socketFd = vcs::Helper::Socket::connect(m_host.c_str(), m_port);
    if (m_socketFd < 0) {
        std::cerr << "vcs::Helper::Socket::connect fail, errno=" << errno << ",msg=" << strerror(errno) << std::endl;
        return;
    }

    struct epoll_event ev = {0};
    ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
    ev.data.fd = m_socketFd;
    if(epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_socketFd, &ev) < 0) {
        std::cerr << "epoll_ctl add fd(" << m_socketFd << ") fail, errno=" << errno << ",msg=" << strerror(errno) << std::endl;
        _close();
        return;
    }
}

void AsyncClient::_close() {
    m_connected = false;
    m_readBuffer.shrink(m_readBuffer.size());
    m_writeBuffer.shrink(m_writeBuffer.size());

    if (m_socketFd < 0) {
        return;
    }
    ::close(m_socketFd);
    m_socketFd = -1;
}

void AsyncClient::reconnect() {
    _close();
}

bool AsyncClient::read(vcs::Buffer* buffer) {
    if (!m_connected) {
        return false;
    }
    buffer->append(&m_readBuffer);
    m_readBuffer.shrink(m_readBuffer.size());
    return true;
}

bool AsyncClient::write(const char* data, size_t count) {
    if (!m_connected) {
        return false;
    }

    m_writeBuffer.write(data, count);
    if (m_writeBuffer.size() > 0){
        if(!vcs::Helper::Socket::write(m_socketFd, &m_writeBuffer)) {
            std::cerr << "vcs::Helper::Socket::write fail, errno=" << errno << ",msg=" << strerror(errno) << std::endl; 
            _close();
        }
    }
    return true;
}

void AsyncClient::dispatch() {
    // reconnect if need
    if (m_socketFd < 0) {
        std::cerr << "connection lost, try to reconnect, host=" << m_host << ",port=" << m_port << std::endl;
        _connect();
        usleep(20000);
    }

    if (m_connectBeginTime != 0 && time(NULL) - m_connectBeginTime > MAX_CONNECT_TIME) {
        std::cerr << "can not connect to map server, has tried " << MAX_CONNECT_TIME << " seconds";
        usleep(800000);
        std::abort();
    }

    // epoll loop 
    struct epoll_event events[MAX_EVENT_NUM];
    int nfds = ::epoll_wait(m_epollFd, events, MAX_EVENT_NUM, EPOLL_WAIT_TIMEOUT);
    if (nfds < 0) {
        if (errno != EINTR) {
            std::cerr << "epoll_wait fail, errno=" << errno << ",msg=" << strerror(errno) << std::endl;
        }
        return;
    }

    for (int i = 0; i < nfds; ++i) {
        if (m_socketFd != events[i].data.fd) {
            std::cerr << "something strange wrong, fd mix, m_socketFd=" << m_socketFd
                << ",events[" << i << "].data.fd=" << events[i].data.fd;

            ::close(events[i].data.fd);
            continue;
        }

        if (events[i].events & EPOLLIN) {
            if (!vcs::Helper::Socket::read(events[i].data.fd, &m_readBuffer)) {
                std::cerr << "vcs::Helper::Socket::read fail, errno=" << errno << ",msg=" << strerror(errno);
                if (errno != EINPROGRESS) {
                    _close();
                }
                continue;
            }
        }
        
        if (events[i].events & EPOLLOUT) {
            if (!m_connected) {
                int result;
                socklen_t result_len = sizeof(result);
                if ((getsockopt(events[i].data.fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0) || (result != 0)) {
                    std::cerr << "connect fail for socket fd " << events[i].data.fd << std::endl;
                    _close();
                    continue;
                } else {
                    m_connected = true;
                    m_callback->onConnected(this);
                    m_connectBeginTime = 0;
                }
            }

            if (m_writeBuffer.size() > 0){
                if(!vcs::Helper::Socket::write(events[i].data.fd, &m_writeBuffer)) {
                    std::cerr << "vcs::Helper::Socket::write fail, errno=" << errno << ",msg=" << strerror(errno) << std::endl; 
                    _close();
                }
            }
        }
    }

}
#else
#include "easylogging++.h"
#include "async_client.h"

AsyncClient::AsyncClient(const std::string& host, unsigned short port, AsyncClientCallback* callback)
{
    LOG(ERROR) << "only linux support AsyncClient";
    abort();

}

#endif
