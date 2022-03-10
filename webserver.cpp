#include "webserver.hpp"

const std::string webservHeader = "\n \033[32m\
__  _  __ ____   _____ _______   ____     ____   ____ |  | |__| ____   ____  \n \
\\ \\/ \\/ // __ \\  \\__  \\\\_  __ \\_/ __ \\   /  _ \\ /    \\|  | |  |/    \\_/ __ \\ \n\
  \\     /\\  ___/   / __ \\|  | \\/\\  ___/  (  <_> )   |  \\  |_|  |   |  \\  ___/ \n\
   \\/\\_/  \\___  > (____  /__|    \\___  >  \\____/|___|  /____/__|___|  /\\___  >\n\
             \\/       \\/            \\/              \\/             \\/     \\/ \n\033[0m";

sig_atomic_t    quit_f = 0;
static void sighandl(int signal) {

    if (signal == SIGINT || signal == SIGQUIT) {
        quit_f = 1;
    }
}

bool    Webserver::addr_comp::operator()(const Socket& other) {
    return addr == other.get_socket_addr();
}

Webserver::addr_comp::addr_comp(const listen_directive_t& addr) : addr(addr) { }

/* condición de server duplicado: comparten mismas directivas listen y server_name */
void    Webserver::check_server_duplicates(const std::vector<Server>& srv_v) {
    for (std::vector<Server>::const_iterator it = srv_v.begin(); it != --srv_v.end(); it++) {
        for (std::vector<Server>::const_iterator it_n = it + 1; it_n != srv_v.end(); it_n++) {
            if (*it_n == *it) {
                throw std::runtime_error("duplicate server error\n");
            }
        }
    }
}

std::string    Webserver::timestamp(void) const {
    std::time_t t = std::time(0);
    std::tm*    time = std::localtime(&t);
    std::string format_time(asctime(time));

    format_time.erase(format_time.size() - 1);
    return format_time;
}

void    Webserver::log(const std::string& action, const std::string& error) const {
    std::cerr << "[ " << timestamp() << "] " << action << error << "\n";
}

void    Webserver::nfds_up(int fd) {
    if (fd + 1 >= nfds) {
        nfds = fd + 1;
    }
}

void    Webserver::nfds_down(int fd) {
    if (fd + 1 == nfds) {
        --nfds;
    }
}

/* acepta nuevas conexiones de los sockets pasivos; no trata fallos en accept como críticos */
void    Webserver::accept_new_connection(const Socket& passv) {
    struct sockaddr_in addr_in;
    socklen_t addr_len;

    int new_conn = accept(passv.fd, reinterpret_cast<sa_t*>(&addr_in), &addr_len);
    if (new_conn == -1) {
        log ("accept(): ", strerror(errno));
        return ;
    }
    fcntl(new_conn, F_SETFL, O_NONBLOCK);
    read_v.push_back(Socket(new_conn, passv));
    nfds_up(read_v.back().fd);
}

socket_status_f    Webserver::read_from_socket(Socket& conn_socket) {
    //Request& req = conn_socket.get_request();
    char     req_buff[REQUEST_BUFFER_SIZE];

    memset(req_buff, 0, REQUEST_BUFFER_SIZE);
    /* paso a una sola llamada a read por llamada a select, para evitar que una petición muy grande
     * nos bloquee el servidor */
    int socket_rd_stat = read(conn_socket.fd, req_buff, REQUEST_BUFFER_SIZE);
    if (socket_rd_stat == -1) {
        /* supón error EAGAIN, la conexión estaba marcada como activa pero ha bloqueado,
         * se guarda a la espera de que el cliente envíe información */
        log("read(): ", std::strerror(errno));
        return STANDBY;
    }
    if (socket_rd_stat == 0) {
        /* cliente ha cerrado conexión; cerramos socket y eliminamos de la lista */
        conn_socket.close_socket();
        nfds_down(conn_socket.fd);
        //read_v.erase(read_v.begin() + i);
        return CLOSED;
    }
    /* puede ser que una lectura del socket traiga más de una request ?? (std::vector<Request>) */
    /* (pipelining; sending multiple requests without waiting for an response) */
    try {
        conn_socket.build_request(req_buff);
    } catch (StatusLine& sl) {
        log("request: ", sl.getReason() + ": " + sl.getAdditionalInfo());
        conn_socket.build_response(sl);
        return CONTINUE;
    }
    return STANDBY;
    //return (req.getStage() == request_is_ready) ? CONTINUE : STANDBY;
}

socket_status_f    Webserver::write_to_socket(Socket& conn_socket) {
    /* llamada a write con el mensaje guardado en el Socket */
    const std::string& response = conn_socket.get_response_string();
    std::cout << "[debug] response:\n" << response << "\n";
    int socket_wr_stat = write(conn_socket.fd, response.c_str(), response.size());
    
    if (socket_wr_stat == -1) {
        /* supón error EAGAIN, el buffer de write está lleno y como trabajamos con sockets
         * no bloqueadores retorna con señal de error, la respuesta sigue siendo válida y el cliente espera */
        log("write(): ", strerror(errno));
        return STANDBY;
    }
    //if (/* Cliente ha indicado en cabecera que hay que cerrar la conexión tras enviar la respuesta */) {
    /* si la respuesta contiene un codigo de error, hay que chapar conexion */
    if (conn_socket.marked_for_closing()) {
        conn_socket.close_socket();
        return CLOSED;
    }
    conn_socket.clear_response();
    return CONTINUE;
}

void Webserver::ready_to_read_loop(void) {
    for (size_t i = 0; i < read_v.size(); i++) {
        if (FD_ISSET(read_v[i].fd, &readfds)){
            if (read_v[i].is_passv()) {
                accept_new_connection(read_v[i]);
                continue ;
            }
            socket_status_f conn_stat = read_from_socket(read_v[i]);

            if (conn_stat == CONTINUE) {
                write_v.push_back(read_v[i]);
            }
            if (conn_stat != STANDBY) {
                read_v.erase(read_v.begin() + i);
            }
        }
    }
}

void Webserver::ready_to_write_loop(void) {
    for (size_t i = 0; i < write_v.size(); i++) {
        if (FD_ISSET(write_v[i].fd, &writefds)) {
            socket_status_f conn_stat = write_to_socket(write_v[i]);

            if (conn_stat == CONTINUE) {
                read_v.push_back(write_v[i]);
            }
            if (conn_stat != STANDBY) {
                write_v.erase(write_v.begin() + i);
            }
        }
    }
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

/* inicializa servidores y sockets pasivos en base a las estructuras generadas por el parser */
Webserver::Webserver(const std::vector<server_block_t>& srv_blk_v) {
    for (std::vector<server_block_t>::const_iterator it = srv_blk_v.begin(); it != srv_blk_v.end(); it++) {
        server_v.push_back(*it);
    }
    check_server_duplicates(server_v);
    for (std::vector<Server>::iterator it = server_v.begin(); it != server_v.end(); it++) {
        Webserver::addr_comp comp(it->get_server_addr());
        std::vector<Socket>::iterator sock_it = std::find_if(read_v.begin(), read_v.end(), comp);

        if (sock_it == read_v.end()) {
            read_v.push_back(it->get_server_addr());
            read_v.back().add_server_ref(*it);
            nfds_up(read_v.back().fd);
        } else {
            sock_it->add_server_ref(*it);
        }
    }
}

Webserver::~Webserver() { 
    for (std::vector<Socket>::iterator it = read_v.begin(); it != read_v.end(); it++) {
        it->close_socket();
    }
    for (std::vector<Socket>::iterator it = write_v.begin(); it != write_v.end(); it++) {
        it->close_socket();
    }
}

void p(std::vector<Socket>& s) {
    std::cout << "[ ";
    for (std::vector<Socket>::iterator it = s.begin(); it != s.end(); it++) {
        std::cout << it->fd << " ";
    }
    std::cout << "]\n";
}

/* main loop */
void Webserver::run(void) {
    signal(SIGINT, &sighandl);
    signal(SIGQUIT, &sighandl);

    std::cout << webservHeader;
    while (!quit_f) {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        p(read_v), p(write_v);

        for (std::vector<Socket>::iterator it = read_v.begin(); it != read_v.end(); it++) {
            FD_SET(it->fd, &readfds);
        }
        for (std::vector<Socket>::iterator it = write_v.begin(); it != write_v.end(); it++) {
            FD_SET(it->fd, &writefds);
        }
        if ((select(nfds, &readfds, &writefds, NULL, NULL)) == -1) {
            throw std::runtime_error(strerror(errno));
        }
        ready_to_read_loop();
        ready_to_write_loop();
    }
}
