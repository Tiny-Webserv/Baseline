#include "Request.hpp"
#include "utils.hpp"
#include <algorithm>
//#include "ServerBlock.hpp" // TODO 이후에 해당 파일 생겼을 때 주석 해제하기

Request::Request(ServerBlock &server) : _chunked(false), _server(server) {

}

// TODO 생성자에서 chunked인지 아닌지 처리하므로 굳이 초기화 리스트에서 초기화할 필요 없음. 수정할 것
Request::Request(ServerBlock &server, std::stringstream	&stream) : _server(server) {
// TODO start line, header, body 파싱하기
	int startPos = 0;
	int crlfPos;
	std::vector<std::string> splited = Split(stream.str(), "\r\n");
	setStartLine(splited[0]);
	setHeader(splited[1]);
	for (std::vector<std::string>::iterator	iter = splited.begin() + 2; iter != splited.end(); iter++)
		_stream << *iter << "\n";
}

Request::~Request() {
}

void  Request::SetMethod(Method  method) {
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

void Request::setStartLine(std::string startLine){
	   	std::vector<std::string> data = Split(startLine, std::string(" "));

		if (!data[0].compare("GET"))
			_method = GET;
		else if (!data[0].compare("POST"))
			_method = POST;
		else if (!data[0].compare("DELETE"))
			_method = DELETE;
		else
			throw MethodError();
		_target = data[1];

		if (data[2].compare("HTTP/1.1"))
			throw HTTPVersionError();

}

void Request::setHeader(std::string header) {
	std::cout << "header : " << header << std::endl;
	std::vector<std::string>	splited = Split(header, ": ");
	std::vector<std::string>::iterator	iter;

	iter = find(splited.begin(), splited.end(), "Content-Type");
	if (iter != splited.end() && iter + 1 != splited.end())
		_contentType = *(iter + 1);
	iter = find(splited.begin(), splited.end(), "Transfer-Encoding");
	if (iter != splited.end() && iter + 1 != splited.end() && !(iter + 1)->compare("chunked"))
		_chunked = true;
}
