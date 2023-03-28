#include "config.hpp"

config::config() {}

config::config(std::string filename) {
    // 파일 열어서 파싱하기
    std::string config_file;
    config_file = open_File(filename);
    std::string token;
    std::vector<std::string> server_stack;
    std::vector<std::string> location_stack;
    int server_Block_Idx = 0;
    /////////
    std::stringstream ss(config_file);
    while (ss >> token) {
        if (token == "server")
            continue;
        if (token == "{") { ///{ 아닌 경우 에러 처리 필요하긴 함.
            server_stack.push_back("{");
            server_Block.push_back(server_block());
            // continue;
        }

        // else ??delete!
        // {
        // 	std::cout << "err1\n" << std::endl;
        // 	exit(1);
        // }
        while (!server_stack.empty()) {
            (ss >> token);
            if (token == "}") {
                server_stack.pop_back();
                server_Block_Idx++;
                continue;
            }
            if (token == "listen") {
                if (this->server_Block[server_Block_Idx].listen.second != -1) {
                    std::cout << "err 2\n" << std::endl;
                    exit(1);
                }
                (ss >> token); // token 전진
                std::string temp_str = token;
                std::string save_str = "";
                std::string test = "";
                // const char *temp_str_char = temp_str.c_str();
                int i = 0;
                int flag = 0;
                while (temp_str[i]) {
                    if (temp_str[i] == ':') {
                        flag = 1;
                        i++;
                        while (temp_str[i]) {
                            test += temp_str[i];
                            i++;
                        }
                        break;
                    }
                    save_str += temp_str[i];
                    i++;
                }
                // char *save_chr_ptr = static_cast<char*>(&temp_str[i]);
                if (flag) {
                    this->server_Block[server_Block_Idx].listen.first =
                        save_str;
                    this->server_Block[server_Block_Idx].listen.second =
                        //(atoi(temp_str.c_str()));
                        (atoi(test.c_str()));
                } else
                    this->server_Block[server_Block_Idx].listen.second =
                        (atoi(save_str.c_str()));
                continue;
            }
            if (token == "location") {
                (ss >> token); // token 전진
                this->server_Block[server_Block_Idx].loca.push_back(
                    location_block());
                this->server_Block[server_Block_Idx]
                    .loca[this->server_Block[server_Block_Idx].loca_block_cnt]
                    .location_Target = token;
                (ss >> token); // token 전진
                if (token != "{") {
                    std::cout << "err3\n" << std::endl;
                    exit(1);
                }
                location_stack.push_back("{");

                //(ss >> token);
                while (!location_stack.empty()) { // ;삭제 필요함
                    (ss >> token);
                    if (token == "}") {
                        location_stack.pop_back();
                        if (this->server_Block[server_Block_Idx]
                                .loca[this->server_Block[server_Block_Idx]
                                          .loca_block_cnt]
                                .root == "")
                            this->server_Block[server_Block_Idx]
                                .loca[this->server_Block[server_Block_Idx]
                                          .loca_block_cnt]
                                .root =
                                this->server_Block[server_Block_Idx].root;
                        if (this->server_Block[server_Block_Idx]
                                .loca[this->server_Block[server_Block_Idx]
                                          .loca_block_cnt]
                                .root == "") {
                            std::cout << "err serroot, location root"
                                      << std::endl;
                            exit(1);
                        }
                        this->server_Block[server_Block_Idx].loca_block_cnt++;
                        continue;
                    }
                    if (token == "root") {
                        (ss >> token);
                        //
                        this->server_Block[server_Block_Idx]
                            .loca[this->server_Block[server_Block_Idx]
                                      .loca_block_cnt]
                            .root = token;
                        // std::cout
                        //     << "test root"
                        //     << this->server_Block[server_Block_Idx]
                        //            .loca[this->server_Block[server_Block_Idx]
                        //                      .loca_block_cnt]
                        //            .root
                        //     << std::endl;
                        continue;
                    }
                    if (token == "limit_except") {
                        while (token != "{") {
                            ss >> token;
                            this->server_Block[server_Block_Idx]
                                .limit_Except->push_back(token);
                        }
                        while (token != "}")
                            ss >> token;
                        continue;
                    }
                }
            }
            if (token == "server_name") {
                ss >> token;
                token.erase(token.size() - 1, 1);
                this->server_Block[server_Block_Idx].server_Name = token;
                continue;
            }
            if (token == "error_page") {
                std::vector<std::string> temp;
                while (token[0] != '/') {
                    ss >> token;
                    temp.push_back(token);
                }
                token.erase(token.size() - 1, 1);
                for (size_t i = 0; i < temp.size(); i++) {
                    this->server_Block[server_Block_Idx]
                        .error_Page[atoi(temp[i].c_str())] = token;
                }
                continue;
            }
            if (token == "client_max_body_size") {
                ss >> token;
                this->server_Block[server_Block_Idx].client_Max_Body_Size =
                    atoi(token.c_str());
                continue;
            }
            if (token == "return") {
                std::string temp;
                ss >> token;
                temp = token;
                ss >> token;
                token.erase(token.size() - 1, 1);
                this->server_Block[server_Block_Idx]
                    .return_Info[atoi(temp.c_str())] = token;
                continue;
            }
            if (token == "root") {
                ss >> token;
                token.erase(token.size() - 1, 1);
                this->server_Block[server_Block_Idx].root = token;
                continue;
            }
            if (token == "autoindex") {
                ss >> token;
                if (token == "on") {
                    this->server_Block[server_Block_Idx].autoindex = true;
                }
                continue;
            }
            if (token == "index") {
                while (token[token.size() - 1] != ';') {
                    ss >> token;
                    this->server_Block[server_Block_Idx].index.push_back(token);
                }
                token.erase(token.size() - 1, 1);
                this->server_Block[server_Block_Idx].index.push_back(token);
                continue;
            }
            // if token == "upload"
        }
    }

    // if (server_stack.empty() && token != "server") {
    // 	std::cout << "err\n" << std::endl;
    // 	exit(1);

    // else if(token == "{"){
    // 	server_stack.push_back("{");

    // }
    // else
    // 	continue;
}

// server 확인
// 확인 >> server_stack add
// while(server_stack)
// std::cout << " " << token;
// if (token != "server")
// {
// 	std::cout << "err1\n" << std::endl;
// 	exit(1);
// }
// }
// std::cout << std::endl;

/////////
// while () {
//     서버클래스 생성 과 동시에 server_root =
//         "" server_stack.append("{") while (server_stack)
//         : if getline_return ==
//         "}" server_stack.pop() continue server.listen = root =
//             >> server_root =
//                 저장 x - root = "" location location_stack =
//                     [] location_stack.append("{") location.dir =
//                         "/" while (location_stack)
//         : if getline_return ==
//           "}" location_stack.pop() continue if getline_return () ==
//           "root" : loaction_root = 저장 if !loaction_root : loaction_root
//           =
//                                                                 server_root;
//     loaction_root == "" :: >>> error "
// }
//////////

config::~config() {}

std::string config::open_File(std::string filename) {
    // 파일 open
    std::ifstream file(filename);
    std::string file_Save = "";
    std::string line;
    if (!file.is_open()) {
        std::cout << "file not found" << std::endl;
        exit(1);
    }
    while (getline(file, line, '\n')) {
        file_Save += line;
        file_Save += " ";
    }
    return file_Save;
}

// 실제 지시어 체크하는 부분
// int config::nginx_word(std::string word)
// {
// 	std::string words[12] = {
// 		"listen",
// 		"server_names",
// 		"error_page",
// 		"client_max_body_size",
// 		"limit_except",
// 		"return",
// 		"root",
// 		"autoindex",
// 		"index",
// 		"location",
// 		"upload_pass",
// 		"upload_store"
// 	};
// 	return(0);
// }

// 26번줄 반복문에서 상태 체크 조건문은 0, 1, 2 만 확인해서 각 상태 함수
// 호출하면 깔끔? status server - > 그냥 좆 된 듯

// 파일 전체 확인 반복문
// while (getline(file, line, '\n')) {
// 	std::stringstream ss(line);
// 	//한줄 확인 반복문
// 	while (ss >> token) {
// 		// none 일 때
// 		if (status == NONE) {
// 			if (token == "server")
// 				status = SERVER;
// 			else {
// 				std::cout << "server block이 지정되지 않음" <<
// std::endl; 				exit(1);
// 			}
// 		// server block 일 때
// 		} else if (status == SERVER || status == SERVER_START) {
// 			if (token == "location") {
// 				status = LOCATION;
// 			}
// 			else {
// 				server_Status(ss, status);
// 			}
// 		// location block 일 때
// 		} else if (status == LOCATION || status == LOCATION_START) {
// 			location_Status(ss);
// 		}
// 	}
// }
