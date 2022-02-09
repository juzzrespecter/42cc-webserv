#include "request.hpp"

Request::Request(void) {

}

Request::Request(const Request&) {

}

Request::Request(const std::string& message) {
    /* parse request line */ /*separator*/
    /* parse loop() headers */ /*crlf*/
    /* crlf*/
    /* parse request body */
}

Request& Request::operator=(const Request&) {

}

const std::string& Request::method(void) const {

}

const std::string& Request::uri(void) const {

}

const std::string& Request::host(void) const {

}

const std::string& Request::header(request_header_f) const {

}