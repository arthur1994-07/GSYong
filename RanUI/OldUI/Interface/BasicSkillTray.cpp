#include "StdAfx.h"
#include "BasicSkillTray.h"
#include  "../../../RanLogic/GLSkillToolTip.h"
#include "BasicQuickSkillSlotEx.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../InnerInterface.h"
#include "../Skill/SkillWindowToTray.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../RanLogicClient/Tutorial/GLBaseTutorial.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"
#include "../../../RanLogicClient/RanParamClient.h"
#include "../../../SigmaCore/String/StringFormat.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



const	int	CBasicSkillTray::nLBUPSKIP = 1;

CBasicSkillTray::CBasicSkillTray(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pInterface(pInterface)
    , m_nLBUPSKIP(0)
    , m_pSkillInfo(NULL)
    , m_pSlotEnd(NULL)
{
    memset( m_pSlotEx, 0, sizeof ( m_pSlotEx ) );
}

CBasicSkillTray::~CBasicSkillTray()
{
}

void CBasicSkillTray::CreateSubControl( INT nTabIndex )
{
    GASSERT( ( 0 <= nTabIndex ) && ( nTabIndex < MAX_TAB_INDEX ) );

    CD3DFontPar * m_pFont8 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

    for ( int nNumber = 0; nNumber < QUICK_SKILL_SLOT_MAX; ++nNumber )
    {
        CString strKeyword;
        strKeyword.Format( "BASIC_QUICK_SKILL_TRAY_SLOT%d", nNumber );

        CBasicQuickSkillSlotEx*	pSlot = new CBasicQuickSkillSlotEx ( m_pGaeaClient, m_pInterface, m_pEngineDevice );
        pSlot->CreateSub( this, strKeyword.GetString(), UI_FLAG_DEFAULT, QUICK_SKILL_SLOT1 + nNumber );
        pSlot->CreateSubControl( nTabIndex );
        pSlot->CreateNumberText( m_pFont8, nNumber );		
        RegisterControl( pSlot );
        m_pSlotEx[nNumber] = pSlot;
    }

    /*
    CUIControl* pSlotEnd = new CUIControl(m_pRenderDevice);
    pSlotEnd->CreateSub( this, "BASIC_QUICK_SKILL_TRAY_SLOT_END" );
    pSlotEnd->SetControlNameEx( "슬롯 엔드" );
    RegisterControl( pSlotEnd );
    m_pSlotEnd = pSlotEnd;
    */

    CBasicVarTextBox* pSkillInfo = new CBasicVarTextBox(m_pEngineDevice);
    pSkillInfo->CreateSub( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    pSkillInfo->CreateSubControl();
    pSkillInfo->SetVisibleSingle( FALSE );
    RegisterControl( pSkillInfo );
    m_pSkillInfo = pSkillInfo;

	CBasicVarTextBox* pSkillInfoMulti = new CBasicVarTextBox(m_pEngineDevice);
	pSkillInfoMulti->CreateSub( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pSkillInfoMulti->CreateSubControl();
	pSkillInfoMulti->SetVisibleSingle( FALSE );
	pSkillInfoMulti->SetBlockMouseTracking( TRUE );
	RegisterControl( pSkillInfoMulti );
	m_pSkillInfoMulti = pSkillInfoMulti;

	// 링크스킬;
	for( int i=0; i<SKILL::MAX_ADDON; ++i )
	{
		CBasicVarTextBox* pSkillInfoLink = new CBasicVarTextBox(m_pEngineDevice);
		pSkillInfoLink->CreateSub( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pSkillInfoLink->CreateSubControl();
		pSkillInfoLink->SetVisibleSingle( FALSE );
		pSkillInfoLink->SetBlockMouseTracking( TRUE );
		RegisterControl( pSkillInfoLink );
		m_pSkillInfoLink[i] = pSkillInfoLink;
	}

	CBasicVarTextBox* pSkillInfoDealyAction = new CBasicVarTextBox(m_pEngineDevice);
	pSkillInfoDealyAction->CreateSub( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pSkillInfoDealyAction->CreateSubControl();
	pSkillInfoDealyAction->SetVisibleSingle( FALSE );
	pSkillInfoDealyAction->SetBlockMouseTracking( TRUE );
	RegisterControl( pSkillInfoDealyAction );
	m_pSkillInfoDealyAction = pSkillInfoDealyAction;

}

void CBasicSkillTray::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	bool bDelayActionSkill = FALSE;

    switch ( ControlID )
    {
    case QUICK_SKILL_SLOT1:
    case QUICK_SKILL_SLOT2:
    case QUICK_SKILL_SLOT3:
    case QUICK_SKILL_SLOT4:
    case QUICK_SKILL_SLOT5:
    case QUICK_SKILL_SLOT6:
    case QUICK_SKILL_SLOT7:
    case QUICK_SKILL_SLOT8:
    case QUICK_SKILL_SLOT9:
    case QUICK_SKILL_SLOT0:
        {			
            if ( CHECK_MOUSE_IN( dwMsg ) )
            {	
                int nG_Index = m_pSlotEx[0]->GetTabIndex() * QUICK_SKILL_SLOT_MAX;
                int nIndex = ControlID - QUICK_SKILL_SLOT1;
                nG_Index += nIndex;

                SNATIVEID sOverSkill = m_pGaeaClient->GetCharacter()->GetCharData().m_sSKILLQUICK[nG_Index];
                if ( sOverSkill != NATIVEID_NULL() )
                {
                    UpdateSkillInfo( sOverSkill );
                    m_pSkillInfo->SetVisibleSingle( TRUE );
					
					PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sOverSkill.wMainID, sOverSkill.wSubID );
					const SKILL::SAPPLY& sApply = pSkill->m_sAPPLY;
					if ( pSkill )	//	스킬이 존재할 경우
					{
						UIRECT rcSkillInfo = m_pSkillInfo->GetGlobalPos();
						float fPosY = rcSkillInfo.bottom;
						if ( pSkill->m_sEXT_DATA.idMultiTargetSkill !=NATIVEID_NULL() )
						{
							m_pSkillInfoMulti->RePosControl(static_cast<int>(rcSkillInfo.left) - 20, static_cast<int>(rcSkillInfo.bottom) - 20);
							m_pSkillInfoMulti->SetVisibleSingle( TRUE );

							fPosY += m_pSkillInfoMulti->GetGlobalPos().sizeY;
						}

						// 링크 스킬;
						for ( DWORD i = 0; i <  m_vecSkillInfoLinkID.size(); ++i )
						{
							m_pSkillInfoLink[i]->RePosControl(static_cast<int>(rcSkillInfo.left) - 20, static_cast<int>(fPosY) - 20);
							m_pSkillInfoLink[i]->SetVisibleSingle( TRUE );

							fPosY += m_pSkillInfoLink[i]->GetGlobalPos().sizeY;
						}

						SKILL::SAPPLY sApply = pSkill->m_sAPPLY;
						for ( DWORD i = 0; i < sApply.vecSPEC.size(); ++i )
						{

							if( sApply.vecSPEC[i].emSPEC == EMSPECA_DELAY_ACTION_SKILL )
							{
								bDelayActionSkill = TRUE;
							}
						}

						if ( bDelayActionSkill )
						{
							m_pSkillInfoDealyAction->RePosControl(static_cast<int>(rcSkillInfo.left) - 20, static_cast<int>(fPosY) - 20);
							m_pSkillInfoDealyAction->SetVisibleSingle( TRUE );

							fPosY += m_pSkillInfoDealyAction->GetGlobalPos().sizeY;
						}
					}
					
                }				

                //	스냅, 스킬 이미지 붙이기
                ISkillWindowToTray* pSkillWindowToTray= m_pInterface->GetSkillWindowToTray();
                if ( !pSkillWindowToTray )
                {
                    GASSERT ( 0 && "CSkillWindowToTray가 널입니다." );
                    return ;
                }

                if ( pSkillWindowToTray->GetSkill() != NATIVEID_NULL() )
                {
                    const UIRECT& rcSlotPos = m_pSlotEx[nIndex]->GetAbsPosSkillImage();
                    pSkillWindowToTray->SetUIWindowGlobalPos( rcSlotPos );						

                    AddMessageEx( UIMSG_MOUSEIN_SKILLSLOT );
                }		

                if ( (dwMsg & UIMSG_LB_UP)  )
                {
					if (!(m_pInterface->GetSkillTrayTab()->IsMoveLock()))
					{

						//! 손에 들고있는 스킬을 가져온다
						SNATIVEID sNativeID = m_pInterface->SkillWindowToTrayGetSkill();

						//	스킬 등록
						if ( sNativeID != NATIVEID_NULL() )
						{
							//슬롯에 있는 스킬을 가져와 백업 한다
							SNATIVEID sOverSkill = m_pGaeaClient->GetCharacter()->GetCharData().m_sSKILLQUICK[nG_Index];

							//슬롯이 비어있나?
							if ( sOverSkill != NATIVEID_NULL() )
							{
								//손에 들고 있는 스킬을 슬롯에 넣는다
								GLInputManager::GetInstance().ExternEvent_SkillTrayLeftClickUp( static_cast<WORD>(nG_Index) );	
								m_nLBUPSKIP = 0;

								//위에서 백업한 슬롯의 스킬을 손에 든다
								PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sOverSkill.wMainID, sOverSkill.wSubID );
								if ( !pSkill )
								{
									return ;
								}

								ISkillWindowToTray* pSkillWindowToTray = m_pInterface->GetSkillWindowToTray();
								if ( !pSkillWindowToTray ) 
									return;

								pSkillWindowToTray->SetSkill( pSkill->GetId() );
								m_pInterface->SkilSlotTrayOpen();
							}
							//슬롯이 비어있다면
							else
							{
								GLInputManager::GetInstance().ExternEvent_SkillTrayLeftClickUp( static_cast<WORD>(nG_Index) );	
								m_nLBUPSKIP = 0;
							}
						}
						else
						{
							//슬롯에 있는 스킬을 가져와 백업 한다
							SNATIVEID sOverSkill = m_pGaeaClient->GetCharacter()->GetCharData().m_sSKILLQUICK[nG_Index];
							//위에서 백업한 슬롯의 스킬을 손에 든다
							PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sOverSkill.wMainID, sOverSkill.wSubID );
							if ( !pSkill )
							{
								return ;
							}

							ISkillWindowToTray* pSkillWindowToTray = m_pInterface->GetSkillWindowToTray();
							if ( !pSkillWindowToTray ) 
								return;

							pSkillWindowToTray->SetSkill( pSkill->GetId() );
							m_pInterface->SkilSlotTrayOpen();

							//	스킬 해제
							m_pGaeaClient->GetCharacter()->ReqSkillQuickReSet( nG_Index );
						}	
					}
					else
					{
						GLInputManager::GetInstance().ExternEvent_SkillTrayLeftClickUp( static_cast<WORD>(nG_Index) );	
						m_nLBUPSKIP = 0;
					}
                }


                //	스킬 해제
                if ( dwMsg & UIMSG_RB_UP )
                {					
                    m_pGaeaClient->GetCharacter()->ReqSkillQuickReSet( nG_Index );
                }
            }
        }
        break;
    }
}

void	CBasicSkillTray::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if ( !IsVisible() ) return ;

    m_pSkillInfo->SetVisibleSingle( FALSE );
	m_pSkillInfoMulti->SetVisibleSingle( FALSE );
	m_pSkillInfoDealyAction->SetVisibleSingle( FALSE );

	for( int i=0; i<SKILL::MAX_ADDON; ++i )
	{
		if( m_pSkillInfoLink[i] )
			m_pSkillInfoLink[i]->SetVisibleSingle(FALSE);
	}

    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    DWORD dwMsg = GetMessageEx();
    if ( dwMsg & UIMSG_LB_UP )
    {
        ISkillWindowToTray* pSkillWindowToTray = m_pInterface->GetSkillWindowToTray();
        if ( !pSkillWindowToTray )	return ;

        //	스킬이 손에 들려져있고...
        //	첫번째 왼쪽클릭메시지는 무시한다.
        BOOL bExist = ( pSkillWindowToTray->GetSkill() != NATIVEID_NULL() );
        if ( bExist )
        {
            if ( nLBUPSKIP <= m_nLBUPSKIP )
            {
                pSkillWindowToTray->ResetSkill();
                m_nLBUPSKIP = 0;

                // 튜토리얼때 들고 있는 스킬 놓기.
                if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
                {
                    if ( m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
                        m_pGaeaClient->GetTutorial()->IsSubSixStep()
                        || m_pGaeaClient->GetTutorial()->IsQuickNActionSlotStep() &&
                        m_pGaeaClient->GetTutorial()->IsSubSevenStep() )
                    {
                        m_pGaeaClient->GetTutorial()->m_sHoldSkill = NATIVEID_NULL();
                    }
                }
                //
            }
            else
            {
                m_nLBUPSKIP++;
            }
        }		
    }

	//! 예약스킬 관련
	//---------------------------------------------------------------
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	if ( !pCharacter )
	{
		return;
	}

	static std::map<SNATIVEID, CBasicQuickSkillSlotEx*, SNATIVEID_MAP_PRED> s_mapSkillSlot;
	s_mapSkillSlot.clear();

	//! 먼저 스킬들을 map에 등록한다.
	for ( UINT i=0; i<QUICK_SKILL_SLOT_MAX; ++i )
	{
		if ( !m_pSlotEx[i] )
		{
			continue;
		}

		UINT nSkillIndex = static_cast<UINT>(m_pSlotEx[i]->GetTabIndex() * QUICK_SKILL_SLOT_MAX) + i;

		if ( pCharacter->m_sSKILLQUICK[nSkillIndex] != SNATIVEID(false) )
		{
			s_mapSkillSlot.insert( std::make_pair(pCharacter->m_sSKILLQUICK[nSkillIndex], m_pSlotEx[i]) );
		}
	}

	//! 예약스킬을 전부 꺼준다.
	SetVisibleReservedSkill( false );

	if ( RANPARAM::bSKILL_RESERVATION )
	{
		//! 예약된 스킬들을 돌면서 표시를 해준다.
		static std::vector<std::pair<CBasicQuickSkillSlotEx*, UINT> > s_vecSlot;
		s_vecSlot.clear();

		UINT nMaxReservedSlot  = 0;
		UINT nMaxReservedSkill = 0; 

		GLREACTION_ITER iterSkill = pCharacter->BeginReservedSkillReaction();
		GLREACTION_ITER iterEnd   = pCharacter->EndReactionIter();

		//! 돌면서 예약스킬로 사용할 슬롯들을 세팅한다.
		while ( iterSkill != iterEnd )
		{
			const GLREACTION_SKILL& sSkillReaction = (*iterSkill);
			const SNATIVEID			sSkillID	   = sSkillReaction.GetSKILLID();

			std::map<SNATIVEID, CBasicQuickSkillSlotEx*, SNATIVEID_MAP_PRED>::iterator finditer = s_mapSkillSlot.find( sSkillID );
			if ( finditer != s_mapSkillSlot.end() )
			{
				s_vecSlot.push_back( std::make_pair(finditer->second, nMaxReservedSkill) );
				++nMaxReservedSlot;
			}

			iterSkill = pCharacter->NextReservedSkillReaction(iterSkill);
			++nMaxReservedSkill;
		}

		//! 예약스킬로 등록된 슬롯들을 세팅한다.
		for ( UINT i=0; i<nMaxReservedSlot; ++i )
		{
			if ( !s_vecSlot[i].first )
			{
				continue;
			}

			s_vecSlot[i].first->SetVisibleReservedSkill( true );
			s_vecSlot[i].first->SetReservedSkillOrder( s_vecSlot[i].second, nMaxReservedSkill );
		}	
	}
}

void CBasicSkillTray::SetVisibleReservedSkill( const bool bEnable )
{
	for ( UINT i=0; i<QUICK_SKILL_SLOT_MAX; ++i )
	{
		if ( m_pSlotEx[i] )
		{
			m_pSlotEx[i]->SetVisibleReservedSkill( bEnable );
		}
	}
}

const UIRECT& CBasicSkillTray::GetSlotGlobalPos( const WORD wSlot )
{
	return m_pSlotEx[wSlot]->GetSkillImageGlobalPos();
}

void CBasicSkillTray::UpdateSkillInfo( SNATIVEID sNativeID )
{
    if ( sNativeID != NATIVEID_NULL() )
    {
        static SNATIVEID sNativeIDBack = NATIVEID_NULL();

        BOOL bUPDATE = FALSE;
        if ( sNativeID != sNativeIDBack )
        {			
            bUPDATE = TRUE;
        }

        if ( bUPDATE )
        {
            PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sNativeID.wMainID, sNativeID.wSubID );
            if ( pSkill )	//	스킬이 존재할 경우
            {
                SCHARSKILL* pCharSkill = m_pGaeaClient->GetCharacter()->GETLEARNED_SKILL( sNativeID );
                LoadSkillInfo( pSkill, pCharSkill );
            }

            sNativeIDBack = sNativeID;
        }		
    }
}

void CBasicSkillTray::LoadSkillInfo( PGLSKILL const pSkill, SCHARSKILL* const pCharSkill )
{
	if( !pCharSkill )
		return;

	WORD wolevel = pCharSkill->wLevel + 1;
	
	CString strTemp;
	strTemp.Format("%s(Lv.%d)", pSkill->GetName(), wolevel);
    m_pSkillInfo->SetTextNoSplit( strTemp , NS_UITEXTCOLOR::PALEGREEN );

	const SKILL::SAPPLY& sApply = pSkill->m_sAPPLY;

	if ( pSkill->m_sEXT_DATA.idMultiTargetSkill !=NATIVEID_NULL() )
	{
		SNATIVEID pSkillmultiId = pSkill->m_sEXT_DATA.idMultiTargetSkill;
		PGLSKILL pSkillmulti = GLSkillMan::GetInstance().GetData( pSkillmultiId.wMainID, pSkillmultiId.wSubID );
			strTemp.Format("%s - %s(Lv.%d)", ID2GAMEWORD( "MULTI_SKILL_TEXT" ),pSkillmulti->GetName(), wolevel );
		m_pSkillInfoMulti->SetTextNoSplit( strTemp, NS_UITEXTCOLOR::DODGERBLUE );

		//if ( GLUseFeatures::GetInstance().IsUsingSkillTooltip() )
		//	SetInformationSkillInfo( pSkillmulti, pCharSkill );
	}

	// 링크 스킬;
	ResetLinkSkillText();
	for ( DWORD i = 0; i <  sApply.vecSPEC.size(); ++i )
	{
		if( sApply.vecSPEC[i].emSPEC != EMSPECA_SKILL_LINK )
			continue;

		SNATIVEID sLinkID = sApply.vecSPEC[i].sSPEC[pCharSkill->wLevel].dwLinkID;
		if( sLinkID == NATIVEID_NULL() )
			continue;

		SNATIVEID sNativeID = sApply.vecSPEC[i].sSPEC[pCharSkill->wLevel].dwNativeID;
		if( sNativeID == NATIVEID_NULL() )
			continue;

		if( IsHaveSkill( sNativeID ) )
			continue;

		m_vecSkillInfoLinkID.push_back( sNativeID );
	}

	for ( DWORD i = 0; i <  m_vecSkillInfoLinkID.size(); ++i )
	{
		GLSKILL* pSNativeSkill = GLSkillMan::GetInstance().GetData( m_vecSkillInfoLinkID[i].wMainID, m_vecSkillInfoLinkID[i].wSubID );
		if( !pSNativeSkill )
			continue;

		strTemp.Format( "%s - %s(Lv.%d)", ID2GAMEWORD( "LINK_SKILL_TEXT" ), pSNativeSkill->GetName(), wolevel );
		m_pSkillInfoLink[i]->SetTextNoSplit( strTemp, NS_UITEXTCOLOR::DODGERBLUE );
	}

	BOOL bDelayActionSkill = FALSE;
	WORD wMid;
	WORD wSid;

	for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
	{

		if( sApply.vecSPEC[i].emSPEC == EMSPECA_DELAY_ACTION_SKILL )
		{
			bDelayActionSkill = TRUE;
			wMid = static_cast<WORD>(sApply.vecSPEC[i].sSPEC[ 0 ].fVAR1);
			wSid = static_cast<WORD>(sApply.vecSPEC[i].sSPEC[ 0 ].fVAR2);
		}
	}

	if ( bDelayActionSkill )
	{
		PGLSKILL pSkillDealyAction = GLSkillMan::GetInstance().GetData( wMid, wSid );
		if ( pSkillDealyAction )
		{
			strTemp.Format("%s - %s(Lv.%d)", ID2GAMEWORD( "DEALY_ACTION_SKILL_TEXT" ),pSkillDealyAction->GetName(), wolevel );
			m_pSkillInfoDealyAction->SetTextNoSplit( strTemp, NS_UITEXTCOLOR::DODGERBLUE );
		}
	}

	if ( GLUseFeatures::GetInstance().IsUsingSkillTooltip() )
		SetInformationSkillInfo( pSkill, pCharSkill );

	
}
void CBasicSkillTray::SetShotcutText( DWORD nID, CString& strTemp )
{
    m_pSlotEx[nID]->SetShotcutText(strTemp);	
}

void CBasicSkillTray::TurnHorizontal ()
{
    for ( int nNumber = 0; nNumber < QUICK_SKILL_SLOT_MAX; ++nNumber )
    {
        CString strKeyword;
        strKeyword.Format( "BASIC_QUICK_SKILL_TRAY_SLOT_H%d", nNumber );

        m_pSlotEx[nNumber]->ResetControl( GetGlobalPos(), strKeyword.GetString() );
        m_pSlotEx[nNumber]->TurnHorizontal();
    }

    //m_pSlotEnd->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY_SLOT_END_H" );
}

void CBasicSkillTray::TurnVertical ()
{
    for ( int nNumber = 0; nNumber < QUICK_SKILL_SLOT_MAX; ++nNumber )
    {
        CString strKeyword;
        strKeyword.Format( "BASIC_QUICK_SKILL_TRAY_SLOT%d", nNumber );

        m_pSlotEx[nNumber]->ResetControl( GetGlobalPos(), strKeyword.GetString() );
        m_pSlotEx[nNumber]->TurnVertical();
    }

    //m_pSlotEnd->ResetControl( GetGlobalPos(), "BASIC_QUICK_SKILL_TRAY_SLOT_END" );
}

void CBasicSkillTray::SetSkillInfoVisibleOff ()
{
    m_pSkillInfo->SetVisibleSingle( FALSE );
}

void CBasicSkillTray::ResetSlotText()
{
	for ( int nNumber = 0; nNumber < QUICK_SKILL_SLOT_MAX; ++nNumber )
	{
		m_pSlotEx[nNumber]->ResetText();
	}
}

void CBasicSkillTray::ResetLinkSkillText()
{
	for ( int nNumber = 0; nNumber < SKILL::MAX_ADDON; ++nNumber )
	{
		m_pSkillInfoLink[nNumber]->ClearText();
	}
	m_vecSkillInfoLinkID.clear();
}

bool CBasicSkillTray::IsHaveSkill( const SNATIVEID& sNativeID )
{
	for( unsigned int j=0; j<m_vecSkillInfoLinkID.size(); ++j )
	{
		if( m_vecSkillInfoLinkID[j] == sNativeID )
		{
			return true;
		}
	}

	return false;
}

void CBasicSkillTray::SetInformationSkillInfo( PGLSKILL const pSkill, SCHARSKILL* const pCharSkill )
{
    std::string strText = "";

    WORD wLevel = 0;

    if ( pCharSkill )
    {
        wLevel = pCharSkill->wLevel;
    }

    const SKILL::SAPPLY& sAPPLY = pSkill->m_sAPPLY;
    const SKILL::CDATA_LVL& sDATA_LVL = sAPPLY.sDATA_LVL[wLevel];

    SKILL::EMROLE emRole = pSkill->m_sBASIC.emROLE;


    // Info : 스킬 효과.
    {
        strText = GLSkillToolTip::MakeTooltipSting_In_Skill_Type( m_pGaeaClient->GetCharacter(), pSkill, wLevel );
        m_pSkillInfo->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
    }

    // Info : 부가 효과
    {
        std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Impact( pSkill, wLevel );

        BOOST_FOREACH( std::string& strText, vecString)
        {
            m_pSkillInfo->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
        }
    }

    // Info : 특수 효과
    {
        std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Special( pSkill, wLevel );

        BOOST_FOREACH( std::string& strText, vecString)
        {
            m_pSkillInfo->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
        }
    }

	// Info : 링크 스킬;
	{
		std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Link( pSkill, wLevel );

		BOOST_FOREACH( std::string& strText, vecString)
		{
			m_pSkillInfo->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
		}
	}

	if ( pSkill->m_sEXT_DATA.idMultiTargetSkill !=NATIVEID_NULL() )
	{
		SNATIVEID pSkillMulitId = pSkill->m_sEXT_DATA.idMultiTargetSkill;
		PGLSKILL pSkillMulit = GLSkillMan::GetInstance().GetData( pSkillMulitId.wMainID, pSkillMulitId.wSubID );
		SKILL::EMROLE emRole = pSkillMulit->m_sBASIC.emROLE;

		//IS 패시브 SKILL::EMROLE::EMROLE_PASSIVE 
		if ( emRole == SKILL::EMROLE_PASSIVE )
		{
			//strText = GLSkillToolTip::MakeTooltipSting_In_Skill_Type( m_pGaeaClient->GetCharacter(), pSkillMulit, wLevel );
			m_pSkillInfoMulti->AddTextNoSplit( ID2GAMEWORD( "SKILL_BASIC_INFO", 8 ), NS_UITEXTCOLOR::PALEGREEN );
		}
		// Info : 스킬 효과.
		{
			strText = GLSkillToolTip::MakeTooltipSting_In_Skill_Type( m_pGaeaClient->GetCharacter(), pSkillMulit, wLevel );
			m_pSkillInfoMulti->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
		}

		// Info : 부가 효과
		{
			std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Impact( pSkillMulit, wLevel );

			BOOST_FOREACH( std::string& strText, vecString)
			{
				m_pSkillInfoMulti->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
			}
		}

		// Info : 특수 효과
		{
			std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Special( pSkillMulit, wLevel );

			BOOST_FOREACH( std::string& strText, vecString)
			{
				m_pSkillInfoMulti->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
			}
		}
	}

	// 링크 스킬;
	for ( DWORD i = 0; i <  m_vecSkillInfoLinkID.size(); ++i )
	{
		SNATIVEID sNativeID = m_vecSkillInfoLinkID[i];
		if( sNativeID == NATIVEID_NULL() )
			continue;

		GLSKILL* pSNativeSkill = GLSkillMan::GetInstance().GetData( sNativeID.wMainID, sNativeID.wSubID );
		if( !pSNativeSkill )
			continue;

		SKILL::EMROLE emRole = pSNativeSkill->m_sBASIC.emROLE;

		//IS 패시브 SKILL::EMROLE::EMROLE_PASSIVE 
		if ( emRole == SKILL::EMROLE_PASSIVE )
		{
			//strText = GLSkillToolTip::MakeTooltipSting_In_Skill_Type( m_pGaeaClient->GetCharacter(), pSkillMulit, wLevel );
			m_pSkillInfoLink[i]->AddTextNoSplit( ID2GAMEWORD( "SKILL_BASIC_INFO", 8 ), NS_UITEXTCOLOR::PALEGREEN );
		}
		// Info : 스킬 효과.
		{
			strText = GLSkillToolTip::MakeTooltipSting_In_Skill_Type( m_pGaeaClient->GetCharacter(), pSNativeSkill, wLevel );
			m_pSkillInfoLink[i]->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
		}

		// Info : 부가 효과
		{
			std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Impact( pSNativeSkill, wLevel );

			BOOST_FOREACH( std::string& strText, vecString)
			{
				m_pSkillInfoLink[i]->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
			}
		}

		// Info : 특수 효과
		{
			std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Special( pSNativeSkill, wLevel );

			BOOST_FOREACH( std::string& strText, vecString)
			{
				m_pSkillInfoLink[i]->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
			}
		}
	}


	BOOL bDelayActionSkill = FALSE;
	WORD wMid, wSid;

	SKILL::SAPPLY sApply = pSkill->m_sAPPLY;
	for ( DWORD i = 0; i < sApply.vecSPEC.size(); ++i )
	{

		if( sApply.vecSPEC[i].emSPEC == EMSPECA_DELAY_ACTION_SKILL )
		{
			bDelayActionSkill = TRUE;
			wMid = static_cast<WORD>(sApply.vecSPEC[i].sSPEC[ 0 ].fVAR1);
			wSid = static_cast<WORD>(sApply.vecSPEC[i].sSPEC[ 0 ].fVAR2);
		}
	}

	if ( bDelayActionSkill )
	{
		PGLSKILL pSkillDealyAction = GLSkillMan::GetInstance().GetData( wMid, wSid);
		if (!pSkillDealyAction)
			return;
		SKILL::EMROLE emRole = pSkillDealyAction->m_sBASIC.emROLE;

		//IS 패시브 SKILL::EMROLE::EMROLE_PASSIVE 
		if ( emRole == SKILL::EMROLE_PASSIVE )
		{
			//strText = GLSkillToolTip::MakeTooltipSting_In_Skill_Type( m_pGaeaClient->GetCharacter(), pSkillMulit, wLevel );
			m_pSkillInfoDealyAction->AddTextNoSplit( ID2GAMEWORD( "SKILL_BASIC_INFO", 8 ), NS_UITEXTCOLOR::PALEGREEN );
		}
		// Info : 스킬 효과.
		{
			strText = GLSkillToolTip::MakeTooltipSting_In_Skill_Type( m_pGaeaClient->GetCharacter(), pSkillDealyAction, wLevel );
			m_pSkillInfoDealyAction->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
		}

		// Info : 부가 효과
		{
			std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Impact( pSkillDealyAction, wLevel );

			BOOST_FOREACH( std::string& strText, vecString)
			{
				m_pSkillInfoDealyAction->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
			}
		}

		// Info : 특수 효과
		{
			std::vector<std::string> vecString = GLSkillToolTip::MakeTooltipSting_In_Skill_Special( pSkillDealyAction, wLevel );

			BOOST_FOREACH( std::string& strText, vecString)
			{
				m_pSkillInfoDealyAction->AddTextNoSplit( strText.c_str(), SKILL_INFOR_COLOR::dwAddAbility );
			}
		}
	}
}
