#include "stdafx.h"
#include "ExternalDispatch.h"
#include <mshtml.h>
#include <mshtmhst.h>
#include <Mshtml.h>
//#include "ui/UIFlashGameWindow.h"
//#include "database/GameDBManager.h"

// 函数 ID 定义
#define  FUNCTION_GET_XMLHTTPOBJ							101
#define  FUNCTION_GET_LOGINCODE								102
#define  FUNCTION_GET_LOGINURL								103
#define  FUNCTION_QQ_LOGIN									104
#define  FUNCTION_SET_LOGINSCIPT							105
#define  FUNCTION_ADD_EXPLAIN								106

						
#ifdef _DEBUG
#define  FUNCTION_JSTRACE									201
#endif

#define  FUNCTION_FLASH_SUGGEST								300

CExternalDispatch::CExternalDispatch()
{
	
}


STDMETHODIMP_(ULONG)CExternalDispatch::AddRef(THIS)
{
	return 1;
}

STDMETHODIMP_(ULONG)CExternalDispatch::Release(THIS)
{
	return 1;
}

STDMETHODIMP CExternalDispatch::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
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

STDMETHODIMP CExternalDispatch::GetTypeInfoCount( UINT *pctinfo )
{
	return E_NOINTERFACE;
}

STDMETHODIMP CExternalDispatch::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
{
	return E_NOINTERFACE;
}

STDMETHODIMP CExternalDispatch::GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId )
{
	static METHODMAP s_methodMap[] = {
		{ OLESTR("SetLoginScript"),				FUNCTION_SET_LOGINSCIPT},
		{ OLESTR("GetXMLHttpObj"),				FUNCTION_GET_XMLHTTPOBJ},
		{ OLESTR("GetLoginCode"),				FUNCTION_GET_LOGINCODE},
		{ OLESTR("GetLoginURL"),				FUNCTION_GET_LOGINURL},
		{ OLESTR("QQLogin"),					FUNCTION_QQ_LOGIN},
		{ OLESTR("SoftMgr_Notify"),				FUNCTION_FLASH_SUGGEST},
		{ OLESTR("AddExplain"),					FUNCTION_ADD_EXPLAIN}

#ifdef _DEBUG
		,{ OLESTR("JsTrace"),							FUNCTION_JSTRACE
		}
#endif
	};
	
	// 将调用过来的函数名转换为我们的函数 ID
	UINT n = 0;
	for(; n < cNames; n++)
	{
		bool bFound = false;
		for(int i = 0; i < sizeof(s_methodMap)/sizeof(s_methodMap[0]); i++)
		{
			if(wcscmp(rgszNames[n], s_methodMap[i].pstr) == 0)
			{
				rgDispId[n] = s_methodMap[i].id;
				bFound = true;
				break;
			}
		}
		
		if(bFound)
			;
		else
			break;
	}
	
	if(n == cNames)
		return S_OK;
	else
		return E_FAIL;

}

STDMETHODIMP CExternalDispatch::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, 
	VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr )
{
	DWORD dwID = 0;

	if (!pDispParams )
		return E_INVALIDARG;

	HRESULT hr = NULL;
	UINT count = pDispParams->cArgs;
	switch(dispIdMember)
	{
	case FUNCTION_GET_XMLHTTPOBJ:
		{
			if(pVarResult && count >= 1)
			{
				IDispatch* pWindow = pDispParams->rgvarg[0].pdispVal;
				if (CheckWindowSID(pWindow))
				{
					// 返回QQ的登录码
					CComPtr<IDispatch> spDisp;
					CoCreateInstance(CLSID_XMLHTTPRequest, NULL, CLSCTX_ALL, IID_IDispatch, (void **) &spDisp);

					CComQIPtr<IXMLHttpRequest> spXMLHttpRequest = spDisp;
					if (spXMLHttpRequest)
					{
						pVarResult->vt = VT_DISPATCH;
						pVarResult->pdispVal = spXMLHttpRequest;
						pVarResult->pdispVal->AddRef();
					}
				}
			}	
			hr = S_OK;
		}
		break;
	case FUNCTION_GET_LOGINCODE:
		{
			if(pVarResult && count >= 1)
			{
				IDispatch* pWindow = pDispParams->rgvarg[0].pdispVal;
				if (CheckWindowSID(pWindow))
				{
					// 获得QQ的登录脚本
					/*
					LPCTSTR lpszLoginCode = CGlobal::GetLoginCode();
					pVarResult->bstrVal = CString(lpszLoginCode).AllocSysString();
					pVarResult->vt = VT_BSTR;
					*/
				}
			}	
			hr = S_OK;
		}
		break;
	case FUNCTION_GET_LOGINURL:
		{
			if(pVarResult && count >= 1)
			{
				IDispatch* pWindow = pDispParams->rgvarg[0].pdispVal;
				if (CheckWindowSID(pWindow))
				{
					/*
					// 获得QQ的登录地址
					LPCTSTR lpszLoginURL = global._szStartUrl;
					pVarResult->bstrVal = CString(lpszLoginURL).AllocSysString();
					pVarResult->vt = VT_BSTR;
					*/
				}
			}	
			hr = S_OK;
		}
		break;
	case FUNCTION_QQ_LOGIN:
		{
			if(count >= 1)
			{
				IDispatch* pWindow = pDispParams->rgvarg[0].pdispVal;
				if (CheckWindowSID(pWindow))
				{
					// 用idispatchd对象登录QQ
					static LPCSTR s_lpszJs =  "_360_XMPHttpReq=external.GetXMLHttpObj(window); try{window.LoginQQ(); }catch(e){ alert(e.description); }";
					
					CComQIPtr<IHTMLWindow2, &IID_IHTMLWindow2> ptrWindow = pWindow;
					if (ptrWindow)
					{
						CComVariant varResult;
						ptrWindow->execScript(CComBSTR(s_lpszJs), CComBSTR("JavaScript"), &varResult);
					}
				}
			}	
			hr = S_OK;
		}
		break;
	case FUNCTION_SET_LOGINSCIPT:
		{
			if(count >= 2)
			{
				/*
				IDispatch* pWindow = pDispParams->rgvarg[1].pdispVal;
				if ( CheckWindowSID(pWindow))
				{
					BSTR bstrUrl = pDispParams->rgvarg[0].bstrVal;

					//获取用户名和密码数据
					AccountItem accountInfo;
					CGameDBManager::GetInstance().GetAccountInfo(global._nAccountID, accountInfo);

					// 把登陆的URL设置过去，用于script方式的自动登录
					TCHAR szScript[INTERNET_MAX_URL_LENGTH] = {0};
					//_sntprintf(szScript, SIZEOF(szScript) - 1, _T("var sw=document.getElementById('switch');if(sw && sw.innerText.indexOf('使用其他号码登录') >= 0){switchpage();}document.getElementById('u').value='%s';document.getElementById('p').value='%s';"), CGlobal::GetOwnUserName(), CGlobal::GetOwnPwd());
					_sntprintf_s(szScript, SIZEOF(szScript), _TRUNCATE, _T("var sw=document.getElementById('switch');if(sw && sw.innerText.indexOf('使用其他号码登录') >= 0){switchpage();}var $u = document.getElementById('u');var $p = document.getElementById('p');var $b=document.getElementById('login_button'); var $verify=document.getElementById('verifycode'); var $tm=setTimeout(function(){clearTimeout($tm);$u.value='%s';$p.value='%s';$b.click(); if($verify)$verify.focus();}, 400);"), accountInfo.strUserName, accountInfo.strPassword);					
					CGlobal::SetRedirectUrl(bstrUrl);
					CGlobal::SetRedirectScript(szScript);
					::PostMessage(global._hMainWnd, WM_WEBGAME_BROWSER_REDIRECT_NAVI, 0, 0);
				}
				*/
			}
			hr = S_OK;
		}
		break;
	case FUNCTION_FLASH_SUGGEST:
		{
			if(count >= 2)
			{
				
				BSTR bstr = pDispParams->rgvarg[0].bstrVal; //app_download
				TCHAR *pChar =_tcsstr(bstr,_T("app_download"));
				if(pChar != NULL)
				{
					pChar =wcsstr(bstr,_T("|"));
					if(pChar)
					{
						++pChar;
						long Appid = _ttol(pChar);
					}
				}
				
			}
		}
		break;
	case  FUNCTION_ADD_EXPLAIN:
		{
			if(count == 1)
			{
				//int nVal = pDispParams->rgvarg[0].intVal;
				//::PostMessage(global._hMainWnd,WM_FLASH_EXPLAIN,(WPARAM)nVal,0);

			}
		}
	default:
		break;
	}


	return S_OK;
}

bool CExternalDispatch::CheckWindowSID(IDispatch* pWindow)
{
	bool bRet = false;
	if (pWindow)
	{
		CComQIPtr<IHTMLWindow2, &IID_IHTMLWindow2> ptrWindow = pWindow;
		if (ptrWindow)
		{
			CComPtr<IHTMLDocument2> ptrDoc;
			if (SUCCEEDED(ptrWindow->get_document(&ptrDoc)) && ptrDoc)
			{
				CComBSTR bstrUrl;
				if (SUCCEEDED(ptrDoc->get_URL(&bstrUrl)) && bstrUrl)
				{
					if (_tcsncmp(bstrUrl, _T("res://"), 6) == 0)
						bRet = true;
				}
			}
		}
	}
	return bRet;
}
