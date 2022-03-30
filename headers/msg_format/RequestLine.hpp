#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include "../webserv.hpp"
#include <iostream>

class RequestLine
{
private:
	
  /* ------------------------- ATTRIBUTES ------------------------ */
	
  int		_method;	// Enum methods { GET, HEAD, PUT, POST, DELETE };
  std::string 	_path;		// Contains the path where search / put the file
  std::string 	_query;		// Contains args coming after first '?' in URI


public:

  /* ------------------------ COPLIEN FORM ----------------------- */

  RequestLine();
  RequestLine(const RequestLine& c) ;
  ~RequestLine();
  RequestLine& operator=(RequestLine a);
		
  /* --------------------------- GETTERS ------------------------- */

  int get_method() const;
  const std::string& get_path() const;
  const std::string& get_query() const;


  /* --------------------------- SETTERS ------------------------- */
		
  void set_method(int method);
  void set_path(const std::string& path);
  void set_query(const std::string& query);
		

  /* -------------------------- METHODS -------------------------- */

  // Clear RequestLine object
  void clear();

  // Returns true if no path has been set in RequestLine object, false otherwise
  bool empty() const ;

  // Print on stdout RequestLine object
  void print() const;


  /* --------------- NON-MEMBER FUNCTION OVERLOADS --------------- */

  friend void swap(RequestLine& a, RequestLine& b);
		
}; 

/* -------------------- OPERATOR OVERLOADS --------------------- */

// Print the RequestLine method, path, and query if there is one
inline std::ostream& operator<<(std::ostream& stream, const RequestLine& reqLine);

#endif
