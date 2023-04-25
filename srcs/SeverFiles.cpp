
#include <cerrno>
#include <fstream>
#include "../incs/ServerFiles.hpp"
#include <vector>
#include <sstream>

ServerFiles::ServerFiles() {
}

ServerFiles::~ServerFiles() {
}

// getter
std::map<std::string, std::vector<char> > &ServerFiles::getFile() {
	return _file;
}


/*
	1. filename으로 파일 열기
		- 실패시
			1. 404 던지기
			2. errno 확인해보기
				- no search file ~ 404
				- 파일 읽기 권한 ~ 403
	2. 파일을 읽고
	3. map에 저장하기
	4. 이걸 리턴
*/

std::vector<char>	ServerFiles::readTextFile(std::string	filename) {
	std::ifstream tmp;
    std::stringstream	ss;

	tmp.open(filename);
	if (tmp.fail()) {
		if (errno == EACCES)
			throw /* permission exception */;
		else if (errno == ENOENT)
			throw /* not found */ ;
		else if (errno == EISDIR)
			throw /* is a directory */;
	}
	ss << tmp.rdbuf();
	tmp.close();

	std::vector<char>	file(ss.str().size());
	std::copy(ss.str().begin(), ss.str().end(), file.begin());
	return file;
}

std::vector<char> ServerFiles::readBinaryFile(std::string filename) {
        std::ifstream tmp;
    std::stringstream	ss;

	tmp.open(filename, std::ios::binary);
	if (tmp.fail()) {
		if (errno == EACCES)
			throw /* permission exception */;
		else if (errno == ENOENT)
			throw /* not found */ ;
		else if (errno == EISDIR)
			throw /* is a directory */;
	}
	ss << std::ios::binary << tmp.rdbuf();
	tmp.close();

	std::vector<char>	file(ss.str().size());
	std::copy(ss.str().begin(), ss.str().end(), file.begin());
	return file;
}

std::vector<char> &ServerFiles::getFile(std::string filename) {
        std::map<std::string, std::vector<char> >::iterator	iter = _file.find(filename);

	if (iter != _file.end())
		return iter->second;
	if (!filename.substr(filename.size() - 4).compare(".png"))
		_file.insert(std::pair<std::string, std::vector<char> >(filename, readBinaryFile(filename)));
	else
		_file.insert(std::pair<std::string, std::vector<char> >(filename, readTextFile(filename)));
	iter = _file.find(filename);
	return iter->second;
}
