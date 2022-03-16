#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <cstring>
#include "server.hpp"
#include "request.hpp"
#include "response.hpp"

enum socket_mode_f {PASSV, ACTV};

typedef struct sockaddr sa_t;

class Socket {
    private:
        socket_mode_f _type; /* tipo de socket: pasivo o activo */
        listen_directive_t _sock_addr; /* dirección y puerto en los qu e el socket escucha */
        std::vector<const Server*> _serv_v; /* vector de servidores que comparten el mismo socket */
        Request _req; /* se almacena la petición por si requiere varias lecturas el obtener la petición completa (chunked request) */
        Response _resp; /* respuesta generada a partir de la petición y la config. del servidor */

    public:
        int fd;

        Socket(void);
        Socket(const Socket&);
        Socket(const listen_directive_t&); /* passv Socket constructor */
        Socket(int, const Socket&); /* active Socket constructor */
        ~Socket();

        Socket& operator=(const Socket&);

        const listen_directive_t& get_socket_addr(void) const;

        void build_request(const std::string&);
        void build_response(const StatusLine&);

        std::string get_response_string(void) const;

        void add_server_ref(const Server&);

        bool is_passv(void) const;
        bool marked_for_closing(void) const;

        void close_socket(void) const;
        void clear_response(void);
};

#endif //__SOCKET_HPP__
