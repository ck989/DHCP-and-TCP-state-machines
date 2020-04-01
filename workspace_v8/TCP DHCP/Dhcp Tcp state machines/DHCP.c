/*
 * DHCP.c
 *
 *  Created on: 31-Mar-2020
 *      Author: chaya kumar gowda
 */


#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "DHCP.h"

extern uint32_t ipAddress[IP_ADD_LENGTH];
extern uint32_t ipSubnetMask[4];

extern void etherSumWords(void* data, uint16_t sizeInBytes);
extern uint16_t getEtherChecksum();
extern uint32_t sum;
//ipAddress
uint8_t DhcpIpaddress[4];
uint8_t DhcpipGwAddress[4];


bool IsDhcpOffer(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    udpFrame* udp = (udpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));
    dhcpFrame* dhcp = (dhcpFrame*)&udp->data;

    bool ok;
    // my MAC address
    ok = (dhcp->chaddr[0] == 2);
    ok &= (dhcp->chaddr[1] == 3);
    ok &= (dhcp->chaddr[2] == 4);
    ok &= (dhcp->chaddr[3] == 5);
    ok &= (dhcp->chaddr[4] == 6);
    ok &= (dhcp->chaddr[5] == 141);

    ok &= ((dhcp->options[0] == 53) && (dhcp->options[2] == DHCPOFFER));
    //validating the offer checksum
    uint16_t tmp16;

    if (ok)
    {
        /*
         *validate IP checksum from offer
         */
        sum = 0;
        etherSumWords(&ip->revSize, (ip->revSize & 0xF) * 4);
        ok = (getEtherChecksum() == 0);


        /*
         * validate UDP checksum from offer
         */
        // 32-bit sum over pseudo-header
        sum = 0;
        etherSumWords(ip->sourceIp, 8);
        tmp16 = ip->protocol;
        sum += (tmp16 & 0xff) << 8;
        etherSumWords(&udp->length, 2);
        // add udp header and data
        etherSumWords(udp, ntohs(udp->length));
        //etherSumWords(&udp->data, ntohs(240 + 34));
        ok &= (getEtherChecksum() == 0);
    }

    return ok;

}

bool IsDhcpAck(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    udpFrame* udp = (udpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));
    dhcpFrame* dhcp = (dhcpFrame*)&udp->data;

    bool ok;
    ok = (dhcp->chaddr[0] == 2);
    ok &= (dhcp->chaddr[1] == 3);
    ok &= (dhcp->chaddr[2] == 4);
    ok &= (dhcp->chaddr[3] == 5);
    ok &= (dhcp->chaddr[4] == 6);
    ok &= (dhcp->chaddr[5] == 141);

    ok &= ((dhcp->options[0] == 53) && (dhcp->options[2] == DHCPACK));

    //validating ack checksum
    uint16_t tmp16;
    if (ok)
    {
           // 32-bit sum over pseudo-header
        //udp->length = htons(8 + 240 + 32);
        sum = 0;
        etherSumWords(ip->sourceIp, 8);
        tmp16 = ip->protocol;
        sum += (tmp16 & 0xff) << 8;
        etherSumWords(&udp->length, 2);
        // add udp header and data
        etherSumWords(udp, ntohs(udp->length));
        //etherSumWords(&udp->data, 240 + 32);
        ok = (getEtherChecksum() == 0);
    }


    return ok;

}

bool IsDhcpNAck(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    udpFrame* udp = (udpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));
    dhcpFrame* dhcp = (dhcpFrame*)&udp->data;

    bool ok;
    ok = (dhcp->chaddr[0] == 2);
    ok &= (dhcp->chaddr[1] == 3);
    ok &= (dhcp->chaddr[2] == 4);
    ok &= (dhcp->chaddr[3] == 5);
    ok &= (dhcp->chaddr[4] == 6);
    ok &= (dhcp->chaddr[5] == 141);

    ok &= ((dhcp->options[0] == 53) && (dhcp->options[2] == DHCPNACK));

    return ok;
}

void sendDHCPmessage(uint8_t packet[], uint8_t type, uint8_t ipAdd[])
{
    uint8_t i;
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    udpFrame* udp = (udpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));
    dhcpFrame* dhcp = (dhcpFrame*)&udp->data;


    //populating ether frame
    for(i=0;i<6;i++)
    {
        ether->destAddress[i] =  0xFF;
    }
    ether->sourceAddress[0] = 2;
    ether->sourceAddress[1] = 3;
    ether->sourceAddress[2] = 4;
    ether->sourceAddress[3] = 5;
    ether->sourceAddress[4] = 6;
    ether->sourceAddress[5] = 141;

    ether->frameType = htons(0x0800);


    //populating ip frame
    ip->typeOfService = 0;
    ip->id = 0;
    ip->protocol = 17;//udp
    ip->flagsAndOffset = 0;
    ip->ttl = 128;
    for(i=0;i<4;i++)
    {
        ip->sourceIp[i] = 0;
    }
    for(i=0;i<4;i++)
    {
        ip->destIp[i] = 255;
    }

    //populating udp frame
    udp->sourcePort = htons(68);
    udp->destPort = htons(67);


    //populating dhcp frame
    dhcp->op = BOOTREQUEST;
    dhcp->htype = 0x01;
    dhcp->hlen = 6;
    dhcp->hops = 0;
    dhcp->xid = 0xdca15b8c;
    dhcp->secs = 0;
    dhcp->flags = htons(0x8000);
    dhcp->ciaddr[0] = 0;
    dhcp->ciaddr[1] = 0;
    dhcp->ciaddr[2] = 0;
    dhcp->ciaddr[3] = 0;
    dhcp->yiaddr[0] = 0;
    dhcp->yiaddr[1] = 0;
    dhcp->yiaddr[2] = 0;
    dhcp->yiaddr[3] = 0;
    dhcp->siaddr[0] = 0;
    dhcp->siaddr[1] = 0;
    dhcp->siaddr[2] = 0;
    dhcp->siaddr[3] = 0;
    dhcp->giaddr[0] = 0;
    dhcp->giaddr[1] = 0;
    dhcp->giaddr[2] = 0;
    dhcp->giaddr[3] = 0;
    dhcp->chaddr[0] = 2;
    dhcp->chaddr[1] = 3;
    dhcp->chaddr[2] = 4;
    dhcp->chaddr[3] = 5;
    dhcp->chaddr[4] = 6;
    dhcp->chaddr[5] = 141;
    for(i=6;i<16;i++)
    {
        dhcp->chaddr[i] = 0;
    }
    for( i=0; i<192; i++)
    {
        dhcp->data[i] = 0;
    }

    dhcp->magicCookie = 0x63538263;


    dhcp->options[0] = type; // type 53
    dhcp->options[1] = 1;
    dhcp->options[2] = DHCPDISCOVER;
    dhcp->options[3] = 55; // parameter request list
    dhcp->options[4] = 5;  // hardware length
    dhcp->options[5] = 1;  // SN mask
    dhcp->options[6] = 2;  // time offset
    dhcp->options[7] = 3;  // router
    dhcp->options[8] = 6;  // DNS
    dhcp->options[9] = 51; // lease time
    dhcp->options[10] = 61; // client identifier
    dhcp->options[11] = 7;
    dhcp->options[12] = 0x01;
    dhcp->options[13] = 0x02;
    dhcp->options[14] = 0x03;
    dhcp->options[15] = 0x04;
    dhcp->options[16] = 0x05;
    dhcp->options[17] = 0x06;
    dhcp->options[18] = 0x8D;

    dhcp->options[19] = 255; // end

    uint16_t tmp16;

    ip->length = htons(((ip->revSize & 0xF) * 4) + 8 + 240+20 );
    sum=0;
    //uint16_t dhcpSize = sizeof(dhcpFrame);
    etherSumWords(&ip->revSize, 10);
    etherSumWords(ip->sourceIp, ((ip->revSize & 0xF) * 4) - 12);
    ip->headerChecksum = getEtherChecksum();


    udp->length = htons(8 + 240 + 20 );
    sum = 0;
    etherSumWords(ip->sourceIp, 8);
    tmp16 = ip->protocol;
    sum += (tmp16 & 0xff) << 8;
    etherSumWords(&udp->length, 2);
    // add udp header except crc
    etherSumWords(udp, 6);
    etherSumWords(&udp->data, 240 + 20);
    udp->check = getEtherChecksum();


    etherPutPacket((uint8_t*)ether, 14 + ((ip->revSize & 0xF) * 4) + 8 + 240+20);

}

uint8_t etherOfferBuff[HW_ADD_LENGTH];
uint8_t ipOfferBuff[IP_ADD_LENGTH];
uint8_t DhcpYraddOffBuff[IP_ADD_LENGTH];
uint8_t DhcpGiaddOffBuff[IP_ADD_LENGTH];
uint8_t DhcpSerIdOffBuff[IP_ADD_LENGTH];
int32_t DhcpOffLTime = 0;
uint8_t DhcpSnOffBuff[IP_ADD_LENGTH];
uint8_t DhcpRoutOffBuff[IP_ADD_LENGTH];

void CollectDhcpOfferData(uint8_t packet[])
{
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    udpFrame* udp = (udpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));
    dhcpFrame* dhcp = (dhcpFrame*)&udp->data;

    uint8_t i = 0;

    // store source fields in buffer
    for (i = 0; i < HW_ADD_LENGTH; i++)
    {
        etherOfferBuff[i] = ether->sourceAddress[i];
    }

    // store source Ip in buffer
    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        ipOfferBuff[i] = ip->sourceIp[i];
    }

    // store your IP in buffer
    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        DhcpYraddOffBuff[i] = dhcp->yiaddr[i];
    }

    // store your relay IP in buffer
    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        DhcpGiaddOffBuff[i] = dhcp->giaddr[i];
    }
/*
 * storing options field from DHCP offer
 */
    uint8_t j, len;
    uint32_t temp;

    i = 3; // skipping type 53 in options


    while( dhcp->options[i]!= 255)
    {
        switch(dhcp->options[i])
        {
        case 1: // save SN mask
            len = dhcp->options[++i];
            i++;
            for (j = 0; j < len; j++)
            {
                DhcpSnOffBuff[j] = dhcp->options[i];
                i++;
            }
            break;

        case 3: // save router IP
            len = dhcp->options[++i];
            i++;
            for (j = 0; j < len; j++)
            {
                DhcpRoutOffBuff[j] = dhcp->options[i];
                i++;
            }
            break;

        case 51:// save IP address lease time
            len = dhcp->options[++i];
            //k = (len*8) - 8;
            DhcpOffLTime = 0;
            j=0;
            //DhcpOffLTime = (uint32_t)dhcp->options[20];
            while(j < (len-1))
            {
                temp = dhcp->options[++i];
                DhcpOffLTime += (temp & 0xff) ;
                DhcpOffLTime = DhcpOffLTime << 8;
                //k = k - 8;
                j++;
            }
            temp = dhcp->options[++i];
            DhcpOffLTime += temp;
            i++;
            break;

        case 54:// store DHCP server Identifier in buffer
            len = dhcp->options[++i];
            i++;
            for (j = 0; j < len; j++)
            {
                DhcpSerIdOffBuff[j] = dhcp->options[i];
                i++;
            }
            break;


        default:
            len = dhcp->options[++i];
            i = i + len;
            i++;
        }

    }

}

void  SendDhcpRequest(uint8_t packet[], uint8_t type, uint8_t ipAdd[])
{
    uint8_t i;
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    udpFrame* udp = (udpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));
    dhcpFrame* dhcp = (dhcpFrame*)&udp->data;

    //populating ether frame
        for(i=0;i<6;i++)
        {
            ether->destAddress[i] =  0xFF;
        }
        ether->sourceAddress[0] = 2;
        ether->sourceAddress[1] = 3;
        ether->sourceAddress[2] = 4;
        ether->sourceAddress[3] = 5;
        ether->sourceAddress[4] = 6;
        ether->sourceAddress[5] = 141;

        ether->frameType = htons(0x0800);


        //populating ip frame
        ip->typeOfService = 0;
        ip->id = 0;
        ip->protocol = 17;//udp
        ip->flagsAndOffset = 0;
        ip->ttl = 128;
        for(i=0;i<4;i++)
        {
            ip->sourceIp[i] = 0;
        }
        for(i=0;i<4;i++)
        {
            ip->destIp[i] = 255;
        }

        //populating udp frame
        udp->sourcePort = htons(68);
        udp->destPort = htons(67);


        //populating dhcp frame
        dhcp->op = BOOTREQUEST;
        dhcp->htype = 0x01;
        dhcp->hlen = 6;
        dhcp->hops = 0;
        dhcp->xid = 0xdca15b8c;
        dhcp->secs = 0;
        dhcp->flags = htons(0x8000);
        dhcp->ciaddr[0] = 0;
        dhcp->ciaddr[1] = 0;
        dhcp->ciaddr[2] = 0;
        dhcp->ciaddr[3] = 0;
        dhcp->yiaddr[0] = 0;
        dhcp->yiaddr[1] = 0;
        dhcp->yiaddr[2] = 0;
        dhcp->yiaddr[3] = 0;
        dhcp->siaddr[0] = 0;
        dhcp->siaddr[1] = 0;
        dhcp->siaddr[2] = 0;
        dhcp->siaddr[3] = 0;
        dhcp->giaddr[0] = 0;
        dhcp->giaddr[1] = 0;
        dhcp->giaddr[2] = 0;
        dhcp->giaddr[3] = 0;
        dhcp->chaddr[0] = 2;
        dhcp->chaddr[1] = 3;
        dhcp->chaddr[2] = 4;
        dhcp->chaddr[3] = 5;
        dhcp->chaddr[4] = 6;
        dhcp->chaddr[5] = 141;
        for(i=6;i<16;i++)
        {
            dhcp->chaddr[i] = 0;
        }
        for( i=0; i<192; i++)
        {
            dhcp->data[i] = 0;
        }

        dhcp->magicCookie = 0x63538263;


        dhcp->options[0] = type; // type 53
        dhcp->options[1] = 1;
        dhcp->options[2] = DHCPREQUEST;

        dhcp->options[3] = 61; // client identifier
        dhcp->options[4] = 7;
        dhcp->options[5] = 0x01;
        dhcp->options[6] = 0x02;
        dhcp->options[7] = 0x03;
        dhcp->options[8] = 0x04;
        dhcp->options[9] = 0x05;
        dhcp->options[10] = 0x06;
        dhcp->options[11] = 0x8D;

        dhcp->options[12] = 50;// requested IP address
        dhcp->options[13] = 4;
        dhcp->options[14] = DhcpYraddOffBuff[0];
        dhcp->options[15] = DhcpYraddOffBuff[1];
        dhcp->options[16] = DhcpYraddOffBuff[2];
        dhcp->options[17] = DhcpYraddOffBuff[3];

        dhcp->options[18] = 54;// DHCP server identifier
        dhcp->options[19] = 4;
        dhcp->options[20] = DhcpSerIdOffBuff[0];
        dhcp->options[21] = DhcpSerIdOffBuff[1];
        dhcp->options[22] = DhcpSerIdOffBuff[2];
        dhcp->options[23] = DhcpSerIdOffBuff[3];

        dhcp->options[24] = 55;// parameter request list
        dhcp->options[25] = 5; // length
        dhcp->options[26] = 1; // subnet mask
        dhcp->options[27] = 3; // router
        dhcp->options[28] = 6; // domain name server
        dhcp->options[29] = 15;// domain name
        dhcp->options[30] = 51;//lease time

        dhcp->options[31] = 255; // end

        uint16_t tmp16;

        //IP checksum
        ip->length = htons(((ip->revSize & 0xF) * 4) + 8 + 240 + 32 );
        sum=0;
        //uint16_t dhcpSize = sizeof(dhcpFrame);
        etherSumWords(&ip->revSize, 10);
        etherSumWords(ip->sourceIp, ((ip->revSize & 0xF) * 4) - 12);
        ip->headerChecksum = getEtherChecksum();

        //UDP checksum
        udp->length = htons(8 + 240 + 32 );
        sum = 0;
        etherSumWords(ip->sourceIp, 8);
        tmp16 = ip->protocol;
        sum += (tmp16 & 0xff) << 8;
        etherSumWords(&udp->length, 2);
        // add udp header except crc
        etherSumWords(udp, 6);
        etherSumWords(&udp->data, 240 + 32);
        udp->check = getEtherChecksum();

        etherPutPacket((uint8_t*)ether, 14 + ((ip->revSize & 0xF) * 4) + 8 + 240 + 32);

}


void CollectDhcpAckData(uint8_t packet[])
{

    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    udpFrame* udp = (udpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));
    dhcpFrame* dhcp = (dhcpFrame*)&udp->data;

    uint8_t i = 0;

   /*
    * Get IP address given by server
    */

    for(i = 0; i < IP_ADD_LENGTH; i++)
    {
        DhcpIpaddress[i] = dhcp->yiaddr[i];
    }

    /*
     * Get IP gateway address given by server
     */

     for(i = 0; i < IP_ADD_LENGTH; i++)
     {
         DhcpipGwAddress[i] = dhcp->giaddr[i];
     }

    /*
     * storing options field from DHCP ACK
     */
        uint8_t j, len;
        uint32_t temp;

        i = 3; // skipping type 53 in options

        while(dhcp->options[i] != 255)
        {
            switch(dhcp->options[i])
            {
            case 1:// get Sub net mask fiven by server
                len = dhcp->options[++i];
                i++;
                for (j = 0; j < len; j++)
                {
                    ipSubnetMask[j] = dhcp->options[i];
                    i++;
                }
                break;

            case 3: // save router IP
                len = dhcp->options[++i];
                i++;
                for (j = 0; j < len; j++)
                {
                    DhcpRoutOffBuff[j] = dhcp->options[i];
                    i++;
                }
                break;

            case 51:// save IP address lease time
                len = dhcp->options[++i];
                //k = (len*8) - 8;
                DhcpOffLTime = 0;
                j=0;
                //DhcpOffLTime = (uint32_t)dhcp->options[20];
                while(j < (len-1))
                {
                    temp = dhcp->options[++i];
                    DhcpOffLTime += (temp & 0xff) ;
                    DhcpOffLTime = DhcpOffLTime << 8;
                    //k = k - 8;
                    j++;
                }
                temp = dhcp->options[++i];
                DhcpOffLTime += temp;
                i++;
                break;

            case 54:// store DHCP server Identifier in buffer
                len = dhcp->options[++i];
                i++;
                for (j = 0; j < len; j++)
                {
                    DhcpSerIdOffBuff[j] = dhcp->options[i];
                    i++;
                }
                break;


            default:
                len = dhcp->options[++i];
                i = i + len;
                i++;


            }
        }

}

void SendDhcpRefresh(uint8_t packet[], uint8_t type, uint8_t ipAdd[])
{
    uint8_t i;
    etherFrame* ether = (etherFrame*)packet;
    ipFrame* ip = (ipFrame*)&ether->data;
    ip->revSize = 0x45;
    udpFrame* udp = (udpFrame*)((uint8_t*)ip + ((ip->revSize & 0xF) * 4));
    dhcpFrame* dhcp = (dhcpFrame*)&udp->data;

    //populating ether frame
        for(i=0;i<6;i++)
        {
            ether->destAddress[i] =  0xFF;
        }
        ether->sourceAddress[0] = 2;
        ether->sourceAddress[1] = 3;
        ether->sourceAddress[2] = 4;
        ether->sourceAddress[3] = 5;
        ether->sourceAddress[4] = 6;
        ether->sourceAddress[5] = 141;

        ether->frameType = htons(0x0800);


        //populating ip frame
        ip->typeOfService = 0;
        ip->id = 0;
        ip->protocol = 17;//udp
        ip->flagsAndOffset = 0;
        ip->ttl = 128;
        for(i=0;i<4;i++)
        {
            ip->sourceIp[i] = 0;
        }
        for(i=0;i<4;i++)
        {
            ip->destIp[i] = 255;
        }

        //populating udp frame
        udp->sourcePort = htons(68);
        udp->destPort = htons(67);


        //populating dhcp frame
        dhcp->op = BOOTREQUEST;
        dhcp->htype = 0x01;
        dhcp->hlen = 6;
        dhcp->hops = 0;
        dhcp->xid = 0xdca15b8c;
        dhcp->secs = 0;
        dhcp->flags = htons(0x8000);
        dhcp->ciaddr[0] = 0;
        dhcp->ciaddr[1] = 0;
        dhcp->ciaddr[2] = 0;
        dhcp->ciaddr[3] = 0;
        dhcp->yiaddr[0] = 0;
        dhcp->yiaddr[1] = 0;
        dhcp->yiaddr[2] = 0;
        dhcp->yiaddr[3] = 0;
        dhcp->siaddr[0] = 0;
        dhcp->siaddr[1] = 0;
        dhcp->siaddr[2] = 0;
        dhcp->siaddr[3] = 0;
        dhcp->giaddr[0] = 0;
        dhcp->giaddr[1] = 0;
        dhcp->giaddr[2] = 0;
        dhcp->giaddr[3] = 0;
        dhcp->chaddr[0] = 2;
        dhcp->chaddr[1] = 3;
        dhcp->chaddr[2] = 4;
        dhcp->chaddr[3] = 5;
        dhcp->chaddr[4] = 6;
        dhcp->chaddr[5] = 141;
        for(i=6;i<16;i++)
        {
            dhcp->chaddr[i] = 0;
        }
        for( i=0; i<192; i++)
        {
            dhcp->data[i] = 0;
        }

        for( i=0; i < IP_ADD_LENGTH; i++)
        {
            ipAddress[i] = dhcp->yiaddr[i];
        }
        dhcp->magicCookie = 0x63538263;


        dhcp->options[0] = type; // type 53
        dhcp->options[1] = 1;
        dhcp->options[2] = DHCPRELEASE;

        dhcp->options[3] = 255; // end

        uint16_t tmp16;

        //IP checksum
        ip->length = htons(((ip->revSize & 0xF) * 4) + 8 + 240 + 4 );
        sum=0;
        //uint16_t dhcpSize = sizeof(dhcpFrame);
        etherSumWords(&ip->revSize, 10);
        etherSumWords(ip->sourceIp, ((ip->revSize & 0xF) * 4) - 12);
        ip->headerChecksum = getEtherChecksum();

        //UDP checksum
        udp->length = htons(8 + 240 + 4 );
        sum = 0;
        etherSumWords(ip->sourceIp, 8);
        tmp16 = ip->protocol;
        sum += (tmp16 & 0xff) << 8;
        etherSumWords(&udp->length, 2);
        // add udp header except crc
        etherSumWords(udp, 6);
        etherSumWords(&udp->data, 240 + 4);
        udp->check = getEtherChecksum();

        etherPutPacket((uint8_t*)ether, 14 + ((ip->revSize & 0xF) * 4) + 8 + 240 + 4);

}
