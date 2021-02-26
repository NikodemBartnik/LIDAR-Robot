/*********************************************************************************
File name:	  CLidarPacketSender.h
Author:       Kimbo
Version:      V1.7.1
Date:	 	  2017-02-03
Description:  lidar packet Sender
Others:       None

History:
	1. Date:
	Author:
	Modification:
***********************************************************************************/

/********************************* File includes **********************************/
#include "CLidarPacketSender.h"

/******************************* Current libs includes ****************************/

/********************************** Name space ************************************/
using namespace everest;
using namespace everest::hwdrivers;


/* Constructor */
CLidarPacketSender::CLidarPacketSender()
{

}

/* Destructor */
CLidarPacketSender::~CLidarPacketSender()
{

}


void CLidarPacketSender::WorkModePacket(CLidarPacket &packet,u8 mode)///////////////////////////////
{
	u16 temp = 0x00;
	packet.m_params.sendbuf_size = 10;
    packet.m_sendbuf.resize(packet.m_params.sendbuf_size);
	packet.m_sendbuf[0]=PacketHeader;
	temp = packet.m_params.sendbuf_size-2;
	packet.m_sendbuf[1]=temp&0x00FF;
	packet.m_sendbuf[2]=temp>>8;
	packet.m_sendbuf[3]=0x02;//protocol version
	packet.m_sendbuf[4]=0x00;
	//bit7: Communication error sign
	//bit6:0 master --> lidar
	packet.m_sendbuf[4] |=0x00;
	//bit5~0:commond id
	packet.m_sendbuf[4] |=0x01;
	temp = 0x0001;
	packet.m_sendbuf[5]=temp&0x00FF;
	packet.m_sendbuf[6]=temp>>8;
	packet.m_sendbuf[7]=mode;
	temp = packet.calc16BitAccCheckSum(&packet.m_sendbuf[0],packet.m_params.sendbuf_size-2);
	packet.m_sendbuf[8]= temp&0x00FF;
	packet.m_sendbuf[9]= temp>>8;
	//packet.printHex(&packet.m_sendbuf[0],packet.m_params.sendbuf_size);
}

void CLidarPacketSender::setLidarRotationlSpeedPacket(CLidarPacket &packet,uint32_t speed)///////////////
{
	u16 temp = 0x00;
	packet.m_params.sendbuf_size = 11;
	packet.m_sendbuf.resize(packet.m_params.sendbuf_size);
	packet.m_sendbuf[0]=PacketHeader;
	temp = packet.m_params.sendbuf_size-2;
	packet.m_sendbuf[1]=temp&0x00FF;
	packet.m_sendbuf[2]=temp>>8;
	packet.m_sendbuf[3]=0x04;
	packet.m_sendbuf[4]=0x00;
	//bit7: Communication error sign
	//bit6:0 master --> lidar
	packet.m_sendbuf[4] |=0x00;
	//bit5~0:commond id
	packet.m_sendbuf[4] |=0x04;
	temp = 0x0002;
	packet.m_sendbuf[5]=temp&0x00FF;
	packet.m_sendbuf[6]=temp>>8;
	temp = speed;
	packet.m_sendbuf[7]=temp&0x00FF;
	packet.m_sendbuf[8]=temp>>8;
	temp = packet.calc16BitAccCheckSum(&packet.m_sendbuf[0],packet.m_params.sendbuf_size-1);
	packet.m_sendbuf[9]= temp&0x00FF;
	packet.m_sendbuf[10]= temp>>8;
	//packet.printHex(&packet.m_sendbuf[0],packet.m_params.sendbuf_size);
}



