#include "StdAfx.h"
#include "WebCommon.h"

BOOL			_bAppShutDown = FALSE;

HWND GetContainerWindow( IWebBrowser2* pWeb2 )
{
	if( NULL == pWeb2 ) return NULL;
	
	IOleWindowPtr pWindow;
	if(SUCCEEDED(pWeb2->QueryInterface(IID_IOleWindow,(void**)&pWindow)))
	{
		HWND wnd=NULL;
		pWindow->GetWindow(&wnd);
		return wnd;
	}
	
	IOleInPlaceSitePtr pIPSite=pWeb2;
	
	IOleInPlaceFramePtr pFrame;
	IOleInPlaceUIWindowPtr pDoc;
	RECT rcPosRect,rcClipRect;
	OLEINPLACEFRAMEINFO FrameInfo;
	FrameInfo.cb=sizeof(OLEINPLACEFRAMEINFO);
	
	if( pIPSite )
	{		
		if( SUCCEEDED(pIPSite->GetWindowContext(&pFrame,&pDoc,&rcPosRect,&rcClipRect,&FrameInfo)))
		{
			HWND hwnd=NULL;
			if(pDoc)
				pDoc->GetWindow(&hwnd);
			
			if(!hwnd)
			{
				if(pFrame)
					pFrame->GetWindow(&hwnd);
			}
			
			return hwnd;
		}
	}
	
	return NULL;
}

HMENU GetIEEncodeMenu( IUnknown *pcmdTarget )
{
#define SHDVID_GETMIMECSETMENU   27
#define SHDVID_ADDMENUEXTENSIONS 53
#define IDM_LANGUAGE 1400
	
	HRESULT hr;
	CComPtr<IOleCommandTarget> spCT;
	CComVariant var;
	
	// Get the language submenu
	HMENU hMenu = NULL;
	hr = pcmdTarget->QueryInterface(IID_IOleCommandTarget, (void**)&spCT);
	if( spCT )
	{
		hr = spCT->Exec(&CGID_ShellDocView, SHDVID_GETMIMECSETMENU, OLECMDEXECOPT_DODEFAULT, NULL, &var);
		hMenu = (HMENU) var.byref;
	}
	return hMenu;
}

HRESULT GetTopBrowser( IWebBrowser2* pWeb2, IWebBrowser2** ppTopWeb2 )
{
	assert( pWeb2 );
	HRESULT hr = S_FALSE;
	if( !pWeb2 )
		return hr;
	
	ULONG lRef1 = pWeb2->AddRef();
	
	while( 1 )
	{
		IDispatchPtr pDispPtr = NULL;
		if( pWeb2 )
		{
			VARIANT_BOOL bTop;
			pWeb2->get_TopLevelContainer( &bTop );
			if( VARIANT_TRUE == bTop )
			{
				break;
			}
		}
		else
			break;
		pWeb2->get_Container( &pDispPtr );
		if( pDispPtr )
		{
			IServiceProviderPtr pIspPtr = NULL;	
			if( pDispPtr )
			{
				pDispPtr->QueryInterface( IID_IServiceProvider, (VOID**)&pIspPtr );				
				if( pIspPtr )
				{
					if( pWeb2 )
						pWeb2->Release();
					IWebBrowser2* pWeb2Old = pWeb2;
					pWeb2 = NULL;
					pIspPtr->QueryService(IID_IWebBrowserApp,IID_IWebBrowser2, (void**)&pWeb2 );
					if( pWeb2 == pWeb2Old )
						break;
				}
			}
		}
		else
			break;
	}
	*ppTopWeb2 = pWeb2;
	if( pWeb2 )
		hr = S_OK;
	
	return hr;
}

HRESULT GetBrowserFromDoc( IDispatch* pDispDoc, IWebBrowser2** ppWeb2 )
{
	HRESULT hr = NULL;
	IServiceProviderPtr pIspPtr = NULL;
	hr = pDispDoc->QueryInterface( IID_IServiceProvider, (VOID**)&pIspPtr );
	if( pIspPtr )
	{
		hr = pIspPtr->QueryService(IID_IWebBrowserApp,IID_IWebBrowser2, (void**)ppWeb2);
	}
	return hr;
}
