#ifndef SERVERFILES_HPP
# define SERVERFILES_HPP

#include <map>
#include <iostream>
#include <string>

class ServerFiles {
private:
	std::map<std::string, std::vector<char> >	_file;
public:
	ServerFiles();
	~ServerFiles();

	//getter
	std::map<std::string, std::vector<char> > &getFile();

	std::vector<char>	&getFile(std::string	filename);

    std::vector<char> readTextFile(std::string filename);

    std::vector<char> readBinaryFile(std::string filename);
};

#endif