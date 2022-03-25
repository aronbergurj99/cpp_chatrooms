#include "network_data.h"
#include <string.h>

using std::string;

int NetworkData::serialize(char* &buffer) {
    int total_length;
    total_length += sizeof(Actions) + sizeof(int);
    total_length += data.length() + 1;
    buffer = new char[total_length];
    
    char *ptr = buffer;
    *((Actions *)ptr) = action;
    ptr += sizeof(Actions);

    //set the len of data;
    *((int *)ptr) = data.length();
    ptr += sizeof(int);
    

    strcpy(ptr, data.c_str());
    return total_length;
}

void NetworkData::deserialize(char *buffer, int length) {
    action = *((Actions *)(buffer));
    buffer += sizeof(Actions);

    int str_len = *((int *) (buffer));
    buffer += sizeof(int);

    string tmp_str;
    for (int i = 0; i < str_len; i++) {
        tmp_str.push_back(*(buffer++));   
    }
    tmp_str.push_back('\0');
    data = tmp_str;
}