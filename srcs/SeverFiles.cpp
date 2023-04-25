
#include "ServerFiles.hpp"
#include "StateCode.hpp"
#include <cerrno>
#include <fstream>
#include <sstream>
#include <vector>

ServerFiles::ServerFiles() {}

ServerFiles::~ServerFiles() {}

// getter
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
	ss << tmp.rdbuf();
	tmp.close();

	std::vector<char> file(ss.str().size());
	std::copy(ss.str().begin(), ss.str().end(), file.begin());
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
	ss << std::ios::binary << tmp.rdbuf();
	tmp.close();

	std::vector<char> file(ss.str().size());
	std::copy(ss.str().begin(), ss.str().end(), file.begin());
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

const char SeverFiles::IsDirectory::what() const throw() {
	return "is a directory";
}
