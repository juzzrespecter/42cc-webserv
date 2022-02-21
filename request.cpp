#include "Request.hpp"

/* REQUEST FORMAT:
        request-line
        *((header-line) CRLF)
        CRLF
        [ message-body ]
*/

/* el único header obligatorio en peticiones para HTTP 1.1 es el header HOST, que también puede ser 
 * declarado de forma implícita en la URI de la petición si está formateada como Absolute-URI
 * Absolute-URI = "http://" + host + /
 * 
 * ¿qué headers aceptamos?
 * Todos ??
 */

/* al final del parseo de headers hay que comprobar si está seteado Host; si no, matamos request*/

Request::Request() { }

Request::Request(const std::vector<Server*> infoVirServs) { }

Request::Request(const Request& c) { }

Request::~Request() { }

Request& Request::operator=(Request a) { 
    /* swap requests */
}

const RequestLine& Request::getRequestLine() const { }

}
const std::map<std::string, std::string>& Request::getHeaders() const {
    return _headers;
}

const Body& Request::getBody() const {
    return _body;
}

const std::string& Request::getBuffer() const {
    return _buffer;
}

int Request::getMethod() const {

}

const std::string& Request::getPath() const {
    return 
}
const std::string& Request::getQuery() const {

}

void Request::setPath(const std::string& path) {

}

bool Request::newLineReceived(size_t posCLRF) {

}

void Request::parseRequestLine(size_t posCLRF);

void Request::parseRequestLine(size_t posCRLF) {
    std::string request_line = _buffer.substr(posCRLF, _buffer.find(CRLF));


    parseMethodToken();
    parseURI();
    parseHTTPVersion();

    if more tokens in request_line {
        throw StatusLine(400, REASON_400, "syntax error on request-line");
    }
}

void Request::parseMethodToken(const std::string& token) {
    std::string method_list[NB_METHODS] = { "HEAD","GET","POST","DELETE" };
    int id = 0;

    for (; id < NB_METHODS; id++) {
        if (!token.compare(method_list[id])) break ;
    }
    if (id == NB_METHODS) {
        throw StatusLine(400, REASON_400, "syntax error on METHOD token");
    }
    _reqLine.setMethod(id);
    _index += token.size();
}

void Request::parseURI(std::string token) {
    /*
        absolureURI -> http://[...][/abs_path], if ![abs_path], then abs_path == '/'
        abs_path -> /[...] 
    */
    /* si es absolute-URI, pillar el Host y meterlo en el mapa de headers */

    std::string abs_path;
    std::string host;

    if (!token.compare(0, sizeof("http://"), "http://")) {
        host = token.substr(0, token.find('/'));
        abs_path = token.substr(token.find(sizeof("http://"), '/'));
    } else if (token[0] != '/') {
        throw StatusLine(400, REASON_400, "syntax error on request-URI");
    } else {
        abs_path = token;
    }
    if (!host.empty()) {
        _headers.insert(std::pair<std::string, std::string>("Host", host));
    }
    _reqLine.setPath(abs_path);
    _index += token.size();
}


void Request::parseHTTPVersion(const std::string& token) {
    if (token.compare("HTTP/1.1")) {
        throw StatusLine(400, REASON_400, "syntax error on HTTP-version token");
    }
    _index += token.size();
}
        
void Request::parseHeaderLine(size_t posCRLF) {
    /* 
     * muy permisivo, la única condición es que termine en CRLF
     * si está mal formateado, o es un header desconocido, ignorar 
    */

    std::string header = _buffer.substr(posCRLF, _buffer.find(CRLF, posCRLF));

    std::string field_name = header.substr(0, header.find(':'));
    std::string field_value = header.substr(header.find(':') + 1);
    if (field_value.empty()) {
        return ;
    }
    for (int i = 0; i < N_HEADER_MAX; i++) {
        if (!field_name.compare(header::hlist[i])) {
            _headers.insert(std::pair<std::string, std::string>(field_name, field_value));
            return ;
        }
    }
}
		
void Request::parseBody() {
    /* casos:
       Transfer-Encoding header no seteado: una única lectura.
       T-E seteado a 'chunked': varias lecturas hasta recibir el chunk final.

       Si en una lectura el chunk o el msg-body no corresponde con el valor del tamaño 
       en la cabezera del chunk (primer caso) o la cabezera Content-Type (segundo), throw 400

       Si max_size == set y content-type > max_size, throw
    */
}
		
long Request::findMaxSize(const std::string& hostName) {
    /* encuentra servidor, encuentra ruta, pilla body_size */
    find_server_by_host comp_srv(hostName);

    std::vector<Server*>::const_iterator server_it = std::find(_infoVirServs.begin(), _infoVirServs.end(), comp_srv);
    Server* server_host = (server_it == _infoVirServs.end()) ? _infoVirServs[0] : *server_it;

    /* algoritmo de selección de ruta, tanto para aquí como para la respuesta */
}
		
void swap(Request& a, Request& b) {

}
		