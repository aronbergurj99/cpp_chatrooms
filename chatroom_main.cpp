#include "networking/server.h"

//dev

int main (int argc, char *argv[])
{
    // if (argc > 1) {S
    //     if (strcmp(argv[1], "server") == 0) {
    //         cout << "Server" << endl;
    //     } else {
    //         cout << "Client" << endl;
    //     }
    // }
    Server *server = new Server();
    // NetworkData data = {message, "Hello aron\0"};
    // cout << data.action << " and " << data.data << endl;

    // char *some_buffer;
    // int data_len = data.serialize(some_buffer);
    // NetworkData data2;
    // data2.deserialize(some_buffer, data_len);
    // cout << data2.action << " and " << data2.data << endl;

}