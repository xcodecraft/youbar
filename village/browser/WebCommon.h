#pragma once

#include <WinInet.h>
#include <exdispid.h>
#include <ComDef.h>
#include <process.h>
#include <MsHtmdid.h>
#include <shellapi.h>
#include <malloc.h>
#include <memory.h>
#include <shlwapi.h>
#include <process.h>
#include <urlhist.h>
#include <shlguid.h>
#include <comdef.h>
#include <objbase.h>
#include <objidl.h>
#include <exdisp.h>
#include <exdispid.h>
#include <Mshtml.h>
#include <Mshtmdid.h>
#include <Wininet.h>
#include <Ddeml.h>
#include <urlmon.h>
#pragma comment (lib, "shlwapi.lib")

//////////////////////////////////////////////////////////////////////////
// Const
#ifndef MAX_URL_LEN
#define MAX_URL_LEN	INTERNET_MAX_URL_LENGTH
#endif

#ifndef _DEBUG
#define RELEASE_TRY try{
#else
#define RELEASE_TRY 
#endif

#ifndef _DEBUG
#define RELEASE_CATCH 	}catch( ... ){}
#else
#define RELEASE_CATCH 
#endif

#define SIZEOF(A) (sizeof(A)/sizeof((A)[0]))

extern BOOL			_bAppShutDown;
extern BOOL			_bIE8;
extern BOOL			_bIE7;

// function
HWND GetContainerWindow( IWebBrowser2* pWeb2 );
HMENU GetIEEncodeMenu( IUnknown *pcmdTarget );
HRESULT GetTopBrowser( IWebBrowser2* pWeb2, IWebBrowser2** ppTopWeb2 );
HRESULT GetBrowserFromDoc( IDispatch* pDispDoc, IWebBrowser2** ppWeb2 );

// webbrowser状态条改变通知主窗口
#define WEBBROWSE_COMMAND_STATE_NOTIFY		(WM_USER + 0x3000)
#define WEBBROWSE_PROGRESS_CHANGE			(WM_USER + 0x3001)
#define WEBBROWSE_URL_CHANGE                (WM_USER + 0x3002)
#define WEBBROWSE_DOCUMENT_COMPLETE			(WM_USER + 0x3003)
#define WEBBROWSE_TITLE_CHANGE				(WM_USER + 0x3004)

//
// 通知调用线程，可以处理UrlChange消息了
// wParam：HWND
// 
#define WM_NOTIFY_BANKMODE_INIT   (WM_APP + 0x2010)

//
// 调用线程发送过来的俄消息
// wParam: HWND, 弹出提示的父窗体
// lParam: LPCTSTR, Url地址, _tcsdup()分配, 需要free
//
#define WM_URL_CHANGE   (WM_APP + 0x2011)

//
// 通知调用线程刷新当前页面
// wParam,lParam = 0
//
#define WM_NOTIFY_REFRESH (WM_APP + 0x2012)
