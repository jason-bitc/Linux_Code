.PHONY:all
all:client_chat server_chat

server_chat:ChatServer.cc $(wildcard *.hpp)
	g++ -o $@ $< -std=c++17 -pthread
client_chat:ChatClient.cc $(wildcard *.hpp)
	g++ -o $@ $< -std=c++17 -pthread -static

.PHONY:clean
clean:
	rm -f client_chat server_chat
