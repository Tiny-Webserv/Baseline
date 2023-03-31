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
    int _AutoIndex;
    // std::string try_Files;
    // std::string cgi경로
    location_block() {
        _UploadPass = "";
        _LocationTarget = "";
        _Root = "";
        _AutoIndex = -1;
    }
};

struct ServerBlock {
    int _Port;
    std::vector<std::string> _ServerName;
    std::map<int, std::string> _ErrorPage;
    unsigned long _ClientMaxBodySize;
    std::string _Root;
    std::vector<std::string> _Index;
    std::vector<location_block> _Location;
    int _LocationBlockSize;
    ServerBlock() {
        _Port = -1;
        _ClientMaxBodySize = 1024;
        _Root = "";
        _LocationBlockSize = 0;
    }
};

class Config {
  private:
  public:
    std::vector<ServerBlock> _ServerBlockObject;
    Config();
    Config(std::string filename);
    ~Config();
    std::string open_File(std::string filename);
    // int		nginx_word(std::string word);
};

#endif