#include "Server.hpp"

listen_directive_t::listen_directive_t(void) : addr("127.0.0.1"), port(8080) { }

listen_directive_t::listen_directive_t(const listen_directive_t& other) : addr(other.addr), port(other.port) { }

listen_directive_t::listen_directive_t(const std::string& raw_directive) {
    std::string::size_type c(raw_directive.find(':'));

    if (c != std::string::npos) {
        this->addr = raw_directive.substr(0, c);
        this->port = std::atoi(raw_directive.substr(c + 1).c_str());
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

listen_directive_t& listen_directive_t::operator=(const listen_directive_t& other) {
    if (this == &other) {
        return *this;
    }
    addr = other.addr;
    port = other.port;
    return *this;
}

bool listen_directive_t::operator==(const listen_directive_t& rhs) const {
    return addr == rhs.addr && port == rhs.port;
}
bool listen_directive_t::operator!=(const listen_directive_t& rhs) const {
    return !(*this == rhs);
}

Server::Server(void) : listen() { }

Server::Server(const Server& other) : 
    listen(other.listen), server_name(other.server_name), routes(other.routes) { }

Server::Server(const server_block_t& srv_blk) {
    location_block_t default_route(srv_blk);
    std::vector<Location>::iterator it;

    if (!srv_blk.dir[D_LISTEN].empty()) {
        listen = listen_directive_t(srv_blk.dir[D_LISTEN].front());
    }
    this->server_name = srv_blk.dir[D_SERVER_NAME];
    for (location_vector::const_iterator it = srv_blk.loc.begin(); it != srv_blk.loc.end(); it++) {
        routes.push_back(*it);
    }
    for (it = routes.begin(); it != routes.end(); it++) {
        if (!it->uri.compare("/")) break ;
    }
    if (it == routes.end()) {
        routes.push_back(default_route);
    }
}

Server::~Server() { }

const listen_directive_t Server::get_server_addr(void) const {
    return listen;
}

/* 
 * Según la uri solicitada en la petición del cliente, selecciona de entre 
 * los posibles bloques de ruta (locations) dentro del servidor.
 * Selecciona la ruta con la uri más extensa coincidente (hace una búsqueda aproximada).
 */

const Location& Server::select_requested_location(const std::string& request) const { /* check constness */
    if (routes.size() == 1) {
        return routes.front(); /* sólo existe la ruta por default dentro del servidor */
    }
    std::string requested_uri = get_uri_from_request(request);
    for (location_vector::iterator it_l = routes.begin(); it_l != routes.end(); it_l++) {
        /* algoritmo de selección de rutas */
    }
    /* return x */
}
