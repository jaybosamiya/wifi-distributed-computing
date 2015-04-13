/* pcap_manager.cpp */

#include "pcap_manager.h"
#include "util.h"
#include "protocol_headers.h"
#include <cstdlib>
#include <cstring>
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

  struct bpf_program fp;   /* The compiled filter expression */
  char filter_exp[] = "ether[0]==0x1a && ether[1]==0x14 && ether[2]==0x95 && ether[3]==0x00"; /* The filter expression */

  if (pcap_compile(handle, &fp, filter_exp, 0, 0) == -1) {
    error("Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    abort();
  }

  if (pcap_setfilter(handle, &fp) == -1) {
    error("Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
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

u_char PRISM_WRAPPER[] = {
  0x00, 0x00, 0x00, 0x41,             // msgcode
  0x08, 0x00, 0x00, 0x00,             // msglen
};

u_char RADIOTAP_WRAPPER[] = {
  0x00,                   // it_version
  0x00,                   // padding
  0x0a, 0x00,             // length
  0x00, 0x00, 0x80, 0x00, // IEEE80211_RADIOTAP_TX_FLAGS
  0x00, 0x08,             // no-ack required
};

const Packet PRISM_WRAP(PRISM_WRAPPER,8);
const Packet RADIOTAP_WRAP(RADIOTAP_WRAPPER,0x0a);

Packet wrap_packet_with(Packet p, Packet wrap) {
  u_char* packet = new u_char[p.second+wrap.second];
  int length = p.second+wrap.second;

  memcpy(packet,wrap.first,wrap.second);
  memcpy(packet+wrap.second,p.first,p.second);

  Packet ret;
  ret.first = packet;
  ret.second = length;
  return ret;
}

Packet wrap_datalink(Packet p) {
  if ( p.first == NULL ) {
    return p;
  }

  if ( datalink ==  DLT_PRISM_HEADER ) {
    p = wrap_packet_with(p,PRISM_WRAP);
  }

  if ( datalink == DLT_IEEE802_11_RADIO ) {
    p = wrap_packet_with(p,RADIOTAP_WRAP);
  }

  return p;
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
