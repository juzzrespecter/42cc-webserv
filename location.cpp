#include "location.hpp"

cgi_pass_directive_t::cgi_pass_directive_t(void) { }

cgi_pass_directive_t::cgi_pass_directive_t(const cgi_pass_directive_t& other) :
    cgi_file_ext(other.cgi_file_ext), cgi_path(other.cgi_path) { }

cgi_pass_directive_t::cgi_pass_directive_t(const std::string& cgi_file_ext_, const std::string& cgi_path_) :
    cgi_file_ext(cgi_file_ext_), cgi_path(cgi_path_) { }

cgi_pass_directive_t& cgi_pass_directive_t::operator=(const cgi_pass_directive_t& other) {
    if (this == &other)  {
        return *this;
    }
    this->cgi_path = other.cgi_path;
    this->cgi_file_ext = other.cgi_file_ext;
    return *this;
}

location::location(void) { }

location::location(const location& other) {
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
}

location::location(const location_block_t& loc_blk) {
    typedef std::vector<std::string> string_vector;

    this->uri = loc_blk.uri;

    if (!loc_blk.dir[D_ERROR_PAGE].empty()){
        this->error_page = loc_blk.dir[D_ERROR_PAGE].front();
    }
    this->root = loc_blk.dir[D_ROOT].front();
    if (!loc_blk.dir[D_RETURN].empty()) {
        this->return_uri = loc_blk.dir[D_RETURN].front();
    }
    if (!loc_blk.dir[D_UPLOAD].empty()) {
        this->return_uri = loc_blk.dir[D_UPLOAD].front();
    }
    for (string_vector::const_iterator it = loc_blk.dir[D_INDEX].begin(); it != loc_blk.dir[D_INDEX].end(); it++) {
        if (std::find(this->index.begin(), this->index.end(), *it) == this->index.end()) {
            this->index.push_back(*it);
        }
    }
    for (string_vector::const_iterator it = loc_blk.dir[D_METHOD].begin(); it != loc_blk.dir[D_METHOD].end(); it++) {
        if (std::find(this->accept_method.begin(), this->accept_method.end(), *it) == this->accept_method.end()) {
            this->accept_method.push_back(*it);
        }
    }
    /* cgi pass directive constructor */
    this->body_size = std::atoi(loc_blk.dir[D_BODY_SIZE].front().c_str());
    this->autoindex = !loc_blk.dir[D_AUTOINDEX].front().compare("on") ? true : false;
}

location::~location() { }