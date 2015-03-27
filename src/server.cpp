/* server.cpp */

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
    Packet p = capture_math_packet(MATH_TYPE_REQUEST);
    verbose("Received Request Packet");
    print_packet(p);
    make_ack_packet(p); // TODO: Check if this location is OK since it modifies p
    Packet p_ack = wrap_datalink(p);
    pcap_sendpacket(handle,p_ack.first,p_ack.second);
    verbose("Sent acknowledgement");

    MathPacketHeader *mph = extract_math_packet_header(p);

    Packet answer = make_answer_packet(p.first);

    Packet p_ack_ans;

    while ( !is_capture_math_packet(p_ack_ans,MATH_TYPE_ACK_ANSWER,mph->user_id_of_requester, mph->request_id) ) {
      pcap_sendpacket(handle,answer.first,answer.second);
    }

    verbose("Finished sending reply");
  }

  return 0;
}
