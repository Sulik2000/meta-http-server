#include "server.h"
#include <cstdio>
#include <mutex>
#include <ostream>
#include <queue>
#include <string>
#include <sys/socket.h>
#include <fstream>

Server::Server(unsigned int port){
	this->PORT = port;
	// Initializing socket for listening

	server_fd = socket(AF_INET, SOCK_STREAM, 0);	
	if(server_fd < 0) {
		std::cout << "socket creating error\n";
		exit(EXIT_FAILURE);
	}
	int opt = 1;
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		std::cout << "setsockopt error\n";
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	if(bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0){
		std::cout << "Binding error\n";
		exit(EXIT_FAILURE);
	}
	if(listen(server_fd, 10) < 0){
		std::cout << "Listen socket error!\n";
		exit(EXIT_FAILURE);
	}
}

int Server::AcceptConnection() {
	socklen_t addrLen = sizeof(address);
	int client = accept(server_fd, (sockaddr*)&address, &addrLen);
	
	return client;
}

void Server::Close(){
	close(server_fd);
}

void Server::StartProcessing(std::queue<int> *fds, std::mutex *_queue_mutex){
	_processing = true;
	while(_processing){
		char request[4096];
		int fd;
		{
			std::lock_guard<std::mutex> lock(*_queue_mutex);
			fd = fds->front();
			if(fds->size() == 0)
				continue;
			fds->pop();
		}
		if(fd < 0)
			continue;
		int n = read(fd, &request, 4096);
		
		std::string response = GenerateResponse(request, n);

		write(fd, response.c_str(), response.size());
		
		shutdown(fd, SHUT_WR);
	}
	// Closing all sockets in queue
	while(fds->front())
	{
		std::lock_guard<std::mutex> lock(*_queue_mutex);
		fds->pop();
	}
}

HTTP_Request Server::GetHttpRequest(char* request, int n){
	HTTP_Request result;
	std::string buf = "";
	int last_index = 0;
	for(int i = 0; i < n; i++){
		if(request[i] == ' '){
			last_index = i;
			break;
		}
		buf.push_back(request[i]);
	}
	if(buf == "GET")
		result.Method = HTTP_Method::GET;
	else if(buf == "POST")
		result.Method = HTTP_Method::POST;
	buf = "";
	for(int i = last_index + 1; i < n; i++){
		if(request[i] == ' ' || request[i] == '?'){
			last_index = i;
			break;
		}
		buf.push_back(request[i]);
	}
	result.Url = buf;
	buf = "";
	std::string var = "";
	bool recordVal = false;
	// Parsing variables from HTTP GET request
	for(int i = last_index; i < n; i++){
		if(request[i] == ' '){
			if(recordVal)
				result.Variables.insert({buf, std::stoi(var)});
			break;
		}
		if(request[i] == '?')
			continue;

		if(request[i] == '='){
			recordVal = true;
			continue;
		}
		if(request[i] == ';'){
			recordVal = false;
			result.Variables.insert({buf, std::stoi(var)});
		}
		
		if(recordVal)
			var.push_back(request[i]);
		if(!recordVal)
			buf.push_back(request[i]);
	}

	return result;
}

std::string Server::OpenStaticFile(std::string path){
	std::string result = "";
	path.insert(0, "static");
	if(path.at(path.size() - 1) == '/')
		path += "index.html";
	std::cout << path << std::endl;
	std::FILE *flptr = std::fopen(path.c_str(), "r");
	if(flptr == nullptr)
		return "This file doesn't exists";
	
	char buf[4096];
	while(fgets(buf, 4096, flptr) != nullptr)
		result += buf;
	
	return result;
}

std::string Server::GenerateResponse(char* request, int n){
	HTTP_Request httpRequest = GetHttpRequest(request, n);
	
	if(httpRequest.Url == "/led"){
		try{
			TurnOnLed(httpRequest.Variables.at("on"));
			return "HTTP/1.1 200 OK\r\n"
                           "Connection: close\r\n" // або keep-alive
                           "\r\nOK";
		}
		catch(...){
			return "HTTP/1.1 200 OK\r\n"
                           "Connection: close\r\n" // або keep-alive
                           "\r\nOK";
		}
	}
	std::string file = OpenStaticFile(httpRequest.Url);

	std::string response = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Content-Length: " + std::to_string(file.size()) + "\r\n"
		"Connection: close\r\n"
		"\r\n" + file;

	std::cout << response;

	return response; 

}

void Server::StopProcessing(){
	_processing = false;
}

void Server::TurnOnLed(bool turnOn){
	std::string LedPath = "/sys/class/leds/heartbeat/";

	std::ofstream triggerFile(LedPath + "trigger");
	if(!triggerFile.is_open()){
		std::cout << "Failed open led trigger file!\n";
		return;
	}

	triggerFile << "none";
	triggerFile.close();

	std::ofstream brightnessFile(LedPath + "brightness");
	if(!brightnessFile.is_open()){
		std::cout << "Failed open led brightness file";
		return;
	}
	
	if(turnOn) {
		brightnessFile << "255";
		std::cout << "Heartbeat led is ON\n";
	}
	else {
		brightnessFile << "0";
		std::cout << "Heartbeat led is OFF\n";
	}
}
