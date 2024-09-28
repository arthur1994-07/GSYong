#include "StdAfx.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/DxTools/d3dfont.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/Item/ItemDurability.h"
#include "../../../RanLogic/RandomOption/RandomOption.h"

//#include "../../../RanLogicClient/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Pet/GLPetClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"
#include "../../../RanLogicClient/Input/GLInputManager.h"

#include "../../InnerInterface.h"
#include "../Item/ItemImage.h"
#include "./ItemInfoTooltip.h"
#include "../../RanLogic/Item/ItemSet/GLItemSet.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CItemInfoTooltip::ShowColorBox()
{
	if( m_pItemImage == NULL || m_pColorBox == NULL ) return;

	const UIRECT& rcImagePos =
		m_pItemImage->GetGlobalPos ();
	const UIRECT& rcImageLocalPos =
		m_pItemImage->GetLocalPos ();


	m_pColorBox->SetLocalPos ( rcImageLocalPos );
	m_pColorBox->SetGlobalPos ( rcImagePos );
	m_pColorBox->SetVisibleSingle ( TRUE );
}

void CItemInfoTooltip::HideColorBox()
{
	if( m_pColorBox == NULL ) return;

	m_pColorBox->SetVisibleSingle(FALSE);
}

void CItemInfoTooltip::SetInformationLabel2_cs( const SITEMCUSTOM& sItemCustom )
{
	std::string strTemp = "";

	// 코스튬 부착 아이템 옵션
	const ITEM::SSUIT* psDisguiseSuit = NULL;
	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
	if( NULL != pDisguise )
		psDisguiseSuit = &pDisguise->sSuitOp;

    if ( m_bIsWEAR_ITEM )
        m_pItemLabel2->AddText( ID2GAMEWORD("WEAR_ITEM"), ITEM_INFOR_COLOR::dwEquipCostum );

    // Info : 타입.
    {
        switch ( m_pItemData->BasicType() )
        {
        case ITEM_SUIT:
            {
                if ( m_pItemData->Suit() == SUIT_HANDHELD )
                {
                    // 무기 일때.
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						strTemp = sc::string::format( "%s(%s)",
							ID2GAMEWORD( "ITEM_BASIC_INFO", 11 ),
							CGameTextMan::GetInstance().GetCommentText("ITEMATTACK", m_pItemData->sSuitOp.emAttack).GetString() );
					}
					else
					{
						strTemp = sc::string::format( "%s(%s)",
							ID2GAMEWORD( "ITEM_BASIC_INFO", 11 ),
							COMMENT::ITEMATTACK[ m_pItemData->sSuitOp.emAttack ] );
					}
                    m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
                }
                else
                {
                    // 무기가 아닌 복장류 일때.
					if ( m_pItemData->sSuitOp.IsSTATE(EMITEM_SUIT_UNIQUE) == true )
					{
						// 무기 일때.
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							strTemp = sc::string::format( "%s(%s)",
								CGameTextMan::GetInstance().GetCommentText("ITEMSUIT", m_pItemData->sSuitOp.emSuit).GetString(),
								ID2GAMEWORD("ITEM_BASIC_INFO", 19));
						}
						else
						{
							strTemp = sc::string::format( "%s(%s)",
								COMMENT::ITEMSUIT[ m_pItemData->sSuitOp.emSuit ].c_str(),
								ID2GAMEWORD("ITEM_BASIC_INFO", 19));
						}
					}
					else
					{
						if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						{
							strTemp = sc::string::format( "%s",
								CGameTextMan::GetInstance().GetCommentText("ITEMSUIT", m_pItemData->sSuitOp.emSuit).GetString());
						}
						else
						{
							strTemp = sc::string::format( "%s",
								COMMENT::ITEMSUIT[ m_pItemData->sSuitOp.emSuit ].c_str());
						}
					}
                    int nIndex = m_pItemLabel2->AddText(strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault);				
					
                    // 코스튬 체크.
                    if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
                    {
                        const SITEM* pDisguiseData = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
                        if ( !pDisguiseData )   return;

                        strTemp = sc::string::format( "%s :%s", ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ), pDisguiseData->GetName() );			
                        m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );

                        if ( pDisguiseData->IsColorChange() )
                        {
                            strTemp = sc::string::format( "(%s %s)",
                                ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ),
                                ID2GAMEWORD ( "ITEM_SUIT_COLOR_CHANGE", 0 ) );

                            m_pItemLabel2->AddString( nIndex,
                                strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );
                        }
                        else
                        {
                            strTemp = sc::string::format( "(%s)",
                                ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ) );

                            m_pItemLabel2->AddString( nIndex,
                                strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );
                        }
                    }
                    //else if ( m_pItemData->sBasicOp.dwFlags & ITEM_DISGUISE )
					else if ( sItemCustom.IsDisguise() )
                    {
                        if ( m_pItemData->IsColorChange() )
                        {
                            strTemp = sc::string::format( "(%s %s)",
                                ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ),
                                ID2GAMEWORD ( "ITEM_SUIT_COLOR_CHANGE", 0 ) );

                            m_pItemLabel2->AddString( nIndex,
                                strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );
                        }
                        else
                        {
                            strTemp = sc::string::format( "(%s)",
                                ID2GAMEWORD( "ITEM_BASIC_INFO", 3 ) );

                            m_pItemLabel2->AddString( nIndex,
                                strTemp.c_str(), ITEM_INFOR_COLOR::dwEquipCostum );
                        }
                    }
                }
            }
            break;
		case ITEM_LOCK_BOX:
			{
				std::string strBasicType;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					strBasicType = m_pItemData->BasicType() < ITEM_NSIZE ? CGameTextMan::GetInstance().GetCommentText("ITEMTYPE", m_pItemData->BasicType()).GetString(): "";
				}
				else
				{
					strBasicType = COMMENT::ItemTypeName( m_pItemData->BasicType() ).c_str();
				}
				strBasicType.append(sItemCustom.bVietnamGainItem ? CGameTextMan::GetInstance().GetGameWord("LOCKBOX_TOOLTIP_LOCK", 0).GetString() : CGameTextMan::GetInstance().GetGameWord("LOCKBOX_TOOLTIP_LOCK", 1).GetString());
				m_pItemLabel2->AddText( strBasicType.c_str(), ITEM_INFOR_COLOR::dwDefault );				
			}
			break;

        default:
            {
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					m_pItemLabel2->AddText( m_pItemData->BasicType() < ITEM_NSIZE ? CGameTextMan::GetInstance().GetCommentText("ITEMTYPE", m_pItemData->BasicType()).GetString():"",
						ITEM_INFOR_COLOR::dwDefault );
				}
				else
				{
					m_pItemLabel2->AddText( COMMENT::ItemTypeName( m_pItemData->BasicType() ).c_str(),
						ITEM_INFOR_COLOR::dwDefault );
				}
            }
        }
    }

	GLCharacter* const pChar = m_pGaeaClient->GetCharacter();
	EMCHARINDEX charIdx = GLCI_NUM_ETC;
	int nLevel = -1;
	if ( pChar )
	{
		charIdx = pChar->GETCHARINDEX();
		nLevel = pChar->GETLEVEL();
	}

    // Info : 내구도
    {
        // 내구도 기능 사용 여부
        if ( GLUseFeatures::GetInstance().IsUsingItemDurability() )
        {   
            if ( m_pItemData->BasicType() == ITEM_SUIT )
            {
                // 내구도가 설정된 장비라면
                if( ItemDurability::GetInstance().IsUse(sItemCustom) )
                {
                    CString strDurability;
                    strDurability.Format( "%s :", ID2GAMEWORD( "ITEM_BASIC_INFO", 18 ) );
                    int nIndex = m_pItemLabel2->AddText( strDurability.GetString(), ITEM_INFOR_COLOR::dwDefault );
					
					// 내구도 컬러
					DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
					if( m_fDurabilityPerformance < 1.0f )
						dwColor = ItemDurability::GetInstance().GetTextColor(sItemCustom, dwColor);

                    strDurability.Format("%d/%d (%d%%)", (int)ItemDurability::GetInstance().Get(sItemCustom), (int)ItemDurability::GetInstance().GetMax(sItemCustom), ItemDurability::GetInstance().GetPercent(sItemCustom) );
                    m_pItemLabel2->AddString( nIndex, strDurability.GetString(), dwColor );
                }
            }
        }
    }

	bool bGenItemToCs = false;

	if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
	{
		SITEMCUSTOM sCustomeItem(sItemCustom.nidDISGUISE);

		if( sCustomeItem.IsDisguise() ) {
			bGenItemToCs = true;
		}
	}
	
	// Info : 공격
	{		
		//	공격력

		GLPADATA& sDamage = sItemCustom.getdamage(0.0f, charIdx, nLevel);
		const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID()); 

		int nMinDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wLow ); 
		int nMaxDmg = static_cast<int>( pTempITEM.sSuitOp.gdDamage.wHigh );
		int nRndOptDamage = static_cast<int>( sDamage.wLow - pTempITEM.sSuitOp.gdDamage.wLow );
		int nExtraValue = static_cast<int>( sItemCustom.GETGRADE_DAMAGE() );
		BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_DAMAGE);

		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{

				nRndOptDamage = static_cast<int>( nRndOptDamage * m_fDurabilityPerformance );
				nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityPerformance );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				GLDWDATA& damage = sItemCustom.GETDAMAGE(charIdx, nLevel);
				nMinDmg = static_cast<int>(damage.nNow * m_fDurabilityPerformance) - nRndOptDamage - nExtraValue;
				nMaxDmg = static_cast<int>(damage.nMax * m_fDurabilityPerformance) - nRndOptDamage - nExtraValue;
			}
		}

		DWORD dwDefault = ITEM_INFOR_COLOR::dwDefault;
		DWORD dwInchant1 = ITEM_INFOR_COLOR::dwInchant1;
		DWORD dwInchant2 = ITEM_INFOR_COLOR::dwInchant2;

		if( !bGenItemToCs ) {
			nMinDmg = 0;  nMaxDmg = 0; nRndOptDamage = 0; nExtraValue = 0; uGRADE = 0;
		}

		//VAR_PARAM 함수에서 1 이하일때 1로 잡아주는 코드가 있어서 튤팁에서 조건 검사를 한번 더 해줘야함
		if ( nMinDmg || nMaxDmg || nRndOptDamage || nExtraValue || uGRADE )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;

			if ( uGRADE )
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 0) );
				int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nMinDmg );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDamage > 0 )
				{				
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				if ( nExtraValue > 0 )
					strTemp = sc::string::format( "(+%d)", nExtraValue );
				else if ( nExtraValue < 0 )
					strTemp = sc::string::format( "(%d)", nExtraValue );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant2 );

				m_pItemLabel2->AddString( nIndex, " ~ ", dwColor );

				strTemp = sc::string::format( "%d", nMaxDmg );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDamage > 0)
				{			
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				if ( nExtraValue > 0 )
					strTemp = sc::string::format( "(+%d)", nExtraValue );
				else if ( nExtraValue < 0 )
					strTemp = sc::string::format( "(%d)", nExtraValue );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant2 );

			}
			else
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 0) );
				int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nMinDmg );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDamage > 0)
				{			
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				m_pItemLabel2->AddString( nIndex, " ~ ", dwColor );

				strTemp = sc::string::format( "%d", nMaxDmg );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDamage > 0)
				{			
					strTemp = sc::string::format( "(+%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDamage < 0)
				{				
					strTemp = sc::string::format( "(%d)", nRndOptDamage );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

			}
		}
	}

	// 방어
	{
		const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID()); 

		COSTUMEUSERVALUE::sCostumeUser::sInvest rTemp;

		int iDefCsValue = 0;

		if( sItemCustom.costumeUser.getTypeInfo( (BYTE)E_DEFENCE, rTemp ) ) {
			
			const COSTUMESTATSDATA::sPsData* pData = COSSDATA.GetStatInfo(pTempITEM.sSuitOp.emSuit, E_DEFENCE);

			if( pData ) {
				iDefCsValue = (short)(rTemp.wInvestPt*pData->fInc);
			}
		}
		
		//여기서 해당 아이템 정보가져옴

		//	방어력
		int nBasicDef = static_cast<int>( pTempITEM.sSuitOp.nDefense);
		int nRndOptDef = static_cast<int>( sItemCustom.getdefense(charIdx, nLevel) - pTempITEM.sSuitOp.nDefense);
		int nExtraValue = sItemCustom.GETGRADE_DEFENSE();
		BYTE uGRADE = sItemCustom.GETGRADE(EMGRINDING_DEFENSE);

		if( !bGenItemToCs )
		{
			nBasicDef  = iDefCsValue;
			nRndOptDef = static_cast<int>( sItemCustom.getdefense(charIdx, nLevel) - iDefCsValue);

			iDefCsValue = 0;
		}

		DWORD dwDefault = ITEM_INFOR_COLOR::dwDefault;
		DWORD dwInchant1 = ITEM_INFOR_COLOR::dwInchant1;
		DWORD dwInchant2 = ITEM_INFOR_COLOR::dwInchant2;

		//내구도 적용 시
		if( m_fDurabilityPerformance < 1.0f )
		{
			//nBasicDef = static_cast<int>( nBasicDef * m_fDurabilityPerformance );
			nRndOptDef = static_cast<int>( nRndOptDef * m_fDurabilityPerformance );
			nExtraValue = static_cast<int>( nExtraValue * m_fDurabilityPerformance );

			// 실제 적용 수치와 튤팁 출력간의 오차 보정
			nBasicDef = static_cast<int>(sItemCustom.GETDEFENSE(charIdx, nLevel) * m_fDurabilityPerformance) - nRndOptDef - nExtraValue;
		}		

		DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;

		if( iDefCsValue > 0 ) dwColor = ITEM_INFOR_COLOR::dwAddDisguise;

		if ( nBasicDef || uGRADE || nRndOptDef )
		{
			if ( uGRADE )
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 1) );
				int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nBasicDef );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDef > 0)
				{
					strTemp = sc::string::format( "(+%d)", nRndOptDef );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDef < 0)
				{
					strTemp = sc::string::format( "(%d)", nRndOptDef );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}

				if ( nExtraValue > 0 )
				{
					strTemp = sc::string::format( "(+%d)", nExtraValue );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant2 );
				}
				else if ( nExtraValue < 0 )
				{
					strTemp = sc::string::format( "(%d)", nExtraValue );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant2 );
				}

			}
			else
			{
				strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 1) );
				int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

				strTemp = sc::string::format( "%d", nBasicDef );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

				if(nRndOptDef > 0)
				{
					strTemp = sc::string::format( "(+%d)", nRndOptDef );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
				else if(nRndOptDef < 0)
				{
					strTemp = sc::string::format( "(%d)", nRndOptDef );
					m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwInchant1 );
				}
			}
		}

	}


	// Info : 명중률, 회피율, 소모 SP
	{
		DWORD dwDefault = ITEM_INFOR_COLOR::dwDefault;
		DWORD dwRndOptColor = ITEM_INFOR_COLOR::dwInchant1;

		const SITEM pTempITEM = *GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID()); 

		//	명중률
		int nBasicHitRate = static_cast<int>( pTempITEM.sSuitOp.nHitRate );
		int nRndOptHitRate = static_cast<int>( sItemCustom.GETHITRATE() - pTempITEM.sSuitOp.nHitRate );

		if( !bGenItemToCs )
		{
			nBasicHitRate  = 0;
			nRndOptHitRate = static_cast<int>( sItemCustom.GETHITRATE() );
		}

		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptHitRate -= nBasicHitRate;

				nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurabilityPerformance );	

				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptHitRate += nBasicHitRate;

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicHitRate = static_cast<int>( sItemCustom.GETHITRATE() * m_fDurabilityPerformance ) - nRndOptHitRate;				
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//nBasicHitRate = static_cast<int>( nBasicHitRate * m_fDurabilityPerformance );
				nRndOptHitRate = static_cast<int>( nRndOptHitRate * m_fDurabilityPerformance );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicHitRate = static_cast<int>(sItemCustom.GETHITRATE() * m_fDurabilityPerformance) - nRndOptHitRate;
			}
		}

		if ( nBasicHitRate ||  nRndOptHitRate )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->nHitRate )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 3) );
			int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

			strTemp = sc::string::format( "%d", nBasicHitRate );
			m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

			if(nRndOptHitRate > 0)
			{	
				strTemp = sc::string::format( "(+%d)", nRndOptHitRate );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
			else if(nRndOptHitRate < 0)
			{
				strTemp = sc::string::format( "(%d)", nRndOptHitRate );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
		}

		//	회피율
		int nBasicAvoidRate = static_cast<int>( pTempITEM.sSuitOp.nAvoidRate );
		int nRndOptAvoidRate = static_cast<int>( sItemCustom.GETAVOIDRATE() - pTempITEM.sSuitOp.nAvoidRate );

		if( !bGenItemToCs )
		{
			nBasicAvoidRate  = 0;
			nRndOptAvoidRate = sItemCustom.GETAVOIDRATE();
		}

		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptAvoidRate -= nBasicAvoidRate;

				//nBasicAvoidRate = static_cast<int>( nBasicAvoidRate * m_fDurabilityPerformance );
				nRndOptAvoidRate = static_cast<int>( (nRndOptAvoidRate -  psDisguiseSuit->nAvoidRate) * m_fDurabilityPerformance ); 

				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptAvoidRate += nBasicAvoidRate;

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicAvoidRate = static_cast<int>(sItemCustom.GETAVOIDRATE() * m_fDurabilityPerformance) - nRndOptAvoidRate;		
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//nBasicAvoidRate = static_cast<int>( nBasicAvoidRate * m_fDurabilityPerformance );
				nRndOptAvoidRate = static_cast<int>( nRndOptAvoidRate * m_fDurabilityPerformance ); 

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicAvoidRate = static_cast<int>(sItemCustom.GETAVOIDRATE() * m_fDurabilityPerformance) - nRndOptAvoidRate;	
			}
		}

		if ( nBasicAvoidRate ||  nRndOptAvoidRate )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->nAvoidRate )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 4) );
			int nIndex =  m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

			strTemp = sc::string::format( "%d", nBasicAvoidRate );
			m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

			if(nRndOptAvoidRate > 0)
			{	
				strTemp = sc::string::format( "(+%d)", nRndOptAvoidRate );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
			else if(nRndOptAvoidRate < 0)
			{
				strTemp = sc::string::format( "(%d)", nRndOptAvoidRate );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
		}

		//	소모 SP
		int nBasicReqSP = static_cast<int>( pTempITEM.sSuitOp.wReqSP );
		int nRndOptReqSP = static_cast<int>( sItemCustom.GETREQ_SP() - pTempITEM.sSuitOp.wReqSP);

		//코스튬 적용 시 
		if( NULL != psDisguiseSuit )
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//코스튬 기본 옵션은 합성 될 때 랜덤 옵션으로 통합 되므로 빼준다(내구도 적용 안됨)
				nRndOptReqSP -= static_cast<int>( psDisguiseSuit->wReqSP );

				//nBasicReqSP = static_cast<int>( nBasicReqSP * m_fDurabilityPerformance );
				nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurabilityPerformance ); 

				//코스튬 옵션은 랜덤 옵션에 포함됨
				nRndOptReqSP += static_cast<int>( psDisguiseSuit->wReqSP );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicReqSP = static_cast<int>( sItemCustom.GETREQ_SP()* m_fDurabilityPerformance ) - nRndOptReqSP;
			}
		}	
		else
		{
			//내구도 적용 시
			if( m_fDurabilityPerformance < 1.0f )
			{
				//nBasicReqSP = static_cast<int>( nBasicReqSP * m_fDurabilityPerformance );
				nRndOptReqSP = static_cast<int>( nRndOptReqSP * m_fDurabilityPerformance );

				// 실제 적용 수치와 튤팁 출력간의 오차 보정
				nBasicReqSP = static_cast<int>(sItemCustom.GETREQ_SP() * m_fDurabilityPerformance) - nRndOptReqSP;
			}
		}

		if ( nBasicReqSP || nRndOptReqSP )
		{
			DWORD dwColor = ITEM_INFOR_COLOR::dwDefault;
			if( NULL != psDisguiseSuit )
			{
				if( 0 != psDisguiseSuit->wReqSP )
					dwColor = ITEM_INFOR_COLOR::dwAddDisguise;
			}

			strTemp = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO", 7)  );
			int nIndex = m_pItemLabel2->AddText( strTemp.c_str(), dwColor );

			strTemp = sc::string::format( "%d", nBasicReqSP );
			m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwColor );

			if(nRndOptReqSP > 0)
			{	
				strTemp = sc::string::format( "(+%d)", nRndOptReqSP );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
			else if(nRndOptReqSP < 0)
			{
				strTemp = sc::string::format( "(%d)", nRndOptReqSP );
				m_pItemLabel2->AddString( nIndex, strTemp.c_str(), dwRndOptColor );
			}
		}
	}

    // Info : 약품류 사용 효과
    {
        if ( m_pItemData->BasicType() == ITEM_CURE )
        {
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				if( m_pItemData->sDrugOp.dwCureVolume == 0 )
					strTemp = sc::string::format( "%s", CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pItemData->sDrugOp.emDrug).GetString() );
				else
					strTemp = sc::string::format( "%s :%d",CGameTextMan::GetInstance().GetCommentText("ITEMDRUG", m_pItemData->sDrugOp.emDrug).GetString(), m_pItemData->sDrugOp.dwCureVolume );
			}
			else
			{
				if( m_pItemData->sDrugOp.dwCureVolume == 0 )
					strTemp = sc::string::format( "%s", COMMENT::ITEMDRUG[m_pItemData->sDrugOp.emDrug].c_str() );
				else
					strTemp = sc::string::format( "%s :%d",COMMENT::ITEMDRUG[m_pItemData->sDrugOp.emDrug].c_str(), m_pItemData->sDrugOp.dwCureVolume );
			}

            if ( m_pItemData->sDrugOp.bRatio )
            {
                strTemp += "%";
            }

            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
    }

    // Info : 재사용 시간
    {
        //  재사용 시간
        if ( m_pItemData->sBasicOp.IsCoolTime() )
        {
            std::string strTime = "";
            CTimeSpan cCoolTime( m_pItemData->sBasicOp.dwCoolTime );

            if ( cCoolTime.GetHours() > 0 )
                strTime += sc::string::format( "%d%s ", cCoolTime.GetHours(), ID2GAMEWORD("ITEM_BASIC_TIME", 0) );
            if ( cCoolTime.GetMinutes() > 0 )	
                strTime += sc::string::format( "%d%s ", cCoolTime.GetMinutes(), ID2GAMEWORD("ITEM_BASIC_TIME", 1) );
            if ( cCoolTime.GetSeconds() > 0 )	
                strTime += sc::string::format( "%d%s ", cCoolTime.GetSeconds(), ID2GAMEWORD("ITEM_BASIC_TIME", 2) );

            strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 8), strTime );			
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }

        //	남은시간
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        if ( pCharacter && pCharacter->IsCoolTime( m_pItemData->sBasicOp.sNativeID ) )
        {
            std::string strTime = "";
            __time64_t tCurTime =  m_pGaeaClient->GetCurrentTime().GetTime();
            __time64_t tCoolTime = pCharacter->GetMaxCoolTime( m_pItemData->sBasicOp.sNativeID );				

            CTimeSpan cReTime( tCoolTime - tCurTime );

            if ( cReTime.GetHours() > 0 )	
                strTime += sc::string::format( "%d%s ", cReTime.GetHours(), ID2GAMEWORD("ITEM_BASIC_TIME", 0) );
            if ( cReTime.GetMinutes() > 0 )	
                strTime += sc::string::format( "%d%s ", cReTime.GetMinutes(), ID2GAMEWORD("ITEM_BASIC_TIME", 1) );
            if ( cReTime.GetSeconds() > 0 )	
                strTime += sc::string::format( "%d%s ", cReTime.GetSeconds(), ID2GAMEWORD("ITEM_BASIC_TIME", 2) );

            strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_BASIC_INFO", 9), strTime );			
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }

		if( m_pItemData->sDrugOp.dwERList > 0 )
		{
			std::string str = "";

			for( int i = 0; i < ITEM_DRUG_ER_SIZE; ++i )
			{
				DWORD dwValue = 1 << i;

				if( dwValue & m_pItemData->sDrugOp.dwERList )
				{
					if( i > 0 )
						str += " + ";

					EMITEM_DRUG_ER eType = (EMITEM_DRUG_ER)i;

					switch( eType )
					{
					case ITEM_DRUG_ER_STUN        : str += sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_SET_BONUS", 26) ); break;
					case ITEM_DRUG_ER_HALLUCINATE : str += sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_SET_BONUS", 49) ); break;
					case ITEM_DRUG_ER_NON_DRUG    : str += sc::string::format( "%s", ID2GAMEWORD("ITEM_INFO_SET_BONUS", 34) ); break;
					}
				}
			}

			if( !str.empty() )
			{
				str = sc::string::format( "%s : %s", ID2GAMEWORD("ITEM_BASIC_INFO", 21), str );
				m_pItemLabel2->AddText( str.c_str(), ITEM_INFOR_COLOR::dwDefault );
			}
		}
    }

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( m_sNpcNativeID );

	EMITEM_TYPE emType = m_pItemData->BasicType();

	bool bKeyTip = false;

	if ( pCrowData && m_iPageIndex >= 0 && m_pWishListItem->GetTotalLine() == 0 )
	{
		GLInventorySale* rInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven( m_iPageIndex ));

		EEXTYPE exType = rInven->GetExChangeType(sItemCustom.GetNativeID(), m_wPosX, m_wPosY);

		if( exType == EX_ITEM_TYPE && m_pWishItem == NULL ) 
			bKeyTip = true;

	}

	switch(emType)
	{
	case ITEM_SKILL :
	case ITEM_PET_SKILL:
	case ITEM_GRINDING:
	case ITEM_VEHICLE:
	case ITEM_PET_CARD:
	case ITEM_LUNCHBOX:
	case ITEM_PET_DUALSKILL:
		{
			if(bKeyTip ){
				strTemp = ID2GAMEINTEXT("HELP_SHOW_ITEM_TOOLTIP");
				m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
			}
		}
		break;
	}
	
    switch ( emType )
    {
    case ITEM_SKILL:
        {			
            DisplaySkillItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_PET_SKILL:
        {
            DisplayPetSkillItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_GRINDING:
        {
            //m_pItemLabel2->AddText( ID2GAMEWORD( "ITEM_CATEGORY_EX", 0 ), ITEM_INFOR_COLOR::dwDefault );
        }
        break;

    case ITEM_VIETNAM_EXPGET:
        {
            if ( m_pItemData->sDrugOp.bRatio )
                strTemp = sc::string::format( ID2GAMEWORD("ITEM_CATEGORY_VIETNAM_EXP", 1) , m_pItemData->sDrugOp.dwCureVolume );
            else
                strTemp = sc::string::format( ID2GAMEWORD("ITEM_CATEGORY_VIETNAM_EXP", 0), m_pItemData->sDrugOp.dwCureVolume );

            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
        break;

    case ITEM_VIETNAM_ITEMGET:
        {
            strTemp = sc::string::format( ID2GAMEWORD("ITEM_CATEGORY_VIETNAM_ITEM", 0) , m_pItemData->sDrugOp.dwCureVolume );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
        break;

    case ITEM_VEHICLE:
        {
            DisplayVehicleItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_PET_CARD:
        {
            DisplayPetCardItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_LUNCHBOX:
        {
            DisplayLunchBoxItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_PET_DUALSKILL:
        {
            DisplayPetDualSkillItem( m_pItemLabel2, sItemCustom );
        }
        break;

    case ITEM_POINT_CARD_REFUND:
    case ITEM_POINT_CARD_NOT_REFUND:
        {
            // Note : 포인트 양 표시.
            strTemp = sc::string::format( ID2GAMEWORD("POINT_CARD_POINT_VOLUME"), m_pItemData->sDrugOp.dwCureVolume );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }
        break;

    case ITEM_RANDOMITEM:
    case ITEM_BOX:
        strTemp = ID2GAMEINTEXT("HELP_SHOW_ITEM_TOOLTIP");
        m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        break;
    };

	
    // Info : 그 외.
    EMITEM_TYPE emItemType = m_pItemData->BasicType();

    if( emItemType != ITEM_PET_SKILL && emItemType != ITEM_PET_DUALSKILL
        && emItemType != ITEM_POINT_CARD_REFUND && emItemType != ITEM_POINT_CARD_NOT_REFUND )
    {
        // Note : 시귀/직귀 카드에 맵이름 좌표 출력
        if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_REGEN )
        {
            m_pItemLabel2->AddText( ID2GAMEWORD( "ITEM_BASIC_INFO", 17 ), ITEM_INFOR_COLOR::dwDefault );

            GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();	

            CString strName( m_pGaeaClient->GetMapName( pCharacter->m_sStartMapID ) );
            if( strName == _T("(null)") ) strName.Empty();

            strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_CALL_MAP", 0 ), strName );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );

            SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode( pCharacter->m_sStartMapID );
            if( pMapNode )
            {
                GLLevelFileClient cLevelFile(m_pGaeaClient);
                BOOL bOk = cLevelFile.LoadFile( pMapNode->m_LevelFile.c_str(), NULL );
                if( bOk )
                {	
                    D3DXVECTOR3 vStartPos;
                    PDXLANDGATE pGate = cLevelFile.GetLandGateMan()->FindLandGate( pCharacter->m_dwStartGate );
                    if( pGate )
                    {
                        vStartPos = pGate->GetGenPos( DxLandGate::GEN_RENDUM );
                    }
                    else
                    {
                        pGate = cLevelFile.GetLandGateMan()->FindLandGate( DWORD(0) );
                        if( pGate ) vStartPos = pGate->GetGenPos( DxLandGate::GEN_RENDUM );
                    }

                    int nPosX(0), nPosY(0);
                    cLevelFile.GetMapAxisInfo().Convert2MapPos( vStartPos.x, vStartPos.z, nPosX, nPosY );

                    strTemp = sc::string::format( "%s :%d,%d", ID2GAMEWORD("ITEM_CALL_MAP", 1 ), nPosX, nPosY );
                    m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
                }
            }
        }
        else if( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_LASTCALL )
        {
            m_pItemLabel2->AddText( ID2GAMEWORD( "ITEM_BASIC_INFO", 17 ), ITEM_INFOR_COLOR::dwDefault );

            GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

            if( pCharacter->m_sLastCallMapID.IsValidNativeID() )
            {
                CString strName( m_pGaeaClient->GetMapName( pCharacter->m_sLastCallMapID ) );
                if( strName == _T("(null)") ) strName.Empty();

                strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_CALL_MAP", 0 ), strName );
                m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );

                SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode( pCharacter->m_sLastCallMapID );
                if( pMapNode )
                {
                    GLLevelFileClient cLevelFile(m_pGaeaClient);
                    BOOL bOk = cLevelFile.LoadFile( pMapNode->m_LevelFile.c_str(), NULL );
                    if( bOk )
                    {	
                        int nPosX(0), nPosY(0);
                        cLevelFile.GetMapAxisInfo().Convert2MapPos( pCharacter->m_vLastCallPos.x, pCharacter->m_vLastCallPos.z, nPosX, nPosY );

                        strTemp = sc::string::format( "%s :%d,%d", ID2GAMEWORD("ITEM_CALL_MAP", 1 ), nPosX, nPosY );
                        m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
                    }
                }
            }
            else
            {
                m_pItemLabel2->AddText( ID2GAMEWORD("ITEM_CALL_MAP", 2 ), ITEM_INFOR_COLOR::dwReqWrong );
            }
        }
        else if ( m_pItemData->sDrugOp.emDrug == ITEM_DRUG_CALL_TELEPORT )
        {
            m_pItemLabel2->AddText( ID2GAMEWORD( "ITEM_BASIC_INFO", 17 ), ITEM_INFOR_COLOR::dwDefault );

            CString strName( m_pGaeaClient->GetMapName( m_pItemData->sBasicOp.sSubID ) );
            if( strName == _T("(null)") ) strName.Empty();

            strTemp = sc::string::format( "%s :%s", ID2GAMEWORD("ITEM_TELEPORT_MAP", 0 ), strName );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );

            strTemp = sc::string::format( "%s :%d,%d", ID2GAMEWORD("ITEM_TELEPORT_MAP", 1 ), m_pItemData->sBasicOp.wPosX, m_pItemData->sBasicOp.wPosY );
            m_pItemLabel2->AddText( strTemp.c_str(), ITEM_INFOR_COLOR::dwDefault );
        }        
    }
	// Info : 랜덤 수치
	{
		if( sItemCustom.IsSetRandOpt() )
		{
			DWORD dwRandomAbilityColor = ITEM_INFOR_COLOR::dwRandomAbility;

			{
				int nRandOptType = sItemCustom.GETOptTYPE1();
				float fVal = sItemCustom.GETOptVALUE1() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
				bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

				SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
			}
			{
				int nRandOptType = sItemCustom.GETOptTYPE2();
				float fVal = sItemCustom.GETOptVALUE2() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
				bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

				SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
			}
			{
				int nRandOptType = sItemCustom.GETOptTYPE3();
				float fVal = sItemCustom.GETOptVALUE3() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
				bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

				SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
			}
			{
				int nRandOptType = sItemCustom.GETOptTYPE4();
				float fVal = sItemCustom.GETOptVALUE4() * COMMENT::ITEM_RANDOM_OPT_SCALE[nRandOptType];
				bool bIsPerRandOpt = sItemCustom.IsPerRandOpt( nRandOptType );

				SetInformationLabel3_RandomOption( nRandOptType, fVal, bIsPerRandOpt, dwRandomAbilityColor );
			}
		}
	}
	//

    // 아이템 미리보기
    if ( m_bUsePreview )
    {
        if ( m_pInterface->ItemPreviewWindowIsPreviewItem( sItemCustom ) )
        {
            m_pItemLabel2->AddText( ID2GAMEWORD("ITEM_USEPREVIEW_RN"), ITEM_INFOR_COLOR::dwDefault );
        }
    }
}

void CItemInfoTooltip::SetInformationLabel3_cs( const SITEMCUSTOM& sItemCustom )
{
	const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;

	std::string strText = "";

	int iGenItemToCs = 0;

	const SITEM* pItem = NULL; GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID());

	if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
	{
		SITEMCUSTOM sCustomeItem(sItemCustom.nidDISGUISE);

		if( sCustomeItem.IsDisguise() ) {
			iGenItemToCs = 1;
			pItem = GLogicData::GetInstance().GetItem(sCustomeItem.GetNativeID());
		}
	}
	else 
	{
		pItem = GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID());
	}

	if( pItem == NULL ) return;

	// Info : 가산 수치
	{		
		int arrVALUE[EMADD_SIZE];
		SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );
		
		for ( int i = 0; i < ITEM::SSUIT::ADDON_SIZE; ++i )
		{
			EMITEM_ADDON emTYPE = sSUIT.sADDON[i].emTYPE;

			if ( emTYPE != EMADD_NONE )
			{
				int nVALUE = static_cast<int>( sSUIT.sADDON[i].nVALUE * m_fDurabilityPerformance );

				nVALUE = nVALUE * iGenItemToCs;

				switch ( emTYPE )
				{
					//	밑에서 한번에 계산
				case EMADD_HP:
				case EMADD_MP:
				case EMADD_SP:
				case EMADD_MA:
					break;
				case EMADD_HITRATE:		arrVALUE[EMADD_HITRATE] += nVALUE;		break;
				case EMADD_AVOIDRATE:	arrVALUE[EMADD_AVOIDRATE] += nVALUE;	break;
				case EMADD_DAMAGE:		arrVALUE[EMADD_DAMAGE] += nVALUE;		break;
				case EMADD_DEFENSE:		arrVALUE[EMADD_DEFENSE] += nVALUE;		break;
				case EMADD_STATS_POW:	arrVALUE[EMADD_STATS_POW] += nVALUE;	break;
				case EMADD_STATS_STR:	arrVALUE[EMADD_STATS_STR] += nVALUE;	break;
				case EMADD_STATS_SPI:	arrVALUE[EMADD_STATS_SPI] += nVALUE;	break;
				case EMADD_STATS_DEX:	arrVALUE[EMADD_STATS_DEX] += nVALUE;	break;
				case EMADD_STATS_INT:	arrVALUE[EMADD_STATS_INT] += nVALUE;	break;
				case EMADD_STATS_STA:	arrVALUE[EMADD_STATS_STA] += nVALUE;	break;
				case EMADD_PA:			arrVALUE[EMADD_PA] += nVALUE;			break;
				case EMADD_SA:			arrVALUE[EMADD_SA] += nVALUE;			break;
				};
			}
		}

		//	전체 옵션을 따로 계산
		arrVALUE[EMADD_HP] = static_cast<int>( sItemCustom.GETADDHP() * m_fDurabilityPerformance * iGenItemToCs );
		arrVALUE[EMADD_MP] = static_cast<int>( sItemCustom.GETADDMP() * m_fDurabilityPerformance * iGenItemToCs );
		arrVALUE[EMADD_SP] = static_cast<int>( sItemCustom.GETADDSP() * m_fDurabilityPerformance * iGenItemToCs );
		arrVALUE[EMADD_MA] = static_cast<int>( sItemCustom.GETADDMA() * m_fDurabilityPerformance * iGenItemToCs );

		// 코스튬 능력치 ///////////////////////////////////////////////////////////////////////////////////////
		

		for( int i = 0; i < E_ADDED_MAX; ++i)
		{
			BYTE iType = (BYTE)(E_ADD_HITRATE+i);

			COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

			if( sItemCustom.costumeUser.getTypeInfo(iType, sData) )
			{
				double iValue = COSSDATA.GetStatInc(pItem->sSuitOp.emSuit, (COSTUME_STATS)(E_ADD_HITRATE+i));

				int nApplyValue = (int)(iValue*sData.wInvestPt);

				COSTUME_STATS eType = (COSTUME_STATS)(iType);

				switch( eType )
				{
				case E_ADD_HITRATE   : arrVALUE[EMADD_HITRATE]   += nApplyValue;	break;
				case E_ADD_AVOIDRATE : arrVALUE[EMADD_AVOIDRATE] += nApplyValue;	break;
				case E_ADD_DAMAGE    : arrVALUE[EMADD_DAMAGE]    += nApplyValue;	break;
				case E_ADD_DEFENSE   : arrVALUE[EMADD_DEFENSE]   += nApplyValue;	break;
				case E_ADD_HP        : arrVALUE[EMADD_HP]	     += nApplyValue;	break;
				case E_ADD_STATS_POW : arrVALUE[EMADD_STATS_POW] += nApplyValue;	break;
				case E_ADD_STATS_STA : arrVALUE[EMADD_STATS_STR] += nApplyValue;	break;
				case E_ADD_STATS_SPI : arrVALUE[EMADD_STATS_SPI] += nApplyValue;	break;
				case E_ADD_STATS_DEX : arrVALUE[EMADD_STATS_DEX] += nApplyValue;	break;
				case E_ADD_ENERGY    : arrVALUE[EMADD_MA]		 += nApplyValue;	break;
				case E_ADD_SHOOTING  : arrVALUE[EMADD_SA]		 += nApplyValue;	break;
				case E_ADD_MELEE     : arrVALUE[EMADD_PA]		 += nApplyValue;	break;
				}
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// 내구도 컬러
		DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;

		for( int i=1; i<EMADD_SIZE; ++i)
		{
			if( arrVALUE[i] != 0 )
			{
				strText = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADDON_INFO", i) );

				int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				if ( arrVALUE[i] > 0 ) {
					strText = sc::string::format( "+%d", arrVALUE[i] );
				}
				else if ( arrVALUE[i] < 0 ) {
					strText = sc::string::format( "%d", arrVALUE[i] );
				}

				m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
			}
		}
	}

	/******************************************************************************************************************************************************************************/

	// Info : 특수 기능
	{
		EMITEM_VAR emITEM_VAR = m_pItemData->sSuitOp.sVARIATE.emTYPE;
		EMITEM_VAR emITEM_VOL = m_pItemData->sSuitOp.sVOLUME.emTYPE;

		float arrVALUE[EMVAR_SIZE];
		SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );

		//	특수기능 ( 변화율 )
		//
		if ( emITEM_VAR != EMVAR_NONE )
		{
			switch ( emITEM_VAR )
			{
			case EMVAR_HP:
			case EMVAR_MP:
			case EMVAR_SP:
			case EMVAR_AP:
				break;
			default:
				arrVALUE[emITEM_VAR] = ( m_pItemData->sSuitOp.sVARIATE.fVariate * m_fDurabilityPerformance  * iGenItemToCs);
				break;
			};
		}

		arrVALUE[EMVAR_HP] = ( sItemCustom.GETINCHP() * m_fDurabilityPerformance * iGenItemToCs );
		arrVALUE[EMVAR_MP] = ( sItemCustom.GETINCMP() * m_fDurabilityPerformance * iGenItemToCs );
		arrVALUE[EMVAR_SP] = ( sItemCustom.GETINCSP() * m_fDurabilityPerformance * iGenItemToCs );
		arrVALUE[EMVAR_AP] = ( sItemCustom.GETINCAP() * m_fDurabilityPerformance * iGenItemToCs ); 

		//	코스튬 능력치
		float fCostumVolume = 0.0f;
		
		for( int i = 0; i < E_VARRATE_MAX; ++i)
		{
			BYTE iType = (BYTE)(E_R_HP_INC+i);

			COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

			if( sItemCustom.costumeUser.getTypeInfo(iType, sData) )
			{
				float fValue = (float)COSSDATA.GetStatInc(pItem->sSuitOp.emSuit, (COSTUME_STATS)(E_R_HP_INC+i));

				float fApplyValue = fValue*sData.wInvestPt;

				COSTUME_STATS eType = (COSTUME_STATS)iType;

				switch(eType)
				{
				case E_R_HP_INC      : arrVALUE[EMVAR_HP]            += fApplyValue; break;
				case E_R_MP_INC      : arrVALUE[EMVAR_MP]            += fApplyValue; break;
				case E_R_SP_INC      : arrVALUE[EMVAR_SP]            += fApplyValue; break;
				case E_R_HMSP_INC    : arrVALUE[EMVAR_AP]            += fApplyValue; break;
				case E_R_MOVESPEED   : arrVALUE[EMVAR_MOVE_SPEED]    += fApplyValue; break;
				case E_R_ATTACKSPEED : arrVALUE[EMVAR_ATTACK_SPEED]  += fApplyValue; break;
				case E_R_CRITICALHIT : arrVALUE[EMVAR_CRITICAL_RATE] += fApplyValue; break;
				case E_R_STRIKEHIT   : arrVALUE[EMVAR_CRUSHING_BLOW] += fApplyValue; break;
				}
			}
		}

		// 내구도 컬러 //이동 속도는 여기서 처리가 안됨
		DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;

		for ( int i = 1; i < EMVAR_SIZE; ++i )
		{
			if ( arrVALUE[i] != 0.0f ) 
			{
				if ( m_pItemData->BasicType() != ITEM_PET_FOOD
					&& m_pItemData->BasicType() != ITEM_VEHICLE_OIL )
					arrVALUE[i] *= COMMENT::ITEMVAR_SCALE[i];

				CString strTemp = sc::string::format( "%.2f", arrVALUE[i] ).c_str();

				CutZero(strTemp,2);

				strText = sc::string::format( "%s :", ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", i ) );
				int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp);

				if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
				{
					const SITEM* pNativeItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
					const SITEM* pDisguiseItem = GLogicData::GetInstance().GetItem( sItemCustom.nidDISGUISE );
					if( NULL != pNativeItem && NULL != pDisguiseItem )
					{
						//if( COMMENT::IsITEMVAR_SCALE( pDisguiseItem->sSuitOp.sVARIATE.emTYPE ) ||
						//	COMMENT::IsITEMVAR_SCALE( pNativeItem->sSuitOp.sVARIATE.emTYPE ) )
						strText += "%";
					}
				}
				else
				{
					//if ( COMMENT::IsITEMVAR_SCALE(emITEM_VAR) )
						strText += "%";
				}

				m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
			}
		}


		// 경험치 배율
		float fExpMultiple = m_pItemData->GetExpMultiple(); //내구도 적용

		if( iGenItemToCs == 0 )
			fExpMultiple = 1.f;
		
		//내구도 적용
		if( (fExpMultiple > 1.0f) || (m_fDurabilityPerformance < 1.0f) )
		{
			fExpMultiple = ( (fExpMultiple - 1.0f) * m_fDurabilityPerformance ) + 1.0f;
		}
		
		BYTE iType = (BYTE)(E_EXPMULTIPLE);

		COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

		if( sItemCustom.costumeUser.getTypeInfo(iType, sData) )
		{
			float fValue = (float)COSSDATA.GetStatInc(pItem->sSuitOp.emSuit, E_EXPMULTIPLE);
			float fApplyValue = fValue*sData.wInvestPt;

			fExpMultiple += fApplyValue;
			//fExpMultiple = max( fExpMultiple, 1.0f );
		}

		if ( fExpMultiple != 1.0f )
		{
			CString strTemp = sc::string::format( "%.2f", fExpMultiple * 100.0f - 100.0f ).c_str();

			CutZero(strTemp,2); 

			strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_EX", 0 ));
			int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

			strText = sc::string::format( "+%s%%", strTemp);
			m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
		}	

		//특수기능 ( 변화량 )
			EMITEM_VAR emDisguiseItemVol = EMVAR_NONE;
		if( NULL != pItem )
			emDisguiseItemVol = pItem->sSuitOp.sVOLUME.emTYPE;

		if( emITEM_VOL != emDisguiseItemVol && emITEM_VOL != EMVAR_NONE && emDisguiseItemVol != EMVAR_NONE )
		{
			float fVOLUME = 0.0f;
			int nIndex = 0;
			CString strTemp;

			// Basic--------------------------------------------
			if( emITEM_VOL == EMVAR_MOVE_SPEED )
				fVOLUME = sItemCustom.GETMOVESPEED() * m_fDurabilityPerformance;
			else
				fVOLUME = m_pItemData->sSuitOp.sVOLUME.fVolume * m_fDurabilityPerformance;

			if( 0.0f != fVOLUME )
			{
				strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
				CutZero( strTemp, 4 );
				strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ) );
				nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp );
				m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
			}

			// Disguise------------------------------------------------
			if( emDisguiseItemVol == EMVAR_MOVE_SPEED )
				fVOLUME = sItemCustom.GETMOVESPEED();
			else
				fVOLUME = fCostumVolume;

			if( 0.0f != fVOLUME )
			{
				strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
				CutZero( strTemp, 4 );
				strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emDisguiseItemVol ) );
				nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp );
				m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
			}			
		}
		else if( emITEM_VOL != EMVAR_NONE || emDisguiseItemVol != EMVAR_NONE )
		{
			float fVOLUME = 0.0f;
			if( emITEM_VOL == EMVAR_MOVE_SPEED || emDisguiseItemVol == EMVAR_MOVE_SPEED )
				fVOLUME = sItemCustom.GETMOVESPEED() * m_fDurabilityPerformance;
			else
				fVOLUME = (m_pItemData->sSuitOp.sVOLUME.fVolume * m_fDurabilityPerformance) + fCostumVolume;	//공격속도

			if( 0.0f != fVOLUME )
			{
				CString strTemp = sc::string::format( "%.4f", fVOLUME ).c_str();
				CutZero( strTemp, 4 );

				int nIndex = 0;

				if( emITEM_VOL == emDisguiseItemVol || emDisguiseItemVol == EMVAR_NONE )
					strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emITEM_VOL ) );
				else if( emITEM_VOL == EMVAR_NONE )
					strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_ADVANCED_INFO_SPECIAL_ADD_VOL", emDisguiseItemVol ) );

				nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

				strText = sc::string::format( "%s", strTemp );
				m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
			}
		}
	}

	// Info : 저항
	{
		float fRegist = 0.f;

		BYTE iType = (BYTE)(E_RESIST);

		COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

		if( sItemCustom.costumeUser.getTypeInfo(iType, sData) )
		{
			float fValue = (float)COSSDATA.GetStatInc(pItem->sSuitOp.emSuit, E_RESIST);

			fRegist = fValue*sData.wInvestPt;
		}

		//	기본 저항값
		int nELEC   = sSUIT.sResist.nElectric;
		int nFIRE   = sSUIT.sResist.nFire;
		int nICE    = sSUIT.sResist.nIce;
		int nPOISON = sSUIT.sResist.nPoison;
		int nSPIRIT = sSUIT.sResist.nSpirit;

		if( iGenItemToCs == 0 )
		{
			nELEC = fRegist; nFIRE = fRegist; nICE = fRegist; nPOISON = fRegist; nSPIRIT = fRegist;
		}

		//보너스 효과 분리
		int nAddELEC( static_cast<int>(sItemCustom.GETRESIST_ELEC() - nELEC) ); 
		int	nAddFIRE( static_cast<int>(sItemCustom.GETRESIST_FIRE() - nFIRE) ); 
		int	nAddICE( static_cast<int>(sItemCustom.GETRESIST_ICE() - nICE) ); 
		int	nAddPOISON( static_cast<int>(sItemCustom.GETRESIST_POISON() - nPOISON) ); 
		int	nAddSPIRIT( static_cast<int>(sItemCustom.GETRESIST_SPIRIT() - nSPIRIT) );

		DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;
		
		if( m_fDurabilityPerformance < 1.0f )
		{
			nELEC   = static_cast<int>( nELEC   * m_fDurabilityPerformance );
			nFIRE   = static_cast<int>( nFIRE   * m_fDurabilityPerformance );
			nICE    = static_cast<int>( nICE    * m_fDurabilityPerformance );
			nPOISON = static_cast<int>( nPOISON * m_fDurabilityPerformance );
			nSPIRIT = static_cast<int>( nSPIRIT * m_fDurabilityPerformance );

			nAddELEC   = static_cast<int>( nAddELEC   * m_fDurabilityPerformance );
			nAddFIRE   = static_cast<int>( nAddFIRE   * m_fDurabilityPerformance );
			nAddICE    = static_cast<int>( nAddICE    * m_fDurabilityPerformance );
			nAddPOISON = static_cast<int>( nAddPOISON * m_fDurabilityPerformance );
			nAddSPIRIT = static_cast<int>( nAddSPIRIT * m_fDurabilityPerformance );
		}

		if ( nELEC || nFIRE || nICE || nPOISON || nSPIRIT ||
			nAddELEC || nAddFIRE || nAddICE || nAddPOISON || nAddSPIRIT )
		{
			// 내구도 컬러
			DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;

			m_bDisplayResist = true;

			//strText = sc::string::format("%s :", ID2GAMEWORD ("ITEM_BASIC_INFO", 12) );
			strText = " ";
			m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

			int nIndex = 0;
			//if ( nSPIRIT )
			{
				strText = sc::string::format("%d", nSPIRIT);
				nIndex = m_pResistSpiritText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddSPIRIT )
				{
					strText = sc::string::format("(+%d)", nAddSPIRIT);
					m_pResistSpiritText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}

			//if ( nFIRE )
			{
				strText = sc::string::format("%d", nFIRE );
				nIndex = m_pResistFireText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddFIRE )
				{
					strText = sc::string::format("(+%d)", nAddFIRE);
					m_pResistFireText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}

			//if ( nICE )
			{
				strText = sc::string::format("%d", nICE );
				nIndex = m_pResistIceText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddICE )
				{
					strText = sc::string::format("(+%d)", nAddICE );
					m_pResistIceText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}

			//if ( nELEC )
			{
				strText = sc::string::format("%d", nELEC );
				nIndex = m_pResistElectricText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddELEC )
				{
					strText = sc::string::format("(+%d)", nAddELEC );
					m_pResistElectricText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}

			//if ( nPOISON )
			{
				strText = sc::string::format("%d", nPOISON );
				nIndex = m_pResistPoisonText->AddText( strText.c_str(), dwAddAbility );

				if ( nAddPOISON )
				{
					strText = sc::string::format("(+%d)", nAddPOISON );
					m_pResistPoisonText->AddString(nIndex, strText.c_str(), ITEM_INFOR_COLOR::dwElementalAbility );
				}
			}

			//추가 저항 증가 옵션이 없을 경우 출력 간격 조절 22는 임의의 숫자 보기 좋은 간격!
			if(nAddSPIRIT || nAddFIRE || nAddICE || nAddELEC || nAddPOISON)
				m_pResistTextGap = 20.0f;
			else
				m_pResistTextGap = 0.0f;
		}
	}
}

/*
void CItemInfoTooltip::SetInformationLabel3_cs( const SITEMCUSTOM& sItemCustom )
{
	const ITEM::SSUIT& sSUIT = m_pItemData->sSuitOp;

	std::string strText = "";

	const SITEM* pItem = GLogicData::GetInstance().GetItem(sItemCustom.GetNativeID());

	if( pItem == NULL ) return;

	{
		int arrVALUE[E_ADDED_MAX];
		SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );
		
		for( int i = 0; i < E_ADDED_MAX; ++i)
		{
			BYTE iType = (BYTE)(E_ADD_HITRATE+i);

			COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

			if( sItemCustom.costumeUser.getTypeInfo(iType, sData) )
			{
				double iValue = COSSDATA.GetStatInc(pItem->sSuitOp.emSuit, (COSTUME_STATS)(E_ADD_HITRATE+i));

				arrVALUE[i] = (int)(iValue*sData.wInvestPt*m_fDurabilityPerformance);
			}
		}

		DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;

		const char* pszTitle = NULL;

		for( int i = 0; i < E_ADDED_MAX; ++i)
		{
			if( arrVALUE[i] > 0 )
			{
				COSTUME_STATS eType = (COSTUME_STATS)(i + E_ADD_HITRATE);

				switch( eType )
				{
				case E_ADD_HITRATE   : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 1);  break;
				case E_ADD_AVOIDRATE : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 2);  break;
				case E_ADD_DAMAGE    : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 3);  break;
				case E_ADD_DEFENSE   : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 4);  break;
				case E_ADD_HP        : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 5);  break;
				case E_ADD_STATS_POW : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 8);  break;
				case E_ADD_STATS_STA : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 9);  break;
				case E_ADD_STATS_SPI : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 10); break;
				case E_ADD_STATS_DEX : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 11); break;
				case E_ADD_ENERGY    : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 16); break;
				case E_ADD_SHOOTING  : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 15); break;
				case E_ADD_MELEE     : pszTitle = ID2GAMEWORD("ITEM_ADDON_INFO", 14); break;
				default : pszTitle = NULL; break;
				}

				if( pszTitle != NULL )
				{
					strText = sc::string::format( "%s :", pszTitle );

					int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

					strText = sc::string::format( "+%d", arrVALUE[i] );

					m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
				}
			}
		}
	}
	
	// Info : 특수 기능
	{
		EMITEM_VAR emITEM_VAR = m_pItemData->sSuitOp.sVARIATE.emTYPE;
		EMITEM_VAR emITEM_VOL = m_pItemData->sSuitOp.sVOLUME.emTYPE;

		float arrVALUE[E_VARRATE_MAX];
		SecureZeroMemory( arrVALUE, sizeof(arrVALUE) );

		for( int i = 0; i < E_VARRATE_MAX; ++i)
		{
			BYTE iType = (BYTE)(E_R_HP_INC+i);

			COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

			if( sItemCustom.costumeUser.getTypeInfo(iType, sData) )
			{
				float fValue = (float)COSSDATA.GetStatInc(pItem->sSuitOp.emSuit, (COSTUME_STATS)(E_R_HP_INC+i));

				arrVALUE[i] = fValue*sData.wInvestPt*m_fDurabilityPerformance;
			}
		}

		
		//	코스튬 능력치
		float fCostumVolume = 0.0f;
		
		// 내구도 컬러 //이동 속도는 여기서 처리가 안됨
		DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;

		const char* pszTitle = NULL;

		for ( int i = 0; i < E_VARRATE_MAX; ++i )
		{
			if ( arrVALUE[i] > 0 ) 
			{
				COSTUME_STATS eType = (COSTUME_STATS)(i + E_R_HP_INC);

				CString strTemp = sc::string::format( "%.2f", arrVALUE[i] ).c_str();

				CutZero(strTemp,2);

				switch(eType)
				{
				case E_R_HP_INC      : pszTitle = ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", 1 ); break;
				case E_R_MP_INC      : pszTitle = ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", 2 ); break;
				case E_R_SP_INC      : pszTitle = ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", 3 ); break;
				case E_R_HMSP_INC    : pszTitle = ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", 4 ); break;
				case E_R_MOVESPEED   : pszTitle = ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", 5 ); break;
				case E_R_ATTACKSPEED : pszTitle = ID2GAMEWORD("ITEM_ADVANCED_INFO_SPECIAL_ADD", 6 ); break;
				case E_R_CRITICALHIT : pszTitle = ID2GAMEWORD("PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM", 22 ); break;
				case E_R_STRIKEHIT   : pszTitle = ID2GAMEWORD("PRIVATE_MARKET_ITEM_ADDON_VAR_RANDOM", 23 ); break;
				default : pszTitle = NULL; break;
				}
				
				if( pszTitle )
				{
					strText = sc::string::format( "%s :", pszTitle );
					int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

					strText = sc::string::format( "%s%%", strTemp);

					m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
				}				
			}
		}

		// 경험치 배율
		float fExpMultiple = 1.0f;

		BYTE iType = (BYTE)(E_EXPMULTIPLE);

		COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

		if( sItemCustom.costumeUser.getTypeInfo(iType, sData) )
		{
			float fValue = (float)COSSDATA.GetStatInc(pItem->sSuitOp.emSuit, E_EXPMULTIPLE);

			fExpMultiple += fValue*sData.wInvestPt;
		}

		if( (fExpMultiple > 1.0f) || (m_fDurabilityPerformance < 1.0f) )
		{
			fExpMultiple = ( (fExpMultiple - 1.0f) * m_fDurabilityPerformance ) + 1.0f;
		}

		if ( fExpMultiple != 1 )
		{
			CString strTemp = sc::string::format( "%.2f", fExpMultiple * 100.0f - 100.0f ).c_str();

			CutZero(strTemp,2);

			strText = sc::string::format( "%s :", ID2GAMEWORD( "CHARACTER_STATUS_STATIC", 20 ));
			int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

			strText = sc::string::format( "+%s%%", strTemp);
			m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
		}		
	}
	//
	// Info : 저항	
	{
		int iRegist = 0;

		BYTE iType = (BYTE)(E_RESIST);

		COSTUMEUSERVALUE::sCostumeUser::sInvest sData;

		DWORD dwAddAbility = ITEM_INFOR_COLOR::dwAddAbility;

		if( sItemCustom.costumeUser.getTypeInfo(iType, sData) )
		{
			int iValue = (int)COSSDATA.GetStatInc(pItem->sSuitOp.emSuit, E_RESIST);

			iRegist = iValue*sData.wInvestPt*m_fDurabilityPerformance;
		}

		if( iRegist > 0 )
		{
			CString strTemp = sc::string::format( "%d", iRegist ).c_str();

			strText = sc::string::format( "%s :", ID2GAMEWORD( "ITEM_INFO_SET_BONUS", 78 ));
			int nIndex = m_pItemLabel3->AddText( strText.c_str(), ITEM_INFOR_COLOR::dwAddAbility );

			strText = sc::string::format( "+%s", strTemp);
			m_pItemLabel3->AddString( nIndex, strText.c_str(), dwAddAbility );
		}

		m_pResistTextGap = 0.0f;
	}		
	

	//
}
*/