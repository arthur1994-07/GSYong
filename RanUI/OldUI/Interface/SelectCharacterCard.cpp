#include "StdAfx.h"
#include "SelectCharacterCard.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../OuterInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSelectCharacterCard::CSelectCharacterCard ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice ) :
	CUIOuterWindow ( pInterface, pEngineDevice ),
	m_uCharRemain ( USHRT_MAX )
{
}

CSelectCharacterCard::~CSelectCharacterCard ()
{
}

void CSelectCharacterCard::CreateSubControl ()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	���� �̹���
	CreateControl ( "SELECT_CHARACTER_CARD_USABLE_IMAGE" );
//	CreateControl ( "SELECT_CHARACTER_CARD_USED_IMAGE" );

    //	���� �׵θ�
	{
		{
			CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
			pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pBasicLineBox->CreateBaseBoxOuter ( "SELECT_CHARACTER_CARD_USABLE_BACK" );
			RegisterControl ( pBasicLineBox );
		}

		{
			CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
			pTextBox->CreateSub ( this, "SELECT_CHARACTER_CARD_USABLE_TEXTBOX_STATIC" );
			pTextBox->SetFont ( pFont );
			pTextBox->AddText ( ID2GAMEWORD("SELECT_CHARACTER_CARD_STATIC",0), NS_UITEXTCOLOR::PALEGOLDENROD );
			RegisterControl ( pTextBox );
		}

		{
			CBasicLineBox* pBasicLineBox = new CBasicLineBox(m_pEngineDevice);
			pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_OUTER", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
			pBasicLineBox->CreateBaseBoxOuter ( "SELECT_CHARACTER_CARD_USABLE_TEXTBOX_BACK" );
			RegisterControl ( pBasicLineBox );
		}

		{
			CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
			pTextBox->CreateSub ( this, "SELECT_CHARACTER_CARD_USABLE_TEXTBOX" );
			pTextBox->SetFont ( pFont );
			pTextBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
			RegisterControl ( pTextBox );
			m_pUsableTextBox = pTextBox;
		}
	}
}

void CSelectCharacterCard::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIOuterWindow::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	const USHORT uCharRemain = m_pInterface->GetCharRemain ();

	if ( m_uCharRemain != uCharRemain )
	{
		CString strTemp;
		strTemp.Format ( "%u", uCharRemain );
		m_pUsableTextBox->SetText ( strTemp, NS_UITEXTCOLOR::PALEGOLDENROD );

		m_uCharRemain = uCharRemain;
	}
}