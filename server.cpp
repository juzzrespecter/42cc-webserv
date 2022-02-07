#include "server.hpp"

listen_directive_t::listen_directive_t(void) : addr("127.0.0.1"), port(8080) { }

listen_directive_t::listen_directive_t(const listen_directive_t& other) : addr(other.addr), port(other.port) { }

listen_directive_t::listen_directive_t(const std::string& raw_directive) {
    std::string::size_type c(raw_directive.find('.'));

    if (c != std::string::npos) {
        this->addr = raw_directive.substr(0, c);
        this->port = std::atoi(raw_directive.substr(c).c_str());
    } else {
        char *ptr;

        strtol(raw_directive.c_str(), &ptr, 0);
        this->addr = !*ptr ? "127.0.0.1" : raw_directive;
        this->port = !*ptr ? std::atoi(raw_directive.c_str()) : 8080;
    }
    if (!this->addr.compare("localhost")) {
        this->addr = "127.0.0.1";
    }
}

bool listen_directive_t::operator==(const listen_directive_t& rhs) {
    return addr == rhs.addr && port == rhs.port;
}
bool listen_directive_t::operator!=(const listen_directive_t& rhs) {
    return !(*this == rhs);
}

server::server(void) : listen() { }

server::server(const server& other) : 
    listen(other.listen), server_name(other.server_name), routes(other.routes) { }

server::server(const server_block_t& srv_blk) {
    location_block_t default_route(srv_blk);
    std::vector<location>::iterator it;

    if (!srv_blk.dir[D_LISTEN].empty()) {
        listen = listen_directive_t(srv_blk.dir[D_LISTEN].front());
    }
    this->server_name = srv_blk.dir[D_SERVER_NAME];
    for (location_vector::const_iterator it = server_block.location.begin(); it != server_block.location.end(); it++) {
        routes.push_back(*it);
    }
    for (it = routes.begin(); it != routes.end(); it++) {
        if (!it->uri.compare("/")) break ;
    }
    if (it == routes.end()) {
        routes.push_back(default_route);
    }
}

const listen_directive_t server::get_server_addr(void) const {
    return listen;
}
