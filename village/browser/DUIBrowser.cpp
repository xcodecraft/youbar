#include "StdAfx.h"
#include "DUIBrowser.h"



CBrowserEvent * __stdcall AfxCreateBrowserEvent()
{
	return new CBrowserEvent;
}

CBrowserWnd * __stdcall AfxCreateBrowserWnd()
{
	return new CBrowserWnd;
}

CWebBrowserEx * CDUIBrowser::AfxGetBrowserExtend()
{
	m_WebBrowserEx.SetExternal(&m_External);
	return &m_WebBrowserEx;
}


CDUIBrowser::CDUIBrowser(HWND father)
{
	_host = father;
	_handleMouse = FALSE;
	_class = _T("DUIBrowser");
	m_pBrowserWnd = NULL;
	_bSize = TRUE;
	m_hWnd = NULL;
}

CDUIBrowser::~CDUIBrowser(void)
{
}

void CDUIBrowser::Navigate(LPCTSTR lpszUrl, BOOL bActive)
{
	if(m_pBrowserWnd)
	{
		m_pBrowserWnd->Navigate(lpszUrl);
		if(bActive)
			m_pBrowserWnd->Active( BS_ACTIVE );
	}
}

//��������
void CDUIBrowser::CreateElement(XmlElementPtr element)
{
	//��ȡ����������Ϣ
	GetElementStyle(element);

	_borderColor = GetRGBAttribute(element, "border");
	_bmpBackground = GetBitmapAttribute(element, "background"); 

	//�������������
	DUI_Rect rcBrowser(_location, _size);
	rcBrowser.left   += 1;
	rcBrowser.top    += 1;
	rcBrowser.right  -= 1;
	rcBrowser.bottom -= 1;
	m_WebBrowser.Init( MULTI_THREAD, AfxGetBrowserExtend(), AfxCreateBrowserWnd, AfxCreateBrowserEvent, TRUE );
	m_pBrowserWnd = m_WebBrowser.CreateBrowser(_host, &rcBrowser, FALSE, NULL );
	if(m_pBrowserWnd)
		m_hWnd = m_pBrowserWnd->m_hWnd;
}

//��������
void CDUIBrowser::CreateElement(LPCTSTR lpszXmlBrowser)
{
	CXmlHelper xml;
	if(xml.LoadString(lpszXmlBrowser))
	{
		//XML�ĸ��ڵ�
		XmlElementPtr element = xml.GetRoot();

		//��ȡ����������Ϣ
		GetElementStyle(element);

		_borderColor = GetRGBAttribute(element, "border");
		_bmpBackground = GetBitmapAttribute(element, "background"); 

		//�������������
		DUI_Rect rcBrowser(_location, _size);
		rcBrowser.left   += 1;
		rcBrowser.top    += 1;
		rcBrowser.right  -= 1;
		rcBrowser.bottom -= 1;
		m_WebBrowser.Init( MULTI_THREAD, AfxGetBrowserExtend(), AfxCreateBrowserWnd, AfxCreateBrowserEvent, TRUE );
		m_pBrowserWnd = m_WebBrowser.CreateBrowser(_host, &rcBrowser, FALSE, NULL );
		if(m_pBrowserWnd)
			m_hWnd = m_pBrowserWnd->m_hWnd;
	}
}


void CDUIBrowser::OnPaint(HDC hDC)
{
	if (_visible)
	{
		if(_title.size() > 0)
		{
			DUI_Rect rcText(_location, _size);
			CDUIGDIResource::GetInstance().GDI_DrawText(hDC, _title.c_str(), &rcText);
		}

		CRect rcBrowser(_location, _size);
		//�軭����
		CDUIGDIResource::GetInstance().GDI_DrawRectangle(hDC, rcBrowser, _borderColor, _borderColor);

		if(_bmpBackground)
		{
			BITMAP bmpBackInfor;
			GetObject (_bmpBackground, sizeof (bmpBackInfor), &bmpBackInfor);
			
			//�軭����ͼƬ
			CDUIGDIResource::GetInstance().GDI_DrawImage(hDC, _bmpBackground, _location.x, _location.y, bmpBackInfor.bmWidth, bmpBackInfor.bmHeight);
		}
	}
}

void CDUIBrowser::OnSizeChanged(DUI_Size wndChangingSize, int w, int h)
{
	if(!_bSize)
		return;

	CDUIElement::OnSizeChanged(wndChangingSize, w, h);
	
	if(m_pBrowserWnd)
	{
		//���༭������Ϣλ�ñ仯
		::SetWindowPos(m_pBrowserWnd->m_hWnd, HWND_TOP, _location.x + 1, _location.y + 1, _size.cx - 2, _size.cy - 2, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOOWNERZORDER);
	}
	//::MoveWindow(m_pBrowserWnd->m_hWnd, _location.x + 1, _location.y + 1, _size.cx - 2, _size.cy - 2, FALSE);
}

void CDUIBrowser::Close()
{
	if(::IsWindow(m_hWnd))
	{
		DWORD dwRes = 0;
		if(!::SendMessageTimeout(m_hWnd, WM_BROWSER_QUIT, 0, 0, SMTO_ABORTIFHUNG|SMTO_NORMAL, 2000, &dwRes))
		{
			//��ʱ��
			
		}
	}
}
