#include "server.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <sstream>

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

void Server::load_chatrooms(){
    fstream db;

    db.open("networking/persistance/chatrooms.fic", ios::binary|ios::in|ios::out|ios::app);
    db.clear();
    string tmp;
    while(!db.eof()) {
        db >> tmp;
        chatrooms[tmp] = vector<SOCKET_TYPE>();
    }
    db.close();
}

Server::Server(int port, std::string ip_adress) : Socket(port, ip_adress) {
    Socket::get_sock(this->listening);
    bind_socket_to_address();
    listen_at_socket();
    std::cout << "Server starting" << std::endl;
    load_chatrooms();
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

                NetworkData data = {message, "\nWelcome to cpp chatrooms, please login/signup using command(no spaces in in password or username):\n-l|-s <username> <password>\n\n"};
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
                            resp.data = "\nYou are not logged in, use -l or -s <username> <password> to login|signup\n";
                            send_message(resp, curr_sock);
                        }
                    } else {
                        //user is logged in
                        NetworkData resp;
                        resp.action = message;
                        switch (data.action) {
                            case list_cmnd:
                                handle_list(data, curr_sock);
                                break;
                            case create_cmnd:
                                handle_create(data, curr_sock);
                                break;
                            case join_cmnd:
                                handle_join(data, curr_sock);
                                break;
                            case message:
                                handle_msg(data, curr_sock);
                                break;
                            case leave_cmnd:
                                handle_leave(data, curr_sock);
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

void Server::handle_leave(NetworkData &data, SOCKET_TYPE &sock) {
    if(inchatroom.count(sock)) {
        //user is in chatroom.
        ostringstream ss2;
        ss2 << users[sock] << " left the room";
        data.data = ss2.str();
        handle_msg(data, sock);
        vector<SOCKET_TYPE> &vec = chatrooms[inchatroom[sock]];
        //remove user from chatroom.
        vec.erase(std::remove(vec.begin(), vec.end(), sock), vec.end());
        ostringstream ss;
        ss << "You left the chatroom: " << inchatroom[sock];
        data.data = ss.str();
        inchatroom.erase(sock);

    } else {
        data.data = "You are not in chatroom.\n\n";
    }
    send_message(data, sock);
}

void Server::handle_msg(NetworkData &data, SOCKET_TYPE &sock) {
    if(inchatroom.count(sock)) {
        string chatroom = inchatroom[sock];
        ostringstream ss;
        ss << users[sock] << ": " << data.data;
        data.data = ss.str();
        for (SOCKET_TYPE tmp : chatrooms[chatroom]) {
            if (tmp != sock) {

                send_message(data, tmp);
            }
        }
    } else {
        //user is not in chatroom and cant send msg.
        data.data = "You are not in chatroom, you can join with -j <chatrom> and -a to see all chatroms\n\n";
        send_message(data, sock);
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
            ostringstream ss;
            ss << "\n\nWelcome " << user.username << ", use -a to list chatroom, -c <chatroom> to create and -j <chatroom> to join!\n";

            resp.data= ss.str();
            send_message(resp, sock);
            return;
        }
    }
    
    db.close();
    send_message(resp, sock);

}

void Server::handle_list(NetworkData &data, SOCKET_TYPE &sock) {
    data.data ="Chatrooms:";
    send_message(data, sock);
    for (auto const& m : chatrooms) {
        ostringstream ss;
        ss << "\t" << m.first;
        data.data = ss.str();
        send_message(data, sock);
    }

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

void Server::handle_join(NetworkData &data, SOCKET_TYPE &sock) {
    string s = data.data;
    ostringstream ss;
    if(chatrooms.count(s)) {
        //chat room exists.
        ss << "Joining chatroom: " << s; 

        chatrooms[s].push_back(sock);
        inchatroom[sock] = s;
        data.data = "Joined.";
        handle_msg(data, sock);
        data.data = ss.str();
    } else {
        data.data = "Chatroom does not exist. You can create it with -c <chatroom> or see all chatrooms with -a";
    }
    send_message(data, sock);
}

void Server::handle_create(NetworkData &data, SOCKET_TYPE &sock) {
    fstream db;
    string s = data.data;
    NetworkData resp = {message, "Chatroom already exists"};
    //remove white space;
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    db.open("networking/persistance/chatrooms.fic", ios::binary|ios::in|ios::out|ios::app);
    s.push_back(' ');
    db.clear();
    db.seekg(0, ios::beg);
    while(!db.eof()) {
        string tmp;
        db >> tmp;
        if (strcmp(tmp.c_str(), s.c_str()) == 0) {
            db.close();
            send_message(resp, sock);
            return;
        }
    }
    db.clear();
    db.seekp(0, ios::end);
    db << s;
    chatrooms[s] = vector<SOCKET_TYPE>();
    resp.data = "Chatroom created, you can join it";
    db.close();
    send_message(resp, sock);
    return;
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

