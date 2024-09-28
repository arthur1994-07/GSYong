#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"

#include "../../../RanLogic/Item/GLItemMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Summon/GLSummonClient.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "../Item/ItemMove.h"
#include "../Pet/PetQuickSkillSlot.h"
#include "Summonwindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const D3DCOLOR CSummonWindow::dwDEFAULT_TRANSPARENCY = D3DCOLOR_ARGB ( 160, 255, 255, 255 );;
const D3DCOLOR CSummonWindow::dwFULL_TRANSPARENCY = D3DCOLOR_ARGB ( 255, 255, 255, 255 );;

CSummonWindow::CSummonWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIWindowEx(pInterface, pEngineDevice),
	m_pGaeaClient ( pGaeaClient ),
	m_pNameText(NULL),
	m_pLevelText(NULL),

    m_pDamageText(NULL),
    m_pDefenseText(NULL),
    m_pHitText(NULL),
    m_pDodgeText(NULL),

    m_pRegFireText(NULL),
    m_pRegColdText(NULL),
    m_pRegElectricText(NULL),
    m_pRegPoisonText(NULL),
    m_pRegSpritsText(NULL),

    m_pHPText(NULL),

    m_pHP(NULL),

    m_pSummonClient(NULL)
{
}

CSummonWindow::~CSummonWindow(void)
{
}


void CSummonWindow::CreateSubControl()
{
	CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG/*D3DFONT_SHADOW | D3DFONT_ASCII*/ );
	CD3DFontPar* pFont2 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CreateControl ( "SUMMON_BASIC_BACK_L" );
	CreateControl ( "SUMMON_STATE_BACK_L" );
	CreateControl ( "SUMMON_REGSITANCE_BACK_L" );
	CreateControl ( "SUMMON_BASIC_BACK_R" );
	CreateControl ( "SUMMON_STATE_BACK_R" );
	CreateControl ( "SUMMON_REGSITANCE_BACK_R" );


	CBasicTextBox*	  pTextStatic = NULL;
	// 기본 정보
	pTextStatic = CreateStaticControl ( "SUMMON_BASIC_STATIC", pFont, TEXT_ALIGN_CENTER_X );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",0), NS_UITEXTCOLOR::PALEGOLDENROD );
	// 이름
	pTextStatic = CreateStaticControl ( "SUMMON_NAME_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",1), NS_UITEXTCOLOR::SILVER );
	// 레벨
	pTextStatic = CreateStaticControl ( "SUMMON_LEVEL_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",2), NS_UITEXTCOLOR::SILVER );

	// 상태
	pTextStatic = CreateStaticControl ( "SUMMON_STATE_STATIC", pFont, TEXT_ALIGN_CENTER_X );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",3), NS_UITEXTCOLOR::PALEGOLDENROD );
	// HP
	pTextStatic = CreateStaticControl ( "SUMMON_HP_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",4), NS_UITEXTCOLOR::SILVER );
	// 타격력
	pTextStatic = CreateStaticControl ( "SUMMON_DAMAGE_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",5), NS_UITEXTCOLOR::SILVER );
	// 방어력
	pTextStatic = CreateStaticControl ( "SUMMON_DEFENSE_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",6), NS_UITEXTCOLOR::SILVER );
	// 명중률
	pTextStatic = CreateStaticControl ( "SUMMON_HIT_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",7), NS_UITEXTCOLOR::SILVER );
	// 회피율
	pTextStatic = CreateStaticControl ( "SUMMON_DODGE_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",8), NS_UITEXTCOLOR::SILVER );

	// 저항력
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_STATIC", pFont, TEXT_ALIGN_CENTER_X );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",9), NS_UITEXTCOLOR::PALEGOLDENROD );
	// 화기저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_FIRE_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",10), NS_UITEXTCOLOR::SILVER );
	// 냉기저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_COLD_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",11), NS_UITEXTCOLOR::SILVER );
	// 전기저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_ELECTRIC_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",12), NS_UITEXTCOLOR::SILVER );
	// 독저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_POISON_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",13), NS_UITEXTCOLOR::SILVER );
	// 기저항
	pTextStatic = CreateStaticControl ( "SUMMON_REGSITANCE_SPRITS_STATIC", pFont, TEXT_ALIGN_LEFT );
	pTextStatic->AddText ( ID2GAMEWORD("SUMMON_WINDOW_TEXT",14), NS_UITEXTCOLOR::SILVER );

	m_pNameText		= CreateStaticControl ( "SUMMON_NAME_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pLevelText	= CreateStaticControl ( "SUMMON_LEVEL_TEXT", pFont, TEXT_ALIGN_LEFT );

	m_pHP = new CBasicProgressBar(m_pEngineDevice);
	m_pHP->CreateSub ( this, "SUMMON_HP" );
	m_pHP->CreateOverImage ( "SUMMON_HP_WINDOW_OVERIMAGE" );	
	m_pHP->SetControlNameEx ( "SUMMON HP 프로그래스바" );
	RegisterControl ( m_pHP );

	m_pDamageText	= CreateStaticControl ( "SUMMON_DAMAGE_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pDefenseText	= CreateStaticControl ( "SUMMON_DEFENSE_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pHitText		= CreateStaticControl ( "SUMMON_HIT_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pDodgeText	= CreateStaticControl ( "SUMMON_DODGE_TEXT", pFont, TEXT_ALIGN_LEFT );

	m_pRegFireText	   = CreateStaticControl ( "SUMMON_REGSITANCE_FIRE_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pRegColdText	   = CreateStaticControl ( "SUMMON_REGSITANCE_COLD_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pRegElectricText = CreateStaticControl ( "SUMMON_REGSITANCE_ELECTRIC_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pRegPoisonText   = CreateStaticControl ( "SUMMON_REGSITANCE_POISON_TEXT", pFont, TEXT_ALIGN_LEFT );
	m_pRegSpritsText   = CreateStaticControl ( "SUMMON_REGSITANCE_SPRITS_TEXT", pFont, TEXT_ALIGN_LEFT );

	m_pHPText = CreateStaticControl ( "SUMMON_HP_TEXT", pFont2, TEXT_ALIGN_CENTER_X );

	{
		CBasicTextButton *pSearchButton1;
		pSearchButton1 = CreateTextButton ( "SUMMON_CONFIRM_BUTTON", SUMMON_CONFIRM_BUTTON, const_cast<char*>(ID2GAMEWORD("SUMMON_WINDOW_TEXT",16)) );
		pSearchButton1->SetShortcutKey ( DIK_ESCAPE );
	}

}

void CSummonWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		if( !m_pSummonClient || !m_pSummonClient->IsValidBody() ) return;
		PCROWDATA pSummonData = m_pSummonClient->m_pSummonCrowData;
		if( pSummonData )
		{
			CString strTemp;
			strTemp = pSummonData->GetName();
			m_pNameText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_wLevel );
			m_pLevelText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );

			strTemp.Format( "%d - %d", pSummonData->m_vecCrowAttack[0].sDamage.wLow, pSummonData->m_vecCrowAttack[0].sDamage.wHigh );
			m_pDamageText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_wDefense );
			m_pDefenseText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_wHitRate );
			m_pHitText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_wAvoidRate );
			m_pDodgeText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );

			strTemp.Format( "%d", pSummonData->m_sResist.nFire );
			m_pRegFireText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nIce );
			m_pRegColdText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nElectric );
			m_pRegElectricText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nPoison );
			m_pRegPoisonText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nPoison );
			m_pRegPoisonText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			strTemp.Format( "%d", pSummonData->m_sResist.nSpirit );
			m_pRegSpritsText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );
			int curHP = m_pSummonClient->GETHP();
			int maxHP = m_pSummonClient->GETMAXHP();
			strTemp.Format( "%d/%d", curHP, maxHP );
			m_pHPText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );

			GLDWDATA dwHP = m_pSummonClient->GetHp();

			const float fPercent = float(dwHP.dwNow) / float(dwHP.dwMax);
			if ( m_pHP ) m_pHP->SetPercent ( fPercent );
		}		
	}else
	{
		m_pInterface->UiHideGroup( SUMMON_WINDOW );
	}
}

CItemImage*	CSummonWindow::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage ( m_pGaeaClient, m_pEngineDevice );
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}


CBasicTextButton* CSummonWindow::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pTextButton = new CBasicTextButton(m_pEngineDevice);
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pTextButton );
	return pTextButton;
}

CBasicTextBox* CSummonWindow::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, cID );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );

	return pStaticText;
}

void CSummonWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{

	if( m_pSummonClient == NULL || m_pSummonClient->IsValidBody() == FALSE )
	{
		SetVisibleSingle( FALSE );
		return;
	}

	int curHP = m_pSummonClient->GETHP();
	int maxHP = m_pSummonClient->GETMAXHP();
	CString strTemp;
	strTemp.Format( "%d/%d", curHP, maxHP );
	m_pHPText->SetOneLineText( strTemp.GetString(), NS_UITEXTCOLOR::WHITE );

	GLDWDATA dwHP = m_pSummonClient->GetHp();
	const float fPercent = float(dwHP.dwNow) / float(dwHP.dwMax);
	if ( m_pHP ) m_pHP->SetPercent ( fPercent );

	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSummonWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case SUMMON_CONFIRM_BUTTON:
	case ET_CONTROL_BUTTON:
		if( CHECK_KEYFOCUSED ( dwMsg ) || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
		{
			m_pInterface->UiHideGroup( GetWndID() );
		}
		break;
	}
}
