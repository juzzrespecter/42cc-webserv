#include "parser.hpp"

typedef Parser::server_vector                 server_vector;
typedef std::vector<Parser::location_block_t> location_vector;

void pr(const std::vector<std::string>& v) {
    std::cout << "{ ";
    for (std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << "}\n";
}

std::string dir[N_SERVER_DIR] = {
    "error_page:",
    "body_size :"  ,
    "root      :"  ,
    "autoindex :"  ,
    "index     :"  ,
    "return    :"  ,
    "cgi_pass  :"  ,
    "methods   :"  ,
    "upload    :"  ,
    "listen    :"  ,
    "serv_name :"  
};

void print_server(const server_vector& s) {
    for (server_vector::const_iterator it = s.begin(); it != s.end(); it++) {
        std::cout << "[SERVER]\n";
        for (int i = 0; i < N_SERVER_DIR; i++) { 
            std::cout << "\t" << dir[i]; pr(it->directive[i]);
        }
        for (location_vector::const_iterator it_2 = it->location.begin(); it_2 != it->location.end(); it_2++) {
            std::cout << "\t{location}\n";
            std::cout << "\turi - " << it_2->uri << "\n";
            for (int i = 0; i < N_LOCATION_DIR; i++) {
                std::cout << "\t\t" << dir[i]; pr(it_2->directive[i]);
            }
        }
        
    }
}

int main(int argc, char *argv[]) {
    std::string path("./config_file");
    if (argc == 2) {
        path = argv[1];
    }
    if (argc > 2) {
        std::cerr << "too many arguments\n";
        return EXIT_FAILURE;
    }

    Parser p;
    try {
        Parser::server_vector sv = p.parse(path);
        print_server(sv);
    } catch (std::exception& e) {
        std::cout << "[PARSER] " << e.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
