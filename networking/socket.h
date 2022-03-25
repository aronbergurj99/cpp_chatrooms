#ifndef SOCKET_2393874938
#define SOCKET_2393874938

#include <string>

#define TCP_PORT 4037 //IPEN TCP PORTS ON SKEL: 4000-4100
#define UDP_PORT 4101 //IPEN UDP PORTS ON SKEL: 4100-4200

#ifdef _WIN32
    #define MY_OS "Windows"
    #include <WinSock2.h>
    typedef SOCKET SOCKET_TYPE;
    typedef int ADDRESS_SIZE;
    #define ON_WINDOWS
#else
    #define MY_OS "NOT Windows"
    typedef int SOCKET_TYPE;
    typedef unsigned int ADDRESS_SIZE;
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <errno.h>
   
#endif

class Socket {
public:
    Socket(int port, std::string ip_address = "");
private:
    bool check_and_start_if_windows();
    SOCKET_TYPE sock;

protected:
    sockaddr address;
    void get_sock(SOCKET_TYPE &sock);
};

#endif //SOCKET_2393874938