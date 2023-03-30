#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// TODO 같은 순서의 스트링 기준이 아닌, 둘 중 하나만 오면 처리됨. 이전에
// 라피신의 스플릿 함수와 같은 상황. 고칠 필요성 있음
std::vector<std::string> Split(std::string lines, std::string charSet) {
	char *token;
	char *rest = const_cast<char *>(lines.c_str());
	std::vector<std::string> splited;

	while ((token = strtok_r(rest, charSet.c_str(), &rest)))
		splited.push_back(std::string(token));
	return splited;
}

std::vector<std::string> Split2(std::string lines, std::string charSet) {
	std::string::size_type startPos = 0;
	std::string::size_type charSetPos = 0;
	std::vector<std::string> splited;

	charSetPos = lines.find(charSet, startPos);
	while (charSetPos != std::string::npos) {
		splited.push_back(lines.substr(startPos, charSetPos - startPos));
		startPos = charSetPos + charSet.length();
		if (startPos < lines.length())
			charSetPos = lines.find(charSet, startPos);
		else
			break;
	}
	if (startPos < lines.length())
		splited.push_back(lines.substr(startPos));
	return splited;
}
