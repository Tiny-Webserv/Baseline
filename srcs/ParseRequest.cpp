#include "Request.hpp"
#include "get_next_line.h"
#include "utils.hpp"
#include "StateCode.hpp"
#include <algorithm>
#include <map>

Request *ParseRequest(int fd, std::map<int, Request *> &clients,
					  ServerBlock &server) {
	char *line = get_next_line(fd);
	std::stringstream ss;

	while (line) {
		ss << line;
		free(line);
		line = get_next_line(fd);
	}
	std::map<int, Request *>::iterator clientsIterator = clients.find(fd);
	if (clientsIterator == clients.end()) {
		Request *request = new Request(server, ss);
		clients.insert(std::pair<int, Request *>(fd, request));
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
