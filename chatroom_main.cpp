#include "networking/server.h"
#include "networking/client.h"


//dev
#include "models/network_data.h"
#include <iostream>
using namespace std;



int main (int argc, char *argv[])
{
    if (argc > 1) {
        if (strcmp(argv[1], "server") == 0) {
            Server * server = new Server();
        } else {
            Client * client = new Client(TCP_PORT, argv[1]);
        }
    }
    
    // NetworkData data = {message, "Hello aron\0"};
    // cout << data.action << " and " << data.data << endl;

    // char *some_buffer;
    // int data_len = data.serialize(some_buffer);
    // NetworkData data2;
    // data2.deserialize(some_buffer);
    // cout << data2.action << " and " << data2.data << endl;

}