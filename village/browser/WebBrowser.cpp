// WebBrowser.cpp: implementation of the CWebBrowser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WebBrowser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


extern BOOL			_bShouldExitApp;

CWebBrowser::CWebBrowser()
{
	//m_lpDownloadMgr = NULL;
	m_bEnableDragDrop = TRUE;
	m_bEnableWheelZoomPage = TRUE;
	m_bShowStatusTextLinkOnly = FALSE;

	m_pActiveBrowser = NULL;
	m_pCreatingBrowser = NULL;
	m_nThreadStyle = SINGLE_THREAD;
	m_bMutiThreadIMM = FALSE;
	m_bMessageFilter = FALSE;
	m_szMessageFilter[0] = _T('\0');
	m_lpfnCBW = NULL;
	m_lpfnCBE = NULL;
	m_dwTlsIndex = 0;
	m_bOptimizeMode = TRUE;
	m_bShowMessageDlg = TRUE;
	m_hTopMostWnd = HWND_TOP;

	m_hFrameWnd = NULL;
#ifdef _DEBUG
	m_bCSInited = FALSE;
#endif
}

CWebBrowser::~CWebBrowser()
{ 
}

void CWebBrowser::Init(int nThreadStyle, CWebBrowserEx * pBrowserEx, 
					   LPCREATEBROWSERWNDCALLBACK lpfnCBW, LPCREATEBROWSEREVENTCALLBACK lpfnCBE, BOOL bMutiThreadIMM,
					   HWND hFrameWnd )
{
#ifdef _DEBUG
	m_bCSInited = TRUE;
#endif
	m_lpfnCBW = lpfnCBW;
	m_lpfnCBE = lpfnCBE;
	m_nThreadStyle = nThreadStyle;
	m_bMutiThreadIMM = bMutiThreadIMM;

	m_hFrameWnd = hFrameWnd;
	if( m_nThreadStyle == SINGLE_THREAD )
		OleInitialize(NULL);

	InitializeCriticalSection( &m_CriticalSection );
	m_pBrowserEx = pBrowserEx;
	if( m_pBrowserEx )
		m_pBrowserEx->m_pParentWebBrowser = this;
	assert( m_pBrowserEx );

	InitTlsStorage( TRUE );
}

void CWebBrowser::UnInit()
{
	DeleteCriticalSection( &m_CriticalSection );
	if( m_nThreadStyle == SINGLE_THREAD )
		OleUninitialize();
	InitTlsStorage( FALSE );
}

BOOL CWebBrowser::InitTlsStorage( BOOL bInit )
{
	BOOL bRet = FALSE;
	if( bInit )
	{
		m_dwTlsIndex = TlsAlloc();
		if( m_dwTlsIndex != -1 )
			bRet = TRUE;
	}
	else
	{
		if( m_dwTlsIndex != -1 )
		{
			bRet = TlsFree( m_dwTlsIndex );
		}
	}
	return bRet;
}


//#include <Htiface.h>
STDMETHODIMP CWebBrowser::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	if( IsEqualIID(riid, IID_IUnknown) )
		*ppvObj = this;
	else if( IsEqualIID(riid, IID_IOleClientSite) )
		*ppvObj = (IOleClientSite *)this;
	else if( IsEqualIID(riid, IID_IOleInPlaceSite) )
		*ppvObj = (IOleInPlaceSite *)this;
	else if( IsEqualIID(riid, IID_IOleInPlaceFrame) )
		*ppvObj = (IOleInPlaceFrame *)this;
	else if( IsEqualIID(riid, IID_IOleCommandTarget) )
		*ppvObj = (IOleCommandTarget *)this;
	else if( IsEqualIID(riid, IID_IStorage) )
		*ppvObj = (IStorage *)this;

	else if( IsEqualIID(riid, IID_IServiceProvider) )
		*ppvObj = (IServiceProvider *)this;

	else if( IsEqualIID(riid, IID_IDocHostUIHandler) )
		*ppvObj = (IDocHostUIHandler *)m_pBrowserEx;
	else if( IsEqualIID(riid, IID_IDispatch) )
		*ppvObj = (IDispatch *)m_pBrowserEx;
	//else if( IsEqualIID(riid, IID_IDocHostShowUI) )
	//	*ppvObj = (IDispatch *)m_pBrowserEx;

	else
	{
		*ppvObj = 0;
		return E_NOINTERFACE;
	}

	return S_OK;
	
}

STDMETHODIMP_(ULONG)CWebBrowser::AddRef(THIS)
{
	return 1;
}

STDMETHODIMP_(ULONG)CWebBrowser::Release(THIS)
{
	return 1;
}

STDMETHODIMP CWebBrowser::GetContainer( IOleContainer **ppContainer )
{
	*ppContainer = 0;
	return E_NOINTERFACE;
}

STDMETHODIMP CWebBrowser::GetWindow( HWND *phwnd )
{
	assert( m_pCreatingBrowser );
	if( IsMultiThread() )
	{
		CBrowserWnd * pBrowser = GetBrowserWnd();
		if( pBrowser )
		{
			*phwnd = pBrowser->m_hWnd;
			return S_OK;
		}
	}

	if( m_pCreatingBrowser )
		*phwnd = m_pCreatingBrowser->m_hWnd;
	else if( m_pActiveBrowser )
		*phwnd = m_pActiveBrowser->m_hWnd;
	else
		*phwnd = NULL;
	return S_OK;
}

STDMETHODIMP CWebBrowser::GetWindowContext( IOleInPlaceFrame **ppFrame,
	IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, 
	LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo )
{
	//NOTIMPLEMENTED;
	assert( m_pCreatingBrowser );
	*ppFrame = this;
	*ppDoc = NULL;
	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->haccel = NULL;
	lpFrameInfo->cAccelEntries = 0;

/*	//²âÊÔ°æ
	lpFrameInfo->fMDIApp = TRUE;
	lpFrameInfo->hwndFrame = m_hFrameWnd;
	return S_OK;
*/

	if( IsMultiThread() )
	{
		CBrowserWnd * pBrowser = GetBrowserWnd();
		if( pBrowser )
		{
			lpFrameInfo->hwndFrame = pBrowser->m_hWnd;
			return S_OK;
		}
	}

	if( m_pCreatingBrowser )
		lpFrameInfo->hwndFrame = m_pCreatingBrowser->m_hWnd;
	else if( m_pActiveBrowser )
		lpFrameInfo->hwndFrame = m_pActiveBrowser->m_hWnd;
	else
		lpFrameInfo->hwndFrame = NULL;

	return S_OK;
}

STDMETHODIMP CWebBrowser::SetActiveObject( IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName )
{
	NOTIMPLEMENTED;
}


STDMETHODIMP CWebBrowser::OnPosRectChange( LPCRECT lprcPosRect )
{
	NOTIMPLEMENTED;
}

STDMETHODIMP CWebBrowser::QueryService( REFGUID guidService, REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject )
{
	HRESULT hr = E_NOINTERFACE;

    return hr;
}

CBrowserWnd * CWebBrowser::CreateBrowser( HWND hParent, LPRECT lpRect, BOOL bCreateForNewWindow2, CBrowserWnd * pParentBrowser )
{
#ifdef _DEBUG
	if( !m_bCSInited )
	{
		assert( FALSE );
	}
	HWND hwndMain = GetAncestor( hParent, GA_ROOT );
	//ValidateThreadModel( hwndMain );
	assert( m_lpfnCBE != NULL );
	assert( m_lpfnCBW != NULL );
#endif

	CBrowserEvent * pEvent = m_lpfnCBE();
	CBrowserWnd * pBrowserWnd = m_lpfnCBW();
	pBrowserWnd->SetCreateForNewWindow2( bCreateForNewWindow2 );


	m_pCreatingBrowser = pBrowserWnd;
	pBrowserWnd->SetObject( this, pEvent, pParentBrowser );

	HWND hWndBrowserParent = pBrowserWnd->Create( hParent, 0, lpRect, (IOleClientSite *)this, (IStorage *)this, m_nThreadStyle );
	if( hWndBrowserParent )
	{
		if( m_pActiveBrowser == NULL )
			m_pActiveBrowser = pBrowserWnd;
	}
	else
	{
		assert( 0 );
		delete pBrowserWnd;
		pBrowserWnd = NULL;
	}
	m_pCreatingBrowser = NULL;
	return pBrowserWnd;

}

void CWebBrowser::DestroyBrowser( CBrowserWnd * pBrowser )
{
	if( m_pActiveBrowser == pBrowser )
		m_pActiveBrowser = NULL;

	//pBrowser->Resume();
	pBrowser->Destroy();
}

void CWebBrowser::Enter()
{
	EnterCriticalSection( &m_CriticalSection );
}

void CWebBrowser::Leave()
{
	LeaveCriticalSection( &m_CriticalSection );
}

BOOL CWebBrowser::IsInMessageTextFilter( BSTR bstrString )
{
	return TRUE;
}

STDMETHODIMP CWebBrowser::Exec( const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
{
	HRESULT hr = S_OK;
	BOOL bNeedFilter = FALSE;
	try
	{
		if(pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
		{
			if(nCmdID == OLECMDID_SHOWMESSAGE)
			{
				if( !m_bShowMessageDlg )
				{
					(*pvaOut).vt = VT_BOOL;
					(*pvaOut).boolVal = VARIANT_TRUE;
					return S_OK;
				}


				IHTMLDocument2*             pDoc = NULL;
				IHTMLWindow2*               pWindow = NULL;
				IHTMLEventObj*              pEventObj = NULL;
				BSTR                        rgwszName = L"messageText";
				DISPID                      rgDispID;
				VARIANT                     rgvaEventInfo;
				DISPPARAMS                  params;

				params.cArgs = 0;
				params.cNamedArgs = 0;
				rgvaEventInfo.bstrVal = NULL;

				if( SUCCEEDED(pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) &pDoc)) )
				{
					hr = pDoc->get_parentWindow(&pWindow);
					if( pWindow )
					{
						if( !bNeedFilter )
						{
							if( SUCCEEDED(pWindow->get_event(&pEventObj)) )
							{
								pEventObj->GetIDsOfNames( IID_NULL, &rgwszName, 1, LOCALE_SYSTEM_DEFAULT, &rgDispID );
								// Get the value of the property.
								hr = pEventObj->Invoke( rgDispID, IID_NULL,LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &params, &rgvaEventInfo, NULL, NULL );
								pEventObj->Release();
							}
							
							if( rgvaEventInfo.bstrVal )
							{
								if( wcsstr(rgvaEventInfo.bstrVal, L" ActiveX ") != NULL
									|| wcsstr(rgvaEventInfo.bstrVal, L"Java Script Error") != NULL )
									bNeedFilter = TRUE;

								SysFreeString(rgvaEventInfo.bstrVal);
							}
						}

						pWindow->Release();
					}						
					pDoc->Release();
				}

				(*pvaOut).vt = VT_BOOL;
				if( bNeedFilter )
				{
					(*pvaOut).boolVal = VARIANT_TRUE;
					return S_OK;
				}
				else
				{
					(*pvaOut).boolVal = VARIANT_FALSE;
		 			return OLECMDERR_E_NOTSUPPORTED;
				}
			}
			else if(nCmdID == OLECMDID_SHOWSCRIPTERROR)
			{
				if( !m_bShowMessageDlg )
				{
					(*pvaOut).vt = VT_BOOL;
					(*pvaOut).boolVal = VARIANT_TRUE;
					return S_OK;
				}

				// Continue running scripts on the page.
				(*pvaOut).vt = VT_BOOL;
				(*pvaOut).boolVal = VARIANT_TRUE;


				CBrowserWnd* pBrowserWnd = NULL;
				if( m_nThreadStyle != SINGLE_THREAD )
				{
					pBrowserWnd = GetBrowserWnd( NULL, NULL );
				}
				else
				{
					IHTMLDocument2* pDoc = NULL;
					pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) &pDoc);
					if(pDoc)
						pBrowserWnd = GetBrowserWnd( NULL, pDoc );
				}

				return S_OK;
			}
		}
	}
	catch(...)
	{
	}
	return OLECMDERR_E_NOTSUPPORTED;
}

void CWebBrowser::Active(CBrowserWnd *pBrowserWnd)
{
	m_pActiveBrowser = pBrowserWnd;
}

void CWebBrowser::DeActive(CBrowserWnd *pBrowserWnd)
{
	if( m_pActiveBrowser == pBrowserWnd )
	{
		m_pActiveBrowser = NULL;
	}
}

CBrowserWnd *CWebBrowser::GetBrowserWnd( IHTMLWindow2* pWin2, IHTMLDocument2* pDoc2 )
{
	CBrowserWnd *pWndBrowser = NULL;
	if( m_nThreadStyle != SINGLE_THREAD )
		pWndBrowser = (CBrowserWnd*)::TlsGetValue( m_dwTlsIndex );
	else
	{
		IHTMLWindow2* pWin2Temp = NULL;
		if( !pWin2 && pDoc2 )
		{
			pDoc2->get_parentWindow( &pWin2Temp );
			pWin2 = pWin2Temp;
		}

		if( pWin2 )
		{
			IHTMLDocument2Ptr pHD2Ptr;
			pWin2->get_document( &pHD2Ptr );
			if( pHD2Ptr )
			{
				IWebBrowser2Ptr pWB2Ptr, pWB2TopPtr;
				GetBrowserFromDoc( pHD2Ptr, &pWB2Ptr );
				if( pWB2Ptr )
				{
					GetTopBrowser( pWB2Ptr, &pWB2TopPtr );
					if( pWB2TopPtr )
					{
						pWndBrowser = CBrowserWnd::GetBrowserWndPtr( pWB2TopPtr );
					}
				}
			}
		}
		if( pWin2Temp )
			pWin2Temp->Release();
	}

	return pWndBrowser;
}

static BOOL IsChildSafe( HWND hParent, HWND hTest )
{
	if( IsWindow(hParent) && hTest )
		return IsChild( hParent, hTest );
	return FALSE;
}

BOOL CWebBrowser::TranslateAccelerator( LPMSG lpMsg )
{
	if( m_pActiveBrowser )
	{
		if( m_pActiveBrowser->m_pOIPA )
		{
			HWND hFocus = GetFocus();
			if( hFocus == m_pActiveBrowser->m_hWnd || IsChildSafe( m_pActiveBrowser->m_hWnd, hFocus ) )
			{
				return ( S_OK == m_pActiveBrowser->m_pOIPA->TranslateAccelerator( lpMsg ) ) ;
			}
		}
	}
	return FALSE;
}


BOOL CWebBrowser::IsMultiThread()
{
	return m_nThreadStyle == MULTI_THREAD;
}

void CWebBrowser::SetStatusRule(BOOL bShowStatusTextLinkOnly)
{
	m_bShowStatusTextLinkOnly = bShowStatusTextLinkOnly;
}

void CWebBrowser::SetMessageRule(BOOL bMessageFilter, LPTSTR pszFilter)
{
	m_bMessageFilter = bMessageFilter;
	_sntprintf_s( m_szMessageFilter, SIZEOF(m_szMessageFilter), _TRUNCATE, _T("%s"), pszFilter );
	m_szMessageFilter[SIZEOF(m_szMessageFilter)-1] = _T('\0');
}

void CWebBrowser::SetShowMessageDlg(BOOL bShow)
{
	m_bShowMessageDlg = bShow;
}
