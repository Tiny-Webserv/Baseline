#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Socket.hpp"
#include <fcntl.h>
#include <unistd.h>

#include <map>

class EventLoop {
  private:
    int _kqFd;
    std::vector<struct kevent> _ChangeList;

    std::map<int, Request *> _cli;
    std::map<int, std::string> _request;
    std::map<int, std::string> _response;
    std::map<std::string, std::string> _html;
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
    void PhStart(struct kevent *curEvnts);
};

#endif
