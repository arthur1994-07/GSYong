#include "../../pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../RanLogic/Msg/GLContrlPetMsg.h"
#include "../../Pet/GLPetField.h"
#include "./AdoExt.h"
#include "./ADOManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{
//! 새로운 팻을 생성한다.
//! \param nChaNum  팻 소유자 캐릭터 번호
//! \param szPetName 팻이름
//! \param nPetType 팻의 타입
//! \param nPetMID 팻 MID
//! \param nPetSid 팻 SID
//! \param nPetStyle 팻의 스타일
//! \param nPetColor 팻의 컬러
//! \param m_nPetCardMID 팻 카드의 MID
//! \param m_nPetCardSID 팻 카드의 SID
//! \return sc::db::DB_ERROR 또는 성공했을때는 Pet 의 고유번호
int AdoManager::CreatePet(
    int nChaNum,
    const TCHAR* szPetName,
    int nPetType,
    int nPetMID,
    int nPetSID,
    int nPetStyle,
    int nPetColor,
    int nPetCardMID,
    int nPetCardSID)
{
    if (nChaNum <=0 || !szPetName)
    {
        sc::writeLogError("COdbcManager::CreatePet nChaNum <=0 || !szPetName");
        return sc::db::DB_ERROR;
    }

    std::string PetName(szPetName);

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_VARCHAR(AdoGame, "@szPetName", PetName.c_str(), PetName.length());
    APPEND_IPARAM_INT    (AdoGame, "@nPetChaNum", nChaNum);
    APPEND_IPARAM_INT    (AdoGame, "@nPetType", nPetType);
    APPEND_IPARAM_INT    (AdoGame, "@nPetMID", nPetMID);
    APPEND_IPARAM_INT    (AdoGame, "@nPetSID", nPetSID);
    APPEND_IPARAM_INT    (AdoGame, "@nPetCardMID", nPetCardMID);
    APPEND_IPARAM_INT    (AdoGame, "@nPetCardSID", nPetCardSID);
    APPEND_IPARAM_INT    (AdoGame, "@nPetStyle", nPetStyle);
    APPEND_IPARAM_INT    (AdoGame, "@nPetColor", nPetColor);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_InsertPet", AdoGame, "@nReturn");
}

//! 펫의 이름을 변경한다.
//! \param nChaNum 캐릭터 번호
//! \param nPetNum 팻의 고유번호
//! \param szPetName 팻이름
//! \return sc::db::DB_ERROR, sc::db::DB_OK
int AdoManager::RenamePet(int nChaNum, int nPetNum, const TCHAR* szPetName)
{
    if (!szPetName)
    {
        sc::writeLogError("AdoManager::RenamePet szPetName==NULL");
        return sc::db::DB_ERROR;
    }

    TCHAR szPetNameTemp[PETNAMESIZE+1] = {0};
    ::StringCchCopy(szPetNameTemp, PETNAMESIZE+1, szPetName);

    std::string PetName(szPetNameTemp);

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT    (AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT    (AdoGame, "@nPetNum", nPetNum);
    APPEND_IPARAM_VARCHAR(AdoGame, "@szPetName", PetName.c_str(), PetName.length());

    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_RenamePet", AdoGame, "@nReturn");
}

//! 팻의 소유권을 이전한다.
//! \param nChaNum 팻의 새로운 소유자
//! \param nPetNum 소유권을 이전할 팻 번호
int AdoManager::ExchangePet(int nChaNum, int nPetNum)
{
    if (nChaNum <= 0 || nPetNum <=0)
    {
        sc::writeLogError("AdoManager::ExchangePet nChaNum <= 0 || nPetNum <=0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nPetChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetNum", nPetNum);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_UpdatePetChaNum", AdoGame, "@nReturn");
}

//! 팻의 컬러를 변경한다.
//! \param nChaNum 캐릭터 번호
//! \param nPetNum 컬러를 변경하려는 팻의 번호
//! \param nPetColor 팻의 컬러
int AdoManager::SetPetColor(int nChaNum, int nPetNum, int nPetColor)
{
    if (nPetNum <= 0)
    {
        sc::writeLogError("AdoManager::SetPetColor nPetNum <= 0");
        return sc::db::DB_ERROR;
    }
    
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetNum", nPetNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetColor", nPetColor);
    
    APPEND_OPARAM_INT(AdoGame, "@nReturn");
    
    return ExecuteStoredProcedureIntReturn("dbo.sp_UpdatePetColor", AdoGame, "@nReturn");
}

//! 팻의 스타일을 변경한다.
//! \param nPetNum 스타일을 변경하려는 팻의 번호
//! \param nPetStyle 팻의 스타일
int AdoManager::SetPetStyle(int nChaNum, int nPetNum, int nPetStyle)
{
    if (nPetNum <= 0)
    {
        sc::writeLogError("AdoManager::SetPetStyle nPetNum <= 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetNum", nPetNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetStyle", nPetStyle);
        
    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_UpdatePetStyle", AdoGame, "@nReturn");
}

//! 팻의 포만도를 설정한다. 포만도는 0-100 사이
//! \param nPetNum 포만도를 설정할 팻의 번호
//! \param fPetFull 포만도
int AdoManager::SetPetFull(int nChaNum, int nPetNum, int nPetFull)
{
    if (nPetNum <= 0 || nPetFull < 0)
    {
        sc::writeLogError("AdoManager::SetPetFull nPetNum <= 0 || nPetFull < 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetNum", nPetNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetFull", nPetFull);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_UpdatePetFull", AdoGame, "@nReturn");
}

//! 팻을 삭제한다.
//! \param nPetNum 삭제할 팻의 번호
int AdoManager::DeletePet(int nChaNum, int nPetNum)	
{
    if (nPetNum <= 0)
    {
        sc::writeLogError("AdoManager::DeletePet nPetNum <= 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetNum", nPetNum);
    
    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_DeletePet", AdoGame, "@nReturn");
}

//! 팻의 스킬 업데이트
int AdoManager::SetPetSkill(
    int nChaNum,
    int nPetNum,
    int nPetInvenType,
    int nPetInvenMID,
    int nPetInvenSID,
    int nPetInvenCMID,
    int nPetInvenCSID,
    int nPetInvenAvailable)
{
    if (nPetNum <= 0)
    {
        sc::writeLogError("AdoManager::SetPetSkill nPetNum <= 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetNum", nPetNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetInvenType", nPetInvenType);
    APPEND_IPARAM_INT(AdoGame, "@nPetInvenMID", nPetInvenMID);
    APPEND_IPARAM_INT(AdoGame, "@nPetInvenSID", nPetInvenSID);
    APPEND_IPARAM_INT(AdoGame, "@nPetInvenCMID", nPetInvenCMID);
    APPEND_IPARAM_INT(AdoGame, "@nPetInvenCSID", nPetInvenCSID);
    APPEND_IPARAM_INT(AdoGame, "@nPetInvenAvailable", nPetInvenAvailable);
    
    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_UpdatePetInven", AdoGame, "@nReturn");
}

//! 팻의 스킬슬롯 갯수 업데이트
int AdoManager::SetPetSkillSlotNum(int nChaNum, int nPetNum, WORD wSkillSlotNum)
{
    if (wSkillSlotNum == 0)
    {
        sc::writeLogError("AdoManager::SetPetSkillSlotNum wSkillSlotNum == 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT  (AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT  (AdoGame, "@nPetNum", nPetNum);
    APPEND_IPARAM_SMALL(AdoGame, "@nPetSkillSlotNum", static_cast<USHORT> (wSkillSlotNum));

    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_UpdatePetSkillSlotNum", AdoGame, "@nReturn");
}

//! 팻의 스킬을 가져온다.
//! \param pPet 펫의 포인터
int AdoManager::GetPetSkill(int nChaNum, GLPET* pPet, DWORD dwPetNum)
{
    if (!pPet) 
    {
        sc::writeLogError("AdoManager::GetPetSkill pPet==NULL");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@PetChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@PetNum", dwPetNum);
    APPEND_IPARAM_INT(AdoGame, "@PetInvenType", static_cast<int> (pet::SKILL));
    
    if (!AdoGame.ExecuteStoredProcedure("dbo.PetInvenSelect"))
        return sc::db::DB_ERROR;

    if (AdoGame.GetEOF())
        return sc::db::DB_OK;

    do
    {        
        int nMainID = 0;
        int nSubID = 0;
        int nAvailable = 0;

        // Bind data        
        AdoGame.GetCollect("PetInvenMID", nMainID);
        AdoGame.GetCollect("PetInvenSID", nSubID);
        AdoGame.GetCollect("PetInvenAvailable", nAvailable);

        PETSKILL PetSkill;
        PetSkill.sNativeID.wMainID = (WORD) nMainID;
        PetSkill.sNativeID.wSubID  = (WORD) nSubID;			

        // pPet->m_ExpSkills.insert ( PETSKILL_PAIR ( PetSkill.sNativeID.dwID, PetSkill ) );
        // pPet->m_ExpSkills[PetSkill.sNativeID.dwID] = PetSkill;
        pPet->m_ExpSkills.insert(std::make_pair(PetSkill.sNativeID.dwID, PetSkill));

        // 활성화된 스킬
        if (nAvailable)
        {
            for (WORD i=0; i<MAX_PETSKILLSLOT; ++i)
            {
                if (nAvailable & (1<<i))
                {
                    pPet->m_sActiveSkillID[i] = PetSkill.sNativeID;
                }
            }
        }	
    } while (AdoGame.Next());
    return sc::db::DB_OK;
}

//! 팻의 포만감을 가져온다
//! \param dwPetNum 팻 번호
//! \return 팻의 포만감 or sc::db::DB_ERROR
int AdoManager::GetPetFull(int nChaNum, DWORD dwPetNum)
{
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);
    
    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetNum", dwPetNum);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");
    
    return ExecuteStoredProcedureIntReturn("dbo.sp_GetPetFull", AdoGame, "@nReturn");
}

//! 소멸된 팻의 리스트를 가져온다.
//! \param nChaNum 소멸된 펫 리스트를 출력하려는 캐릭터 번호
//! \param vecPetReList 소멸된 펫 리스트를 넣을 벡터
int AdoManager::GetRestorePetList(int nChaNum, std::vector<PET_RELIST>& vecPetReList)
{
    // 부활할 PET의 리스트를 가져온다.
    // PetNum, PetName, PetType의 정보를 DB Table에서 가져온다.
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@PetChaNum", nChaNum);

    if (!AdoGame.ExecuteStoredProcedure("dbo.PetInfoSelectDeleted"))
        return sc::db::DB_ERROR;

    if (AdoGame.GetEOF())
        return sc::db::DB_OK;

    do
    {
        std::string PetName; //SQLCHAR    szPetName[PETNAMESIZE+1] = {0};
        int nPetNum = 0;
        int nPetType = 0;
        int nPetCardMID = 0;
        int nPetCardSID = 0;

        // Bind data
        AdoGame.GetCollect("PetNum",     nPetNum);
        AdoGame.GetCollect("PetName",    PetName);
        AdoGame.GetCollect("PetType",    nPetType);
        AdoGame.GetCollect("PetCardMID", nPetCardMID);
        AdoGame.GetCollect("PetCardSID", nPetCardSID);

        PET_RELIST sPetReList;
        
        if (!PetName.empty())
            sPetReList.SetName(PetName);

        sPetReList.nPetNum = nPetNum;
        sPetReList.emType = (int) nPetType;
        sPetReList.sPetCardID.wMainID = (WORD) nPetCardMID;
        sPetReList.sPetCardID.wSubID = (WORD) nPetCardSID;

        vecPetReList.push_back(sPetReList);

    } while (AdoGame.Next());

    return sc::db::DB_OK;
}

//! 팻을 부활시킨다.
//! \param nPetNum 부활할 팻 번호
//! \param nChaNum 부활을 요구한 캐릭터 번호
int AdoManager::RestorePet(DWORD dwPetNum, int nChaNum)
{
    if (dwPetNum <= 0)
    {
        sc::writeLogError("AdoManager::RestorePet dwPetNum <= 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nPetNum", dwPetNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetChaNum", nChaNum);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");
    
    return ExecuteStoredProcedureIntReturn("dbo.sp_RestorePet", AdoGame, "@nReturn");
}

//! 팻의 스킨팩을 업데이트 한다.
//! \param nChaNum		팻소유 캐릭터 번호
//! \param dwPetNum		펫 번호
//! \param nPetSkinMID	펫 스킨 MID
//! \param nPetSkinSID	펫 스킨 SID
//! \param nPetSkinScale	펫 스킨 Scale
//! \param nPetSkinTime	펫 스킨 적용시간
int AdoManager::UpdatePetSkin(
    int nChaNum,
    DWORD dwPetNum,
    int nPetSkinMID,
    int nPetSkinSID,
    int nPetSkinScale,
    int nPetSkinTime)
{
    if (dwPetNum <= 0 || nChaNum <= 0)
    {
        sc::writeLogError("AdoManager::UpdatePetSkin dwPetNum <= 0 || nChaNum <= 0");
        return sc::db::DB_ERROR;
    }

    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);

    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetNum", dwPetNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetSkinMID", nPetSkinMID);
    APPEND_IPARAM_INT(AdoGame, "@nPetSkinSID", nPetSkinSID);
    APPEND_IPARAM_INT(AdoGame, "@nPetSkinScale", nPetSkinScale);
    APPEND_IPARAM_INT(AdoGame, "@nPetSkinTime", nPetSkinTime);
    
    APPEND_OPARAM_INT(AdoGame, "@nReturn");

    return ExecuteStoredProcedureIntReturn("dbo.sp_UpdatePetSkin", AdoGame, "@nReturn");
}

//! 팻의 정보를 가져온다.
int AdoManager::GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID)
{
    if (!pPet)
    {
        sc::writeLogError("AdoManager::GetPet pPet==NULL");
        return sc::db::DB_ERROR;
    }

    // Pet 의 기본정보를 가져온다
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_IPARAM_INT(AdoGame, "@PetNum", dwPetNum);
    APPEND_IPARAM_INT(AdoGame, "@PetChaNum", dwClientID);

    if (!AdoGame.ExecuteStoredProcedure("dbo.PetInfoSelect"))
        return sc::db::DB_ERROR;

    if (AdoGame.GetEOF())
        return sc::db::DB_OK;

    int nRowCount = 0;
    do
    {
        std::string PetName; // [PETNAMESIZE+1]
        int nOwnerNum = 0;
        int nType = 0;
        int nMainID = 0;
        int nSubID = 0;
        int nPetCardMID = 0;
        int nPetCardSID = 0;
        int nStyle = 0;
        int nColor = 0;
        int nFull = 0;
        // Pet 스킨팩 추가
        int nPetSkinMID = 0;
        int nPetSkinSID = 0;
        int nPetSkinScale = 0;
        int nPetSkinTime = 0;
		int nMigrationState = 1;

        _variant_t PetSkinStartDate;

        // 스킬 슬롯 갯수 추가
        short nPetSkillSlotNum = 0;

        AdoGame.GetCollect("PetName", PetName);
        AdoGame.GetCollect("PetChaNum", nOwnerNum);
        AdoGame.GetCollect("PetType", nType);
        AdoGame.GetCollect("PetMID", nMainID);
        AdoGame.GetCollect("PetSID", nSubID);
        AdoGame.GetCollect("PetCardMID", nPetCardMID);
        AdoGame.GetCollect("PetCardSID", nPetCardSID);
        AdoGame.GetCollect("PetStyle", nStyle);
        AdoGame.GetCollect("PetColor", nColor);
        AdoGame.GetCollect("PetFull", nFull);
        AdoGame.GetCollect("PetSkinMID", nPetSkinMID);
        AdoGame.GetCollect("PetSkinSID", nPetSkinSID);
        AdoGame.GetCollect("PetSkinScale", nPetSkinScale);
        AdoGame.GetCollect("PetSkinTime", nPetSkinTime);
        AdoGame.GetCollect("PetSkinStartDate", PetSkinStartDate);
        AdoGame.GetCollect("PetSkillSlotNum", nPetSkillSlotNum);

		AdoGame.GetCollect( "MigrationState", nMigrationState );
        //std::vector<BYTE> vBuffer;
        //AdoGame.GetChunk("PetPutOnItems", vBuffer);
        
        if (!PetName.empty())
            pPet->SetName(PetName);
        pPet->m_dwOwner								= (DWORD) nOwnerNum;
        pPet->m_emTYPE								= int(nType);
        pPet->m_sPetID.wMainID						= (WORD) nMainID;
        pPet->m_sPetID.wSubID						= (WORD) nSubID;
        pPet->m_sPetCardID.wMainID					= (WORD) nPetCardMID;
        pPet->m_sPetCardID.wSubID					= (WORD) nPetCardSID;
        pPet->m_wStyle								= (WORD) nStyle;
        pPet->m_wColor								= (WORD) nColor;
        pPet->m_nFull								= nFull;
        pPet->m_sPetSkinPackData.dwPetSkinTime		= (DWORD) nPetSkinTime;
        pPet->m_sPetSkinPackData.sMobID.wMainID		= (WORD) nPetSkinMID;
        pPet->m_sPetSkinPackData.sMobID.wSubID		= (WORD) nPetSkinSID;
        pPet->m_sPetSkinPackData.fScale				= (float)nPetSkinScale / 100.0f;
        pPet->m_wMaxSkillSlot						= (WORD) nPetSkillSlotNum;

        // 프리미엄 만료기간
        // 1970-02-01 : Default
        // 태국에서 TimeZone 세팅이 틀려서 오류가 발생
        // 이 코드를 추가로 삽입한다.
        __time64_t sDate = sc::time::GetTime(PetSkinStartDate);
        sc::time::AdjustTime(sDate);
        /*
        if (PetSkinStartDate.year <= 1970 || PetSkinStartDate.year >= 2999)
        {
            PetSkinStartDate.year = 1970;
            PetSkinStartDate.month = 2;
            PetSkinStartDate.day = 1;
            PetSkinStartDate.hour = 1;
            PetSkinStartDate.minute = 1;
            PetSkinStartDate.second = 1;
        }
        */
        //CTime cTemp(sDate);
        pPet->m_sPetSkinPackData.startTime = sDate; //cTemp.GetTime();

        if (pPet->m_sPetSkinPackData.dwPetSkinTime == 0)
        {
            pPet->m_sPetSkinPackData.Init();
        }
        else
        {
            CTime currentTime = CTime::GetCurrentTime();
            CTimeSpan timeSpan = currentTime - pPet->m_sPetSkinPackData.startTime;
            if (timeSpan.GetTotalSeconds() >= pPet->m_sPetSkinPackData.dwPetSkinTime)
            {
                pPet->m_sPetSkinPackData.Init();
            }
            else
            {
                pPet->m_sPetSkinPackData.bUsePetSkinPack = TRUE;
            }
        }

        // Put on items
        //se::ByteStream PutonItems(vBuffer);
        //if (!PutonItems.IsEmpty())
		//{
		//	if ( 0 == nMigrationState )
		//	{
		//		SETPUTONITEMS_BYBUF_FOR_MIGRATION( pPet->m_PutOnItems, PutonItems );
		//	}
		//	else
		//	{
		//		SETPUTONITEMS_BYBUF(pPet->m_PutOnItems, PutonItems);
		//	}
		//}
        //se::ByteStream PutonItems(vBuffer);
        //if (!PutonItems.IsEmpty())
        //    SETPUTONITEMS_BYBUF(pPet->m_PutOnItems, PutonItems);

        nRowCount++;
    } while (AdoGame.Next());
    
    if (nRowCount == 1)	
    {
        nRowCount = 0;
    }
    else
    {
        sc::writeLogError(
            sc::string::format(
                "DB:GetPet RowCount = 0, PetID=%1% CharID=%2%",
                dwPetNum, dwClientID));

        return sc::db::DB_ROWCNT_ERROR;
    }

	if ( GetPetPutonItems( pPet, dwPetNum ) != sc::db::DB_OK )
		return sc::db::DB_ERROR;

    // Pet 의 인벤토리를 가져온다.
    //if (GetPetInven(dwClientID, pPet, dwPetNum) == sc::db::DB_ERROR)
    //    return sc::db::DB_ERROR;

    // Pet 의 스킬을 가져온다.
    if (GetPetSkill(dwClientID, pPet, dwPetNum) == sc::db::DB_ERROR)
        return sc::db::DB_ERROR;
    return sc::db::DB_OK;
}

//! 팻의 인벤토리 정보를 가져온다
int AdoManager::GetPetInven(int nChaNum, GLPET* pPet, DWORD dwPetNum)
{
    sc::writeLogError("Do not call this function. GetPetInven");
    return sc::db::DB_ERROR;
}

//! 팻의 인벤토리 업데이트
int AdoManager::SetPetInven(int nChaNum, int nPetNum, se::ByteStream &ByteStream)
{
    if (nPetNum <= 0)
    {
        sc::writeLogError("AdoManager::SetPetInven nPetNum <= 0");
        return sc::db::DB_ERROR;
    }

    LPBYTE pBuffer = NULL;
    DWORD dwSize = 0;
    int nResult = 0;
    ByteStream.GetBuffer(pBuffer, dwSize);
    if (pBuffer)
    {        
        sc::db::AdoExt AdoGame(m_GameDBConnString);
        APPEND_IPARAM_INT(AdoGame, "@PetChaNum", nChaNum);
        APPEND_IPARAM_INT(AdoGame, "@PetNum", nPetNum);

        _variant_t varBlob;
        if (dwSize > 0)
        {
            if (!AdoGame.MakeChunk(varBlob, ByteStream, dwSize))
            {
                sc::writeLogError(
                    sc::string::format(
                        "SavePetInven Makechunk ChaNum %1% PetNum %2% Size %3%",
                        nChaNum,
                        nPetNum,
                        dwSize));
            }
        }
        else
        {
            varBlob = VT_NULL;
            dwSize = sizeof(_variant_t);

            sc::writeLogError(
                sc::string::format(
                    "SavePetInven dwSize 0 ChaNum %1% PetNum %2% Size %3%",
                    nChaNum,
                    nPetNum));
        }
        APPEND_IPARAM_IMAGE(AdoGame, "@PetPutOnItems", varBlob, dwSize);

        if (AdoGame.ExecuteStoredProcedure("dbo.PetInfoPetPutOnItemsUpdate"))
            return sc::db::DB_OK;
        else
            return sc::db::DB_ERROR;
    }
    else
    {
        return sc::db::DB_ERROR;
    }
}

int AdoManager::SetPetMigrationState( int ChaNum, int PetNum, int MigrationState )
{
	sc::db::AdoExt ado( m_GameDBConnString );
	APPEND_IPARAM_INT( ado, "@PetChaNum", ChaNum );
	APPEND_IPARAM_INT( ado, "@PetNum", PetNum );
	APPEND_IPARAM_INT( ado, "@MigrationState", MigrationState );

	if ( ExecuteStoredProcedure( "dbo.UpdatePetMigrationState", ado ) != sc::db::DB_OK )
	{
		sc::writeLogError( std::string( "AdoManager::SetPetMigrationState() - ExecuteStoredProcedure failure." ) );
		return sc::db::DB_ERROR;
	}

	return sc::db::DB_OK;	
}

int AdoManager::GetPetPutonItems( GLPetField* pPet, DWORD dwPetNum )
{
	if ( !pPet )
		return sc::db::DB_ERROR;

	sc::db::AdoExt Ado( m_GameDBConnString );
	APPEND_IPARAM_INT( Ado, "@PetNum", dwPetNum );
	if ( !Ado.Execute4Cmd( "dbo.sp_ItemGetItemPetPuton", adCmdStoredProc ) )
		return sc::db::DB_ERROR;

	if ( Ado.GetEOF() )
	{
		return sc::db::DB_OK;
	}

	do
	{
		std::string strGuid;
		SNATIVEID sItemID( false );
		_variant_t CreateDate, CostumeExpireDate, BasicPeriodEx, DisguisePeriodEx;
		SINVENITEM_SAVE sItem;

		DWORD dwBasicAttackDamageData = 0;
		WORD wBasicDefence = 0;

		SNATIVEID sLinkSkillID( false );
		WORD wLinkSkillLevel = 0;
		WORD wLinkSkillTarget = 0;
		float fLinkSkillOccurRate = 0.f;

		Ado.GetCollect( "ItemUUID", strGuid );
		char szGuid[ sc::string::UUID_LENGTH_BRACES ] = { 0 };
		StringCchCopy( szGuid, sc::string::UUID_LENGTH_BRACES, strGuid.c_str() );
		sc::string::stringToUUID( szGuid, sItem.sItemCustom.guid );

		Ado.GetCollect( "ItemMID", sItemID.wMainID );
		Ado.GetCollect( "ItemSID", sItemID.wSubID );

		sItem.sItemCustom.SetNativeID( sItemID );

		Ado.GetCollect( "ItemMakeType", sItem.sItemCustom.cGenType );
		Ado.GetCollect( "ItemCostumeMID", sItem.sItemCustom.nidDISGUISE.wMainID );
		Ado.GetCollect( "ItemCostumeSID", sItem.sItemCustom.nidDISGUISE.wSubID );

		Ado.GetCollect( "ItemPosX", sItem.wPosX );
		Ado.GetCollect( "ItemPosY", sItem.wPosY );

		Ado.GetCollect( "ItemRemain", sItem.sItemCustom.wTurnNum );

		Ado.GetCollect( "ItemCreateDate", CreateDate );
		Ado.GetCollect( "ItemCostumeExpireDate", CostumeExpireDate );

		Ado.GetCollect( "ItemBasicPeriodExTime", BasicPeriodEx );
		Ado.GetCollect( "ItemDisguisePeriodExTime", DisguisePeriodEx );
		Ado.GetCollect( "ItemBasicPeriodExCount", sItem.sItemCustom.cPeriodExtensionCount );
		Ado.GetCollect( "ItemDisuisePeriodExCount", sItem.sItemCustom.cCostumePeriodExtensionCount );

		if ( CreateDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tBORNTIME = sc::time::GetTime( CreateDate );

		if ( CostumeExpireDate.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.tDISGUISE = sc::time::GetTime( CostumeExpireDate );

		if ( BasicPeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExBasic_FromDB(sc::time::GetTime( BasicPeriodEx ));

		if ( DisguisePeriodEx.GetVARIANT().vt == VT_DATE )
			sItem.sItemCustom.SetPeriodExDisguise_FromDB(sc::time::GetTime( DisguisePeriodEx ));

		Ado.GetCollect( "ItemDurability", sItem.sItemCustom.wDurability );
		Ado.GetCollect( "ItemDurabilityCount", sItem.sItemCustom.dwDurabilityCount );
		Ado.GetCollect( "PointTradeCount", sItem.sItemCustom.wRanPointTradeCount );
		Ado.GetCollect( "ItemAttack", sItem.sItemCustom.cDAMAGE );
		Ado.GetCollect( "ItemDefense", sItem.sItemCustom.cDEFENSE );
		Ado.GetCollect( "ItemRandomType1", sItem.sItemCustom.randomOption.option[0].cOptType );
		Ado.GetCollect( "ItemRandomValue1", sItem.sItemCustom.randomOption.option[0].nOptValue );
		Ado.GetCollect( "ItemRandomType2", sItem.sItemCustom.randomOption.option[1].cOptType );
		Ado.GetCollect( "ItemRandomValue2", sItem.sItemCustom.randomOption.option[1].nOptValue );
		Ado.GetCollect( "ItemRandomType3", sItem.sItemCustom.randomOption.option[2].cOptType );
		Ado.GetCollect( "ItemRandomValue3", sItem.sItemCustom.randomOption.option[2].nOptValue );
		Ado.GetCollect( "ItemRandomType4", sItem.sItemCustom.randomOption.option[3].cOptType );
		Ado.GetCollect( "ItemRandomValue4", sItem.sItemCustom.randomOption.option[3].nOptValue );
		Ado.GetCollect( "ItemRandomType5", sItem.sItemCustom.randomOption.option[4].cOptType );
		Ado.GetCollect( "ItemRandomValue5", sItem.sItemCustom.randomOption.option[4].nOptValue );
		Ado.GetCollect( "ItemResistFire", sItem.sItemCustom.cRESIST_FIRE );
		Ado.GetCollect( "ItemResistIce", sItem.sItemCustom.cRESIST_ICE );
		Ado.GetCollect( "ItemResistElec", sItem.sItemCustom.cRESIST_ELEC );
		Ado.GetCollect( "ItemResistPoison", sItem.sItemCustom.cRESIST_POISON );
		Ado.GetCollect( "ItemResistSpirit", sItem.sItemCustom.cRESIST_SPIRIT );
		Ado.GetCollect( "ItemPetNum", sItem.sItemCustom.m_PetDbNum );
		Ado.GetCollect( "ItemVehicleNum", sItem.sItemCustom.m_VehicleDbNum );
		Ado.GetCollect( "ItemMainColor", sItem.sItemCustom.dwMainColor );
		Ado.GetCollect( "ItemSubColor", sItem.sItemCustom.dwSubColor );

		Ado.GetCollect( "BasicAttackDamage", dwBasicAttackDamageData );
		Ado.GetCollect( "BasicDefence", wBasicDefence );

		if ( dwBasicAttackDamageData == MAXDWORD && wBasicDefence == MAXWORD )
		{
			// 두 값이 모두 세팅이 안되어 있는 상태라면 세팅해준다;
			// 이는 두 값이 0인 것과는 다른 의미이다;
			// 두 값이 모두 최고인 아이템은 존재하지도 않으며 존재해서는 안된다;
			sItem.sItemCustom.GenerateBasicStat();
		}
		else
		{
			sItem.sItemCustom.SetBasicAttackDamage( dwBasicAttackDamageData );
			sItem.sItemCustom.SetBasicDefence( wBasicDefence );
		}

		Ado.GetCollect( "ItemLinkSkillMID", sLinkSkillID.wMainID );
		Ado.GetCollect( "ItemLinkSkillSID", sLinkSkillID.wSubID );
		Ado.GetCollect( "ItemLinkSkillLevel", wLinkSkillLevel );
		Ado.GetCollect( "ItemLinkSkillTarget", wLinkSkillTarget );
		Ado.GetCollect( "ItemLinkSkillOccurRate", fLinkSkillOccurRate );

		if ( sLinkSkillID.dwID == MAXDWORD &&
			wLinkSkillLevel == MAXWORD &&
			wLinkSkillTarget == MAXWORD &&
			fLinkSkillOccurRate == -1.f )
		{
			sItem.sItemCustom.GenerateLinkSkill();
		}
		else
		{
			sItem.sItemCustom.SetLinkSkillID( sLinkSkillID );
			sItem.sItemCustom.SetLinkSkillLevel( wLinkSkillLevel );
			sItem.sItemCustom.SetLinkSkillTarget( static_cast< EMITEM_SKILL_LINK >( wLinkSkillTarget ) );
			sItem.sItemCustom.SetLinkSkillOccurRate( fLinkSkillOccurRate );
		}

		WORD wType(EMADD_NOTINIT);
		Ado.GetCollect( "ItemAddonType1", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[0].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType2", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[1].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType3", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[2].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType4", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[3].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType5", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[4].emTYPE = (EMITEM_ADDON)wType;
		Ado.GetCollect( "ItemAddonType6", wType);sItem.sItemCustom.sAddonInfo.m_sADDON[5].emTYPE = (EMITEM_ADDON)wType;
		if ( sItem.sItemCustom.sAddonInfo.IsInit() == false )
		{
			sItem.sItemCustom.GenerateAddOption();
		}
		else
		{
			Ado.GetCollect( "ItemAddonValue1", sItem.sItemCustom.sAddonInfo.m_sADDON[0].nVALUE );
			Ado.GetCollect( "ItemAddonValue2", sItem.sItemCustom.sAddonInfo.m_sADDON[1].nVALUE );
			Ado.GetCollect( "ItemAddonValue3", sItem.sItemCustom.sAddonInfo.m_sADDON[2].nVALUE );
			Ado.GetCollect( "ItemAddonValue4", sItem.sItemCustom.sAddonInfo.m_sADDON[3].nVALUE );
			Ado.GetCollect( "ItemAddonValue5", sItem.sItemCustom.sAddonInfo.m_sADDON[4].nVALUE );
			Ado.GetCollect( "ItemAddonValue6", sItem.sItemCustom.sAddonInfo.m_sADDON[5].nVALUE );
		}

		sItem.sItemCustom.SetDbState( db::DB_UPDATE );

		pPet->SETPUTONITEM( static_cast< ACCESSORYTYPE >( sItem.wPosX ), sItem.sItemCustom );

	} while ( Ado.Next() );

	return sc::db::DB_OK;
}

int AdoManager::GetAllPetPutonItems( DWORD ChaDbNum, std::vector< GLPET >& vecPets )
{
    sc::db::AdoExt AdoGame( m_GameDBConnString );
    APPEND_IPARAM_INT( AdoGame, "@PetChaNum", ChaDbNum );

    if ( !AdoGame.ExecuteStoredProcedure( "dbo.sp_GetAllPetPutOnItems" ) )
        return sc::db::DB_ERROR;

    if ( AdoGame.GetEOF() )
        return sc::db::DB_OK;

    do
    {
		GLPET sPet;
        AdoGame.GetCollect( "PetNum", sPet.m_DbNum );

		int nMigrationState = 1;
		AdoGame.GetCollect( "MigrationState", nMigrationState );
		
		std::vector< BYTE > vBuffer;
        AdoGame.GetChunk( "PetPutOnItems", vBuffer );

        // Put on items
        se::ByteStream PutonItems( vBuffer );
        if ( !PutonItems.IsEmpty() )
		{
			if ( 0 == nMigrationState )
			{
				SETPUTONITEMS_BYBUF_FOR_MIGRATION( sPet.m_PutOnItems, PutonItems );
			}
			else
			{
				SETPUTONITEMS_BYBUF( sPet.m_PutOnItems, PutonItems );
			}
		}

		vecPets.push_back( sPet );

    } while ( AdoGame.Next() );
    
    return sc::db::DB_OK;
}

} // namespace db
