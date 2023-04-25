#include "Response.hpp"
#include "Request.hpp"
#include "ServerBlock.hpp"

Response::Response(Request &request) : _request(request) {
    if (request.GetMethod() == "GET")
		; // autoindex 처리
    else if (request.GetMethod() == "DELETE")
        ; // delete method;
    else
        ; // post method;
}

Response::~Response() {}

void Response::SetResponseMessage(std::string responseMessage) {
    _responseMessage = responseMessage;
}

std::string Response::GetResponseMessage() { return _responseMessage; }


LocationBlock	&Response::getLocationBlock() {
	std::string	target = _request.GetTarget();
	for (size_t i = 0; i < _request.GetServer().GetLocation().size(); i++) {
		std::string	locationTarget = _request.GetServer().GetLocation()[i].GetLocationTarget();
        if (!locationTarget.compare(0, locationTarget.size(), target))
			return _request.GetServer().GetLocation()[i];
    }
	throw NotFound();
}

bool Response::isCGI() {
        LocationBlock block = getLocationBlock();
		if(block.GetLocationTarget().find(".cgi"))
        	return true;
        return false;
}

bool Response::isAutoIndex() {
        LocationBlock block = getLocationBlock();
        if (block.GetAutoIndex())
			return true;
        return false;
}

bool Response::isAllowed(std::string method) {
	std::vector<std::string>	limit_except = getLocationBlock().GetLimitExcept();

	if (limit_except.size() == 0)
		return true ;
	else if (find(limit_except.begin(), limit_except.end(), method) == limit_except.end())
		return false ;
	else
		return true ;
}

void Response::getMethod() {
    if (!isAllowed("GET"))
		throw Forbidden();

}
