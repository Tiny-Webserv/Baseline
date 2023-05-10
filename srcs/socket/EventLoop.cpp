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
                    this->_response2.end())
                    MakeResponse(curEvnts);
                else
                    SendResponse(curEvnts);
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
    // ServerBlock *serverBlock = static_cast<ServerBlock *>(curEvnts->udata);
    Request *reque = ParseRequest(curEvnts->ident, this->_cli, _server);
    if (reque == NULL) {
		std::cout << curEvnts->ident << "로 부터 fin 요청!" << std::endl;
		EraseMemberMap(curEvnts->ident);
		close(curEvnts->ident);
        return;
	}
    struct kevent tmpEvnt;
    EV_SET(&tmpEvnt, curEvnts->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
           curEvnts->udata);
    _ChangeList.push_back(tmpEvnt);
}

void EventLoop::MakeResponse(struct kevent *curEvnts) {
    int error = 0;
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
        return;
    }
    Request *reque = this->_cli[curEvnts->ident];
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!throw before!!!!" << std::endl;
    this->_response2[curEvnts->ident] = new Response(reque);
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!throw after!!!!" << std::endl;
    // ServerBlock *serverBlock = static_cast<ServerBlock *>(curEvnts->udata);
    // std::string requestPath = reque->GetTarget();

    // if (requestPath == "/")
    //     requestPath = serverBlock->GetLocation()[0].GetRoot() + "/" +
    //                   serverBlock->GetLocation()[0].GetIndex()[0];
    // else
    //     requestPath = serverBlock->GetRoot() + requestPath;
    // if (requestPath.find(".php") == requestPath.size() - 4) {
    //     PhStart(curEvnts);
    //     EraseMemberMap(curEvnts->ident);
    //     // close(curEvnts->ident);
    //     return;
    // }
    std::string response_str;
    // std::cout << requestPath << std::endl;
    // if (_html.find(requestPath) == _html.end()) {
    //     std::ifstream file(requestPath);
    //     if (!file.is_open()) {
    //         response_str =
    //             "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    //         send(curEvnts->ident, response_str.c_str(), response_str.size(), 0);
    //     } else {
    //         std::ostringstream file_contents;
    //         file_contents << file.rdbuf();
    //         file.close();

    //         std::string response_body = file_contents.str() + "\r\n\r\n";
    //         std::string response_header =
    //             "HTTP/1.1 200 OK\r\nContent-Length: " +
    //             std::to_string(response_body.size()) + "\r\n\r\n";
    //         response_str = response_header + response_body;
    //         _html[requestPath] = response_str;
    //     }
    // }
    // this->_response[curEvnts->ident] = _html[requestPath];
    // this->_response[curEvnts->ident] = response_str;
}

void EventLoop::SendResponse(struct kevent *curEvnts) {
    int error = 0;
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
    //std::vector<char>::iterator it, its;
    //it = _response2[curEvnts->ident]->getResponseMessage().begin();
    //its = _response2[curEvnts->ident]->getResponseMessage().end();
    //std::string response_str(it + _offset[curEvnts->ident], its);
    // std::string response_str = this->_response[curEvnts->ident];
    //int response_size = response_str.size();
    std::vector<char> resMsg = _response2[curEvnts->ident]->getResponseMessage();
    int res = send(curEvnts->ident, &resMsg[_offset[curEvnts->ident]], resMsg.size() - _offset[curEvnts->ident],
                   MSG_NOSIGNAL);
    if (res > 0) {
        std::cout << curEvnts->ident << "번 다시 보내" << std::endl;
        _offset[curEvnts->ident] += res;
        // this->_response[curEvnts->ident].erase(0, res);
        return;
    }
    if (res == -1) {
        std::cout << "Write fail" << '\n';
        EraseMemberMap(curEvnts->ident);
        // delete (this->_cli[curEvnts->ident]);
        // this->_cli.erase(curEvnts->ident);
        // this->_response.erase(curEvnts->ident);
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

void EventLoop::PhStart(struct kevent *curEvnts) {
    (void)curEvnts;
    int parentWrite[2];
    int childWrite[2];
    char buf[1024];
    int pid;

    if (pipe(parentWrite) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    std::string msg = "Name=test&Age=123";
    pid = fork(); // dup2를 할 경우엔 부모단에서 parentWrite[1]로 리퀘스트
                  // 전체를 넘겨야함.
    /// 오히려 파싱하기 전에 전체 문장을 받아오는 부분에서 파싱전에 이 부분을
    /// 실행하는 게 합리적일 것 같음.
    //
    // char query[] = "QUERY_STRING=Name=test&Age=123"; //GET 전용

    char *const env[] = {
        "REQUEST_METHOD=POST", "CONTENT_LENGTH=17",
        "CONTENT_TYPE=application/x-www-form-urlencoded",
        "SCRIPT_FILENAME=/Users/jang-insu/42seoul/webserv/html/post/index.php",
        "REDIRECT_STATUS=200",
        // "PHP_SELF=/Users/jang-insu/webservTest/html/index.php",
        "GATEWAY_INTERFACE=CGI/1.1", "SERVER_PROTOCOL=HTTP/1.1",
        "SCRIPT_NAME=/Users/jang-insu/42seoul/webserv/html/post/index.php",
        "REQUEST_URI=/index.php",
        "DOCUMENT_ROOT=/Users/jang-insu/42seoul/webserv/html/post",
        // "SERVER_ADDR=127.0.0.1",
        // "SERVER_PORT=80",
        // "SERVER_NAME=localhost",
        NULL};
    if (pid == 0) // child
    {
        close(parentWrite[1]);
        close(childWrite[0]);
        dup2(parentWrite[0], 0);
        close(parentWrite[0]);
        dup2(childWrite[1], 1);
        close(childWrite[1]);
        chdir("/Users/jang-insu/webservTest/html"); ////
        char *arg[] = {"/opt/homebrew/bin/php-cgi", "index.php", NULL};

        // std::cout << "=======in child!!!!!======" << std::endl;
        execve(arg[0], arg, env);
        std::cout << "exe err" << std::endl;
        exit(-1);

    } else // Parant
    {
        close(parentWrite[0]);
        close(childWrite[1]);
        write(parentWrite[1], msg.c_str(), msg.length());
        close(parentWrite[1]);
        waitpid(pid, 0, 0);
        read(childWrite[0], buf, 1024);
        close(childWrite[0]);
    }
//     X-Powered-By: PHP/8.2.3
// Content-type: text/html; charset=UTF-8

// Name: test<br>Age: 123<br>
    // std::string response_body = file_contents.str() + "\r\n\r\n";
    // std::string response_header = "HTTP/1.1 200 OK\r\nContent-Length: " +
    //                               std::to_string(response_body.size()) +
    //                               "\r\n\r\n";
    // response_str = response_header + response_body;
    // _html[requestPath] = response_str;
}

void EventLoop::EraseMemberMap(int key) {
    delete (this->_cli[key]);
    delete (this->_response2[key]);
    this->_cli.erase(key);
    this->_response2.erase(key);
    this->_offset.erase(key);
}

EventLoop::EventLoop() {}

EventLoop::~EventLoop() {}
