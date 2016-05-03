#pragma once
#include "IRenderImage_.h"
#include "../DirectUICom/DirectUICom.h"
#include <atltypes.h>

class IRenderImage_;

class IRenderDC_
{
public:
	IRenderDC_();

	virtual ~IRenderDC_();

	void SetRenderDCComPtr(IDUIRenderDC *pDC);
	IDUIRenderDC* GetRenderDCComPtr();

	virtual BOOL DrawImage(IRenderImage_ *pImage,const RECT& rect);

	virtual BOOL AlphaBlend_(IRenderImage_* pImage,const RECT& rcDest,const RECT& rcSrc);

	virtual BOOL DrawImageSec(IRenderImage_ *pImage,const RECT& rcDest,const RECT& rcSrc,const RECT& rcBorder);

	//2016/01/07
	virtual BOOL DrawText(IRenderImage_ *pImage, const RECT& rcDest, BSTR strText);

	void BeginRender(HDC hdc, CRect rc, COLORREF clr);
	void EndRender(CRect rc,BOOL bRefresh);
	void Clear(CRect rc);
	void RenderDraw(IRenderDC_* pRenderDC,CRect rc);

protected:
	IDUIRenderDC *m_pRenderDCComPtr;
};

////////////////////////////////
////////////////////////////////

class CRenderDC_GDI_ : public IRenderDC_
{
public:
	CRenderDC_GDI_();
	virtual ~CRenderDC_GDI_();

public:
	virtual BOOL DrawImage(IRenderImage_ *pImage,const RECT& rect);

	virtual BOOL AlphaBlend_(IRenderImage_* pImage,const RECT& rcDest,const RECT& rcSrc);

	BOOL DrawImage(IRenderImage_* pImage, RECT rcDest, RECT rcSrc,BOOL bStretch,COLORREF clrTrans,BOOL bTrans,BOOL bClient = FALSE);
	virtual BOOL DrawImageSec(IRenderImage_ *pImage,const RECT& rcDest,const RECT& rcSrc,const RECT& rcBorder);

	void SetHDC(HDC hDC);
	HDC GetHDC();

	//2016/01/07
	virtual BOOL DrawText(IRenderImage_ *pImage, const RECT& rcDest, BSTR strText);

	void SetFont(BSTR strFontName, short nSize, bool bBold);

private:
	HDC m_hDC;
	LOGFONT m_logfont;
	HFONT m_hFont;
	//
	BSTR m_strFontName;
	short	m_nSize;
	bool	m_bBold;
};