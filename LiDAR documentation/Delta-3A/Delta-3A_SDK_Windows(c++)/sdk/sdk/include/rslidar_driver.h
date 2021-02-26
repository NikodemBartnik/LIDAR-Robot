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
//#define USE_CONSOLE

#pragma once

#ifndef __cplusplus
#error "The RSlidar SDK requires a C++ compiler to be built"
#endif

namespace rs { namespace standalone{ namespace rslidar {

class RSlidarDriver {
public:
    enum {
        DEFAULT_TIMEOUT = 300, //5000 ms
    };

	RUNNING_STATE RunningStates;
public:
    /// Create an RSLIDAR Driver Instance
    /// This interface should be invoked first before any other operations
    ///
    /// \param drivertype the connection type used by the driver. 
    static RSlidarDriver * CreateDriver();

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
    virtual u_result resetlidar(_u32 timeout = DEFAULT_TIMEOUT) = 0;

	virtual u_result getDeviceInfo(LIDAR_RESPONSE_DEV_INFO_T *info, _u32 timeout = DEFAULT_TIMEOUT) = 0;
    /// Start scan operation
	virtual u_result startScan(_u8 scanmode, _u32 timeout) = 0;

    /// Stop scan operation
    virtual u_result stopScan(_u32 timeout = DEFAULT_TIMEOUT) = 0;

    /// Wait and grab a complete 0-360 degree scan data.
	virtual u_result grabScanData(LIDAR_MEASURE_INFO_T * nodebuffer, size_t & count, _u32 timeout = DEFAULT_TIMEOUT) = 0;
	virtual u_result ascendScanData(LIDAR_MEASURE_INFO_T * nodebuffer, size_t count) = 0;

	virtual u_result setMotorRpm(_u16 speed, _u32 timeout = DEFAULT_TIMEOUT) = 0;

protected:
    RSlidarDriver() {}
    virtual ~RSlidarDriver() {}
};


}}}
