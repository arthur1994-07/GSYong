#include "StdAfx.h"

#include "ModalWindow.h"

#include "../../InnerInterface.h"
#include "../../OuterInterface.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"

#include "../../OldUI/Interface/RandPassKeyboard.h" // 2차비번
#include "../../OldUI/Interface/PassKeyboard.h" // 2차비번
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h" // 2차비번
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
 
#include "../../../EngineLib/Common/DXInputString.h"

#include "../../../EngineLib/DxTools/DxFontMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CModalWindow::nLIMIT_CHAR = 16;
const int CModalWindow::nMONEY_UNIT = 3;
const int CModalWindow::nLIMIT_NUMBER_CHAR = 11;
const int CModalWindow::nLIMIT_MONEY_CHAR = 11;
int CModalWindow::m_nMODAL_NEWID = 1;
const CString CModalWindow::strMONEY_SYMBOL = _T(",");

static bool bINTERFACE_TEST = true;

CModalWindow::CModalWindow (EngineDeviceMan* pEngineDevice)
    : CUIModal(pEngineDevice)
    , m_pOK ( NULL )
    , m_pCANCEL ( NULL )
    , m_pYES ( NULL )
    , m_pNO ( NULL )
    , m_p1BUTTON ( NULL )
    , m_p2BUTTONLEFT ( NULL )
    , m_p2BUTTONRIGHT ( NULL )
    , m_pTextBox ( NULL )
    , m_pTextBoxTail ( NULL )
    , m_pEditBox ( NULL )
    , m_pLineBoxWindow ( NULL )
    , m_pLineBoxWindowTipbox ( NULL )
    , m_pLineBoxTextBox ( NULL )
    , m_pLineBoxTipbox ( NULL )
    , m_nModalType ( UI::MODAL_INFOMATION )
    , m_bTEST_MULTI ( false )
    , m_nData1 ( -1 )
    , m_nData2 ( -1 )
	, m_pPassKeyboard ( NULL ) // 2차비번
	, m_pRandPassKeyboard ( NULL ) // 2차비번
	, m_pEditBoxMan( NULL ) // 2차비번
	, m_pLineBoxEditMan( NULL ) // 2차비번
	, m_pLineBoxEditMan2( NULL ) // 2차비번
{
}

CModalWindow::~CModalWindow ()
{
}

void CModalWindow::CreateBaseModal ( char* szWindowKeyword )
{
	if ( bINTERFACE_TEST )
	{
	}
	else
	{
		CreateTitle ( "MODAL_WINDOW_TITLE", "MODAL_WINDOW_TITLE_LEFT", "MODAL_WINDOW_TITLE_MID", "MODAL_WINDOW_TITLE_RIGHT", "MODAL_WINDOW_TEXTBOX", NULL );
	//	CreateCloseButton ( "MODAL_WINDOW_CLOSE", "MODAL_WINDOW_CLOSE_F", "MODAL_WINDOW_CLOSE_OVER" );
		CreateBody ( "MODAL_WINDOW_BODY", "MODAL_WINDOW_BODY_LEFT", "MODAL_WINDOW_BODY_UP", "MODAL_WINDOW_BODY_MAIN", "MODAL_WINDOW_BODY_DOWN", "MODAL_WINDOW_BODY_RIGHT" );

		ResizeControl ( szWindowKeyword );
	}
}

void CModalWindow::CreateSubControl ()
{
	if ( bINTERFACE_TEST )
	{
		CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pFont11 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 11, _DEFAULT_FONT_FLAG );

		CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessage ( "BASIC_MODAL_LINE_BOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );
        m_pLineBoxWindow = pLineBox;

        pLineBox = new CBasicLineBox(m_pEngineDevice);
        pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateBaseBoxSystemMessage ( "BASIC_MODAL_LINE_BOX_TIP" );
        pLineBox->ResetAlignFlag ();
        RegisterControl ( pLineBox );
        m_pLineBoxWindowTipbox = pLineBox;

		pLineBox = new CBasicLineBox(m_pEngineDevice);
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp ( "BASIC_MODAL_LINE_TEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );
        m_pLineBoxTextBox = pLineBox;

        pLineBox = new CBasicLineBox(m_pEngineDevice);
        pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pLineBox->CreateBaseBoxSystemMessageUp ( "BASIC_MODAL_LINE_TEXTBOX_TIP" );
        pLineBox->ResetAlignFlag ();
        RegisterControl ( pLineBox );
        m_pLineBoxTipbox = pLineBox;

		CBasicTextBox* pMapMoveTextBox = new CBasicTextBox(m_pEngineDevice);
		pMapMoveTextBox->CreateSub ( this, "BASIC_MODAL_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pMapMoveTextBox->SetFont ( pFont9 );
		pMapMoveTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		pMapMoveTextBox->ResetAlignFlag ();
		RegisterControl ( pMapMoveTextBox );
		m_pTextBox = pMapMoveTextBox;

        CBasicTextBox* pTextBoxTail = new CBasicTextBox(m_pEngineDevice);
        pTextBoxTail->CreateSub ( this, "BASIC_MODAL_TEXTBOX_TAIL", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        pTextBoxTail->SetFont ( pFont9 );
        pTextBoxTail->SetTextAlign ( TEXT_ALIGN_BOTH_X );
        pTextBoxTail->ResetAlignFlag ();
        RegisterControl ( pTextBoxTail );
        m_pTextBoxTail = pTextBoxTail;

		m_pOK = CreateTextButton ( "MODAL_OK", MODAL_OK, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 0 ) );
		m_pOK->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
		m_pCANCEL = CreateTextButton ( "MODAL_CANCEL", MODAL_CANCEL, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 1 ) );
		m_pCANCEL->SetShortcutKey ( DIK_ESCAPE );
		m_pYES = CreateTextButton ( "MODAL_YES", MODAL_YES, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 2 ) );
		m_pYES->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
		m_pNO = CreateTextButton ( "MODAL_NO", MODAL_NO, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 3 ) );
		m_pNO->SetShortcutKey ( DIK_ESCAPE );

		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( "BASIC_MODAL_EDIT_BOX_BACK" );
		RegisterControl ( pBasicLineBox );
		m_pEditBoxBack = pBasicLineBox;

		CUIEditBox* pEditBox = new CUIEditBox(m_pEngineDevice);
		pEditBox->CreateSub ( this, "BASIC_MODAL_EDIT_BOX", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, MODAL_EDITBOX );
		pEditBox->CreateCarrat ( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput ( nLIMIT_CHAR );
		pEditBox->SetFont ( pFont9 );		
		RegisterControl ( pEditBox );
		m_pEditBox = pEditBox;	

		CBasicLineBox* pBasicEditManLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicEditManLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicEditManLineBox->CreateBaseBoxEditBox ( "PASS_SET_INPUT_BACK" );
		RegisterControl ( pBasicEditManLineBox );
		m_pLineBoxEditMan = pBasicEditManLineBox;

		CBasicLineBox* pBasicEditManLineBox2 = new CBasicLineBox(m_pEngineDevice);
		//pBasicEditManLineBox2 = new CBasicLineBox(m_pEngineDevice);
		pBasicEditManLineBox2->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicEditManLineBox2->CreateBaseBoxEditBox ( "PASS_SET_RE_INPUT_BACK" );
		RegisterControl ( pBasicEditManLineBox2 );
		m_pLineBoxEditMan2 = pBasicEditManLineBox2;

		CUIEditBoxMan* pEditBoxMan = new CUIEditBoxMan(m_pEngineDevice);
		pEditBoxMan->CreateSub ( this, "LOGIN_EDITMAN", UI_FLAG_DEFAULT, MODAL_EDITBOX_MAN );
		pEditBoxMan->CreateEditBox ( LOGIN_EDIT_PWD, "PASS_SET_INPUT", "LOGIN_CARRAT", TRUE, UINT_MAX, pFont9, 4 );
		pEditBoxMan->CreateEditBox ( LOGIN_EDIT_2ND_PWD, "PASS_SET_RE_INPUT", "LOGIN_CARRAT", TRUE, UINT_MAX, pFont9, 4 );
		pEditBoxMan->SetHide ( LOGIN_EDIT_PWD, TRUE );
		pEditBoxMan->SetHide ( LOGIN_EDIT_2ND_PWD, TRUE );
		RegisterControl( pEditBoxMan );
		m_pEditBoxMan = pEditBoxMan;

		// 2차 비밀번호 - 신규 생성 설정
		// 텍스트 - 2차 비밀번호 입력
		CBasicTextBox* pTextBoxPassSetInput = new CBasicTextBox(m_pEngineDevice);
		pTextBoxPassSetInput->CreateSub ( this, "PASS_SET_TEXTBOX_INPUT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pTextBoxPassSetInput->SetFont ( pFont9 );
		pTextBoxPassSetInput->AddText ( ID2GAMEWORD( "PASS_SET_TEXT", 0 ) );
		//pTextBoxPassSetInput->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		pTextBoxPassSetInput->ResetAlignFlag ();
		RegisterControl ( pTextBoxPassSetInput );
		m_pTextBoxPassSetInput = pTextBoxPassSetInput;

		// 2차 비밀번호 - 신규 생성 설정
		// 텍스트 - 2차 비밀번호 재입력
		CBasicTextBox* pTextBoxPassSetReInput = new CBasicTextBox(m_pEngineDevice);
		pTextBoxPassSetReInput->CreateSub ( this, "PASS_SET_TEXTBOX_RE_INPUT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pTextBoxPassSetReInput->SetFont ( pFont9 );
		pTextBoxPassSetReInput->AddText ( ID2GAMEWORD( "PASS_SET_TEXT", 1 ) );
		//pTextBoxPassSetReInput->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		pTextBoxPassSetReInput->ResetAlignFlag ();
		RegisterControl ( pTextBoxPassSetReInput );
		m_pTextBoxPassSetReInput = pTextBoxPassSetReInput;

		m_p1BUTTON = CreateControl ( "BASIC_MODAL_1BUTTON" );
		m_p2BUTTONLEFT = CreateControl ( "BASIC_MODAL_2BUTTON_LEFT" );
		m_p2BUTTONRIGHT = CreateControl ( "BASIC_MODAL_2BUTTON_RIGHT" );

		// 2차 비밀번호 설정되어있을시
		m_pPassKeyboard = new CPassKeyboard (this, m_pEngineDevice);
		m_pPassKeyboard->CreateSub( this, "MODAL_PASS_KEYBOARD", UI_FLAG_CENTER_X );
		m_pPassKeyboard->CreateSubControl();
		RegisterControl ( m_pPassKeyboard );

		// 2차 비밀번호 신규시
		m_pRandPassKeyboard = new CRandPassKeyboard (this, m_pEngineDevice);
		m_pRandPassKeyboard->CreateSub( this, "MODAL_PASS_KEYBOARD", UI_FLAG_CENTER_X );
		m_pRandPassKeyboard->CreateSubControl();
		RegisterControl ( m_pRandPassKeyboard );

		// 최초 생성시 Visible이기때문에 강제로 꺼준다
		m_pPassKeyboard->SetVisibleSingle(FALSE);;
		m_pRandPassKeyboard->SetVisibleSingle(FALSE);
		///////////////////////
	}
}

CBasicTextButton* CModalWindow::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE19;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pButton );

	return pButton;
}

void CModalWindow::DoModal(const std::string& strText, int nModalTitle, int nModalType, UIGUID CallerID, bool bShowPassKey, bool bTwian)
{
	if (nModalTitle < UI::MODAL_INFOMATION || MODAL_NOTITLE < nModalTitle)
	{
        std::string ErrorMsg(
            sc::string::format(
                "[Critical] Check index. CModalWindow::DoModal %1% %2% Type %3% Caller %4%",
                strText, nModalTitle, nModalType, static_cast<int> (CallerID)));
		GASSERT_MSG(0, ErrorMsg);
		return;
	}

	if ( MODAL_NOTITLE == nModalTitle )
	{
		SetTitleName ( NULL );
	}
	else
	{
		SetTitleName ( (char*)ID2GAMEWORD ( "MODAL_TITLE", nModalTitle ) );
	}

	SetCallerID ( CallerID );

	m_pTextBox    ->SetText ( strText.c_str() );
    m_pTextBoxTail->SetVisibleSingle( FALSE );

    m_pLineBoxWindow        ->SetVisibleSingle( TRUE );
    m_pLineBoxWindowTipbox  ->SetVisibleSingle( FALSE );
    m_pLineBoxTipbox        ->SetVisibleSingle( FALSE );    

    ModalWindowResize ( "BASIC_MODAL_WINDOW" );

	BOOL bLimitedInputForKR_GS = FALSE;

	// 2차 비밀번호 신규일 시 초기화
	m_pEditBoxMan->Init();
	// 2차비번
	// 아래쪽 ReArrangeControl에서 가상키보드 여부로 에디트박스 입력 막는 부분이 있어서 반드시 먼저 수행
	if( bShowPassKey )
	{
		if( CallerID == OUTER_MODAL_JOIN_SECOUNDPASSWORD || bTwian == true )
		{
			// 대만 2차비번일 경우
			m_pRandPassKeyboard->SetVisibleSingle( FALSE );
			m_pPassKeyboard->SetVisibleSingle( TRUE );
		}
		else
		{
			bLimitedInputForKR_GS = TRUE;

			if( nModalType != UI::EDITBOX_CHECK_PWD )	// 2차 비밀번호가 설정되어있을 시
			{
				m_pPassKeyboard->SetVisibleSingle( FALSE );
				m_pRandPassKeyboard->SetVisibleSingle( TRUE );
			}
			else // 2차 비밀번호가 설정이 안되어있을 시 ( 신규 )
			{
				m_pPassKeyboard->SetVisibleSingle( FALSE );
				m_pRandPassKeyboard->SetVisibleSingle( TRUE );
			}
		}
	}
	else
	{
			m_pPassKeyboard->SetVisibleSingle( FALSE );
			m_pRandPassKeyboard->SetVisibleSingle( FALSE );
	}
	///////////////////////////

	m_nModalType = nModalType;	
	ReArrangeControl ( nModalType, bLimitedInputForKR_GS );
}

void CModalWindow::DoModalTipBox ( const std::string& strText, const std::string& strTip, int nModalTitle, int nModalType, UIGUID CallerID )
{
    if (nModalTitle < UI::MODAL_INFOMATION || MODAL_NOTITLE < nModalTitle)
    {
        std::string ErrorMsg(
            sc::string::format(
                "[Critical] Check index. CModalWindow::DoModalTipBox %1% %2% Title %3% Type %4% Call %5%",
                strText, strTip, nModalTitle, nModalType, static_cast<int> (CallerID)));
        GASSERT_MSG(0, ErrorMsg);
        return;
    }

    if ( MODAL_NOTITLE == nModalTitle )
    {
        SetTitleName ( NULL );
    }
    else
    {
        SetTitleName ( (char*)ID2GAMEWORD ( "MODAL_TITLE", nModalTitle ) );
    }

    SetCallerID ( CallerID );

    m_pTextBox    ->SetText ( strText.c_str() );
    m_pTextBoxTail->SetText ( strTip.c_str(), NS_UITEXTCOLOR::YELLOW );
    m_pTextBoxTail->SetVisibleSingle( TRUE );

    m_pLineBoxWindow        ->SetVisibleSingle( FALSE );
    m_pLineBoxWindowTipbox  ->SetVisibleSingle( TRUE );
    m_pLineBoxTipbox        ->SetVisibleSingle( TRUE );

    ModalWindowResize ( "BASIC_MODAL_WINDOW_TAIL_TEXT" );

    m_nModalType = nModalType;	
    ReArrangeControl ( nModalType );
}

void CModalWindow::ReArrangeControl ( int nModalType,  BOOL bUseLimitedInput )
{
	m_pOK->SetVisibleSingle ( FALSE );
	m_pCANCEL->SetVisibleSingle ( FALSE );
	m_pYES->SetVisibleSingle ( FALSE );
	m_pNO->SetVisibleSingle ( FALSE );
	m_pEditBox->SetVisibleSingle ( FALSE );
	m_pEditBoxBack->SetVisibleSingle ( FALSE );
	m_pEditBoxMan->SetVisibleSingle( FALSE );
	m_pLineBoxEditMan->SetVisibleSingle( FALSE );
	m_pLineBoxEditMan2->SetVisibleSingle( FALSE );
	m_pTextBoxPassSetInput->SetVisibleSingle( FALSE );
	m_pTextBoxPassSetReInput->SetVisibleSingle( FALSE );

	const UIRECT& rcParentPos = GetGlobalPos ();
	D3DXVECTOR2	vParent ( rcParentPos.left, rcParentPos.top );	

	switch ( nModalType )
	{
	case UI::OKCANCEL:
		{
			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONLEFT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pOK->SetLocalPos ( vDummy );
				m_pOK->SetGlobalPos ( vPos );
				m_pOK->SetVisibleSingle ( TRUE );
			}

			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONRIGHT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pCANCEL->SetLocalPos ( vDummy );
				m_pCANCEL->SetGlobalPos ( vPos );
				m_pCANCEL->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case UI::YESNO:
		{
			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONLEFT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pYES->SetLocalPos ( vDummy );
				m_pYES->SetGlobalPos ( vPos );
				m_pYES->SetVisibleSingle ( TRUE );
			}

			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONRIGHT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pNO->SetLocalPos ( vDummy );
				m_pNO->SetGlobalPos ( vPos );
				m_pNO->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case UI::OK:
		{
			const UIRECT& rc2BUTTONPOS = m_p1BUTTON->GetLocalPos ();
			D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
			D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

			m_pOK->SetLocalPos ( vDummy );
			m_pOK->SetGlobalPos ( vPos );
			m_pOK->SetVisibleSingle ( TRUE );
		}
		break;

	case UI::CANCEL:
		{
			const UIRECT& rc2BUTTONPOS = m_p1BUTTON->GetLocalPos ();
			D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
			D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

			m_pCANCEL->SetLocalPos ( vDummy );
			m_pCANCEL->SetGlobalPos ( vPos );
			m_pCANCEL->SetVisibleSingle ( TRUE );
		}
		break;

	case UI::EDITBOX:
	case UI::EDITBOX_PETNAME:
	case UI::EDITBOX_NUMBER:
	case UI::EDITBOX_MONEY:
	case UI::MODAL_EDITBOX_LOTTERY:
	case UI::MODAL_FILE_NAME:
	case UI::EDITBOX_CHECK_PWD: // 2차비번
		{
			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONLEFT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pOK->SetLocalPos ( vDummy );
				m_pOK->SetGlobalPos ( vPos );
				m_pOK->SetVisibleSingle ( TRUE );
			}

			{
				const UIRECT& rc2BUTTONPOS = m_p2BUTTONRIGHT->GetLocalPos ();
				D3DXVECTOR2 vDummy ( rc2BUTTONPOS.left, rc2BUTTONPOS.top );
				D3DXVECTOR2 vPos ( vParent.x + vDummy.x, vParent.y + vDummy.y );

				m_pCANCEL->SetLocalPos ( vDummy );
				m_pCANCEL->SetGlobalPos ( vPos );
				m_pCANCEL->SetVisibleSingle ( TRUE );
			}

			m_pEditBoxBack->SetVisibleSingle ( TRUE );

			m_pEditBox->SET_MONEY_FORMAT ( 0 );
			m_pEditBox->SetAlign ( EDIT_LEFT_ALIGN );

			if( UI::EDITBOX_PETNAME == nModalType )
			{
				m_pEditBox->SetLimitInput( 12 ); // 펫 이름은 한글 6자로 제한
			}
			else if ( UI::EDITBOX_NUMBER == nModalType )
			{
				m_pEditBox->DoUSE_NUMBER ( true );
				m_pEditBox->SetLimitInput (	nLIMIT_NUMBER_CHAR );
				m_pEditBox->SetAlign ( EDIT_RIGHT_ALIGN );
			}
			else if ( UI::EDITBOX_MONEY == nModalType )
			{
				m_pEditBox->SET_MONEY_FORMAT ( nMONEY_UNIT, strMONEY_SYMBOL );
				m_pEditBox->SetLimitInput ( nLIMIT_MONEY_CHAR );
				m_pEditBox->SetAlign ( EDIT_RIGHT_ALIGN );
			}
			else if ( nModalType == UI::MODAL_EDITBOX_LOTTERY )
			{
				m_pEditBox->SetLimitInput ( 10 );
			}
			else if ( nModalType == UI::MODAL_FILE_NAME )
			{
				m_pEditBox->SetLimitInput ( 20 );	//	 16 + 1 + 3
			}			
			else if( nModalType == UI::EDITBOX_CHECK_PWD ) // 2차비번 
			{
				m_pEditBoxMan->SetVisibleSingle( TRUE );
			}
			else
			{
				m_pEditBox->SetLimitInput ( nLIMIT_CHAR );
			}

			// 2차비번 사용하는 경우에는 EditBox 를 보이지 않도록. EditBoxMan 을 사용.
			if( nModalType != UI::EDITBOX_CHECK_PWD )
			{
				m_pEditBox->SetVisibleSingle ( TRUE );
				m_pEditBox->EndEdit ();

				m_pEditBox->ClearEdit ();
			}
			else
			{
				m_pEditBoxBack->SetVisibleSingle(FALSE);

				m_pTextBoxPassSetInput->SetVisibleSingle( TRUE );
				m_pTextBoxPassSetReInput->SetVisibleSingle( TRUE );
				m_pLineBoxEditMan->SetVisibleSingle( TRUE );
				m_pLineBoxEditMan2->SetVisibleSingle( TRUE );

				m_pEditBoxMan->EndEdit ();

				m_pEditBoxMan->ClearEdit(LOGIN_EDIT_PWD);
				m_pEditBoxMan->ClearEdit(LOGIN_EDIT_2ND_PWD);
			}

			if( !m_pPassKeyboard->IsVisible() ) // KR/GS
			{
				if( nModalType == UI::EDITBOX_CHECK_PWD )
				{
					m_pEditBoxMan->BeginEdit();
				}
				else
				{
					if( m_pEditBox->IsLimitedKeyInput() )
					{
						m_pEditBox->UsableKeyInput();
					}

					m_pEditBox->BeginEdit();
				}
			}
			else // 대만의 경우
			{
				if( nModalType == UI::EDITBOX )
				{
					m_pEditBox->DisableKeyInput();
				}
				else
				{
					// 2차비번
					// 임시로 풀어둠
					//m_pEditBoxMan->BeginEdit();
				}
			}

			// KR과 GS의 경우에만 2차비번 입력을 4자리로 제한하므로 기존 로직은 그대로두고 해당 경우에만 추가적으로 변경하도록 함
			// KR과 GS의 경우에만 키보드 입력을 차단
			if( bUseLimitedInput )
			{
				m_pEditBox->SetLimitInput ( 4 );

				m_pEditBox->EndEdit();
				//m_pEditBoxMan->EndEdit();

				m_pEditBox->DisableKeyInput();
				//m_pEditBoxMan->DisableKeyInput();
			}
		}
		break;
	}
}

void CModalWindow::ResizeControl ( char* szWindowKeyword )
{
    CUIControl TempControl(m_pEngineDevice);
    TempControl.Create ( 1, szWindowKeyword );
    const UIRECT& rcParentOldPos = GetLocalPos ();
    const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
    AlignSubControl ( rcParentOldPos, rcParentNewPos );

    SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CModalWindow::ModalWindowResize ( char* szWindowKeyword )
{
    CUIControl TempControl(m_pEngineDevice); 
    TempControl.Create ( 1, szWindowKeyword );

    UIRECT rcParentLocalPos   = GetLocalPos ();
    UIRECT rcControlLoacalPos = TempControl.GetLocalPos ();

    AlignSubControl ( rcParentLocalPos, rcControlLoacalPos );
    SetLocalPos     ( rcControlLoacalPos );

    UIRECT rcParentGlobalOldPos = GetGlobalPos ();
    UIRECT rcParentGlobalNewPos = rcParentGlobalOldPos;

    rcParentGlobalNewPos.left   = rcParentGlobalOldPos.left;
    rcParentGlobalNewPos.top    = rcParentGlobalOldPos.top;
    rcParentGlobalNewPos.right  = rcParentGlobalNewPos.left + rcControlLoacalPos.sizeX;
    rcParentGlobalNewPos.bottom = rcParentGlobalNewPos.top  + rcControlLoacalPos.sizeY;
    rcParentGlobalNewPos.sizeX  = rcControlLoacalPos.sizeX;
    rcParentGlobalNewPos.sizeY  = rcControlLoacalPos.sizeY;

    SetGlobalPos ( rcParentGlobalNewPos );
}

CUIControl*	CModalWindow::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}

void	CModalWindow::SetModalData ( int nData1, int nData2 )
{
	m_nData1 = nData1;
	m_nData2 = nData2;
}

void CModalWindow::SetModalData(const D3DXVECTOR3& vData )
{
	m_vData = vData;
}

void	CModalWindow::GetModalData ( int* pnData1, int* pnData2 )
{
	*pnData1 = m_nData1;
	*pnData2 = m_nData2;
}

void	CModalWindow::GetModalData( D3DXVECTOR3* vData )
{
	*vData = m_vData;
}

bool	CModalWindow::IsEditBoxType()
{
	return ( EDITBOX <= m_nModalType && m_nModalType <= UI::MODAL_FILE_NAME );
}


void CModalWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIModal::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	//	탭 이동
	if ( m_pEditBoxMan )
	{
		CUIControl* pParent = m_pEditBoxMan->GetTopParent ();
		if ( !pParent )	pParent = this;	//	만약 이클래스가 최상위 컨트롤인 경우
		BOOL bFocus = ( pParent->IsFocusControl() );

		if ( bFocus )
		{
			if ( UIKeyCheck::GetInstance()->Check ( DIK_TAB, DXKEY_DOWN ) )
			{
				m_pEditBoxMan->GoNextTab ();
			}
		}
	}
}


void CModalWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );
}

BOOL CModalWindow::Is2ndPWDModal() 
{ 
	return m_nModalType == UI::EDITBOX_CHECK_PWD ? TRUE : FALSE; 
}

// 2차비번 둘다 동일하게 입력되었는지 확인
// 2차비번 최소, 최대 길이검사 / 같은숫자 반복번호인지 검사
int CModalWindow::CheckNew2ndPassString()
{

	CString strPWD = m_pEditBoxMan->GetEditString( LOGIN_EDIT_PWD );
	CString strPWD2 = m_pEditBoxMan->GetEditString( LOGIN_EDIT_2ND_PWD );

	int nLen = strPWD.GetLength();

	if( nLen != 4 )
		return -1;

	char *pPWD = (char *)strPWD.GetBuffer(nLen);
	int nCount = 0;

	for( int i = 1; i < 4; i++ )
	{
		if( (int)pPWD[0] == (int)pPWD[i] )
			nCount++;
	}

	if( nCount == 3 )
		return -2;

	for( int i=0; i<nLen; i++ ) 
	{ 
		char temp = strPWD.GetAt(i); 

		// 입력된 키가 0 ~ 9 사이인가를 체크. 
		if(temp >= '0' && temp <= '9') continue; 
		else return -1; 
	} 

	if( strPWD.IsEmpty() == FALSE && strPWD == strPWD2 )
		return 1;
	else
		return 0;
}

CString CModalWindow::GetNew2ndPassString()
{
	return m_pEditBoxMan->GetEditString( LOGIN_EDIT_PWD );
}

/*
void DoModal ( const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID, BOOL bHide )
{
	//	m_pInterface->SetModalWindowOpen ();

	CModalWindow* pModalWindow = m_pInterface->GetModalWindow ();
	if ( pModalWindow )
	{
		//		m_pInterface->UiHideGroup ( MODAL_WINDOW, true );
		if ( m_pInterface->UiIsVisibleGroup ( MODAL_WINDOW ) )
		{
			//			GASSERT ( 0 && "이미 켜져있습니다." );
			m_pInterface->UiHideGroup ( MODAL_WINDOW, true );
		}

		pModalWindow->SetHide ( bHide );
		pModalWindow->SetUseInner ( true );
		pModalWindow->DoModal ( strText, nModalTitle, nModalType, CallerID );
		m_pInterface->UiShowGroupFocus ( MODAL_WINDOW, true );
	}
}

void DoModalOuter ( const CString& strText, int nModalTitle, int nModalType, UIGUID CallerID, BOOL bHide )
{
	UIGUID RenderStage = m_pInterface->GetRenderStage ();
	CModalWindow* pModalWindow = m_pInterface->GetModalWindow ();
	if ( pModalWindow )
	{
		m_pInterface->UiHideGroup ( MODAL_WINDOW_OUTER, true );

		pModalWindow->SetPrevPageID ( RenderStage );
		pModalWindow->SetHide ( bHide );
		pModalWindow->SetUseInner ( false );
		pModalWindow->DoModal ( strText, nModalTitle, nModalType, CallerID );
		m_pInterface->UiShowGroupFocus ( MODAL_WINDOW_OUTER, true );
	}
}

void SetModalData ( int nData1, int nData2 )
{
	CModalWindow* pModalWindow = m_pInterface->GetModalWindow ();
	if ( pModalWindow ) pModalWindow->SetModalData ( nData1, nData2 );
}

void SetModalData( D3DXVECTOR3 vData )
{
	CModalWindow* pModalWindow = m_pInterface->GetModalWindow ();
	if ( pModalWindow ) pModalWindow->SetModalData ( vData );
}
*/
