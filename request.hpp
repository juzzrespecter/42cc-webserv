#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__
#include <vector>
#include <string>

/* if header Connection is set to close, server must close connection after sending response */

    /* header flag identifiers */
enum request_header_f {
    /* general header fields */
    H_Cache_Control,     
    H_Connection,
    H_Date,
    H_Pragma,
    H_Trailer,
    H_Transfer_Encoding,
    H_Upgrade,
    H_Via,
    H_Warning,
    /* request header fields */
    H_Accept,
    H_Accept_Charset,
    H_Accept_Encoding,
    H_Accept_Language,
    H_Authorization,
    H_Expect,
    H_From,
    H_Host,
    H_If_Match,
    H_If_Modified_Since,
    H_If_None_Match,
    H_If_Range,
    H_If_Unmodified_Since,
    H_Max_Forwards,
    H_Proxy_Authorization,
    H_Range,
    H_Referer,
    H_TE,
    H_User_Agent,
    /* entity header fields */
    H_Allow,
    H_Content_Encoding,
    H_Content_Language,
    H_Content_Length,
    H_Content_Location,
    H_Content_MD5,
    H_Content_Range,
    H_Content_Type,
    H_Expires,
    H_Last_Modified  
};
/* todos opcionales menos Host */


/* que pasa si encuentra un header no implementado? bad syntax o lo ignora ? */
struct request_header_t {
    std::string header_name;
    std::string header_field;
};

class Request {
    private:
        std::string request_line[3];
        std::vector<request_header_t> header_v;
        std::string message_body;
    public:
        Request(void);
        Request(const Request&);
        Request(const std::string&);
        Request& operator=(const Request&);

        const std::string& method(void) const;
        const std::string& uri(void) const;
        const std::string& host(void) const;
        const std::string& header(request_header_f) const;

        bool parse_err; /* if true send 400 BAD REQUEST */
};

#endif // __REQUEST_HPP__