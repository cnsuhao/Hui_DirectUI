// E:\UI\directui_simple_sourcecode\DirectUICom\TextStyleProp.cpp : CTextStyleProp 的实现

#include "stdafx.h"
#include "TextStyleProp.h"


// CTextStyleProp

STDMETHODIMP CTextStyleProp::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ITextStyleProp
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CTextStyleProp::RenderDraw(IDUIRenderDC* pDC, SkinRect* rect, BSTR strValue, VARIANT_BOOL* pbResult)
{
	// TODO: 在此添加实现代码
	IRenderDC_ *pDC_ = NULL;
	pDC->GetRenderDCPtr((OLE_HANDLE*)&pDC_);

	RECT rcDest = {rect->left,rect->top,rect->right,rect->bottom};

	BOOL bResult = m_pImageBase->RenderDraw(pDC_,rcDest, strValue);

	*pbResult = bResult ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CTextStyleProp::SetFont(BSTR strFontName, short nSize, VARIANT_BOOL bBold, VARIANT_BOOL* pbResult)
{
	// TODO: 在此添加实现代码

	m_pImageBase->SetTextFont(strFontName, nSize, bBold);

	return S_OK;
}
