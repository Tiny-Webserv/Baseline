#include "Request.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>

int  main() {

// 1. 리퀘스트 파일 안에 리퀘스트문 추가
	ServerBlock serverBlock;
	std::map<int, Request *> clients;
	int fd = open("./docs/RequestExample.md", O_RDONLY, 0666);
	std::cout << "fd : " << fd << std::endl;
	ParseRequest(fd, clients, serverBlock);
	close(fd);
	fd = open("./docs/delete.md", O_RDONLY, 0666);
	std::cout << "fd : " << fd << std::endl;
	ParseRequest(fd, clients, serverBlock);
	return 0;

}