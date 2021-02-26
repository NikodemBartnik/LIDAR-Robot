/*
 * Copyright (C) 2014  RoboPeak
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
/*
 *  RoboPeak Lidar System
 *  Simple Data Grabber Demo App
 *
 *  Copyright 2009 - 2014 RoboPeak Team
 *  http://www.robopeak.com
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "rslidar.h" //RPLIDAR standard sdk, all-in-one header

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

using namespace rs::standalone::rslidar;

void print_usage(int argc, const char * argv[])
{
    printf("Simple LIDAR data grabber for RPLIDAR.\n"
           "Usage:\n"
           "%s <com port> [baudrate]\n"
           "The default baudrate is 230400. Please refer to the datasheet for details.\n"
           , argv[0]);
}


void plot_histogram(RSLIDAR_SIGNAL_DISTANCE_UNIT_T * nodes, size_t count)
{
    const int BARCOUNT =  75;
    const int MAXBARHEIGHT = 20;
    const float ANGLESCALE = 360.0f/BARCOUNT;

    float histogram[BARCOUNT];
    for (int pos = 0; pos < _countof(histogram); ++pos) {
        histogram[pos] = 0.0f;
    }

    float max_val = 0;
    for (int pos =0 ; pos < (int)count; ++pos) {
        int int_deg = (int)((nodes[pos].angle)/64.0f/ANGLESCALE);
        if (int_deg >= BARCOUNT) int_deg = 0;
        float cachedd = histogram[int_deg];
        if (cachedd == 0.0f ) {
            cachedd = nodes[pos].distanceValue/4.0f;
        } else {
			cachedd = (nodes[pos].distanceValue / 4.0f + cachedd) / 2.0f;
        }

        if (cachedd > max_val) max_val = cachedd;
        histogram[int_deg] = cachedd;
    }

    for (int height = 0; height < MAXBARHEIGHT; ++height) {
        float threshold_h = (MAXBARHEIGHT - height - 1) * (max_val/MAXBARHEIGHT);
        for (int xpos = 0; xpos < BARCOUNT; ++xpos) {
            if (histogram[xpos] >= threshold_h) {
                putc('*', stdout);
            }else {
                putc(' ', stdout);
            }
        }
        printf("\n");
    }
    for (int xpos = 0; xpos < BARCOUNT; ++xpos) {
        putc('-', stdout);
    }
    printf("\n");
}

u_result capture_and_display(RSlidarDriver * drv)
{
    u_result ans;
    
	RSLIDAR_SIGNAL_DISTANCE_UNIT_T nodes[360 * 2];
    size_t   count = _countof(nodes);

    printf("waiting for data...\n");

    // fetech extactly one 0-360 degrees' scan
    ans = drv->grabScanData(nodes, count);
	_cprintf("%d\n", count);
    if (IS_OK(ans) || ans == RESULT_OPERATION_TIMEOUT) {
        drv->ascendScanData(nodes, count);
        plot_histogram(nodes, count);

        printf("Do you want to see all the data? (y/n) ");
        int key = getchar();
        if (key == 'Y' || key == 'y') {
            for (int pos = 0; pos < (int)count ; ++pos) {
                printf("theta: %03.2f Dist: %08.2f \n", 
                    (nodes[pos].angle)/64.0f,
                    nodes[pos].distanceValue/4.0f);
            }
        }
    } else {
        printf("error code: %x\n", ans);
    }

    return ans;
}

int main(int argc, const char * argv[]) {
    const char * opt_com_path = NULL;
    _u32         opt_com_baudrate = 230400;
    u_result     op_result;

	// read serial port from the command line...
	if (argc>1) opt_com_path = argv[1];

	// read baud rate from the command line if specified...
	if (argc>2) opt_com_baudrate = strtoul(argv[2], NULL, 10);


	if (!opt_com_path) {
#ifdef _WIN32
		// use default com port
		opt_com_path = "\\\\.\\com3";
#else
		opt_com_path = "/dev/ttyUSB0";
#endif
	}

    // create the driver instance
    RSlidarDriver * drv = RSlidarDriver::CreateDriver(RSlidarDriver::DRIVER_TYPE_SERIALPORT);

    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(-2);
    }

	RSLIDAR_RESPONSE_HEALTH_INFO_T healthinfo;
	RSLIDAR_RESPONSE_DEVICE_INFO_T devinfo;
    do {
        // try to connect
        if (IS_FAIL(drv->connect(opt_com_path, opt_com_baudrate))) {
            fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n"
                , opt_com_path);
            break;
        }

        // retrieving the device info
        ////////////////////////////////////////
    //    op_result = drv->getDeviceInfo(&devinfo);

    //    if (IS_FAIL(op_result)) {
    //        if (op_result == RESULT_OPERATION_TIMEOUT) {
    //            // you can check the detailed failure reason
    //            fprintf(stderr, "Error, operation time out.\n");
				//_u16 gearNumber = (devinfo.gearNum[0] - 48) * 100 + (devinfo.gearNum[1] - 48) * 10 + (devinfo.gearNum[2] - 48);
				//printf("RPLidar Device Info ccc: %d\n", gearNumber);
    //        } else {
    //            fprintf(stderr, "Error, unexpected error, code: %x\n", op_result);
    //            // other unexpected result
    //        }
    //        break;
    //    }

        // check the device health
        ////////////////////////////////////////
   //     op_result = drv->getHealth(healthinfo);
   //     if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
			//printf("RPLidar health status: %d\n", healthinfo.deviceHealthInfo);
			//if (!(0x02 == healthinfo.deviceHealthInfo || 0x00 == healthinfo.deviceHealthInfo))
			//{
			//	break;
			//}
   //     } else {
   //         fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
   //         break;
   //     }

        // take only one 360 deg scan and display the result as a histogram
        ////////////////////////////////////////////////////////////////////////////////
        if (IS_FAIL(drv->startScan( /* true */ ))) // you can force rplidar to perform scan operation regardless whether the motor is rotating
        {
            fprintf(stderr, "Error, cannot start the scan operation.\n");
            break;
        }

        if (IS_FAIL(capture_and_display(drv))) {
            fprintf(stderr, "Error, cannot grab scan data.\n");
            break;

        }

    } while(0);


    RSlidarDriver::DisposeDriver(drv);
    return 0;
}
