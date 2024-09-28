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
    //	Note : 친구 on/off 상태 갱신.
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

            //	note : 메시지 발생.				
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

    // db 친구 목록 갱신
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

    // 추가할 상대 찾음
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spChaData = pServer->GetCharCacheData(szFriend);
    if( !spChaData )
        return false;

    // 추가할 상대가 이미 등록되어 있는지 검사.
    if (IsExist(spChaData->ChaName()))
    {
        //	Note : 대상에게는 flag 조정.
        pServer->AddGameAdoJob(
            db::DbActionPtr(
                new db::FriendFlagUpdate(pChar->m_CharDbNum, spChaData->ChaDbNum(), EMFRIEND_VALID)));

        // 현재 목록에서 상태 갱신.
        SendOnlineMessage( spChaData->ChaName().c_str(), true, pServer, pChar );
        return true;
    }	

    //! 새로운 친구
    SFRIEND NewFriend(Group);
    NewFriend.SetFriendName(spChaData->ChaName());
    NewFriend.m_ChaDbNum = spChaData->ChaDbNum();
    NewFriend.m_Flag     = EMFRIEND_VALID;
    NewFriend.m_Online   = false;
	NewFriend.m_ChaClass = static_cast<DWORD>(spChaData->GetClass());
	NewFriend.m_ChaLevel = static_cast<WORD>(spChaData->GetLv());

    //! 친구 추가.
    if( Add(NewFriend, pServer, pChar) == FALSE )
        return false;

    //! 상대방 찾기
    GLCharAG* pTarChar = pServer->GetChar(spChaData->ChaName());

	if( pTarChar )
	{
		//! 상대방이 생각하는 나
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
		//! 서로 친구라면 정보를 담는다
		else
		{
			//아직 상대방이 수락하지 않았기에 오프라인으로 표시
			SFRIEND TempNewFriend(NewFriend);
			TempNewFriend.m_Online		= false;	
			TempNewFriend.m_PosX		= int(pTarChar->GetFieldMapPosition().x);
			TempNewFriend.m_PosY		= int(pTarChar->GetFieldMapPosition().z);
			TempNewFriend.m_ChaMap	= pTarChar->GetCurrentMap();

			NewFriend = TempNewFriend;
		}
	}

    SendOnlineMessage( spChaData->ChaName().c_str(), true, pServer, pChar );

    //! 친구 정보 메시지
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

    // 온라인에 있는 친구들에게 알림
    GLMSG::SNETPC_REQ_FRIENDRENAME_CLT NetMsgFriendClt(pChar->m_dwGaeaID);
    StringCchCopy(NetMsgFriendClt.szOldName, CHAR_SZNAME, OldName);
    StringCchCopy(NetMsgFriendClt.szNewName, CHAR_SZNAME, NewName);

    for (friendlogic::FRIENDMAP_ITER iter = m_mapFriend.begin(); iter!=m_mapFriend.end(); ++iter)
    {
        std::tr1::shared_ptr<SFRIEND> cFRIEND = iter->second;

        // 나를 친구로 등록한 케릭터의 친구리스트에 이름을 변경한다. 
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
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError(std::string("Can not find GetCharByDbNum on FriendAgent::Del "));
        return false;
	}

    // 제거할 상대가 등록되어 있는지 검사
    std::tr1::shared_ptr<SFRIEND> pFriend = GetData(szName);
    if (!pFriend)
        return false;

    //	Note : 친구 삭제 로그 기록.
	// 통계 및 로그 수정
    pChar->LogCharAction(
        pChar->m_CharDbNum,				//	당사자
        gamelog::ACT_FRIEND_DEL,		//	행위
        gamelog::ID_CHAR, pFriend->m_ChaDbNum,	//	상대방
        0,							//	exp
        0,							//	bright
        0,							//	life
        0,							//	money
		( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap()
        );

    // db 에서 삭제    
    pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::FriendDelete(pChar->m_CharDbNum, pFriend->m_ChaDbNum)));

	//  [2/5/2016 gbgim];
	// Changeset# 23980에서 사라진 내용인데;
	// 이게 없으면 상태정보가 갱신안된다; 이때 친구로 삭제되어있어도;
	// 클럽접속시 온라인상태 알리는 로직이 친구 접속과 동일해서 접속한것처럼 된다;
	// 왜지웠는가?;
	// db에 지워지는 대상에게는 flag 조정;
	// COdbcManager::SetChaFriend. Use ADO 에러가 있길래 확인하니 ado 에 넣지않고 odbc 에 넣고있다. ado 에 넣도록 수정함 [3/29/2017 khkim]
	pServer->AddGameAdoJob(
		db::DbActionPtr(
		new db::FriendFlagUpdate(pFriend->m_ChaDbNum, pChar->m_CharDbNum, EMFRIEND_BETRAY)));	

    // 목록에서 삭제
    Friend::Del(szName);

    // 플레그를 유지한채로 오프라인으로 갱신 시킨다.
	// Why?;
    GLCharAG* pCharDel = pServer->GetChar(szName);
    if (pCharDel)
    {
		//  [2/5/2016 gbgim];
		// 상태갱신을안하면안된다;
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

    // 캐싱 되어있지 않다면 삭제할 수 없다.
    if( spChaData == NULL )
        return false;
    
	bool bOLD_FRIEND(false);

	//	Note : 내 목록에 변경할 상대가 등록되어 있는지 검사.
	std::tr1::shared_ptr<SFRIEND> spFriend = GetData(szFriend);
	if (spFriend)
	{
		bOLD_FRIEND = true;
		// 블럭조치
		spFriend->m_Flag = EMFRIEND_BLOCK;
	}
	else
	{
		// 대상의 현제 목록에서 상태 갱신
		// 자신의 목록 갱신
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

    // db에 flag 조정.
    pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::FriendFlagUpdate(
                pChar->m_CharDbNum, 
                spFriend->m_ChaDbNum, 
                EMFRIEND_BLOCK )));

	// 블락할 캐릭터가 온라인일 때
	GLCharAG* pCharBlock = pServer->GetChar(szFriend);
	if (pCharBlock)
	{
		std::tr1::shared_ptr<SFRIEND> pTarFriend = pCharBlock->GetFriend(pChar->m_szName);
		if (pTarFriend && spFriend)
			pCharBlock->m_pFriend->SendOnlineMessage(pChar->m_szName, false, pServer, pCharBlock);
	}
	
    // 친구 정보 메시지	    
//     msgpack::sbuffer PackBuffer;
//     msgpack::pack(PackBuffer, *spFriend.get());
//     pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_FRIENDINFO, PackBuffer);


    MSGPACK_AUTO_PACK<SFRIEND> autoPack(pChar->ChaDbNum());
    autoPack.PushData( *spFriend );
    pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_FRIENDINFO, autoPack.GetVecBuffer());

	// 현재 목록에서 블락상태 갱신
	pChar->SetStateFriend(szFriend, EMFRIEND_BLOCK);	

	return true;
}

void FriendAgent::SetState(const char* szFriend, DWORD dwFlag, GLAgentServer* pServer, GLCharAG* pChar)
{
    if (!szFriend || !pServer || !pChar)
        return;

    // 상대 찾음
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

    // 정보를 찾아 갱신.
    pFriend->m_Flag = dwFlag;
    pFriend->m_Online = bONLINE;


    // 메시지 발생
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

        // 친구 찾음
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

    //! 친구
    std::tr1::shared_ptr<SFRIEND> spFriend = pChar->GetFriend( pTarChar->GetChaName() );
    //! 상대방이 보는 나.
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


    // 상대 찾음
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
				// 온라인으로 갱신한다면
				// 자신의 친구의 데이터를 이미 가지고 있다는 의미는 
				// 이미 친구로 되어있다는 의미로 친구 상태로 변경

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
	//		"SendOnlineMessage :: 친구 OffLine");
	//}

    // 메시지 발생
    GLMSG::SNETPC_REQ_FRIENDSTATE SendData(0, false, 0);
    SendData.m_FriendFlag = pFriend->GetFlag();
    SendData.m_Online     = bOnline;
    SendData.m_FriendName = szFriend;
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    pServer->SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_FRIENDSTATE, SendBuffer);
}

} // namespace friendlogic
