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

//
/////////////////////////////////////////////////////////////////////////////

#pragma once


struct scanDot {
    _u8   quality;
	float	  speed;
	float angleoffset;
    float angle;
    float dist;
};

class CScanView : public CWindowImpl<CScanView>
{
public:
	
	DECLARE_WND_CLASS(NULL)

	
	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CScanView)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_PAINT(OnPaint)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_MOUSEMOVE(OnMouseMove)
	END_MSG_MAP()

    void DoPaint(CDCHandle dc);



    const std::vector<scanDot> & getScanList() const {
        return _scan_data;
    }


    void onDrawSelf(CDCHandle dc);
	void setScanData(LIDAR_MEASURE_INFO_T *buffer, size_t count);
    CScanView();

    BOOL OnEraseBkgnd(CDCHandle dc);
	void OnMouseMove(UINT nFlags, CPoint point);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnPaint(CDCHandle dc);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
protected:
    CFont stdfont;
    CFont bigfont;
    POINT                _mouse_pt;
    float                _mouse_angle;
    std::vector<scanDot> _scan_data;
    float                _current_display_range;
public:
	_u16	Speed = 500;
};
