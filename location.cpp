#include "Location.hpp"

std::pair<std::string, std::string>::std::pair<std::string, std::string>(void) { }

std::pair<std::string, std::string>::std::pair<std::string, std::string>(const std::pair<std::string, std::string>& other) :
    cgi_file_ext(other.cgi_file_ext), cgi_path(other.cgi_path) { }

std::pair<std::string, std::string>::std::pair<std::string, std::string>(const std::string& cgi_file_ext_, const std::string& cgi_path_) :
    cgi_file_ext(cgi_file_ext_), cgi_path(cgi_path_) { }


std::pair<std::string, std::string>& std::pair<std::string, std::string>::operator=(const std::pair<std::string, std::string>& other) {
    if (this == &other)  {
        return *this;
    }
    this->cgi_path = other.cgi_path;
    this->cgi_file_ext = other.cgi_file_ext;
    return *this;
}

Location::Location(void) { }

Location::Location(const Location& other) {
    *this = other;
}

Location::Location(const location_block_t& loc_b) {
    typedef std::vector<std::string> string_vector;

    this->uri = loc_b.uri;

    if (!loc_b.dir[D_ERROR_PAGE].empty()){
        this->error_page = loc_b.dir[D_ERROR_PAGE].front();
    }
    this->root = loc_b.dir[D_ROOT].front();
    if (!loc_b.dir[D_RETURN].empty()) {
        this->return_uri = loc_b.dir[D_RETURN].front();
    }
    if (!loc_b.dir[D_UPLOAD].empty()) {
        this->return_uri = loc_b.dir[D_UPLOAD].front();
    }
    for (string_vector::const_iterator it = loc_b.dir[D_INDEX].begin(); it != loc_b.dir[D_INDEX].end(); it++) {
        if (std::find(this->index.begin(), this->index.end(), *it) == this->index.end()) {
            this->index.push_back(*it);
        }
    }
    for (string_vector::const_iterator it = loc_b.dir[D_METHOD].begin(); it != loc_b.dir[D_METHOD].end(); it++) {
        if (std::find(this->accept_method.begin(), this->accept_method.end(), *it) == this->accept_method.end()) {
            this->accept_method.push_back(*it);
        }
    }
    for (string_vector::const_iterator it = loc_b.dir[D_CGI_PASS].begin(); it != loc_b.dir[D_CGI_PASS].end(); it++) {
        if ((it + 1) == loc_b.dir[D_CGI_PASS].end()) throw std::runtime_error("si ves esto el parser está roto\n");
        cgi_pass.push_back(std::pair<std::string, std::string>(*it, *++it));
    }
    this->body_size = std::atoi(loc_b.dir[D_BODY_SIZE].front().c_str());
    this->autoindex = !loc_b.dir[D_AUTOINDEX].front().compare("on") ? true : false;
}

Location::~Location() { }

Location& Location::operator=(const Location& other) {
    if (this == &other) {
        return *this;
    }
    uri = other.uri;
    error_page = other.error_page;
    root = other.root;
    return_uri = other.return_uri;
    upload_path = other.upload_path;
    index = other.index;
    accept_method = other.accept_method;
    cgi_pass = other.cgi_pass;
    body_size = other.body_size;
    autoindex = other.autoindex;
    return *this;
}

const std::string& get_error_page(void) const {
    return error_page;
}
const std::string& get_root(void) const {
    return root;
}
const std::string& get_return_uri(void) const {
    return return_uri;
}
const std::string& get_upload_path(void) const {
    return upload_path;
}

const std::vector<std::string>& get_index(void) const {
    return index;
}        
const std::vector<std::string>& get_methods(void) const {
    return accept_method;
}

const std::pair<std::string, std::string>& get_cgi_pass(void) const {
    return cgi_pass;
}

unsigned int get_body_size(void) const {
    return body_size;
}
bool get_autoindex(void) const {
    return autoindex;
}