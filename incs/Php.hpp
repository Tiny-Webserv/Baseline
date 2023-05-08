#ifndef PHP_HPP
#define PHP_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Socket.hpp"
#include <fcntl.h>
#include <map>
#include <unistd.h>

void PhpStart(struct kevent *curEvnts, std::vector<struct kevent> &_ChangeList,
              std::map<int, Request *> &_cli, std::map<int, int[2]> &_cgi);
bool IsPhp(Request *reque);
void PhpResult(struct kevent *curEvnts, std::vector<struct kevent> &_ChangeList,
               std::map<int, Request *> &_cli, std::map<int, int[2]> &_cgi);

#endif
