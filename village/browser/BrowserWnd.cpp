#include "stdafx.h"
#include "BrowserWnd.h"
#include "WebBrowser.h"
#include <process.h>
#include <tlogstg.h> // IE 5.5 
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
//#include "../Util/UtilApi.h"

#define PROP_BROWSERWNDPTR _T("BROWSER_PTR")

#define HINT_HEIGHT	26

extern BOOL _bIE7;
extern BOOL _bIE8;
extern BOOL			_bAppShutDown;


HWND _hWndAutoDropFocus = NULL;
BOOL _bAutoReduceProcessMemory = TRUE;
TCHAR _szKeyCLName[KL_NAMELENGTH] = {0};//_T("E00E0804");



CBrowserWnd::CBrowserWnd() : m_HintWnd(this)
{
	Clear();
}

void CBrowserWnd::Clear()
{
	m_bPauseCreateProcessGuard = FALSE;
	m_bNormalQuit = FALSE;//退出的时候浏览空白页以清理插件内存
	m_bBusy = FALSE;
	m_bAddressIsPathFile = FALSE;
	m_bIsAboutBlank = FALSE;
	m_bSafeModeBlocked = FALSE;
	m_bPauseSafeMode = FALSE;

	m_pointLBTNDown.x = -1;
	m_bCreateForNewWindow2 = FALSE;
	m_lMax = 0;
	m_lCurrent = 0;
	m_pOleObject = NULL;
	m_pOIPA = NULL;
	m_pWebBrowser = NULL;
	m_pDispatch = NULL;
	m_hWndBrowser = NULL;
	m_pWebBrowserThread = NULL;
	m_pBrowserEvent = NULL;
	m_bEventCanFree = TRUE;

	m_pStream = NULL;
	m_pStream2 = NULL;

	m_hThread = NULL;
	m_nThreadID = 0;

	m_bDestroyed = FALSE;

	m_pParentBrowser = NULL;
	m_pWebBrowserHost = NULL;
	m_hWndRoot = NULL;

	m_nThreadStyle = MULTI_THREAD;

	m_bBackEnable = FALSE;
	m_bForwardEnable = FALSE;
	m_uState = BS_HIDDEN;
	
	memset(m_wszZoomPercent, 0, sizeof(m_wszZoomPercent));
	wcscpy( m_wszZoomPercent, L"100%" );

	m_bInPrepareBlankPage = FALSE;
	m_nPrepareCount = 0;

	m_szHint[0] = _T('\0');
	m_pointLastLDown.x = -1;
	m_rcClose.left = m_rcClose.right = -1;

	m_bNeedUIActive = TRUE;


}

CBrowserWnd::~CBrowserWnd()
{
	Detach();
}

void CBrowserWnd::SetObject( CWebBrowser * pWebHost, CBrowserEvent * pEvent, CBrowserWnd * pParentBrowser, BOOL bEventCanFree )
{
	m_pBrowserEvent = pEvent;
	m_pWebBrowserHost = pWebHost;
	m_pParentBrowser = pParentBrowser;
	m_bEventCanFree = bEventCanFree;
}

BOOL CBrowserWnd::DoCreate()
{
	assert( m_pBrowserEvent != NULL );

	SetThreadInfo( TRUE );

/*#ifdef _UNICODE
	// 浏览器创建doverb的时候， 设置焦点或者不设焦点的操作会和搜狗拼音和搜狗五笔有一些冲突。 所以创建结束后再设置真正的父窗口
	m_hWnd = XWnd::Create( WS_EX_NOPARENTNOTIFY, BROWSERWND_NAME, 
		WS_CHILDWINDOW | 
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		CCS_NODIVIDER | CCS_NOPARENTALIGN |  CCS_NORESIZE, 
		&m_Rect, HWND_MESSAGE, m_wID );
#else*/
	/*m_hWnd = Create( WS_EX_NOPARENTNOTIFY, BROWSERWND_NAME, 
		WS_CHILDWINDOW | 
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		CCS_NODIVIDER | CCS_NOPARENTALIGN |  CCS_NORESIZE, 
		&m_Rect, m_hWndParent, m_wID )*/

	DWORD dwStyle = 0;
	if (m_bCreateForNewWindow2)
	{
		dwStyle = WS_TILEDWINDOW;
	}
	else
		dwStyle = WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN |  CCS_NORESIZE;

	m_hWnd =  CWindowImpl<CBrowserWnd>::Create(m_hWndParent, m_Rect, BROWSERWND_NAME, dwStyle, WS_EX_NOPARENTNOTIFY,m_wID);
//#endif

	SetProp( m_hWnd, PROP_BROWSERWNDPTR, (HANDLE)this );
	if( S_OK == OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, 0, m_pClientSite, m_pStorage, (void**)&m_pOleObject) )
	{
		m_pOleObject->SetHostNames(_T("TEST"), 0 );
		m_pOleObject->SetClientSite(m_pClientSite); 
		OleSetContainedObject( m_pOleObject, TRUE );

		m_pOleObject->DoVerb( OLEIVERB_SHOW, NULL, (IOleClientSite *)m_pClientSite, -1, m_hWnd, &m_Rect );
		
		m_pOleObject->QueryInterface( IID_IOleInPlaceActiveObject, (void**)&m_pOIPA );
		assert( m_pOIPA != NULL );

		m_pOleObject->QueryInterface( IID_IWebBrowser2, (void**)&m_pWebBrowser );
		assert( m_pWebBrowser != NULL );

		//m_pWebBrowser->put_RegisterAsDropTarget( VARIANT_TRUE );
		m_pWebBrowser->QueryInterface( IID_IDispatch, (void **)&m_pDispatch );		

		if( MULTI_THREAD == m_nThreadStyle )
		{
			if( m_bCreateForNewWindow2 )
				::CoMarshalInterThreadInterfaceInStream(IID_IDispatch, m_pDispatch, &m_pStream ); // release by other thread's newwindow2

			::CoMarshalInterThreadInterfaceInStream(IID_IWebBrowser2, m_pWebBrowser, &m_pStream2 );
		}

		assert( m_pBrowserEvent != NULL );
		m_pBrowserEvent->Connect( this );

		m_hWndRoot = GetAncestor( m_hWndParent, GA_ROOT );
		m_pWebBrowser->put_RegisterAsBrowser( VARIANT_TRUE );

		UpdateBrowserRect( m_Rect.right-m_Rect.left, m_Rect.bottom - m_Rect.top );
	}

	if( m_pDispatch )
	{
		return TRUE;
	}
	else
	{
		Destroy();
		m_hWnd = NULL;
		return FALSE;
	}
}

void CBrowserWnd::DoDestroy()
{
	m_bDestroyed = TRUE;

	RELEASE_TRY
	{
		if( m_pDispatch )
			m_pDispatch->Release();

		if( m_pWebBrowser )
		{
			m_pWebBrowser->put_RegisterAsBrowser( VARIANT_FALSE );
			if( !m_bNormalQuit && !_bAppShutDown )
			{
				m_pBrowserEvent->m_bCanSuspend = FALSE;
				RELEASE_TRY
					Stop();
				RELEASE_CATCH
				try
				{
					if( 1 )
					{
						BOOL bNaviRet = InternalNavigate( _T("about:blank"), FALSE );						
						DWORD dwTickCount = 0;

// 						if( bNaviRet )
// 						{
// 							do
// 							{
// 								MSG msg;
// 								if( ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
// 								{
// 									::TranslateMessage(&msg);
// 									::DispatchMessage(&msg);
// 								}
// 								else
// 									Sleep( 1 );
// 								
// 								dwTickCount ++;
// 								if( dwTickCount > 7777 )
// 									break;
// 								
// 							}while( m_pBrowserEvent->m_bCanSuspend == FALSE );
// 						}
					}
				}
				catch( ... )
				{
					assert( 0 );
				}
			}

			//_ThreadGuard.AddThread( GetCurrentThreadId(), m_hThread, 4000, ThreadGuardBrowserCallBack );

			if( m_pBrowserEvent )
				m_pBrowserEvent->OnDestroy();
			
			m_pWebBrowser->Release();
		}
		if( m_pOIPA )
			m_pOIPA->Release();

		m_pBrowserEvent->DisConnect();
		if( m_pOleObject )
		{
			OleSetContainedObject( m_pOleObject, FALSE );

			// 如果浏览器还没有创建到ax控件那层，则SetClientSite会崩溃
			//if( m_hWndBrowser )
			{
				try
				{
					m_pOleObject->SetClientSite(NULL); //?????
				}
				catch( ... )
				{
					assert( 0 );
				}

			}
// 			MSG msg;
// 			while( ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
// 			{
// 				::TranslateMessage(&msg);
// 				::DispatchMessage(&msg);
// 			}

			try
			{
				m_pOleObject->Close( OLECLOSE_NOSAVE );
			}
			catch( ... )
			{
				assert( 0 );
			}
		


			RELEASE_TRY
			{
				long l = m_pOleObject->Release();
			}
			RELEASE_CATCH
		}
	}
	RELEASE_CATCH
	
	DestroyWindow();
	assert(m_hWnd != NULL);
	SetThreadInfo( FALSE );
}

unsigned int __stdcall CBrowserWnd::BrowserThreadProc( void * pArgu )
{
	CBrowserWnd * pThis = (CBrowserWnd *)pArgu;
	HANDLE hThread = NULL;
	DWORD dwID = GetCurrentThreadId();
	if ( pThis )
	{
		OleInitialize(0);
		
		if( pThis->DoCreate() )
		{
			MSG msg;
			DWORD dwPID = GetCurrentProcessId();
			DWORD dwWID = dwID;
			
			int nRoundTime = 0;
ROUND:
			try
			{
				pThis->m_nTimerCount = 0;
				while( GetMessage(&msg, NULL, 0, 0) ) 
				{
					if( !pThis->m_bDestroyed )
					{
						{
							if( msg.message == WM_KEYDOWN  )
							{
								if( msg.wParam == VK_F5 )
								{
									pThis->OnRefresh();
								}
							}

							if( S_OK == pThis->m_pOIPA->TranslateAccelerator( &msg ) )
								continue; 
						}
					}

					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
			
			catch(...)
//			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				nRoundTime++;
				if( nRoundTime < 3 )
				{
					// 如果发生了3次异常，则退出
					if( ::IsWindow(pThis->m_hWnd) ) 
						goto ROUND;
				}
		
				goto ROUND;
			}
			
			hThread = pThis->m_hThread;
			//pThis->DoDestroy();
		}

//		__try{
		try{
			OleUninitialize();
		}//__except( EXCEPTION_CONTINUE_EXECUTION ){}
		catch(...){};
		
		//CloseHandle( pThis->m_hThread );
		//pThis->m_hThread = NULL;
		delete pThis->m_pBrowserEvent;
	
		//if( !_bAppShutDown )
		delete pThis;

	}

	return 0;
}

HWND CBrowserWnd::Create( HWND hWndParent, WORD wID, LPRECT lpRect, IOleClientSite * pClientSite, IStorage * pStorage, int nThreadStyle )
{
	m_nThreadStyle = nThreadStyle;
	m_hWndParent = hWndParent;
	m_wID = wID;
	CopyRect( &m_Rect, lpRect );
	m_pClientSite = pClientSite;
	m_pStorage = pStorage;

	//ValidateThreadModel( m_hWndParent );

	if( m_nThreadStyle == SINGLE_THREAD )
	{
		DoCreate();
	}
	else
	{
		DWORD dwThreadCode;
		DWORD dwTime = 0;
		DWORD dwRetryCount = 0;
RETRY_CREATE:
		m_hThread = (HANDLE)_beginthreadex( NULL, 0, BrowserThreadProc, this, 0, &m_nThreadID );

		while( m_hWndRoot == NULL )
		{
			MSG msg;
			if( ::PeekMessage(&msg, NULL, WM_PARENTNOTIFY, WM_PARENTNOTIFY, PM_REMOVE) ) 
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			else
			{
				Sleep(1);
				dwTime++;
				if( dwTime > 7000 )
				{
					assert( 0 );
					TerminateThread( m_hThread, 0 );
					Clear();

					dwRetryCount++;
					if( dwRetryCount >= 2 )
						return NULL;

					dwTime = 0;
					goto RETRY_CREATE;
				}
			}

			if( !GetExitCodeThread(m_hThread,&dwThreadCode) || dwThreadCode!=STILL_ACTIVE )
				break; //线程错误、退出
		
		}

		if( m_pStream2 )
		{
			::CoGetInterfaceAndReleaseStream( m_pStream2, IID_IWebBrowser2, (void**)&m_pWebBrowserThread );
			m_pStream2 = NULL;
		}

	}
	return m_hWnd;
}

void CBrowserWnd::Destroy()
{
	if( m_nThreadStyle == SINGLE_THREAD )
	{
		DoDestroy();
		if( m_bEventCanFree )
			if( m_pBrowserEvent )
				delete m_pBrowserEvent;
	}
	else
	{
		if( m_pWebBrowserThread )
			m_pWebBrowserThread->Release();

		m_bDestroyed = TRUE;
		//PostThreadMessage( m_nThreadID, WM_QUIT, 0, 0 );
		HANDLE hThread = m_hThread;
		::PostMessage( m_hWnd, WM_BROWSER_QUIT, 1, 0 );

		//等待浏览器线程2秒，超时强制关闭
		if(WaitForSingleObject(hThread, 2000) == WAIT_TIMEOUT)
		{
			TerminateThread(hThread, -1);
		}
// 		CloseHandle(m_hThread);
// 		m_hThread = NULL;
		
		//PostThreadMessage( m_nThreadID, WM_BROWSER_QUIT, 0, 0 );
	}
}

LRESULT CBrowserWnd::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_nThreadStyle != SINGLE_THREAD )
		::PostQuitMessage(0);
	if(  m_bDestroyed == FALSE )
	{
		//				assert( 0 == m_pWebBrowserThread );//这个应该在主线程释放， 但没找到好时机， 所以暂时没释放
		m_bDestroyed = TRUE;
		DoDestroy( );
		SetThreadInfo( FALSE );
	}
	return 0;
}

LRESULT CBrowserWnd::OnBrowserQuit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoDestroy();
	return 0;
}

LRESULT CBrowserWnd::OnBrowserOp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bRet = OnBrowserWndOperation( wParam, lParam, bHandled );

	if (bHandled)
		return bRet;
	
	return 0;
}

LRESULT CBrowserWnd::OnBrowserActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoActive( wParam );
	return 0;
}

LRESULT CBrowserWnd::OnBrowserFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetBrowserFocus( wParam );
	return 0;
}

LRESULT CBrowserWnd::OnBrowserMsgSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	OnBrowserSize( wParam, lParam );
	return 0;
}

LRESULT CBrowserWnd::OnBrowserFunThread(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return Browser_FuncThread( wParam, lParam );
}

LRESULT CBrowserWnd::OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( (LOWORD(wParam) == WM_DESTROY) && !m_bDestroyed ) 	
	{
		return 0;
	}
	else if( (LOWORD(wParam) == WM_LBUTTONDOWN) ) 	
	{
		DWORD dwPos = GetMessagePos();
		m_pointLastLDown.x = GET_X_LPARAM(dwPos);
		m_pointLastLDown.y = GET_Y_LPARAM(dwPos);
		return 0;
	}

	return 0;
}

LRESULT CBrowserWnd::OnBrowserDestroyed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CBrowserWnd::OnSizeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam & SIZE_MINIMIZED )
		return 0;
	OnSize( lParam );
	return 0;
}

LRESULT CBrowserWnd::OnBrowserCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return OnCommand( wParam, lParam, bHandled, TRUE );
}

LRESULT CBrowserWnd::OnBrowserEventMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	assert( m_pBrowserEvent );
	m_pBrowserEvent->BrowserEventMsg( wParam, lParam );
	return 0;
}

LRESULT CBrowserWnd::OnNcDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RemoveProp( m_hWnd, PROP_BROWSERWNDPTR );
	return 0;
}

LRESULT CBrowserWnd::OnBrowserNaviAdvance(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( 1234 != lParam )
		return 0;
	LPBROWSERWNDNAVIDATA pData = (LPBROWSERWNDNAVIDATA)wParam;
	if( pData )
	{
		if( pData->URL.vt == VT_BSTR && pData->URL.bstrVal )
		{
			IWebBrowser2 * pWebBrowser = GetBrowser();
			pWebBrowser->Navigate2( &pData->URL, &pData->Flags, &pData->TargetFrameName, &pData->PostData, &pData->Headers );
		}
		delete pData;
	}

	return 0;
}

LRESULT CBrowserWnd::OnBrowserNavi(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam )
	{
		LPTSTR pszUrl = (LPTSTR)wParam;
		InternalNavigate( pszUrl, (BOOL)lParam );
		delete []pszUrl;
		return 0;
	}

	return 0;
}

LRESULT CBrowserWnd::OnCommand( WPARAM wParam, LPARAM lParam, BOOL &bHandled, BOOL bChained )
{
	WORD wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	bHandled = TRUE;

	/*
	switch( wmId )
	{
	case ID_BACKWARD:
		Back();
		break;

	case ID_FORWARD:
		Forward();
		break;

	//case ID_STOP:
		Stop();
		break;

	case ID_REFRESH:
		Refresh();
		break;

	case ID_HOME:
		Home();
		break;

	// 当子线程收到Command的时候，转发给主线程
	default:
		return 0;
	}
    */
	return 1;

}

void CBrowserWnd::OnSize( LPARAM lParam )
{
	UpdateBrowserRect( LOWORD(lParam), HIWORD(lParam) );
}

void CBrowserWnd::DoActive( UINT uState )
{
#ifdef _DEBUG
	DWORD dwThreadID = GetCurrentThreadId();
	DWORD dwWndThreadID = GetWindowThreadProcessId( m_hWnd, NULL );
	assert( dwThreadID == dwWndThreadID );
#endif

	m_uState = uState;
	m_pointLastLDown.x = -1;

	switch( uState )
	{
	case BS_ACTIVE:
		
		m_pWebBrowserHost->Active( this );
		::SetWindowPos(m_hWnd, m_pWebBrowserHost->m_hTopMostWnd, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_DEFERERASE|SWP_SHOWWINDOW);
		InitObject();
		::PostMessage( m_hWnd, WM_BROWSER_FOCUS, 1, 0 );
		break;
	}
}


void CBrowserWnd::Active( UINT uState )
{
	//PostMessage( m_hWnd, WM_BROWSER_ACTIVE, uState, 0 );
	DWORD dwResult;
	if( !SendMessageTimeout( m_hWnd, WM_BROWSER_ACTIVE, uState, 0, SMTO_ABORTIFHUNG, 6000, &dwResult ) )
	{
		m_uState = uState;
	}
}

void CBrowserWnd::SetProgress(long lMax, long lCurrent)
{
//	::PostMessage(global._hMainWnd, WEBBROWSE_PROGRESS_CHANGE, lMax, lCurrent);
}

void CBrowserWnd::ChangeBrowserSize( LPRECT lpRect )
{
	IOleInPlaceObject * pObject;
	if( m_pOleObject )
	{
		if( S_OK == m_pOleObject->QueryInterface( IID_IOleInPlaceObject, (void**)&pObject ) )
		{
			pObject->SetObjectRects( lpRect, lpRect );
			pObject->Release();
		}
	}
}

void CBrowserWnd::Navigate( VARIANT *URL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers)
{
	if( NULL == URL || NULL == URL->bstrVal )
		return;

	LPBROWSERWNDNAVIDATA pData = new BROWSERWNDNAVIDATA;
	memset( pData, 0, sizeof(BROWSERWNDNAVIDATA) );
	pData->URL.bstrVal = ::SysAllocString( URL->bstrVal );
	pData->URL.vt = VT_BSTR;
	memcpy( &pData->Flags, Flags, sizeof(VARIANT) );

	if( VT_BSTR == TargetFrameName->vt && TargetFrameName->bstrVal )
	{
		memcpy( &pData->TargetFrameName, TargetFrameName, sizeof(VARIANT) );
		pData->TargetFrameName.bstrVal = ::SysAllocString( TargetFrameName->bstrVal );
	}
	if( VT_BSTR == Headers->vt && Headers->bstrVal )
	{
		memcpy( &pData->Headers, Headers, sizeof(VARIANT) );
		pData->Headers.bstrVal = ::SysAllocString( Headers->bstrVal );
	}

	::PostMessage( m_hWnd, WM_BROWSER_NAVIADVANCE, (WPARAM)pData, (LPARAM)1234 );
}

void CBrowserWnd::Navigate( LPCTSTR pszUrlPage, BOOL bNew )
{
	int nLen = _tcslen(pszUrlPage);
	if( nLen == 0 )
		return ;
	nLen++;

	LPTSTR pszIUrl = new TCHAR[nLen];
	if( pszIUrl )
	{
		_tcscpy( pszIUrl, pszUrlPage );
		
		::PostMessage( m_hWnd, WM_BROWSER_NAVI, (WPARAM)pszIUrl, (LPARAM)bNew );
	}
}


BOOL CBrowserWnd::InternalNavigate( LPCTSTR pszUrlPage, BOOL bNew )
{
	IWebBrowser2 * pWebBrowser = GetBrowser();
	if( pWebBrowser )
	{
		if( _tcslen(pszUrlPage) == 0 )
			pszUrlPage = ABOUTBLANK;

		//if( IsScriptUrl( pszUrlPage ) )
		//	bNew = FALSE;

		LPWSTR pszUrl;
#ifdef _UNICODE
		pszUrl = (LPWSTR)pszUrlPage;
#else
		WCHAR wszUrlPage[MAX_URL_LEN];
		memset( wszUrlPage,0,sizeof(wszUrlPage) );
		MultiByteToWideChar(CP_ACP, 0, pszUrlPage, -1, wszUrlPage, sizeof(wszUrlPage)/sizeof(WCHAR));
		pszUrl = wszUrlPage;
#endif

		VARIANT varUrl;
		VARIANT vtEmpty;
		VARIANT vtFlags;
		vtEmpty.vt = VT_EMPTY; 
		vtFlags.vt = VT_I4;
		vtFlags.lVal = navOpenInNewWindow;
		varUrl.vt = VT_BSTR;
		
		 // 打开一个目录
		HRESULT hr = S_FALSE;
		if ( hr != S_OK )
		{
			varUrl.bstrVal = SysAllocString(pszUrl);
//			m_bNeedAddAgent = TRUE;
			hr = pWebBrowser->Navigate2(&varUrl, bNew?&vtFlags:&vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty );
//			m_bNeedAddAgent = FALSE;
			SysFreeString( varUrl.bstrVal );
		}

		m_bBusy = FALSE;
		return hr == S_OK;
	}
	m_bBusy = FALSE;
	return FALSE;
}

void CBrowserWnd::QueueBack( int nStep )
{
	for( int i=0;i<nStep;i++ )
		GetBrowser()->GoBack();
}

void CBrowserWnd::QueueForward( int nStep )
{
	for( int i=0;i<nStep;i++ )
		GetBrowser()->GoForward();
}

BOOL CBrowserWnd::QueuePopupMenu( LPRECT lpRect )
{
	ITravelLogStg * pITravelLog = NULL;
	IServiceProvider* pSP;
	DWORD flag;
	HMENU hMenu = NULL;
	
	USES_CONVERSION;
	int nStepMe = -1;
	BOOL bHasMe = FALSE;
	
	
	if(m_pWebBrowser!=NULL)
	{
		hMenu = CreatePopupMenu();
		{
			HRESULT hr = m_pWebBrowser->QueryInterface(IID_IServiceProvider, (void**)&pSP);
			if(SUCCEEDED(hr) && pSP!=NULL) 
			{
				hr = pSP->QueryService(SID_STravelLogCursor , IID_ITravelLogStg, (void**)&pITravelLog);
				if(SUCCEEDED(hr) && pITravelLog )
				{
					DWORD nC = 0;
					pITravelLog->GetCount(TLEF_RELATIVE_BACK| TLEF_RELATIVE_FORE,&nC);
					// enum travellog into menu
					IEnumTravelLogEntry * pEnum;
					ITravelLogEntry* pEntry;
					LPOLESTR pszTitle;
					
					long nStep = 0;
					ULONG celt;
					
					for( int i=0;i<2;i++ )
					{
						if( i == 0 )
						{
							flag = TLEF_RELATIVE_BACK;
						}
						else
						{
							nStepMe = nStep+1;
							BSTR name = NULL;
							m_pWebBrowser->get_LocationName( &name );
							if( name )
							{
								InsertMenu( hMenu, nStep, MF_BYPOSITION|MF_STRING, nStep+1, LPCTSTR(OLE2T(name)) );
								SysFreeString( name );
								
								bHasMe = TRUE;
								MENUITEMINFO itemInfo;
								memset( &itemInfo, 0, sizeof( MENUITEMINFO ) );
								itemInfo.cbSize = sizeof( MENUITEMINFO );
								itemInfo.fMask = MIIM_STATE;
								itemInfo.fState = MFS_DEFAULT;
								SetMenuItemInfo( hMenu, nStep, TRUE, &itemInfo );
								nStep++;
							}
							flag = TLEF_RELATIVE_FORE;
						}
						
						pEnum = NULL;
						if( pITravelLog->EnumEntries(flag, &pEnum) == S_OK )
						{
							pEnum->Reset();
							HRESULT hr;
							while(hr=pEnum->Next(1, &pEntry, &celt)==S_OK && nStep<9)
							{
								pEntry->GetTitle(&pszTitle);
								if( i == 0 )
									InsertMenu( hMenu, 0, MF_BYPOSITION|MF_STRING, nStep+1, LPCTSTR(OLE2T(pszTitle)) );
								else
									InsertMenu( hMenu, nStep, MF_BYPOSITION|MF_STRING, nStep+1, LPCTSTR(OLE2T(pszTitle)) );
								nStep++;
								CoTaskMemFree(pszTitle);
								pEntry->Release();
							}
							pEnum->Release();
						}
					}
					
					if( GetMenuItemCount( hMenu ) > 1 )
					{
						TPMPARAMS tpm;
						tpm.cbSize = sizeof(TPMPARAMS);
						//_OwnerDrawMenu.UpdateAfterInsert( hMenu );
						
						int cmd = TrackPopupMenu( hMenu, TPM_RETURNCMD|TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL, lpRect->left, lpRect->bottom, 0, m_hWnd, NULL );
						
						if( cmd > 0 )
						{
							//cmd;
							ITravelLogEntry * ptle = NULL;
							pITravelLog->GetRelativeEntry( (cmd<nStepMe)?-cmd:cmd-nStepMe, &ptle );
							if( ptle )
							{
								pITravelLog->TravelTo(ptle);
								ptle->Release();
							}
						}
					}
					DestroyMenu( hMenu );
					
					pITravelLog->Release();
				}
				pSP->Release();
			}
		}	
	}
	return TRUE;
}

CBrowserEvent* CBrowserWnd::GetBrowserEvent( )
{
	return m_pBrowserEvent;
}

IDispatch * CBrowserWnd::OnNewWindow2()
{
	IDispatch * pDisp = NULL;
	DWORD dwResult=0;
	if( !SendMessageTimeout( m_hWndParent, WM_BROWSER_CREATEREQUEST, 0, (LPARAM)this, SMTO_ABORTIFHUNG, 6000, &dwResult ) )
		return NULL;

	LPVOID lpVoid = (LPVOID)dwResult;
	if( lpVoid )
	{
		if( m_nThreadStyle == SINGLE_THREAD )
		{
			pDisp = (IDispatch *)lpVoid;
			pDisp->AddRef();
		}
		else
		{
			::CoGetInterfaceAndReleaseStream( (LPSTREAM)lpVoid, IID_IDispatch, (void**)&pDisp);
		}
	}
	return pDisp;
}

void CBrowserWnd::InitObject()
{
	// 不能单纯根据有wnd决定不再查找，因为有可能本控件内的ax window已经不是IE，是Explorer View
	if( m_hWndBrowser && ::IsWindow(m_hWndBrowser) )
		return;

	HWND hNowBrowser;
	HWND hWnd = FindWindowEx( m_hWnd, NULL, _T("Shell Embedding"), NULL );
	if( hWnd )
	{
		hWnd = FindWindowEx( hWnd, NULL, _T("Shell DocObject View"), NULL );
		if( hWnd )
		{
			hNowBrowser = FindWindowEx( hWnd, NULL, _T("Internet Explorer_Server"), NULL );
			if( !hNowBrowser )
				hNowBrowser = FindWindowEx( hWnd, NULL, _T("SysListView32"), NULL );
			if( hNowBrowser && m_hWndBrowser != hNowBrowser )
			{
				m_hWndBrowser = hNowBrowser;
				
				if( m_pWebBrowserHost && m_pWebBrowserHost->m_bEnableDragDrop )
				{
					RevokeDragDrop( m_hWndBrowser );
					//m_DropTarget.Init( TRUE );
					//m_DropTarget.Register( m_hWndBrowser, (LPARAM)this, BrowserDropProc );
				}
				if( m_pWebBrowserHost->m_bMutiThreadIMM && m_pWebBrowserHost->IsMultiThread() )
				{
					//if( ::_bIsWindowsVista )
					//	SetKLO();
					//else
					::PostMessage( m_hWnd, WM_BROWSER_SETKLO, 1, 2 );
				}

			}
		}
	}
}
void CBrowserWnd::Forward()
{
	RELEASE_TRY
	{
		Stop();
		QueueForward();
	}
	RELEASE_CATCH
}

void CBrowserWnd::Back()
{
	RELEASE_TRY
	{
		Stop();
		QueueBack();
	}
	RELEASE_CATCH
}

BOOL CBrowserWnd::Home( BOOL bIgnoreBlank )
{
	//Navigate(_arrBankUrls[0]);
	return TRUE;
}

void CBrowserWnd::Stop_SafeThread()
{
	m_pWebBrowser->Stop();
}

void CBrowserWnd::Stop()
{
	if( m_nThreadID == 0 || GetCurrentThreadId() == m_nThreadID )
		Stop_SafeThread();
	else
		::PostMessage( m_hWnd, WM_BROWSER_FUNCTHREAD, 0, MAKEWPARAM(TB_FUNC_STOP,2008) );
}

void CBrowserWnd::Refresh_SafeThread()
{
	OnRefresh();
	m_pWebBrowser->Refresh();
}

void CBrowserWnd::Refresh()
{
	if( m_nThreadID == 0 || GetCurrentThreadId() == m_nThreadID )
		Refresh_SafeThread();
	else
		::PostMessage( m_hWnd, WM_BROWSER_FUNCTHREAD, 0, MAKEWPARAM(TB_FUNC_REFRESH,2008) );
}

void CBrowserWnd::RefreshCompletely_SafeThread()
{
	OnRefresh();
	VARIANT var;
	var.vt = VT_I4;
	var.lVal = REFRESH_COMPLETELY;
	m_pWebBrowser->Refresh2( &var );
}

void CBrowserWnd::RefreshCompletely()
{
	if( m_nThreadID == 0 || GetCurrentThreadId() == m_nThreadID )
	{
		RefreshCompletely_SafeThread();
	}
	else
		::PostMessage( m_hWnd, WM_BROWSER_FUNCTHREAD, 0, MAKEWPARAM(TB_FUNC_REFRESH2,2008) );
}

IWebBrowser2 * CBrowserWnd::GetBrowser()
{
	assert( m_nThreadID == 0 || GetCurrentThreadId() == m_nThreadID );
	return m_pWebBrowser;
}

IWebBrowser2 * CBrowserWnd::GetSafeBrowser()
{
#ifdef _DEBUG
	DWORD dwTID = GetCurrentThreadId();
	HWND hWndRoot = GetAncestor( m_hWnd, GA_ROOT );
	if( dwTID == 0 || (dwTID != m_nThreadID && dwTID != GetWindowThreadProcessId( hWndRoot, NULL )) )
		assert( FALSE );
#endif

	if( m_nThreadID == 0 || GetCurrentThreadId() == m_nThreadID )
		return m_pWebBrowser;
	else
		return m_pWebBrowserThread;
}

IWebBrowser2 * CBrowserWnd::GetSafeThreadBrowser()//must release
{
	if( m_nThreadID == 0 || GetCurrentThreadId() == m_nThreadID )
	{
		m_pWebBrowser->AddRef();
		return m_pWebBrowser;
	}
	else
	{
		BROWSERWNDOPERATION fo;
		memset( &fo, 0, sizeof(BROWSERWNDOPERATION) );
		fo.cbSize = sizeof(BROWSERWNDOPERATION);
		fo.uID = BO_GETSAFETHREADBROWSER;
		SendMessage( m_hWnd, WM_BROWSERWND_OPERATION, (WPARAM)&fo, NULL ); // Check - pass
		IStream * pStream = (IStream *)fo.lRet;
		if( pStream )
		{
			IWebBrowser2* pWeb2 = NULL;
			::CoGetInterfaceAndReleaseStream( pStream, IID_IWebBrowser2, (void**)&pWeb2 );
			return pWeb2;
		}
	}
	return NULL;
}

BOOL CBrowserWnd::IsShown()
{
	return m_uState == BS_ACTIVE||m_uState == BS_SHOWN;
}

BOOL CBrowserWnd::IsActivate()
{
	return m_uState == BS_ACTIVE;
}

CBrowserWnd * CBrowserWnd::GetBrowserWndPtr(IWebBrowser2 *pWeb)
{
	HWND hWnd = GetContainerWindow( pWeb );
	if( hWnd )
	{
		hWnd = ::GetParent( hWnd );
	}
	return GetBrowserWndPtr( hWnd );
}

CBrowserWnd * CBrowserWnd::GetBrowserWndPtr( HWND hWnd )
{
	if( hWnd )
		return (CBrowserWnd *)GetProp( hWnd, PROP_BROWSERWNDPTR );
	else
		return NULL;
}

void CBrowserWnd::Close( )
{
	if( m_bDestroyed )
		return;
}

void CBrowserWnd::SetCreateForNewWindow2(BOOL bNewWindow2)
{
	m_bCreateForNewWindow2 = bNewWindow2;
}

LPVOID CBrowserWnd::GetNewWindowObject()
{
	LPVOID lpVoid;
	if( m_nThreadStyle == SINGLE_THREAD )
	{
		lpVoid = (LPVOID)m_pDispatch;
	}
	else
	{
		lpVoid = (LPVOID)m_pStream;
		m_pStream = NULL;
	}
	return lpVoid;
}

void CBrowserWnd::UIActivate()
{
	if( m_bNeedUIActive )
	{
		m_bNeedUIActive = FALSE;
		m_pOleObject->DoVerb( OLEIVERB_UIACTIVATE, NULL, NULL, -1, NULL, NULL );
	}
}

void CBrowserWnd::SetBrowserFocus( BOOL bCondition )
{
	if( !m_hWndBrowser )
		InitObject();

	if( m_uState==BS_ACTIVE )
	{
		if( ::IsWindow(m_hWndBrowser) )
		{
			::SetFocus( m_hWndBrowser );
		}
	}
}

BOOL CBrowserWnd::IsRefferUrl(LPCTSTR pszUrl)
{

	if( !UrlIs( pszUrl, URLIS_OPAQUE ) )
		return TRUE;

	return FALSE;
}

LRESULT CBrowserWnd::OnBrowserWndOperation(WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LPBROWSERWNDOPERATION lpFO = (LPBROWSERWNDOPERATION)wParam;
	if( !lpFO || lpFO->cbSize != sizeof(BROWSERWNDOPERATION) )
		return 0;

	USES_CONVERSION;
	switch( lpFO->uID )
	{
	case BO_GETSAFETHREADBROWSER:
		{
			IStream * pStream = NULL;
			::CoMarshalInterThreadInterfaceInStream(IID_IWebBrowser2, m_pWebBrowser, &pStream );
			if( pStream )
			{
				bHandled = TRUE;
				lpFO->lRet = (LONG)pStream;
			}
		}
		break;
	}
	return 0;
}

HRESULT CBrowserWnd::Browser_FuncThread( WPARAM wParam, LPARAM lParam )
{
	if( 2008 != HIWORD(lParam) )
	{
		assert( 0 );
		return 0;
	}

	switch( LOWORD(lParam) )
	{
	case TB_FUNC_GETTRAVELCOUNT:
		return GetTravelCount_SafeThread( wParam );
		break;
	default:
		break;
	}
	RELEASE_TRY
	{
		switch( LOWORD(lParam) )
		{
		case TB_FUNC_STOP:
			Stop_SafeThread();
			break;
		case TB_FUNC_REFRESH:
			Refresh_SafeThread();
			break;
		case TB_FUNC_REFRESH2:
			RefreshCompletely_SafeThread();
			break;
		default:
			break;
		}		
	}
	RELEASE_CATCH
	return 0;
}

int CBrowserWnd::GetTravelCount(BOOL bBackward)
{
	if( m_nThreadID == 0 || GetCurrentThreadId() == m_nThreadID )
		return GetTravelCount_SafeThread( bBackward );
	else
		return SendMessage( m_hWnd, WM_BROWSER_FUNCTHREAD, bBackward, MAKEWPARAM(TB_FUNC_GETTRAVELCOUNT,2008) );
}

int CBrowserWnd::GetTravelCount_SafeThread(BOOL bBackward)
{
	ITravelLogStg * pITravelLog = NULL;
	IServiceProvider* pSP;
	IWebBrowser2 * pDisp = GetBrowser();

	DWORD  nCount = 0;	

	if(pDisp!=NULL)
	{
		HRESULT hr = pDisp->QueryInterface(IID_IServiceProvider, (void**)&pSP);
		if(SUCCEEDED(hr) && pSP!=NULL) 
		{
			hr = pSP->QueryService(SID_STravelLogCursor , IID_ITravelLogStg, (void**)&pITravelLog);
			if(SUCCEEDED(hr) && pITravelLog )
			{
				pITravelLog->GetCount( bBackward?TLEF_RELATIVE_BACK:TLEF_RELATIVE_FORE, &nCount );
				pITravelLog->Release();
			}
			pSP->Release();
		}
	}

	return nCount;
}

void CBrowserWnd::ShowTopHint(LPTSTR pszHint)
{
}

void CBrowserWnd::OnHintWndPaint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint( m_HintWnd, &ps );
	RECT rect;
	::GetClientRect( m_HintWnd, &rect );
	FillRect( hDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH) );

	::EndPaint( m_HintWnd, &ps );
}

void CBrowserWnd::SetThreadInfo( BOOL bSet )
{
	if( m_pWebBrowserHost && m_pWebBrowserHost->IsMultiThread() )
	{
		TlsSetValue( m_pWebBrowserHost->GetTlsIndex(), bSet?this:0 );
	}
}

void CBrowserWnd::GetBrowserRect(RECT *pRect)
{
	::GetClientRect( m_hWnd, pRect );

	pRect->top = 0;
	pRect->left = 0;
	if( m_szHint[0] != _T('\0') )
	{
		pRect->top += HINT_HEIGHT;
	}
	pRect->left -= WEBBROWSERFRAMEBORDERSIZE;
	pRect->top -= WEBBROWSERFRAMEBORDERSIZE;
	pRect->right += WEBBROWSERFRAMEBORDERSIZE;
	pRect->bottom += WEBBROWSERFRAMEBORDERSIZE;
}

void CBrowserWnd::GetCommandState(BOOL &bBack, BOOL &bForward)
{
	bBack = m_bBackEnable;
	bForward = m_bForwardEnable;
}

void CBrowserWnd::SetWindowPosSafe( int x, int y, int cx, int cy, UINT uFlags)
{
	BROWSERSIZE bs;
	bs.hWndInsertAfter = m_pWebBrowserHost->m_hTopMostWnd;
	bs.x = x;
	bs.y = y;
	bs.cx = cx;
	bs.cy = cy;
	bs.uFlags = uFlags;

	DWORD dwResult;
	SendMessageTimeout( m_hWnd, WM_BROWSER_SIZE, (WPARAM)&bs, 0, SMTO_ABORTIFHUNG, 6000, &dwResult );
}

void CBrowserWnd::OnBrowserSize( WPARAM wParam, LPARAM lParam )
{
	LPBROWSERSIZE lpBS = (LPBROWSERSIZE)wParam;
	::SetWindowPos( m_hWnd, lpBS->hWndInsertAfter, lpBS->x, lpBS->y, lpBS->cx, lpBS->cy, lpBS->uFlags );
//	ATLTRACE(_T("CBrowserWnd::OnBrowserSize %d"),bRt);
//		lpBS->x-WEBBROWSERFRAMEBORDERSIZE, lpBS->y-WEBBROWSERFRAMEBORDERSIZE, 
//		lpBS->cx+WEBBROWSERFRAMEBORDERSIZE*2, lpBS->cy+WEBBROWSERFRAMEBORDERSIZE*2, lpBS->uFlags );
}

LRESULT CBrowserWndHint::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_pOwnerBrowserWnd)
		m_pOwnerBrowserWnd->OnHintWndPaint();

	return 0;
}

LRESULT CBrowserWndHint::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostMessage( m_pOwnerBrowserWnd->m_hWnd, uMsg, wParam, lParam );
	return 0;
}

void CBrowserWnd::UpdateBrowserRect(int nWidth, int nHeight)
{
	if( ::IsWindow(m_hWnd) )
	{
		RECT rect = {0,0,nWidth, nHeight };

		if( m_szHint[0] != _T('\0') && m_HintWnd.m_hWnd )
		{
			::SetWindowPos( m_HintWnd, HWND_TOP, rect.left, rect.top, rect.right-rect.left, HINT_HEIGHT, SWP_NOMOVE|SWP_DEFERERASE|SWP_SHOWWINDOW|SWP_NOZORDER );
			::InvalidateRect( m_HintWnd, NULL, TRUE );
		}
		
		if( m_szHint[0] != _T('\0') )
		{
			rect.top += HINT_HEIGHT;
		}
		rect.left -= WEBBROWSERFRAMEBORDERSIZE;
		rect.top -= WEBBROWSERFRAMEBORDERSIZE;
		rect.right += WEBBROWSERFRAMEBORDERSIZE;
		rect.bottom += WEBBROWSERFRAMEBORDERSIZE;
		ChangeBrowserSize( &rect );
	}
}

BOOL CBrowserWnd::IsTopWebBrowser(IDispatch *pDisp)
{
    if( pDisp )
    {
        IWebBrowser2Ptr pWeb = NULL;
        pWeb = pDisp;
        if(pWeb)
        {
            if(pWeb == m_pWebBrowser)
                return TRUE;
        }
    }
    return FALSE;
}

BOOL CBrowserWnd::GetBrowserUrl(LPTSTR lpszUrl, DWORD dwSize)
{
    BOOL bRet = FALSE;
    BSTR bstrUrl=NULL;
    m_pWebBrowser->get_LocationURL( &bstrUrl );
    if( bstrUrl && bstrUrl[0] )
    {
        USES_CONVERSION;
        _sntprintf_s(lpszUrl, dwSize, dwSize-1, _T("%s"), OLE2T(bstrUrl));
        SysFreeString( bstrUrl );
        bRet = TRUE;
    }
    return bRet;
}

BOOL CBrowserWnd::GetBrowserTitle(LPTSTR lpszTitle, DWORD dwSize)
{
    BOOL bRet = FALSE;
    BSTR bstrTitle = NULL;
    m_pWebBrowser->get_LocationName( &bstrTitle );
    if( bstrTitle && bstrTitle[0] )
    {
        USES_CONVERSION;
        _sntprintf_s(lpszTitle, dwSize, dwSize-1, _T("%s"), OLE2T(bstrTitle));
        SysFreeString( bstrTitle );
        bRet = TRUE;
    }
    return bRet;
}


