#pragma once
#include "WebCommon.h"
#include "BrowserEvent.h"

#define MARKBW_NOUSE	0
#define MARKBW_USE		1

#define ABOUTBLANK					_T("about:blank")
#define ABOUTBLANKW					L"about:blank"
#define BROWSERWND_NAME				_T("Browser")
#define WM_BROWSER_DESTROYED		WM_USER+0x9000
#define WM_BROWSER_CREATEREQUEST	WM_USER+0x9001
#define WM_BROWSER_EVENTMSG			WM_USER+0x9002
#define WM_BROWSER_CHAINCOMMAND		WM_USER+0x9003
#define WM_BROWSER_EXECWB			WM_USER+0x9004
#define WM_BROWSER_FONTSIZE			WM_USER+0x9005
#define WM_BROWSER_ZOOM				WM_USER+0x9006
#define WM_BROWSER_ACTIVE			WM_USER+0x9007
#define WM_BROWSER_NAVI				WM_USER+0x9008
#define WM_BROWSER_QUIT				WM_USER+0x9009
#define WM_BROWSER_EXECHTML			WM_USER+0x9010
#define WM_BROWSER_FOCUS			WM_USER+0x9011
#define WM_BROWSER_SIZE				WM_USER+0x9012
#define WM_BROWSER_GETENCODE		WM_USER+0x9013
#define WM_BROWSER_NAVIADVANCE		WM_USER+0x9014
#define WM_BROWSER_UIACTIVATE		WM_USER+0x9015
#define WM_BROWSER_SETENCODEVAL		WM_USER+0x9016
#define WM_BROWSER_SETKLO			WM_USER+0x9017
#define WM_BROWSER_STOP				WM_USER+0x9018
#define WM_BROWSER_HOOK				WM_USER+0x1002

// HTML CmdTarget ID
#define HTMLID_FIND			1
#define HTMLID_VIEWSOURCE	2
#define HTMLID_OPTIONS		3 

enum
{
	BS_ACTIVE=0,
	BS_SHOWN,
	BS_HIDDEN,
};

#define WM_BROWSER_FUNCTHREAD		WM_USER+0x9050
#define TB_FUNC_SHOWSSLDLG		1
#define TB_FUNC_DESIGNMODE		2
#define TB_FUNC_GETTRAVELCOUNT  3
#define TB_FUNC_SENDMAIL		4
#define TB_FUNC_STOP			5
#define TB_FUNC_REFRESH			6
#define TB_FUNC_REFRESH2		7
#define TB_FUNC_ZOOMFONT		8
#define TB_FUNC_ONAMBIENTPROPERTYCHANGE		9


#define WM_BROWSERWND_OPERATION	WM_USER+0x4001
#define BO_FINDSTRING				1
#define BO_HILIGHTSTRING			2
#define BO_GETSAFETHREADBROWSER		3
#define BO_SAVETOIMAGE				4


#define WEBBROWSERFRAMEBORDERSIZE		2

typedef struct tagBrowserWndOperation
{
	DWORD cbSize;
	UINT	uID;
	HWND	hWnd;
	WPARAM wParam;
	LPARAM lParam;
	TCHAR szBuffer[MAX_URL_LEN];
	LRESULT lRet;
}BROWSERWNDOPERATION, *LPBROWSERWNDOPERATION;

typedef struct tagBrowserWndSize
{
	HWND hWndInsertAfter;
	int x;
	int y;
	int cx;
	int cy;
	UINT uFlags;
}BROWSERSIZE, *LPBROWSERSIZE;


static CLSID const CGID_IWebBrowser
= { 0xED016940L,0xBD5B,0x11cf, { 0xBA, 0x4E,0x00,0xC0,0x4F,0xD7,0x08,0x16 } }; 

class CMultiTabWnd;
class CWebBrowser;



typedef LRESULT CALLBACK ExternalBrowserDropProc( LPTSTR pszText, CBrowserWnd * pBrowser );

class CBrowserWndHint : public CWindowImpl<CBrowserWndHint>
{
public:
	CBrowserWndHint( CBrowserWnd* pBrowserWnd ){
		m_pOwnerBrowserWnd = pBrowserWnd;
	};

	BEGIN_MSG_MAP(CBrowserWndHint)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	END_MSG_MAP()
		
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//virtual LRESULT __stdcall WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

private:
	CBrowserWnd* m_pOwnerBrowserWnd;
};

typedef struct tagBrowserWndNaviData
{
	tagBrowserWndNaviData(){};
	~tagBrowserWndNaviData()
	{
		if( URL.vt == VT_BSTR && URL.bstrVal )
			SysFreeString( URL.bstrVal );
		if( TargetFrameName.vt == VT_BSTR && TargetFrameName.bstrVal )
			SysFreeString( TargetFrameName.bstrVal );
		if( PostData.vt == VT_BSTR && PostData.bstrVal )
			SysFreeString( PostData.bstrVal );
		if( Headers.vt == VT_BSTR && Headers.bstrVal )
			SysFreeString( Headers.bstrVal );
	};
	VARIANT URL;
	VARIANT Flags;
	VARIANT TargetFrameName;
	VARIANT PostData;
	VARIANT Headers;
}BROWSERWNDNAVIDATA, *LPBROWSERWNDNAVIDATA;

class CBrowserWnd : public CWindowImpl<CBrowserWnd>
{
	friend class CBrowserEvent;
	friend class CWebBrowser;
	friend class CWebBrowserEx;
public:
	DECLARE_WND_CLASS_EX(_T("360browser"),  CS_DBLCLKS, (HBRUSH)(COLOR_WINDOW + 1))

	void SetWindowPosSafe(int x, int y, int cx, int cy, UINT uFlags);
	void GetCommandState(BOOL &bBack, BOOL &bForward);
	void GetBrowserRect(RECT* pRect);
	void SetThreadInfo(BOOL bSet);
	void OnHintWndPaint();
	void ShowTopHint( LPTSTR pszHint );
	POINT m_pointLastLDown;
	int GetTravelCount( BOOL bBackward );
	int GetTravelCount_SafeThread(BOOL bBackward);
	void UIActivate();
	virtual void SetBrowserFocus( BOOL bCondition = FALSE );
	LPVOID GetNewWindowObject();
	void SetCreateForNewWindow2( BOOL bNewWindow2 );
	static CBrowserWnd * GetBrowserWndPtr( IWebBrowser2 * pWeb );
	static CBrowserWnd * GetBrowserWndPtr( HWND hWnd );
	BOOL IsActivate();
	BOOL IsShown();
	IWebBrowser2 * GetBrowser();
	IWebBrowser2 * GetSafeBrowser();
	IWebBrowser2 * GetSafeThreadBrowser();
    BOOL IsTopWebBrowser(IDispatch *pDisp);
    BOOL GetBrowserUrl(LPTSTR lpszUrl, DWORD dwSize);
	BOOL GetBrowserTitle(LPTSTR lpszTitle, DWORD dwSize);

	CBrowserWnd();
	virtual ~CBrowserWnd();
	HWND Create( HWND hWndParent, WORD wID, LPRECT lpRect, IOleClientSite * pClientSite, IStorage * pStorage, BOOL bSingleThread );
	void Destroy();
	void Close( );
	void Active(UINT uState);
	
	// 设置进度条
	void SetProgress(long lMax, long lCurrent);

	void SetObject(CWebBrowser * pWebHost, CBrowserEvent * pEvent, CBrowserWnd * pParentBrowser, BOOL bEventCanFree = TRUE);
	CBrowserEvent* GetBrowserEvent( );

	IDispatch * GetDispatch(){  return m_pDispatch; }

	virtual void Navigate(LPCTSTR pszUrl, BOOL bNew = FALSE);
	virtual HRESULT Browser_FuncThread( WPARAM wParam, LPARAM lParam );
	BOOL Home( BOOL bIgnoreBlank = FALSE );
	virtual void OnRefresh( ){};
	virtual void Stop();
	virtual void Stop_SafeThread();
	void Refresh_SafeThread();
	void Refresh();
	void RefreshCompletely_SafeThread();
	virtual void RefreshCompletely();
	void QueueBack( int nStep = 1 );
	void QueueForward( int nStep = 1 );

	virtual void InitObject();

	virtual void Navigate( VARIANT *URL, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers);

	IWebBrowser2 *		m_pWebBrowserThread;
	IOleInPlaceActiveObject * m_pOIPA;

	long				m_lMax;
	long				m_lCurrent;
	HWND				m_hWndBrowser;
	WCHAR				m_wszZoomPercent[32];
	BOOL				m_bSafeModeBlocked;

	BOOL				m_bDestroyed;
	BOOL				m_bPauseCreateProcessGuard;
	int					m_nTimerCount;

	BEGIN_MSG_MAP(CBrowserWnd)
		MESSAGE_HANDLER(WM_DESTROY, OnDestory)
		MESSAGE_HANDLER(WM_BROWSER_QUIT, OnBrowserQuit)
		MESSAGE_HANDLER(WM_BROWSERWND_OPERATION, OnBrowserOp)
		MESSAGE_HANDLER(WM_BROWSER_ACTIVE, OnBrowserActive)
		MESSAGE_HANDLER(WM_BROWSER_FOCUS, OnBrowserFocus)
		MESSAGE_HANDLER(WM_BROWSER_SIZE, OnBrowserMsgSize)
		MESSAGE_HANDLER(WM_BROWSER_FUNCTHREAD, OnBrowserFunThread)
		MESSAGE_HANDLER(WM_PARENTNOTIFY, OnParentNotify)
		MESSAGE_HANDLER(WM_BROWSER_DESTROYED, OnBrowserDestroyed)
		MESSAGE_HANDLER(WM_SIZE, OnSizeMsg)
		MESSAGE_HANDLER(WM_COMMAND, OnBrowserCommand)
		MESSAGE_HANDLER(WM_BROWSER_EVENTMSG, OnBrowserEventMsg)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestory)
		MESSAGE_HANDLER(WM_BROWSER_NAVIADVANCE, OnBrowserNaviAdvance)
		MESSAGE_HANDLER(WM_BROWSER_NAVI, OnBrowserNavi)
	END_MSG_MAP()
	
	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBrowserQuit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBrowserOp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBrowserActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBrowserFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBrowserMsgSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT	OnBrowserFunThread(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBrowserDestroyed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSizeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBrowserCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBrowserEventMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBrowserNaviAdvance(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT	OnBrowserNavi(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	BOOL m_bNeedUIActive;
	int m_nPrepareCount;
	BOOL m_bInPrepareBlankPage;
	BOOL	m_bNormalQuit;
	BOOL	m_bBusy;
	BOOL	m_bAddressIsPathFile;
	BOOL	m_bIsAboutBlank;
	BOOL	m_bPauseSafeMode;
	RECT    m_rcClose;
	TCHAR   m_szHint[MAX_PATH];
	POINT	m_pointLBTNDown;
	BOOL m_bCreateForNewWindow2;
	////////////////////////////////////////////////////////////////////
	// Variants
	UINT				m_uState;
	IWebBrowser2 *		m_pWebBrowser;
	int					m_nThreadStyle;

	IOleObject   *		m_pOleObject;
	IDispatch    *		m_pDispatch;

	IStream		 *		m_pStream;
	IStream		 *		m_pStream2;
	//
	BOOL				m_bEventCanFree;
	CBrowserEvent *		m_pBrowserEvent;
	CBrowserWnd   *     m_pParentBrowser;
	// Thread
	HANDLE				m_hThread;
	UINT				m_nThreadID;
	WORD				m_wID;
	RECT				m_Rect;
	IOleClientSite *	m_pClientSite;
	IStorage *			m_pStorage;

	CWebBrowser		*	m_pWebBrowserHost;
	HWND				m_hWndRoot;
	HWND				m_hWndParent;

	CBrowserWndHint		m_HintWnd;


	BOOL m_bBackEnable;
	BOOL m_bForwardEnable;


	////////////////////////////////////////////////////////////////////
	// Functions
	void Clear();
	BOOL IsRefferUrl( LPCTSTR pszUrl );
	void Forward();
	void Back();
	virtual BOOL InternalNavigate( LPCTSTR pszUrlPage, BOOL bNew );
	virtual LRESULT OnCommand( WPARAM wParam, LPARAM lParam, BOOL &bHandled, BOOL bChained );
	virtual BOOL DoCreate();
	virtual void DoDestroy();
	void DoActive( UINT uState );

	void UpdateBrowserRect(  int nWidth, int nHeight  );
	void ChangeBrowserSize( LPRECT lpRect );
	BOOL QueuePopupMenu( LPRECT lpRect );

	virtual LRESULT OnBrowserWndOperation(WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	IDispatch * OnNewWindow2();
	void OnSize( LPARAM lParam );
	void OnBrowserSize( WPARAM wParam, LPARAM lParam );
	static unsigned int __stdcall BrowserThreadProc( void * pArgu );
};