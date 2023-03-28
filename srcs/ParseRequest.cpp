#include "Request.hpp"
#include <algorithm>
#include <map>

// 1. 파일 디스크립터로부터 리드해오기
// 2. startline, header, body 분리하기
// 3. 해당 값에 따라 변수에 저장하기
// 4. 예외처리 들어가기

Request	*ParseRequest(int	fd, std::map<int, Request> clients, ServerBlock	&server) {
	std::map<int, Request>::iterator clientsIterator = clients.find(fd);
    if (clientsIterator == clients.end())
	{
        // std::pair<std::map<int, Request>::iterator, bool> pairValue;
        // pairValue = clients.insert(std::pair(fd, Request(server)));
        // clientsIterator = pairValue.first();

        Request	request(server);
        clients.insert(std::pair<int, Request>(fd, request));
		clients[fd] = request;
    }
        //   std::stringstream &ss = clients->GetStream();

        //Request	request(server);
		clients.insert(std::make_pair(fd, request));


    auto ret = clients.insert(std::make_pair(fd, request));
    if (!ret.second) {
                ret.first->second = request;
    }
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