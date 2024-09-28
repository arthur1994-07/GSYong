#include "StdAfx.h"
#include "SimpleHP.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../RanLogic/GLogicEx.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSimpleHP::CSimpleHP(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient ( pGaeaClient )
    , m_pHP ( NULL )
{
}

CSimpleHP::~CSimpleHP ()
{
}

void CSimpleHP::CreateSubControl ()
{
	CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_SIMPLEHP", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxTargetInfo ( "BASIC_SIMPLEHP_LINE_BOX" );
	RegisterControl ( pLineBox );
	m_pHPBox = pLineBox;

	CBasicProgressBar* pHP = new CBasicProgressBar(m_pEngineDevice);
	pHP->CreateSub ( this, "SIMPLE_HP_IMAGE" );
	pHP->CreateOverImage ( "SIMPLE_HP_OVERIMAGE" );		

	RegisterControl ( pHP );
	m_pHP = pHP;	
}

void CSimpleHP::SetHP ( const int nNow, const int nMax )
{
	const float fPercent(float(nNow) / float(nMax));
	m_pHP->SetPercent ( fPercent );		
}

void CSimpleHP::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () )
		return ;	

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	const GLCHARLOGIC& sCharData = m_pGaeaClient->GetCharacterLogic ();
	SetHP ( sCharData.m_sHP.nNow, sCharData.m_sHP.nMax );

}