#include "reassembler.hh"
#include "debug.hh"
#include <iostream>
using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // cout << "New insert" << endl;
  uint64_t first_unacceptable = first_unassembled + output_.writer().available_capacity();
  uint64_t last_index = first_index + static_cast<uint64_t>(data.size());
  if (first_index >= first_unacceptable || last_index <= first_unassembled) {  // 头都出去了或者尾都在前面
    if (is_last_substring) {
      is_end = true;
      all_index = last_index;
    }
    if (first_unassembled == all_index && is_end){
      output_.writer().close();
    }
    return;
  }
  else if (first_index <= first_unassembled) { // 在放进去的位置或之前；判断要不要截断后进字节流
    // cout << first_index << " " << first_unassembled << " " << data << endl;
    if (first_index < first_unassembled) { // 头上也要砍掉一截
      data = data.substr(first_unassembled - first_index, last_index - first_unassembled + 1);
    }
    if (last_index > first_unacceptable){ // 头进来了屁股出去了
      data.resize(static_cast<size_t>(first_unacceptable - first_index));
    }
    // cout << first_index << " " << first_unassembled << " " << data << endl;
    uint64_t tip = first_unassembled;  // 有容量限制不一定能push进去
    first_unassembled += min(output_.writer().available_capacity(), static_cast<uint64_t>(data.size()));
    // cout << first_index << " " << first_unassembled << " " << data << endl;
    output_.writer().push(data);
    for (auto idx = tip; idx < first_unassembled; idx++){ // 只要push了就得erase掉维护的字典
      dict_.erase(idx);
    }
    // cout << first_unassembled << " " << flush_.count(first_unassembled) << endl;
    
    // 注意：这个时候如果接上去了，还要继续判断一次后面还有没有。
    if (dict_.find(first_unassembled) != dict_.end()) {
      std::string segment(1, dict_[first_unassembled]);
      dict_.erase(first_unassembled);
      while (dict_.find(++first_unassembled) != dict_.end()){
        segment += dict_[first_unassembled];
        dict_.erase(first_unassembled);
      }
      output_.writer().push(segment);
    }
  }
  else {  // 不是放进去的位置，但是同样需要判断要不要截断
    if (last_index > first_unacceptable){ 
      data.resize(static_cast<size_t>(first_unacceptable - first_index));
    }
    for (auto idx=first_index; idx<first_index+static_cast<uint64_t>(data.size()); idx++){
      if (dict_.find(idx) == dict_.end()){  // 没找到这个键
        dict_[idx] = data[idx-first_index];
      }
    }  
  }

  if (is_last_substring) {
    is_end = true;
    all_index = last_index;
  }
  if (first_unassembled == all_index && is_end){
    output_.writer().close();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  return dict_.size();
}
