#include "webserver.hpp"

socket_t::socket_t(void) { }

socket_t::socket_t(const socket_t& other) : 
    socket_fd(other.socket_fd), type(other.type), sock_addr(other.sock_addr), server_ref_v(other.server_ref_v) { }

socket_t::socket_t(const listen_directive_t& sock_addr_) : sock_addr(sock_addr_), type(PASSV) {
    struct in_addr addr = { .s_addr = inet_addr(sock_addr_.addr.c_str()) };
    struct sockaddr_in sockaddr_s {
        .sin_family = AF_INET,
        .sin_port = htons(sock_addr_.port),
        .sin_addr = addr,
        .sin_zero = { 0 }
    };

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        throw std::runtime_error(strerror(errno));
    }
    if ((bind(socket_fd, reinterpret_cast<sa_t*>(&sockaddr_s), sizeof(sockaddr_s))) == -1) {
        throw std::runtime_error(strerror(errno));
    }
    if ((listen(socket_fd, FD_SETSIZE)) == -1) {/* fd_setsize?? */
        throw std::runtime_error(strerror(errno));
    }
}

socket_t::socket_t(int conn_fd, const socket_t& passv_socket) : 
    socket_fd(conn_fd), type(ACTV), sock_addr(passv_socket.sock_addr), server_ref_v(passv_socket.server_ref_v) { }

socket_t& socket_t::operator=(const socket_t& other) {
    if (this == &other) {
        return *this;
    }
    socket_fd = other.socket_fd;
    type = other.type;
    sock_addr = other.sock_addr;
    server_ref_v = other.server_ref_v;
    return *this;
}

bool    webserver::addr_comp::operator()(const socket_t& other) {
    return addr == other.sock_addr;
}

webserver::addr_comp::addr_comp(const listen_directive_t& addr) : addr(addr) { }

bool    webserver::socket_comp::operator()(const socket_t& lhs, const socket_t& rhs) {
    return lhs.socket_fd < rhs.socket_fd;
}

void    webserver::accept_new_connection(int i) {
    struct sockaddr_in addr_in;
    socklen_t addr_len;

    int new_conn = accept(rd_socket_v[i].socket_fd, reinterpret_cast<sa_t*>(&addr_in), &addr_len);
    if (new_conn == -1) {
        throw std::runtime_error(strerror(errno));
    }
    rd_socket_v.push_back(socket_t(new_conn, rd_socket_v[i]));
}

void    webserver::read_from_socket(int i) {
    char msg_buff[BUFFER_SIZE];
    std::string msg;
    int read_ret;

    /* limpia buffer */
    while ((read_ret = read(rd_socket_v[i].socket_fd, msg_buff, BUFFER_SIZE)) == BUFFER_SIZE) {
        msg += msg_buff;
    }
    if (!read_ret) {
        close(rd_socket_v[i].socket_fd);
    } else {
        /* pasa el mensaje a request parser y guarda en socket */
        wr_socket_v.push_back(rd_socket_v[i]);
    }
    rd_socket_v.erase(rd_socket_v.begin() + i);
}

void    webserver::write_to_socket(int i) {
    /* llamada a write con el mensaje guardado en el socket */

    rd_socket_v.push_back(wr_socket_v[i]);
    wr_socket_v.erase(wr_socket_v.begin() + i);
}

webserver::webserver(void) { }

webserver::webserver(const webserver& other) { }

webserver& webserver::operator=(const webserver& other) { }

webserver::webserver(const server_vector& srv_blk_v) {
    for (server_vector::const_iterator it = srv_blk_v.begin(); it != srv_blk_v.end(); it++) {
        server_v.push_back(*it);
    }
    for (std::vector<server>::iterator it = server_v.begin(); it != server_v.end(); it++) {
        webserver::addr_comp comp(it->get_server_addr());
        std::vector<socket_t>::iterator sock_it = std::find_if(rd_socket_v.begin(), rd_socket_v.end(), comp);

        if (sock_it == rd_socket_v.end()) {
            rd_socket_v.push_back(it->get_server_addr());
            rd_socket_v.back().server_ref_v.push_back(&*it);
        } else {
            sock_it->server_ref_v.push_back(&*it);
        }
    }
}

void webserver::run(void) {
    while (true) {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        for (std::vector<socket_t>::iterator it = rd_socket_v.begin(); it != rd_socket_v.end(); it++) {
            FD_SET(it->socket_fd, &readfds);
        }
        for (std::vector<socket_t>::iterator it = wr_socket_v.begin(); it != wr_socket_v.end(); it++) {
            FD_SET(it->socket_fd, &writefds);
        }
        std::vector<socket_t>::iterator max_rd = std::max_element(rd_socket_v.begin(), rd_socket_v.end(), webserver::socket_comp());
        std::vector<socket_t>::iterator max_wr = std::max_element(wr_socket_v.begin(), wr_socket_v.end(), webserver::socket_comp());
        int nfds = max_wr == wr_socket_v.end() ? max_rd->socket_fd : std::max(max_rd->socket_fd, max_wr->socket_fd);

        if ((select(nfds, &readfds, &writefds, NULL, NULL)) == -1) {
            throw std::runtime_error(strerror(errno));
        }

        for (size_t i = 0; i < rd_socket_v.size(); i++) {
            if (FD_ISSET(rd_socket_v[i].socket_fd, &readfds)){
                if (rd_socket_v[i].type == PASSV) {
                    accept_new_connection(i);
                } else {
                    read_from_socket(i);
                }
            }
        }

        for (size_t i = 0; i < wr_socket_v.size(); i++) {
            if (FD_ISSET(wr_socket_v[i].socket_fd, &writefds)) {
                write_to_socket(i);
            }
        }

    }
}