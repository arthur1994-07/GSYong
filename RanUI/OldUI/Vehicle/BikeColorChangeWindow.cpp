#include "StdAfx.h"
#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../../EngineLib/DxMeshs/SkinMesh/DxVehicleData.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "BikeColorChangeWindow.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "BikeColorSlot.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CBikeColorChangeWindow::nLIMIT_NUMBER_CHAR = 3;
const int CBikeColorChangeWindow::nMAX_COLOR = 255;
const float CBikeColorChangeWindow::fRGB_RATIO = 8.225806f;

CBikeColorChangeWindow::CBikeColorChangeWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindowEx(pInterface, pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_nMainSelect( -1 )
    , m_nSubSelect( -1 )
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
{
}


CBikeColorChangeWindow::~CBikeColorChangeWindow ()
{
}

void CBikeColorChangeWindow::CreateSubControl ()
{
	CD3DFontPar * pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9 );

	CreateControl( "BIKE_COLOR_TITLE_BACK_L" );
	CreateControl( "BIKE_COLOR_TITLE_BACK_R" );
	CreateStaticControl( "BIKE_COLOR_TITLE", ID2GAMEINTEXT("BIKE_COLOR_TITLE"), pFont, TEXT_ALIGN_CENTER_X );
	
	CreateStaticControl( "BIKE_COLOR_IN_TEXT", ID2GAMEINTEXT("BIKE_COLOR_IN_TEXT"), pFont, TEXT_ALIGN_CENTER_X );
	CreateStaticControl( "BIKE_COLOR_OUT_TEXT", ID2GAMEINTEXT("BIKE_COLOR_OUT_TEXT"), pFont, TEXT_ALIGN_CENTER_X );

	CreateStaticControl( "BIKE_COLOR_BODY_TEXT", ID2GAMEINTEXT("BIKE_COLOR_BODY_TEXT"), pFont, TEXT_ALIGN_CENTER_X );
	CreateStaticControl( "BIKE_COLOR_HEAD_TEXT", ID2GAMEINTEXT("BIKE_COLOR_HEAD_TEXT"), pFont, TEXT_ALIGN_CENTER_X );
	CreateStaticControl( "BIKE_COLOR_TIRE_TEXT", ID2GAMEINTEXT("BIKE_COLOR_TIRE_TEXT"), pFont, TEXT_ALIGN_CENTER_X );
	CreateStaticControl( "BIKE_COLOR_MUFFLER_TEXT", ID2GAMEINTEXT("BIKE_COLOR_MUFFLER_TEXT"), pFont, TEXT_ALIGN_CENTER_X );
	CreateStaticControl( "BIKE_COLOR_SUB_SEAT_TEXT", ID2GAMEINTEXT("BIKE_COLOR_SUB_SEAT_TEXT"), pFont, TEXT_ALIGN_CENTER_X );
	CreateStaticControl( "BIKE_COLOR_ACC_TEXT", ID2GAMEINTEXT("BIKE_COLOR_SUB_ACC_TEXT"), pFont, TEXT_ALIGN_CENTER_X );

	{ // Note : R,G,B Static
		CUIControl * pUIControl = new CUIControl(m_pEngineDevice);
		pUIControl->CreateSub( this, "BIKE_COLOR_R" );
		RegisterControl( pUIControl );

		pUIControl = new CUIControl(m_pEngineDevice);
		pUIControl->CreateSub( this, "BIKE_COLOR_G" );
		RegisterControl( pUIControl );

		pUIControl = new CUIControl(m_pEngineDevice);
		pUIControl->CreateSub( this, "BIKE_COLOR_B" );
		RegisterControl( pUIControl );
	}

	{	// Note : 프로그래스바
		CBasicProgressBar* pProgressBar( NULL );
		CUIControl*	pDummyBar( NULL );

		pProgressBar = new CBasicProgressBar(m_pEngineDevice);
		pProgressBar->CreateSub ( this, "BIKE_COLOR_R_PROGRESS" );
		pProgressBar->CreateOverImage ( "BIKE_COLOR_R_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pRProgressBar = pProgressBar;

		pDummyBar = new CUIControl(m_pEngineDevice);
		pDummyBar->CreateSub ( this, "BIKE_COLOR_R_PROGRESS_DUMMY", UI_FLAG_DEFAULT, BIKE_COLOR_R_PROGRESS );
		RegisterControl ( pDummyBar );

		pProgressBar = new CBasicProgressBar(m_pEngineDevice);
		pProgressBar->CreateSub ( this, "BIKE_COLOR_G_PROGRESS" );
		pProgressBar->CreateOverImage ( "BIKE_COLOR_G_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pGProgressBar = pProgressBar;

		pDummyBar = new CUIControl(m_pEngineDevice);
		pDummyBar->CreateSub ( this, "BIKE_COLOR_G_PROGRESS_DUMMY", UI_FLAG_DEFAULT, BIKE_COLOR_G_PROGRESS );
		RegisterControl ( pDummyBar );

		pProgressBar = new CBasicProgressBar(m_pEngineDevice);
		pProgressBar->CreateSub ( this, "BIKE_COLOR_B_PROGRESS" );
		pProgressBar->CreateOverImage ( "BIKE_COLOR_B_PROGRESS_OVERIMAGE" );
		RegisterControl ( pProgressBar );
		m_pBProgressBar = pProgressBar;		

		pDummyBar = new CUIControl(m_pEngineDevice);
		pDummyBar->CreateSub ( this, "BIKE_COLOR_B_PROGRESS_DUMMY", UI_FLAG_DEFAULT, BIKE_COLOR_B_PROGRESS );
		RegisterControl ( pDummyBar );
	}

	{ // Note : 입력 에디터 박스
		CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( "BIKE_COLOR_R_EDITBOX_BACK" );
		RegisterControl( pBasicLineBox );

		CUIEditBox* pEditBox = new CUIEditBox(m_pEngineDevice);
		pEditBox->CreateSub( this, "BIKE_COLOR_R_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_RIGHT, BIKE_COLOR_R_EDITBOX );
		pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
		pEditBox->SetFont( pFont );
		pEditBox->DoUSE_NUMBER( true );
		RegisterControl( pEditBox );
		m_pREditBox = pEditBox;

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( "BIKE_COLOR_G_EDITBOX_BACK" );
		RegisterControl( pBasicLineBox );

		pEditBox = new CUIEditBox(m_pEngineDevice);
		pEditBox->CreateSub( this, "BIKE_COLOR_G_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_RIGHT, BIKE_COLOR_G_EDITBOX );
		pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
		pEditBox->SetFont( pFont );	
		pEditBox->DoUSE_NUMBER( true );
		RegisterControl( pEditBox );
		m_pGEditBox = pEditBox;

		pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
		pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox( "BIKE_COLOR_B_EDITBOX_BACK" );
		RegisterControl( pBasicLineBox );

		pEditBox = new CUIEditBox(m_pEngineDevice);
		pEditBox->CreateSub( this, "BIKE_COLOR_B_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_RIGHT, BIKE_COLOR_B_EDITBOX );
		pEditBox->CreateCarrat( "MODAL_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_NUMBER_CHAR );
		pEditBox->SetFont( pFont );
		pEditBox->DoUSE_NUMBER( true );
		RegisterControl( pEditBox );
		m_pBEditBox = pEditBox;
	}

	{ // Note : 버튼
		CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, BIKE_COLOR_OK );
		pButton->CreateBaseButton( "BIKE_COLOR_OK", CBasicTextButton::SIZE14, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 0 ) );
		pButton->SetShortcutKey( DIK_RETURN, DIK_NUMPADENTER );
		RegisterControl( pButton );

		pButton = new CBasicTextButton(m_pEngineDevice);
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, BIKE_COLOR_CANCEL );
		pButton->CreateBaseButton( "BIKE_COLOR_CANCEL", CBasicTextButton::SIZE14, 
			CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD( "MODAL_BUTTON", 1 ) );
		pButton->SetShortcutKey( DIK_ESCAPE );
		RegisterControl( pButton );
	}


	CString strBikeMainSlot[BIKE_COLOR_SLOT] = 
	{
		"BIKE_COLOR_BODY_FOCUS_0",
		"BIKE_COLOR_BODY_FOCUS_2",		
		"BIKE_COLOR_BODY_FOCUS_4",
		"BIKE_COLOR_BODY_FOCUS_6",
		"BIKE_COLOR_BODY_FOCUS_8",
		"BIKE_COLOR_BODY_FOCUS_10",
	};

	CString strBikeSubSlot[BIKE_COLOR_SLOT] = 
	{
		"BIKE_COLOR_BODY_FOCUS_1",
		"BIKE_COLOR_BODY_FOCUS_3",
		"BIKE_COLOR_BODY_FOCUS_5",
		"BIKE_COLOR_BODY_FOCUS_7",
		"BIKE_COLOR_BODY_FOCUS_9",
		"BIKE_COLOR_BODY_FOCUS_11",

	};

	for( INT i = 0; i < BIKE_COLOR_SLOT; i++ )
	{
		m_pColorMainSlot[ i ] = CreateColorSlot ( strBikeMainSlot[ i ], COLOR_MAIN_SLOT_0 + i );
		m_pColorSubSlot[ i ] = CreateColorSlot ( strBikeSubSlot[ i ], COLOR_SUB_SLOT_0 + i );
	}
}

CBasicTextBox* CBikeColorChangeWindow::CreateStaticControl(const char* szControlKeyword, const char* szText, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );
	pStaticText->AddText( szText );

	return pStaticText;
}

CBikeColorSlot* CBikeColorChangeWindow::CreateColorSlot ( CString strKeyword, UIGUID ControlID )
{
	CBikeColorSlot* pItemSlot = new CBikeColorSlot(m_pEngineDevice);
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl();
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CBasicTextBox*	CBikeColorChangeWindow::CreateTextBox ( const CString& strKeyword, const CString& strText, const UIGUID& cID )
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

BOOL CBikeColorChangeWindow::UpdateProgressBar( CBasicProgressBar * pBar, int x )
{
	if( pBar && pBar->IsExclusiveSelfControl() )
	{
		const float fSize = pBar->GetGlobalPos ().sizeX;
		const float fPos = float(x) - pBar->GetGlobalPos ().left;
		const float fPercent = fPos / fSize;
		pBar->SetPercent ( fPercent );

		return TRUE;
	}

	return FALSE;
}

void CBikeColorChangeWindow::SetProgressBarEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal )
{
	pBar->SetPercent( nColorVal / float( nMAX_COLOR ) );


	CString strTemp;
	strTemp.Format( "%d", nColorVal );

	pEditBox->ClearEdit();
	pEditBox->SetEditString( strTemp );
}

void CBikeColorChangeWindow::UpdateProgressBarToEditBox( CBasicProgressBar * pBar, CUIEditBox * pEditBox, INT & nColorVal )
{
	int nVal(0);
	float fPercent = pBar->GetPercent();
	if( fPercent > 0 )	nVal = (int)(nMAX_COLOR * fPercent);

	CString strTemp;
	strTemp.Format( "%d", nVal );

	pEditBox->ClearEdit();
	pEditBox->SetEditString( strTemp );

	nColorVal = nVal;
}

BOOL CBikeColorChangeWindow::UpdateEditBoxToProgressBar( CUIEditBox * pEditBox, CBasicProgressBar * pBar, INT & nColorVal )
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
		pBar->SetPercent( fPercent );

		return TRUE;
	}

	return FALSE;
}

void CBikeColorChangeWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
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

		if( UpdateEditBoxToProgressBar( m_pREditBox, m_pRProgressBar, m_nR ) )
            bChange = TRUE;
		else if( UpdateEditBoxToProgressBar( m_pGEditBox, m_pGProgressBar, m_nG ) )
            bChange = TRUE;
		else if( UpdateEditBoxToProgressBar( m_pBEditBox, m_pBProgressBar, m_nB ) )
            bChange = TRUE;

		if( bChange )
		{
			RegulateRGB();
            GLCharacter* pChar = m_pGaeaClient->GetCharacter();
			if( m_nMainSelect >= 0 )
			{
				m_pColorMainSlot[ m_nMainSelect ]->SetColor( D3DCOLOR_ARGB( 255, m_nRegR, m_nRegG, m_nRegB ) );

                DxVehicle* pSkinChar = pChar->VehicleSkinChar();
				DxCharPart* pSkinPiece = pSkinChar->GetVehiclePart( m_nMainSelect + 1 );
				if( pSkinPiece )
				{
					if( pChar->VehicleState() == EMVEHICLE_DRIVER )
						pSkinPiece->SetHairColor( m_pColorMainSlot[ m_nMainSelect ]->GetColor() );
				}
			}

			if( m_nSubSelect >= 0 )
			{
				m_pColorSubSlot[ m_nSubSelect ]->SetColor( D3DCOLOR_ARGB( 255, m_nRegR, m_nRegG, m_nRegB ) );

                DxVehicle* pSkinChar = pChar->VehicleSkinChar();
				DxCharPart* pSkinPiece = pSkinChar->GetVehiclePart(m_nSubSelect+1);
				if (pSkinPiece)
				{
					if (pChar->VehicleState() == EMVEHICLE_DRIVER)
						pSkinPiece->SetSubColor(m_pColorSubSlot[m_nSubSelect]->GetColor());
				}
			}
			//WORD wTemp = m_nR << 10	| m_nG << 5 | m_nB;
			//m_pGaeaClient->GetCharacter()->HairColorChange( wTemp );
		}
	}
}

void CBikeColorChangeWindow::TranslateProgressBarMsg( CBasicProgressBar * pBar, DWORD dwMsg )
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

void CBikeColorChangeWindow::TranslateEditBoxMsg( CUIEditBox * pEditBox, DWORD dwMsg )
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

void CBikeColorChangeWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );
	
	if( COLOR_MAIN_SLOT_0 <= cID && cID <= COLOR_MAIN_SLOT_5 )
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

	if( COLOR_SUB_SLOT_0 <= cID && cID <= COLOR_SUB_SLOT_5 )
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
	case BIKE_COLOR_R_PROGRESS: TranslateProgressBarMsg( m_pRProgressBar, dwMsg ); break;
	case BIKE_COLOR_G_PROGRESS: TranslateProgressBarMsg( m_pGProgressBar, dwMsg ); break;
	case BIKE_COLOR_B_PROGRESS: TranslateProgressBarMsg( m_pBProgressBar, dwMsg ); break;

	case BIKE_COLOR_R_EDITBOX: TranslateEditBoxMsg( m_pREditBox, dwMsg ); break;
	case BIKE_COLOR_G_EDITBOX: TranslateEditBoxMsg( m_pGEditBox, dwMsg ); break;
	case BIKE_COLOR_B_EDITBOX: TranslateEditBoxMsg( m_pBEditBox, dwMsg ); break;

	case BIKE_COLOR_OK:
		{
			if( CHECK_KEYFOCUSED( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				m_bOKButton = TRUE;
				m_pInterface->UiHideGroup( GetWndID() );
			}
		}
		break;

	case BIKE_COLOR_CANCEL:
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

void CBikeColorChangeWindow::SetVisibleSingle ( BOOL bVisible )
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
			DWORD dwMainColor[ ACCE_TYPE_SIZE ];
			DWORD dwSubColor[ ACCE_TYPE_SIZE ];
			for( INT i = 1; i < BIKE_COLOR_SLOT + 1; i++ )
			{
               dwMainColor[ i ] = m_pColorMainSlot[ i - 1 ]->GetColor();
			   dwSubColor[ i ] = m_pColorSubSlot[ i - 1 ]->GetColor();
			}
			//이 UI 사용 하지 않음, 컬러변경은 CUIColorSelectorWindow 사용함;
			//m_pGaeaClient->GetCharacter()->ReqInvenBikeColorChange( dwMainColor, dwSubColor );

			m_bOKButton = FALSE;
		}
		else
		{
			m_pGaeaClient->GetCharacter()->UpdateSuit();
		}

		ClearEditBox();
	}
}

BOOL CBikeColorChangeWindow::IsSetColor()
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	for (INT i=0; i<BIKE_COLOR_SLOT; ++i)
	{
		DWORD dwColor = pChar->VehicleColorMain(VEHICLE_ACCESSORYTYPE(i+1));
		if (dwColor != 0)
			return FALSE;
	}

	for (INT i=0; i<BIKE_COLOR_SLOT; ++i)
	{
		DWORD dwColor = pChar->VehicleColorSub(VEHICLE_ACCESSORYTYPE(i+1));
		if (dwColor != 0)
			return FALSE;
	}
	return TRUE;
}

void CBikeColorChangeWindow::SetDefaultColorUIControl()
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	DxVehicle* pVehicle = pChar->VehicleSkinChar();
	if (!pVehicle)
		return;

	DWORD dwMainColor = 0, dwSubColor = 0;
	for ( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++ )
	{
		DxCharPart* pSkinPiece = pVehicle->GetVehiclePart( i );
		if( pSkinPiece )
		{
			pSkinPiece->GetDefaultOverlayColor( dwMainColor , dwSubColor );

			if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
				dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
			{
				int	R,G,B;
				R = (dwMainColor&0xff0000)>>16;
				G = (dwMainColor&0xff00)>>8;
				B = dwMainColor&0xff;

				m_pColorMainSlot[ i - 1 ]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );

				R = (dwSubColor&0xff0000)>>16;
				G = (dwSubColor&0xff00)>>8;
				B = dwSubColor&0xff;

				m_pColorSubSlot[ i - 1]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );
			}
			else
			{
				m_pColorMainSlot[ i - 1 ]->SetColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
				m_pColorSubSlot[ i - 1]->SetColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
			}
		}
		else
		{
			m_pColorMainSlot[ i - 1 ]->SetColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
			m_pColorSubSlot[ i - 1]->SetColor( D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
		}
	}
}

void CBikeColorChangeWindow::SetColorUIControl()
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
	for( INT i = 0; i < BIKE_COLOR_SLOT; i++ )
	{
		DWORD dwColor = pChar->VehicleColorMain(VEHICLE_ACCESSORYTYPE(i+1));
		int	R,G,B;
		R = (dwColor&0xff0000)>>16;
		G = (dwColor&0xff00)>>8;
		B = dwColor&0xff;
		
		//m_pColorMainSlot[ i ]->SetDiffuse( D3DCOLOR_ARGB( 255, m_nRegR, m_nRegG, m_nRegB ) );
		m_pColorMainSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );
	}

	for( INT i = 0; i < BIKE_COLOR_SLOT; i++ )
	{
		DWORD dwColor = pChar->VehicleColorSub(VEHICLE_ACCESSORYTYPE(i+1));
		int	R,G,B;
		R = (dwColor&0xff0000)>>16;
		G = (dwColor&0xff00)>>8;
		B = dwColor&0xff;

		m_pColorSubSlot[ i ]->SetColor( D3DCOLOR_ARGB( 255, R, G, B ) );
	}
}

void CBikeColorChangeWindow::InitUIControl( BOOL bMain, INT nIndex )
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

void CBikeColorChangeWindow::InitSelect()
{
	for (INT i = 0; i < BIKE_COLOR_SLOT; i++ )
	{
        m_pColorMainSlot[ i ]->SetSelect( FALSE );
		m_pColorSubSlot[ i ]->SetSelect( FALSE );
	}

	m_nMainSelect = 0;
	m_nSubSelect = -1;
	m_pColorMainSlot[ m_nMainSelect ]->SetSelect( TRUE );	
}

void CBikeColorChangeWindow::RegulateRGB()
{
	m_nRegR = m_nR;
	m_nRegG = m_nG;
	m_nRegB = m_nB;
}

void CBikeColorChangeWindow::ClearEditBox()
{
	m_pREditBox->ClearEdit();
	m_pREditBox->EndEdit();

	m_pGEditBox->ClearEdit();
	m_pGEditBox->EndEdit();

	m_pBEditBox->ClearEdit();
	m_pBEditBox->EndEdit();
}
