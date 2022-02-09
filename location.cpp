#include "Location.hpp"

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
        cgi_pass.push_back(cgi_pass_directive_t(*it, *++it));
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

/* 
 * Primero comprueba que el método existe, después comprueba que está dentro de los permitidos
 * conforme a la configuración del servidor.
 * 
 * Lanza una respuesta predeterminada Not Implemented 501 si el método no está definido,
 * y una respuesta predeterminada Method Not Allowed 405 si el método existe pero
 * no se encuentra dentro del vector de métodos permitidos.
 */
std::string Location::select_requested_method(const std::string& request) const {
    static const std::string method_arr[N_METHODS] = {"GET", "POST", "DELETE"};
    method_options  option_arr[N_METHODS] = {
        Location::http_method_get,
        Location::http_method_post,
        Location::http_method_delete
    };
    std::string req_method = get_method_from_request(request);
    /* std::string req_method = request.method() */
    int method_id;

    for (method_id = 0; method_id < N_METHODS; method_id++) {
        if (!req_method.compare(method_arr[method_id])) break ;
    }
    if (method_id == N_METHODS) {
        return /* NOT IMPLEMENTED */;
    }
    if (std::find(accept_method.begin(), accept_method.end(), req_method) != accept_method.end()) {
        return /* METHOD NOT ALLOWED */;
    }
    return (this->*option_arr[method_id])(request);
}