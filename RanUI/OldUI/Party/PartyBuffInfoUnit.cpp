#include "StdAfx.h"

#include "PartyBuffInfoUnit.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../Skill/SkillImage33.h"
#include "../../InnerInterface.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPartyBuffInfoUnit::CPartyBuffInfoUnit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
    , m_pInterface(pInterface)
{
}

CPartyBuffInfoUnit::~CPartyBuffInfoUnit ()
{
}

void CPartyBuffInfoUnit::CreateSubControl ()
{
	CSkillImage33* pSkillImage = new CSkillImage33 ( m_pGaeaClient, m_pEngineDevice );
	pSkillImage->CreateSub ( this, "PARTYBUFF_IMAGE" );	
	pSkillImage->SetUseRender ( TRUE );
	RegisterControl ( pSkillImage );
	m_pSkillImage = pSkillImage;

	CBasicProgressBar* pProgressBar = new CBasicProgressBar(m_pEngineDevice);
	pProgressBar->CreateSub ( this, "PARTYBUFF_PROGRESS" );
	pProgressBar->CreateOverImage ( "PARTYBUFF_PROGRESS_OVERIMAGE" );
	pProgressBar->SetType ( CBasicProgressBar::VERTICAL );
	RegisterControl ( pProgressBar );
	m_pProgressBar = pProgressBar;
}

void CPartyBuffInfoUnit::SetLeftTime ( const float& fLeftTime )
{
    m_fLEFT_TIME = fLeftTime;
}

void CPartyBuffInfoUnit::SetSkill ( const DWORD& dwID, const float& fLifeTime, const CString& strSkillName )
{
	m_fLIFE_TIME = fLifeTime;
	m_pSkillImage->SetSkill ( dwID );
	m_pSkillImage->SetVisibleSingle ( TRUE );

	m_strSkillName = strSkillName;
}

void CPartyBuffInfoUnit::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	if ( m_pProgressBar )
	{
		float fPercent = m_fLEFT_TIME/m_fLIFE_TIME;

        if ( fPercent > 1.0f )
        {
            fPercent = 1.0f;
        }

		m_pProgressBar->SetPercent ( fPercent );
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( CHECK_MOUSE_IN ( GetMessageEx () ) )
	{
		m_pInterface->SHOW_COMMON_LINEINFO ( m_strSkillName, NS_UITEXTCOLOR::GREENYELLOW );
	}
}
