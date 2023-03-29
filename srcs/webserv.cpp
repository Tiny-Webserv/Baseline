#include "config.hpp"
#include <iostream>
#include <string>

int main(int ac, char **av) {
	if (ac != 2) {
		std::cout << "Usage ./webserv [configuration]" << std::endl;
		exit(1);
	}
	config con(static_cast<std::string>(av[1]));

	// test
	size_t n = con.server_Block.size();

	// server 블록 순회
	for (size_t i = 0; i < n; i++) {
		// port 체크
		std::cout << "port : " << con.server_Block[i].port << std::endl;

		// server_name 체크
		size_t n2 = con.server_Block[i].server_Name.size();
		for (size_t j = 0; j < n2; j++)
			std::cout << "server name : " << con.server_Block[i].server_Name[j]
					  << std::endl;
		std::cout << "================" << std::endl;

		// root 체크
		std::cout << "root : " << con.server_Block[i].root << std::endl;

		// index 체크
		n2 = con.server_Block[i].index.size();
		for (size_t j = 0; j < n2; j++) {
			std::cout << "index: " << con.server_Block[i].index[j] << std::endl;
		}

		// error_page 체크
		if (!con.server_Block[i].error_Page.empty()) {
			std::map<int, std::string>::iterator it, its;
			it = con.server_Block[i].error_Page.begin();
			its = con.server_Block[i].error_Page.end();
			std::cout << "error_page================" << std::endl;
			for (; it != its; it++) {
				std::cout << it->first << " = " << it->second << std::endl;
			}
			std::cout << "error_page end============" << std::endl;
		}
		std::cout << std::endl;

		// location 블록 체크
		for (int j = 0; j < con.server_Block[i].loca_block_cnt; j++) {
			std::cout << "========= " << j + 1
					  << " 번째 location block=============" << std::endl;
			// location target check
			std::cout << "target : "
					  << con.server_Block[i].loca[j].location_Target
					  << std::endl;
			std::cout << "root : " << con.server_Block[i].loca[j].root
					  << std::endl;

			// limit_except 체크
			int k2 = con.server_Block[i].loca[j].limit_Except.size();
			if (k2 != 0)
				std::cout << "limit_except 목록 : ";
			for (int k = 0; k < k2; k++) {
				std::cout << con.server_Block[i].loca[j].limit_Except[k] << " ";
			}

			// autoindex 체크
			std::cout << "============auto index=============" << std::endl;
			std::cout << "autoindex : ";
			if (con.server_Block[i].loca[j].autoindex == 1)
				std::cout << "true" << std::endl;
			else
				std::cout << "false" << std::endl;
			std::cout << "===================================" << std::endl;

			// return 체크
			std::cout << "===================return=============" << std::endl;
			std::cout << "return : code = "
					  << con.server_Block[i].loca[j].Return.first << std::endl;
			std::cout << "return : uri  = "
					  << con.server_Block[i].loca[j].Return.second << std::endl;

			std::cout << std::endl;
			std::cout << "========= " << j + 1
					  << " 번째 location block 끝===========" << std::endl;
			std::cout << std::endl;
		}

		// max_body_size 체크
		std::cout << "=====max body size=====" << std::endl;
		std::cout << con.server_Block[i].client_Max_Body_Size << std::endl;
		std::cout << "=======================" << std::endl;

		// std::cout << "return info : " << con.server_Block[i].return_Info[key]
		// << std::endl;

		// for (std::vector<std::string>::iterator it =
		// con.server_Block[i].index.begin(); it !=
		// con.server_Block[i].limit_Except[k].end(); it++) 	std::cout<<
		// "index : " << *it << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
	}
}
