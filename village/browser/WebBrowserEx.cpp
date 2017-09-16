// DocHostUIHandler.cpp: implementation of the CWebBrowserEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WebBrowserEx.h"
#include "WebBrowser.h"

extern CWebBrowser	_WebBrowser;
extern BOOL _bIE7;
extern BOOL _bIE8;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWebBrowserEx::CWebBrowserEx()
{
	m_pDispExternal = NULL;
	m_dwPageItemShow = PISHOW_DEF;
	m_bNoUseThemeUI = FALSE;
	m_bImgBicubic = FALSE; 
	m_pParentWebBrowser = NULL;
	//m_bScroll = global._bBrowserScroll;
	m_bScroll =  false;
}

CWebBrowserEx::~CWebBrowserEx()
{
}

STDMETHODIMP_(ULONG)CWebBrowserEx::AddRef(THIS)
{
	return 1;
}

STDMETHODIMP_(ULONG)CWebBrowserEx::Release(THIS)
{
	return 1;
}

STDMETHODIMP CWebBrowserEx::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	if( IsEqualIID(riid, IID_IUnknown) )
		*ppvObj = this;
	else if( IsEqualIID(riid, IID_IDocHostUIHandler) )
		*ppvObj = (IDocHostUIHandler *)this;
	else if( IsEqualIID(riid, IID_IDispatch) )
		*ppvObj = (IDispatch *)this;
	else if( IsEqualIID(riid, IID_IOleCommandTarget) )
	{
		m_pParentWebBrowser->QueryInterface( riid, ppvObj );
		if( NULL == *ppvObj )
			return E_NOINTERFACE;
	}
//	else if( IsEqualIID(riid, IID_IDocHostShowUI) )
//		*ppvObj = (IDispatch *)this;
	else
	{
		//是否应该是所有CWebBrowserEx没有的接口都应该直接返到m_pParentWebBrowser->QueryInterface( riid, ppvObj );
		assert( 0 );
		*ppvObj = 0;
		return E_NOINTERFACE;
	}

	return S_OK;	
}

STDMETHODIMP CWebBrowserEx::GetExternal( IDispatch **ppDispatch )
{
	if( m_pDispExternal )
	{
		*ppDispatch = m_pDispExternal;
		return(S_OK);
	}
	else
		return(S_FALSE);
}

STDMETHODIMP CWebBrowserEx::TranslateUrl( DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	*ppchURLOut = 0;
	return(S_FALSE);
}

STDMETHODIMP CWebBrowserEx::FilterDataObject( IDataObject *pDO, IDataObject **ppDORet )
{
	*ppDORet = 0;
	return(S_FALSE);
}

STDMETHODIMP CWebBrowserEx::GetHostInfo( DOCHOSTUIINFO *pInfo )
{
	pInfo->cbSize = sizeof(DOCHOSTUIINFO);
	pInfo->dwFlags = DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE;
	if ( !m_bNoUseThemeUI )
		pInfo->dwFlags |= DOCHOSTUIFLAG_THEME;
	if ( !m_bScroll)
		pInfo->dwFlags |= DOCHOSTUIFLAG_SCROLL_NO;
	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

	if ( m_bImgBicubic )
	{
//		TCHAR szVersion[ MAX_PATH ] = _T( "" );
//		GetIEVersion( szVersion, SIZEOF( szVersion ) - 1 );
//		if ( _T( '7' ) == szVersion[0] )
		if( _bIE7 || _bIE8 )
		{
			WCHAR* szCSS = L"img { -ms-interpolation-mode:bicubic; }";
			OLECHAR* pCSSBuffer=(OLECHAR*) CoTaskMemAlloc((wcslen(szCSS)+1)*sizeof(OLECHAR));
			wcscpy( pCSSBuffer, szCSS );
			pInfo->pchHostCss = pCSSBuffer;
		}
	}

/*
	WCHAR* szCSS = L"BODY {background-color:#ffcccc}";
	WCHAR* szNS = L"IE;MyTags;MyTags2='www.microsoft.com'";
	
	
	OLECHAR* pCSSBuffer=(OLECHAR*) CoTaskMemAlloc((wcslen(szCSS)+1)*sizeof(OLECHAR));
	wcscpy( pCSSBuffer, szCSS );
	
	OLECHAR* pNSBuffer=(OLECHAR*) CoTaskMemAlloc((wcslen(szNS)+1)*sizeof(OLECHAR));
	wcscpy( pNSBuffer, szNS );
	
	pInfo-> pchHostCss= pCSSBuffer;
	pInfo-> pchHostNS= pNSBuffer;
*/
    return S_OK;
}

STDMETHODIMP CWebBrowserEx::GetTypeInfoCount( UINT *pctinfo )
{
	return E_NOINTERFACE;
}

STDMETHODIMP CWebBrowserEx::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
{
	return E_NOINTERFACE;
}

STDMETHODIMP CWebBrowserEx::GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId )
{
	return E_NOINTERFACE;
}

STDMETHODIMP CWebBrowserEx::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, 
	VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr )
{
	DWORD dwID = 0;
	if (!pDispParams)
		return E_INVALIDARG;

// 	switch( dispIdMember )
// 	{
// 		case DISPID_AMBIENT_DLCONTROL:
// 			pVarResult->vt = VT_I4;
// 			pVarResult->lVal = 0;
// 			if( m_dwPageItemShow & PISHOW_IMAGE )
// 				pVarResult->lVal |=  DLCTL_DLIMAGES;
// 			if( m_dwPageItemShow & PISHOW_VIDEO )
// 				pVarResult->lVal |=  DLCTL_VIDEOS;
// 			if( m_dwPageItemShow & PISHOW_BGSOUND )
// 				pVarResult->lVal |=  DLCTL_BGSOUNDS;
// 			if( !(m_dwPageItemShow & PISHOW_SCRIPT) )
// 				pVarResult->lVal |=  DLCTL_NO_SCRIPTS;
// 			if( !(m_dwPageItemShow & PISHOW_JAVA) )
// 				pVarResult->lVal |=  DLCTL_NO_JAVA;
// 			break;
// 
// //		case DISPID_AMBIENT_USERAGENT:
// //			return OnAmbientUserAgent( pVarResult );
// 
// 		default:
// 			return DISP_E_MEMBERNOTFOUND;
// 	}
	

	return DISP_E_MEMBERNOTFOUND/*S_OK*/;
}

STDMETHODIMP CWebBrowserEx::ShowContextMenu( DWORD dwID, POINT *ppt, IUnknown *pcmdTarget, IDispatch *pdispReserved )
{
	if (dwID != 2)
		return S_OK;
	
	return S_FALSE;
}

DWORD CWebBrowserEx::GetPageItemShown()
{
	return m_dwPageItemShow;
}

void CWebBrowserEx::SetPageItemShown(DWORD dwShown)
{
	m_dwPageItemShow = dwShown;
}



STDMETHODIMP CWebBrowserEx::GetDropTarget( IDropTarget *pDropTarget, IDropTarget **ppDropTarget )
{	
	return E_NOTIMPL;
}