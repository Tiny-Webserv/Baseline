#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerBlock.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

//struct ServerBlock {
//    int _Port;
//    std::vector<std::string> _ServerName;
//    std::map<int, std::string> _ErrorPage;
//    unsigned long _ClientMaxBodySize;
//    std::string _Root;
//    std::vector<std::string> _Index;
//    std::vector<LocationBlock> _Location;
//    int _LocationBlockSize;
//    ServerBlock() {
//        _Port = -1;
//        _ClientMaxBodySize = 1024;
//        _Root = "";
//        _LocationBlockSize = 0;
//    }
//};

class Config {
  private:
  public:
    std::vector<ServerBlock> _ServerBlockObject;
    Config();
    Config(std::string filename);
    ~Config();
    std::string OpenFile(std::string filename);
    // int		nginx_word(std::string word);
};

#endif