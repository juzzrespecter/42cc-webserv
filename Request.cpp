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

Request::Request() : stage(request_line_stage) { }

Request::Request(const std::vector<Server*> vservVec) :
    _vservVec(vservVec), stage(request_line_stage) { }

Request::Request(const Request& c) { 
    *this = c;
}

Request::~Request() { }

Request& Request::operator=(Request a) { 
    if (this == &a) {
        return *this;
    }
    swap(*this, a);
    return *this;
}

const RequestLine& Request::getRequestLine() const { }
    return _reqLine;
}
const header_map& Request::getHeaders() const {
    return _headers;
}

const Body& Request::getBody() const {
    return _body;
}

const std::string& Request::getBuffer() const {
    return _buffer;
}

int Request::getMethod() const {
    return _reqLine.getMethod();
}

const std::string& Request::getPath() const {
    return _reqLine.getPath();
}
const std::string& Request::getQuery() const {
    return _reqLine.getQuery();
}

void Request::setPath(const std::string& path) {
    _reqLine.setPath(path);
}
		
void swap(Request& a, Request& b) {
   swap(a._buffer, b._buffer);
   swap(a._index, b._index);
   swap(a._vservVec, b._vservVec);
   swap(a._headerCount, b._headerCount);
   swap(a._reqLine, b._reqLine);
   swap(a._headers, b._headers);
   swap(a._body, b._body);
   swap(a._stage, b._stage);
}

void    Request::recvBuffer(const std::string& newBuffer) {
    _buffer.append(newBuffer);

    if (_stage == request_line_stage) {
        parseRequestLine();
    }
    while (_stage == header_stage && _buffer[_index]) {
        parseHeaderLine();
        if (_stage != header_stage) {/* move to header checking and body setup */
            headerMeetsRequirements();
            setUpRequestBody();
        }
    }
    if (_stage == request_body_stage) {
        if (transferEncodingIsChunked() == true) {
            parseChunkedRequestBody();
        } else {
            parseRequestBody();
        }
    }
}

void    Request::parseRequestLine(void) {
    std::string requestLine = _buffer.substr(_index, _buffer.find(CRLF, _index));

    _index += requestLine.size();
    if (requestLine.empty() || !_buffer[_index]) {
        THROW_STATUS("syntax error in request-line");
    }
    std::stringstream   lineBuffer(requestLine);
    std::string         token[3];

    for (int i = 0; i < 3; i++) {
        lineBuffer >> token[i];
    }
    if (!lineBuffer.rdbuf()->in_avail()) {
        THROW_STATUS("syntax error in request-line");
    }
    parseMethodToken(token[0]);
    parseURI(token[1]);
    parseHTTPVersion(token[2]);
}

void    Request::parseMethodToken(const std::string& token) {
    std::string method_opts[NB_METHODS] = {"GET", "HEAD", "POST", "DELETE"};

    for (int method_id = 0; method_id < NB_METHODS; method_id++) {
        if (!token.compare(method_opts[method_id])) {
            _reqLine.setMethod(method_id);
            return ;
        }
    }
    THROW_STATUS("unknown method in request");
}

void    Request::parseURI(const std::string& token) {
    std::string allowed_ptcl[2] = { "http://", "https://" };
    size_t queryPos = token.find('?');

    if (queryPos != std::string::npos) {
            _reqLine.setQuery(token.substr(++queryPos));
            token = token.substr(0, --queryPos);
    }
    for (size_t i = 0; i < 2; i++) {
        if (!token.compare(0, allowed_ptcl[i].size(), allowed_ptcl[i])) {
            std::string hostname = token.substr(0, token.find("/", allowed_ptcl[i].size()));
            std::string abs_path = token.substr(hostname.size());

            _headers.insert(std::pair<std::string, std::string>("Host", hostname));
            if (abs_path.empty()) {
                abs_path.append("/");
            }
            _reqLine.setPath(abs_path);
            return ;
        }
    }
    if (token[0] == '/') {
        _reqLine.setPath(token);
        return ;
    }
    THROW_STATUS("bad URI format");
}

void    Request::parseHTTPVersion(const std::string& token) {
    if (token.compare("HTTP/1.1")) {
        THROW_STATUS("unknown or not supported HTTP version");
    }
}

void    Request::parseHeaderLine(void) {
    std::string headerLine = _buffer.substr(_index, _buffer.find(CRLF));

    if (headerLine.empty()) {
        _stage = (_reqLine.getMethod() == POST) ? request_body_stage : request_is_ready;
        return ;
    }
    _index += headerLine.size();
    if (!_buffer[_index]) {
        THROW_STATUS("syntax error on request");
    }
    std::string fieldName = headerLine.substr(0, headerLine.find(':'));
    std::string fieldValue = headerLine.substr(fieldName.size() + 1);

    if (fieldValue.empty()) {
        return ;
    }
    if (_headerCount++ > HEADER_LIM) {
        THROW_STATUS("too many headers on request");
    }
    for (int i = 0; i < N_HEADER_MAX; i++) {
        if (!fieldName.compare(header::hlist[i])) {
            _headers.insert(std::pair<std::string, std::string>(fieldName, fieldValue));
            return ;
        }
    }
}

void    Request::headerMeetsRequirements(void) const {
    header_map::const_iterator hd = _headers.find("Host");
    header_map::const_iterator cl = _headers.find("Content-Length");
    
    /* debe estar presente un Host definido en la request */
    if (hd == _headers.end()) {
        THROW_STATUS("requested host not defined");
    }

    /* si existe un header Content-Length definido, su field-value ha de ser un numero positivo */
    if (cl != _headers.end()) {
        char *ptr;

        int contentLengthVal = strtol(cl->second, &ptr, 0);
        if (contentLengthVal < 0 || *ptr) {
            THROW_STATUS("invalid value for header Content-Length");
        }
    }
}

void    Request::setUpRequestBody(void) {
    find_server_by_host comp(host());
    header_map::iterator cl = _headers.find("Content-Length");

    _body.setMaxSize(findMaxSize());
    if (cl != _headers.end()) {
        _body.setSize(std::atoi(cl->second));
    }
}


/* casos:
       Transfer-Encoding header no seteado: una única lectura.
       T-E seteado a 'chunked': varias lecturas hasta recibir el chunk final.

       Si en una lectura el chunk o el msg-body no corresponde con el valor del tamaño 
       en la cabezera del chunk (primer caso) o la cabezera Content-Type (segundo), throw 400

       Si max_size == set y content-type > max_size, throw
    */
void    Request::parseChunkedRequestBody(void) {        /* to do */
    /* parseo sizeOfChunk CRLF Chunk CRLF */
    /* ultimo chunk 0 CRLF CRLF */
    {
        std::string sizeChunkStr = _buffer.substr(_index, _buffer.find(CRLF));
        size_t  sizeChunk = std::atoi(sizeChunkStr);
        /* case sizeChunk == 0 */
        _index += sizeChunkStr.size();
        // _buffer[_index] == 0
    }
    {
        std::string chunk = _buffer.substr(_index, _buffer.find(CRLF));

        if (chunk.size() != sizeChunk) {
            THROW_STATUS("...");
        }
        _body.recvBuffer(chunk);
    }

}

void    Request::parseChunkedBody(void) {
    std::string bodyBuffer = _buffer.substr(_index);

    _index += _bodyBuffer.size();
    _body.recvBuffer(bodyBuffer);
    if (_body.getBuffer().size() > contentLength()) {
        THROW_STATUS("request body overflows content-length limit");
    }
}

long Request::findMaxSize(void) const {
    std::vector<Server*>::const_iterator it = /
        std::find(_vservVec.begin(), _vservVec.end(), find_server_by_host(host()));
    Server* server_host = (it == _vservVec.end()) ? _vservVec[0] : *it;

    return server_host->get_location_by_path(getPath()).getBodySize();
}

bool    Request::transferencodingIsChunked(void) const {
    header_map::const_iterator te = _headers.find("Transfer-Encoding");
    header_map::const_iterator 

    if ((te != _headers.end() && !te->second.compare("chunked")))
}

const std::string&  Request::host(void) const {
    header_map::const_iterator h = _headers.find("Host");

    return (h != _headers.end()) ? h->second : "";
}

/* Undefined behavior if used before safety check in headerMeetsRequirements */
size_t  Request::contentLength(void) const {
    header_map::const_iterator cl = _headers.find("Content-Length");

    if (cl != _headers.end()) {
        return std::atoi(cl->second);
    }
    return 0;
}




