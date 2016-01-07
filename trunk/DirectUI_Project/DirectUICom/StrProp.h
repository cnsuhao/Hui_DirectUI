// E:\UI\directui_simple_sourcecode\DirectUICom\StrProp.h : CStrProp ������

#pragma once
#include "resource.h"       // ������

#include "DirectUICom.h"
#include "DirectUISupport.h"
#include "DUIPropBase.h"
#include "DUIPropType.h"
#include "_IStrPropEvents_CP.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif



// CStrProp

class ATL_NO_VTABLE CStrProp :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CStrProp, &CLSID_StrProp>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CStrProp>,
	public CProxy_IStrPropEvents<CStrProp>,
	public IDispatchImpl<IStrProp, &IID_IStrProp, &LIBID_DirectUIComLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CStrProp()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_STRPROP)


BEGIN_COM_MAP(CStrProp)
	COM_INTERFACE_ENTRY(IStrProp)
	COM_INTERFACE_ENTRY(IDUIPropBase)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CStrProp)
	CONNECTION_POINT_ENTRY(__uuidof(_IStrPropEvents))
END_CONNECTION_POINT_MAP()
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		IMPLEMENT_PROP_BASE(m_pPropBase)
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
	IDUIPropBase *m_pPropBase;

	_bstr_t m_strText;

public:
	INTERFACE_PROP_BASE_IMPL_DEFAULT(m_pPropBase)

	STDMETHOD(GetValue)(BSTR* strValue);
	STDMETHOD(SetValue)(BSTR strValue);
};

OBJECT_ENTRY_AUTO(__uuidof(StrProp), CStrProp)
