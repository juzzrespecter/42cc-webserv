#include "msg_format/Body.hpp"

Body::Body() : _recv(false), _size(), _maxSize(-1) {}

Body::Body(const Body& c) :_recv(c._recv), _size(c._size), _maxSize(c._maxSize), _buff(c._buff) {}

Body::~Body() {}

Body& Body::operator=(Body a)
{
  swap(*this, a);
  return *this;
}

const std::string& Body::get_body() const 
{
  return _buff; 
}

size_t Body::get_size() const 
{
  return _size; 
}

size_t Body::get_max_size() const 
{
  return _maxSize; 
}

void Body::set_size(size_t size) 
{
  _size = size; 
}

void Body::set_max_size(long maxSize) 
{
  _maxSize = maxSize; 
}

void Body::setBuff(std::string const &buf) 
{
  _buff = buf; 
}

void Body::clear()
{
  _recv = false;
  _size = 0;
  _buff.clear();
}

void Body::recv_buffer(const std::string& buffer/*, size_t index, size_t lenToRead*/)
{
  _buff.append(buffer);
  if (_maxSize > 0 && _buff.size() > static_cast<size_t>(_maxSize)) {
    throw StatusLine(413, REASON_413, "request body exceeded location max body configuration");
  }
  if (_size > 0 && _buff.size() > _size) {
    throw StatusLine(400, REASON_400, "request body exceeded content-length value");
  }
}

void swap(Body& a, Body& b)
{
  std::swap(a._recv, b._recv);
  std::swap(a._size, b._size);
  std::swap(a._maxSize, b._maxSize);
  std::swap(a._buff, b._buff);
}
