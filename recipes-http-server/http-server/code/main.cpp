#include "server.h"
#include <mutex>
#include <queue>
#include <sys/socket.h>
#include <pthread.h>

std::queue<int> q;
std::mutex queue_mutex;

void* Process(void* server){
	((Server*)server)->StartProcessing(&q, &queue_mutex);
}

int main(){
	Server serv(8080);
	pthread_t processing_thread;
	int processing_fd = pthread_create(&processing_thread, NULL, Process, (void*)&serv);

	while(true){
		int fd = serv.AcceptConnection();
		std::lock_guard<std::mutex> lock(queue_mutex);
		q.push(fd);
	}
}
