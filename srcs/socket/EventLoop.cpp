#include <iostream>

#include "EventLoop.hpp"

EventLoop::EventLoop(Config &con) {
    Socket sock(con);
    this->_server = con._ServerBlockObject;
    this->_kqFd = kqueue();
    this->_ChangeList = sock.GetChangeList();
    EventHandler();
}

void EventLoop::EventHandler() {
    int servcnt = this->_ChangeList.size(); // this->추가?
    struct kevent evntLst[16];
    struct kevent *curEvnts;
    int newEvnts;
    while (1) {
        newEvnts = kevent(_kqFd, &_ChangeList[0], _ChangeList.size(), evntLst,
                          16, NULL);
        _ChangeList.clear();
        for (int i = 0; i < newEvnts; i++) {
            std::cout
                << "========================================================"
                << std::endl;
            curEvnts = &evntLst[i];
            if (curEvnts->filter == EVFILT_READ) {
                if (curEvnts->ident < (uintptr_t)servcnt + 3) { // 서버 소켓?
                    int clnt_fd;
                    if (curEvnts->udata != NULL) {
                        ServerBlock *sptr =
                            static_cast<ServerBlock *>(curEvnts->udata);
                        clnt_fd = accept(curEvnts->ident, 0, 0);
                        struct linger linger_opt;
                        linger_opt.l_onoff = 1;
                        linger_opt.l_linger = 0; // TIME_WAIT 상태를 0초로 설정
                        if (setsockopt(clnt_fd, SOL_SOCKET, SO_LINGER,
                                       &linger_opt, sizeof(linger_opt)) < 0) {
                            std::cout << "setsockopt error" << std::endl;
                        }
                        int sigpipe_opt = 1;
                        if (setsockopt(clnt_fd, SOL_SOCKET, SO_NOSIGPIPE,
                                       &sigpipe_opt, sizeof(sigpipe_opt)) < 0) {
                            std::cout << "setsockopt error" << std::endl;
                        }
                        std::cout << sptr->GetPort() << "에 새 클라이언트("
                                  << clnt_fd << ") 연결" << std::endl;
                        fcntl(clnt_fd, F_SETFL, O_NONBLOCK);
                        struct kevent tmpEvnt;
                        EV_SET(&tmpEvnt, clnt_fd, EVFILT_READ,
                               EV_ADD | EV_ENABLE, 0, 0, curEvnts->udata);
                        _ChangeList.push_back(tmpEvnt);
                    } else
                        std::cout << "이상한 연결 요청" << std::endl;
                } else {
                    std::cout << "클라이언트(" << curEvnts->ident
                              << ")으로부터 request 들어옴" << std::endl;
                    HandleRequest(curEvnts);
                }
            } else if (curEvnts->filter == EVFILT_WRITE) {
                std::cout << "클라이언트(" << curEvnts->ident
                          << ")에게 Write 이벤트 발생" << std::endl;
                if (this->_response2.find(curEvnts->ident) ==
                    this->_response2.end()) {
                    std::cout << "send failed non exist response" << std::endl;
                    close(curEvnts->ident);
                    EraseMemberMap(curEvnts->ident);
                } else
                    SendResponse(curEvnts);
            } else if (curEvnts->filter == EVFILT_PROC) {
                std::cout << Response::getCGI()[curEvnts->ident][0]
                          << " cgi 호출 끝" << std::endl;
                int sock_fd = Response::getCGI()[curEvnts->ident][0];
                _response2[sock_fd]->generatePhpResponse(curEvnts, _ChangeList);
                // if (Response::getCGI()[curEvnts->ident].size()) {
                // _cgiResponse[sock_fd] =
                // _response2[sock_fd]->getResponseMessage();
                // }
            } else {
                std::cout << curEvnts->ident << "번 알 수 없는 이벤트 필터("
                          << curEvnts->filter << ") 발생" << std::endl;
                close(curEvnts->ident);
                EraseMemberMap(curEvnts->ident);
            }
        }
    }
}

void EventLoop::HandleRequest(struct kevent *curEvnts) {
    Request *reque = ParseRequest(curEvnts->ident, this->_cli, _server);
    if (reque == NULL) {
        std::cout << curEvnts->ident << "로 부터 fin 요청!" << std::endl;
        EraseMemberMap(curEvnts->ident);
        close(curEvnts->ident);
        return;
    }
    MakeResponse(curEvnts);
}

void EventLoop::MakeResponse(struct kevent *curEvnts) {
    int error;
    socklen_t error_len = sizeof(error);
    int ret =
        getsockopt(curEvnts->ident, SOL_SOCKET, SO_ERROR, &error, &error_len);
    if (ret == -1 || error != 0) {
        // Socket is dead, close it and return
        close(curEvnts->ident);
        EraseMemberMap(curEvnts->ident);
        std::cout << "이미 뒤진 소켓 MakeResponse 에서 발견되다" << std::endl;
        return;
    }
    std::cout << curEvnts->ident << " 에게 보낼 응답 msg 생성 " << std::endl;
    if (_cli.find(curEvnts->ident) == _cli.end()) {
        std::cout << "dlfjsruddnrk dlTdmfRk??" << std::endl;
        close(curEvnts->ident);
        EraseMemberMap(curEvnts->ident);
        return;
    }
    if (_response2.find(curEvnts->ident) != _response2.end())
        delete (_response2[curEvnts->ident]);
    Request *reque = this->_cli[curEvnts->ident];
    if (IsPhp(reque)) {
        // PhpStart(curEvnts, _ChangeList, this->_cli, _cgi);
        std::cout << reque->GetTarget() << std::endl;
        this->_response2[curEvnts->ident] =
            new Response(reque, curEvnts, _ChangeList);
        if (this->_response2[curEvnts->ident]->isDone()) {
            std::cout << "php failed" << std::endl;
            struct kevent tmpEvnt;
            EV_SET(&tmpEvnt, curEvnts->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE,
                   0, 0, curEvnts->udata);
            _ChangeList.push_back(tmpEvnt);
        }
        std::cout << "start end" << std::endl;

        // this->_response2[curEvnts->ident] = new Response(reque,
        // _ChangeList);
    } else {
        this->_response2[curEvnts->ident] = new Response(reque);
        struct kevent tmpEvnt;
        EV_SET(&tmpEvnt, curEvnts->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0,
               0, curEvnts->udata);
        _ChangeList.push_back(tmpEvnt);
    }
}

void EventLoop::SendResponse(struct kevent *curEvnts) {
    int error;
    socklen_t error_len = sizeof(error);
    int ret =
        getsockopt(curEvnts->ident, SOL_SOCKET, SO_ERROR, &error, &error_len);
    if (ret == -1 || error != 0) {
        // Socket is dead, close it and return
        close(curEvnts->ident);
        EraseMemberMap(curEvnts->ident);
        std::cout << "이미 뒤진 소켓 SendResponse 에서 발견되다" << std::endl;
        return;
    }
    std::cout << curEvnts->ident << " 에게 응답 msg 전송 " << std::endl;
    // std::vector<char>::iterator it, its;
    // it = _response2[curEvnts->ident]->getResponseMessage().begin();
    // its = _response2[curEvnts->ident]->getResponseMessage().end();
    // std::string response_str(it + _offset[curEvnts->ident], its);
    //  std::string response_str = this->_response[curEvnts->ident];
    // int response_size = response_str.size();
    std::vector<char> resMsg;
    // if (IsPhp(_cli[curEvnts->ident]) ||
    // !_response2[curEvnts->ident]->isDone()) {
    //     resMsg =  _response2[curEvnts->ident]->getResponseMessage();
    // } else
    resMsg = _response2[curEvnts->ident]->getResponseMessage();
    int res = send(curEvnts->ident, &resMsg[_offset[curEvnts->ident]],
                   resMsg.size() - _offset[curEvnts->ident], 0);
    if (res > 0) {
        std::cout << curEvnts->ident << "번 다시 보내" << std::endl;
        _offset[curEvnts->ident] += res;
        // this->_response[curEvnts->ident].erase(0, res);
        return;
    }
    if (res == -1) {
        std::cout << "Write fail" << '\n';
        EraseMemberMap(curEvnts->ident);
        close(curEvnts->ident);
        return;
    } else {
        std::cout << "전송 완료 해쓰여 " << std::endl;
    }
    std::cout << "Event Delete" << std::endl;
    struct kevent tmpEvnt;
    EV_SET(&tmpEvnt, curEvnts->ident, EVFILT_WRITE, EV_DELETE, 0, 0,
           curEvnts->udata);
    _ChangeList.push_back(tmpEvnt);
    EraseMemberMap(curEvnts->ident);
}

void EventLoop::EraseMemberMap(int key) {
    delete (this->_cli[key]);
    delete (this->_response2[key]);
    this->_cli.erase(key);
    this->_response2.erase(key);
    this->_offset.erase(key);
    // this->_cgiResponse.erase(key);
    /* feature/phpResponse
                                    //_cgi[pid] = std::vector<int>[0], key,
     child_pipe
                                    // _zz[key] = pid;

     develop*/
}

EventLoop::EventLoop() {}

EventLoop::~EventLoop() {}
