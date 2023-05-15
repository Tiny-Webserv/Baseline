#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <vector>
#include "ServerFiles.hpp"
#include <sys/types.h>
#include <sys/stat.h>

class Request;

class LocationBlock;

class Response {
private:
	std::vector<char>  _responseMessage;
    std::vector<char> _statusHeaderMessage;
	std::vector<char> _bodyMessage;
	Request	*_request;
	static ServerFiles _serverFiles;
	std::string	_contentType;
	bool _isRedirection;
	std::string _redirectLocation;
	LocationBlock	&getLocationBlock();
public:
	Response(Request	*request);
	Response(Request	*request, std::vector<struct kevent>& ChangeList);
	Response(Response &response); //
	~Response();

	// getter
	std::vector<char> getResponseMessage();
    std::vector<char> getStatusHeaderMessage();
	std::vector<char> getBodyMessage();
	// std::vector<char> getResponseMessage();

	// setter
	void SetResponseMessage(std::string	responseMessage);
	void setResponseMessage(std::vector<char> &getResponseMessage);
    void setStatusHeaderMessage(std::vector<char> &getStatusHeaderMessage);
	void setBodyMessage(std::vector<char> &getBodyMessage);

	// checker
	bool	isAllowed(std::string	method);
	bool	isAutoIndex();
    bool 	isCGI();
	bool	isDirectory(const char *directory);
	bool	isRedirect();

	void	getMethod();
	void	postMethod();
	void	deleteMethod();

	void	generateStatusLine();
	void	generateHeader();
	void	joinResponseMessage();
	void	generateAutoindex(const std::string& directory);
	void	generateErrorBody();
	void	generateDefaultErrorPage();

	std::string	fetchFilePath();
	// auto index 처리 함수
	// serverBlock 참고해서 특정 파일이 있는지 확인
	// 특정 파일 읽어와 body에 실어주기
	// cgi 처리
	// http 통신 규약에 맞춘 status line, header, body 처리해서 ss에 넣어주기

	// public:
    // Client(Server* server) : _server(server) { }
    // // Client 클래스의 다른 멤버 함수들

	// 1. responseMsg에다가 다 담기
	// 2. 생성자에서 404 등 에러 나오는 거 처리해주기
};

#endif

// 1. 에러 페이지 만드는 거 함수 따로 만들기