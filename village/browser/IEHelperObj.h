// IEHelperObj.h : Declaration of the CIEHelperObj
#pragma once

#include <Exdisp.h>
#include <exdispid.h>
#include <Mshtml.h>
#include <Mshtmdid.h>
#include <comdef.h>

/////////////////////////////////////////////////////////////////////////////
// CIEHelperObj
class CIEHelperObj
{	
public:
	CIEHelperObj();
	CIEHelperObj(IWebBrowser2* pWebBrowser);
	~CIEHelperObj();
public:
	static CIEHelperObj& CIEHelperObj::GetInstance()
	{
		static CIEHelperObj This;
		return This;
	}
	void InitWebBrowser(IWebBrowser2* pWebBrowser)
	{
		m_pWebBrowser = pWebBrowser;
	}

	void BeginPostLogin(LPCTSTR strLoginHtml);
	void BeginScriptLogin(LPCTSTR strLoginHtml);
	void RunScript();
	void BeginQQLogin(LPCTSTR strLoginHtml);

private:
	static DWORD WINAPI LoginPostThread(LPVOID lParam);
	static DWORD WINAPI LoginScriptThread(LPVOID lParam);
	static void SyncPostRequest(LPCSTR lpMethod, LPCSTR lpHost, LPCSTR lpFile, LPCSTR lpReferrer, LPCSTR lpData, std::string & strResult);
	static bool FindFlag(const std::string &strMain, const std::string &strStart, const std::string &strEnd, std::string &strRet);


private:
	// 控制BHO中的事件只执行一次
    BOOL m_bInit;
	HANDLE m_hThread;
	static BOOL m_bRunning;

	std::string m_strScript;
	IWebBrowser2* m_pWebBrowser;
//	CString m_strLoginHtml;

	//CJsInjectManager m_jsInjectManager;
};