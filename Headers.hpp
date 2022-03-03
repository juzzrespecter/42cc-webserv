#ifndef HEADER_HPP
#define HEADER_HPP

#include <string>
#define N_HEADER_MAX 37

/* to fix */

namespace header {
    std::string hlist[N_HEADER_MAX] = {
        "A-IM", "Accept", "Accept-Charset", "Accept-Encoding",
        "Accept-Language", "Accept-Datetime", "Access-Control-Request-Method", "Access-Control-Request-Headers",
        "Authorization", "Cache-Control", "Connection", "Content-Length",
        "Content-Type", "Cookie", "Date", "Expect",
        "Forwarded", "From", "Host", "If-Match",
        "If-Modified-Since", "If-None-Match", "If-Range", "If-Unmodified-Since",
        "Max-Forwards", "Origin", "Pragma", "Proxy-Authorization",
        "Range", "Referer", "TE", "Trailer",
        "Transfer-Encoding", "User-Agent", "Upgrade", "Via", "Warning"
    };
}

#endif // HEADER_HPP