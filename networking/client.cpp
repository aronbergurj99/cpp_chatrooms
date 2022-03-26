#include "client.h"
#include <iostream>
#include "server.h"

#include "../models/network_data.h"
#include <thread>
#include <mutex>

using std::cout;
using std::endl;

using namespace std;

bool running = true;
mutex print_mutex;
mutex input_mutex;



bool parse_input(NetworkData &data, string inp) {
    if (inp.size() < 2) {
        return false;
    }
    data.action = message;
    if (inp[0] == '-') {
        //command

        char command = inp[1];
        switch (command)
        {
        case 'l':
            data.action = login_cmnd;
            
            break;
        case 's':
            data.action = signup_cmnd;
        
            break;
        case 'c':
            //create chatroom
            data.action = create_cmnd;

            break;
        case 'a':
            //list chatrooms
            data.action = list_cmnd;
            break;
        case 'j':
            //join chatroom
            data.action = join_cmnd;
            break;
        case 'e':
            //leave chatroom
            data.action = leave_cmnd;
            break;
        case 'q':
            data.action = quit_cmnd;
            running = false;
            break;
        default:
            //unknown
            return false;

            break;
        }        
    } else {
        //message
        data.action = message;
        data.data = inp;
        return true;
    }
    inp.erase(0,3);
    
    data.data = inp;
    return true;
}


void input(SOCKET_TYPE sock) {
    while (running) {
        string inp;
        // input_mutex.lock();
        getline(cin, inp);
        // input_mutex.unlock();
        NetworkData data;
        if (parse_input(data, inp)){
            send_message(data, sock);
        }
    }
}

void output(SOCKET_TYPE sock) {
    while(running) {
        NetworkData data;
        recv_message(data, sock);
        // print_mutex.lock();
        cout << data.data << endl;
        // print_mutex.unlock();
    }
}

Client::Client(int port, std::string ip_address) : Socket(port, ip_address) {
    Socket::get_sock(this->client_sock);
    if (!connect_to_server()) exit(1);

    // NetworkData data;
    // recv_message(data, this->client_sock);
    // cout << data.data << endl;

    // data.action = login_cmnd;
    // data.data = "aron bergur";
    // send_message(data, this->client_sock);
    // NetworkData data1;
    
    // recv_message(data1, this->client_sock);
    // cout <<data1.data<<endl;
    std::thread i_thread = std::thread(input, this->client_sock);
    std::thread o_thread = std::thread(output, this->client_sock);
    i_thread.join();
    o_thread.join();
}

bool Client::connect_to_server(){
    if(connect(this->client_sock, (sockaddr *) &this->address, sizeof(this->address)) < 0){
        cout << "connection failed " << endl;
        return false;
    }
    cout << "connected " << endl;
    return true;
}