#include "config_blocks.hpp"

location_block_t::location_block_t(void) { }

location_block_t::location_block_t(const std::string& _uri) : uri(_uri) { }

location_block_t::location_block_t(const location_block_t& other) : 
    uri(other.uri) { 
    *this = other;
}

location_block_t& location_block_t::operator=(const location_block_t& other) {
    if (this == &other) {
        return *this;
    }
    uri = other.uri;
    for (int i = 0; i < N_DIR_LOC; i++) {
        dir[i] = other.dir[i];
    }
    return *this;
}

server_block_t::server_block_t(void) { }

server_block_t::server_block_t(const server_block_t& other) { 
    *this = other;
}

server_block_t& server_block_t::operator=(const server_block_t& other) {
    if (this == &other) {
        return *this;
    }
    loc = other.loc;
    for (int i = 0; i < N_DIR_SRV; i++) {
        dir[i] = other.dir[i];
    }
    return *this;
}

void    server_block_t::setup_default_directives(void) {
    static const std::string default_dir[N_DIR_LOC - 4] = {"0","./html/","off","index.html","GET"};
    for (int i = 0; i < N_DIR_LOC - 4; i++) {
        if (dir[i].empty()) {
            dir[i].push_back(default_dir[i]);
        }
    }
    for (std::vector<location_block_t>::iterator it = loc.begin(); it != loc.end(); it++) {
        location_inherits_from_server(*it);
    }
}

void    server_block_t::location_inherits_from_server(location_block_t& loc) {
    for (int i = 0; i < 3; i++) {
        if (loc.dir[i].empty()) {
            loc.dir[i].push_back(dir[i].front());
        }
    }
    if (loc.dir[D_INDEX].empty()) {
        loc.dir[D_INDEX].insert(loc.dir[D_INDEX].begin(), dir[D_INDEX].begin(), dir[D_INDEX].end());
    }
    if (loc.dir[D_METHOD].empty()) {
        loc.dir[D_METHOD].insert(loc.dir[D_METHOD].begin(), dir[D_METHOD].begin(), dir[D_METHOD].end());
    }
    if (loc.dir[D_UPLOAD].empty() && !dir[D_UPLOAD].empty()) {
        loc.dir[D_UPLOAD].push_back(dir[D_UPLOAD].front());
    }
    if (loc.dir[D_RETURN].empty() && !dir[D_RETURN].empty()) {
        loc.dir[D_RETURN].push_back(dir[D_RETURN].front());
    }
    if (loc.dir[D_CGI_PASS].empty() && !dir[D_CGI_PASS].empty()) {
        loc.dir[D_CGI_PASS].insert(loc.dir[D_CGI_PASS].begin(), dir[D_CGI_PASS].begin(), dir[D_CGI_PASS].end());
    }
    if (loc.dir[D_ERROR_PAGE].empty() && !dir[D_ERROR_PAGE].empty()) {
        loc.dir[D_ERROR_PAGE].push_back(dir[D_ERROR_PAGE].front());
    }
}

server_block_t::operator location_block_t(void) const {
    location_block_t default_loc("/");

    for (int i = 0; i < N_DIR_LOC; i++) {
        default_loc.dir[i].insert(default_loc.dir[i].begin(), dir[i].begin(), dir[i].end());
    }
    return default_loc;
}