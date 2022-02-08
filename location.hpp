#ifndef __LOCATION_HPP__
#define __LOCATION_HPP__

#include "parser.hpp"

struct cgi_pass_directive_t {
    std::string cgi_file_ext;
    std::string cgi_path;

    cgi_pass_directive_t(void);
    cgi_pass_directive_t(const cgi_pass_directive_t&);
    cgi_pass_directive_t(const std::string&, const std::string&);
    cgi_pass_directive_t& operator=(const cgi_pass_directive_t&);
};

typedef std::vector<cgi_pass_directive_t> cgi_pass_vector;

class location {
    private:
        std::string error_page;
        std::string root;
        std::string return_uri;
        std::string upload_path;

        std::vector<std::string> index;
        std::vector<std::string> accept_method;

        cgi_pass_vector cgi_pass;

        unsigned int body_size;
        bool autoindex;
    public:
        std::string uri;

        location(void);
        location(const location&);
        location(const location_block_t&);
        ~location();
        location& operator=(const location_block_t&);
};

#endif // __LOCATION_HPP__