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

// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "scanView.h"
#include "MainFrm.h"
#include "drvlogic\lidarmgr.h"

const int REFRESEH_TIMER = 0x800;

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;
    if (m_hWndClient == m_scanview.m_hWnd){
        return m_scanview.PreTranslateMessage(pMsg);
    } else {
        return FALSE;
    }
	
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	CreateSimpleStatusBar();
    m_hWndClient =m_scanview.Create(m_hWnd, rcDefault, NULL, WS_CHILD  | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

    workingMode = WORKING_MODE_IDLE;
    onUpdateTitle();
    // setup timer
    this->SetTimer(REFRESEH_TIMER, 1000/30);

#ifdef feidangong
	checkDeviceHealth();
#endif // feidangong

   
    UISetCheck(ID_CMD_STOP, 1);
	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
    this->KillTimer(REFRESEH_TIMER);
    
    CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;

	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
    switch (workingMode)
    {
    case WORKING_MODE_SLOW_SCAN:
	case WORKING_MODE_RAPID_SCAN:
        onRefreshScanData();
        break;
    }
}

LRESULT CMainFrame::OnCmdReset(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if (MessageBox("The lidar will reboot.", "Confirm?",MB_OKCANCEL|MB_ICONQUESTION) != IDOK) {
           return 0;
    }

    LidarMgr::GetInstance().lidar_drv->resetlidar();
	UISetCheck(ID_CMD_STOP, 0);
	UISetCheck(ID_CMD_SLOW_SCAN, 0);
	UISetCheck(ID_CMD_RAPID_SCAN, 0);
	UISetCheck(ID_CMD_RESET, 0);
	UISetCheck(ID_CMD_MOTOR_SPEED_UP, 0);
	UISetCheck(ID_CMD_MOTOR_SPEED_DOWN, 0);
	UISetCheck(ID_CMD_ABOUT, 0);
	
	system("cls");
	return 0;
}

LRESULT CMainFrame::OnCmdStop(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	onSwitchMode(WORKING_MODE_IDLE);
	return 0;
}

LRESULT CMainFrame::OnCmdMotorSpeed(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	IsAdjustSpeed = true;
	UISetCheck(ID_CMD_STOP, 0);
	UISetCheck(ID_CMD_SLOW_SCAN, 0);
	UISetCheck(ID_CMD_RAPID_SCAN, 0);
	UISetCheck(ID_CMD_RESET, 0);
	UISetCheck(ID_CMD_MOTOR_SPEED_UP, 0);
	UISetCheck(ID_CMD_MOTOR_SPEED_DOWN, 0);
	UISetCheck(ID_CMD_ABOUT, 0);
	if (wID == ID_CMD_MOTOR_SPEED_UP)
		m_scanview.Speed += 100;
	else
		m_scanview.Speed -= 100;
	if (m_scanview.Speed < 500)
		m_scanview.Speed = 500;
	else if (m_scanview.Speed > 1500)
		m_scanview.Speed = 1500;

	LidarMgr::GetInstance().lidar_drv->setMotorRpm(m_scanview.Speed,50);
		
	return 0;
}

LRESULT CMainFrame::OnCmdAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UISetCheck(ID_CMD_STOP, 0);
	UISetCheck(ID_CMD_SLOW_SCAN, 0);
	UISetCheck(ID_CMD_RAPID_SCAN, 0);
	UISetCheck(ID_CMD_RESET, 0);
	UISetCheck(ID_CMD_MOTOR_SPEED_UP, 0);
	UISetCheck(ID_CMD_MOTOR_SPEED_DOWN, 0);
	UISetCheck(ID_CMD_ABOUT, 0);
	return 0;
}


LRESULT CMainFrame::OnCmdSlowScan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    onSwitchMode(WORKING_MODE_SLOW_SCAN);
	return 0;
}

LRESULT CMainFrame::OnCmdRapidScan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	onSwitchMode(WORKING_MODE_RAPID_SCAN);
	return 0;
}

LRESULT CMainFrame::OnFileDumpdata(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    switch (workingMode) {
    case WORKING_MODE_SLOW_SCAN:
	case WORKING_MODE_RAPID_SCAN:
        {
            //capture the snapshot
            std::vector<scanDot> snapshot = m_scanview.getScanList();

            //prompt
            CFileDialog dlg(FALSE);
            if (dlg.DoModal()==IDOK) {
                FILE * outputfile = fopen(dlg.m_szFileName, "w");
                //fprintf(outputfile, "#RPLIDAR SCAN DATA\n#COUNT=%d\n#Angule\tDistance\tQuality\n",snapshot.size());
				fprintf(outputfile, "#RPLIDAR SCAN DATA\n#COUNT=%d\n#Angule\tDistance\n", snapshot.size());
                for (int pos = 0; pos < (int)snapshot.size(); ++pos) {
                    //fprintf(outputfile, "%.4f %.1f %d\n", snapshot[pos].angle, snapshot[pos].dist, snapshot[pos].quality);
					fprintf(outputfile, "%.4f %.1f\n", snapshot[pos].angle, snapshot[pos].dist,1);
                }
                fclose(outputfile);
            }
        }
        break;

    }
	return 0;
}


void    CMainFrame::onRefreshScanData()
{
	LIDAR_MEASURE_INFO_T nodes[360 * 6];
    size_t cnt = _countof(nodes);
	RSlidarDriver * lidar_drv = LidarMgr::GetInstance().lidar_drv;

	if (IS_OK(lidar_drv->grabScanData(nodes, cnt)))
    {
		//_cprintf(">>>>>>%d\n", cnt);
		m_scanview.setScanData(nodes, cnt);
		if (IsAdjustSpeed == true)
			return;

		if (cnt > 100)
		{
			if ((nodes[cnt / 2].motorspeed >= 5) && (nodes[cnt / 2].motorspeed <= 15))
			{
				m_scanview.Speed = (nodes[cnt / 2].motorspeed + 0.4) * 100;
				m_scanview.Speed = (m_scanview.Speed / 100) * 100;
			}
		}
    }
}

void    CMainFrame::onUpdateTitle()
{
    char titleMsg[200];
    const char * workingmodeDesc;
    switch (workingMode) {
    case WORKING_MODE_IDLE:
        workingmodeDesc = "IDLE";
        break;
    case WORKING_MODE_SLOW_SCAN:
        workingmodeDesc = "8K/s SCAN"; //"4K/s SCAN";
        break;

	case WORKING_MODE_RAPID_SCAN:
		workingmodeDesc = "8K/s SCAN";
		break;

    default:
        assert(!"should not come here");
    }

	LIDAR_RESPONSE_DEV_INFO_T & devinfo = LidarMgr::GetInstance().devinfo;

    sprintf(titleMsg, "[%s] GearNumber: %d"
        , workingmodeDesc
		,16);

    this->SetWindowTextA(titleMsg);
}

void    CMainFrame::onSwitchMode(int newMode)
{
    
    // switch mode
    //if (newMode == workingMode) return;
	if ((workingMode > WORKING_MODE_IDLE)&&(newMode != WORKING_MODE_IDLE))
		return;
    
    switch (newMode) {
    case WORKING_MODE_IDLE:
        {
            // stop the previous operation
            LidarMgr::GetInstance().lidar_drv->stopScan();
			UISetCheck(ID_CMD_STOP, 1);
			UISetCheck(ID_CMD_SLOW_SCAN, 0);
			UISetCheck(ID_CMD_RAPID_SCAN, 0);
			UISetCheck(ID_CMD_RESET, 0);
			UISetCheck(ID_CMD_MOTOR_SPEED_UP, 0);
			UISetCheck(ID_CMD_MOTOR_SPEED_DOWN, 0);
			UISetCheck(ID_CMD_ABOUT, 0);
        }
        break;
    case WORKING_MODE_SLOW_SCAN:
        {
            CWindow  hwnd = m_hWndClient;
            hwnd.ShowWindow(SW_HIDE);
            m_hWndClient = m_scanview;
            m_scanview.ShowWindow(SW_SHOW);

			UISetCheck(ID_CMD_STOP, 0);
			UISetCheck(ID_CMD_SLOW_SCAN, 1);
			UISetCheck(ID_CMD_RAPID_SCAN, 0);
			UISetCheck(ID_CMD_RESET, 0);
			UISetCheck(ID_CMD_MOTOR_SPEED_UP, 0);
			UISetCheck(ID_CMD_MOTOR_SPEED_DOWN, 0);
			UISetCheck(ID_CMD_ABOUT, 0);

            LidarMgr::GetInstance().lidar_drv->startScan(SLOW_SCAN_MODE,5000);
            
        }
        break;
	case WORKING_MODE_RAPID_SCAN:
	{
		CWindow  hwnd = m_hWndClient;
		hwnd.ShowWindow(SW_HIDE);
		m_hWndClient = m_scanview;
		m_scanview.ShowWindow(SW_SHOW);

		UISetCheck(ID_CMD_STOP, 0);
		UISetCheck(ID_CMD_SLOW_SCAN, 0);
		UISetCheck(ID_CMD_RAPID_SCAN, 1);
		UISetCheck(ID_CMD_RESET, 0);
		UISetCheck(ID_CMD_MOTOR_SPEED_UP, 0);
		UISetCheck(ID_CMD_MOTOR_SPEED_DOWN, 0);
		UISetCheck(ID_CMD_ABOUT, 0);
		LidarMgr::GetInstance().lidar_drv->startScan(RAPID_SCAN_MODE, 5000);
	}
	break;

    default:
        assert(!"unsupported mode");
    }
    
    UpdateLayout();
    workingMode = newMode;
    onUpdateTitle();
}
