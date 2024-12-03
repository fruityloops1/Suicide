#include "LogServer.h"
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

extern "C" {
#include "switch/runtime/diag.h"
}

namespace nxdb {

    static LogServer* sInstance = nullptr;
    LogServer*& LogServer::instance() { return sInstance; }

    LogServer::LogServer(u16 port)
        : m_Port(port) {
#ifdef LOGGING
        StartServer();
#endif
    }

    void LogServer::StartServer() {
#ifdef LOGGING
        if (m_Fd != -1)
            close(m_Fd);

        m_Fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_Fd < 0) {
            diagAbortWithResult(55555);
            return;
        }

        memset(&m_ServAddr, 0, sizeof(m_ServAddr));
        memset(&m_CliAddr, 0, sizeof(m_CliAddr));

        m_ServAddr.sin_family = AF_INET;
        m_ServAddr.sin_addr.s_addr = INADDR_ANY;
        m_ServAddr.sin_port = htons(m_Port);

        if (bind(m_Fd, (const struct sockaddr*)&m_ServAddr, sizeof(m_ServAddr)) < 0) {
            diagAbortWithResult(44444);
            return;
        }
#endif
    }

    void LogServer::Poll() {
#ifdef LOGGING
        // if (!m_Connected)
        //     StartServer();

        u8 buf[128];
        int n = recvfrom(m_Fd, buf, 128, MSG_DONTWAIT, (struct sockaddr*)&m_CliAddr, &m_CliAddrLen);
        if (n > 0) {
            m_Connected = true;
        }
#endif
    }

    void LogServer::LogMsg(const char* msg) {
#ifdef LOGGING
        sendto(m_Fd, msg, strlen(msg), 0, (const struct sockaddr*)&m_CliAddr, m_CliAddrLen);
#endif
    }

} // namespace nxdb
