/*
*  3iRoboticsLIDAR System II
*  Driver Interface
*
*  Copyright 2017 3iRobotics
*  All rights reserved.
*
*	Author: 3iRobotics, Data:2017-09-15
*
*/

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"

#include "C3iroboticsLidar.h"
#include "CSerialConnection.h"

#define DEG2RAD(x) ((x)*M_PI/180.)
//#ifndef _countof
//#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
//#endif


typedef struct _rslidar_data
{
    _rslidar_data()
    {
       //signal = 0;
        angle = 0.0;
        distance = 0.0;
    }
   // uint8_t signal;
    float   angle;
    float   distance;
}RslidarDataComplete;


using namespace std;
using namespace everest::hwdrivers;

void publish_scan(ros::Publisher *pub,
                  _rslidar_data *nodes,
                  size_t node_count, ros::Time start,
                  double scan_time,
                  float angle_min, float angle_max,
                  std::string frame_id)
{
    sensor_msgs::LaserScan scan_msg;

    scan_msg.header.stamp = start;
    scan_msg.header.frame_id = frame_id;
    scan_msg.angle_min = angle_min;
    scan_msg.angle_max = angle_max;
    scan_msg.angle_increment = (scan_msg.angle_max - scan_msg.angle_min) /(double)(node_count - 1);

    scan_msg.scan_time = scan_time;
    scan_msg.time_increment = scan_time / (double)(node_count-1);
    scan_msg.range_min = 0.15;
    scan_msg.range_max = 16.0;

    scan_msg.ranges.resize(node_count);
    scan_msg.intensities.resize(node_count);

    //Unpack data
    for (size_t i = 0; i < node_count; i++)
    {
        float read_value = (float) nodes[i].distance;
        if (read_value == 0.0)
            scan_msg.ranges[node_count- 1- i] = std::numeric_limits<float>::infinity();
        else
            scan_msg.ranges[node_count -1- i] = read_value;

		//scan_msg.intensities[node_count -1- i] = (float) nodes[i].signal;

	}

    pub->publish(scan_msg);
}

int main(int argc, char * argv[])
{
    // read ros param
    ros::init(argc, argv, "iiirobotics_lidar2_node");

	int    opt_com_baudrate = 230400;
	string opt_com_path;
    string frame_id;
    string lidar_scan;

    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");
    nh_private.param<string>("serial_port", opt_com_path, "/dev/ttyUSB0");
    nh_private.param<string>("frame_id", frame_id, "laser");
//    nh_private.param<string>("lidar_scan", lidar_scan, "scan");
//    ros::Publisher scan_pub = nh.advertise<sensor_msgs::LaserScan>(lidar_scan, 1000);
    ros::Publisher scan_pub = nh.advertise<sensor_msgs::LaserScan>("scan", 1000);

	ros::Time start_scan_time;
    ros::Time end_scan_time;
    double scan_duration;

	start_scan_time = ros::Time::now();

    CSerialConnection serial_connect;
    C3iroboticsLidar robotics_lidar;

    serial_connect.setBaud(opt_com_baudrate);
    serial_connect.setPort(opt_com_path.c_str());
    if(serial_connect.openSimple())
    {
        printf("[AuxCtrl] Open serail port sucessful!\n");
    }
    else
    {
        printf("[AuxCtrl] Open serail port %s failed! \n", opt_com_path.c_str());
        return -1;
    }

    printf("3iRoboticsLidar connected\n");

    robotics_lidar.initilize(&serial_connect);


	#if 1
	bool ret = robotics_lidar.RecvAndAnalysisPthread(&robotics_lidar);
	if(ret)
		printf("RecvAndAnalysisPthread create success!\n");
	else
		printf("RecvAndAnalysisPthread create fail!\n");
	#endif


	TLidarError retvalue;
	
  	// start scan：AA 08 00 04 01 01 00 02 B9 00  
	#if 1
	retvalue = robotics_lidar.setLidarWorkMode(HIGHSPEED_SCAN);
	if(retvalue == EXECUTE_SUCCESS )
		printf("High speed scan set successs!\n");
	else
		printf("High speed scan set fail! ...TLidarError=%d...\n",retvalue);
	#endif

	//reset lidar ：
	#if 0
	retvalue = robotics_lidar.setLidarWorkMode(LIDAR_RESET);
	if(retvalue == EXECUTE_SUCCESS )
		printf("LIDAR reset successs!\n");
	else
		printf("LIDAR reset fail! ...TLidarError=%d...\n",retvalue);
	#endif
	//stop scan		   send: AA 08 00 04 01 01 00 00 B8 00   	return:AA 08 00 04 41 01 00 00 F8 00 
	#if 0
	retvalue = robotics_lidar.setLidarWorkMode(IDLE_MODE);
	if(retvalue == EXECUTE_SUCCESS )
		printf("stop scan set successs!\n");
	else
		printf("stop scan set fail! ...TLidarError=%d...\n",retvalue);
	#endif
	//set rotate_speed     send: AA 09 00 04 04 02 00 0B 00 C8 00 	return:AA 08 00 04 44 01 00 00 FB 00
	#if 0
	retvalue = robotics_lidar.setLidarRotationlSpeed(11);
	if(retvalue == EXECUTE_SUCCESS )
		printf("RotationlSpeed set successs!\n");
	else
		printf("RotationlSpeed set fail! ...TLidarError=%d...\n",retvalue);
	#endif

	
	int size = 0;
	TLidarGrabResult result;
	CLidarDynamicScan lidar_dynamicscan;
	std::vector<RslidarDataComplete> send_lidar_scan_data;
	int lidar_scan_size;
	size_t i;
	RslidarDataComplete one_lidar_data;
	float angle_min; 
	float angle_max;
	int start_node = 0, end_node = 359;
    while (ros::ok())
    {
		 result = robotics_lidar.m_dynamic_scan.getGrabResult();
        switch(result)
        {
            case LIDAR_GRAB_ING:
            {
                break;
            }
            case LIDAR_GRAB_SUCESS:
            {
				robotics_lidar.m_dynamic_scan.resetGrabResult();
			
                lidar_dynamicscan = robotics_lidar.getLidarDynamicScan();
                lidar_scan_size = lidar_dynamicscan.getSize();              
                send_lidar_scan_data.resize(lidar_scan_size);             
                for( i = 0; i < lidar_scan_size; i++)
                {
                    //one_lidar_data.signal = lidar_dynamicscan.signal[i];
                    one_lidar_data.angle = lidar_dynamicscan.m_angle[i];
                    one_lidar_data.distance = lidar_dynamicscan.m_distance[i];
                    send_lidar_scan_data[i] = one_lidar_data;
                }

            	 angle_min = DEG2RAD(0.0f);
            	 angle_max = DEG2RAD(359.0f);

				end_scan_time = ros::Time::now();
				scan_duration = (end_scan_time - start_scan_time).toSec() * 1e-3;
                printf("Receive Lidar count %u!\n", lidar_scan_size);

                //if successful, publish lidar scan
                
                publish_scan(&scan_pub, &send_lidar_scan_data[0], lidar_scan_size,
                         start_scan_time, scan_duration,
                         angle_min, angle_max,
                         frame_id);

				start_scan_time = end_scan_time;

                break;
            }
            case LIDAR_GRAB_ERRO:
            {
                break;
            }
            case LIDAR_GRAB_ELSE:
            {
                printf("[Main] LIDAR_GRAB_ELSE!\n");
                break;
            }
        }
        usleep(50);
		
        ros::spinOnce();
    }

    return 0;
}
