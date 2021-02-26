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

#pragma once

namespace rs { namespace standalone{ namespace rslidar {

class RSlidarDriverSerialImpl : public RSlidarDriver
{
public:

    RSlidarDriverSerialImpl();
    virtual ~RSlidarDriverSerialImpl();

public:
    virtual u_result connect(const char * port_path, _u32 baudrate, _u32 flag);
    virtual void disconnect();
    virtual bool isConnected();

    virtual u_result resetlidar(_u32 timeout = DEFAULT_TIMEOUT);

	virtual u_result getDeviceInfo(LIDAR_RESPONSE_DEV_INFO_T *info, _u32 timeout = DEFAULT_TIMEOUT);

	virtual u_result startScan(_u8 scanmode = RAPID_SCAN_MODE,_u32 timeout = DEFAULT_TIMEOUT);
    virtual u_result stopScan(_u32 timeout = DEFAULT_TIMEOUT);
	virtual u_result grabScanData(LIDAR_MEASURE_INFO_T * nodebuffer, size_t & count, _u32 timeout = DEFAULT_TIMEOUT);
    virtual u_result ascendScanData(LIDAR_MEASURE_INFO_T * nodebuffer, size_t count);
	
	virtual u_result setMotorRpm(_u16 speed, _u32 timeout = DEFAULT_TIMEOUT);

protected:
	u_result _waitNode(_u8 * node, _u32 timeout);
	u_result _waitScanData(LIDAR_MEASURE_INFO_T * nodebuffer, size_t & count, _u32 timeout = DEFAULT_TIMEOUT);
	u_result _cacheScanData();
	u_result _cacheSerialData();

    u_result _sendCommand(_u8 cmd, const void * payload = NULL, size_t payloadsize = 0);
	u_result _waitResponseHeader(COMM_FRAME_T * header, _u8 Cmmd,const void * payload, size_t payloadsize, _u32 timeout);
	uint16_t _checksum(uint8_t *startByte, uint16_t numBytes);
	uint16_t RSlidarDriverSerialImpl::_Cyclechecksum(uint8_t *startByte, _u32 pos, uint16_t numBytes, _u32 buffersize);

    void     _disableDataGrabbing();

	int     _serial_fd;

    bool     _isConnected;
	bool     _isScanning;
	bool     _isGetData;


	rs::hal::Locker         _lock;
    rs::hal::Event          _dataEvt;
    rs::hal::serial_rxtx  * _rxtx;

	_u8								_flag_cached_scan_node_ping_pong;
	LIDAR_MEASURE_INFO_T			_cached_scan_note_bufferPing[2048];
	LIDAR_MEASURE_INFO_T			_cached_scan_note_bufferPong[2048];
    size_t									_cached_scan_node_count;
	rs::hal::Thread _cachethread;

	_u8 _paramReceiveBuf[1024];

	//FILE *receive_data;
	//FILE *error_data;
	SYSTEMTIME tm_error_frame, tm_time_out;
};


}}}
