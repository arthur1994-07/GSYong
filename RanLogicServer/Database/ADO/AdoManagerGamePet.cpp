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
//! ���ο� ���� �����Ѵ�.
//! \param nChaNum  �� ������ ĳ���� ��ȣ
//! \param szPetName ���̸�
//! \param nPetType ���� Ÿ��
//! \param nPetMID �� MID
//! \param nPetSid �� SID
//! \param nPetStyle ���� ��Ÿ��
//! \param nPetColor ���� �÷�
//! \param m_nPetCardMID �� ī���� MID
//! \param m_nPetCardSID �� ī���� SID
//! \return sc::db::DB_ERROR �Ǵ� ������������ Pet �� ������ȣ
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

//! ���� �̸��� �����Ѵ�.
//! \param nChaNum ĳ���� ��ȣ
//! \param nPetNum ���� ������ȣ
//! \param szPetName ���̸�
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

//! ���� �������� �����Ѵ�.
//! \param nChaNum ���� ���ο� ������
//! \param nPetNum �������� ������ �� ��ȣ
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

//! ���� �÷��� �����Ѵ�.
//! \param nChaNum ĳ���� ��ȣ
//! \param nPetNum �÷��� �����Ϸ��� ���� ��ȣ
//! \param nPetColor ���� �÷�
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

//! ���� ��Ÿ���� �����Ѵ�.
//! \param nPetNum ��Ÿ���� �����Ϸ��� ���� ��ȣ
//! \param nPetStyle ���� ��Ÿ��
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

//! ���� �������� �����Ѵ�. �������� 0-100 ����
//! \param nPetNum �������� ������ ���� ��ȣ
//! \param fPetFull ������
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

//! ���� �����Ѵ�.
//! \param nPetNum ������ ���� ��ȣ
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

//! ���� ��ų ������Ʈ
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

//! ���� ��ų���� ���� ������Ʈ
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

//! ���� ��ų�� �����´�.
//! \param pPet ���� ������
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

        // Ȱ��ȭ�� ��ų
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

//! ���� �������� �����´�
//! \param dwPetNum �� ��ȣ
//! \return ���� ������ or sc::db::DB_ERROR
int AdoManager::GetPetFull(int nChaNum, DWORD dwPetNum)
{
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    APPEND_RPARAM_INT(AdoGame);
    
    APPEND_IPARAM_INT(AdoGame, "@nChaNum", nChaNum);
    APPEND_IPARAM_INT(AdoGame, "@nPetNum", dwPetNum);

    APPEND_OPARAM_INT(AdoGame, "@nReturn");
    
    return ExecuteStoredProcedureIntReturn("dbo.sp_GetPetFull", AdoGame, "@nReturn");
}

//! �Ҹ�� ���� ����Ʈ�� �����´�.
//! \param nChaNum �Ҹ�� �� ����Ʈ�� ����Ϸ��� ĳ���� ��ȣ
//! \param vecPetReList �Ҹ�� �� ����Ʈ�� ���� ����
int AdoManager::GetRestorePetList(int nChaNum, std::vector<PET_RELIST>& vecPetReList)
{
    // ��Ȱ�� PET�� ����Ʈ�� �����´�.
    // PetNum, PetName, PetType�� ������ DB Table���� �����´�.
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

//! ���� ��Ȱ��Ų��.
//! \param nPetNum ��Ȱ�� �� ��ȣ
//! \param nChaNum ��Ȱ�� �䱸�� ĳ���� ��ȣ
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

//! ���� ��Ų���� ������Ʈ �Ѵ�.
//! \param nChaNum		�ּ��� ĳ���� ��ȣ
//! \param dwPetNum		�� ��ȣ
//! \param nPetSkinMID	�� ��Ų MID
//! \param nPetSkinSID	�� ��Ų SID
//! \param nPetSkinScale	�� ��Ų Scale
//! \param nPetSkinTime	�� ��Ų ����ð�
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

//! ���� ������ �����´�.
int AdoManager::GetPet(GLPetField* pPet, DWORD dwPetNum, DWORD dwClientID)
{
    if (!pPet)
    {
        sc::writeLogError("AdoManager::GetPet pPet==NULL");
        return sc::db::DB_ERROR;
    }

    // Pet �� �⺻������ �����´�
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
        // Pet ��Ų�� �߰�
        int nPetSkinMID = 0;
        int nPetSkinSID = 0;
        int nPetSkinScale = 0;
        int nPetSkinTime = 0;
		int nMigrationState = 1;

        _variant_t PetSkinStartDate;

        // ��ų ���� ���� �߰�
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

        // �����̾� ����Ⱓ
        // 1970-02-01 : Default
        // �±����� TimeZone ������ Ʋ���� ������ �߻�
        // �� �ڵ带 �߰��� �����Ѵ�.
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

    // Pet �� �κ��丮�� �����´�.
    //if (GetPetInven(dwClientID, pPet, dwPetNum) == sc::db::DB_ERROR)
    //    return sc::db::DB_ERROR;

    // Pet �� ��ų�� �����´�.
    if (GetPetSkill(dwClientID, pPet, dwPetNum) == sc::db::DB_ERROR)
        return sc::db::DB_ERROR;
    return sc::db::DB_OK;
}

//! ���� �κ��丮 ������ �����´�
int AdoManager::GetPetInven(int nChaNum, GLPET* pPet, DWORD dwPetNum)
{
    sc::writeLogError("Do not call this function. GetPetInven");
    return sc::db::DB_ERROR;
}

//! ���� �κ��丮 ������Ʈ
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
			// �� ���� ��� ������ �ȵǾ� �ִ� ���¶�� �������ش�;
			// �̴� �� ���� 0�� �Ͱ��� �ٸ� �ǹ��̴�;
			// �� ���� ��� �ְ��� �������� ���������� ������ �����ؼ��� �ȵȴ�;
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
