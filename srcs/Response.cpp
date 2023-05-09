#include "Response.hpp"
#include "Request.hpp"
#include "ServerBlock.hpp"
#include "StateCode.hpp"
// #include "ServerFiles.hpp"
#include "dirent.h"
#include "unistd.h"
#include "utils.hpp"
#include <cstdio>
#include <iostream>

Response::Response(Request *request) : _request(request) {
    //_serverFiles = ServerFiles();
    try {
		std::cout << "I'm in Request constructor\n";
        if (request->GetErrorCode() != 200)
            generateErrorBody();
        else if (request->GetMethod() == "GET") {
            getMethod(); // autoindex 처리
        }
        else if (request->GetMethod() == "POST") {
            postMethod(); // autoindex 처리
        }
        else if (request->GetMethod() == "DELETE") {
           deleteMethod();
            ; // delete method;

            }
    } catch (NotExist &e) {
        std::cerr << e.what() << std::endl;
		_request->SetErrorCode(e._errorCode);
		_request->SetErrorMessages(e.what());
		try {
			if (isAutoIndex())
				generateAutoindex(_request->GetServer().GetRoot()); // autoindex 처리
			else {
				generateErrorBody();
			}
		} catch (NotExist &e) {
			generateErrorBody();
		}
        _contentType = "text/html";
    } catch (StateCode &e) {
        _request->SetErrorCode(e._errorCode);
        _request->SetErrorMessages(e.what());
        generateErrorBody();
        _contentType = "text/html";
    }
	generateStatusLine();
	generateHeader();
	joinResponseMessage();
	std::cout << "_responseMessage" << std::endl << std::endl;
	write(1, &_responseMessage[0], _responseMessage.size());
}

Response::~Response() {}

Response::Response(Response &response) {
    this->_responseMessage = response.getResponseMessage();
}

ServerFiles Response::_serverFiles = ServerFiles();

std::string	Response::fetchFilePath() {
	std::string	location = getLocationBlock().GetLocationTarget();
	std::string	target = _request->GetTarget().substr(location.size());

	if (target.size())
		target.insert(0, getLocationBlock().GetRoot());
	else {
		struct stat	buffer;
		std::vector<std::string> index = getLocationBlock().GetIndex();

        for (size_t i = 0; i < index.size(); i++) {
			target = getLocationBlock().GetRoot() + "/" + index[i];
			if (stat(target.c_str(), &buffer) != 0)
				continue ;
            else
                return target;
		}
		std::cerr << "from fetchFilePath (" << target << ")" << std::endl;
		throw NotExist();
    }
	return target;
}

LocationBlock &Response::getLocationBlock() {
    ServerBlock Block;
	int rootLocationIndex = -1;

    std::string target = _request->GetTarget();
    for (size_t i = 0; i < _request->GetServer().GetLocation().size(); i++) {
        std::string locationTarget =
            _request->GetServer().GetLocation()[i].GetLocationTarget();
        if (!strncmp(locationTarget.c_str(), target.c_str(),
                     locationTarget.size())) {
			if (locationTarget == "/") {
				rootLocationIndex = static_cast<int>(i);
				continue ;
			}
            return _request->GetServer().GetLocation()[i];
        }
    }
	if (rootLocationIndex != -1)
		return _request->GetServer().GetLocation()[rootLocationIndex];
	std::cerr << "from getLocationBlock()" << std::endl;
    throw NotExist();
}

bool Response::isCGI() {
    LocationBlock block = getLocationBlock();
    if (block.GetLocationTarget().find(".php"))
        return true;
    return false;
}

bool Response::isAutoIndex() {
    LocationBlock block = getLocationBlock();
    if (block.GetAutoIndex() == 1)
        return true;
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

void Response::generateAutoindex(const std::string &directory) {
    std::string dir_path = directory;
    DIR *dir = opendir(dir_path.c_str());
    struct dirent *entry;
	std::stringstream ss;
    std::string tmp;
    ss << "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " << dir_path
              << "</title>\n</head>\n<body>\n<h1>Index of " << dir_path
              << "</h1>\n<hr>\n<table>\n<tr><th>Name</th><th>Last "
                 "Modified</th><th>Size</th></tr>\n";

    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name; // 파일/디렉토리 이름
        ss << "<tr><td><a href=\"" << name << "\">" << name
                  << "</a></td><td></td><td></td></tr>\n";
    }

    ss << "</table>\n<hr>\n</body>\n</html>\n";
    tmp = ss.str();

    closedir(dir);
	_bodyMessage.clear();
	std::copy(tmp.begin(), tmp.end(),
              std::back_inserter(_bodyMessage));

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
	if (_request->GetErrorCode() == NotAllowed) {
		std::vector<std::string> limit_except = getLocationBlock().GetLimitExcept();
		ss << "Allow: ";
		for (size_t i = 0; i < limit_except.size(); ++i) {
			if (i)
				ss << ", ";
			ss << limit_except[i];
		}
		ss << CRLF;
	}
    ss << CRLF;
    std::string line = ss.str();
    std::copy(line.begin(), line.end(),
              std::back_inserter(_statusHeaderMessage));
}

void Response::generateDefaultErrorPage() {
    std::vector<char> vec = _serverFiles.getFile("./docs/error.html");
    std::string str(vec.begin(), vec.end());
    str.push_back('\0');
    std::stringstream ss;
    ss << _request->GetErrorCode();
    size_t header = str.find("errorCode");

    if (header != std::string::npos) {
        str.replace(header, std::string("errorCode").length(), ss.str());
    }
    size_t body = str.find("errorMessageShort");
    if (body != std::string::npos) {
        str.replace(body, std::string("errorMessageShort").length(),
                    _request->GetErrorMessages().substr(
                        0, _request->GetErrorMessages().find(" : ")));
    }
    size_t pos = str.find("errorMessage");
    if (pos != std::string::npos) {
        str.replace(pos, std::string("errorMessage").length(),
                    _request->GetErrorMessages().substr(
                        _request->GetErrorMessages().find(" : ") + 3));
    }
    _bodyMessage.clear();
    std::copy(str.begin(), str.end(), std::back_inserter(_bodyMessage));
}

void Response::generateErrorBody() {
    std::map<int, std::string>::iterator it =
        _request->GetServer().GetErrorPage().find(_request->GetErrorCode());
    try {
        if (it != _request->GetServer().GetErrorPage().end()) {
            std::string filename = _request->GetServer().GetRoot() + it->second;
            _bodyMessage = _serverFiles.getFile(filename);
        } else
            generateDefaultErrorPage();
    } catch (const StateCode &e) {
        generateDefaultErrorPage();
    }
    _contentType = "text/html";
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

bool	Response::isDirectory(const char *directory) {
	struct stat	buffer;

	if (stat(directory, &buffer) != 0) {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

void Response::getMethod() {
		std::string fileToRead;
		if (!isAllowed("GET"))
			throw MethodNotAllowed();
		// 파일 경로 가져오기
		fileToRead = fetchFilePath();
		std::cerr << "file to read : " << fileToRead << std::endl;
		// 디렉토리인지 체크
		if (isDirectory(fileToRead.c_str())) {
			if (!isAutoIndex())
				throw PermissionDenied();
			generateAutoindex(fileToRead);
			return ;
		}
		// 파일 가져오는 부분
		_bodyMessage = _serverFiles.getFile(fileToRead);
		// Content-Type 설정하는 부분
		// 확장자가 .로 끝날경우 text/plain
		if (fileToRead.find(".") == std::string::npos ||
			fileToRead.find(".") == fileToRead.size() - 1)
			_contentType = "text/plain";
		else {
			std::string extention =
				fileToRead.substr((fileToRead.rfind(".") + 1 <= fileToRead.size()
									? fileToRead.rfind(".") + 1
									: -1));
			if (!extention.compare(".png")) // png 확장자 확인
				_contentType = "image/png";
			else if (!extention.compare("txt"))
				_contentType = "text/plain";
			else
				_contentType =
					"text/" +
                extention; // 그 외라면 오는 확장자 맞춰서 콘텐츠 타입 설정
		}
}

void	Response::postMethod() {
        std::string fileToRead;
		std::cerr << "I'm in POST Method" << std::endl;
		if (!isAllowed("POST"))
			throw MethodNotAllowed();
		fileToRead = fetchFilePath();
		std::cout << "file to read : " << fileToRead << std::endl;
		if (isDirectory(fileToRead.c_str())) {
			if (!isAutoIndex())
				throw PermissionDenied();
			generateAutoindex(fileToRead);
			return ;
		}
		_bodyMessage = _serverFiles.getFile(fileToRead);
		// 확장자가 .로 끝날경우 text/plain
		if (fileToRead.find(".") == std::string::npos ||
			fileToRead.find(".") == fileToRead.size() - 1)
			_contentType = "text/plain";
		else {
			std::string extention =
				fileToRead.substr((fileToRead.rfind(".") + 1 <= fileToRead.size()
									? fileToRead.rfind(".") + 1
									: -1));
			if (!extention.compare(".png")) // png 확장자 확인
				_contentType = "image/png";
			else if (!extention.compare("txt"))
				_contentType = "text/plain";
			else
				_contentType =
					"text/" +
                extention; // 그 외라면 오는 확장자 맞춰서 콘텐츠 타입 설정
		}
}

void	Response::deleteMethod(){
    //1. target 삭제가능 200 ok
    //2. 만약 delete 메소드를 허용하지 않는다면 405 method not allowed
    //3. 삭제할 타켓을 못찾으면 404 not found
    std::string fileToRead;
    if (!isAllowed("DELETE"))
		throw MethodNotAllowed();
	fileToRead = getLocationBlock().GetRoot() + _request->GetTarget();

    if (remove(fileToRead.c_str()) != 0) {
        throw NotExist();
    } else {
        std::cout << "File " << fileToRead << " successfully deleted" << std::endl;
    }
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
