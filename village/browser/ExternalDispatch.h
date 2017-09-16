#pragma once

#include <objidl.h>
#include <exdisp.h>
class CExternalDispatch : public IDispatch
{

public:
	//BOOL CanSuspend();
	CExternalDispatch();

	////////////////////////////////////////////////////////////////////
	// IUnKnown
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj);
	STDMETHOD_(ULONG, AddRef)(THIS);
	STDMETHOD_(ULONG, Release)(THIS);

	////////////////////////////////////////////////////////////////////
	// IDispatch
	STDMETHOD(GetTypeInfoCount)( UINT *pctinfo );
	STDMETHOD(GetTypeInfo)( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo );
	STDMETHOD(GetIDsOfNames)( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId );
    STDMETHOD(Invoke)( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );

	bool CheckWindowSID(IDispatch* pWindow);
	struct METHODMAP 
	{
		LPCWSTR pstr;
		UINT id;
	};
};
