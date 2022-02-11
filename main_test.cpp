#include "webserver.hpp"

std::string dir_name[N_DIR_SRV] = {
    "D_BODY_SIZE:  ",
    "D_ROOT:       ",
    "D_AUTOINDEX:  ",
    "D_INDEX:      ",
    "D_METHOD:     ",
    "D_UPLOAD:     ",
    "D_RETURN:     ",
    "D_CGI_PASS:   ",
    "D_ERROR_PAGE: ",
    "D_LISTEN:     ",
    "D_SERVER_NAME:"
};

void check_parser(std::vector<server_block_t>& v) {
    for (std::vector<server_block_t>::iterator it_s = v.begin(); it_s != v.end(); it_s++) {
        std::cout << "[server]\n";
        for (int i = 0; i < N_DIR_SRV; i++) {
            std::cout << "\t" << dir_name[i];
            for (std::vector<std::string>::iterator it_d = it_s->dir[i].begin(); it_d != it_s->dir[i].end(); it_d++) {
                std::cout << *it_d << " ";
            }
            std::cout << "\n";
        }
        for (std::vector<location_block_t>::iterator it_l = it_s->loc.begin(); it_l != it_s->loc.end(); it_l++) {
            std::cout << "\t[loc]\n";
            for (int i = 0; i < N_DIR_LOC; i++) {
                std::cout << "\t\t"  << dir_name[i];;
                for (std::vector<std::string>::iterator it_d = it_l->dir[i].begin(); it_d != it_l->dir[i].end(); it_d++) {
                    std::cout << *it_d << " ";
                }
                std::cout << "\n";
            }
        }
    }
}

int main(int argc, char *argv[]) {
    std::string config_path("./config/config_file");

    if (argc > 2) {
        std::cout << "wrong number of arguments.\n";
        return EXIT_FAILURE;
    }
    if (argc == 2) {
        config_path = argv[1];
    }
    Parser p;
    try {
        std::vector<server_block_t> v = p.parse(config_path);
        check_parser(v);
        Webserver w(v);

        w.run();
    } catch (std::runtime_error& e) {
        std::cout << "error: " << e.what() << "\n";
    }
}