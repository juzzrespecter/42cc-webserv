#ifndef __LOCATION_HPP__
#define __LOCATION_HPP__

#include "parser.hpp"
#include <unistd.h>
#include <utility>

#define N_METHODS 3

typedef std::pair<std::string, std::string>   cgi_pair;
typedef std::vector<cgi_pair>::const_iterator cgi_iterator;

class Location {
    private:
        std::string error_page;
        std::string root;
        std::string return_uri;
        std::string upload_path;

        std::vector<std::string> index;
        std::vector<std::string> accept_method;

        std::vector<std::pair<std::string, std::string> > cgi_pass;

        unsigned int body_size;
        bool autoindex;

        typedef std::string (Location::*method_options)(const std::string&) const;

    public:
        std::string uri;

        Location(void);
        Location(const Location&);
        Location(const location_block_t&);
        ~Location();
        Location& operator=(const Location&);

        const std::string& get_error_page(void) const;
        const std::string& get_root(void) const;
        const std::string& get_return_uri(void) const;
        const std::string& get_upload_path(void) const;

        const std::vector<std::string>& get_index(void) const;        
        const std::vector<std::string>& get_methods(void) const;

        const std::vector<std::pair<std::string, std::string> >& get_cgi_pass(void) const;

        unsigned int get_body_size(void) const;
        bool get_autoindex(void) const;

};

#endif // __LOCATION_HPP__
