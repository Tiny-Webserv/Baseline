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
    size_t n = con._ServerBlockObject.size();

    // server 블록 순회
    for (size_t i = 0; i < n; i++) {
        // port 체크
        std::cout << "port : " << con._ServerBlockObject[i].GetPort()
                  << std::endl;

        // server_name 체크
        size_t n2 = con._ServerBlockObject[i].GetServerName().size();
        for (size_t j = 0; j < n2; j++)
            std::cout << "server name : "
                      << con._ServerBlockObject[i].GetServerName()[j]
                      << std::endl;

        // root 체크
        std::cout << "root : " << con._ServerBlockObject[i].GetRoot()
                  << std::endl;

        // index 체크
        n2 = con._ServerBlockObject[i].GetIndex().size();
        for (size_t j = 0; j < n2; j++) {
            std::cout << "index: " << con._ServerBlockObject[i].GetIndex()[j]
                      << std::endl;
        }

        // error_page 체크
        if (!con._ServerBlockObject[i].GetErrorPage().empty()) {
            std::map<int, std::string>::iterator it, its;
            it = con._ServerBlockObject[i].GetErrorPage().begin();
            its = con._ServerBlockObject[i].GetErrorPage().end();
            std::cout << "error_page" << std::endl;
            for (; it != its; it++) {
                std::cout << it->first << " = " << it->second << std::endl;
            }
        }
        // max_body_size 체크
        std::cout << "max body size : ";
        std::cout << con._ServerBlockObject[i].GetClientMaxBodySize()
                  << std::endl;

        // location 블록 체크
        for (int j = 0; j < con._ServerBlockObject[i].GetLocationBlockSize();
             j++) {
            std::cout << "========= " << j + 1
                      << " 번째 location block=============" << std::endl;
            // location index 체크
            n2 = con._ServerBlockObject[i].GetLocation()[j].GetIndex().size();
            for (size_t k = 0; k < n2; k++) {
                std::cout
                    << "index: "
                    << con._ServerBlockObject[i].GetLocation()[j].GetIndex()[k]
                    << std::endl;
            }
            std::cout
                << "upload_pass : "
                << con._ServerBlockObject[i].GetLocation()[j].GetUploadPass()
                << std::endl;
            // location target check
            std::cout << "target : "
                      << con._ServerBlockObject[i]
                             .GetLocation()[j]
                             .GetLocationTarget()
                      << std::endl;
            std::cout << "root : "
                      << con._ServerBlockObject[i].GetLocation()[j].GetRoot()
                      << std::endl;
            // limit_except 체크
            int k2 = con._ServerBlockObject[i]
                         .GetLocation()[j]
                         .GetLimitExcept()
                         .size();
            if (k2 != 0) {
                std::cout << "limit_except 목록 : ";
                for (int k = 0; k < k2; k++) {
                    std::cout << con._ServerBlockObject[i]
                                     .GetLocation()[j]
                                     .GetLimitExcept()[k]
                              << " ";
                }
            }
            std::cout << std::endl;
            // autoindex 체크
            std::cout << "autoindex : ";
            if (con._ServerBlockObject[i].GetLocation()[j].GetAutoIndex() == 1)
                std::cout << "true" << std::endl;
            else
                std::cout << "false" << std::endl;

            // return 체크
            // std::cout << "===================return=============" <<
            // std::endl;
            std::cout
                << "return : code = "
                << con._ServerBlockObject[i].GetLocation()[j].GetReturn().first;
            std::cout
                << " uri  = "
                << con._ServerBlockObject[i].GetLocation()[j].GetReturn().second
                << std::endl;

            std::cout << std::endl;
            std::cout << std::endl;
        }

        // std::cout << "return info : " <<
        // con._ServerBlockObject[i].return_Info[key]
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
