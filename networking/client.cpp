#include "client.h"
#include <iostream>

#include "../models/network_data.h"

using std::cout;
using std::endl;

Client::Client(int port, std::string ip_address) : Socket(port, ip_address) {
    Socket::get_sock(this->client_sock);
    if (!connect_to_server()) exit(1);

    int incoming_len;
    NetworkData data;

    recv(this->client_sock, (char*) &incoming_len, sizeof(int), 0);
    char * buf = new char[incoming_len];

    recv(this->client_sock, buf, incoming_len, 0);
    cout << incoming_len << endl;

    data.deserialize(buf);
    cout << data.data << endl;
    delete[] buf;


}

bool Client::connect_to_server(){
    if(connect(this->client_sock, (sockaddr *) &this->address, sizeof(this->address)) < 0){
        cout << "connection failed " << endl;
        return false;
    }
    cout << "connected " << endl;
    return true;
}