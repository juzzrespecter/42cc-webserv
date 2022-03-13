#include "webserv.hpp"
#include "webserver.hpp"
//#define REQ_BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cerr << "[ server ] wrong number of arguments\n";
        return EXIT_FAILURE;
    }
    std::string parser_path("test.conf");
    Parser p;
    if (argc == 2) {
        parser_path = argv[1];
    }
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
