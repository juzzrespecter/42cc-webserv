#include "webserv.hpp"
#include "webserver.hpp"
#define REQ_BUFFER_SIZE 4096

int main(void) {
    std::string parser_path("./config/config_file");
    Parser p;

    try {
        std::vector<server_block_t> srv_t = p.parse(parser_path);
        Webserver websrv(srv_t);

        websrv.run();
    } catch(std::runtime_error& e) {
        std::cerr << "[ server fatal error ] " <<  e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
