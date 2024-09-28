#include "StdAfx.h"
#include "../../../SigmaCore/String/StringFormat.h"

#include "SkillTimeUnit.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../Skill/SkillImage.h"
#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogic/Skill/SpecialEffect/GLSpecExperiencePoint.h"
#include "../../../RanLogic/Skill/GLSkill.h"
#include "../../../RanLogic/GLStringTable.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSkillTimeUnit::CSkillTimeUnit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface	  ( pInterface )
    , m_sSKILLID	  ( NATIVEID_NULL() )
    , m_wSKILLLEVEL   ( 0 )
    , m_wSKILLSLOT    ( 0 )
    , m_bENABLE       ( FALSE )
    , m_bDETAILVISBLE ( FALSE )
{
}

CSkillTimeUnit::~CSkillTimeUnit ()
{
}

void CSkillTimeUnit::CreateSubControl ()
{
	CSkillImage* pSkillImage = new CSkillImage(m_pGaeaClient, m_pEngineDevice);
	pSkillImage->CreateSub ( this, "SKILLTIME_IMAGE", UI_FLAG_DEFAULT, SKILLTIMEUNIT_1 );	
	pSkillImage->SetUseRender ( TRUE );
	RegisterControl ( pSkillImage );
	m_pSkillImage = pSkillImage;

	CUIControl* pProgressBack = new CUIControl(m_pEngineDevice);
	pProgressBack->CreateSub ( this, "SKILLTIME_BACK", UI_FLAG_DEFAULT, SKILLTIMEUNIT_2 );
	RegisterControl ( pProgressBack );

	CBasicProgressBar* pProgressBar = new CBasicProgressBar(m_pEngineDevice);
	pProgressBar->CreateSub ( this, "SKILLTIME_PROGRESS", UI_FLAG_DEFAULT, SKILLTIMEUNIT_3 );
	pProgressBar->CreateOverImage ( "SKILLTIME_PROGRESS_OVERIMAGE");
	pProgressBar->SetType ( CBasicProgressBar::VERTICAL );
	RegisterControl ( pProgressBar );
	m_pProgressBar = pProgressBar;
	
	m_sSKILLID		= NATIVEID_NULL();
	m_wSKILLLEVEL   = 0;
	m_wSKILLSLOT    = 0;
	m_bENABLE       = FALSE;
	m_bDETAILVISBLE = FALSE;
}

void CSkillTimeUnit::SetLeftTime ( const float& fLeftTime )
{
	m_fLEFT_TIME = fLeftTime;
}

void CSkillTimeUnit::SetLifeTime ( const float& fLifeTime )
{
    m_fLIFE_TIME = fLifeTime;
}

void CSkillTimeUnit::SetAdditional( const DWORD dwOption, const float fTimeLength )
{
	m_sSKILLID    = dwOption;

	m_fLIFE_TIME = fTimeLength;
	m_pSkillImage->SetAdditional( dwOption );
	m_pSkillImage->SetVisibleSingle( TRUE );
//	m_strSkillName = strSkillName;
}

void CSkillTimeUnit::SetSkill ( const DWORD& dwID, const WORD& wLevel, const WORD& wSlot, const float& fLifeTime, const CString& strSkillName )
{
	m_sSKILLID    = dwID;
	m_wSKILLLEVEL = wLevel;
	m_wSKILLSLOT  = wSlot;

	m_fLIFE_TIME = fLifeTime;
	m_pSkillImage->SetSkill ( dwID );
	m_pSkillImage->SetVisibleSingle ( TRUE );

	m_strSkillName = strSkillName;
}

void CSkillTimeUnit::SetEnable ( const BOOL bEnable )
{
	m_bENABLE = bEnable;
}

void CSkillTimeUnit::SetDetailVisible ( const BOOL bEnable )
{
	m_bDETAILVISBLE = bEnable;
}

void CSkillTimeUnit::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		NSGUI::SetCharMoveBlock();

		if ( m_bDETAILVISBLE )
		{
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

			SNATIVEID sNativeID ( m_sSKILLID );
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );

			if ( pCharacter && pSkill )
			{	
				CString strName;
				strName = pSkill->GetName();

				CString strDesc;
				pSkill->GetDescFormatByLevel( strDesc, m_wSKILLLEVEL, GLSKILL::DESC_FORMAT_IMPACT | GLSKILL::DESC_FORMAT_SPEC );

				CString strAccum;
				for ( DWORD i=0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
				{
					SKILL::SSPEC_ADDON* pSpec = &pSkill->m_sAPPLY.vecSPEC[i];

					if ( !pSpec )
						continue;

					if ( pSpec->emSPEC == EMSPECA_RANDOM_EXP_RATE ) 
					{
						const SEXPERIENCERATE& experienceRate(pCharacter->m_ExperienceRate);
						const DWORD nIncreasePercent(DWORD(experienceRate.getTotalRate() * 100.0f));
						strDesc.Format(ID2GAMEINTEXT("SKILLFACT_ACCUM_COUNT_PERCENT"), nIncreasePercent);

						const DWORD nMaxAccumCount(experienceRate.getMaxSize());
						const DWORD nNowAccumCount(experienceRate.size());
						strAccum.Format(ID2GAMEINTEXT("SKILLFACT_ACCUM_COUNT"), nNowAccumCount, nMaxAccumCount);
					}
				}

				CString strTime;

                CTime cCurTime( m_pGaeaClient->GetCurrentTime() );
                m_pGaeaClient->GetConvertServerTime( cCurTime );

				SSKILLFACT::GetTimeByStringFormat(
                    strTime,
                    cCurTime.GetTime(), 
                    SSKILLFACT::STIME(pCharacter->GetSkillFact(m_wSKILLSLOT)),
                    SSKILLFACT::EMTIMESTRINGFORMAT_SIMPLE);

				strTime = " " + strTime;
				strTime = ID2GAMEINTEXT("SKILLFACT_LIFETIME") + strTime;

				m_pInterface->BEGIN_COMMON_LINEINFO_MULTI( TEXT_ALIGN_LEFT );
				m_pInterface->ADD_COMMON_LINEINFO_MULTI( strName, NS_UITEXTCOLOR::GREENYELLOW );
				if ( !strDesc.IsEmpty() )
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( strDesc, NS_UITEXTCOLOR::WHITE );
				if ( !strAccum.IsEmpty() )
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( strAccum, NS_UITEXTCOLOR::GREENYELLOW );

				if ( m_bENABLE )
				{
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( strTime, NS_UITEXTCOLOR::WHITE );
				}
				else
				{
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( strTime, NS_UITEXTCOLOR::RED );
					m_pInterface->ADD_COMMON_LINEINFO_MULTI( ID2GAMEINTEXT("SKILLFACT_DISABLE"), NS_UITEXTCOLOR::RED );
				}

				m_pInterface->END_COMMON_LINEINFO_MULTI();
			}
		}
		else
		{
			m_pInterface->SHOW_COMMON_LINEINFO ( m_strSkillName, NS_UITEXTCOLOR::GREENYELLOW );
		}

		if ( CHECK_RB_UP_LIKE ( dwMsg ) )
		{
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

			SNATIVEID sNativeID ( m_sSKILLID );
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID );

			if ( pCharacter && pSkill )
			{
				
				//if ( static_cast<EMSKILLCLASS>(pSkill->m_sBASIC.sNATIVEID.wMainID) == EMSKILL_LUNCHBOX )
				//{
				PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
				BOOL bLunchBoxForbid = pLand ? pLand->IsLunchBoxForbid() : TRUE;
				if ( bLunchBoxForbid )
				{
					m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::RED, sc::string::format( ID2GAMEINTEXT("LUNCHBOX_DISABLE_RELEASE"), pSkill->GetName() ));						
					return;
				}
				//}
				// Note : 버프만 취소할 수 있게 한다.
				if ( pSkill->m_sBASIC.emACTION_TYPE == SKILL::EMACTION_TYPE_BUFF )
				{
					m_pInterface->ModalWindowSetModalData ( (int)m_sSKILLID.dwID, (int)m_wSKILLSLOT );
					m_pInterface->DoModal ( sc::string::format( ID2GAMEINTEXT("SKILLFACT_ASK_RESET"), pSkill->GetName() ).c_str(), UI::MODAL_QUESTION, UI::YESNO, UI::MODAL_SKILLFACT_REMOVE );
					return;
				}
			}
		}
	}

}

void CSkillTimeUnit::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	if ( m_pProgressBar )
	{        
        const float fPercent = m_fLIFE_TIME < 0.00001f ? 0.0f : m_fLEFT_TIME / m_fLIFE_TIME;
		m_pProgressBar->SetPercent ( fPercent );
	}

	if ( m_bENABLE )
	{
		m_pSkillImage ->SetDiffuse(DIFFUSE_ENABLE);
		m_pProgressBar->SetDiffuse(DIFFUSE_ENABLE);
	}
	else
	{
		m_pSkillImage ->SetDiffuse(DIFFUSE_DISABLE);
		m_pProgressBar->SetDiffuse(DIFFUSE_DISABLE);
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}
