#pragma once 

#include "defines.h"
#include "IRenderDC_.h"
#include "stringex.h"
#include <vector>

using namespace std;

// class CDirectUI_;
// class CDUIResource_;
class SkinObjResBase_;

///////////////////////////
///////////////////////////
class KDUI32_API CSubclassWnd
{
	friend class CSubclassWndMap;

public:
	CSubclassWnd();
	~CSubclassWnd();

	// Subclass a window. Hook(NULL) to unhook (automatic on WM_NCDESTROY)
	BOOL	HookWindow(HWND  hwnd);
	void	Unhook();
	BOOL	IsHooked() {return m_hWnd!=NULL; }

	friend	LRESULT CALLBACK HookWndProc(HWND, UINT, WPARAM, LPARAM);
	virtual	LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
	LRESULT Default();				// call this at the end of handler fns

	static MSG& GetCurrMsg();

public:
	inline HWND GetHwnd(){return m_hWnd;}
	inline WNDPROC& GetOldWndProc(){return m_pOldWndProc;}

protected:
	HWND					m_hWnd;				// the window hooked
	WNDPROC			m_pOldWndProc;		// ..and original window proc
	CSubclassWnd*	m_pNext;				// next in chain of hooks for this window

	static MSG s_curMsg;
};


//////////////////////////////////////////////////////////////////////////
//类  名 : class CMsgHooker
//功  能 : 把指定句柄的消息发送到指定的CObjectBase
//////////////////////////////////////////////////////////////////////////
class KDUI32_API CMsgHooker : public CSubclassWnd
{
public:
	CMsgHooker();
	virtual ~CMsgHooker();

public:
	void SetMsgRecv(SkinObjResBase_* pObjBase){ m_MsgRecvObj = pObjBase; }
	BOOL HasMsgRecv() { return m_MsgRecvObj!=NULL; }

public:
	LRESULT BaseProc(HWND hWnd,UINT msg, WPARAM wp, LPARAM lp);
	virtual LRESULT WindowProc(HWND hWnd,UINT msg, WPARAM wp, LPARAM lp);

protected:
	SkinObjResBase_* m_MsgRecvObj; //消息接受者容器

};

////////////////////////////////
////////////////////////////////
class CDUIResource_;
class CDirectUI_;

class KDUI32_API CDUIObj_
{
public:
	CDUIObj_();
	virtual ~CDUIObj_();

public:
	tstring GetName();
	void SetName(LPCTSTR strName);
	_ObjType GetType();

	virtual	BOOL AccessConfig(BOOL bRead);
	void SetDispatch(IDispatch *pDispatch);
	IDispatch* GetDispatch();
	CDUIResource_* GetDUIRes();
	void SetDUIRes(CDUIResource_ *pDUIRes);

protected:
	tstring m_strName;
	_ObjType m_objType;
	IDispatch *m_pDispatch;
	CDUIResource_ *m_pDUIResource;
};


class KDUI32_API SkinObjResBase_ : public CDUIObj_
{
public:
	SkinObjResBase_();
	virtual ~SkinObjResBase_();

	void DestroyObjects();

public:
	virtual BOOL AccessConfig(BOOL bRead);
	virtual BOOL RenderDraw(IRenderDC_ *pDC);

	void SetRect(const RECT& rect);
	RECT GetRect();

	void SetParent(SkinObjResBase_ *pObjParent);

	SkinObjResBase_* GetParent();

	SkinObjResBase_* GetRoot();

	BOOL AddObject(SkinObjResBase_ *pObject);

	BOOL RemoveObject(SkinObjResBase_ *pObject);

	//获取节点
	SkinObjResBase_* GetSubObject(int nIndex);
	int GetSubObjIndex(SkinObjResBase_* pSubObj);
	size_t GetChildCount();

	//移动兄弟节点
	BOOL MoveBack(SkinObjResBase_* pSubObj);
	BOOL MoveAhead(SkinObjResBase_* pSubObj);
	BOOL MoveTop(SkinObjResBase_* pSubObj);
	BOOL MoveBottom(SkinObjResBase_* pSubObj);

	//深度优先递归遍历:参数1前序、2后续、3从左到右
	//	渲染顺序：从左到右前序
	//  控件删除顺序：从左到有后序
	//  鼠标命中检测：从右到左前序
	void DepthFirst_Iterate(BOOL bForward, BOOL bBackward, BOOL bLeft2Right = TRUE);

	CDirectUI_* GetDirectUI();

	IRenderDC_* GetRenderDC();

	void SetDirectUIHwnd(HWND hWnd);

	virtual BOOL WindowProc(HWND hWnd,UINT uMsgID,WPARAM wParam,LPARAM lParam, LRESULT& lResult);

	//*********
//******************
    virtual BOOL OnEventNotify(DUINotifyInfo* lpNotifyInfo);
    BOOL DoNotifyMessage(long nDUIMsgId,long nWinMsgId=-1,LONG_PTR nParam1=-1,LONG_PTR nParam2=-1, LONG_PTR nParam3=-1 ,
                         LONG_PTR nParam4=-1, LONG_PTR nParam5=-1,BOOL bChildren=FALSE);

protected:
	RECT  m_rect;
	std::deque<SkinObjResBase_*> m_dequeObjects;
	HWND m_hWndDirectUI;
	SkinObjResBase_ *m_pObjParent;
};

////////////////////////////////////
////////////////////////////////////
class KDUI32_API SkinControlBase_ : public SkinObjResBase_
{
public:
	SkinControlBase_();
	virtual ~SkinControlBase_();

public:
	virtual BOOL RenderDraw(IRenderDC_ *pDC);

//**************
    virtual BOOL OnEventNotify(DUINotifyInfo* lpNotifyInfo);
	
private:

};

////////////////////////////////////
////////////////////////////////////
//typedef void (*UpdateTimerProc)(CDirectUI_* pDirectUI);

class KDUI32_API CDirectUI_ : public SkinObjResBase_
{
private:
	HWND m_hWnd;
	CMsgHooker m_msghook;
	CRenderDC_GDI_ m_dcRender;

	//
	SkinObjResBase_* m_pObjOldMouseMove;

private:
//	UpdateTimeProc m_updateTimeProc;
	vector<RECT>* m_vecUpdateRect;

public:
	CDirectUI_();
	virtual ~CDirectUI_();

public:
	BOOL Create(HWND hWnd);

	HWND GetHwnd();

	virtual BOOL WindowProc(HWND hWnd,UINT uMsgID,WPARAM wParam,LPARAM lParam, LRESULT& lResult);

	SkinObjResBase_* _FindObjByPoint(POINT point);

	void OnPaint();

	void OnSize();

	//
// 	void RegisterCallbackUpdate(UpdateTimerProc updateProc)
// 	{
// 		m_updateTimerProc = updateProc;
// 	}
	void UpdateTimerProc(CDirectUI_* pDirectUI);

	vector<RECT>* GetUpdateVecRect();

};

////////////////////////
class KDUI32_API CDUIResource_
{
private:
	string strSkinPath;
	IDispatch *m_pDUIResComPtr;
	deque<CDirectUI_*> m_dequeDirectUIs;

public:
	CDUIResource_();
	virtual ~CDUIResource_();

public:
	BOOL OpenSkin(LPCSTR strSkinPath);

	void CloseSkin();

	void DestroyDirectUIs();

	CDirectUI_* CreateDirectUI(HWND hWnd);

	SkinControlBase_* CreateControl(CDirectUI_ *pDirectUI,BSTR strClsid);

	string GetSkinPath();

	IDispatch* GetDUIResComPtr();
	void SetDUIResComPtr(IDispatch *pDUIRes);


};

