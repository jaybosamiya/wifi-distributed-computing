/* client.cpp */

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

    print_packet(p);

    pcap_sendpacket(handle, p.first, p.second);
    verbose("Sent packet");

    MathPacketHeader *mph = extract_math_packet_header(p);
    Packet p_ack;

    while (!is_capture_math_packet(p_ack, MATH_TYPE_ACK_REQUEST, mph->user_id_of_requester, mph->request_id) ) {
      pcap_sendpacket(handle, p.first, p.second);
    }

    Packet p_ans = capture_math_packet(MATH_TYPE_SEND_ANSWER, mph->user_id_of_requester, mph->request_id, extract_math_packet_header(p_ack)->user_id_of_sender);

    cout << "Answer: " << read_answer(p_ans) << "\n\n";

  }

  return 0;
}
