#include "../pch.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"

#include "./GLPartyField.h"
#include "./GLPartyTenderManagerField.h"

#include "../../SigmaCore/DebugInclude.h"

GLPartyField::GLPartyField(GLGaeaServer* pGaeaServer)
    : m_pGLGaeaServer(pGaeaServer)
{
	m_cMEMBER.reserve( MAXPARTY );
	RESET();
}

void GLPartyField::RESET ()
{
	m_PartyID.Reset();	

	m_bQBoxEnable = TRUE;
	
	m_cMEMBER.clear();

	m_ConfrontPartyID.Reset();
	m_dwConfrontMemberAverageLevel = 1;

	m_arrayROUND_ITEM_CUR.clear();
	m_arrayROUND_ITEM_CUR.reserve(MAXPARTY);

	for ( int i=0; i<MAXPARTY; ++i )
	{
		m_arrayROUND_ITEM_CUR.push_back(0);

		m_arrayROUND_ITEM[i].clear();
		m_arrayROUND_ITEM[i].reserve(MAXPARTY);
		for ( int j=0; j<MAXPARTY; ++j )
		{
			m_arrayROUND_ITEM[i].push_back(j);
		}

		std::random_shuffle ( m_arrayROUND_ITEM[i].begin(), m_arrayROUND_ITEM[i].end() );
	}

	m_arrayROUND_MONEY_CUR.clear();
	m_arrayROUND_MONEY_CUR.reserve(MAXPARTY);

	for ( int i=0; i<MAXPARTY; ++i )
	{
		m_arrayROUND_MONEY_CUR.push_back(0);

		m_arrayROUND_MONEY[i].clear();
		m_arrayROUND_MONEY[i].reserve(MAXPARTY);
		for ( int j=0; j<MAXPARTY; ++j )
		{
			m_arrayROUND_MONEY[i].push_back(j);
		}

		std::random_shuffle ( m_arrayROUND_MONEY[i].begin(), m_arrayROUND_MONEY[i].end() );
	}
}

BOOL GLPartyField::ISVAILD ()
{
	return m_cMEMBER.size() ? TRUE : FALSE;
}


void GLPartyField::SetMemberSlot(const DWORD dwSlotIndex, const DWORD dwGaeaID, const GLPARTY_FNET& MemberSlot)
{
	if ( dwSlotIndex > (m_cMEMBER.size() - 1) )
		return;

	m_cMEMBER[dwSlotIndex].first = dwGaeaID;
	m_cMEMBER[dwSlotIndex].second = MemberSlot;
}


BOOL GLPartyField::ISFULL ()
{
	return (m_cMEMBER.size() == MAXPARTY);
}

const DWORD GLPartyField::GetNMember(void) const
{
	return (DWORD)m_cMEMBER.size();
}

const DWORD GLPartyField::GetMasterGaeaID(void) const
{	
	return m_cMEMBER.size() ? m_cMEMBER[0].first : GAEAID_NULL;
}

const DWORD GLPartyField::GetPartyMemberGaeaIDs(PartyMemberGaeaIDs& dwPartyMemberGaeaIDs) const
{
	GLPartyField::MEMBER_CITER iter = m_cMEMBER.begin ();
	GLPartyField::MEMBER_CITER iter_end = m_cMEMBER.end ();
	const size_t nPartyMember = m_cMEMBER.size();
	GASSERT(!(nPartyMember > MAXPARTY));

	DWORD nMember = 0;
	for ( ; iter != iter_end; ++iter )
		dwPartyMemberGaeaIDs[nMember++] = (*iter).first;

	return nMember;
}

const GLPartyID& GLPartyField::GetConfrontPartyID(void) const
{
	return m_ConfrontPartyID;
}
const GLPartyID& GLPartyField::GetPartyID(void) const
{
	return m_PartyID;
}

const SPartyOption& GLPartyField::GetPartyOption(void) const
{
	return m_sOPTION;
}

const DWORD GLPartyField::GetMemberSlotIndex(const DWORD dwGaeaID) const
{
	MEMBER_CITER iter = m_cMEMBER.begin();
	MEMBER_CITER iter_end = m_cMEMBER.end();
	DWORD dwSlotIndex = 0;
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET* const pMember = (GLPARTY_FNET*)&iter->second;
		if ( pMember->GetGaeaID() == dwGaeaID )
			return dwSlotIndex;
		++dwSlotIndex;
	}

	return DEFSLOT_NULL;
}

GLPARTY_FNET* const GLPartyField::GetMemberSlot(const DWORD dwGaeaID) const
{
	if ( dwGaeaID == GAEAID_NULL )
		return NULL;

	MEMBER_CITER iter = m_cMEMBER.begin();
	MEMBER_CITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET* const pMember = (GLPARTY_FNET*)&iter->second;
		if ( pMember->GetGaeaID() == dwGaeaID )
			return pMember;
	}

	return NULL;
}

const GLPARTY_FNET* const GLPartyField::GetMasterSlot(void) const
{
	if ( m_cMEMBER.size() == 0 )
		return NULL;

	return &m_cMEMBER[MASTER_PARTY].second;
}

const bool GLPartyField::isConfront(const DWORD dwGaeaID) const
{

	MEMBER_CITER iter = m_cMEMBER.begin();
	MEMBER_CITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		const GLPARTY_FNET& sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
			return sMember.m_bConfront;
	}

	return false;
}

const bool GLPartyField::isMasterByGaeaID(const DWORD dwGaeaID) const
{
	if ( m_cMEMBER.size() == 0 )
		return false;
	return (m_cMEMBER[0].first == dwGaeaID);
}

const bool GLPartyField::isMemberByGaeaID(const DWORD dwGaeaID) const 
{
	MEMBER_CITER iter = m_cMEMBER.begin();
	MEMBER_CITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		const GLPARTY_FNET& sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
			return true;
	}

	return false;
}

const SCONFT_OPTION& GLPartyField::GetConfrontOption(void) const
{
	return m_ConfrontOption;
}

const bool GLPartyField::MBR_CONFRONT_JOIN(const DWORD dwGaeaID)
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
		{
			sMember.m_bConfront = true;
			return true;
		}
	}

	return false;
}

const bool GLPartyField::MBR_CONFRONT_LEAVE(const DWORD dwGaeaID)
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
		{
			sMember.m_bConfront = false;
			return true;
		}
	}

	return false;
}

void GLPartyField::SetQBox(const bool bEnable)
{
	m_bQBoxEnable = bEnable;
}
void GLPartyField::SetConfrontOption(const SCONFT_OPTION& ConfrontOption)
{
	m_ConfrontOption = ConfrontOption;
}
void GLPartyField::SetConfrontPartyID(const GLPartyID& PartyID)
{
	m_ConfrontPartyID = PartyID;
}

void GLPartyField::SetPartyID(const GLPartyID& PartyID)
{
	m_PartyID = PartyID;
}

void GLPartyField::LogMonsterKill(GLGaeaServer* const pGaeaServer, const SNATIVEID& MonsterID, const std::string& MonsterName)
{
	if (!pGaeaServer)
		return;

	if (GLogicData::GetInstance().IsClubLogKillMonster(MonsterID))
	{   
		for (size_t i=0; i<m_cMEMBER.size(); ++i)
		{
			GLChar* pChar = pGaeaServer->GetChar(m_cMEMBER[i].first);
			if (pChar)
			{
				DWORD ClubDbNum = pChar->GetClubDbNum();
				if (ClubDbNum != CLUB_NULL)
				{
					// [%1%]님이 [%2%]을(를) 쓰러뜨렸습니다.
					std::string ClubLog(
						sc::string::format(
						ID2GAMEINTEXT("CLUB_LOG_27"),
						pChar->GetName(),
						MonsterName));
					pGaeaServer->SENDCLUBLOG(ClubDbNum, club::LOG_KILL_MONSTER, ClubLog);
				}
			}
		}
	}
}

const DWORD GLPartyField::ADDMEMBER ( GLPARTY_FNET &sParty )
{
	if ( ISFULL () )
        return DEFSLOT_NULL;

	if ( isMemberByGaeaID(sParty.m_dwGaeaID) )
        return DEFSLOT_NULL;

	m_cMEMBER.push_back(std::make_pair(sParty.m_dwGaeaID, sParty));

	return m_cMEMBER.size() - 1;
}

const bool GLPartyField::removeMember(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID)
{
	GLChar* pChar = pGaeaServer->GetChar(dwGaeaID);
	if ( pChar != NULL )
	{
		pChar->ResetPartyID();

		GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
		NetMsgPartyBrd.dwGaeaID = dwGaeaID;
		NetMsgPartyBrd.PartyID.Reset(); // 파티 소속 제거;
		NetMsgPartyBrd.dwPMasterID = GAEAID_NULL;
		pChar->SendMsgViewAround(&NetMsgPartyBrd);		
	}

	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
		{
			m_cMEMBER.erase(iter);
			return true;
		}
	}

	return false;
}

BOOL GLPartyField::MBR_MOVEMAP(const DWORD dwGaeaID, const MapID& sMapID )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
		{
			sMember.m_sMapID = sMapID;
			return TRUE;
		}
	}

	return FALSE;
}

const MapID GLPartyField::GETMBR_MAP ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for( ; iter != iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		if ( sMember.m_dwGaeaID == dwGaeaID )
            return sMember.m_sMapID;
	}
	return MapID();
}

void GLPartyField::RESET_CONFRONT(GLGaeaServer* const pGaeaServer, const EMCONFRONT_END emEND )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();

	GLMSG::SNETPC_CONFRONTPTY_END2_CLT NetMsgClt(emEND);
	GLMSG::SNETPC_CONFRONT_END2_CLT_BRD NetMsgBrd(emEND);

	const DWORD dwMaster = m_cMEMBER.size() ? m_cMEMBER[0].first : GAEAID_NULL;
	GLChar *pMaster = pGaeaServer->GetChar ( dwMaster );
	if( !pMaster )
		return;

	for ( ; iter!=iter_end; ++iter )
	{
		GLPARTY_FNET &sMember = (*iter).second;
		sMember.m_bConfront = false;
		GLLandMan* pLandMan = NULL;

		GLChar *pChar = pGaeaServer->GetChar ( sMember.m_dwGaeaID );
		if ( pChar )
		{
			if ( pChar->m_sCONFTING.IsCONFRONTING() )
			{
                //  대련 종료 처리
				pChar->EndConfting();
			}

			//
			//mjeon.attendance
			//
			//pChar->AttendanceCheckSparingParty();

           
			// 대련종료 후 무적시간 설정
			//	Note : 파티 대련 종료를 클라이언트들에 알림.	
			if( pMaster->GetCurrentMap() == sMember.m_sMapID )
			{
				pChar->m_sCONFTING.SETPOWERFULTIME ( 10.0f ); 
				pGaeaServer->SENDTOCLIENT ( pChar->ClientSlot(), &NetMsgClt );
			}

			NetMsgBrd.dwGaeaID = pChar->GetGaeaID();			
			if ( emEND==EMCONFRONT_END_PWIN || emEND==EMCONFRONT_END_PLOSS )
			{
				if( pMaster->GetCurrentMap() == sMember.m_sMapID )
				{
					pChar->SendMsgViewAround(&NetMsgBrd);
				}
				
			}
		}
	}
	m_ConfrontPartyID.Reset();
}

const bool GLPartyField::DOITEM_ALLOT(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, CItemDrop* pItemDrop)
{
	GASSERT(pGaeaServer&&"GLPARTY_FIELD::DOITEM_ALLOT()");
	if ( !pGaeaServer )
        return false;

	GASSERT(pItemDrop&&"GLPARTY_FIELD::DOITEM_ALLOT()");
	if ( !pItemDrop )
        return false;

	GLChar* const pKnockChar = pGaeaServer->GetChar(dwGaeaID);
	switch ( m_sOPTION.GetItemOption() )
	{
	case EMPARTY_ITEM_LEADER:
		{
			const DWORD dwMasterGaeaID = m_cMEMBER.size() ? m_cMEMBER[0].first : GAEAID_NULL;
			PGLCHAR pMaster = m_pGLGaeaServer->GetChar(dwMasterGaeaID);
			
			//	파티 마스터가 존제하지 않으므로 루팅 하지 않음.
			if ( !pMaster )
				return false;
			if ( pMaster->GetCurrentMap() != pKnockChar->GetCurrentMap() )
				return false;

			//	파티 마스터가 유효 거리내에 존재하는지 검사.
			const D3DXVECTOR3 vDist = pMaster->GetPosition() - pKnockChar->GetPosition();
			const float fDist = D3DXVec3Length(&vDist);
			if ( fDist > MAX_VIEWRANGE )
				return false;

			if (pMaster->InsertToInven( pItemDrop, true, true, true, false, true ))
				return true;
		}
		return false;

	case EMPARTY_ITEM_ROUND:
		{
			//	Note : 멤버 배열 정리.
			//
			DWORD dwKnockIndex(MAXPARTY);
			DWORD dwMemberNum(0);
			DWORD dwaryMEMBER[MAXPARTY];
			for ( int i=0; i<MAXPARTY; ++i )
				dwaryMEMBER[i] = GAEAID_NULL;

			MEMBER_ITER iter = m_cMEMBER.begin();
			MEMBER_ITER iter_end = m_cMEMBER.end();
			for ( ; iter!=iter_end; ++iter )
			{
				GLPARTY_FNET &sMember = (*iter).second;

				// 아이템을 노크한 케릭터의 인덱스(순서) 저장
				if ( pKnockChar->GetGaeaID() == sMember.m_dwGaeaID )
					dwKnockIndex = dwMemberNum;
				// 멤버배열에 GaeaID 설정
				dwaryMEMBER[dwMemberNum++] = sMember.m_dwGaeaID;
			}

			// 아무도 없으면
			if ( dwMemberNum==0 )
				return false;

			// 노크한 케릭터가 없으면
			if ( dwKnockIndex==MAXPARTY )
				return false;

			// 현재 노크한 케릭터의 습득순서를 갖는 인덱스
			DWORD &dwCUR = m_arrayROUND_ITEM_CUR[dwKnockIndex];
			DWORD dwMEMBERID = GAEAID_NULL;
			for ( int i=0; i<MAXPARTY; ++i )
			{
				// 현재 노크한 케릭터가 갖고있는 아이템 습득인덱스(순서)
				DWORD dwALLOT_GAEAID = m_arrayROUND_ITEM[dwKnockIndex][dwCUR++];
				if ( dwCUR >= MAXPARTY )
					dwCUR = 0;

				// 이번에 아이템을 습득할 케릭터의 GaeaID
				dwMEMBERID = dwaryMEMBER[dwALLOT_GAEAID];
				if(dwMEMBERID == GAEAID_NULL)
					continue;

				PGLCHAR pMember = m_pGLGaeaServer->GetChar(dwMEMBERID);
				if ( !pMember )
					continue;
				if ( pMember->GetCurrentMap() != pKnockChar->GetCurrentMap() )
					continue;
				if ( !pMember->IsValidBody() )
					continue;

				//	파티원이 유효 거리내에 존재하는지 검사.
				const D3DXVECTOR3 vDist = pMember->GetPosition() - pKnockChar->GetPosition();
				const float fDist = D3DXVec3Length(&vDist);
				if ( fDist > MAX_VIEWRANGE )
					continue;

				if (pMember->InsertToInven(pItemDrop, true, true, true, false, true))
					return true;
			}
		}
		return false;

	case EMPARTY_ITEM_DICE:
	case EMPARTY_ITEM_LEADER_SHARE:
		{
			GLPartyTenderManagerField* const pTenderManager = m_pGLGaeaServer->GetPartyTenderItemManager();			
			if ( pTenderManager->isLootable(pKnockChar->GetPartyID(), pItemDrop->dwGlobID) == false || pItemDrop->bLock )
			{
				GLMSG::SNETPC_REQ_TAKE_FB NetMsg;
				NetMsg.emCrow = CROW_ITEM;
				NetMsg.emTakeFB = EMTAKE_FB_TENDERING;

				pKnockChar->SendToClient( &NetMsg );
				return true;
			}
		}		
		break;
	default:
		break;
	};

	return false;
}

const bool GLPartyField::DOMONEY_ALLOT(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, CMoneyDrop* pMoneyDrop, GLLandMan* pLandMan)
{
	GASSERT(pGaeaServer&&"GLPARTY_FIELD::DOMONEY_ALLOT()");
	if ( !pGaeaServer )
		return false;

	GASSERT(pMoneyDrop&&"GLPARTY_FIELD::DOMONEY_ALLOT()");
	if ( !pMoneyDrop )
		return false;

	GASSERT(pLandMan&&"GLPARTY_FIELD::DOMONEY_ALLOT()");
	if ( !pLandMan )
		return false;

	GLChar* const pKnockChar = pGaeaServer->GetChar(dwGaeaID);
	if ( pKnockChar == NULL )
		return false;

	switch ( m_sOPTION.GetMoneyOption() )
	{
	case EMPARTY_MONEY_LEADER:
		{
			const DWORD dwMasterGaeaID = m_cMEMBER.size() ? m_cMEMBER[0].first : GAEAID_NULL;
			PGLCHAR pMaster = pGaeaServer->GetChar(dwMasterGaeaID);
			
			//	파티 마스터가 존제하지 않으므로 루팅 하지 않음.
			if (!pMaster || (pMaster->GetCurrentMap() != pKnockChar->GetCurrentMap()) )
				return false;

			//	파티 마스터가 유효 거리내에 존재하는지 검사.
			const D3DXVECTOR3 vDist(pMaster->GetPosition() - pKnockChar->GetPosition());
			const float fDist = D3DXVec3Length(&vDist);
			if (fDist > MAX_VIEWRANGE)
				return false;

			if (pMaster->InsertToInven(pMoneyDrop))
				return true;
		}
		return false;
	case EMPARTY_MONEY_EQUAL:
		{
			//	Note : 멤버 배열 정리.
			//
			DWORD dwMemberNum(0);
			DWORD dwaryMEMBER[MAXPARTY];
			for ( int i=0; i<MAXPARTY; ++i )
				dwaryMEMBER[i] = GAEAID_NULL;

			MEMBER_ITER iter = m_cMEMBER.begin();
			MEMBER_ITER iter_end = m_cMEMBER.end();
			for ( ; iter!=iter_end; ++iter )
			{
				GLPARTY_FNET &sMember = (*iter).second;

				const DWORD dwMemberID = sMember.m_dwGaeaID;

				PGLCHAR pMember = m_pGLGaeaServer->GetChar(dwMemberID);
				if ( !pMember )
					continue;
				if ( pMember->GetCurrentMap() != pKnockChar->GetCurrentMap() )
					continue;
				if ( !pMember->IsValidBody() )
					continue;

				//	파티원이 유효 거리내에 존재하는지 검사.
				D3DXVECTOR3 vDist = pMember->GetPosition() - pKnockChar->GetPosition();
				float fDist = D3DXVec3Length(&vDist);
				if ( fDist > MAX_VIEWRANGE )
					continue;

				dwaryMEMBER[dwMemberNum++] = dwMemberID;
			}
			if ( dwMemberNum==0 )
				return false;

			LONGLONG lnAmount = pMoneyDrop->lnAmount;

			LONGLONG lnSplit = lnAmount / dwMemberNum;		//	균등 분할.
			LONGLONG lnSurplus = lnAmount % dwMemberNum;	//	균등 분할후 나머지.

			for ( DWORD dwIndex=0; dwIndex<dwMemberNum; dwIndex++ )
			{
				PGLCHAR pMember = pGaeaServer->GetChar(dwaryMEMBER[dwIndex]);
				if ( !pMember )
					continue;

				LONGLONG lnInsertMoney = lnSplit;
				if ( lnSurplus>0 )
				{
					lnSurplus -= 1;
					lnInsertMoney += 1;
				}
				pMember->InsertMoney ( lnInsertMoney );
			}

			pLandMan->DropOutMoney ( pMoneyDrop->dwGlobID );
		}
		return true;

	default:
		break;
	};

	return false;
}

// bool GLPartyField::SENDMSG2NEAR (GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, NET_MSG_GENERIC* pNetMsg )
// {
// 	GASSERT(pGaeaServer&&"GLPARTY_FIELD::SENDMSG2NEAR()");
// 	if (!pGaeaServer)
//         return false;
// 
// 	GLChar* const pKnockChar = pGaeaServer->GetChar(dwGaeaID);
// 	GASSERT(pKnockChar&&"GLPARTY_FIELD::SENDMSG2NEAR()");
// 	if ( !pKnockChar )
// 		return false;
// 
// 	GASSERT(pNetMsg&&"GLPARTY_FIELD::SENDMSG2NEAR()");
// 	if (!pNetMsg)
//         return false;
// 
// 	MEMBER_ITER iter = m_cMEMBER.begin();
// 	MEMBER_ITER iter_end = m_cMEMBER.end();
// 	for ( ; iter!=iter_end; ++iter )
// 	{
// 		GLPARTY_FNET &sMember = (*iter).second;
// 		const DWORD dwMemberID = sMember.m_dwGaeaID;
// 		if ( pKnockChar->GetGaeaID() == dwMemberID )
// 			continue;
// 
// 		PGLCHAR pMember = m_pGLGaeaServer->GetChar(dwMemberID);
// 		if ( !pMember )
// 			continue;
// 		if ( pMember->m_sMapID!=pKnockChar->m_sMapID )
// 			continue;
// 
// 		//	파티원이 유효 거리내에 존재하는지 검사.
// 		D3DXVECTOR3 vDist(pMember->GetPosition() - pKnockChar->GetPosition());
// 		float fDist = D3DXVec3Length(&vDist);
// 		if ( fDist > MAX_VIEWRANGE )
// 			continue;
// 	
// 		m_pGLGaeaServer->SENDTOCLIENT(pMember->ClientSlot(), pNetMsg);
// 	}
// 
// 	return true;
// }

// const bool GLPartyField::SENDPICKUPMSG2NEAR(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, SITEMCUSTOM* pItemCustom)
// {
//     GASSERT(pGaeaServer&&"GLPARTY_FIELD::SENDPICKUPMSG2NEAR()");
//     if ( !pGaeaServer )
// 		return false;
// 
//     GASSERT(pItemCustom&&"GLPARTY_FIELD::SENDPICKUPMSG2NEAR()");
//     if ( !pItemCustom )
// 		return false;
// 
//     const SITEM* pItem = GLogicData::GetInstance().GetItem(pItemCustom->sNativeID);
//     GASSERT(pItem&&"GLPARTY_FIELD::SENDPICKUPMSG2NEAR()");
//     if ( !pItem )
// 		return false;
// 
//     // 아이템의 가치가 일반보다 높으면 아이템 링크 출력
//     if ( LEVEL_NORMAL < pItem->sBasicOp.emLevel )
//     {
//         GLMSG::SNETPC_PARTY_MBR_PICKUP_LINK_BRD NetMsg;
//         NetMsg.dwGaeaID  = dwGaeaID;
//         NetMsg.sItem     = (*pItemCustom);
//         return SENDMSG2NEAR(pGaeaServer, dwGaeaID, &NetMsg );
//     }
//     else
//     {
//         GLMSG::SNETPC_PARTY_MBR_PICKUP_BRD NetMsg;
//         NetMsg.dwGaeaID  = dwGaeaID;
//         NetMsg.sNID_ITEM = pItemCustom->sNativeID;
//         return SENDMSG2NEAR(pGaeaServer, dwGaeaID, &NetMsg );
//     }
// 
//     return false;
// }

const DWORD GLPartyField::GETAVER_CONFT_LEVEL(GLGaeaServer* const pGaeaServer)
{
	DWORD dwMem(0);
	DWORD dwTotalLevel(0);
	
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		const GLPARTY_FNET &sPARTY_MEM = (*iter).second;
		if ( sPARTY_MEM.m_bConfront )
		{
			PGLCHAR pMember = pGaeaServer->GetChar(sPARTY_MEM.m_dwGaeaID);
			if ( !pMember )
				continue;

			dwMem++;
			dwTotalLevel += pMember->m_wLevel;
		}
	}
	
	if ( dwMem==0 )
		return 0;

	m_dwConfrontMemberAverageLevel = (dwTotalLevel/dwMem);
	return m_dwConfrontMemberAverageLevel;
}

void GLPartyField::RECEIVELIVINGPOINT(GLGaeaServer* const pGaeaServer, const int nLivePoint )
{
	MEMBER_ITER iter = m_cMEMBER.begin();
	MEMBER_ITER iter_end = m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		const GLPARTY_FNET &sPARTY_MEM = (*iter).second;
		if ( sPARTY_MEM.m_bConfront )
		{
			PGLCHAR pMember = pGaeaServer->GetChar(sPARTY_MEM.m_dwGaeaID);
			if ( !pMember )
				continue;

			pMember->ReceiveLivingPoint ( nLivePoint );
		}
	}	
}


void GLPartyField::SetMemberRevival()
{
    BOOST_FOREACH( const PARTYPAIR& Member, m_cMEMBER )
    {
        const GLPARTY_FNET& sPlayer = Member.second;
        GLChar* pChar = m_pGLGaeaServer->GetChar( sPlayer.GetGaeaID() );

        if( pChar )
            pChar->SetAlive_PVE();
        
    }
}
