#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// test를 위해 crlf를 아래와 같이 정의했습니다. 이후에 "\r\n"으로 수정 바랍니다.
#ifndef CRLF
#define CRLF "\\r\n"
#endif

enum Method { GET = 1, POST = 2, DELETE = 3 };
class ServerBlock {};

class Request {
  private:
	Method _method;
	std::string _target;
	std::string _body;
	std::string _contentType;
	int	_contentLength;
	std::string _errorMessages;
	int _errorCode;
	bool _chunked;
	std::stringstream _stream;
	ServerBlock &_server;

  public:
	Request(ServerBlock &server);
	Request(ServerBlock &server, std::stringstream &stream);
	Request(const Request &request);
	Request &operator=(const Request &request);
	virtual ~Request();

	// Setter
	void SetTarget(std::string target);
	void SetMethod(Method method);
	void SetContentType(std::string contentType);
	void SetChunked(bool chunked);
	void SetStream(std::stringstream &stream);
	void SetErrorCode(int _errorCode);
	void SetErrorMessages(std::string _errorMessages);

	// Getter
	int GetMethod();
	std::string GetTarget();
	std::string GetContentType();
	bool GetChunked();
	std::stringstream &GetStream();
	ServerBlock &GetServer();
	int GetErrorCode();
	std::string GetErrorMessages();

	void setStartLine(std::string startLine);
	void setHeader(std::string header);
	void SetBody(std::vector<std::string>::iterator iter);

	class HTTPVersionError : public std::exception {
	  public:
		const char *what() const throw();
	};

	class MethodError : public std::exception {
	  public:
		const char *what() const throw();
	};

	class BodySizeError : public std::exception {
	  public:
		const char *what() const throw();
	};

};

Request *ParseRequest(int fd, std::map<int, Request *> &clients,
					  ServerBlock &server);

#endif