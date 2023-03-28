#include "config.hpp"
#include <iostream>
#include <string>

int main(int ac, char **av) {
	if (ac != 2) {
		std::cout << "Usage ./webserv [configuration]" << std::endl;
		exit(1);
	}
	config con(static_cast<std::string>(av[1]));

	//test
	size_t n = con.server_Block.size();
	for (size_t i = 0 ; i < n ; i++)
	{
		std::cout << "hostname : " << con.server_Block[i].listen.first << std::endl;
		std::cout << "port : " << con.server_Block[i].listen.second << std::endl;
		std::cout << "server name : " << con.server_Block[i].server_Name << std::endl;
		//std::cout << "client max body size : " << con.server_Block[i].client_Max_Body_Size << std::endl;
		std::cout << "root : " << con.server_Block[i].root << std::endl;
		//std::cout << "autoindex : " << con.server_Block[i].autoindex << std::endl;
		
		for (int j = 0; j < con.server_Block[i].loca_block_cnt; j++)
		{
			std::cout << "location target : " << con.server_Block[i].loca[j].location_Target << std::endl;
			std::cout << "location root : " << con.server_Block[i].loca[j].root << std::endl;
		}

		// std::cout << "error page : " << con.server_Block[i].error_Page[key] << std::endl;

		// for (size_t k = 0; k < 3; k++)
		// {
		// 	for (std::vector<std::string>::iterator it = con.server_Block[i].limit_Except[k].begin(); it != con.server_Block[i].limit_Except[k].end(); it++)
		// 	{
		// 		std::cout << "limit except ["<< k << "] : " << *it << std::endl;
		// 	}
		// }

		// std::cout << "return info : " << con.server_Block[i].return_Info[key] << std::endl;

		// for (std::vector<std::string>::iterator it = con.server_Block[i].index.begin(); it != con.server_Block[i].limit_Except[k].end(); it++)
		// 	std::cout<< "index : " << *it << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
	}
}