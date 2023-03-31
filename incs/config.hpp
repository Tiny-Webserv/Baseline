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
    std::string _UploadPass;
    std::pair<int, std::string> _Return;
    std::vector<std::string> _LimitExcept;
    std::vector<std::string> _Index;
    int _AutoIndex; ///-1,0,1일 경우 초기화 -1로 해야함. 구현부에서 처리(1)만
    // std::string try_Files;
    // std::string cgi경로
    location_block() {
        _UploadPass = "";
        _LocationTarget = "";
        _Root = "";
        _AutoIndex = -1;
    }
};

// server_block.loca[].push_back(location_block());
struct server_block {
    int _Port;
    std::vector<std::string> _ServerName;
    std::map<int, std::string> _ErrorPage;
    unsigned long _ClientMaxBodySize;
    std::string _Root;
    std::vector<std::string> _Index;
    std::vector<location_block> _Location;
    int _LocationBlockSize;
    server_block() {
        _Port = -1;
        _ClientMaxBodySize = 1024;
        _Root = "";
        _LocationBlockSize = 0;
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