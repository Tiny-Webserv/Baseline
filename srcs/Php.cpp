#include "Php.hpp"

void PhpStart(struct kevent *curEvnts, std::vector<struct kevent> &_ChangeList,
              std::map<int, Request *> &_cli, std::map<int, int[2]> &_cgi) {
    (void)curEvnts;
    int parentWrite[2];
    int childWrite[2];
    char buf[1024];
    int pid;

    if (pipe(parentWrite) == -1) {
        perror("pipe");
        return;
    }
    std::string method = _cli[curEvnts->ident]->GetMethod();
	std::string body = _cli[curEvnts->ident]->GetTarget();
    if (method == "GET")
	{
		body.erase(0, body.find("?") + 1);
		// "REQUEST_METHOD=GET"
	}
    else if (method == "POST")
	{
		// "REQUEST_METHOD=POST"
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
        struct kevent tmpEvnt;
        EV_SET(&tmpEvnt, pid, EVFILT_PROC, EV_ADD | EV_ENABLE, NOTE_EXIT, 0,
               curEvnts->udata);
        _ChangeList.push_back(tmpEvnt);
        _cgi[pid][0] = curEvnts->ident;
        _cgi[pid][1] = childWrite[0];
    }
    //     X-Powered-By: PHP/8.2.3
    // Content-type: text/html; charset=UTF-8

    // Name: test<br>Age: 123<br>
}

//<pid, [소켓, 자식 파이프 읽기 fd]>

void PhpResult(struct kevent *curEvnts, std::vector<struct kevent> &_ChangeList,
               std::map<int, Request *> &_cli, std::map<int, int[2]> &_cgi) {
    struct kevent tmpEvnt;
    EV_SET(&tmpEvnt, curEvnts->ident, EVFILT_PROC, EV_DELETE, 0, 0,
           curEvnts->udata);
    _ChangeList.push_back(tmpEvnt);
    // map
    waitpid(curEvnts->ident, 0, 0); // option
    char buf[1024];
    read(_cgi[curEvnts->ident][1], buf, 1024);

    // read(childWrite[0], buf, 1024);
    close(_cgi[curEvnts->ident][1]);
    // response(buf) 생성
    // {
    // header붙히기
    // send (clnt_sock, buf, bufsize())
    // }
    // _response[curEvnts->ident] = new Response(???);
    struct kevent tmpEvnt;
    EV_SET(&tmpEvnt, _cgi[curEvnts->ident][0], EVFILT_WRITE, EV_ADD, 0, 0,
           curEvnts->udata);
    _ChangeList.push_back(tmpEvnt);
}

bool IsPhp(Request *reque) {
    std::string target = reque->GetTarget();
    if (reque->GetMethod() == "GET")
    	target.erase(target.find('?'), target.length()); // length 실험중
    if (target.find(".php") == target.length() - 4) {
        size_t size = reque->GetServer().GetLocation().size();
        for (size_t i = 0; i < size; i++) {
            std::string locationTarget =
                reque->GetServer().GetLocation()[i].GetLocationTarget();
            if (!strncmp(locationTarget.c_str(), target.c_str(),
                         locationTarget.size()))
                return (true);
        }
    }
    return false;
}