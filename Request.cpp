#include "Request.hpp"
//#include "ServerBlock.hpp"

Request::Request(ServerBlock &server) : _server(server), _chunked(false) {

}


Request::~Request() {
}

void  Request::SetMethod(int  method) {
	_method = method;
}

int  Request::GetMethod() {
	return _method;
}

void  Request::SetTarget(std::string  target) {
	_target = target;
}

std::string  Request::GetTarget() {
	return _target;
}

void  Request::SetContentType(std::string  contentType) {
	_contentType = contentType;
}

std::string  Request::GetContentType() {
	return _contentType;
}

void  Request::SetStream(std::string  stream) {
	_stream.clear();
	_stream << stream;
}

std::stringstream  &Request::GetStream() {
	return _stream;
}

ServerBlock	&Request::GetServer() {
	return _server;
}