    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <unistd.h>

    #include <cstdio>
    #include <cstring>
    #include <fstream>
    #include <iostream>
    #include <map>
    #include <sstream>

    #include "Config.hpp"
    #include "Request.hpp"
    #include "Response.hpp"
    #include "ServerBlock.hpp"
    #include "ServerFiles.hpp"

    #define PORT 80

    int main(int argc, char const *argv[]) {
        // std::map<int, Response> ins;

        (void)argc;
        Config con(static_cast<std::string>(argv[1]));
        int server_fd, new_socket;
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        const char *hello;
        // Only this line has been changed. Everything is same.
        std::ifstream file("./docs/error.html");
        if (!file.is_open()) return 1;
        std::stringstream response_body;
        response_body << file.rdbuf();
        std::string response_body_str = response_body.str();
        std::string response_message =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " +
            std::to_string(response_body_str.length()) + "\r\n\r\n" +
            response_body_str;
        hello = response_message.c_str();
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("In socket");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        memset(address.sin_zero, '\0', sizeof address.sin_zero);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("In bind");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 10) < 0) {
            perror("In listen");
            exit(EXIT_FAILURE);
        }
        std::map<int, Request *> clnts;
        while (1) {
            printf("\n+++++++ Waiting for new connection ++++++++\n\n");
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                    (socklen_t *)&addrlen)) < 0) {
                perror("In accept");
                exit(EXIT_FAILURE);
            }
            fcntl(new_socket, F_SETFL, fcntl(new_socket, F_GETFL) | O_NONBLOCK);
            //std::ofstream _file("./img.png", std::ios::binary);
            //if (!_file) return (1);
            Request *req = ParseRequest(new_socket, clnts, con._ServerBlockObject);
            if (req == nullptr) {
                close(new_socket);
                continue;
            }

            // int j = 0;
            std::cout << "error code : " << req->GetErrorCode() << std::endl;
            // std::cout  << "req->getBinary().size() : " << req->getBinary().size()
            // << std::endl;
            std::cout << "test" << std::endl;
            Response res(req);
            //  ins[1] = res;
            //  for (size_t	i = 0; i < req->getBinary().size(); i++) {

            //_file.write(reinterpret_cast<const char *>(&req->getBinary()[0]),
            // req->getBinary().size()); std::cout << "rmain : " << j++ <<
            // std::endl;
            //}

            // char *buff =
            //     new char[req->getBinary().size()]; // 동적 메모리 할당
            // std::copy(req->getBinary().begin(), req->getBinary().end(),
            //           buff);
            //_file.write(buff, req->getBinary().size());
            // delete[] buff; // 동적 메모리 해제

            //_file.close();
			std::vector<char> msg = res.getResponseMessage();
            write(new_socket, &msg[0], msg.size());
            //write(new_socket, hello, std::string(hello).size());
            //std::cout << "========= hello ========= " << std::endl << hello << std::endl;
            std::cout << "------------------Hello message sent-------------------"
                    << std::endl;
            close(new_socket);
			clnts.erase(new_socket);
			(void) hello;
			delete req;
        }
        return 0;
    }

    // int  main(int argc, char *argv[]) {
    //	(void) argc;
    //     Config con(static_cast<std::string>(argv[1]));

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

    // int main() {
    //	ServerFiles	files;

    //	std::vector<char> _file = files.getFile("./html/test2/abc");
    //	write(1, reinterpret_cast<const char *>(&_file[0]), _file.size());
    //	//std::ofstream copiedFile("./img.png", std::ios::binary);
    //	//if (!copiedFile)
    //	//	return (1);

    //	//copiedFile.write(reinterpret_cast<const char *>(&_file[0]),
    //_file.size());
    //	//copiedFile.close();
    //	return 0;
    //}