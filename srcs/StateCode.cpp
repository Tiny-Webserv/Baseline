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
    return "NotFound : File dose not exist";
}

const char *PermissionDenied::what() const throw() {
    return "Forbidden : Permission denied";
}

ServerError::ServerError(const char *condition) {
    char *msg = new char[std::strlen(condition) + 25];
    strncpy(msg, "Internal Server Error : ", 25);
    strncat(msg, condition, std::strlen(condition));
    _condition = msg;
}

const char *ServerError::what() const throw() { return _condition; }