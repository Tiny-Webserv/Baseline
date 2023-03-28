#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct location_block {
	std::string location_Target;
	std::string root;
	// std::string try_Files;
	// std::string cgi경로
	location_block() {
		location_Target = "";
		root = "";
	}
};

// server_block.loca[].push_back(location_block());
struct server_block {
	std::pair<std::string, int> listen;
	std::string server_Name;
	std::map<int, std::string> error_Page;
	unsigned long client_Max_Body_Size;
	std::vector<std::string> limit_Except[3];
	std::map<int, std::string> return_Info;
	std::string root;
	bool autoindex;
	std::vector<std::string> index;
	std::vector<location_block> loca;
	int loca_block_cnt;
	server_block() {
		listen.first = "";
		listen.second = -1;
		server_Name = "";
		client_Max_Body_Size = 1024;
		root = "";
		autoindex = false;
		loca_block_cnt = 0;
		// error_page;
		// limit_Except
		// return_Info
		// loca.location_Target = "";
		//  index = "";
		//  upload
	}
	// 	if token == "location"
	// 	if token == "error_page"
	// 	if token == "client_max_body_size"
	// 	if token == "limit_except"
	// 	if token == "return"
	// 	if token == "root"
	// 	if token == "autoindex"
	// 	if token == "index"

	// 	if token == "upload"
};

class config {
  private:

  public:
	std::vector<server_block> server_Block;
	config();
	config(std::string filename);
	~config();
	std::string open_File(std::string filename);
	// int		nginx_word(std::string word);
};

#endif