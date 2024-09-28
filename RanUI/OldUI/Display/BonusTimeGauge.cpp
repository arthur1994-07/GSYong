#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "BonusTimeGauge.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBonusTimeGauge::CBonusTimeGauge(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
	, m_pConsume(NULL)
	, m_pCharge(NULL) 
	, m_bCharging(FALSE)
{
}

CBonusTimeGauge::~CBonusTimeGauge ()
{
}

void CBonusTimeGauge::CreateSubControl ()
{
	CBasicProgressBar* pHP = new CBasicProgressBar(m_pEngineDevice);
	pHP->CreateSub ( this, "BONUSTIME_CHARGE_IMAGE" );
	pHP->CreateOverImage ( "BONUSTIME_CHARGE_OVERIMAGE" );	
	RegisterControl ( pHP );
	m_pCharge = pHP;	

	pHP = new CBasicProgressBar(m_pEngineDevice);
	pHP->CreateSub ( this, "BONUSTIME_CONSUME_IMAGE" );
	pHP->CreateOverImage ( "BONUSTIME_CONSUME_OVERIMAGE" );	
	RegisterControl ( pHP );
	m_pConsume = pHP;	


	CUIControl* pImage = new CUIControl(m_pEngineDevice);
	pImage->CreateSub ( this, "BONUSTIME_IMAGE" );	
	pImage->SetUseRender ( TRUE );
	pImage->SetVisibleSingle ( TRUE );
	RegisterControl ( pImage );
}

void CBonusTimeGauge::SetTimeCharge ( int nNOW, int nMAX )
{
	const float fPercent = float(nNOW) / float(nMAX);
	m_pCharge->SetPercent ( fPercent );
}

void CBonusTimeGauge::SetTimeConsume ( int nNOW, int nMAX )
{
	const float fPercent = float(nNOW) / float(nMAX);
	m_pConsume->SetPercent ( fPercent );
}

void CBonusTimeGauge::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	int nRemainTime(0);
	if ( m_bCharging ) 
	{		
		const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();
		nRemainTime = sCharData.m_RemainEventTime;
		SetTimeCharge( sCharData.m_EventStartTime - sCharData.m_RemainEventTime, sCharData.m_EventStartTime );
	}
	else
	{	
        const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();
		nRemainTime = sCharData.m_RemainBusterTime;
		SetTimeConsume( sCharData.m_RemainBusterTime, sCharData.m_EventBusterTime );
	}

	if ( CHECK_MOUSE_IN ( GetMessageEx () ) )
	{
		if ( m_pInterface->BEGIN_COMMON_LINEINFO_MULTI() )
		{
			const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();

			const std::string strEvent(sc::string::format( ID2GAMEINTEXT("BONUS_TIME_MESSAGE", 0)
				, static_cast<int>(sCharData.m_EventBusterExpGain * 100.0f)
				, static_cast<int>(sCharData.m_EventBusterItemGain * 100.0f)
				, nRemainTime) );
			m_pInterface->ADD_COMMON_LINEINFO_MULTI( strEvent.c_str(), NS_UITEXTCOLOR::WHITE );
			m_pInterface->ADD_COMMON_LINEINFO_MULTI( ID2GAMEINTEXT("BONUS_TIME_MESSAGE",1), NS_UITEXTCOLOR::WHITE  );
			m_pInterface->ADD_COMMON_LINEINFO_MULTI( ID2GAMEINTEXT("BONUS_TIME_MESSAGE",2), NS_UITEXTCOLOR::WHITE  );
			m_pInterface->END_COMMON_LINEINFO_MULTI();
		}
	}
}

void CBonusTimeGauge::BONUS_TIME_EVENT_START( bool bCharging )
{
	SetVisibleSingle ( TRUE );
	m_bCharging = bCharging;
	m_pCharge->SetVisibleSingle( m_bCharging );	
	m_pConsume->SetVisibleSingle( !m_bCharging );	
}

void CBonusTimeGauge::BONUS_TIME_EVENT_END()
{
	SetVisibleSingle ( FALSE );
}

void CBonusTimeGauge::BONUS_TIME_BUSTER_START()
{
	m_bCharging = FALSE;
	m_pConsume->SetPercent( 1.0f );
	m_pConsume->SetVisibleSingle( TRUE );
	m_pCharge->SetVisibleSingle( FALSE );	
	
}

void CBonusTimeGauge::BONUS_TIME_BUSTER_END()
{
	m_bCharging = TRUE;
	m_pCharge->SetPercent( 0.0f );
	m_pCharge->SetVisibleSingle( TRUE );
	m_pConsume->SetVisibleSingle( FALSE );	
}