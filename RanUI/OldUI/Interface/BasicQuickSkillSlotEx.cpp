#include "StdAfx.h"
#include "BasicQuickSkillSlotEx.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "SkillTrayTab.h"
#include "../../InnerInterface.h"
#include "../../../RanLogic/RANPARAM.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const int CBasicQuickSkillSlotEx::nNOT_INIT = -1;

CBasicQuickSkillSlotEx::CBasicQuickSkillSlotEx(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CBasicQuickSkillSlot(pGaeaClient, pInterface, pEngineDevice)
    , m_pInterface(pInterface)
	, m_nNumber ( nNOT_INIT )
	, m_pNumberText ( NULL )
	, m_pReserved ( NULL )
	, m_nTabIndex(0)
{
}

CBasicQuickSkillSlotEx::~CBasicQuickSkillSlotEx ()
{
}

void CBasicQuickSkillSlotEx::CreateSubControl ( INT nTabIndex )
{
	GASSERT( 0 <= nTabIndex && nTabIndex < MAX_TAB_INDEX );
	m_nTabIndex = nTabIndex;

	CBasicQuickSkillSlot::CreateSubControl ();
	if ( m_pSkillMouseOver ) m_pSkillMouseOver->SetVisibleSingle ( FALSE );

	m_pReserved = new CUIControl(m_pEngineDevice);
	m_pReserved->CreateSub( this, "QUICK_SKILL_RESERVED", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pReserved->SetVisibleSingle( FALSE );
	RegisterControl(m_pReserved);
}

void CBasicQuickSkillSlotEx::CreateNumberText ( CD3DFontPar* pFont8, int nNumber )
{
	m_nNumber = nNumber;

	int nDisplayNumber = nNumber + 1;
	if ( nDisplayNumber == QUICK_SKILL_SLOT_MAX )
	{
		nDisplayNumber = 0;
	}

	std::string strText;	
	strText = GLInputManager::GetInstance().GetShortcutsToString( static_cast<EMSHORTCUTS>(EMSHORTCUTS_SKILLSLOT0+m_nNumber) );

	CBasicTextBox* pNumberText = new CBasicTextBox(m_pEngineDevice);
	pNumberText->CreateSub ( this, "QUICK_SKILL_SLOT_NUMBER" );
    pNumberText->SetFont ( pFont8 );
	pNumberText->SetTextAlign ( TEXT_ALIGN_RIGHT );
	pNumberText->SetOneLineText ( strText.c_str(), NS_UITEXTCOLOR::WHITE );
	pNumberText->SetControlNameEx ( "스킬 넘버 텍스트박스" );
	RegisterControl ( pNumberText );
	m_pNumberText = pNumberText;
}

void CBasicQuickSkillSlotEx::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	//	바로 상위 ( CBasicQuickSkillSlot::Update() )를 부르지 않은 것은 의도된 것입니다.
	//	상위의 Update는 다른 용도로 사용되기 때문에, 호출할경우
	//	다른 의미로 동작하기 때문에, 건너뛰고 그 상위 클래스를 호출합니다.
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    int nNumber = m_nNumber;
	GASSERT ( 0 <= nNumber && nNumber < EMSKILLQUICK_SIZE && "범위를 벗어납니다." );

	INT nG_Index = m_nTabIndex * QUICK_SKILL_SLOT_MAX;
	nG_Index += nNumber;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	SNATIVEID sNativeID		= pCharacter->m_sSKILLQUICK[nG_Index];
	SNATIVEID sScopeSkillID = pCharacter->GetScopeSkillRun().sSKILLID;
	UpdateSlot ( sNativeID );

	BOOL bSlotEmpty = (sNativeID == NATIVEID_NULL ());

	DWORD dwMsg = GetMessageEx ();

	if ( m_pSkillMouseOver )
	{
		m_pSkillMouseOver->SetVisibleSingle ( FALSE );

		if ( !bSlotEmpty )
		{
			if ( sScopeSkillID == sNativeID )
			{
				m_pSkillMouseOver->SetVisibleSingle ( TRUE );
			}

			if ( CHECK_MOUSE_IN ( dwMsg ) && bFirstControl )
			{
				m_pSkillMouseOver->SetVisibleSingle ( TRUE );
			}
		}
	}
}

void CBasicQuickSkillSlotEx::SetVisibleReservedSkill( const bool bEnable )
{
	if ( m_pReserved )
	{
		m_pReserved->SetVisibleSingle( bEnable ? TRUE : FALSE );
	}
}

void CBasicQuickSkillSlotEx::SetReservedSkillOrder( const UINT nOrder, const UINT nMax )
{
	if ( m_pReserved && (nMax != 0) )
	{
		m_pReserved->SetDiffuseAlpha( static_cast<WORD>(255.0f * (1.0f - (static_cast<float>(nOrder)/static_cast<float>(nMax)) )) );
	}
}

void CBasicQuickSkillSlotEx::SetShotcutText( CString strTemp )
{
	m_pNumberText->SetText( strTemp, NS_UITEXTCOLOR::WHITE );
}

void CBasicQuickSkillSlotEx::TurnHorizontal ()
{
    CBasicQuickSkillSlot::TurnHorizontal ();

    m_pNumberText->ResetControl( GetGlobalPos(), "QUICK_SKILL_SLOT_NUMBER_H" );
}

void CBasicQuickSkillSlotEx::TurnVertical ()
{
    CBasicQuickSkillSlot::TurnVertical ();

    m_pNumberText->ResetControl( GetGlobalPos(), "QUICK_SKILL_SLOT_NUMBER" );
}

void CBasicQuickSkillSlotEx::ResetText ()
{
	if ( !m_pNumberText )
	{
		return;
	}

	std::string strText;	
	strText = GLInputManager::GetInstance().GetShortcutsToString( static_cast<EMSHORTCUTS>(EMSHORTCUTS_SKILLSLOT0+m_nNumber) );
	m_pNumberText->SetOneLineText ( strText.c_str(), NS_UITEXTCOLOR::WHITE );
}