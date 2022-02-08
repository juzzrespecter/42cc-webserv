#ifndef __Socket_HPP__
#define __Socket_HPP__

#include <sys/Socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "server.hpp"

enum socket_mode_f {PASSV, ACTV};

typedef struct sockaddr sa_t;

class Socket {
    private:
        socket_mode_f type; /* tipo de socket: pasivo o activo */
        listen_directive_t sock_addr; /* dirección y puerto en los que el socket escucha */
        std::vector<Server*> server_ref_v; /* vector de servidores que comparten el mismo socket */ 
        std::string response; /* respuesta generada a partir de la petición y la config. del servidor */

        std::string get_host_from_request(const std::string&) const;
    public:
        int fd;

        Socket(void);
        Socket(const Socket&);
        Socket(const listen_directive_t&); /* passv Socket constructor */
        Socket(int, const Socket&); /* active Socket constructor */
        ~Socket();

        Socket& operator=(const Socket&);

        bool is_passv(void) const;
        const listen_directive_t& get_socket_addr(void) const;
        void add_server_ref(Server&);
        void set_response(const std::string&);

        const Server& select_requested_server(const std::string&);
};

/* Uso en webserver::run para seleccionar el socket con mayor fd en la llamada a select */
struct socket_comp {
    public:
        bool operator()(const Socket&, const Socket&);
};

#endif //__Socket_HPP__
