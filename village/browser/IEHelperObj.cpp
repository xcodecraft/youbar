// IEHelperObj.cpp : Implementation of CIEHelperObj
#include "stdafx.h"
#include <string>
#include <Mshtml.h>
#include "IEHelperObj.h"
//#include "hook/JsInjectManager.h"
#include <time.h>

#define USER_AGENT												"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0; Trident/4.0; .NET CLR 2.0.50727; Media Center PC 5.0; .NET CLR 3.0.04506; .NET CLR 3.5.21022)"
#define REQUEST_HEADER_ACCEPT_LANGUAGE_AND_CONTENT_TYPE			"Accept-Language: zh-CN\r\n\r\nContent-Type: application/x-www-form-urlencoded"
#define REQUEST_HEADER_ACCEPT_LANGUAGE_AND_CONTENT_TYPE_LENGTH	strlen(REQUEST_HEADER_ACCEPT_LANGUAGE_AND_CONTENT_TYPE)


BOOL CIEHelperObj::m_bRunning = FALSE;
/////////////////////////////////////////////////////////////////////////////
// CIEHelperObj
CIEHelperObj::CIEHelperObj() : m_hThread(NULL)//,m_jsInjectManager(this)
{
    m_bInit = FALSE;
	m_bRunning = FALSE;
}

CIEHelperObj::~CIEHelperObj()
{
	//结束时关闭线程
	if (m_hThread)
	{
		m_bRunning = FALSE;
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 100))
			TerminateThread(m_hThread, -1);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// CIEHelperObj
//开始进行Post数据提交处理
void CIEHelperObj::BeginPostLogin(LPCTSTR strLoginHtml)
{
	//m_strLoginHtml = strLoginHtml;
	m_bRunning = TRUE;
	m_hThread = CreateThread(NULL, 0, LoginPostThread, (LPVOID)this, 0, NULL);
}

//开始进行脚本登录处理
void CIEHelperObj::BeginScriptLogin(LPCTSTR strLoginHtml)
{
	//m_strLoginHtml = strLoginHtml;
	m_bRunning = TRUE;

	// 获得FileMapping中的数据
	TCHAR szLoginInfo[1024] = {0};
	USES_CONVERSION;
	CHAR* pchInfo = T2A(strLoginHtml);
	
	std::string strScript;
	std::string strUrl;
	FindFlag(pchInfo, "360_LOGIN_SCRIPT=", "\n", strScript);
	FindFlag(pchInfo, "360_LOGIN_URL=", "\n", strUrl);
	if(strScript.size() > 0 && strUrl.size() > 0)
	{
		//m_strScript = strScript;
		//CGlobal::SetRedirectScript(A2T(strScript.c_str()));
		//CGlobal::SetRedirectUrl(A2T(strUrl.c_str()));
		//::PostMessage(global._hMainWnd, WM_WEBGAME_BROWSER_REDIRECT_NAVI, 0, 0);
	}

	//m_hThread = CreateThread(NULL, 0, LoginScriptThread, (LPVOID)this, 0, NULL);
}

DWORD WINAPI CIEHelperObj::LoginPostThread(LPVOID lParam)
{
	CIEHelperObj* pThis = (CIEHelperObj*)lParam;
	if (pThis)
	{
		// 获得FileMapping中的数据
		/*
		TCHAR szLoginInfo[1024] = {0};
		USES_CONVERSION;
		CHAR* pchInfo = T2A(pThis->m_strLoginHtml);
		std::string strPostData;
		std::string strFile;
		std::string strHost;
		std::string strTargetRef; //目标重定向网址
		std::string strResult;
		std::string strRef;
		FindFlag(pchInfo, "360HTTP_REQUEST_HEAD_PostData=", "\n", strPostData);
		FindFlag(pchInfo, "360HTTP_REQUEST_HEAD_Target=", "\n", strFile);
		FindFlag(pchInfo, "360HTTP_REQUEST_HEAD_Host=", "\n", strHost);
		FindFlag(pchInfo, "360HTTP_REQUEST_HEAD_Ref=", "\n", strRef);
		FindFlag(pchInfo, "360HTTP_REQUEST_HEAD_Referrer=", "\n", strTargetRef);
		FindFlag(pchInfo, "360HTTP_RESPONSE_HEAD_Result=", "\n", strResult);

		std::string strRet;
		if(strPostData.size() > 0 && strFile.size() > 0 && strHost.size()  > 0)
		{
			pThis->SyncPostRequest("POST", strHost.c_str(), strFile.c_str(), strRef.size() > 0 ? strRef.c_str() : NULL, strPostData.c_str(), strRet);
			//CGlobal::SetRedirectUrl(A2T(strTargetRef.c_str()));
			//::PostMessage(global._hMainWnd, WM_WEBGAME_BROWSER_REDIRECT_NAVI, 0, 0);
		}
		*/
	}
	return 0;
}

DWORD WINAPI CIEHelperObj::LoginScriptThread(LPVOID lParam)
{
	CIEHelperObj* pThis = (CIEHelperObj*)lParam;
	if (pThis)
	{
		// 获得FileMapping中的数据
		/*
		TCHAR szLoginInfo[1024] = {0};
		USES_CONVERSION;
		CHAR* pchInfo = T2A(pThis->m_strLoginHtml);

		std::string strScript;
		std::string strUrl;
		FindFlag(pchInfo, "360_LOGIN_SCRIPT=", "\n", strScript);
		FindFlag(pchInfo, "360_LOGIN_URL=", "\n", strUrl);
		if(strScript.size() > 0 && strUrl.size() > 0)
		{
			//pThis->m_strScript = strScript;
			CGlobal::SetRedirectScript(A2T(strScript.c_str()));
			CGlobal::SetRedirectUrl(A2T(strUrl.c_str()));
			::PostMessage(global._hMainWnd, WM_WEBGAME_BROWSER_REDIRECT_NAVI, 0, 0);
		}
		*/
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// 运行登录脚本
// 
void CIEHelperObj::RunScript()
{
	/*
	LPCTSTR lpScript = CGlobal::GetRedirectScript();
	CComPtr<IDispatch> ptrDisp;
	if(lpScript && lpScript[0] && m_pWebBrowser && SUCCEEDED(m_pWebBrowser->get_Document(&ptrDisp)) && ptrDisp)
	{
		CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> ptrDoc2 = ptrDisp;
		CComQIPtr<IHTMLWindow2, &IID_IHTMLWindow2> ptrWindow;
		if(ptrDoc2 && SUCCEEDED(ptrDoc2->get_parentWindow(&ptrWindow)) && ptrWindow)
		{
			CComVariant varReturn(VT_EMPTY);
			ptrWindow->execScript(CComBSTR(lpScript), CComBSTR(L"JavaScript"), &varReturn);
			CGlobal::SetRedirectScript(_T(""));
		}
	}
	*/
}

bool CIEHelperObj::FindFlag(const std::string &strMain, const std::string &strStart, const std::string &strEnd, std::string &strRet)
{
    int start = strMain.find(strStart);
    if (start != std::string::npos)
    {
        start += strStart.size();
		
        if (strEnd == "")
        {
            strRet = strMain.substr(start);
            return strRet != "";
        }
        int end = strMain.find(strEnd, start);
        if (end != std::string::npos)
        {
            strRet = strMain.substr(start, end - start);
            return true;
        }
    }
    return false;
}

void CIEHelperObj::SyncPostRequest(LPCSTR lpMethod, LPCSTR lpHost, LPCSTR lpFile, LPCSTR lpReferrer, LPCSTR lpData, std::string & strResult)
{
	HINTERNET hInternet = InternetOpenA(USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInternet)
	{
		HINTERNET hConnect = InternetConnectA(hInternet, lpHost, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
		if (hConnect)
		{
			LPCSTR AcceptTypes[] = { NULL, "*/*", NULL };
			HINTERNET hRequest = HttpOpenRequestA(hConnect, lpMethod, lpFile, "HTTP/1.1", lpReferrer, AcceptTypes, 0, NULL);
			if (hRequest)
			{
				if (HttpSendRequestA(hRequest, REQUEST_HEADER_ACCEPT_LANGUAGE_AND_CONTENT_TYPE, REQUEST_HEADER_ACCEPT_LANGUAGE_AND_CONTENT_TYPE_LENGTH, (void *)lpData, strlen(lpData)))
				{
					// 只接收 10K 的数据就足够分析了。
					char buffer_read_once[100] = {0};
					DWORD dwBytesToRead = _countof(buffer_read_once);
					DWORD dwBytesRead = 0;
					DWORD dwTotalBytesReaded = 0;
					while (m_bRunning && InternetReadFile(hRequest, buffer_read_once, dwBytesToRead, &dwBytesRead) && dwBytesRead > 0)
					{
						if (dwTotalBytesReaded < 1024 * 10)
						{
							strResult += buffer_read_once;
							dwTotalBytesReaded += dwBytesRead;
						}
						else
							break;
					}
				}
				InternetCloseHandle(hRequest);
			}
			InternetCloseHandle(hConnect);
		}
		InternetCloseHandle(hInternet);
	}
}

