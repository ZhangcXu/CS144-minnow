#include "byte_stream.hh"
#include "iostream"
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ){}

void Writer::push( string data )
{
  if (Endflag) return;  // 不能再写了
  std::string data_cut = "";
  if (static_cast<uint64_t>(data.size()) > capacity_ - size_){
    for (uint64_t idx=0; idx<capacity_ - size_; idx++){
      data_cut += data[idx];
    }
    bytes_push += capacity_ - size_;
    size_ = capacity_;
    
  }
  else {
    data_cut = data;
    size_ += static_cast<uint64_t>(data.size());
    bytes_push += static_cast<uint64_t>(data.size());
  }
  data_ += data_cut;
}

void Writer::close()
{
  Endflag = true;
}

bool Writer::is_closed() const
{
  return Endflag;
}

uint64_t Writer::available_capacity() const 
{
  return capacity_ - size_;
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_push;
}

string_view Reader::peek() const
{
  return std::string_view(data_.data(), size_);
}

void Reader::pop( uint64_t len )
{
  if (size_ < len) {
    bytes_pop += size_;
    data_.clear();
    size_ = 0;
  }
  else{
    bytes_pop += len;
    data_.erase(0, static_cast<size_t>(len));
    size_ -= len;
  }
}

bool Reader::is_finished() const
{
  return Endflag && size_ == 0;
}

uint64_t Reader::bytes_buffered() const
{
  return size_;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_pop;
}

