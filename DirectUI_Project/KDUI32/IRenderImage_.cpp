#include "stdafx.h"
#include "IRenderImage_.h"
#include "../DirectUICom/DirectUICom.h"

IRenderImage_::IRenderImage_()
	:m_pDispatch(NULL),m_picType(DUI_PICTYPE_BMP)
{

}
IRenderImage_::~IRenderImage_()
{
	if (m_pDispatch)
		m_pDispatch->Release();
}

BOOL IRenderImage_::Draw(IRenderDC_ *pDC,const RECT& rect)
{
	return FALSE;
}

BOOL IRenderImage_::LoadImage(LPCTSTR strImagePath)
{
	return FALSE;
}

LONG IRenderImage_::GetWidth()
{
	return 0;
}
LONG IRenderImage_::GetHeight()
{
	return 0;
}

void IRenderImage_::SetPicType(enumDUIPICTYPE pictype)
{
	m_picType = pictype;
}

enumDUIPICTYPE IRenderImage_::GetPicType()
{
	return m_picType;
}

void IRenderImage_::SetDispatch(IDispatch *pDispatch)
{
	m_pDispatch = pDispatch;
}
IDispatch* IRenderImage_::GetDispatch()
{
	return m_pDispatch;
}

//////////////////////////////////////////////
//////////////////////////////////////////////

CRenderImage_GDI_::CRenderImage_GDI_()
	:m_hBitmap(NULL),m_pImage(NULL)
{

}
CRenderImage_GDI_::~CRenderImage_GDI_()
{

}

BOOL CRenderImage_GDI_::Draw(IRenderDC_ *pDC,const RECT& rect)
{
	RECT rcSrc;
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = rcSrc.left + GetWidth();
	rcSrc.bottom = rcSrc.top + GetHeight();

	return pDC->AlphaBlend_(this,rect,rcSrc);
}

BOOL CRenderImage_GDI_::LoadImage(LPCTSTR strImagePath, enumDUIPICTYPE pictype)
{
	m_sImgPath = strImagePath;
	if (  pictype == DUI_PICTYPE_BMP )
	{
		m_hBitmap = (HBITMAP)::LoadImage(NULL, strImagePath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_DEFAULTSIZE|LR_LOADFROMFILE);
		return (m_hBitmap != NULL);
	}
	else if ( pictype == DUI_PICTYPE_PNG )
	{
		m_pImage = Image::FromFile(strImagePath);
		return (m_pImage != NULL);
	}

	return FALSE;
}

LONG CRenderImage_GDI_::GetWidth()
{
	if ( m_pImage )
	{
		return m_pImage->GetWidth();
	}

	BITMAP bmp;
	::GetObject(m_hBitmap,sizeof(BITMAP),&bmp);
	return bmp.bmWidth;
}

LONG CRenderImage_GDI_::GetHeight()
{
	if ( m_pImage )
	{
		return m_pImage->GetHeight();
	}

	BITMAP bmp;
	::GetObject(m_hBitmap,sizeof(BITMAP),&bmp);
	return bmp.bmHeight;
}

void CRenderImage_GDI_::SetHBitmap(HBITMAP hBitmap)
{
	m_hBitmap = hBitmap;
}

HBITMAP CRenderImage_GDI_::GetHBitmap()
{
	return m_hBitmap;
}

Image* CRenderImage_GDI_::GetPngImage()
{
	return m_pImage;
}
