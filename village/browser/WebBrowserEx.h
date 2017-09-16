// DocHostUIHandler.h: interface for the CWebBrowserEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCHOSTUIHANDLER_H__E21BA4F7_AA22_45C3_9076_352FCC1EDFD7__INCLUDED_)
#define AFX_DOCHOSTUIHANDLER_H__E21BA4F7_AA22_45C3_9076_352FCC1EDFD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mshtmhst.h>

// Page item show
#define PISHOW_IMAGE		1
#define PISHOW_VIDEO		2
#define PISHOW_BGSOUND		4
#define PISHOW_SCRIPT		8
#define PISHOW_JAVA			16
#define PISHOW_DEF			PISHOW_IMAGE|PISHOW_VIDEO|PISHOW_BGSOUND|PISHOW_SCRIPT|PISHOW_JAVA

class CWebBrowser;
class CWebBrowserEx : public IDocHostUIHandler,
					  //public IDocHostShowUI,
					  public IDispatch  
{
public:
	CWebBrowser* m_pParentWebBrowser;
	BOOL m_bNoUseThemeUI;
	BOOL m_bImgBicubic;
	void SetPageItemShown( DWORD dwShown );
	DWORD GetPageItemShown();
	CWebBrowserEx();
	virtual ~CWebBrowserEx();

	virtual HRESULT OnAmbientUserAgent( VARIANT * pVarResult ){return DISP_E_MEMBERNOTFOUND;}
    void SetExternal( IDispatch *pDispatch ){ m_pDispExternal = pDispatch; };

	////////////////////////////////////////////////////////////////////
	// IUnKnown
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);
	////////////////////////////////////////////////////////////////////
	// IDocHostUIHandler
    STDMETHOD(ShowContextMenu)( DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved );
    STDMETHOD(ShowUI)( DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc ){return S_OK;};
    STDMETHOD(HideUI)( void ){return S_OK;};
    STDMETHOD(UpdateUI)( void ){return S_OK;};
    STDMETHOD(EnableModeless)( BOOL fEnable ){return S_OK;};
    STDMETHOD(OnDocWindowActivate)( BOOL fActivate ){return S_OK;};
    STDMETHOD(OnFrameWindowActivate)( BOOL fActivate ){return S_OK;};
    STDMETHOD(ResizeBorder)( LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow ){return S_OK;};
	STDMETHOD(TranslateAccelerator)( LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID ){return S_FALSE;};
    STDMETHOD(GetOptionKeyPath)( LPOLESTR *pchKey, DWORD dw ){return S_FALSE;};
    STDMETHOD(GetDropTarget)( IDropTarget *pDropTarget, IDropTarget **ppDropTarget );
    STDMETHOD(GetHostInfo)( DOCHOSTUIINFO *pInfo );
    STDMETHOD(GetExternal)( IDispatch **ppDispatch );
    STDMETHOD(TranslateUrl)( DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
    STDMETHOD(FilterDataObject)( IDataObject *pDO, IDataObject **ppDORet );
	////////////////////////////////////////////////////////////////////
	// IDocHostShowUI
    //STDMETHOD(ShowMessage)( HWND hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption, DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext, LRESULT *plResult ){return S_OK;};
	//STDMETHOD(ShowHelp)(HWND hwnd,LPOLESTR pszHelpFile,UINT uCommand,DWORD dwData,POINT ptMouse,IDispatch *pDispatchObjectHit){return S_FALSE;};
	////////////////////////////////////////////////////////////////////
	// IDispatch
	STDMETHOD(GetTypeInfoCount)( UINT *pctinfo );
	STDMETHOD(GetTypeInfo)( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo );
	STDMETHOD(GetIDsOfNames)( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId );
    STDMETHOD(Invoke)( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );

private:
	DWORD m_dwPageItemShow;
	BOOL m_bScroll;
	IDispatch* m_pDispExternal;
};

#endif // !defined(AFX_DOCHOSTUIHANDLER_H__E21BA4F7_AA22_45C3_9076_352FCC1EDFD7__INCLUDED_)
