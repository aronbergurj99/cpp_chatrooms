#ifndef SERVER_987896876
#define SERVER_987896876

#include "socket.h"

class Server : public Socket {

private:
    bool bind_socket_to_address();
    bool listen_at_socket();
    bool accept_connection(SOCKET_TYPE &new_socket);
    void run();

    SOCKET_TYPE listening;

public:
    Server(int port=TCP_PORT, std::string ip_address = "");
};

#endif //SERVER_987896876