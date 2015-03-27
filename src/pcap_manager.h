/* pcap_manager.h */

#ifndef PCAP_MANAGER_H
#define PCAP_MANAGER_H

#include <pcap.h>
#include <algorithm>

extern pcap_t *handle;
extern int datalink;

void initialize();

typedef std::pair<u_char*,int> Packet;

Packet wrap_datalink(Packet p);
Packet unwrap_datalink(Packet p);

#endif
