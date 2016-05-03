#pragma once
#include "SkinShareObj_.h"

class KDUI32_API SkinImageBase_ : public SkinShareObj_
{
public:
	SkinImageBase_():m_pSkinPicture(NULL){}
	virtual ~SkinImageBase_(){}

public:
	void SetPicture(SkinPicture_* pSkinPicture){m_pSkinPicture = pSkinPicture;}
	SkinPicture_* GetPicture(){return m_pSkinPicture;}

	void SetSourceRect(const RECT& rect){m_rect = rect;}
	RECT GetSourceRect(){return m_rect;}

	void SetBorderRect(const RECT& rect){m_rcBorder = rect;}
	RECT GetBorderRect(){return m_rcBorder;}

	BOOL RenderDraw(IRenderDC_ *pDC,const RECT& rcDest)
	{
		IRenderImage_* pImage = m_pSkinPicture->GetRenderImage();
		return pDC->DrawImageSec(	pImage,rcDest,m_rect,m_rcBorder);		
	}

	//2016/01/07
	BOOL RenderDraw(IRenderDC_ *pDC,const RECT& rcDest, BSTR strText)
	{
		IRenderImage_* pImage = m_pSkinPicture->GetRenderImage();
		return pDC->DrawText(pImage, rcDest, strText);
	}

	void SetTextFont(BSTR strFontName, short nSize, bool bBold)
	{
		m_strFontName = strFontName;
		m_nSize = nSize;
		m_bBold = bBold;


	}

protected:
	SkinPicture_ *m_pSkinPicture;
	RECT m_rect;
	RECT m_rcBorder;

	//
	BSTR m_strFontName;
	short	m_nSize;
	bool	m_bBold;
};