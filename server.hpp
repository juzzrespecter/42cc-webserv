#include "parser.hpp"
#include "location.hpp"

struct listen_directive_t {
    std::string addr;
    id_t        port;

    listen_directive_t(void);
    listen_directive_t(const listen_directive_t&);
    listen_directive_t(const std::string&);
    listen_directive_t& operator=(const listen_directive_t&);

    bool operator==(const listen_directive_t&) const;
    bool operator!=(const listen_directive_t&) const;
};

class server {
    private:
        listen_directive_t       listen;
        std::vector<std::string> server_name;
        std::vector<location>    routes;
    public:
        server(void);
        server(const server& other);
        server(const server_block_t& server_block);
        ~server();

        const listen_directive_t get_server_addr(void) const;
};