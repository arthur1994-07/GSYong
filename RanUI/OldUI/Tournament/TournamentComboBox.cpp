#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../Util/RnButton.h"

#include "TournamentComboBox.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace PVP;

// ----------------------------------------------------------------------------

TournamentComboBox::TournamentComboBox(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIGroup(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
, m_nCurrentFilter(0)
, m_MaxSize(-1)
{
}

TournamentComboBox::~TournamentComboBox ()
{
}

void TournamentComboBox::CreateSubControl ()
{

	{ // 콤보박스 
		// 콤보박스 
		// 배경
		CreateLineBox( "TOURNAMENT_VIEWCOMBO", "PRODUCT_WINDOW_BASE_SMARTLINEBOX_GRAY" );

		CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

		// 텍스트 영역
		m_pComboBoxText = new CBasicTextBox(m_pEngineDevice);
		m_pComboBoxText->CreateSub( this, "TOURNAMENT_VIEWCOMBO", UI_FLAG_DEFAULT, NO_ID );
		m_pComboBoxText->SetFont( pFont9 );
		m_pComboBoxText->SetTextAlign( TEXT_ALIGN_LEFT | TEXT_ALIGN_CENTER_Y  );
		RegisterControl( m_pComboBoxText );

		//버튼
		RnButton::CreateArg arg;
		//////////////////////////////////////////////////////////////////////////
		arg.defaultTextureKey = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_NORMAL";
		arg.mouseOverTextureKey = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_OVER";
		arg.mouseClickTextureKey = "ANOTHER_CHARACTER_COMBOBOX_BUTTON_DOWN";
		//////////////////////////////////////////////////////////////////////////

		RnButton* pRnButton = new RnButton(m_pEngineDevice);
		pRnButton->CreateSub(this, "TOURNAMENT_COMBO_COMBO_BUTTON", UI_FLAG_DEFAULT, CONTROL_ID_COMBO_BUTTON );
		pRnButton->CreateSubControl(arg);
		RegisterControl(pRnButton);


		m_pComboBoxRollOver =  new CBasicComboBoxRollOver(m_pEngineDevice);
		m_pComboBoxRollOver->CreateSub( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, CONTROL_ID_COMBO_ROLLOVER );
		m_pComboBoxRollOver->CreateBaseComboBoxRollOver( "TOURNAMENT_COMBO_COMBO_ROLLOVER");
		m_pComboBoxRollOver->SetVisibleSingle( FALSE );
		m_pComboBoxRollOver->SetVisibleThumb( FALSE );
		RegisterControl( m_pComboBoxRollOver );

		//CString strTemp;
		//strTemp.Format(" 64", strComboTextAll ); // 공백 삽입
		//m_pComboBoxRollOver->AddText( " Test" );
		//strTemp.Format(" 32", strComboTextOnline ); // 공백 삽입
		

		//m_pComboBoxText->SetText( m_pComboBoxRollOver->GetSelectText( m_nCurrentFilter ) );
	}
}

int TournamentComboBox::AddText(
	const CString& str,
	bool bVisibleScroll /*= FALSE*/,
	bool bOneLine /*= FALSE*/,
	const D3DCOLOR& TextColor /*= NS_UITEXTCOLOR::WHITE*/ )
{
	m_MaxSize++;
	return m_pComboBoxRollOver->AddText( str ,bVisibleScroll,bOneLine,TextColor);

}

void TournamentComboBox::ClearText()
{
	m_pComboBoxRollOver->ClearText();
	m_MaxSize = -1;
	//m_pComboBoxRollOver->SetSelectIndex(0);
}

//CString	TournamentComboBox::GetSelectText ( int nIndex )
//{
//
//	m_nCurrentFilter = nIndex;
//	return m_pComboBoxRollOver->GetSelectText( m_nCurrentFilter );
//}

void TournamentComboBox::SetSelectText(int nIndex)
{
	if(nIndex < 0)
		nIndex = 0;

	if(m_MaxSize < nIndex)
		return;

	m_nCurrentFilter = nIndex;
	
		

	CString strTemp = m_pComboBoxRollOver->GetSelectText(nIndex);
	m_pComboBoxText->SetText( strTemp, NS_UITEXTCOLOR::WHITE );
}

void TournamentComboBox::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void TournamentComboBox::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage(ControlID,dwMsg);
	switch( ControlID )
	{
		
	case CONTROL_ID_COMBO_BUTTON:
		if( CHECK_MOUSE_IN( dwMsg ) && CHECK_LB_UP_LIKE( dwMsg ) )
		{
			m_pComboBoxRollOver->SetVisibleSingle( !m_pComboBoxRollOver->IsVisible() );
			return;
		}
		break;
	case CONTROL_ID_COMBO_ROLLOVER:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			if( CHECK_LB_UP_LIKE( dwMsg ) )
			{
				int nIndex = m_pComboBoxRollOver->GetSelectIndex();
				if( nIndex == -1)
					return;
				SetSelectText(nIndex);
				m_pComboBoxRollOver->SetVisibleSingle( FALSE );
				return;
			}
		}
	}
}


CUIControl* TournamentComboBox::CreateLineBox( const std::string& strControl, const std::string& strTexInfo ,UIGUID id )
{
	CBasicLineBoxSmart* pLineBox;
	pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, strControl.c_str() ,UI_FLAG_DEFAULT,id);
	pLineBox->CreateSubControl( strTexInfo.c_str() );
	RegisterControl( pLineBox );
	return pLineBox;
}
