
all: ChatPP

ChatPP: chatroom_main.cpp networking/server.cpp networking/socket.cpp models/network_data.cpp
	g++ -std=c++11 -o chatpp chatroom_main.cpp networking/server.cpp networking/socket.cpp models/network_data.cpp -lws2_32
