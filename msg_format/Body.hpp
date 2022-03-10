#ifndef BODY_HPP
#define BODY_HPP

#include "../webserv.hpp"
#include "StatusLine.hpp"

class Body
{
	private:

		
		/* ------------------------- ATTRIBUTES ------------------------ */
		
			bool		_recv;		// Indicates when request line + headers have been received
			size_t		_size;		// Content-length size
			long		_maxSize;	// Max octets that Body object can receive
			std::string	_buff;		// Buffer containing the payload


	public:

		
		/* ------------------------ COPLIEN FORM ----------------------- */

		Body();
		Body(const Body& c);
		~Body();
		Body& operator=(Body a);
		
		/* --------------------------- GETTERS ------------------------- */

		const std::string& getBody() const ;
		size_t getSize() const ;
		size_t getMaxSize() const ;


		/* --------------------------- SETTERS ------------------------- */
		
		void setSize(size_t size);
		void setMaxSize(long maxSize);
		void setBuff(std::string const &buf);
		void startReceiving();


		/* --------------------------- METHODS ------------------------- */

		// Returns true or false depending if request line + all headers have been received
		bool isReceiving() const;
		
		// Reset the Body object
		void clear();
		
		// Append buffer received from client until content-length octets have been received.
		/*int*/void recvBuffer(const std::string& buffer/*, size_t index, size_t lenToRead*/);
		
	
		/* --------------- NON-MEMBER FUNCTION OVERLOADS --------------- */

		friend void swap(Body& a, Body& b);
}; 

#endif