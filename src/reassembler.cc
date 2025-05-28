#include "reassembler.hh"
#include "debug.hh"
#include <iostream>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  uint64_t first_unacceptable = first_unassembled + output_.writer().available_capacity();

  cout << "--------" << endl;
  cout << first_index << " " << first_unassembled << " " << first_unacceptable << endl;
  if (first_index >= first_unacceptable) {  // 头都出去了
    return;
  }
  else if (first_index == first_unassembled) { // 正好就是要放进去的位置；判断要不要截断后进字节流
    if (first_index + static_cast<uint64_t>(data.size()) > first_unacceptable){ // 头进来了屁股出去了
      data.resize(static_cast<size_t>(first_unacceptable - first_index));
    }
    output_.writer().push(data);
    first_unassembled += static_cast<uint64_t>(data.size());
    
    // 注意：这个时候如果接上去了，还要继续循环判断下去。
    while (true){
      if (flush_.count(first_unassembled) != 0){
        output_.writer().push(flush_[first_unassembled]);
        first_unassembled += static_cast<uint64_t>(flush_[first_unassembled].size());
        flush_.erase(first_index);
        continue;
      }
      break;
    }
  }
  else {  // 不是放进去的位置，但是同样需要判断要不要截断
    if (first_index + static_cast<uint64_t>(data.size()) > first_unacceptable){ 
      data.resize(static_cast<size_t>(first_unacceptable - first_index));
    }
    
    flush_[first_index] = data;
  }

  if (is_last_substring) {
    output_.writer().close();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  uint64_t cnt = 0;
  for (const auto& data: flush_){
    cnt += static_cast<uint64_t>(data.second.size());
  }
  return cnt;
}
