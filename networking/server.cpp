#include "server.h"
#include "../models/network_data.h"
#include <iostream>
#include <vector>

using std::vector;

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
    fd_set master;
    FD_ZERO(&master);
    FD_SET(this->listening, &master); // add the listening port to the fd_set

    bool running = true;
    while (running) {
        fd_set copy = master;
        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
        for (int i =0; i < socketCount; i++) {
            SOCKET_TYPE curr_sock = copy.fd_array[i];
            if (curr_sock == this->listening) { // server socket
                SOCKET_TYPE client;
                Server::accept_connection(client);
                FD_SET(client, &master);

                NetworkData data = {message, "Welcome to cpp chatrooms, please login using command:\n\t-l <username> <password>\0"};
                char * buf;
                int len = data.serialize(buf);
                send(client, (char*) &len, sizeof(int), 0);
                send(client, buf, len, 0);
                std::cout << "SEND DATA" << std::endl;
                delete[] buf;
            } else {
                int incoming_bytes;
                NetworkData incoming_data;
                int bytes_recv = 0;
                // int bytes_recv = recv(curr_sock, (char *) &incoming_bytes, sizeof(int), 0);
                // char * buf = new char[incoming_bytes];
                // bytes_recv += recv(curr_sock, buf , incoming_bytes, 0);
                // incoming_data.deserialize(buf);
                
                if (bytes_recv <= 0) {
                    std::cout<< "DISCONNECT" << std::endl;
                    closesocket(curr_sock);
                    FD_CLR(curr_sock, &master);
                    continue;
                } else {
                    //we got data from socket.
                    std::cout << incoming_data.data << std::endl;
                }
            
            }

        }
    }
}