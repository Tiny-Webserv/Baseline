#include "Request.hpp"
#include "utils.hpp"
#include "StateCode.hpp"
#include <cstdlib>
#include <algorithm>
// #include "ServerBlock.hpp" // TODO 이후에 해당 파일 생겼을 때 주석 해제하기

Request::Request(ServerBlock &server) : _chunked(false), _server(server) {}

Request::Request(ServerBlock &server, std::stringstream &stream)
    : _contentLength(-1), _errorCode(OK), _chunked(false), _server(server) {
    std::vector<std::string> splited = Split2(stream.str(), CRLF);

    try {
        setStartLine(splited[0]);
        setHeader(splited[1]);
		std::vector<std::string>::iterator	iter = splited.begin() + 3;
        SetBody(iter);
    } catch (const Request::BodySizeError	&e) {
		SetErrorCode(PayloadTooLarge);
		SetErrorMessages(e.what());
	} catch (const Request::HTTPVersionError	&e) {
		SetErrorCode(HTTPVersionNotSupported);
		SetErrorMessages(e.what());
	} catch (const std::exception &e) {
        SetErrorCode(BadRequest);
        SetErrorMessages(e.what());
    }
}

Request::~Request() {}

void Request::SetMethod(Method method) { _method = method; }

int Request::GetMethod() { return _method; }

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

ServerBlock &Request::GetServer() { return _server; }

void Request::SetErrorCode(int errorCode) { _errorCode = errorCode; }

int Request::GetErrorCode() { return _errorCode; }

void Request::SetErrorMessages(std::string errorMessages) {
    _errorMessages = errorMessages;
}

std::string Request::GetErrorMessages() { return _errorMessages; }

const char *Request::HTTPVersionError::what() const throw() {
    return "HTTP Version error : The version must be HTTP 1.1";
}

const char *Request::MethodError::what() const throw() {
    return "Method error : Bad method request";
}

const char *Request::BodySizeError::what() const throw() {
    return "Size error : Request Entity Too Large";
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
    std::vector<std::string> data = Split2(startLine, std::string(" "));
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
    std::vector<std::string> splited = Split2(header, " ");
    std::vector<std::string>::iterator iter;

    iter = find(splited.begin(), splited.end(), "Content-Type:");
    if (iter != splited.end() && iter + 1 != splited.end())
        _contentType = *(iter + 1);
    iter = find(splited.begin(), splited.end(), "Transfer-Encoding:");
    if (iter != splited.end() && iter + 1 != splited.end() &&
        !(iter + 1)->compare("chunked"))
        _chunked = true;
	iter = find(splited.begin(), splited.end(), "Content-Length:");
    if (iter != splited.end() && iter + 1 != splited.end())
        _contentLength = atoi((iter + 1)->c_str());
}

void Request::SetBody(std::vector<std::string>::iterator	iter) {
    unsigned int x;

    if (GetChunked()) {
        std::stringstream ss;
        ss << std::hex << *iter;
        ss >> x;
        if ((iter + 1)->length() > x) {
			GetStream() << (iter + 1)->substr(0, x);
            throw BodySizeError();
		}
		else if (x == 0)
			SetChunked(false);
        else
            GetStream() << *(iter + 1);
    } else {
		if (_contentLength != -1 && static_cast<int>(iter->length()) > _contentLength) {
			GetStream() << iter->substr(0, _contentLength);
                        throw BodySizeError();
        }
        GetStream() << *iter;
	}
}
