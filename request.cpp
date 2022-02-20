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

/*Request::Request(const std::vector<Server*> infoVirServs) { */

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

bool newLineReceived(size_t posCLRF) {

}

void parseRequestLine(size_t posCLRF);

void Request::parseRequestLine(size_t posCRLF) {
    std::string request_line = _buffer.substr(posCRLF, _buffer.find(CRLF));

    parseMethodToken();
    parseURI();
    parseHTTPVersion();

    if more tokens in request_line {
        throw StatusLine(400, REASON_400, "syntax error on request-line");
    }

    _reqLine = /*tal*/;
}

void parseMethodToken(const std::string& token) {
    std::string method_list[NB_METHODS] = { "HEAD","GET","POST","DELETE" };
    int id = 0;

    for (; id < NB_METHODS; id++) {
        if (!token.compare(method_list[id])) break ;
    }
    if (id == NB_METHODS) {
        throw StatusLine(400, REASON_400, "syntax error on METHOD token");
    }
}

void parseURI(std::string token) {
    /* check uri syntax: {regular URI, absolute-URI} */
    /* si es absolute-URI, pillar el Host y meterlo en el mapa de headers */
}


void parseHTTPVersion(const std::string& token) {
    if (token.compare("HTTP/1.1")) {
        throw StatusLine(400, REASON_400, "syntax error on HTTP-version token");
    }
}
        
void parseHeaderLine(size_t posCRLF) {
    /* muy permisivo, la única condición es que termine en CRLF */
    /* si está mal formateado, o es un header desconocido, ignorar */
}
		
void parseBody() {
    /* casos:
       Transfer-Encoding header no seteado: una única lectura.
       T-E seteado a 'chunked': varias lecturas hasta recibir el chunk final.

       Si en una lectura el chunk o el msg-body no corresponde con el valor del tamaño 
       en la cabezera del chunk (primer caso) o la cabezera Content-Type (segundo), throw 400

       Si max_size == set y content-type > max_size, throw
    */
}
		
long findMaxSize(const std::string& hostName) {

}
		
void swap(Request& a, Request& b) {

}
		