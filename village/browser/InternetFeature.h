//
//  InternetFeature.h
//
//  Created by wanyong on 10/23/2008
//
//	Window XP SP2 ���ṩ�� IE �������ã�����һЩ��ȫ���ԣ��������ڣ�ActiveX ��

#pragma once
#include "urlmon.h"

typedef HRESULT (_stdcall * pfnCoInternetSetFeatureEnabled)(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags, BOOL fEnable);
typedef HRESULT (_stdcall * pfnCoInternetIsFeatureEnabled)(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags);

class CInternetFeature
{
public:
	CInternetFeature(void);
	~CInternetFeature(void);

	/** ��ʼ�� */
	static void Initialize();

	/** �Ƿ���ã������øú������ж��Ƿ��ǵ�ǰϵͳ�Ƿ���� Window XP SP2 */
	static bool IsEnabled();

	/** ����/�ر���Ϣ����ʾ */
	static bool EnableSecurityBand(BOOL bEnable);

	/** ����/�ر� ActiveX �ؼ���װ */
	static bool RestrictActivexInstall(BOOL bEnable);

	/** �Ƿ����� ActiveX �ؼ���װ */
	static bool IsActivexInstallEnabled();

	/** ����/�رյ������ڿؼ���װ */
	static bool WebOcPopupManagement(BOOL bEnable);

	/** �Ƿ����õ������ڿؼ���װ */
	static bool IsWebPopupManagementEnabled();

	/** IE �Դ򿪱��غ��нű��ļ������ƣ���ÿ���߳��е��� */
	static void EnableLocalMachineLockDown(BOOL bEnable);

protected:

	/** ��ϵͳ API(CoInternetSetFeatureEnabled) �ķ�װ */
	static bool SetFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags, BOOL fEnable);

	static bool IsFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags);

private:

	/** ϵͳ API(CoInternetSetFeatureEnabled) �ĵ�ַ */
	static pfnCoInternetSetFeatureEnabled m_funcCoInternetSetFeatureEnabled;

	/** ϵͳ API(CoInternetIsFeatureEnabled) �ĵ�ַ */
	static pfnCoInternetIsFeatureEnabled m_funcCoInternetIsFeatureEnabled;

	static bool sm_bEnabled;
};
