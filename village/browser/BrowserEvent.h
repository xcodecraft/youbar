#pragma once
enum
{
	SINGLE_THREAD = 0,
	MULTI_THREAD
};

class CBrowserWnd;
#include <objidl.h>
#include <exdisp.h>
class CBrowserEvent : public IDispatch
{

friend class CBrowserWnd;
public:
	//BOOL CanSuspend();
	CBrowserEvent();

protected:
	////////////////////////////////////////////////////////////////////
	// Variants
	BOOL			m_bCanSuspend;
	LONG			m_lSecurityIcon;
	CBrowserWnd *   m_pBrowserWnd;

	////////////////////////////////////////////////////////////////////
	// Functions
	virtual HRESULT OnDestroy(){ return S_OK; };
	virtual HRESULT BrowserEventMsg( WPARAM wParam, LPARAM lParam ){return S_OK;};
	virtual HRESULT Connect( CBrowserWnd * pBrowserWnd );
	virtual HRESULT DisConnect();
	void Clear();
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
private:
	virtual void OnUrlChanged(){};
	IConnectionPointContainer * m_pCPContainer;
	IConnectionPoint * m_pCP;
	DWORD m_dwCookie;
};
