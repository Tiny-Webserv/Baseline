#include "Config.hpp"
#include <iostream>
#include <string>

int main(int ac, char **av) {
    if (ac != 2) {
        std::cout << "Usage ./webserv [Configuration]" << std::endl;
        exit(1);
    }
    Config con(static_cast<std::string>(av[1]));

    // test
    size_t n = con.server_Block.size();

    // server 블록 순회
    for (size_t i = 0; i < n; i++) {
        // port 체크
        std::cout << "port : " << con.server_Block[i]._Port << std::endl;

        // server_name 체크
        size_t n2 = con.server_Block[i]._ServerName.size();
        for (size_t j = 0; j < n2; j++)
            std::cout << "server name : " << con.server_Block[i]._ServerName[j]
                      << std::endl;

        // root 체크
        std::cout << "root : " << con.server_Block[i]._Root << std::endl;

        // index 체크
        n2 = con.server_Block[i]._Index.size();
        for (size_t j = 0; j < n2; j++) {
            std::cout << "index: " << con.server_Block[i]._Index[j]
                      << std::endl;
        }

        // error_page 체크
        if (!con.server_Block[i]._ErrorPage.empty()) {
            std::map<int, std::string>::iterator it, its;
            it = con.server_Block[i]._ErrorPage.begin();
            its = con.server_Block[i]._ErrorPage.end();
            std::cout << "error_page" << std::endl;
            for (; it != its; it++) {
                std::cout << it->first << " = " << it->second << std::endl;
            }
        }
        // max_body_size 체크
        std::cout << "max body size : ";
        std::cout << con.server_Block[i]._ClientMaxBodySize << std::endl;

        // location 블록 체크
        for (int j = 0; j < con.server_Block[i]._LocationBlockSize; j++) {
            std::cout << "========= " << j + 1
                      << " 번째 location block=============" << std::endl;
            // location index 체크
            n2 = con.server_Block[i]._Location[j]._Index.size();
            for (size_t k = 0; k < n2; k++) {
                std::cout << "index: "
                          << con.server_Block[i]._Location[j]._Index[k]
                          << std::endl;
            }
            std::cout << "upload_pass : "
                      << con.server_Block[i]._Location[j]._UploadPass
                      << std::endl;
            // location target check
            std::cout << "target : "
                      << con.server_Block[i]._Location[j]._LocationTarget
                      << std::endl;
            std::cout << "root : " << con.server_Block[i]._Location[j]._Root
                      << std::endl;
            // limit_except 체크
            int k2 = con.server_Block[i]._Location[j]._LimitExcept.size();
            if (k2 != 0) {
                std::cout << "limit_except 목록 : ";
                for (int k = 0; k < k2; k++) {
                    std::cout
                        << con.server_Block[i]._Location[j]._LimitExcept[k]
                        << " ";
                }
            }
            std::cout << std::endl;
            // autoindex 체크
            std::cout << "autoindex : ";
            if (con.server_Block[i]._Location[j]._AutoIndex == 1)
                std::cout << "true" << std::endl;
            else
                std::cout << "false" << std::endl;

            // return 체크
            // std::cout << "===================return=============" <<
            // std::endl;
            std::cout << "return : code = "
                      << con.server_Block[i]._Location[j]._Return.first;
            std::cout << " uri  = "
                      << con.server_Block[i]._Location[j]._Return.second
                      << std::endl;

            std::cout << std::endl;
            std::cout << std::endl;
        }

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
