/* client.cpp */

#include <iostream>
#include <unistd.h>
#include "util.h"
#include "pcap_manager.h"
#include "math_packet.h"

using namespace std;

int main(int argc, char ** argv) {
  cout << "WiFi Math Client\n"
          "----------------\tBy Jay H. Bosamiya\n"
          "                \t------------------\n\n";

  int ret = handle_params(argc,argv);

  if ( ret != 0 ) {
    return ret;
  }

  initialize();

  verbose("Initialization done.");

  while ( true ) {
    pcap_pkthdr hdr;
    u_char* packet = const_cast<u_char*> (pcap_next(handle,&hdr));

    int length = hdr.len;
    Packet p;
    p.first = packet;
    p.second = length;
    p = unwrap_datalink(p);

    MathPacketHeader *mph = (MathPacketHeader *)p.first;

    if ( !mph ) {
      continue;
    }

    if ( mph->magic_number != MATH_MAGIC ) {
      continue;
    }

    verbose("Captured a MATH packet");

    if ( mph->type_of_packet != MATH_TYPE_REQUEST ) {
      continue;
    }

    Packet answer = make_answer_packet(p.first);
    make_ack_packet(p);

    pcap_sendpacket(handle,p.first,p.second);

    while ( true ) {
      pcap_sendpacket(handle,answer.first,answer.second);

      packet = const_cast<u_char*> (pcap_next(handle,&hdr));
      length = hdr.len;

      p.first = packet;
      p.second = length;
      p = unwrap_datalink(p);

      MathPacketHeader* mph2 = (MathPacketHeader *)p.first;
      if ( mph2->magic_number != MATH_MAGIC ) {
        continue;
      }

      if ( mph2->type_of_packet != MATH_TYPE_ACK_ANSWER ) {
        continue;
      }

      if ( mph2->user_id_of_requester != mph->user_id_of_requester ) {
        continue;
      }

      if ( mph2->request_id != mph2->request_id ) {
        continue;
      }

      break;
    }
  }

  return 0;
}
