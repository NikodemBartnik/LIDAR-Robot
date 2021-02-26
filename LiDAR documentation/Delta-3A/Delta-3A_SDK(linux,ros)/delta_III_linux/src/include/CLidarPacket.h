/*********************************************************************************
File name:	  CLidarPacket.h
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

#ifndef EVEREST_LIDAR_CLIDARPACKET_H
#define EVEREST_LIDAR_CLIDARPACKET_H

/******************************* Current libs includes ****************************/
#include "typedef.h"

/******************************* System libs includes *****************************/
#include <vector>
#include <stddef.h>


namespace everest
{
	namespace hwdrivers
	{
		const u8 PacketHeader = 0xAA;

		struct TParams
                {
                    TParams()
                    {
                        recvbuf_size = 1024;
						sendbuf_size = 10;
                        // 帧头 帧长度 协议版本          命令字 参数长度 参数 校验位
                        //  1    2         1     1			2     1      2
                        //  当帧类型为命令帧时，参数长度以及参数可为0，因此最小帧长度为
                        //  1 +  2  +  1  +1  +  2  +  1  +  2 = 6
                        least_packet_len = 10;
                    }

                    u16 recvbuf_size;
			u16 sendbuf_size;
                    u16 least_packet_len;
                };
	    class CLidarPacket
	    {
	        public:
                /* Constructor */
                CLidarPacket();

                /* Destructor */
                ~CLidarPacket() { }

            public:
                std::vector<u8>    m_recvbuf;
				std::vector<u8>	   m_sendbuf;
                u16                m_read_length;
                u16                m_length;
                TParams            m_params;


				static void swap(u8* buf)
				{
					u8 temp=*buf;
					*buf = *(buf+1);
					*(buf+1)=temp;
				}
				
                /* buf to u16 */
                static u16 bufToUByte2(u8 *src_ptr);

                /* buf to s16 */
                static s16 bufToByte2(u8 *src_ptr);

                /* buf to u8 */
                static u8 bufToUByte(u8 *src_ptr);

                /* Return true if buffer is empty */
                bool isEmpty() { return m_length == 0? true: false; }

                /* Return buffer size */
                u16 getSize() const { return m_length; }
          
                /* Push ch in buffer end */
                void pushBack(u8 ch);

                /* Reset packet */
                void reset();

                /* return true if has write capacity */
				bool hasWriteCapacity(int bytes);

				static void printHex(u8* buf,u16 size);
				//static unsigned char HighBitToLowBitForword(unsigned char byte);
				//static unsigned char LowBitToHighBitForword(unsigned char byte); 

                /* Get command ID */
                u8 getCommandID(void) const
				{ 
					u8 temp =(m_recvbuf[4]&0x3F); 
					return  temp;
				}

                /* Get params data pointer */
                u8* getParamPtr(void)      
                {
                    u8* p =  &m_recvbuf[7];
                    return p;
                }

                /* Get params data length */
                u16 getParamLength(void)const 
                {					
					u8 value[2] = {0,0};
					value[0]=m_recvbuf[5];
					value[1]=m_recvbuf[6];
					u8 temp = value[0];
					value[0] = value[1];
					value[1] = temp;					
                    return bufToUByte2(&value[0]);
                 
                }

                /* Buffer to data */
                static void bufferToData(void *dest_ptr, void *src_ptr, size_t length);

		         u16 calc16BitAccCheckSum(u8 *ptr, u16 len);
				
		         bool verify16BitAccCheckSum(void);
				
                
            private:
                u8* getBufPtr() { return &m_recvbuf[0]; }

	    };
	}
}

#endif


