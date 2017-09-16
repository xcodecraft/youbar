// WebBrowser.h: interface for the CWebBrowser class.
//				 : Create by Xiezhenyu
//////////////////////////////////////////////////////////////////////

#pragma once

//#include "stdafx.h"
#include <shlobj.h>
#include <mshtml.h>
#include <mshtmhst.h>
//#import  <shdocvw.dll>
#include "BrowserWnd.h"
#include "WebBrowserEx.h"

#define NOTIMPLEMENTED	return(E_NOTIMPL)
typedef CBrowserWnd * ( CALLBACK *LPCREATEBROWSERWNDCALLBACK )();
typedef CBrowserEvent * ( CALLBACK *LPCREATEBROWSEREVENTCALLBACK )();

class CWebBrowser : public IOleClientSite,
					public IOleInPlaceSite,
					public IOleInPlaceFrame,
					public IOleCommandTarget,
					public IServiceProvider,
					public IStorage
{

public:
	CBrowserWnd * m_pActiveBrowser;
	CBrowserWnd * m_pCreatingBrowser;
	CWebBrowserEx * m_pBrowserEx;
	BOOL m_bEnableDragDrop;
	BOOL m_bEnableWheelZoomPage;
	BOOL m_bShowStatusTextLinkOnly;
	BOOL m_bOptimizeMode;
	BOOL m_bMutiThreadIMM;
	LPCREATEBROWSERWNDCALLBACK m_lpfnCBW;
	LPCREATEBROWSEREVENTCALLBACK m_lpfnCBE;

public:
	CWebBrowser();
	virtual ~CWebBrowser();

	CBrowserWnd * CreateBrowser( HWND hParent, LPRECT lpRect, BOOL bCreateForNewWindow2 = FALSE, CBrowserWnd * pParentBrowser = NULL );
	void DestroyBrowser( CBrowserWnd * pBrowser );
	BOOL TranslateAccelerator( LPMSG lpMsg );

	void Init(int nThreadStyle, CWebBrowserEx * pBrowserEx, LPCREATEBROWSERWNDCALLBACK lpfnCBW, LPCREATEBROWSEREVENTCALLBACK lpfnCBE, BOOL bMutiThreadIMM = FALSE, HWND hFrameWnd = NULL );
	void UnInit();
	DWORD GetTlsIndex(){ return m_dwTlsIndex; };
	void Enter();
	void Leave();

	HWND				m_hTopMostWnd;//浏览器窗口相对应的最上层窗口，浏览器窗口在setposition改变order的时候不能超过这个
	HWND				m_hFrameWnd;
private:
	#ifdef _DEBUG
		BOOL m_bCSInited;
	#endif
	CRITICAL_SECTION	m_CriticalSection;			// 操作tab的临界区
	BOOL				m_nThreadStyle;

	BOOL InitTlsStorage( BOOL bInit );
	DWORD m_dwTlsIndex;
	BOOL m_bMessageFilter;
	TCHAR m_szMessageFilter[1024];
	//IDownloadManager*	m_lpDownloadMgr;
	BOOL m_bShowMessageDlg;

public:
	void SetShowMessageDlg( BOOL bShow );
	BOOL IsInMessageTextFilter( BSTR bstrString );	
	void SetMessageRule( BOOL bMessageFilter, LPTSTR pszFilter );
	void SetStatusRule( BOOL bShowStatusTextLinkOnly );
	BOOL IsMultiThread();
	CBrowserWnd *GetBrowserWnd( IHTMLWindow2* pWin2 = NULL, IHTMLDocument2* pDoc2 = NULL );
	void Active( CBrowserWnd * pBrowserWnd );
	void DeActive( CBrowserWnd * pBrowserWnd );
	//inline void SetDownloadManager( IDownloadManager* lpDownloadMgr ){ m_lpDownloadMgr = lpDownloadMgr; }

	////////////////////////////////////////////////////////////////////
	// IUnKnown
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	////////////////////////////////////////////////////////////////////
	// IOleClientSite
    STDMETHOD(SaveObject)(void){NOTIMPLEMENTED;};
    STDMETHOD(GetMoniker)( DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk){NOTIMPLEMENTED;};
    STDMETHOD(GetContainer)( IOleContainer **ppContainer );
	STDMETHOD(ShowObject)( void ){return S_OK;};
    STDMETHOD(OnShowWindow)( BOOL fShow ){NOTIMPLEMENTED;};
    STDMETHOD(RequestNewObjectLayout)( void ){NOTIMPLEMENTED;};

	////////////////////////////////////////////////////////////////////
	// IOleInPlaceSite
    STDMETHOD(CanInPlaceActivate)( void){return S_OK;};
    STDMETHOD(OnInPlaceActivate)( void){return S_OK;};
    STDMETHOD(OnUIActivate)( void){return S_OK;};
    STDMETHOD(GetWindowContext)( IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo );
    STDMETHOD(Scroll)( SIZE scrollExtant ){NOTIMPLEMENTED;};
    STDMETHOD(OnUIDeactivate)( BOOL fUndoable ){return S_OK;};
    STDMETHOD(OnInPlaceDeactivate)( void){return S_OK;};
    STDMETHOD(DiscardUndoState)( void){NOTIMPLEMENTED;};
    STDMETHOD(DeactivateAndUndo)( void){NOTIMPLEMENTED;};
    STDMETHOD(OnPosRectChange)( LPCRECT lprcPosRect);

	////////////////////////////////////////////////////////////////////
	// IOleWindow
    STDMETHOD(GetWindow)( HWND *phwnd );
    STDMETHOD(ContextSensitiveHelp)( BOOL fEnterMode ){NOTIMPLEMENTED;};

	////////////////////////////////////////////////////////////////////
	// IOleInPlaceFrame
    STDMETHOD(InsertMenus)( HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths ){NOTIMPLEMENTED;};
    STDMETHOD(SetMenu)( HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject ){return S_OK;};
    STDMETHOD(RemoveMenus)( HMENU hmenuShared ){NOTIMPLEMENTED;};
    STDMETHOD(SetStatusText)( LPCOLESTR pszStatusText ){return S_OK;};
    STDMETHOD(EnableModeless)( BOOL fEnable ){return S_OK;};
    STDMETHOD(TranslateAccelerator)( LPMSG lpmsg, WORD wID ){NOTIMPLEMENTED;};

	////////////////////////////////////////////////////////////////////
	// IOleInPlaceUIWindow
	STDMETHOD(GetBorder)( LPRECT lprectBorder ){NOTIMPLEMENTED;};
    STDMETHOD(RequestBorderSpace)( LPCBORDERWIDTHS pborderwidths ){NOTIMPLEMENTED;};
    STDMETHOD(SetBorderSpace)( LPCBORDERWIDTHS pborderwidths ){NOTIMPLEMENTED;};
    STDMETHOD(SetActiveObject)( IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName );

    ////////////////////////////////////////////////////////////////////
	// IStorage
	STDMETHOD(CreateStream)( const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm ){NOTIMPLEMENTED;};
    STDMETHOD(OpenStream)( const OLECHAR *pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm ){NOTIMPLEMENTED;};
    STDMETHOD(CreateStorage)( const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg ){NOTIMPLEMENTED;};
    STDMETHOD(OpenStorage)( const OLECHAR *pwcsName, IStorage *pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg ){NOTIMPLEMENTED;};
    STDMETHOD(CopyTo)( DWORD ciidExclude, const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest ){NOTIMPLEMENTED;};
    STDMETHOD(MoveElementTo)( const OLECHAR *pwcsName, IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags ){NOTIMPLEMENTED;};
    STDMETHOD(Commit)( DWORD grfCommitFlags ){NOTIMPLEMENTED;};
    STDMETHOD(Revert)( void ){NOTIMPLEMENTED;};
    STDMETHOD(EnumElements)( DWORD reserved1, void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum ){NOTIMPLEMENTED;};
    STDMETHOD(DestroyElement)( const OLECHAR *pwcsName ){NOTIMPLEMENTED;};
    STDMETHOD(RenameElement)( const OLECHAR *pwcsOldName, const OLECHAR *pwcsNewName ){NOTIMPLEMENTED;};
    STDMETHOD(SetElementTimes)( const OLECHAR *pwcsName, const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime){NOTIMPLEMENTED;};
    STDMETHOD(SetClass)( REFCLSID clsid ){return S_OK;};
    STDMETHOD(SetStateBits)( DWORD grfStateBits, DWORD grfMask ){NOTIMPLEMENTED;};
    STDMETHOD(Stat)( STATSTG *pstatstg, DWORD grfStatFlag ){NOTIMPLEMENTED;};
	
	////////////////////////////////////////////////////////////////////
	// IServiceProvider
	STDMETHOD(QueryService)( REFGUID guidService, REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject );

	////////////////////////////////////////////////////////////////////
	// IOleCommandTarget
    STDMETHOD(QueryStatus)( const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText){return OLECMDERR_E_NOTSUPPORTED;};
    STDMETHOD(Exec)( const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut);

};

