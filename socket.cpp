#include "socket.hpp"

Socket::Socket(void) { }

Socket::Socket(const Socket& other) : 
    _type(other._type), _sock_addr(other._sock_addr), _vserv_v(other._vserv_v), fd(other.fd) { }

Socket::Socket(const listen_directive_t& sock_addr) : _type(PASSV), _sock_addr(sock_addr) {
    struct in_addr addr = { .s_addr = inet_addr(_sock_addr.addr.c_str()) };
    struct sockaddr_in sockaddr_s = {
        .sin_family = AF_INET,
        .sin_port = htons(_sock_addr.port),
        .sin_addr = addr,
        .sin_zero = { 0 }
    };

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        throw std::runtime_error(strerror(errno));
    }
    if ((bind(fd, reinterpret_cast<sa_t*>(&sockaddr_s), sizeof(sockaddr_s))) == -1) {
        throw std::runtime_error(strerror(errno));
    }
    if ((listen(fd, FD_SETSIZE)) == -1) {/* fd_setsize?? */
        throw std::runtime_error(strerror(errno));
    }
}

Socket::Socket(int conn_fd, const Socket& passv_Socket) : 
    _type(ACTV), _sock_addr(passv_Socket._sock_addr), _vserv_v(passv_Socket._vserv_v), fd(conn_fd) { }

Socket::~Socket() { }

Socket& Socket::operator=(const Socket& other) {
    if (this == &other) {
        return *this;
    }
    fd = other.fd;
    _type = other._type;
    _sock_addr = other._sock_addr;
    _vserv_v = other._vserv_v;
    _req = other._req;
    //_resp = other._resp;
    return *this;
}

const listen_directive_t& Socket::get_socket_addr(void) const {
    return _sock_addr;
}

//void Socket::set_response(const Response& resp) {
//    _resp = resp;
//}

Request& Socket::get_request(void) {
    return _req;
}

//Response& Socket::get_response(void) {
//    return _resp;
//}

void  Socket::add_server_ref(Server& srv) {
    _vserv_v.push_back(&srv);
}

bool  Socket::is_passv(void) const {
    return _type == PASSV;
}

void Socket::close_socket(void) const {
    close(fd);
}