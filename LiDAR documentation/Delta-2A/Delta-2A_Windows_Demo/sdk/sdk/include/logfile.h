/******************************************************************************
//功能:	输出日志信息  logfile.h
//  
//版本号:	1.
//
//作者:	王小丁hixi@21cn.com
//时间:	2013/5/15
******************************************************************************/

/******************************************************************************
	1,	使用简单方便.只有一个头文件logfile.h include后,直接调用函数即可
	2,	VC6,VC7(VS2008) 兼容VC版本
	3,	可输出文件名及行号
	4,	支持多线程应用

  例如:
  在cpp源代码文件中只要#include "logfile.h"后，就可以直接调用以下函数输出日志信息

	CLogout("I am CLogout \r\n");
	CLogout(AT"I am CLogoutAT \r\n");
	CLoglevelout(3,"I am CLoglevelout");

	CString test = " i am  wangxiaoding!";
	int n = 8;
	CLogout("CString = %s \r\n",test);
	CLogout("CString = %d \r\n",n);


******************************************************************************/
//防止多次include头文件
#if !defined(AFX_LOGFILE_H__EF4BC4B2_3BB6_46E8_B936_0F3A61E20BF0__INCLUDED_)
#define AFX_LOGFILE_H__EF4BC4B2_3BB6_46E8_B936_0F3A61E20BF0__INCLUDED_

#pragma once

//-----------------------------------------------------------------------------

// Debug版本宏1
#if _DEBUG
#ifndef _FLAG_OUTLOG_ENABLE
//#define _FLAG_OUTLOG_ENABLE TRUE
#endif // _FLAG_OUTLOG_ENABLE
#endif // _DEBUG

// 设置控制台宏2
//#define  _DEBUGCONSOLE

// 设置文件名宏3
//#define  _SETFILENAME
#ifdef _SETFILENAME
#define  FILENAME "log.txt"
#endif	//_SETFILENAME

// 设置等级宏4
#define  _LOGLEVEL
#ifdef _LOGLEVEL
#define	MIN_LEVEL	1
#define	MAX_LEVEL	5
#endif	// _LOGLEVEL


//-----------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h> 
#include <stdarg.h>

// 日志输出类，静态版
struct CLog
{    
    
	// 取进程执行文件名称
    static void GetProcessFileName(char* lpName)
    {
        if ( ::GetModuleFileNameA(NULL, lpName, MAX_PATH) > 0)
        {
            char* pBegin = lpName;
            char* pTemp  = lpName;
            while ( *pTemp != 0 )
            {
                if ( *pTemp == '\\' )
                {
                    pBegin = pTemp + 1;
                }
                pTemp++;
            }

            memcpy(lpName, pBegin, strlen(pBegin)+1);
        }

    }

	// 输出到文件
    // lpFile   : 源文件名
    // nLine    : 源文件行号
    // lpFormat : 输出的内容
    static void logout(LPCSTR lpFile, int nLine,LPCSTR lpFormat, ...)
    {
		static CRITICAL_SECTION  m_crit;
		if (m_crit.DebugInfo==NULL)
		::InitializeCriticalSection(&m_crit); 
		/*-----------------------进入临界区(输出信息)------------------------------*/   
		::EnterCriticalSection(&m_crit);   

        if ( NULL == lpFormat )
            return;

        //当前时间
        SYSTEMTIME st;
        ::GetLocalTime(&st);

        //设置消息头
        const DWORD BufSize = 2048;
        char szMsg[BufSize];

		if (nLine==0)
		{	
			//当nLine==0 时,即CLogout("xxx")只打印信息
			sprintf(szMsg, "[%02d:%02d:%02d.%03d]:", 
				st.wHour, st.wMinute, st.wSecond, 
									st.wMilliseconds);
		}
		else
		{
			//当nLine不等于0 时,即CLogout(AT"xxx")打印文件名行号及信息
			sprintf(szMsg, "[%02d:%02d:%02d.%03d]文件%s第%04d行:", 
				st.wHour, st.wMinute, st.wSecond, 
									  st.wMilliseconds, lpFile, nLine);

		}

		//格式化消息,并完善整条消息
        char* pTemp = szMsg;
        pTemp += strlen(szMsg);
        va_list args; 
        va_start(args, lpFormat);    
        wvsprintfA(pTemp,  lpFormat, args);	 //vsprintf_s BufSize - strlen(szMsg),
        va_end(args);  

        DWORD dwMsgLen = (DWORD)strlen(szMsg);

        //获取日志文件名
		char szFileName[MAX_PATH];
        char szExeName[MAX_PATH];
        GetProcessFileName(szExeName);
        sprintf(szFileName, "Log(%s)%d-%d-%d.txt", szExeName, //sprintf_s MAX_PATH
                                        st.wYear, st.wMonth, st.wDay);

        // 判断文件名称是否相同,句柄是否有效.
        // 如果不同或无效,则关闭当前文件,创建新文件
        static char   s_szFileName[MAX_PATH] = {0};
        static HANDLE s_hFile = INVALID_HANDLE_VALUE; 

		//设置自定义日志文件名
#ifdef _SETFILENAME
		strcpy(szFileName,FILENAME);
#endif	//_SETFILENAME

        BOOL bNew = ((strcmp(s_szFileName, szFileName) != 0) || (s_hFile == INVALID_HANDLE_VALUE));
        

#ifdef _DEBUGCONSOLE	//控制台输出
		static BOOL bOpenConsole = FALSE;

		if (!bOpenConsole)
		{
			bOpenConsole = ::AllocConsole();
			if (bOpenConsole)
			{
				freopen("CONOUT$","w+t",stdout);  
				freopen("CONIN$","r+t",stdin);
				freopen("CONERR", "w", stderr);
				
				HANDLE handle= GetStdHandle(STD_OUTPUT_HANDLE);	
				SetConsoleTitle("DebugCosole");
				SetConsoleTextAttribute((HANDLE)handle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				
				HWND hwnd=NULL; 
				while(NULL==hwnd) 
					hwnd=::FindWindow(NULL,(LPCTSTR)"DebugCosole"); 
				
				HMENU hmenu = ::GetSystemMenu ( hwnd, FALSE ); 
				DeleteMenu ( hmenu, SC_CLOSE, MF_BYCOMMAND );
			}
		}
#endif	//_DEBUGCONSOLE

        printf("%s", szMsg);

        if ( bNew ) // 关闭旧文件，创建新文件
        {
            if ( s_hFile != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(s_hFile);
                s_hFile = INVALID_HANDLE_VALUE;
            }
			
			//创建日志文件. 有文件时追加方式打开,没有时创建.
            s_hFile = ::CreateFileA( szFileName, 
                                     GENERIC_WRITE, 
                                     FILE_SHARE_WRITE | FILE_SHARE_READ, 
                                     0, 
                                     OPEN_ALWAYS, 
                                     FILE_ATTRIBUTE_NORMAL, 
                                     0);

            if ( s_hFile == INVALID_HANDLE_VALUE)
            {
                printf("::CreateFile Error: %d", ::GetLastError());
                return;
            }
        }



        //把消息写到文件
        if ( s_hFile != INVALID_HANDLE_VALUE) 
        {
            DWORD dwWrite = 0;
            ::SetFilePointer(s_hFile, 0, NULL, FILE_END);
            ::WriteFile(s_hFile, szMsg, dwMsgLen, &dwWrite, NULL);
            
			//备份创建成功的新文件名
			strcpy(s_szFileName,szFileName);
        }

		::LeaveCriticalSection(&m_crit);    
		/*----------------------------退出临界区---------------------------------*/
    }

}; // CLog


//宏定义文件名和行号
#define AT __FILE__, __LINE__,



#if (_FLAG_OUTLOG_ENABLE) 

//日志输出接口函数1
static void CLogout(LPCSTR lpFormat, ...)
{
	const DWORD BufSize = 2048;
    char szMsg[BufSize];
	
	va_list args; 	//格式化消息

	va_start(args, lpFormat);    
	wvsprintfA(szMsg,  lpFormat, args);	 //vsprintf_s BufSize - strlen(szMsg),
	va_end(args);  
		
	//输出信息
	CLog::logout("0",0,szMsg);
}

//日志输出接口函数2  使用于Clogout(AT"xxxx")形式
static void CLogout(LPCSTR lpFile, int nLine,LPCSTR lpFormat, ...)
{
	const DWORD BufSize = 2048;
    char szMsg[BufSize];
	
	char* pTemp = szMsg;
	
	va_list args; //格式化消息
	
	va_start(args, lpFormat);    
	wvsprintfA(szMsg,  lpFormat, args);	 //vsprintf_s BufSize - strlen(szMsg),
	va_end(args);  
	
	//输出有文件名及行号的消息
	CLog::logout(lpFile, nLine,szMsg);
}

#ifdef _LOGLEVEL
//日志输出接口函数3
static void CLoglevelout(int nshowlevel,LPCSTR lpFormat, ...)
{

#ifdef _SETFILENAME
	if (MIN_LEVEL<=nshowlevel && nshowlevel<= MAX_LEVEL)
#endif
	{
		const DWORD BufSize = 2048;
		char szMsg[BufSize];
		
		va_list args; 	//格式化消息
		
		va_start(args, lpFormat);    
		wvsprintfA(szMsg,  lpFormat, args);	 //vsprintf_s BufSize - strlen(szMsg),
		va_end(args);  
		
		char buffer[20];
   		_itoa(nshowlevel, buffer, 10 );
		strcat(szMsg,"......Level=");
        strcat(szMsg,buffer);
		strcat(szMsg,"\r\n");

		//输出信息
		CLog::logout("0",0,szMsg);
	}

}
#endif //_LOGLEVEL

#ifdef _DEBUGCONSOLE
//关闭控制台接口函数4 空 用于Release版本
static void CLogconsole_close()
{
	FreeConsole();
}

static void CLogcconsole_win(BOOL pSHWinConsole = FALSE)
{
	static BOOL bGetWinConsole = FALSE;
	HWND wincmd = NULL;

	if (!bGetWinConsole)
		{
			typedef HWND (WINAPI *PROCGETCONSOLEWINDOW)();
			PROCGETCONSOLEWINDOW GetConsoleWindow;
			
			HMODULE hKernel32 = GetModuleHandle("kernel32");
			GetConsoleWindow = (PROCGETCONSOLEWINDOW)GetProcAddress(hKernel32,"GetConsoleWindow");

			wincmd=GetConsoleWindow();
		}
	if (pSHWinConsole)
	{
		ShowWindowAsync(wincmd, SW_SHOWNORMAL);
	}
	else
	{
		ShowWindowAsync(wincmd, SW_HIDE );
	}
}
#endif //_DEBUGCONSOLE

#else  //_FLAG_OUTLOG_ENABLE

//日志输出接口函数1 空 用于Release版本
static void CLogout(LPCSTR lpFormat, ...)
{
}
//日志输出接口函数2 空 用于Release版本
static void CLogout(LPCSTR lpFile, int nLine,LPCSTR lpFormat, ...)
{
}

#ifdef _LOGLEVEL
//日志输出接口函数3 空 用于Release版本
static void CLoglevelout(int nshowlevel,LPCSTR lpFormat, ...)
{
}
#endif //_LOGLEVEL

#ifdef _DEBUGCONSOLE
//关闭控制台接口函数4 空 用于Release版本
static void CLogconsole_close()
{
}

static void CLogcconsole_win(BOOL pSHWinConsole = FALSE)
{
}
#endif //_DEBUGCONSOLE

#endif //_FLAG_OUTLOG_ENABLE

#endif //!defined(AFX_LOGFILE_H__EF4BC4B2_3BB6_46E8_B936_0F3A61E20BF0__INCLUDED_)
