#include "stdafx.h"
#include "BrowserEvent.h"
#include "BrowserWnd.h"
#include "WebBrowser.h"
//#include "../Util/UtilApi.h"

CBrowserEvent::CBrowserEvent()
{
	Clear();
}

void CBrowserEvent::Clear()
{
	m_pBrowserWnd = NULL;
	m_lSecurityIcon = 0;
	m_pCPContainer = NULL;
	m_pCP = NULL;
	m_bCanSuspend = TRUE;
}

STDMETHODIMP_(ULONG)CBrowserEvent::AddRef(THIS)
{
	return 1;
}

STDMETHODIMP_(ULONG)CBrowserEvent::Release(THIS)
{
	return 1;
}

STDMETHODIMP CBrowserEvent::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	if( IsEqualIID(riid, IID_IUnknown) )
		*ppvObj = this;
	else if( IsEqualIID(riid, IID_IDispatch) )
		*ppvObj = (IDispatch *)this;
	else
	{
		*ppvObj = 0;
		return E_NOINTERFACE;
	}

	return S_OK;
}

STDMETHODIMP CBrowserEvent::GetTypeInfoCount( UINT *pctinfo )
{
	return E_NOINTERFACE;
}

STDMETHODIMP CBrowserEvent::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
{
	return E_NOINTERFACE;
}

STDMETHODIMP CBrowserEvent::GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId )
{
	return E_NOINTERFACE;
}

HRESULT CBrowserEvent::Connect( CBrowserWnd * pBrowserWnd )
{
	HRESULT hr = S_FALSE;
	assert( m_pBrowserWnd == NULL );

	m_pBrowserWnd = pBrowserWnd;

	if( m_pBrowserWnd->m_pWebBrowser )
	{
		m_pBrowserWnd->m_pWebBrowser->QueryInterface( IID_IConnectionPointContainer, (void**)&m_pCPContainer );
		if( m_pCPContainer )
		{
			m_pCPContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pCP);
			if( m_pCP )
				hr = m_pCP->Advise( this, &m_dwCookie );
		}
	}
	return hr;
}

HRESULT CBrowserEvent::DisConnect()
{
	assert( m_pBrowserWnd != NULL );

	HRESULT hr = S_FALSE;
	if( m_pCPContainer )
	{
		if( m_pCP )
		{
			hr = m_pCP->Unadvise( m_dwCookie );
			m_pCP->Release();
		}
		else
		{
			assert( FALSE );
		}
		m_pCPContainer->Release();
	}

	Clear();
	return hr;
}

STDMETHODIMP CBrowserEvent::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, 
	VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr )
{
 
	DWORD dwID = 0;
	assert( m_pBrowserWnd );

	if (!pDispParams )
		return E_INVALIDARG;

#ifdef _DEBUG
	if( m_pBrowserWnd->m_nThreadID != 0 )
	{
		assert( GetCurrentThreadId() == m_pBrowserWnd->m_nThreadID );
	}
#endif

	switch( dispIdMember )
	{
	case DISPID_NEWWINDOW2:
		{
	
		}
		break;

	case DISPID_NEWWINDOW3:
		{

			// *(pDispParams->rgvarg[4].ppdispVal) = m_pBrowserWnd->GetDispatch();
			DWORD dwFlags = pDispParams->rgvarg[2].intVal;
			LPCTSTR lpszUrl = OLE2T(pDispParams->rgvarg[0].bstrVal);
            // NWMF_USERINITED | NWMF_OVERRIDEKEY 
 			//if ((dwFlags & 0x2) || (dwFlags & 0x8))
 			//{
                *(pDispParams->rgvarg[3].pboolVal) = VARIANT_TRUE;
				
				m_pBrowserWnd->Navigate(lpszUrl);
				//ShellExecute(NULL,_T("open"),lpszUrl,NULL,NULL,SW_NORMAL);
			//}
            //else
			//{
			//	Util::Tools::OpenBrowser(lpszUrl);
			//}
//                 *(pDispParams->rgvarg[4].ppdispVal) = m_pBrowserWnd->OnNewWindow2();

			//*(pDispParams->rgvarg[0].pboolVal) = VARIANT_TRUE;*/
		}
		break;

	case DISPID_SETSECURELOCKICON:
		if( m_pBrowserWnd->m_hWndBrowser == NULL && m_pBrowserWnd->IsActivate() )
			m_pBrowserWnd->InitObject();
		m_lSecurityIcon = pDispParams->rgvarg[0].lVal;
		break;

	case DISPID_TITLECHANGE:
        {
            if (pDispParams->cArgs < 1)
               return E_FAIL;
            CComBSTR bstrTitle = pDispParams->rgvarg[0].bstrVal;
            if (!bstrTitle)
                return E_FAIL;
            m_pBrowserWnd->SetWindowText((LPCTSTR)bstrTitle);

//			::PostMessage(global._hMainWnd, WEBBROWSE_TITLE_CHANGE, (WPARAM)StrDup((LPCTSTR)bstrTitle), 0);
        }
		break;

	case DISPID_STATUSTEXTCHANGE:			
		break;
	
	case DISPID_DOWNLOADCOMPLETE:
		break;

	case DISPID_DOWNLOADBEGIN:
		break;

	case DISPID_DOCUMENTCOMPLETE:
		//通知主界面文档已经下载完毕
		{
//			::PostMessage(global._hMainWnd, WEBBROWSE_DOCUMENT_COMPLETE, (WPARAM)m_pBrowserWnd, 0);
		}
		break;
		
	case DISPID_BEFORENAVIGATE2:

		break;

	case DISPID_WINDOWCLOSING:
		{
			//BOOL bChildWnd = pDispParams->rgvarg[1].boolVal;
			//if (bChildWnd)
			{
				*(pDispParams->rgvarg[0].pboolVal) = VARIANT_TRUE;
				m_pBrowserWnd->Home();
			}
		}
		break;
		
	case DISPID_NAVIGATECOMPLETE2:
	    {
            if (pDispParams->rgvarg[1].pdispVal && pDispParams->rgvarg[1].vt == VT_DISPATCH)
            {
                if (m_pBrowserWnd->IsTopWebBrowser( pDispParams->rgvarg[1].pdispVal))
                {
  //                  TCHAR szUrl[MAX_URL_LEN]=_T("");
 //                   if(m_pBrowserWnd->GetBrowserUrl(szUrl, SIZEOF(szUrl)) && _T('\0')!=szUrl[0])
//                        ::PostMessage(global._hMainWnd, WEBBROWSE_URL_CHANGE, (WPARAM)StrDup(szUrl), 0);
                }
            }
			m_pBrowserWnd->UIActivate();
        }

		break;

	case DISPID_COMMANDSTATECHANGE:
		{
			long Command = pDispParams->rgvarg[1].lVal;
			BOOL bEnable = pDispParams->rgvarg[0].boolVal;
			if ( Command == CSC_NAVIGATEBACK) 
			{
				m_pBrowserWnd->m_bBackEnable = bEnable;				
			}		
			else if (Command == CSC_NAVIGATEFORWARD)
			{
				m_pBrowserWnd->m_bForwardEnable = bEnable;
			}

//			::PostMessage(global._hMainWnd, WEBBROWSE_COMMAND_STATE_NOTIFY, 0, 0);
		}
		break;

	case DISPID_PROGRESSCHANGE:
		{
			// lCurrent == -1 mean complete
			long lMax = pDispParams->rgvarg[0].lVal;
			long lCurrent = pDispParams->rgvarg[1].lVal; 
			
			if( lCurrent != -1 )
			{
				if( lMax != 0 )
				{
					lCurrent = lCurrent*100/lMax;
					if( lCurrent > 100 )
						lCurrent = 100;
					if( lCurrent < 0 )
						lCurrent = 0;
					lMax = 100;
				}
				else
					lCurrent = lMax = 100;
				
			}
			assert( lCurrent <= lMax );

		//	::PostMessage(global._hMainWnd, WEBBROWSE_PROGRESS_CHANGE, lMax, lCurrent);
		}
		break;
	}

	return S_OK;
}
