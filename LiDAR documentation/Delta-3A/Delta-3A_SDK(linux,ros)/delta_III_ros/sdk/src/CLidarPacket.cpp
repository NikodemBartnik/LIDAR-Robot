/*********************************************************************************
File name:	  CLidarPacket.cpp
Author:       Kimbo
Version:      V1.7.1
Date:	 	  2017-02-04
Description:  lidar packet
Others:       None

History:
	1. Date:
	Author:
	Modification:
***********************************************************************************/

/********************************** File includes *********************************/
#include "CLidarPacket.h"

/********************************** Current libs includes *************************/
//#include "C3iroboticsLidarProtocol.h"

/********************************** System includes *******************************/
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

/********************************** Name space ************************************/
using namespace std;
using namespace everest;
using namespace everest::hwdrivers;

/***********************************************************************************
Function:     CLidarPacket
Description:  The constructor of CLidarPacket
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
CLidarPacket::CLidarPacket()
{
	reset();
}

/***********************************************************************************
Function:     reset
Description:  Reset packet
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
void CLidarPacket::reset()
{
    m_recvbuf.resize(m_params.recvbuf_size);
	m_sendbuf.resize(m_params.sendbuf_size);
    m_read_length = 0;
    m_length = 0;
}

/***********************************************************************************
Function:     hasWriteCapacity
Description:  Reset packet
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
bool CLidarPacket::hasWriteCapacity(int bytes)
{
	if (bytes < 0)
	{
	    printf("[CLidarPacket] You should input bytes %d less than 0!\n", bytes);
		return false;
	}

	// Make sure there's enough room in the packet
	if ((m_length + bytes) <= m_params.recvbuf_size)
	{
		
        return true;
	}
	else
	{
		
		return false;
	}
}

/***********************************************************************************
Function:     pushBack
Description:  pushBack
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
void CLidarPacket::pushBack(u8 ch)
{
	if(!hasWriteCapacity(1))
	{
		return ;
	}
	m_recvbuf[m_length] = ch;
	m_length++;
}

/***********************************************************************************
Function:     bufToUByte2
Description:  buf to u16
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
u16 CLidarPacket::bufToUByte2(u8 *src_ptr)
{
    u16 data = (src_ptr[0] << 8) | ((u8)src_ptr[1]);
    return data;
}

/***********************************************************************************
Function:     bufToUByte2
Description:  buf to u16
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
s16 CLidarPacket::bufToByte2(u8 *src_ptr)
{
    s16 data = (src_ptr[0] << 8) | ((s8)src_ptr[1]);
    return data;
}

/***********************************************************************************
Function:     bufToUByte
Description:  buf to u8
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
u8 CLidarPacket::bufToUByte(u8 *src_ptr)
{
    u8 data = src_ptr[0];
    return data;
}

/***********************************************************************************
Function:     printHex
Description:  Buffer to data
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
void CLidarPacket::bufferToData(void *dest_ptr, void *src_ptr, size_t length)
{
    printf("[CLidarPacket] It has not realize now!\n");
}

/***********************************************************************************
Function:     printHex
Description:  printf packet buf as hex format
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/

void CLidarPacket::printHex(u8* buf,u16 size)
{
	{
		int i = 0;
		printf("buf:");
		while(size--)
		{
			printf("%02X ",*(buf+i));
			i++;
		}
		printf("\n");
	}
	/*
    printf("[CRobotPacket] length %d, read_length %d!\n",
              m_length, m_read_length);

    for(size_t i = 0; i < m_length; i++)
    {
        printf("buf[%d] = 0x%x!\n", (int)i, (m_recvbuf[i]));
    }    
	*/
}


u16 CLidarPacket::calc16BitAccCheckSum(u8 *ptr, u16 len)
{
	unsigned short cal_checksum = 0; 
	u16 i=0;
	while (i<len) 
	{ 
		cal_checksum += *(ptr+i);
		i++;
	} 
	return cal_checksum ; 
}

bool CLidarPacket::verify16BitAccCheckSum(void)
{
	u16 cal_checksum = 0;
	u16 recv_checksum = 0;
	u8 value[2] = {0,0};
	value[0]=m_recvbuf[m_length-2];
	value[1]=m_recvbuf[m_length-1];
	u8 temp = value[0];
	value[0] = value[1];
	value[1] = temp;
	recv_checksum = bufToUByte2(&value[0]); 
	if (m_length < m_params.least_packet_len)
    {
    	printf("packer.m_length<least_packet_len,error\n");
        return FALSE;
    }
	
	cal_checksum = calc16BitAccCheckSum(&m_recvbuf[0],(m_length -2));
	if(cal_checksum == recv_checksum)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


