#ifndef __WEBSERVER_HPP__
#define __WEBSERVER_HPP__

#include "parser.hpp"
#include "socket.hpp"
#include <unistd.h>

#define BUFFER_SIZE 100

class Webserver {
    private:
        std::vector<Server> server_v;
        std::vector<Socket> /*rd_socket_v;*/ read_v;
        std::vector<Socket> /*wr_socket_v; */ write_v;

        fd_set readfds;
        fd_set writefds;

        struct addr_comp {
            public:
                bool operator()(const Socket&);
                addr_comp(const listen_directive_t&);
            private:
                const listen_directive_t& addr;
        };
        void check_server_duplicates(const std::vector<Server>&);

        void accept_new_connection(int);
        void read_from_socket(int);
        void write_to_socket(int);

        Webserver(void);
        Webserver(const Webserver&);
        Webserver& operator=(const Webserver&);
    public:
        Webserver(const std::vector<server_block_t>&);
        ~Webserver();

        void run(void);
};

#endif // __WEBSERVER_HPP__
