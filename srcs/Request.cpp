#include "Request.hpp"
#include "ServerBlock.hpp"
#include "StateCode.hpp"
#include "get_next_line.h"
#include "utils.hpp"
#include <algorithm>
#include <cstdlib>
#include <sys/socket.h>

Request::Request() : _chunked(false) {}

Request::Request(int fd, std::stringstream &stream)
    : _contentLength(-1), _errorMessages("OK"), _errorCode(OK), _chunked(false), _hostPort(80), _isEnd(false) {

    try {
        std::string buff;
        std::getline(stream, buff, '\n');
        setStartLine(buff);
        setHeader(stream.str());
        readBody(fd);
        splitHost();
		if (_contentType == "multipart/form-data")
			parseFormData();

    } catch (const StateCode &e) {
        SetErrorCode(_errorCode);
        SetErrorMessages(e.what());
    }
}

Request::Request(std::vector<char>	&formData, std::string	delimeter){
	std::string	headerStr(formData.begin(), formData.end());
	std::string doubleCRLF = CRLF;
	doubleCRLF.append(CRLF);
	size_t delimeterPos = headerStr.find(delimeter);
	if (delimeterPos == std::string::npos)
		return ;
    setHeader(headerStr.substr(0, delimeterPos));
	if (headerStr.find(doubleCRLF) == std::string::npos)
		return ;
	formData.erase(formData.begin(), formData.begin() + headerStr.find(doubleCRLF) + doubleCRLF.size());
    std::copy(formData.begin(), find(formData.begin(), formData.end(), '\r'),
              std::back_inserter(_binary));
	formData.erase(formData.begin(), find(formData.begin(), formData.end(), '\r') + 2);
}

Request::~Request() {
	if (_contentType == "multipart/form-data") {
		for (size_t i = 0; i < _formData.size(); i++)
			delete _formData[i];
    }
}

void Request::SetMethod(std::string method) { _method = method; }

std::string Request::GetMethod() { return _method; }

void Request::SetTarget(std::string target) { _target = target; }

std::string Request::GetTarget() { return _target; }

void Request::SetContentType(std::string contentType) {
    _contentType = contentType;
}

std::string Request::GetContentType() { return _contentType; }

void Request::SetChunked(bool chunked) { _chunked = chunked; }

bool Request::GetChunked() { return _chunked; }

void Request::SetStream(std::stringstream &stream) {
    _stream.clear();
    _stream << stream.str();
}

std::stringstream &Request::GetStream() { return _stream; }

ServerBlock &Request::GetServer() { return *_server; }

void Request::SetErrorCode(int errorCode) { _errorCode = errorCode; }

int Request::GetErrorCode() { return _errorCode; }

void Request::SetErrorMessages(std::string errorMessages) {
    _errorMessages = errorMessages;
}

std::string Request::GetErrorMessages() { return _errorMessages; }

void Request::SetServer(ServerBlock *serverBlock) { _server = serverBlock; }
bool Request::GetIsEnd() { return _isEnd; }

void Request::SetIsEnd(bool isEnd) { _isEnd = isEnd; }

std::vector<Request *>	Request::getFormData() {
	return _formData;
}

Request &Request::operator=(const Request &request) {
    _method = request._method;
    _target = request._target;
    _contentType = request._contentType;
    _chunked = request._chunked;
    _stream.clear();
    _stream << request._stream.str();
    return *this;
}

Request::Request(const Request &request)
    : _chunked(false), _server(request._server) {
    if (this == &request)
        return;
    *this = request;
}

void Request::setStartLine(std::string startLine) {
    std::vector<std::string> data = Split(startLine, std::string(" "));
    if (!data[0].compare("GET") || !data[0].compare("POST") ||
        !data[0].compare("DELETE"))
        _method = data[0];
    else
        throw MethodError();
    _target = Split(data[1], std::string(":"))[0]; //[0]은 vector 0번째 인덱스 접근을 위함
    std::cout << data[2] << std::endl;
    // std::cout << std::isstring(data[2]) << std::endl;
    if (data[2].compare("HTTP/1.1\r"))
        throw HTTPVersionError();
}

void Request::setHeader(std::string header) {
    std::vector<std::string> splited = Split(header, " \r\n;");
    std::vector<std::string>::iterator iter;
    for (std::vector<std::string>::iterator i = splited.begin();
         i != splited.end(); i++) {
    }

    iter = find(splited.begin(), splited.end(), "Content-Type:");
    if (iter != splited.end() && iter + 1 != splited.end()) {
        _contentType = *(iter + 1);
		if (_contentType == "multipart/form-data")
			_boundary = (*(iter + 2)).substr(9);
	}
    iter = find(splited.begin(), splited.end(), "Host:");
    if (iter != splited.end() && iter + 1 != splited.end())
        _hostName = *(iter + 1); // example.com:8080
    iter = find(splited.begin(), splited.end(), "Transfer-Encoding:");
    if (iter != splited.end() && iter + 1 != splited.end() &&
        !(iter + 1)->compare("chunked"))
        _chunked = true;
    iter = find(splited.begin(), splited.end(), "Content-Length:");
    if (iter != splited.end() && iter + 1 != splited.end())
        _contentLength = atoi((iter + 1)->c_str());
    // std::cout << _contentLength << std::endl;
	iter = find(splited.begin(), splited.end(), "Content-Disposition:");
	if (iter != splited.end()) {
		for (int i = 0; iter + i < splited.end(); i++) {
			if ((iter + i)->find("filename=") != std::string::npos) {
				_fileName = (iter + i)->substr(std::string("filename=").size());
				_fileName.erase(0, 1);
				_fileName.erase(_fileName.size() - 1, 1);
				//break ;
            }
		}
	}
}

void Request::splitHost() {
    std::vector<std::string> splited = Split(_hostName, ":");
    _hostName = splited[0]; // name
    if (splited.size() > 1)
        _hostPort = atoi(splited[1].c_str());
}

std::string Request::GetHostName() { return this->_hostName; }

int Request::GetHostPort() { return this->_hostPort; }

void Request::readBody(int fd) {
    unsigned int x = 0;
    std::stringstream ss;
    std::string line;
    int valRead;
    char buffer[1024];

    if (_chunked) {
        line = get_next_line(fd);
        line = line.substr(0, line.size() - CRLF_SIZE);
        ss << std::hex << line;
        ss >> x;
        if (x == 0) {
            _isEnd = true;
            return;
        }
        _contentLength = static_cast<int>(x);
    }
    int j = 0;
    while ((valRead = recv(fd, buffer, 1024, 0)) > 0) {
        for (int i = 0; i < valRead; i++)
            _binary.push_back(buffer[i]);
        std::cout << j++ << std::endl;
    }
    // CRLF가 2번인지 확인해서 다 읽었는지 아닌지 확인
    if (!_chunked && _binary.size()) {
        size_t end = _binary.size() - 1;
        if (end > 3 && _binary[end] == '\n' && _binary[end - 1] == '\r' &&
            _binary[end - 2] == '\n' && _binary[end - 3] == '\r')
            _isEnd = true;
        else if (valRead != -1)
            throw BodySizeError(); // TODO 얼마나 읽었는지 체크해서 다시 예외
                                   // 처리
    } else {
        // if (valRead != CRLF_SIZE * 2 || line.compare(0, valRead, crlf) != 0)
        //	throw BodySizeError(); // TODO 얼마나 읽었는지 체크해서 다시
        //예외 처리
        _isEnd = false;
    }
}

void Request::parseFormData() {
	size_t i = 0;
	while (_boundary[i] == '-') i++;
	_boundary.erase(0, i);
	i = 0;
	while (1) {
		i = 0;
		while (i < _binary.size() && _binary[i] == '-') i++;
		_binary.erase(_binary.begin(), _binary.begin() + i);
		i = 0;
		while (i < _binary.size() && _binary[i] == _boundary[i]) i++;
		_binary.erase(_binary.begin(), _binary.begin() + i) + 2;
		if (_binary.size() == 0 || _binary[0] == '-')
			break ;
		_formData.push_back(new Request(_binary, _boundary));
	}
}
