#include "stdafx.h"
#include "SkinObjResBase_.h"
#include "IRenderDC_.h"
#include <map>
#include <atltypes.h>

using namespace std;


// #ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
// #endif

#ifndef GWL_WNDPROC
#define GWL_WNDPROC         (-4)
#endif

MSG	CSubclassWnd::s_curMsg = {0};

#if (_MSC_VER <= 1200) && !defined(_WIN64)
#define GetWindowLongPtrPtrW GetWindowLongPtrW
#define GetWindowLongPtrPtrA GetWindowLongPtrA
#define SetWindowLongPtrW SetWindowLongW
#define SetWindowLongPtrA SetWindowLongA
#endif

class CSubclassWndMap
{
public:
	CSubclassWndMap();
	~CSubclassWndMap();
	static CSubclassWndMap& GetHookMap();
	void Add(HWND hwnd, CSubclassWnd* pSubclassWnd);
	void Remove(CSubclassWnd* pSubclassWnd);
	void RemoveAll(HWND hwnd);
	CSubclassWnd* Lookup(HWND hwnd);

protected:
	map<HWND,CSubclassWnd*> m_mapSubclasswnd;
};

#define	theHookMap	(CSubclassWndMap::GetHookMap())

CSubclassWnd::CSubclassWnd()
{
	m_pNext = NULL;
	m_pOldWndProc = NULL;	
	m_hWnd  = NULL;
}

CSubclassWnd::~CSubclassWnd()
{
	if (m_hWnd) 
		HookWindow((HWND)NULL);		// unhook window
}

void	CSubclassWnd::Unhook()
{
	HookWindow((HWND)NULL);
}

//////////////////
// Hook a window.
// This installs a new window proc that directs messages to the CSubclassWnd.
// pWnd=NULL to remove.
//
BOOL CSubclassWnd::HookWindow(HWND hwnd)
{
	if (hwnd) {
		// Hook the window
		theHookMap.Add(hwnd, this);			// Add to map of hooks
	} else if (m_hWnd) {
		// Unhook the window
		theHookMap.Remove(this);				// Remove from map
		m_pOldWndProc = NULL;
	}
	m_hWnd = hwnd;
	return TRUE;
}

LRESULT CSubclassWnd::WindowProc(HWND hWnd,UINT msg, WPARAM wp, LPARAM lp)
{
	return m_pNext ? m_pNext->WindowProc(hWnd,msg, wp, lp) :	
		::CallWindowProc(m_pOldWndProc, m_hWnd, msg, wp, lp);
}

MSG& CSubclassWnd::GetCurrMsg()
{
	return s_curMsg;
}

//////////////////
// Like calling base class WindowProc, but with no args, so individual
// message handlers can do the default thing. Like CWnd::Default
//
LRESULT CSubclassWnd::Default()
{
	MSG& curMsg = GetCurrMsg();
	// MFC stores current MSG in thread state
	//MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;
	// Note: must explicitly call CSubclassWnd::WindowProc to avoid infinte
	// recursion on virtual function
	return CSubclassWnd::WindowProc(curMsg.hwnd,curMsg.message, curMsg.wParam, curMsg.lParam);
}

#ifdef _DEBUG
//void CSubclassWnd::AssertValid() const
//{
//	CObject::AssertValid();
//	//assert(m_hWnd==NULL || ::IsWindow(m_hWnd));
//	if (m_hWnd) 
//	{
//		CSubclassWnd* p = NULL;
//		for (p = theHookMap.Lookup(m_hWnd); p; p=p->m_pNext) 
//		{
//			if (p==this)
//				break;
//		}
//		//assert(p); // should have found it!
//	}
//}
//
//void CSubclassWnd::Dump(CDumpContext& dc) const
//{
//	CObject::Dump(dc);
//}

#endif

//////////////////
// Subclassed window proc for message hooks. Replaces AfxWndProc (or whatever
// else was there before.)
//
LRESULT CALLBACK
HookWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	MSG& curMsg = CSubclassWnd::GetCurrMsg();
	MSG  oldMsg = curMsg; 

	curMsg.hwnd	= hwnd;
	curMsg.message = msg;
	curMsg.wParam  = wp;
	curMsg.lParam  = lp;

	// Set up MFC message state just in case anyone wants it
	// This is just like AfxCallWindowProc, but we can't use that because
	// a CSubclassWnd is not a CWnd.
	//

	// Get hook object for this window. Get from hook map
	CSubclassWnd* pSubclassWnd = theHookMap.Lookup(hwnd);
	//assert(pSubclassWnd);

	LRESULT lr;
	if (msg==WM_NCDESTROY) {
		// Window is being destroyed: unhook all hooks (for this window)
		// and pass msg to orginal window proc
		//
		WNDPROC wndproc = pSubclassWnd->m_pOldWndProc;
		theHookMap.RemoveAll(hwnd);
		lr = ::CallWindowProc(wndproc, hwnd, msg, wp, lp);

	} else {
		// pass to msg hook
		if (pSubclassWnd)
			lr = pSubclassWnd->WindowProc(hwnd,msg, wp, lp);
	}
	curMsg = oldMsg;			// pop state
	return lr;
}

////////////////////////////////////////////////////////////////
// CSubclassWndMap implementation
//
CSubclassWndMap::CSubclassWndMap()
{
}

CSubclassWndMap::~CSubclassWndMap()
{
	// This //assert bombs when posting WM_QUIT, so I've deleted it.
	//	//assert(IsEmpty());	// all hooks should be removed!	
}

//////////////////
// Get the one and only global hook map
// 
CSubclassWndMap& CSubclassWndMap::GetHookMap()
{
	// By creating theMap here, C++ doesn't instantiate it until/unless
	// it's ever used! This is a good trick to use in C++, to
	// instantiate/initialize a static object the first time it's used.
	//
	static CSubclassWndMap theMap;
	return theMap;
}

/////////////////
// Add hook to map; i.e., associate hook with window
//
void CSubclassWndMap::Add(HWND hwnd, CSubclassWnd* pSubclassWnd)
{
	//assert(hwnd && ::IsWindow(hwnd));

	// Add to front of list
	pSubclassWnd->m_pNext = Lookup(hwnd);
	//SetAt(hwnd, pSubclassWnd);
	m_mapSubclasswnd[hwnd] = pSubclassWnd;

	if (pSubclassWnd->m_pNext==NULL) {
		// If this is the first hook added, subclass the window
		pSubclassWnd->m_pOldWndProc = 
			(WNDPROC)SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)HookWndProc);

	} else {
		// just copy wndproc from next hook
		pSubclassWnd->m_pOldWndProc = pSubclassWnd->m_pNext->m_pOldWndProc;
	}
	//assert(pSubclassWnd->m_pOldWndProc);
}

//////////////////
// Remove hook from map
//
void CSubclassWndMap::Remove(CSubclassWnd* pUnHook)
{
	HWND hwnd = pUnHook->m_hWnd;
	//assert(hwnd && ::IsWindow(hwnd));

	CSubclassWnd* pHook = Lookup(hwnd);
	//assert(pHook);
	if (pHook==pUnHook) {
		// hook to remove is the one in the hash table: replace w/next
		if (pHook->m_pNext)
			//SetAt(hwnd, pHook->m_pNext);
			m_mapSubclasswnd[hwnd] = pHook->m_pNext;
		else {
			// This is the last hook for this window: restore wnd proc
			//RemoveKey(hwnd);
			map<HWND,CSubclassWnd*>::iterator it = m_mapSubclasswnd.find(hwnd);
			m_mapSubclasswnd.erase(it);

			LONG nResult = SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)pHook->m_pOldWndProc);
			//DWORD dwError = GetLastError();
		}
	} else {
		// Hook to remove is in the middle: just remove from linked list
		while (pHook->m_pNext!=pUnHook)
			pHook = pHook->m_pNext;
		//assert(pHook && pHook->m_pNext==pUnHook);
		pHook->m_pNext = pUnHook->m_pNext;
	}
}

//////////////////
// Remove all the hooks for a window
//
void CSubclassWndMap::RemoveAll(HWND hwnd)
{
	CSubclassWnd* pSubclassWnd;
	while ((pSubclassWnd = Lookup(hwnd))!=NULL)
		pSubclassWnd->HookWindow((HWND)NULL);	// (unhook)
}

/////////////////
// Find first hook associate with window
//
CSubclassWnd* CSubclassWndMap::Lookup(HWND hwnd)
{
	map<HWND,CSubclassWnd*>::iterator it = m_mapSubclasswnd.find(hwnd);
	if (it == m_mapSubclasswnd.end()) return NULL;

	return it->second;
}

///////////////////////////////
///////////////////////////////
///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//class CMsgHooker
CMsgHooker::CMsgHooker() : CSubclassWnd(),
m_MsgRecvObj(NULL)
{

}

CMsgHooker::~CMsgHooker()
{
}

LRESULT CMsgHooker::BaseProc(HWND hWnd,UINT msg, WPARAM wp, LPARAM lp)
{
	return CSubclassWnd::WindowProc(hWnd,msg, wp, lp);
}

LRESULT CMsgHooker::WindowProc(HWND hWnd,UINT msg, WPARAM wp, LPARAM lp)
{
	BOOL bTurnDefProc = TRUE;
	LRESULT lRes = FALSE;

	BOOL bSendToUser = FALSE;

	if(HasMsgRecv())
	{
		CDirectUI_* pDirectUI = dynamic_cast<CDirectUI_*>(m_MsgRecvObj);
		if (pDirectUI)
		{
			bSendToUser = TRUE;
		}

		bTurnDefProc = m_MsgRecvObj->WindowProc(hWnd,msg, wp, lp, lRes);
	}

	if(bTurnDefProc == -1) //  拖拽后不需要发送LButtonDown消息 2011-9-20 yyc
		return lRes;
	else if((bTurnDefProc && m_pOldWndProc) || bSendToUser)
		return CSubclassWnd::WindowProc(hWnd,msg, wp, lp);
	else
		return lRes;
}

/////////////////////////
/////////////////////////
/////////////////////////

CDUIObj_::CDUIObj_()
:m_pDispatch(NULL),m_objType(OBJTYPE_UNDEFINED),m_pDUIResource(NULL)
{

}

CDUIObj_::~CDUIObj_()
{

}

tstring CDUIObj_::GetName()
{
	return m_strName;
}

void CDUIObj_::SetName(LPCTSTR strName)
{
	m_strName = strName;
}

_ObjType CDUIObj_::GetType(){return m_objType;}

BOOL CDUIObj_::AccessConfig(BOOL bRead)
{
	return TRUE;
}

void CDUIObj_::SetDispatch(IDispatch *pDispatch)
{
	m_pDispatch = pDispatch;
}

IDispatch* CDUIObj_::GetDispatch()
{
	return m_pDispatch;
}

CDUIResource_* CDUIObj_::GetDUIRes()
{
	return m_pDUIResource;
}

void CDUIObj_::SetDUIRes(CDUIResource_ *pDUIRes)
{
	m_pDUIResource = pDUIRes;
}

//////////////////////
//////////////////////
//////////////////////

SkinObjResBase_::SkinObjResBase_()
	:m_hWndDirectUI(NULL),m_pObjParent(NULL)
{

}
SkinObjResBase_::~SkinObjResBase_()
{
	DestroyObjects();
}

void SkinObjResBase_::DestroyObjects()
{
	for (size_t i=0;i<m_dequeObjects.size();i++)
	{
		SkinObjResBase_* pObj = m_dequeObjects[i];
		IDispatch *pCom = pObj->GetDispatch();
		if (pCom) pCom->Release();

		delete pObj;
	}

	m_dequeObjects.clear();
}

BOOL SkinObjResBase_::AccessConfig(BOOL bRead)
{
	return TRUE;
}

BOOL SkinObjResBase_::RenderDraw(IRenderDC_ *pDC)
{
	for (size_t i=0;i<m_dequeObjects.size();i++)
	{
		SkinObjResBase_* pObj = m_dequeObjects[i];
		pObj->RenderDraw(pDC);
	}

	return TRUE;
}

void SkinObjResBase_::SetRect(const RECT& rect)
{
	m_rect = rect;
}

RECT SkinObjResBase_::GetRect()
{
	return m_rect;
}

void SkinObjResBase_::SetParent(SkinObjResBase_ *pObjParent)
{
	m_pObjParent = pObjParent;
}

SkinObjResBase_* SkinObjResBase_::GetParent()
{
	return m_pObjParent;
}

SkinObjResBase_* SkinObjResBase_::GetRoot()
{
	SkinObjResBase_* pParent = GetParent();
	if (pParent)
		return pParent->GetRoot();

	return this;
}

BOOL SkinObjResBase_::AddObject(SkinObjResBase_ *pObject)
{
	if (pObject == NULL) return FALSE;

	for(size_t i=0;i<m_dequeObjects.size();i++)
	{
		if (pObject == m_dequeObjects[i])
			return FALSE;
	}

	m_dequeObjects.push_back(pObject);

	return TRUE;
}

BOOL SkinObjResBase_::RemoveObject(SkinObjResBase_ *pObject)
{
	if (pObject == NULL) return FALSE;

	for(size_t i=0;i<m_dequeObjects.size();i++)
	{
		if (pObject == m_dequeObjects[i])
		{
			m_dequeObjects.erase(m_dequeObjects.begin() + i);
			return TRUE;
		}
	}

	return FALSE;
}

//获取节点
SkinObjResBase_* SkinObjResBase_::GetSubObject(int nIndex)
{
	if ( nIndex < 0 || nIndex >= m_dequeObjects.size() )
	{
		return NULL;
	}

	SkinObjResBase_* pSubObj = m_dequeObjects[nIndex];
	return pSubObj;
}

int SkinObjResBase_::GetSubObjIndex(SkinObjResBase_* pSubObj)
{
	size_t nCount = GetChildCount();
	for ( size_t i=0; i<nCount; i++ )
	{
		if ( m_dequeObjects[i] == pSubObj )
		{
			return i;
		}
	}

	return -1;
}

size_t SkinObjResBase_::GetChildCount()
{
	return m_dequeObjects.size();
}

//移动兄弟节点
BOOL SkinObjResBase_::MoveBack(SkinObjResBase_* pSubObj)
{
	int nPos = GetSubObjIndex(pSubObj);
	if ( nPos == m_dequeObjects.size()-1 )
	{
		return TRUE;
	}

	SkinObjResBase_* pTmpObj = m_dequeObjects[nPos];
	m_dequeObjects[nPos] = m_dequeObjects[nPos+1];
	m_dequeObjects[nPos+1] = pTmpObj;
	return TRUE;
}

BOOL SkinObjResBase_::MoveAhead(SkinObjResBase_* pSubObj)
{
	int nPos = GetSubObjIndex(pSubObj);
	if ( nPos <= 0 )
	{
		return TRUE;
	}

	SkinObjResBase_* pTmpObj = m_dequeObjects[nPos];
	m_dequeObjects[nPos] = m_dequeObjects[nPos-1];
	m_dequeObjects[nPos-1] = pTmpObj;
	return TRUE;
}

BOOL SkinObjResBase_::MoveTop(SkinObjResBase_* pSubObj)
{
	int nPos = GetSubObjIndex(pSubObj);
	if ( nPos <= 0 )
	{
		return TRUE;
	}

	return TRUE;
}

BOOL SkinObjResBase_::MoveBottom(SkinObjResBase_* pSubObj)
{
	return TRUE;
}

void SkinObjResBase_::DepthFirst_Iterate(BOOL bForward, BOOL bBackward, BOOL bLeft2Right /*= TRUE*/)
{
	//前序
	if ( bForward == TRUE )
	{
		//DoForwardSomething(this);
	}

	//开始递归
	if ( bLeft2Right )
	{
		for ( size_t i=0; i<GetChildCount(); i++ )
		{
			GetSubObject(i)->DepthFirst_Iterate(bForward, bBackward, bLeft2Right);
		}
	}
	else
	{
		for ( size_t i= GetChildCount()-1; i >= 0; i-- )
		{
			GetSubObject(i)->DepthFirst_Iterate(bForward, bBackward, bLeft2Right);
		}
	}

	//后序
	if ( bBackward == TRUE )
	{
		//DoBackwardSomething(this);
	}
}

IRenderDC_* SkinObjResBase_::GetRenderDC()
{
	return NULL;
}

CDirectUI_* SkinObjResBase_::GetDirectUI()
{
	if (::IsWindow(m_hWndDirectUI))
	{
		return (CDirectUI_*)(GetWindowLongPtr(m_hWndDirectUI,GWL_USERDATA));
	}

	SkinObjResBase_* pRoot = GetRoot();
	if (pRoot && pRoot->GetType() == OBJTYPE_DIRECTUI)
		return (CDirectUI_*)pRoot;

	return NULL;
}

void SkinObjResBase_::SetDirectUIHwnd(HWND hWnd){m_hWndDirectUI = hWnd;}

BOOL SkinObjResBase_::WindowProc(HWND hWnd,UINT uMsgID,WPARAM wParam,LPARAM lParam, LRESULT& lResult)
{
	return 0;
}

//*********
//******************
BOOL SkinObjResBase_::OnEventNotify(DUINotifyInfo* lpNotifyInfo)
{
	if(!lpNotifyInfo->bChildren)
		return FALSE;

	BOOL bDoDefault = FALSE;
	for (int i=0; i<m_dequeObjects.size(); i++) {
		SkinObjResBase_* pResStruct = m_dequeObjects[i];
		//
		if (pResStruct->OnEventNotify(lpNotifyInfo)) {
			bDoDefault = TRUE;
		}
		return !bDoDefault;
	}
}
BOOL SkinObjResBase_::DoNotifyMessage(long nDUIMsgId,long nWinMsgId/*=-1*/,LONG_PTR nParam1/*=-1*/,LONG_PTR nParam2/*=-1*/, LONG_PTR nParam3/*=-1*/ ,
									  LONG_PTR nParam4/*=-1*/, LONG_PTR nParam5/*=-1*/,BOOL bChildren/*=FALSE*/)
{
	DUINotifyInfo duiNotifyInfo;
	memset(&duiNotifyInfo, 0, sizeof(DUINotifyInfo));
	duiNotifyInfo.eDUIMsgId = (DUIMsgID/*DirectUIMsgID*/)nDUIMsgId;
	duiNotifyInfo.eWinMsgId = (long/*WinMsgID*/)nWinMsgId;
	duiNotifyInfo.lParam1 = nParam1;
	duiNotifyInfo.lParam2 = nParam2;
	duiNotifyInfo.lParam3 = nParam3;
	duiNotifyInfo.lParam4 = nParam4;
	duiNotifyInfo.lParam5 = nParam5;
	duiNotifyInfo.bChildren = bChildren;

	return OnEventNotify(&duiNotifyInfo);
}

///////////////////////////
///////////////////////////
///////////////////////////
SkinControlBase_::SkinControlBase_()
{
	m_objType = OBJTYPE_CONTROL;
}

SkinControlBase_::~SkinControlBase_()
{

}

BOOL SkinControlBase_::RenderDraw(IRenderDC_ *pDC)
{
	IDUIControlBase *pDUIControlBase = (IDUIControlBase*)m_pDispatch;
	SkinRect rect = {m_rect.left,m_rect.top,m_rect.right,m_rect.bottom};
	VARIANT_BOOL bResult = VARIANT_FALSE;
	pDUIControlBase->RenderDrawObject(pDC->GetRenderDCComPtr(),&rect,&bResult);

	SkinObjResBase_::RenderDraw(pDC);

	return TRUE;
}

//**************
BOOL SkinControlBase_::OnEventNotify(DUINotifyInfo* lpNotifyInfo)
{
	if (m_pDispatch == NULL) {
		return FALSE;
	}

	BOOL bDoDefault = FALSE;

	IDUIControlBase* pControlBase = (IDUIControlBase*)m_pDispatch;

	DUINotify duiNotify;
	duiNotify.eDuiMsgId = (DUIMsgID)lpNotifyInfo->eDUIMsgId;
	duiNotify.eWinMsgId = (long)lpNotifyInfo->eWinMsgId;
	duiNotify.lParam1 = (long)lpNotifyInfo->lParam1;
	duiNotify.lParam2 = (long)lpNotifyInfo->lParam2;
	duiNotify.lParam3 = (long)lpNotifyInfo->lParam3;
	duiNotify.lParam4 = (long)lpNotifyInfo->lParam4;
	duiNotify.lParam5 = (long)lpNotifyInfo->lParam5;

	VARIANT_BOOL bResult = VARIANT_FALSE;
	pControlBase->EventNotify(&duiNotify, &bResult);

	//
	if (lpNotifyInfo->eWinMsgId != -1 ) {
		if (bResult) {
			bDoDefault = TRUE;
		}
	}

	//
	if (lpNotifyInfo->bChildren) {
		if(SkinObjResBase_::OnEventNotify(lpNotifyInfo))
		{
			bDoDefault = TRUE;
		}
	}

	//
	if (lpNotifyInfo->eDUIMsgId != -1) {
		return bResult;
	}
	return !bDoDefault;
}

///////////////////////////
///////////////////////////
///////////////////////////
CDirectUI_::CDirectUI_()
{
	m_objType = OBJTYPE_DIRECTUI;
	m_pObjOldMouseMove = NULL;
}

CDirectUI_::~CDirectUI_()
{

}


BOOL CDirectUI_::Create(HWND hWnd)
{
	m_hWnd = hWnd;

	m_msghook.SetMsgRecv(this);
	m_msghook.HookWindow(hWnd);

	/////////
// 	RECT rc = GetRect();
// 	m_vecUpdateRect->push_back(rc);
// 
// 	//
// 	::SetTimer(m_hWnd, 101,30, NULL);

	

	return TRUE;
}

HWND CDirectUI_::GetHwnd()
{
	return  m_hWnd;
}

vector<RECT>* CDirectUI_::GetUpdateVecRect()
{
	return m_vecUpdateRect;
}

void CDirectUI_::UpdateTimerProc(CDirectUI_* pDirectUI)
{
	vector<RECT>* pUpdateRect = pDirectUI->GetUpdateVecRect();
	if ( pUpdateRect->size() == 0 )
	{
		return;
	}

	//最小化时
	if ( ::IsIconic(pDirectUI->GetHwnd()) )
	{
		pUpdateRect->clear();
		return;
	}

	//合并新的脏区
	CRect rcMerge;
	vector<RECT>::iterator it = pUpdateRect->begin();
	for ( ; it != pUpdateRect->end(); ++it )
	{
		rcMerge.UnionRect(rcMerge, &(*it));
	}

	if ( rcMerge.IsRectEmpty() )
	{
		pUpdateRect->clear();
		return;
	}

	//得到一个脏区不为0的rect
	HDC pDevContext = NULL;
	pDevContext = ::GetDC(pDirectUI->GetHwnd());

	IRenderDC_* pRenderDC = pDirectUI->GetRenderDC();
	if ( !pRenderDC )
	{
		return;
	}

	//////////
 	pRenderDC->BeginRender(pDevContext, rcMerge, RGB(255,255,255));
 	pRenderDC->Clear(rcMerge);
 
 	pRenderDC->RenderDraw(pRenderDC, rcMerge);
 	pRenderDC->EndRender(rcMerge,TRUE);

	pUpdateRect->clear();
}

BOOL CDirectUI_::WindowProc(HWND hWnd,UINT uMsgID,WPARAM wParam,LPARAM lParam, LRESULT& lResult)
{
	DUIMsgID msgid = DUISM_RESKIN;

	switch(uMsgID)
	{
	case WM_TIMER:
		{
			if ( wParam == 101 )
			{
				UpdateTimerProc(this);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			msgid = DUISM_LBUTTONDOWN;

			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);

// 			SkinObjResBase_* pObjRoot = GetRoot();
// 			if ( pObjRoot )
// 			{
// 				size_t nCount = pObjRoot->GetChildCount();
// 				for ( int i=0; i<nCount; i++ )
// 				{
// 					SkinObjResBase_* pSubObj = pObjRoot->GetSubObject(i);
// 					if ( pSubObj )
// 					{
// 						RECT rcSubObj = pSubObj->GetRect();
// 					}
// 				}
// 			}

			//
			SkinObjResBase_* pObjCur = _FindObjByPoint(point);

			///////////////////////////////
			if ( pObjCur )
			{
				//OutputDebugString(_T("lbuttondown: \n"));
				IDUIControlBase* pCtrl = (IDUIControlBase*)pObjCur->GetDispatch();
				RECT rc = pObjCur->GetRect();

				//
				HDC hdc = m_dcRender.GetHDC();

				DUINotifyInfo notifyinfo;
				notifyinfo.eDUIMsgId = DUISM_LBUTTONDOWN;
				notifyinfo.eWinMsgId = WM_LBUTTONDOWN;
				notifyinfo.lParam1 = (long)pCtrl;
				notifyinfo.lParam2 = (long)point.x;
				notifyinfo.lParam3 = (long)point.y;
				notifyinfo.lParam4 = (long)pCtrl;
				notifyinfo.lParam5 = (long)m_hWnd;
				notifyinfo.bChildren = VARIANT_TRUE;
				OnEventNotify(&notifyinfo);

				/*				IDUIControlBase *pResult = NULL;
				VARIANT_BOOL bResult = VARIANT_FALSE;
				DUINotify duiNotify;
				duiNotify.eDuiMsgId = DUISM_LBUTTONDOWN;
				duiNotify.eWinMsgId = (LONG)-1;
				duiNotify.lParam1 = (long)&pResult;
				duiNotify.lParam4 = (long)pCtrl;
				duiNotify.lParam5 = (long)m_hWnd;
				duiNotify.bChildren = VARIANT_TRUE;
				pCtrl->EventNotify(&duiNotify,&bResult);

				::InvalidateRect(m_hWnd, &rc, FALSE);*/
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			msgid = DUISM_LBUTTONUP;

			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);

			//
			SkinObjResBase_* pObjCur = _FindObjByPoint(point);

			///////////////////////////////
			if ( pObjCur )
			{
				//OutputDebugString(_T("lbuttondown: \n"));
				IDUIControlBase* pCtrl = (IDUIControlBase*)pObjCur->GetDispatch();
				RECT rc = pObjCur->GetRect();

				IDUIControlBase *pResult = NULL;
				VARIANT_BOOL bResult = VARIANT_FALSE;
				DUINotify duiNotify;
				duiNotify.eDuiMsgId = DUISM_LBUTTONUP;
				duiNotify.eWinMsgId = (LONG)-1;
				duiNotify.lParam1 = (long)&pResult;
				duiNotify.bChildren = VARIANT_TRUE;
				if ( point.x >= rc.left && point.x <= rc.right &&
					point.y >= rc.top && point.y <= rc.bottom )
				{
					duiNotify.lParam2 = 1;
				}
				else
				{
					duiNotify.lParam2 = 0;
				}
				pCtrl->EventNotify(&duiNotify,&bResult);

				::InvalidateRect(m_hWnd, &rc, FALSE);
			}
		}
		break;
	case WM_PAINT:
		{
			OnPaint();
			return FALSE;
		}
		break;

	case WM_SIZE:
		{
			OnSize();
			msgid = DUISM_SIZE;
			break;
		}
	case WM_MOUSEMOVE:
		{		
			msgid = DUISM_MOUSEMOVE;

			POINT point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);

			//
			SkinObjResBase_* pObjCur = _FindObjByPoint(point);

			///////////////////////////////
			if ( m_pObjOldMouseMove != pObjCur && m_pObjOldMouseMove )
			{
				//上一个mousemove的ctrl发送MOUSE_LEAVE
				//OutputDebugString(_T("mouseleave: \n"));
				IDUIControlBase* pCtrl = (IDUIControlBase*)m_pObjOldMouseMove->GetDispatch();
				RECT rc = m_pObjOldMouseMove->GetRect();

				IDUIControlBase *pResult = NULL;
				VARIANT_BOOL bResult = VARIANT_FALSE;
				DUINotify duiNotify;
				duiNotify.eDuiMsgId = DUISM_MOUSELEAVE;
				duiNotify.eWinMsgId = (LONG)-1;
				duiNotify.lParam1 = (long)&pResult;
				duiNotify.lParam2 = (long)point.x;
				duiNotify.lParam3 = (long)point.y;
				duiNotify.lParam4 = (long)m_dcRender.GetHDC();
				duiNotify.bChildren = VARIANT_TRUE;
				pCtrl->EventNotify(&duiNotify,&bResult);

				::InvalidateRect(m_hWnd, &rc, TRUE);
			}

			///mousemove
			if ( pObjCur && m_pObjOldMouseMove != pObjCur )
			{
				IDUIControlBase* pCtrl = (IDUIControlBase*)pObjCur->GetDispatch();
				RECT rc = pObjCur->GetRect();

				//OutputDebugString(_T("mousemove: \n"));

				IDUIControlBase *pResult = NULL;
				VARIANT_BOOL bResult = VARIANT_FALSE;
				DUINotify duiNotify;
				duiNotify.eDuiMsgId = msgid;
				duiNotify.eWinMsgId = (LONG)-1;
				duiNotify.lParam1 = (long)&pResult;
				duiNotify.lParam2 = (long)point.x;
				duiNotify.lParam3 = (long)point.y;
				duiNotify.bChildren = VARIANT_TRUE;
				pCtrl->EventNotify(&duiNotify,&bResult);

				::InvalidateRect(m_hWnd, &rc, TRUE);
			}

			m_pObjOldMouseMove = pObjCur;


			break;
		}
	}	

	return TRUE;
}

SkinObjResBase_* CDirectUI_::_FindObjByPoint(POINT point)
{
	SkinObjResBase_* pObjCur = NULL;

	int nCtrlCount = m_dequeObjects.size();
	std::deque<SkinObjResBase_*>::iterator it = m_dequeObjects.begin();
	for ( ; it != m_dequeObjects.end(); ++it )
	{
		SkinObjResBase_* pObj = (*it);
		if ( !pObj )
		{
			continue;
		}

		RECT rc = pObj->GetRect();
		if ( ::PtInRect(&rc, point) )
		{
			pObjCur = pObj;
			break;
		}
	}

	return pObjCur;
}

void CDirectUI_::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd,&ps);
	m_dcRender.SetHDC(hDC);

	RECT rcClient;
	::GetClientRect(m_hWnd,&rcClient);

	RenderDraw(&m_dcRender);

	::EndPaint(m_hWnd,&ps);
}

void CDirectUI_::OnSize()
{
}

/////////////////////////////
/////////////////////////////
/////////////////////////////
CDUIResource_::CDUIResource_()
:m_pDUIResComPtr(NULL)
{

}
CDUIResource_::~CDUIResource_()
{
	CloseSkin();
}


BOOL CDUIResource_::OpenSkin(LPCSTR strSkinPath)
{
	return TRUE;
}

void CDUIResource_::CloseSkin()
{
	DestroyDirectUIs();
}

void CDUIResource_::DestroyDirectUIs()
{
	for (size_t i=0;i<m_dequeDirectUIs.size();i++)
	{
		CDirectUI_* pDUI = m_dequeDirectUIs[i];
		IDispatch *pCom = pDUI->GetDispatch();
		if (pCom)
		{
			pCom->Release();
			pDUI->SetDispatch(NULL);
		}

		delete pDUI;
	}

	m_dequeDirectUIs.clear();
}

CDirectUI_* CDUIResource_::CreateDirectUI(HWND hWnd)
{
	CDirectUI_ *pNewDUI = new CDirectUI_();

	pNewDUI->Create(hWnd);

	m_dequeDirectUIs.push_back(pNewDUI);

	return pNewDUI;
}

SkinControlBase_* CDUIResource_::CreateControl(CDirectUI_ *pDirectUI,BSTR strClsid)
{
	if (pDirectUI == NULL) return NULL;

	SkinControlBase_ *pNewCtrl = new SkinControlBase_();
	pNewCtrl->SetDUIRes(this);
	pNewCtrl->SetParent((SkinObjResBase_*)pDirectUI);

	pDirectUI->AddObject(pNewCtrl);

	GUID clsid;
	CLSIDFromString(strClsid,&clsid);

	IDispatch *pDispatch = NULL;
	::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void**)&pDispatch);

	pNewCtrl->SetDispatch(pDispatch);

	return pNewCtrl;
}

string CDUIResource_::GetSkinPath()
{
	return strSkinPath;
}

IDispatch* CDUIResource_::GetDUIResComPtr()
{
	return m_pDUIResComPtr;
}
void CDUIResource_::SetDUIResComPtr(IDispatch *pDUIRes)
{
	m_pDUIResComPtr = pDUIRes;
}