#include "Request.hpp"
#include "get_next_line.h"
#include "utils.hpp"
#include "StateCode.hpp"
#include "ServerBlock.hpp"
#include <algorithm>
#include <map>

ServerBlock	*FindServer(std::vector<ServerBlock> &servers, Request	*request) {
	int	idx = -1;
    std::vector<std::string>::iterator iter;


    // request 포인터가 NULL인 경우 예외 처리
    if (request == NULL) {
        //throw std::runtime_error("Error : Invalid request pointer");
        return NULL;
    }
	// port가 같은 객체가 있는지 확인
    for (size_t i = 0; i < servers.size(); i++) {
        if (servers[i].GetPort() == request->GetHostPort()) {
            if (idx == -1)
                idx = static_cast<int>(i);
            // std::find 대신 for 루프와 문자열 비교를 통해 조건 검사
            for (size_t j = 0; j < servers[i].GetServerName().size(); j++) {
                if (servers[i].GetServerName()[j] == request->GetHostName()) {
					return &servers[i];
                }
            }
        }
    }
    if (idx == -1) {
        //throw std::runtime_error("Error : No matching server found");
        return NULL;
    }
    return &servers[idx];
}

Request *ParseRequest(int fd, std::map<int, Request *> &clients,
					  std::vector<ServerBlock> &servers) {
	std::stringstream ss;

	std::map<int, Request *>::iterator clientsIterator = clients.find(fd);
	if (clientsIterator == clients.end()) {
		char *line = get_next_line(fd);
		while (line) {
			ss << line;
			std::cout << line;
			std::string tmp(line);
			free(line);
			if (tmp == CRLF)
				break;
			line = get_next_line(fd);
		}
		std::cout << ss.str() << std::endl;
		Request *request = new Request(fd, ss);
		clients.insert(std::pair<int, Request *>(fd, request));
		request->SetServer(FindServer(servers, request));
        //request->readBody(fd);
    } else {

                Request *request = clients[fd];
		std::vector<std::string> body = Split2(ss.str(), CRLF);
		try {
			request->readBody(fd);
		} catch (const BodySizeError &e) {
			request->SetErrorCode(PayloadTooLarge);
			request->SetErrorMessages(e.what());
		} catch (const std::exception &e) {
			request->SetErrorCode(BadRequest);
			request->SetErrorMessages(e.what());
		} catch (const std::runtime_error& e) {
            request->SetErrorCode(NotImplemented);
            request->SetErrorMessages(e.what());
        }
	}
	return clients[fd];
}
