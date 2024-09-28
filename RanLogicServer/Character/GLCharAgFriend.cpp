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
    // ģ���鿡�� online ���� ����, Ŭ�����鿡�� online ���� ����
    GLMSG::NET_CHAR_STATE_ONLINE_AC SendData;
    SendData.m_ChaDbNum   = m_CharDbNum;
    SendData.m_CurrentMap = m_mapIDCurrent;
	SendData.m_fPositionX	= m_vFieldPosition.x;
	SendData.m_fPositionY	= m_vFieldPosition.y;
	SendData.m_fPositionZ	= m_vFieldPosition.z;
    SendData.m_Channel    = m_nChannel;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    // �¶��� ���¸� �˷��� ģ�� ����� �����´�.
    std::set<DWORD> vOnlineChar;
    m_pFriend->GetValidFriend(vOnlineChar);

    // �¶��� ���¸� �˷��� Ŭ���� ����� �����´�.
    std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(m_ClubDbNum);
    if (spClub)
    {
        spClub->SetMemberState(m_CharDbNum, true, m_nChannel, m_mapIDCurrent);
        spClub->GetMemberDbNumList(vOnlineChar);
    }

	GLEvent::ManagerAgent* const _pEventManager(m_pServer->getEventManager());
	_pEventManager->online(this->m_CharDbNum);

    // �¶��� ���� ĳ���͸� �˸� ����
    for (std::set<DWORD>::iterator iter=vOnlineChar.begin(); iter!=vOnlineChar.end(); ++iter)
    {
        GLCharAG* pChar = m_pServer->GetCharByDbNum(*iter);

        if (pChar)
        {
			// �¶��� ������ ĳ������ ������ �� ��ġ������ ��ģ�� ������ ��´�
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
    // ����� ��Ƽ�Խ��������� ������ ����
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

    // �˸�
    GLMSG::NET_CHAR_STATE_OFFLINE_AC SendData;
    SendData.m_ChaDbNum = m_CharDbNum;
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    m_pFriend->SendToFriend(m_pServer, NET_MSG_CHAR_OFFLINE_AC, SendBuffer, true);

    // Club play time ����
    std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(m_ClubDbNum);
    if (spClub)
    {
        spClub->SetMemberLastPlayTime(m_CharDbNum, sc::time::GetCurrentTime());
        spClub->SetMemberState(m_CharDbNum, false, m_nChannel, m_mapIDCurrent);
        // Ŭ�����鿡�� �˸�
        spClub->SendClubClient(NET_MSG_CHAR_OFFLINE_AC, SendBuffer);
    }

    // �δ� ��ũ��Ʈ�� Offline�̺�Ʈ �˸�;
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
    // Cache ���� ã�ƺ���.
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
        // �������� �ʴ´ٸ� DB ���� ��ȸ�ؾ� �Ѵ�.
        // �����Ѵٸ� ĳ�� �� �� �Լ��� �ٽ� ȣ��ȴ�.
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
    // Cache ���� ã�ƺ���.
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spChaData = m_pServer->GetCharCacheData(szFriend);
    if (spChaData)
    {
       if( (int)GLCONST_CHAR::nMAX_FRIEND_BLOCK <= m_pFriend->GetBlockListCount() )
            return;

        //	Note : ��Ͽ��� �� ����.
        bool bOK = m_pFriend->Block( szFriend, m_pServer, this );

        //	Note : �� �޽��� ����.
        //
        GLMSG::SNETPC_REQ_FRIENDBLOCK_FB NetMsgFB(EMFRIENDBLOCK_FAIL);
        if ( bOK )	NetMsgFB.emFB = EMFRIENDBLOCK_ON; //(pNetMsg->bBLOCK?EMFRIENDBLOCK_ON:EMFRIENDBLOCK_OFF);
        else		NetMsgFB.emFB = EMFRIENDBLOCK_FAIL;

        StringCchCopy( NetMsgFB.szCHAR, CHR_ID_LENGTH+1, szFriend );
        m_pServer->SENDTOCLIENT( m_ClientSlot, &NetMsgFB );

    }
    else
    {
        // �������� �ʴ´ٸ� DB ���� ��ȸ�ؾ� �Ѵ�.
        // �����Ѵٸ� ĳ�� �� �� �Լ��� �ٽ� ȣ��ȴ�.
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
// Desc : ģ������Ʈ ��û ó��
// *****************************************************
HRESULT GLCharAG::MsgReqFriendList(NET_MSG_GENERIC* nmg)
{
    m_pFriend->MsgReqFriendList(m_pServer, this);
    return S_OK;
}

HRESULT GLCharAG::MsgReqFriendAdd(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_REQ_FRIENDADD* pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD *) nmg;
    
    //! ������ ģ���� �߰��Ѵ�.
    // MSN �޽����� �����ϸ� ���ذ� ����.
    // ģ���� �߰��԰� ���ÿ� ������������ �߰��ȴ�.
    // �׸��� ���濡�� ģ���� �����Ѵ�.

    GLCharAG* pChar = m_pServer->GetChar(pNetMsg->szADD_CHAR);
    if (!pChar)
    {
        if( GLUseFeatures::GetInstance().IsUsingNotifyWindow() )
        {
            m_pServer->FriendOfflineRequest(m_szName, pNetMsg->szADD_CHAR);
        }
        else
        {
            //! �˸�â�� ������� ������ ������ ģ���� �߰��� �� ����.
            GLMSG::SNETPC_REQ_FRIENDADD_FB NetMsgFB(EMFRIENDADD_ABSENCE, pNetMsg->szADD_CHAR);
            m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
            return S_OK;
        }
    }
    else
    {
        // ģ�� ��� ��û.
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
        // �����Ѵٸ� ģ������ �����Ѵ�.

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
    
    //! �˸�â�� ����Ѵٸ� ����� �˸��� ����
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

    //	Note : ��Ͽ��� ���� ����.
    bool bDEL = DelFriend ( pNetMsg->szDEL_CHAR );

    //	Note : ���� �޽��� ��û ���.
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