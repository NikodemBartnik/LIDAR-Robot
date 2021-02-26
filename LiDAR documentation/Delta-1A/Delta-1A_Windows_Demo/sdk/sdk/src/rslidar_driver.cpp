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
RSlidarDriver * RSlidarDriver::CreateDriver(_u32 drivertype)
{
    switch (drivertype) {
    case DRIVER_TYPE_SERIALPORT:
        return new RSlidarDriverSerialImpl();
    default:
        return NULL;
    }
}


void RSlidarDriver::DisposeDriver(RSlidarDriver * drv)
{
    delete drv;
}

//CRC High Byte Vaule Table
const uint8_t RSlidarDriverSerialImpl::auchCRCHi[256] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};
//CRC Low  Byte Vaule Table
const uint8_t RSlidarDriverSerialImpl::auchCRCLo[256] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

// Serial Driver Impl

RSlidarDriverSerialImpl::RSlidarDriverSerialImpl() 
    : _isConnected(false)
    , _isScanning(false)
{
    _rxtx = rs::hal::serial_rxtx::CreateRxTx();
	_flag_cached_scan_node_ping_pong = NOTE_BUFFER_PING;
	memset(_cached_scan_note_bufferPing, 0, sizeof(_cached_scan_note_bufferPing));
	memset(_cached_scan_note_bufferPong, 0, sizeof(_cached_scan_note_bufferPong));
    _cached_scan_node_count = 0;
	RunningStates = RxDataError;
	StableCnt = 0;
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
    stop();
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


u_result RSlidarDriverSerialImpl::reset(_u32 timeout)
{
    u_result ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);

		if (IS_FAIL(ans = _sendCommand(CMD_SYSTEM_RST, NULL, CMMD_RESET_REQ_LENS))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, NULL, CMMD_DEV_RSP_LENS, timeout))) {
			return ans;
		}

		if (CMMD_FRAME_RSP_ERROR == response_header->frameType)
		{
			switch (*(response_header->paramBuf))
			{
				case 0x01:
					return CMMD_FRAME_RSP_CMD_ERROR;
					break;

				case 0x02:
					return CMMD_FRAME_RSP_PARAM_LEN_ERROR;
					break;

				case 0x03:
					return CMMD_FRAME_RSP_PARAM_ERROR;
					break;

				case 0x04:
					return CMMD_FRAME_RSP_CRC_ERROR;
					break;

				default:
					break;
			}
		}
	}

	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::setMotorControl(bool force, _u32 timeout)
{
	u_result ans;
	_u8 motorControl[1];
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);

		motorControl[0] = force ? CMMD_FRAME_PARAM_START_MOTOR : CMMD_FRAME_PARAM_STOP_MOTOR;

		if (IS_FAIL(ans = _sendCommand(CMD_MOTOR_WORK_CTRL, &motorControl, CMMD_MOTOR_CONTROL_REQ_lENS))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, NULL, CMMD_DEV_RSP_LENS, timeout))) {
			return ans;
		}

		if (CMMD_FRAME_RSP_ERROR == response_header->frameType)
		{
			switch ( *(response_header->paramBuf) )
			{
				case 0x01:
					return CMMD_FRAME_RSP_CMD_ERROR;
					break;

				case 0x02:
					return CMMD_FRAME_RSP_PARAM_LEN_ERROR;
					break;

				case 0x03:
					return CMMD_FRAME_RSP_PARAM_ERROR;
					break;

				case 0x04:
					return CMMD_FRAME_RSP_CRC_ERROR;
					break;

				default:
					break;
			}
		}
	}

	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::setMotorRpm(RSLIDAR_RESPONSE_MOTOR_INFO_T & motorInfo, _u32 timeout)
{
	u_result ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);

		if (IS_FAIL(ans = _sendCommand(CMD_MOTOR_RPM_SET, &motorInfo, CMMD_SET_MOTOR_RPM_REQ_LENS))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, NULL, CMMD_DEV_RSP_LENS, timeout))) {
			return ans;
		}

		if (CMMD_FRAME_RSP_ERROR == response_header->frameType)
		{
			switch (*(response_header->paramBuf))
			{
				case 0x01:
					return CMMD_FRAME_RSP_CMD_ERROR;
					break;

				case 0x02:
					return CMMD_FRAME_RSP_PARAM_LEN_ERROR;
					break;

				case 0x03:
					return CMMD_FRAME_RSP_PARAM_ERROR;
					break;

				case 0x04:
					return CMMD_FRAME_RSP_CRC_ERROR;
					break;

				default:
					break;
			}
		}
	}

	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::setMeatureUnit(RSLIDAR_RESPONSE_MEATURE_INIT_T & meatureUint, _u32 timeout)
{
	u_result ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);

		if (IS_FAIL(ans = _sendCommand(CMD_MEAS_UNIT, &meatureUint, CMMD_SET_MEASURE_UNIT_REQ_LENS))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, NULL, CMMD_DEV_RSP_LENS, timeout))) {
			return ans;
		}

		if (CMMD_FRAME_RSP_ERROR == response_header->frameType)
		{
			switch (*(response_header->paramBuf))
			{
				case 0x01:
					return CMMD_FRAME_RSP_CMD_ERROR;
					break;

				case 0x02:
					return CMMD_FRAME_RSP_PARAM_LEN_ERROR;
					break;

				case 0x03:
					return CMMD_FRAME_RSP_PARAM_ERROR;
					break;

				case 0x04:
					return CMMD_FRAME_RSP_CRC_ERROR;
					break;

				default:
					break;
			}
		}
	}

	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::getHealth(RSLIDAR_RESPONSE_HEALTH_INFO_T & healthinfo, _u32 timeout)
{
    u_result  ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);
    
    if (!isConnected()) return RESULT_OPERATION_FAIL;
    
    _disableDataGrabbing();

    {
        rs::hal::AutoLocker l(_lock);
		if (IS_FAIL(ans = _sendCommand(ATTR_READ_DEVICE_HEALTH, NULL, ATTR_DEV_INFO_PARAM_REQ_LENS))) {
            return ans;
        }

		if (IS_FAIL(ans = _waitResponseHeader(response_header, NULL, ATTR_DEV_INFO_HEALTH_RSP_LENS, timeout))) {
            return ans;
        }

		if (ATTR_FRAME_RSP_ERROR == response_header->frameType)
		{
			if (0x01 == *(response_header->paramBuf))
			{
				return ATTR_FRAME_RSP_CMD_ERROR;
			}		
			else
			{
				return ATTR_FRAME_RSP_CRC_ERROR;
			}
		}
		response_header->paramLen = (response_header->paramLen >> 8) | (response_header->paramLen << 8);
		healthinfo.deviceHealthInfo = *(response_header->paramBuf);
    }
    return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::getDeviceInfo(RSLIDAR_RESPONSE_DEVICE_INFO_T * info, _u32 timeout)
{
	u_result  ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);
		if (IS_FAIL(ans = _sendCommand(ATTR_READ_DEVICE_INFO, NULL, ATTR_DEV_INFO_PARAM_REQ_LENS))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, NULL, ATTR_DEV_INFO_PARAM_RSP_LENS, timeout))) {
			return ans;
		}

		if (ATTR_FRAME_RSP_ERROR == response_header->frameType)
		{
			if (0x01 == *(response_header->paramBuf))
			{
				return ATTR_FRAME_RSP_CMD_ERROR;
			}
			else
			{
				return ATTR_FRAME_RSP_CRC_ERROR;
			}
		}
		response_header->paramLen = (response_header->paramLen >> 8) | (response_header->paramLen << 8);
		memcpy(info, response_header->paramBuf, response_header->paramLen);
	}
	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::getMotorRpm(RSLIDAR_RESPONSE_MOTOR_INFO_T & motorInfo, _u32 timeout)
{
	u_result  ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);
		if (IS_FAIL(ans = _sendCommand(ATTR_READ_MOTOR_RPM, NULL, ATTR_DEV_INFO_PARAM_REQ_LENS))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, NULL, ATTR_DEV_INFO_MOTOR_RSP_LENS, timeout))) {
			return ans;
		}

		if (ATTR_FRAME_RSP_ERROR == response_header->frameType)
		{
			if (0x01 == *(response_header->paramBuf))
			{
				return ATTR_FRAME_RSP_CMD_ERROR;
			}
			else
			{
				return ATTR_FRAME_RSP_CRC_ERROR;
			}
		}
		response_header->paramLen = (response_header->paramLen >> 8) | (response_header->paramLen << 8);
		motorInfo.motorSpeed = *(_u16 *)(response_header->paramBuf);
		motorInfo.motorSpeed = (motorInfo.motorSpeed >> 8) | (motorInfo.motorSpeed << 8);
	}
	return RESULT_OK;
}


u_result RSlidarDriverSerialImpl::startScan(_u32 timeout)
{
	u_result ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;
	if (_isScanning) return RESULT_ALREADY_DONE;


	{
		rs::hal::AutoLocker l(_lock);

		_isScanning = true;
		_cachethread = CLASS_THREAD(RSlidarDriverSerialImpl, _cacheScanData);
	}


	return RESULT_OK;
}


u_result RSlidarDriverSerialImpl::stop(_u32 timeout)
{
	u_result ans;
	COMM_FRAME_T *response_header = reinterpret_cast<COMM_FRAME_T *>(_paramReceiveBuf);

	if (!isConnected()) return RESULT_OPERATION_FAIL;

	_disableDataGrabbing();

	{
		rs::hal::AutoLocker l(_lock);

		if (IS_FAIL(ans = _sendCommand(CMD_STOP, NULL, CMMD_SCAN_REQ_RSP_LENS))) {
			return ans;
		}

		if (IS_FAIL(ans = _waitResponseHeader(response_header, NULL, CMMD_DEV_RSP_LENS, timeout))) {
			return ans;
		}

		if (CMMD_FRAME_RSP_ERROR == response_header->frameType)
		{
			switch (*(response_header->paramBuf))
			{
			case 0x01:
				return CMMD_FRAME_RSP_CMD_ERROR;
				break;

			case 0x02:
				return CMMD_FRAME_RSP_PARAM_LEN_ERROR;
				break;

			case 0x03:
				return CMMD_FRAME_RSP_PARAM_ERROR;
				break;

			case 0x04:
				return CMMD_FRAME_RSP_CRC_ERROR;
				break;

			default:
				break;
			}
		}
	}

	return RESULT_OK;
}

u_result RSlidarDriverSerialImpl::_cacheScanData()
{
	RSLIDAR_SIGNAL_DISTANCE_UNIT_T		*local_buf = NULL;
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

u_result RSlidarDriverSerialImpl::_waitScanData(RSLIDAR_SIGNAL_DISTANCE_UNIT_T * nodebuffer, size_t & count, _u32 timeout)
{
	_u8		recvBuffer[1024];
	_u16	frameLens = 0;
	_u16	paramLens = 0;
	_u16	angleRange = 0;
	_u16	angleStep = 0;
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
	//while (waitTime <= timeout && recvNodeCount < count)
	{
		if (IS_FAIL(ans = _waitNode(recvBuffer, timeout - waitTime))) 
		{
			return ans;
		}

		frameLens = (recvBuffer[1] << 8) | recvBuffer[2];
		crcCheckNum = *(_u16 *)(recvBuffer + frameLens);

		angleRange = (recvBuffer[11] << 8) | recvBuffer[12];

		if (recvBuffer[3] > 0)
			calcchecksum = _checksum(recvBuffer, frameLens);
		else
			calcchecksum = _crc16(recvBuffer, frameLens);

		if (crcCheckNum == calcchecksum)
		{
			paramLens = (recvBuffer[6] << 8) | recvBuffer[7];
			sampleNumber = (paramLens - 5) / 3;
			if ((angleRange >= 35000) || (angleRange <= 100))  //find original point
			{
				flagOfNewNodes = true;
				recvNodeCount = 0;
				sampleNumberCount = 0;
				angleStep = 36000 / sampleNumber / NUMBER_OF_TEETH;
			}

			if (flagOfNewNodes)
			{
				speed = recvBuffer[8];
				speed *= 0.05;
				(nodebuffer + sampleNumberCount)->speed = speed;
				angleoffset = (recvBuffer[9] << 8) | recvBuffer[10];
				(nodebuffer + sampleNumberCount)->angleoffset = angleoffset;

				for (int i = 0; i < sampleNumber; i++)
				{
					(nodebuffer + sampleNumberCount)->speed = speed;
					(nodebuffer + sampleNumberCount)->angle = angleRange + i * angleStep;
					(nodebuffer + sampleNumberCount)->signalValue = recvBuffer[13 + i * 3];
					(nodebuffer + sampleNumberCount)->angleoffset = angleoffset;
					(nodebuffer + sampleNumberCount)->distanceValue = (recvBuffer[14 + i * 3] << 8) | recvBuffer[15 + i * 3];

#ifdef USE_CONSOLE
					//for (size_t i = 0; i < 10; i++)
					//{
					//	_cprintf("%02x ", recvBuffer[i]);
					//}
					//_cprintf("signalValue = %d\n", (nodebuffer + sampleNumberCount)->signalValue);
					
#endif


					sampleNumberCount++;
				}
				recvNodeCount++;
			}			
		}
		else
		{
			if (RunningStates != Crc_Error)
			{
				_cprintf("[Eror]: CRC check failed!\r\n");
			}
			StableCnt = 0;
			RunningStates = Crc_Error;
			//CLogout("_waitScanData001:%d \r\n", sampleNumberCount);
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
			//CLogout("_waitNode000: \r\n");
			return RESULT_OPERATION_FAIL;
		}

		if ((ans == rs::hal::serial_rxtx::ANS_TIMEOUT) && (0 == recvSize))
		{
			GetLocalTime(&tm_time_out);
			int tm_cha = ((tm_time_out.wMinute - tm_error_frame.wMinute) * 60 + (tm_time_out.wSecond - tm_error_frame.wSecond)) * 1000 + (tm_time_out.wMilliseconds - tm_error_frame.wMilliseconds);
			if (tm_cha > 500)
			{
				if (RunningStates != RxDataError)
				{
					_cprintf("[Eror]: Receive data timeout!\r\n");
				}
				StableCnt = 0;
				RunningStates = RxDataError;
				//sprintf(tmp_message, "%d.%d.%d  %d:%d:%02d.%03d  No Lidar Data!\n", tm_time_out.wYear, tm_time_out.wMonth, tm_time_out.wDay, tm_time_out.wHour, tm_time_out.wMinute, tm_time_out.wSecond, tm_time_out.wMilliseconds);
				//_cprintf(tmp_message);
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
				if (COMM_HEAD_FLAGE == currentByte)
				{

				}
				else
				{
					continue;
				}
			}
			else if (4 == recvPos)
			{
				if (COMM_FRAME_TYPE_MESSAGE == currentByte)
				{
					//find frame type
					frameLens = (nodeBuffer[1] << 8) | nodeBuffer[2];
				}
				else
				{
					recvPos = 0;
					continue;
				}
			}
			else if (5 == recvPos)
			{
				
				if (COMM_FRAME_TYPE_ERROR == currentByte)
				{
					if (RunningStates != Motor_Error)
					{
						_cprintf("[Eror]: Motor speed instability!\r\n");
					}
					StableCnt = 0;
					RunningStates = Motor_Error;
					continue;					
				}
			}
			else
			{
			}

			if (frameLens > 300)  //max size of per frame
			{
				recvPos = 0;
				frameLens = 0;
				continue;
			}
			nodeBuffer[recvPos++] = currentByte;
			if (recvPos == sizeof(COMM_FRAME_T))
			{
				frameDataRcvFlag = true;
			}

			if ((frameLens > 0) && (recvPos == (frameLens + 2)) && (frameDataRcvFlag == true))
			{
				if (StableCnt < 0xFFF)
					StableCnt++;
				if (StableCnt > 32)
				{
					if (RunningStates != ExecSuccess)
					{
						_cprintf("[Eror]: Running well!\r\n");
					}
					RunningStates = ExecSuccess;
				}
				return RESULT_OK;
			}
		}



	}
	//CLogout("_waitNode004: \r\n");
	return RESULT_OPERATION_TIMEOUT;
}

u_result RSlidarDriverSerialImpl::grabScanData(RSLIDAR_SIGNAL_DISTANCE_UNIT_T * nodebuffer, size_t & count, _u32 timeout)
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
					memcpy(nodebuffer, _cached_scan_note_bufferPing, size_to_copy*sizeof(RSLIDAR_SIGNAL_DISTANCE_UNIT_T));
				}
				else if (NOTE_BUFFER_PONG == _flag_cached_scan_node_ping_pong)
				{
					memcpy(nodebuffer, _cached_scan_note_bufferPong, size_to_copy*sizeof(RSLIDAR_SIGNAL_DISTANCE_UNIT_T));
				}
				else{}

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

u_result RSlidarDriverSerialImpl::ascendScanData(RSLIDAR_SIGNAL_DISTANCE_UNIT_T * nodebuffer, size_t count)
{
    float inc_origin_angle = 360.0/count;
    int i = 0;

    //Tune head
    for (i = 0; i < count; i++) {
        if(nodebuffer[i].distanceValue == 0) {
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
		if (nodebuffer[i].distanceValue == 0) {
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
		if (nodebuffer[i].distanceValue == 0) {
            float expect_angle =  frontAngle + i * inc_origin_angle;
            if (expect_angle > 360.0f) expect_angle -= 360.0f;
			nodebuffer[i].angle = (_u16)(expect_angle * 100.0f);
        }
    }

    // Reorder the scan according to the angle value
    for (i = 0; i < (count-1); i++){
        for (int j = (i+1); j < count; j++){
			if (nodebuffer[i].angle > nodebuffer[j].angle){
				RSLIDAR_SIGNAL_DISTANCE_UNIT_T temp = nodebuffer[i];
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

	commFrameTmp->frameStart = COMM_HEAD_FLAGE;
	commFrameTmp->frameLen = sizeof(COMM_FRAME_T)+payloadsize;
	commFrameTmp->frameLen = (commFrameTmp->frameLen >> 8) | (commFrameTmp->frameLen << 8);
	commFrameTmp->addr = 0;
	if (cmd >= CMD_STOP && cmd <= CMD_SYSTEM_RST)
	{
		commFrameTmp->frameType = COMM_FRAME_TYPE_CMD;
	}
	else if (cmd >= ATTR_READ_DEVICE_INFO && cmd <= ATTR_READ_DEVICE_HEALTH)
	{
		commFrameTmp->frameType = COMM_FRAME_TYPE_ATTR;
	}
	commFrameTmp->cmd = cmd;
	if (payloadsize && payload) {
		commFrameTmp->paramLen = payloadsize;
	}
	else
	{
		commFrameTmp->paramLen = 0;
	}
	commFrameTmp->paramLen = (commFrameTmp->paramLen >> 8) | (commFrameTmp->paramLen << 8);

	if (payloadsize > 0)
	{
		for (int i = payloadsize-1; i >= 0; i--)
			*(commFrameTmp->paramBuf+i) = *(pPayload+i);
	}

	if (commFrameTmp->addr  > 0)
		checksum = _checksum(commBuf, sizeof(COMM_FRAME_T) + payloadsize);
	else
		checksum = _crc16(commBuf, sizeof(COMM_FRAME_T) + payloadsize);

	
	commBuf[sizeof(COMM_FRAME_T)+payloadsize] = checksum & 0xff;
	commBuf[sizeof(COMM_FRAME_T)+payloadsize + 1] = (checksum >> 8) & 0xff;

	// send header first
	for (int i = 0; i < (sizeof(COMM_FRAME_T)+payloadsize + sizeof(_u16));i++)
		printf("%02x ", commBuf[i]);
	printf("\n");
	_rxtx->senddata(commBuf, sizeof(COMM_FRAME_T)+payloadsize + sizeof(_u16));

	return RESULT_OK;
}


u_result RSlidarDriverSerialImpl::_waitResponseHeader(COMM_FRAME_T * header, const void * payload, size_t payloadsize, _u32 timeout)
{
	int  recvPos = 0;
	_u32 startTs = getms();
	//_u8  recvBuffer[1024];
	_u8  *headerBuffer = reinterpret_cast<_u8 *>(header);
	//const _u8 *pParamBuff = reinterpret_cast<const _u8 *>(payload);
	_u32 waitTime;
	_u16 checksumCac, checksumRcv;
	//_cprintf("_waitResponseHeader %d\n", __LINE__);
	while ((waitTime = getms() - startTs) <= timeout) 
	{
		size_t remainSize = sizeof(COMM_FRAME_T) + payloadsize + sizeof(_u16) - recvPos;
		size_t recvSize;

		int ans = _rxtx->waitfordata(remainSize, timeout - waitTime, &recvSize);
		if (ans == rs::hal::serial_rxtx::ANS_DEV_ERR)
		{
			return RESULT_OPERATION_FAIL;
		}
		else if (ans == rs::hal::serial_rxtx::ANS_TIMEOUT)
		{
			return RESULT_OPERATION_TIMEOUT;
		}

		if (recvSize > remainSize)
		{
			recvSize = remainSize;
		}

		_rxtx->recvdata(headerBuffer, recvSize);
		for (int i = 0; i<recvSize; i++)
			printf("%02x ", headerBuffer[i]);

		recvPos += recvSize;

		if (recvPos == (sizeof(COMM_FRAME_T)+payloadsize + sizeof(_u16)))
		{
			if (*(headerBuffer + 3) > 0)
				checksumCac = _checksum(headerBuffer, sizeof(COMM_FRAME_T) + payloadsize);
			else
				checksumCac = _crc16(headerBuffer, sizeof(COMM_FRAME_T)+payloadsize);

			checksumRcv = *(_u16 *)(headerBuffer+sizeof(COMM_FRAME_T)+payloadsize);
			if (checksumCac == checksumRcv)
			{
				return RESULT_OK;
			}
			else
			{
				return RESULT_INVALID_DATA;
			}
		}
	}
	return RESULT_OPERATION_TIMEOUT;
}


//====================================================================
//    函数返回值是无符号短整型CRC值
//    待进行CRC校验计算的报文
//    待校验的报文长度
//====================================================================
uint16_t RSlidarDriverSerialImpl::_crc16(uint8_t *startByte, uint16_t numBytes)
{
	uint8_t  uchCRCHi = 0xFF;             // CRC高字节的初始化
	uint8_t  uchCRCLo = 0xFF;             // CRC低字节的初始化
	uint16_t uIndex;                           // CRC查找表的指针
	while (numBytes--)
	{
		uIndex = uchCRCLo ^ *startByte++;      // 计算CRC
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
		uchCRCHi = auchCRCLo[uIndex];
	}
	return(uchCRCLo << 8 | uchCRCHi);
}

uint16_t RSlidarDriverSerialImpl::_checksum(uint8_t *startByte, uint16_t numBytes)
{
	uint16_t checksum = 0;                           // CRC查找表的指针
	uint16_t Temp;
	while (numBytes--)
	{
		checksum += *startByte++;      // 计算CRC
	}
	Temp = checksum & 0xFF;
	checksum >>= 8;
	checksum |= (Temp << 8);
	return checksum;
}

void RSlidarDriverSerialImpl::_disableDataGrabbing()
{
    _isScanning = false;
    _cachethread.join();
	_rxtx->flush(0);
}

}}}

