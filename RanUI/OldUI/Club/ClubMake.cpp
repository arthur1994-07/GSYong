#include "StdAfx.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/DxResponseMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../InnerInterface.h"
#include "ClubMake.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CClubMake::nLIMIT_CHAR = 10;
const char CClubMake::BLANK_SYMBOL = ' ';

CClubMake::CClubMake(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindow(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
{
}

CClubMake::~CClubMake()
{
}

void CClubMake::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "PRIVATE_MARKET_MAKE_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "PRIVATE_MARKET_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pTextBox->SetFont ( pFont9 );
//	pTextBox->SetTextAlign ( TEXT_CENTER_ALIGN );
	pTextBox->AddText ( ID2GAMEINTEXT("CLUB_MAKE_OPEN_MESSAGE") );
//	pTextBox->SetPartInterval ( 5.0f );
	RegisterControl ( pTextBox );
//	m_pTextBox = pTextBox;

	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxEditBox ( "PRIVATE_MARKET_MAKE_EDIT_BACK" );
	RegisterControl ( pBasicLineBox );

	CUIEditBox* pEditBox = new CUIEditBox(m_pEngineDevice);
	pEditBox->CreateSub ( this, "PRIVATE_MARKET_MAKE_EDIT", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, CLUB_MAKE_EDIT );
	pEditBox->CreateCarrat ( "PRIVATE_MARKET_MAKE_CARRAT", TRUE, UINT_MAX );
	// pEditBox->SetLimitInput ( nLIMIT_CHAR );
	pEditBox->SetLimitInput( club::CLUBNAME_LIMIT );
	pEditBox->SetFont ( pFont9 );		
	RegisterControl ( pEditBox );
	m_pEditBox = pEditBox;

	CBasicTextButton* pButton = NULL;
	pButton = CreateTextButton ( "PRIVATE_MARKET_MAKE_OK", CLUB_MAKE_OK, (char*)ID2GAMEWORD ( "PRIVATE_MARKET_MAKE_BUTTON", 0 ) );
	pButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	pButton = CreateTextButton ( "PRIVATE_MARKET_MAKE_CANCEL", CLUB_MAKE_CANCEL, (char*)ID2GAMEWORD ( "PRIVATE_MARKET_MAKE_BUTTON", 1 ) );
	pButton->SetShortcutKey ( DIK_ESCAPE );
}

CBasicTextButton* CClubMake::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );

	return pButton;
}

void CClubMake::Init ( const DWORD& dwNpcGlobID )
{
	m_pEditBox->EndEdit ();
	m_pEditBox->ClearEdit ();
	m_pEditBox->BeginEdit ();

	m_dwNpcGlobID = dwNpcGlobID;
}

void CClubMake::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIWindow::TranslateUIMessage(ControlID, dwMsg);

	switch (ControlID)
	{
	case CLUB_MAKE_OK:
		{
			if (CHECK_KEYFOCUSED(dwMsg) || CHECK_MOUSEIN_LBUPLIKE(dwMsg))
			{
                DXInputString* pInputString = m_pEngineDevice->GetInputString();
				if (CHECK_KEYFOCUSED(dwMsg) && !pInputString->CheckEnterKeyDown())
                {
                    return;
                }

				CString strTrim = m_pEditBox->GetEditString();
				strTrim.Trim(BLANK_SYMBOL);
				if (!m_pEditBox->GetEditLength() || !strTrim.GetLength())
                    return;

				m_pEditBox->EndEdit();
				m_pInterface->UiHideGroup(GetWndID(), true);
//				m_pInterface->SetPrivateMarketOpen ( true, NATIVEID_NULL().dwID );	

				//	타이틀 설정
				CString strTITLE = m_pEditBox->GetEditString ();
				m_pGaeaClient->GetCharacter()->ReqClubNew(m_dwNpcGlobID, strTITLE.GetString());
			}
		}
		break;

	case CLUB_MAKE_CANCEL:
	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pEditBox->EndEdit ();

				m_pInterface->UiHideGroup( GetWndID (), true );
			}
		}
		break;
	}
}

void	CClubMake::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindow::SetVisibleSingle ( bVisible );

	if ( !bVisible )
	{
		m_pEditBox->EndEdit ();
	}
}

const CString& CClubMake::GetTITLE ()
{
	static CString strTemp;

	CString strTrim = m_pEditBox->GetEditString ();
	strTrim.Trim ( BLANK_SYMBOL );
	if ( !m_pEditBox->GetEditLength () || !strTrim.GetLength () ) return strTemp;

	return m_pEditBox->GetEditString ();
}
