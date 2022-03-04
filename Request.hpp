#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
#include <map>

#include "webserv.hpp"
#include "server.hpp"
#include "msg_format/StatusLine.hpp"
#include "msg_format/RequestLine.hpp"
#include "msg_format/Body.hpp"

#define HEADER_LIST_SIZE 37

enum request_stage_f { 
	request_line_stage,
	header_stage,
	request_body_stage,
	request_is_ready
};

#define THROW_STATUS(extraInfo)	throw StatusLine(400, REASON_400, extraInfo)

class Request
{
	private:
		typedef std::map<std::string, std::string>	header_map;
		/* ------------------------- ATTRIBUTES ------------------------ */

		std::string	_buffer;		// Store the request received
		size_t		_index;			// Indicates which part of the buffer is left to treat

		std::vector<const Server*>	_vservVec;	// Server blocks from config file that match the appropriate port
        
		size_t	_headerCount;

		RequestLine	_reqLine;	// Contains all the information from the request line
        header_map	_headers;	// Store the headers
        Body		_body;		// Store the body
	
		request_stage_f	_stage;
	
		static const std::string header_list[HEADER_LIST_SIZE];

	public:

		/* ------------------------ COPLIEN FORM ----------------------- */

		Request();
		Request(const std::vector<const Server*>& vservVec);
		Request(const Request& c);
		~Request();
		Request& operator=(Request a);
		

		/* --------------------------- GETTERS ------------------------- */

		const RequestLine& getRequestLine() const;
		const header_map& getHeaders() const;
		const Body& getBody() const;
		const std::string& getBuffer() const;
		int getMethod() const;
		const std::string& getPath() const;
		const std::string& getQuery() const;


		/* --------------------------- SETTERS ------------------------- */

        	void setPath(const std::string& path);


		/* --------------------------- METHODS ------------------------- */

		// Add the buffer from receive to request object
		//Request& operator+=(const char* charBuffer);
		void recvBuffer(const std::string&);

		// Check if the data received is correct. If an error occur or if a request was
		// fully received, throws a status line with the appropriate code.
		// void parsingCheck();

		// Reset the request object
		void clear();

		// Print the Request object on stdout
		void print() const;
	
		/* void parseChunkedRequest() */
	
	private:

		/* ------------------ PRIVATE MEMBER FUNCTIONS ----------------- */
		
		// Check if a new line was received (corresponding to a request line or an header).
		// If the request line is superior to MAX_URI_LEN or if an header line is superior 
		// to MAX_HEADER_LEN, throws a status line with the appropriate code.
        //	bool newLineReceived(size_t posCRLF);

		// Checks that the request line received respect the RFC norme.
		void parseRequestLine(void);

		// Checks that the first word in a request line in a appropriate method.
		void parseMethodToken(const std::string& token);

		// Check that the URI in the request line respect the RFC norme.
		void parseURI(std::string& token);

		// Check that the HTTP version in the request line is respecting the RFC norme and is equal 
		// to 1.1.
		void parseHTTPVersion(const std::string& token);
        
		// Check that the header line received respect the RFC norme.
       	void parseHeaderLine(void);
		void headerMeetsRequirements(void) const;
		void setUpRequestBody(void);
		
		// Received the body until n octets (from content-length header) has been received. Then throw 
		// a status line with the appropriate code.
		void parseRequestBody(void);

		size_t parseChunkSize(void);
		void parseChunkedRequestBody(void);
		
		// Search for the correct server block (matching host header field, if not using default server
		// block) and returns an unsigned max_body_size if the field is existing in the config file, otherwise
		// returns -1.
		long findMaxSize(void) const;
		bool transferEncodingIsChunked(void) const;

		// Header accesses
		std::string	host(void) const;
		size_t 		contentLength(void) const;

		std::string _getNextLine(void);

		
		/* --------------- NON-MEMBER FUNCTION OVERLOADS --------------- */
		
		friend void swap(Request& a, Request& b);
		
};

#endif