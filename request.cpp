#include "Request.hpp"

Request::Request() : statusFlag(EMPTY) { }

/*Request::Request(const std::vector<Server*> infoVirServs) {

}*/

Request::Request(const Request& c) {

}
Request::~Request() {

}
Request& Request::operator=(Request a) {

}

const RequestLine& Request::getRequestLine() const {

}
const std::map<std::string, std::string>& Request::getHeaders() const {

}
const Body& Request::getBody() const {

}
const std::string& Request::getBuffer() const {

}
int Request::getMethod() const {

}
const std::string& Request::getPath() const {

}
const std::string& Request::getQuery() const {

}

void Request::setPath(const std::string& path) {

}

bool newLineReceived(size_t posCLRF) {

}

void parseRequestLine(size_t posCLRF);