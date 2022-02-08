#include "socket.hpp"

Socket::Socket(void) { }

Socket::Socket(const Socket& other) : 
    type(other.type), sock_addr(other.sock_addr), server_ref_v(other.server_ref_v), fd(other.fd) { }

Socket::Socket(const listen_directive_t& sock_addr_) : type(PASSV), sock_addr(sock_addr_) {
    struct in_addr addr = { .s_addr = inet_addr(sock_addr_.addr.c_str()) };
    struct sockaddr_in sockaddr_s = {
        .sin_family = AF_INET,
        .sin_port = htons(sock_addr_.port),
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
    type(ACTV), sock_addr(passv_Socket.sock_addr), server_ref_v(passv_Socket.server_ref_v), fd(conn_fd) { }

Socket::~Socket() { }

Socket& Socket::operator=(const Socket& other) {
    if (this == &other) {
        return *this;
    }
    fd = other.fd;
    type = other.type;
    sock_addr = other.sock_addr;
    server_ref_v = other.server_ref_v;
    return *this;
}

bool  Socket::is_passv(void) const {
    return type == PASSV;
}

const listen_directive_t& Socket::get_socket_addr(void) const {
    return sock_addr;
}

void  Socket::add_server_ref(Server& srv) {
    server_ref_v.push_back(&srv);
}

void Socket::set_response(const std::string& response_) {
    response = response_;
}

/* 
 * Realiza busqueda del servidor solicitado según el header host 
 * que contiene (o debería contener) el mensaje mandado por el cliente.
 * Si el socket solo tiene un servidor como referencia, o no existe una
 * cabecera host en el mensaje, retorna el primer servidor.
 * Si encuentra una coincidencia entre los posibles nombres del servidor,
 * retorna el servidor.
 * Si no encunetra ninguna coincidencia, el primer servidor es retornado
 * como default.
 */

const Server& Socket::select_requested_server(const std::string& request) {
    if (server_ref_v.size() == 1) {
        return *server_ref_v.front();
    }
    std::string host = get_host_from_request(request);
    if (host.empty()) {
        return *server_ref_v.front();
    }
    for (std::vector<Server *>::iterator it_s = server_ref_v.begin(); it_s != server_ref_v.end(); it_s++) {
        Server& srv = **it_s;
        if (std::find(srv.server_name.begin(), srv.server_name.end(), host) != srv.server_name.end()) {
            return srv;
        }
    }
    return *server_ref_v.front();
}

bool    socket_comp::operator()(const Socket& lhs, const Socket& rhs) {
    return lhs.fd < rhs.fd;
}