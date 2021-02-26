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

// framegrabber.cpp : main source file for framegrabber.exe
//

#include "stdafx.h"

#include "resource.h"
#include "scanView.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "SerialSelDlg.h"
#include "drvlogic\lidarmgr.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;
    CSerialSelDlg serialsel;

    if (serialsel.DoModal() == IDCANCEL) return 0;

    char serialpath[255];
	sprintf(serialpath, "\\\\.\\%s", serialsel.PortList[serialsel.getSelectedID()]);
    
    if ((serialsel.SerialNumber < 1)||(!LidarMgr::GetInstance().onConnect(serialpath))) {
        MessageBox(NULL, "Cannot open the specified port.", "Error", MB_OK);
        return -1;
    }


	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();

    LidarMgr::GetInstance().onDisconnect();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));
#ifdef  USE_CONSOLE
	AllocConsole();
#endif//USE_CONSOLE

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
    

	::CoUninitialize();

	return nRet;
}
