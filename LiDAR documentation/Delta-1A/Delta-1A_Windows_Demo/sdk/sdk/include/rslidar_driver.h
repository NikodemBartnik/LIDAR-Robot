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
#include <conio.h>
#define USE_CONSOLE

#pragma once





#ifndef __cplusplus
#error "The RSlidar SDK requires a C++ compiler to be built"
#endif

namespace rs { namespace standalone{ namespace rslidar {

class RSlidarDriver {
public:
    enum {
        DEFAULT_TIMEOUT = 200, //5000 ms
    };

    enum {
        DRIVER_TYPE_SERIALPORT = 0x0,
    };
	RUNNING_STATE RunningStates;
	uint16_t StableCnt;
public:
    /// Create an RSLIDAR Driver Instance
    /// This interface should be invoked first before any other operations
    ///
    /// \param drivertype the connection type used by the driver. 
    static RSlidarDriver * CreateDriver(_u32 drivertype = DRIVER_TYPE_SERIALPORT);

    /// Dispose the RSLIDAR Driver Instance specified by the drv parameter
    /// Applications should invoke this interface when the driver instance is no longer used in order to free memory
    static void DisposeDriver(RSlidarDriver * drv);


public:
    /// Open the specified serial port and connect to a target RSLIDAR device
    ///
    /// \param port_path     the device path of the serial port 
    ///        e.g. on Windows, it may be com3 or \\.\com10 
    ///             on Unix-Like OS, it may be /dev/ttyS1, /dev/ttyUSB2, etc
    ///
    /// \param baudrate      the baudrate used
    ///        For most RSLIDAR models, the baudrate should be set to 230400
    ///
    /// \param flag          other flags
    ///        Reserved for future use, always set to Zero
    virtual u_result connect(const char * port_path, _u32 baudrate, _u32 flag = 0) = 0;


    /// Disconnect with the RSLIDAR and close the serial port
    virtual void disconnect() = 0;

    /// Returns TRUE when the connection has been established
    virtual bool isConnected() = 0;

    /// Reset           
    virtual u_result reset(_u32 timeout = DEFAULT_TIMEOUT) = 0;

    /// Get the health status
	virtual u_result getHealth(RSLIDAR_RESPONSE_HEALTH_INFO_T & healthInfo, _u32 timeout = DEFAULT_TIMEOUT) = 0;

    /// Get the device information
	virtual u_result getDeviceInfo(RSLIDAR_RESPONSE_DEVICE_INFO_T * info, _u32 timeout = DEFAULT_TIMEOUT) = 0;


    /// Get current scanning motorRPM
	virtual u_result getMotorRpm(RSLIDAR_RESPONSE_MOTOR_INFO_T & motorInfo, _u32 timeout = DEFAULT_TIMEOUT) = 0;

    /// Start scan operation
	virtual u_result startScan(_u32 timeout = DEFAULT_TIMEOUT) = 0;


    /// Stop scan operation
    virtual u_result stop(_u32 timeout = DEFAULT_TIMEOUT) = 0;


    /// Wait and grab a complete 0-360 degree scan data.
	virtual u_result grabScanData(RSLIDAR_SIGNAL_DISTANCE_UNIT_T * nodebuffer, size_t & count, _u32 timeout = DEFAULT_TIMEOUT) = 0;
	virtual u_result ascendScanData(RSLIDAR_SIGNAL_DISTANCE_UNIT_T * nodebuffer, size_t count) = 0;

	virtual u_result setMotorControl(bool force = false, _u32 timeout = DEFAULT_TIMEOUT) = 0;
	virtual u_result setMotorRpm(RSLIDAR_RESPONSE_MOTOR_INFO_T & motorInfo, _u32 timeout = DEFAULT_TIMEOUT) = 0;
	virtual u_result setMeatureUnit(RSLIDAR_RESPONSE_MEATURE_INIT_T & meatureUint, _u32 timeout = DEFAULT_TIMEOUT) = 0;

protected:
    RSlidarDriver() {}
    virtual ~RSlidarDriver() {}
};


}}}
