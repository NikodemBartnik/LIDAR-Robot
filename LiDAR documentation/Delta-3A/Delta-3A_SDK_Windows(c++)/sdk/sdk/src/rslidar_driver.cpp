/*
*  RSLIDAR System
*  Driver Interface
*
*  Copyright 2015 RS Team
*  All rights reserved.
*
*	Author: ruishi, Data:2015-12-25
*
*/
#include <time.h>
#include "sdkcommon.h"
#include "hal/abs_rxtx.h"
#include "hal/thread.h"
#include "hal/locker.h"
#include "hal/event.h"
#include "rslidar_driver_serial.h"
//#include <ioctl.h>

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

namespace rs { namespace standalone{ namespace rslidar {


// Factory Impl
RSlidarDriver * RSlidarDriver::CreateDriver()
{
    return new RSlidarDriverSerialImpl();
}


void RSlidarDriver::DisposeDriver(RSlidarDriver * drv)
{
    delete drv;
}

// Serial Driver Impl

RSlidarDriverSerialImpl::RSlidarDriverSerialImpl(): _isConnected(false), _isScanning(false)
{
    _rxtx = rs::hal::serial_rxtx::CreateRxTx();
	_flag_cached_scan_node_ping_pong = NOTE_BUFFER_PING;
	memset(_cached_scan_note_bufferPing, 0, sizeof(_cached_scan_note_bufferPing));
	memset(_cached_scan_note_bufferPong, 0, sizeof(_cached_scan_note_bufferPong));
    _cached_scan_node_count = 0;
	RunningStates = PORT_RDY;
}

RSlidarDriverSerialImpl::~RSlidarDriverSerialImpl()
{
    disconnect();
    rs::hal::serial_rxtx::ReleaseRxTx(_rxtx);
}

u_result RSlidarDriverSerialImpl::connect(const char * port_path, _u32 baudrate, _u32 flag)
{
    rs::hal::AutoLocker l(_lock);
    if (isConnected()) return RESULT_ALREADY_DONE;

    if (!_rxtx) return RESULT_INSUFFICIENT_MEMORY;

    // establish the serial connection...
    if (!_rxtx->bind(port_path, baudrate)  ||  !_rxtx->open()) {
        return RESULT_INVALID_DATA;
    }

    _rxtx->flush(0);
    _isConnected = true;

	_isGetData = true;
    return RESULT_OK;
}

void RSlidarDriverSerialImpl::disconnect()
{
    if (!_isConnected) return ;
		stopScan();
    _rxtx->close();
}

bool RSlidarDriverSerialImpl::isConnected()
{
    return _isConnected;
}

u_result RSlidarDriverSerialImpl::_cacheSerialData()
{
	return RESULT_OK;
}


u_result RSlidarDriverSerialImpl::resetlidar(_u32 timeout)
{
	_u8 param = 0x08;
    u_result ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);
		param = RESET_MODE;
		if (IS_FAIL(ans = _sendCommand(CMD_WORK_MODE, &param, 1))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, CMD_WORK_MODE, NULL,1, timeout))) {
			return ans;
		}

		if ((response_header->command&COMM_FRAME_ERROR) > 0)
		{
			return *(response_header->paramBuf);
		}
	}

	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::setMotorRpm(_u16 speed, _u32 timeout)
{
	_u8 Data[] = { 0x23,0x01,0x67,0x45,0xAB,0x89,0xEF,0xCD,0x00,0x00};
	u_result ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	Data[8] = speed;
	Data[9] = speed >> 8;

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	//_disableDataGrabbing();

	{
		//rs::hal::AutoLocker l(_lock);

		if (IS_FAIL(ans = _sendCommand(CMD_SET_MOTOR_SPEED, &Data, 10))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, CMD_SET_MOTOR_SPEED, NULL,1, timeout))) {
			return ans;
		}

		if ((response_header->command&COMM_FRAME_ERROR) > 0)
		{
			return (*(response_header->paramBuf));
		}
	}

	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::getDeviceInfo(LIDAR_RESPONSE_DEV_INFO_T * info, _u32 timeout)
{
	u_result  ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);
		if (IS_FAIL(ans = _sendCommand(CMD_READ_DEV_VER, NULL, 0))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, CMD_READ_DEV_VER, NULL,45, timeout))) {
			return ans;
		}

		if ((response_header->command&COMM_FRAME_ERROR) > 0)
		{
			return (*(response_header->paramBuf));
		}

		memcpy(info, response_header->paramBuf, response_header->paramLen);
	}
	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::startScan(_u8 scanmode,_u32 timeout)
{
	_u8 param = scanmode;
	u_result ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;
	if (_isScanning) return RESULT_ALREADY_DONE;

	{
		rs::hal::AutoLocker l(_lock);
		_rxtx->flush(0);
		if (IS_FAIL(ans = _sendCommand(CMD_WORK_MODE, &param, 1))) {
			return ans;
		}
		
		/*if (IS_FAIL(ans = _waitResponseHeader(response_header, CMD_WORK_MODE, NULL, 1, timeout))) {
			return ans;
		}
	
		if ((response_header->command&COMM_FRAME_ERROR) > 0)
		{
			return (*(response_header->paramBuf));
		}
		*/
		_isScanning = true;
		_cachethread = CLASS_THREAD(RSlidarDriverSerialImpl, _cacheScanData);
	}

	return RESULT_OK;
}


u_result RSlidarDriverSerialImpl::stopScan(_u32 timeout)
{
	_u8 param = 0x00;
	u_result ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);

		if (IS_FAIL(ans = _sendCommand(CMD_WORK_MODE,&param,1))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, CMD_WORK_MODE, NULL,1, timeout))) {
			return ans;
		}

		if ((response_header->command&COMM_FRAME_ERROR) > 0)
		{
			return (*(response_header->paramBuf));
		}
	}

	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::_cacheScanData()
{
	LIDAR_MEASURE_INFO_T				*local_buf = NULL;
	size_t                              count = NUMBER_OF_TEETH;
	size_t                              scan_count = 0;
	u_result                            ans;

	_flag_cached_scan_node_ping_pong = NOTE_BUFFER_PING;
	local_buf = _cached_scan_note_bufferPong;
	_waitScanData(local_buf, count); // // always discard the first data since it may be incomplete
	count = NUMBER_OF_TEETH;

	while (_isScanning)
	{
		if (IS_FAIL(ans = _waitScanData(local_buf, count))) {
			if (!((ans == RESULT_OPERATION_TIMEOUT) || (ans == RESULT_RECEIVE_NODE_ERROR))) {
				_isScanning = false;
				return RESULT_OPERATION_FAIL;
			}
		}

		_lock.lock();
		_flag_cached_scan_node_ping_pong = _flag_cached_scan_node_ping_pong ^ 0x01;
		if (NOTE_BUFFER_PING == _flag_cached_scan_node_ping_pong)
		{
			local_buf = _cached_scan_note_bufferPong;
		}
		else if (NOTE_BUFFER_PONG == _flag_cached_scan_node_ping_pong)
		{
			local_buf = _cached_scan_note_bufferPing;
		}
		else{}
		_cached_scan_node_count = count;
		_dataEvt.set();
		_lock.unlock();
		count = NUMBER_OF_TEETH;
	}
	_isScanning = false;
	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::_waitScanData(LIDAR_MEASURE_INFO_T * nodebuffer, size_t & count, _u32 timeout)
{
	_u8		recvBuffer[1024];
	_u16	frameLens = 0;
	_u16	paramLens = 0;
	_u16	angleRange = 0;
	float	angleStep = 0;
	float  speed;
	float  angleoffset;
	bool	flagOfNewNodes = false;
	_u16	crcCheckNum = 0,calcchecksum;
	_u16	sampleNumber = 0;
	_u16    sampleNumberCount = 0;

	if (!_isConnected) {
		count = 0;
		return RESULT_OPERATION_FAIL;
	}

	size_t   recvNodeCount = 0;
	_u32     startTs = getms();
	_u32     waitTime;
	u_result ans;


	while ((waitTime = getms() - startTs) <= timeout && recvNodeCount < count) 
	{
		if (IS_FAIL(ans = _waitNode(recvBuffer, timeout - waitTime))) 
		{
			return ans;
		}

		frameLens = (recvBuffer[2] << 8) | recvBuffer[1];
		crcCheckNum = *(_u16 *)(recvBuffer + frameLens);
		calcchecksum = _checksum(recvBuffer, frameLens);
		angleRange = (recvBuffer[10] << 8) | recvBuffer[9];

		if (crcCheckNum == calcchecksum)
		{
			paramLens = (recvBuffer[6] << 8) | recvBuffer[5];
			sampleNumber = (paramLens - 4) / 2;
			if ((angleRange >= 35000) || (angleRange <= 100))  //find original point
			{
				flagOfNewNodes = true;
				recvNodeCount = 0;
				sampleNumberCount = 0;
				angleStep = 36000.0 / sampleNumber / NUMBER_OF_TEETH;
			}

			if (flagOfNewNodes)
			{
				speed = ((recvBuffer[8] << 8) | recvBuffer[7])*0.01;
				angleoffset = 0;

				for (int i = 0; i < sampleNumber; i++)
				{
					(nodebuffer + sampleNumberCount)->motorspeed = speed;
					(nodebuffer + sampleNumberCount)->angle = angleRange + i * angleStep;
					(nodebuffer + sampleNumberCount)->angleoffset = angleoffset;
					(nodebuffer + sampleNumberCount)->distance = (recvBuffer[12 + i * 2] << 8) | recvBuffer[11 + i * 2];
					sampleNumberCount++;
				}
				recvNodeCount++;
			}			
		}
		else
		{
			RunningStates = CHKSUM_ERROR;
			continue;
		}

		if (recvNodeCount == count)
		{
			flagOfNewNodes = false;
			count = sampleNumberCount;
			return RESULT_OK;
		}
	}

	//CLogout("_waitScanData002:%d \r\n", sampleNumberCount);
	count = sampleNumberCount;
	return RESULT_OPERATION_TIMEOUT;
}

u_result RSlidarDriverSerialImpl::_waitNode(_u8 * nodeBuffer, _u32 timeout)
{
	int		recvPos = 0;
	_u32	startTs = getms();
	_u8		recvBuffer[512];
	_u32	waitTime;
	_u16	frameLens = 0;
	size_t  recvSize;
	size_t  remainSize;
	size_t  pos;
	bool    frameDataRcvFlag = false;
	
	
	char tmp_message[100];
	memset(tmp_message, 0, 100);

	while ((waitTime = getms() - startTs) <= timeout) {

		if (false == frameDataRcvFlag)
		{
			//receive head info
			remainSize = sizeof(COMM_FRAME_T) - recvPos;
		}
		else
		{
			//receive data
			remainSize = frameLens + 2 - recvPos;
		}
		recvSize = 0;
		//printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		int ans = _rxtx->waitfordata(remainSize, timeout, &recvSize);

		if (ans == rs::hal::serial_rxtx::ANS_DEV_ERR)
		{
			return RESULT_OPERATION_FAIL;
		}

		if ((ans == rs::hal::serial_rxtx::ANS_TIMEOUT) && (0 == recvSize))
		{
			GetLocalTime(&tm_time_out);
			int tm_cha = ((tm_time_out.wMinute - tm_error_frame.wMinute) * 60 + (tm_time_out.wSecond - tm_error_frame.wSecond)) * 1000 + (tm_time_out.wMilliseconds - tm_error_frame.wMilliseconds);
			if (tm_cha > 500)
			{
				_cprintf("[Eror]: Receive data timeout!\r\n");
				RunningStates = RX_DATA_TIMOUT;
				continue;
			}
		}
		if (recvSize > remainSize) recvSize = remainSize;
		_rxtx->recvdata(recvBuffer, recvSize);
		for (pos = 0; pos < recvSize; ++pos)
		{
			_u8 currentByte = recvBuffer[pos];

			if (0 == recvPos)
			{
				if (COMM_HEAD_FLAGE != currentByte)
				{
					frameDataRcvFlag = false;
					continue;
				}
			}
			else if (4 == recvPos)
			{
				frameLens = (nodeBuffer[2] << 8) | nodeBuffer[1];
				//if ((frameLens > 300) || (recvPos < (sizeof(COMM_FRAME_T) - 1)))
				if (frameLens > 300)
				{
					recvPos = 0;
					frameLens = 0;
					frameDataRcvFlag = false;
					continue;
				}
				else
					frameDataRcvFlag = true;

				if (CMD_REPORT_DEV_ERROR == (currentByte&COMM_MASK))
				{
					RunningStates = MOTOR_STALL;
					continue;					
				}

				if (CMD_REPORT_DIST != (currentByte&COMM_MASK))
					continue;
			}

			nodeBuffer[recvPos++] = currentByte;

			if ((recvPos >= (frameLens + 2))&&(recvPos > sizeof(COMM_FRAME_T)))
			{
				RunningStates = WORK_WELL;
				return RESULT_OK;
			}
		}
	}
	return RESULT_OPERATION_TIMEOUT;
}

u_result RSlidarDriverSerialImpl::grabScanData(LIDAR_MEASURE_INFO_T * nodebuffer, size_t & count, _u32 timeout)
{
    switch (_dataEvt.wait(timeout))
    {
		case rs::hal::Event::EVENT_TIMEOUT:
			count = 0;
			return RESULT_OPERATION_TIMEOUT;
			break;

		case rs::hal::Event::EVENT_OK:
			{
				rs::hal::AutoLocker l(_lock);

				size_t size_to_copy = min(count, _cached_scan_node_count);

				if (NOTE_BUFFER_PING == _flag_cached_scan_node_ping_pong)
				{
					memcpy(nodebuffer, _cached_scan_note_bufferPing, size_to_copy*sizeof(LIDAR_MEASURE_INFO_T));
				}
				else if (NOTE_BUFFER_PONG == _flag_cached_scan_node_ping_pong)
				{
					memcpy(nodebuffer, _cached_scan_note_bufferPong, size_to_copy*sizeof(LIDAR_MEASURE_INFO_T));
				}

				count = size_to_copy;
				_cached_scan_node_count = 0;
			}
			return RESULT_OK;
			break;

		default:
			count = 0;
			return RESULT_OPERATION_FAIL;
			break;
    }

}

u_result RSlidarDriverSerialImpl::ascendScanData(LIDAR_MEASURE_INFO_T * nodebuffer, size_t count)
{
    float inc_origin_angle = 360.0/count;
    int i = 0;

    //Tune head
    for (i = 0; i < count; i++) {
        if(nodebuffer[i].distance == 0) {
            continue;
        } else {
            while(i != 0) {
                i--;
                float expect_angle = (nodebuffer[i+1].angle)/100.0f - inc_origin_angle;
                if (expect_angle < 0.0f) expect_angle = 0.0f;
				nodebuffer[i].angle = (_u16)(expect_angle * 100.0f);
            }
            break;
        }
    }

    // all the data is invalid
    if (i == count) return RESULT_OPERATION_FAIL;

    //Tune tail
    for (i = count - 1; i >= 0; i--) {
		if (nodebuffer[i].distance == 0) {
            continue;
        } else {
            while(i != (count - 1)) {
                i++;
				float expect_angle = (nodebuffer[i - 1].angle) / 100.0f + inc_origin_angle;
                if (expect_angle > 360.0f) expect_angle -= 360.0f;
				nodebuffer[i].angle = (_u16)(expect_angle * 100.0f);
            }
            break;
        }
    }

    //Fill invalid angle in the scan
	float frontAngle = (nodebuffer[0].angle) / 100.0f;
    for (i = 1; i < count; i++) {
		if (nodebuffer[i].distance == 0) {
            float expect_angle =  frontAngle + i * inc_origin_angle;
            if (expect_angle > 360.0f) expect_angle -= 360.0f;
			nodebuffer[i].angle = (_u16)(expect_angle * 100.0f);
        }
    }

    // Reorder the scan according to the angle value
    for (i = 0; i < (count-1); i++){
        for (int j = (i+1); j < count; j++){
			if (nodebuffer[i].angle > nodebuffer[j].angle){
				LIDAR_MEASURE_INFO_T temp = nodebuffer[i];
                nodebuffer[i] = nodebuffer[j];
                nodebuffer[j] = temp;
            }
        }
    }

    return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::_sendCommand(_u8 cmd, const void * payload, size_t payloadsize)
{
	_u16 checksum = 0;
	_u8 commBuf[1024] = {0};
	COMM_FRAME_T *commFrameTmp = reinterpret_cast<COMM_FRAME_T *>(commBuf);
	const _u8 *pPayload = reinterpret_cast<const _u8 *>(payload);

	if (!_isConnected) return RESULT_OPERATION_FAIL;

	commFrameTmp->framehead = COMM_HEAD_FLAGE;
	commFrameTmp->frameLen = sizeof(COMM_FRAME_T)+payloadsize;
	commFrameTmp->protocolver = 4;
	commFrameTmp->command = cmd;
	if (payloadsize && payload) {
		commFrameTmp->paramLen = payloadsize;
	}
	else
	{
		commFrameTmp->paramLen = 0;
	}
	if (payloadsize > 0)
	{
		for (int i = payloadsize-1; i >= 0; i--)
			*(commFrameTmp->paramBuf+i) = *(pPayload+i);
	}

	checksum = _checksum(commBuf, sizeof(COMM_FRAME_T) + payloadsize);
	
	commBuf[sizeof(COMM_FRAME_T)+payloadsize] = checksum;
	commBuf[sizeof(COMM_FRAME_T)+payloadsize + 1] = checksum >> 8;

	// send header first
	for (int i = 0; i < (sizeof(COMM_FRAME_T)+payloadsize + sizeof(_u16));i++)
		printf("%02x ", commBuf[i]);
	printf("\n");
	_rxtx->senddata(commBuf, sizeof(COMM_FRAME_T)+payloadsize + sizeof(_u16));
	return RESULT_OK;
}


u_result RSlidarDriverSerialImpl::_waitResponseHeader(COMM_FRAME_T * header, _u8 Cmmd,const void * payload, size_t payloadsize, _u32 timeout)
{
	_u8 Data[512];
	int  recvPos = 0,readPos = 0,i;
	_u32 startTs = getms();
	_u8  *headerBuffer = reinterpret_cast<_u8 *>(header);
	_u32 waitTime;
	_u16 framelen,checksumCac, checksumRcv;
	size_t remainSize = sizeof(COMM_FRAME_T) + payloadsize + sizeof(_u16);
	size_t recvSize;

	while ((waitTime = getms() - startTs) <= timeout) 
	{
		int ans = _rxtx->waitfordata(remainSize, timeout - waitTime, &recvSize);
		if (ans == rs::hal::serial_rxtx::ANS_DEV_ERR)
		{
			return RESULT_OPERATION_FAIL;
		}
		else if (ans == rs::hal::serial_rxtx::ANS_TIMEOUT)
		{
			return RESULT_OPERATION_TIMEOUT;
		}

		if (recvSize > (sizeof(Data) - (recvPos & 0x1FF)))
		{
			_rxtx->recvdata(&Data[recvPos & 0x1FF], sizeof(Data) - (recvPos & 0x1FF));
			_rxtx->recvdata(Data,recvSize - sizeof(Data) + (recvPos & 0x1FF));
		}
		else
			_rxtx->recvdata(&Data[recvPos & 0x1FF], recvSize);

		recvPos += recvSize;
		if ((readPos + remainSize) > recvPos)
			continue;
		else
		{
			for (; readPos < recvPos; readPos++)
			{
				if (Data[readPos & 0x1FF] != COMM_HEAD_FLAGE)
					continue;
				else
				{
					framelen = (Data[(readPos + 2) & 0x1FF] << 8) | Data[(readPos + 1) & 0x1FF];
					if (framelen > 100)
						continue;

					if ((framelen + 2) > (recvPos - readPos))
						break;

					if ((Data[(readPos + 4) & 0x1FF]&COMM_MASK) != Cmmd)
						continue;

					checksumCac = _Cyclechecksum(Data,readPos,framelen,sizeof(Data));
					checksumRcv = (Data[(readPos + framelen + 1) & 0x1FF] << 8) | Data[(readPos + framelen) & 0x1FF];
					if (checksumCac == checksumRcv)
					{
						for (i = 0; i < (payloadsize + sizeof(COMM_FRAME_T)); i++)
						{
							*headerBuffer++ = Data[(readPos + i) & 0x1FF];
						}
						return RESULT_OK;
					}
					else
						return RESULT_INVALID_DATA;
				}
			}
		}
	}
	return RESULT_OPERATION_TIMEOUT;
}

uint16_t RSlidarDriverSerialImpl::_Cyclechecksum(uint8_t *startByte, _u32 pos,uint16_t numBytes,_u32 buffersize)
{
	uint16_t checksum = 0;                           // CRC查找表的指针
	while (numBytes--)
	{
		checksum += *(startByte + (pos%buffersize));      // 计算CRC
		pos++;
	}
	return checksum;
}

uint16_t RSlidarDriverSerialImpl::_checksum(uint8_t *startByte, uint16_t numBytes)
{
	uint16_t checksum = 0;                           // CRC查找表的指针
	while (numBytes--)
	{
		checksum += *startByte++;      // 计算CRC
	}
	return checksum;
}

void RSlidarDriverSerialImpl::_disableDataGrabbing()
{
    _isScanning = false;
    _cachethread.join();
	_rxtx->flush(0);
}

}}}

