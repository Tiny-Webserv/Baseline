#include "Request.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include "ServerBlock.hpp"
#include "Config.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 80

int main(int argc, char const *argv[])
{
	(void) argc;
    Config con(static_cast<std::string>(argv[1]));
	int server_fd, new_socket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	const char	*hello;
	// Only this line has been changed. Everything is same.
	std::ifstream file("./docs/error.html");
	if (!file.is_open())
		return 1;
	std::stringstream response_body;
	response_body << file.rdbuf();
	std::string response_body_str = response_body.str();
	std::string response_message = "HTTP/1.1 200 OK\n"
									"Content-Type: text/html\n"
									"Content-Length: " + std::to_string(response_body_str.length()) + "\n\n"
									+ response_body_str;
	hello = response_message.c_str();
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}


	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	memset(address.sin_zero, '\0', sizeof address.sin_zero);


	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("In bind");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0)
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}
	std::map<int, Request *> clnts;
	while(1)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
		{
			perror("In accept");
			exit(EXIT_FAILURE);
		}
		fcntl(new_socket, F_SETFL, fcntl(new_socket, F_GETFL) | O_NONBLOCK);
		std::ofstream _file("./img.png", std::ios::binary);
		if (!_file)
			return (1);
		ParseRequest(new_socket, clnts, con._ServerBlockObject);
		Request *req = clnts[new_socket];

		int j = 0;
		for (size_t	i = 0; i < req->getBinary().size(); i++) {
			_file.write(&req->getBinary()[0], req->getBinary().size());
			std::cout << "rmain : " << j++ << std::endl;
		}

                //char *buff =
                //    new char[req->getBinary().size()]; // 동적 메모리 할당
                //std::copy(req->getBinary().begin(), req->getBinary().end(),
                //          buff);
                //_file.write(buff, req->getBinary().size());
                //delete[] buff; // 동적 메모리 해제

           _file.close();
                write(new_socket , hello , strlen(hello));
		printf("------------------Hello message sent-------------------");
		close(new_socket);
	}
	return 0;
}

//int  main(int argc, char *argv[]) {
//	(void) argc;
//    Config con(static_cast<std::string>(argv[1]));

//// 1. 리퀘스트 파일 안에 리퀘스트문 추가
//	ServerBlock serverBlock;
//	std::map<int, Request *> clients;
//	int fd = open("./docs/requestTest1", O_RDONLY, 0666);
//	std::cout << "fd : " << fd << std::endl;
//	ParseRequest(fd, clients, con._ServerBlockObject);
//	close(fd);
//	fd = open("./docs/requestTest2", O_RDONLY, 0666);
//	ParseRequest(fd, clients, con._ServerBlockObject);
//	close(fd);
//	std::cout << clients[fd]->GetErrorCode() << std::endl;
//	fd = open("./docs/requestTest3", O_RDONLY, 0666);
//	ParseRequest(fd, clients, con._ServerBlockObject);
//	close(fd);
//	std::cout << "================================" << std::endl;
//	std::cout << clients[fd]->GetStream().str() << std::endl;
//	std::cout << clients[fd]->GetErrorMessages() << std::endl;
//	std::cout << clients[fd]->GetIsEnd() << std::endl;
//	return 0;

//}