#include "msg_format/RequestLine.hpp"

RequestLine::RequestLine() : _method(-1) { }

RequestLine::RequestLine(const RequestLine& c) :_method(c._method), _path(c._path), _query(c._query) { }

RequestLine::~RequestLine() { }

RequestLine& RequestLine::operator=(RequestLine a)
{
	swap(*this, a);
	return *this;
}

int RequestLine::getMethod() const 
{ 
    return _method; 
}

const std::string& RequestLine::getPath() const 
{ 
    return _path; 
}

const std::string& RequestLine::getQuery() const 
{ 
    return _query; 
}
		
void RequestLine::setMethod(int method) 
{ 
    _method = method;
}

void RequestLine::setPath(const std::string& path) 
{ 
    _path = path;
}

void RequestLine::setQuery(const std::string& query) 
{ 
    _query = query;
}

void RequestLine::clear()
{
	_method = -1;
	_path.clear();
	_query.clear();
}

bool RequestLine::empty() const 
{
    return _path.empty();
}

void RequestLine::print() const
{
	std::cout << "------ REQUEST LINE ------\nmet: " << _method << ", path: |" << _path
		<< "|, query: |" << _query << "|\n";
}

void swap(RequestLine& a, RequestLine& b)
{
	std::swap(a._method, b._method);
	std::swap(a._path, b._path);
	std::swap(a._query, b._query);
}

inline std::ostream& operator<<(std::ostream& stream, const RequestLine& reqLine)
{
	stream << "Method = " << reqLine.getMethod() << ", path = |" << reqLine.getPath() << "|";
	
	if (!reqLine.getQuery().empty())
		stream << ", query = |" << reqLine.getQuery() << "|";
		
	return stream;
}
