#pragma once
#include "IRenderDC_.h"
#include <GdiPlus.h>
#include "stringex.h"
#include "../DirectUICom/DirectUICom.h"

using namespace Gdiplus;

class IRenderDC_;

class IRenderImage_
{
public:
	IRenderImage_();
	virtual ~IRenderImage_();

public:
	virtual BOOL Draw(IRenderDC_ *pDC,const RECT& rect);

	virtual BOOL LoadImage(LPCTSTR strImagePath);

	virtual LONG GetWidth();
	virtual LONG GetHeight();

	virtual void SetPicType(enumDUIPICTYPE pictype);
	virtual enumDUIPICTYPE GetPicType();

	void SetDispatch(IDispatch *pDispatch);
	IDispatch* GetDispatch();


private:
	IDispatch *m_pDispatch;
	enumDUIPICTYPE m_picType;
};

////////////////////////////////////
////////////////////////////////////

class CRenderImage_GDI_ : public IRenderImage_
{
public:
	CRenderImage_GDI_();
	virtual ~CRenderImage_GDI_();

public:
	virtual BOOL Draw(IRenderDC_ *pDC,const RECT& rect);

	virtual BOOL LoadImage(LPCTSTR strImagePath, enumDUIPICTYPE pictype);

	virtual LONG GetWidth();

	virtual LONG GetHeight();

	void SetHBitmap(HBITMAP hBitmap);
	HBITMAP GetHBitmap();

	Image* GetPngImage();

private:
	HBITMAP m_hBitmap;
	tstring m_sImgPath;

	Image* m_pImage;
};

