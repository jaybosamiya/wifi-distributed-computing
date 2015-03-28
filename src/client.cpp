/* client.cpp */

#include <iostream>
#include <unistd.h>
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

  cout << "Enter a math expression: ";
  string expression;
  getline(cin,expression);
  Packet p = make_packet_from_expression(expression);

  p = wrap_datalink(p);

  pcap_sendpacket(handle, p.first, p.second);
  verbose("Sent packet");

  MathPacketHeader *mph = extract_math_packet_header(p);
  Packet p_ack;

  int counter = 0;

  while (!is_capture_math_packet(p_ack, MATH_TYPE_ACK_REQUEST) ) {
    counter = (counter+1)%2000;
    if ( counter == 0 )
      pcap_sendpacket(handle, p.first, p.second);
    usleep(100);
  }

  verbose("Got Acknowledgement of Request. Waiting for answer.");

  Packet p_ans = capture_math_packet(MATH_TYPE_SEND_ANSWER);

  verbose("Got Answer");

  int ans = read_answer(p_ans);

  make_ack_packet(p_ans);
  p_ans = wrap_datalink(p_ans);

  for ( int i = 0 ; i < 100 ; i++ ) {
    pcap_sendpacket(handle,p_ans.first,p_ans.second);
  }

  verbose("Sent acknowledgement for receiving answer");

  cout << "Answer: " << ans << "\n\n";

  return 0;
}
