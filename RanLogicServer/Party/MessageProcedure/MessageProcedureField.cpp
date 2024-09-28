#include "../../pch.h"
#include "../../Character/GLChar.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "../../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../../TriggerSystem/ServerPartyOutMessage.h"
#include "../GLPartyField.h"
#include "../GLPartyManagerField.h"

BOOL GLPartyManagerField::MsgPartyFNewUpdate(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_FNEW_UPDATE *pNetMsg((GLMSG::SNET_PARTY_FNEW_UPDATE*)nmg);
	
	const DWORD nMember(pNetMsg->nMember);
	if ( nMember == 0 )
		return FALSE;

	PGLCHAR _pChar(m_pGaeaServer->GetChar(pNetMsg->dwGaeID));

	if( _pChar == NULL ) return FALSE;

	// 파티가 제대로 세팅되어 있다면 메시지처리를 하지 않는다;
	if( _pChar->GetPartyID() != PARTY_NULL )
	{
		return TRUE;
	}

	const GLPartyID& _partyID(pNetMsg->PartyID);

	GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;

	{
		//	Note : 현제 필드 서버에 존제하는 케릭터의 경우 파티 ID 설정.
		SetPartyID ( pNetMsg->dwGaeID, _partyID );

		m_pGaeaServer->SENDTOCLIENT ( _pChar->ClientSlot(), pNetMsg );

		//	Note : 파티에 참가하는 케릭 주위(플레이어)에 파티소속 정보를 전송.			
		NetMsgPartyBrd.dwGaeaID = _pChar->GetGaeaID();
		NetMsgPartyBrd.PartyID  = _partyID;
		NetMsgPartyBrd.dwPMasterID = pNetMsg->dwMaster;

		_pChar->SendMsgViewAround(&NetMsgPartyBrd);		

		for ( DWORD _j(0); _j < nMember; ++_j )
		{ //	Note : 파티 멤버간 종전 적대행위 리스트 삭제.
			GLPARTY_FNET &sMember = pNetMsg->sPartyInfo[_j];
			PGLCHAR pMEMBER(m_pGaeaServer->GetChar ( sMember.m_dwGaeaID ));
			if ( pMEMBER )
			{
				_pChar->RemovePlayHostile( pMEMBER->m_CharDbNum );
			}
		}
	}


	GLPartyField* pParty = GetParty(_partyID);

	if( pParty && _pChar )
	{
		std::vector<DWORD> vecNotify;
		vecNotify.reserve(MAXPARTY);

		const DWORD dwMemberGaeaID(pNetMsg->dwGaeID);
		//GLChar* pMember(m_pGaeaServer->GetChar(dwMemberGaeaID));
		GLChar* pMember = _pChar;

		pMember->AttendanceCheckParty((UINT)pParty->m_cMEMBER.size());

		GLLandManager* const pLandManager(pMember->GetParentLandManager());
		if ( pLandManager == NULL )
			return FALSE;

		const DWORD instanceMapID(pLandManager->getInstanceMapMainID());
		std::vector<DWORD>::iterator _iteratorInstanceMapID(std::find(vecNotify.begin(), vecNotify.end(), instanceMapID));
		if ( _iteratorInstanceMapID != vecNotify.end() )
			return FALSE;

		InstanceSystem::InstanceField* const _pInstanceManager(pLandManager->GetScriptInstance());
		if ( _pInstanceManager == NULL )
			return FALSE;

		vecNotify.push_back(instanceMapID);
		for ( GLPartyField::MEMBER_CITER iter(pParty->m_cMEMBER.begin()); iter != pParty->m_cMEMBER.end(); ++iter )
		{
			const GLPARTY_FNET& _member((*iter).second);
			const DWORD _instanceMapIDMember(DWORD(_member.m_sMapID.getGaeaMapID().Mid()));
			_pInstanceManager->EventPartyJoin(_partyID, _partyID.isValidExpedition(), _member.GetChaDbNum(), instanceMapID == _instanceMapIDMember);
		}		
	}

	return TRUE;
}

BOOL GLPartyManagerField::MsgPartyFNew(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_FNEW *pNetMsg((GLMSG::SNET_PARTY_FNEW*)nmg);

	const DWORD nMember(pNetMsg->nMember);
	if ( nMember == 0 )
		return FALSE;

	const GLPartyID& _partyID(pNetMsg->PartyID);
	GLPartyField* pParty(NewParty(pNetMsg->dwMaster, _partyID));
	if ( pParty == NULL )
		return FALSE;

	pParty->m_sOPTION = pNetMsg->sOption;

	GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;

	for ( DWORD _index(0); _index < nMember; ++_index )
	{
		GLPARTY_FNET& sPartyInfo(pNetMsg->sPartyInfo[_index]);

		pParty->ADDMEMBER(sPartyInfo);

		// 현재 파티원 수 기록;
		GLLandMan* pLand = m_pGaeaServer->GetLand( sPartyInfo.m_sMapID );
		if ( pLand )
		{
			pLand->AddCurPartyPlayerCount( pNetMsg->PartyID.GetPartyID() );
		}

		//	Note : 현제 필드 서버에 존제하는 케릭터의 경우 파티 ID 설정.
		SetPartyID ( sPartyInfo.m_dwGaeaID, _partyID );

		//	Note : 파티 생성 정보 클라이언트에 전송.
		PGLCHAR pChar(m_pGaeaServer->GetChar(sPartyInfo.m_dwGaeaID));
		if ( pChar == NULL )
			continue;

		m_pGaeaServer->SENDTOCLIENT ( pChar->ClientSlot(), pNetMsg );

		//	Note : 파티에 참가하는 케릭 주위(플레이어)에 파티소속 정보를 전송.			
		NetMsgPartyBrd.dwGaeaID = pChar->GetGaeaID();
		NetMsgPartyBrd.PartyID = pParty->m_PartyID;
		NetMsgPartyBrd.dwPMasterID = pParty->GetMasterGaeaID();
		pChar->SendMsgViewAround(&NetMsgPartyBrd);		

		
		for ( DWORD _j(0); _j < nMember; ++_j )
		{ //	Note : 파티 멤버간 종전 적대행위 리스트 삭제.
			GLPARTY_FNET &sMember = pNetMsg->sPartyInfo[_j];
			PGLCHAR pMEMBER(m_pGaeaServer->GetChar ( sMember.m_dwGaeaID ));
			if ( pMEMBER )
			{
				pChar->RemovePlayHostile( pMEMBER->m_CharDbNum );
			}
		}
	}

	std::vector<DWORD> vecNotify;
	vecNotify.reserve(MAXPARTY);

	//	Note : 각각 파티원의 세부-정보를 전송해줌.
	//
	for ( DWORD _index(0); _index < nMember; ++_index )
	{
		const DWORD dwMemberGaeaID(pNetMsg->sPartyInfo[_index].m_dwGaeaID);
		GLChar* pMember(m_pGaeaServer->GetChar(dwMemberGaeaID));
		if ( pMember == NULL )
			continue;
		pMember->AttendanceCheckParty((UINT)pParty->m_cMEMBER.size());

		GLLandManager* const pLandManager(pMember->GetParentLandManager());
		if ( pLandManager == NULL )
			continue;		

		const DWORD instanceMapID(pLandManager->getInstanceMapMainID());
		std::vector<DWORD>::iterator _iteratorInstanceMapID(std::find(vecNotify.begin(), vecNotify.end(), instanceMapID));
		if ( _iteratorInstanceMapID != vecNotify.end() )
			continue;

		InstanceSystem::InstanceField* const _pInstanceManager(pLandManager->GetScriptInstance());
		if ( _pInstanceManager == NULL )
			continue;

		vecNotify.push_back(instanceMapID);
		for ( GLPartyField::MEMBER_CITER iter(pParty->m_cMEMBER.begin()); iter != pParty->m_cMEMBER.end(); ++iter )
		{
			const GLPARTY_FNET& _member((*iter).second);
			const DWORD _instanceMapIDMember(DWORD(_member.m_sMapID.getGaeaMapID().Mid()));
			_pInstanceManager->EventPartyJoin(_partyID, _partyID.isValidExpedition(), _member.GetChaDbNum(), instanceMapID == _instanceMapIDMember);
		}		
	}

	return TRUE;
}

BOOL GLPartyManagerField::MsgPartyAdd(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_ADD *pNetMsg = ( GLMSG::SNET_PARTY_ADD * ) nmg;

	const GLPartyID& _partyID(pNetMsg->PartyID);
	GLPartyField* const pParty = GetParty( _partyID );

	if ( !pParty )
		return FALSE;

	GLPARTY_FNET& sPartyInfo(pNetMsg->sPartyInfo);	
	const DWORD instanceMapID(sPartyInfo.m_sMapID.getGaeaMapID().Mid());	

    std::vector< DWORD > vecNotify;
    vecNotify.reserve(MAXPARTY);
	for ( GLPartyField::MEMBER_ITER iter(pParty->m_cMEMBER.begin()); iter != pParty->m_cMEMBER.end(); ++iter )
	{
		GLChar* const pCharMem(m_pGaeaServer->GetChar((*iter).first));
		if ( !pCharMem )
			continue;
		
		GLLandManager* const pLandManager(pCharMem->GetParentLandManager());
		InstanceSystem::InstanceField* const _pInstanceManager(pLandManager->GetScriptInstance());
		if ( _pInstanceManager == NULL )
			continue;

		const DWORD _instanceMapIDMember(pLandManager->getInstanceMapMainID());
		std::vector<DWORD>::iterator _iteratorInstanceMapID(std::find(vecNotify.begin(), vecNotify.end(), _instanceMapIDMember));
		if ( _iteratorInstanceMapID != vecNotify.end() )
			continue;       

        vecNotify.push_back(_instanceMapIDMember);
		_pInstanceManager->EventPartyJoin(_partyID, _partyID.isValidExpedition(), sPartyInfo.m_dwDbNum, instanceMapID == _instanceMapIDMember);
	}

 	const DWORD dwSlotIndex = pParty->ADDMEMBER ( sPartyInfo );

	// 현재 파티원 수 기록;
	GLLandMan* pLand = m_pGaeaServer->GetLand( sPartyInfo.m_sMapID );
	if ( pLand )
	{
		pLand->AddCurPartyPlayerCount( pNetMsg->PartyID.GetPartyID() );
	}

	//	Note : 현재 필드 서버에 존재하는 케릭터의 경우 파티 ID 설정.
	//
	SetPartyID ( sPartyInfo.m_dwGaeaID, _partyID );

	SendMsgToMember(_partyID, pNetMsg);

	PGLCHAR pCharAdd = m_pGaeaServer->GetChar ( sPartyInfo.m_dwGaeaID );
	if ( pCharAdd == NULL )
		return TRUE;
	
	const DWORD dwClientSlot = pCharAdd->ClientSlot();
	if ( _partyID.isValidExpedition() == true )
	{
		GLExpedition* const pExpedition = GetExpedition(_partyID);
		if ( pExpedition == NULL )
			return FALSE;

		if ( dwSlotIndex == MASTER_PARTY )
			pExpedition->SetMasterGaeaID(sPartyInfo.m_dwGaeaID, pExpedition->GetPartySlotIndex(pNetMsg->PartyID));			
	}

	if ( SendMsgToMember_PartyDetail(_partyID, sPartyInfo.m_dwGaeaID) == FALSE )
		return FALSE;

	//	Note : 파티에 참가하는 케릭 주위(플레이어)에 파티소속 정보를 전송.
	//
	GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
	NetMsgPartyBrd.dwGaeaID = sPartyInfo.m_dwGaeaID;
	NetMsgPartyBrd.PartyID = pParty->m_PartyID;
	NetMsgPartyBrd.dwPMasterID = pParty->GetMasterGaeaID();
	pCharAdd->SendMsgViewAround(&NetMsgPartyBrd);

	// 현재 파티의 QBox 상태를 보낸다.
	GLMSG::SNET_QBOX_OPTION_MEMBER QBoxMsg(pNetMsg->PartyID, pParty->m_bQBoxEnable);
	m_pGaeaServer->SENDTOCLIENT(dwClientSlot, &QBoxMsg);

	//	Note : 다른 맴버들의 정보를 새로 추가되는 맴버에게 전송.
	//
	GLPartyField::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPartyField::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();

	PGLCHAR pCharMem = NULL;		

	for ( ; iter!=iter_end; ++iter )
	{
		pCharMem = m_pGaeaServer->GetChar ( (*iter).first );

		if ( !pCharMem )
			continue;
		//
		//mjeon.attendance
		//
		pCharMem->AttendanceCheckParty((UINT)pParty->m_cMEMBER.size());
	}

	return TRUE;
}


BOOL GLPartyManagerField::MsgPartyDel ( NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_DEL *pNetMsg = ( GLMSG::SNET_PARTY_DEL * ) nmg;

	GLPartyField* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return FALSE;

	const DWORD _delMemberGaeaID(pNetMsg->delMemberGaeaID);
	const DWORD _delMemberDbNum(pNetMsg->delMemberDbNum);
	const DWORD _instanceMapIDCurrent(pNetMsg->currentMapID.Mid());
	//	Note : 검색용으로 등록된 m_GaeaParty에서 제거;
	resetGaeaIDMap(_delMemberGaeaID);	

	if ( pParty->removeMember(m_pGaeaServer, _delMemberGaeaID) == false )
	{
		return FALSE;
	}

	// 현재 파티원 수 기록;
	GLChar* const pDelChar( m_pGaeaServer->GetCharByDbNum( _delMemberDbNum ) );
	if ( pDelChar )
	{
		GLLandMan* pLand = pDelChar->GetLandMan();
		if ( pLand )
		{
			pLand->DelCurPartyPlayerCount( pNetMsg->PartyID.GetPartyID() );

			// Trigger System 파티 탈퇴 메시지 전송;
			{
				TriggerSystem::SServerPartyOutMessage sMsg;
				sMsg.dwCharDbNum = _delMemberDbNum;
				pDelChar->SendTriggerMessage( &sMsg );
			}
		}
	}

	std::vector<DWORD> vecNotify;
	vecNotify.reserve(MAXPARTY);
	for ( GLPartyField::MEMBER_ITER iter(pParty->m_cMEMBER.begin()); iter != pParty->m_cMEMBER.end(); ++iter )
	{
		GLChar* const pMember(m_pGaeaServer->GetChar((*iter).first));
		if ( pMember == NULL )
			continue;

		GLLandManager* const pLandManager(pMember->GetParentLandManager());
		if ( pLandManager == NULL )
			continue;

		InstanceSystem::InstanceField* const _pInstanceManager(pLandManager->GetScriptInstance());
		if ( _pInstanceManager == NULL )
			continue;

		const DWORD _instanceMapIDMember(pLandManager->getInstanceMapMainID());
		std::vector<DWORD>::iterator _iteratorInstanceMapID(std::find(vecNotify.begin(), vecNotify.end(), _instanceMapIDMember));
		if ( _iteratorInstanceMapID != vecNotify.end() )
			continue;

		vecNotify.push_back(_instanceMapIDMember);
		_pInstanceManager->EventPartyOut(pNetMsg->PartyID, pNetMsg->PartyID.isValidExpedition(), _delMemberDbNum, _instanceMapIDCurrent == _instanceMapIDMember);
	}

	_refreshExpeditionMaster(pNetMsg->PartyID);

	return TRUE;
}

BOOL GLPartyManagerField::MsgPartyDissolve(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_DISSOLVE *pNetMsg = ( GLMSG::SNET_PARTY_DISSOLVE * ) nmg;

	const GLPartyID& _partyID(pNetMsg->PartyID);
	if ( _partyID.isValidExpedition() == true )
	{
		EXPEDITION_MAP_ITER iter = m_Expedition.find(_partyID.GetExpeditionID());
		if ( iter == m_Expedition.end() )
			return FALSE;

		GLExpedition* pExpedition = iter->second;		

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			const GLPartyID& PartyID = pExpedition->GetPartyID(_i - 1);
			Dissolve(PartyID);
		}

		m_ExpeditionPool.destroy(pExpedition);
		m_Expedition.erase(iter);

		return TRUE;
	}


	GLPartyField* const pParty(GetParty(_partyID));
	if ( pParty == NULL )
		return FALSE;

	std::vector<DWORD> vecNotify;
	vecNotify.reserve(MAXPARTY);
	for ( GLPartyField::MEMBER_CITER iter(pParty->m_cMEMBER.begin()); iter != pParty->m_cMEMBER.end(); ++iter )
	{
		const DWORD dwMemberGaeaID((*iter).first);
		GLChar* pMember(m_pGaeaServer->GetChar(dwMemberGaeaID));
		if ( pMember == NULL )
			continue;

		// 현재 파티원 수 기록;
		GLLandMan* pLand = pMember->GetLandMan();
		if ( pLand )
		{
			pLand->DelCurPartyPlayerCount( pNetMsg->PartyID.GetPartyID() );

			// Trigger System 파티 탈퇴 메시지 전송;
			{
				TriggerSystem::SServerPartyOutMessage sMsg;
				sMsg.dwCharDbNum = pMember->CharDbNum();
				pMember->SendTriggerMessage( &sMsg );
			}
		}

		GLLandManager* const pLandManager(pMember->GetParentLandManager());
		if ( pLandManager == NULL )
			continue;		

		const DWORD instanceMapID(pLandManager->getInstanceMapMainID());
		std::vector<DWORD>::iterator _iteratorInstanceMapID(std::find(vecNotify.begin(), vecNotify.end(), instanceMapID));
		if ( _iteratorInstanceMapID != vecNotify.end() )
			continue;

		InstanceSystem::InstanceField* const _pInstanceManager(pLandManager->GetScriptInstance());
		if ( _pInstanceManager == NULL )
			continue;

		vecNotify.push_back(instanceMapID);
		for ( GLPartyField::MEMBER_CITER iter(pParty->m_cMEMBER.begin()); iter != pParty->m_cMEMBER.end(); ++iter )
		{
			const GLPARTY_FNET& _member((*iter).second);
			const DWORD _instanceMapIDMember(_member.m_sMapID.getGaeaMapID().Id());
			_pInstanceManager->EventPartyOut(_partyID, _partyID.isValidExpedition(), _member.GetChaDbNum(), instanceMapID == _instanceMapIDMember);
		}		
	}

	return Dissolve(pNetMsg->PartyID);	
}

BOOL GLPartyManagerField::MsgPartyAuthority(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_AUTHORITY *pNetMsg = (GLMSG::SNET_PARTY_AUTHORITY*)nmg;

	const GLPartyID& _partyID(pNetMsg->PartyID);

	if ( _partyID.isValidExpedition() )
	{
		GLExpedition* const pExpedition = GetExpedition(_partyID);
		if ( pExpedition == NULL )
			return FALSE;

		const DWORD dwPartySlotIndex = pExpedition->GetPartySlotIndex(_partyID);
		pExpedition->SetMasterGaeaID(pNetMsg->dwNewMasterGaeaID, dwPartySlotIndex);
	}

	GLPartyField* const pParty(GetParty(_partyID));
	if ( pParty == NULL )
		return FALSE;

	// 이전 파티장과 신규 파티장의 GaeaID 를 얻어온다;
	const DWORD dwOldMaster = pParty->GetMasterGaeaID();	
	const DWORD dwNewMaster = pNetMsg->dwNewMasterGaeaID;

	// 파티장을 변경시킨다;
	{
		GLPARTY_FNET* const pOldMaster = pParty->GetMemberSlot(dwOldMaster);
		GLPARTY_FNET* const pNewMaster = pParty->GetMemberSlot(dwNewMaster);
		if ( (pOldMaster == NULL) || (pNewMaster == NULL) )
			return FALSE;

		StringCchCopy(pNetMsg->szCharName, CHAR_SZNAME, pOldMaster->GetName());

		const DWORD dwNewMasterSlot = pParty->GetMemberSlotIndex(dwNewMaster);
		const GLPARTY_FNET OldMaster = *pOldMaster;

		pParty->SetMemberSlot(MASTER_PARTY, dwNewMaster, *pNewMaster);
		pParty->SetMemberSlot(dwNewMasterSlot, dwOldMaster, OldMaster);
	}

	// 이전 파티장과 신규 파티장의 정보를 얻어온다;
	GLPARTY_FNET* const pOldMaster = pParty->GetMemberSlot( dwOldMaster );
	GLPARTY_FNET* const pNewMaster = pParty->GetMemberSlot( dwNewMaster );
	if ( ( pOldMaster == NULL ) || ( pNewMaster == NULL ) )
		return FALSE;

	// 새로운 파티마스터의 변경을 알림
	GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
	
	PGLCHAR pOldMasterChar = m_pGaeaServer->GetChar(dwOldMaster);	
	if ( pOldMasterChar )
	{		
		NetMsgPartyBrd.dwGaeaID = pOldMasterChar->GetGaeaID();
		NetMsgPartyBrd.PartyID = _partyID;
		NetMsgPartyBrd.dwPMasterID = pNetMsg->dwNewMasterGaeaID;
		pOldMasterChar->SendMsgViewAround(&NetMsgPartyBrd);
	}
	
	PGLCHAR pNewMasterChar = m_pGaeaServer->GetChar ( pNetMsg->dwNewMasterGaeaID );
	if ( pNewMasterChar )
	{
		NetMsgPartyBrd.dwGaeaID = pNewMasterChar->GetGaeaID();
		NetMsgPartyBrd.PartyID = _partyID;
		NetMsgPartyBrd.dwPMasterID = pNetMsg->dwNewMasterGaeaID;
		pNewMasterChar->SendMsgViewAround(&NetMsgPartyBrd);
	}

	const DWORD dbNumOldMaster(pOldMaster->GetChaDbNum());
	const DWORD instanceMapIDOldMaster(pOldMaster->m_sMapID.getGaeaMapID().Mid());
	const DWORD dbNumNewMaster(pNewMaster->GetChaDbNum());
	const DWORD instanceMapIDNewMaster(pNewMaster->m_sMapID.getGaeaMapID().Mid());

	std::vector<DWORD> vecNotify;
	vecNotify.reserve(MAXPARTY);
	for ( GLPartyField::MEMBER_CITER iter(pParty->m_cMEMBER.begin()); iter != pParty->m_cMEMBER.end(); ++iter )
	{
		const DWORD dwMemberGaeaID((*iter).first);
		GLChar* pMember(m_pGaeaServer->GetChar(dwMemberGaeaID));
		if ( pMember == NULL )
			continue;

		GLLandManager* const pLandManager(pMember->GetParentLandManager());
		if ( pLandManager == NULL )
			continue;		

		const DWORD instanceMapID(pLandManager->getInstanceMapMainID());
		std::vector<DWORD>::iterator _iteratorInstanceMapID(std::find(vecNotify.begin(), vecNotify.end(), instanceMapID));
		if ( _iteratorInstanceMapID != vecNotify.end() )
			continue;

		InstanceSystem::InstanceField* const _pInstanceManager(pLandManager->GetScriptInstance());
		if ( _pInstanceManager == NULL )
			continue;

		vecNotify.push_back(instanceMapID);
		for ( GLPartyField::MEMBER_CITER iter(pParty->m_cMEMBER.begin()); iter != pParty->m_cMEMBER.end(); ++iter )
		{
			const GLPARTY_FNET& _member((*iter).second);
			const DWORD _instanceMapIDMember(_member.m_sMapID.getGaeaMapID().Mid());
			_pInstanceManager->EventPartyChangeMaster(_partyID, _partyID.isValidExpedition(), dbNumNewMaster, instanceMapIDNewMaster == _instanceMapIDMember, dbNumOldMaster, instanceMapIDOldMaster == _instanceMapIDMember);
		}		
	}

	SendMsgToMember(_partyID, pNetMsg);

	return TRUE;    
}

BOOL GLPartyManagerField::MsgPartyMbrMoveMap(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_MBR_MOVEMAP *pNetMsg = ( GLMSG::SNET_PARTY_MBR_MOVEMAP * ) nmg;

	//	Note : 파티원 정보 수정.
	//
	GLPartyField* const pParty = (GLPartyField*)GetParty( pNetMsg->PartyID );
	if ( !pParty )
		return FALSE;

	// 이동한 파티원이 속한 파티에 알림(주변 파티원에게만);
	pParty->MBR_MOVEMAP(pNetMsg->dwGaeaID, pNetMsg->mapID); 

//	SendMsgToMembers_MemberDetail(pNetMsg->PartyID, pNetMsg->dwGaeaID, pNetMsg->dwPartyIndex);	

	return TRUE;
}

BOOL GLPartyManagerField::MsgPartyMbrRename(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_MEMBER_RENAME *pNetMsg = ( GLMSG::SNET_PARTY_MEMBER_RENAME * ) nmg;

	//	Note : 파티원 정보 수정.
	//
	GLPartyField* const pParty = GetParty(pNetMsg->PartyID);
	if ( pParty == NULL )
		return FALSE;

	GLPARTY_FNET* const pPartyMember = (GLPARTY_FNET*)pParty->GetMemberSlot ( pNetMsg->dwGaeaID );
	if ( pPartyMember )
		StringCchCopy ( pPartyMember->m_szName, CHAR_SZNAME, pNetMsg->szName );

	PGLCHAR pCHAR = m_pGaeaServer->GetChar ( pNetMsg->dwGaeaID );
	if ( pCHAR )
	{
		m_pGaeaServer->ChangeNameMap ( pCHAR, pCHAR->m_szName, pNetMsg->szName );
	}

	return TRUE;
}

BOOL GLPartyManagerField::MsgExpeditionFNew(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_EXPEDITION_NEW_FB* pNetMsg = (GLMSG::SNET_EXPEDITION_NEW_FB*)nmg;	

	const DWORD dwMasterGaeaID = pNetMsg->dwMasterGaeaID;
	const GLPartyID& PartyID = pNetMsg->PartyIDs[0];		
	GASSERT(m_pGaeaServer->IsValidGaeaID(dwMasterGaeaID));		

	if ( !m_pGaeaServer->IsValidGaeaID(dwMasterGaeaID) )
		return FALSE;	

	if ( PartyID.isValidParty() == false )
		return FALSE;

	PARTY_MAN_ITER iter = m_PartyMan.find(PartyID.GetPartyID());
	if ( iter == m_PartyMan.end() )
		return FALSE;

	GLPartyField* pParty = iter->second;	
	pParty->SetPartyID(PartyID);
	for ( DWORD _i = pParty->m_cMEMBER.size(); _i; --_i )
	{
		const DWORD dwGaeaID = pParty->m_cMEMBER[_i - 1].first;
		SetPartyID(dwGaeaID, PartyID);

		GLChar* const pChar = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pChar == NULL )
			continue;

		GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
		NetMsgPartyBrd.dwPMasterID = dwMasterGaeaID;
		NetMsgPartyBrd.PartyID = PartyID;
		NetMsgPartyBrd.dwGaeaID = dwGaeaID;
		pChar->SendMsgViewAround(&NetMsgPartyBrd);
	}

	for ( DWORD _i = 1; _i < MAX_PARTYGROUP; ++_i )
	{
		const GLPartyID& ExpeditionPartyID = pNetMsg->PartyIDs[_i];
		GLPartyField* pExpeditionParty = m_PoolParty.construct(m_pGaeaServer);		
		if ( NULL == pExpeditionParty )
		{
			return FALSE;
		}
		pExpeditionParty->SetPartyID(ExpeditionPartyID);		
		pExpeditionParty->m_sOPTION = pParty->m_sOPTION;
		m_PartyMan.insert(PARTY_MAN_VALUE(ExpeditionPartyID.GetPartyID(), pExpeditionParty));
	}	

	// 원정대 생성;
	GLExpedition* pExpedition = m_ExpeditionPool.construct();
	if ( NULL == pExpedition )
	{
		return FALSE;
	}

	const DWORD dwExpeditionID = PartyID.GetExpeditionID();
	pExpedition->SetExpeditionID(dwExpeditionID);
	pExpedition->SetMasterGaeaID(dwMasterGaeaID, MASTER_PARTY);
	pExpedition->SetPartyID(pNetMsg->PartyIDs);
	// 원정대 생성;

	m_Expedition.insert(EXPEDITION_MAP_VALUE(dwExpeditionID, pExpedition));
	SendMsgToMember(pNetMsg->PartyIDs[0], pNetMsg);

	return TRUE;
}

BOOL GLPartyManagerField::MsgPartyChangeOption(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_PARTY_CHANGE_OPTION* pNetMsg = (GLMSG::SNET_PARTY_CHANGE_OPTION*)nmg;
	const GLPartyID& PartyID = pNetMsg->PartyID;

	if ( PartyID.isValidExpedition() == true )
	{
		GLExpedition* pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return FALSE;

		if ( pExpedition->isMaster(pNetMsg->dwMasterGaeaID) == false )
			return FALSE;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			const GLPartyID& ExpeditionPartyID = pExpedition->GetPartyID(_i - 1);
			GLPartyField* pParty = GetParty(ExpeditionPartyID);
			GASSERT(pParty);			
			pParty->m_sOPTION = pNetMsg->PartyOption;
		}		
	}
	else if ( PartyID.isValidParty() == true )
	{
		GLPartyField* pParty = GetParty(PartyID);
		if ( pParty == NULL )
			return FALSE;

		pParty->m_sOPTION = pNetMsg->PartyOption;
	}
	else
		return FALSE;

	SendMsgToMember(PartyID, pNetMsg);

	return TRUE;
}

BOOL GLPartyManagerField::MsgExpeditionSwapSlot(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_EXPEDITION_SWAP_SLOT* pNetMsg = (GLMSG::SNET_EXPEDITION_SWAP_SLOT*)nmg;
	GLExpedition* const pExpedition = GetExpedition(pNetMsg->PartyID);
	if ( pExpedition == NULL )
		return FALSE;	

	const bool bResult = SwapSlot(pNetMsg->PartyID, pNetMsg->SwapSlotFrom, pNetMsg->SwapSlotTo);

	return bResult ? TRUE : FALSE;
}

BOOL GLPartyManagerField::MsgPartyRefreshRequestCF(NET_MSG_GENERIC* nmg, const DWORD dwGaeaID)
{
	GLMSG::SNET_PARTY_REFRESH_REQ* pNetMsg = (GLMSG::SNET_PARTY_REFRESH_REQ*)nmg;
	const GLPartyID& PartyID = pNetMsg->PartyID;
	const DWORD dwMyGaeaID = pNetMsg->dwMyGaeaID;	

	if ( dwGaeaID != dwMyGaeaID )
	{
		GLMSG::SNET_PARTY_ERROR_TO_CLIENT RequestFbMsg(PartyID);
		RequestFbMsg.emPartyErrorCode = EMPARTY_ERROR_NOCHAR;
		m_pGaeaServer->SENDTOCLIENT(dwGaeaID, &RequestFbMsg);
		return FALSE;
	}	

	if ( isValidPartyID(PartyID, dwMyGaeaID) == FALSE )
		return FALSE;

	GLChar* pChar = m_pGaeaServer->GetChar(dwGaeaID);
	switch ( pNetMsg->emPartyRefreshRequest )
	{
	case EMPARTY_REFRESH_REQ_PARTY_INFORMATION:
		return SendMsgToMember_PartyDetail(PartyID, dwMyGaeaID);
	}

	return TRUE;
}