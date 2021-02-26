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

#include "stdafx.h"
#include "resource.h"
#include <math.h>
#include "scanView.h"

const int DEF_MARGIN = 20;
const int DISP_RING_ABS_DIST = 100;
const float DISP_FULL_DIST   = 20000;
const float DISP_MIN_DIST    = 100;
const float PI = (float)3.14159265;

const COLORREF dot_full_brightness = RGB(44,233,22);
CScanView::CScanView()
{
    bigfont.CreateFontA(32/*-MulDiv(8, GetDeviceCaps(desktopdc, LOGPIXELSY), 72)*/,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH ,"Verdana");
    stdfont.CreateFontA(14/*-MulDiv(8, GetDeviceCaps(desktopdc, LOGPIXELSY), 72)*/,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH ,"Verdana");

    _current_display_range = DISP_FULL_DIST/2;

    _mouse_angle = 0;
    _mouse_pt.x= _mouse_pt.y = 0;
}

BOOL CScanView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

void CScanView::DoPaint(CDCHandle dc)
{

}


void CScanView::onDrawSelf(CDCHandle dc)
{
    CDC memDC;
    CBitmap memBitmap;
    memDC.CreateCompatibleDC(dc);
    CRect clientRECT;
    this->GetClientRect(&clientRECT);

    memBitmap.CreateCompatibleBitmap(dc,clientRECT.Width() , clientRECT.Height());
    HBITMAP oldBitmap = memDC.SelectBitmap(memBitmap);

    HPEN oldPen = memDC.SelectStockPen(DC_PEN);
   
    HBRUSH oldBrush = memDC.SelectStockBrush(NULL_BRUSH);
    HFONT  oldFont  = memDC.SelectFont(stdfont);

    memDC.SetBkMode(0);
    memDC.SetTextColor(RGB(90, 90, 90));
    memDC.SetTextAlign(TA_CENTER | TA_BASELINE);

    memDC.SetDCPenColor(RGB(60,60,60));

    CPoint centerPt(clientRECT.Width()/2, clientRECT.Height()/2);
    const int maxPixelR = min(clientRECT.Width(), clientRECT.Height())/2 - DEF_MARGIN;
    const float distScale = (float)maxPixelR/_current_display_range;

    char txtBuffer[100];

    // plot rings
    for (int angle = 0; angle<360; angle += 30) {
        float rad = (float)(angle*PI/180.0);

        float endptX = sin(rad)*(maxPixelR+DEF_MARGIN/2) + centerPt.x;
        float endptY = centerPt.y - cos(rad)*(maxPixelR+DEF_MARGIN/2);

        memDC.MoveTo(centerPt);
        memDC.LineTo((int)endptX, (int)endptY);

        sprintf(txtBuffer, "%d", angle);
        memDC.TextOutA((int)endptX, (int)endptY, txtBuffer);

    }

    for (int plotR = maxPixelR; plotR > 0; plotR-=DISP_RING_ABS_DIST)
    {
        memDC.Ellipse(centerPt.x-plotR, centerPt.y-plotR,
            centerPt.x+plotR, centerPt.y+plotR);

        sprintf(txtBuffer, "%.1f", (float)plotR/distScale);
        memDC.TextOutA(centerPt.x, centerPt.y-plotR, txtBuffer);
    }

    memDC.SelectStockBrush(DC_BRUSH);
    memDC.SelectStockPen(NULL_PEN);

    int picked_point = 0;
    float min_picked_dangle = 100;

    for (int pos =0; pos < (int)_scan_data.size(); ++pos) {
        float distPixel = _scan_data[pos].dist*distScale;
        float rad = (float)(_scan_data[pos].angle*PI/180.0);
        float endptX = sin(rad)*(distPixel) + centerPt.x;
        float endptY = centerPt.y - cos(rad)*(distPixel);

        float dangle = fabs(rad - _mouse_angle);

        if (dangle<min_picked_dangle) {
            min_picked_dangle = dangle;
            picked_point = pos;
        }
		memDC.FillSolidRect((int)endptX - 1, (int)endptY - 1, 2, 2, RGB(0,255,255));
    }

    memDC.SelectFont(bigfont);

    memDC.SetTextAlign(TA_LEFT | TA_BASELINE);
    memDC.SetTextColor(RGB(255,255,255));

    if ((int)_scan_data.size() > picked_point) {
        float distPixel = _scan_data[picked_point].dist*distScale;
        float rad = (float)(_scan_data[picked_point].angle*PI/180.0);
        float endptX = sin(rad)*(distPixel) + centerPt.x;
        float endptY = centerPt.y - cos(rad)*(distPixel);


        memDC.SetDCPenColor(RGB(129,10,16));
        memDC.SelectStockPen(DC_PEN);
        memDC.MoveTo(centerPt.x,centerPt.y);
        memDC.LineTo((int)endptX,(int)endptY);
        memDC.SelectStockPen(NULL_PEN);

        memDC.FillSolidRect((int)endptX-1,(int)endptY-1, 2, 2,RGB(255,0,0));

		memDC.SetTextColor(RGB(35, 186, 99));
		sprintf(txtBuffer, " Motor set speed:    %.2fr/s", ((float)Speed/100.0));
		memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 20, txtBuffer);

		sprintf(txtBuffer, "Motor current  speed:    %.2fr/s",_scan_data[picked_point].speed);
		memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 60, txtBuffer);

		sprintf(txtBuffer, "Current  angle:    %.2f¡ã", _scan_data[picked_point].angle);
        memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 100, txtBuffer);

		sprintf(txtBuffer, "Current distance: %.3fm", (_scan_data[picked_point].dist / 1000));
		memDC.TextOutA(DEF_MARGIN, DEF_MARGIN + 140, txtBuffer);

		
    }

    dc.BitBlt(0, 0, clientRECT.Width(), clientRECT.Height()
        , memDC, 0, 0, SRCCOPY);

    memDC.SelectFont(oldFont);
    memDC.SelectBrush(oldBrush);
    memDC.SelectPen(oldPen);
    memDC.SelectBitmap(oldBitmap);

    
}


BOOL CScanView::OnEraseBkgnd(CDCHandle dc)
{
 

    return 0;
}

void CScanView::OnMouseMove(UINT nFlags, CPoint point)
{
    _mouse_pt = point;

    CRect clientRECT;
    this->GetClientRect(&clientRECT);

    int dy = -(point.y - ((clientRECT.bottom-clientRECT.top)/2));
    int dx = point.x - ((clientRECT.right-clientRECT.left)/2);

    if (dx >=0 ) {

        _mouse_angle = atan2((float)dx, (float)dy);
    } else {
        _mouse_angle = PI*2 - atan2((float)-dx, (float)dy);
    }

    this->Invalidate();
}

int CScanView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    return 0;//CScrollWindowImpl<CPeakgrabberView>::OnCreate(lpCreateStruct);
}
void CScanView::OnPaint(CDCHandle dc)
{
    if (dc) 
    {
        onDrawSelf(dc);
    }
    else
    {
        CPaintDC tDC(m_hWnd);
        onDrawSelf(tDC.m_hDC);
    }
}

BOOL CScanView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    _current_display_range+=zDelta;
    if (_current_display_range > DISP_FULL_DIST) _current_display_range= DISP_FULL_DIST;
    else if (_current_display_range < DISP_MIN_DIST) _current_display_range = DISP_MIN_DIST;
    this->Invalidate();
	return 0;
}

void CScanView::setScanData(LIDAR_MEASURE_INFO_T *buffer, size_t count)
{
    _scan_data.clear();

	for (int pos = 0; pos < (int)count; ++pos) 
	{
		scanDot dot;
		if (!buffer[pos].distance) continue;
		dot.speed = buffer[pos].motorspeed;
		dot.angleoffset = buffer[pos].angleoffset;
		dot.quality = buffer[pos].signal;
		dot.angle	= ((float)(buffer[pos].angle))/100.0f;
		dot.dist	= buffer[pos].distance;

		_scan_data.push_back(dot);
	}
    this->Invalidate();
}

