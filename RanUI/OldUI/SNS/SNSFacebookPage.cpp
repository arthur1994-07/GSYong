
// bjju.sns 추가

#include "StdAfx.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxGrapUtils.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/UIMultiEditBoxSmart.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/SmartComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/SNS/GLSNSClient.h"

#include "../../InnerInterface.h"
#include "../../ModalCallerID.h"
#include "../Util/ModalWindow.h"
#include "../Util/CheckBox.h"
#include "../Util/UIPage.h"

#include "./SNSWindow.h"
#include "./SNSFacebookPage.h"
#include "./SNSMenuIcon.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSNSFacebookPage::CSNSFacebookPage (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIPage(pEngineDevice)
    , m_pGaeaClient                 (pGaeaClient)
    , m_pInterface                  (pInterface)
    , m_pStaticTextBox_WriteWall	( NULL  )
	, m_pStaticTextBox_Picture		( NULL  )
	, m_pStaticTextBox_Friend		( NULL  )
	, m_pLineBox_WriteWall			( NULL  )
	, m_pLineBox_Picture			( NULL  )
	, m_pLineBox_Friend				( NULL  )
	, m_pButton_WriteWall			( NULL  )
	, m_pButton_Picture				( NULL  )
	, m_pButton_Friend				( NULL  )
	, m_pEditBox_WriteWall			( NULL  )
	, m_pEditBox_Picture			( NULL  )
	, m_pEditBox_Friend				( NULL  )
	, m_pComboBox_Picture			( NULL  )
	, m_pComboBox_RollOver_Picture  ( NULL  )
	, m_RollOverID					( NO_ID )
	, m_bRollOverLBUP				( FALSE )
	, m_dwPicture					( 0 )
{
}

CSNSFacebookPage::~CSNSFacebookPage ()
{
	for ( int i=0; i<PAGE_CHECKBOX_MAX; ++i )
	{
		m_pCheckBox[i] = NULL;
	}

	for ( int i=0; i<PAGE_CHECKBOX_MAX; ++i )
	{
		m_pCheckBoxText[i] = NULL;
	}
}

void CSNSFacebookPage::CreateSubControl ()
{
	CD3DFontPar*	pFont		= m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	const int		nAlignLeft	= TEXT_ALIGN_LEFT;

	// Note : 정적 텍스트
	{
		m_pStaticTextBox_WriteWall = CreateStaticControl( "SNS_WINDOW_PAGE_FACEBOOK_STATIC_TEXT_WALL", pFont, nAlignLeft );
		m_pStaticTextBox_WriteWall->SetText( ID2GAMEWORD( "SNS_FACEBOOK_PAGE_TEXT", 0 ) );

		m_pStaticTextBox_Picture = CreateStaticControl( "SNS_WINDOW_PAGE_FACEBOOK_STATIC_TEXT_PICTURE", pFont, nAlignLeft );
		m_pStaticTextBox_Picture->SetText( ID2GAMEWORD( "SNS_FACEBOOK_PAGE_TEXT", 1 ) );

		m_pStaticTextBox_Friend = CreateStaticControl( "SNS_WINDOW_PAGE_FACEBOOK_STATIC_TEXT_FRIEND", pFont, nAlignLeft );
		m_pStaticTextBox_Friend->SetText( ID2GAMEWORD( "SNS_FACEBOOK_PAGE_TEXT", 2 ) );
	}

	// Note : 에디트박스
	{
		// Note : 배경
		{
			m_pLineBox_WriteWall = CreateLineBox ( "SNS_WINDOW_PAGE_FACEBOOK_LINEBOX_WALL" );	
			m_pLineBox_Picture   = CreateLineBox ( "SNS_WINDOW_PAGE_FACEBOOK_LINEBOX_PICTURE" );	
			m_pLineBox_Friend    = CreateLineBox ( "SNS_WINDOW_PAGE_FACEBOOK_LINEBOX_FRIEND" );	
		}

		m_pEditBox_WriteWall = new CUIMultiEditBoxSmart(m_pEngineDevice);
		m_pEditBox_WriteWall->CreateSub				( this, "SNS_WINDOW_PAGE_FACEBOOK_EDITBOX_WALL", UI_FLAG_DEFAULT, PAGE_EDITBOX_WALL );
		m_pEditBox_WriteWall->CreateCarrat			( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_WriteWall->CreateScrollVertical	();
		m_pEditBox_WriteWall->SetFont				( pFont );
		m_pEditBox_WriteWall->SetLimitInput			( 1024 );
		m_pEditBox_WriteWall->SetAutoTurn			( true );
		RegisterControl ( m_pEditBox_WriteWall );

		m_pEditBox_Picture = new CUIMultiEditBoxSmart(m_pEngineDevice);
		m_pEditBox_Picture->CreateSub				( this, "SNS_WINDOW_PAGE_FACEBOOK_EDITBOX_PICTURE", UI_FLAG_DEFAULT, PAGE_EDITBOX_PICTURE );
		m_pEditBox_Picture->CreateCarrat			( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_Picture->CreateScrollVertical	();
		m_pEditBox_Picture->SetFont					( pFont );
		m_pEditBox_Picture->SetLimitInput			( 1024 );
		m_pEditBox_Picture->SetAutoTurn				( true );
		RegisterControl ( m_pEditBox_Picture );

		m_pEditBox_Friend = new CUIEditBox(m_pEngineDevice);
		m_pEditBox_Friend->CreateSub ( this, "SNS_WINDOW_PAGE_FACEBOOK_EDITBOX_FRIEND", UI_FLAG_DEFAULT, PAGE_EDITBOX_FRIEND );
		m_pEditBox_Friend->CreateCarrat ( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		m_pEditBox_Friend->SetFont ( pFont );
		m_pEditBox_Friend->SetLimitInput ( CHR_ID_LENGTH+1 );	
		RegisterControl ( m_pEditBox_Friend );

	}

	// Note : 버튼
	{
		CBasicTextButton* pButton;
		m_pButton_WriteWall = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, PAGE_BUTTON_WALL );
		pButton->CreateBaseButton( "SNS_WINDOW_PAGE_FACEBOOK_BUTTON_WALL", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SNS_FACEBOOK_PAGE_BUTTON", 0 ) );
		RegisterControl( pButton );

		m_pButton_Picture = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, PAGE_BUTTON_PICTURE );
		pButton->CreateBaseButton( "SNS_WINDOW_PAGE_FACEBOOK_BUTTON_PICTURE", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP,ID2GAMEWORD( "SNS_FACEBOOK_PAGE_BUTTON", 1 ) );
		RegisterControl( pButton );

		m_pButton_Friend = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, PAGE_BUTTON_FRIEND );
		pButton->CreateBaseButton( "SNS_WINDOW_PAGE_FACEBOOK_BUTTON_FRIEND", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SNS_FACEBOOK_PAGE_BUTTON", 2 ) );
		RegisterControl( pButton );

		m_pButton_Back = pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON19", UI_FLAG_XSIZE, PAGE_BUTTON_BACK );
		pButton->CreateBaseButton( "SNS_WINDOW_PAGE_FACEBOOK_BUTTON_BACK", CBasicTextButton::SIZE19, 
			CBasicButton::CLICK_FLIP, ID2GAMEWORD( "SNS_PAGE_BUTTON", 0 ) );
		RegisterControl( pButton );
	}

	m_pStaticTextBox_Friend->SetVisibleSingle( FALSE );
	m_pLineBox_Friend->SetVisibleSingle( FALSE );
	m_pEditBox_Friend->SetVisibleSingle( FALSE );
	m_pButton_Friend->SetVisibleSingle( FALSE );

	// Note : 체크박스
	{
		// Note : 상자
		for ( int i=0; i<PAGE_CHECKBOX_MAX; ++i )
		{
			CString strControl;

			strControl.Format( "SNS_WINDOW_PAGE_FACEBOOK_CHECK_%d", i );

			m_pCheckBox[i] = new CCheckBox(m_pEngineDevice);
			m_pCheckBox[i]->CreateSub ( this, strControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE, PAGE_CHECKBOX_BEGIN+i );
			m_pCheckBox[i]->CreateSubControl ( "SNS_WINDOW_IMAGE_CHECKBOX_ON", "SNS_WINDOW_IMAGE_CHECKBOX_OFF" );
			RegisterControl ( m_pCheckBox[i] );
		}

		// Note : 텍스트
		for ( int i=0; i<PAGE_CHECKBOX_MAX; ++i )
		{
			CString strControl;

			strControl.Format( "SNS_WINDOW_PAGE_FACEBOOK_CHECK_TEXT_%d", i );

			m_pCheckBoxText[i] = CreateStaticControl( strControl.GetString(), pFont, nAlignLeft );
			m_pCheckBoxText[i]->SetOneLineText( "Empty" );
		}

		CString strFormat;
		m_pCheckBoxText[0]->SetOneLineText( ID2GAMEINTEXT( "SNS_WINDOW_PAGE_FACEBOOK_CHECK_LEVELUP") );

		strFormat.Format ( ID2GAMEINTEXT("SNS_WINDOW_PAGE_FACEBOOK_CHECK_GRINDING"), GLCONST_SNS::dw_REGISTER_GRINDING_LEVEL );
		m_pCheckBoxText[1]->SetOneLineText( strFormat );

		// Note : 사용하지 않은 체크박스들을 임시로 꺼준다.
		m_pCheckBox		[2]->SetVisibleSingle( FALSE );
		m_pCheckBoxText	[2]->SetVisibleSingle( FALSE );

		m_pCheckBox		[3]->SetVisibleSingle( FALSE );
		m_pCheckBoxText	[3]->SetVisibleSingle( FALSE );
	}

	// Note : 콤보 박스
	{
		CSmartComboBoxBasic* pComboBox; 
		pComboBox = m_pComboBox_Picture = new CSmartComboBoxBasic(m_pEngineDevice);
		pComboBox->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE, PAGE_COMBOBOX_PICTURE );
		pComboBox->CreateSmartComboBox ( "SNS_WINDOW_PAGE_FACEBOOK_COMBOBOX_PICTURE", "SNS_WINDOW_TEXTINFO_DEFAULT_LINEBOX" );			
		RegisterControl( pComboBox );

		CBasicComboBoxRollOver * pComboBoxRollOver;
		pComboBoxRollOver = m_pComboBox_RollOver_Picture = new CBasicComboBoxRollOver(m_pEngineDevice);
		pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, PAGE_COMBOBOX_ROLLOVER_PICTURE );
		pComboBoxRollOver->CreateBaseComboBoxRollOver( "SNS_WINDOW_PAGE_FACEBOOK_COMBOBOX_ROLLOVER_PICTURE" );
		pComboBoxRollOver->SetVisibleSingle( FALSE );
		RegisterControl( pComboBoxRollOver );
	}
}

void CSNSFacebookPage::OpenSubControl ()
{
    //  Note : 체크박스 세팅
    RANPARAM::SNSFACEBOOK_SET& sOptFaceBook = RANPARAM::sSNSFaceBook;
	m_pCheckBox[INDEX_CHECK_BOX_LEVELUP		]->SetCheck  ( sOptFaceBook.bLEVELUP	);
	m_pCheckBox[INDEX_CHECK_BOX_GRINDING	]->SetCheck  ( sOptFaceBook.bGRINDING	);

	ResetPictureCombo ( FALSE );
}

void CSNSFacebookPage::CloseSubControl ()
{
    //  Note : 체크박스 저장
    RANPARAM::SNSFACEBOOK_SET& sOptFaceBook = RANPARAM::sSNSFaceBook;
	sOptFaceBook.bLEVELUP		= m_pCheckBox[INDEX_CHECK_BOX_LEVELUP		]->IsChecked();
	sOptFaceBook.bGRINDING		= m_pCheckBox[INDEX_CHECK_BOX_GRINDING		]->IsChecked();
}

void CSNSFacebookPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	// Note : 콤보박스 업데이트
	UpdateComboBox ();

	CUIPage::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSNSFacebookPage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case PAGE_EDITBOX_WALL:
		{
			if( !m_pEditBox_WriteWall ) return;

			if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
			{
				m_pEditBox_WriteWall->EndEdit();
				m_pEditBox_WriteWall->BeginEdit();
			}
			else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
			{
				m_pEditBox_WriteWall->EndEdit();
			}

			m_pEditBox_WriteWall->TranslateUIMessage( PAGE_BUTTON_WALL, dwMsg );
		}
		break;

	case PAGE_EDITBOX_PICTURE:
		{
			if( !m_pEditBox_Picture ) return;

			if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
			{
				m_pEditBox_Picture->EndEdit();
				m_pEditBox_Picture->BeginEdit();
			}
			else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
			{
				m_pEditBox_Picture->EndEdit();
			}

			m_pEditBox_Picture->TranslateUIMessage( PAGE_EDITBOX_PICTURE, dwMsg );
		}
		break;

	case PAGE_EDITBOX_FRIEND:
		{
			if( !m_pEditBox_Friend ) return;

			if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
			{
				m_pEditBox_Friend->EndEdit();
				m_pEditBox_Friend->BeginEdit();
			}
			else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
			{
				m_pEditBox_Friend->EndEdit();
			}

			m_pEditBox_Friend->TranslateUIMessage( PAGE_EDITBOX_FRIEND, dwMsg );
		}
		break;

	// Note : 이미지 첨부 콤보박스 오픈
	case PAGE_COMBOBOX_PICTURE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER )
				{
					m_pComboBox_RollOver_Picture->SetVisibleSingle ( TRUE );
					//m_pComboBox_RollOver_Design->SetScrollPercent ( 0.0f );

					m_RollOverID	= PAGE_COMBOBOX_ROLLOVER_PICTURE;
					m_bRollOverLBUP = TRUE;

					ResetPictureCombo ( TRUE );
				}
			}
		}
		break;

	// Note : 이미지 첨부 콤보박스 롤오버
	case PAGE_COMBOBOX_ROLLOVER_PICTURE:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );
				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				int nIndex = m_pComboBox_RollOver_Picture->GetSelectIndex ();
				if ( nIndex < 0 ) return ;

				m_dwPicture = (DWORD)nIndex;

				CString strText = m_pComboBox_RollOver_Picture->GetSelectText( nIndex );
				m_pComboBox_Picture->SetText( strText );
			}
		}
		break;

	// Note : 담벼락 쓰기
	case PAGE_BUTTON_WALL:
		{
			if ( !m_pGaeaClient->GetSNSClient()->FBIsAuthed() )
			{
				break;
			}

			if ( !m_pEditBox_WriteWall )
			{
				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CString strWall = m_pEditBox_WriteWall->GetEditString ();

				if ( strWall.GetLength() == 0 )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_EMPTY_EDITBOX"), MODAL_ERROR, OK );
					break;
				}

				if ( !m_pGaeaClient->GetSNSClient()->FBWallPost ( std::string(strWall.GetString()) ) )
				{
					break;
				}

				SetEnableWallButton( FALSE );
			}
			break;
		}
		break;

		// Note : 이미지 올리기
	case PAGE_BUTTON_PICTURE:
		{
			if ( !m_pGaeaClient->GetSNSClient()->FBIsAuthed() )
			{
				break;
			}

			if ( !m_pEditBox_Picture )
			{
				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_dwPicture == 0 )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT("SNS_MODAL_MSG_ERROR_SELECT_SCREENSHOT"), MODAL_ERROR, OK );
					break;
				}

				CString strPath	   = DxGrapUtils::strCapturePath.c_str();
				CString strCaption = m_pEditBox_Picture->GetEditString ();
				
				strPath += m_pComboBox_RollOver_Picture->GetSelectText( m_dwPicture );

				if ( !m_pGaeaClient->GetSNSClient()->FBUploadPhoto ( std::string(strCaption.GetString()), std::string(strPath.GetString()) ) )
				{
					break;
				}

				SetEnablePicture( FALSE );
			}
			break;
		}
		break;

		// Note : 친구요청
	case PAGE_BUTTON_FRIEND:
		{

		}
		break;

	// Note : 메뉴로 돌아가기
	case PAGE_BUTTON_BACK:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				RequestPush( CSNSWindow::SNS_PAGE_REQ_MENU_PAGE_OPEN );
			}
			break;
		}
		break;
	}

	CUIPage::TranslateUIMessage ( ControlID, dwMsg );
}

void CSNSFacebookPage::UpdateComboBox ()
{
	if ( m_RollOverID != NO_ID )
	{
		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "심각한 오류, 노드가 널입니다." );
			return ;
		}

		DWORD dwControlMsg = pControl->GetMessageEx ();

		ResetMessageEx ();

		//	스크롤바에 관계된 메시지가 아니고		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = dwControlMsg;//GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bRollOverLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle ( FALSE );					
				}

				m_bRollOverLBUP = FALSE;
			}
		}
	}
}

void CSNSFacebookPage::ResetPictureCombo ( const BOOL bUpdate )
{
	CString strDefaultText = ID2GAMEWORD( "SNS_FACEBOOK_PAGE_TEXT", 3 );

	m_dwPicture = 0;
	m_pComboBox_Picture->SetText( strDefaultText );

	if ( !bUpdate )
	{
		return;
	}

	m_pComboBox_RollOver_Picture->ClearText   ();
	m_pComboBox_RollOver_Picture->AddText	  ( strDefaultText, FALSE, FALSE );
	m_pComboBox_RollOver_Picture->SetTextData ( 0, 0 );

	// Note : 스샷 폴더 내부의 그림 이미지를 전부 찾는다.
	CStringArray FileArray;
	FindSubDir ( DxGrapUtils::strCapturePath.c_str(), FileArray, "*.jpg" );

	int nSize = (int)FileArray.GetCount();
	for ( int i=0; i<nSize; ++i )
	{
		CString strPath = FileArray.GetAt( i );
		CString strName = strrchr( (const char*)strPath.GetString(), '\\' ) + 1;

		int	nIndex  = m_pComboBox_RollOver_Picture->AddText( strName, FALSE, TRUE );
		m_pComboBox_RollOver_Picture->SetTextData( nIndex, nIndex );
	}
}

void CSNSFacebookPage::ClearEditWall	()
{
	if ( m_pEditBox_WriteWall )
	{
		m_pEditBox_WriteWall->ClearEdit();
	}
}

void CSNSFacebookPage::ClearEditPicture ()
{
	if ( m_pEditBox_Picture )
	{
		m_pEditBox_Picture->ClearEdit();
	}
}

CBasicLineBoxSmart* CSNSFacebookPage::CreateLineBox ( const char* szControl, UIGUID ControlID )
{
	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub ( this, szControl, UI_FLAG_XSIZE | UI_FLAG_YSIZE, ControlID );
	pLineBox->CreateSubControl( "SNS_WINDOW_TEXTINFO_DEFAULT_LINEBOX" );
	RegisterControl ( pLineBox );		

	return pLineBox;
}

void CSNSFacebookPage::FindSubDir ( CString strDir, CStringArray& FileArray, const CString& strWildcard )
{
	strDir += "\\";
	strDir += strWildcard;

	CFileFind ff;
	BOOL bFound = ff.FindFile( strDir );

	while ( bFound )
	{
		bFound = ff.FindNextFile();

		if ( ff.IsDots() ) continue;
		
		if ( ff.IsDirectory() )
		{
			FindSubDir( ff.GetFilePath(), FileArray );
		}
		else
		{
			FileArray.Add( ff.GetFilePath() );
		}
	}

}

void CSNSFacebookPage::SetEnableWallButton ( const BOOL bEnable )
{
	if ( bEnable )
	{
		m_pButton_WriteWall->SetOneLineText( ID2GAMEWORD( "SNS_FACEBOOK_PAGE_BUTTON", 0 ) );
		m_pButton_WriteWall->SetNoMessage( FALSE );
		m_pButton_WriteWall->SetDiffuse( CUIPage::cCOLOR_BUTTON_ENABLE );
	}
	else
	{
		m_pButton_WriteWall->SetOneLineText( ID2GAMEINTEXT("SNS_WINDOW_PAGE_BUTTON_REQUIRE") );
		m_pButton_WriteWall->SetNoMessage( TRUE );
		m_pButton_WriteWall->SetDiffuse( CUIPage::cCOLOR_BUTTON_DISABLE );
	}
}

void CSNSFacebookPage::SetEnablePicture ( const BOOL bEnable )
{
	if ( bEnable )
	{
		m_pButton_Picture->SetOneLineText( ID2GAMEWORD( "SNS_FACEBOOK_PAGE_BUTTON", 1 ) );
		m_pButton_Picture->SetNoMessage( FALSE );
		m_pButton_Picture->SetDiffuse( CUIPage::cCOLOR_BUTTON_ENABLE );
	}
	else
	{

		m_pButton_Picture->SetOneLineText( ID2GAMEINTEXT("SNS_WINDOW_PAGE_BUTTON_REQUIRE") );
		m_pButton_Picture->SetNoMessage( TRUE );
		m_pButton_Picture->SetDiffuse( CUIPage::cCOLOR_BUTTON_DISABLE );
	}
}
