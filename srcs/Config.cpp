#include "Config.hpp"


int NginxWord(std::string line);
bool CheckSemicolon(std::string line);	//

Config::Config() {}

void Itos(int num, std::string &ret) {
	if (num >= 10) {
		Itos(num / 10, ret);
		ret += static_cast<char>((num % 10) + 48);
	} else
		ret += static_cast<char>(num + 48);
}

Config::Config(std::string filename) {
	// 파일 열어서 파싱하기
	std::string ConfigFile;
	ConfigFile = OpenFile(filename);
	std::string token;
	std::vector<std::string> serverStack;
	std::vector<std::string> locationStack;
	ServerBlock curServerBlockObject;

	// ServerBlock CurrServerBlock; /////////
	/////////
	std::stringstream ss(ConfigFile);
	while (ss >> token) {
		if (token == "server") {
			ss >> token;
			if (token == "{") {
				serverStack.push_back("{");
				curServerBlockObject.ServerBlockClear();
			} else {
				std::cout << "server { 필수" << std::endl;
			}
		} else {
			std::cout << " or must \"server\" first" << std::endl;
			exit(1);
		}
		while (!serverStack.empty()) {
			(ss >> token);
			if (token == "}") {
				serverStack.pop_back();
				_ServerBlockObject.push_back(curServerBlockObject);
				continue;
			}
			if (token == "listen") {
				(ss >> token);
				int size = token.size() - 1;
				std::string test_len = "";
				if (curServerBlockObject.GetPort() != -1)
					continue;
				Itos(atoi(token.c_str()), test_len);
				if ((int)test_len.size() == size)
					curServerBlockObject.SetPort(atoi(token.c_str()));
				else {
					std::cout << "port err" << std::endl;
					exit(1);
				}
				continue;
			}
			if (token == "location") {
				(ss >> token);
				LocationBlock curLocationBlock;
				curLocationBlock.SetLocationTarget(token);
				(ss >> token);
				if (token != "{") {
					std::cout << "err3\n" << std::endl;
					exit(1);
				}
				locationStack.push_back("{");

				while (!locationStack.empty()) {
					(ss >> token);
					if (token == "}") {
						locationStack.pop_back();
						if (curLocationBlock.GetRoot() == "")
							curLocationBlock.SetRoot(
								curServerBlockObject.GetRoot());
						if (curLocationBlock.GetIndex().empty()) {
							curLocationBlock.SetIndex(
								curServerBlockObject.GetIndex());
						}
						if (curLocationBlock.GetRoot() == "" ||
							curLocationBlock.GetIndex().empty()) {
							std::cout << "err serroot, location root or _Index"
									  << std::endl;
							exit(1);
						}
						curServerBlockObject.AddLocationBlock(curLocationBlock);
						continue;
					}
					if (token == "root") {
						(ss >> token);
						if (curLocationBlock.GetRoot().empty()) {
							token.erase(token.size() - 1, 1);
							curLocationBlock.SetRoot(token);
						}
						continue;
					}
					if (token == "limit_except") {
						while (ss >> token) {
							if (token == "{")
								break;
							curLocationBlock.AddLimitExcept(token);
						}
						while (token != "}")
							ss >> token;
						continue;
					}
					if (token == "autoindex") {
						ss >> token;
						if (curLocationBlock.GetAutoIndex() == -1) {
							if (token == "on;") {
								curLocationBlock.SetAutoIndex(1);
							} else if (token == "off;") {
								curLocationBlock.SetAutoIndex(0);
							} else {
								std::cout << "autoindex err" << std::endl;
								exit(1);
							}
						}
						continue;
					}
					if (token == "index") {
						while (ss >> token) {
							if (token[token.size() - 1] == ';')
								break;
							curLocationBlock.AddIndex(token);
						}
						token.erase(token.size() - 1, 1);
						curLocationBlock.AddIndex(token);
						continue;
					}
					if (token == "return") {
						if (curLocationBlock.GetReturn()
									.first == 0 &&
							curLocationBlock.GetReturn()
									.second == "") {
							ss >> token;
							std::pair<int, std::string> temp;
							temp.first = atoi(token.c_str());
							ss >> token;
							token.erase(token.size() - 1, 1);
							temp.second = token;
							curLocationBlock
								.SetReturn(temp);
						} else {
							while (ss >> token) {
								if (token[token.size() - 1] == ';')
									break;
							}
						}
						continue;
					}
					if (token == "upload_pass") {
						if (curLocationBlock.GetUploadPass() != "") {
							continue;
						}
						ss >> token;
						token.erase(token.size() - 1, 1);
						curLocationBlock.SetUploadPass(token);
						continue;
					}
				}
			}
			if (token == "server_name") {
				while (ss >> token) {
					if (token[token.size() - 1] == ';') {
						token.erase(token.size() - 1, 1);
						curServerBlockObject.AddServerName(token);
						break;
					}
					curServerBlockObject.AddServerName(token);
				}
				continue;
			}
			if (token == "error_page") {
				std::vector<std::string> temp;
				while (ss >> token) {
					if (token[0] == '/')
						break;
					temp.push_back(token);
				}
				token.erase(token.size() - 1, 1);
				for (size_t i = 0; i < temp.size(); i++) {
					curServerBlockObject.AddErrorPage(atoi(temp[i].c_str()),
													  token);
				}
				continue;
			}
			if (token == "client_max_body_size") {
				ss >> token;
				if (curServerBlockObject.GetClientMaxBodySize() == 1024) {
					curServerBlockObject.SetClientMaxBodySize(
						atoi(token.c_str()));
				}
				continue;
			}
			if (token == "root") {
				ss >> token;
				if (curServerBlockObject.GetRoot().empty()) {
					token.erase(token.size() - 1, 1);
					curServerBlockObject.SetRoot(token);
				}
				continue;
			}
			if (token == "index") {
				while (ss >> token) {
					if (token[token.size() - 1] == ';')
						break;
					curServerBlockObject.AddServerIndex(token);
				}
				token.erase(token.size() - 1, 1);
				curServerBlockObject.AddServerIndex(token);
				continue;
			}
		}
	}

	if (!serverStack.empty()) {
		std::cout << "scope err" << std::endl;
	}
}

Config::~Config() {}

std::string Config::OpenFile(std::string filename) {
	// 파일 open
	std::ifstream file(filename);
	std::string file_Save = "";
	std::string line;
	if (!file.is_open()) {
		std::cout << "file not found" << std::endl;
		exit(1);
	}
	while (getline(file, line, '\n')) {
		if (NginxWord(line) == 0) {
			std::cout << "; err" << std::endl;
			exit(1);
		}
		file_Save += line;
		file_Save += " ";
	}
	return file_Save;
}

bool CheckSemicolon(std::string line) {
	size_t len = line.size() - 1;
	size_t idx = 0;
	while (line[idx] != ';') {
		idx++;
		if (len < idx)
			return false;
	}
	if (len != idx)
		return false;
	return true;
}
// 실제 지시어 체크하는 부분
int NginxWord(std::string line) {
	std::stringstream ss(line);
	std::string FirstToken;
	ss >> FirstToken;
	std::string words[8] = {
		"listen", "server_name", "error_page", "client_max_body_size",
		"return", "root",        "autoindex",  "index"};
	for (int i = 0; i < 8; i++) {
		if (words[i] == FirstToken) {
			if (CheckSemicolon(line))
				return 1;
			else {
				std::cout << "=====semicolon test======" << std::endl;
				std::cout << FirstToken << std::endl;
				std::cout << "=====semicolon test======" << std::endl;

				return 0;
			}
		}
	}
	return 1;
}
