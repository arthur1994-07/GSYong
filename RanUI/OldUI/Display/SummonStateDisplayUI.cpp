#include "stdafx.h"

#include "./SummonStateDisplayUI.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/UITextUtil.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogic/Crow/GLCrowDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Summon/GLSummonClient.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSummonStateDisplayUI::CSummonStateDisplayUI ( GLGaeaClient* pClient
											 , CInnerInterface* pInterface
											 , EngineDeviceMan* pEngineDevice )
											 : CUIGroupHelperNonClose( pEngineDevice
											 , pInterface )
											 , m_pClient( pClient )
											 , m_pSummonImage( NULL )
											 , m_pHPGauge( NULL )
{
}

CSummonStateDisplayUI::~CSummonStateDisplayUI () { }

void CSummonStateDisplayUI::CreateSubControl ()
{
	// Summon HP State;
	m_pHPGauge = new CMiniGauge( m_pClient, m_pEngineDevice );
	m_pHPGauge->CreateSub( this, "SUMMON_STATE_UI_HPGAUGE", UI_FLAG_DEFAULT );
	m_pHPGauge->CreateSubControl();
	m_pHPGauge->SetVisibleSingle( FALSE );
	RegisterControl( m_pHPGauge );

	// Summon State Image;
	m_pSummonImage = new CSwapImage( m_pEngineDevice );
	m_pSummonImage->CreateSub ( this, "SUMMON_STATE_UI_REGION", UI_FLAG_DEFAULT, ENotifyButton );
	m_pSummonImage->SetImage( "SUMMON_STATEICON_DIE" );
	m_pSummonImage->SetVisibleSingle( TRUE );
	RegisterControl( m_pSummonImage );
}

void CSummonStateDisplayUI::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroupHelperNonClose::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	static bool bAttackState = false;
	static float fAttackTime = 0.f;

	if ( m_pClient )
	{
		GLCharacter* pChar = m_pClient->GetCharacter();
		if ( pChar )
		{
			// 저장된 고용 소환수 정보;
			PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pChar->m_sBasicSummonID );
			if ( pCrowData )
			{
				m_strSummonName = pCrowData->GetName();
			}


			// 실제 소환된 소환수 정보;
			std::tr1::shared_ptr< GLSummonClient > spSummon =
				m_pClient->GetSummon(
				pChar->m_sSummonable.hireInfo.dwGaeaID );
			if ( spSummon )
			{
				if ( spSummon->isAction( GLAT_ATTACK, GLAT_SKILL ) )
				{
					fAttackTime = 0.f;

					m_pSummonImage->SetImage( "SUMMON_STATEICON_ATTACK" );
					bAttackState = true;

					m_strTooltip = ID2GAMEINTEXT( "SUMMON_STATE_TOOLTIP_FIGHT_TEXT" );
				}
				else if ( spSummon->isAction( GLAT_IDLE ) )
				{
					// 공격상태가 되었을 경우 계속 체크한다;
					if ( bAttackState )
						fAttackTime += fElapsedTime;

					// 공격상태 이후 3초가 지나면 Idle 상태로 변할 수 있게 된다;
					if ( fAttackTime >= 3.f )
					{
						fAttackTime = 0.f;
						bAttackState = false;
					}

					if ( !bAttackState )
					{
						m_pSummonImage->SetImage( "SUMMON_STATEICON_IDLE" );

						m_strTooltip = ID2GAMEINTEXT( "SUMMON_STATE_TOOLTIP_IDLE_TEXT" );
					}
				}

				m_pHPGauge->SetVisibleSingle( TRUE );
				m_pHPGauge->SetAutoBar( spSummon->GETMAXHP(), false );
				int iHP = spSummon->GETHP();
				m_pHPGauge->GaugeUpdate( static_cast<float>( iHP ) );
			}
			else
			{
				if ( pChar->m_sBasicSummonID == SNATIVEID( false ) )
				{
					m_pHPGauge->SetVisibleSingle( FALSE );
					m_pSummonImage->SetImage( "SUMMON_STATEICON_NONHIRE" );
					m_strTooltip = ID2GAMEINTEXT( "SUMMON_STATE_TOOLTIP_NONHIRE_TEXT" );
				}
				else
				{
					m_pHPGauge->SetVisibleSingle( FALSE );

					//PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pChar->m_sBasicSummonID );
					// 고용이 안된 경우;
					if ( !pCrowData )
					{
						
						m_pSummonImage->SetImage( "SUMMON_STATEICON_NONHIRE" );
						m_strTooltip = ID2GAMEINTEXT( "SUMMON_STATE_TOOLTIP_NONHIRE_TEXT" );
					}
					// 죽었을 경우, 소환 안된 경우;
					else
					{
						m_pSummonImage->SetImage( "SUMMON_STATEICON_DIE" );

						// 죽으면 상태 값을 초기화 시킨다;
						fAttackTime = 0.f;
						bAttackState = false;

						m_strTooltip = sc::string::format( ID2GAMEINTEXT( "SUMMON_STATE_TOOLTIP_HIRE_TEXT" ), m_strSummonName );
					}
				}
			}
		}
	}
}

void CSummonStateDisplayUI::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{	
	CUIGroupHelperNonClose::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ENotifyButton:
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			m_pInterface->SHOW_COMMON_LINEINFO(
				m_strTooltip.c_str(),
				NS_UITEXTCOLOR::WHITE,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );         
		}
		break;
	}
}

MySummonStateDisplayUI::MySummonStateDisplayUI ( GLGaeaClient* pClient
											   , CInnerInterface* pInterface
											   , EngineDeviceMan* pEngineDevice )
											   : CSummonStateDisplayUI( pClient
											   , pInterface
											   , pEngineDevice )
{

}

void MySummonStateDisplayUI::CreateUIWindowAndRegisterOwnership()
{
	CSummonStateDisplayUI::Create( SUMMON_STATE_DISPLAY_UI, "SUMMON_STATE_UI", UI_FLAG_LEFT|UI_FLAG_TOP );
	CSummonStateDisplayUI::CreateNonBackground( false, false );
	CSummonStateDisplayUI::CreateSubControl();
	CSummonStateDisplayUI::m_pInterface->UiRegisterControl( this );
	CSummonStateDisplayUI::m_pInterface->UiShowGroupFocus( SUMMON_STATE_DISPLAY_UI );
}

bool MySummonStateDisplayUI::IsVisible()
{
	return CSummonStateDisplayUI::IsVisible();
}