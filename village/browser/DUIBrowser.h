#pragma once
#include "DUIElement.h"
#include "../Browser/BrowserWnd.h"
#include "../Browser/WebBrowser.h"
#include "../Browser/WebBrowserEx.h"
#include "../Browser/BrowserEvent.h"
#include "../Browser/ExternalDispatch.h"
#include "../Browser/IEHelperObj.h"



class CDUIBrowser : public CDUIElement

	{
public:
	CDUIBrowser(HWND father);
	~CDUIBrowser(void);

	void SetPos(WTL::CRect rc);
	void SetPos(CPoint pt, CSize sz);
	void SetVisible(BOOL visible);
	void CreateElement(XmlElementPtr element);
	void CreateElement(LPCTSTR lpszXmlBrowser);
	void OnPaint(HDC hDC);
	void OnSizeChanged(DUI_Size wndChangingSize, int w, int h);
	void SetFixedSize(BOOL bSize);
	void Navigate(LPCTSTR lpszUrl, BOOL bActive = TRUE);
	CBrowserWnd* GetBrowserObject()
	{
		return m_pBrowserWnd;
	}
	IWebBrowser2* GetSafeBrowser()
	{
		if(m_pBrowserWnd)
			return m_pBrowserWnd->GetSafeBrowser();

		return NULL;
	}

	void Close();

	CWebBrowserEx * AfxGetBrowserExtend();
protected:
	COLORREF _borderColor;
	HBITMAP _bmpBackground;

	// 浏览器窗口
	CBrowserWnd *   m_pBrowserWnd;
	HWND m_hWnd;
	BOOL _bSize;

	CWebBrowser m_WebBrowser;
	CWebBrowserEx m_WebBrowserEx;
	CExternalDispatch m_External;
};

//设定元素是否显示
inline void CDUIBrowser::SetVisible(BOOL visible)
{
	if (_visible != visible)
	{
		_visible = visible;
		if(_visible)
		{
			if(m_pBrowserWnd)
			{
				m_pBrowserWnd->ShowWindow(SW_SHOW);
			//	m_pBrowserWnd->Active(BS_ACTIVE);
			}
		}
		else
		{
			if(m_pBrowserWnd)
			{
				m_pBrowserWnd->ShowWindow(SW_HIDE);
			//	m_pBrowserWnd->Active(BS_HIDDEN);
			}

		}

		RaiseInvalidate(TRUE);
	}
}

inline void CDUIBrowser::SetPos(CPoint pt, CSize sz)
{

	_location.x = pt.x;
	_location.y = pt.y;
	_size.cx = sz.cx;
	_size.cy = sz.cy;


	
	CString strDebug;
	strDebug.Format(_T("CDUIBrowser::SetPos(w = %d, h = %d)"), _size.cx, _size.cy);

	//给列表发送消息位置变化
	if(m_pBrowserWnd)
		m_pBrowserWnd->SetWindowPosSafe(_location.x + 1, _location.y + 1, _size.cx - 2, _size.cy - 2,  SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
		
	//::MoveWindow(m_pBrowserWnd->m_hWnd, _location.x + 1, _location.y + 1, _size.cx - 2, _size.cy - 2, FALSE);
}

inline void CDUIBrowser::SetPos(WTL::CRect rc)
{
	_location.x = rc.left;
	_location.y = rc.top;
	_size.cx = rc.Width();
	_size.cy = rc.Height();
	
	CString strDebug;
	strDebug.Format(_T("CDUIBrowser::SetPos(w = %d, h = %d)"), rc.Width(), rc.Height());

	//给列表发送消息位置变化
	if(m_pBrowserWnd)
		m_pBrowserWnd->SetWindowPosSafe(_location.x + 1, _location.y + 1, _size.cx - 2, _size.cy - 2,  SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
	//::SetWindowPos(m_pBrowserWnd->m_hWnd, HWND_TOP, _location.x + 1, _location.y + 1, _size.cx - 2, _size.cy - 2, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOOWNERZORDER);
	//::MoveWindow(m_pBrowserWnd->m_hWnd, _location.x + 1, _location.y + 1, _size.cx - 2, _size.cy - 2, FALSE);
}

//是否自动变化大小
inline void CDUIBrowser::SetFixedSize(BOOL bSize)
{
	_bSize = bSize;
}


