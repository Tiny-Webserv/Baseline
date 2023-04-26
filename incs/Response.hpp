#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>

class Request;

class LocationBlock;

class Response {
private:
	std::stringstream	_responseMessage;
	Request	&_request;
public:
	Response(Request	&request);
	~Response();

	void	SetResponseMessage(std::string	responseMessage);
	std::string	GetResponseMessage();

	LocationBlock	&getLocationBlock();

	void	getMethod();

	bool	isAllowed(std::string	method);
	bool	isAutoIndex();
    bool 	isCGI();
        // auto index 처리 함수
	// serverBlock 참고해서 특정 파일이 있는지 확인
	// 특정 파일 읽어와 body에 실어주기
	// cgi 처리
	// http 통신 규약에 맞춘 status line, header, body 처리해서 ss에 넣어주기


};

#endif