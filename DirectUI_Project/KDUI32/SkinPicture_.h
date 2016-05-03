#pragma once
#include "SkinShareObj_.h"

class KDUI32_API SkinPicture_ : public SkinShareObj_
{
public:
	SkinPicture_():m_hBitmap(NULL){}
	virtual ~SkinPicture_(){}

	BOOL LoadImage(LPCTSTR strFilePath, enumDUIPICTYPE pictype=DUI_PICTYPE_BMP)
	{
		m_renderImage.SetPicType(pictype);
		if (!m_renderImage.LoadImage(strFilePath, pictype)) 
			return FALSE;

		m_hBitmap	 = m_renderImage.GetHBitmap();

		return TRUE;
	}

	CRenderImage_GDI_* GetRenderImage(){return &m_renderImage;}

protected:
	string m_strFilePath;
	HBITMAP m_hBitmap;
	CRenderImage_GDI_ m_renderImage;
};