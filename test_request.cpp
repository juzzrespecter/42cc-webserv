#include "webserv.hpp"
#include "webserver.hpp"
#define REQ_BUFFER_SIZE 4096

/*
void test() {
    std::string path = "./test_request_msg";

    if (argc > 2) {
        std::cerr << "wrong arguments\n";
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        path = argv[1];
    }
    std::fstream req_file(path.c_str());

    if (!req_file.is_open()) {
        std::cerr << "wrong file path\n";
        return EXIT_FAILURE;
    }
    Request req;
    char req_buffer[REQ_BUFFER_SIZE];

    req_file.read(req_buffer, REQ_BUFFER_SIZE);
    try {
        req.recvBuffer(req_buffer);
    } catch(StatusLine& sl){
        std::cerr << sl.getReason() << ": " << sl.getAdditionalInfo() << "\n";
        return EXIT_FAILURE;
    }
    req.print();
}*/

int main(void) {
    std::string parser_path("./config/config_file");
    Parser p;

    try {
        std::vector<server_block_t> srv_t = p.parse(parser_path);
        Webserver websrv(srv_t);

        websrv.run();
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}