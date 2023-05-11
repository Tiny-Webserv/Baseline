#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include "Config.hpp"
#include "Php.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Socket.hpp"
#include <array>
#include <fcntl.h>
#include <map>
#include <unistd.h>

class EventLoop {
  private:
    int _kqFd;
    std::vector<struct kevent> _ChangeList;

    std::map<int, Request *> _cli;
    std::map<int, std::string> _request;
    std::map<int, Response *> _response2;
    std::map<int, int> _offset;
    std::map<int, std::vector<char> > _cgiResponse;
    std::map<std::string, std::string> _html;
    std::map<int, std::pair<int, int> > _cgi;
    std::string _phpheader;
    std::vector<ServerBlock> _server;
    EventLoop();
    void OpenHtmlFile(std::string filePath);

  public:
    EventLoop(Config &con);
    ~EventLoop();
    void MakeHtmlFile(Config &con);
    void EventHandler();
    void HandleRequest(struct kevent *curEvnts);
    void MakeResponse(struct kevent *curEvnts);
    void SendResponse(struct kevent *curEvnts);
    void EraseMemberMap(int key);
};

#endif
