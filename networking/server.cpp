#include "server.h"
#include <iostream>
#include <fstream>

using namespace std;

struct User {
    char username[32];
    char password[32];
};

bool parse_user(std::string ss, User &user) {
    string s = ss;
    string uTmp;
    string pTmp;
    uTmp = s.substr(0, s.find(" "));
    s.erase(0, s.find(" ") + 1);
    pTmp = s;
    if (uTmp.size() <= 32 && pTmp.size() <=32) {
        strcpy(user.username, uTmp.c_str());
        strcpy(user.password, pTmp.c_str());
        return true;
    }
    return false;
}

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

                NetworkData data = {message, "\nWelcome to cpp chatrooms, please login/signup using command(no spaces in in password or username):\n-l|-s <username> <password>\0"};
                send_message(data, client);
            } else {
                NetworkData data;
                int bytes_recv = recv_message(data, curr_sock);
                
                if (bytes_recv <= 0) {
                    std::cout<< "DISCONNECT" << std::endl;
                    closesocket(curr_sock);
                    FD_CLR(curr_sock, &master);
                    continue;
                } else {
                    //we got data from socket.
                    NetworkData resp;
                    resp.action = message;
                    if (!users.count(curr_sock)) {
                        //if the user is not logged in
                    
                        switch (data.action)
                        {
                        case login_cmnd:

                            handle_login(data, curr_sock);

                            break;
                        case signup_cmnd:
                            handle_signup(data, curr_sock);
                            break;
                        default:
                            resp.data = "You must be logged in!!!";
                            send_message(resp, curr_sock);
                        }
                    } else {
                        //user is logged in
                        NetworkData resp;
                        resp.action = message;
                        switch (data.action) {
                            case list_cmnd:
                                break;
                            default:
                                resp.data = "Invalid command";
                                send_message(resp, curr_sock);
                                break;
                        }
                    }

                }
            
            }

        }
    }
}

void Server::handle_login(NetworkData &data, SOCKET_TYPE &sock) {
    fstream db;
    User user;
    NetworkData resp = {message, "Loggin in failed, please try again\0"};
    if (!parse_user(data.data, user)) {
        send_message(resp, sock);
        return;
    }
    db.open("networking/persistance/users.fic", ios::binary|ios::in|ios::out|ios::app);
    db.clear();
    db.seekg(0, ios::beg);
    User readUser;
    while(!db.eof()) {
        db.read((char*) &readUser, sizeof(User));
        if (strcmp(readUser.username, user.username) == 0 && strcmp(readUser.password, user.password) == 0) {
            db.close();
            users[sock] = user.username;
            resp.data= "Logged in";
            send_message(resp, sock);
            return;
        }
    }
    
    db.close();
    send_message(resp, sock);

}

void Server::handle_signup(NetworkData &data, SOCKET_TYPE &sock) {
    fstream db;
    User user;
    NetworkData resp = {message, "\nSignup failed, user exists or password and username is longer then 32chars.\n\0"};
    if (!parse_user(data.data, user)) {
        send_message(resp, sock);
        return;
    }
    db.open("networking/persistance/users.fic", ios::binary|ios::in|ios::out|ios::app);
    db.clear();
    db.seekg(0, ios::beg);
    User readUser;
    while(!db.eof()) {
        db.read((char*) &readUser, sizeof(User));
        if (strcmp(readUser.username, user.username) == 0) {
            db.close();
            send_message(resp, sock);
            return;
        }
    }
    // user name is not taken;
    db.clear();
    db.seekp(0, ios::end);
    db.write((char*)&user, sizeof(User));
    db.close();
    resp.data = "Signed up and logged in :)\n\0";
    users[sock] = user.username;
    send_message(resp, sock);
}

int recv_message(NetworkData &data, SOCKET_TYPE &sock) {
    int len;
    int bytes;
    bytes = recv(sock, (char*) &len, sizeof(int), 0);
    char * buf = new char[len];
    bytes += recv(sock, buf, len, 0);
    data.deserialize(buf);
    delete[] buf;
    return bytes;
}
void send_message(NetworkData &data, SOCKET_TYPE &sock) {
    char * buf;
    int len = data.serialize(buf);
    send(sock, (char *) &len, sizeof(int), 0);
    send(sock, buf, len, 0);
    delete[] buf;
}

