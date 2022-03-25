#include "server.h"
#include <iostream>


Server::Server(int port, std::string ip_adress) : Socket(port, ip_adress) {
    Socket::get_sock(this->listening);
    bind_socket_to_address();
    listen_at_socket();
    std::cout << "Server starting" << std::endl;
    run();
}

bool Server::bind_socket_to_address() {
    if(bind(this->listening, &this->address, sizeof(this->address)) < 0) {
        std::cout << "bind failed" << std::endl;
        return false;
    }
    std::cout << "Bind complete" << std::endl;
    return true;
}

bool Server::listen_at_socket() {
    if(listen(this->listening, 1) < 0) {
        std::cout << "listen failed" << std::endl;
        return false;
    }
    std::cout << "Listen complete" << std::endl;
    return true;
}

bool Server::accept_connection(SOCKET_TYPE &new_socket) {
    ADDRESS_SIZE addr_size = sizeof(this->address);
    new_socket = accept(this->listening, &this->address, &addr_size);
    if(new_socket == -1 || new_socket == ~0){
        std::cout << "invalid socket" << std::endl;
        return false;
    }
    std::cout << "New connection" << std::endl;
    return true;
}

void Server::run(){
    std::cout << "Running\n"; 
}