#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "CostumColorChangeWindow.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../RanLogic/Msg/GLContrlMsg.h"
#include "CostumColorSlot.h"
//#include "../../ModalCallerID.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CCostumColorChangeWindow::nLIMIT_NUMBER_CHAR = 3;
const int CCostumColorChangeWindow::nMAX_COLOR = 255;
const float CCostumColorChangeWindow::fRGB_RATIO = 8.225806f;

CCostumColorChangeWindow::CCostumColorChangeWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_nMainSelect(-1)
    , m_nSubSelect(-1)
    , m_pRProgressBar(NULL)
    , m_pGProgressBar(NULL)
    , m_pBProgressBar(NULL)
    , m_pREditBox(NULL)
    , m_pGEditBox(NULL)
    , m_pBEditBox(NULL)
    , m_nR(0)
    , m_nG(0)
    , m_nB(0)
    , m_nRBack(0)
    , m_nGBack(0)
    , m_nBBack(0)
    , m_nRegR(0)
    , m_nRegG(0)
    , m_nRegB(0)
    , m_bOKButton(FALSE)
    , m_bRestrict(false)
    , m_pRProgressBarRestrict(NULL)
    , m_pGProgressBarRestrict(NULL)
    , m_pBProgressBarRestrict(NULL)
    , m_fRestrictMax(1.0f)
    , m_fRestrictMin(0.0f)
{
}


CCostumColorChangeWindow::~CCostumColorChangeWindow ()
{
}

void CCostumColorChangeWindow::CreateSubControl ()
{
	CD3DFontPar * pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9 );

	CreateControl( "COSTUM_COLOR_TITLE_BACK_L" );
	CreateControl( "COSTUM_COLOR_TITLE_BACK_R" );
	CreateStaticControl( "COSTUM_COLOR_TITLE", ID2GAMEINTEXT("COSTUM_COLOR_TITLE"), pFont, TEXT_ALIGN_CENTER_X );

	//CreateStaticControl( "COSTUM_COLOR_IN_TEXT", ID2GAMEINTEXT("COSTUM_COLOR_IN_TEXT"), pFont, TEXT_ALIGN_CENTER_X );
	//CreateStaticControl( "COSTUM_COLOR_OUT_TEXT", ID2GAMEINTEXT("COSTUM_COLOR_OUT_TEXT"), pFont, TEXT_ALIGN_CENTER_X );

	CreateStaticControl( "COSTUM_COLOR1_TEXT", ID2GAMEINTEXT("COSTUM_COLOR1_TEXT"), pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	CreateStaticControl( "COSTUM_COLOR2_TEXT", ID2GAMEINTEXT("COSTUM_COLOR2_TEXT"), pFont, TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
	{ // Note : R,G,B Static
		CUIControl * pUIControl = new CUIControl(m_pEngineDevice);
		pUIControl->CreateSub( this, "COSTUM_COLOR_R" );
		RegisterControl( pUIControl );

		pUIControl = new CUIControl(m_pEngineDevice);
		pUIControl->CreateSub( this, "COSTUM_COLOR_G" );
		RegisterControl( pUIControl );

		pUIControl = new CUIControl(m_pEngineDevice);
		pUIControl->CreateSub( this, "COSTUM_COLOR_B" );
		RegisterControl( pUIControl );
	}

	//	Note : 프로그래스바 배경.
	{

		CUIControl * pUIControl = new CUIControl(m_pEngineDevice);
		pUIControl->CreateSub( this, "COSTUM_COLOR_R_PROGRESS" );
		RegisterControl( pUIControl );
		m_pRProgressBarRestrict = pUIControl;
		m_pRProgressBarRestrict->SetFlowBar( m_pRProgressBarRestrict->GetGlobalPos(), m_pRProgressBarRestrict->GetTexturePos() );
		m_pRProgressBarRestrict->FlowBarUpdate( 1.0f );

		pUIControl = new CUIControl(m_pEngineDevice);
		pUIControl->CreateSub( this, "COSTUM_COLOR_G_PROGRESS" );
		RegisterControl( pUIControl );
		m_pGProgressBarRestrict = pUIControl;
		m_pGProgressBarRestrict->SetFlowBar( m_pGProgressBarRestrict->GetGlobalPos(), m_pGProgressBarRestrict->GetTexturePos() );
		m_pGProgressBarRestrict->FlowBarUpdate( 1.0f );

		pUIControl = new CUIControl(m_pEngineDevice);
		pUIControl->CreateSub( this, "COSTUM_COLOR_B_PROGRESS" );
		RegisterControl( pUIControl );
		m_pBProgressBarRestrict = pUIControl;
		m_pBProgressBarRestrict->SetFlowBar( m_pBProgressBarRestrict->GetGlobalPos(), m_pBProgressBarRestrict->GetTexturePos() );
		m_pBProgressBarRestrict->FlowBarUpdate( 1.0f );
	}

	{	// Note : 프로그래스바
		CBasicProgressBar* pProgressBar( NULL );
		CUIControl*	pDummyBar( NULL );

		pProgressBar = new CBasicProgressBar(m_pEngineDevice);
		pProgressBar->CreateSub ( this, "COSTUM_COLOR_R_PROGRESS_BACK" );
		//pProgressBar->CreateSub ( this, "COSTUM_COLOR_R_PROGRESS" );
		pProgressBar->CreateOverImage ( "COSTUM_COLOR_R_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pRProgressBar = pProgressBar;

		pDummyBar = new CUIControl(m_pEngineDevice);
		pDummyBar->CreateSub ( this, "COSTUM_COLOR_R_PROGRESS_DUMMY", UI_FLAG_DEFAULT, COSTUM_COLOR_R_PROGRESS );
		RegisterControl ( pDummyBar );

		pProgressBar = new CBasicProgressBar(m_pEngineDevice);
		pProgressBar->CreateSub ( this, "COSTUM_COLOR_G_PROGRESS_BACK" );
		pProgressBar->CreateOverImage ( "COSTUM_COLOR_G_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pGProgressBar = pProgressBar;

		pDummyBar = new CUIControl(m_pEngineDevice);
		pDummyBar->CreateSub ( this, "COSTUM_COLOR_G_PROGRESS_DUMMY", UI_FLAG_DEFAULT, COSTUM_COLOR_G_PROGRESS );
		RegisterControl ( pDummyBar );

		pProgressBar = new CBasicProgressBar(m_pEngineDevice);
		pProgressBar->CreateSub ( this, "COSTUM_COLOR_B_PROGRESS_BACK" );
		pProgressBar->CreateOverImage ( "COSTUM_COLOR_B_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pBProgressBar = pProgressBar;		

		pDummyBar = new CUIControl(m_pEngineDevice);
		pDummyBar->CreateSub ( this, "COSTUM_COLOR_B_PROGRESS_DUMMY", UI_FLAG_DEFAULT, COSTUM_COLOR_B_PROGRESS );
		RegisterControl ( pDummyBar );
	}

	{ // Note : 입력 에디터 박스
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( "COSTUM_COLOR_R_EDITBOX_BACK" );
		RegisterControl( pBasicLineBox );

		CUIEditBox* pEditBox = new CUIEditBox(m_pEngineDevice);
		pEditBox->CreateSub( this, "COSTUM_COLOR_R_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_RIGHT, COSTUM_COLOR_R_EDITBOX );
		pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
		pEditBox->SetFont( pFont );
		pEditBox->DoUSE_NUMBER( true );
		RegisterControl( pEditBox );
		m_pREditBox = pEditBox;

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( "COSTUM_COLOR_G_EDITBOX_BACK" );
		RegisterControl( pBasicLineBox );

		pEditBox = new CUIEditBox(m_pEngineDevice);
		pEditBox->CreateSub( this, "COSTUM_COLOR_G_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_RIGHT, COSTUM_COLOR_G_EDITBOX );
		pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
		pEditBox->SetFont( pFont );	
		pEditBox->DoUSE_NUMBER( true );
		RegisterControl( pEditBox );
		m_pGEditBox = pEditBox;

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( "COSTUM_COLOR_B_EDITBOX_BACK" );
		RegisterControl( pBasicLineBox );

		pEditBox = new CUIEditBox(m_pEngineDevice);
		pEditBox->CreateSub( this, "COSTUM_COLOR_B_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_RIGHT, COSTUM_COLOR_B_EDITBOX );
		pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
		pEditBox->SetFont( pFont );
		pEditBox->DoUSE_NUMBER( true );
		RegisterControl( pEditBox );
		m_pBEditBox = pEditBox;
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, COSTUM_COLOR_OK );
		pButton->CreateBaseButton( "COSTUM_COLOR_OK", CBasicTextButton::SIZE14, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, COSTUM_COLOR_CANCEL );
		pButton->CreateBaseButton( "COSTUM_COLOR_CANCEL", CBasicTextButton::SIZE14, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, COSTUM_COLOR_RANDOM );
		pButton->CreateBaseButton( "COSTUM_COLOR_RANDOM_BUTTON", CBasicTextButton::SIZE14, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "COSTUM_MODAL_RANDOM_BUTTON") );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, COSTUM_COLOR_RESET );
		pButton->CreateBaseButton( "COSTUM_COLOR_RESET_BOTTON", CBasicTextButton::SIZE14, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "COSTUM_MODAL_RESET_BUTTON") );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );
	}


	CString strCostumMainSlot[COSTUM_COLOR_SLOT] = 
	{
		"COSTUM_COLOR_BODY_FOCUS_0",
	};

	CString strCostumSubSlot[COSTUM_COLOR_SLOT] = 
	{
		"COSTUM_COLOR_BODY_FOCUS_1",
	};

	for( INT i = 0; i < COSTUM_COLOR_SLOT; i++ )
	{
		m_pColorMainSlot[ i ] = CreateColorSlot ( strCostumMainSlot[ i ], COLOR_MAIN_SLOT_0 + i );
		m_pColorSubSlot[ i ] = CreateColorSlot ( strCostumSubSlot[ i ], COLOR_SUB_SLOT_0 + i );
	}
}

CBasicTextBox* CCostumColorChangeWindow::CreateStaticControl(const char* szControlKeyword, const char* szText, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );
	pStaticText->AddText( szText );

	return pStaticText;
}

CCostumColorSlot* CCostumColorChangeWindow::CreateColorSlot ( CString strKeyword, UIGUID ControlID )
{
	CCostumColorSlot* pItemSlot = new CCostumColorSlot(m_pEngineDevice);
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl();
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CBasicTextBox*	CCostumColorChangeWindow::CreateTextBox ( const CString& strKeyword, const CString& strText, const UIGUID& cID )
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox*	pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, const_cast<CString&>(strKeyword).GetString(), UI_FLAG_DEFAULT, cID );
	pTextBox->SetFont ( pFont );
	pTextBox->AddText ( strText );
	pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pTextBox );
	return pTextBox;
}

void CCostumColorChangeWindow::SetRestrict( bool bRestrict )
{
	m_bRestrict = bRestrict;

	if ( m_bRestrict )
	{
		m_fRestrictMax = RESTRICT_MAX / float( nMAX_COLOR );
		m_fRestrictMin = RESTRICT_MIN / float( nMAX_COLOR );
	}
	else
	{
		m_fRestrictMax = 1.0f;
		m_fRestrictMin = 0.0f;
	}

	m_pRProgressBarRestrict->FlowBarUpdate( 1.0f );
	m_pGProgressBarRestrict->FlowBarUpdate( 1.0f );
	m_pBProgressBarRestrict->FlowBarUpdate( 1.0f );
}

bool CCostumColorChangeWindow::CheckChangeOK()
{
	DWORD dwColor = m_pColorMainSlot[ 0 ]->GetColor();

	INT nR, nG, nB;
	nR = (dwColor&0xff0000)>>16;
	nG = (dwColor&0xff00)>>8;
	nB = dwColor&0xff;

	float fRPercent = nR / float( nMAX_COLOR );
	float fGPercent = nG / float( nMAX_COLOR );
	float fBPercent = nB / float( nMAX_COLOR );

	bool bRCheck = CheckRestrict( fRPercent );
	bool bGCheck = CheckRestrict( fGPercent );
	bool bBCheck = CheckRestrict( fBPercent );

	if ( !bRCheck || !bGCheck || !bBCheck )
		return false;

	dwColor = m_pColorSubSlot[ 0 ]->GetColor();

	nR = (dwColor&0xff0000)>>16;
	nG = (dwColor&0xff00)>>8;
	nB = dwColor&0xff;

	fRPercent = nR / float( nMAX_COLOR );
	fGPercent = nG / float( nMAX_COLOR );
	fBPercent = nB / float( nMAX_COLOR );

	bRCheck = CheckRestrict( fRPercent );
	bGCheck = CheckRestrict( fGPercent );
	bBCheck = CheckRestrict( fBPercent );

	if ( !bRCheck || !bGCheck || !bBCheck )
		return false;

	return true;
}

bool CCostumColorChangeWindow::CheckRestrict( float& fPercent )
{
	if ( fPercent < m_fRestrictMin )
	{
		fPercent = m_fRestrictMin;

		return false;
	}
	else if ( fPercent > m_fRestrictMax )
	{
		fPercent = m_fRestrictMax;

		return false;
	}

	return true;
}

void CCostumColorChangeWindow::ShowRestrictBar()
{
	m_pRProgressBarRestrict->FlowBarUpdate( m_fRestrictMax );
	m_pGProgressBarRestrict->FlowBarUpdate( m_fRestrictMax );
	m_pBProgressBarRestrict->FlowBarUpdate( m_fRestrictMax );
}

BOOL CCostumColorChangeWindow::UpdateProgressBar( CBasicProgressBar * pBar, int x )
{
	if( pBar && pBar->IsExclusiveSelfControl() )
	{
		const float fSize = pBar->GetGlobalPos ().sizeX;
		const float fPos = float(x) - pBar->GetGlobalPos ().left;
		float fPercent = fPos / fSize;

		CheckRestrict( fPercent );
		pBar->SetPercent( fPercent );

		ShowRestrictBar();

		return TRUE;
	}

	return FALSE;
}

void CCostumColorChangeWindow::SetProgressBarEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal, bool bReset /* = false */ )
{
	float fPercent = nColorVal / float( nMAX_COLOR );

	if ( IsRestrict() )
	{
		if ( bReset )
		{
			pBar->SetPercent ( fPercent );
		}
		else
		{
			CheckRestrict( fPercent );
			pBar->SetPercent( fPercent );

			nColorVal = (int)(nMAX_COLOR * fPercent);
		}

		CString strTemp;
		strTemp.Format( "%d", nColorVal );

		pEditBox->ClearEdit();
		pEditBox->SetEditString( strTemp );

		ShowRestrictBar();
	}
	else
	{
		pBar->SetPercent ( fPercent );

		CString strTemp;
		strTemp.Format( "%d", nColorVal );

		pEditBox->ClearEdit();
		pEditBox->SetEditString( strTemp );

		ShowRestrictBar();
	}

	//pBar->SetPercent( nColorVal / float( nMAX_COLOR ) );


	//CString strTemp;
	//strTemp.Format( "%d", nColorVal );

	//pEditBox->ClearEdit();
	//pEditBox->SetEditString( strTemp );
}

void CCostumColorChangeWindow::UpdateProgressBarToEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal )
{
	int nVal(0);
	float fPercent = pBar->GetPercent();

	CheckRestrict( fPercent );

	if( fPercent > 0 )	nVal = (int)(nMAX_COLOR * fPercent);

	CString strTemp;
	strTemp.Format( "%d", nVal );

	pEditBox->ClearEdit();
	pEditBox->SetEditString( strTemp );

	nColorVal = nVal;
}

BOOL CCostumColorChangeWindow::UpdateEditBoxToProgressBar( CUIEditBox * pEditBox, CBasicProgressBar * pBar, INT & nColorVal )
{
	if( pEditBox && pEditBox->IsBegin() )
	{
		const CString & strRVal = pEditBox->GetEditString();
		nColorVal = _ttoi( strRVal.GetString() );
		if( nColorVal > nMAX_COLOR )
		{
			nColorVal = nMAX_COLOR;

			pEditBox->ClearEdit();
			pEditBox->SetEditString( CString("255") );
		}

		float fPercent(0);
		if( nColorVal > 0 )	fPercent = nColorVal / float(nMAX_COLOR);

		CheckRestrict( fPercent );
		pBar->SetPercent( fPercent );

		if( nColorVal > 0 ) nColorVal = (int)(nMAX_COLOR * fPercent);

		CString strTemp;
		strTemp.Format( "%d", nColorVal );

		pEditBox->ClearEdit();
		pEditBox->SetEditString( strTemp );

		ShowRestrictBar();

		return TRUE;
	}

	return FALSE;
}

void CCostumColorChangeWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( bFirstControl )
	{
		BOOL bChange( FALSE );

		if( UpdateProgressBar( m_pRProgressBar, x ) )
		{
			UpdateProgressBarToEditBox( m_pRProgressBar, m_pREditBox, m_nR );
			bChange = TRUE;
		}
		else if( UpdateProgressBar( m_pGProgressBar, x ) )
		{
			UpdateProgressBarToEditBox( m_pGProgressBar, m_pGEditBox, m_nG );
			bChange = TRUE;
		}
		else if( UpdateProgressBar( m_pBProgressBar, x ) )
		{
			UpdateProgressBarToEditBox( m_pBProgressBar, m_pBEditBox, m_nB );
			bChange = TRUE;
		}

		if( UpdateEditBoxToProgressBar( m_pREditBox, m_pRProgressBar, m_nR ) )		bChange = TRUE;
		else if( UpdateEditBoxToProgressBar( m_pGEditBox, m_pGProgressBar, m_nG ) )	bChange = TRUE;
		else if( UpdateEditBoxToProgressBar( m_pBEditBox, m_pBProgressBar, m_nB ) )	bChange = TRUE;

		if( bChange )
		{
			RegulateRGB();
			if( m_nMainSelect >= 0 )
			{
				m_pColorMainSlot[ m_nMainSelect ]->SetColor( D3DCOLOR_ARGB( 255, m_nRegR, m_nRegG, m_nRegB ) );

				EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
				PDXCHARPART pCharPart = m_pGaeaClient->GetCharacter()->GetSkinChar()->GetPiece( SLOT_2_PIECE( emCostumSelectSlot) );
				if ( pCharPart )
				{	// Memo : overlay 효과가 안들어가있으면 염색 할 수 없다. by luxes.
					DWORD dwMainColor = 0, dwSubColor = 0;
					pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );

					if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
					{
						pCharPart->SetHairColor( m_pColorMainSlot[ m_nMainSelect ]->GetColor() );
					}
				}
			}

			if( m_nSubSelect >= 0 )
			{
				m_pColorSubSlot[ m_nSubSelect ]->SetColor( D3DCOLOR_ARGB( 255, m_nRegR, m_nRegG, m_nRegB ) );

				EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
				PDXCHARPART pCharPart = m_pGaeaClient->GetCharacter()->GetSkinChar()->GetPiece( SLOT_2_PIECE( emCostumSelectSlot) );
				if ( pCharPart )
				{	// Memo : overlay 효과가 안들어가있으면 염색 할 수 없다. by luxes.
					DWORD dwMainColor = 0, dwSubColor = 0;
					pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );

					if( dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
					{
						pCharPart->SetSubColor( m_pColorSubSlot[ m_nSubSelect ]->GetColor() );
					}
				}
			}
			//WORD wTemp = m_nR << 10	| m_nG << 5 | m_nB;
			//m_pGaeaClient->GetCharacter()->HairColorChange( wTemp );
		}
	}
}

void CCostumColorChangeWindow::TranslateProgressBarMsg( CBasicProgressBar * pBar, DWORD dwMsg )
{
	if( !pBar ) return;

	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
		{			
			pBar->SetExclusiveControl();
		}
		else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
		{
			pBar->ResetExclusiveControl();
		}
	}
	else if ( CHECK_LB_UP_LIKE ( dwMsg ) )
	{									
		pBar->ResetExclusiveControl();
	}
}

void CCostumColorChangeWindow::TranslateEditBoxMsg( CUIEditBox * pEditBox, DWORD dwMsg )
{
	if( !pEditBox ) return;

	if( CHECK_MOUSE_IN_LBDOWNLIKE( dwMsg ) )
	{
		pEditBox->BeginEdit();
	}
	else if( CHECK_LB_DOWN_LIKE( dwMsg ) )
	{
		pEditBox->EndEdit();
	}
}

void CCostumColorChangeWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	if( COLOR_MAIN_SLOT_0 == cID )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			if ( dwMsg & UIMSG_LB_UP )
			{
				if( m_nMainSelect >= 0 )
				{
					m_pColorMainSlot[ m_nMainSelect ]->SetSelect( FALSE );
				}

				if( m_nSubSelect >= 0 )
				{
					m_pColorSubSlot[ m_nSubSelect ]->SetSelect( FALSE );
				}				
				m_nMainSelect = cID - COLOR_MAIN_SLOT_0;
				m_nSubSelect = -1;
				m_pColorMainSlot[ m_nMainSelect ]->SetSelect( TRUE );

				DWORD dwColor = m_pColorMainSlot[ m_nMainSelect ]->GetColor();
				m_nR = (dwColor&0xff0000)>>16;
				m_nG = (dwColor&0xff00)>>8;
				m_nB = dwColor&0xff;

				m_pColorMainSlot[ m_nMainSelect ]->SetColor( D3DCOLOR_ARGB( 255, m_nR, m_nG, m_nB ) );

				SetProgressBarEditBox( m_pRProgressBar, m_pREditBox, m_nR );
				SetProgressBarEditBox( m_pGProgressBar, m_pGEditBox, m_nG );
				SetProgressBarEditBox( m_pBProgressBar, m_pBEditBox, m_nB );
			}
		}
	}

	if( COLOR_SUB_SLOT_0 == cID )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			if ( dwMsg & UIMSG_LB_UP )
			{
				if( m_nMainSelect >= 0 )
				{
					m_pColorMainSlot[ m_nMainSelect ]->SetSelect( FALSE );
				}

				if( m_nSubSelect >= 0 )
				{
					m_pColorSubSlot[ m_nSubSelect ]->SetSelect( FALSE );
				}				
				m_nMainSelect = -1;
				m_nSubSelect = cID - COLOR_SUB_SLOT_0;
				m_pColorSubSlot[ m_nSubSelect ]->SetSelect( TRUE );

				DWORD dwColor = m_pColorSubSlot[ m_nSubSelect ]->GetColor();
				m_nR = (dwColor&0xff0000)>>16;
				m_nG = (dwColor&0xff00)>>8;
				m_nB = dwColor&0xff;

				m_pColorSubSlot[ m_nSubSelect ]->SetColor( D3DCOLOR_ARGB( 255, m_nR, m_nG, m_nB ) );

				SetProgressBarEditBox( m_pRProgressBar, m_pREditBox, m_nR );
				SetProgressBarEditBox( m_pGProgressBar, m_pGEditBox, m_nG );
				SetProgressBarEditBox( m_pBProgressBar, m_pBEditBox, m_nB );
			}
		}
	}

	switch ( cID )
	{
	case COSTUM_COLOR_R_PROGRESS: TranslateProgressBarMsg( m_pRProgressBar, dwMsg ); break;
	case COSTUM_COLOR_G_PROGRESS: TranslateProgressBarMsg( m_pGProgressBar, dwMsg ); break;
	case COSTUM_COLOR_B_PROGRESS: TranslateProgressBarMsg( m_pBProgressBar, dwMsg ); break;

	case COSTUM_COLOR_R_EDITBOX: TranslateEditBoxMsg( m_pREditBox, dwMsg ); break;
	case COSTUM_COLOR_G_EDITBOX: TranslateEditBoxMsg( m_pGEditBox, dwMsg ); break;
	case COSTUM_COLOR_B_EDITBOX: TranslateEditBoxMsg( m_pBEditBox, dwMsg ); break;

	case COSTUM_COLOR_RANDOM:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( m_nMainSelect >= 0 )
					{
						m_nR = (INT)rand() % 255;
						m_nG = (INT)rand() % 255;
						m_nB = (INT)rand() % 255;

						float fPercent = m_nR / 255.0f;
						CheckRestrict( fPercent );
						m_nR = (int)(fPercent * nMAX_COLOR);

						fPercent = m_nG / 255.0f;
						CheckRestrict( fPercent );
						m_nG = (int)(fPercent * nMAX_COLOR);

						fPercent = m_nB / 255.0f;
						CheckRestrict( fPercent );
						m_nB = (int)(fPercent * nMAX_COLOR);

						m_pColorMainSlot[ 0 ]->SetColor( D3DCOLOR_ARGB( 255, m_nR, m_nG, m_nB ) );

						INT nR = (INT)rand() % 255;
						INT nG = (INT)rand() % 255;
						INT nB = (INT)rand() % 255;

						fPercent = nR / 255.0f;
						CheckRestrict( fPercent );
						nR = (int)(fPercent * nMAX_COLOR);

						fPercent = nG / 255.0f;
						CheckRestrict( fPercent );
						nG = (int)(fPercent * nMAX_COLOR);

						fPercent = nB / 255.0f;
						CheckRestrict( fPercent );
						nB = (int)(fPercent * nMAX_COLOR);

						m_pColorSubSlot[ 0 ]->SetColor( D3DCOLOR_ARGB( 255, nR, nG, nB ) );

						SetProgressBarEditBox( m_pRProgressBar, m_pREditBox, m_nR );
						SetProgressBarEditBox( m_pGProgressBar, m_pGEditBox, m_nG );
						SetProgressBarEditBox( m_pBProgressBar, m_pBEditBox, m_nB );
					}
					else if ( m_nSubSelect >= 0 )
					{
						INT nR = (INT)rand() % 255;
						INT nG = (INT)rand() % 255;
						INT nB = (INT)rand() % 255;

						float fPercent = nR / 255.0f;
						CheckRestrict( fPercent );
						nR = (int)(fPercent * nMAX_COLOR);

						fPercent = nG / 255.0f;
						CheckRestrict( fPercent );
						nG = (int)(fPercent * nMAX_COLOR);

						fPercent = nB / 255.0f;
						CheckRestrict( fPercent );
						nB = (int)(fPercent * nMAX_COLOR);

						m_pColorMainSlot[ 0 ]->SetColor( D3DCOLOR_ARGB( 255, nR, nG, nB ) );

						m_nR = (INT)rand() % 255;
						m_nG = (INT)rand() % 255;
						m_nB = (INT)rand() % 255;

						fPercent = m_nR / 255.0f;
						CheckRestrict( fPercent );
						m_nR = (int)(fPercent * nMAX_COLOR);

						fPercent = m_nG / 255.0f;
						CheckRestrict( fPercent );
						m_nG = (int)(fPercent * nMAX_COLOR);

						fPercent = m_nB / 255.0f;
						CheckRestrict( fPercent );
						m_nB = (int)(fPercent * nMAX_COLOR);

						m_pColorSubSlot[ 0 ]->SetColor( D3DCOLOR_ARGB( 255, m_nR, m_nG, m_nB ) );

						SetProgressBarEditBox( m_pRProgressBar, m_pREditBox, m_nR );
						SetProgressBarEditBox( m_pGProgressBar, m_pGEditBox, m_nG );
						SetProgressBarEditBox( m_pBProgressBar, m_pBEditBox, m_nB );
					}

					{
						RegulateRGB();
						
						EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
						PDXCHARPART pCharPart = m_pGaeaClient->GetCharacter()->GetSkinChar()->GetPiece( SLOT_2_PIECE( emCostumSelectSlot ) );
						if ( pCharPart )
						{	// Memo : overlay 효과가 안들어가있으면 염색 할 수 없다. by luxes.
							DWORD dwMainColor = 0, dwSubColor = 0;
							pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );

							if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
							{
								pCharPart->SetHairColor( m_pColorMainSlot[ 0 ]->GetColor() );
							}

							if( dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
							{
								pCharPart->SetSubColor( m_pColorSubSlot[ 0 ]->GetColor() );
							}
						}
					}
				}
			}
		}
		break;

	case COSTUM_COLOR_RESET:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_UP )
				{
					m_pGaeaClient->GetCharacter()->UpdateSuit();

					SetDefaultColorUIControl();

					EMSLOT emSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();

					if ( m_pGaeaClient->GetCharacter()->m_PutOnItems[ emSlot ].GetNativeID() != NATIVEID_NULL() )
					{
						if ( m_pGaeaClient->GetCharacter()->m_PutOnItems[ emSlot ].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
							&& m_pGaeaClient->GetCharacter()->m_PutOnItems[ emSlot ].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
						{
							DWORD dwMainColor = m_pGaeaClient->GetCharacter()->m_PutOnItems[ emSlot ].dwMainColor;
							DWORD dwSubColor = m_pGaeaClient->GetCharacter()->m_PutOnItems[ emSlot ].dwSubColor;

							int	R,G,B;
							R = (dwMainColor&0xff0000)>>16;
							G = (dwMainColor&0xff00)>>8;
							B = dwMainColor&0xff;

							m_pColorMainSlot[ 0 ]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );

							R = (dwSubColor&0xff0000)>>16;
							G = (dwSubColor&0xff00)>>8;
							B = dwSubColor&0xff;

							m_pColorSubSlot[ 0 ]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );
						}
					}

					if ( m_nMainSelect >= 0 )
					{
						DWORD dwColor = m_pColorMainSlot[ 0 ]->GetColor();
						m_nR = (dwColor&0xff0000)>>16;
						m_nG = (dwColor&0xff00)>>8;
						m_nB = dwColor&0xff;

						SetProgressBarEditBox( m_pRProgressBar, m_pREditBox, m_nR, true );
						SetProgressBarEditBox( m_pGProgressBar, m_pGEditBox, m_nG, true );
						SetProgressBarEditBox( m_pBProgressBar, m_pBEditBox, m_nB, true );
					}
					else if ( m_nSubSelect >= 0 )
					{
						DWORD dwColor = m_pColorSubSlot[ 0 ]->GetColor();
						m_nR = (dwColor&0xff0000)>>16;
						m_nG = (dwColor&0xff00)>>8;
						m_nB = dwColor&0xff;

						SetProgressBarEditBox( m_pRProgressBar, m_pREditBox, m_nR, true );
						SetProgressBarEditBox( m_pGProgressBar, m_pGEditBox, m_nG, true );
						SetProgressBarEditBox( m_pBProgressBar, m_pBEditBox, m_nB, true );
					}

					{
						RegulateRGB();

						EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
						PDXCHARPART pCharPart = m_pGaeaClient->GetCharacter()->GetSkinChar()->GetPiece( SLOT_2_PIECE( emCostumSelectSlot ) );
						if ( pCharPart )
						{	// Memo : overlay 효과가 안들어가있으면 염색 할 수 없다. by luxes.
							DWORD dwMainColor = 0, dwSubColor = 0;
							pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );

							if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
							{
								pCharPart->SetHairColor( m_pColorMainSlot[ 0 ]->GetColor() );
							}

							if( dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
							{
								pCharPart->SetSubColor( m_pColorSubSlot[ 0 ]->GetColor() );
							}
						}
					}
					m_pRProgressBarRestrict->FlowBarUpdate( 1.0f );
					m_pGProgressBarRestrict->FlowBarUpdate( 1.0f );
					m_pBProgressBarRestrict->FlowBarUpdate( 1.0f );
				}
			}
		}
		break;

	case COSTUM_COLOR_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				if ( m_bRestrict )
				{
					bool bCheckOK = CheckChangeOK();

					if ( bCheckOK )
					{
						m_bOKButton = TRUE;
						m_pInterface->UiHideGroup( GetWndID() );
					}
					else
					{
						int nMin = (int)(m_fRestrictMin * 255.0f);
						int nMax = (int)(m_fRestrictMax * 255.0f);
						std::string strCombine = sc::string::format( ID2GAMEINTEXT( "COSTUM_RESTRICT_ERROR" ), nMin, nMax ).c_str();
						m_pInterface->DoModal( strCombine, UI::MODAL_ERROR, UI::OK );
					}
				}
				else
				{
					m_bOKButton = TRUE;
					m_pInterface->UiHideGroup( GetWndID() );
				}
			}
		}
		break;

	case COSTUM_COLOR_CANCEL:
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;
	}
}

void CCostumColorChangeWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		if( IsSetColor() )
		{
			SetDefaultColorUIControl();

		}
		else
		{
			SetColorUIControl();
		}


		InitUIControl( TRUE , 0 );
		InitSelect();
	}
	else
	{
		if( m_bOKButton )
		{
			DWORD dwMainColor;
			DWORD dwSubColor;

			dwMainColor = m_pColorMainSlot[ 0 ]->GetColor();
			dwSubColor = m_pColorSubSlot[ 0 ]->GetColor();

			m_pGaeaClient->GetCharacter()->ReqInvenCostumColorChange( dwMainColor, dwSubColor );
			m_bOKButton = FALSE;
		}
		else
		{
			m_pGaeaClient->GetCharacter()->UpdateSuit();
		}

		ClearEditBox();
	}
}

BOOL CCostumColorChangeWindow::IsSetColor()
{
	for( INT i = 0; i < COSTUM_COLOR_SLOT; i++ )
	{
		EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
		DxSkinChar* pSkinChar = m_pGaeaClient->GetCharacter()->GetSkinChar();
		if (!pSkinChar )	return FALSE;

		PDXCHARPART pCharPart = pSkinChar->GetPiece( SLOT_2_PIECE( emCostumSelectSlot) );
		if ( !pCharPart )	return FALSE;

		DWORD dwColor = pCharPart->GetHairColor();
		if( dwColor != 0 )
			return FALSE;
	}

	for( INT i = 0; i < COSTUM_COLOR_SLOT; i++ )
	{
		EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
		DxSkinChar* pSkinChar = m_pGaeaClient->GetCharacter()->GetSkinChar();
		if (!pSkinChar )	return FALSE;

		PDXCHARPART pCharPart = pSkinChar->GetPiece( SLOT_2_PIECE( emCostumSelectSlot) );
		if ( !pCharPart )	return FALSE;

		DWORD dwColor = pCharPart->GetSubColor();
		if( dwColor != 0 )
			return FALSE;
	}
	return TRUE;
}

void CCostumColorChangeWindow::SetDefaultColorUIControl()
{
	DxSkinChar* pSkinChar = m_pGaeaClient->GetCharacter()->GetSkinChar();
	if( !pSkinChar )
		return;

	EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
	DWORD dwMainColor = 0, dwSubColor = 0;
	for ( INT i = 0; i < COSTUM_COLOR_SLOT; i++ )
	{
		PDXCHARPART pCharPart = pSkinChar->GetPiece( SLOT_2_PIECE( emCostumSelectSlot ) );
		if( pCharPart )
		{
			pCharPart->GetDefaultOverlayColor( dwMainColor , dwSubColor );

			if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
				dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
			{
				int	R,G,B;
				R = (dwMainColor&0xff0000)>>16;
				G = (dwMainColor&0xff00)>>8;
				B = dwMainColor&0xff;

				m_pColorMainSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );

				R = (dwSubColor&0xff0000)>>16;
				G = (dwSubColor&0xff00)>>8;
				B = dwSubColor&0xff;

				m_pColorSubSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );
			}
			else
			{
				m_pColorMainSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
				m_pColorSubSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
			}
		}
		else
		{
			m_pColorMainSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
			m_pColorSubSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
		}
	}
}

void CCostumColorChangeWindow::SetColorUIControl()
{
	DxSkinChar* pSkinChar = m_pGaeaClient->GetCharacter()->GetSkinChar();
	if (!pSkinChar )	return;

	for( INT i = 0; i < COSTUM_COLOR_SLOT; i++ )
	{
		EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
		PDXCHARPART pCharPart = pSkinChar->GetPiece( SLOT_2_PIECE( emCostumSelectSlot) );
		if ( pCharPart )
		{
			DWORD dwColor = pCharPart->GetHairColor();
			int	R,G,B;
			R = (dwColor&0xff0000)>>16;
			G = (dwColor&0xff00)>>8;
			B = dwColor&0xff;

			//m_pColorMainSlot[ i ]->SetDiffuse( D3DCOLOR_ARGB( 255, m_nRegR, m_nRegG, m_nRegB ) );
			m_pColorMainSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );
		}
	}

	for( INT i = 0; i < COSTUM_COLOR_SLOT; i++ )
	{
		EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
		PDXCHARPART pCharPart = pSkinChar->GetPiece( SLOT_2_PIECE( emCostumSelectSlot) );
		if ( pCharPart )
		{
			DWORD dwColor = pCharPart->GetSubColor();
			int	R,G,B;
			R = (dwColor&0xff0000)>>16;
			G = (dwColor&0xff00)>>8;
			B = dwColor&0xff;

			m_pColorSubSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );
		}
	}
}

void CCostumColorChangeWindow::InitUIControl( BOOL bMain, INT nIndex )
{
	if( bMain )
	{
		m_nMainSelect = nIndex;
		m_nSubSelect = -1;
	}
	else
	{
		m_nMainSelect = -1;
		m_nSubSelect = nIndex;
	}
	D3DCOLOR wColor;

	wColor = m_pColorMainSlot[ m_nMainSelect ]->GetColor();

	int	R,G,B;
	R = (wColor&0xff0000)>>16;
	G = (wColor&0xff00)>>8;
	B = wColor&0xff;


	// Note : Hair Color Data 구조
	// |  X |       RED      |   GREEN   |    BLUE   |
	// | 15 | 14 13 12 11 10 | 9 8 7 6 5 | 4 3 2 1 0 | 
	m_nRBack = m_nR = R;
	m_nGBack = m_nG = G;
	m_nBBack = m_nB = B;

	CString strTemp;

	strTemp.Format( "%d", m_nR );
	m_pREditBox->SetEditString( strTemp );

	strTemp.Format( "%d", m_nG );
	m_pGEditBox->SetEditString( strTemp );

	strTemp.Format( "%d", m_nB );
	m_pBEditBox->SetEditString( strTemp );

	float fPercent(0);
	if( m_nR > 0 )	fPercent = m_nR / float(nMAX_COLOR);
	m_pRProgressBar->SetPercent( fPercent );

	fPercent = 0;
	if( m_nG > 0 )	fPercent = m_nG / float(nMAX_COLOR);
	m_pGProgressBar->SetPercent( fPercent );

	fPercent = 0;
	if( m_nB > 0 )	fPercent = m_nB / float(nMAX_COLOR);
	m_pBProgressBar->SetPercent( fPercent );

	RegulateRGB();
}

void CCostumColorChangeWindow::InitSelect()
{
	for (INT i = 0; i < COSTUM_COLOR_SLOT; i++ )
	{
		m_pColorMainSlot[ i ]->SetSelect( FALSE );
		m_pColorSubSlot[ i ]->SetSelect( FALSE );
	}

	m_nMainSelect = 0;
	m_nSubSelect = -1;
	m_pColorMainSlot[ m_nMainSelect ]->SetSelect( TRUE );	
}

void CCostumColorChangeWindow::RegulateRGB()
{
	m_nRegR = m_nR;
	m_nRegG = m_nG;
	m_nRegB = m_nB;
}

void CCostumColorChangeWindow::ClearEditBox()
{
	m_pREditBox->ClearEdit();
	m_pREditBox->EndEdit();

	m_pGEditBox->ClearEdit();
	m_pGEditBox->EndEdit();

	m_pBEditBox->ClearEdit();
	m_pBEditBox->EndEdit();
}