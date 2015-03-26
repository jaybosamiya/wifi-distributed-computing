/* server.cpp */

#include <iostream>
#include "util.h"
#include "pcap_manager.h"

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
  // TODO: Actual work

  return 0;
}
