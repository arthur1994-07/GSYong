
#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DeviceMan.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"

#include "./SelectiveformBoxWindow.h"
#include "./SelectiveformBoxPage.h"

SelectiveformBoxWindow::SelectiveformBoxWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
{
}

SelectiveformBoxWindow::~SelectiveformBoxWindow(void)
{
}

// UI Cfg;
// GUI/UiInner/Selectiveform/Selectiveform.xml;
void SelectiveformBoxWindow::CreateSubControl(void)
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	// 설명글;
	CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
	pBasicLineBox->CreateSub( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestList( "SELFORMBOX_EXPLAIN_LINEBOX" );
	RegisterControl( pBasicLineBox );

	m_pExplainTextBox = new CBasicTextBox(m_pEngineDevice);
	m_pExplainTextBox->CreateSub( this, "SELFORMBOX_EXPLAIN_TEXTBOX" );
	m_pExplainTextBox->SetFont( pFont );
	RegisterControl( m_pExplainTextBox );

	m_pExplainTextBox->AddText(ID2GAMEWORD("SELFORMBOX_WINDOW_DESC_STATIC"));

	// 페이지;
	m_pPage = new SelectiveformBoxPage(m_pGaeaClient, m_pInterface, m_pEngineDevice);
	m_pPage->CreateSub( this, "SELFORMBOX_PAGE", UI_FLAG_DEFAULT, SELFORMW_PAGE );
	m_pPage->CreateSubControl();
	RegisterControl(m_pPage);

	// 확인/취소;
	m_pBtnSelect = CreateTextButton( "SELFORMBOX_BUTTON_SELECT", SELFORMW_SELECT, const_cast<char*>(ID2GAMEWORD("SELFORMBOX_WINDOW_BTN",0)) );
	m_pBtnCancel = CreateTextButton( "SELFORMBOX_BUTTON_CANCEL", SELFORMW_CANCEL, const_cast<char*>(ID2GAMEWORD("SELFORMBOX_WINDOW_BTN",1)) );
}

HRESULT	SelectiveformBoxWindow::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIWindowEx::RestoreDeviceObjects( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const UIRECT& rcWindow = GetGlobalPos();

	const long lResolution = m_pInterface->UiGetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	D3DXVECTOR2 vPos;
	vPos.x = ((X_RES) / 2.0f) - rcWindow.sizeX;
	vPos.y = (Y_RES - rcWindow.sizeY) / 2.0f;
	SetGlobalPos( vPos );

	return S_OK;
}

void SelectiveformBoxWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	//..
}

void SelectiveformBoxWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage(cID, dwMsg);
	switch ( cID )
	{
	case SELFORMW_SELECT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
			{
				INT nSelPos(-1);
				if ( m_pPage->GetSelectPos(nSelPos) )
					m_pInterface->DoModal( ID2GAMEWORD("SELFORMBOX_WINDOW_SELMODAL"), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SELFORMW_SELECTITEM );
			}
		}
		break;
	case ET_CONTROL_BUTTON:
	case SELFORMW_CANCEL:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				m_pInterface->UiHideGroup(this->GetWndID());
		}
		break;
	case SELFORMW_PAGE:
		{
		}
		break;
	}
}

CBasicTextButton* SelectiveformBoxWindow::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl( pTextButton );
	return pTextButton;
}

void SelectiveformBoxWindow::Open(std::vector<ITEM::SBOX_ITEM>& vecItemID)
{
	SelectiveformBoxPage* pPage = static_cast<SelectiveformBoxPage*>(this->FindControl(SELFORMW_PAGE));
	if ( pPage )
		pPage->LoadItemPage(vecItemID);

	if ( IsVisible() == FALSE )
	{
		m_pInterface->UiShowGroupFocus( SELECTIVEFORMBOX_WINDOW );
	}
}

BOOL SelectiveformBoxWindow::GetSelectPos(INT& nPos) const
{
	if ( m_pPage == NULL)
		return FALSE;

	return m_pPage->GetSelectPos(nPos);
}