#include "StdAfx.h"
#include "StatsResetComboBoxRollOver.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/*
const int CBasicComboBoxRollOver::nSTARTLINE = 0;
const int CBasicComboBoxRollOver::nLIMITLINE = 100;
const int CBasicComboBoxRollOver::nNOT_SELECTED = -1;
const D3DCOLOR	CBasicComboBoxRollOver::dwMOUSEOVERCOLOR = D3DCOLOR_ARGB(0xFF,0x69,0x69,0x69);
*/
CStatsResetComboBoxRollOver::CStatsResetComboBoxRollOver(EngineDeviceMan* pEngineDevice)
    : CBasicComboBoxRollOver(pEngineDevice)
{
}

CStatsResetComboBoxRollOver::~CStatsResetComboBoxRollOver()
{
}

void CStatsResetComboBoxRollOver::CreateComboBoxRollOverImage ( bool bSelectInit )
{
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_COMBO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateBaseBoxStatsReset( "BASIC_COMBOBOX_ROLLOVER_BACK" );
	RegisterControl ( pLineBoxCombo );

	//	폰트
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	텍스트 박스
	CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx(m_pEngineDevice);
	pTextBox->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE, COMBOBOX_ROLLOVER_TEXT_BOX );
	pTextBox->CreateMouseOver ( dwMOUSEOVERCOLOR );
	pTextBox->SetFont ( pFont8 );
	pTextBox->SetSensitive ( true );
	pTextBox->SetLimitLine ( nLIMITLINE );	
	pTextBox->SetSelectInit( bSelectInit );
	RegisterControl ( pTextBox );
	m_pTextBox = pTextBox;
}

void CStatsResetComboBoxRollOver::CreateScrollBar()
{
	//	스크롤바 뒷면 검은 라인, 회색 배경
	CBasicLineBox* pLineBoxCombo = new CBasicLineBox(m_pEngineDevice);
	pLineBoxCombo->CreateSub ( this, "BASIC_LINE_BOX_COMBO", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBoxCombo->CreateBaseBoxCombo ( "STATSRESET_ITEM_COMBO_ROLLOVER_SCROLLBAR_BOX" );
	RegisterControl ( pLineBoxCombo );
	m_pLineBox = pLineBoxCombo;

	//	사각 박스 사이즈를 정상 사이즈로 재조절하고 난 이후가 실제로
	//	원하는 크기의 이미지입니다. 즉, 'COMBOBOX_ROLLOVER_SCROLLBAR'에 설정한
	//	만큼의 크기란 것이지요. 그러나, 그 이후에 스크롤바가 어떻게 리사이징이 되더라도,
	//	XSIZE는 재조절되어서는 안되기 때문에 그 플래그는 빼 버린 것입니다.
	pLineBoxCombo->ResetAlignFlagLine ();
	pLineBoxCombo->SetAlignFlagLine ( CBasicLineBox::LEFT, UI_FLAG_YSIZE );
	pLineBoxCombo->SetAlignFlagLine ( CBasicLineBox::RIGHT,UI_FLAG_YSIZE );
	pLineBoxCombo->SetAlignFlagLine ( CBasicLineBox::BOTTOM, UI_FLAG_BOTTOM );
	pLineBoxCombo->SetAlignFlag ( UI_FLAG_YSIZE | UI_FLAG_RIGHT );

	int nTotalLine = m_pTextBox->GetVisibleLine();

	//	스크롤바
	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BASIC_COMBOBOX_ROLLOVER_SCROLLBAR );
	pScrollBar->CreateBaseScrollBar ( "STATSRESET_ITEM_COMBO_ROLLOVER_SCROLLBAR" );
	pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
	RegisterControl ( pScrollBar );
	m_pScrollBar = pScrollBar;
}
