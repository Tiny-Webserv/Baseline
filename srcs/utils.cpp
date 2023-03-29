#include <vector>
#include <string>
#include <cstring>

std::vector<std::string>	Split(std::string	lines, std::string	charSet) {
	char	*token;
	char	*rest = const_cast<char *>(lines.c_str());
	size_t charSetPos;
	std::vector<std::string>	splited;


    while ((token = strtok_r(rest, charSet.c_str(), &rest)))
		splited.push_back(std::string(token));
	return splited;
}