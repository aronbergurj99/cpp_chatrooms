#ifndef CLIENT_234324
#define CLIENT_234324

#include "socket.h"

class Client : public Socket
{
    private:
        SOCKET_TYPE client_sock;

        bool connect_to_server();
    public:
        Client(int port = TCP_PORT, std::string ip_address = "");
};

#endif //CLIENT_234324