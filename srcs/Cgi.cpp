#include "Cgi.hpp"

CGI::CGI(Body *body, Request *req, const std::string &realUri, const cgi_pair& cgi_info) 
			: _emptyBody(body), _req(req)
{	
	std::string tmpBuf;
    std::string cgi_name = cgi_info.first;
    std::string cgi_path = cgi_info.second;
	
	char* pwd = new char[PWD_BUFFER]; // needs to be freed
	if (!(pwd = getcwd(pwd, PWD_BUFFER)))
		throw std::runtime_error("Error in getcwd command in cgi constructor\n");

	_realUri = pwd + ("/" + realUri);
    delete pwd;

	if (!cgi_name.compare(".cgi"))
		//_exec = "cgi";
        _exec = _realUri;
	else
		_exec = cgi_name;
	
	if (cgi_name.compare(".cgi"))
		_openArgfile.open(_realUri.c_str());

	// ** set environment variable for the CGI **
	// GET : QUERY_STRING + PATH_INFO 
	// POST : PATH_INFO + CONTENT_length 
	if ((_envvar = new char*[7]) == NULL)
		throw std::runtime_error("Error on a cgi malloc\n");
		
	int i = 0;
	_envvar[i++] = strdup(("PATH_INFO=" + _realUri).c_str());
	_envvar[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");

    // used for php-cgi
	_envvar[i++] = strdup("REDIRECT_STATUS=200");
	
	if (_req->getMethod() == GET){

        // stupid bug in php-cgi
        if (!_exec.compare("php-cgi"))
            _envvar[i++] = strdup("REQUEST_METHOD=GET");
		
		tmpBuf = "QUERY_STRING=" + _req->getQuery();
		_envvar[i++] = strdup(tmpBuf.c_str());

		if (cgi_path.compare(".cgi")) // ------------------------------------ !
		{
			while (std::getline(_openArgfile, tmpBuf))
				_getBuffFile += tmpBuf;
			
			std::stringstream intToString;
			intToString << _getBuffFile.size();
			
			tmpBuf = std::string("CONTENT_LENGTH=") + intToString.str();
			_envvar[i++] = strdup(tmpBuf.c_str());
            _openArgfile.close();
		} // -------------------------------------- no entiendo este bloque - !

	}
	
	else {
        
        // stupid bug in php-cgi
        if (!_exec.compare("php-cgi"))
            _envvar[i++] = strdup("REQUEST_METHOD=POST");	
		
		std::stringstream intToString;
		intToString << _req->getBody().getBody().size();
		tmpBuf = std::string("CONTENT_LENGTH=") + intToString.str();
		_envvar[i++] = strdup(tmpBuf.c_str());
	}
	
	_envvar[i++] = NULL;

	// ** Set args for execve **
	if ((_args = new char*[3]) == NULL)
		throw StatusLine(500, REASON_500, "malloc failed in CGI constructor");
		
	_args[0] = strdup(_exec.c_str()); // ruta absoluta a cgi
	_args[1] = (!cgi_name.compare(".cgi")) ? NULL : strdup(_realUri.c_str()); // ruta absoluta al script
	_args[2] = NULL;
	
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
}


void CGI::executeCGI()
{
	int fdOut[2];
	int fdIN[2];
	
	if (pipe(fdOut) < 0 || pipe(fdIN) < 0)
		throw StatusLine(500, REASON_500, "pipe failed in executeCGI method");
	
	pid_t pid = fork();
    if (pid == -1) {
        close(fdOut[0]);
        close(fdOut[1]);
        if (_req->getMethod() == POST) {
            close(fdIN[0]);
            close(fdIN[1]);
        }
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
		chdir(_path_info.first.c_str());
	
		if (execve(_args[0], _args, _envvar) < 0){
            std::cerr << strerror(errno) << "\n";
            std::cerr << _args[0] << "\n";
			exit(EXECVE_FAIL);
		}
	
	}
	close(fdOut[1]);
	if (_req->getMethod() == POST){
		if (write(fdIN[1], _req->getBody().getBody().c_str(), _req->getBody().getBody().size()) < 0)
			throw StatusLine(500, REASON_500, "write failed in executeCGI method");
	}
	
	else{
		if (write(fdIN[1], _getBuffFile.c_str(), _getBuffFile.size()) < 0)
			throw StatusLine(500, REASON_500, "write failed in executeCGI method");
	}
	
	close(fdIN[1]);
	close(fdIN[0]);
	
       // Checking if execve correctly worked
    int status = 0;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == EXECVE_FAIL)
    {
        std::cerr << WIFEXITED(status) << ", " << WEXITSTATUS(status) << ", " << EXECVE_FAIL << "\n";
       throw StatusLine(500, REASON_500, "execve failed in executeCGI method");
    }
        
    char buf[CGI_PIPE_BUFFER_SIZE + 1] = {0};
	std::string msgbody;	
    int rd_out;
	while ((rd_out = read(fdOut[0], buf, CGI_PIPE_BUFFER_SIZE)) > 0) // check if -1
	{
		msgbody += buf;
		memset(buf, 0, CGI_PIPE_BUFFER_SIZE + 1);
	}
    if (rd_out == -1) {
        std::cerr << strerror(errno) << "\n";
    }
	msgbody += buf;
	close(fdOut[0]);
	_emptyBody->setBuff(msgbody);
		
		// remove the header part of the cgi output
	_emptyBody->setSize(msgbody.size() - (msgbody.find("\r\n\r\n") + 4)); 
}

 
void CGI::mySwap(CGI &a, CGI &b)
{
	std::swap(a._envvar, b._envvar);
	std::swap(a._emptyBody, b._emptyBody);
	std::swap(a._req, b._req);
	std::swap(a._exec, b._exec);
	std::swap(a._path_info, b._path_info);
}
