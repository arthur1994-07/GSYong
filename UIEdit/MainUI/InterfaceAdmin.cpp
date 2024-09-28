#include "StdAfx.h"

#include "../../EngineLib/Common/DXInputString.h"
#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/GUInterface/BasicButton.h"
#include "../../EngineLib/GUInterface/InterfaceCfg.h"
#include "../../EngineLib/GUInterface/interfacecfgxml.h"
#include "../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../EngineLib/DxTools/d3dfont.h"
#include "../../EngineLib/DxTools/DxFontMan.h"

#include "../../RanUI/OldUI/Util/UIWindowEx.h"

#include "../DlgUIEdit.h"
#include "../ExtendUI/UIEditButton.h"
#include "../ExtendUI/UIEditControl.h"
#include "../ExtendUI/UIEditGroup.h"
#include "../ExtendUI/UIEditModal.h"
#include "InterfaceAdmin.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CInterfaceAdmin* CInterfaceAdmin::SelfInstance = NULL;
// 
// CInterfaceAdmin* CInterfaceAdmin::GetInstance()
// {
// 	if (SelfInstance == NULL) {
// 		SelfInstance = new CInterfaceAdmin;
// 	}
// 	return SelfInstance;
// }
// 
// void CInterfaceAdmin::ReleaseInstance()
// {
// 	SAFE_DELETE( SelfInstance );
// }

CInterfaceAdmin::CInterfaceAdmin(EngineDeviceMan* pEngineDevice) 
    : m_pEngineDevice(pEngineDevice)
    , m_LineColor( 255, 255, 255, 255 )
    , m_pSelectedControl( NULL )
    , m_nTotalWindowNum( WINDOW_NUM )
    , m_pMainDlg( NULL )
    , m_bListSelect( FALSE )
    , m_nAnimaitionCtrlNum( 0 )
    , m_nStaticCtrlNum( 0 )
    , m_nDialogCtrlNum( 0 )
    , m_nPicCtrlNum( 0 )
    , m_nProgressCtrlNum( 0 )
    , m_nButtonCtrlNum( 0 )
    , m_nEditCtrlNum( 0 )
    , m_nScrollFormNum( 0 )	
    , m_nLineInterval( 100 )
    , m_bGraphPaper(FALSE)
    , m_nViewWidth( 1024 )
    , m_nViewHeight( 768 )
    , m_nLastViewWidth( 1024 )
    , m_nLastViewHeight( 768 )
    , m_nCountryCode( KOREA )
    , m_fInputElapsedTime( 0.0f )
{
	m_pUIContainer.clear();
	memset( &m_sTempContainer, NULL, sizeof( UIContainer ) );
	memset( m_bKeyInput, FALSE, sizeof( int ) * KEY_NUM );	
}

CInterfaceAdmin::~CInterfaceAdmin(void)
{
//	SAFE_DELETE( SelfInstance );
}

void CInterfaceAdmin::ResetControl()
{
	m_pSelectedControl = NULL;

	m_nTotalWindowNum = WINDOW_NUM;

	// 각각 콘트롤의 생성숫자
	m_nAnimaitionCtrlNum = 0;
	m_nStaticCtrlNum = 0;															
	m_nDialogCtrlNum = 0;
	m_nPicCtrlNum = 0;
	m_nProgressCtrlNum = 0;
	m_nButtonCtrlNum = 0;
	m_nEditCtrlNum = 0;
	m_nScrollFormNum = 0;
}

void CInterfaceAdmin::Init(CDlgUIEdit* dialog)
{
	m_pMainDlg = dialog;
}

HRESULT CInterfaceAdmin::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	CloseAllWindow();
	return CUIMan::InitDeviceObjects ( pd3dDevice );
}

HRESULT CInterfaceAdmin::DeleteDeviceObjects ()
{
	ClearUIList();
	ResetControl();

	return CUIMan::DeleteDeviceObjects ();
}

HRESULT CInterfaceAdmin::FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
	//MIN_ASSERT( pd3dDevice );

	const long lResolution = GetResolution();
	WORD X_RES = HIWORD ( lResolution );
	WORD Y_RES = LOWORD ( lResolution );

	// 에디트 창에 입력되는 것이 없을때에만 UI위치 및 크기 수정을 할 수 있다
	if( !m_pMainDlg->isEdit() )
		KeyInputCheck( fElapsedTime );

	if( GetExactFocusControl() != NULL )
	{
		CUIEditGroup* tempUI = (CUIEditGroup*)( GetExactFocusControl() );
		if(m_pSelectedControl != tempUI)
		{
			if ( tempUI )
				tempUI->ResetFocusControl();
			m_pSelectedControl = tempUI;

			m_pMainDlg->SetSelectedItem(FindUIListIndex( m_pSelectedControl) );
		}

		CUIFocusContainer::UICONTROL_FOCUSLIST focus_list = GetFocusList()->GetFocusList();
		CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter = focus_list.rbegin ();
		CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter_end = focus_list.rend ();

		bool bBasicChatBoxShow = false;
		for ( ; riter != riter_end; ++riter )
		{
			if( !riter->pUIControl )
				continue;

			CUIControl* pParentControl = riter->pUIControl->GetParent();

			if ( !pParentControl )
				continue;

			if( pParentControl == m_pSelectedControl )
			{
				GetFocusList()->InsertAfter ( pParentControl->GetWndID(), riter->cID, riter->pUIControl );
			}
		}
	}

	if(GetSelectedControl())
	{

		CPoint CurPos = CPoint(int( m_pSelectedControl->GetLocalPos().left ), int( m_pSelectedControl->GetLocalPos().top) );
		CPoint CurSize = CPoint(int( m_pSelectedControl->GetLocalPos().sizeX ),int( m_pSelectedControl->GetLocalPos().sizeY) );

		if( m_PrevPos != CurPos || m_PrevSize != CurSize )
		{
			CString strTemp;
			if( m_pSelectedControl->GetParent() )
			{
				strTemp = m_pSelectedControl->GetParent()->GetControlNameEx().c_str();
			}
			else
			{
				strTemp.Format(_T("NONE"));
			}

			m_pMainDlg->SetSelectedUIInfoMain( m_pSelectedControl );
		}

		m_PrevPos = CurPos;
		m_PrevSize = CurSize;
	}
	else
	{
		m_pMainDlg->ClearUIInfo();
	}

	//return CUIMan::FrameMove ( pd3dDevice, fElapsedTime );
    return S_OK;
}

HRESULT CInterfaceAdmin::FinalCleanup ()
{
	ClearUIList();
	ResetControl();

	return CUIMan::FinalCleanup ();
}

void CInterfaceAdmin::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{	
	//CUIMan::TranslateUIMessage (ControlID, dwMsg);
}

void CInterfaceAdmin::CloseAllWindow ()
{	
	//HideGroup (MAIN_WINDOW_UI);
}

void CInterfaceAdmin::ReSizeWindow( int cx ,int cy )
{
	CUIMan::SetResolution (MAKELONG(cy,cx));

	if(DXUTGetD3DDevice())
	{
		InvalidateDeviceObjects();
		RestoreDeviceObjects(DXUTGetD3DDevice());
	}
}

void CInterfaceAdmin::SetViewSize( int nWidth, int nHeight )
{
	BOOL bChangeSize = FALSE;

	if( m_nViewWidth != nWidth || m_nViewHeight != nHeight )
	{
		m_nLastViewWidth = m_nViewWidth;
		m_nLastViewHeight = m_nViewHeight;

		m_nViewWidth = nWidth;
		m_nViewHeight = nHeight;

		AlignControlAll();
	}
}

void CInterfaceAdmin::AlignControlAll()
{
	for(int i = 0; i < signed( m_pUIContainer.size() );i++ )
	{
		CUIEditGroup* tempUI = m_pUIContainer[ i ].pControl;

		if( !tempUI->GetParent() ) 
		{
			UIRECT rcNewWindow( 0, 0, m_nViewWidth, m_nViewHeight );
			UIRECT rcLastWindow( 0, 0, m_nLastViewWidth, m_nLastViewHeight );

			tempUI->AlignMainControlEX( rcLastWindow, rcNewWindow );
			tempUI->SetGlobalPos( tempUI->GetGlobalPos() );
		}
	}

	RestoreDeviceObjects( DXUTGetD3DDevice() );
}

void CInterfaceAdmin::ResetMainUI()
{
}

void CInterfaceAdmin::MakeDynamicDialog()
{
	CString strName;
	strName.Format( _T( "DIALOG_%d" ), m_nDialogCtrlNum + 1 );

	CUIEditModal* pMainUI = new CUIEditModal(m_pEngineDevice, this);
	pMainUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T( "BASIC_EDIT_CONTROL" ), UI_FLAG_DEFAULT );
	pMainUI->CreateWindowControl( _T( "BASIC_EDIT_CONTROL" ));
	pMainUI->SetControlNameEx( strName.GetString() );
	pMainUI->InitDeviceObjects ( DXUTGetD3DDevice() );

	RegisterControl ( pMainUI );
	//ShowGroupFocus ( MAIN_WINDOW_UI + m_nTotalWindowNum );

	AddUIList( pMainUI,DIALOG_CON );
	m_nDialogCtrlNum++;
}

void CInterfaceAdmin::MakeDynamicControl()
{
	CString strName;
	strName.Format( _T("CONTROL_%d" ),m_nPicCtrlNum+1 );

	CUIEditControl* pMainUI = new CUIEditControl(m_pEngineDevice, this);
	pMainUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T( "BASIC_EDIT_CONTROL" ), UI_FLAG_DEFAULT );
	pMainUI->CreateWindowControl( _T( "BASIC_EDIT_CONTROL" ) );
	pMainUI->SetControlNameEx( strName.GetString() );
	pMainUI->InitDeviceObjects ( DXUTGetD3DDevice() );
	pMainUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
	RegisterControl ( pMainUI );
	//ShowGroupFocus ( MAIN_WINDOW_UI + m_nTotalWindowNum );

	AddUIList(pMainUI,PICURE_CON);
	m_nPicCtrlNum++;
}

void CInterfaceAdmin::MakeDynamicButton()
{
	CString strName;
	strName.Format( _T("BUTTON_%d"),m_nButtonCtrlNum + 1 );

	CUIEditButton* pMainUI = new CUIEditButton(m_pEngineDevice, this);
	pMainUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T( "BASIC_EDIT_CONTROL" ), UI_FLAG_DEFAULT );
	pMainUI->SetAllMessage( TRUE );
	pMainUI->CreateWindowControl( _T("BASIC_EDIT_CONTROL" ) );
	pMainUI->SetControlNameEx( strName.GetString() );
	pMainUI->InitDeviceObjects ( DXUTGetD3DDevice() );
	pMainUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
	RegisterControl ( pMainUI );
	//ShowGroupFocus ( MAIN_WINDOW_UI + m_nTotalWindowNum );

	AddUIList( pMainUI,BUTTON_CON );
	m_nButtonCtrlNum++;
}

void CInterfaceAdmin::MakeDynamicStatic()
{
//	CString strName;
//	strName.Format( _T( "STATIC_%d" ), m_nStaticCtrlNum + 1 );
//
//	CUIEditStatic* pMainUI = new CUIEditStatic;
//	pMainUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T( "BASIC_EDIT_CONTROL" ), UI_FLAG_DEFAULT );
//	pMainUI->CreateWindowControl( _T( "BASIC_EDIT_CONTROL" ) );
//	pMainUI->SetControlNameEx( strName.GetString() );
//	pMainUI->InitDeviceObjects ( DXUTGetD3DDevice() );
//	pMainUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
//	RegisterControl ( pMainUI );
//	//ShowGroupFocus ( MAIN_WINDOW_UI + m_nTotalWindowNum );
//
//	AddUIList(pMainUI,STATIC_CON);
//	m_nStaticCtrlNum++;
}

void CInterfaceAdmin::MakeDynamicAnimation()
{
//	CString strName;
//	strName.Format( _T( "ANI_%d" ), m_nAnimaitionCtrlNum + 1 );
//
//	CUIEditAnimation* pMainUI = new CUIEditAnimation;
//	pMainUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T( "BASIC_EDIT_CONTROL" ), UI_FLAG_DEFAULT );
//	pMainUI->CreateWindowControl( _T( "BASIC_EDIT_CONTROL" ) );
//	pMainUI->SetControlNameEx( strName.GetString() );
//	pMainUI->SetAniTime( 0.3f );
//	pMainUI->InitDeviceObjects ( DXUTGetD3DDevice() );
//	pMainUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
//	RegisterControl ( pMainUI );
//	//ShowGroupFocus ( MAIN_WINDOW_UI + m_nTotalWindowNum );
//
//	AddUIList( pMainUI,ANIMATION_CON );
//	m_nAnimaitionCtrlNum++;
}

void CInterfaceAdmin::MakeDynamicScroll()
{
	UIRECT rcUIControl;
	CString strName;

	CUIEditControl* pScrollbarMainUI;
	CUIEditButton*	pUpButtonUI;
	CUIEditButton*	pDnButtonUI;
	CUIEditControl*	pThumbUI;
	CUIEditControl* pScrollFrameUI;
	CUIEditControl*	pScrollBackUI;

	//스크롤바 부모
	strName.Format(_T("SCROLLBAR_%d"),m_nScrollFormNum+1);

	pScrollbarMainUI = new CUIEditControl(m_pEngineDevice, this);
	pScrollbarMainUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T( "BASIC_EDIT_CONTROL" ), UI_FLAG_DEFAULT );
	pScrollbarMainUI->CreateWindowControl( _T( "BASIC_EDIT_CONTROL" ) );
	pScrollbarMainUI->SetControlNameEx( strName.GetString() );
	pScrollbarMainUI->InitDeviceObjects ( DXUTGetD3DDevice() );
	pScrollbarMainUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
	RegisterControl ( pScrollbarMainUI );

	AddUIList(pScrollbarMainUI,PICURE_CON);
	//
	rcUIControl.sizeX  = 18;
	rcUIControl.sizeY  = 270;
	rcUIControl.left   = 0;
	rcUIControl.top    = 0;
	rcUIControl.right  = rcUIControl.left + rcUIControl.sizeX;
	rcUIControl.bottom = rcUIControl.top + rcUIControl.sizeY;

	pScrollbarMainUI->SetLocalPos( rcUIControl );
	pScrollbarMainUI->InitDeviceObjects( DXUTGetD3DDevice() );

	//위쪽버튼
	strName.Format( _T( "SCROLLBAR_UP_BUTTON_%d "), m_nScrollFormNum + 1 );

	pUpButtonUI = new CUIEditButton(m_pEngineDevice, this);
	pUpButtonUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T("BASIC_EDIT_CONTROL"), UI_FLAG_DEFAULT );
	pUpButtonUI->SetAllMessage(TRUE);
	pUpButtonUI->CreateWindowControl(_T("BASIC_EDIT_CONTROL"));
	pUpButtonUI->SetControlNameEx(strName.GetString());
	pUpButtonUI->InitDeviceObjects ( DXUTGetD3DDevice() );
	pUpButtonUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
	RegisterControl ( pUpButtonUI );

	AddUIList(pUpButtonUI,BUTTON_CON);
	//
	rcUIControl.sizeX  = 14;
	rcUIControl.sizeY  = 16;
	rcUIControl.left   = 2;
	rcUIControl.top    = 0;
	rcUIControl.right  = rcUIControl.left + rcUIControl.sizeX;
	rcUIControl.bottom = rcUIControl.top + rcUIControl.sizeY;

	pUpButtonUI->SetLocalPos(rcUIControl);
	pUpButtonUI->InitDeviceObjects(DXUTGetD3DDevice());

	SetParentControl(pUpButtonUI, pScrollbarMainUI);

	//아래쪽 버튼
	strName.Format(_T("SCROLLBAR_DOWN_BUTTON_%d"),m_nScrollFormNum+1);

	pDnButtonUI = new CUIEditButton(m_pEngineDevice, this);
	pDnButtonUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T("BASIC_EDIT_CONTROL"), UI_FLAG_DEFAULT );
	pDnButtonUI->SetAllMessage(TRUE);
	pDnButtonUI->CreateWindowControl(_T("BASIC_EDIT_CONTROL"));
	pDnButtonUI->SetControlNameEx(strName.GetString());
	pDnButtonUI->InitDeviceObjects ( DXUTGetD3DDevice() );
	pDnButtonUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
	RegisterControl ( pDnButtonUI );

	AddUIList(pDnButtonUI,BUTTON_CON);
	//
	rcUIControl.sizeX  = 14;
	rcUIControl.sizeY  = 16;
	rcUIControl.left   = 2;
	rcUIControl.top    = 254;
	rcUIControl.right  = rcUIControl.left + rcUIControl.sizeX;
	rcUIControl.bottom = rcUIControl.top + rcUIControl.sizeY;

	pDnButtonUI->SetLocalPos(rcUIControl);
	pDnButtonUI->InitDeviceObjects(DXUTGetD3DDevice());

	SetParentControl(pDnButtonUI, pScrollbarMainUI);

	//스크롤바 이동막대
	strName.Format(_T("SCROLLBAR_THUMB_%d"),m_nScrollFormNum+1);

	pThumbUI = new CUIEditControl(m_pEngineDevice, this);
	pThumbUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T("BASIC_EDIT_CONTROL"), UI_FLAG_DEFAULT );
	pThumbUI->CreateWindowControl(_T("BASIC_EDIT_CONTROL"));
	pThumbUI->SetControlNameEx(strName.GetString());
	pThumbUI->InitDeviceObjects ( DXUTGetD3DDevice() );
	pThumbUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
	RegisterControl ( pThumbUI );

	AddUIList(pThumbUI,PICURE_CON);
	//
	rcUIControl.sizeX  = 14;
	rcUIControl.sizeY  = 16;
	rcUIControl.left   = 2;
	rcUIControl.top    = 19;
	rcUIControl.right  = rcUIControl.left + rcUIControl.sizeX;
	rcUIControl.bottom = rcUIControl.top + rcUIControl.sizeY;

	pThumbUI->SetLocalPos(rcUIControl);
	pThumbUI->InitDeviceObjects(DXUTGetD3DDevice());

	//배경 프레임
	strName.Format(_T("SCROLLBAR_FRAME_%d"),m_nScrollFormNum+1);

	pScrollFrameUI = new CUIEditControl(m_pEngineDevice, this);
	pScrollFrameUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T("BASIC_EDIT_CONTROL"), UI_FLAG_DEFAULT );
	pScrollFrameUI->CreateWindowControl(_T("BASIC_EDIT_CONTROL"));
	pScrollFrameUI->SetControlNameEx(strName.GetString());
	pScrollFrameUI->InitDeviceObjects ( DXUTGetD3DDevice() );
	pScrollFrameUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
	RegisterControl ( pScrollFrameUI );

	AddUIList(pScrollFrameUI,PICURE_CON);
	//
	rcUIControl.sizeX  = 18;
	rcUIControl.sizeY  = 239;
	rcUIControl.left   = 0;
	rcUIControl.top    = 16;
	rcUIControl.right  = rcUIControl.left + rcUIControl.sizeX;
	rcUIControl.bottom = rcUIControl.top + rcUIControl.sizeY;

	pScrollFrameUI->SetLocalPos(rcUIControl);
	pScrollFrameUI->InitDeviceObjects(DXUTGetD3DDevice());


	//배경
	strName.Format(_T("SCROLLBAR_BACK_%d"),m_nScrollFormNum+1);

	pScrollBackUI= new CUIEditControl(m_pEngineDevice, this);
	pScrollBackUI->Create ( MAIN_WINDOW_UI + m_nTotalWindowNum ,_T("BASIC_EDIT_CONTROL"), UI_FLAG_DEFAULT );
	pScrollBackUI->CreateWindowControl(_T("BASIC_EDIT_CONTROL"));
	pScrollBackUI->SetControlNameEx(strName.GetString());
	pScrollBackUI->InitDeviceObjects ( DXUTGetD3DDevice() );
	pScrollBackUI->RestoreDeviceObjects( DXUTGetD3DDevice() );
	RegisterControl ( pScrollBackUI );

	AddUIList(pScrollBackUI,PICURE_CON);

	rcUIControl.sizeX  = 14;
	rcUIControl.sizeY  = 238;
	rcUIControl.left   = 2;
	rcUIControl.top    = 1;
	rcUIControl.right  = rcUIControl.left + rcUIControl.sizeX;
	rcUIControl.bottom = rcUIControl.top + rcUIControl.sizeY;

	pScrollBackUI->SetLocalPos(rcUIControl);
	pScrollBackUI->InitDeviceObjects(DXUTGetD3DDevice());

	////
	SetParentControl(pUpButtonUI, pScrollbarMainUI);
	SetParentControl(pDnButtonUI, pScrollbarMainUI);
	SetParentControl(pThumbUI, pScrollbarMainUI);
	SetParentControl(pScrollFrameUI, pScrollbarMainUI);
	SetParentControl(pScrollBackUI, pScrollFrameUI);

	m_nScrollFormNum++;
	m_pMainDlg->ResetList();
}

void CInterfaceAdmin::AddUIList(CUIEditGroup* pCreateUI, int nControlType)	
{
	UIContainer TempContainer;

	TempContainer.pControl       = pCreateUI;
	TempContainer.nTextureWidth  = 0;
	TempContainer.nTextureHeight = 0;
	TempContainer.nControlType   = nControlType;

	m_nTotalWindowNum++;
	m_pUIContainer.push_back(TempContainer);

	m_pMainDlg->SetSelectedItem(m_pUIContainer.size() -1);
	SetSelectedControl(FindUIListIndex(pCreateUI));
}

void CInterfaceAdmin::CopyUIList(int nUINum)
{
	m_sTempContainer = m_pUIContainer[nUINum];
	m_pMainDlg->ResetList();
}

void CInterfaceAdmin::PasteUIList()
{
	if(!m_sTempContainer.pControl)
		return;

	switch(m_sTempContainer.nControlType)
	{
	case DIALOG_CON:	
		{
			MakeDynamicDialog();
		}
		break;
	case PICURE_CON:
		{
			MakeDynamicControl();
		}
		break;
	case BUTTON_CON:
		{
			MakeDynamicButton();
		}
		break;
	case STATIC_CON:
		{
			MakeDynamicStatic();
		}
		break;
	case ANIMATION_CON:
		{
			MakeDynamicAnimation();
		}
		break;
	case BUTTON_TEXT_CON:
		{
			MakeDynamicAnimation();
		}
		break;
	default:
		return;
	}

	m_pUIContainer[m_pUIContainer.size() - 1].nTextureWidth  = m_sTempContainer.nTextureWidth;
	m_pUIContainer[m_pUIContainer.size() - 1].nTextureHeight = m_sTempContainer.nTextureHeight;
	m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetTextureName(m_sTempContainer.pControl->GetTextureName());
	m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetTexturePos(m_sTempContainer.pControl->GetTexturePos());
	m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetTextureSize(m_sTempContainer.pControl->GetTextureSizeWidth(),m_sTempContainer.pControl->GetTextureSizeHeight());
	m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetUseRender ( m_sTempContainer.pControl->GetTextureName().GetLength () );
	m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetLocalPos(m_sTempContainer.pControl->GetLocalPos());
	m_pUIContainer[m_pUIContainer.size() - 1].pControl->ResizeWindow(m_sTempContainer.pControl->GetLocalPos());
	m_pUIContainer[m_pUIContainer.size() - 1].pControl->InitDeviceObjects(DXUTGetD3DDevice());	
	m_pUIContainer[m_pUIContainer.size() - 1].pControl->ResizeWindow(m_sTempContainer.pControl->GetLocalPos());
	m_pUIContainer[m_pUIContainer.size() - 1].pControl->RestoreDeviceObjects(DXUTGetD3DDevice());

	if(m_sTempContainer.nControlType == BUTTON_CON)
	{
		CUIEditButton* pWinCon = ( CUIEditButton* ) (m_pUIContainer[m_pUIContainer.size() - 1].pControl);
		CUIEditButton* pWinTemp = ( CUIEditButton* ) (m_sTempContainer.pControl);

		pWinCon->m_pMouseInFlip->SetTextureName(pWinTemp->m_pMouseInFlip->GetTextureName());
		pWinCon->m_pMouseInFlip->SetTexturePos(pWinTemp->m_pMouseInFlip->GetTexturePos());
		pWinCon->m_pMouseInFlip->SetTextureSize(pWinTemp->m_pMouseInFlip->GetTextureSizeWidth(),pWinTemp->m_pMouseInFlip->GetTextureSizeHeight());
		pWinCon->m_pMouseInFlip->SetUseRender( pWinTemp->m_pMouseInFlip->GetTextureName().GetLength() );
		pWinCon->m_pMouseInFlip->InitDeviceObjects( DXUTGetD3DDevice() );
		pWinCon->m_pMouseClickFlip->SetTextureName(pWinTemp->m_pMouseClickFlip->GetTextureName());
		pWinCon->m_pMouseClickFlip->SetTexturePos(pWinTemp->m_pMouseClickFlip->GetTexturePos());
		pWinCon->m_pMouseClickFlip->SetTextureSize(pWinTemp->m_pMouseClickFlip->GetTextureSizeWidth(),pWinTemp->m_pMouseClickFlip->GetTextureSizeHeight());
		pWinCon->m_pMouseClickFlip->SetUseRender( pWinTemp->m_pMouseClickFlip->GetTextureName().GetLength() );
		pWinCon->m_pMouseClickFlip->InitDeviceObjects( DXUTGetD3DDevice() );
	}

	m_pMainDlg->SetSelectedUIInfo(FindUIListIndex(m_pSelectedControl));
	m_pMainDlg->ResetList();
}

void CInterfaceAdmin::DelUIList(CUIEditGroup* pDeleteUI)
{
	std::vector<UIContainer>::iterator iter;
	int nDeleteIdx;

	//현재 UI를 부모로 하고있는 컨트롤을 찾아 부모삭제
	for(int i=0;i<signed(m_pUIContainer.size());i++)
	{
		CUIEditGroup* pWin = ( CUIEditGroup* )( m_pUIContainer[i].pControl->GetParent() );

		if(pDeleteUI == pWin)
		{
			m_pUIContainer[i].pControl->SetParent(NULL);
			m_pUIContainer[i].pControl->RestoreDeviceObjects( DXUTGetD3DDevice() );
		}
	}



	nDeleteIdx = FindUIListIndex(pDeleteUI);

	if(nDeleteIdx != -1)
	{
		CUIEditGroup* pDelControl=NULL;

		iter = m_pUIContainer.begin() + nDeleteIdx;
		m_pUIContainer.erase(iter);
		HideGroup(pDeleteUI->GetWndID());

		//DeleteControl(pDeleteUI->GetWndID());
		//SAFE_DELETE(pDeleteUI);

		for(int i=0;i<signed(m_pUIContainer.size());i++)
		{
			CUIEditGroup* pWin = ( CUIEditGroup* )( m_pUIContainer[i].pControl );
			pWin->SetVisibleSingle( true );
		}

		ResetExclusiveControl();
		ResetExactFocusControl();

		if(m_pUIContainer.size() <= 0)
		{
			m_pSelectedControl = NULL;
			m_pMainDlg->SetSelectedItem(- 1);
			m_pMainDlg->ResetList();
		}
		else
		{
			SetSelectedControl(nDeleteIdx - 1);
			m_pMainDlg->SetSelectedItem(nDeleteIdx - 1);
			m_pMainDlg->ResetList();
		}
	}


}

void CInterfaceAdmin::ClearUIList()
{
	for(int i=0;i<signed(m_pUIContainer.size());i++)
	{
		DeleteControl(m_pUIContainer[i].pControl->GetWndID());
		//SAFE_DELETE(m_pUIContainer[i].pControl);	
	}	
	m_pUIContainer.clear();
	m_pMainDlg->SetSelectedItem(-1);
	m_pMainDlg->ResetList();


	if( m_pSelectedControl ) m_pSelectedControl = NULL;	
	//ResetExactFocusControl();
	DeleteAllControl();
	ResetControl();

	ResetExclusiveControl();
}

int CInterfaceAdmin::FindUIListIndex(CUIEditGroup* pFindUI)
{

	for(int i=0;i<signed(m_pUIContainer.size());i++)
	{
		if(m_pUIContainer[i].pControl == pFindUI)
			return i;
	}

	return -1;
}

int CInterfaceAdmin::FindUIListIndex(CString strFindUI)
{

	for(int i=0;i<signed(m_pUIContainer.size());i++)
	{
		if(m_pUIContainer[i].pControl->GetControlNameEx().c_str() == strFindUI)
			return i;
	}

	return -1;
}

int CInterfaceAdmin::FindUIListIndex(UIGUID FindUI)
{

	for(int i=0;i<signed(m_pUIContainer.size());i++)
	{
		if(m_pUIContainer[i].pControl->GetWndID() == FindUI)
			return i;
	}

	return -1;
}


CUIEditGroup* CInterfaceAdmin::FindUIListCtrl(CString strFindUI)
{
	for(int i=0;i<signed(m_pUIContainer.size());i++)
	{
		if(m_pUIContainer[i].pControl->GetControlNameEx().c_str() == strFindUI)
			return m_pUIContainer[i].pControl;
	}

	return NULL;
}

CUIEditGroup* CInterfaceAdmin::FindUIListCtrl(UIGUID FindUI)
{
	for(int i=0;i<signed(m_pUIContainer.size());i++)
	{
		if(m_pUIContainer[i].pControl->GetWndID() == FindUI)
			return m_pUIContainer[i].pControl;
	}

	return NULL;
}

int	CInterfaceAdmin::GetSelectedControlType()
{
	if(GetSelectedControl())
	{
		return m_pUIContainer[FindUIListIndex(GetSelectedControl())].nControlType;
	}

	return -1;
}

void CInterfaceAdmin::SetSelectedControl(int nSelNum, bool dlgUpdate)
{
	if(nSelNum < 0 || signed(m_pUIContainer.size()) <= nSelNum)
	{
		if(dlgUpdate)
			m_pMainDlg->SetSelectedItem(-1);

		m_pSelectedControl = NULL;	
		ResetExclusiveControl();
		ResetExactFocusControl();
		return;
	}

	ShowGroupFocus( m_pUIContainer[nSelNum].pControl->GetWndID());

	m_pSelectedControl = m_pUIContainer[nSelNum].pControl;

	if(dlgUpdate)
		m_pMainDlg->SetSelectedItem(FindUIListIndex( m_pSelectedControl));

	if(m_pSelectedControl)
		SetExactFocusControl(m_pSelectedControl);
}

void CInterfaceAdmin::SetParentControl(CUIEditGroup* pSelpUI, CUIEditGroup* pParentUI)
{
	if( pSelpUI->GetParent() )
	{
		CUIEditGroup* pWin = ( CUIEditGroup* )( pSelpUI->GetParent() );
		pWin->DelChild(pSelpUI->GetWndID()); //이전 부모의 자식(해당UI) 삭제
	}
	pSelpUI->SetParent(pParentUI);

	CUIEditGroup* pWin = ( CUIEditGroup* )( pParentUI );
	pWin->AddChild(pSelpUI->GetWndID());

	UIRECT rcOld, rcParent;

	rcParent = pSelpUI->GetParent()->GetLocalPos();
	rcOld    = pSelpUI->GetLocalPos();

	float fX,fY;

	if(rcOld.left < rcParent.left)
		fX = 0;
	else
		fX = rcOld.left - rcParent.left;

	if(rcOld.top < rcParent.top)
		fY = 0;
	else
		fY = rcOld.top - rcParent.top;

	pSelpUI->SetLocalPos(D3DXVECTOR2(rcOld.left - rcParent.left, rcOld.top - rcParent.top));
	pSelpUI->AlignSubControlEX(rcParent,rcParent);
	pSelpUI->RestoreDeviceObjects(DXUTGetD3DDevice());
}

void CInterfaceAdmin::DeleteParentControl()
{
	CUIEditGroup* pWin = ( CUIEditGroup* )( m_pSelectedControl->GetParent() );
	pWin->DelChild(m_pSelectedControl->GetWndID()); //이전 부모 삭제
	m_pSelectedControl->SetParent(NULL);
	m_pSelectedControl->RestoreDeviceObjects( DXUTGetD3DDevice() );
}

void CInterfaceAdmin::KeyInputCheck(float fElapsedTime )
{
	int nX = 0;
	int nY = 0;

	BOOL bMove = FALSE;
	BOOL bChange = FALSE;

	if(DxInputDevice::GetInstance().GetKeyState( DIK_LEFT ) & DXKEY_PRESSED)
	{
		m_bKeyInput[KEY_LEFT] = TRUE;
		bMove = TRUE;
	}
	else
	{
		m_bKeyInput[KEY_LEFT] = FALSE;
	}

	if(DxInputDevice::GetInstance().GetKeyState( DIK_RIGHT ) & DXKEY_PRESSED)
	{
		m_bKeyInput[KEY_RIGHT] = TRUE;
		bMove = TRUE;
	}
	else
	{
		m_bKeyInput[KEY_RIGHT] = FALSE;
	}

	if(DxInputDevice::GetInstance().GetKeyState( DIK_UP ) & DXKEY_PRESSED)
	{
		m_bKeyInput[KEY_UP] = TRUE;
		bMove = TRUE;
	}
	else
	{
		m_bKeyInput[KEY_UP] = FALSE;
	}


	if(DxInputDevice::GetInstance().GetKeyState( DIK_DOWN ) & DXKEY_PRESSED)
	{
		m_bKeyInput[KEY_DOWN] = TRUE;
		bMove = TRUE;
	}
	else
	{
		m_bKeyInput[KEY_DOWN] = FALSE;
	}

	if( m_bKeyInput[KEY_LEFT] )
	{
		nX = -1;
	}

	if( m_bKeyInput[KEY_RIGHT] )
	{
		nX = 1;
	}

	if( m_bKeyInput[KEY_UP] )
	{
		nY = -1;
	}

	if( m_bKeyInput[KEY_DOWN] )
	{
		nY = 1;
	}


	if(DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED)
	{
		if(bMove)
		{
			nX *= 10;
			nY *= 10;
		}

		if(DxInputDevice::GetInstance().GetKeyState( DIK_C ) & DXKEY_DOWN)
		{
			if(GetSelectedControl()) CopyUIList(FindUIListIndex(GetSelectedControl()));	
			else
			{
				MessageBox(NULL,_T("You have to Select UIControl.."),_T("Error"),MB_OK);
			}
		}

		if(DxInputDevice::GetInstance().GetKeyState( DIK_V ) & DXKEY_DOWN)
		{
			PasteUIList();
		}
	}

	/*	if(DxInputMapper::GetInputState( DIK_T ) & DXKEY_DOWN)
	{

	CUIControl* tempUI = CInterfaceAdmin::GetInstance()->GetSelectedControl();
	if(tempUI != NULL)
	{
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;

	if(pFrame->m_cDlgEditText.IsWindowVisible())
	{
	pFrame->m_cDlgEditText.ShowWindow( SW_HIDE );
	pFrame->m_cDlgMainView.ShowWindow( SW_SHOWNOACTIVATE );
	}
	else
	{
	pFrame->m_cDlgMainView.ShowWindow( SW_HIDE );
	pFrame->m_cDlgEditText.ShowWindow( SW_SHOW );
	pFrame->m_cDlgEditText.SetTextureInfo(tempUI->GetTextureName(),tempUI->GetTexturePos().left,
	tempUI->GetTexturePos().top,tempUI->GetTexturePos().right,tempUI->GetTexturePos().bottom);
	pFrame->m_cDlgEditText.Invalidate();
	}

	}
	else
	{
	MessageBox(NULL,_T("You have to Select UIControl.."),_T("Error"),MB_OK);
	}

	}*/

	if(DxInputDevice::GetInstance().GetKeyState( DIK_DELETE ) & DXKEY_DOWN)
	{
		//콘트롤 삭제 단축키
		if(GetSelectedControl()) DelUIList(GetSelectedControl());	
		else
		{
			MessageBox(NULL,_T("You have to Select UIControl.."),_T("Error"),MB_OK);
		}

	}

	if(DxInputDevice::GetInstance().GetKeyState( DIK_LMENU ) & DXKEY_PRESSED)
	{
		if(bMove)
		{
			if( IsGraphVisible() )
			{
				CUIEditGroup* tempUI = GetSelectedControl();
				INT nInterval = GetLineInterval();

				UIRECT rcUIControl;
				rcUIControl.sizeX  =  tempUI->GetLocalPos().sizeX;
				rcUIControl.sizeY  =  tempUI->GetLocalPos().sizeY;
				rcUIControl.left   = ( ( ( INT ) tempUI->GetLocalPos().left / GetLineInterval() ) ) * GetLineInterval();
				rcUIControl.top    = ( ( INT ) tempUI->GetLocalPos().top / GetLineInterval() ) * GetLineInterval();
				
				tempUI->SetLocalPos(rcUIControl);
				tempUI->InitDeviceObjects(DXUTGetD3DDevice());

				nX *= GetLineInterval();
				nY *= GetLineInterval();

			}
;
		}

	}

	//콘트롤 크기변경 단축키
	if(DxInputDevice::GetInstance().GetKeyState( DIK_LSHIFT ) & DXKEY_PRESSED)
	{
		bChange = TRUE;
	}

	if( m_fInputElapsedTime == 0.0f ||  m_fInputElapsedTime >= 0.5f )
	{
		if( bMove && GetSelectedControl() )
		{
			CUIEditGroup* tempUI = GetSelectedControl();

			UIRECT rcUIControl;

			if(bChange)
			{
				rcUIControl.sizeX  =  max(tempUI->GetLocalPos().sizeX + nX,0);
				rcUIControl.sizeY  =  max(tempUI->GetLocalPos().sizeY + nY,0 );
				rcUIControl.left   = tempUI->GetLocalPos().left;
				rcUIControl.top    = tempUI->GetLocalPos().top;
			}
			else
			{
				rcUIControl.sizeX  = tempUI->GetLocalPos().sizeX;
				rcUIControl.sizeY  = tempUI->GetLocalPos().sizeY;
				rcUIControl.left   = max(tempUI->GetLocalPos().left + nX,0);
				rcUIControl.top    = max(tempUI->GetLocalPos().top + nY,0 );
			}

			rcUIControl.right  = rcUIControl.left + rcUIControl.sizeX;
			rcUIControl.bottom = rcUIControl.top + rcUIControl.sizeY;

			tempUI->SetLocalPos(rcUIControl);
			tempUI->InitDeviceObjects(DXUTGetD3DDevice());

			if(bChange)
			{
				if(tempUI->GetParent())
				{
					UIRECT rcParent = tempUI->GetParent()->GetGlobalPos();
					tempUI->AlignSubControlEX( rcParent, rcParent );
				}
				else 
				{
					UIRECT rcNewWindow( 0, 0, m_nViewWidth, m_nViewHeight );
					tempUI->AlignMainControlEX( rcNewWindow, rcNewWindow );
				}
			}

			tempUI->RestoreDeviceObjects(DXUTGetD3DDevice());
		}
	}

	if ( bMove )
	{
		m_fInputElapsedTime += fElapsedTime;
	}
	else
	{
		m_fInputElapsedTime = 0.0f;
	}

}

//void CInterfaceAdmin::SetCountry(int nCode)
//{
//	m_nCountryCode = nCode;
//
//	for(int i=0;i<signed(m_pUIContainer.size());i++)
//	{
//		if(m_pUIContainer[i].nControlType == STATIC_CON)
//		{
//			CUIEditStatic* pWinCon = dynamic_cast<CUIEditStatic*> (m_pUIContainer[i].pControl);
//			pWinCon->ResetText();
//		}
//	}	
//}
//
BOOL CInterfaceAdmin::SaveXML(CString p_filename)
{
	FILE *lp;
	TCHAR szBuffer[1024] = {0};
	CString strTemp;

	if(signed(m_pUIContainer.size()) < 0)
		return FALSE;

	lp = _fsopen( p_filename.GetString(), "w+", _SH_DENYNO );

	_ftprintf(lp,_T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"),szBuffer);
	_ftprintf(lp,_T("<INTERFACE>\n"),szBuffer);

	for(int i=0;i<signed(m_pUIContainer.size());i++)
	{
		// 콘트롤의 이름을 입력한다.
		strTemp.Format(_T("<CONTROL Local=\"Common\" Id=\"%s\">\n"),m_pUIContainer[i].pControl->GetControlNameEx().c_str());
		_ftprintf(lp,strTemp,szBuffer);

		// 콘트롤 타입을 입력한다.
		strTemp.Format(_T("<TYPE Type=\"%d\"/>\n"), m_pUIContainer[i].nControlType);
		_ftprintf(lp,strTemp,szBuffer);

		//if(m_pUIContainer[i].nControlType == ANIMATION_CON)
		//{
		//	CUIEditAnimation* pWinCon = dynamic_cast<CUIEditAnimation*> (m_pUIContainer[i].pControl);

		//	strTemp.Format(_T("<ANIMATION Number=\"%d\" Delay=\"%f\"/>\n"), pWinCon->m_pAni.size(), pWinCon->m_fAniTime );
		//	_ftprintf(lp,strTemp,szBuffer);
		//}

		//// 스태틱콘트롤의 텍스트나 팁의 내용을 입력한다.
		//if(m_pUIContainer[i].nControlType == STATIC_CON)
		//{
		//	CUIEditStatic* pWinCon = dynamic_cast<CUIEditStatic*> (m_pUIContainer[i].pControl);

		//	strTemp.Format(_T("<TIP File=\"%s\" Id=\"%s\" Index=\"%d\" Use=\"%d\"/>\n"),pWinCon->m_strFile,pWinCon->m_strID,pWinCon->m_nIndex, 0);
		//	_ftprintf(lp,strTemp,szBuffer);
		//}
		//else
		//{
		//	if(m_pUIContainer[i].pControl->GetTipFile().GetLength() > 0 && m_pUIContainer[i].pControl->GetTipId().GetLength() > 0 )
		//	{
		//		strTemp.Format(_T("<TIP File=\"%s\" Id=\"%s\" Index=\"%d\" Use=\"%d\"/>\n"),m_pUIContainer[i].pControl->GetTipFile(),m_pUIContainer[i].pControl->GetTipId()
		//			,m_pUIContainer[i].pControl->GetTipIndex(), int(m_pUIContainer[i].pControl->GetUseTip()));
		//		_ftprintf(lp,strTemp,szBuffer);
		//	}
		//}

		// 부모가 있으면 부모정보의 콘트롤 이름을 입력
		if(m_pUIContainer[i].pControl->GetParent())
		{
			strTemp.Format(_T("<PARENT_CONTROL Id=\"%s\"/>\n"), m_pUIContainer[i].pControl->GetParent()->GetControlNameEx().c_str());
			_ftprintf(lp,strTemp,szBuffer);
		}

		strTemp.Format(_T("<WINDOW_POS X=\"%d\" Y=\"%d\" W=\"%d\" H=\"%d\" />\n")
			, int(m_pUIContainer[i].pControl->GetLocalPos().left)  , int(m_pUIContainer[i].pControl->GetLocalPos().top) 
			, int(m_pUIContainer[i].pControl->GetLocalPos().sizeX) , int(m_pUIContainer[i].pControl->GetLocalPos().sizeY));
		_ftprintf(lp,strTemp,szBuffer);

		// 정렬 정보가 있으면 입력
		/*{
			WORD wAlign = m_pUIContainer[i].pControl->GetAlignFlag();

			if( wAlign & UI_FLAG_LEFT
				|| wAlign & UI_FLAG_RIGHT
				|| wAlign & UI_FLAG_CENTER_X
				|| wAlign & UI_FLAG_X_PROPORTION
				|| wAlign & UI_FLAG_TOP
				|| wAlign & UI_FLAG_BOTTOM
				|| wAlign & UI_FLAG_CENTER_Y
				|| wAlign & UI_FLAG_Y_PROPORTION	)
			{
				CString strAlignX;
				CString strAlignY;

				if( wAlign & UI_FLAG_LEFT )
					strAlignX = _T("LEFT");
				else if( wAlign & UI_FLAG_RIGHT ) 
					strAlignX = _T("RIGHT");
				else if( wAlign & UI_FLAG_CENTER_X )
					strAlignX = _T("CENTER");
				else if( wAlign & UI_FLAG_X_PROPORTION )
					strAlignX = _T("PROPORTION");

				if( wAlign & UI_FLAG_TOP )
					strAlignY = _T("TOP");
				else if( wAlign & UI_FLAG_BOTTOM ) 
					strAlignY = _T("BOTTOM");
				else if( wAlign & UI_FLAG_CENTER_Y )
					strAlignY = _T("CENTER");
				else if( wAlign & UI_FLAG_Y_PROPORTION )
					strAlignY = _T("PROPORTION");

				strTemp.Format(_T("<WINDOW_ALIGN X=\"%s\" Y=\"%s\" />\n") , strAlignX, strAlignY);
				_ftprintf(lp,strTemp,szBuffer);
			}

			if( wAlign & UI_FLAG_XSIZE
				|| wAlign & UI_FLAG_XSIZE_PROPORTION
				|| wAlign & UI_FLAG_YSIZE
				|| wAlign & UI_FLAG_YSIZE_PROPORTION)
			{
				CString strAlignX;
				CString strAlignY;

				if( wAlign & UI_FLAG_XSIZE )
					strAlignX = _T("SIZE");
				else if( wAlign & UI_FLAG_XSIZE_PROPORTION ) 
					strAlignX = _T("PROPORTION");

				if( wAlign & UI_FLAG_YSIZE )
					strAlignY = _T("SIZE");
				else if( wAlign & UI_FLAG_YSIZE_PROPORTION ) 
					strAlignY = _T("PROPORTION");

				strTemp.Format(_T("<WINDOW_CHANGE X=\"%s\" Y=\"%s\" />\n"), strAlignX, strAlignY);
				_ftprintf(lp,strTemp,szBuffer);
			}

			if(wAlign & UI_FLAG_BACKGROUND
				|| wAlign & UI_FLAG_WIDE
				|| wAlign & UI_FLAG_BASIC_PROPORTION)
			{
				BOOL bBackGround = FALSE;
				BOOL bWide = FALSE;
				BOOL bBasicProp = FALSE;

				if( wAlign & UI_FLAG_BACKGROUND )
					bBackGround = TRUE;
				if( wAlign & UI_FLAG_WIDE )
					bWide = TRUE;
				if( wAlign & UI_FLAG_BASIC_PROPORTION )
					bBasicProp = TRUE;

				strTemp.Format(_T("<WINDOW_ETC BackGround=\"%d\" Wide=\"%d\" BasicProp=\"%d\" />\n"), bBackGround, bWide, bBasicProp);
				_ftprintf(lp,strTemp,szBuffer);
			}
		}*/
		// 텍스쳐 정보가 있으면 입력해준다.
		if(m_pUIContainer[i].pControl->GetTextureName().GetLength() > 0)
		{
			int nTextSizeWidth,nTextSizeHeight;

			nTextSizeWidth = m_pUIContainer[i].nTextureWidth;
			nTextSizeHeight = m_pUIContainer[i].nTextureHeight;

			strTemp.Format(_T("<TEXTURE SizeX=\"%d\" SizeY=\"%d\">%s</TEXTURE>\n"),nTextSizeWidth,nTextSizeHeight,m_pUIContainer[i].pControl->GetTextureName());
			_ftprintf(lp,strTemp,szBuffer);

			int nPos_x,nPos_y,nWidth,nHeight;

			nPos_x  = int(m_pUIContainer[i].pControl->GetTexturePos().left * nTextSizeWidth);
			nPos_y  = int(m_pUIContainer[i].pControl->GetTexturePos().top * nTextSizeHeight);
			nWidth  = int((m_pUIContainer[i].pControl->GetTexturePos().right - m_pUIContainer[i].pControl->GetTexturePos().left) * nTextSizeWidth);
			nHeight = int((m_pUIContainer[i].pControl->GetTexturePos().bottom - m_pUIContainer[i].pControl->GetTexturePos().top) * nTextSizeHeight);

			strTemp.Format(_T("<TEXTURE_POS X=\"%d\" Y=\"%d\" W=\"%d\" H=\"%d\" />\n"),nPos_x, nPos_y, nWidth, nHeight);
			_ftprintf(lp,strTemp,szBuffer);
		}		
		_ftprintf(lp,_T("</CONTROL>\n\n"),szBuffer);

		// 버튼 콘트롤일 경우 In과 Flip 콘트롤에 대한 정보도 등록 해 준다.
		if(m_pUIContainer[i].nControlType == BUTTON_CON)
		{
			{
				CUIEditButton* pWinCon = ( CUIEditButton* ) (m_pUIContainer[i].pControl);
				strTemp.Format(_T("<CONTROL Local=\"Common\" Id=\"%s_IN_FLIP\">\n"),m_pUIContainer[i].pControl->GetControlNameEx().c_str());
				_ftprintf(lp,strTemp,szBuffer);


				strTemp.Format(_T("<TYPE Type=\"%d\"/>\n"), BUTTON_IN_CON);
				_ftprintf(lp,strTemp,szBuffer);

				{
					strTemp.Format(_T("<PARENT_CONTROL Id=\"%s\"/>\n"), m_pUIContainer[i].pControl->GetControlNameEx().c_str());
					_ftprintf(lp,strTemp,szBuffer);
				}

				strTemp.Format(_T("<WINDOW_POS X=\"%d\" Y=\"%d\" W=\"%d\" H=\"%d\" />\n")
					, 0 ,0
					, int(m_pUIContainer[i].pControl->GetLocalPos().sizeX) , int(m_pUIContainer[i].pControl->GetLocalPos().sizeY));
				_ftprintf(lp,strTemp,szBuffer);

				if(pWinCon->m_pMouseInFlip->GetTextureName().GetLength() > 0)
				{
					int nTextSizeWidth,nTextSizeHeight;

					nTextSizeWidth = pWinCon->m_pMouseInFlip->GetTextureSizeWidth();
					nTextSizeHeight = pWinCon->m_pMouseInFlip->GetTextureSizeHeight();

					strTemp.Format(_T("<TEXTURE SizeX=\"%d\" SizeY=\"%d\">%s</TEXTURE>\n"),nTextSizeWidth,nTextSizeHeight,pWinCon->m_pMouseInFlip->GetTextureName());
					_ftprintf(lp,strTemp,szBuffer);

					int nPos_x,nPos_y,nWidth,nHeight;

					nPos_x  = int(pWinCon->m_pMouseInFlip->GetTexturePos().left * nTextSizeWidth);
					nPos_y  = int(pWinCon->m_pMouseInFlip->GetTexturePos().top * nTextSizeHeight);
					nWidth  = int((pWinCon->m_pMouseInFlip->GetTexturePos().right - pWinCon->m_pMouseInFlip->GetTexturePos().left) * nTextSizeWidth);
					nHeight = int((pWinCon->m_pMouseInFlip->GetTexturePos().bottom - pWinCon->m_pMouseInFlip->GetTexturePos().top) * nTextSizeHeight);

					strTemp.Format(_T("<TEXTURE_POS X=\"%d\" Y=\"%d\" W=\"%d\" H=\"%d\" />\n"),nPos_x, nPos_y, nWidth, nHeight);
					_ftprintf(lp,strTemp,szBuffer);
				}		
				_ftprintf(lp,_T("</CONTROL>\n\n"),szBuffer);				
			}

			{
				CUIEditButton* pWinCon = ( CUIEditButton* ) (m_pUIContainer[i].pControl);
				strTemp.Format(_T("<CONTROL Local=\"Common\" Id=\"%s_CLICK_FLIP\">\n"),m_pUIContainer[i].pControl->GetControlNameEx().c_str());
				_ftprintf(lp,strTemp,szBuffer);


				strTemp.Format(_T("<TYPE Type=\"%d\"/>\n"), BUTTON_CLICK_CON);
				_ftprintf(lp,strTemp,szBuffer);

				{
					strTemp.Format(_T("<PARENT_CONTROL Id=\"%s\"/>\n"), m_pUIContainer[i].pControl->GetControlNameEx().c_str());
					_ftprintf(lp,strTemp,szBuffer);
				}

				strTemp.Format(_T("<WINDOW_POS X=\"%d\" Y=\"%d\" W=\"%d\" H=\"%d\" />\n")
					, 0 ,0
					, int(m_pUIContainer[i].pControl->GetLocalPos().sizeX) , int(m_pUIContainer[i].pControl->GetLocalPos().sizeY));
				_ftprintf(lp,strTemp,szBuffer);

				if(pWinCon->m_pMouseClickFlip->GetTextureName().GetLength() > 0)
				{
					int nTextSizeWidth,nTextSizeHeight;

					nTextSizeWidth = pWinCon->m_pMouseClickFlip->GetTextureSizeWidth();
					nTextSizeHeight = pWinCon->m_pMouseClickFlip->GetTextureSizeHeight();

					strTemp.Format(_T("<TEXTURE SizeX=\"%d\" SizeY=\"%d\">%s</TEXTURE>\n"),nTextSizeWidth,nTextSizeHeight,pWinCon->m_pMouseClickFlip->GetTextureName());
					_ftprintf(lp,strTemp,szBuffer);

					int nPos_x,nPos_y,nWidth,nHeight;

					nPos_x  = int(pWinCon->m_pMouseClickFlip->GetTexturePos().left * nTextSizeWidth);
					nPos_y  = int(pWinCon->m_pMouseClickFlip->GetTexturePos().top * nTextSizeHeight);
					nWidth  = int((pWinCon->m_pMouseClickFlip->GetTexturePos().right - pWinCon->m_pMouseClickFlip->GetTexturePos().left) * nTextSizeWidth);
					nHeight = int((pWinCon->m_pMouseClickFlip->GetTexturePos().bottom - pWinCon->m_pMouseClickFlip->GetTexturePos().top) * nTextSizeHeight);

					strTemp.Format(_T("<TEXTURE_POS X=\"%d\" Y=\"%d\" W=\"%d\" H=\"%d\" />\n"),nPos_x, nPos_y, nWidth, nHeight);
					_ftprintf(lp,strTemp,szBuffer);
				}		
				_ftprintf(lp,_T("</CONTROL>\n\n"),szBuffer);
			}

			{
				CUIEditButton* pWinCon = ( CUIEditButton* ) (m_pUIContainer[i].pControl);
				strTemp.Format(_T("<CONTROL Local=\"Common\" Id=\"%s_TEXT\">\n"), m_pUIContainer[i].pControl->GetControlNameEx().c_str());
				_ftprintf(lp,strTemp,szBuffer);

				strTemp.Format(_T("<TYPE Type=\"%d\"/>\n"), BUTTON_TEXT_CON);
				_ftprintf(lp,strTemp,szBuffer);


				{
					strTemp.Format(_T("<PARENT_CONTROL Id=\"%s\"/>\n"), m_pUIContainer[i].pControl->GetControlNameEx().c_str());
					_ftprintf(lp,strTemp,szBuffer);
				}

				strTemp.Format(_T("<WINDOW_POS X=\"%d\" Y=\"%d\" W=\"%d\" H=\"%d\" />\n")
					, 4 ,0
					, int(m_pUIContainer[i].pControl->GetLocalPos().sizeX - 8) , int(m_pUIContainer[i].pControl->GetLocalPos().sizeY));
				_ftprintf(lp,strTemp,szBuffer);

				_ftprintf(lp,_T("</CONTROL>\n\n"),szBuffer);
			}
		}
		//else if(m_pUIContainer[i].nControlType == ANIMATION_CON)
		//{

		//	CUIEditAnimation* pWinCon = dynamic_cast<CUIEditAnimation*> (m_pUIContainer[i].pControl);

		//	for(int nIndex=0;nIndex<pWinCon->m_pAni.size();nIndex++)
		//	{
		//		strTemp.Format(_T("<CONTROL Local=\"Common\" Id=\"%s_%d\">\n"),m_pUIContainer[i].pControl->GetControlNameEx().c_str(),nIndex);
		//		_ftprintf(lp,strTemp,szBuffer);


		//		strTemp.Format(_T("<TYPE Type=\"%d\"/>\n"), ANIMATION_SUB_CON);
		//		_ftprintf(lp,strTemp,szBuffer);

		//		{
		//			strTemp.Format(_T("<PARENT_CONTROL Id=\"%s\"/>\n"), m_pUIContainer[i].pControl->GetControlNameEx().c_str());
		//			_ftprintf(lp,strTemp,szBuffer);
		//		}

		//		strTemp.Format(_T("<WINDOW_POS X=\"%d\" Y=\"%d\" W=\"%d\" H=\"%d\" />\n")
		//			, int(pWinCon->m_pAni[nIndex]->GetLocalPos().left)  , int(pWinCon->m_pAni[nIndex]->GetLocalPos().top) 
		//			, int(pWinCon->m_pAni[nIndex]->GetLocalPos().sizeX) , int(pWinCon->m_pAni[nIndex]->GetLocalPos().sizeY));
		//		_ftprintf(lp,strTemp,szBuffer);

		//		if(pWinCon->m_pAni[nIndex]->GetTextureName().GetLength() > 0)
		//		{
		//			int nTextSizeWidth,nTextSizeHeight;

		//			nTextSizeWidth = pWinCon->m_pAni[nIndex]->GetTextureSizeWidth();
		//			nTextSizeHeight = pWinCon->m_pAni[nIndex]->GetTextureSizeHeight();

		//			strTemp.Format(_T("<TEXTURE SizeX=\"%d\" SizeY=\"%d\">%s</TEXTURE>\n"),nTextSizeWidth,nTextSizeHeight,pWinCon->m_pAni[nIndex]->GetTextureName());
		//			_ftprintf(lp,strTemp,szBuffer);

		//			int nPos_x,nPos_y,nWidth,nHeight;

		//			nPos_x  = int(pWinCon->m_pAni[nIndex]->GetTexturePos().left * nTextSizeWidth);
		//			nPos_y  = int(pWinCon->m_pAni[nIndex]->GetTexturePos().top * nTextSizeHeight);
		//			nWidth  = int((pWinCon->m_pAni[nIndex]->GetTexturePos().right - pWinCon->m_pAni[nIndex]->GetTexturePos().left) * nTextSizeWidth);
		//			nHeight = int((pWinCon->m_pAni[nIndex]->GetTexturePos().bottom - pWinCon->m_pAni[nIndex]->GetTexturePos().top) * nTextSizeHeight);

		//			strTemp.Format(_T("<TEXTURE_POS X=\"%d\" Y=\"%d\" W=\"%d\" H=\"%d\" />\n"),nPos_x, nPos_y, nWidth, nHeight);
		//			_ftprintf(lp,strTemp,szBuffer);
		//		}		
		//		_ftprintf(lp,_T("</CONTROL>\n\n"),szBuffer);
		//	}
		//}
	}

	_ftprintf(lp,_T("</INTERFACE>\n"),szBuffer);
	fclose(lp);

	return TRUE;
}

// XML 파일 로드
BOOL CInterfaceAdmin::LoadXML(CString p_filename)
{
	BOOL bLoad = FALSE;
	CString temp = _T("kr");

	MAP_UI_CFG m_mapUICfg;
	ITER_MAP_UI_CFG iter;

	CInterfaceCfgXml interfaceCfgXml(temp);
	bLoad = interfaceCfgXml.Parse( m_mapUICfg, p_filename.GetString() );

	if(bLoad)
	{
		// 동적생성
		for(iter = m_mapUICfg.begin(); iter != m_mapUICfg.end(); iter++)
		{
			TSTRING pCfgName = (*iter).first;
			INTERFACE_CFG* pCfg = (*iter).second;

			if(FindUIListCtrl(pCfgName.c_str()))
				continue;

			switch(pCfg->nControlType)
			{
			case DIALOG_CON:	
				{
					MakeDynamicDialog();
				}
				break;
			case PICURE_CON:
				{
					MakeDynamicControl();
				}
				break;
			case BUTTON_CON:
				{
					MakeDynamicButton();
				}
				break;
			case BUTTON_IN_CON:
				{
					continue;
				}
				break;
			case BUTTON_CLICK_CON:
				{
					continue;
				}
				break;
			case STATIC_CON:
				{
					MakeDynamicStatic();
				}
				break;
			case EDIT_CON:
				{

				}
				break;
			case PROGRESS_CON:
				{

				}
				break;
			case ANIMATION_CON:
				{
					MakeDynamicAnimation();
				}
				break;
			case ANIMATION_SUB_CON:
				{
					continue;
				}
				break;
			case BUTTON_TEXT_CON:
				{
					continue;
				}
				break;
			case -1:
				{
					MakeDynamicControl();
					//continue;
				}
				break;

			}

			// 기본적인 정보 설정
			m_pUIContainer[m_pUIContainer.size() - 1].nTextureWidth  = int(pCfg->X_Size);
			m_pUIContainer[m_pUIContainer.size() - 1].nTextureHeight = int(pCfg->Y_Size);
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetControlNameEx(pCfgName.c_str());
			//if(pCfg->strTipID.GetLength() > 0 && pCfg->strTipFile.GetLength() > 0)
			//{
			//	m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetTipInfo(pCfg->strTipFile,pCfg->strTipID,pCfg->nTipIndex);
			//	m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetUseTip(pCfg->bUseTip);
			//}
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetTextureName(pCfg->strTextureName);
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetTexturePos(pCfg->rcTexturePos);
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetTextureSize(int(pCfg->X_Size),int(pCfg->Y_Size));
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetUseRender( pCfg->strTextureName.GetLength () );
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->ResetAlignFlag();
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetAlignFlag(pCfg->wAlignFlag);
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->SetLocalPos(pCfg->rcControlPos);
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->ResizeWindow(pCfg->rcControlPos);
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->InitDeviceObjects(DXUTGetD3DDevice());
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->ResizeWindow(pCfg->rcControlPos);
			m_pUIContainer[m_pUIContainer.size() - 1].pControl->RestoreDeviceObjects(DXUTGetD3DDevice());

			// 버튼 콘트롤의 Mouse In Flip 과 Mouse Click Flip 설정
			if(pCfg->nControlType == BUTTON_CON)
			{
				CString temp;

				ITER_MAP_UI_CFG tempiter;
				INTERFACE_CFG* pTempCfg;

				CUIEditButton* pWinCon = ( CUIEditButton* ) (m_pUIContainer[m_pUIContainer.size() - 1].pControl);

				temp.Format(_T("%s_IN_FLIP"),pCfgName.c_str());
				tempiter = m_mapUICfg.find((TCHAR*)(LPCTSTR)temp );

				if( tempiter != m_mapUICfg.end() )
				{
					pTempCfg = (*tempiter).second;
					pWinCon->m_pMouseInFlip->SetTextureName(pTempCfg->strTextureName );
					pWinCon->m_pMouseInFlip->SetTexturePos(pTempCfg->rcTexturePos);
					pWinCon->m_pMouseInFlip->SetTextureSize(int(pTempCfg->X_Size),int(pTempCfg->Y_Size));
					pWinCon->m_pMouseInFlip->SetUseRender( pTempCfg->strTextureName.GetLength() );
					pWinCon->m_pMouseInFlip->InitDeviceObjects( DXUTGetD3DDevice() );
				}

				temp.Format(_T("%s_CLICK_FLIP"),pCfgName.c_str());
				tempiter = m_mapUICfg.find((TCHAR*)(LPCTSTR)temp);
				if( tempiter != m_mapUICfg.end() )
				{
					pTempCfg = (*tempiter).second;
					pWinCon->m_pMouseClickFlip->SetTextureName(pTempCfg->strTextureName );
					pWinCon->m_pMouseClickFlip->SetTexturePos(pTempCfg->rcTexturePos);
					pWinCon->m_pMouseClickFlip->SetTextureSize(int(pTempCfg->X_Size),int(pTempCfg->Y_Size));
					pWinCon->m_pMouseClickFlip->SetUseRender( pTempCfg->strTextureName.GetLength() );
					pWinCon->m_pMouseClickFlip->InitDeviceObjects( DXUTGetD3DDevice() );
				}
			}
			//else if(pCfg->nControlType == STATIC_CON)
			//{
			//	CUIEditStatic* pWinCon = dynamic_cast<CUIEditStatic*> (m_pUIContainer[m_pUIContainer.size() - 1].pControl);

			//	pWinCon->SetXmlInfo(pCfg->strTipFile,pCfg->strTipID,pCfg->nTipIndex);
			//	pWinCon->ResetText();
			//}
			//else if(pCfg->nControlType == ANIMATION_CON)
			//{
			//	CString temp;

			//	MAP_UI_CFG_ITER tempiter;
			//	INTERFACE_CFG* pTempCfg;

			//	CUIEditAnimation* pWinCon = dynamic_cast<CUIEditAnimation*> (m_pUIContainer[m_pUIContainer.size() - 1].pControl);

			//	pWinCon->SetAniTime(pCfg->fAniTime);

			//	for(int nIndex=0;nIndex<pCfg->nAniNum;nIndex++)
			//	{
			//		temp.Format(_T("%s_%d"),pCfgName.c_str(),nIndex);
			//		tempiter = m_mapUICfg.find((TCHAR*)(LPCTSTR)temp );
			//		if( tempiter != m_mapUICfg.end() )
			//		{
			//			pTempCfg = (*tempiter).second;
			//			pWinCon->AddAni(pTempCfg->strTextureName,pTempCfg->rcTexturePos,int(pTempCfg->X_Size),int(pTempCfg->Y_Size)); 					
			//		}
			//	}
			//}
		}

		// 부모&자식 관계 설정
		for(iter = m_mapUICfg.begin();iter != m_mapUICfg.end();iter++)
		{
			TSTRING pCfgName = (*iter).first;
			INTERFACE_CFG* pCfg = (*iter).second;

			if(FindUIListCtrl(pCfgName.c_str()) && FindUIListCtrl(pCfg->strParentName) 
				&& pCfg->nControlType != BUTTON_CLICK_CON 
				&& pCfg->nControlType != BUTTON_IN_CON 
				&& pCfg->nControlType != ANIMATION_SUB_CON)
			{
				FindUIListCtrl(pCfgName.c_str())->SetParent(FindUIListCtrl(pCfg->strParentName));
				CUIEditGroup* pWin = dynamic_cast<CUIEditGroup*> (FindUIListCtrl(pCfg->strParentName));
				pWin->AddChild(FindUIListCtrl(pCfgName.c_str())->GetWndID());
				//pWin->RegisterControl(FindUIList(pCfgName.c_str()));

				FindUIListCtrl(pCfgName.c_str())->SetLocalPos(pCfg->rcControlPos);
				FindUIListCtrl(pCfgName.c_str())->ResizeWindow(pCfg->rcControlPos);
				FindUIListCtrl(pCfgName.c_str())->InitDeviceObjects(DXUTGetD3DDevice());
				FindUIListCtrl(pCfgName.c_str())->ResizeWindow(pCfg->rcControlPos);
				FindUIListCtrl(pCfgName.c_str())->RestoreDeviceObjects(DXUTGetD3DDevice());
			}
		}
	}
	else
	{
		MessageBox(NULL,_T("Failed Load XML file"),_T("Error"),MB_OK);
	}

	m_pMainDlg->ResetList();

	return bLoad;
}

//BOOL CInterfaceAdmin::SaveRUI(CString p_filename)
//{
//	/*FILE *lp;
//	TCHAR szBuffer[1024] = {0};
//	CString strTemp;
//
//	if(signed(m_pUIContainer.size()) < 0)
//	return FALSE;
//
//	_tfopen_s(&lp, p_filename.GetString(), _T("w+"));
//
//	_ftprintf(lp,_T("=================================================================================\n"),szBuffer);
//	_ftprintf(lp,_T("=================================================================================\n"),szBuffer);
//	_ftprintf(lp,_T("==                               Ran_UI_File Ver1.0                            ==\n"),szBuffer);
//	_ftprintf(lp,_T("==													                            ==\n"),szBuffer);
//	_ftprintf(lp,_T("=================================================================================\n"),szBuffer);
//	_ftprintf(lp,_T("=================================================================================\n"),szBuffer);
//
//	_ftprintf(lp,_T("\n\n"),szBuffer);
//
//	for(int i=0;i<signed(m_pUIContainer.size());i++)
//	{
//	_ftprintf(lp,_T("UI_CONTROL_NUM[%d]\n"),i,szBuffer);
//	}
//
//	fclose(lp);*/
//
//	return TRUE;
//}
//
//// 모눈종이 선색 설정
void CInterfaceAdmin::SetGraphLineColor(int nA, int nR,int nG,int nB)
{
	m_LineColor.a = nA;
	m_LineColor.r = nR;
	m_LineColor.g = nG;
	m_LineColor.b = nB;
}
//
//void CInterfaceAdmin::EndTextEdit()
//{
//	//if( m_pMainUIRight )
//	//	m_pMainUIRight->EndTextEdit();
//}
