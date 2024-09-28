#include "../pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Event/GLIPEvent.h"
#include "../../RanLogic/Msg/ServerMsg.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Party/GLPartyField.h"
#include "../Party/GLPartyTenderManagerField.h"
#include "../Util/GLItemLimit.h"
#include "./GLCrow.h"

#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	typedef std::vector<SDAMAGELOG>	VECDAMAGE;
	typedef VECDAMAGE::iterator		VECDAMAGE_ITER;

	bool damage_less ( const SDAMAGELOG &lhs, const SDAMAGELOG &rhs )
	{
		return lhs.dwDamage < rhs.dwDamage;
	}
};

namespace
{
	typedef std::vector<SDAMAGELOGPARTY> VECDAMAGEPARTY;
	typedef VECDAMAGEPARTY::iterator VECDAMAGEPARTY_ITER;

	bool party_damage_less ( const SDAMAGELOGPARTY &lhs, const SDAMAGELOGPARTY &rhs )
	{
		return lhs.m_dwDamage < rhs.m_dwDamage;
	}
};


//! �̺�Ʈ�� Ž�� ���� �ý��ۿ� ���� ����� ������ ������� ���Ѵ�.
float GLCrow::CalculateItemGenRate_20060417( float fGenRate, PGLCHAR pChar, bool bParty )
{
	float fEVENT_RATE = 1.0f; 

	float fRate = 1.0f; 

	if( !pChar ) 
	{
		fRate = fGenRate * fEVENT_RATE;
		return fRate;
	}

	fEVENT_RATE = m_pGaeaServer->GetItemGainRate(pChar->m_CHARINDEX,pChar->m_wLevel);
    fRate = fGenRate * fEVENT_RATE;


	EMSERVICE_PROVIDER sp = m_pGaeaServer->GetServiceProvider();
//	IP���ʽ�
//#if defined( TH_PARAM ) || defined( MYE_PARAM ) || defined(MY_PARAM) || defined( PH_PARAM ) //|| defined(_RELEASED)
	if (sp == SP_THAILAND || sp == SP_MALAYSIA || sp == SP_MALAYSIA_EN || sp == SP_PHILIPPINES)
	{
		if( pChar->m_emIPBonus == EMIP_BONUS_A || pChar->m_emIPBonus == EMIP_BONUS_B || pChar->m_emIPBonus == EMIP_BONUS_C ) 
		{
			fRate *= 2.0f;
		}
	}
//#endif 


#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	if( pChar->m_ChinaGainType == GAINTYPE_HALF )
	{
		fRate /= 2.0f;
	}else if( pChar->m_ChinaGainType == GAINTYPE_EMPTY )
	{
		fRate = 0.0f;
	}
#else
	if( pChar->m_bEventStart && pChar->m_bEventApply)
	{
		SEventState sEventState = m_pGaeaServer->m_sEventState;
		if( sEventState.bEventStart )
				fRate *= sEventState.fItemGainRate;
	}
#endif


	return fRate;
}

//! ���ʿ� ����� Ȯ�� ������ ���� Callback
float GLCrow::Callback_CalculateGenProb ( EMGENINFOTYPE emInfoType, float fDefaultProb, GLChar *pChar, bool bParty )
{
    float fProb = fDefaultProb;

    switch ( emInfoType )
    {
    case EMGENINFOTYPE_NONE:
        {
            fProb *= GLCONST_CHAR::GET_ITEM_SCALE(); 

            if ( !bParty )
            {
                fProb = CalculateItemGenRate_20060417( fProb, pChar, bParty );
            }

            if ( pChar )
            {
                fProb *= pChar->m_fGENITEM_RATE;

                if ( pChar->m_sEVENTFACT.IsIPEvent() )
                {
                    float IPEventItemRate = 1.0f + (ran_event::GLIPEvent::Instance()->GetItempDrop() * 0.01f);
                    fProb *= IPEventItemRate;
                }
            }
        }
        break;

    case EMGENINFOTYPE_ONLY_MONEY:
        {
			// '��� �Ӵ� Ȯ��'�� '�⺻ ��� �Ӵ� ������'�� ����Ǵ°� �´°ǰ�?;
			float fIncR_DropGenMoney = 0.f;
			if ( pChar )
			{
				fIncR_DropGenMoney = pChar->GetIncR_DropGenMoney();
			}

            fProb = GLCONST_CHAR::GET_MONEY_SCALE( fIncR_DropGenMoney ) * fProb;

            if ( pChar )
            {
                fProb *= pChar->m_fGENMONEY_RATE;

                if ( pChar->m_sEVENTFACT.IsIPEvent() )
                {
                    float IPEventMoneyRate = 1.0f + (ran_event::GLIPEvent::Instance()->GetMoneyDrop() * 0.01f);

                    fProb *= IPEventMoneyRate;
                }
            }
        }
        break;

    case EMGENINFOTYPE_ONLY_QITEM:
    case EMGENINFOTYPE_ONLY_ITEMSKILLFACT:
        {
            //blank
        }
        break;
    }

    return fProb;
}

//! �� �������� Ȯ���� ���� Callback
float GLCrow::Callback_CalculateGenItemRate ( EMGENITEMTYPE emItemType, float fDefaultRate, WORD wGrade, PGLCHAR pChar )
{
    float fRate = fDefaultRate;

    //! Ư�� ����� ������ Ȯ���� ��½�Ų��. (����� genitem���� ����)
    if ( wGrade == GLCONST_CHAR::wITEM_DROP_SPECIFIC_GRADE )
    {
        fRate *= GLCONST_CHAR::fITEM_DROP_SPECIFIC_GRADE_SCALE;
    }

    float fITEM_DROP_SPECIFIC_GRADE_SCALE = 1.0f;   //  Note : Ư�� ������ ��� �����.(����� genitem���� ����)
    WORD  wITEM_DROP_SPECIFIC_GRADE       = 0;      //  Note : ������� ������ų Ư�� �������� ���

    switch ( emItemType )
    {
    case EMGENITEMTYPE_ITEM:
        {
            //! ���� Ȯ���� ���� �������� Ȯ���� ���δ�.
            if ( fRate < 2.0f )
            {
                fRate *= GLCONST_CHAR::fITEM_DROP_LOWER_SCALE;

                if ( pChar )
                {
                    fRate *= pChar->m_fGENITEM_RATE;
                }
            }
        }
        break;

    case EMGENITEMTYPE_MONEY:
    case EMGENITEMTYPE_QITEM:
    case EMGENITEMTYPE_ITEMSKILLFACT:
        {
            //blank
        }
        break;
    };

    return fRate;
}

//! ���������� �߻��� �����ۿ� ���� Callback (�ݾ� Ȯ�� ��)
void GLCrow::Callback_CalculateGenResult ( OUT SGENRESULT& sResult, GLChar* const pChar )
{
    switch ( sResult.emType )
    {
    case EMGENRESULTTYPE_ITEM:
        {			
			//blank
        }
        break;

    case EMGENRESULTTYPE_MONEY:
        {
			float fIncR_DropGenMoney = 0.f;
			if ( pChar )
			{
				fIncR_DropGenMoney = pChar->GetIncR_DropGenMoney();
			}

            DWORD dwGemMoney = DWORD ( GLCONST_CHAR::GET_MONEY_SCALE( fIncR_DropGenMoney ) * sResult.Data.dwMoney * ( 1.0f + 0.3f*sc::Random::RANDOM_NUM() ) );
            sResult.Data.dwMoney = dwGemMoney;
        }
        break;
    };
}
/*

//! �߻��� �����ۿ� ���� �α� Callback
void GLCrow::Callback_GenItemLog( EMGENITEMTYPE emItemType, float fRate, SGENRESULT& sResult, PGLCHAR pChar, bool bParty, SNATIVEID sCrowID )
{
	if ( !pChar )
	{
		sc::writeLogDebug( "GLCrow::Callback_GenItemLog - pChar is null" );
		return;
	}

    switch ( sResult.emType )
    {
    case EMGENRESULTTYPE_ITEM:
        {
            sc::writeLogDebug( sc::string::format("GenItem [Type=%1%, FinalRate=%2%, UserRate=%3%, UserID=%4%, CrowID=(%5%,%6%), ItemID=(%7%,%8%), Party=%9%]"
                , emItemType
                , fRate
                , pChar->m_fGENITEM_RATE
                , pChar->GetUserID()
                , sCrowID.Mid()
                , sCrowID.Sid()
                , sResult.Data.sItemID.Mid()
                , sResult.Data.sItemID.Sid()
                , bParty ? 1 : 0));
        }
        break;

    case EMGENRESULTTYPE_MONEY:
        {
            sc::writeLogDebug( sc::string::format("GenMoney [FinalRate=%1%, UserRate=%2%, UserID=%3%, CrowID=(%4%,%5%), Money=%6%, Party=%7%]"
                , fRate
                , pChar->m_fGENMONEY_RATE
                , pChar->GetUserID()
                , sCrowID.Mid()
                , sCrowID.Sid()
                , sResult.Data.dwMoney
                , bParty ? 1 : 0));
        }
        break;
    }
}
*/

void GLCrow::GenerateReward()
{
    // ������ �ι��̻� �Ǵ� ���� [10/28/2013 hsshin]
    // �ѹ��̶� ������ �־��ٸ� return;
    if ( m_bUseReward )
        return;

    m_bUseReward = true;

#if defined( _ITEM_GEN_060417_H_ ) || defined( _QUESTION_ITEM_GEN_060417_H_ )
    GenerateReward_20060417();
    // GenerateReward_20060402();
    // GenerateReward_200502();
#else
    GenerateReward_20110722();
#endif
}

//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
// -> ItemGen�� callback ��� ����

#if defined( _ITEM_GEN_110722_H_ )
void GLCrow::GenerateReward_20110722()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    STARGETID mytarid(GETCROW(),m_dwGaeaID);
    DWORD dwOWNER(GAEAID_NULL);
	bool bBoss = false;
	if ( m_pCrowData )
		bBoss = static_cast< bool >( m_pCrowData->IsBoss() );

    if ((m_sAssault.emCrow == CROW_PC) && (m_sAssault.GaeaId != GAEAID_NULL))
    {
        PGLCHAR pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
        if (pChar)
        {
            pChar->ReceiveKillExp(mytarid, false, false, bBoss);
            pChar->ReceiveKillCP(mytarid);
			pChar->NotifyKillSomeone(m_dwGaeaID);
            dwOWNER = pChar->GetGaeaID();

			/*if( pChar->m_bTracingUser )
			{
				GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
				TracingMsg.nUserNum  = pChar->GetUserID();
				TracingMsg.SetAccount(pChar->m_szUID);

				CString strTemp;
				strTemp.Format(
					"Monster Kill!!, [%s][%s], Monster ID:mid[%d]sid[%d]", 
					pChar->m_szUID,
					pChar->m_szName,
					m_sNativeID.wMainID,
					m_sNativeID.wSubID);

				TracingMsg.SetLog(strTemp.GetString());

				m_pGaeaServer->SENDTOAGENT(&TracingMsg);
			}*/
        }
    }


    if ((m_sAssault.emCrow == CROW_SUMMON) && (m_sAssault.GaeaId != GAEAID_NULL))
    {
        PGLSUMMONFIELD pSummon = pLand->GetSummon(m_sAssault.GaeaId);
        if (pSummon)
        {
			if ( pSummon->m_sOwnerID.actorType == CROW_PC )
			{
				GLChar* pSummonOwner = static_cast< GLChar* >(
					m_pGaeaServer->GetActor(
					pLand,
					pSummon->m_sOwnerID ) );
				if (pSummonOwner)
				{
					pSummonOwner->ReceiveKillExp(mytarid, false, true, bBoss);
					pSummonOwner->ReceiveKillCP(mytarid);
					dwOWNER = pSummonOwner->GetGaeaID();				
				}
			}
        }
    }

    DWORD dwRefPartyMasterGaeaID = GAEAID_NULL;	
    
	// ��Ƽ������ ���;
	const DWORD dwRefPartyDamage = CalculateMaxPartyDamage(dwRefPartyMasterGaeaID);
	
    // Note : ��ȯ���� �� ������
    //        ���� ��Ȳ�� �Ǻ��Ͽ� �������� m_cDamageLog�� �ѱ��
    if (!m_cSummonDamageLog.empty())
    { 
        PGLCHAR        pChar    = NULL;
        DAMAGELOG_ITER iter     = m_cSummonDamageLog.begin();
        DAMAGELOG_ITER iter_end = m_cSummonDamageLog.end();

        for (; iter!=iter_end; ++iter)
        {
            SDAMAGELOG &sDamageLog = (*iter).second;

            // ��ȯ�� �α׿� ������ GaeaID�� ����ߴ�.
            pChar = m_pGaeaServer->GetChar(sDamageLog.dwGaeaID);

            if ( !pChar )
				continue;

            if ( !pChar->IsValidBody() )
				continue;

            if (sDamageLog.dwUserID != pChar->GetUserID())
				continue;

            if ( pChar->GetCurrentMap() != GetCurrentMap() )
				continue;

            D3DXVECTOR3 vDis = GetPosition() - pChar->GetPosition();
            float fDis = D3DXVec3Length ( &vDis );
            
			if (fDis > GLCONST_CHAR::fPARTYEXPRANGE)
				continue;

            AddDamageLog ( m_cDamageLog, pChar->GetGaeaID(), pChar->GetUserID(), sDamageLog.dwDamage );
        }
    }

    // ĳ���Ͱ� �� ������ ���
    DWORD dwCharGaeaID = GAEAID_NULL;
    DWORD dwCharDamage = 0;

    if (!m_cDamageLog.empty())
    {
        PGLCHAR pChar = NULL;
        DAMAGELOG_ITER iter = m_cDamageLog.begin ();
        DAMAGELOG_ITER iter_end = m_cDamageLog.end ();
        for (; iter!=iter_end; ++iter)
        {
            SDAMAGELOG &sDamageLog = (*iter).second;

            pChar = m_pGaeaServer->GetChar ( sDamageLog.dwGaeaID );

            if (!pChar)
                continue;
            if (sDamageLog.dwUserID != pChar->GetUserID())
                continue;

            pChar->ReceiveAttackExp(mytarid, sDamageLog.dwDamage, FALSE, false, false, bBoss);

            if (sDamageLog.dwDamage > dwCharDamage)
            {
                // ĳ�����߿� ���� ���� �������� �� ĳ���͸� ã�´�.
                dwCharGaeaID = pChar->GetGaeaID();
                dwCharDamage = sDamageLog.dwDamage;
                m_dwKillerDbNum = pChar->GetCharID();
            }
        }
    }

    // ��Ƽ�� ĳ���� �� ���� �������� �� �ʿ�
    // ������ �������� �ο��Ѵ�.
    if (dwCharDamage > dwRefPartyDamage)
    {
        dwOWNER = dwCharGaeaID;
    }
    else
    {
        dwOWNER = dwRefPartyMasterGaeaID;
    }

    m_cDamageLog.clear();
    m_cDamageLogParty.clear();	
    m_cSummonDamageLog.clear();

    GenerateRewardItem_20110722(dwOWNER);
}

const DWORD GLCrow::CalculateMaxPartyDamage(DWORD& dwRefPartyMasterGaeaID)
{	
	DWORD dwPartyDamage = 0;
	if (!m_cDamageLogParty.empty())
	{
		DAMAGELOGPARTY_ITER iter = m_cDamageLogParty.begin();
		DAMAGELOGPARTY_ITER iter_end = m_cDamageLogParty.end();
		for (; iter != iter_end; ++iter)
		{
			SDAMAGELOGPARTY& sDamageLogParty = (*iter).second;
			if ( sDamageLogParty.m_dwDamage > dwPartyDamage)
			{
				const GLPartyField* const pParty = m_pGaeaServer->GetParty(sDamageLogParty.m_PartyID);
				if ( pParty == NULL )
					continue;

				dwPartyDamage = sDamageLogParty.m_dwDamage;				
				dwRefPartyMasterGaeaID = pParty->GetMasterGaeaID();
			}
		}
	}
	return dwPartyDamage;
}

void GLCrow::GenerateRewardItem_20110722(DWORD dwOWNER /* GAEAID_NULL */)
{
    EMGROUP emItemHoldGrp(EMGROUP_ONE);
    DWORD dwItemHoldID(GAEAID_NULL);

    EMGROUP emMoneyHoldGrp(EMGROUP_ONE);
    DWORD dwMoneyHoldID(GAEAID_NULL);

    bool bParty = FALSE;

    //	Note : ȹ�� ���� �ɼ� ����.
    //

    GLChar* pChar = m_pGaeaServer->GetChar(dwOWNER);
	GLPartyField* const pParty = ( pChar == NULL ) ? NULL : m_pGaeaServer->GetParty(pChar->GetPartyID());

    bool bgenitemhold = m_pGaeaServer->IsGenItemHold();
    if (bgenitemhold == false)
    {
        dwItemHoldID = GAEAID_NULL;
        dwMoneyHoldID = GAEAID_NULL;
    }
    else if (dwOWNER != GAEAID_NULL)
    {        
        if ( pChar != NULL )
        {            
            if (pParty)
            {
                bParty = TRUE;

				const SPartyOption& PartyOption = pParty->GetPartyOption();

                switch (PartyOption.GetItemOption())
                {
                default:
                case EMPARTY_ITEM_DEFAULT:
                    emItemHoldGrp = EMGROUP_ONE;
                    dwItemHoldID = dwOWNER;
                    break;

                case EMPARTY_ITEM_FREE:
                case EMPARTY_ITEM_LEADER:
                case EMPARTY_ITEM_ROUND:
				case EMPARTY_ITEM_DICE:
				case EMPARTY_ITEM_LEADER_SHARE:
                    emItemHoldGrp = EMGROUP_PARTY;
                    dwItemHoldID = pChar->GetPartyID().GetID();
                    break;
                };

                switch (PartyOption.GetMoneyOption())
                {
                default:
                case EMPARTY_MONEY_DEFAULT:
                    emMoneyHoldGrp = EMGROUP_ONE;
                    dwMoneyHoldID = dwOWNER;
                    break;

                case EMPARTY_MONEY_FREE:
                case EMPARTY_MONEY_LEADER:
                case EMPARTY_MONEY_EQUAL:
                    emMoneyHoldGrp = EMGROUP_PARTY;
                    dwMoneyHoldID = pChar->GetPartyID().GetID();
                    break;
                };

                // Club log
                pParty->LogMonsterKill(m_pGaeaServer, m_sNativeID, GetNameStr());
            }
            else
            {
                emItemHoldGrp = EMGROUP_ONE;
                dwItemHoldID = dwOWNER;

                emMoneyHoldGrp = EMGROUP_ONE;
                dwMoneyHoldID = dwOWNER;

                // Club log
                pChar->LogMonsterKill(m_sNativeID, GetNameStr());
            }
        }
    }

	/*
    // Note : ����� ������ �̺�Ʈ ���� �ݹ��Լ� ����
    CGENITEM& cGenItem = m_pCrowData->m_cGenItem;
	
    cGenItem.SetGenProbCallback   ( boost::bind(&GLCrow::Callback_CalculateGenProb, this, _1, _2, pChar, bParty) );
    cGenItem.SetGenRateCallback   ( boost::bind(&GLCrow::Callback_CalculateGenItemRate, this, _1, _2, _3, pChar) );
    cGenItem.SetGenResultCallback ( boost::bind(&GLCrow::Callback_CalculateGenResult, this, _1) );

#if defined(_RELEASED)
    // Note : �α��Լ� ����
    cGenItem.SetGenLogCallback ( boost::bind(&GLCrow::Callback_GenItemLog, this, _1, _2, _3, pChar, bParty, m_pCrowData->sNativeID) );
#endif
	*/
    
	//���� �߻�
	Generate(pChar, bParty);
    
    VEC_RESULT_ITER iter = m_vecResult.begin();
    for (; iter != m_vecResult.end(); ++iter)
    {
        SGENRESULT& sResult = (*iter);

        if ( sResult.IsNull() )
        {
            continue;
        }

		GLLandMan* const pLandMan(GetLandMan());
		if ( !pLandMan )
			return;

        D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

        BOOL bCollision;
        D3DXVECTOR3 vCollision;
        bCollision = pLandMan->IsCollisionNavi(
            D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
            D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
            vCollision);

        if (bCollision)
        {
            switch (sResult.emType)
            {
            case EMGENRESULTTYPE_ITEM:
                {
                    //	�߻� ������ �ִ��� �˻�.
                    GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
					const bool bValid = pItemLimit->DoCheckItemGen ( sResult.Data.sItemID );
                    if ( bValid == false )
						break;

					CItemDrop* pItemDrop = pLandMan->DropGenItem ( vGenPos, sResult.Data.sItemID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
					if ( pItemDrop == NULL )
						break;

					if ( bParty == FALSE )
						break;

					const SITEM* const pItem = GLogicData::GetInstance().GetItem(sResult.Data.sItemID);
					if ( pItem == NULL )
						break;

					const GLPartyID PartyID(pItemDrop->dwHoldGID);
					const SPartyOption& PartyOption = pParty->GetPartyOption();

					const DWORD dwItemFilterLevel = DWORD(PartyOption.GetItemFilter());
					const DWORD dwItemLevel = DWORD(pItem->sBasicOp.emLevel);

					// �⺻������ ���� �����ۺ��� �����ϹǷ� ��;������� ��ȣ�� �����ش�;
					if ( dwItemFilterLevel+EMITEM_LEVEL_RARE > dwItemLevel )
						break;

					GLPartyTenderManagerField* pTenderManager = m_pGaeaServer->GetPartyTenderItemManager();
					const DWORD dwItemOption = DWORD(PartyOption.GetItemOption());
					switch ( dwItemOption )
					{
					case EMPARTY_ITEM_DICE:
						pTenderManager->AddTenderItem(PartyID, m_mapID, vGenPos, pItemDrop);
						break;
					case EMPARTY_ITEM_LEADER_SHARE:
						pTenderManager->AddDistributionItem(PartyID, m_mapID, vGenPos, pItemDrop);
						break;
					}
                }
                break;

            case EMGENRESULTTYPE_MONEY:
                {
                    pLandMan->DropMoney ( vGenPos, sResult.Data.dwMoney, TRUE, emMoneyHoldGrp, dwMoneyHoldID );
                }
                break;
            }
        }
    }
}
#endif


/*
�Ʒ��� �Լ����� �ݹ�뵵�� ���۵Ǿ� ���Ǿ� ������, �������� ������ ���� ��� ���� ȣ�������� �����Ѵ�.
�Լ� �̸��� ���Ŀ� �����ϵ��� �Ѵ�.
Callback_CalculateGenProb
Callback_CalculateGenItemRate
Callback_CalculateGenResult
Callback_GenItemLog
*/
void GLCrow::Generate(GLChar* pChar, bool bParty)
{
    m_vecResult.clear();

	const CGENITEM::VEC_SGENINFO &vecGenInfo = m_pCrowData->m_cGenItem.m_vecGenInfo;

	SMAPNODE* const pMapNode(m_pGaeaServer->FindMapNode(GetCurrentMap().getBaseMapID()));
	const float fRateItemDrop(pMapNode == NULL ? 1.0f : pMapNode->GetRateItemDrop());

    CGENITEM::VEC_SGENINFO_CITER InfoIter = vecGenInfo.begin();
    for ( ; InfoIter != vecGenInfo.end(); ++InfoIter )
    {
        const SGENINFO& sInfo = (*InfoIter);

        if ( sInfo.fProbability == 0.0f )
        {
            continue;
        }

		float fProbability = sInfo.fProbability * GetMultipleDrop() * fRateItemDrop;
		fProbability = fProbability > 100.f ? 100.f : fProbability;

        // �������� �߻�Ȯ�� üũ
        if ( fProbability < 100.0f )
        {
            float fProb = fProbability;

            fProb = Callback_CalculateGenProb(sInfo.emType, fProb, pChar, bParty);

            if ( !sc::Random::getInstance().IsSucceed(fProb) )
            {
                continue;
            }
        }

        // �߻����� Ȯ�� üũ
        WORD wNum = m_pCrowData->m_cGenItem.GenerateNum(sInfo.vecGenNum);

        if ( wNum == 0 )
        {
            continue;
        }

        // ������ �߻�
        for ( WORD i=0; i<wNum; ++i )
        {
            SGENRESULT sResult;
            GenerateItem( pChar, sInfo.vecGenItem, sResult );
            
            Callback_CalculateGenResult(sResult, pChar);

            if(sResult.IsNull())
            {
                continue;
            }

            m_vecResult.push_back(sResult);
        }
    }

	// Jackpot
	{
		JACKPOT_ITEM Item;
		if ( m_pGaeaServer->PopJackpotItem( Item ) )
		{
			InterlockedIncrement( &Item.PopCount );

			CTime Current = CTime::GetCurrentTime();
			CTime ExpireTime( Item.ExpireTime );
			if ( Current < ExpireTime )
			{
				if ( Item.RequiredLevel != NATIVEID_NULL() )
				{
					// �ش� �������� ���� ���� ��� ���� ó��
					if ( Item.PopCount > 300 )
					{
						JackpotProcess( pChar, &Item );
					}
					else
					{
						WORD CrowLevel = GETLEVEL();
						WORD LevelMin = Item.RequiredLevel.wMainID;
						WORD LevelMax = Item.RequiredLevel.wSubID;
						if ( CrowLevel >= LevelMin && CrowLevel <= LevelMax )
						{
							JackpotProcess( pChar, &Item );
						}
						else
						{
							// ���� ������ �����ʴ�.
							m_pGaeaServer->PushJackpotItem( Item );
						}
					}
				}
				else
				{
					JackpotProcess( pChar, &Item );
				}
			}
			else
			{
				/*sc::writeLogInfo( "Jackpot item expired." );*/
			}
		}
	}
}

void GLCrow::JackpotProcess( GLChar* pChar, JACKPOT_ITEM* Item )
{
	GLLandMan* const pLandMan( GetLandMan() );
	if ( !pLandMan || !pChar || !Item || Item->ItemID == NATIVEID_NULL() )
		return;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( Item->ItemID );
	if ( !pITEM )
		return;

	SGENRESULT sResult;
	sResult.emType = EMGENRESULTTYPE_ITEM;
	sResult.Data.sItemID = Item->ItemID;
	m_vecResult.push_back( sResult );

	GLMSG::NET_JACKPOT_NOTIFY NetMsg;
	NetMsg.MapID = pLandMan->GetBaseMapID();
	NetMsg.CrowID = m_sNativeID;
	NetMsg.ItemID = Item->ItemID;
	NetMsg.SetName( pChar->GetName() );

	if ( Item->bMain )
	{
		m_pGaeaServer->SENDTOAGENT( &NetMsg );
	}
	else
	{
		if ( pLandMan->IsInstantMap() ) 
		{
			m_pGaeaServer->SENDTOCLIENT_ONMAP_INSTANT( pLandMan->GetBaseMapID().dwID, &NetMsg );
		}
		else
		{
			m_pGaeaServer->SENDTOCLIENT_ONMAP( pLandMan->GetBaseMapID().dwID, &NetMsg );
		}
	}

	if ( Item->GameMoney > 0 )
	{
		SGENRESULT sResult;
		sResult.emType = EMGENRESULTTYPE_MONEY;
		sResult.Data.dwMoney = Item->GameMoney;
		m_vecResult.push_back( sResult );
	}
}

/*
�Ʒ��� �Լ����� �ݹ�뵵�� ���۵Ǿ� ���Ǿ� ������, �������� ������ ���� ��� ���� ȣ�������� �����Ѵ�.
�Լ� �̸��� ���Ŀ� �����ϵ��� �Ѵ�.
Callback_CalculateGenProb
Callback_CalculateGenItemRate
Callback_CalculateGenResult
Callback_GenItemLog
*/
void GLCrow::GenerateItem(GLChar* pChar, const SGENINFO::VEC_SPGENITEM& vecGenItem, OUT SGENRESULT& sResult, bool bParty)
{   
	float fRate    = sc::Random::getInstance().GetFloatPercent();
	float fNowRate = 0.0f;

	SGENINFO::VEC_SPGENITEM_CITER iter = vecGenItem.begin();
	for (; iter!=vecGenItem.end(); ++iter)
	{
		SGENITEMBASE* pItem = (*iter).get();

		if ( !pItem )
		{
			continue;
		}

		GLCHARLOGIC* pCharLogic = NULL;
		if ( pChar )
		{
			pCharLogic = static_cast< GLCHARLOGIC* >( pChar );
		}

		if ( !pItem->IsGen( pCharLogic ) )
		{
			continue;
		}

		float fItemRate = pItem->fRate;

		fItemRate = Callback_CalculateGenItemRate(pItem->emType, fItemRate, pItem->wGrade, pChar);

		if ( (fNowRate <= fRate) && (fRate < fNowRate+fItemRate) )
		{
			pItem->GetResult( sResult );

			//! �α� �Լ��� ȣ���Ѵ�.            
			//Callback_GenItemLog( pItem->emType, fItemRate, sResult, pChar, bParty, m_sNativeID);

			break;
		}

		fNowRate += fItemRate;
	}
}


#if defined( _ITEM_GEN_060417_H_ ) || defined( _QUESTION_ITEM_GEN_060417_H_ )
//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
//! 2006-04-17 Jgkim ���ʿ��� vector ����
void GLCrow::GenerateReward_20060417()
{
	STARGETID mytarid(GETCROW(),m_dwGaeaID);

	DWORD dwOWNER(GAEAID_NULL);
	if ((m_sAssault.emCrow == CROW_PC) &&
		(m_sAssault.GaeaId != GAEAID_NULL))
	{
		PGLCHAR pChar = m_pGaeaServer->GetChar ( m_sAssault.GaeaId );
		if ( pChar )
		{
			pChar->ReceiveKillExp ( mytarid );
			pChar->ReceiveKillCP ( mytarid );
			dwOWNER = pChar->GetGaeaID();

			if( pChar->m_bTracingUser )
			{
				GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
				TracingMsg.nUserNum  = pChar->GetUserID();
				TracingMsg.SetAccount(pChar->m_szUID);

				CString strTemp;
				strTemp.Format(
					"Moster Kill!!, [%s][%s], Monster ID:mid[%d]sid[%d]", 
					pChar->m_szUID,
					pChar->m_szName,
					m_sNativeID.wMainID,
					m_sNativeID.wSubID);

				TracingMsg.SetLog(strTemp.GetString());

				m_pGaeaServer->SENDTOAGENT(&TracingMsg);
			}
		}
	}


	if ((m_sAssault.emCrow == CROW_SUMMON) &&
		(m_sAssault.GaeaId != GAEAID_NULL))
	{
		GLLandMan* const pLandMan(GetLandMan());
		if ( pLandMan )
		{
			PGLSUMMONFIELD pSummon = pLandMan->GetSummon ( m_sAssault.GaeaId );
			if (pSummon)
			{
				GLChar* pSummonOwner = m_pGaeaServer->GetChar64(pSummon->GetOwnerDbNum(), pSummon->GetOwnerSvrGroupNum());
				if (pSummonOwner)
				{
					pSummonOwner->ReceiveKillExp ( mytarid, false, true );
					pSummonOwner->ReceiveKillCP ( mytarid );
					dwOWNER = pSummonOwner->GetGaeaID();
				}
			}
		}
	}
	
	DWORD dwPartyMasterGaeaID = GAEAID_NULL;
	DWORD dwPartyDamage = 0;
	// ��Ƽ������ ���	
	if (!m_cDamageLogParty.empty())
	{
		DAMAGELOGPARTY_ITER iterP     = m_cDamageLogParty.begin();
		DAMAGELOGPARTY_ITER iterP_end = m_cDamageLogParty.end();

		GLPartyField* pParty = NULL;
		for ( ; iterP != iterP_end; ++iterP )
		{
			SDAMAGELOGPARTY &sDamageLogParty = (*iterP).second;
			pParty = m_pGaeaServer->GetParty(sDamageLogParty.m_dwPartyID);
			if (pParty == NULL) 
			{
				continue;
			}
			else
			{				
				if (sDamageLogParty.m_dwDamage > dwPartyDamage)
				{
					// ��Ƽ�߿��� ���� ���� �������� �� ��Ƽ�� �����͸� ã�´�.
					dwPartyMasterGaeaID = pParty->m_dwMASTER;
					dwPartyDamage       = sDamageLogParty.m_dwDamage;
				}
			}
		}
	}

	// Note : ��ȯ���� �� ������
	//        ���� ��Ȳ�� �Ǻ��Ͽ� �������� m_cDamageLog�� �ѱ��
	if (!m_cSummonDamageLog.empty())
	{ 
		PGLCHAR        pChar    = NULL;
		DAMAGELOG_ITER iter     = m_cSummonDamageLog.begin ();
		DAMAGELOG_ITER iter_end = m_cSummonDamageLog.end ();
		for (; iter!=iter_end; ++iter)
		{
			SDAMAGELOG &sDamageLog = (*iter).second;

			// ��ȯ�� �α׿� ������ GaeaID�� ����ߴ�.
			pChar = m_pGaeaServer->GetChar ( sDamageLog.dwGaeaID );

			if ( !pChar )										   continue;
			if ( !pChar->IsValidBody() )						   continue;
			if (sDamageLog.dwUserID != pChar->GetUserID())         continue;
			if ( pChar->GetCurrentMap() != GetCurrentMap() )				   continue;

			D3DXVECTOR3 vDis = GetPosition() - pChar->GetPosition();
			float fDis = D3DXVec3Length ( &vDis );
			if ( fDis > GLCONST_CHAR::fPARTYEXPRANGE )			   continue;

			AddDamageLog ( m_cDamageLog, pChar->GetGaeaID(), pChar->GetUserID(), sDamageLog.dwDamage );
		}
	}

	// ĳ���Ͱ� �� ������ ���
	DWORD dwCharGaeaID = GAEAID_NULL;
	DWORD dwCharDamage = 0;
	if (!m_cDamageLog.empty())
	{
		PGLCHAR pChar = NULL;
		DAMAGELOG_ITER iter = m_cDamageLog.begin ();
		DAMAGELOG_ITER iter_end = m_cDamageLog.end ();
		for (; iter!=iter_end; ++iter)
		{
			SDAMAGELOG &sDamageLog = (*iter).second;

			pChar = m_pGaeaServer->GetChar ( sDamageLog.dwGaeaID );
			
			if (!pChar) continue;
			if (sDamageLog.dwUserID != pChar->GetUserID()) continue;

			pChar->ReceiveAttackExp(mytarid, sDamageLog.dwDamage, FALSE);

			if (sDamageLog.dwDamage > dwCharDamage)
			{
				// ĳ�����߿� ���� ���� �������� �� ĳ���͸� ã�´�.
                dwCharGaeaID = pChar->GetGaeaID();
				dwCharDamage = sDamageLog.dwDamage;
			}
		}
	}


	// ��Ƽ�� ĳ���� �� ���� �������� �� �ʿ�
	// ������ �������� �ο��Ѵ�.
	if (dwCharDamage > dwPartyDamage)
	{
        dwOWNER = dwCharGaeaID;
	}
	else
	{
		dwOWNER = dwPartyMasterGaeaID;
	}

	m_cDamageLog.clear();
	m_cDamageLogParty.clear();	
	m_cSummonDamageLog.clear();

    GenerateRewardItem_20060417(dwOWNER);
}

//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
//! 2006-04-17 Jgkim GenerateReward �Լ����� �и�.
void GLCrow::GenerateRewardItem_20060417(DWORD dwOWNER /* GAEAID_NULL */)
{
	EMGROUP emItemHoldGrp(EMGROUP_ONE);
	DWORD dwItemHoldID(GAEAID_NULL);

	EMGROUP emMoneyHoldGrp(EMGROUP_ONE);
	DWORD dwMoneyHoldID(GAEAID_NULL);

	bool bParty = FALSE;

	//	Note : ȹ�� ���� �ɼ� ����.
	//
	float fGEN_RATE = 1.0f;
	bool bgenitemhold = m_pGaeaServer->IsGenItemHold();
	if ( bgenitemhold==false )
	{
		dwItemHoldID = GAEAID_NULL;
		dwMoneyHoldID = GAEAID_NULL;
	}
	else if ( dwOWNER!=GAEAID_NULL )
	{
		PGLCHAR pChar = m_pGaeaServer->GetChar ( dwOWNER );
		if ( pChar )
		{
			fGEN_RATE = pChar->m_fGENITEM_RATE;

			GLPartyField *pParty = m_pGaeaServer->GetParty(pChar->m_PartyID);
			if ( pParty )
			{
				bParty = TRUE;

				switch ( pParty->m_sOPTION.GetItemOption() )
				{
				default:
				case EMPARTY_ITEM_DEFAULT:
					emItemHoldGrp = EMGROUP_ONE;
					dwItemHoldID = dwOWNER;
					break;
				
				case EMPARTY_ITEM_FREE:
				case EMPARTY_ITEM_LEADER:
				case EMPARTY_ITEM_ROUND:
					emItemHoldGrp = EMGROUP_PARTY;
					dwItemHoldID = pChar->m_PartyID;
					break;
				};

				switch ( pParty->m_sOPTION.GetMoneyOption() )
				{
				default:
				case EMPARTY_MONEY_DEFAULT:
					emMoneyHoldGrp = EMGROUP_ONE;
					dwMoneyHoldID = dwOWNER;
					break;
				
				case EMPARTY_MONEY_FREE:
				case EMPARTY_MONEY_LEADER:
				case EMPARTY_MONEY_EQUAL:
					emMoneyHoldGrp = EMGROUP_PARTY;
					dwMoneyHoldID = pChar->m_PartyID;
					break;
				};
			}
			else
			{
				emItemHoldGrp = EMGROUP_ONE;
				dwItemHoldID = dwOWNER;

				emMoneyHoldGrp = EMGROUP_ONE;
				dwMoneyHoldID = dwOWNER;
			}
		}
	}

	PGLCHAR pChar = m_pGaeaServer->GetChar ( dwOWNER );
	GLLandMan* const pLandMan(GetLandMan());
	if ( pLandMan )
	{
		//	Note : ������ �߻�. ( �ܼ� )
		//
		const SCROWGEN &sCROWGEN = m_pCrowData->m_sGenerate;
		if ( sCROWGEN.m_wGenItem_Rate!=0 && sCROWGEN.m_sGenItemID!=NATIVEID_NULL() )
		{
			float fGEN_RATE_R = sCROWGEN.m_wGenItem_Rate * GLCONST_CHAR::GET_ITEM_SCALE() * fGEN_RATE;

			if( !bParty )
			{
				fGEN_RATE_R = CalculateItemGenRate_20060417( fGEN_RATE_R, pChar, bParty );
			}
			if (sc::Random::getInstance().IsSucceed(fGEN_RATE_R))
			{
				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLandMan->IsCollisionNavi
					(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
					);

				if ( bCollision )
				{
					//	�߻� ������ �ִ��� �˻�.
					GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
					const SNATIVEID &sNID = sCROWGEN.m_sGenItemID;
					bool bvalid = pItemLimit->DoCheckItemGen ( sNID );
					if ( bvalid )
					{
						//	������ �߻�.
						pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
					}
				}
			}
		}

		//	Note : ���� ������ �߻�.
		//
		float fGenRate = 1.0f; 

		if( !bParty )
		{
			fGenRate = CalculateItemGenRate_20060417( fGenRate, pChar, bParty );
		}

		SGENITEM_060417 &sGenItem = m_pCrowData->m_sGenItem;



		if ( pLandMan && sGenItem.ISGEN(fGenRate) )
		{
			GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
			WORD wGenNum = sGenItem.SELECTNUM();		
			for ( WORD i=0; i<wGenNum; ++i )
			{
				bool bSEPCID = false;
				DWORD dwSpecID = sGenItem.SELECTSPECID(fGEN_RATE, bSEPCID);
				SNATIVEID sNID;

				if ( dwSpecID==SGENITEM_060417::EMSPEC_NULL )	continue;

				if ( bSEPCID )	sNID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
				else	sNID = dwSpecID;

				if ( sNID==NATIVEID_NULL() )	continue;

				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLandMan->IsCollisionNavi
					(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
					);

				if ( !bCollision )				continue;

				//	�߻� ������ �ִ��� �˻�.
				bool bvalid = pItemLimit->DoCheckItemGen ( sNID, EMGEN_MOB );
				if ( bvalid )
				{
					//	������ �߻�.
					pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
				}
			}
		}

		//	Note : Question ������ �߻� by ���
		//
		SQTGENITEM_060417 & SQTGENITEM_060417 = m_pCrowData->m_sQtGenItem;
		SNATIVEID nidQITEM = SQTGENITEM_060417.SELECTITEM();
		if ( nidQITEM != SNATIVEID(false) )
		{
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = pLandMan->IsCollisionNavi
				(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
				);

			if ( bCollision )
			{
				//	�߻� ������ �ִ��� �˻�.
				GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
				bool bvalid = pItemLimit->DoCheckItemGen ( nidQITEM, EMGEN_MOB );
				if ( bvalid )
				{
					//	������ �߻�.
					pLandMan->DropGenItem ( vGenPos, nidQITEM, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
				}
			}
		}

		//	Note : �� �߻�.
		//
		if ( sCROWGEN.m_wGenMoney_Rate!=0 && sCROWGEN.m_dwGenMoney!=0 )
		{
			float fGEN_RATE = sCROWGEN.m_wGenMoney_Rate * GLCONST_CHAR::GET_MONEY_SCALE();
			if (sc::Random::getInstance().IsSucceed(fGEN_RATE))
			{
				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLandMan->IsCollisionNavi
					(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
					);

				//*/*/*/*
				if ( bCollision )
				{
					//	�� �߻�.
					DWORD dwGemMoney = DWORD ( GLCONST_CHAR::GET_MONEY_SCALE() * sCROWGEN.m_dwGenMoney * ( 1.0f + 0.3f*sc::Random::RANDOM_NUM() ) );
					pLandMan->DropMoney ( vGenPos, dwGemMoney, TRUE, emMoneyHoldGrp, dwMoneyHoldID );				
				}
			}
		}
	}

	//	Note : �˸� �޽��� �ʿ����� �˻��� �޽��� �߻�.
	//
	if (m_pCrowData->IsKnock())
	{
		const char *szMAP_NAME = m_pGaeaServer->GetMapName ( m_mapID );
        std::string strText = sc::string::format( ID2SERVERTEXT("EMCROWACT_REPULSE"), m_pCrowData->GetName(), szMAP_NAME );

		GLMSG::SNET_SERVER_GENERALCHAT SendData;
		if (SendData.SETTEXT(strText.c_str()))
        {
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
		    m_pGaeaServer->SENDTOAGENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }
	}
}

//! ���� �׾����� ������ �߻���Ų�� (������, ���ӸӴ�, ?�ڽ� ���...)
//! 2006-04-02 Jgkim ��Ƽ������ ����ϵ��� ����
void GLCrow::GenerateReward_20060402 ()
{
	STARGETID mytarid(GETCROW(),m_dwGaeaID);

	DWORD dwOWNER(GAEAID_NULL);
	if ((m_sAssault.emCrow == CROW_PC) &&
		(m_sAssault.GaeaId != GAEAID_NULL))
	{
		PGLCHAR pChar = m_pGaeaServer->GetChar ( m_sAssault.GaeaId );
		if ( pChar )
		{
			pChar->ReceiveKillExp ( mytarid );
			dwOWNER = pChar->GetGaeaID();
		}
	}

	// ��Ƽ������ ���
	VECDAMAGEPARTY vecDAMAGEPARTY;
	if ( !m_cDamageLogParty.empty() )
	{		
		vecDAMAGEPARTY.reserve ( m_cDamageLogParty.size() );

		DAMAGELOGPARTY_ITER iterP     = m_cDamageLogParty.begin();
		DAMAGELOGPARTY_ITER iterP_end = m_cDamageLogParty.end();

		GLPartyField* pParty = NULL;
		for ( ; iterP != iterP_end; ++iterP )
		{
			SDAMAGELOGPARTY &sDamageLogParty = (*iterP).second;
			pParty = m_pGaeaServer->GetParty(sDamageLogParty.m_dwPartyID);
			if (pParty == NULL) 
			{
				continue;
			}
			else
			{
				vecDAMAGEPARTY.push_back(sDamageLogParty);
			}
		}
	}
	
	//	Note : �Ϲ� ������ ���
	VECDAMAGE vecDAMAGE;
	if ( !m_cDamageLog.empty() )
	{		
		vecDAMAGE.reserve ( m_cDamageLog.size() );

		PGLCHAR pChar = NULL;
		DAMAGELOG_ITER iter = m_cDamageLog.begin ();
		DAMAGELOG_ITER iter_end = m_cDamageLog.end ();
		for ( ; iter!=iter_end; ++iter )
		{
			SDAMAGELOG &sDamageLog = (*iter).second;

			pChar = m_pGaeaServer->GetChar ( sDamageLog.dwGaeaID );
			
			if ( !pChar ) continue;

			if ( sDamageLog.dwUserID != pChar->GetUserID() )		continue;

			pChar->ReceiveAttackExp ( mytarid, sDamageLog.dwDamage, FALSE );

			vecDAMAGE.push_back ( sDamageLog );
		}
	}	

	// ��Ƽ�߿��� ���� ���� �������� �� ��Ƽ�� �����͸� ���Ѵ�.
	DWORD dwPartyMasterGaeaID = GAEAID_NULL;
	DWORD dwPartyDamage = 0;
	if ( !vecDAMAGEPARTY.empty() )
	{
		VECDAMAGEPARTY_ITER posP = std::max_element ( vecDAMAGEPARTY.begin(), 
			                                         vecDAMAGEPARTY.end(), 
													 party_damage_less );
		if ( posP != vecDAMAGEPARTY.end() )
		{
			const SDAMAGELOGPARTY &sDAMAGEPARTY = (*posP);
			GLPartyField* pParty2 = m_pGaeaServer->GetParty(sDAMAGEPARTY.m_dwPartyID);
			if (pParty2 != NULL)
			{				
				PGLCHAR pCharX = m_pGaeaServer->GetChar(pParty2->m_dwMASTER);
				if (pCharX != NULL)
				{
					dwPartyDamage = sDAMAGEPARTY.m_dwDamage; // ��Ƽ�� �� ������
					dwPartyMasterGaeaID = pParty2->m_dwMASTER; // ��Ƽ ������
					dwOWNER = dwPartyMasterGaeaID;
				}
			}
		}
	}

	//	Note : ������ ���� ���� ��� �ѻ������ ������ ȹ�� ���� �ο�.
	//		���� ���� ����� �������� ��� ȹ�� ������ ����.
	DWORD dwCharGaeaID = GAEAID_NULL;
	DWORD dwCharDamage = 0;
	if ( !vecDAMAGE.empty() )
	{
		VECDAMAGE_ITER pos = std::max_element ( vecDAMAGE.begin(), vecDAMAGE.end(), damage_less );
		if ( pos!=vecDAMAGE.end() )
		{
			const SDAMAGELOG &sDAMAGE = (*pos);

			PGLCHAR pChar = m_pGaeaServer->GetChar ( sDAMAGE.dwGaeaID );
			if ( pChar && sDAMAGE.dwUserID==pChar->GetUserID() )
			{
				dwCharGaeaID = sDAMAGE.dwGaeaID;
				dwCharDamage = sDAMAGE.dwDamage;
				if (dwCharDamage > dwPartyDamage)
				{
					dwOWNER = dwCharGaeaID;
				}
			}
		}
	}	

	m_cDamageLog.clear();
	m_cDamageLogParty.clear();

	EMGROUP emItemHoldGrp(EMGROUP_ONE);
	DWORD dwItemHoldID(GAEAID_NULL);

	EMGROUP emMoneyHoldGrp(EMGROUP_ONE);
	DWORD dwMoneyHoldID(GAEAID_NULL);

	//	Note : ȹ�� ���� �ɼ� ����.
	//
	float fGEN_RATE = 1.0f;
	bool bgenitemhold = m_pGaeaServer->IsGenItemHold();
	if ( bgenitemhold==false )
	{
		dwItemHoldID = GAEAID_NULL;
		dwMoneyHoldID = GAEAID_NULL;
	}
	else if ( dwOWNER!=GAEAID_NULL )
	{
		PGLCHAR pChar = m_pGaeaServer->GetChar ( dwOWNER );
		if ( pChar )
		{
			fGEN_RATE = pChar->m_fGENITEM_RATE;

			GLPartyField *pParty = m_pGaeaServer->GetParty(pChar->m_PartyID);
			if ( pParty )
			{
				switch ( pParty->m_sOPTION.GetItemOption() )
				{
				default:
				case EMPARTY_ITEM_DEFAULT:
					emItemHoldGrp = EMGROUP_ONE;
					dwItemHoldID = dwOWNER;
					break;
				
				case EMPARTY_ITEM_FREE:
				case EMPARTY_ITEM_LEADER:
				case EMPARTY_ITEM_ROUND:
					emItemHoldGrp = EMGROUP_PARTY;
					dwItemHoldID = pChar->m_PartyID;
					break;
				};

				switch ( pParty->m_sOPTION.GetMoneyOption() )
				{
				default:
				case EMPARTY_MONEY_DEFAULT:
					emMoneyHoldGrp = EMGROUP_ONE;
					dwMoneyHoldID = dwOWNER;
					break;
				
				case EMPARTY_MONEY_FREE:
				case EMPARTY_MONEY_LEADER:
				case EMPARTY_MONEY_EQUAL:
					emMoneyHoldGrp = EMGROUP_PARTY;
					dwMoneyHoldID = pChar->m_PartyID;
					break;
				};
			}
			else
			{
				emItemHoldGrp = EMGROUP_ONE;
				dwItemHoldID = dwOWNER;

				emMoneyHoldGrp = EMGROUP_ONE;
				dwMoneyHoldID = dwOWNER;
			}
		}
	}

	GLLandMan* const pLandMan(GetLandMan());
	if ( pLandMan )
	{
		//	Note : ������ �߻�. ( �ܼ� )
		//
		const SCROWGEN &sCROWGEN = m_pCrowData->m_sGenerate;
		if ( sCROWGEN.m_wGenItem_Rate!=0 && sCROWGEN.m_sGenItemID!=NATIVEID_NULL() )
		{
			float fGEN_RATE_R = sCROWGEN.m_wGenItem_Rate * GLCONST_CHAR::GET_ITEM_SCALE() * fGEN_RATE;

			if (sc::Random::getInstance().IsSucceed(fGEN_RATE_R))
			{
				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLandMan->IsCollisionNavi
					(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
					);

				if ( bCollision )
				{
					//	�߻� ������ �ִ��� �˻�.
					GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
					const SNATIVEID &sNID = sCROWGEN.m_sGenItemID;
					bool bvalid = pItemLimit->DoCheckItemGen ( sNID );
					if ( bvalid )
					{
						//	������ �߻�.
						pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
					}
				}
			}
		}

		//	Note : ���� ������ �߻�.
		//
		SGENITEM_060417 &sGenItem = m_pCrowData->m_sGenItem;
		if ( sGenItem.ISGEN() )
		{
			GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
			WORD wGenNum = sGenItem.SELECTNUM();

			for ( WORD i=0; i<wGenNum; ++i )
			{
				bool bSEPCID = false;
				DWORD dwSpecID = sGenItem.SELECTSPECID(fGEN_RATE, bSEPCID);
				SNATIVEID sNID;

				if ( dwSpecID==SGENITEM_060417::EMSPEC_NULL )	continue;

				if ( bSEPCID )	sNID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
				else	sNID = dwSpecID;

				if ( sNID==NATIVEID_NULL() )	continue;

				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLandMan->IsCollisionNavi
					(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
					);

				if ( !bCollision )				continue;

				//	�߻� ������ �ִ��� �˻�.
				bool bvalid = pItemLimit->DoCheckItemGen ( sNID, EMGEN_MOB );
				if ( bvalid )
				{
					//	������ �߻�.
					pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
				}
			}
		}

		//	Note : Question ������ �߻� by ���
		//
		SQTGENITEM_060417 & SQTGENITEM_060417 = m_pCrowData->m_sQtGenItem;
		SNATIVEID nidQITEM = SQTGENITEM_060417.SELECTITEM();
		if ( nidQITEM != SNATIVEID(false) )
		{
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = pLandMan->IsCollisionNavi
				(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
				);

			if ( bCollision )
			{
				//	�߻� ������ �ִ��� �˻�.
				GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
				bool bvalid = pItemLimit->DoCheckItemGen ( nidQITEM, EMGEN_MOB );
				if ( bvalid )
				{
					//	������ �߻�.
					pLandMan->DropGenItem ( vGenPos, nidQITEM, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
				}
			}
		}

		//	Note : �� �߻�.
		//
		if ( sCROWGEN.m_wGenMoney_Rate!=0 && sCROWGEN.m_dwGenMoney!=0 )
		{
			float fGEN_RATE = sCROWGEN.m_wGenMoney_Rate * GLCONST_CHAR::GET_MONEY_SCALE();
			if (sc::Random::getInstance().IsSucceed(fGEN_RATE))
			{
				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLandMan->IsCollisionNavi
					(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
					);

				if ( bCollision )
				{
					//	�� �߻�.
					DWORD dwGemMoney = DWORD ( GLCONST_CHAR::GET_MONEY_SCALE() * sCROWGEN.m_dwGenMoney * ( 1.0f + 0.3f*sc::Random::RANDOM_NUM() ) );

					pLandMan->DropMoney ( vGenPos, dwGemMoney, TRUE, emMoneyHoldGrp, dwMoneyHoldID );
				}
			}
		}
	}

	//	Note : �˸� �޽��� �ʿ����� �˻��� �޽��� �߻�.
	//
	if (m_pCrowData->IsKnock())
	{
		const char *szMAP_NAME = m_pGaeaServer->GetMapName ( m_mapID );
        std::string strText = sc::string::format( ID2SERVERTEXT("EMCROWACT_REPULSE"), m_pCrowData->GetName(), szMAP_NAME );

		GLMSG::SNET_SERVER_GENERALCHAT SendData;
		if (SendData.SETTEXT(strText))
        {
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
		    m_pGaeaServer->SENDTOAGENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }
	}
}

void GLCrow::GenerateReward_200502 ()
{
	STARGETID mytarid(GETCROW(),m_dwGaeaID);

	DWORD dwOWNER(GAEAID_NULL);
	if ((m_sAssault.emCrow == CROW_PC) &&
		(m_sAssault.GaeaId != GAEAID_NULL))
	{
		PGLCHAR pChar = m_pGaeaServer->GetChar ( m_sAssault.GaeaId );
		if ( pChar )
		{
			pChar->ReceiveKillExp ( mytarid );
			dwOWNER = pChar->GetGaeaID();
		}
	}

	//	Note : �Ϲ� ������.
	//
	if ( !m_cDamageLog.empty() )
	{
		VECDAMAGE vecDAMAGE;
		vecDAMAGE.reserve ( m_cDamageLog.size() );

		PGLCHAR pChar = NULL;
		DAMAGELOG_ITER iter = m_cDamageLog.begin ();
		DAMAGELOG_ITER iter_end = m_cDamageLog.end ();
		for ( ; iter!=iter_end; ++iter )
		{
			SDAMAGELOG &sDamageLog = (*iter).second;

			pChar = m_pGaeaServer->GetChar ( sDamageLog.dwGaeaID );
			
			if ( !pChar ) continue;

			if ( sDamageLog.dwUserID != pChar->GetUserID() )		continue;

			pChar->ReceiveAttackExp ( mytarid, sDamageLog.dwDamage, FALSE );

			vecDAMAGE.push_back ( sDamageLog );
		}

		//	Note : ������ ���� ���� ��� �ѻ������ ������ ȹ�� ���� �ο�.
		//		���� ���� ����� �������� ��� ȹ�� ������ ����.
		if ( !vecDAMAGE.empty() )
		{
			VECDAMAGE_ITER pos = std::max_element ( vecDAMAGE.begin(), vecDAMAGE.end(), damage_less );
			if ( pos!=vecDAMAGE.end() )
			{
				const SDAMAGELOG &sDAMAGE = (*pos);

				PGLCHAR pChar = m_pGaeaServer->GetChar ( sDAMAGE.dwGaeaID );
				if ( pChar && sDAMAGE.dwUserID==pChar->GetUserID() )
				{
					dwOWNER = sDAMAGE.dwGaeaID;
				}
			}
		}
	}

	m_cDamageLog.clear();
	m_cDamageLogParty.clear();

	EMGROUP emItemHoldGrp(EMGROUP_ONE);
	DWORD dwItemHoldID(GAEAID_NULL);

	EMGROUP emMoneyHoldGrp(EMGROUP_ONE);
	DWORD dwMoneyHoldID(GAEAID_NULL);

	//	Note : ȹ�� ���� �ɼ� ����.
	//
	float fGEN_RATE = 1.0f;
	bool bgenitemhold = m_pGaeaServer->IsGenItemHold();
	if ( bgenitemhold==false )
	{
		dwItemHoldID = GAEAID_NULL;
		dwMoneyHoldID = GAEAID_NULL;
	}
	else if ( dwOWNER!=GAEAID_NULL )
	{
		PGLCHAR pChar = m_pGaeaServer->GetChar ( dwOWNER );
		if ( pChar )
		{
			fGEN_RATE = pChar->m_fGENITEM_RATE;

			GLPartyField *pParty = m_pGaeaServer->GetParty(pChar->m_PartyID);
			if ( pParty )
			{
				switch ( pParty->m_sOPTION.GetItemOption() )
				{
				default:
				case EMPARTY_ITEM_DEFAULT:
					emItemHoldGrp = EMGROUP_ONE;
					dwItemHoldID = dwOWNER;
					break;
				
				case EMPARTY_ITEM_FREE:
				case EMPARTY_ITEM_LEADER:
				case EMPARTY_ITEM_ROUND:
					emItemHoldGrp = EMGROUP_PARTY;
					dwItemHoldID = pChar->m_PartyID;
					break;
				};

				switch ( pParty->m_sOPTION.GetMoneyOption() )
				{
				default:
				case EMPARTY_MONEY_DEFAULT:
					emMoneyHoldGrp = EMGROUP_ONE;
					dwMoneyHoldID = dwOWNER;
					break;
				
				case EMPARTY_MONEY_FREE:
				case EMPARTY_MONEY_LEADER:
				case EMPARTY_MONEY_EQUAL:
					emMoneyHoldGrp = EMGROUP_PARTY;
					dwMoneyHoldID = pChar->m_PartyID;
					break;
				};
			}
			else
			{
				emItemHoldGrp = EMGROUP_ONE;
				dwItemHoldID = dwOWNER;

				emMoneyHoldGrp = EMGROUP_ONE;
				dwMoneyHoldID = dwOWNER;
			}
		}
	}

	//	Note : ������ �߻�. ( �ܼ� )
	//
	GLLandMan* const pLandMan(GetLandMan());
	if ( pLandMan )
	{
		const SCROWGEN &sCROWGEN = m_pCrowData->m_sGenerate;
		if ( sCROWGEN.m_wGenItem_Rate!=0 && sCROWGEN.m_sGenItemID!=NATIVEID_NULL() )
		{
			float fGEN_RATE_R = sCROWGEN.m_wGenItem_Rate * GLCONST_CHAR::GET_ITEM_SCALE() * fGEN_RATE;

			if (sc::Random::getInstance().IsSucceed(fGEN_RATE_R))
			{
				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLandMan->IsCollisionNavi
					(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
					);

				if ( bCollision )
				{
					//	�߻� ������ �ִ��� �˻�.
					GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
					const SNATIVEID &sNID = sCROWGEN.m_sGenItemID;
					bool bvalid = pItemLimit->DoCheckItemGen ( sNID );
					if ( bvalid )
					{
						//	������ �߻�.
						pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
					}
				}
			}
		}

		//	Note : ���� ������ �߻�.
		//
		SGENITEM_060417 &sGenItem = m_pCrowData->m_sGenItem;
		if ( sGenItem.ISGEN() )
		{
			WORD wGenNum = sGenItem.SELECTNUM();

			for ( WORD i=0; i<wGenNum; ++i )
			{
				bool bSEPCID = false;
				DWORD dwSpecID = sGenItem.SELECTSPECID(fGEN_RATE, bSEPCID);
				SNATIVEID sNID;

				if ( dwSpecID==SGENITEM_060417::EMSPEC_NULL )	continue;

				if ( bSEPCID )	sNID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
				else	sNID = dwSpecID;

				if ( sNID==NATIVEID_NULL() )	continue;

				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLandMan->IsCollisionNavi
					(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
					);

				if ( !bCollision )				continue;

				//	�߻� ������ �ִ��� �˻�.
				GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
				bool bvalid = pItemLimit->DoCheckItemGen ( sNID, EMGEN_MOB );
				if ( bvalid )
				{
					//	������ �߻�.
					pLandMan->DropGenItem ( vGenPos, sNID, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
				}
			}
		}

		//	Note : Question ������ �߻� by ���
		//
		SQTGENITEM_060417 & SQTGENITEM_060417 = m_pCrowData->m_sQtGenItem;
		SNATIVEID nidQITEM = SQTGENITEM_060417.SELECTITEM();
		if ( nidQITEM != SNATIVEID(false) )
		{
			D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

			BOOL bCollision;
			D3DXVECTOR3 vCollision;
			bCollision = pLandMan->IsCollisionNavi
				(
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
				D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
				vCollision
				);

			if ( bCollision )
			{
				//	�߻� ������ �ִ��� �˻�.
				GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
				bool bvalid = pItemLimit->DoCheckItemGen ( nidQITEM, EMGEN_MOB );
				if ( bvalid )
				{
					//	������ �߻�.
					pLandMan->DropGenItem ( vGenPos, nidQITEM, EMGEN_MOB, emItemHoldGrp, dwItemHoldID );
				}
			}
		}

		//	Note : �� �߻�.
		//
		if ( sCROWGEN.m_wGenMoney_Rate!=0 && sCROWGEN.m_dwGenMoney!=0 )
		{
			float fGEN_RATE = sCROWGEN.m_wGenMoney_Rate * GLCONST_CHAR::GET_MONEY_SCALE();
			if (sc::Random::getInstance().IsSucceed(fGEN_RATE))
			{
				D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

				BOOL bCollision;
				D3DXVECTOR3 vCollision;
				bCollision = pLandMan->IsCollisionNavi
					(
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
					D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
					vCollision
					);

				if ( bCollision )
				{
					//	�� �߻�.
					DWORD dwGemMoney = DWORD ( GLCONST_CHAR::GET_MONEY_SCALE() * sCROWGEN.m_dwGenMoney * ( 1.0f + 0.3f*sc::Random::RANDOM_NUM() ) );

					pLandMan->DropMoney ( vGenPos, dwGemMoney, TRUE, emMoneyHoldGrp, dwMoneyHoldID );
				}
			}
		}
	}

	//	Note : �˸� �޽��� �ʿ����� �˻��� �޽��� �߻�.
	//
	if (m_pCrowData->IsKnock())
	{
		const char *szMAP_NAME = m_pGaeaServer->GetMapName ( m_mapID );
        std::string strText(sc::string::format(ID2SERVERTEXT("EMCROWACT_REPULSE"), m_pCrowData->GetName(), szMAP_NAME));
		GLMSG::SNET_SERVER_GENERALCHAT SendData;
		if (SendData.SETTEXT(strText))
        {
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
		    m_pGaeaServer->SENDTOAGENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }
	}
}

#endif
