#pragma once

enum _ObjType
{
	OBJTYPE_UNDEFINED = 100,
	OBJTYPE_IMAGE,
	OBJTYPE_IMAGEBASE,
	OBJTYPE_TEXTSTYLE,
	OBJTYPE_FONTEX,
	OBJTYPE_DIRECTUI,
	OBJTYPE_CONTROL,
	OBJTYPE_LAST
};

typedef struct DUINotifyInfo
{
	long eDUIMsgId;
	long eWinMsgId;
	BOOL bChildren;
	DWORD_PTR lParam1;
	DWORD_PTR lParam2;
	DWORD_PTR lParam3;
	DWORD_PTR lParam4;
	DWORD_PTR lParam5;
};