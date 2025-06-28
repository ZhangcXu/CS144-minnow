#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return zero_point + static_cast<uint32_t>(n);
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  uint64_t period = 1ULL << 32;
  uint32_t offset = this->raw_value_ - wrap( checkpoint, zero_point ).raw_value_;
  uint64_t ans = checkpoint + offset;

  if ( offset >= 1ULL << 31 && ans >= period ) 
  // 关键在于判断ans有没有越过offset再转一圈
    ans -= period; 
  return ans;

}
