/*
 * DHCP.h
 *
 *  Created on: 31-Mar-2020
 *      Author: chaya kumar gowda
 */

#ifndef DHCP_H_
#define DHCP_H_

#include <stdint.h>
#include <stdbool.h>
#include "eth0.h"

typedef struct _dhcpFrame
{
  uint8_t op;
  uint8_t htype;
  uint8_t hlen;
  uint8_t hops;
  uint32_t  xid;
  uint16_t secs;
  uint16_t flags;
  uint8_t ciaddr[4];
  uint8_t yiaddr[4];
  uint8_t siaddr[4];
  uint8_t giaddr[4];
  uint8_t chaddr[16];
  uint8_t data[192];
  uint32_t magicCookie;
  uint8_t options[50];
} dhcpFrame;

/*
 * DHCP parameters
 */

#define DHCPDISCOVER       1
#define DHCPOFFER          2
#define DHCPREQUEST        3
#define DHCPDECLINCE       4
#define DHCPACK            5
#define DHCPNACK           6
#define DHCPRELEASE        7
#define DHCPINFORM         8
#define TEST_IP            9
#define BOUND              10
#define WAITING            11
#define INIT               12


#define REQUESTING         20

//DHCP OP fields

#define BOOTREQUEST        1
#define BOOTREPLY          2

void sendDHCPmessage(uint8_t packet[], uint8_t type, uint8_t ipAdd[]);
void SendDhcpRequest(uint8_t packet[], uint8_t type, uint8_t ipAdd[]);
void SendDhcpRefresh(uint8_t packet[], uint8_t type, uint8_t ipAdd[]);

void etherEnableDhcpMode();
void etherDisableDhcpMode();
bool etherIsDhcpEnabled();

bool IsDhcpOffer(uint8_t packet[]);
bool IsDhcpAck(uint8_t packet[]);
bool IsDhcpNAck(uint8_t packet[]);

void CollectDhcpOfferData(uint8_t packet[]);
void CollectDhcpAckData(uint8_t packet[]);
#endif /* DHCP_H_ */
