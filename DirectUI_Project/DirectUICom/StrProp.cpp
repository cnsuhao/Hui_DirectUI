// E:\UI\directui_simple_sourcecode\DirectUICom\StrProp.cpp : CStrProp ��ʵ��

#include "stdafx.h"
#include "StrProp.h"

#pragma comment(lib,"comsuppw.lib")

// CStrProp

STDMETHODIMP CStrProp::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IStrProp
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CStrProp::GetValue(BSTR* strValue)
{
	// TODO: �ڴ����ʵ�ִ���
	*strValue = _com_util::ConvertStringToBSTR(m_strText);
	SysFreeString(m_strText);
	return S_OK;
}

STDMETHODIMP CStrProp::SetValue(BSTR strValue)
{
	// TODO: �ڴ����ʵ�ִ���
	m_strText =  _com_util::ConvertBSTRToString(strValue);
	return S_OK;
}
