// E:\UI\directui_simple_sourcecode\DirectUICom\TextStyleProp.h : CTextStyleProp ������

#pragma once
#include "resource.h"       // ������

#include "DirectUICom.h"
#include "DirectUISupport.h"
#include "DUIPropBase.h"
#include "DUIPropType.h"
#include "_ITextStylePropEvents_CP.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE ƽ̨(�粻�ṩ��ȫ DCOM ֧�ֵ� Windows Mobile ƽ̨)���޷���ȷ֧�ֵ��߳� COM ���󡣶��� _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA ��ǿ�� ATL ֧�ִ������߳� COM ����ʵ�ֲ�����ʹ���䵥�߳� COM ����ʵ�֡�rgs �ļ��е��߳�ģ���ѱ�����Ϊ��Free����ԭ���Ǹ�ģ���Ƿ� DCOM Windows CE ƽ̨֧�ֵ�Ψһ�߳�ģ�͡�"
#endif



// CTextStyleProp

class ATL_NO_VTABLE CTextStyleProp :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CTextStyleProp, &CLSID_TextStyleProp>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CTextStyleProp>,
	public CProxy_ITextStylePropEvents<CTextStyleProp>,
	public IDispatchImpl<ITextStyleProp, &IID_ITextStyleProp, &LIBID_DirectUIComLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CTextStyleProp()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TEXTSTYLEPROP)


BEGIN_COM_MAP(CTextStyleProp)
	COM_INTERFACE_ENTRY(ITextStyleProp)
	COM_INTERFACE_ENTRY(IDUIPropBase)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CTextStyleProp)
	CONNECTION_POINT_ENTRY(__uuidof(_ITextStylePropEvents))
END_CONNECTION_POINT_MAP()
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_pImageBase = new SkinImageBase_();

		IMPLEMENT_PROP_BASE(m_pPropBase)
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_pImageBase)
		{
			delete m_pImageBase;
			m_pImageBase = NULL;
		}
	}

private:
	IDUIPropBase *m_pPropBase;
	SkinImageBase_ *m_pImageBase;

public:
	INTERFACE_PROP_BASE_IMPL_DEFAULT(m_pPropBase)

	STDMETHOD(RenderDraw)(IDUIRenderDC* pDC, SkinRect* rect, BSTR strValue, VARIANT_BOOL* pbResult);
};

OBJECT_ENTRY_AUTO(__uuidof(TextStyleProp), CTextStyleProp)
