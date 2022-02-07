#ifndef __WEBSERVER_HPP__
#define __WEBSERVER_HPP__

#include "parser.hpp"
#include "server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>

#define BUFFER_SIZE 100

enum socket_mode_e {PASSV, ACTV};

typedef struct sockaddr sa_t;

struct socket_t {
    int socket_fd;
    socket_mode_e type;
    listen_directive_t sock_addr;
    std::vector<server*> server_ref_v;

    socket_t(void);
    socket_t(const socket_t&);
    socket_t(const listen_directive_t&); /* passv socket constructor */
    socket_t(int, const socket_t&); /* active socket constructor */

    socket_t& operator=(const socket_t&);
};

class webserver {
    private:
        std::vector<server>   server_v;
        std::vector<socket_t> rd_socket_v;
        std::vector<socket_t> wr_socket_v;

        fd_set readfds;
        fd_set writefds;

        struct addr_comp {
            public:
                bool operator()(const socket_t&);
                addr_comp(const listen_directive_t&);
            private:
                const listen_directive_t& addr;
        };

        struct socket_comp {
            public:
                bool operator()(const socket_t&, const socket_t&);
        };

        void accept_new_connection(int i);
        void read_from_socket(int i);
        void write_to_socket(int i);

        webserver(void);
        webserver(const webserver&);
        webserver& operator=(const webserver&);
    public:
        webserver(const server_vector&);
        ~webserver();

        void run(void);
};

#endif // __WEBSERVER_HPP__