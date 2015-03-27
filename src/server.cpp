/* server.cpp */

#include <iostream>
#include <string>
#include "util.h"
#include "pcap_manager.h"
#include "math_packet.h"

using namespace std;

int main(int argc, char ** argv) {
  cout << "WiFi Math Server\n"
          "----------------\tBy Jay H. Bosamiya\n"
          "                \t------------------\n\n";

  int ret = handle_params(argc,argv);

  if ( ret != 0 ) {
    return ret;
  }

  initialize();

  while ( true ) {
    cout << "Enter a math expression: ";
    string expression;
    getline(cin,expression);
    Packet p = make_packet_from_expression(expression);

    pcap_pkthdr hdr;
    u_char* packet;
    int length;
    MathPacketHeader *mph;

    while ( true ) {
      pcap_sendpacket(handle,p.first,p.second);

      mph = (MathPacketHeader *)p.first;

      packet = const_cast<u_char*> (pcap_next(handle,&hdr));
      length = hdr.len;

      Packet p2;
      p2.first = packet;
      p2.second = length;
      p2 = unwrap_datalink(p2);

      MathPacketHeader* mph2 = (MathPacketHeader *)p2.first;
      if ( mph2->magic_number != MATH_MAGIC ) {
        continue;
      }

      if ( mph2->type_of_packet != MATH_TYPE_ACK_REQUEST ) {
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

    while ( true ) {
      packet = const_cast<u_char*> (pcap_next(handle,&hdr));
      length = hdr.len;

      Packet p2;
      p2.first = packet;
      p2.second = length;
      p2 = unwrap_datalink(p2);

      MathPacketHeader* mph2 = (MathPacketHeader *)p2.first;
      if ( mph2->magic_number != MATH_MAGIC ) {
        continue;
      }

      if ( mph2->type_of_packet != MATH_TYPE_SEND_ANSWER ) {
        continue;
      }

      if ( mph2->user_id_of_requester != mph->user_id_of_requester ) {
        continue;
      }

      if ( mph2->request_id != mph2->request_id ) {
        continue;
      }

      cout << "Answer: " << read_answer(p2) << endl;

      break;
    }

  }

  return 0;
}
