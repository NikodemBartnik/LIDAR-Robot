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

#include "hal/abs_rxtx.h"

namespace rs{ namespace arch{ namespace net{

class raw_serial : public rs::hal::serial_rxtx
{
public:
    enum{
        SERIAL_RX_BUFFER_SIZE = 512,
        SERIAL_TX_BUFFER_SIZE = 128,
    };

    raw_serial();
    virtual ~raw_serial();
    virtual bool bind(const char * portname, uint32_t baudrate, uint32_t flags = 0);
    virtual bool open();
    virtual void close();
    virtual void flush( _u32 flags);
    
    virtual int waitfordata(size_t data_count,_u32 timeout = -1, size_t * returned_size = NULL);

    virtual int senddata(const unsigned char * data, size_t size);
    virtual int recvdata(unsigned char * data, size_t size);

    virtual int waitforsent(_u32 timeout = -1, size_t * returned_size = NULL);
    virtual int waitforrecv(_u32 timeout = -1, size_t * returned_size = NULL);

    virtual size_t rxqueue_count();

    _u32 getTermBaudBitmap(_u32 baud);
protected:
    bool open(const char * portname, uint32_t baudrate, uint32_t flags = 0);
    void _init();

    char _portName[200];
    uint32_t _baudrate;
    uint32_t _flags;

    int serial_fd;

    size_t required_tx_cnt;
    size_t required_rx_cnt;
};

}}}
