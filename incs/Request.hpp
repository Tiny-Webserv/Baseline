#include <iostream>
#include <string>
#include <sstream>

enum Method { GET = 1, POST = 2, DELETE = 3 };

class ServerBlock {
};

/* TODO 목표 : 파싱
        - 리퀘스트문 예시 4-5개 만들기
			- get, delete, post
			- chunked
			- errors?
					- http version error
					- method error
						- ex. get, delete, post 아닌 것들
*/

class Request {
private :
	int _method;
	std::string _target;
	std::string _contentType;
	// FIX : chunked 변수 필요한지 고민해봐야함
	bool _chunked;
	std::stringstream _stream;
	ServerBlock &_server;
public :
	Request(ServerBlock &server);
	virtual ~Request();

	// Setter
	void	SetMethod(int	method);
	void	SetTarget(std::string	target);
	void	SetContentType(std::string contentType);
	void	SetChunked(bool	chunked);
	void	SetStream(std::string	stream);

	// Getter
	int					GetMethod();
	std::string			GetTarget();
	std::string			GetContentType();
	bool				GetChunked();
	std::stringstream	&GetStream();
	ServerBlock			&GetServer();

	class HTTPVersionError : public std::exception {
	public:
    const char * what() const;
	};
};