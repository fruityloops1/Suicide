#pragma once

#include "types.h"

#include <alloca.h>
#include <cstdio>
#include <netinet/in.h>

extern "C" {
#include "switch/kernel/svc.h"
}

namespace nxdb {

class LogServer {
    u16 m_Port = 2404;
    int m_Fd = -1;
    struct sockaddr_in m_ServAddr, m_CliAddr;
    u32 m_CliAddrLen = sizeof(m_CliAddr);
    bool m_Connected = false;

public:
    LogServer(u16 port = 2404);

    static LogServer*& instance();

    bool IsConnected() const { return m_Connected; }
    void StartServer();
    void Poll();
    void LogMsg(const char* msg);

    template <typename... Args>
    void Log(const char* fmt, Args... args)
    {
#ifdef LOGGING
        size_t size = snprintf(nullptr, 0, fmt, args...);
        char* buf = (char*)alloca(size + 2);
        snprintf(buf, size + 1, fmt, args...);
        buf[size] = '\n';
        buf[size + 1] = '\0';
        LogMsg(buf);
#endif
    }
};

template <typename... Args>
void log(const char* fmt, Args... args)
{
    LogServer::instance()->Log(fmt, args...);
}

} // namespace nxdb
