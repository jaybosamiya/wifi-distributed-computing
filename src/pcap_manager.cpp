/* pcap_manager.cpp */

#include "pcap_manager.h"
#include "util.h"
#include "protocol_headers.h"
#include <cstdlib>
#include <pcap.h>

pcap_t *handle = NULL;
int datalink;
char * interface;

// Place the interface into monitor mode
// Special code for Ubuntu (or similar systems) which use crazy Network Managers
void set_monitor_mode() {
  char * const argv[] = {(char*)("iwconfig"),interface,(char*)("mode"),(char*)("monitor"),0};
  int ret = run_command(argv);
  if ( ret ) {
    debug("Probably on an Ubuntu system. Trying to set monitor using ifconfig technique.");
    char * const ifconfig_down[] = {(char*)("ifconfig"),interface,(char*)("down"),0};
    char * const ifconfig_up[] = {(char*)("ifconfig"),interface,(char*)("up"),0};
    ret = run_command(ifconfig_down);
    if ( ret ) {
      error("Interface error. Quitting.");
      abort();
    }
    ret = run_command(argv);
    if ( ret ) {
      error("Interface error. Quitting.");
      abort();
    }
    ret = run_command(ifconfig_up);
    if ( ret ) {
      error("Interface error. Quitting.");
      abort();
    }
  }
}

// Set up the interface, by setting to monitor mode, non-blocked
// Also, initialize the globals
void initialize() {
  if ( handle ) {
    error("Trying to reinitialize using interface %s",interface);
    abort();
  }

  char errbuf[BUFSIZ];

  set_monitor_mode();

  handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);
  if (handle == NULL) {
    error("Couldn't open interface %s. Error: %s",interface,errbuf);
    abort();
  }

  if ( pcap_setnonblock(handle,1,errbuf) == -1 ) {
    error("Couldn't set to non-blocking mode. Error: %s",errbuf);
    abort();
  }

  datalink = pcap_datalink(handle);

  verbose("Opened interface %s.",interface);
  debug("Datalink is %d.", datalink);
}

Packet wrap_datalink(Packet p) {
  // TODO
}

Packet unwrap_datalink(Packet p) {
  u_char* packet = p.first;
  int length = p.second;

  if ( packet == NULL ) {
    return p;
  }

  if ( datalink ==  DLT_PRISM_HEADER ) {
    prism_header* rth1 = (prism_header*)(packet);
    packet = packet + rth1->msglen;
    length -= rth1->msglen;
  }

  if ( datalink == DLT_IEEE802_11_RADIO ) {
    ieee80211_radiotap_header* rth2 = (ieee80211_radiotap_header*)(packet);
    packet = packet + rth2->it_len;
    length -= rth2->it_len;
  }

  Packet ret;
  ret.first = packet;
  ret.second = length;
  return ret;
}