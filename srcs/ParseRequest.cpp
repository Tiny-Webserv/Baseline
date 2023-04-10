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

	for (int i = 0; i < servers.size(); i++) {
		if (servers[i].GetPort() == request->GetHostPort()) {
			if (idx == -1)
				idx = i;
			iter = std::find(servers[i].GetServerName().begin(), servers[i].GetServerName().end(), request->GetHostName());
			if (iter != servers[i].GetServerName().end())
				return &servers[i];
		}
	}
	return &servers[idx];
}

Request *ParseRequest(int fd, std::map<int, Request *> &clients,
					  std::vector<ServerBlock> &servers) {
	char *line = get_next_line(fd);
	std::stringstream ss;

	while (line) {
		ss << line;
		free(line);
		line = get_next_line(fd);
	}
	std::map<int, Request *>::iterator clientsIterator = clients.find(fd);
	if (clientsIterator == clients.end()) {
		Request *request = new Request(ss);
		clients.insert(std::pair<int, Request *>(fd, request));
		request->SetServer(FindServer(servers, request));
	} else {
		Request *request = clients[fd];
		std::vector<std::string> body = Split2(ss.str(), CRLF);
		try {
			request->SetBody(body.begin());
		} catch (const Request::BodySizeError &e) {
			request->SetErrorCode(PayloadTooLarge);
			request->SetErrorMessages(e.what());
		} catch (const std::exception &e) {
			request->SetErrorCode(BadRequest);
			request->SetErrorMessages(e.what());
		}
	}
	return clients[fd];
}
