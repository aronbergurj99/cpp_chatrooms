#ifndef NETWORK_DATA_1231245
#define NETWORK_DATA_1231245

#include <string>


enum Actions {
    login_cmnd,
    signup_cmnd,
    list_cmnd,
    create_cmnd,
    join_cmnd,
    quit_cmnd,
    message,
};

struct NetworkData {
    Actions action;
    std::string data;
    int serialize(char* &buffer);
    void deserialize(char *buffer);
};

#endif //NETWORK_DATA_1231245