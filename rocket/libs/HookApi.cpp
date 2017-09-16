#include "common.h"
#include <windows.h>
#include <stdio.h>
#include "baselib/HookApi.h"

#pragma comment(linker, "/SECTION:HookStub,RW")

#define ALLOCATE_HookStub ALLOCATE(HookStub)

#pragma code_seg("HookStub")
#pragma optimize("",off)
ALLOCATE_HookStub HOOKENVIRONMENT pEnv={0};

NAKED DWORD GetDelta()
{
	__asm
	{
		call next
next:
		pop eax
		sub eax,offset next
		ret
	}
}

NAKED void NewStub()
{
	__asm
	{	
		jmp next
back:
		_emit 0xE9
		NOP
		NOP
		NOP
		NOP
next:
		push [esp]
		push [esp]
		push eax		//±£´æÒ»ÏÂStubÖÐÎ¨Ò»Ê¹ÓÃµ½µÄEAX
		call GetDelta
		lea eax,[eax+pEnv]
		mov dword ptr [esp+0xC],eax
		pop eax			//»Ö¸´EAX
		jmp back
	}
}

NAKED DWORD  GetEndAddr()
{
	__asm
	{
		call next
next:
		pop eax
		sub eax,5
		ret
	}
}

#pragma optimize("",off)
#pragma code_seg()


DWORD __stdcall GetOpCodeSize(BYTE* iptr0)
{
	BYTE* iptr = iptr0;

	DWORD f = 0;

prefix:
	BYTE b = *iptr++;

	f |= table_1[b];

	if (f&C_FUCKINGTEST)
		if (((*iptr)&0x38)==0x00)   // ttt
			f=C_MODRM+C_DATAW0;       // TEST
		else
			f=C_MODRM;                // NOT,NEG,MUL,IMUL,DIV,IDIV

	if (f&C_TABLE_0F)
	{
		b = *iptr++;
		f = table_0F[b];
	}

	if (f==C_ERROR)
	{
		//printf("error in %02X\n",b);
		return C_ERROR;
	}

	if (f&C_PREFIX)
	{
		f&=~C_PREFIX;
		goto prefix;
	}

	if (f&C_DATAW0) if (b&0x01) f|=C_DATA66; else f|=C_DATA1;

	if (f&C_MODRM)
	{
		b = *iptr++;
		BYTE mod = b & 0xC0;
		BYTE rm  = b & 0x07;
		if (mod!=0xC0)
		{
			if (f&C_67)         // modrm16
			{
				if ((mod==0x00)&&(rm==0x06)) f|=C_MEM2;
				if (mod==0x40) f|=C_MEM1;
				if (mod==0x80) f|=C_MEM2;
			}
			else                // modrm32
			{
				if (mod==0x40) f|=C_MEM1;
				if (mod==0x80) f|=C_MEM4;
				if (rm==0x04) rm = (*iptr++) & 0x07;    // rm<-sib.base
				if ((rm==0x05)&&(mod==0x00)) f|=C_MEM4;
			}
		}
	} // C_MODRM

	if (f&C_MEM67)  if (f&C_67) f|=C_MEM2;  else f|=C_MEM4;
	if (f&C_DATA66) if (f&C_66) f|=C_DATA2; else f|=C_DATA4;

	if (f&C_MEM1)  iptr++;
	if (f&C_MEM2)  iptr+=2;
	if (f&C_MEM4)  iptr+=4;

	if (f&C_DATA1) iptr++;
	if (f&C_DATA2) iptr+=2;
	if (f&C_DATA4) iptr+=4;

	return iptr - iptr0;
}


PHOOKENVIRONMENT __stdcall InstallHookApi(LPCTSTR DllName,LPCSTR ApiName,PVOID HookProc)
{
	HMODULE DllHandle;
	PVOID ApiEntry;
	int ReplaceCodeSize;
	DWORD oldpro;
	DWORD SizeOfStub;
	DWORD delta;
	DWORD RetSize =0;
	
	PHOOKENVIRONMENT pHookEnv;

	if (HookProc == NULL)
	{
		return NULL;
	}

	DllHandle = GetModuleHandle(DllName);
	if (DllHandle == NULL)
		DllHandle = LoadLibrary(DllName);
	if (DllHandle == NULL)
		return NULL;

	ApiEntry = GetProcAddress(DllHandle,ApiName);
	if (ApiEntry == NULL) return NULL;

	ReplaceCodeSize = GetOpCodeSize((BYTE*)ApiEntry);
 
	while (ReplaceCodeSize < 5)
		ReplaceCodeSize += GetOpCodeSize((BYTE*)((DWORD)ApiEntry + (DWORD)ReplaceCodeSize));

	if (ReplaceCodeSize > 16) return NULL;

	SizeOfStub = (DWORD)GetEndAddr-(DWORD)&pEnv;
	
	pHookEnv = (PHOOKENVIRONMENT)VirtualAlloc(NULL,SizeOfStub,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	memset((void*)&pEnv,0x90,sizeof(pEnv));
	CopyMemory(pHookEnv,(PVOID)&pEnv,SizeOfStub);
	CopyMemory((void*)pHookEnv,(void*)&pEnv,sizeof(pEnv.savebytes));
	CopyMemory(pHookEnv->savebytes,ApiEntry,ReplaceCodeSize);

	pHookEnv->OrgApiAddr = ApiEntry;
	pHookEnv->SizeOfReplaceCode = ReplaceCodeSize;

	pHookEnv->jmptoapi[0]=0xE9;
	*(DWORD*)(&pHookEnv->jmptoapi[1]) = (DWORD)ApiEntry + ReplaceCodeSize - ((DWORD)pHookEnv->jmptoapi + 5);

	//patch api
	if (!VirtualProtect(ApiEntry,ReplaceCodeSize,PAGE_EXECUTE_READWRITE,&oldpro))
		return FALSE;

	delta = (DWORD)pHookEnv - (DWORD)&pEnv;

	*(DWORD*)(&JMPGate[1]) = ((DWORD)NewStub + delta) - ((DWORD)ApiEntry + 5);

	WriteProcessMemory(GetCurrentProcess(), ApiEntry, JMPGate, sizeof(JMPGate),&RetSize);

	if (!VirtualProtect(ApiEntry,ReplaceCodeSize,oldpro,&oldpro))
		return FALSE;

	//Ð´Èë±äÁ¿
	*(DWORD*)((DWORD)NewStub + delta + 3) = (DWORD)HookProc - ((DWORD)NewStub + delta + 3 + 4);

	return pHookEnv;
}

BOOL __stdcall UnInstallHookApi(PHOOKENVIRONMENT pHookEnv)
{
	DWORD oldpro;
	DWORD RetSize;

	//Èç¹ûÄÚ´æ²»´æÔÚÁË£¬ÔòÍË³ö
	if(IsBadReadPtr((const void*)pHookEnv,sizeof(HOOKENVIRONMENT)))
		return FALSE;

	if(!VirtualProtect(pHookEnv->OrgApiAddr,pHookEnv->SizeOfReplaceCode,PAGE_EXECUTE_READWRITE,&oldpro))
		return FALSE;
	WriteProcessMemory(GetCurrentProcess(),pHookEnv->OrgApiAddr,pHookEnv->savebytes,pHookEnv->SizeOfReplaceCode,&RetSize);

	if(!VirtualProtect(pHookEnv->OrgApiAddr,pHookEnv->SizeOfReplaceCode,oldpro,&oldpro))
		return FALSE;

	VirtualFree((LPVOID)pHookEnv,0,MEM_RELEASE);
	return true;
}


//¶¨ÒåÏÂÃæÕâÐÐ¿ÉÒÔ×÷ÎªÑÝÊ¾Ê¹ÓÃ
//#define TEST_MAIN


#ifdef TEST_MAIN

BOOL IsMe = FALSE;

//ÏÈ¶¨ÒåÒ»ÏÂÒªhookµÄWINAPI
typedef HMODULE (WINAPI __pfnLoadLibraryA)(LPCTSTR lpFileName);

/*
HookProcµÄ²ÎÊýÉùÃ÷·½Ê½ÀàÐÍµÈºÍÔ­À´µÄapiÒ»Ñù£¬Ö»ÊÇ²ÎÊý±ÈÔ­API¶à2¸ö
DWORD WINAPI HookProc(DWORD RetAddr ,__pfnXXXX pfnXXXX, ...);

//²ÎÊý±ÈÔ­Ê¼µÄAPI¶à2¸ö²ÎÊý
RetAddr	//µ÷ÓÃapiµÄ·µ»ØµØÖ·
pfnXXX 	//ÀàÐÍÎª__pfnXXXX£¬´ýhookµÄapiµÄÉùÃ÷ÀàÐÍ£¬ÓÃÓÚµ÷ÓÃÎ´±»hookµÄapi

Ïê¼ûMy_LoadLibraryA
Ô­Ê¼µÄLoadLibraryAµÄÉùÃ÷ÊÇ£º

HMODULE WINAPI LoadLibraryA( LPCSTR lpLibFileName );

ÄÇÃ´Ê×ÏÈ¶¨ÒåÒ»ÏÂhookµÄWINAPIµÄÀàÐÍ
typedef HMODULE (WINAPI __pfnLoadLibraryA)(LPCTSTR lpFileName);

È»ºóhookprocµÄº¯ÊýÉùÃ÷ÈçÏÂ£º
HMODULE WINAPI My_LoadLibraryA(DWORD RetAddr,
							   __pfnLoadLibraryA pfnLoadLibraryA,
							   LPCTSTR lpFileName
							   );

±ÈÔ­À´µÄ¶àÁË2¸ö²ÎÊý£¬²ÎÊýÎ»ÖÃ²»ÄÜµßµ¹£¬ÔÚMy_LoadLibraryAÖÐ¿ÉÒÔ×ÔÓÉµÄµ÷ÓÃÎ´±»hookµÄpfnLoadLibraryA
Ò²¿ÉÒÔµ÷ÓÃÏµÍ³µÄLoadLibraryA£¬²»¹ýÒª×Ô¼ºÔÚhookprocÖÐ´¦ÀíºÃÖØÈëÎÊÌâ

ÁíÍâ£¬Ò²¿ÉÒÔÔÚMy_LoadLibraryAÖÐÊ¹ÓÃUnInstallHookApi()º¯ÊýÀ´Ð¶ÔØhook£¬ÓÃ·¨ÈçÏÂ£º
½«µÚ¶þ¸ö²ÎÊý__pfnLoadLibraryA pfnLoadLibraryAÇ¿ÖÆ×ª»»³ÉPHOOKENVIRONMENTÀàÐÍ£¬Ê¹ÓÃUnInstallHookApiÀ´Ð¶ÔØ

ÀýÈç£º
UnInstallHookApi((PHOOKENVIRONMENT)pfnLoadLibraryA);


ÖÁÓÚÒÔÇ°°æ±¾µÄHookBeforeºÍHookAfter£¬ÍêÈ«¿ÉÒÔÔÚ×Ô¼ºµÄHookProcÀïÃæÁé»îÊ¹ÓÃÁË

*/

HMODULE WINAPI My_LoadLibraryA(DWORD RetAddr,
							   __pfnLoadLibraryA pfnLoadLibraryA,
							   LPCTSTR lpFileName
							   )
{
	HMODULE hLib;
	
	//ÐèÒª×Ô¼º´¦ÀíÖØÈëºÍÏß³Ì°²È«ÎÊÌâ
	if (!IsMe)
	{
		IsMe = TRUE;
		MessageBoxA(NULL,lpFileName,"test",MB_ICONINFORMATION);
		hLib = LoadLibrary(lpFileName);//ÕâÀïµ÷ÓÃµÄÊÇÏµÍ³µÄ£¬ÒÑ¾­±»hook¹ýµÄ
		IsMe = FALSE;
		//ÕâÀïÊÇÐ¶ÔØHook£¬ÕâÀïÐ¶ÔØÍê¾Í²»ÄÜÓÃpfnLoadLibraryAÀ´µ÷ÓÃÁË
		UnInstallHookApi((PHOOKENVIRONMENT)pfnLoadLibraryA);
		return hLib;
	}
	return pfnLoadLibraryA(lpFileName);//ÕâÀïµ÷ÓÃ·ÇhookµÄ
}


int main()
{
	DWORD RetSize =0;
	DWORD dwThreadId;
	HANDLE hThread;
	PHOOKENVIRONMENT pHookEnv;

	pHookEnv = InstallHookApi("Kernel32.dll", "LoadLibraryA", My_LoadLibraryA);
	LoadLibrary("InjectDll.dll");
	MessageBoxA(NULL,"Safe Here!!!","Very Good!!",MB_ICONINFORMATION);
	UnInstallHookApi(pHookEnv);//ÓÉÓÚHookProcÖÐÐ¶ÔØ¹ýÁË£¬ËùÒÔÕâÀïµÄÐ¶ÔØ¾ÍÎÞÐ§ÁË
	MessageBoxA(NULL,"UnInstall Success!!!","Good!!",MB_ICONINFORMATION);
	return 0;
}

#endif

 
