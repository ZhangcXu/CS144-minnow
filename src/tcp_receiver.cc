#include "tcp_receiver.hh"
#include "debug.hh"
#include <iostream>
using namespace std;

const uint16_t MAX_size = 65535; // Maximum window size for TCP

void TCPReceiver::receive( TCPSenderMessage message )
{
  if (!message.SYN && !has_syn_) {
    return; // If the message is not a SYN and we haven't received a SYN yet, ignore it
  }
  
  if (message.SYN) {
    has_syn_ = true;
    zero_point = message.seqno; // Set the zero point to the sequence number of the SYN
  }

  if (message.FIN) {
    has_fin_ = true;
    last_point = message.seqno + message.sequence_length(); // Set the last point to the sequence number of the FIN
  }

  if (has_syn_) {
    // From Abs Seqno to Stream Indices
    if (message.payload.empty()){
      reassembler_.insert( 0, message.payload, message.FIN );
    }
    reassembler_.insert( message.seqno.unwrap( zero_point, reassembler_.get_first_unassembled()) - 1, message.payload, message.FIN );
  }
}

TCPReceiverMessage TCPReceiver::send() const
{
  auto arkno = zero_point + reassembler_.get_first_unassembled() + has_syn_;
  if (has_syn_ && has_fin_ && arkno + 1 == last_point) {
    arkno = arkno + 1; // If the last point is reached, increment the ack number
  }
  auto cap = reassembler_.get_capacity();

  return TCPReceiverMessage {
    .ackno = has_syn_? std::make_optional( arkno ) : std::nullopt,
    .window_size = cap >= MAX_size ? MAX_size : static_cast<uint16_t>( cap ),
    .RST = false
  };
}
