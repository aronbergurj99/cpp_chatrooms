
all: ChatPP

ChatPP: chatroom_main.cpp networking/server.cpp networking/socket.cpp models/network_data.cpp networking/client.cpp
	g++ -std=c++11 -o chatpp chatroom_main.cpp networking/server.cpp networking/socket.cpp models/network_data.cpp networking/client.cpp -lws2_32
