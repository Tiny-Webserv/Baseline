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
              std::map<int, Request *> &_cli);
bool IsPhp(Request *reque);
void PhpResult(struct kevent *curEvnts, std::vector<struct kevent> &_ChangeList,
               std::map<int, Request *> &_cli);
void PhpEnvSet(struct kevent *curEvnts, std::map<std::string, char*> &_envMap, std::map<int, Request *> &_cli);
#endif
