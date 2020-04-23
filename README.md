# DHCP-and-TCP-state-machines

This is a University of Texas at arlington course based project using ARM cortex M4 based microcontroller.

DHCP client and TCP server application is implemented on TM4C123GH6PM microcontroller.

Aim:
The aim of this project is to build the essential components of basic embedded ethernet appliances.

The solution of this project is implemented on TM4C123GH6PM microcontroller along with ENC28J60 ethernet interface.
ENC28J60 ethernet is interfaced with TM4C123GH6PM microcontroller using SSI interface or in other words, using SPI communication protocol.
SPI in TM4C123GH6PM microcontroller is called as SSI. 

Hardware connections:
The following pins are connected via using SPI0 of TM4C123GH6PM microcontroller

 ENC28J60 Ethernet controller on SPI0
   MOSI (SSI0Tx) on PA5
   MISO (SSI0Rx) on PA4
   SCLK (SSI0Clk) on PA2
   ~CS (SW controlled) on PA3
   WOL on PB3
   INT on PC6
Left side are the pins of ENC28J60 and right side are the pins of TM4C123GH6PM microcontroller.

Functionalities of DHCP client in our project:
Our microntroller acts as a DHCP client.
Client sends DISCOVER message to DHCP sever and the server sends OFFER message to the client. In turn client sends REQUEST message and gets an ACK from server.
Hence IP will be assigned to the client.

IP can set in both statically as well as dynamically (By DHCP server). Static IP can be set by user. Statically assigned will be persistently stored in EEPROM of microcontroller (client).

Also, additional DHCP functionalities like Renew request to renew the IP when assigned dynammically by server are also supported.

Putty software is the user interface. The communication between microcontroller and user interface was setup by UART protocol with baud rate of 115200.

when user types "DHCP on", the IP address to the client will be assigned dynamically by DHCP server.
when user types "DHCP off", the IP address can be set statically by user.

The DHCP server used for this project was dhcpsrv2.5.2 for windows.

Codes for DHCP client states machines:
https://github.com/ck989/DHCP-and-TCP-state-machines/blob/master/workspace_v8/TCP%20DHCP/Dhcp%20Tcp%20state%20machines/ethernet.c
https://github.com/ck989/DHCP-and-TCP-state-machines/blob/master/workspace_v8/TCP%20DHCP/Dhcp%20Tcp%20state%20machines/DHCP.c
https://github.com/ck989/DHCP-and-TCP-state-machines/blob/master/workspace_v8/TCP%20DHCP/Dhcp%20Tcp%20state%20machines/DHCP.h

Functionalities of TCP server in our project:

The putty telnet session with port 23 was used as a client and our microntroller was used as a server.

"IP address of the TCP server (microntroller) can either assigned dynamically as well as statically as discussed above in DHCP portion.
If user types "DHCP on" it uses DHCP assigned IP, or if the user types "DHCP off" it will static IP, since static IP will be stored persistently in EEPROM.
The perfromance of TCP will be unaffected irrespective of DHCP modes (on or off)".

As soon as putty tenet session is turned on, TCP SYN will be sent to server or microcontroller.Then this sends TCP SYN ACK. The state flow is illustarted below

               (client)                          (server)
                 SYN      ---------------->       
                          <----------------      SYN ACK
                 ACK       ---------------->
                 
        The above illustration is for starting the connection.
        
        For closing the connection either server or client can initiate 
        
             (client or server)                  (client or server)
                  FIN-ACK    ------------------>
                             <------------------       FIN-ACK
                    ACK      ------------------>
                    
         Connection closes.
         
         All these functionalities are supported in our project.
         
 codes for TCP server state machines:
 
 https://github.com/ck989/DHCP-and-TCP-state-machines/blob/master/workspace_v8/TCP%20DHCP/Dhcp%20Tcp%20state%20machines/ethernet.c
 https://github.com/ck989/DHCP-and-TCP-state-machines/blob/master/workspace_v8/TCP%20DHCP/Dhcp%20Tcp%20state%20machines/TCP.c
 https://github.com/ck989/DHCP-and-TCP-state-machines/blob/master/workspace_v8/TCP%20DHCP/Dhcp%20Tcp%20state%20machines/TCP.h
 
 
 










