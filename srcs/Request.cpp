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
    // std::vector<std::string> splited = Split2(stream.str(), CRLF);

    try {
        int i = 0;
        std::cout << i++ << std::endl;
        std::string buff;
        std::getline(stream, buff, '\n');
        std::cout << buff << std::endl;
        setStartLine(buff);
        std::cout << i++ << std::endl;
        setHeader(stream.str());
        std::cout << i++ << std::endl;
        // std::vector<std::string>::iterator iter = splited.begin() + 3;
        // SetBody(iter);
        readBody(fd);
        std::cout << i++ << std::endl;
        splitHost();
    } catch (const BodySizeError &e) {
        std::cout << "payloadTooLarge" << std::endl;
        SetErrorCode(PayloadTooLarge);
        SetErrorMessages(e.what());
    } catch (const HTTPVersionError &e) {
        SetErrorCode(HTTPVersionNotSupported);
        SetErrorMessages(e.what());
    } catch (const std::exception &e) {
        SetErrorCode(BadRequest);
        SetErrorMessages(e.what());
    }
}

Request::~Request() { std::cout << "!!!!!!!!!!!!!!!!die" << std::endl; }

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
    std::cout << "startLine : " << startLine << std::endl;
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
    std::cout << "this is header " << header << std::endl;
    std::vector<std::string> splited = Split(header, " \r\n");
    std::vector<std::string>::iterator iter;
    for (std::vector<std::string>::iterator i = splited.begin();
         i != splited.end(); i++) {
        std::cout << "what is splited ? " << *i << std::endl;
        /* code */
    }

    iter = find(splited.begin(), splited.end(), "Content-Type:");
    if (iter != splited.end() && iter + 1 != splited.end())
        _contentType = *(iter + 1);
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
}

// void Request::SetBody(std::vector<std::string>::iterator iter) {
//	unsigned int x;

//	if (GetChunked()) {
//		std::stringstream ss;
//		ss << std::hex << *iter;
//		ss >> x;
//		if ((iter + 1)->length() > x) {
//			GetStream() << (iter + 1)->substr(0, x);
//			throw BodySizeError();
//		} else if (x == 0)
//			SetChunked(false);
//		else
//			GetStream() << *(iter + 1);
//	} else {
//		if (_contentLength != -1 &&
//			static_cast<int>(iter->length()) > _contentLength) {
//			GetStream() << iter->substr(0, _contentLength);
//			throw BodySizeError();
//		}
//		GetStream() << *iter;
//	}
//}

void Request::splitHost() {
    std::vector<std::string> splited = Split(_hostName, ":");
    _hostName = splited[0]; // name
    if (splited.size() > 1)
        _hostPort = atoi(splited[1].c_str());
    std::cout << "hostName : " << _hostName << std::endl;
    std::cout << "hostPort :" << _hostPort << std::endl;
}

std::string Request::GetHostName() { return this->_hostName; }

int Request::GetHostPort() { return this->_hostPort; }

void Request::readBody(int fd) {
    unsigned int x = 0;
    std::stringstream ss;
    std::string line;
    int valRead;
    char buffer[1024];
    // int		bufferSize = 1024;
    //  char	crlf[CRLF_SIZE * 2 + 1];

    // if (_contentType == "img/png")
    //	_stream << std::ios::binary;
    std::cout << " 1: ==> " << std::endl;

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
    std::cout << " 2: ==> " << std::endl;

    // buffer = (char *)malloc(_contentLength + 1);
    // if (buffer == NULL)
    //	throw std::runtime_error("malloc failed");
    // line = CRLF;
    // line.append(CRLF);
    // memset(buffer, 0, _contentLength + 1);
    // valRead = recv(fd, buffer, _contentLength, 0);
    // buffer[valRead] = 0;
    // for (int i = 0; i < valRead; i++)
    //	_binary.push_back(buffer[i]);
    // if (valRead == -1)
    //	_isEnd = false ;
    // memset(crlf, 0, CRLF_SIZE * 2);
    // valRead = read(fd, crlf, CRLF_SIZE * 2);
    int j = 0;
    while ((valRead = recv(fd, buffer, 1024, 0)) > 0) {
        for (int i = 0; i < valRead; i++)
            _binary.push_back(buffer[i]);
        std::cout << j++ << std::endl;
    }
    std::cout << " 3: ==> " << std::endl;

    std::cout << "valRead : " << valRead << std::endl;
    std::cout << "errno : " << errno << std::endl;
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
    // free(buffer);
    if (_contentType != "image/png" && _contentType != "img/png") {
        for (size_t i = 0; i < _binary.size(); i++)
            _stream << _binary[i];
        _binary.clear();
    }
}

// int bytes_received;

// while ((bytes_received = recv(client_socktet, buffer, MAX_BUFFER_SIZE, 0)) >
// 0) { 	for (int i = 0; i < bytes_received; i++)
//	{
//		_file.push_back(buffer[i]);
//	}
// }
// for (std::vector<char>::size_type i = 0; i < _file.size(); i++)
//{
//	char buff[2];
//	buff[0] = _file[i];
//	buff[1] = 0;
//	image_file.write(buff, 1);
// }
// image_file.close();