// CommandButton.cpp : CCommandButton ��ʵ��

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
	case DUISM_RESKIN://����
		{
			//�¼���Ƥ���ľ���·��
			BSTR bstrNewSkinName = (BSTR)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_REFRESH://ˢ��
		{
			//û��ʹ��DUINotify ��lParam*.

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_LBUTTONDOWN://������������
		{
			//HitTest Code,������Ŀǰʼ��ΪHTCLIENT.
			LONG nHitTest = peVentNotify->lParam1;
			//CPoint��x,yֵ,���DirectUI�ͻ���λ��.
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			//����ֵ
			LONG* pnResult = (LONG*)peVentNotify->lParam4;

			m_eState = DUI_BUTTON_STATUS_PRESS;

			*pbResult = VARIANT_TRUE;

			MessageBox(NULL,_T("DirectUI EventNotify: LButtonDown"), _T("CommandButton"), MB_OK);
		}
		break;

	case DUISM_LBUTTONUP://��������̧��
		{
			//HitTest Code,������Ŀǰʼ��ΪHTCLIENT.
			LONG nHitTest = peVentNotify->lParam1;
			//CPoint��x,yֵ,���DirectUI�ͻ���λ��.
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			//����ֵ
			LONG* pnResult = (LONG*)peVentNotify->lParam4;
			*pbResult = VARIANT_FALSE;

			//MessageBox(NULL,_T("DirectUI EventNotify: LButtonUp"), _T("CommandButton"), MB_OK);
		}
		break;

	case DUISM_COMMAND://����
		{
			WPARAM wParam = (WPARAM)peVentNotify->lParam1;
			WPARAM lParam = (WPARAM)peVentNotify->lParam2;
			LRESULT* plResult = (LRESULT*)peVentNotify->lParam3;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_RBUTTONDOWN://����Ҽ�������
		{
			//HitTest Code,������Ŀǰʼ��ΪHTCLIENT.
			LONG nHitTest = peVentNotify->lParam1;
			//CPoint��x,yֵ,���DirectUI�ͻ���λ��.
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			//����ֵ
			LONG* pnResult = (LONG*)peVentNotify->lParam4;
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_RBUTTONUP://����Ҽ���̧��
		{
			//HitTest Code,������Ŀǰʼ��ΪHTCLIENT.
			LONG nHitTest = peVentNotify->lParam1;
			//CPoint��x,yֵ,���DirectUI�ͻ���λ��.
			LONG x = peVentNotify->lParam2;
			LONG y = peVentNotify->lParam3;
			//����ֵ
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

	case DUISM_ONSETFOCUS://��ǰ���������뽹��
		{
			//CPoint��x,yֵ,���DirectUI�ͻ���λ��.
			LONG x = peVentNotify->lParam1;
			LONG y = peVentNotify->lParam2;
			//����ֵ
			LONG* pnResult = (LONG*)peVentNotify->lParam3;
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_ONKILLFOCUS://��ǰ����ʧȥ���뽹��
		{
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_INITOBJECT://��ǰ�������ڵ�DirectUI���ɹ����ص������Ĵ�����.
		{
//			m_pDirectUI->GetWindowHandle((OLE_HANDLE*)&m_hWndDirectUI);

			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_INITDIALOG://��ǰ�������ڵĴ��ڱ���ʼ��(WM_INITDIALOG)��
		{

		}
		break;

	case DUISM_DESTROYING://��ǰ�������ڱ�����,���ִ�к�ִ��delete����.
		{

		}
		break;

	case DUISM_ENABLEOBJECT://��ǰ����Ŀ���״̬�����ı�.
		{
			//�µĿ���״̬
			VARIANT_BOOL bEnableNew = (VARIANT_BOOL)peVentNotify->lParam1;
			//ԭ���Ŀ���״̬
			VARIANT_BOOL bEnableOld = (VARIANT_BOOL)peVentNotify->lParam2;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_ACCESSCONFIGED://��ȡ�ؼ����Ժ�
		{
			//bRead,���ΪVARIANT_TRUE��ʾ��ȡ������Ϣ,VARIANT_FALSE��ʾΪ����������Ϣ
			VARIANT_BOOL bRead = (VARIANT_BOOL)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_EXPORTCONFIGED://�����ؼ����Ժ�
		{
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_CLONED://���Ƶ�ǰ�����
		{
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_GETDRAGCURSOR://��Builder�л�ö�����½����.
		{
			//IDC_CURSOR_XXXX_NEW Ϊ�½�������ԴID��
//			OLE_HANDLE* phCursor = (OLE_HANDLE*)peVentNotify->lParam1;
//			*phCursor = (OLE_HANDLE)HandleToLong(::LoadCursor(AfxGetResourceHandle(),
//				MAKEINTRESOURCE(IDC_CURSOR_XXXX_NEW)));

			//�����˱���Ϣ,������VARIANT_TRUE,֪ͨƽ̨�Ѿ��Ը���Ϣ�����˴���.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_GETCONTROLICON://��Builder�л�ÿؼ���ͼ��
		{
			//Ҫ��ȡ��ͼ��Ŀ��
			LONG nWidth = peVentNotify->lParam1;
			//Ҫ��ȡ��ͼ��ĸ߶�
			LONG nHeight = peVentNotify->lParam2;
			//IDI_ICON_XXXX Ϊͼ�����ԴID��
			OLE_HANDLE* phIcon = (OLE_HANDLE*)peVentNotify->lParam3;
//			*phIcon = (OLE_HANDLE)HandleToLong(::LoadImage(AfxGetResourceHandle(),
//				MAKEINTRESOURCE(IDI_ICON_XXXX),IMAGE_ICON,
//				nWidth,nHeight,LR_CREATEDIBSECTION|LR_DEFAULTSIZE));

			//�����˱���Ϣ,������VARIANT_TRUE,֪ͨƽ̨�Ѿ��Ը���Ϣ�����˴���.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_GETCATEGORYNAME://��ÿؼ��ķ�������
		{
			BSTR* pbstrResult = (BSTR*)peVentNotify->lParam1;
			*pbstrResult = L"KernelControls";

			//�����˱���Ϣ,������VARIANT_TRUE,֪ͨƽ̨�Ѿ��Ը���Ϣ�����˴���.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_GETCONTROLTYPENAME://��ÿؼ�����������
		{
			BSTR* pbstrResult = (BSTR*)peVentNotify->lParam1;
			*pbstrResult = L"XXXX";

			//�����˱���Ϣ,������VARIANT_TRUE,֪ͨƽ̨�Ѿ��Ը���Ϣ�����˴���.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_GETAUTHORINFO://��ÿؼ�������Ϣ
		{
//			AuthorInfo* pAI = (AuthorInfo*)peVentNotify->lParam1;
//			pAI->bstrAuthorName = L"UIPower";
//			pAI->bstrCompany = L"Yongjin Software Co,Ltd.";
//			pAI->bstrUrl = L"http://www.uipower.com";
//			pAI->bstrVersion = L"1.0.0";

			//�����˱���Ϣ,������VARIANT_TRUE,֪ͨƽ̨�Ѿ��Ը���Ϣ�����˴���.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_DESTROYBMPPERPIXEL:
		{   
			//�����˱���Ϣ,������VARIANT_TRUE,֪ͨƽ̨�Ѿ��Ը���Ϣ�����˴���.
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

			//Ŀ��DC
			//OLE_HANDLE hDC = (OLE_HANDLE)peVentNotify->lParam4;
			//Ŀ���������
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

	case DUISM_SIZE://��ǰ���󱻸ı��С��
		{
			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_TIMER://��Ȼ�������ڴ��ڷ�����WM_TIMER��Ϣ
		{
			//nIDEvent:��ϢID��
			UINT nIDEvent = (UINT)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_CALLPROP://����Զ����������
		{
			//����ID��

			long lPropId = peVentNotify->lParam1;
			//���Է���ֵ
			BSTR* pbstrResult = (BSTR*)peVentNotify->lParam2;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_DESIGNSTATUSCHANGED://���״̬�ı�
		{
			//bDesign,ΪTrue��ʾ���״̬,False��ʾΪ����״̬
			VARIANT_BOOL bDesign = (VARIANT_BOOL)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_DRAWPREVIEW://Ԥ��
		{
			//Ŀ��DC
			OLE_HANDLE hDC = (OLE_HANDLE)peVentNotify->lParam1;
			//Ŀ���������
			SkinRect *sknrc = (SkinRect*)peVentNotify->lParam2;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_FINALCREATE://�ؼ��������,��ִ��AccessConfig��.
		{
//			IMPLEMENT_CONTROLBASE_GETVARS();

			//�����˱���Ϣ,������VARIANT_TRUE,֪ͨƽ̨�Ѿ��Ը���Ϣ�����˴���.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_IMPORTCONFIG:
		{
			IDUIRes* pDUIRes = (IDUIRes*)peVentNotify->lParam1;
			OLE_HANDLE hObjParent = (OLE_HANDLE)peVentNotify->lParam2;
			IDUIControlBase **ppResult = (IDUIControlBase**)peVentNotify->lParam3;

			//IDR_DEFXXXX ΪԤ����Ƥ����ԴID��.
//			pDUIRes->ImportSkin((OLE_HANDLE)HandleToLong(AfxGetInstanceHandle()),
//				IDR_DEFXXXX,L"UIFILES",hObjParent,ppResult);

			//�����˱���Ϣ,������VARIANT_TRUE,֪ͨƽ̨�Ѿ��Ը���Ϣ�����˴���.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_ISDRAGABLED://֪ͨƽ̨��ǰ�����Ƿ����ק,DirectUI��PopupSingleListΪTrue,�������͵Ŀؼ���ΪFALSE.
		{
			VARIANT_BOOL* pbIsDragabled = (VARIANT_BOOL*)peVentNotify->lParam1;

			*pbIsDragabled = VARIANT_FALSE;

			//�����˱���Ϣ,������VARIANT_TRUE,֪ͨƽ̨�Ѿ��Ը���Ϣ�����˴���.
			*pbResult = VARIANT_TRUE;
		}
		break;

	case DUISM_PROPCHANGEDNOTIFY://���Ըı��
		{
			//���Զ���ָ��

			IDUIPropBase *pProp = (IDUIPropBase*)peVentNotify->lParam1;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_GETTABINFO://��ȡTab��Ϣ
		{
			//ָ���ؼ��Ƿ�TabStop
			VARIANT_BOOL *pbTabStop = (VARIANT_BOOL*)peVentNotify->lParam1;

			//ָ���ؼ��Ƿ����Tab���
			VARIANT_BOOL *pbTabDraw = (VARIANT_BOOL*)peVentNotify->lParam2;

			*pbResult = VARIANT_FALSE;
		}
		break;

	case DUISM_SETTOOPTIP://������ʾ��Ϣ
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
