#ifndef CGI_HPP
# define CGI_HPP

#include "msg_format/Body.hpp"
#include "msg_format/StatusLine.hpp"
#include "request.hpp"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sstream>
#include <sys/wait.h>

// temp
#ifndef TMP_def
# define CGI_PATH "nil" 
# define EXECVE_FAIL EXIT_FAILURE 
# define CGI_PIPE_BUFFER_SIZE 0
#endif

class CGI
{
	private:
		char **_envvar;
		char **_args;
		Body *_emptyBody;
		Request *_req;
		std::fstream _openArgfile;
		std::string _exec;
		std::string _realUri;
		std::string _getBuffFile;
		std::pair<std::string, std::string> _path_info;
		
		
	public:

		/* ------------------------ COPLIEN FORM ----------------------- */

		CGI(Body *, Request *, const std::string &, const cgi_pair &);
		CGI &operator=(CGI &);
		~CGI();
	
	

		/* ------------------------ PUBLIC METHODS ----------------------- */

		void executeCGI();
		
	private:
	

		/* ------------------------ PRIVATE METHODS ----------------------- */

		void mySwap(CGI &, CGI &);
		CGI(CGI const &);

};

#endif
