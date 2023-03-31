#ifndef STATECODE_HPP
# define STATECODE_HPP

enum StateCode {

    // 200 ~
    OK = 200,

    // 300 ~

    // 400 ~
    BadRequest = 400,
    Forbidden = 403,
    NotFound = 404,
    PayloadTooLarge = 413,

    // 500 ~
    NotImplemented = 501,
    BadGateway = 502,
    GatewayTimeout = 504,
    HTTPVersionNotSupported = 505,
};

#endif