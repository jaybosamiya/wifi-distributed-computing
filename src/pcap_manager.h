/* pcap_manager.h */

#ifndef PCAP_MANAGER_H
#define PCAP_MANAGER_H

#include <pcap.h>

extern pcap_t *handle;
extern int datalink;

void initialize();

#endif
