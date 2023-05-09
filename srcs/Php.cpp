#include "Php.hpp"
// #include <sstream>

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
    std::string body;
    if (method == "GET" || body.find('?') != std::string::npos) {
        body = _cli[curEvnts->ident]->GetTarget();
        body.erase(0, body.find("?") + 1);
        // "REQUEST_METHOD=GET"
    } else if (method == "POST") {
        body = _cli[curEvnts->ident]->GetStream().str();
        // "REQUEST_METHOD=POST"
    }
    // else {} //"DELETE" 안쓰일듯?

    body += "\r\n\r\n"; // event_Loop

    //////////////////nhwang memo 5.9//////////////////
    // 아래의 msg는 body로 바꿔준다.
    // body+="\r\n\r\n" 을 해줘야하는지 실험해보고 싶음 - event_Loop
    // 브랜치에서는 해주고 있음. Get 일때 쿼리스트링도 환경변수로 줘야함 >>
    // 안줘도 될지도.... body?
    //////////////////nhwang memo 5.9//////////////////
    //\r\n\r\n 추가

    pid = fork();
    // char *env[];
    
    std::map<std::string, char*>_envMap;



    PhpEnvSet(_envMap);
    _envMap ={ "REQUEST_METHOD" : "REQUEST_METHOD=POST"}
    char *env[] = {
        (char*)_envMap["REQUEST_METHOD"];
        "REQUEST_METHOD=POST",
        "CONTENT_LENGTH=17", // NAME test AGE 123
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
        write(parentWrite[1], body.c_str(),
              body.length()); // msg를 body로 바꿨음
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
    // get일때 본문이 있으면 url 로 들어와서 짤라줌
    if (reque->GetMethod() == "GET" && target.find('?') != std::string::npos)
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

void PhpEnvSet(struct kevent *curEvnts, std::map<std::string, char *> &_envMap, std::map<int, Request *> &_cli, std::map<int, int[2]> &_cgi) {
    // _envMap["REQUEST_METHOD"] = ;
    int sock = _cgi[curEvnts->ident][0];
    std::string content_length = "CONTENTLENGTH=";
    std::string content_type = "CONTENT_TYPE=";
    std::string script_filename = "SCRIPT_FILENAME=";
    std::string redirect_status = "REDIRECT_STATUS=";
    std::string script_name = "SCRIPT_NAME=";
    std::string request_uri = "REQUEST_URI=";
    std::string document_root = "DOCUMENT_ROOT=";

    std::ostringstream os;
    os << _cli[sock]->GetStream().str().size();
    content_length += os.str();
    content_type += _cli[sock]->GetContentType();
    // script_filename += 
    os << _cli[sock]->GetErrorCode();
    redirect_status += os.str();
    _envMap["CONTENT_LENGTH"] = (char *)content_length.c_str();
    _envMap["CONTENT_TYPE"] = (char *)content_type.c_str();
    // _envMap["SCRIPT_FILENAME"] = (char *)_cli[sock]->GetTarget().c_str();
    _envMap["REDIRECT_STATUS"] = (char *)redirect_status.c_str();
    // _envMap["SCRIPT_NAME"] = (char *)
    // _envMap["REQUEST_URI"] = (char *)
    // _envMap["DOCUMENT_ROOT"] = (char *)


//     env {"REQUEST_METHOD=POST",
//         "CONTENT_LENGTH=17", // NAME test AGE 123
//         "CONTENT_TYPE=application/x-www-form-urlencoded",
//         "SCRIPT_FILENAME=/Users/jang-insu/42seoul/webserv/html/post/index.php",
//         "REDIRECT_STATUS=200",
//         // "PHP_SELF=/Users/jang-insu/webservTest/html/index.php",
//         "GATEWAY_INTERFACE=CGI/1.1", "SERVER_PROTOCOL=HTTP/1.1",
//         "SCRIPT_NAME=/Users/jang-insu/42seoul/webserv/html/post/index.php",
//         "REQUEST_URI=/index.php",
//         "DOCUMENT_ROOT=/Users/jang-insu/42seoul/webserv/html/post",
//         // "SERVER_ADDR=127.0.0.1",
//         // "SERVER_PORT=80",
//         // "SERVER_NAME=localhost",
//         NULL
// };
}
