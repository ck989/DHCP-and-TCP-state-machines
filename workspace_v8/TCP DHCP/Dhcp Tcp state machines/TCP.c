/*
 * TCP.c
 *
 *  Created on: 31-Mar-2020
 *      Author: chaya kumar gowda
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "TCP.h"

extern uint32_t DhcpIpaddress[IP_ADD_LENGTH];

extern void etherSumWords(void* data, uint16_t sizeInBytes);
extern uint16_t getEtherChecksum();
extern uint32_t sum;


bool TcpListen(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));

    bool ok;
    uint8_t x;

    ok = (ip->protocol == 6); // if it is TCP

    tcp->sourcePort = htons(tcp->sourcePort);
    tcp->DoRF = htons(tcp->DoRF);

    x = (tcp->DoRF & 0xFF); //choose flags

    if(ok)
    {
        ok = (ether->destAddress[0] == 2);
        ok &= (ether->destAddress[1] == 3);
        ok &= (ether->destAddress[2] == 4);
        ok &= (ether->destAddress[3] == 5);
        ok &= (ether->destAddress[4] == 6);
        ok &= (ether->destAddress[5] == 141);

        //ok &= (tcp->destPort == htons(23));

        ok &= (x == 0x02);  //if it is SYN
    }

    return ok;
}

bool IsTcpAck(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));

    bool ok;

    ok = (ether->destAddress[0] == 2);
    ok &= (ether->destAddress[1] == 3);
    ok &= (ether->destAddress[2] == 4);
    ok &= (ether->destAddress[3] == 5);
    ok &= (ether->destAddress[4] == 6);
    ok &= (ether->destAddress[5] == 141);

    uint8_t x;

    tcp->sourcePort = htons(tcp->sourcePort);

    if(ok)
    {
        tcp->DoRF = htons(tcp->DoRF);

        x = (tcp->DoRF & 0xFF); //choose flags

        ok &= (x == 0x10);  // if it is ACK

    }

    return ok;
}

uint32_t PayloadSize;
bool IsTcpTelnet(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));

    bool ok;

    ok = (ether->destAddress[0] == 2);
    ok &= (ether->destAddress[1] == 3);
    ok &= (ether->destAddress[2] == 4);
    ok &= (ether->destAddress[3] == 5);
    ok &= (ether->destAddress[4] == 6);
    ok &= (ether->destAddress[5] == 141);


    uint32_t x;

    tcp->sourcePort = htons(tcp->sourcePort);

    if(ok)
    {
        x = (tcp->DoRF & 0xFF00); //choose flags
        ok &= (x == 0x1800);  // if it is PSH ACK

        //IP length - IP header size - TCP frame size
        PayloadSize = htons(ip->length) - 20 - 20;
    }

    return ok;
}

bool IsTcpFin(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));

    bool ok;

    ok = (ether->destAddress[0] == 2);
    ok &= (ether->destAddress[1] == 3);
    ok &= (ether->destAddress[2] == 4);
    ok &= (ether->destAddress[3] == 5);
    ok &= (ether->destAddress[4] == 6);
    ok &= (ether->destAddress[5] == 141);


    uint32_t x;

    if(ok)
    {
        x = (tcp->DoRF & 0xFF00); //choose flags
        ok &= (x == 0x0100);  // if it is FIN
    }

    return ok;

}

bool ISTcpFinAck(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));

    bool ok;

    ok = (ether->destAddress[0] == 2);
    ok &= (ether->destAddress[1] == 3);
    ok &= (ether->destAddress[2] == 4);
    ok &= (ether->destAddress[3] == 5);
    ok &= (ether->destAddress[4] == 6);
    ok &= (ether->destAddress[5] == 141);


    uint32_t x;

    if(ok)
    {
        tcp->DoRF = htons(tcp->DoRF);
        x = (tcp->DoRF & 0xFF); //choose flags
        ok &= (x == 0x11);  // if it is FIN ACK
    }

    return ok;
}



void SendTcpSynAckmessage(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));
    tcpOptions* tcpopt;

    uint8_t i,flags,Offset;
    uint16_t x = 24;
    uint16_t a;

    //uint8_t* tcpoptions;

    // populating ether frame
    for(i = 0; i < HW_ADD_LENGTH; i++)
    {
        ether->destAddress[i] = ether->sourceAddress[i];
    }

    ether->sourceAddress[0] = 2;
    ether->sourceAddress[1] = 3;
    ether->sourceAddress[2] = 4;
    ether->sourceAddress[3] = 5;
    ether->sourceAddress[4] = 6;
    ether->sourceAddress[5] = 141;

    ether->frameType = htons(0x0800);

    //populating IP field
    ip->typeOfService = 0;
    ip->ttl = 55;
    ip->protocol = 6; // TCP
    ip->id = 0;

    uint8_t temp8;
    for(i = 0; i < IP_ADD_LENGTH; i++)
    {
        temp8 = ip->destIp[i];
        ip->destIp[i] = ip->sourceIp[i];
        ip->sourceIp[i] = temp8;
    }
/*
    for(i = 0; i < IP_ADD_LENGTH; i++)
    {
        ip->sourceIp[i] = DhcpIpaddress[i];
    }
*/
    //populating TCP
    tcp->destPort = ntohs(tcp->sourcePort);
    tcp->sourcePort = htons(23);

    tcp->AckNum = tcp->SeqNum + htons32(0x00000001);
    tcp->SeqNum = 0x00000000;

    Offset = x >> 2;
    flags = 0x12; // for SYN and ACK
    a = (Offset << 12) + flags;
    tcp->DoRF = htons(a);
    tcp->WindowSize = htons(1280);
    tcp->CheckSum = 0;
    tcp->UrgentPtr = 0;
/*
    tcpoptions = (uint8_t*)&tcp->OptionsPad;
    tcpoptions[0] = 2;   // Maximum Segment Size
    tcpoptions[1] = 4;
    tcpoptions[2] = 0x05;
    tcpoptions[3] = 0x00;

*/

    //populating TCP options

    tcpopt->MSSOption = 2;
    tcpopt->MSSlen = 4;
    tcpopt->MSSval = htons(0x0500);

    ip->length = htons(((ip->revSize & 0xF) * 4) + 20 + 4);
    //Ip checksum
    sum=0;
    etherSumWords(&ip->revSize, 10);
    etherSumWords(ip->sourceIp, ((ip->revSize & 0xF) * 4) - 12);
    ip->headerChecksum = getEtherChecksum();

    //TCP checksum

    uint16_t tmp16;

    uint16_t tcpLen = htons(20+4);
    sum = 0;
    etherSumWords(ip->sourceIp, 8);

    tmp16 = ip->protocol;
    sum += (tmp16 & 0xff) << 8;
    etherSumWords(&tcpLen,2);

    etherSumWords(tcp, 20+4);
    //etherSumWords(tcpopt,4);
   // etherSumWords(&tcp->data, 0);
    tcp->CheckSum = getEtherChecksum();

    etherPutPacket((uint8_t*)ether, 14 + ((ip->revSize & 0xF) * 4) +  20 + 4);
}

void SendTcpAck(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));

    uint8_t i,flags,Offset;
    uint16_t x = 20; // total header length
    uint16_t a;

    // populating ether frame
    for(i = 0; i < HW_ADD_LENGTH; i++)
    {
        ether->destAddress[i] = ether->sourceAddress[i];
    }

    ether->sourceAddress[0] = 2;
    ether->sourceAddress[1] = 3;
    ether->sourceAddress[2] = 4;
    ether->sourceAddress[3] = 5;
    ether->sourceAddress[4] = 6;
    ether->sourceAddress[5] = 141;

    ether->frameType = htons(0x0800);

    //populating IP field
    ip->typeOfService = 0;
    ip->ttl = 55;
    ip->protocol = 6; // TCP
    ip->id = 0;
    uint8_t temp8;
    for(i = 0; i < IP_ADD_LENGTH; i++)
    {
        temp8 = ip->destIp[i];
        ip->destIp[i] = ip->sourceIp[i];
        ip->sourceIp[i] = temp8;
    }
/*
    for(i = 0; i < IP_ADD_LENGTH; i++)
    {
        ip->sourceIp[i] = DhcpIpaddress[i];
    }
*/
    //populating TCP
    uint32_t temp32;
    tcp->destPort = ntohs(tcp->sourcePort);
    tcp->sourcePort = htons(23);


    temp32 = tcp->AckNum;
    tcp->AckNum = tcp->SeqNum;
    tcp->SeqNum = temp32;

    tcp->AckNum = tcp->AckNum + htons32(PayloadSize);

    Offset = x >> 2;
    flags = 0x10; // ACK
    a = (Offset << 12) + flags;
    tcp->DoRF = htons(a);
    tcp->WindowSize = htons(1280);
    tcp->CheckSum = 0;
    tcp->UrgentPtr = 0;



    ip->length = htons(((ip->revSize & 0xF) * 4) + 20);
    //Ip checksum
    sum=0;
    etherSumWords(&ip->revSize, 10);
    etherSumWords(ip->sourceIp, ((ip->revSize & 0xF) * 4) - 12);
    ip->headerChecksum = getEtherChecksum();

    //TCP checksum

    uint16_t tmp16;

    uint16_t tcpLen = htons(20);
    sum = 0;
    etherSumWords(ip->sourceIp, 8);

    tmp16 = ip->protocol;
    sum += (tmp16 & 0xff) << 8;
    etherSumWords(&tcpLen,2);

    etherSumWords(tcp, 20);
    // etherSumWords(&tcp->data, 0);
    tcp->CheckSum = getEtherChecksum();

    etherPutPacket((uint8_t*)ether, 14 + ((ip->revSize & 0xF) * 4) +  20);
}

void SendTcpmessage(uint8_t packet[], uint8_t* tcpData, uint8_t tcpSize)
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));

    uint8_t i,flags,Offset;
    uint16_t x = 20;
    uint16_t a;

    uint8_t *copyData;
    for(i = 0; i < HW_ADD_LENGTH; i++)
     {
         ether->destAddress[i] = ether->destAddress[i];
     }

    ether->sourceAddress[0] = 2;
    ether->sourceAddress[1] = 3;
    ether->sourceAddress[2] = 4;
    ether->sourceAddress[3] = 5;
    ether->sourceAddress[4] = 6;
    ether->sourceAddress[5] = 141;

    ether->frameType = htons(0x0800);

    //populating IP field
    ip->typeOfService = 0;
    ip->ttl = 55;
    ip->protocol = 6; // TCP
    ip->id = 0;
/*
    for(i = 0; i < IP_ADD_LENGTH; i++)
    {
        ip->destIp[i] = ip->destIp[i];
    }

    for(i = 0; i < IP_ADD_LENGTH; i++)
    {
        ip->sourceIp[i] = DhcpIpaddress[i];
    }
*/
    //populating TCP
    tcp->destPort = tcp->destPort;
    //tcp->sourcePort = htons(23);

    tcp->SeqNum = tcp->SeqNum;
    tcp->AckNum = tcp->AckNum;

    Offset = x >> 2;
    flags = 0x18; // for PSH and ACK
    a = (Offset << 12) + flags;
    tcp->DoRF = htons(a);
    tcp->WindowSize = htons(1280);
    tcp->CheckSum = 0;
    tcp->UrgentPtr = 0;

    ip->length = htons(((ip->revSize & 0xF) * 4) + 20 + tcpSize);

    // 32-bit sum over ip header
    sum = 0;
    etherSumWords(&ip->revSize, 10);
    etherSumWords(ip->sourceIp, ((ip->revSize & 0xF) * 4) - 12);
    ip->headerChecksum = getEtherChecksum();

    copyData = &tcp->data;
    for (i = 0; i < tcpSize; i++)
    {
        copyData[i] = tcpData[i];
    }
/*
    copyData[i+1] = 0;

    uint8_t j;

    //right shift the datas
      for(j = i+1 ; j>0; j--)
      {
          copyData[j] = copyData[j-1];
      }

      copyData[0] = 0;
*/
    uint16_t tmp16;

    uint16_t tcpLen = htons(20 + tcpSize);
    // 32-bit sum over pseudo-header
    sum = 0;

    etherSumWords(ip->sourceIp, 8);
    tmp16 = ip->protocol;
    sum += (tmp16 & 0xff) << 8;
    etherSumWords(&tcpLen, 2);
    // add udp header except crc
    /*
    for(i = 0; i < 40; i++)
    {
        tcp->OptionsPad[i] = 0;
    }
    */

    //uint16_t destPort = htons(tcp->destPort);
    //etherSumWords(&tcp->sourcePort,2);
    //sum += (destPort & 0xFF) << 8;
//    etherSumWords(tcp,4);
//    sum += htons32(tcp->SeqNum);
//    sum += htons32(tcp->AckNum);
//    etherSumWords(&tcp->DoRF, 8);

    etherSumWords(tcp,20+tcpSize);
    //etherSumWords(&tcp->data, tcpSize);

    tcp->CheckSum = getEtherChecksum();

    // send packet with size = ether + tcp hdr + ip header + tcp_size
    etherPutPacket((uint8_t*)ether, 14 + 20 + ((ip->revSize & 0xF) * 4) + tcpSize );
}

void SendTcpFin(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));

    uint8_t i,flags,Offset;
    uint16_t x = 20;
    uint16_t a;

    for(i = 0; i < HW_ADD_LENGTH; i++)
    {
         ether->destAddress[i] = ether->destAddress[i];
    }

    ether->sourceAddress[0] = 2;
    ether->sourceAddress[1] = 3;
    ether->sourceAddress[2] = 4;
    ether->sourceAddress[3] = 5;
    ether->sourceAddress[4] = 6;
    ether->sourceAddress[5] = 14;

    ether->frameType = htons(0x0800);

    //populating IP field
        ip->typeOfService = 0;
        ip->ttl = 128;
        ip->protocol = 6; // TCP
        ip->id = 0;

        uint8_t temp8;
        for(i = 0; i < IP_ADD_LENGTH; i++)
        {
            temp8 = ip->destIp[i];
            ip->destIp[i] = ip->sourceIp[i];
            ip->sourceIp[i] = temp8;
        }
/*
        for(i = 0; i < IP_ADD_LENGTH; i++)
        {
            ip->sourceIp[i] = DhcpIpaddress[i];
        }
*/
        //populating TCP
        tcp->destPort = htons(tcp->sourcePort);
        tcp->sourcePort = htons(23);

        //tcp->SeqNum = tcp->AckNum;
        //tcp->AckNum = tcp->SeqNum + htons32(1);
        uint32_t temp32;

        temp32 = tcp->AckNum;

        tcp->AckNum = tcp->SeqNum;

        tcp->SeqNum = temp32;
        //tcp->AckNum = 0;


        Offset = x >> 2;
        flags = 0x11; // for FIN and ACK
        a = (Offset << 12) + flags;
        tcp->DoRF = htons(a);
        tcp->WindowSize = htons(1280);
        tcp->CheckSum = 0;
        tcp->UrgentPtr = 0;

        ip->length = htons(((ip->revSize & 0xF) * 4) + 20);

        //IP checksum
        sum=0;
        etherSumWords(&ip->revSize, 10);
        etherSumWords(ip->sourceIp, ((ip->revSize & 0xF) * 4) - 12);
        ip->headerChecksum = getEtherChecksum();

        //TCP checksum

        uint16_t tmp16;

        uint16_t tcpLen = htons(20);
        sum = 0;
        etherSumWords(ip->sourceIp, 8);

        tmp16 = ip->protocol;
        sum += (tmp16 & 0xff) << 8;
        etherSumWords(&tcpLen,2);

        etherSumWords(tcp, 20);
        // etherSumWords(&tcp->data, 0);
        tcp->CheckSum = getEtherChecksum();

        etherPutPacket((uint8_t*)ether, 14 + ((ip->revSize & 0xF) * 4) +  20);


}

void SendTcpLastAck(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    tcpFrame* tcp = (tcpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));

        uint8_t i,flags,Offset;
        uint16_t x = 20; // total header length
        uint16_t a;

        // populating ether frame
        for(i = 0; i < HW_ADD_LENGTH; i++)
        {
            ether->destAddress[i] = ether->sourceAddress[i];
        }

        ether->sourceAddress[0] = 2;
        ether->sourceAddress[1] = 3;
        ether->sourceAddress[2] = 4;
        ether->sourceAddress[3] = 5;
        ether->sourceAddress[4] = 6;
        ether->sourceAddress[5] = 141;

        ether->frameType = htons(0x0800);

        //populating IP field
        ip->typeOfService = 0;
        ip->ttl = 55;
        ip->protocol = 6; // TCP
        ip->id = 0;
        uint8_t temp8;
        for(i = 0; i < IP_ADD_LENGTH; i++)
        {
            temp8 = ip->destIp[i];
            ip->destIp[i] = ip->sourceIp[i];
            ip->sourceIp[i] = temp8;
        }
/*
        for(i = 0; i < IP_ADD_LENGTH; i++)
        {
            ip->sourceIp[i] = DhcpIpaddress[i];
        }
*/
        //populating TCP
        uint32_t temp32;
        tcp->destPort = ntohs(tcp->sourcePort);
        tcp->sourcePort = htons(23);


        temp32 = tcp->SeqNum;
        tcp->SeqNum = tcp->AckNum;
        tcp->AckNum = temp32;

        tcp->AckNum = tcp->AckNum + htons32(1);

        Offset = x >> 2;
        flags = 0x10; // ACK
        a = (Offset << 12) + flags;
        tcp->DoRF = htons(a);
        tcp->WindowSize = htons(1280);
        tcp->CheckSum = 0;
        tcp->UrgentPtr = 0;



        ip->length = htons(((ip->revSize & 0xF) * 4) + 20);
        //Ip checksum
        sum=0;
        etherSumWords(&ip->revSize, 10);
        etherSumWords(ip->sourceIp, ((ip->revSize & 0xF) * 4) - 12);
        ip->headerChecksum = getEtherChecksum();

        //TCP checksum

        uint16_t tmp16;

        uint16_t tcpLen = htons(20);
        sum = 0;
        etherSumWords(ip->sourceIp, 8);

        tmp16 = ip->protocol;
        sum += (tmp16 & 0xff) << 8;
        etherSumWords(&tcpLen,2);

        etherSumWords(tcp, 20);
        // etherSumWords(&tcp->data, 0);
        tcp->CheckSum = getEtherChecksum();

        etherPutPacket((uint8_t*)ether, 14 + ((ip->revSize & 0xF) * 4) +  20);
}
