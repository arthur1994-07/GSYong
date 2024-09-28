#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/Common/DXInputString.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UIEditBox.h"
#include "../../../EngineLib/GUInterface/UIKeyCheck.h"
#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"
#include "../../../EngineLib/DxResponseMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../MfcExLib/RanFilter.h"
#include "../../InnerInterface.h"
#include "PrivateMarketMake.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CPrivateMarketMake::nLIMIT_CHAR = 40;
const char CPrivateMarketMake::BLANK_SYMBOL = ' ';

CPrivateMarketMake::CPrivateMarketMake(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIWindow(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pGaeaClient(pGaeaClient)
{
}

CPrivateMarketMake::~CPrivateMarketMake ()
{
}

void CPrivateMarketMake::CreateSubControl ()
{
	CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_WAITSERVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxWaitServer ( "PRIVATE_MARKET_MAKE_LINE_BOX" );
	RegisterControl ( pLineBox );	

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "PRIVATE_MARKET_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pTextBox->SetFont ( pFont9 );
	//pTextBox->SetTextAlign ( TEXT_CENTER_ALIGN );
	pTextBox->AddText ( ID2GAMEINTEXT("PRIVATE_MARKET_MAKE_OPEN_MESSAGE") );
	//pTextBox->SetPartInterval ( 5.0f );
	RegisterControl ( pTextBox );

	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxEditBox ( "PRIVATE_MARKET_MAKE_EDIT_BACK" );
	RegisterControl ( pBasicLineBox );

	m_pEditBox = new CUIEditBox(m_pEngineDevice);
	m_pEditBox->CreateSub ( this, "PRIVATE_MARKET_MAKE_EDIT", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, PRIVATE_MARKET_MAKE_EDIT );
	m_pEditBox->CreateCarrat ( "PRIVATE_MARKET_MAKE_CARRAT", TRUE, UINT_MAX );
	m_pEditBox->SetLimitInput ( nLIMIT_CHAR );
	m_pEditBox->SetFont ( pFont9 );		
	RegisterControl ( m_pEditBox );

	CBasicTextButton* pButton = NULL;
	pButton = CreateTextButton ( "PRIVATE_MARKET_MAKE_OK", PRIVATE_MARKET_MAKE_OK, (char*)ID2GAMEWORD ( "PRIVATE_MARKET_MAKE_BUTTON", 0 ) );
	pButton->SetShortcutKey ( DIK_RETURN, DIK_NUMPADENTER );
	pButton = CreateTextButton ( "PRIVATE_MARKET_MAKE_CANCEL", PRIVATE_MARKET_MAKE_CANCEL, (char*)ID2GAMEWORD ( "PRIVATE_MARKET_MAKE_BUTTON", 1 ) );
	pButton->SetShortcutKey ( DIK_ESCAPE );
}

CBasicTextButton* CPrivateMarketMake::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton(m_pEngineDevice);
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );

	return pButton;
}

void CPrivateMarketMake::Init ()
{
	m_pEditBox->EndEdit ();
	m_pEditBox->ClearEdit ();
	m_pEditBox->BeginEdit ();
}

void	CPrivateMarketMake::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case PRIVATE_MARKET_MAKE_OK:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
                const SMAPNODE *pMapNode(m_pGaeaClient->FindMapNode(m_pGaeaClient->GetActiveMapID().getBaseMapID()));
                if ( pMapNode && !pMapNode->IsPrivateMarketOpen() )	
                {
                    m_pGaeaClient->PrintMsgTextDlg ( 
                        NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_NOT_OPENABLE") );
                    return;
                }

				if( m_pGaeaClient->GetCharacter()->VehicleState() )
				{
					m_pGaeaClient->PrintMsgTextDlg ( 
						NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_NOIDLE") );
					return;
				}

                DXInputString* pInputString = m_pEngineDevice->GetInputString();
				if (CHECK_KEYFOCUSED(dwMsg) && !pInputString->CheckEnterKeyDown())
                    return;

				CString strTrim = m_pEditBox->GetEditString ();
				strTrim.Trim ( BLANK_SYMBOL );
				if (!m_pEditBox->GetEditLength() || !strTrim.GetLength())
                    return;

				m_pEditBox->EndEdit();				

				m_pInterface->UiHideGroup ( GetWndID (), true );
				m_pInterface->SetPrivateMarketOpen ( true, NATIVEID_NULL().dwID );	

				//	타이틀 설정
				CString strTITLE = m_pEditBox->GetEditString ();

				// 금지어 필터링
				if ( CRanFilter::GetInstance().ChatFilter( strTITLE ) )
				{
					strTITLE = CRanFilter::GetInstance().GetProverb();
				}

				m_pGaeaClient->GetCharacter()->ReqPMarketTitle ( strTITLE );
			}
		}
		break;

	case PRIVATE_MARKET_MAKE_CANCEL:
	case ET_CONTROL_BUTTON:
		{
			if ( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pEditBox->EndEdit ();

				m_pInterface->UiHideGroup ( GetWndID (), true );				
			}
		}
		break;
	}
}

void	CPrivateMarketMake::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindow::SetVisibleSingle ( bVisible );

	if ( !bVisible )
	{
		m_pEditBox->EndEdit ();
	}
}

const CString& CPrivateMarketMake::GetTITLE ()
{
	static CString strTemp;

	CString strTrim = m_pEditBox->GetEditString ();
	strTrim.Trim ( BLANK_SYMBOL );
	if ( !m_pEditBox->GetEditLength () || !strTrim.GetLength () ) return strTemp;

	return m_pEditBox->GetEditString ();
}