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

#include "sdkcommon.h"
#include "net_serial.h"

namespace rs{ namespace arch{ namespace net{

raw_serial::raw_serial()
    : rs::hal::serial_rxtx()
    , _baudrate(0)
    , _flags(0)
	, _serial_handle(NULL)
{
    _init();
}

raw_serial::~raw_serial()
{
    close();

    CloseHandle(_ro.hEvent);
    CloseHandle(_wo.hEvent);
    CloseHandle(_wait_o.hEvent);
}

bool raw_serial::open()
{
    return open(_portName, _baudrate, _flags);
}

int  raw_serial::get_serialfd()
{
	//return _serial_handle;
	return 0;
}

bool raw_serial::bind(const char * portname, _u32 baudrate, _u32 flags)
{   
    strncpy(_portName, portname, sizeof(_portName));
    _baudrate = baudrate;
    _flags    = flags;
    return true;
}

bool raw_serial::open(const char * portname, _u32 baudrate, _u32 flags)
{
    if (isOpened()) close();
    
    _serial_handle = CreateFile(
        portname,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL
        );

    if (_serial_handle == INVALID_HANDLE_VALUE) return false;

    if (!SetupComm(_serial_handle, SERIAL_RX_BUFFER_SIZE, SERIAL_TX_BUFFER_SIZE))
    {
        close();
        return false;
    }
    
    _dcb.BaudRate = baudrate;
    _dcb.ByteSize = 8;
    _dcb.Parity   = NOPARITY;
    _dcb.StopBits = ONESTOPBIT;
    _dcb.fDtrControl = DTR_CONTROL_ENABLE;

    if (!SetCommState(_serial_handle, &_dcb))
    {
        close();
        return false;
    }

    //Clear the DTR bit to let the motor spin
    EscapeCommFunction(_serial_handle, CLRDTR);

    if (!SetCommTimeouts(_serial_handle, &_co))
    {
        close();
        return false;
    }

    if (!SetCommMask(_serial_handle, EV_RXCHAR | EV_ERR ))
    {
        close();
        return false;
    }

    if (!PurgeComm(_serial_handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ))
    {
        close();
        return false;
    }

    Sleep(30); 
    _is_serial_opened = true;
    return true;
}

void raw_serial::close()
{
    SetCommMask(_serial_handle, 0);
    ResetEvent(_wait_o.hEvent);

    CloseHandle(_serial_handle);
    _serial_handle = INVALID_HANDLE_VALUE;
    
    _is_serial_opened = false;
}

int raw_serial::senddata(const unsigned char * data, size_t size)
{
    DWORD    error;
    DWORD w_len = 0, o_len = -1;
    if (!isOpened()) return ANS_DEV_ERR;

    if (data == NULL || size ==0) return 0;
    
    if(ClearCommError(_serial_handle, &error, NULL) && error > 0)
        PurgeComm(_serial_handle, PURGE_TXABORT | PURGE_TXCLEAR);

    if(!WriteFile(_serial_handle, data, size, &w_len, &_wo))
        if(GetLastError() != ERROR_IO_PENDING)
            w_len = ANS_DEV_ERR;

    return w_len;
}

int raw_serial::recvdata(unsigned char * data, size_t size)
{
    if (!isOpened()) return 0;
    DWORD r_len = 0;


    if(!ReadFile(_serial_handle, data, size, &r_len, &_ro))
    {
        if(GetLastError() == ERROR_IO_PENDING) 
        {
            if(!GetOverlappedResult(_serial_handle, &_ro, &r_len, FALSE))
            {
                if(GetLastError() != ERROR_IO_INCOMPLETE)
                    r_len = 0;
            }
        }
        else
            r_len = 0;
    }

    return r_len;
}

void raw_serial::flush( _u32 flags)
{
    PurgeComm(_serial_handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
}

int raw_serial::waitforsent(_u32 timeout, size_t * returned_size)
{
    if (!isOpened() ) return ANS_DEV_ERR;
    DWORD w_len = 0;
    _word_size_t ans =0;

    if (WaitForSingleObject(_wo.hEvent, timeout) == WAIT_TIMEOUT)
    {
        ans = ANS_TIMEOUT;
        goto _final;
    }
    if(!GetOverlappedResult(_serial_handle, &_wo, &w_len, FALSE))
    {
        ans = ANS_DEV_ERR;
    }
_final:
    if (returned_size) *returned_size = w_len;
    return ans;
}

int raw_serial::waitforrecv(_u32 timeout, size_t * returned_size)
{
    if (!isOpened() ) return -1;
    DWORD r_len = 0;
    _word_size_t ans =0;

    if (WaitForSingleObject(_ro.hEvent, timeout) == WAIT_TIMEOUT)
    {
        ans = ANS_TIMEOUT;
    }
    if(!GetOverlappedResult(_serial_handle, &_ro, &r_len, FALSE))
    {
        ans = ANS_DEV_ERR;
    }
    if (returned_size) *returned_size = r_len;
    return ans;
}


int raw_serial::waitfordata(size_t data_count, _u32 timeout, size_t * returned_size)
{
	COMSTAT  stat;
	DWORD error;
	DWORD msk, length;
	size_t rxqueue_remaining = 0;
	if (returned_size == NULL) returned_size = (size_t *)&length;

	if (isOpened()) {
		rxqueue_remaining = rxqueue_count();
		if (rxqueue_remaining >= data_count) {
			*returned_size = rxqueue_remaining;
			return 0;
		}
	}

	while (isOpened())
	{
		msk = 0;
		SetCommMask(_serial_handle, EV_RXCHAR | EV_ERR);
		if (!WaitCommEvent(_serial_handle, &msk, &_wait_o))  //如果异步操作不能马上完成，那么该函数会返回一个FALSE
		{
			if (GetLastError() == ERROR_IO_PENDING)//表示操作转到后台运行
			{
				switch (WaitForSingleObject(_wait_o.hEvent, timeout))
				{
					case WAIT_TIMEOUT:
						//printf("WAIT_TIMEOUT:%d\n", rxqueue_remaining);
						*returned_size = 0;
#if 0
						if (rxqueue_remaining > 0)
						{
							*returned_size = rxqueue_remaining;
						}
#endif
						return ANS_TIMEOUT;
						break;

					case WAIT_OBJECT_0:
						//printf("WAIT_OBJECT_0:%d\n", rxqueue_remaining);
						GetOverlappedResult(_serial_handle, &_wait_o, &length, TRUE);
						::ResetEvent(_wait_o.hEvent);
						break;

					case WAIT_FAILED:
						return ANS_DEV_ERR;
						break;

					default:
						break;
				}
			}
			else
			{
				ClearCommError(_serial_handle, &error, &stat);
				*returned_size = stat.cbInQue;
				return ANS_DEV_ERR;
			}
		}
		// WaitCommEvent TURE, returned immediately

		if (msk & EV_ERR){
			// FIXME: may cause problem here
			ClearCommError(_serial_handle, &error, &stat);
		}

		if (msk & EV_RXCHAR){
			ClearCommError(_serial_handle, &error, &stat);
			if (stat.cbInQue >= data_count)
			{
				*returned_size = stat.cbInQue;
				return 0;
			}
		}
	}

	*returned_size = 0;
	return ANS_DEV_ERR;
}

size_t raw_serial::rxqueue_count()
{
    if  ( !isOpened() ) return 0;
    COMSTAT  com_stat;
    DWORD error;
    DWORD r_len = 0;

    if(ClearCommError(_serial_handle, &error, &com_stat) && error > 0)
    {
        PurgeComm(_serial_handle, PURGE_RXABORT | PURGE_RXCLEAR);
        return 0;
    }
    return com_stat.cbInQue;
}


void raw_serial::_init()
{
    memset(&_dcb, 0, sizeof(_dcb));
    _dcb.DCBlength = sizeof(_dcb);
    _serial_handle = INVALID_HANDLE_VALUE;
    memset(&_co, 0, sizeof(_co));
    _co.ReadIntervalTimeout = 0;
    _co.ReadTotalTimeoutMultiplier = 0;
    _co.ReadTotalTimeoutConstant = 0;
    _co.WriteTotalTimeoutMultiplier = 0;
    _co.WriteTotalTimeoutConstant = 0;

    memset(&_ro, 0, sizeof(_ro));
    memset(&_wo, 0, sizeof(_wo));
    memset(&_wait_o, 0, sizeof(_wait_o));

    _ro.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    _wo.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    _wait_o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    _portName[0] = 0;
}

}}} //end rs::arch::net


//begin rs::hal
namespace rs{ namespace hal{

serial_rxtx * serial_rxtx::CreateRxTx()
{
    return new rs::arch::net::raw_serial();
}

void  serial_rxtx::ReleaseRxTx( serial_rxtx * rxtx)
{
    delete rxtx;
}


}} //end rs::hal
