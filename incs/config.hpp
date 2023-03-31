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
    std::string _LocationTarget;
    std::string _Root;
    std::string UploadPass;
    std::pair<int, std::string> Return;
    std::vector<std::string> limit_Except;
    std::vector<std::string> index;
    int autoindex; ///-1,0,1일 경우 초기화 -1로 해야함. 구현부에서 처리(1)만
    // std::string try_Files;
    // std::string cgi경로
    location_block() {
        UploadPass = "";
        _LocationTarget = "";
        _Root = "";
        autoindex = -1;
    }
};

// server_block.loca[].push_back(location_block());
struct server_block {
    int port;
    std::vector<std::string> server_Name;
    std::map<int, std::string> error_Page;
    unsigned long client_Max_Body_Size;
    std::map<int, std::string> return_Info;
    std::string _Root;
    std::vector<std::string> index;
    std::vector<location_block> loca;
    int loca_block_cnt;
    server_block() {
        port = -1;
        client_Max_Body_Size = 1024;
        _Root = "";
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

class Config {
  private:
  public:
    std::vector<server_block> server_Block;
    Config();
    Config(std::string filename);
    ~Config();
    std::string open_File(std::string filename);
    // int		nginx_word(std::string word);
};

#endif