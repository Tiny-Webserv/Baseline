#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>

class Request;

class Response {
private:
	std::string	_responseMessage;
public:
	Response(Request	&request);
	~Response();

	void	SetResponseMessage(std::string	responseMessage);
	std::string	GetResponseMessage();

	class	NotFound : public std::exception {
	public :
		const char *what() const throw();
	};

	class	Forbidden : public std::exception {
	public :
		const char *what() const throw();
	};
};

#endif