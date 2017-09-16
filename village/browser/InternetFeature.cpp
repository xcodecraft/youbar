#include "stdafx.h"
#include "urlmon.h"
#include "InternetFeature.h"


/************************************************************************
FEATURE_OBJECT_CACHING = 0,
FEATURE_ZONE_ELEVATION = 1,
FEATURE_MIME_HANDLING = 2,
FEATURE_MIME_SNIFFING = 3,
FEATURE_WINDOW_RESTRICTIONS = 4,
FEATURE_WEBOC_POPUPMANAGEMENT = 5,
FEATURE_BEHAVIORS = 6,
FEATURE_DISABLE_MK_PROTOCOL = 7,
FEATURE_LOCALMACHINE_LOCKDOWN = 8,
FEATURE_SECURITYBAND = 9,
FEATURE_RESTRICT_ACTIVEXINSTALL = 10,
FEATURE_VALIDATE_NAVIGATE_URL = 11,
FEATURE_RESTRICT_FILEDOWNLOAD = 12,
FEATURE_ADDON_MANAGEMENT = 13,
FEATURE_PROTOCOL_LOCKDOWN = 14,
FEATURE_HTTP_USERNAME_PASSWORD_DISABLE = 15,
FEATURE_SAFE_BINDTOOBJECT = 16,
FEATURE_UNC_SAVEDFILECHECK = 17,
FEATURE_GET_URL_DOM_FILEPATH_UNENCODED = 18,
FEATURE_ENTRY_COUNT = 19                                                                
************************************************************************/

pfnCoInternetSetFeatureEnabled CInternetFeature::m_funcCoInternetSetFeatureEnabled = NULL;
pfnCoInternetIsFeatureEnabled CInternetFeature::m_funcCoInternetIsFeatureEnabled = NULL;
bool CInternetFeature::sm_bEnabled = false;


CInternetFeature::CInternetFeature()
{
	
}

CInternetFeature::~CInternetFeature()
{

}

void CInternetFeature::Initialize()
{
	HMODULE hMod = LoadLibrary(_T("urlmon.dll"));
	if (hMod)
	{
		m_funcCoInternetSetFeatureEnabled = (pfnCoInternetSetFeatureEnabled)GetProcAddress(hMod, "CoInternetSetFeatureEnabled");
		m_funcCoInternetIsFeatureEnabled = (pfnCoInternetIsFeatureEnabled)GetProcAddress(hMod, "CoInternetIsFeatureEnabled");
		if (m_funcCoInternetSetFeatureEnabled && m_funcCoInternetIsFeatureEnabled)
		{
			// 改成手动控制。
			// SetFeatureEnabled(FEATURE_LOCALMACHINE_LOCKDOWN, SET_FEATURE_ON_PROCESS, TRUE);

			// 该属性没有宏定义，我们直接操作注册表。
			/*if (CAxControl::GetIEVersion() >= 8) 
			{
				DWORD dwValue = 1;
				SHSetValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_SCRIPTURL_MITIGATION"), _T("setask.exe"), REG_DWORD, &dwValue, sizeof(DWORD));
			}*/
			
			sm_bEnabled = true;
		}
		else
		{
			FreeLibrary(hMod);
		}
	}
}

bool CInternetFeature::IsEnabled()
{
	return sm_bEnabled;
}

bool CInternetFeature::EnableSecurityBand(BOOL bEnable)
{
	return SetFeatureEnabled(FEATURE_SECURITYBAND, SET_FEATURE_ON_PROCESS, bEnable);
}

bool CInternetFeature::RestrictActivexInstall(BOOL bEnable)
{
	if (bEnable)
		EnableSecurityBand(bEnable);
	return SetFeatureEnabled(FEATURE_RESTRICT_ACTIVEXINSTALL, SET_FEATURE_ON_PROCESS, bEnable);
}

bool CInternetFeature::IsActivexInstallEnabled()
{
	return IsFeatureEnabled(FEATURE_RESTRICT_ACTIVEXINSTALL, SET_FEATURE_ON_PROCESS);
}

bool CInternetFeature::WebOcPopupManagement(BOOL bEnable)
{
	if (bEnable)
		EnableSecurityBand(bEnable);
	return SetFeatureEnabled(FEATURE_WEBOC_POPUPMANAGEMENT, SET_FEATURE_ON_PROCESS, bEnable);
}

bool CInternetFeature::IsWebPopupManagementEnabled()
{
	return IsFeatureEnabled(FEATURE_WEBOC_POPUPMANAGEMENT, SET_FEATURE_ON_PROCESS);
}

void CInternetFeature::EnableLocalMachineLockDown(BOOL bEnable)
{
	// 这个是本地文件中带有控件或脚本的情况！
	// 暂时开启这个，因为ie自带的对于本地弹窗是没有限制的。
	SetFeatureEnabled(FEATURE_LOCALMACHINE_LOCKDOWN, SET_FEATURE_ON_PROCESS, bEnable);
}

bool CInternetFeature::SetFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags, BOOL fEnable)
{
	bool bRet = false;
	if (m_funcCoInternetSetFeatureEnabled)
		bRet = SUCCEEDED(m_funcCoInternetSetFeatureEnabled(FeatureEntry, dwFlags, fEnable));
	return bRet;
}

bool CInternetFeature::IsFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags)
{
	bool bRet = false;
	if (m_funcCoInternetIsFeatureEnabled)
		bRet = SUCCEEDED(m_funcCoInternetIsFeatureEnabled(FeatureEntry, dwFlags));
	return bRet;
}