#include "stdafx.h"
#include "IRenderDC_.h"
#include "../DirectUICom/DirectUICom.h"
#include <math.h>
#include <Windowsx.h>

///////////////////////
IRenderDC_::IRenderDC_():m_pRenderDCComPtr(NULL)
{
	::CoCreateInstance(__uuidof(DUIRenderDC), NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void**)&m_pRenderDCComPtr);
	m_pRenderDCComPtr->SetRenderDCPtr((OLE_HANDLE)this);
}

IRenderDC_::~IRenderDC_()
{
	m_pRenderDCComPtr->Release();
}

void IRenderDC_::SetRenderDCComPtr(IDUIRenderDC *pDC)
{
	m_pRenderDCComPtr = pDC;
}

IDUIRenderDC* IRenderDC_::GetRenderDCComPtr()
{
	return m_pRenderDCComPtr;
}

BOOL IRenderDC_::DrawImage(IRenderImage_ *pImage,const RECT& rect)
{
	return FALSE;
}

BOOL IRenderDC_::AlphaBlend_(IRenderImage_* pImage,const RECT& rcDest,const RECT& rcSrc)
{
	return FALSE;
}

BOOL IRenderDC_::DrawImageSec(IRenderImage_ *pImage,const RECT& rcDest,const RECT& rcSrc,const RECT& rcBorder)
{
	return FALSE;
}

//2016/01/07
BOOL IRenderDC_::DrawText(IRenderImage_ *pImage, const RECT& rcDest, BSTR strText)
{
	return FALSE;
}

void IRenderDC_::BeginRender( HDC gdc, CRect rc, COLORREF clr )
{
	CPoint pt;
	::SetViewportOrgEx(gdc,rc.left,rc.top,&pt);
}

void IRenderDC_::EndRender( CRect rc,BOOL bRefresh )
{

}

void IRenderDC_::Clear( CRect rc )
{

}

void IRenderDC_::RenderDraw( IRenderDC_* pRenderDC,CRect rc )
{

}
///////////////////////
///////////////////////
CRenderDC_GDI_::CRenderDC_GDI_():m_hDC(NULL)
{
	//		m_pRenderDCComPtr->SetRenderDCPtr((OLE_HANDLE)this);
	memset(&m_logfont,0,sizeof(LOGFONT));
	m_logfont.lfHeight = 32;
	m_logfont.lfWidth=0;
	m_logfont.lfWeight=400;
	m_logfont.lfCharSet=134;
	_tcscpy((TCHAR*)m_logfont.lfFaceName, _T("微软雅黑")); 

	m_hFont = CreateFontIndirect(&m_logfont);
}

CRenderDC_GDI_::~CRenderDC_GDI_()
{

}

BOOL CRenderDC_GDI_::DrawImage(IRenderImage_ *pImage,const RECT& rect)
{
	RECT rcSrc;
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = rcSrc.left + pImage->GetWidth();
	rcSrc.bottom = rcSrc.top + pImage->GetHeight();

	return AlphaBlend_(pImage,rect,rcSrc);
}

BOOL CRenderDC_GDI_::AlphaBlend_(IRenderImage_* pImage,const RECT& rcDest,const RECT& rcSrc)
{
	int nDestWidth = rcDest.right - rcDest.left;
	int nDestHeight = rcDest.bottom - rcDest.top;
	int nSrcWidth = rcSrc.right - rcSrc.left;
	int nSrcHeight = rcSrc.bottom - rcSrc.top;
	CRenderImage_GDI_ *pGDIImage = (CRenderImage_GDI_*)pImage;

	enumDUIPICTYPE pictype = pImage->GetPicType();
	if ( pictype == DUI_PICTYPE_BMP )
	{
		///////////
		HBITMAP hBitmap = pGDIImage->GetHBitmap();
		HDC hDC = ::CreateCompatibleDC(m_hDC);
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC,hBitmap);

		BLENDFUNCTION bf = { AC_SRC_OVER, 0,255, AC_SRC_OVER/*AC_SRC_ALPHA*/ };
		BOOL bRet = ::AlphaBlend(m_hDC,rcDest.left,rcDest.top,nDestWidth,nDestHeight,
			hDC,rcSrc.left,rcSrc.top,nSrcWidth,nSrcHeight,bf);
		if(!bRet)
		{
			DWORD dwErr = ::GetLastError();
			int a = 0;
		}

		::SelectObject(hDC,hOldBitmap);
		::DeleteDC(hDC);
	}
	else if ( pictype == DUI_PICTYPE_PNG )
	{
		Image* pImg = pGDIImage->GetPngImage();
		if ( pImg )
		{
			Graphics graphics(m_hDC);

			Rect rc(rcDest.left,rcDest.top,rcDest.right-rcDest.left,rcDest.bottom-rcDest.top);
			Status status = graphics.DrawImage(pImg, rc,rcSrc.left,rcSrc.top,
				rcSrc.right-rcSrc.left,rcSrc.bottom-rcSrc.top/*0,0, pGDIImage->GetWidth(), pGDIImage->GetHeight()*/,UnitPixel);
			if(status != Status::Ok)
			{
				OutputDebugString(_T("DrawImage Error.\n"));
			}
		}
	}

	return TRUE;
}

BOOL CRenderDC_GDI_::DrawImage(IRenderImage_* pImage, RECT rcDest, RECT rcSrc,BOOL bStretch,COLORREF clrTrans,BOOL bTrans,BOOL bClient /*= FALSE*/)
{
	RECT rectDest = rcDest;

	clrTrans = RGB(255,0,255);

	int nDesWidth = rectDest.right - rectDest.left;
	int nDesHeight = rectDest.bottom - rectDest.top;
	float fDesWidth = static_cast<float>(nDesWidth);
	float fDesHeight = static_cast<float>(nDesHeight);

	int nSrcWidth = rcSrc.right - rcSrc.left;
	int nSrcHeight = rcSrc.bottom - rcSrc.top;

	BOOL bSameSize = (nDesWidth == nSrcWidth) && (nDesHeight == nSrcHeight);

	if (bStretch && !bSameSize)
	{		
		AlphaBlend_(pImage,rcDest,rcSrc);
	}
	else
	{
		int nRowNum = 0, nColNum = 0;
		if (nSrcWidth <= fDesWidth || nSrcHeight <= fDesHeight)
		{
			nRowNum = ceil(fDesWidth / nSrcWidth) ;
			nColNum = ceil(fDesHeight / nSrcHeight);
			int nImgLeft = 0,nImgRight = 0,nImgTop = 0,nImgBottom = 0;

			for (int j = 0; j < nColNum; j++)
			{
				for (int i = 0; i < nRowNum; i++)
				{
					nImgLeft = rectDest.left + i * nSrcWidth ;
					nImgRight = nImgLeft + nSrcWidth;

					nImgTop = rectDest.top + j * nSrcHeight ;
					nImgBottom = nImgTop + nSrcHeight; 

					RECT rcSrcDraw = rcSrc;

					if (nImgRight > rectDest.right)
					{						
						rcSrcDraw.right = rcSrcDraw.left + nSrcWidth - (nImgRight - rectDest.right);
					}

					if (nImgBottom > rectDest.bottom)
					{
						rcSrcDraw.bottom = rcSrcDraw.top + nSrcHeight - (nImgBottom - rectDest.bottom);
					}

					RECT rcDest;
					rcDest.left = nImgLeft;
					rcDest.top = nImgTop;
					rcDest.right = rcDest.left + (rcSrcDraw.right - rcSrcDraw.left);
					rcDest.bottom = rcDest.top + (rcSrcDraw.bottom - rcSrcDraw.top);
					AlphaBlend_(pImage,rcDest,rcSrcDraw);
				}
			}
		}
		else
		{
			rcSrc.right = rcSrc.left + nDesWidth;
			rcSrc.bottom = rcSrc.top + nDesHeight;

			RECT rcDest;
			rcDest.left = rectDest.left;
			rcDest.top = rectDest.top;
			rcDest.right = rcDest.left + (rcSrc.right - rcSrc.left);
			rcDest.bottom = rcDest.top + (rcSrc.bottom - rcSrc.top);

			AlphaBlend_(pImage,rcDest,rcSrc);
		}
	}

	return TRUE;
}

BOOL CRenderDC_GDI_::DrawImageSec(IRenderImage_ *pImage,const RECT& rcDest,const RECT& rcSrc,const RECT& rcBorder)
{
	if (pImage == NULL) return FALSE;

	int nDesWidth = rcDest.right - rcDest.left;
	int nDesHeight = rcDest.bottom - rcDest.top;

	if (nDesHeight == 0 || nDesWidth == 0) return FALSE;	

	//画九宫图	
	//  +---|--------------------|---+
	//  |1	|			2		 |3	 |
	//  ------------------------------
	//  |4	|			5		 |6	 |
	//  ------------------------------
	//  |7	|			8		 |9	 |
	//  +---|--------------------|---+	
	RECT rcOffset = rcBorder;

	int nTopHeight = rcOffset.top;
	int nBottomHeight = rcOffset.bottom;
	int nLeftWidth = rcOffset.left;
	int nRightWidth = rcOffset.right;

	RECT rcTex = rcSrc;
	RECT rcDestTemp = {0,0,0,0},rcTexTemp = {0,0,0,0};

	COLORREF clrTrans = -1;
	BOOL bTrans = TRUE;

	//第一步: 贴四个角：1,3,7,9
	//1
	//&& nDesWidth >= nLeftWidth && nDesHeight >= nTopHeight
	if (nTopHeight != 0 && nLeftWidth !=0 )
	{  
		rcDestTemp.top = rcDest.top;
		rcDestTemp.bottom = rcDest.top + nTopHeight;
		rcDestTemp.left = rcDest.left;
		rcDestTemp.right = rcDest.left + nLeftWidth;

		rcTexTemp.top = rcTex.top;
		rcTexTemp.bottom = rcTex.top + nTopHeight;
		rcTexTemp.left = rcTex.left;
		rcTexTemp.right = rcTex.left + nLeftWidth;		

		DrawImage(pImage, rcDestTemp, rcTexTemp, FALSE,RGB(0,0,0),clrTrans,bTrans);
	}	

	//3
	//&& nDesWidth >= nRightWidth && nDesHeight >= nTopHeight
	if (nTopHeight != 0 && nRightWidth !=0 )
	{  
		rcDestTemp.top = rcDest.top;
		rcDestTemp.bottom = rcDest.top + nTopHeight;
		rcDestTemp.left = rcDest.right - nRightWidth;
		rcDestTemp.right = rcDest.right ;

		rcTexTemp.top = rcTex.top;
		rcTexTemp.bottom = rcTex.top + nTopHeight;
		rcTexTemp.left = rcTex.right - nRightWidth;
		rcTexTemp.right = rcTex.right;

		DrawImage(pImage, rcDestTemp, rcTexTemp, FALSE,clrTrans,bTrans,FALSE);
	}

	//7
	//&& nDesWidth >= nLeftWidth && nDesHeight >= nBottomHeight
	if (nBottomHeight != 0 && nLeftWidth !=0 )
	{  
		rcDestTemp.top = rcDest.bottom - nBottomHeight;
		rcDestTemp.bottom = rcDest.bottom;
		rcDestTemp.left = rcDest.left;
		rcDestTemp.right = rcDest.left + nLeftWidth;

		rcTexTemp.top = rcTex.bottom - nBottomHeight;
		rcTexTemp.bottom = rcTex.bottom;
		rcTexTemp.left = rcTex.left;
		rcTexTemp.right = rcTex.left + nLeftWidth;

		DrawImage(pImage, rcDestTemp, rcTexTemp, FALSE,clrTrans,bTrans,FALSE);
	}	

	//9
	// && nDesWidth >= nRightWidth && nDesHeight >= nBottomHeight
	if (nBottomHeight != 0 && nRightWidth !=0)
	{  
		rcDestTemp.top = rcDest.bottom - nBottomHeight;
		rcDestTemp.bottom = rcDest.bottom;
		rcDestTemp.left = rcDest.right - nRightWidth;
		rcDestTemp.right = rcDest.right ;

		rcTexTemp.top = rcTex.bottom - nBottomHeight;
		rcTexTemp.bottom = rcTex.bottom;
		rcTexTemp.left = rcTex.right - nRightWidth;
		rcTexTemp.right = rcTex.right;

		DrawImage(pImage, rcDestTemp, rcTexTemp, FALSE,clrTrans,bTrans,FALSE);
	}

	//第二步: 贴四条边：2,4,6,8
	//2
	//&& nDesHeight >= nTopHeight
	if (nTopHeight != 0 )
	{  
		rcDestTemp.top = rcDest.top;
		rcDestTemp.bottom = rcDest.top + nTopHeight;
		rcDestTemp.left = rcDest.left + nLeftWidth;
		rcDestTemp.right = rcDest.right - nRightWidth;

		rcTexTemp.top = rcTex.top;
		rcTexTemp.bottom = rcTex.top + nTopHeight;
		rcTexTemp.left = rcTex.left + nLeftWidth;
		rcTexTemp.right = rcTex.right - nRightWidth;

		if((rcDestTemp.right > rcDestTemp.left) && (rcDestTemp.bottom > rcDestTemp.top) )
		{
			DrawImage(pImage,rcDestTemp,rcTexTemp,TRUE,clrTrans,bTrans,FALSE);
		}
	}	

	//4
	//&& nDesWidth >= nLeftWidth
	if (nLeftWidth != 0 )
	{  
		rcDestTemp.top = rcDest.top + nTopHeight;
		rcDestTemp.bottom = rcDest.bottom - nBottomHeight;
		rcDestTemp.left = rcDest.left;
		rcDestTemp.right = rcDest.left + nLeftWidth;

		rcTexTemp.top = rcTex.top + nTopHeight;
		rcTexTemp.bottom = rcTex.bottom - nBottomHeight;
		rcTexTemp.left = rcTex.left;
		rcTexTemp.right = rcTex.left + nLeftWidth;

		if((rcDestTemp.right > rcDestTemp.left) && (rcDestTemp.bottom > rcDestTemp.top) )
		{
			DrawImage(pImage,rcDestTemp,rcTexTemp,TRUE,clrTrans,bTrans,FALSE);
		}
	}

	//6
	// && nDesWidth >= nRightWidth
	if (nRightWidth != 0)
	{  
		rcDestTemp.top = rcDest.top + nTopHeight;
		rcDestTemp.bottom = rcDest.bottom - nBottomHeight;
		rcDestTemp.left = rcDest.right - nRightWidth;
		rcDestTemp.right = rcDest.right;

		rcTexTemp.top = rcTex.top + nTopHeight;
		rcTexTemp.bottom = rcTex.bottom - nBottomHeight;
		rcTexTemp.right = rcTex.right;
		rcTexTemp.left = rcTex.right - nRightWidth;

		if((rcDestTemp.right > rcDestTemp.left) && (rcDestTemp.bottom > rcDestTemp.top) )
		{
			DrawImage(pImage,rcDestTemp,rcTexTemp,TRUE,clrTrans,bTrans,FALSE);
		}
	}

	//8
	//&& nDesHeight >= nBottomHeight
	if (nBottomHeight != 0 )
	{  
		rcDestTemp.top = rcDest.bottom - nBottomHeight;
		rcDestTemp.bottom = rcDest.bottom;
		rcDestTemp.left = rcDest.left + nLeftWidth;
		rcDestTemp.right = rcDest.right - nRightWidth;

		rcTexTemp.top = rcTex.bottom - nBottomHeight;
		rcTexTemp.bottom = rcTex.bottom;
		rcTexTemp.left = rcTex.left + nLeftWidth;
		rcTexTemp.right = rcTex.right - nRightWidth;

		if((rcDestTemp.right > rcDestTemp.left) && (rcDestTemp.bottom > rcDestTemp.top) )
		{
			DrawImage(pImage,rcDestTemp,rcTexTemp,TRUE,clrTrans,bTrans,FALSE);
		}
	}

	//第三步: 贴中间部分，Tile平铺或拉伸Stretch
	//if (nDesHeight - nBottomHeight - nTopHeight > 0 && nDesWidth - nRightWidth - nLeftWidth > 0)
	{  
		rcDestTemp.top = rcDest.top + nTopHeight;
		rcDestTemp.bottom = rcDest.bottom - nBottomHeight;
		rcDestTemp.left = rcDest.left + nLeftWidth;
		rcDestTemp.right = rcDest.right - nRightWidth;

		rcTexTemp.top = rcTex.top + nTopHeight;
		rcTexTemp.bottom = rcTex.bottom - nBottomHeight;
		rcTexTemp.left = rcTex.left + nLeftWidth;
		rcTexTemp.right = rcTex.right - nRightWidth;

		if((rcDestTemp.right > rcDestTemp.left) && (rcDestTemp.bottom > rcDestTemp.top) )
		{
			DrawImage(pImage,rcDestTemp,rcTexTemp,TRUE,clrTrans,bTrans,FALSE);
		}
	}
	return TRUE;
}

void CRenderDC_GDI_::SetHDC(HDC hDC)
{
	m_hDC = hDC;
}

HDC CRenderDC_GDI_::GetHDC()
{
	return m_hDC;
}

//2016/01/07
BOOL CRenderDC_GDI_::DrawText(IRenderImage_ *pImage, const RECT& rcDest, BSTR strText)
{
	CString str = strText;

	RECT rc = rcDest;
	SetBkMode(m_hDC, TRANSPARENT);

	SetTextColor(m_hDC, RGB(0xff,0xff,0xff));


	::SelectObject(m_hDC, m_hFont);

	::FillRect(m_hDC, &rc, (HBRUSH)GetStockBrush(NULL_BRUSH));
	::DrawText(m_hDC, str, str.GetLength(), &rc, DT_WORDBREAK |DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	return TRUE;
}

void CRenderDC_GDI_::SetFont(BSTR strFontName, short nSize, bool bBold)
{
	m_strFontName = strFontName;
	m_nSize = nSize;
	m_bBold = bBold;

	//////////
	m_logfont.lfHeight = MulDiv(nSize, 72, GetDeviceCaps(m_hDC, LOGPIXELSY));
	m_logfont.lfWeight= bBold ? FW_BOLD : FW_NORMAL;
	m_logfont.lfCharSet=CHINESEBIG5_CHARSET;
	_tcscpy((TCHAR*)m_logfont.lfFaceName, strFontName); 

	m_hFont = CreateFontIndirect(&m_logfont);
}