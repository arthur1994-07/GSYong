#include "pch.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Json/MinJson.h"
#include "../../SigmaCore/DebugInclude.h"

#include "../../RanLogic/Msg/FriendMsg.h"
#include "../../RanLogic/Msg/GLContrlFriendMsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Msg/GLContrlEventMsg.h"
#include "../../RanLogic/Notify/Notify.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Timer/Timer.h"

#include "../AgentServer/GLAgentServer.h"
#include "../Club/GLClubAgent.h"
#include "../Friend/FriendAgent.h"
#include "./GLCharAG.h"

#include "../Notify/NotifyManAgent.h"

#include "../Database/DBAction/DbActionGame.h"
#include "../Database/DBAction/DbActionGameFriend.h"
#include "../Database/DBAction/DbActionGameNotify.h"

#include "../Character/CharNameCache.h"


bool GLCharAG::AddFriendGroup(const std::string& GroupName)
{    
    return m_pFriend->GroupAdd(GroupName, m_pServer, this);
}

bool GLCharAG::DelFriendGroup(WORD GroupNum)
{
    return m_pFriend->GroupDel(GroupNum, m_pServer, this);
}

bool GLCharAG::SetFriendGroup(DWORD FriendDbNum, WORD GroupNum)
{
    return m_pFriend->GroupSet(FriendDbNum, GroupNum, m_pServer, this);
}

bool GLCharAG::FriendGroupRename(WORD GroupNum, const std::string& NewName)
{
    return m_pFriend->GroupRename(GroupNum, NewName, m_pServer, this);
}

void GLCharAG::MsgFriendGroupReq()
{
    m_pFriend->MsgFriendGroupReq(m_pServer, this);
}
/*
void GLCharAG::SendMyLevelToFriend()
{
    m_pFriend->SendMyLevelToFriend(m_pServer, this);
}
*/
void GLCharAG::Online()
{
    // 친구들에게 online 상태 전송, 클럽원들에게 online 상태 전송
    GLMSG::NET_CHAR_STATE_ONLINE_AC SendData;
    SendData.m_ChaDbNum   = m_CharDbNum;
    SendData.m_CurrentMap = m_mapIDCurrent;
	SendData.m_fPositionX	= m_vFieldPosition.x;
	SendData.m_fPositionY	= m_vFieldPosition.y;
	SendData.m_fPositionZ	= m_vFieldPosition.z;
    SendData.m_Channel    = m_nChannel;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    // 온라인 상태를 알려줄 친구 목록을 가져온다.
    std::set<DWORD> vOnlineChar;
    m_pFriend->GetValidFriend(vOnlineChar);

    // 온라인 상태를 알려줄 클럽원 목록을 가져온다.
    std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(m_ClubDbNum);
    if (spClub)
    {
        spClub->SetMemberState(m_CharDbNum, true, m_nChannel, m_mapIDCurrent);
        spClub->GetMemberDbNumList(vOnlineChar);
    }

	GLEvent::ManagerAgent* const _pEventManager(m_pServer->getEventManager());
	_pEventManager->online(this->m_CharDbNum);

    // 온라인 상태 캐릭터만 알림 전송
    for (std::set<DWORD>::iterator iter=vOnlineChar.begin(); iter!=vOnlineChar.end(); ++iter)
    {
        GLCharAG* pChar = m_pServer->GetCharByDbNum(*iter);

        if (pChar)
        {
			// 온라인 상태의 캐릭터의 맵정보 및 위치정보를 내친구 정보에 담는다
			m_pFriend->SetFriendMap(pChar->ChaDbNum(),pChar->GetCurrentMap());
			m_pFriend->SetFriendMapPosition(pChar->ChaDbNum(), 
											int(pChar->GetFieldMapPosition().x), 
											int(pChar->GetFieldMapPosition().z));

            std::tr1::shared_ptr<SFRIEND> Friend = pChar->GetFriend( GetChaName() );
            if ( Friend )
                Friend->m_Online = true;

            m_pServer->SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_CHAR_ONLINE_AC, SendBuffer, false);
        }
    }	
}

void GLCharAG::Offline()
{
    // 등록한 파티게시판정보가 있으면 삭제
    m_pServer->MapDeletePartyInfoHelper(m_szName);
	m_pServer->OfflinePartyMember(m_dwGaeaID, m_CharDbNum, m_PartyID);	
    
    //mjeon.CaptureTheField
    if (m_bCTFPlayer)
        m_pServer->CTFQuit(m_CharDbNum, m_wSchool);

	Timer::Manager& _managerTimer(Timer::Manager::getInstance());
	for ( Timer::HandleListConstIterator _iteratorHandleTimer(m_listHandleTimer.begin()); _iteratorHandleTimer != m_listHandleTimer.end(); ++_iteratorHandleTimer )
	{
		const Timer::handle _handleTimer(*_iteratorHandleTimer);
		_managerTimer.eraseTimer(_handleTimer);
	}

	GLEvent::ManagerAgent* const _pEventManager(m_pServer->getEventManager());
	_pEventManager->offline(this->m_CharDbNum);	

    // 알림
    GLMSG::NET_CHAR_STATE_OFFLINE_AC SendData;
    SendData.m_ChaDbNum = m_CharDbNum;
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    m_pFriend->SendToFriend(m_pServer, NET_MSG_CHAR_OFFLINE_AC, SendBuffer, true);

    // Club play time 세팅
    std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(m_ClubDbNum);
    if (spClub)
    {
        spClub->SetMemberLastPlayTime(m_CharDbNum, sc::time::GetCurrentTime());
        spClub->SetMemberState(m_CharDbNum, false, m_nChannel, m_mapIDCurrent);
        // 클럽원들에게 알림
        spClub->SendClubClient(NET_MSG_CHAR_OFFLINE_AC, SendBuffer);
    }

    // 인던 스크립트에 Offline이벤트 알림;
    InstanceSystem::ManagerAgent* _pInstanceMan = m_pServer->getInstanceManager();
    if ( _pInstanceMan )
        _pInstanceMan->offline(m_CharDbNum);
}

size_t GLCharAG::GetFriendSize() const
{
    return m_pFriend->Size();
}

void GLCharAG::AddFriendAndGroup(const char* szFriend)
{
    // Cache 에서 찾아본다.
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spChaData = m_pServer->GetCharCacheData(szFriend);

    if (spChaData)
    {
        if( m_pFriend->Add(szFriend, 0, m_pServer, this) == FALSE )
        {
            if( (int)GLCONST_CHAR::nMAX_FRIEND_LIST <= m_pFriend->GetFriendListCount() )
            {
                GLMSG::SNETPC_REQ_FRIENDADD_FB NetMsg(EMFRIENDADD_MAX, m_szName);
                m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsg);
            }
            else
            {
                GLMSG::SNETPC_REQ_FRIENDADD_FB NetMsg(EMFRIENDADD_FAIL, m_szName);
                m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsg);
            }
        }
    }
    else
    {
        // 존재하지 않는다면 DB 에서 조회해야 한다.
        // 존재한다면 캐싱 후 이 함수가 다시 호출된다.
        AddGameAdoJob(
            db::DbActionPtr(
                new db::GetinfofromChaDb(
                    0, 
                    0, 
                    szFriend, 
                    charnamechache::REQUEST_SERVER_ADDFIREND, 
                    m_szName )));
    }
}

void GLCharAG::DelFriend2(char* szFriend)
{
    m_pFriend->Del(szFriend);
}

bool GLCharAG::DelFriend(char* szFriend)
{
    return m_pFriend->Del(szFriend, m_pServer, this);
}

void GLCharAG::BlockFriend ( const char* szFriend )
{
    // Cache 에서 찾아본다.
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spChaData = m_pServer->GetCharCacheData(szFriend);
    if (spChaData)
    {
       if( (int)GLCONST_CHAR::nMAX_FRIEND_BLOCK <= m_pFriend->GetBlockListCount() )
            return;

        //	Note : 목록에서 블럭 수행.
        bool bOK = m_pFriend->Block( szFriend, m_pServer, this );

        //	Note : 블럭 메시지 전송.
        //
        GLMSG::SNETPC_REQ_FRIENDBLOCK_FB NetMsgFB(EMFRIENDBLOCK_FAIL);
        if ( bOK )	NetMsgFB.emFB = EMFRIENDBLOCK_ON; //(pNetMsg->bBLOCK?EMFRIENDBLOCK_ON:EMFRIENDBLOCK_OFF);
        else		NetMsgFB.emFB = EMFRIENDBLOCK_FAIL;

        StringCchCopy( NetMsgFB.szCHAR, CHR_ID_LENGTH+1, szFriend );
        m_pServer->SENDTOCLIENT( m_ClientSlot, &NetMsgFB );

    }
    else
    {
        // 존재하지 않는다면 DB 에서 조회해야 한다.
        // 존재한다면 캐싱 후 이 함수가 다시 호출된다.
        AddGameAdoJob(
            db::DbActionPtr(
                new db::GetinfofromChaDb(
                    0, 
                    0, 
                    szFriend, 
                    charnamechache::REQUEST_SERVER_BLOCKFIREND, 
                    m_szName )));
    }

    m_pFriend->Block(szFriend, m_pServer, this);
}

void GLCharAG::SetStateFriend(const char* szFriend, DWORD dwFlag)
{
    m_pFriend->SetState(szFriend, dwFlag, m_pServer, this);
}

EMFRIEND_FLAGS GLCharAG::GetStateFriend(char* szFriend)
{
    return m_pFriend->GetState(szFriend);
}

EMFRIEND_FLAGS GLCharAG::GetStateFriend(DWORD FriendDbNum)
{
    return m_pFriend->GetState(FriendDbNum);
}

bool GLCharAG::IsFriend(const std::string& ChaName)
{
    return m_pFriend->IsExist(ChaName);
}

bool GLCharAG::IsBlock( const std::string& ChaName )
{
    std::tr1::shared_ptr<SFRIEND> Friend = GetFriend( ChaName.c_str() );
    if( Friend )
        if( Friend->IsBLOCK() )
            return true;

    return false;
}

std::tr1::shared_ptr<SFRIEND> GLCharAG::GetFriend(const char* szName)
{
    return m_pFriend->GetData(szName);
}


// *****************************************************
// Desc : 친구리스트 요청 처리
// *****************************************************
HRESULT GLCharAG::MsgReqFriendList(NET_MSG_GENERIC* nmg)
{
    m_pFriend->MsgReqFriendList(m_pServer, this);
    return S_OK;
}

HRESULT GLCharAG::MsgReqFriendAdd(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_REQ_FRIENDADD* pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD *) nmg;
    
    //! 무조건 친구로 추가한다.
    // MSN 메신저를 생각하면 이해가 쉽다.
    // 친구를 추가함과 동시에 오프라인으로 추가된다.
    // 그리고 상대방에게 친구를 권유한다.

    GLCharAG* pChar = m_pServer->GetChar(pNetMsg->szADD_CHAR);
    if (!pChar)
    {
        if( GLUseFeatures::GetInstance().IsUsingNotifyWindow() )
        {
            m_pServer->FriendOfflineRequest(m_szName, pNetMsg->szADD_CHAR);
        }
        else
        {
            //! 알림창을 사용하지 않으면 무조건 친구로 추가할 수 없다.
            GLMSG::SNETPC_REQ_FRIENDADD_FB NetMsgFB(EMFRIENDADD_ABSENCE, pNetMsg->szADD_CHAR);
            m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
            return S_OK;
        }
    }
    else
    {
        // 친구 등록 요청.
        GLMSG::SNETPC_REQ_FRIENDADD_LURE NetMsgLure(m_szName);
        m_pServer->SENDTOCLIENT(pChar->m_ClientSlot, &NetMsgLure);
    }
    
    AddFriendAndGroup(pNetMsg->szADD_CHAR);

    return S_OK;
}

HRESULT GLCharAG::MsgReqFriendAddAns(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_REQ_FRIENDADD_ANS* pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD_ANS *) nmg;

    if( pNetMsg->bOK )
    {
        AddFriendAndGroup(pNetMsg->szREQ_CHAR);
    }
	else if( pNetMsg->bOK == FALSE )
	{
        // 거절한다면 친구에서 삭제한다.

		std::tr1::shared_ptr<SFRIEND> spFriend = GetFriend(pNetMsg->szREQ_CHAR);
		if( spFriend && spFriend->IsVALID() )
		{
			bool bDel = DelFriend(pNetMsg->szREQ_CHAR);

			GLMSG::SNETPC_REQ_FRIENDDEL_FB NetMsgFB ( EMFRIENDDEL_FAIL );
			if ( bDel )		NetMsgFB.emFB = EMFRIENDDEL_OK;
			else			NetMsgFB.emFB = EMFRIENDDEL_FAIL;
			StringCchCopy ( NetMsgFB.szDEL_CHAR, CHR_ID_LENGTH+1, pNetMsg->szREQ_CHAR );
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		}
	}
    
    //! 알림창을 사용한다면 예약된 알림을 삭제
    if( GLUseFeatures::GetInstance().IsUsingNotifyWindow() )
    {
        if( m_pNotify )
            if( m_pNotify->Del(pNetMsg->Guid) == true )
                m_pServer->AddGameAdoJob( db::DbActionPtr( new db::NotifyDelete( pNetMsg->Guid )));
    }

    return S_OK;
}

HRESULT GLCharAG::MsgReqFriendDel(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_REQ_FRIENDDEL *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDDEL *) nmg;

    //	Note : 목록에서 삭제 수행.
    bool bDEL = DelFriend ( pNetMsg->szDEL_CHAR );

    //	Note : 삭제 메시지 요청 결과.
    //
    GLMSG::SNETPC_REQ_FRIENDDEL_FB NetMsgFB ( EMFRIENDDEL_FAIL );
   
	if ( bDEL )	
		NetMsgFB.emFB = EMFRIENDDEL_OK;
    else		
		NetMsgFB.emFB = EMFRIENDDEL_FAIL;

	memcpy_s( &NetMsgFB.iDelFlag, sizeof(int), &pNetMsg->iDelFlag, sizeof(int) );
    StringCchCopy ( NetMsgFB.szDEL_CHAR, CHR_ID_LENGTH+1, pNetMsg->szDEL_CHAR );

    m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );

    return S_OK;
}

HRESULT GLCharAG::MsgReqFriendBlock(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_REQ_FRIENDBLOCK *pNetMsg = (GLMSG::SNETPC_REQ_FRIENDBLOCK *) nmg;

    BlockFriend( pNetMsg->szCHAR );

    return S_OK;
}

void GLCharAG::GetValidFriend(std::set<DWORD>& FriendList) const
{
    m_pFriend->GetValidFriend(FriendList);
}