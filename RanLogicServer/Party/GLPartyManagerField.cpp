#include "../pch.h"
#include "../Character/GLChar.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Club/GLClubManField.h"
#include "../Club/GLClubField.h"
#include "./GLPartyField.h"
#include "./GLPartyManagerField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLPartyManagerField::GLPartyManagerField(GLGaeaServer* pGaeaServer, DWORD dwMaxClient)
    : m_pGaeaServer(pGaeaServer)
    , m_dwMaxClient(dwMaxClient)

	, m_PoolParty(_T("GLPartyField"))
	, m_ExpeditionPool(_T("GLExpedition"))
{    
}

GLPartyManagerField::~GLPartyManagerField(void)
{
	//SAFE_DELETE_ARRAY(m_pParty);
    for (PARTY_MAN_ITER iter=m_PartyMan.begin(); iter!=m_PartyMan.end(); ++iter)
    {
        GLPartyField* pParty = iter->second;
        //SAFE_DELETE(pParty);
        m_PoolParty.destroy(pParty);
    }
    m_PartyMan.clear();
	//SAFE_DELETE_ARRAY(m_pGaeaTOParty);
    m_GaeaParty.clear();
}


GLPartyField* const GLPartyManagerField::GetParty(const DWORD dwPartyID) const
{
	PARTY_MAN_CITER iter = m_PartyMan.find(dwPartyID);
	if (iter != m_PartyMan.end())
		return iter->second;
	else
		return NULL;
}
GLPartyField* const GLPartyManagerField::GetParty(const GLPartyID& PartyID) const
{
	PARTY_MAN_CITER iter = m_PartyMan.find(PartyID.GetPartyID());
	if (iter != m_PartyMan.end())
		return iter->second;
	else
		return NULL;
}
GLExpedition* const GLPartyManagerField::GetExpedition(const DWORD dwExpeditionID) const
{
	EXPEDITION_MAP_CITER iter = m_Expedition.find(dwExpeditionID);
	if ( iter != m_Expedition.end() )
		return iter->second;
	else
		return NULL;
}
GLExpedition* const GLPartyManagerField::GetExpedition(const GLPartyID& PartyID) const
{
	EXPEDITION_MAP_CITER iter = m_Expedition.find(PartyID.GetExpeditionID());
	if ( iter != m_Expedition.end() )
		return iter->second;
	else
		return NULL;
}

const GLPARTY_FNET* const GLPartyManagerField::GetMasterSlot(const GLPartyID& PartyID) const
{
	GLPartyID MasterPartyID;
	if ( PartyID.isValidExpedition() == true )
	{
		GLExpedition* const pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return NULL;

		MasterPartyID = pExpedition->GetPartyID(MASTER_PARTY);
	}
	else
		MasterPartyID = PartyID;

	const GLPartyField* const pParty = GetParty(MasterPartyID);
	if ( pParty == NULL )
		return NULL;

	return pParty->GetMasterSlot();
}

const DWORD GLPartyManagerField::GetNMember(const GLPartyID& PartyID) const
{
	GLPartyField* pParty = GetParty(PartyID);
	if ( pParty == NULL )	
		return 1; // ��Ƽ ���� �ƴϸ� �ڱ� �ڽ� 1�� ��ȯ;

	if ( PartyID.isValidExpedition() )
	{
		GLExpedition* const pExpedition = GetExpedition(PartyID);		
		DWORD nMember = 0;
		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			GLPartyField* pParty = GetParty(pExpedition->GetPartyID(_i - 1));
			if ( pParty == NULL )
				return 1;
			nMember += pParty->GetNMember();
		}

		return nMember;
	}

	// ������ ��Ƽ ������ �˻縦 �Ͽ��� ������ ���⼭ 0�� ��ȯ �Ǹ� �ȵ�;
	return pParty->GetNMember(); 
}

void GLPartyManagerField::SetPartyID(DWORD dwGaeaID, const GLPartyID& PartyID )
{
	if ( (m_pGaeaServer->IsValidGaeaID(dwGaeaID) == FALSE) || (PartyID.isValidParty() == false) )
        return;

	//m_pGaeaTOParty[dwGaeaID] = wPartyID;
    m_GaeaParty.insert(GAEA_PARTY_MAN_VALUE(dwGaeaID, PartyID.GetPartyID()));

	// ���� �ʵ� ������ �����ϴ� �ɸ����� ��� ��Ƽ ID ����.
	PGLCHAR pChar = m_pGaeaServer->GetChar(dwGaeaID);
	if (pChar)
        pChar->SetPartyID(PartyID);
}

void GLPartyManagerField::resetGaeaIDMap(const DWORD dwGaeaID)
{
	if ( m_pGaeaServer->IsValidGaeaID(dwGaeaID) == FALSE )
        return;

    GAEA_PARTY_MAN_ITER iter = m_GaeaParty.find(dwGaeaID);
    if ( iter != m_GaeaParty.end() )
        m_GaeaParty.erase(iter);	
}

BOOL GLPartyManagerField::IsPartyMaster( GLPartyID& PartyID, DWORD dwGaeaID )
{
    PARTY_MAN_CITER iter = m_PartyMan.find( PartyID.GetPartyID() );
    if (iter == m_PartyMan.end())
        return FALSE;

    const GLPartyField* pParty = iter->second;
    return pParty->isMasterByGaeaID(dwGaeaID);
}

//! GaeaID�� ��ȿ�ϴٰ� ������.
GLPartyField* GLPartyManagerField::NewParty(const DWORD dwMaster, const GLPartyID& PartyID )
{	
	if ( m_pGaeaServer->IsValidGaeaID(dwMaster) == FALSE )
        return NULL;

	const DWORD dwPartyID = PartyID.GetPartyID();
    PARTY_MAN_ITER iter = m_PartyMan.find(dwPartyID);
    if (iter != m_PartyMan.end())
    {
        m_PoolParty.destroy(iter->second);
        m_PartyMan.erase(iter);
    }

    GLPartyField* pParty = m_PoolParty.construct(m_pGaeaServer);
	if ( NULL == pParty )
	{
		return NULL;
	}
    pParty->m_PartyID = PartyID;	
    SetPartyID(dwMaster, PartyID);
    m_PartyMan.insert(PARTY_MAN_VALUE(dwPartyID, pParty));
    return pParty;
}

void GLPartyManagerField::DelParty(const GLPartyID& PartyID)
{
	if ( PartyID.isValidParty() == false )
		return;	

	const DWORD dwPartyID = PartyID.GetPartyID();

    PARTY_MAN_ITER iter = m_PartyMan.find( dwPartyID );
    if (iter == m_PartyMan.end())
        return;

    GLPartyField* pParty = iter->second;
    GLPartyField::MEMBER_CITER iterMember= pParty->m_cMEMBER.begin();
    GLPartyField::MEMBER_CITER iterMemberEnd = pParty->m_cMEMBER.end();

    for (; iterMember!=iterMemberEnd; ++iterMember)
	{
		const DWORD dwGaeaID = (*iterMember).first;
        resetGaeaIDMap(dwGaeaID);
		GLChar* pChar = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pChar != NULL )
			pChar->ResetPartyID();
	}

    m_PoolParty.destroy(pParty);
    m_PartyMan.erase(iter);
}

void GLPartyManagerField::DelPartyAll ()
{
    PARTY_MAN_ITER iter = m_PartyMan.begin();
    while( iter != m_PartyMan.end() )
	{
		GLPartyField* pParty = iter->second;
		GLPartyField::MEMBER_CITER iterMember= pParty->m_cMEMBER.begin();
		GLPartyField::MEMBER_CITER iterMemberEnd = pParty->m_cMEMBER.end();

		while( iterMember != iterMemberEnd )
		{
			resetGaeaIDMap( (*iterMember).first );
			iterMember++;
		} //while

		m_PoolParty.destroy(pParty);

		iter++;
	} //while
	m_PartyMan.clear();
}

const bool GLPartyManagerField::SendMsgToMember(const GLPartyID& PartyID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
	NET_MSG_PACK MsgPack;
	if (MsgPack.SetData(MsgType, Buffer, UseCrc))
		return SendMsgToMember(PartyID, &MsgPack);

	return false;
}
const bool GLPartyManagerField::SendMsgToMember(const GLPartyID& PartyID, NET_MSG_GENERIC* nmg)
{
	if ( PartyID.isValidExpedition() )
	{
		GLExpedition* pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return false;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{ // ���;
			// ��Ƽ�� �޼����� ������ ��ƾ�� ��ƼId���� �ʿ�� �ϹǷ�;
			// ������ ID�� INVALID�� ����� ���� ������ ������ �ʵ��� ��;
			const DWORD dwPartyID = pExpedition->GetPartyID(_i - 1).GetPartyID();
			if ( SendMsgToMember(GLPartyID(dwPartyID, GLPartyID::INVALID), nmg) == false )
				return false;
		}
		return true;
	}

	GLPartyField* const pParty = GetParty(PartyID);
	if ( pParty == NULL )
		return false;

	GLPartyField::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPartyField::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();

	for (; iter!=iter_end; ++iter)
	{
		GLChar* const pChar = m_pGaeaServer->GetChar((*iter).first);
		if ( pChar == NULL )
			continue;		

		m_pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), nmg);
	}
	return true;
}

const bool GLPartyManagerField::SendMsgToMember(const GLPartyID& PartyID, const MapID& mapID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
	NET_MSG_PACK MsgPack;
	if (MsgPack.SetData(MsgType, Buffer, UseCrc))
		return SendMsgToMember(PartyID, mapID, &MsgPack);

	return false;
}

/*
 * ���� ���� �� ��Ƽ/��������鿡�� �޼��� ����; �ʵ� -> Ŭ���̾�Ʈ;
*/
const bool GLPartyManagerField::SendMsgToMember(const GLPartyID& PartyID, const MapID& mapID, NET_MSG_GENERIC* nmg)
{
	if ( PartyID.isValidExpedition() )
	{
		GLExpedition* pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return false;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{ // ���;
			// ��Ƽ�� �޼����� ������ ��ƾ�� ��ƼId���� �ʿ�� �ϹǷ�;
			// ������ ID�� INVALID�� ����� ���� ������ ������ �ʵ��� ��;
			const DWORD dwPartyID = pExpedition->GetPartyID(_i - 1).GetPartyID();
			if ( SendMsgToMember(GLPartyID(dwPartyID, GLPartyID::INVALID), mapID, nmg) == false )
				return false;
		}
		return true;
	}

	GLPartyField* const pParty = GetParty(PartyID);
	if ( pParty == NULL )
		return false;

	GLPartyField::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPartyField::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();

	for (; iter!=iter_end; ++iter)
	{
		const GLPartyField::PARTYPAIR& partyPair = (*iter);
		GLChar* const pChar = m_pGaeaServer->GetChar((*iter).first);
		if ( pChar == NULL )	// �ٸ� �ʵ忡 ���� ���� �ִ�;
			continue;
		//  [12/18/2014 gbgim];
		// ���� �ʿ� �����ϴ� ��Ƽ���鿡�� �����ϴ� �Լ��� ���� �ִ�;
// 		if ( pChar->GetCurrentMap() != mapID )
// 			continue;

		m_pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), nmg);
	}
	return true;
}

const bool GLPartyManagerField::SendMsgToMember(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vReferencePosition, const float fRange, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc)
{
	NET_MSG_PACK MsgPack;
	if ( MsgPack.SetData(MsgType, Buffer, UseCrc) )
		return SendMsgToMember(PartyID, mapID, vReferencePosition, fRange, &MsgPack);

	return false;
}


const bool GLPartyManagerField::SendMsgToMember(const GLPartyID& PartyID, const MapID& mapID, const D3DXVECTOR3& vReferencePosition, const float fRange, NET_MSG_GENERIC* nmg)
{
	if ( PartyID.isValidExpedition() )
	{
		GLExpedition* pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return false;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{ // ���;
			// ��Ƽ�� �޼����� ������ ��ƾ�� ��ƼId���� �ʿ�� �ϹǷ�;
			// ������ ID�� INVALID�� ����� ���� ������ ������ �ʵ��� ��;
			const DWORD dwPartyID = pExpedition->GetPartyID(_i - 1).GetPartyID();
			if ( SendMsgToMember(GLPartyID(dwPartyID, GLPartyID::INVALID), mapID, nmg) == false )
				return false;
		}
		return true;
	}

	GLPartyField* const pParty = GetParty(PartyID);
	if ( pParty == NULL )
		return false;

	const float fLimitRangeSq = fRange * fRange;
	for ( GLPartyField::MEMBER_CITER iter = pParty->m_cMEMBER.begin(); iter != pParty->m_cMEMBER.end(); ++iter)
	{
		GLChar* const pChar = m_pGaeaServer->GetChar((*iter).first);
		if ( pChar == NULL )
			continue;

		if ( pChar->GetCurrentMap() != mapID )
			continue;

		const D3DXVECTOR3& vMemberPosition = pChar->GetPosition();
		const D3DXVECTOR3 vDirect(vMemberPosition - vReferencePosition);
		const float fDistanceSq = D3DXVec3LengthSq(&vDirect);
		if ( fDistanceSq > fLimitRangeSq )
			continue;

		m_pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), nmg);
	}
	return true;
}


BOOL GLPartyManagerField::MsgProcess(NET_MSG_GENERIC* nmg, const DWORD dwGaeaID)
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_PARTY_REFRESH_REQ:
		return MsgPartyRefreshRequestCF(nmg, dwGaeaID);
	}

	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_PARTY_FNEW_UPDATE:
		return MsgPartyFNewUpdate(nmg);
	case NET_MSG_GCTRL_PARTY_FNEW:
		return MsgPartyFNew(nmg);	
	case NET_MSG_GCTRL_PARTY_ADD:
		return MsgPartyAdd(nmg);
	case NET_MSG_GCTRL_PARTY_DEL:
		return MsgPartyDel(nmg);
	case NET_MSG_GCTRL_PARTY_DISSOLVE:
		return MsgPartyDissolve(nmg);
	case NET_MSG_GCTRL_PARTY_AUTHORITY:
		return MsgPartyAuthority(nmg);
	case NET_MSG_GCTRL_PARTY_MBR_MOVEMAP:
		return MsgPartyMbrMoveMap(nmg);
	case NET_MSG_GCTRL_PARTY_MBR_RENAME:
		return MsgPartyMbrRename(nmg);
	case NET_MSG_GCTRL_PARTY_CHANGE_OPTION:
		return MsgPartyChangeOption(nmg);		
	case NET_MSG_GCTRL_EXPEDITION_FNEW_FB:
		return MsgExpeditionFNew(nmg);
	case NET_MSG_GCTRL_EXPEDITION_SWAP_SLOT:
		return MsgExpeditionSwapSlot(nmg);
	};

	return E_FAIL;
}


/*
 * ��ƼID �� GaeaID�� ���� ��ȿ�� �˻���;
 * ��ȿ���� ���� ��� ��û Ŭ���̾�Ʈ�� ���� �ڵ带 ����;
 * �˻縦 ���� �ϹǷ� �������� ����;
*/
BOOL GLPartyManagerField::isValidPartyID(const GLPartyID& PartyID, const DWORD dwGaeaID)
{
	GLChar* pChar = m_pGaeaServer->GetChar(dwGaeaID);
	if ( pChar == NULL )
		return FALSE;

	GLMSG::SNET_PARTY_ERROR_TO_CLIENT RequestFbMsg(PartyID);
	const DWORD dwClientSlot = pChar->GETCLIENTID();
	
	if ( PartyID.isValidExpedition() == true )
	{		
		const GLExpedition* const pExpedition = GetExpedition(PartyID);
		if ( (pExpedition == NULL) )
		{				
			RequestFbMsg.emPartyErrorCode = EMPARTY_ERROR_NOEXPEDITION;
			m_pGaeaServer->SENDTOCLIENT(dwClientSlot, &RequestFbMsg);
			return FALSE;
		}

		if ( pExpedition->GetPartySlotIndex(PartyID) == DEFSLOT_NULL )
		{			
			RequestFbMsg.emPartyErrorCode = EMPARTY_ERROR_NOEXPEDITION;
			m_pGaeaServer->SENDTOCLIENT(dwClientSlot, &RequestFbMsg);
			return FALSE;
		}

		const GLPartyField* const pParty = GetParty(PartyID);
		if ( pParty == NULL )
		{			
			RequestFbMsg.emPartyErrorCode = EMPARTY_ERROR_NOEXPEDITION;
			m_pGaeaServer->SENDTOCLIENT(dwClientSlot, &RequestFbMsg);
			return FALSE;
		}

		if ( pParty->isMemberByGaeaID(dwGaeaID) == false )
		{			
			RequestFbMsg.emPartyErrorCode = EMPARTY_ERROR_NOMEMBER;
			m_pGaeaServer->SENDTOCLIENT(dwClientSlot, &RequestFbMsg);
			return FALSE;
		}
	}
	else
	{
		const GLPartyField* const pParty = GetParty(PartyID);
		if ( pParty == NULL )
		{			
			RequestFbMsg.emPartyErrorCode = EMPARTY_ERROR_NOPARTY;
			m_pGaeaServer->SENDTOCLIENT(dwClientSlot, &RequestFbMsg);
			return FALSE;
		}

		if ( pParty->isMemberByGaeaID(dwGaeaID) == false )
		{			
			RequestFbMsg.emPartyErrorCode = EMPARTY_ERROR_NOMEMBER;
			m_pGaeaServer->SENDTOCLIENT(dwClientSlot, &RequestFbMsg);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL GLPartyManagerField::SendMsgToMember_PartyDetail(const GLPartyID& PartyID, const DWORD dwGaeaID)
{
	const bool bExpedition = PartyID.isValidExpedition();
	const DWORD nCount = bExpedition ? MAX_PARTYGROUP : 1;
	GLPartyID PartyIDs[MAX_PARTYGROUP];
	if ( bExpedition == true )
	{
		GLExpedition* pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return FALSE;

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
			PartyIDs[_i - 1] = pExpedition->GetPartyID(_i - 1);
	}
	else
		PartyIDs[0] = PartyID;

	GLChar* const pChar = m_pGaeaServer->GetChar(dwGaeaID);
	if ( pChar == NULL )
		return FALSE;

	const DWORD dwClientID = pChar->GETCLIENTID();

	for ( DWORD _i = nCount; _i; --_i )
	{
		const DWORD dwPartyIndex = nCount - _i;		
		GLPartyField* const pExpeditionParty = GetParty(PartyIDs[dwPartyIndex]);
		if ( pExpeditionParty == NULL )
			return FALSE;				

		GLMSG::SNET_PARTY_FNEW NetPartyInfo(
			PartyIDs[dwPartyIndex],
			pExpeditionParty->GetMasterGaeaID(),
			pExpeditionParty->m_sOPTION, dwPartyIndex);

		GLPartyField::MEMBER_ITER iter = pExpeditionParty->m_cMEMBER.begin();
		GLPartyField::MEMBER_ITER iter_end = pExpeditionParty->m_cMEMBER.end();
		for ( ; iter!=iter_end; ++iter )
		{
			const GLPARTY_FNET& sPartyInfo = (*iter).second;
			NetPartyInfo.ADDMEMBER ( sPartyInfo );
		}
		m_pGaeaServer->SENDTOCLIENT(dwClientID, &NetPartyInfo );
	}
	return TRUE;
}

BOOL GLPartyManagerField::Dissolve(const GLPartyID& PartyID)
{	
	GLPartyField* const pParty = GetParty( PartyID );
	if ( !pParty )
		return FALSE;

	//	Note : ��Ƽ ��ü�ÿ� �ɸ� ����(�÷��̾�)�� ��Ƽ�Ҽ� ������ ����.
	//
	GLPartyField::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPartyField::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		PGLCHAR pCharMem = m_pGaeaServer->GetChar ( (*iter).first );
		if ( !pCharMem )
			continue;

		GLMSG::SNET_PARTY_BRD NetMsgPartyBrd;
		NetMsgPartyBrd.dwGaeaID = pCharMem->GetGaeaID();
		NetMsgPartyBrd.PartyID.Reset();		//	��Ƽ �Ҽ� ����.
		NetMsgPartyBrd.dwPMasterID = GAEAID_NULL;
		pCharMem->SendMsgViewAround(&NetMsgPartyBrd);
	}

	//	Note : ��Ƽ�� �����ϰ� ��Ƽ���� ������ ��Ƽ ID ����.
	//
	DelParty ( PartyID );
	return TRUE;
}

const bool GLPartyManagerField::_refreshExpeditionMaster(const GLPartyID& PartyID)
{
	GLExpedition* const pExpedition = GetExpedition(PartyID);
	if ( pExpedition == NULL )		
		return false;

	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		const GLPartyID& ExpeditionPartyID = pExpedition->GetPartyID(_i - 1);
		const GLPartyField* const pParty = GetParty(ExpeditionPartyID);
		if ( pParty == NULL )
			return false;

		const DWORD dwMasterGaeaID = pParty->GetMasterGaeaID();
		pExpedition->SetMasterGaeaID(dwMasterGaeaID, _i - 1);
	}

	return true;
}

const bool GLPartyManagerField::SwapSlot(const GLPartyID& PartyID, const GLSwapSlotInformation& SwapSlotFrom, const GLSwapSlotInformation& SwapSlotTo)
{
	const DWORD dwFromGaeaID = SwapSlotFrom.dwGaeaID;
	const DWORD dwToGaeaID = SwapSlotTo.dwGaeaID;
	if ( dwFromGaeaID == dwToGaeaID )
		return true;

	if ( dwFromGaeaID == GAEAID_NULL && dwToGaeaID == GAEAID_NULL )
		return false; // �� �� GAEAID_NULL�̸� �ȵ�;

	if ( (SwapSlotFrom.nPartyIndex >= MAX_PARTYGROUP) || (SwapSlotTo.nPartyIndex >= MAX_PARTYGROUP) )
		return false;

	// field server�� ��� GaeaID�� NULL�� �ƴ϶� �ϴ��� GLChar�� ���� ���� ���� �� ����;

	DWORD dwMasterGaeaID = GAEAID_NULL;
	if ( PartyID.isValidExpedition() == true )
	{
		GLExpedition* const pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
			return false;

		const GLPartyID& PartyIDMaster = pExpedition->GetPartyID(MASTER_PARTY);
		const GLPartyID& PartyIDFrom = pExpedition->GetPartyID(SwapSlotFrom.nPartyIndex);
		const GLPartyID& PartyIDTo = pExpedition->GetPartyID(SwapSlotTo.nPartyIndex);
		GLPartyField* const pPartyMaster = GetParty(PartyIDMaster);
		GLPartyField* const pPartyFrom = GetParty(PartyIDFrom);
		GLPartyField* const pPartyTo = GetParty(PartyIDTo);
		if ( (pPartyFrom == NULL) || (pPartyTo == NULL) || (pPartyMaster == NULL) )
			return false;

		GLPartyField::MEMBER_ITER iterFrom = find_if(pPartyFrom->m_cMEMBER.begin(), pPartyFrom->m_cMEMBER.end(), GLPartyField::FindMember(dwFromGaeaID));
		GLPartyField::MEMBER_ITER iterTo = find_if(pPartyTo->m_cMEMBER.begin(), pPartyTo->m_cMEMBER.end(), GLPartyField::FindMember(dwToGaeaID));

		if ( dwFromGaeaID != GAEAID_NULL && iterFrom == pPartyFrom->m_cMEMBER.end() )
			return false;
		if ( dwToGaeaID != GAEAID_NULL && iterTo == pPartyTo->m_cMEMBER.end() )
			return false;

		if ( dwToGaeaID == GAEAID_NULL )
		{ // toGaeaID�� NULL�̸� fromGaeaID�� toParty�� �̵�;
			if ( pPartyTo->m_cMEMBER.size() < MAXPARTY )
			{
				const GLPartyField::PARTYPAIR copyFrom = *iterFrom;				
				pPartyFrom->m_cMEMBER.erase(iterFrom);
				pPartyTo->m_cMEMBER.push_back(copyFrom);
			}
			else
				return false;
		}
		else if ( dwFromGaeaID == GAEAID_NULL )
		{ // fromGaeaID�� NULL�̸� toGaeaID�� fromParty�� �̵�;
			if ( pPartyFrom->m_cMEMBER.size() < MAXPARTY )
			{
				const GLPartyField::PARTYPAIR copyTo = *iterTo;
				pPartyTo->m_cMEMBER.erase(iterTo);
				pPartyFrom->m_cMEMBER.push_back(copyTo);				
			}
			else
				return false;			
		}
		else
		{ // �� �� NULL�� �ƴϸ� �ڸ� ��ü;
			const GLPartyField::PARTYPAIR copyFrom = *iterFrom;
			*iterFrom = *iterTo;
			*iterTo = copyFrom;
		}

		dwMasterGaeaID = pPartyMaster->GetMasterGaeaID();
		SetPartyID(dwFromGaeaID, PartyIDTo);
		SetPartyID(dwToGaeaID, PartyIDFrom);

		_refreshExpeditionMaster(PartyID);
	}
	else if ( PartyID.isValidParty() == true )
	{
		GLPartyField* const pParty = GetParty(PartyID);
		if ( pParty == NULL )
			return false;

		GLPartyField::MEMBER_ITER iterEnd = pParty->m_cMEMBER.end();
		GLPartyField::MEMBER_ITER iterFrom = find_if(pParty->m_cMEMBER.begin(), iterEnd, GLPartyField::FindMember(dwFromGaeaID));
		GLPartyField::MEMBER_ITER iterTo = find_if(pParty->m_cMEMBER.begin(), iterEnd, GLPartyField::FindMember(dwToGaeaID));

		if ( dwFromGaeaID != GAEAID_NULL && iterFrom == iterEnd )
			return false;
		if ( dwToGaeaID != GAEAID_NULL && iterTo == iterEnd )
			return false;

		if ( dwToGaeaID == GAEAID_NULL )
		{ // toGaeaID�� NULL�̸� fromGaeaID�� toParty�� �̵�;
			const GLPartyField::PARTYPAIR copyFrom = *iterFrom;
			pParty->m_cMEMBER.erase(iterFrom);
			pParty->m_cMEMBER.push_back(copyFrom);
		}
		else if ( dwFromGaeaID == GAEAID_NULL )
		{ // fromGaeaID�� NULL�̸� toGaeaID�� fromParty�� �̵�;
			const GLPartyField::PARTYPAIR copyTo = *iterTo;
			pParty->m_cMEMBER.erase(iterTo);
			pParty->m_cMEMBER.push_back(copyTo);
		}
		else
		{ // �� �� NULL�� �ƴϸ� �ڸ� ��ü;
			const GLPartyField::PARTYPAIR copyFrom = *iterFrom;
			*iterFrom = *iterTo;
			*iterTo = copyFrom;
		}

		dwMasterGaeaID = pParty->GetMasterGaeaID();
	}	

	GLMSG::SNET_EXPEDITION_SWAP_SLOT NetMsg(PartyID, dwMasterGaeaID, SwapSlotFrom, SwapSlotTo);
	SendMsgToMember(PartyID, &NetMsg);

	return true;
}
