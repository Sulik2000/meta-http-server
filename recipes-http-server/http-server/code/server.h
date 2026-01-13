#include <asm-generic/socket.h>
#include <cstdlib>
#include <functional>
#include <queue> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <mutex>
#include <sys/socket.h>
#include <unistd.h>
#include "request.h"
#include <cstdio>
#include <iostream>

class Server {
	unsigned int PORT;
	int server_fd;
	sockaddr_in address;
	bool _processing = false;

	std::string GenerateResponse(char* request, int n);
	HTTP_Request GetHttpRequest(char* request, int n);
	// Reads text from file in folder {programDir}/static/*
	// Returns: contain of file
	std::string OpenStaticFile(std::string path);
	void TurnOnLed(bool turnOn);
public:	
	Server(unsigned int port);
	// Accepts an incoming connection request from the listen queue.
	// Returns: A file descriptor for the accepted socket, or -1 on error.
	int AcceptConnection();
	void Close();
	// Processes connections from queue;
	// fds - reference to queue where is accepted fds
	void StartProcessing(std::queue<int> *fds, std::mutex *_queue_mutex);
	// Stops processing after last acception and closes all sockets
	void StopProcessing();
};


