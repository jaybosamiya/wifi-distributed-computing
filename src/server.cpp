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
    make_ack_packet(p);
    print_packet(p);
    Packet p_ack = wrap_datalink(p);
    pcap_sendpacket(handle,p_ack.first,p_ack.second);
    verbose("Sent acknowledgement");

    for ( int i = 0 ; i < 1000 ; i++ ) {
      pcap_sendpacket(handle,p_ack.first,p_ack.second);
      usleep(1000); // To simulate computation time
    }

    MathPacketHeader *mph = extract_math_packet_header(p);

    debug("Got mph");

    Packet answer = make_answer_packet(p.first);

    debug("Got answer packet");

    Packet p_ack_ans;

    while ( !is_capture_math_packet(p_ack_ans,MATH_TYPE_ACK_ANSWER,mph->user_id_of_requester, mph->request_id) ) {
      pcap_sendpacket(handle,answer.first,answer.second);
    }

    verbose("Finished sending reply");
  }

  return 0;
}
