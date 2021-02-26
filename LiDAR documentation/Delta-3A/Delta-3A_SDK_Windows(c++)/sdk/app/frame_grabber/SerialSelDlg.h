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

// SerialSelDlg.h : interface of the CSerialSelDlg class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __SERIALSELDLG_H__
#define __SERIALSELDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSerialSelDlg : public CDialogImpl<CSerialSelDlg>,
    public CWinDataExchange<CSerialSelDlg>
{
private:
	UINT16 FindSerialPort();
public:
	CComboBox	m_sel_box;
	UINT16 SerialNumber;
	CString PortList[260];
	CSerialSelDlg();
	enum { IDD = IDD_DLG_SERIAL_SEL };


	BEGIN_MSG_MAP(CSerialSelDlg)
		COMMAND_HANDLER(IDC_COMB_SERIAL_SEL, CBN_SELCHANGE, OnCbnSelchangeCombSerialSel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CSerialSelDlg)
		DDX_CONTROL_HANDLE(IDC_COMB_SERIAL_SEL, m_sel_box)
	END_DDX_MAP();
	
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    int     getSelectedID() {
        return selectedID;
    }
	LRESULT OnCbnSelchangeCombSerialSel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
    int     selectedID;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// VisualFC AppWizard will insert additional declarations immediately before the previous line.
#endif // __SERIALSELDLG_H__
