#include "pch.h"
#include "../../SigmaCore/MsgPack/MinMsgPack.h"

#include "../../RanLogic/Friend/FriendGroup.h"
#include "../../RanLogic/Msg/FriendMsg.h"

#include "../AgentServer/GLAgentServer.h"
#include "../Character/CharNameCache.h"
#include "../Database/DBAction/DbActionGame.h"
#include "../Database/DBAction/DbActionGameFriend.h"

#include "FriendAgent.h"

#include "../../SigmaCore/DebugInclude.h"

namespace friendlogic
{

FriendAgent::FriendAgent()
{
}

FriendAgent::~FriendAgent()
{
}

FriendAgent& FriendAgent::operator = (const FriendAgent& rhs)
{
    if (this == &rhs)
        return *this;
    friendlogic::Friend::operator = (rhs);

    return *this;
}
/*
void FriendAgent::FrameMove(float fTime, float fElapsedTime, DWORD ClientSlot, GLAgentServer* pServer)
{
    //	Note : ģ�� on/off ���� ����.
    //
    PGLCHARAG pCHAR = NULL;
    GLMSG::SNETPC_REQ_FRIENDSTATE SendData(0, false, 0);
    friendlogic::FRIENDMAP_ITER iter = m_mapFriend.begin();
    for ( ; iter!=m_mapFriend.end(); ++iter )
    {
        std::tr1::shared_ptr<SFRIEND> cFRIEND = iter->second;        
        if (!cFRIEND->IsVALID() || cFRIEND->IsBETRAY())
            continue;

        bool bONLINE(false);
        int nCHANNEL(0);

        pCHAR = pServer->GetChar(cFRIEND->m_ChaName.c_str());
        if (pCHAR)
        {
            bONLINE = true;
            nCHANNEL = pCHAR->m_nChannel;
        }

        if ((bONLINE != cFRIEND->m_Online) || (nCHANNEL != cFRIEND->m_Channel))
        {
            cFRIEND->m_Online = bONLINE;
            cFRIEND->m_Channel = nCHANNEL;

            //	note : �޽��� �߻�.				
            SendData.m_FriendFlag = cFRIEND->m_Flag;
            SendData.m_Online = cFRIEND->m_Online;
            SendData.m_Channel = cFRIEND->m_Channel;
            SendData.m_FriendName = cFRIEND->m_ChaName;
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
            pServer->SENDTOCLIENT(ClientSlot, NET_MSG_FRIENDSTATE, SendBuffer);
        }
    }
}
*/
bool FriendAgent::Add(const SFRIEND& Info, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!friendlogic::Friend::Add(Info))
        return false;

    // db ģ�� ��� ����
    if (pServer && pChar)
    {
        pServer->AddGameAdoJob(
            db::DbActionPtr(
                new db::FriendAdd(pChar->ChaDbNum(), Info.m_ChaDbNum)));
    }
    return true;
}

bool FriendAgent::Add(const char* szFriend, WORD Group, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!szFriend || !pServer || !pChar)
        return false;

    if( (int)GLCONST_CHAR::nMAX_FRIEND_LIST <= GetFriendListCount() )
        return false;
    
    if (!strcmp(szFriend, pChar->m_szName))
        return false;

    // �߰��� ��� ã��
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spChaData = pServer->GetCharCacheData(szFriend);
    if( !spChaData )
        return false;

    // �߰��� ��밡 �̹� ��ϵǾ� �ִ��� �˻�.
    if (IsExist(spChaData->ChaName()))
    {
        //	Note : ��󿡰Դ� flag ����.
        pServer->AddGameAdoJob(
            db::DbActionPtr(
                new db::FriendFlagUpdate(pChar->m_CharDbNum, spChaData->ChaDbNum(), EMFRIEND_VALID)));

        // ���� ��Ͽ��� ���� ����.
        SendOnlineMessage( spChaData->ChaName().c_str(), true, pServer, pChar );
        return true;
    }	

    //! ���ο� ģ��
    SFRIEND NewFriend(Group);
    NewFriend.SetFriendName(spChaData->ChaName());
    NewFriend.m_ChaDbNum = spChaData->ChaDbNum();
    NewFriend.m_Flag     = EMFRIEND_VALID;
    NewFriend.m_Online   = false;
	NewFriend.m_ChaClass = static_cast<DWORD>(spChaData->GetClass());
	NewFriend.m_ChaLevel = static_cast<WORD>(spChaData->GetLv());

    //! ģ�� �߰�.
    if( Add(NewFriend, pServer, pChar) == FALSE )
        return false;

    //! ���� ã��
    GLCharAG* pTarChar = pServer->GetChar(spChaData->ChaName());

	if( pTarChar )
	{
		//! ������ �����ϴ� ��
		std::tr1::shared_ptr<SFRIEND> pMe = pTarChar->GetFriend(pChar->m_szName);
	
		SFRIEND TempNewFriend(NewFriend);
		TempNewFriend.m_Online		= true;
		TempNewFriend.m_Channel	= pTarChar->GetChannel();
		TempNewFriend.m_ChaClass	= pTarChar->GetClass();
		TempNewFriend.m_ChaLevel	= pTarChar->GetChaLevel();
		TempNewFriend.m_PartyID		= pTarChar->GetPartyID();		
		TempNewFriend.m_PosX		= int(pTarChar->GetFieldMapPosition().x);
		TempNewFriend.m_PosY		= int(pTarChar->GetFieldMapPosition().z);
		TempNewFriend.m_ChaMap	= pTarChar->GetCurrentMap();

		if( pMe != NULL )
		{
			if( pMe->IsBLOCK() != true )
			{
				NewFriend = TempNewFriend;

				pTarChar->m_pFriend->SendOnlineMessage(pChar->m_szName, true, pServer, pTarChar);

				GLMSG::NET_OTHER_CHAR_LEVEL_CHANGE_AC SendData(pTarChar->ChaDbNum() , pTarChar->GetChaLevel());
				msgpack::sbuffer SendBuffer;
				msgpack::pack(SendBuffer, SendData);			
				pServer->SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_OTHER_CHAR_LEVEL_CHANGE_AC, SendBuffer);
			}
			
		}
		//! ���� ģ����� ������ ��´�
		else
		{
			//���� ������ �������� �ʾұ⿡ ������������ ǥ��
			SFRIEND TempNewFriend(NewFriend);
			TempNewFriend.m_Online		= false;	
			TempNewFriend.m_PosX		= int(pTarChar->GetFieldMapPosition().x);
			TempNewFriend.m_PosY		= int(pTarChar->GetFieldMapPosition().z);
			TempNewFriend.m_ChaMap	= pTarChar->GetCurrentMap();

			NewFriend = TempNewFriend;
		}
	}

    SendOnlineMessage( spChaData->ChaName().c_str(), true, pServer, pChar );

    //! ģ�� ���� �޽���
//     msgpack::sbuffer PackBuffer;
//     msgpack::pack(PackBuffer, NewFriend);        
//     pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_FRIENDINFO, PackBuffer);

    MSGPACK_AUTO_PACK<SFRIEND> autoPack(pChar->ChaDbNum());
    autoPack.PushData( NewFriend );
    pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_FRIENDINFO, autoPack.GetVecBuffer());

	if ( spChaData != NULL && spChaData->ChaName().length() != 0 )
	{
		GLMSG::SNETPC_REQ_FRIENDADD_FB msg(EMFRIENDADD_OK, spChaData->ChaName().c_str());
		pServer->SENDTOCLIENT(pChar->m_ClientSlot, &msg);
	}

    return true;
}
/*
void FriendAgent::MsgPhoneNumberBrd(const char* ChaName, const char* NewPhoneNumber, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!ChaName || !NewPhoneNumber || !pServer || !pChar)
        return;

    GLMSG::SNETPC_REQ_FRIENDPHONENUMBER_CLT NetMsgFriendClt(pChar->m_dwGaeaID);
    StringCchCopy(NetMsgFriendClt.szName, CHAR_SZNAME, ChaName);
    StringCchCopy(NetMsgFriendClt.szNewPhoneNumber, SMS_RECEIVER, NewPhoneNumber);
    
    for (friendlogic::FRIENDMAP_ITER iter=m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {	
        std::tr1::shared_ptr<SFRIEND> cFRIEND = iter->second;
        GLCharAG* pFriend = pServer->GetChar(cFRIEND->m_ChaName.c_str());
        if (pFriend)
        {
            std::tr1::shared_ptr<SFRIEND> pData = pFriend->GetFriend(pChar->m_szName);
            if (pData)
            {
                //StringCchCopy(pData->szPhoneNumber, SMS_RECEIVER, NewPhoneNumber);                
                pServer->SENDTOCLIENT(pFriend->m_ClientSlot, &NetMsgFriendClt);
            }
        }
    }
}
*/
void FriendAgent::MsgRenameBrd(const char* OldName, const char* NewName, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!OldName || !NewName || !pServer || !pChar)
        return;

    // �¶��ο� �ִ� ģ���鿡�� �˸�
    GLMSG::SNETPC_REQ_FRIENDRENAME_CLT NetMsgFriendClt(pChar->m_dwGaeaID);
    StringCchCopy(NetMsgFriendClt.szOldName, CHAR_SZNAME, OldName);
    StringCchCopy(NetMsgFriendClt.szNewName, CHAR_SZNAME, NewName);

    for (friendlogic::FRIENDMAP_ITER iter = m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {
        std::tr1::shared_ptr<SFRIEND> cFRIEND = iter->second;

        // ���� ģ���� ����� �ɸ����� ģ������Ʈ�� �̸��� �����Ѵ�. 
        GLCharAG* pFriend = pServer->GetChar(cFRIEND->m_ChaName);
        if (pFriend)
        {
            std::tr1::shared_ptr<SFRIEND> pData = pFriend->GetFriend(pChar->m_szName);
            if (pData)
            {                
                pData->SetFriendName(NewName);

                pFriend->DelFriend2(pChar->m_szName);
                pFriend->AddFriend((const SFRIEND&)*pData);
                // Send to Client
                pServer->SENDTOCLIENT(pFriend->m_ClientSlot, &NetMsgFriendClt);
            }
        }
    }
}

bool FriendAgent::Del(const char* szName, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!szName || !pServer || !pChar)
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError(std::string("Can not find GetCharByDbNum on FriendAgent::Del "));
        return false;
	}

    // ������ ��밡 ��ϵǾ� �ִ��� �˻�
    std::tr1::shared_ptr<SFRIEND> pFriend = GetData(szName);
    if (!pFriend)
        return false;

    //	Note : ģ�� ���� �α� ���.
	// ��� �� �α� ����
    pChar->LogCharAction(
        pChar->m_CharDbNum,				//	�����
        gamelog::ACT_FRIEND_DEL,		//	����
        gamelog::ID_CHAR, pFriend->m_ChaDbNum,	//	����
        0,							//	exp
        0,							//	bright
        0,							//	life
        0,							//	money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
        );

    // db ���� ����    
    pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::FriendDelete(pChar->m_CharDbNum, pFriend->m_ChaDbNum)));

	//  [2/5/2016 gbgim];
	// Changeset# 23980���� ����� �����ε�;
	// �̰� ������ ���������� ���žȵȴ�; �̶� ģ���� �����Ǿ��־;
	// Ŭ�����ӽ� �¶��λ��� �˸��� ������ ģ�� ���Ӱ� �����ؼ� �����Ѱ�ó�� �ȴ�;
	// �������°�?;
	// db�� �������� ��󿡰Դ� flag ����;
	// COdbcManager::SetChaFriend. Use ADO ������ �ֱ淡 Ȯ���ϴ� ado �� �����ʰ� odbc �� �ְ��ִ�. ado �� �ֵ��� ������ [3/29/2017 khkim]
	pServer->AddGameAdoJob(
		db::DbActionPtr(
		new db::FriendFlagUpdate(pFriend->m_ChaDbNum, pChar->m_CharDbNum, EMFRIEND_BETRAY)));	

    // ��Ͽ��� ����
    Friend::Del(szName);

    // �÷��׸� ������ä�� ������������ ���� ��Ų��.
	// Why?;
    GLCharAG* pCharDel = pServer->GetChar(szName);
    if (pCharDel)
    {
		//  [2/5/2016 gbgim];
		// ���°��������ϸ�ȵȴ�;
		pCharDel->SetStateFriend(pChar->m_szName, EMFRIEND_BETRAY);

        std::tr1::shared_ptr<SFRIEND> pTarFriend = pCharDel->GetFriend(pChar->m_szName);
        if( pTarFriend )
			pCharDel->m_pFriend->SendOnlineMessage(pChar->m_szName, false, pServer, pCharDel);
    }
    return true;
}

bool FriendAgent::Block(const char* szFriend, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!szFriend || !pServer || !pChar)
        return false;

    if( (int)GLCONST_CHAR::nMAX_FRIEND_BLOCK <= GetBlockListCount() )
        return false;

    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spChaData = pServer->GetCharCacheData(szFriend);

    // ĳ�� �Ǿ����� �ʴٸ� ������ �� ����.
    if( spChaData == NULL )
        return false;
    
	bool bOLD_FRIEND(false);

	//	Note : �� ��Ͽ� ������ ��밡 ��ϵǾ� �ִ��� �˻�.
	std::tr1::shared_ptr<SFRIEND> spFriend = GetData(szFriend);
	if (spFriend)
	{
		bOLD_FRIEND = true;
		// ����ġ
		spFriend->m_Flag = EMFRIEND_BLOCK;
	}
	else
	{
		// ����� ���� ��Ͽ��� ���� ����
		// �ڽ��� ��� ����
        SFRIEND sNEW_BLOCK(0);
		sNEW_BLOCK.m_ChaDbNum = spChaData->ChaDbNum();
		sNEW_BLOCK.SetFriendName(szFriend);
		sNEW_BLOCK.m_Flag = EMFRIEND_BLOCK;

        pServer->AddGameAdoJob(
            db::DbActionPtr(
                new db::FriendAdd(pChar->m_CharDbNum, spChaData->m_ChaDbNum)));
		
        Friend::Del(szFriend);
        Friend::Add(sNEW_BLOCK);
        spFriend = GetData(szFriend);
	}

    // db�� flag ����.
    pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::FriendFlagUpdate(
                pChar->m_CharDbNum, 
                spFriend->m_ChaDbNum, 
                EMFRIEND_BLOCK )));

	// ����� ĳ���Ͱ� �¶����� ��
	GLCharAG* pCharBlock = pServer->GetChar(szFriend);
	if (pCharBlock)
	{
		std::tr1::shared_ptr<SFRIEND> pTarFriend = pCharBlock->GetFriend(pChar->m_szName);
		if (pTarFriend && spFriend)
			pCharBlock->m_pFriend->SendOnlineMessage(pChar->m_szName, false, pServer, pCharBlock);
	}
	
    // ģ�� ���� �޽���	    
//     msgpack::sbuffer PackBuffer;
//     msgpack::pack(PackBuffer, *spFriend.get());
//     pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_FRIENDINFO, PackBuffer);


    MSGPACK_AUTO_PACK<SFRIEND> autoPack(pChar->ChaDbNum());
    autoPack.PushData( *spFriend );
    pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_FRIENDINFO, autoPack.GetVecBuffer());

	// ���� ��Ͽ��� ������� ����
	pChar->SetStateFriend(szFriend, EMFRIEND_BLOCK);	

	return true;
}

void FriendAgent::SetState(const char* szFriend, DWORD dwFlag, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!szFriend || !pServer || !pChar)
        return;

    // ��� ã��
    std::tr1::shared_ptr<SFRIEND> pFriend = GetData(szFriend);
    if (!pFriend)
        return;

    bool bONLINE = false;
    GLCharAG* pTarChar = pServer->GetChar(szFriend);
    if( pTarChar )
    {
        std::tr1::shared_ptr<SFRIEND> spTarFriend = pTarChar->GetFriend( pChar->GetChaName() );

        bONLINE = IsFriend(pFriend, spTarFriend);
    }    

    // ������ ã�� ����.
    pFriend->m_Flag = dwFlag;
    pFriend->m_Online = bONLINE;


    // �޽��� �߻�
    GLMSG::SNETPC_REQ_FRIENDSTATE SendData(0, false, 0);
    SendData.m_FriendFlag = dwFlag;
    SendData.m_Online     = bONLINE;
    SendData.m_FriendName = szFriend;
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    pServer->SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_FRIENDSTATE, SendBuffer);
}

void FriendAgent::MsgReqFriendList(GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!pServer || !pChar)
        return;

    bool bONLINE = false;    
    

    MSGPACK_AUTO_PACK<SFRIEND> autoPack(pChar->ChaDbNum());

    for (friendlogic::FRIENDMAP_ITER iter=m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {        
        std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;

        // ģ�� ã��
        GLCharAG* pTarChar = pServer->GetCharByDbNum(spFriend->m_ChaDbNum);
        if (pTarChar)
        {
            spFriend->m_Channel	= pTarChar->m_nChannel;
            spFriend->m_Online		= IsFriend(pChar, pTarChar);
            spFriend->m_PartyID		= pTarChar->GetPartyID();
        }
        else
        {
            spFriend->m_Online = false;
            spFriend->m_PartyID.Reset();
        }

        autoPack.PushData(*spFriend);
    }

    pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_FRIENDINFO, autoPack.GetVecBuffer());
}

bool FriendAgent::GroupAdd(const std::string& GroupName, GLAgentServer* pServer, GLCharAG* pChar)
{
    WORD NewGroupNum = m_pGroup->GetFreeNum();
    if (NewGroupNum == INVALID_FRIEND_GROUP_NUMBER)
        return false;

    FRIEND_GROUP Info(NewGroupNum, GroupName);
    if (!m_pGroup->Add(Info))
        return false;

    GLMSG::NET_FRIEND_GROUP_NEW_AC MsgAC(Info.m_GroupNum, std::string(Info.m_GroupName));
    pServer->SENDTOCLIENT(pChar->ClientSlot(), &MsgAC);

    // DB Save
    pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ChaFriendGroupInsert(
                pChar->ChaDbNum(),
                Info.m_GroupNum,
                std::string(Info.m_GroupName))));
    return true;
}

bool FriendAgent::GroupDel(WORD GroupNum, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (GroupNum == 0)
        return false;

    if (!m_pGroup->Del(GroupNum))
        return false;

    for (FRIENDMAP_ITER iter=m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {
        std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
        if (spFriend && spFriend->GetGroup() == GroupNum)
            spFriend->SetGroup(0);
    }

    GLMSG::NET_FRIEND_GROUP_DEL_AC MsgAC(GroupNum);
    pServer->SENDTOCLIENT(pChar->ClientSlot(), &MsgAC);

    // DB Save
    pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ChaFriendGroupDelete(
                pChar->ChaDbNum(),
                GroupNum)));
    return true;
}

bool FriendAgent::GroupSet(DWORD FriendDbNum, WORD Group, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (Group != 0 && !m_pGroup->IsExist(Group))
        return false;

    FRIEND_ID_ITER iter = m_FriendId.find(FriendDbNum);
    if (iter == m_FriendId.end())
        return false;

    std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
    if (!spFriend)
        return false;
    
    spFriend->SetGroup(Group);

    GLMSG::NET_FRIEND_SET_GROUP_AC MsgAC(FriendDbNum, Group);
    pServer->SENDTOCLIENT(pChar->ClientSlot(), &MsgAC);

    // DB Save
    pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ChaFriendGroupSet(
                pChar->ChaDbNum(),
                FriendDbNum,
                Group)));

    return true;
}

bool FriendAgent::GroupRename(WORD GroupNum, const std::string& NewName, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!friendlogic::Friend::GroupRename(GroupNum, NewName))
        return false;
    
    GLMSG::NET_FRIEND_GROUP_NAME_CHANGE_AC MsgAC(GroupNum, NewName);
    pServer->SENDTOCLIENT(pChar->ClientSlot(), &MsgAC);

    // DB Save
    pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ChaFriendGroupUpdate(
                pChar->ChaDbNum(),
                GroupNum,
                NewName)));
    return true;
}

void FriendAgent::MsgFriendGroupReq(GLAgentServer* pServer, GLCharAG* pChar)
{
    std::vector<FRIEND_GROUP> vGroup;
    m_pGroup->GetData(vGroup);
    for (size_t i=0; i<vGroup.size(); ++i)
    {
        GLMSG::NET_FRIEND_GROUP_REQ_AC MsgAC(vGroup[i]);
        pServer->SENDTOCLIENT(pChar->ClientSlot(), &MsgAC);
    }
}
/*
void FriendAgent::SendMyLevelToFriend(GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!pServer || !pChar)
        return;

    DWORD ChaDbNum = pChar->ChaDbNum();
    WORD ChaLevel  = pChar->GetChaLevel();
    GLMSG::NET_FRIEND_LEVEL_CHANGE_AC SendData(ChaDbNum, ChaLevel);
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    for (FRIENDMAP_ITER iter=m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {
        std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
        GLCharAG* pFriend = pServer->GetCharByDbNum(spFriend->GetChaDbNum());
        if (pFriend && spFriend->m_Flag == EMFRIEND_VALID)
        {
            pServer->SENDTOCLIENT(pFriend->ClientSlot(), NET_MSG_FRIEND_LEVEL_CHANGE_AC, SendBuffer);
            pFriend->SetFriendLevel(ChaDbNum, ChaLevel);
        }
    }
}
*/
/*
void FriendAgent::SendMyMapToFriend(GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!pServer || !pChar)
        return;
    DWORD ChaDbNum = pChar->ChaDbNum();
    const SNATIVEID& ChaMap = pChar->GetCurrentMap();

    GLMSG::NET_MAP_CHANGE_FRIEND_CLUB_AC SendData(ChaDbNum, ChaMap);
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    for (FRIENDMAP_ITER iter=m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {
        std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
        GLCharAG* pFriend = pServer->GetCharByDbNum(spFriend->GetChaDbNum());
        if (pFriend && spFriend->m_Flag == EMFRIEND_VALID)
        {
            pServer->SENDTOCLIENT(pFriend->ClientSlot(), NET_MSG_MAP_CHANGE_FRIEND_CLUB_AC, SendBuffer);
            pFriend->SetFriendMap(ChaDbNum, ChaMap);
        }
    }
}
*/
void FriendAgent::SendToFriend(NET_MSG_GENERIC* pMsg, GLAgentServer* pServer, bool OnlyValidFriend)
{
    if (!pMsg || !pServer)
        return;
    for (FRIENDMAP_ITER iter=m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {
        std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
        if (OnlyValidFriend && spFriend->m_Flag != EMFRIEND_VALID)
            continue;
        GLCharAG* pFriend = pServer->GetCharByDbNum(spFriend->GetChaDbNum());
        if (pFriend)
            pServer->SENDTOCLIENT(pFriend->ClientSlot(), pMsg);
    }
}

void FriendAgent::SendToFriend(GLAgentServer* pServer, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool OnlyValidFriend, bool UseCrc)
{
    if (!pServer)
        return;
    NET_MSG_PACK SendData;
    if (SendData.SetData(MsgType, PackBuffer, UseCrc))
        SendToFriend(&SendData, pServer, OnlyValidFriend);
}

void FriendAgent::GetValidFriend(std::set<DWORD>& vOnlineChar) const
{
    for (FRIENDMAP_CITER iter=m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {
        std::tr1::shared_ptr<SFRIEND> spFriend = iter->second;
        if (spFriend->m_Flag == EMFRIEND_VALID)
            vOnlineChar.insert(spFriend->GetChaDbNum());
    }
}

bool FriendAgent::IsFriend( GLCharAG* pChar, GLCharAG* pTarChar )
{
    if( !pChar || !pTarChar)
        return false;

    //! ģ��
    std::tr1::shared_ptr<SFRIEND> spFriend = pChar->GetFriend( pTarChar->GetChaName() );
    //! ������ ���� ��.
    std::tr1::shared_ptr<SFRIEND> spTarFriend = pTarChar->GetFriend( pChar->GetChaName() );

    return IsFriend( spFriend, spTarFriend );
}

bool FriendAgent::IsFriend( std::tr1::shared_ptr<SFRIEND> spFriend, std::tr1::shared_ptr<SFRIEND> spTarFriend )
{
    if( spFriend && spTarFriend )
        if( spFriend->IsVALID() && spTarFriend->IsVALID() )
            return true;

    return false;
}

void FriendAgent::SendOnlineMessage( const char* szFriend, bool bOnline, GLAgentServer* pServer, GLCharAG* pChar )
{
    if (!szFriend || !pServer || !pChar)
        return;

	//sc::writeLogError(
	//	"SendOnlineMessage");


    // ��� ã��
    std::tr1::shared_ptr<SFRIEND> pFriend = GetData(szFriend);
    if (!pFriend)
        return;

	if( bOnline )
	{
		GLCharAG* pTarChar = pServer->GetChar(szFriend);
		if( pTarChar )
		{
			std::tr1::shared_ptr<SFRIEND> spTarFriend = pTarChar->GetFriend( pChar->GetChaName() );
			if( spTarFriend != NULL	)
			{
				// �¶������� �����Ѵٸ�
				// �ڽ��� ģ���� �����͸� �̹� ������ �ִٴ� �ǹ̴� 
				// �̹� ģ���� �Ǿ��ִٴ� �ǹ̷� ģ�� ���·� ����

				spTarFriend->m_Online = true;
				spTarFriend->m_Flag = EMFRIEND_FLAGS::EMFRIEND_VALID;
			}

            bOnline = IsFriend(pFriend, spTarFriend);
		}    
        else
            bOnline = false;
	}

    pFriend->m_Online = bOnline;
	//if( pFriend->m_Online == false )
	//{
	//	sc::writeLogError(
	//		"SendOnlineMessage :: ģ�� OffLine");
	//}

    // �޽��� �߻�
    GLMSG::SNETPC_REQ_FRIENDSTATE SendData(0, false, 0);
    SendData.m_FriendFlag = pFriend->GetFlag();
    SendData.m_Online     = bOnline;
    SendData.m_FriendName = szFriend;
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    pServer->SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_FRIENDSTATE, SendBuffer);
}

} // namespace friendlogic
