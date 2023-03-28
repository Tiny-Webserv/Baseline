#include "Request.hpp"
//#include "ServerBlock.hpp" // TODO 이후에 해당 파일 생겼을 때 주석 해제하기


Request::Request(ServerBlock &server) : _chunked(false), _server(server) {

}

// TODO 생성자에서 chunked인지 아닌지 처리하므로 굳이 초기화 리스트에서 초기화할 필요 없음. 수정할 것
Request::Request(ServerBlock &server, std::stringstream	&stream) : _chunked(false), _server(server) {
// TODO start line, header, body 파싱하기
	_stream << stream.str();
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

void  Request::SetStream(std::stringstream  &stream) {
	_stream.clear();
	_stream << stream.str();
}

std::stringstream  &Request::GetStream() {
	return _stream;
}

ServerBlock	&Request::GetServer() {
	return _server;
}

const char	*Request::HTTPVersionError::what() const throw () {
	return "HTTP Version error : The version must be HTTP 1.1";
}

const char	*Request::MethodError::what() const throw () {
	return "Method error : Bad method request";
}

Request	&Request::operator=(const Request	&request) {
	_method = request._method;
	_target = request._target;
	_contentType = request._contentType;
	_chunked = request._chunked;
	_stream.clear();
	_stream << request._stream.str();
	return *this;
}

Request::Request(const Request	&request) : _chunked(false), _server(request._server) {
	if (this == &request)
		return ;
	*this = request;
}