#include "ServerFiles.hpp"
#include "StateCode.hpp"
#include <cerrno>
#include <fstream>
#include <sstream>
#include <vector>

ServerFiles::ServerFiles() {}

ServerFiles::~ServerFiles() {}

std::map<std::string, std::vector<char> > &ServerFiles::getFile() {
	return _file;
}

std::vector<char> ServerFiles::readTextFile(std::string filename) {
	std::ifstream tmp;
	std::stringstream ss;

	tmp.open(filename);
	if (tmp.fail()) {
		if (errno == EACCES)
			throw PermissionDenied();
		else if (errno == ENOENT)
			throw NotExist();
		else if (errno == EISDIR)
			throw IsDirectory();
	}
	tmp.seekg(0, std::ios::end);
	std::streampos tmpSize = tmp.tellg();
	tmp.seekg(0, std::ios::beg);
	std::vector<char> file(tmpSize);

	// vector로 파일 읽기
	if (!tmp.read(&file[0], tmpSize))
		throw ServerError("reading file failed");
	return file;
}

std::vector<char> ServerFiles::readBinaryFile(std::string filename) {
	std::ifstream tmp;
	std::stringstream ss;

	tmp.open(filename, std::ios::binary);
	if (tmp.fail()) {
		if (errno == EACCES)
			throw PermissionDenied();
		else if (errno == ENOENT)
			throw NotExist();
		else if (errno == EISDIR)
			throw IsDirectory();
	}
	tmp.seekg(0, std::ios::end);
	std::streampos tmpSize = tmp.tellg();
	tmp.seekg(0, std::ios::beg);
	std::vector<char> file(tmpSize);

	if (!tmp.read(&file[0], tmpSize))
		throw ServerError("reading file failed");
	return file;
}

std::vector<char> &ServerFiles::getFile(std::string filename) {
	std::map<std::string, std::vector<char> >::iterator iter =
		_file.find(filename);

	if (iter != _file.end())
		return iter->second;
	if (!filename.substr(filename.size() - 4).compare(".png"))
		_file.insert(std::pair<std::string, std::vector<char> >(
			filename, readBinaryFile(filename)));
	else
		_file.insert(std::pair<std::string, std::vector<char> >(
			filename, readTextFile(filename)));
	iter = _file.find(filename);
	return iter->second;
}

const char *ServerFiles::IsDirectory::what() const throw() {
	return "is a directory";
}
