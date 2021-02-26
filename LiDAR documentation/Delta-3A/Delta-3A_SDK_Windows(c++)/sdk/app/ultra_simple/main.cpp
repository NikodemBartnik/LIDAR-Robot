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


#include <stdio.h>
#include <stdlib.h>
#include "logfile.h"
#include "rslidar.h" //RSLIDAR standard sdk, all-in-one header

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

using namespace rs::standalone::rslidar;


bool checkRSLIDARDevInfo(RSlidarDriver * drv, _u16 *gearNumber)
{
    u_result     op_result;
	LIDAR_RESPONSE_DEV_INFO_T devinfo;

	op_result = drv->getDeviceInfo(&devinfo);
    if (IS_OK(op_result)) {
		printf("RPLidar Device Info ccc: %d\n", *gearNumber);
    } else {
        fprintf(stderr, "Error, cannot retrieve the lidar DevInfo: %x\n", op_result);
        return false;
    }
	return true;
}

bool startScanTest(RSlidarDriver * drv)
{
	u_result     op_result;

	op_result = drv->startScan(RAPID_SCAN_MODE,5000);
	if (IS_OK(op_result)) { 
		printf("RPLidar start scan ok!\n");
	}
	else {
		fprintf(stderr, "Error, cannot retrieve the lidar start scan: %x\n", op_result);
		return false;
	}
	return true;
}

bool stopScanTest(RSlidarDriver * drv)
{
	u_result     op_result;

	op_result = drv->stopScan();
	if (IS_OK(op_result)) { 
		printf("RPLidar stop scan ok!\n");
	}
	else {
		fprintf(stderr, "Error, cannot retrieve the lidar stop scan: %x\n", op_result);
		return false;
	}
	return true;
}

bool resetRSlidar(RSlidarDriver * drv)
{
	u_result     op_result;

	op_result = drv->resetlidar();
	if (IS_OK(op_result)) {
		printf("RPLidar reset ok!\n");
	}
	else {
		fprintf(stderr, "Error, cannot reset the lidar : %x\n", op_result);
		return false;
	}
	return true;
}

int main(int argc, const char * argv[]) {
    const char * opt_com_path = NULL;
    _u32         opt_com_baudrate = 230400;
    u_result     op_result;
	_u16		 gearNum=0;
	int i = 0;
	int n = 0;
	size_t count = 0;

    // read serial port from the command line...
    if (argc>1) opt_com_path = argv[1]; 

    // read baud rate from the command line if specified...
    if (argc>2) opt_com_baudrate = strtoul(argv[2], NULL, 10);


    if (!opt_com_path) {
#ifdef _WIN32
        // use default com port
        opt_com_path = "\\\\.\\com5";
#else
        opt_com_path = "/dev/ttyUSB0";
#endif
    }

    // create the driver instance
    RSlidarDriver * drv = RSlidarDriver::CreateDriver();
    
    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(-2);
    }

    // make connection...
    if (IS_FAIL(drv->connect(opt_com_path, opt_com_baudrate))) {
        fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n"
            , opt_com_path);
        goto on_finished;
    }

	printf("connected!\n");

	//check health...
	//if (!checkRSLIDARHealthInfo(drv))
	//{
	//	printf("Lidar is not healthy!!\n");
	//	goto on_finished;
	//}

	//// get device info
	//if (!checkRSLIDARDevInfo(drv, &gearNum)) {
	//	goto on_finished;
	//}

	//start scan
	if (!startScanTest(drv)) {
		goto on_finished;
}


	while (true)
	{
		LIDAR_MEASURE_INFO_T nodes[360 *6];
		count = _countof(nodes);

		op_result = drv->grabScanData(nodes, count);

		if (IS_OK(op_result)) 
		{
			drv->ascendScanData(nodes, count);
			//CLogout("++++++++++++++++++++++++++++++++++:%d \r\n", count);

			printf("count = %d\n", count);
			
		}
		else
		{
			printf("grabScanData error: %x\n", op_result);
		}

		/*n++;
		if (5 == n)
		{
			stopScanTest(drv);
			resetRSlidar(drv);
			goto on_finished;
		}*/
	}

    // done!
on_finished:
    RSlidarDriver::DisposeDriver(drv);
    return 0;
}

