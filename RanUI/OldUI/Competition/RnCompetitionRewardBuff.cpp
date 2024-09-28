#include "StdAfx.h"
#include "../../../enginelib/DeviceMan.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../Skill/SkillImage.h"

#include "RnCompetitionRewardBuff.h"

RnCompetitionRewardBuff::RnCompetitionRewardBuff( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIGroup(pEngineDevice)
, m_pSkillImage(NULL)
, m_pGaeaClient(pGaeaClient)
, m_pInterface(pInterface)
{

}

RnCompetitionRewardBuff::~RnCompetitionRewardBuff()
{

}

void RnCompetitionRewardBuff::CreateSubControl()
{
	m_pSkillImage = new CSkillImage(m_pGaeaClient, m_pEngineDevice);
	m_pSkillImage->CreateSub( this, "RNCOMPETITION_REWARDBUFF_ITEM", UI_FLAG_DEFAULT );
	m_pSkillImage->SetUseRender( TRUE );
	RegisterControl( m_pSkillImage );

	CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
	pLineBox->CreateSub( this, "RNCOMPETITIO_REWARDBUFF_LINEBOX" );
	pLineBox->CreateSubControl( "COMPETITION_CTF_PAGE_INFO_LINEBOX_TEXINFO_W_BLACK" );
	RegisterControl( pLineBox );
}

void RnCompetitionRewardBuff::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nscroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nscroll, fElapsedTime, bFirstControl);
}

void RnCompetitionRewardBuff::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );
}

void RnCompetitionRewardBuff::SetSkill( SNATIVEID sNativeID )
{
	m_pSkillImage->SetSkill(sNativeID);
}

