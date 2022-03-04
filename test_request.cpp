#include "request.hpp"
#define REQ_BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    std::string path = "./test_request_msg";

    if (argc > 2) {
        std::cerr << "wrong arguments\n";
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        path = argv[1];
    }
    std::fstream req_file(path);

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
}