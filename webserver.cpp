#include "Webserver.hpp"

bool    Webserver::addr_comp::operator()(const Socket& other) {
    return addr == other.get_socket_addr();
}

Webserver::addr_comp::addr_comp(const listen_directive_t& addr) : addr(addr) { }

void    Webserver::accept_new_connection(int i) {
    struct sockaddr_in addr_in;
    socklen_t addr_len;

    int new_conn = accept(read_v[i].fd, reinterpret_cast<sa_t*>(&addr_in), &addr_len);
    if (new_conn == -1) {
        throw std::runtime_error(strerror(errno));
    }
    /* fcntl(fd, F_SETFL, O_NONBLOCK) */
    read_v.push_back(Socket(new_conn, read_v[i]));
}

void    Webserver::read_from_socket(int i) {
    char msg_buff[BUFFER_SIZE];
    std::string request;
    int read_ret;

    /* limpia buffer */
    while ((read_ret = read(read_v[i].fd, msg_buff, BUFFER_SIZE)) == BUFFER_SIZE) { /* problema cuando size(msg) == BUFF */
        request += msg_buff;
    }
    if (!read_ret) {
        close(read_v[i].fd);
    } else {
        /* pasa el mensaje a request parser y guarda en Socket */
        /*
            msg voyage -> respuesta se almacena en Socket a la espera de llamada en write
            1. busca servidor(es) con misma direccion que el Socket
            2. en caso de que haya más de uno, cribar por directiva Server_name y header host en msg
            3. en servidor, cribar por uri en request entre bloques de ruta
            4. parsear según configuración de ruta y generar respuesta
            5. la respuesta se almacena en el Socket a la espera de tal

            1. en teoría ya hecho (vector de punteros a servidores)
            2. competencia de Socket?? select_Server { get_hostname; compare(host, serv_name) ? select : next }
            3. competencia de Server?? select_location(msg) { get_uri; compare(uri, uri) ? select : next}
            4. competencia de location?? 
        */
        const Server& srv = read_v[i].select_requested_server(request);
        const Location& loc = srv.select_requested_location(request);
        read_v[i].set_response(loc.select_requested_method(request));
        
        write_v.push_back(read_v[i]);

    }
    read_v.erase(read_v.begin() + i);
}

void    Webserver::write_to_socket(int i) {
    /* llamada a write con el mensaje guardado en el Socket */
    write(write_v[i].fd, write_v[i].get_response().c_str(), write_v[i].get_response().size());

    read_v.push_back(write_v[i]);
    write_v.erase(write_v.begin() + i);
}

Webserver::Webserver(void) { }

Webserver::Webserver(const Webserver& other) { 
    *this = other;
}

Webserver& Webserver::operator=(const Webserver& other) { 
    if (this == &other) {
        return *this;
    }
    server_v = other.server_v;
    read_v = other.read_v;
    write_v = other.write_v;
    return *this;
}

void    Webserver::check_server_duplicates(const std::vector<Server>& srv_v) {
    for (std::vector<Server>::const_iterator it = srv_v.begin(); it != --srv_v.end(); it++) {
        for (std::vector<Server>::const_iterator it_n = it + 1; it_n != srv_v.end(); it_n++) {
            if (*it_n == *it) {
                throw std::runtime_error("duplicate server error\n");
            }
        }
    }
}

Webserver::Webserver(const std::vector<server_block_t>& srv_blk_v) {
    for (std::vector<server_block_t>::const_iterator it = srv_blk_v.begin(); it != srv_blk_v.end(); it++) {
        server_v.push_back(*it);
    }
    for (std::vector<Server>::iterator it = server_v.begin(); it != server_v.end(); it++) {
        Webserver::addr_comp comp(it->get_server_addr());
        std::vector<Socket>::iterator sock_it = std::find_if(read_v.begin(), read_v.end(), comp);

        if (sock_it == read_v.end()) {
            read_v.push_back(it->get_server_addr());
            read_v.back().add_server_ref(*it);
        } else {
            sock_it->add_server_ref(*it);
        }
    }
}

Webserver::~Webserver() { }

void Webserver::run(void) {
    std::cout << "[servidor levantado]\n";
    while (true) {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        for (std::vector<Socket>::iterator it = read_v.begin(); it != read_v.end(); it++) {
            FD_SET(it->fd, &readfds);
        }
        for (std::vector<Socket>::iterator it = write_v.begin(); it != write_v.end(); it++) {
            FD_SET(it->fd, &writefds);
        }
        std::vector<Socket>::iterator max_rd = std::max_element(read_v.begin(), read_v.end(), socket_comp());
        std::vector<Socket>::iterator max_wr = std::max_element(write_v.begin(), write_v.end(), socket_comp());
        int nfds = max_wr == write_v.end() ? max_rd->fd : std::max(max_rd->fd, max_wr->fd);

        if ((select(nfds, &readfds, &writefds, NULL, NULL)) == -1) {
            throw std::runtime_error(strerror(errno));
        }

        for (size_t i = 0; i < read_v.size(); i++) {
            if (FD_ISSET(read_v[i].fd, &readfds)){
                if (read_v[i].is_passv()) {
                    accept_new_connection(i);
                } else {
                    read_from_socket(i);
                }
            }
        }

        for (size_t i = 0; i < write_v.size(); i++) {
            if (FD_ISSET(write_v[i].fd, &writefds)) {
                write_to_socket(i);
            }
        }
    }
}
