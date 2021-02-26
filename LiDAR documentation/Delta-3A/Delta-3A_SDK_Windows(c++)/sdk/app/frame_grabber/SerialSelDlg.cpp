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

// SerialSelDlg.cpp : implementation of the CSerialSelDlg class
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"

#include "SerialSelDlg.h"

UINT16 CSerialSelDlg::FindSerialPort()
{
	CRegKey RegKey;

	if (RegKey.Open(HKEY_LOCAL_MACHINE, "Hardware\\DeviceMap\\SerialComm") == ERROR_SUCCESS)
	{
		while (true)
		{
			char ValueName[_MAX_PATH];
			unsigned char ValueData[_MAX_PATH];
			DWORD nValueSize = _MAX_PATH;
			DWORD nDataSize = _MAX_PATH;
			DWORD nType;

			if (::RegEnumValue(HKEY(RegKey), SerialNumber, ValueName, &nValueSize, NULL, &nType, ValueData, &nDataSize) == ERROR_NO_MORE_ITEMS)
			{
				break;
			}

			PortList[SerialNumber++] = ValueData;
		}
	}

	return SerialNumber;
}

CSerialSelDlg::CSerialSelDlg()
    : selectedID(-1)
{

}

LRESULT CSerialSelDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
    this->DoDataExchange();
    char buf[100]; 

	SerialNumber = 0;
	FindSerialPort();

	for (int pos = 0; pos < SerialNumber; ++pos) {
		sprintf(buf, "%s", PortList[pos]);
        m_sel_box.AddString(buf);
    }
    m_sel_box.SetCurSel(0);
    selectedID = 0;
	return TRUE;
}

LRESULT CSerialSelDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CSerialSelDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CSerialSelDlg::OnCbnSelchangeCombSerialSel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    selectedID = m_sel_box.GetCurSel();
	return 0;
}
