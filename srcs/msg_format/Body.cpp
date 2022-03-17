#include "msg_format/Body.hpp"

Body::Body() : _recv(false), _size(), _maxSize(-1) {}

Body::Body(const Body& c) :_recv(c._recv), _size(c._size), _maxSize(c._maxSize), _buff(c._buff) {}

Body::~Body() {}

Body& Body::operator=(Body a)
{
	swap(*this, a);
	return *this;
}

const std::string& Body::getBody() const 
{
     return _buff; 
}

size_t Body::getSize() const 
{
     return _size; 
}

size_t Body::getMaxSize() const 
{
     return _maxSize; 
}

void Body::setSize(size_t size) 
{
     _size = size; 
}

void Body::setMaxSize(long maxSize) 
{
     _maxSize = maxSize; 
}

void Body::setBuff(std::string const &buf) 
{
     _buff = buf; 
}

void Body::startReceiving() 
{
     _recv = true; 
}

bool Body::isReceiving() const 
{
     return _recv; 
}

void Body::clear()
{
	_recv = false;
	_size = 0;
	_buff.clear();
}

void Body::recvBuffer(const std::string& buffer/*, size_t index, size_t lenToRead*/)
{
	// maxSize will be evaluated only if previously set using setMaxSize before, because 
	// default constructor initilized it to -1
	//if (_maxSize >= 0 && (_maxSize -= lenToRead) < 0)
	//	return -1;

	_buff.append(buffer);
	std::cout << "[ buffer: " << buffer << " ]\n";
	std::cout << "[ in body: limit " << _size << ", current " << _buff.size() << ", max " << _maxSize << "]\n";
	if (_maxSize > 0 && _buff.size() > static_cast<size_t>(_maxSize)) {
		throw StatusLine(413, REASON_413, "request body exceeded location max body configuration");
	}
	if (_size > 0 && _buff.size() > _size) {
		throw StatusLine(400, REASON_400, "request body exceeded content-length value");
	}
	// controla size -> content-lenght
	// controla max_size -> client_max_body_size
	//_size -= lenToRead;
	//return 0;
}

void swap(Body& a, Body& b)
{
	std::swap(a._recv, b._recv);
	std::swap(a._size, b._size);
	std::swap(a._maxSize, b._maxSize);
	std::swap(a._buff, b._buff);
}
