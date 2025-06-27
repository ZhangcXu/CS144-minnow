#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return Wrap32 {static_cast<uint32_t>(n + zero_point.raw_value_)};
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  const uint64_t period = 1ULL << 32;   
  auto base = static_cast<uint64_t>(raw_value_ - zero_point.raw_value_);
  uint64_t k = checkpoint / period;
  uint64_t cand0 = base + (k - 1) * period;
  uint64_t cand1 = base + k * period;
  uint64_t cand2 = base + (k + 1) * period;
  uint64_t diff0 = cand0 > checkpoint ? cand0 - checkpoint : checkpoint - cand0;
  uint64_t diff1 = cand1 > checkpoint ? cand1 - checkpoint : checkpoint - cand1;
  uint64_t diff2 = cand2 > checkpoint ? cand2 - checkpoint : checkpoint - cand2;
  if (diff0 < diff1 && diff0 < diff2) return cand0;
  else if (diff1 < diff2) return cand1;
  else return cand2;
}
