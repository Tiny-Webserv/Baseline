#include "Request.hpp"
//#include "ServerBlock.hpp" // TODO 이후에 해당 파일 생겼을 때 주석 해제하기


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

const char	*Request::HTTPVersionError::what() const {
	return "HTTP Version error : The version must be HTTP 1.1";
}

const char	*Request::MethodError::what() const {
	return "Method error : Bad method request";
}

Request	&Request::operator=(const Request	&request) {
	_method = request._method;
	_target = request._target;
	_contentType = request._contentType;
	_chunked = request._chunked;
	_stream.clear();
	_stream << request._stream.str();
	_server = request._server;
	return *this;
}

Request::Request(const Request	&request) {
	if (this == &request)
		return ;
	*this = request;
}

//bool	Request::operator > (const Request	&compare) {}
//bool	Request::operator < (const Request	&compare);
//bool	Request::operator >= (const Request	&compare);
//bool	Request::operator <= (const Request	&compare);
//bool	Request::operator == (const Request	&compare);
//bool	Request::operator != (const Request	&compare);