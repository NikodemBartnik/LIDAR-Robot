/*********************************************************************************
File name:	  CLidarPacketReceiver.h
Author:       Kimbo
Version:      V1.7.1
Date:	 	  2017-02-03
Description:  lidar packet receiver
Others:       None

History:
	1. Date:
	Author:
	Modification:
***********************************************************************************/

/********************************* File includes **********************************/
#include "CLidarPacketReceiver.h"

/******************************* Current libs includes ****************************/
#include "CDeviceConnection.h"
#include "CCountDown.h"
#include <unistd.h>

/********************************** Name space ************************************/
using namespace everest;
using namespace everest::hwdrivers;


/***********************************************************************************
Function:     CLidarPacketReceiver
Description:  The constructor of CLidarPacketReceiver
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
CLidarPacketReceiver::CLidarPacketReceiver()
{
    m_device_conn = NULL;
    m_log_when_receive_time_over = false;
    reset();
    m_counter = 0;
}

/***********************************************************************************
Function:     CLidarPacketReceiver
Description:  The destructor of CLidarPacketReceiver
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
CLidarPacketReceiver::~CLidarPacketReceiver()
{
    if(m_save_fp)
    {
        m_save_fp.close();
    }
}

/***********************************************************************************
Function:     reset
Description:  Reset
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
void CLidarPacketReceiver::reset()
{
    m_state = PACKET_HEADER;
    m_actual_count = 0;
    m_packet_length = 0;
	m_packet_remainder_length = 0;
}

/***********************************************************************************
Function:     receivePacket
Description:  Receive lidar packet, if return true, it means receive a valid packet
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
bool CLidarPacketReceiver::receivePacket(CLidarPacket *packet)
{
	/* Judge whether serial is connecting */
	if (packet == NULL || m_device_conn == NULL) 
	{
		printf("[CLidarPacketReceiver] receivePacket: connection not open!\n");
		return false;
	}

    /* Read packet */
	m_count_down.setTime((double)m_params.packet_max_time_ms);
	char ch;
	while(1)
	{
		//usleep(10);
		if(m_count_down.isEnd())
		{
			printf("[CLidarPacketReceiver] Receive packet time %5.2f ms is over!\n", m_count_down.getInputTime());
			if(m_log_when_receive_time_over)
            {
                printf("[CLidarPacketReceiver] Receive packet time is over!\n");
            }
			return false;
		}

        int read_bytes = m_device_conn->read((char *)&ch, 1, 1);
		if(read_bytes == 0)
		{
			continue;
		}
		else if(read_bytes < 0)
		{
		    printf("[CLidarPacketReceiver] finish read data read bytes is %d!\n", read_bytes);
		    return false;
		}
		else
		{
		    TPacketResult packet_result = readPacket(packet, ch);
		    switch(packet_result)
		    {
                case PACKET_ING: break;
		        case PACKET_SUCCESS:
		        {
                    reset();
		            return true;
		        }
		        case PACKET_FAILED:
		        {
                    reset();
		            return false;
		        }
		    }
		}
	}

	printf("[CLidarPacketReceiver] It should not come to here!\n");
	return false;
}

/***********************************************************************************
Function:     readPacket
Description:  Read packet, if it return ture, it means read complete packet or enter
              erro state
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
CLidarPacketReceiver::TPacketResult CLidarPacketReceiver::readPacket(CLidarPacket *packet, u8 ch)
{
    TPacketResult packet_result = PACKET_ING;
    switch(m_state)
    {
        case PACKET_HEADER: packet_result = processPacketHeader(packet, ch); break; 
		case PACKET_LENGHTLO: packet_result =  processPacketLengthLO(packet, ch); break;
        case PACKET_LENGHTHI: packet_result =  processPacketLengthHI(packet, ch); break;
		case PACKET_PROTOCOL_VER: packet_result = processPacketProtocolVer(packet, ch); break;
		//命令字：通讯错位标志/通讯方向 判断:丢包不解析	
        case PACKET_REMAINDER_DATA: packet_result = processPacketRemainderData(packet, ch); break;
        default:
            printf("[CLidarPacketReceiver] Enter erro state %d!\n", m_state);
        break;
    }
    return packet_result;
}

CLidarPacketReceiver::TPacketResult CLidarPacketReceiver::processPacketHeader(CLidarPacket *packet, u8 ch)
{
    if(ch == PacketHeader)
    {
    	packet->reset();
        packet->pushBack(ch);
        m_state = PACKET_LENGHTLO;
        m_count_down.setTime(m_params.packet_wait_time_ms);
    }
    return PACKET_ING;
}

CLidarPacketReceiver::TPacketResult CLidarPacketReceiver::processPacketLengthLO(CLidarPacket *packet, u8 ch)
{
	u8 temp = ch;

    packet->pushBack(ch);
    // add lowbyte
    m_packet_length = ch;
	
    m_state = PACKET_LENGHTHI;

    return PACKET_ING;
}

CLidarPacketReceiver::TPacketResult CLidarPacketReceiver::processPacketLengthHI(CLidarPacket *packet, u8 ch)
{
	u8 temp = ch;
	
    packet->pushBack(ch);
	
	m_packet_length |= (ch << 8);
	
	//one packet remainder length = sub 4 bytes(header(1bytes) and length(2bytes) and protocol_ver),add verify 2 byte
	m_packet_remainder_length = m_packet_length - 4+2;
    m_state = PACKET_PROTOCOL_VER;

    return PACKET_ING;
}


CLidarPacketReceiver::TPacketResult CLidarPacketReceiver::processPacketProtocolVer(CLidarPacket *packet, u8 ch)
{
   // if(ch == ProtocolVer)
    //{
    	packet->pushBack(ch);
   		 m_state = PACKET_REMAINDER_DATA;   
    //}
	//else
	//{
	//	packet->reset();
	//}    
    return PACKET_ING;
}


/***********************************************************************************
Function:     processStateAcquireData
Description:  Process state acquire data
Input:        None
Output:       None
Return:       None
Others:       None
***********************************************************************************/
CLidarPacketReceiver::TPacketResult CLidarPacketReceiver::processPacketRemainderData(CLidarPacket *packet, u8 ch)
{
    m_actual_count++;
    packet->pushBack(ch);
    if(m_actual_count == m_packet_remainder_length)
    {
        reset();
        if(packet->verify16BitAccCheckSum())
        {
        	
            return PACKET_SUCCESS;
        }
        else
        {
            printf("[CLidarPacketReceiver] 16bit Acc verify wrong!\n");
            return PACKET_FAILED;
        }
    }
    return PACKET_ING;
}


