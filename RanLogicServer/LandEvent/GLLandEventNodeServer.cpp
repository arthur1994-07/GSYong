#include "pch.h"
#include "../../RanLogic/LandEvent/GLLandEventGenerator.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../AI/GLFSMContainer.h"

#include "./GLLandEventNodeServer.h"

#include "../../SigmaCore/DebugInclude.h"


//-------------------------------------------------------------------//

// 주변 Char에게 등록과 동시에 ReqNetMsg_Drop 메세지를 보낸다.
void CEVENT_NODE_SERVER::RegisterViewAroundPC(GLGaeaServer* pGLGaeaServer)
{
    if (!m_pLandMan)
    {
        sc::writeLogError(
            std::string("CEVENT_NODE_SERVER::RegisterViewAroundPC() -  m_pLandMan NULL"));
        return;
    }

    LANDQUADTREE* pLandTree = m_pLandMan->GetLandTree();
    if (!pLandTree)
    {
        sc::writeLogError(
            sc::string::format(
                "CEVENT_NODE_SERVER::RegisterViewAroundPC() - pLandTree NULL MAP %1%/%2%",
                m_pLandMan->GetBaseMapID().Mid(),
                m_pLandMan->GetBaseMapID().Sid()));
        return;
    }
    
    LANDQUADNODE* pRootNode = pLandTree->GetRootNode();
    if (!pRootNode)
    {
        sc::writeLogError(
            sc::string::format(
                "CEVENT_NODE_SERVER::RegisterViewAroundPC() - pRootNode NULL MAP %1%/%2%",
                m_pLandMan->GetBaseMapID().Mid(),
                m_pLandMan->GetBaseMapID().Sid()));
        return;
    }

    LANDQUADNODE* pQuadHead = NULL; 
	
	const int nMyPosition[2] = 
	{
		int(m_vPosition.x),
		int(m_vPosition.z)
	};
    BOUDRECT BoundRect(nMyPosition[0]+MAX_VIEWRANGE, nMyPosition[1]+MAX_VIEWRANGE, nMyPosition[0]-MAX_VIEWRANGE, nMyPosition[1]-MAX_VIEWRANGE);
    pLandTree->FindNodes(BoundRect, pRootNode, &pQuadHead);
    
    LANDQUADNODE* pQuadNode = pQuadHead;
    for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
    {
        GLLandNode *pLandNode = pQuadNode->pData;
        if( !pLandNode ) 
            continue;

        if ( pQuadNode->pData->m_PCList.m_dwAmount==0 )
            continue;

        GLCHARNODE *pCharNode = pQuadNode->pData->m_PCList.m_pHead;
        for ( ; pCharNode; pCharNode = pCharNode->pNext )
        {
            GLChar *pChar = pCharNode->Data;
            if ( pChar == NULL ) 
                continue;

            if ( pGLGaeaServer->GetChar(pChar->GetGaeaID()) == NULL )	
                continue;

			const D3DXVECTOR3& vPosition = pChar->GetPosition();
			const int nPosition[2] = 
			{
				int(vPosition.x),
				int(vPosition.z)
			};

            if ( BoundRect.IsWithIn(nPosition[0], nPosition[1]) == FALSE )
                continue;

            if (pChar->AddFieldLandEvent(m_dwGlobID))
            {
                GLMSG::SNETLANDEVENT_SUMMONGATE_CREATE MsgFC;
                ReqNetMsg_Drop(MsgFC);
                pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), &MsgFC);
            }
        }
    }
}

//-------------------------------------------------------------------//

void CEVENT_NODE_SERVER_SUMMONGATE::Restore()
{
    SLANDEVENT_GENERATOR<CEVENT_NODE_SERVER_SUMMONGATE>::Restore(this);
}

void CEVENT_NODE_SERVER_SUMMONGATE::Clean()
{
    // Blank
}

void CEVENT_NODE_SERVER_SUMMONGATE::FrameStart()
{
    // Blank
}

void CEVENT_NODE_SERVER_SUMMONGATE::FrameEvent()
{
    if ( !m_pGaeaServer || !m_pLandMan )
    {
        return;
    }

    // Note : 유효성 체크
	ServerActor* pOwner = m_pGaeaServer->GetActor( m_pLandMan, m_sOwnerID );
    if ( !pOwner || !pOwner->IsValidBody() )
    {
        return;
    }

    if ( pOwner->GetCurrentMap().getGaeaMapID() != m_pLandMan->GetGaeaMapID() )
    {
        return;
    }

    // Note : 정보 세팅
	bool bSummoning = false;
    PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sSummonNativeID );
    if ( !pCrowData )
    {
		if ( pOwner->GetCrow() == CROW_PC )
		{
			// 캐릭터 일때는 캐릭터가 들고있는 Hire Summon 정보로 소환 가능할 수도 있다;
			GLChar* pChar = static_cast< GLChar* >( pOwner );
			if ( pChar && pChar->m_sSummonable.bSummonable &&
				( !pChar->m_sSummonable.hireInfo.bSummoning ||
				pChar->m_sSummonable.hireInfo.bResummoning ) )
			{
				pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pChar->m_sBasicSummonID );
				if ( !pCrowData )
				{
					GLMSG::SNET_SUMMON_FAILED_DROP_SUMMON_FC MsgFC;
					MsgFC.emFC = EMFAILED_FAILED_DROP_SUMMON_NONHIRE;
					m_pGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &MsgFC );
					return;
				}

				// 현재 소환 패시브 스킬이 소환할 수 있는 Crow인지 판별;
				DWORD dwCrowLevel = static_cast< DWORD >( pCrowData->GetLevel() );
				DWORD dwMaxCrowLevel = (pChar->m_sSummonable.dwSummonLevel+1)*
					AISystem::GLFSMContainer::Instance()->GetHireSummonLevelInterval();
				if ( dwCrowLevel > dwMaxCrowLevel )
				{
					GLMSG::SNET_SUMMON_FAILED_DROP_SUMMON_FC MsgFC;
					MsgFC.emFC = EMFAILED_FAILED_OVERFLOW_CROWLEVEL;
					m_pGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &MsgFC );
					return;
				}

				m_sSummonNativeID = pChar->m_sBasicSummonID;
				bSummoning = true;
			}
			else
				return;
		}
		else
			return;
    }

	// 주인의 착용무기 체크;
	GLITEM_ATT emOwnerItemAtt = ITEMATT_NOTHING;
	if ( pOwner->GetCrow() == CROW_PC )
	{
		GLChar* pChar = static_cast< GLChar* >( pOwner );
		if ( pChar )
		{
			EMSLOT emRHand = pChar->GetCurRHand();
			SITEM* pRightItem = pChar->GET_SLOT_ITEMDATA( emRHand );		
			if ( pRightItem )
				emOwnerItemAtt = pRightItem->sSuitOp.emAttack;
		}
	}

    PGLSUMMON pSummonData = GLCONST_SUMMON::GetSummonData ( pCrowData->m_emSummonType );
    if ( !pSummonData )
    {
        return;
    }

    GLSUMMON NewSummonData(*pSummonData);
    NewSummonData.m_sSummonID          = m_sSummonNativeID;
    NewSummonData.m_dwOwnerGaeaID      = pOwner->GetGaeaID();
	NewSummonData.m_sOwnerID			= pOwner->GetActorID();
    NewSummonData.m_fRemainedTime      = m_fSummonRemainedTime;
    NewSummonData.m_fOwnerIncHP        = m_fOwnerIncHP;
    NewSummonData.m_wOwnerAttack       = m_wOwnerAttack;
    NewSummonData.m_wOwnerDefense      = m_wOwnerDefense;
    NewSummonData.m_cOwnerWeaponsGrade = m_cOwnerWeaponsGrade;
    NewSummonData.m_wOwnerSkillLevel   = m_wLevel;
    NewSummonData.m_sOwnerBlow         = m_sOwnerBlow;
	NewSummonData.m_emOwnerItemAtt	   = emOwnerItemAtt;
	NewSummonData.m_emNeedItemAtt	   = m_emNeedItemAtt;
    NewSummonData.m_wOwnerSchool       = m_wOwnerSchool;

	//NewSummonData.m_fOwnerDamagePer = 30.f;

    // Note : 소환수 Drop
    DWORD dwSummonGlobID = m_pLandMan->DropSummon( &NewSummonData, m_sEvent.vPos, m_sTarget, m_dwSummonMax, m_dwType, m_bEventSkill );
    if ( dwSummonGlobID != UINT_MAX )
    {
        GLSummonField* pSummon = m_pLandMan->GetSummon(dwSummonGlobID);
        if (!pSummon)
        {
            m_pLandMan->DropOutSummon(dwSummonGlobID);
            return;
        }

		// 캐릭터가 고용소환수를 소환했을 경우 GaeaID를 등록한다;
		if ( pOwner->GetCrow() == CROW_PC )
		{
			// 캐릭터 일때는 캐릭터가 들고있는 Hire Summon 정보로 소환 가능할 수도 있다;
			GLChar* pChar = static_cast< GLChar* >( pOwner );
			if ( pChar && pChar->m_sSummonable.bSummonable )
			{

				// bSummoning이 참이면 고용소환수이다;
				if ( bSummoning )
				{
					pChar->m_sSummonable.hireInfo.bSummoning = bSummoning;
					pChar->m_sSummonable.hireInfo.dwGaeaID = pSummon->GetGaeaID();
					pChar->MsgSendUpdateHireSummonState( pSummon->GetGaeaID(), m_sSummonNativeID );

					pChar->m_sSummonable.hireInfo.dwMaxSummon = m_dwSummonMax;
					pChar->m_sSummonable.hireInfo.fRemainedTime = pSummon->m_fRemainedTime;
					pChar->m_sSummonable.hireInfo.dwFamiliarType = pSummon->GetAIType();
					pChar->m_sSummonable.hireInfo.bEventSkill = pSummon->IsEventSkill();
					pChar->m_sSummonable.hireInfo.wSkillLevel = m_wLevel;

					// 재소환 상태 해제;
					pChar->m_sSummonable.hireInfo.bResummoning = false;

					pSummon->SetHireSummon( true );
					//sc::writeLogInfo( "Set Hire Summon!!!" );
				}
			}
		}

        GLMSG::SNET_SUMMON_DROP_SUMMON MsgFC;
        pSummon->ReqNetMsg_Drop(MsgFC);        
        MsgFC.Data.m_EventNodeSummonGateID = m_dwGlobID;
        ProcessViewAroundMsg(&MsgFC, CROW_SUMMON, dwSummonGlobID);
    }
}

void CEVENT_NODE_SERVER::ProcessViewAroundMsg(NET_MSG_GENERIC* pMsg, EMCROW emCrow, DWORD ObjectId) const
{
    if (!m_pLandMan)
    {
        sc::writeLogError(std::string("CEVENT_NODE_SERVER::ProcessViewAroundMsg m_pLandMan NULL"));
        return;
    }

    LANDQUADTREE* pQuadTree = m_pLandMan->GetLandTree();
    if (!pQuadTree)
    {
        sc::writeLogError(
            std::string(
            "CEVENT_NODE_SERVER::ProcessViewAroundMsg pQuadTree NULL Map %1%/%2%",
            m_pLandMan->GetBaseMapID().Mid(),
            m_pLandMan->GetBaseMapID().Sid()));
        return;
    }

    LANDQUADNODE* pRootNode = pQuadTree->GetRootNode();
    if (!pRootNode)
    {
        sc::writeLogError(
            std::string(
            "CEVENT_NODE_SERVER::ProcessViewAroundMsg pRootNode NULL Map %1%/%2%",
            m_pLandMan->GetBaseMapID().Mid(),
            m_pLandMan->GetBaseMapID().Sid()));
        return;
    }

    LANDQUADNODE* pQuadHead = NULL;
	const int nMyPosition[2] = 
	{
		int(m_vPosition.x),
		int(m_vPosition.z)
	};
    BOUDRECT bRect(nMyPosition[0]+MAX_VIEWRANGE, nMyPosition[1]+MAX_VIEWRANGE, nMyPosition[0]-MAX_VIEWRANGE, nMyPosition[1]-MAX_VIEWRANGE);
    pQuadTree->FindNodes(bRect, pRootNode, &pQuadHead);

    LANDQUADNODE* pQuadNode = pQuadHead;
    for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext)
    {
        GLLandNode *pLandNode = pQuadNode->pData;
        if( !pLandNode ) 
            continue;

        if ( pQuadNode->pData->m_PCList.m_dwAmount==0 )						
            continue;

        GLCHARNODE* pCharNode = pQuadNode->pData->m_PCList.m_pHead;
        for ( ; pCharNode; pCharNode = pCharNode->pNext)
        {
            GLChar* pChar = pCharNode->Data;
            if (pChar && m_pGaeaServer->GetChar(pChar->GetGaeaID()))
            {
				const D3DXVECTOR3& vPosition = pChar->GetPosition();
				const int nPosition[2] = 
				{
					int(vPosition.x),
					int(vPosition.z)
				};

                if (bRect.IsWithIn(nPosition[0], nPosition[1]))
                {
                    if (emCrow == CROW_PC)
                    {
                        m_pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), pMsg);
                    }
                    else if (emCrow == CROW_SUMMON)
                    {
                        if (pChar->AddFieldCrow(emCrow, ObjectId))
                            m_pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), pMsg);
                        else
						{
							GLLandMan* const pLand(pChar->GetLandMan());
							if ( pLand != NULL )
								pLand->DropOutSummon(ObjectId);
						}
                    }
                    else
                    {
                        sc::writeLogError(
                            std::string(
                                "CEVENT_NODE_SERVER::ProcessViewAroundMsg unkwnon type %1%",
                                static_cast<int> (emCrow)));
                    }
                }
            }
        }
    }
}

void CEVENT_NODE_SERVER_SUMMONGATE::FrameEnd()
{
    // Blank
}

void CEVENT_NODE_SERVER_SUMMONGATE::ReqNetMsg_Drop(GLMSG::SNETLANDEVENT_SUMMONGATE_CREATE& Msg)
{    
    Msg.dwGaeaID     = m_dwGlobID;
    Msg.fTimeCurrent = m_fTimeCurrent;
    Msg.fTimeEnd     = m_fTimeEnd;
    Msg.fTimeEvent   = m_fTimeEvent;
    Msg.sMapID       = m_pLandMan->GetGaeaMapID();
    Msg.vPos         = m_vPosition;
    Msg.skill_id     = m_sSkillID;
    Msg.skill_lev    = m_wLevel;
    Msg.dwOwnerID    = m_sOwnerID.actorID_Num;
    Msg.sEvent       = m_sEvent;
    Msg.sTarget      = m_sTarget;
    //return NetMsg;
}

//-------------------------------------------------------------------//
