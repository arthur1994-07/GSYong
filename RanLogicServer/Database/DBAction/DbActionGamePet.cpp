#include "../../pch.h"

#include "../../Server/SessionServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../Server/AgentServer.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "./DbActionGamePet.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

/**
* ���ο� ���� �����Ѵ�.
* \param nChaNum  �� ������ ĳ���� ��ȣ
* \param szPetName ���̸�
* \param nPetType ���� Ÿ��
* \param nPetMID �� MID
* \param nPetSid �� SID
* \param nPetStyle ���� ��Ÿ��
* \param nPetColor ���� �÷�
* \return sc::db::DB_ERROR �Ǵ� ������������ Pet �� ������ȣ
*/
PetCreate::PetCreate(
    DWORD dwClientID,
	int nChaNum,
	const TCHAR* szPetName,
	int nPetType,
	int nPetMID,
	int nPetSID,
	int nPetStyle,
	int nPetColor,
	WORD wPosX,
	WORD wPosY,
	int nPetCardMID,
	int nPetCardSID)
	: m_nChaNum(nChaNum)
	, m_nPetType(nPetType)
	, m_nPetMID(nPetMID)
	, m_nPetSID(nPetSID)
	, m_nPetStyle(nPetStyle)
	, m_nPetColor(nPetColor)
	, m_wPosX(wPosX)
	, m_wPosY(wPosY)
	, m_nPetCardMID(nPetCardMID)
	, m_nPetCardSID(nPetCardSID)
{
	m_ClientSlot = dwClientID;
	m_strPetName = szPetName;	
	m_strPetName.Trim(_T(" ")); // �յ� ��������
}

int PetCreate::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetCreate::Execute. Use ADO");
        return NET_ERROR;
    }

	int nResult =  m_pDbManager->CreatePet(
        m_nChaNum,
        m_strPetName.GetString(),
        m_nPetType,
        m_nPetMID,
        m_nPetSID,
        m_nPetStyle,
        m_nPetColor,
        m_nPetCardMID, // PetCard�� MID �߰�
        m_nPetCardSID  // PetCard�� SID �߰�
        );

	// ������������ ���� �޽��� �߼�
	if ( nResult != sc::db::DB_ERROR )
	{

        GLMSG::SNETPET_CREATEPET_FROMDB_FB InternalMsg(EMPET_CREATEPET_FB_OK,nResult,m_wPosX,m_wPosY);

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_ClientSlot, (char*) &InternalMsg );
	}
	else
	{
        GLMSG::SNETPET_CREATEPET_FROMDB_FB InternalMsg(EMPET_CREATEPET_FB_FAIL,nResult,m_wPosX,m_wPosY);

        CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_ClientSlot, (char*) &InternalMsg );
	}

	return nResult;
}

/**
* ���� �̸��� �����Ѵ�.
* \param nPetNum ���� ������ȣ
* \param szPetName ���̸�	
*/
PetRename::PetRename(DWORD dwClientID, int nChaNum, int nPetNum, const TCHAR* szPetName, WORD wPosX, WORD wPosY)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_wPosX(wPosX)
	, m_wPosY(wPosY)
{	
	m_ClientSlot = dwClientID;
	m_strPetName = szPetName;
	m_strPetName.Trim(_T(" ")); // �յ� ��������
}

int PetRename::Execute(NetServer* pServer)
{
	if (!pServer)
		return sc::db::DB_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetRename::Execute. Use ADO");
        return sc::db::DB_ERROR;
    }

	int nRESULT = m_pDbManager->RenamePet(m_nChaNum, m_nPetNum, m_strPetName.GetString());

	GLMSG::SNETPET_REQ_RENAME_FROMDB_FB NetMsgDB2Fld;

	if (nRESULT == sc::db::DB_OK)
	{
		NetMsgDB2Fld.emFB = EMPET_REQ_RENAME_FB_FROM_DB_OK;
		NetMsgDB2Fld.wPosX = m_wPosX;
		NetMsgDB2Fld.wPosY = m_wPosY;
		StringCchCopy(NetMsgDB2Fld.szName, PETNAMESIZE+1, m_strPetName.GetString());
	}
	else
	{
		NetMsgDB2Fld.emFB = EMPET_REQ_RENAME_FB_FROM_DB_FAIL;
		StringCchCopy(NetMsgDB2Fld.szName, PETNAMESIZE+1, m_strPetName.GetString());
	}

	CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
	pTemp->InsertMsg(m_ClientSlot, (char*) &NetMsgDB2Fld);
	return nRESULT;
}

// ���� �������� �����Ѵ�.
// \param nChaNum ���� ���ο� ������
// \param nPetNum �������� ������ �� ��ȣ
PetExchange::PetExchange(int nChaNum, int nPetNum)
	: m_nChaNum(nChaNum)
	, m_nPetNum(nPetNum)
{
}

PetExchange::~PetExchange()
{
}

int PetExchange::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetExchange::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->ExchangePet(m_nChaNum, m_nPetNum);
}

/**
* ���� �÷��� �����Ѵ�.
* \param nPetNum �÷��� �����Ϸ��� ���� ��ȣ
* \param nPetColor ���� �÷�
*/
PetColorSet::PetColorSet(int nChaNum, int nPetNum, int nPetColor)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_nPetColor(nPetColor)
{	
}
	
int PetColorSet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetColorSet::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->SetPetColor( m_nChaNum, m_nPetNum, m_nPetColor);
}

/**
* ���� ��Ÿ���� �����Ѵ�.
* \param nPetNum ��Ÿ���� �����Ϸ��� ���� ��ȣ
* \param nPetStyle ���� ��Ÿ��
*/
PetStyleSet::PetStyleSet(int nChaNum, int nPetNum, int nPetStyle)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_nPetStyle(nPetStyle)
{	
}

int PetStyleSet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetStyleSet::Execute Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->SetPetStyle(m_nChaNum, m_nPetNum, m_nPetStyle);
}

/**
* ���� �������� �����Ѵ�. �������� 0-100 ����
* \param nPetNum �������� ������ ���� ��ȣ
* \param fPetFull ������
*/
PetFullSet::PetFullSet(
    int	nChaNum,
	int nPetNum, 
	int nPetFull)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_nPetFull(nPetFull)

{
}

int PetFullSet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetFullSet::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->SetPetFull(m_nChaNum, m_nPetNum, m_nPetFull);
}

/**
* ���� ��ų ������Ʈ
*/
PetSkillSet::PetSkillSet(
	int nChaNum,
	int nPetNum,
	const PETSKILL& PetSkill,
	DWORD dwActiveSlotFlag )
	: m_nChaNum(nChaNum)
	, m_nPetNum(nPetNum)	
	, m_dwActiveSlotFlag(dwActiveSlotFlag)
    , m_PetSkill(PetSkill)
{	
}

int PetSkillSet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetSkillSet::Execute. Use ADO");
        return NET_ERROR;
    }

	// PET ��ų ������Ʈ
	m_pDbManager->SetPetSkill(
		m_nChaNum,
		m_nPetNum,
		3,		
		m_PetSkill.sNativeID.wMainID,
		m_PetSkill.sNativeID.wSubID,
		0,
		0,
		(int)m_dwActiveSlotFlag);

	return sc::db::DB_OK;
}

/**
* ���� �κ��丮 ������Ʈ
*/
PetInvenSet::PetInvenSet(
    int nChaNum,
	int nPetNum,
	GLPET* pPet)
	: m_nChaNum(nChaNum)
	, m_nPetNum(nPetNum)
{	
    //pPet->GETPUTONITEMS_BYBUF( m_ByteStream );
	pPet->GETPUTONITEMS_BYVECTOR( m_vecItem );
}

int PetInvenSet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetInvenSet::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->SavePetInven(
				m_nChaNum,
				m_nPetNum,
				m_vecItem);
}

/**
* ���� ��ų ���� ���� ������Ʈ
*/
PetSkillSlotNumSet::PetSkillSlotNumSet(
    int nChaNum,
	int nPetNum,
	WORD wSkillSlotNum)
	: m_nChaNum(nChaNum)
	, m_nPetNum(nPetNum)
	, m_wSkillSlotNum(wSkillSlotNum)
{	
}

int PetSkillSlotNumSet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetSkillSlotNumSet::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->SetPetSkillSlotNum(
				m_nChaNum,
				m_nPetNum,
				m_wSkillSlotNum);
}

/**
* ���� �����Ѵ�.
* \param nPetNum ������ ���� ��ȣ
*/
PetDelete::PetDelete(int nChaNum, int nPetNum)
	: m_nChaNum(nChaNum)
	, m_nPetNum(nPetNum)
{
}

int PetDelete::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetDelete::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->DeletePet(m_nChaNum,m_nPetNum);
}

/**
* ���� ������ �����´�.
*/
PetGet::PetGet(
	GLPetField* pPet,
	DWORD dwPetNum,
	DWORD dwClientID,
	DWORD dwCharNum,
	WORD  wPosX,
	WORD  wPosY,
	bool bLMTItemCheck,
	bool bCardInfo,
	bool bTrade)
	: m_pPet(pPet)
	, m_dwPetNum(dwPetNum)
	, m_wPosX ( wPosX )
	, m_wPosY ( wPosY )
	, m_bLMTItemCheck(bLMTItemCheck)
	, m_bCardInfo(bCardInfo)
	, m_bTrade(bTrade)
{
	m_ClientSlot = dwClientID;
	m_dwCharNum = dwCharNum;
}

int PetGet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetGet::Execute. Use ADO");
        return NET_ERROR;
    }

	int nResult = m_pDbManager->GetPet(m_pPet, m_dwPetNum, m_dwCharNum);

	// ������ �ҷ����⸦ ������������ �޽��� ó��
	if ( nResult == sc::db::DB_OK )
	{
		GLMSG::SNETPET_GETPET_FROMDB_FB InternalMsg(m_pPet,m_ClientSlot, m_dwCharNum, m_dwPetNum,m_bCardInfo,m_bTrade,m_bLMTItemCheck);

        CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( DBACTION_CLIENT_ID, (char*) &InternalMsg );
	}
	// �����ϸ� �޸� ����
	else if ( nResult == sc::db::DB_ROWCNT_ERROR ) 
	{

		GLMSG::SNETPET_GETPET_FROMDB_ERROR InternalMsg(EMPET_GETPET_FROMDB_NODATA, m_bCardInfo, m_bTrade, m_bLMTItemCheck, m_wPosX, m_wPosY);

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_ClientSlot, (char*) &InternalMsg );

		SAFE_DELETE ( m_pPet );
	}
	else
	{
		GLMSG::SNETPET_GETPET_FROMDB_ERROR InternalMsg(EMPET_GETPET_FROMDB_DBFAIL, m_bCardInfo, m_bTrade, m_bLMTItemCheck, m_wPosX, m_wPosY);

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_ClientSlot, (char*) &InternalMsg );

		SAFE_DELETE ( m_pPet );
	}

	return nResult;
}

/**
* ���� �������� �����´�
* \param dwPetNum �� ��ȣ
* \return ���� ������ or sc::db::DB_ERROR
*/
PetFullGet::PetFullGet(
    int   nChaNum,
	DWORD dwPetNum,
	DWORD dwClientID,
	WORD wCureVolume,
	BOOL bRatio,
	SNATIVEID sID,
	int emType,
    WORD HoldPosX,
    WORD HoldPosY )
	: m_nChaNum( nChaNum )
	//, m_dwClient( dwClientID )
	, m_dwPetNum( dwPetNum )
	, m_wCureVolume(wCureVolume)
	, m_bRatio(bRatio)
	, m_sNativeID(sID)
	, m_emType(emType)
    , m_HoldPosX(HoldPosX)
    , m_HoldPosY(HoldPosY)
{
	m_ClientSlot = dwClientID;
}

int PetFullGet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetFullGet::Execute. Use ADO");
        return sc::db::DB_ERROR;
    }

	int nResult = m_pDbManager->GetPetFull( m_nChaNum, m_dwPetNum );

	GLMSG::SNETPET_REQ_GETFULL_FROMDB_FB InternalMsg;

	if ( nResult != sc::db::DB_ERROR )
	{
		InternalMsg.dwPetID		 = m_dwPetNum;
		InternalMsg.nFull		 = nResult;
		InternalMsg.wCureVolume  = m_wCureVolume;
		InternalMsg.bRatio		 = m_bRatio;
		InternalMsg.sNativeID    = m_sNativeID;
		InternalMsg.emFB		 = EMPET_REQ_GETFULL_FROMDB_OK;
		InternalMsg.emType		 = m_emType;
        InternalMsg.wHoldPosX    = m_HoldPosX;
        InternalMsg.wHoldPosY    = m_HoldPosY;

	}
	else
	{
		InternalMsg.emFB		 = EMPET_REQ_GETFULL_FROMDB_FAIL;
	}

	CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
	pTemp->InsertMsg ( m_ClientSlot, (char*) &InternalMsg );

	return nResult;
}

// �Ҹ�� ���� ����Ʈ�� �����´�.
// \param nChaNum �Ҹ�� �� ����Ʈ�� ����Ϸ��� ĳ���� ��ȣ
// \param vecPetReList �Ҹ�� �� ����Ʈ�� ���� ����
PetListRestoreGet::PetListRestoreGet(int nChaNum, DWORD dwClientID)
    : m_nChaNum(nChaNum)
{
	m_ClientSlot = dwClientID;
}

PetListRestoreGet::~PetListRestoreGet()
{
}

int PetListRestoreGet::Execute(NetServer* pServer)
{
    if (!pServer)
        return NET_ERROR;

    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return NET_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer)
        return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetListRestoreGet::Execute. Use ADO");
        return NET_ERROR;
    }

    std::vector<PET_RELIST> vecPetReList;
	int nResult = m_pDbManager->GetRestorePetList(m_nChaNum, vecPetReList);

	if (nResult != sc::db::DB_ERROR)
	{
		GLMSG::SNETPET_REQ_PETREVIVEINFO_FB NetMsg;

		WORD wNum(0);
        // ���θ޽����� �߻����Ѽ� ���������
		for ( WORD i = 0; i < vecPetReList.size(); ++i )
		{
			NetMsg.arryPetReviveInfo[wNum].m_PetDbNum	   = vecPetReList[i].nPetNum;
			NetMsg.arryPetReviveInfo[wNum].sPetCardID  = vecPetReList[i].sPetCardID;
			NetMsg.arryPetReviveInfo[wNum].emType	   = vecPetReList[i].emType;
			StringCchCopy ( NetMsg.arryPetReviveInfo[wNum].szPetName, PETNAMESIZE+1, vecPetReList[i].szPetName );
			if ( ++wNum >= MAXPETREVINFO )
			{
				NetMsg.wPetReviveNum = MAXPETREVINFO;
				pGaeaServer->SENDTOCLIENT ( m_ClientSlot, &NetMsg );
				wNum = 0;
			}
		}

		if ( wNum > 0 )
		{
			NetMsg.wPetReviveNum = wNum;
			pGaeaServer->SENDTOCLIENT ( m_ClientSlot, &NetMsg );
		}
	}
	else
	{
		// ����ó��
	}

	return nResult;
}

/**
* ���� ��Ȱ��Ų��.
* \param nPetNum ��Ȱ�� �� ��ȣ
* \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
*/
PetRestore::PetRestore(
    DWORD dwPetNum, 
    DWORD dwClientID,
    int nChaNum, 
    WORD wPosX, 
    WORD wPosY,
    WORD HoldPosX,
    WORD HoldPosY )
    : m_dwPetNum(dwPetNum)
    , m_nChaNum(nChaNum)
    , m_wPosX(wPosX)
    , m_wPosY(wPosY)
    , m_HoldPosX(HoldPosX)
    , m_HoldPosY(HoldPosY)
{
	m_ClientSlot = dwClientID;
}

PetRestore::~PetRestore()
{
}

int PetRestore::Execute(NetServer* pServer)
{
    if (!pServer)
        return NET_ERROR;

    CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*> (pServer);
    if (!pFieldServer)
        return NET_ERROR;

    GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
    if (!pGaeaServer)
        return NET_ERROR;

    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetRestore::Execute. Use ADO");
        return NET_ERROR;
    }

	int nResult = m_pDbManager->RestorePet(m_dwPetNum, m_nChaNum);

	if (nResult != sc::db::DB_ERROR)
	{
		// ��Ȱ�� ���� ���θ޽����� �������ش�.
		GLMSG::SNETPET_REQ_REVIVE_FROMDB_FB InternalMsg(m_wPosX, m_wPosY, m_dwPetNum);
        InternalMsg.wHoldPosX = m_HoldPosX;
        InternalMsg.wHoldPosY = m_HoldPosY;
		pFieldServer->InsertMsg( m_ClientSlot, (char*) &InternalMsg );
	}
	else
	{
		// ����ó��
		GLMSG::SNETPET_REQ_REVIVE_FB NetMsg(EMPET_REQ_REVIVE_FB_DBFAIL,m_dwPetNum);
		pGaeaServer->SENDTOCLIENT ( m_ClientSlot, &NetMsg );
	}
	return nResult;
}

/**
* �� ��Ų ���� ī�带 �����Ų��.
* \param nPetNum ��Ȱ�� �� ��ȣ
* \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
* \return �� ��ȣ or sc::db::DB_ERROR
*/
PetSkinPackSet::PetSkinPackSet(
    int nChaNum, 
    DWORD dwPetNum, 
    int nPetSkinMID, 
    int nPetSkinSID, 
    int nPetSkinScale, 
    int nPetSkinTime)
{
	m_nChaNum		= nChaNum;
	m_dwPetNum		= dwPetNum;
	m_nPetSkinMID	= nPetSkinMID;
	m_nPetSkinSID	= nPetSkinSID;
	m_nPetSkinScale	= nPetSkinScale;
	m_nPetSkinTime	= nPetSkinTime;
}

int PetSkinPackSet::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("PetSkinPackSet::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->UpdatePetSkin( m_nChaNum, m_dwPetNum, m_nPetSkinMID, m_nPetSkinSID, m_nPetSkinScale, m_nPetSkinTime );
}

} // namespace db