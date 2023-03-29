#include "config.hpp"

config::config() {}

void Itos(int num, std::string &ret) {
	// char s = 'c';
	// ret += s;
	if (num >= 10) {
		Itos(num / 10, ret);
		ret += static_cast<char>((num % 10) + 48);
	} else
		ret += static_cast<char>(num + 48);
}

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
		if (token == "server") {
			ss >> token;
			if (token == "{") { ///{ 아닌 경우 에러 처리 필요하긴 함.
				server_stack.push_back("{");
				server_Block.push_back(server_block());
				// continue;
			} else {
				std::cout << "server { 필수" << std::endl;
			}
		} else {
			std::cout << "error must \"server\" first" << std::endl;
			exit(1);
		}

		// ##################################################
		// if (token == "server")
		//     continue;
		// if (token == "{") { ///{ 아닌 경우 에러 처리 필요하긴 함.
		//     server_stack.push_back("{");
		//     server_Block.push_back(server_block());
		//     // continue;
		// }
		// ##################################################

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
				(ss >> token);
				///;없는 경우 예외처리 추가?
				int size = token.size() - 1;
				std::string test_len = "";
				// this->server_Block[server_Block_Idx].port =
				// atoi(token.c_str());
				Itos(atoi(token.c_str()), test_len);
				if ((int)test_len.size() == size)
					this->server_Block[server_Block_Idx].port =
						atoi(token.c_str());
				else {
					std::cout << test_len << std::endl;
					std::cout << test_len.size() << std::endl;

					std::cout << atoi(token.c_str()) << std::endl;
					std::cout << size << std::endl;

					std::cout << "port err" << std::endl;
					exit(1);
				}
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
						continue;
					}
					if (token == "limit_except") {
						while (ss >> token) {
							if (token == "{")
								break;
							this->server_Block[server_Block_Idx]
								.loca[this->server_Block[server_Block_Idx]
										  .loca_block_cnt]
								.limit_Except.push_back(token);
						}
						while (token != "}")
							ss >> token;
						continue;
					}
					if (token == "autoindex") {
						ss >> token;
						if (this->server_Block[server_Block_Idx]
								.loca[this->server_Block[server_Block_Idx]
										  .loca_block_cnt]
								.autoindex == -1) {
							if (token == "on;") {
								this->server_Block[server_Block_Idx]
									.loca[this->server_Block[server_Block_Idx]
											  .loca_block_cnt]
									.autoindex = 1;
							} else if (token == "off;") {
								this->server_Block[server_Block_Idx]
									.loca[this->server_Block[server_Block_Idx]
											  .loca_block_cnt]
									.autoindex = 0;
							} else {
								std::cout << "autoindex err" << std::endl;
								exit(1);
							}
						}
						continue;
					}
					if (token == "return") {
						int LocaIdx =
							this->server_Block[server_Block_Idx].loca_block_cnt;
						if (this->server_Block[server_Block_Idx]
									.loca[LocaIdx]
									.Return.first == 0 &&
							this->server_Block[server_Block_Idx]
									.loca[LocaIdx]
									.Return.second == "") {
							ss >> token;
							this->server_Block[server_Block_Idx]
								.loca[LocaIdx]
								.Return.first = atoi(token.c_str());
							ss >> token;
							token.erase(token.size() - 1, 1);
							this->server_Block[server_Block_Idx]
								.loca[LocaIdx]
								.Return.second = token;
						} else {
							while (ss >> token) {
								if (token[token.size() - 1] == ';')
									break;
							}
						}
						continue;
					}
				}
			}
			if (token == "server_name") {
				while (ss >> token) {
					if (token[token.size() - 1] == ';') {
						token.erase(token.size() - 1, 1);
						this->server_Block[server_Block_Idx]
							.server_Name.push_back(token);
						break;
					}
					this->server_Block[server_Block_Idx].server_Name.push_back(
						token);
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
					this->server_Block[server_Block_Idx]
						.error_Page[atoi(temp[i].c_str())] = token;
				}
				continue;
			}
			if (token == "client_max_body_size") {
				ss >> token;
				if (this->server_Block[server_Block_Idx].client_Max_Body_Size ==
					1024) {
					this->server_Block[server_Block_Idx].client_Max_Body_Size =
						atoi(token.c_str());
				}
				continue;
			}
			if (token == "root") {
				ss >> token;
				token.erase(token.size() - 1, 1);
				this->server_Block[server_Block_Idx].root = token;
				continue;
			}
			if (token == "index") {
				while (ss >> token) {
					if (token[token.size() - 1] == ';')
						break ;
					this->server_Block[server_Block_Idx].index.push_back(token);
				}
				token.erase(token.size() - 1, 1);
				this->server_Block[server_Block_Idx].index.push_back(token);
				continue;
			}
			// if token == "upload"
		}
	}

	if (!server_stack.empty()) {
		std::cout << "scope err" << std::endl;
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
//

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
