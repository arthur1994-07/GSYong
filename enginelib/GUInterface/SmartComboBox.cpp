#include "pch.h"

#include "./SmartComboBox.h"
#include "./BasicLineBoxSmart.h"
#include "./BasicLineBoxButton.h"

#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/GUInterface/BasicTextBox.h"
#include "UITextControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSmartComboBoxBasic::CSmartComboBoxBasic (EngineDeviceMan* pEngineDevice)
    : CBasicComboBox(pEngineDevice)
{
}

CSmartComboBoxBasic::~CSmartComboBoxBasic ()
{
}

void CSmartComboBoxBasic::CreateSmartComboBox ( const char* szComboBoxControl, const char* szTextureInfo )
{
	CreateSmartComboImage ( szTextureInfo );
	CreateSmartTextBox ();

	CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szComboBoxControl );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );	
}

void CSmartComboBoxBasic::CreateSmartComboImage ( const char* szTextureInfo )
{
	CBasicLineBoxSmart* pLineBoxCombo = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_SMART", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateSubControl ( szTextureInfo );
	RegisterControl ( pLineBoxCombo );

	CBasicLineBoxButton* pLineBoxComboButton = new CBasicLineBoxButton(m_pEngineDevice);
	pLineBoxComboButton->CreateSub ( this, "BASIC_LINE_BOX_BUTTON", UI_FLAG_RIGHT, BASIC_COMBOBOX_BUTTON_DOWN  );
	pLineBoxComboButton->CreateBaseButton ( "BASIC_COMBOBOX_BUTTON_DOWN", "BASIC_COMBOBOX_BUTTON_DOWN_IMAGE", "BASIC_COMBOBOX_BUTTON_DOWN_IMAGE_F" );
	RegisterControl ( pLineBoxComboButton );
}

void CSmartComboBoxBasic::CreateSmartTextBox ()
{
	//	폰트
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	텍스트 박스
	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "BASIC_COMBOBOX_TEXTBOX", UI_FLAG_XSIZE, BASIC_COMBOBOX_TEXTBOX );
	pTextBox->SetFont ( pFont8 );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;
}

void CSmartComboBoxBasic::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case BASIC_COMBOBOX_BUTTON_DOWN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					AddMessageEx ( UI_MSG_COMBOBOX_ROLLOVER );
					AddMessageEx ( UIMSG_MOUSEIN );
				}
			}
		}
		break;
	}
}

