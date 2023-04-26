#include "StateCode.hpp"

const char *HTTPVersionError::what() const throw() {
	return "HTTP Version error : The version must be HTTP 1.1";
}

const char *MethodError::what() const throw() {
	return "Method error : Bad method request";
}

const char *BodySizeError::what() const throw() {
	return "Size error : Request Entity Too Large";
}

const char *NotExist::what() const throw() {
	return "Not found : File dose not exist";
}

const char *PermissionDenied::what() const throw() {
	return "Forbidden : Permission denied";
}

ServerError::ServerError(std::string	condition) {
	_condition = condition;
}

const char	*ServerError::what () const throw() {
	std::string	msg("Internal Server Error : ");

	msg.append(_condition);
	return msg.c_str();
}