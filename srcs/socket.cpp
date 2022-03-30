#include "socket.hpp"

Socket::Socket(void) { }

Socket::Socket(const Socket& other) {
    *this = other;
}

Socket::Socket(const listen_directive_t& sock_addr) : _type(PASSV), _sock_addr(sock_addr) {
    struct in_addr addr = { .s_addr = inet_addr(_sock_addr.addr.c_str()) };
    struct sockaddr_in sockaddr_s = {
        .sin_family = AF_INET,
        .sin_port = htons(_sock_addr.port),
        .sin_addr = addr,
        .sin_zero = { 0 }
    };
    int opt_val = 1;

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        throw std::runtime_error(strerror(errno));
    }
    if ((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int)) == -1) ||
        (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof(int)) == -1)) {
        throw std::runtime_error(strerror(errno));
    }
    if ((bind(fd, reinterpret_cast<sa_t*>(&sockaddr_s), sizeof(sockaddr_s))) == -1) {
        throw std::runtime_error(strerror(errno));
    }
    if ((listen(fd, FD_SETSIZE)) == -1) {
        throw std::runtime_error(strerror(errno));
    }
}

Socket::Socket(int conn_fd, const Socket& passv_socket) : 
    _type(ACTV), _sock_addr(passv_socket._sock_addr), _serv_v(passv_socket._serv_v), _req(_serv_v), fd(conn_fd) { 
}

Socket::~Socket() { }

Socket& Socket::operator=(const Socket& other) {
    if (this == &other) {
        return *this;
    }
    fd = other.fd;
    _type = other._type;
    _sock_addr = other._sock_addr;
    _serv_v = other._serv_v;
    _req = other._req;
    _resp = other._resp;
    return *this;
}

const listen_directive_t& Socket::get_socket_addr(void) const {
    return _sock_addr;
}

void Socket::build_request(const char buffer[], int buffer_size) {
     _req.recv_buffer(buffer, buffer_size);
}

void Socket::build_response(const StatusLine& sl) {
     _resp.fillBuffer(&_req, _req.get_location(), sl);
}

std::string Socket::get_response_string(void) const {
    return _resp.getBuffer();
}

void  Socket::add_server_ref(const Server& srv) {
    _serv_v.push_back(&srv);
}

bool Socket::is_passv(void) const {
    return _type == PASSV;
}

bool Socket::marked_for_closing(void) const {
    /* case Connection header set to close */
    if (_resp.getEndConn()) {
        return true;
    }
    /* case error response (4xx / 5xx) */
    if (_resp.getCode() >= 400) {
        return true;
    }
	return false;
}

void Socket::close_socket(void) const {
    close(fd);
}

void Socket::clear_response(void) {
    _resp.clear();
    if (_req.get_request_stage() == READY) {
        _req.clear();
    }
}
