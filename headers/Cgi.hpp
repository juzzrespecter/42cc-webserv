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
#include <errno.h>

#define PWD_BUFFER 4095

// temp
#ifndef TMP_def
# define EXECVE_FAIL EXIT_FAILURE 
# define CGI_PIPE_BUFFER_SIZE 4000
#endif

class CGI
{
	private:
		char **_envvar;
		char **_args;

		Request *_req;

        int _fdIN[2];
        int _fdOut[2];
		
		std::string _path_info;
		std::string _raw_response;

		StatusLine _status_line;
		std::map<std::string, std::string> _header_map;
		std::string _body_string;

		
	public:

		/* ------------------------ COPLIEN FORM ----------------------- */

		CGI(Request *, const std::string &, const cgi_pair &);
		CGI &operator=(CGI &);
		~CGI();
	
		/* ------------------------ PUBLIC METHODS ----------------------- */

		void executeCGI();
		void parse_response(void);

		std::string getHeaders(void) const;
		std::string getBody(void) const;

		const StatusLine& getStatusLine(void) const;

		bool isHeaderDefined(const std::string&) const;
	private:
	

		/* ------------------------ PRIVATE METHODS ----------------------- */
        void close_fdIN(void);
        void close_fdOut(void);

		void set_env_variables(const std::string&/*, const std::string&*/);
		void set_args(const std::string&, const std::string&);
        void set_path_info(const std::string&);
//		void executeCGI();

		void parse_response_headers(const std::string&);
		void parse_response_body(const std::string&);
		void parse_status_line(void);

		std::string buildCGIPath(const std::string&, const std::string&, const Location&);
		void mySwap(CGI &, CGI &);
		CGI(void);
		CGI(CGI const &);

};

#endif
