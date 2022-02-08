#ifndef __WEBSERVER_HPP__
#define __WEBSERVER_HPP__

#include "parser.hpp"
#include "socket.hpp"
#include <unistd.h>

#define BUFFER_SIZE 100

typedef std::vector<std::string>      string_vector;
typedef std::vector<server_block_t>   server_block_vector;
typedef std::vector<location_block_t> location_block_vector;
typedef std::vector<Server>           server_vector;
typedef std::vector<Location>         location_vector;

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

        void accept_new_connection(int i);
        void read_from_socket(int i);
        void write_to_socket(int i);

        Webserver(void);
        Webserver(const Webserver&);
        Webserver& operator=(const Webserver&);
    public:
        Webserver(const server_vector&);
        ~Webserver();

        void run(void);
};

#endif // __WEBSERVER_HPP__
