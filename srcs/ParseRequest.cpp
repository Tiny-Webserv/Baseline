#include "Request.hpp"
#include "get_next_line.h"
#include <algorithm>
#include <map>

// TODO 1. 파일 디스크립터로부터 리드해오기
// TODO 2. startline, header, body 분리하기 -> request 객체 안에서
// TODO 3. 해당 값에 따라 변수에 저장하기
// TODO 4. 예외처리 들어가기

Request	*ParseRequest(int	fd, std::map<int, Request *> &clients, ServerBlock	&server) {
	char *line = get_next_line(fd);
	std::stringstream	ss;
	while (line) {
		ss << line;
		free(line);
		line = get_next_line(fd);
	}
	std::map<int, Request *>::iterator clientsIterator = clients.find(fd);
    if (clientsIterator == clients.end()) {
        Request	*request = new Request(server, ss);
        clients.insert(std::pair<int, Request*>(fd, request));
    }
	else {
		Request	*request = clients[fd];
		request->GetStream() << ss.str();
	}
	// test 코드 입니다.
	Request	*request = clients[fd];
	std::cout << request->GetStream().str();
	//
	return request;
}

//std::map<int, Request> my_map;
//int key = 123;
//Request value;

//// std::make_pair 함수를 사용하여 키와 값을 쌍으로 만든 후 insert 함수에 전달
//my_map.insert(std::make_pair(key, value));

/*	iterator clients_iter = map.find(fd);
		// fd에 해당하는 request 존재 유뮤 확인
        if (clients_iter == map.end())
		{
			std::pair<std::map<int, Request>::iterator, bool> ret;
			ret = map.insert(make_pair(fd, Request(Server)));
			clients_iter = ret.first();
		}
		Request &request = *clients_iter;
        std::stringstream &ss = request.GetStream();
        char* line = "";

        while (line != NULL)
        {
                line = gnl(fd);
                ss << line;
				free(line);
        }


    while(ss != NULL)
    {
        char c;
        std::string temp;
        temp = NULL;

         while (ss.get(c))
         {
            temp.append(c);
            if(c == ' ')
            {
                request.setMethod(temp);
                break;
            }
            else if(c == ':')
            {
                    request._method =
            }
            else if
        }
    }
    */
// ==========================
// ==========================
    // try
    // {
    // if (request._method == GET)
    //{
    // }
    // else if (request._method == POST)
    //{
    //}else if (request._method == DELETE)
    //{
    //}
    // else
    //{
        //throw Request::MethodError();
//      }
//}  catch (const std::exception& e) {
// std::cerr << "Error: " << e.what() << std::endl;
//}
//}