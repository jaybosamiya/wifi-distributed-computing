/* server.cpp */

#include <iostream>
#include "util.h"

using namespace std;

char * interface = 0;

int main(int argc, char ** argv) {
  cout << "WiFi Math Server\n"
          "----------------\tBy Jay H. Bosamiya\n"
          "                \t------------------\n\n";

  int ret = handle_params(argc,argv);

  if ( ret != 0 ) {
    return ret;
  }

  // TODO: Actual work

  return 0;
}
