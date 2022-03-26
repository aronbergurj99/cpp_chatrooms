#ifndef SERVER_987896876
#define SERVER_987896876

#include "socket.h"
#include "../models/network_data.h"
#include <vector>
#include <map>
using std::vector;
using std::map;

class Server : public Socket {

private:
    bool bind_socket_to_address();
    bool listen_at_socket();
    bool accept_connection(SOCKET_TYPE &new_socket);
    void run();
    void handle_login(NetworkData &data, SOCKET_TYPE &sock);
    void handle_signup(NetworkData &data, SOCKET_TYPE &sock);
    void handle_create(NetworkData &data, SOCKET_TYPE &sock);
    void handle_list(NetworkData &data, SOCKET_TYPE &sock);
    void load_chatrooms();
    void handle_join(NetworkData &data, SOCKET_TYPE &sock);
    void handle_msg(NetworkData &data, SOCKET_TYPE &sock);
    void handle_leave(NetworkData &data, SOCKET_TYPE &sock);
    void handle_quit(NetworkData &data, SOCKET_TYPE &sock, fd_set &set);

    map<SOCKET_TYPE, std::string> inchatroom;
    map<SOCKET_TYPE, std::string> users;
    map<std::string, vector<SOCKET_TYPE>> chatrooms;
    SOCKET_TYPE listening;

public:
    Server(int port=TCP_PORT, std::string ip_address = "");
};

int recv_message(NetworkData &data, SOCKET_TYPE &sock);
void send_message(NetworkData &data, SOCKET_TYPE &sock);

#endif //SERVER_987896876