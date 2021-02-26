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

    enum {
        MAX_SCAN_NODES = 2048,
    };

	//CRC High Byte Vaule Table
	static const uint8_t auchCRCHi[256];
	//CRC Low  Byte Vaule Table
	static const uint8_t auchCRCLo[256];

    RSlidarDriverSerialImpl();
    virtual ~RSlidarDriverSerialImpl();

public:
    virtual u_result connect(const char * port_path, _u32 baudrate, _u32 flag);
    virtual void disconnect();
    virtual bool isConnected();

    virtual u_result reset(_u32 timeout = DEFAULT_TIMEOUT);

	virtual u_result getHealth(RSLIDAR_RESPONSE_HEALTH_INFO_T & healthInfo, _u32 timeout = DEFAULT_TIMEOUT);
	virtual u_result getDeviceInfo(RSLIDAR_RESPONSE_DEVICE_INFO_T *info, _u32 timeout = DEFAULT_TIMEOUT);
	virtual u_result getMotorRpm(RSLIDAR_RESPONSE_MOTOR_INFO_T & motorInfo, _u32 timeout = DEFAULT_TIMEOUT);

	virtual u_result startScan(_u32 timeout = DEFAULT_TIMEOUT);
    virtual u_result stop(_u32 timeout = DEFAULT_TIMEOUT);
	virtual u_result grabScanData(RSLIDAR_SIGNAL_DISTANCE_UNIT_T * nodebuffer, size_t & count, _u32 timeout = DEFAULT_TIMEOUT);
    virtual u_result ascendScanData(RSLIDAR_SIGNAL_DISTANCE_UNIT_T * nodebuffer, size_t count);
	
	virtual u_result setMotorControl(bool force, _u32 timeout = DEFAULT_TIMEOUT);
	virtual u_result setMotorRpm(RSLIDAR_RESPONSE_MOTOR_INFO_T & motorInfo, _u32 timeout = DEFAULT_TIMEOUT);
	virtual u_result setMeatureUnit(RSLIDAR_RESPONSE_MEATURE_INIT_T & meatureUint, _u32 timeout = DEFAULT_TIMEOUT);

protected:
	u_result _waitNode(_u8 * node, _u32 timeout);
	u_result _waitScanData(RSLIDAR_SIGNAL_DISTANCE_UNIT_T * nodebuffer, size_t & count, _u32 timeout = DEFAULT_TIMEOUT);
	u_result _cacheScanData();
	u_result _cacheSerialData();

    u_result _sendCommand(_u8 cmd, const void * payload = NULL, size_t payloadsize = 0);
	u_result _waitResponseHeader(COMM_FRAME_T * header, const void * payload, size_t payloadsize, _u32 timeout);
	uint16_t _crc16(uint8_t *startByte, uint16_t numBytes);
	uint16_t _checksum(uint8_t *startByte, uint16_t numBytes);

    void     _disableDataGrabbing();

	int     _serial_fd;

    bool     _isConnected;
	bool     _isScanning;
	bool     _isGetData;


	rs::hal::Locker         _lock;
    rs::hal::Event          _dataEvt;
    rs::hal::serial_rxtx  * _rxtx;

	_u8										_flag_cached_scan_node_ping_pong;
	RSLIDAR_SIGNAL_DISTANCE_UNIT_T			_cached_scan_note_bufferPing[1024];
	RSLIDAR_SIGNAL_DISTANCE_UNIT_T			_cached_scan_note_bufferPong[1024];
    size_t									_cached_scan_node_count;
	rs::hal::Thread _cachethread;
	rs::hal::Thread _datathread;

	_u8 _paramReceiveBuf[1024];


	//FILE *receive_data;
	//FILE *error_data;
	SYSTEMTIME tm_tmp;
	SYSTEMTIME tm_error_frame, tm_time_out;
};


}}}
