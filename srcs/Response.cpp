#include "Response.hpp"

#include "Request.hpp"
#include "ServerBlock.hpp"
#include "StateCode.hpp"
// #include "ServerFiles.hpp"
#include "unistd.h"
#include "utils.hpp"

Response::Response(Request *request) : _request(request) {
    //_serverFiles = ServerFiles();
    try {
        if (request->GetMethod() == "GET") {
            getMethod();  // autoindex 처리
        } else if (request->GetMethod() == "DELETE")
            ;  // delete method;
        else
            ;  // post method;
    } catch (NotExist &e) {
        _request->SetErrorCode(404);
        _request->SetErrorMessages(e.what());
        std::cout << "Not Exist 발생" << std::endl;
        std::map<int, std::string>::iterator it =
            _request->GetServer().GetErrorPage().find(404);
        if (it != _request->GetServer().GetErrorPage().end()) {
            _bodyMessage = _serverFiles.getFile("./docs/error.html");
            std::cout << "익셉션 바디메시지" << std::endl;
            write(1, &_bodyMessage[0], _bodyMessage.size());
            std::cout << std::endl;
        }
        _contentType = "text/html";
        // 서버 블록에서 지정된 에러 페이지가 있는지 보기

        // 있으면 그거를 바디에 저장
        // 없으면 default errorpage 가져오기
        // errorCode <=> response.getRequest().getErrorcode()
        // errorMessageShort <=>r
        // response.getRequest().getErrorMessge().substring " : "
        // errorMessage <=>
        // response.getRequest().getErrorMessge().substring " : "
        // std::string crlf(CRLF);
        // std::copy(crlf.begin(), crlf.end(),
        // std::back_inserter(_bodyMessage)); std::copy(crlf.begin(),
        // crlf.end(), std::back_inserter(_bodyMessage));
        generateStatusLine();
        generateHeader();
        joinResponseMessage();
        std::cout << "_responseMessage" << std::endl << std::endl;
        write(1, &_responseMessage[0], _responseMessage.size());
        std::cout << std::endl;
    }
}

Response::~Response() {}

Response::Response(Response &response) {
    this->_responseMessage = response.getResponseMessage();
}

ServerFiles Response::_serverFiles = ServerFiles();

// void Response::SetResponseMessage(std::string responseMessage) {
//	_responseMessage.clear();
//     _responseMessage << responseMessage;
// }

// std::stringstream Response::GetResponseMessage() { return _responseMessage; }

LocationBlock &Response::getLocationBlock() {
    ServerBlock Block;

    std::string target = _request->GetTarget();
    std::cout << "target : " << target << std::endl;
    for (size_t i = 0; i < _request->GetServer().GetLocation().size(); i++) {
        std::string locationTarget =
            _request->GetServer().GetLocation()[i].GetLocationTarget();
        if (!strncmp(locationTarget.c_str(), target.c_str(),
                     locationTarget.size())) {
            std::cout << "헤이2 "
                      << _request->GetServer().GetErrorPage().begin()->first
                      << _request->GetServer().GetErrorPage().begin()->second
                      << std::endl;
            return _request->GetServer().GetLocation()[i];
        }
    }
    throw NotExist();
}

bool Response::isCGI() {
    LocationBlock block = getLocationBlock();
    if (block.GetLocationTarget().find(".cgi")) return true;
    return false;
}

bool Response::isAutoIndex() {
    LocationBlock block = getLocationBlock();
    if (block.GetAutoIndex()) return true;
    return false;
}

bool Response::isAllowed(std::string method) {
    std::vector<std::string> limit_except = getLocationBlock().GetLimitExcept();

    if (limit_except.size() == 0)
        return true;
    else if (find(limit_except.begin(), limit_except.end(), method) ==
             limit_except.end())
        return false;
    else
        return true;
}

void Response::generateStatusLine() {
    std::stringstream ss;

    ss << "HTTP/1.1 " << _request->GetErrorCode() << ' '
       << _request->GetErrorMessages().substr(
              0, _request->GetErrorMessages().find(" : "))
       << CRLF;
    std::string line = ss.str();
    std::copy(line.begin(), line.end(),
              std::back_inserter(_statusHeaderMessage));
}

void Response::generateHeader() {
    std::stringstream ss;

    ss << "Server: Webserv" << CRLF;
    if (_bodyMessage.size()) {
        ss << "Content-Type: " << _contentType << CRLF;
        ss << "Content-Length: " << _bodyMessage.size() << CRLF;
    }
    ss << CRLF;
    std::string line = ss.str();
    std::copy(line.begin(), line.end(),
              std::back_inserter(_statusHeaderMessage));
}
void Response::joinResponseMessage() {
    _responseMessage.insert(_responseMessage.end(),
                            _statusHeaderMessage.begin(),
                            _statusHeaderMessage.end());
    _responseMessage.insert(_responseMessage.end(), _bodyMessage.begin(),
                            _bodyMessage.end());
    std::string crlf(CRLF);
    std::copy(crlf.begin(), crlf.end(), std::back_inserter(_responseMessage));
    std::copy(crlf.begin(), crlf.end(), std::back_inserter(_responseMessage));
}
/*
        1. 허용된 메서드인지 확인하기 O
        2. autoindex <- 404처리나 디렉토리 일 때 처리하면 됨
        3. 타겟 파일 읽어와서 바디에 실어주기
*/

void Response::getMethod() {
    std::string fileToRead;
    if (!isAllowed("GET")) throw PermissionDenied();
    fileToRead = getLocationBlock().GetRoot() + _request->GetTarget();
    if (_request->GetTarget() == getLocationBlock().GetLocationTarget()) {
        fileToRead.append("/");
        fileToRead.append(getLocationBlock().GetIndex()[0]);
    }
    std::cout << "file to read : " << fileToRead << std::endl;
    _bodyMessage = _serverFiles.getFile(fileToRead);
    write(1, &_bodyMessage[0], _bodyMessage.size());
    // 확장자가 .로 끝날경우 text/plain
    if (fileToRead.find(".") == std::string::npos ||
        fileToRead.find(".") == fileToRead.size() - 1)
        _contentType = "text/plain";
    else {
        std::string extention =
            fileToRead.substr((fileToRead.rfind(".") + 1 <= fileToRead.size()
                                   ? fileToRead.rfind(".") + 1
                                   : -1));
        if (!extention.compare(".png"))  // png 확장자 확인
            _contentType = "image/png";
        else if (!extention.compare("txt"))
            _contentType = "text/plain";
        else
            _contentType =
                "text/" +
                extention;  // 그 외라면 오는 확장자 맞춰서 콘텐츠 타입 설정
    }
    generateStatusLine();
    generateHeader();
    joinResponseMessage();
    write(1, &_responseMessage[0], _responseMessage.size());
    std::cout << "befor =====>" << std::endl;
}

std::vector<char> Response::getResponseMessage() { return _responseMessage; }
std::vector<char> Response::getStatusHeaderMessage() {
    return _statusHeaderMessage;
}
std::vector<char> Response::getBodyMessage() { return _bodyMessage; }

// setter
void Response::setResponseMessage(std::vector<char> &responseMessage) {
    _responseMessage = responseMessage;
}
void Response::setStatusHeaderMessage(std::vector<char> &statusHeaderMessage) {
    _statusHeaderMessage = statusHeaderMessage;
}
void Response::setBodyMessage(std::vector<char> &bodyMessage) {
    _bodyMessage = bodyMessage;
}
