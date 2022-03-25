
#include "socket.h"
#include "iostream"

Socket::Socket(int port, std::string ip_address) {
    if (!check_and_start_if_windows()) {exit(EXIT_FAILURE);}
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in *socket_address = (sockaddr_in *) &address;
    socket_address->sin_family = AF_INET;
    socket_address->sin_port = htons(port);
    if(ip_address == ""){
        socket_address->sin_addr.s_addr = INADDR_ANY;
    }
    else{
        socket_address->sin_addr.s_addr = inet_addr(ip_address.c_str());
    }
}

bool Socket::check_and_start_if_windows() {
    #ifdef ON_WINDOWS
        WSADATA wsa_data;
        if(WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0){
            std::cout << "failed at WSAStartup()" << std::endl;
            return false;
        }
    #endif 
    return true;
}

void Socket::get_sock(SOCKET_TYPE &sock) {
    sock = this->sock;
}