#include "Config.hpp"

int nginx_word(std::string line);
bool test(std::string line);

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
    ConfigFile = open_File(filename);
    std::string token;
    std::vector<std::string> server_stack;
    std::vector<std::string> location_stack;
    int server_Block_Idx = 0;
    /////////
    std::stringstream ss(ConfigFile);
    while (ss >> token) {
        if (token == "server") {
            ss >> token;
            if (token == "{") {
                server_stack.push_back("{");
                _ServerBlockObject.push_back(ServerBlock());
            } else {
                std::cout << "server { 필수" << std::endl;
            }
        } else {
            std::cout << " or must \"server\" first" << std::endl;
            exit(1);
        }
        while (!server_stack.empty()) {
            (ss >> token);
            if (token == "}") {
                server_stack.pop_back();
                server_Block_Idx++;
                continue;
            }
            if (token == "listen") {
                (ss >> token);
                int size = token.size() - 1;
                std::string test_len = "";
                if (this->_ServerBlockObject[server_Block_Idx]._Port != -1)
                    continue;
                Itos(atoi(token.c_str()), test_len);
                if ((int)test_len.size() == size)
                    this->_ServerBlockObject[server_Block_Idx]._Port =
                        atoi(token.c_str());
                else {
                    std::cout << "port err" << std::endl;
                    exit(1);
                }
                continue;
            }
            if (token == "location") {
                (ss >> token);
                this->_ServerBlockObject[server_Block_Idx]._Location.push_back(
                    location_block());
                this->_ServerBlockObject[server_Block_Idx]
                    ._Location[this->_ServerBlockObject[server_Block_Idx]
                                   ._LocationBlockSize]
                    ._LocationTarget = token;
                (ss >> token);
                if (token != "{") {
                    std::cout << "err3\n" << std::endl;
                    exit(1);
                }
                location_stack.push_back("{");

                while (!location_stack.empty()) {
                    (ss >> token);
                    if (token == "}") {
                        location_stack.pop_back();
                        if (this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._Root == "")
                            this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._Root =
                                this->_ServerBlockObject[server_Block_Idx]
                                    ._Root;
                        if (this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._Index.empty()) {
                            this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._Index =
                                this->_ServerBlockObject[server_Block_Idx]
                                    ._Index;
                        }
                        if (this->_ServerBlockObject[server_Block_Idx]
                                    ._Location[this->_ServerBlockObject
                                                   [server_Block_Idx]
                                                       ._LocationBlockSize]
                                    ._Root == "" ||
                            this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._Index.empty()) {
                            std::cout << "err serroot, location root or _Index"
                                      << std::endl;
                            exit(1);
                        }
                        this->_ServerBlockObject[server_Block_Idx]
                            ._LocationBlockSize++;
                        continue;
                    }
                    if (token == "root") {
                        (ss >> token);
                        if (this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._Root.empty()) {
                            token.erase(token.size() - 1, 1);
                            this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._Root = token;
                        }
                        continue;
                    }
                    if (token == "limit_except") {
                        while (ss >> token) {
                            if (token == "{")
                                break;
                            this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._LimitExcept.push_back(token);
                        }
                        while (token != "}")
                            ss >> token;
                        continue;
                    }
                    if (token == "autoindex") {
                        ss >> token;
                        if (this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._AutoIndex == -1) {
                            if (token == "on;") {
                                this->_ServerBlockObject[server_Block_Idx]
                                    ._Location[this->_ServerBlockObject
                                                   [server_Block_Idx]
                                                       ._LocationBlockSize]
                                    ._AutoIndex = 1;
                            } else if (token == "off;") {
                                this->_ServerBlockObject[server_Block_Idx]
                                    ._Location[this->_ServerBlockObject
                                                   [server_Block_Idx]
                                                       ._LocationBlockSize]
                                    ._AutoIndex = 0;
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
                            this->_ServerBlockObject[server_Block_Idx]
                                ._Location[_ServerBlockObject[server_Block_Idx]
                                               ._LocationBlockSize]
                                ._Index.push_back(token);
                        }
                        token.erase(token.size() - 1, 1);
                        this->_ServerBlockObject[server_Block_Idx]
                            ._Location[_ServerBlockObject[server_Block_Idx]
                                           ._LocationBlockSize]
                            ._Index.push_back(token);
                        continue;
                    }
                    if (token == "return") {
                        int LocaIdx = this->_ServerBlockObject[server_Block_Idx]
                                          ._LocationBlockSize;
                        if (this->_ServerBlockObject[server_Block_Idx]
                                    ._Location[LocaIdx]
                                    ._Return.first == 0 &&
                            this->_ServerBlockObject[server_Block_Idx]
                                    ._Location[LocaIdx]
                                    ._Return.second == "") {
                            ss >> token;
                            this->_ServerBlockObject[server_Block_Idx]
                                ._Location[LocaIdx]
                                ._Return.first = atoi(token.c_str());
                            ss >> token;
                            token.erase(token.size() - 1, 1);
                            this->_ServerBlockObject[server_Block_Idx]
                                ._Location[LocaIdx]
                                ._Return.second = token;
                        } else {
                            while (ss >> token) {
                                if (token[token.size() - 1] == ';')
                                    break;
                            }
                        }
                        continue;
                    }
                    if (token == "upload_pass") {
                        if (this->_ServerBlockObject[server_Block_Idx]
                                ._Location
                                    [this->_ServerBlockObject[server_Block_Idx]
                                         ._LocationBlockSize]
                                ._UploadPass != "") {
                            continue;
                        }
                        ss >> token;
                        token.erase(token.size() - 1, 1);
                        this->_ServerBlockObject[server_Block_Idx]
                            ._Location
                                [this->_ServerBlockObject[server_Block_Idx]
                                     ._LocationBlockSize]
                            ._UploadPass = token;
                        continue;
                    }
                }
            }
            if (token == "server_name") {
                while (ss >> token) {
                    if (token[token.size() - 1] == ';') {
                        token.erase(token.size() - 1, 1);
                        this->_ServerBlockObject[server_Block_Idx]
                            ._ServerName.push_back(token);
                        break;
                    }
                    this->_ServerBlockObject[server_Block_Idx]
                        ._ServerName.push_back(token);
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
                    this->_ServerBlockObject[server_Block_Idx]
                        ._ErrorPage[atoi(temp[i].c_str())] = token;
                }
                continue;
            }
            if (token == "client_max_body_size") {
                ss >> token;
                if (this->_ServerBlockObject[server_Block_Idx]
                        ._ClientMaxBodySize == 1024) {
                    this->_ServerBlockObject[server_Block_Idx]
                        ._ClientMaxBodySize = atoi(token.c_str());
                }
                continue;
            }
            if (token == "root") {
                ss >> token;
                if (this->_ServerBlockObject[server_Block_Idx]._Root.empty()) {
                    token.erase(token.size() - 1, 1);
                    this->_ServerBlockObject[server_Block_Idx]._Root = token;
                }
                continue;
            }
            if (token == "index") {
                while (ss >> token) {
                    if (token[token.size() - 1] == ';')
                        break;
                    this->_ServerBlockObject[server_Block_Idx]._Index.push_back(
                        token);
                }
                token.erase(token.size() - 1, 1);
                this->_ServerBlockObject[server_Block_Idx]._Index.push_back(
                    token);
                continue;
            }
        }
    }

    if (!server_stack.empty()) {
        std::cout << "scope err" << std::endl;
    }
}

Config::~Config() {}

std::string Config::open_File(std::string filename) {
    // 파일 open
    std::ifstream file(filename);
    std::string file_Save = "";
    std::string line;
    if (!file.is_open()) {
        std::cout << "file not found" << std::endl;
        exit(1);
    }
    while (getline(file, line, '\n')) {
        if (nginx_word(line) == 0) {
            std::cout << "; err" << std::endl;
            exit(1);
        }
        file_Save += line;
        file_Save += " ";
    }
    return file_Save;
}

bool test(std::string line) {
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
int nginx_word(std::string line) {
    std::stringstream ss(line);
    std::string FirstToken;
    ss >> FirstToken;
    std::string words[8] = {
        "listen", "server_name", "error_page", "client_max_body_size",
        "return", "root",        "autoindex",  "index"};
    for (int i = 0; i < 8; i++) {
        if (words[i] == FirstToken) {
            if (test(line))
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
