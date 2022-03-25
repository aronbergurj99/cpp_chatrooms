#include "client.h"
#include <iostream>
#include "server.h"

#include "../models/network_data.h"

using std::cout;
using std::endl;

Client::Client(int port, std::string ip_address) : Socket(port, ip_address) {
    Socket::get_sock(this->client_sock);
    if (!connect_to_server()) exit(1);

    NetworkData data;
    recv_message(data, this->client_sock);
    cout << data.data << endl;

    data.action = login_cmnd;
    data.data = "aron bergur";
    send_message(data, this->client_sock);
    NetworkData data1;
    
    recv_message(data1, this->client_sock);
    cout <<data1.data<<endl;
    

}

bool Client::connect_to_server(){
    if(connect(this->client_sock, (sockaddr *) &this->address, sizeof(this->address)) < 0){
        cout << "connection failed " << endl;
        return false;
    }
    cout << "connected " << endl;
    return true;
}