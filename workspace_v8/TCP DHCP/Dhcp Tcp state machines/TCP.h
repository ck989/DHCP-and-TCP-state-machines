/*
 * TCP.h
 *
 *  Created on: 31-Mar-2020
 *      Author: chaya kumar gowda
 */

#ifndef TCP_H_
#define TCP_H_

#include <stdint.h>
#include <stdbool.h>
#include "eth0.h"

// ------------------------------------------------------------------------------
//  Structures
// ------------------------------------------------------------------------------

typedef struct _tcpFrame
{
   uint16_t sourcePort;
   uint16_t destPort;
   uint32_t SeqNum;
   uint32_t AckNum;
   uint16_t DoRF;
   uint16_t WindowSize;
   uint16_t CheckSum;
   uint16_t UrgentPtr;
   //uint16_t  OptionsPad;
   uint8_t  data;
}tcpFrame;

typedef struct _tcpOptions
{
    uint8_t  MSSOption;
    uint8_t  MSSlen;
    uint16_t MSSval;
    uint8_t  NOP;

}tcpOptions;

//TCP

#define TCPCLOSED          1
#define TCPLISTEN          2
#define TCPSYN             3
#define TCPSYN_SENT        4
#define TCPSYN_RECV        5
#define TCPACK             6
#define TCPESTABLISHED     7
#define TCPFINWAIT1        8
#define TCPFINWAIT2        9
#define TCPTIMEWAIT        10

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------


bool TcpListen(uint8_t packet[]);
bool IsTcpAck(uint8_t packet[]);
bool IsTcpTelnet(uint8_t packet[]);
bool IsTcpFin(uint8_t packet[]);
bool ISTcpFinAck(uint8_t packet[]);

void SendTcpSynAckmessage(uint8_t packet[]);
void SendTcpAck(uint8_t packet[]);
void SendTcpmessage(uint8_t packet[], uint8_t* tcpData, uint8_t tcpSize);
void SendTcpFin(uint8_t packet[]);
void SendTcpLastAck(uint8_t packet[]);

#endif /* TCP_H_ */
