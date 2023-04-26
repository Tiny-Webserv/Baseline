#include "Response.hpp"
#include "Request.hpp"
#include "ServerBlock.hpp"
#include "StateCode.hpp"

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
	_responseMessage.clear();
    _responseMessage << responseMessage;
}

std::stringstream Response::GetResponseMessage() { return _responseMessage; }


LocationBlock	&Response::getLocationBlock() {
	std::string	target = _request.GetTarget();
	for (size_t i = 0; i < _request.GetServer().GetLocation().size(); i++) {
		std::string	locationTarget = _request.GetServer().GetLocation()[i].GetLocationTarget();
        if (!locationTarget.compare(0, locationTarget.size(), target))
			return _request.GetServer().GetLocation()[i];
    }
	throw NotExist();
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

/*
	1. 허용된 메서드인지 확인하기 O
	2. autoindex <- 404처리나 디렉토리 일 때 처리하면 됨
	3. 타겟 파일 읽어와서 바디에 실어주기
*/

void Response::getMethod() {
    if (!isAllowed("GET"))
        throw PermissionDenied();

}
