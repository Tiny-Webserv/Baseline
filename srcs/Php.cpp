#include "Php.hpp"
#include "Response.hpp"
#include "get_next_line.h"
// #include <sstream>

void PhpStart(struct kevent *curEvnts, std::vector<struct kevent> &_ChangeList,
              std::map<int, Request *> &_cli,
              std::map<int, std::pair<int, int> > &_cgi) {
    (void)curEvnts;
    int parentWrite[2];
    int childWrite[2];
    // char buf[1024];
    int pid;

    if (pipe(parentWrite) == -1 || pipe(childWrite) == -1) {
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

    std::map<std::string, std::string> _envMap =
        PhpEnvSet(curEvnts, _cli, _cgi);
    std::map<std::string, std::string>::iterator iti, its;
    iti = _envMap.begin();
    its = _envMap.end();
    ;
    std::cout << "==============env==============" << std::endl;
    for (; iti != its; iti++) {
        std::cout << iti->first << " : " << iti->second << std::endl;
    }
    std::cout << "==============env==============" << std::endl;
    // char *env[] = {
    //     "REQUEST_METHOD=POST",
    //     "CONTENT_LENGTH=17", // NAME test AGE 123
    //     "CONTENT_TYPE=application/x-www-form-urlencoded",
    //     "SCRIPT_FILENAME=/Users/jang-insu/42seoul/webserv/html/post/index.php",
    //     "REDIRECT_STATUS=200",
    //     // "PHP_SELF=/Users/jang-insu/webservTest/html/index.php",
    //     "GATEWAY_INTERFACE=CGI/1.1", "SERVER_PROTOCOL=HTTP/1.1",
    //     "SCRIPT_NAME=/Users/jang-insu/42seoul/webserv/html/post/index.php",
    //     "REQUEST_URI=/index.php",
    //     "DOCUMENT_ROOT=/Users/jang-insu/42seoul/webserv/html/post",
    //     // "SERVER_ADDR=127.0.0.1",
    //     // "SERVER_PORT=80",
    //     // "SERVER_NAME=localhost",
    //     NULL};
    ///////////////////////////////
    char *env[] = {
        "GATEWAY_INTERFACE=CGI/1.1",
        "SERVER_PROTOCOL=HTTP/1.1", /// gateway, protocol is const!
        "CONTENT_TYPE=application/x-www-form-urlencoded", // const same!
        (char *)_envMap["REQUEST_METHOD"].c_str(),
        (char *)_envMap["CONTENT_LENGTH"].c_str(),
        // _envMap["CONTENT_TYPE"].c_str() ,
        (char *)_envMap["SCRIPT_FILENAME"].c_str(),
        (char *)_envMap["REDIRECT_STATUS"].c_str(),
        // "REDIRECT_STATUS=200",
        (char *)_envMap["SCRIPT_NAME"].c_str(),
        (char *)_envMap["REQUEST_URI"].c_str(),
        (char *)_envMap["DOCUMENT_ROOT"].c_str(), NULL};
    /////////////////////////////
    pid = fork();
    // char *env[];

    if (pid == 0) // child
    {
        close(parentWrite[1]);
        close(childWrite[0]);
        dup2(parentWrite[0], 0);
        close(parentWrite[0]);
        dup2(childWrite[1], 1);
        close(childWrite[1]);
        // chdir("/Users/jang-insu/webservTest/html"); ////
        char *arg[] = {"/opt/homebrew/bin/php-cgi", "./html/post/index.php",
                       NULL};

        // std::cout << "=======in child!!!!!======" << std::endl;
        execve(arg[0], arg, env);
        std::cerr << "========이까지 오냐?===========" << std::endl;
        std::cerr << "exe err" << std::endl;
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
        _cgi[pid].first = curEvnts->ident;
        _cgi[pid].second = childWrite[0];
    }
    //     X-Powered-By: PHP/8.2.3
    // Content-type: text/html; charset=UTF-8
    // Name: test<br>Age: 123<br>
}

//////////////////////////////caution!!!!!!!//////////////////////////////
//<pid, [소켓, 자식 파이프 읽기 fd]>
//////////////////////////////caution!!!!!!!//////////////////////////////

std::string generatephpStatusLine(Request *_request) {
    std::stringstream ss;

    ss << "HTTP/1.1 " << _request->GetErrorCode() << ' '
       << _request->GetErrorMessages().substr(
              0, _request->GetErrorMessages().find(" : "))
       << CRLF;
    std::string line = ss.str();
    return (line);
}

void generatePhpBody(std::string result, std::string &_header,
                     std::string &body) {
    size_t len = result.find("/r/n/r/n");
    if (len == std::string::npos) {
        std::cout << "npos result" << std::endl;
        return;
    }
    size_t i = 0;
    while (i + 1 < len) {
        _header += result[i];
        i++;
    }
    _header += "\r\n\r\n";
    i += 4;
    while (result[i]) {
        body += result[i];
        i++;
    }
    body += "\r\n\r\n";
}

void generatePhpHeader(std::string &header, std::string &body) {
    std::cout << "==============bodyphp===========" << std::endl;
    std::stringstream ss;
    ss << "Server: Webserv" << CRLF;
    std::cout << body << std::endl;
    std::cout << "==============bodyphp===========" << std::endl;

    if (body.size()) {
        ss << "Content-Type: "
           << "text/html" << CRLF;
        ss << "Content-Length: " << body.size() - 4 << CRLF;
        // }
        // if (_request->GetErrorCode() == NotAllowed) {
        // 	std::vector<std::string> limit_except =
        // getLocationBlock().GetLimitExcept(); 	ss << "Allow: ";
        // for (size_t i = 0; i < limit_except.size(); ++i) { 		if (i)
        // ss << ", "; 		ss << limit_except[i];
        // 	}
        // 	ss << CRLF;
        // }
        // ss << CRLF;
    }
    header = ss.str() + header;
}
std::vector<char> PhpResult(struct kevent *curEvnts,
                            std::vector<struct kevent> &_ChangeList,
                            std::map<int, Request *> &_cli,
                            std::map<int, std::pair<int, int> > &_cgi) {
    struct kevent tmpEvnt;
    EV_SET(&tmpEvnt, curEvnts->ident, EVFILT_PROC, EV_DELETE, 0, 0,
           curEvnts->udata);
    _ChangeList.push_back(tmpEvnt);
    // map
    waitpid(curEvnts->ident, 0, WNOHANG); // option

    std::stringstream ss;
    std::string startLine;
    std::cout << "test1?" << std::endl;
    startLine = generatephpStatusLine(_cli[_cgi[curEvnts->ident].first]);

    char buf[1024];
    int read_size = 1;
    while (true) {
        read_size = read(_cgi[curEvnts->ident].second, buf, 1024);
        if (read_size == 0) {
            //
            break;
        } else if (read_size == -1) {
            //
            break;
        }
        buf[read_size] = 0;
        ss << buf;
    }

    std::string result = ss.str();
    std::cout << "rererererere" << std::endl;
    std::cout << result << std::endl;
    std::cout << "rererererere" << std::endl;

    std::string body = "";
    std::string header = "";

    std::cout << "test2?" << std::endl;
    generatePhpBody(result, header, body);
    std::cout << "test3?" << std::endl;

    generatePhpHeader(header, body);

    std::string responMsg = startLine + header + body;
    std::vector<char> res(responMsg.begin(), responMsg.end());
    close(_cgi[curEvnts->ident].second);

    EV_SET(&tmpEvnt, _cgi[curEvnts->ident].first, EVFILT_WRITE, EV_ADD, 0, 0,
           curEvnts->udata);
    _ChangeList.push_back(tmpEvnt);
    _cgi.erase(curEvnts->ident);
    return (res);
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

std::map<std::string, std::string>
PhpEnvSet(struct kevent *curEvnts, std::map<int, Request *> &_cli,
          std::map<int, std::pair<int, int> > &_cgi) {
    (void)_cgi;
    std::map<std::string, std::string> _envMap;
    int sock = curEvnts->ident;
    std::cout << "===================" << sock << std::endl;
    std::string _method = _cli[sock]->GetMethod();
    std::string content_length = "CONTENT_LENGTH=";
    // std::string content_type = "CONTENT_TYPE=";
    std::string script_filename = "SCRIPT_FILENAME=";
    std::string redirect_status = "REDIRECT_STATUS=";
    std::string script_name = "SCRIPT_NAME=";
    std::string request_uri = "REQUEST_URI=";
    std::string document_root = "DOCUMENT_ROOT=";
    std::string request_method = "REQUEST_METHOD=";

    std::ostringstream os;
    os << _cli[sock]->GetStream().str().size();
    content_length += os.str();
    // content_type += _cli[sock]->GetContentType();
    request_method += _method;

    //////////////////////////////////////////////////////////////////
    std::string _target =
        _cli[sock]->GetTarget(); // filename임 post요청의 target이니
                                 // index.php가 되겠다.
    std::vector<LocationBlock> location_vector;
    std::vector<LocationBlock>::iterator it;
    location_vector = _cli[sock]->GetServer().GetLocation();
    it = location_vector.begin();
    for (; it != location_vector.end(); it++) {
        if (it->GetLocationTarget() == _target)
            break;
    }
    if (it == location_vector.end()) {
        std::cout << "empty location bolock mirror" << std::endl;
    }
    script_filename += (it->GetRoot()); //./html/post
    script_name += (it->GetRoot());
    script_filename += _target; // index.php
    script_name += _target;     // index.php
    request_uri += _target;
    //////////////////////////////////////////////////////////////////
    std::string _root = document_root + it->GetRoot();
    std::cout << "doc root" << std::endl;
    std::cout << _root << std::endl;
    std::cout << "doc root" << std::endl;

    std::ostringstream os2;
    os2 << _cli[sock]->GetErrorCode();
    redirect_status += os2.str();
    _envMap["CONTENT_LENGTH"] = content_length;
    // _envMap["CONTENT_TYPE"] = (char *)content_type.c_str();
    _envMap["SCRIPT_FILENAME"] = script_filename; // locationblock으ㅣ 루루트트

    _envMap["REDIRECT_STATUS"] = redirect_status;
    _envMap["SCRIPT_NAME"] = script_name;
    _envMap["REQUEST_URI"] = request_uri;
    _envMap["DOCUMENT_ROOT"] = _root;
    std::cout << "============Key============" << std::endl;
    std::cout << _envMap["DOCUMENT_ROOT"] << std::endl;
    std::cout << "============Key============" << std::endl;
    _envMap["REQUEST_METHOD"] = request_method;
    return (_envMap);
    // std::map<std::string, char *>::iterator iti, its;
    // iti = _envMap.begin();
    // its = _envMap.end();;
    // std::cout << "==============env in f==============" << std::endl;
    // for (iti = _envMap.begin() ; iti != its ; iti++) {
    //     std::cout << iti->first << " : " << iti->second << std::endl;
    // }
    // std::cout << "==============env in f==============" << std::endl;
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
    /////////
    ////////
}