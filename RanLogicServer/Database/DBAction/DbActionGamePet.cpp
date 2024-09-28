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
* 새로운 팻을 생성한다.
* \param nChaNum  팻 소유자 캐릭터 번호
* \param szPetName 팻이름
* \param nPetType 팻의 타입
* \param nPetMID 팻 MID
* \param nPetSid 팻 SID
* \param nPetStyle 팻의 스타일
* \param nPetColor 팻의 컬러
* \return sc::db::DB_ERROR 또는 성공했을때는 Pet 의 고유번호
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
	m_strPetName.Trim(_T(" ")); // 앞뒤 공백제거
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
        m_nPetCardMID, // PetCard의 MID 추가
        m_nPetCardSID  // PetCard의 SID 추가
        );

	// 성공했을때만 내부 메시지 발송
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
* 팻의 이름을 변경한다.
* \param nPetNum 팻의 고유번호
* \param szPetName 팻이름	
*/
PetRename::PetRename(DWORD dwClientID, int nChaNum, int nPetNum, const TCHAR* szPetName, WORD wPosX, WORD wPosY)
	: m_nChaNum ( nChaNum )
	, m_nPetNum(nPetNum)
	, m_wPosX(wPosX)
	, m_wPosY(wPosY)
{	
	m_ClientSlot = dwClientID;
	m_strPetName = szPetName;
	m_strPetName.Trim(_T(" ")); // 앞뒤 공백제거
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

// 팻의 소유권을 이전한다.
// \param nChaNum 팻의 새로운 소유자
// \param nPetNum 소유권을 이전할 팻 번호
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
* 팻의 컬러를 변경한다.
* \param nPetNum 컬러를 변경하려는 팻의 번호
* \param nPetColor 팻의 컬러
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
* 팻의 스타일을 변경한다.
* \param nPetNum 스타일을 변경하려는 팻의 번호
* \param nPetStyle 팻의 스타일
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
* 팻의 포만도를 설정한다. 포만도는 0-100 사이
* \param nPetNum 포만도를 설정할 팻의 번호
* \param fPetFull 포만도
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
* 팻의 스킬 업데이트
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

	// PET 스킬 업데이트
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
* 팻의 인벤토리 업데이트
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
* 팻의 스킬 슬롯 갯수 업데이트
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
* 팻을 삭제한다.
* \param nPetNum 삭제할 팻의 번호
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
* 팻의 정보를 가져온다.
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

	// 팻정보 불러오기를 성공했을때만 메시지 처리
	if ( nResult == sc::db::DB_OK )
	{
		GLMSG::SNETPET_GETPET_FROMDB_FB InternalMsg(m_pPet,m_ClientSlot, m_dwCharNum, m_dwPetNum,m_bCardInfo,m_bTrade,m_bLMTItemCheck);

        CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( DBACTION_CLIENT_ID, (char*) &InternalMsg );
	}
	// 실패하면 메모리 해제
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
* 팻의 포만감을 가져온다
* \param dwPetNum 팻 번호
* \return 팻의 포만감 or sc::db::DB_ERROR
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

// 소멸된 팻의 리스트를 가져온다.
// \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
// \param vecPetReList 소멸된 펫 리스트를 넣을 벡터
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
        // 내부메시지를 발생시켜서 전달해줘라
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
		// 실패처리
	}

	return nResult;
}

/**
* 팻을 부활시킨다.
* \param nPetNum 부활할 팻 번호
* \param nChaNum 부활을 요구한 캐릭터 번호
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
		// 부활된 팻을 내부메시지로 전달해준다.
		GLMSG::SNETPET_REQ_REVIVE_FROMDB_FB InternalMsg(m_wPosX, m_wPosY, m_dwPetNum);
        InternalMsg.wHoldPosX = m_HoldPosX;
        InternalMsg.wHoldPosY = m_HoldPosY;
		pFieldServer->InsertMsg( m_ClientSlot, (char*) &InternalMsg );
	}
	else
	{
		// 실패처리
		GLMSG::SNETPET_REQ_REVIVE_FB NetMsg(EMPET_REQ_REVIVE_FB_DBFAIL,m_dwPetNum);
		pGaeaServer->SENDTOCLIENT ( m_ClientSlot, &NetMsg );
	}
	return nResult;
}

/**
* 팻 스킨 변경 카드를 적용시킨다.
* \param nPetNum 부활할 팻 번호
* \param nChaNum 부활을 요구한 캐릭터 번호
* \return 팻 번호 or sc::db::DB_ERROR
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