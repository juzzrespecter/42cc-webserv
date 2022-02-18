#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <sys/Socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "Server.hpp"
#include "Response.hpp"

enum socket_mode_f {PASSV, ACTV};

typedef struct sockaddr sa_t;

class Socket {
    private:
        socket_mode_f type; /* tipo de socket: pasivo o activo */
        listen_directive_t sock_addr; /* dirección y puerto en los qu e el socket escucha */
        std::vector<Server*> server_ref_v; /* vector de servidores que comparten el mismo socket */
        Request req; /* se almacena la petición por si requiere varias lecturas el obtener la petición completa (chunked request) */
        Response resp; /* respuesta generada a partir de la petición y la config. del servidor */

    public:
        int fd;

        Socket(void);
        Socket(const Socket&);
        Socket(const listen_directive_t&); /* passv Socket constructor */
        Socket(int, const Socket&); /* active Socket constructor */
        ~Socket();

        Socket& operator=(const Socket&);

        const listen_directive_t& get_socket_addr(void) const;
        
        Request& get_request(void) const;
        Response& get_response(void) const;

        void add_server_ref(Server&);

        bool is_passv(void) const;
        void close_socket(void) const;
};

/* Uso en webserver::run para seleccionar el socket con mayor fd en la llamada a select */
struct socket_comp {
    public:
        bool operator()(const Socket&, const Socket&);
};

#endif //__Socket_HPP__
