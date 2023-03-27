#include <iostream>
#include <string>
#include <sstream>

class ServerBlock {
};

class Request {
private :
	int _method;
	std::string _target;
	std::string _contentType;
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
};