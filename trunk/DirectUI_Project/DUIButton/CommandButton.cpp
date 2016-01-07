// CommandButton.cpp : CCommandButton 的实现

#include "stdafx.h"
#include "CommandButton.h"
#include "stringex.h"


// CCommandButton
CCommandButton::CCommandButton()
:m_eState(DUI_BUTTON_STATUS_NORMAL)
{
	for (int i=DUI_BUTTON_STATUS_NORMAL;i<DUI_BUTTON_STATUS_LAST;i++)
	{
		m_pImageBack[i] = NULL;
	}
}

CCommandButton::~CCommandButton()
{
}

HRESULT CCommandButton::FinalConstruct()
{
	IMPLEMENT_CONTROLBASE();
	return S_OK;
}

void CCommandButton::FinalRelease()
{
	RELEASE_CONTROLBASE();
}

STDMETHODIMP CCommandButton::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICommandButton
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


STDMETHODIMP CCommandButton::FinalCreate(VARIANT_BOOL bNewObject)
{
	IMPLEMENT_CONTROLBASE_GETVARS();

	return S_OK;
}

STDMETHODIMP CCommandButton::CreateProps(VARIANT_BOOL* pbResult)
{	
	ICtrlPluginProp *pGroupProp = NULL;
	CreateGroupProp(NULL,L"Back",VARIANT_TRUE,&pGroupProp);

	tstringex strKeys[] = {_T("Normal"),_T("Hot"),_T("Press"),_T("Disabled"),_T("Focus")};
	
	for (int i=DUI_BUTTON_STATUS_NORMAL;i<DUI_BUTTON_STATUS_LAST;i++)
	{
		BSTR bstrTemp = strKeys[i].AllocSysString();
		CreateProp(pGroupProp,_PROPTYPE_IMAGESECTION,bstrTemp,L"",VARIANT_TRUE, (IDUIPropBase**)&m_pImageBack[i]);
		::SysFreeString(bstrTemp);
	}

	//text
	CreateProp(pGroupProp,_PROPTYPE_TEXTSTYLE,_T("TextStyle"), _T("Text Style"), VARIANT_TRUE, (IDUIPropBase**)&m_pTextStyleProp);
	CreateProp(pGroupProp,_PROPTYPE_STRING,_T("Text"), _T("Control Display Text"), VARIANT_TRUE, (IDUIPropBase**)&m_pTextContent);

	return S_OK;
}

STDMETHODIMP CCommandButton::RenderDrawObject(IDUIRenderDC* pDC, SkinRect* rect, VARIANT_BOOL* pbResult)
{
	///////////	

	switch ( m_eState )
	{
	case DUI_BUTTON_STATUS_NORMAL:
		m_pImageBack[0]->RenderDraw(pDC,rect,pbResult);
		break;
	case DUI_BUTTON_STATUS_HOT:
		m_pImageBack[1]->RenderDraw(pDC,rect,pbResult);
		break;
	case DUI_BUTTON_STATUS_PRESS:
		m_pImageBack[2]->RenderDraw(pDC,rect,pbResult);
		break;
	default:
		m_pImageBack[0]->RenderDraw(pDC,rect,pbResult);
		break;
	}
	
	////////////
	CComBSTR strText;
	m_pTextContent->GetValue(&strText);

	m_pTextStyleProp->RenderDraw(pDC, rect, strText, pbResult);

	return S_OK;
}

STDMETHODIMP CCommandButton::EventNotify(DUINotify *peVentNotify,VARIANT_BOOL *pbResult)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pbResult = VARIANT_FALSE;

	switch(peVentNotify->eDuiMsgId)
	{
	case DUISM_RESKIN://换肤
		{
			//新加载皮肤的绝对路径
			BSTR bstrNewSkinName = (BSTR)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_REFRESH://刷新
		{
			//没有使用DUINotify 的lParam*.

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_LBUTTONDOWN://鼠标左键被按下
		{
			//HitTest Code,保留，目前始终为HTCLIENT.
			LONG nHitTest = peVentNotify->lParam1;
			//CPoint的x,y值,相对DirectUI客户区位置.
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			//返回值
			LONG* pnResult = (LONG*)peVentNotify->lParam4;

			m_eState = DUI_BUTTON_STATUS_PRESS;

			*pbResult = VARIANT_TRUE;

			MessageBox(NULL,_T("DirectUI EventNotify: LButtonDown"), _T("CommandButton"), MB_OK);
		}
		break;

	case DUISM_LBUTTONUP://鼠标左键被抬起
		{
			//HitTest Code,保留，目前始终为HTCLIENT.
			LONG nHitTest = peVentNotify->lParam1;
			//CPoint的x,y值,相对DirectUI客户区位置.
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			//返回值
			LONG* pnResult = (LONG*)peVentNotify->lParam4;
			*pbResult = VARIANT_FALSE;

			//MessageBox(NULL,_T("DirectUI EventNotify: LButtonUp"), _T("CommandButton"), MB_OK);
		}
		break;

	case DUISM_COMMAND://保留
		{
			WPARAM wParam = (WPARAM)peVentNotify->lParam1;
			WPARAM lParam = (WPARAM)peVentNotify->lParam2;
			LRESULT* plResult = (LRESULT*)peVentNotify->lParam3;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_RBUTTONDOWN://鼠标右键被按下
		{
			//HitTest Code,保留，目前始终为HTCLIENT.
			LONG nHitTest = peVentNotify->lParam1;
			//CPoint的x,y值,相对DirectUI客户区位置.
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			//返回值
			LONG* pnResult = (LONG*)peVentNotify->lParam4;
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_RBUTTONUP://鼠标右键被抬起
		{
			//HitTest Code,保留，目前始终为HTCLIENT.
			LONG nHitTest = peVentNotify->lParam1;
			//CPoint的x,y值,相对DirectUI客户区位置.
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			//返回值
			LONG* pnResult = (LONG*)peVentNotify->lParam4;
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_SETFOCUS://Set Current Object Focus
		{                
			VARIANT_BOOL *pbSetFocusResult = (VARIANT_BOOL*)peVentNotify->lParam1;
			*pbSetFocusResult = VARIANT_FALSE;
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_ONSETFOCUS://当前对象获得输入焦点
		{
			//CPoint的x,y值,相对DirectUI客户区位置.
			LONG x = peVentNotify->lParam1;
			LONG y = peVentNotify->lParam2;
			//返回值
			LONG* pnResult = (LONG*)peVentNotify->lParam3;
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_ONKILLFOCUS://当前对象失去输入焦点
		{
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_INITOBJECT://当前对象所在的DirectUI被成功加载到所属的窗口中.
		{
//			m_pDirectUI->GetWindowHandle((OLE_HANDLE*)&m_hWndDirectUI);

			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_INITDIALOG://当前对象所在的窗口被初始化(WM_INITDIALOG)后
		{

		}
		break;

	case DUISM_DESTROYING://当前对象正在被销毁,这句执行后即执行delete操作.
		{

		}
		break;

	case DUISM_ENABLEOBJECT://当前对象的可用状态发生改变.
		{
			//新的可用状态
			VARIANT_BOOL bEnableNew = (VARIANT_BOOL)peVentNotify->lParam1;
			//原来的可用状态
			VARIANT_BOOL bEnableOld = (VARIANT_BOOL)peVentNotify->lParam2;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_ACCESSCONFIGED://读取控件属性后
		{
			//bRead,如果为VARIANT_TRUE表示读取配置信息,VARIANT_FALSE表示为保存配置信息
			VARIANT_BOOL bRead = (VARIANT_BOOL)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_EXPORTCONFIGED://导出控件属性后
		{
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_CLONED://复制当前对象后
		{
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_GETDRAGCURSOR://在Builder中获得对象的新建光标.
		{
			//IDC_CURSOR_XXXX_NEW 为新建光标的资源ID号
//			OLE_HANDLE* phCursor = (OLE_HANDLE*)peVentNotify->lParam1;
//			*phCursor = (OLE_HANDLE)HandleToLong(::LoadCursor(AfxGetResourceHandle(),
//				MAKEINTRESOURCE(IDC_CURSOR_XXXX_NEW)));

			//处理了本消息,即返回VARIANT_TRUE,通知平台已经对该消息进行了处理.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_GETCONTROLICON://在Builder中获得控件的图标
		{
			//要获取的图标的宽度
			LONG nWidth = peVentNotify->lParam1;
			//要获取的图标的高度
			LONG nHeight = peVentNotify->lParam2;
			//IDI_ICON_XXXX 为图标的资源ID号
			OLE_HANDLE* phIcon = (OLE_HANDLE*)peVentNotify->lParam3;
//			*phIcon = (OLE_HANDLE)HandleToLong(::LoadImage(AfxGetResourceHandle(),
//				MAKEINTRESOURCE(IDI_ICON_XXXX),IMAGE_ICON,
//				nWidth,nHeight,LR_CREATEDIBSECTION|LR_DEFAULTSIZE));

			//处理了本消息,即返回VARIANT_TRUE,通知平台已经对该消息进行了处理.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_GETCATEGORYNAME://获得控件的分类名字
		{
			BSTR* pbstrResult = (BSTR*)peVentNotify->lParam1;
			*pbstrResult = L"KernelControls";

			//处理了本消息,即返回VARIANT_TRUE,通知平台已经对该消息进行了处理.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_GETCONTROLTYPENAME://获得控件的类型名字
		{
			BSTR* pbstrResult = (BSTR*)peVentNotify->lParam1;
			*pbstrResult = L"XXXX";

			//处理了本消息,即返回VARIANT_TRUE,通知平台已经对该消息进行了处理.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_GETAUTHORINFO://获得控件作者信息
		{
//			AuthorInfo* pAI = (AuthorInfo*)peVentNotify->lParam1;
//			pAI->bstrAuthorName = L"UIPower";
//			pAI->bstrCompany = L"Yongjin Software Co,Ltd.";
//			pAI->bstrUrl = L"http://www.uipower.com";
//			pAI->bstrVersion = L"1.0.0";

			//处理了本消息,即返回VARIANT_TRUE,通知平台已经对该消息进行了处理.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_DESTROYBMPPERPIXEL:
		{   
			//处理了本消息,即返回VARIANT_TRUE,通知平台已经对该消息进行了处理.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_MOUSEMOVE:
		{
			LONG nHitTest = peVentNotify->lParam1;
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			LONG* pnResult = (LONG*)peVentNotify->lParam4;

			*pbResult = VARIANT_TRUE;

			//////////////
			m_eState = DUI_BUTTON_STATUS_HOT;

			//目标DC
			//OLE_HANDLE hDC = (OLE_HANDLE)peVentNotify->lParam4;
			//目标绘制区域
			//SkinRect *sknrc = (SkinRect*)peVentNotify->lParam5;

			//RenderDrawObject((IDUIRenderDC*)hDC, sknrc, pbResult);
		}
		break;

	case DUISM_LBUTTONDBCLICK:
		{
			LONG nHitTest = peVentNotify->lParam1;
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			LONG* pnResult = (LONG*)peVentNotify->lParam4;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_MOUSELEAVE:
		{
			LONG* pnResult = (LONG*)peVentNotify->lParam1;

			*pbResult = VARIANT_TRUE;

			//////////////
			m_eState = DUI_BUTTON_STATUS_NORMAL;
		}
		break;

	case DUISM_SIZE://当前对象被改变大小后
		{
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_TIMER://当然对象所在窗口发出的WM_TIMER消息
		{
			//nIDEvent:消息ID号
			UINT nIDEvent = (UINT)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_CALLPROP://插件自定义属性入口
		{
			//属性ID号

			long lPropId = peVentNotify->lParam1;
			//属性返回值
			BSTR* pbstrResult = (BSTR*)peVentNotify->lParam2;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_DESIGNSTATUSCHANGED://设计状态改变
		{
			//bDesign,为True表示设计状态,False表示为运行状态
			VARIANT_BOOL bDesign = (VARIANT_BOOL)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_DRAWPREVIEW://预览
		{
			//目标DC
			OLE_HANDLE hDC = (OLE_HANDLE)peVentNotify->lParam1;
			//目标绘制区域
			SkinRect *sknrc = (SkinRect*)peVentNotify->lParam2;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_FINALCREATE://控件创建完成,并执行AccessConfig后.
		{
//			IMPLEMENT_CONTROLBASE_GETVARS();

			//处理了本消息,即返回VARIANT_TRUE,通知平台已经对该消息进行了处理.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_IMPORTCONFIG:
		{
			IDUIRes* pDUIRes = (IDUIRes*)peVentNotify->lParam1;
			OLE_HANDLE hObjParent = (OLE_HANDLE)peVentNotify->lParam2;
			IDUIControlBase **ppResult = (IDUIControlBase**)peVentNotify->lParam3;

			//IDR_DEFXXXX 为预定义皮肤资源ID号.
//			pDUIRes->ImportSkin((OLE_HANDLE)HandleToLong(AfxGetInstanceHandle()),
//				IDR_DEFXXXX,L"UIFILES",hObjParent,ppResult);

			//处理了本消息,即返回VARIANT_TRUE,通知平台已经对该消息进行了处理.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_ISDRAGABLED://通知平台当前对象是否可拖拽,DirectUI与PopupSingleList为True,其他类型的控件均为FALSE.
		{
			VARIANT_BOOL* pbIsDragabled = (VARIANT_BOOL*)peVentNotify->lParam1;

			*pbIsDragabled = VARIANT_FALSE;

			//处理了本消息,即返回VARIANT_TRUE,通知平台已经对该消息进行了处理.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_PROPCHANGEDNOTIFY://属性改变后
		{
			//属性对象指针

			IDUIPropBase *pProp = (IDUIPropBase*)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_GETTABINFO://获取Tab信息
		{
			//指定控件是否TabStop
			VARIANT_BOOL *pbTabStop = (VARIANT_BOOL*)peVentNotify->lParam1;

			//指定控件是否绘制Tab虚框
			VARIANT_BOOL *pbTabDraw = (VARIANT_BOOL*)peVentNotify->lParam2;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_SETTOOPTIP://设置提示信息
		{
			BSTR *pStrTooltip = (BSTR*)peVentNotify->lParam1;
			*pStrTooltip = L"";

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_KBHOOKPROC://Keyboard Hook
		{
			int code = (int)peVentNotify->lParam1;
			WPARAM wParam = (WPARAM)peVentNotify->lParam2;
			LPARAM lParam = (LPARAM)peVentNotify->lParam3;
			LRESULT* pLR = (LRESULT*)peVentNotify->lParam4;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_HOOKPROC://Hook Proc
		{
			DWORD dwHook = (DWORD)peVentNotify->lParam1;
			int code = (int)peVentNotify->lParam2;
			WPARAM wParam = (WPARAM)peVentNotify->lParam3;
			LPARAM lParam = (LPARAM)peVentNotify->lParam4;
			LRESULT* pLR = (LRESULT*)peVentNotify->lParam5;

			*pbResult = VARIANT_FALSE;
		}
		break;
	}

	return S_OK;
}


STDMETHODIMP CCommandButton::SetText(BSTR strText,VARIANT_BOOL* pbResult)
{
	*pbResult = VARIANT_TRUE;

	m_pTextContent->SetValue(strText);

	return S_OK;
}

STDMETHODIMP CCommandButton::SetBackImage(DUIImageBase *pImageBase,eDUI_BUTTON_STATUS eStatus,VARIANT_BOOL* pbResult)
{
	*pbResult = VARIANT_FALSE;

	if (eStatus < DUI_BUTTON_STATUS_NORMAL || eStatus >= DUI_BUTTON_STATUS_LAST) return S_FALSE;
	
	IImageSecProp * pImageProp = m_pImageBack[eStatus];
	if (pImageProp == NULL) return S_FALSE;

	pImageProp->SetImageBase(pImageBase,pbResult);

	*pbResult = VARIANT_TRUE;

	return S_OK;
}
