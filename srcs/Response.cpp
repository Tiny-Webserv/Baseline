#include "Response.hpp"
#include "Request.hpp"
#include "ServerBlock.hpp"

Response::Response(Request	&request) : _request(request) {
	if (request.GetMethod() == "GET")
		; //get method 처리;
	else if (request.GetMethod() == "DELETE")
		; //delete method;
    else
		; // post method;

}

Response::~Response() {
}

void Response::SetResponseMessage(std::string responseMessage){
    _responseMessage = responseMessage;}

std::string Response::GetResponseMessage() {
    return _responseMessage;
}

bool Response::isAllowed(std::string	method) {
	std::vector<std::string>	*limit_except;

	for (size_t i = 0; i < _request.GetServer().GetLocation().size(); i++) {
		if (_request.GetTarget().compare(_request.GetServer().GetLocation()[i].GetLocationTarget())) {
			limit_except = &_request.GetServer().GetLocation()[i].GetLimitExcept();
			break ;
		}
	}
	// 리미스 익셉 찾아놨으니 알아서 처리할것
}

void	Response::getMethod() {
	if (/* condition */)
	{
		/* code */
	}

}


