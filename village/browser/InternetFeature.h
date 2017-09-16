//
//  InternetFeature.h
//
//  Created by wanyong on 10/23/2008
//
//	Window XP SP2 后提供的 IE 特征设置，包括一些安全属性，弹出窗口，ActiveX 等

#pragma once
#include "urlmon.h"

typedef HRESULT (_stdcall * pfnCoInternetSetFeatureEnabled)(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags, BOOL fEnable);
typedef HRESULT (_stdcall * pfnCoInternetIsFeatureEnabled)(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags);

class CInternetFeature
{
public:
	CInternetFeature(void);
	~CInternetFeature(void);

	/** 初始化 */
	static void Initialize();

	/** 是否可用，可以用该函数来判断是否是当前系统是否高于 Window XP SP2 */
	static bool IsEnabled();

	/** 开启/关闭信息栏提示 */
	static bool EnableSecurityBand(BOOL bEnable);

	/** 开启/关闭 ActiveX 控件安装 */
	static bool RestrictActivexInstall(BOOL bEnable);

	/** 是否启用 ActiveX 控件安装 */
	static bool IsActivexInstallEnabled();

	/** 开启/关闭弹出窗口控件安装 */
	static bool WebOcPopupManagement(BOOL bEnable);

	/** 是否启用弹出窗口控件安装 */
	static bool IsWebPopupManagementEnabled();

	/** IE 对打开本地含有脚本文件的限制，在每个线程中调用 */
	static void EnableLocalMachineLockDown(BOOL bEnable);

protected:

	/** 对系统 API(CoInternetSetFeatureEnabled) 的封装 */
	static bool SetFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags, BOOL fEnable);

	static bool IsFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags);

private:

	/** 系统 API(CoInternetSetFeatureEnabled) 的地址 */
	static pfnCoInternetSetFeatureEnabled m_funcCoInternetSetFeatureEnabled;

	/** 系统 API(CoInternetIsFeatureEnabled) 的地址 */
	static pfnCoInternetIsFeatureEnabled m_funcCoInternetIsFeatureEnabled;

	static bool sm_bEnabled;
};
