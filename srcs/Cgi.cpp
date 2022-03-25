#include "Cgi.hpp"

/* test perl script! */

std::string CGI::buildCGIPath(const std::string& relPath, const std::string& cwd, const Location& loc)
{
	std::string absPath(cwd);

	if (relPath.empty())
		return "";
	if (relPath.at(0) != '/')
	{
		absPath.append(loc.get_root());
		if (absPath.at(absPath.size() - 1) != '/')
			absPath.push_back('/');
	}
	absPath.append(relPath);
	return absPath;
}

void CGI::close_fdIN(void) {
    if (_fdIN[0] != -1) {
        close(_fdIN[0]);
        _fdIN[0] = -1;
    }
    if (_fdIN[1] != -1) {
        close(_fdIN[1]);
        _fdIN[1] = -1;
    }
}

void CGI::close_fdOut(void) {
    if (_fdOut[0] != -1) {
        close(_fdOut[0]);
        _fdOut[0] = -1;
    }
    if (_fdOut[1] != -1) {
        close(_fdOut[1]);
        _fdOut[1] = -1;
    }
}

void CGI::set_env_variables(const std::string& uri, const std::string& file_ext) {
	std::string tmpBuf;
	int i = 0;

	_envvar[i++] = strdup(("PATH_INFO=" + uri).c_str());
	_envvar[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
    // used for php-cgi
	_envvar[i++] = strdup("REDIRECT_STATUS=200");
	if (_req->getMethod() == GET){

        // stupid bug in php-cgi
		if (!file_ext.compare(".php"))
            _envvar[i++] = strdup("REQUEST_METHOD=GET");
		
		tmpBuf = "QUERY_STRING=" + _req->getQuery();
		_envvar[i++] = strdup(tmpBuf.c_str());
	} else {
        // stupid bug in php-cgi
		if (!file_ext.compare(".php"))
            _envvar[i++] = strdup("REQUEST_METHOD=POST");	
		
		std::stringstream intToString;
		intToString << _req->getBody().getBody().size();
		tmpBuf = std::string("CONTENT_LENGTH=") + intToString.str();
		_envvar[i++] = strdup(tmpBuf.c_str());
	}
    _envvar[i] = NULL;
}

void CGI::set_args(const std::string& uri, const std::string& cgi_path) {
	_args[0] = (cgi_path.empty()) ? strdup(uri.c_str()) : strdup(cgi_path.c_str());
	_args[1] = (cgi_path.empty()) ? NULL : strdup(uri.c_str());
	_args[2] = NULL;
}

void CGI::set_path_info(const std::string& resource_path) {
    size_t path_separator = resource_path.rfind('/');

    if (path_separator == std::string::npos) {
        _path_info = "/";
        return ;
    }
    _path_info = resource_path.substr(0, path_separator);
}

// evitar headers redundantes (no imprimir en Response headers ya definidos en CGI)
// Location -> server redirection
// Content-Length policing??

/*  Da formato a la respuesta recibida por el CGI y comprueba posibles errores sintácticos:
	* Una respuesta proveniente de un CGI ha de tener al menos un CGI-Header; si
	  no está presente el servidoro se repiten lanza respuesta 500 (internal server error)
	* CGI-Header: Content-Type / Location / Status
	* Content-Type DEBE estar presente si existe un cuerpo
	* La presencia de un header Location genera una respuesta 302 (Found)
	* Location puede ser una redirección por parte del servidor o del cliente
	* La presencia de un header Status sobreescribe el estado anterior de la respuesta
	* El código de estado por defecto es 200 OK
*/
void CGI::parse_response_headers(const std::string& headers) {
	static const std::string cgi_header[3] = { "Content-Type", "Location", "Status"};

	std::stringstream headers_ss(headers);
	std::string header_line;

	while (std::getline(headers_ss, header_line)) {
		size_t separator = header_line.find(':');

		if (separator == std::string::npos) {
			continue ;
		}
		std::string header_field(header_line.substr(0, separator++));
		std::string header_value(header_line.substr(separator, header_line.find_first_not_of(' ', separator)));

		if (header_value.empty()) {
			continue ;
		}
		for (int i = 0; i < 3; i++) {
			if (!header_field.compare(cgi_header[i])) {
				if (_header_map.find(header_field) != _header_map.end()) {
					throw (StatusLine(500, REASON_500, "CGI: parse(), duplicate CGI-header in CGI response"));
				}
				break ;
			}
		}
		_header_map.insert(std::pair<std::string, std::string>(header_field, header_value));
	}
	if (_header_map.find(cgi_header[0]) == _header_map.end() &&
		_header_map.find(cgi_header[1]) == _header_map.end() &&
		_header_map.find(cgi_header[2]) == _header_map.end()) {
		throw (StatusLine(500, REASON_500, "CGI: parse(), missing necessary CGI-Header in response"));
	}
}

void CGI::parse_response_body(const std::string& body) {
	if (!body.empty() && _header_map.find("Content-Type") == _header_map.end()) {
		throw (StatusLine(500, REASON_500, "CGI: parse(), missing body in response with Content-Type header defined"));
	}
	std::map<std::string, std::string>::const_iterator cl = _header_map.find("Content-Length");
	if (cl != _header_map.end()) {
		char *ptr;

		long length = strtol(cl->second.c_str(), &ptr, 0);
		if (length < 0 || *ptr || length != static_cast<long>(body.size())) {
			throw (StatusLine(500, REASON_500, "CGI: parse(), bad content-length definition in CGI response"));
		}
	}
	_body_string = body;
}

/* Status = "Status:" status-code SP reason-phrase NL */

void CGI::parse_status_line(void) {
	enum status_f {code, reason, size};
	std::map<std::string, std::string>::const_iterator lc(_header_map.find("Location"));
	std::map<std::string, std::string>::iterator st(_header_map.find("Status"));

	if (lc != _header_map.end()) {
		_status_line = StatusLine(302, REASON_302, "CGI: redirection");
		return ;
	}
	if (st != _header_map.end()) {
		std::stringstream status_ss(st->second);
		std::string status_token[size];
		char *ptr;

		for (int i = 0; i < 2; i++) {
			status_ss >> status_token[i];
		}
		long status_code = strtol(status_token[code].c_str(), &ptr, 0);

		if (status_ss.rdbuf()->in_avail() || !(status_code >= 100 && status_code < 600) || *ptr) {
			throw StatusLine(500, REASON_500, "CGI: parse(), malformed Status-Header");
		}
		_status_line = StatusLine(static_cast<int>(code), status_token[reason].c_str(), "CGI-defined Status line");
		_header_map.erase(st);
	}
}

/*
 * encuentra separador headers & body
 * substr headers / body
 *  	headers: while (field ':' value NL)
 *	             comprueba sintaxis de la respuesta {al menos un CGIHeader, no repetidos}
 * 		body: si !body.empty() && !content-type -> 500
 *		      si content-length && !(body.size() == content-length) -> 500
 */
void CGI::parse_response(void) {

	size_t separator = _raw_response.find("\n\n");

	if (separator == std::string::npos) {
		throw (StatusLine(500, REASON_500, "CGI: parse(), bad syntax in CGI repsonse"));
	}
	std::string headers = _raw_response.substr(0, separator);
	std::string body = _raw_response.substr(++separator);

	parse_response_headers(headers);
	parse_response_body(body);
	parse_status_line();
}

CGI::CGI(Request *req, const std::string& uri, const cgi_pair& cgi_info) :
	_req(req), _status_line(200, REASON_200, "CGI-generated response") {
	/* inicializa atributos de CGI (variables de entorno, argumentos) */
    for (int i = 0; i < 2; i++) {
        _fdIN[i] = -1;
        _fdOut[i] = -1;
    }

	// GET : QUERY_STRING + PATH_INFO 
	// POST : PATH_INFO + CONTENT_length 
    std::cerr << "[test] " << _fdIN[0] << ", " << _fdIN[1] << "\n";
	if ((_envvar = new char*[7]) == NULL) {
		throw std::runtime_error("Error on a cgi malloc\n");
	}
	// ** Set args for execve **
	if ((_args = new char*[3]) == NULL) {
		throw StatusLine(500, REASON_500, "malloc failed in CGI constructor");
	}

	/* llamada a getcwd: cambio de malloc a reservar memoria en stack por problemas de fugas de memoria */
	char cwd[PWD_BUFFER];
	if (getcwd(cwd, PWD_BUFFER) == NULL) {
		throw std::runtime_error("CGI: getcwd() call failure");
	}

	std::string resource_path(std::string(cwd) + "/" + uri); /* ruta absoluta al recurso */
	std::string cgi_path = buildCGIPath(cgi_info.second, cwd, _req->getLocation()); /* ruta absoluta al ejecutable */

	set_env_variables(resource_path, cgi_info.first);
	set_args(resource_path, cgi_path);
    set_path_info(resource_path);
}

CGI::~CGI()
{
	int i = -1;
	while (_envvar[++i]){
		free(_envvar[i]); _envvar[i] = NULL;}
	delete[] _envvar;
	
	i = -1;
	while (_args[++i]){
		free(_args[i]); _args[i] = NULL;}
	delete[] _args;

    close_fdIN();
    close_fdOut();
}

void CGI::executeCGI()
{
	int fdOut[2];
	int fdIN[2];
	
	if (pipe(fdOut) < 0 || pipe(fdIN) < 0)
		throw StatusLine(500, REASON_500, "pipe failed in executeCGI method");
	
	pid_t pid = fork();
    if (pid == -1) {
        throw StatusLine(500, REASON_500, "CGI: fork() call error");
    }
	if (!pid){
		
		// stdout is now a copy of fdOut[1] and in case post method, stdin is a copy of fdIn[0]
		dup2(fdOut[1], STDOUT_FILENO);
		close(fdOut[0]);
		close(fdOut[1]);

		dup2(fdIN[0], STDIN_FILENO);
		close(fdIN[0]);
		close(fdIN[1]);

		// change the repo into where the program is
		if (chdir(_path_info.c_str()) == -1) {
            std::cerr << "[CGI error] chdir(): " << strerror(errno) << "\n";
            exit(EXECVE_FAIL);
        }
	
		if (execve(_args[0], _args, _envvar) < 0){
            std::cerr << "[debug]\n" << strerror(errno) << "\n";
            std::cerr << _args[0] << "\n";
            std::cerr << _args[1] << "\n";
            std::cerr << _args[2] << "\n";
			exit(EXECVE_FAIL);
		}
	
	}
	close(fdOut[1]);
	if (_req->getMethod() == POST){
		if (write(fdIN[1], _req->getBody().getBody().c_str(), _req->getBody().getBody().size()) < 0)
			throw StatusLine(500, REASON_500, "write failed in executeCGI method");
	}
	close_fdIN();
    char buf[CGI_PIPE_BUFFER_SIZE + 1] = {0};
    int rd_out;

	while ((rd_out = read(fdOut[0], buf, CGI_PIPE_BUFFER_SIZE)) > 0)
	{
		_raw_response.append(buf, rd_out);
		memset(buf, 0, CGI_PIPE_BUFFER_SIZE + 1);
	}
    if (rd_out == -1) {
        throw StatusLine(500, REASON_500, "CGI: read() - " + strerror(errno));
    }
    close_fdOut();

    // Checking if execve correctly worked
    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == EXECVE_FAIL)
    {
       throw StatusLine(500, REASON_500, "execve failed in executeCGI method");
    }
	_raw_response.append(buf, rd_out);
}

std::string CGI::getHeaders(void) const {
	std::string header_string;

	for (std::map<std::string, std::string>::const_iterator h = _header_map.begin(); h != _header_map.end(); h++) {
		header_string.append(h->first + ": " + h->second + "\n");
	}
	/* si escribimos el doble salto de línea aquí, llamamos a esta función después de escribir los
	 * headers de la respuesta en execCGI */
	header_string.append("\n");
	return header_string;
}

std::string CGI::getBody(void) const {
	return _body_string;
}

const StatusLine& CGI::getStatusLine(void) const {
	return _status_line;
}

bool CGI::isHeaderDefined(const std::string& header_field) const {
	return (_header_map.find(header_field) != _header_map.end());
}
 
void CGI::mySwap(CGI &a, CGI &b)
{
	std::swap(a._envvar, b._envvar);
	//std::swap(a._emptyBody, b._emptyBody);
	std::swap(a._req, b._req);
	//std::swap(a._exec, b._exec);
	std::swap(a._path_info, b._path_info);
}

CGI::CGI(void) :  _status_line(200, REASON_200, "CGI-generated response") {
    _fdIN[0] = -1;
    _fdIN[1] = -1;

    _fdOut[0] = -1;
    _fdOut[1] = -1;
}

CGI::CGI(const CGI& other) : _req(other._req){ 
	// paciencia con este
}
