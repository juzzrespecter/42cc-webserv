#ifndef __LOCATION_HPP__
#define __LOCATION_HPP__

#include "parser.hpp"

#define N_METHODS 3

struct cgi_pass_directive_t {
    std::string cgi_file_ext;
    std::string cgi_path;

    cgi_pass_directive_t(void);
    cgi_pass_directive_t(const cgi_pass_directive_t&);
    cgi_pass_directive_t(const std::string&, const std::string&);
    cgi_pass_directive_t& operator=(const cgi_pass_directive_t&);
};

typedef std::vector<cgi_pass_directive_t> cgi_pass_vector;

class Location {
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

        typedef std::string (Location::*method_options)(const std::string&) const;

        std::string http_method_get(const std::string&) const;
        std::string http_method_post(const std::string&) const;
        std::string http_method_delete(const std::string&) const;

        std::string get_method_from_request(const std::string&) const;
    public:
        std::string uri;

        Location(void);
        Location(const Location&);
        Location(const location_block_t&);
        ~Location();
        Location& operator=(const Location&);

        std::string select_requested_method(const std::string&) const;
};

#endif // __LOCATION_HPP__
