#include "../pch.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/DateTime.h"

#include "../../RanLogic/Item/GLITEMLMT.h"
#include "../../RanLogic/Msg/FriendMsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

#include "../Util/GLItemLimit.h"
#include "../Friend/FriendAgent.h"
#include "../AgentServer/GLAgentServer.h"
#include "../Club/GLClubAgent.h"
#include "../Club/GLClubManAgent.h"
#include "../Party/GLPartyManagerAgent.h"
#include "../Move/MoveManagerServer.h"

#include "./GLCharAG.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLCharAG::GLCharAG(GLAgentServer* pServer, wb::EM_COUNTRY Country)
    : m_pServer(pServer)
	, m_ClientSlot(0)
	, m_nChannel(0)
	, m_nOriginChannel(0)
	, m_dwGaeaID(0)

	, m_dwCurFieldSvr(FIELDSERVER_MAX)

	, m_dwActState(NULL)

	, m_dwLastPostGetData(0)	//mjeon.post.get
    , m_dwLastPostGetState(0)	//mjeon.post.get
    , m_dwLastPostGetSendState(0)
    , m_dwLastPostGetRecvState(0)

	, m_dwNextGateID(0)
	, m_vNextPos(FLT_MAX,FLT_MAX,FLT_MAX)
	, m_dwNextFieldSvr(FIELDSERVER_MAX)
	, m_dwNextChannel(MAX_CHANNEL_NUMBER)
	, m_bIsInstance(FALSE)

	, m_bFIELDCK_OK(false)
	, m_dwFIELDCK_NUM(0)	
	, m_bViewWhisperMSG(FALSE)

	, m_bReciveEventTime(false)
	, m_bTracingUser(false)
    , m_bWhisperIgnore(false)
	, m_dwComboAttend( 0 )
	, m_fCyberClassCheck(0.0f)

	, m_pvPost(new VPOST)

	, m_bPostGotData      (FALSE)
    , m_bPostGotState     (FALSE)
    , m_bPostGotSendState (FALSE)
    , m_bPostGotRecvState (FALSE)

	, m_bCTFPlayer(FALSE)
	, m_dwKey(INVALID_INSTANCE_KEY)
    , m_Country(Country)
    
	, m_vFieldPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
	, m_fCheckValidMapTime(0.0f)
	, m_bMapMoveFCard(FALSE)
{
	if ( m_pServer == NULL )
	{
		sc::writeLogError("GLCharAG::GLCharAG m_pServer == NULL ");
	}
	
	for (int c=0; c<MAX_CHANNEL_NUMBER; ++c)
		for (int i=0; i<FIELDSERVER_MAX; ++i)
            m_bFIELD_CK[c][i] = false;

	m_pvPost->reserve(GLCONST_POST::dwPOST_HOW_MANY_GET);

	m_vecGetWhisperList.clear();
	//m_mapFriend.clear();
    m_pFriend->Clear();
	m_vecAttend.clear();
    m_vecRebuy.clear();

	ZeroMemory(&m_AUTHFB, sizeof(m_AUTHFB));
	ZeroMemory(&m_AUTHTW, sizeof(m_AUTHTW));

    // 로그인 시간
    m_tLoginTime = sc::time::GetCurrentTime();
}

GLCharAG::~GLCharAG()
{    
	//m_mapFriend.clear();
    m_pFriend->Clear();
	m_vecAttend.clear();
    m_vecRebuy.clear();
}

void GLCharAG::AddGameDbJob(db::DbActionPtr spJob)
{
    m_pServer->AddGameDbJob(spJob);
}

//mjeon.ado
void GLCharAG::AddGameAdoJob(db::DbActionPtr spJob)
{
	m_pServer->AddGameAdoJob(spJob);	
}

void GLCharAG::AddLogAdoJob(db::DbActionPtr spJob)
{
	m_pServer->AddLogAdoJob(spJob);	
}

HRESULT GLCharAG::CreateChar(GLCHARAG_DATA* pchar_data)
{
	GLCHARAG_DATA::operator = (*pchar_data);

    // GM이라면 기본으로 귓속말 차단상태로 만든다.
    if (USER_USER_GM <= m_dwUserLvl)
        m_bWhisperIgnore = true;

	return S_OK;
}

//	점검이 끝난 캐릭터인지 검사.
//	(점검이 끝난 캐릭터는 필드서버와의 연결이 유효해야한다.)
bool GLCharAG::IsDoneFieldCheck ()
{
	return m_bFIELDCK_OK;
}

//	필드에 케릭터를 생성전에 필드의 char 무결성 점검 테스트 완료 여부 점검.
//	( 완료후는 단한번만 호출되야함. )
bool GLCharAG::IsFieldCheckComplete(bool* pFieldSvr)
{
	if (m_bFIELDCK_OK)
	{
		sc::writeLogError(std::string("IsFieldCheckComplete again"));
		return false;
	}

	for (int c=0; c<MAX_CHANNEL_NUMBER; ++c)
	{
		for (int i=0; i<FIELDSERVER_MAX; ++i)
		{
			if (m_bFIELD_CK[c][i] != pFieldSvr[c*MAX_CHANNEL_NUMBER+i])
                return false;
		}
	}

	m_bFIELDCK_OK = true;
	return true;
}

void GLCharAG::SetCurrentField(DWORD dwChannel, DWORD dwSvr, const MapID& mapIDNew, BOOL bIsInstance)
{	
	if ( m_nChannel != dwChannel )
	{
		m_pServer->ChangeChannelUser( m_ClientSlot, m_nChannel, dwChannel );
	}

	m_nChannel = dwChannel;
	m_dwCurFieldSvr = dwSvr;
	m_bIsInstance = bIsInstance;

	GLAGLandMan* pOLD_LAND = m_pServer->GetLand(m_mapIDCurrent.getGaeaMapID());
	if (pOLD_LAND)
        pOLD_LAND->DropOutPC(m_CharDbNum);

	GLAGLandMan* pNEW_LAND = m_pServer->GetLand(mapIDNew.getGaeaMapID());
	if (pNEW_LAND)
        pNEW_LAND->DropPC(m_CharDbNum);	

    m_mapIDPrevious = m_mapIDCurrent;
    SetCurrentMap(mapIDNew);    

    std::set<DWORD> NotifyList;
    m_pFriend->GetValidFriend(NotifyList);

    std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(m_ClubDbNum);
    if (spClub)
    {
        spClub->SetMemberState(m_CharDbNum, mapIDNew);
        spClub->GetMemberDbNumList(NotifyList);
    }

    GLMSG::NET_MAP_CHANGE_FRIEND_CLUB_AC SendData(m_CharDbNum, mapIDNew);
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    for (std::set<DWORD>::iterator iter=NotifyList.begin(); iter!=NotifyList.end(); ++iter)
    {
        GLCharAG* pNotifyChar = m_pServer->GetCharByDbNum(*iter);
        if (pNotifyChar)
        {
            pNotifyChar->SetFriendMap(m_CharDbNum, mapIDNew);
            m_pServer->SENDTOCLIENT(pNotifyChar->ClientSlot(), NET_MSG_MAP_CHANGE_FRIEND_CLUB_AC, SendBuffer);
        }
    }

	//
	//mjeon.CaptureTheField
	//
	//m_pServer->CTFCheckPlayerRecall(this, sNID.dwID);
}

void GLCharAG::SetNextFieldSvr(const MapID& mapIDNext, DWORD dwGateID, D3DXVECTOR3 vPos, DWORD dwFieldSvr, DWORD dwChannel, BOOL bIsInstance)
{
	m_mapIDNext = mapIDNext;
	m_dwNextGateID = dwGateID;
	m_vNextPos = vPos;

	m_dwNextFieldSvr = dwFieldSvr;
	m_dwNextChannel = dwChannel;
	m_bIsInstance = bIsInstance; // 사용 하지 않음;
}

void GLCharAG::ResetNextFieldSvr()
{
	m_mapIDNext.reset();
	m_dwNextGateID = 0;

	m_dwNextFieldSvr = FIELDSERVER_MAX;
	m_dwNextChannel = MAX_CHANNEL_NUMBER;
}

//	최근 대련 정보 요청.
const SCONFT_LOG* GLCharAG::GETCONFRONT_LOG(DWORD dwCHARID)
{
	CONFT_MAP_ITER iter = m_mapCONFT.find(dwCHARID);
	if (iter!= m_mapCONFT.end())
	{
		const SCONFT_LOG &sCONFT = (*iter).second;
		return &sCONFT;
	}
	return NULL;
}

void GLCharAG::SaveLogConfront ()
{
	//	Note : 대련 로그 남김.
	if ( m_sCONFTING.IsCONFRONTING() )
	{
		if ( m_sCONFTING.emTYPE == EMCONFT_ONE )
		{
			SCONFT_LOG sCONFT_LOG(0,0.0f);
			CONFT_MAP_ITER iter = m_mapCONFT.find ( m_sCONFTING.dwCHARID );
			if ( iter!=m_mapCONFT.end() )
			{
				sCONFT_LOG = (*iter).second;
				m_mapCONFT.erase ( iter );
			}

			sCONFT_LOG.dwCOUNT++;
			sCONFT_LOG.fTIMER = 0.0f;

			// 위 로직이 왜이런지 이유를 모르겠다. 지웠다가 있을지 없을지 모르는데 삽입은 한다. [] 사용말고 insert 로 삽입 로직 변경
			m_mapCONFT.insert( CONFT_MAP_VALUE( m_sCONFTING.dwCHARID, sCONFT_LOG ) );
			//m_mapCONFT[m_sCONFTING.dwCHARID] = sCONFT_LOG;
		}
	}
}

void GLCharAG::ResetConfront ( EMCONFRONT_END emEND )
{
	SaveLogConfront ();

	m_sCONFTING.RESET ();

	GLMSG::SNETPC_CONFRONT_END2_CLT NetMsgClt(emEND);
	m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgClt );

	GLMSG::SNETPC_CONFRONT_END2_FLD NetMsgFld(emEND);
	NetMsgFld.dwChaNum = ChaDbNum();
	m_pServer->SENDTOFIELD ( m_ClientSlot, &NetMsgFld );
}

void GLCharAG::ResetConfrontOk ()
{
	if ( m_sCONFTING.IsCONFRONTING() )
	{
		SaveLogConfront ();
		m_sCONFTING.RESET ();
	}
}

void GLCharAG::StartConfront ( EMCONFT_TYPE emType, DWORD dwTAR_ID, const SCONFT_OPTION &sOption )
{
	DWORD dwCHARID(0);
	PGLCHARAG pCHAR = m_pServer->GetChar ( dwTAR_ID );
	if ( pCHAR )	dwCHARID = pCHAR->m_CharDbNum;


	m_sCONFTING.RESET ();

	m_sCONFTING.dwTAR_ID = dwTAR_ID;
	m_sCONFTING.dwCHARID = dwCHARID;
	m_sCONFTING.emTYPE = emType;
	m_sCONFTING.fTIMER = 0.0f;
	m_sCONFTING.sOption = sOption;

	//	Note : 필드 서버에 알림.	(자신)
	//
	GLMSG::SNETPC_CONFRONT_START2_FLD NetMsgFld(emType, dwTAR_ID, sOption);
	NetMsgFld.dwChaNum = ChaDbNum();
	m_pServer->SENDTOFIELD ( m_ClientSlot, &NetMsgFld );
}

bool GLCharAG::IsCHATBLOCK ()
{
	if ( m_tCHATBLOCK==0 )	return false;

	CTime cBLOCK(m_tCHATBLOCK);
	CTime cCUR(CTime::GetCurrentTime());

	//	블럭시간인지 검사.
	if ( cBLOCK > cCUR )	return true;

	//	시간이 초과 되었을 경우는 시간을 리셋.
	m_tCHATBLOCK = 0;

	return false;
}

HRESULT GLCharAG::FrameMove ( float fTime, float fElapsedTime )
{
	if ( false == IsDoneFieldCheck() )
	{
		// 정상적으로 필드서버와 연결이 안되었을 경우 
		// 아래 부분에 '현재 필드 서버에 자신이 떠나갈 예정임을 통보' 여기에서 SENDTOFIELD 를 하면  
		// NetAgentClientMan::GetFieldSocket() - The CliendID(%1%) is not mapped to FieldServerInfo yet. 에러가 출력된다.
		// 에이전트 서버가 m_mapFieldServer 에 클라이언트 슬롯번호와 필드정보를 매칭해서 가지고 있는데, 이 정보는 필드서버가
		// 정상적일 경우에만 가지고 있다.
		// ex) 1번 필드서버가 다운된 상황에서 1번 필드서버가 가지고 있는 맵이 마지막 저장지점인 캐릭터가 캐릭터 선택창에서
		// 월드 진입을 하는 경우 NetAgentClientMan::GetFieldSocket() - The CliendID(%1%) is not mapped to FieldServerInfo yet. 에러가 지속적으로 출력됨
		return S_OK;
	}

	// 20140721 gs crash
	if ( NULL == m_pServer )
	{
		sc::writeLogError( sc::string::format( "GLCharAG::FrameMove NULL == m_pServer. ChaDbNum %1%", m_CharDbNum ) );
		return E_FAIL;
	}
//	IP보너스
//#if defined(TH_PARAM) || defined(MYE_PARAM ) || defined(MY_PARAM) || defined ( PH_PARAM ) //|| defined(_RELEASED)
	if (m_pServer->GetServiceProvider() == SP_THAILAND || m_pServer->GetServiceProvider() == SP_MALAYSIA || m_pServer->GetServiceProvider() == SP_MALAYSIA_EN || m_pServer->GetServiceProvider() == SP_PHILIPPINES)
	{
		UpdateIPBonus( fTime, fElapsedTime );
	}
//#endif 

	const SNATIVEID _gaeaMapID(m_mapIDCurrent.getGaeaMapID());
	const SNATIVEID _baseMapID(m_mapIDCurrent.getBaseMapID());
	GLAGLandMan* pLand(m_pServer->GetLand(_gaeaMapID));	

	// 현재 agent에서는 child-map을 사용할 일이 없으므로 기본 맵을 반환한다;
	// 사용할 일이 생길 경우 필드와 동일하게 생성 해주도록 한다;
	// const bool _bInvalidLand((pLand == NULL) || (pLand->GetBaseMapID() != _baseMapID));
	const bool _bInvalidLand(pLand == NULL);
	if ( (_bInvalidLand == true) && ((m_fCheckValidMapTime += fElapsedTime) > 3.0f) )
	{
		m_fCheckValidMapTime = 0.0f;
		const SNATIVEID _returnMapID(m_sStartMapID);
		const DWORD _dwGateID(m_dwStartGate);
		const D3DXVECTOR3& _returnPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		const unsigned int _nFieldServer(m_pServer->GetFieldServer(m_sStartMapID));
		const unsigned int _nFieldChannel(GetChannel());

		MoveSystem::SMOVE_INFO_AGENT sTempMoveInfo;
		sTempMoveInfo.dwChaDBNum = m_CharDbNum;
		sTempMoveInfo.sMapID = _returnMapID;
		sTempMoveInfo.dwGateID = _dwGateID;
		sTempMoveInfo.vPosition = _returnPosition;
		sTempMoveInfo.dwFieldNum = _nFieldServer;
		sTempMoveInfo.dwChannelNum = _nFieldChannel;

		if ( false == MoveSystem::CMoveManagerServer::Instance()->MoveMap( sTempMoveInfo ) )
		{
			return FALSE;
		}

		/*if ( m_pServer->IsFieldOnline(_nFieldChannel, _nFieldServer) == false )
			return E_FAIL;

		m_pServer->ReActionMoveFieldSvr(m_ClientSlot, m_dwGaeaID);
		SetNextFieldSvr(_returnMapID, _dwGateID, _returnPosition, _nFieldServer);

		// 현재 필드 서버에 자신이 떠나갈 예정임을 통보.
		GLMSG::SNETPC_FIELDSVR_OUT NetMsg;
		NetMsg.dwChaNum = m_CharDbNum;
		NetMsg.dwSvrGroupNum = m_pServer->GetServerGroup();	
		NetMsg.targetMapID = _returnMapID;
		NetMsg.bSameFieldServer = (m_dwCurFieldSvr == _nFieldServer);
		m_pServer->SENDTOFIELD(m_ClientSlot, &NetMsg);*/
	}

	if ( !m_mapCONFT.empty() )
	{
		CONFT_MAP_ITER iter = m_mapCONFT.begin();
		for ( ; iter!=m_mapCONFT.end(); ++iter )
		{
			SCONFT_LOG &sCONFT = (*iter).second;

			//	Note : 일정 시간 이상 지낫을 때 대련 횟수 리셋.
			//
			sCONFT.fTIMER += fElapsedTime;
			if ( sCONFT.fTIMER > GLCONST_CHAR::fCONFRONT_RESET )
			{
				sCONFT.dwCOUNT = 0;
			}
		}
	}

	if ( m_sCONFTING.IsCONFRONTING() )
	{
		//	Note : 대련 유효성 검사.
		//
		bool bvalid(false);
		PGLCHARAG pTAR_ONE(NULL);
		switch ( m_sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			{
				pTAR_ONE = m_pServer->GetChar ( m_sCONFTING.dwTAR_ID );
				if ( pTAR_ONE )
				{
					const SCONFTING& sTAR_CONFT = pTAR_ONE->GETCONFRONTING();
					if ( m_sCONFTING.IsSAMENESS(sTAR_CONFT.emTYPE,pTAR_ONE->m_dwGaeaID) )		bvalid = true;
				}
			}
			break;

		case EMCONFT_PARTY:
			{
				bool bpartyA(false), bpartyB(false);
				
				if ( m_PartyID.isValidParty() && m_pServer->GetParty(m_PartyID))
                    bpartyA = true;

				const GLPartyID ConftTargetPartyID(m_sCONFTING.dwTAR_ID);
                if ( ConftTargetPartyID.isValidParty() && m_pServer->GetParty(ConftTargetPartyID) )
                	bpartyB = true;

				if ( bpartyA && bpartyB )
                    bvalid = true;
			}
			break;

		case EMCONFT_GUILD:
			{
				bool bclubA(false), bclubB(false);
				if (CLUB_NULL != m_ClubDbNum && m_pServer->GetClub(m_ClubDbNum))
                    bclubA = true;
				if (CLUB_NULL != m_sCONFTING.dwTAR_ID && m_pServer->GetClub(m_sCONFTING.dwTAR_ID))
                    bclubB = true;
				if (bclubA && bclubB)
                    bvalid = true;
			}
			break;
		};

		if ( !bvalid )
		{
			//	Note : 일반 오류로 인한 대련 취소.
			//
			ResetConfront ( EMCONFRONT_END_FAIL );
		}
		else
		{
			switch ( m_sCONFTING.emTYPE )
			{
			case EMCONFT_ONE:
				//	Note : 시간 제한 검사.
				//
				m_sCONFTING.fTIMER += fElapsedTime;
				if ( m_sCONFTING.fTIMER > GLCONST_CHAR::fCONFRONT_TIME )
				{
					//	Note : 자신에게 대련 취소 알림.
					//
					ResetConfront ( EMCONFRONT_END_TIME );

					//	Note : 상대방에게도 대련 취소 알림.
					//
					pTAR_ONE->ResetConfront ( EMCONFRONT_END_TIME );
				}

				if ( m_mapIDCurrent != pTAR_ONE->m_mapIDCurrent )
				{
					//	Note : 자신에게 맵이동으로 대련 취소 알림.
					//
					ResetConfront ( EMCONFRONT_END_MOVEMAP );

					//	Note : 상대방에게도 맵이동으로 대련 취소 알림.
					//
					pTAR_ONE->ResetConfront ( EMCONFRONT_END_MOVEMAP );
				}
				break;

			case EMCONFT_PARTY:
				break;

			case EMCONFT_GUILD:
				break;
			};
		}
	}
/*
	m_fFRIEND_TIMER += fElapsedTime;
	if ( m_fFRIEND_TIMER > 5.f )
	{
		m_fFRIEND_TIMER = 0.0f;

        m_pFriend->FrameMove(fTime, fElapsedTime, m_ClientSlot, m_pServer);

		
	}
*/
	return S_OK;
}

HRESULT GLCharAG::MsgActState(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_ACTSTATE* pNetMsg = (GLMSG::SNETPC_ACTSTATE*)nmg;

	const DWORD dwOldActState = m_dwActState;

	// 배틀로얄 서버에서 강제적으로 Visible 모드 수정가능하도록 체크항목 추가.

	if (m_dwUserLvl >= USER_USER_GM || GLCONST_CHAR::bBATTLEROYAL)
	{
		if (pNetMsg->dwActState & EM_REQ_VISIBLENONE)
			SetSTATE(EM_REQ_VISIBLENONE);
		else
			ReSetSTATE(EM_REQ_VISIBLENONE);
		
		if (pNetMsg->dwActState & EM_REQ_VISIBLEOFF)
			SetSTATE(EM_REQ_VISIBLEOFF);
		else
			ReSetSTATE(EM_REQ_VISIBLEOFF);
	}

	if (pNetMsg->dwActState & EM_ACT_PEACEMODE)
		SetSTATE(EM_ACT_PEACEMODE);
	else
		ReSetSTATE(EM_ACT_PEACEMODE);

	if ( dwOldActState != m_dwActState || GLCONST_CHAR::bBATTLEROYAL )
	{
		GLMSG::SNETPC_ACTSTATE NetMsgToField(m_dwActState);
		NetMsgToField.dwChaNum = ChaDbNum();
		NetMsgToField.dwSvrGroupNum = m_pServer->GetSvrGroupNum();

		if (IsInInstance())
		{
			m_pServer->SENDTOMATCH(&NetMsgToField);
		}
		else
		{
			m_pServer->SENDTOFIELD(m_ClientSlot, &NetMsgToField);
		}
	}

	return S_OK;
}

// *****************************************************
// Desc : 친구와 클럽맴버들에게 변경된 이름 알림.
// *****************************************************
HRESULT GLCharAG::MsgRenameBrd(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_INVEN_RENAME_AGTBRD *pNetMsg = (GLMSG::SNETPC_INVEN_RENAME_AGTBRD *) nmg;
    
	// 온라인에 있는 친구들에게 알림
    m_pFriend->MsgRenameBrd(pNetMsg->szOldName, pNetMsg->szNewName, m_pServer, this);

	// 파티에게 알림
	const GLPartyAgent* const pParty = m_pServer->GetParty(m_PartyID);
	if ( pParty != NULL )
	{		
		GLMSG::SNET_PARTY_MEMBER_RENAME NetMsgParty(m_PartyID, m_dwGaeaID);
		NetMsgParty.SetName(pNetMsg->szNewName);
		
		m_pServer->SENDTOPARTY(m_PartyID, &NetMsgParty); // Send to Client;		
		m_pServer->SENDTOALLCHANNEL(&NetMsgParty); // Send to Field;
	}

	// 클럽맴버들에게 알림	
	std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub(m_ClubDbNum);
	if ( pCLUB )
	{
		GLCLUBMEMBER* pClubMember = pCLUB->GetMember ( m_CharDbNum );

		if ( pCLUB->IsMaster ( m_CharDbNum ) ) 
		{
			pCLUB->SetMasterName(pNetMsg->szNewName);
		}

		if ( pClubMember )
		{
			pClubMember->SetName(pNetMsg->szNewName);

			// Send to Client
			GLMSG::SNET_CLUB_MEMBER_RENAME_CLT NetMsgClubClt;
			NetMsgClubClt.dwClubID = pCLUB->m_DbNum;
			NetMsgClubClt.dwCharID = m_CharDbNum;
			NetMsgClubClt.SetName(pNetMsg->szNewName);
			m_pServer->SENDTOCLUBCLIENT(pCLUB->m_DbNum, &NetMsgClubClt);

			// Send to Field
			GLMSG::SNET_CLUB_MEMBER_RENAME_FLD NetMsgClubFld;
			NetMsgClubFld.dwClubID = pCLUB->m_DbNum;
			NetMsgClubFld.dwCharID = m_CharDbNum;
			NetMsgClubFld.SetName(pNetMsg->szNewName);
			m_pServer->SENDTOALLCHANNEL(&NetMsgClubFld);
		}
	}
	m_pServer->ChangeNameMap(this, m_szName, pNetMsg->szNewName);
	
	return S_OK;
}
/*
HRESULT GLCharAG::MsgPhoneNumberBrd( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PHONE_NUMBER_AGTBRD *pNetMsg = (GLMSG::SNETPC_PHONE_NUMBER_AGTBRD *) nmg;

	m_pFriend->MsgPhoneNumberBrd(pNetMsg->szName, pNetMsg->szNewPhoneNumber, m_pServer, this);
	m_pServer->ChangeNameMap(this, pNetMsg->szNewPhoneNumber);
	return S_OK;
}
*/
HRESULT	GLCharAG::MsgUpdateStartMap(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_UPDATE_STARTCALL *pNetMsg = (GLMSG::SNETPC_UPDATE_STARTCALL *)nmg;
			
	// 시작위치 변경
	m_sStartMapID = pNetMsg->sStartMapID;
	m_dwStartGate = pNetMsg->dwStartGateID;
	m_vStartPos	  = pNetMsg->vStartPos;

	return S_OK;
}

HRESULT GLCharAG::MsgChangeArm( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_PUTON_CHANGE_AG *pNetMsg = (GLMSG::SNETPC_PUTON_CHANGE_AG *)nmg;
	m_bUseArmSub = pNetMsg->bUseArmSub;

	return S_OK;
}
/*
HRESULT GLCharAG::MsgFriendClubWindowOpen( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIEND_CLUB_OPEN *pNetMsg = (GLMSG::SNETPC_REQ_FRIEND_CLUB_OPEN *)nmg;
	
	if( pNetMsg->emTYPE == EMFRIEND_WINDOW )
	{
		//m_bFriendWindowOpen = pNetMsg->bOpen;
	}
	else if (pNetMsg->emTYPE == EMCLUB_WINDOW)
	{        
        //	Note : 클럽 멤버 on/off 상태 갱신.
        std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pServer->GetClub(m_ClubDbNum);
        if (pCLUB)
        {
            GLMSG::SNET_CLUB_MEMBER_STATE NetMsgState;
            PGLCHARAG pCHAR = NULL;
            bool bINSERT = false;
            CLUBMEMBERS& Members = pCLUB->GetMembers();
            CLUBMEMBERS_ITER pos = Members.begin();
            for ( ; pos != Members.end(); ++pos)
            {
                const GLCLUBMEMBER& sMember = pos->second;
                pCHAR = m_pServer->GetCharByDbNum(sMember.dwID);
                bool bONLINE(false);
                int nCHANNEL(0);
                SNATIVEID nidMAP(false);

                if (pCHAR)
                {
                    bONLINE = true;
                    nCHANNEL = pCHAR->m_nChannel;
                    nidMAP = pCHAR->m_sCurMapID;
                }

                bINSERT = NetMsgState.ADD ( sMember.dwID, bONLINE, nCHANNEL, nidMAP );
                if ( !bINSERT )
                {
                    m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgState );
                    NetMsgState.RESET();
                    NetMsgState.ADD ( sMember.dwID, bONLINE, nCHANNEL, nidMAP );
                }
            }

            if ( NetMsgState.dwNumber!=0 )
            {
                m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgState );
            }
        }
	}

	return S_OK;
}
*/
HRESULT GLCharAG::MsgReqQboxOption( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_QBOX_OPTION_REQ_AG* pNetMsg = (GLMSG::SNET_QBOX_OPTION_REQ_AG*)nmg;

	if ( pNetMsg->dwGaeaID != m_dwGaeaID )
		return E_FAIL;

	GLMSG::SNET_QBOX_OPTION_REQ_FLD NetMsgFld(m_PartyID, pNetMsg->dwGaeaID, pNetMsg->bQBoxEnable);	
	m_pServer->SENDTOALLCHANNEL(&NetMsgFld);

	return S_OK;
}

HRESULT	GLCharAG::MsgReqAttendList( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_ATTENDLIST_FB NetMsgFB;

	NetMsgFB.tLoginTime = m_tLoginTime;
	NetMsgFB.dwAttendTime = GLCONST_ATTENDANCE::dwAttendTime;

	if ( m_vecAttend.empty() )
	{
		NetMsgFB.dwAttendCombo = 0;
		NetMsgFB.SetSize( 0 );		
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );

		return S_OK;
	}

	//	연속 일수 계산
	CarcAttendCombo();

	int nSize = (int)m_vecAttend.size();
	CTime cCurTime( CTime::GetCurrentTime() );	
	
	CTime cTime;
	int nDataSize = 0;

	for ( int i = 0; i < nSize; ++i)
	{
		CTime cBackTime = cTime;
		cTime = m_vecAttend[i].tAttendTime;

		// 같은달 데이터만 보냄
		if ( cCurTime.GetYear() == cTime.GetYear() && cCurTime.GetMonth() == cTime.GetMonth() )
		{
			if ( cBackTime.GetYear() == cTime.GetYear() && cBackTime.GetMonth() == cTime.GetMonth() && cBackTime.GetDay() == cTime.GetDay() ) 
				continue;

			NetMsgFB.sAttend[nDataSize] = m_vecAttend[i];
			nDataSize++;
		}
	}

	NetMsgFB.dwAttendCombo = m_dwComboAttend;
	NetMsgFB.SetSize( nDataSize );
	m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
	
	return S_OK;
}


HRESULT GLCharAG::MsgReqAttendance( NET_MSG_GENERIC *nmg )
{
	GLMSG::SNETPC_REQ_ATTENDANCE_FB NetMsgFB;

	if ( !GLCONST_ATTENDANCE::bUse )
	{
		NetMsgFB.emFB = EMREQ_ATTEND_FB_NOTUSE;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return E_FAIL;
	}

	CTime cCurTime( CTime::GetCurrentTime() );

	//	게임접속시간 체크	
	CTime cDayTime( cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(),0,0,0 );
	CTime cLoginTime;
	if (m_tLoginTime < cDayTime.GetTime())
        cLoginTime = cDayTime;
	else
        cLoginTime = m_tLoginTime;

	CTimeSpan cGameTime = cCurTime - cLoginTime;
	if ( cGameTime.GetTotalMinutes() < GLCONST_ATTENDANCE::dwAttendTime ) 
	{
		NetMsgFB.emFB = EMREQ_ATTEND_FB_ATTENTIME;
		m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
		return S_OK;
	}

	if ( !m_vecAttend.empty() )
	{
		int nSize = (int)m_vecAttend.size();

		USER_ATTEND_INFO& sAttend = m_vecAttend[nSize-1];
		CTime cTime = sAttend.tAttendTime;

		// 오늘 이미 출석 했다면
		if ( cCurTime.GetYear() == cTime.GetYear() && 
			 cCurTime.GetMonth() == cTime.GetMonth() && 
			 cCurTime.GetDay() == cTime.GetDay() )
		{
			NetMsgFB.emFB = EMREQ_ATTEND_FB_ALREADY;
			m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );
			return S_OK;
		}	
	}

	m_dwComboAttend++;
	int nReward = -1;
	// 연속 일수 확인 및 상품 체크
	for ( DWORD i = 1; i <= GLCONST_ATTENDANCE::dwMAXREWARD; ++i )
	{
		int nIndex = GLCONST_ATTENDANCE::dwMAXREWARD-i;
		if ( m_dwComboAttend == GLCONST_ATTENDANCE::vecAttendReward[nIndex].nComboDay )
		{
			nReward = nIndex;
			break;
		}
	}

	USER_ATTEND_INFO sAttend;
	sAttend.tAttendTime = cCurTime.GetTime();
	sAttend.nComboAttend = m_dwComboAttend;
	sAttend.bAttendReward = ( nReward >= 0 ) ? 1 : 0 ;
	m_vecAttend.push_back( sAttend );

	NetMsgFB.emFB = EMREQ_ATTEND_FB_OK;
	NetMsgFB.tAttendTime = sAttend.tAttendTime;
	NetMsgFB.dwComboAttend = (DWORD)sAttend.nComboAttend;
	NetMsgFB.bAttendReward = sAttend.bAttendReward;
	m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );


	SNATIVEID sID = NATIVEID_NULL();
	// 상품넣기	
	if ( nReward >= 0 ) 
	{
		sID = GLCONST_ATTENDANCE::vecAttendReward[nReward].idReward;
		if ( sID != NATIVEID_NULL() && m_dwComboAttend == GLCONST_ATTENDANCE::vecAttendReward[nReward].nComboDay )
		{
			GLMSG::SNETPC_REQ_ATTEND_REWARD_CLT	NetMsgRewardFB;

			NetMsgRewardFB.idAttendReward = sID;
			m_pServer->SENDTOCLIENT( m_ClientSlot, &NetMsgRewardFB );	
		}
		else
		{
			sc::writeLogInfo(sc::string::format("AttendItem Error Item : %1%/%2%, Combo : %3%/%4%", sID.wMainID, sID.wSubID, m_dwComboAttend, 
														   GLCONST_ATTENDANCE::vecAttendReward[nReward].nComboDay ) );
		}
	}

	// DB 저장 로그와 상품 넣기
	int nAttendReward = sAttend.bAttendReward ? 1 : 0;
	CString strUserName = m_szUserName;
	AddGameAdoJob(
        db::DbActionPtr(
            new db::UserAttendInsert(
                m_UserDbNum,
                strUserName,
                sAttend.tAttendTime,
                m_dwComboAttend,
                nAttendReward,
                sID)));
	return S_OK;
}

HRESULT GLCharAG::MsgWhisperState( NET_MSG_GENERIC *nmg )
{
   GLMSG::SNET_GM_WHISPER_STATE *pNetMsg = (GLMSG::SNET_GM_WHISPER_STATE *)nmg;

   if ( pNetMsg->bOn )
   {
       m_bWhisperIgnore = false;
   }
   else
   {
       m_bWhisperIgnore = true;
   }

   GLMSG::SNET_GM_WHISPER_STATE_FB NetMsgFB(pNetMsg->bOn);
   m_pServer->SENDTOCLIENT ( m_ClientSlot, &NetMsgFB );

   return S_OK;
}

HRESULT GLCharAG::MsgAddRebuyItem( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_ADD_REBUY_ITEM_FA* pNetMsg = (GLMSG::SNET_ADD_REBUY_ITEM_FA*)nmg;

    if ( !m_bCheckRebuySupplies )
    {
        SITEM* pItem = GLogicData::GetInstance().GetItem( pNetMsg->sItemCustom.Id() );
        if ( !pItem )
            return E_FAIL;

        if ( pItem->BasicType() == ITEM_CURE
			|| pItem->BasicType() == ITEM_EFFECT_RELEASE_CURE
            || pItem->BasicType() == ITEM_ARROW
            || pItem->BasicType() == ITEM_CHARM
            || pItem->BasicType() == ITEM_BULLET )
            return E_FAIL;
    }

    int nRebuyListSize = static_cast<int>( m_vecRebuy.size() );

    if ( nRebuyListSize >= GLCONST_CHAR::wMaxRebuyList )
    {
        m_vecRebuy.erase( m_vecRebuy.begin() );
    }

    RebuyItem::SREBUY_ITEM sRebuyItem( pNetMsg->sItemCustom, pNetMsg->Price );
    m_vecRebuy.push_back( sRebuyItem );

    GLMSG::SNET_ADD_REBUY_ITEM_AC NetMSgFB( pNetMsg->sItemCustom, pNetMsg->Price );
    m_pServer->SENDTOCLIENT( m_ClientSlot, &NetMSgFB );

    return S_OK;
}

HRESULT GLCharAG::MsgBuyRebuyItem( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_BUY_REBUY_ITEM_CA* pPacket = (GLMSG::SNET_BUY_REBUY_ITEM_CA*) nmg;
    if (pPacket->m_MagicNum != GLMSG::SNET_BUY_REBUY_ITEM_CA::MAGIC_NUM)
    {
        sc::writeLogError(
            sc::string::format(
                "HACKING! MsgBuyRebuyItem. ChaDbNum %1% MagicNum %2%",
                m_CharDbNum,
                pPacket->m_MagicNum));
        return E_FAIL;
    }

    GLMSG::SNET_BUY_REBUY_ITEM_FC NetMsgFB( pPacket->sItemCustom, pPacket->Price );

    if ( m_vecRebuy.empty() )
    {
        NetMsgFB.emFB = EMBUYREBUYITEM_FAILED;
        m_pServer->SENDTOCLIENT( m_ClientSlot, &NetMsgFB );

        return E_FAIL;
    }

    RebuyItem::REBUY_VEC_ITER iter = m_vecRebuy.begin();

    SITEMCUSTOM& sItemCustom = pPacket->sItemCustom;
    LONGLONG& llPrice = pPacket->Price;

    for ( ; iter != m_vecRebuy.end(); ++iter)
    {
        RebuyItem::SREBUY_ITEM& sRebuyItem = (*iter);
        if (sRebuyItem.ItemCustom() == sItemCustom &&
            sRebuyItem.Price() == llPrice &&
            sRebuyItem.bGetState == false)
        {
            GLMSG::SNET_BUY_REBUY_ITEM_AF NetMsgFld(m_CharDbNum, pPacket->sItemCustom, pPacket->Price);
            m_pServer->SENDTOFIELDSVR(m_nChannel, m_dwCurFieldSvr, &NetMsgFld);
            sRebuyItem.bGetState = true;
            return S_OK;
        }
    }

    NetMsgFB.emFB = EMBUYREBUYITEM_FAILED;
    m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);
    return E_FAIL;
}

HRESULT GLCharAG::MsgBuyRebuyItemFB( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_BUY_REBUY_ITEM_FA* pNetMsg = (GLMSG::SNET_BUY_REBUY_ITEM_FA*)nmg;

    if ( pNetMsg->emFB == EMBUYREBUYITEM_SUCCESS
        || pNetMsg->emFB == EMBUYREBUYITEM_OVER_DURATION )
    {
        RebuyItem::REBUY_VEC_ITER iter = m_vecRebuy.begin();

        SITEMCUSTOM& sItemCustom = pNetMsg->sItemCustom;
        LONGLONG& llPrice = pNetMsg->Price;

        for ( ; iter != m_vecRebuy.end(); ++iter)
        {
            RebuyItem::SREBUY_ITEM& sRebuyItem = (*iter);
            if (sRebuyItem.ItemCustom() == sItemCustom &&
                sRebuyItem.Price() == llPrice)
            {
                m_vecRebuy.erase(iter);
                return S_OK;
            }
        }
    }
    else
    {
        RebuyItem::REBUY_VEC_ITER iter = m_vecRebuy.begin();

        SITEMCUSTOM& sItemCustom = pNetMsg->sItemCustom;
        LONGLONG& llPrice = pNetMsg->Price;

        for ( ; iter != m_vecRebuy.end(); ++iter)
        {
            RebuyItem::SREBUY_ITEM& sRebuyItem = (*iter);
            if (sRebuyItem.ItemCustom() == sItemCustom &&
                sRebuyItem.Price() == llPrice &&
                sRebuyItem.bGetState == true)
            {
                sRebuyItem.bGetState = false;
                return S_OK;
            }
        }
    }

    return S_OK;
}

HRESULT GLCharAG::MsgCheckRebuySupplies( NET_MSG_GENERIC* nmg )
{
    GLMSG::SNET_CHECK_REBUY_SUPPLIES* pNetMsg = (GLMSG::SNET_CHECK_REBUY_SUPPLIES*)nmg;

    m_bCheckRebuySupplies = pNetMsg->m_bCheck;

    GLMSG::SNET_CHECK_REBUY_SUPPLIES NetMsgFB( m_bCheckRebuySupplies );
    m_pServer->SENDTOCLIENT(m_ClientSlot, &NetMsgFB);

    return S_OK;
}

void GLCharAG::CarcAttendCombo()
{
	int nSize = (int)m_vecAttend.size();
	CTime cCurTime( CTime::GetCurrentTime() );	
	CTimeSpan cTimeSpan( 1, cCurTime.GetHour(), cCurTime.GetMinute(), cCurTime.GetSecond() );
	CTime cYesTime = cCurTime - cTimeSpan;

	CTime cTime;

	USER_ATTEND_INFO& sAttend = m_vecAttend[nSize-1];
	cTime = sAttend.tAttendTime;
	if ( cTime < cYesTime ) m_dwComboAttend = 0;
	else m_dwComboAttend = sAttend.nComboAttend;

    if ( GLCONST_ATTENDANCE::dwMAXREWARD > 0 ) 
	{
		int nMaxReward = GLCONST_ATTENDANCE::vecAttendReward[GLCONST_ATTENDANCE::dwMAXREWARD-1].nComboDay;
		if ( m_dwComboAttend >= (DWORD)nMaxReward ) m_dwComboAttend = 0;
	}

	return;
}

void GLCharAG::UpdateIPBonus( float fTime, float fElapsedTime )
{
	//	IP보너스
	if( m_emIPBonus != EMIP_BONUS_NONE ) 
	{		
		if( m_fCyberClassCheck >= 3600 )
		{
			m_fCyberClassCheck = 0.0f;

			if (m_pServer->GetServiceProvider() == SP_THAILAND)
			{
//#if defined(TH_PARAM)
				AddGameDbJob( db::DbActionPtr( new db::ThaiCafeClassCheck( m_ClientSlot, (int)m_emIPBonus, m_szIp )));
			}
			else if (m_pServer->GetServiceProvider() == SP_MALAYSIA || m_pServer->GetServiceProvider() == SP_MALAYSIA_EN)
			{
//#elif defined(MYE_PARAM ) || defined(MY_PARAM) 
				AddGameDbJob( db::DbActionPtr( new db::MyCafeClassCheck( m_ClientSlot, (int)m_emIPBonus, m_szIp )));
			}
			else if (m_pServer->GetServiceProvider() == SP_PHILIPPINES)
			{
//#elif defined ( PH_PARAM ) //|| defined(_RELEASED)
				AddGameDbJob( db::DbActionPtr( new db::PhCafeClassCheck( m_ClientSlot, (int)m_emIPBonus, m_szIp )));
			}
//#endif
		}
        else
        {
			m_fCyberClassCheck += fElapsedTime;
		}
	}
}

//! 캐릭터 Action Log
// 통계 및 로그 수정
void GLCharAG::LogCharAction(
    DWORD dwCI, gamelog::EMACTION emACT, 
    gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
    __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID)
{
    if (m_pServer)
        m_pServer->LogCharAction(dwCI, emACT, emTAR, dwTAR, nEXP, nBRIGHT, nLIFE, nMONEY, sMapID);
}

void GLCharAG::SENDTOCACHE(NET_MSG_GENERIC* pMsg)
{
    if (m_pServer)
        m_pServer->SENDTOCACHE(pMsg);
}

void GLCharAG::SENDCLUBLOG(club::EM_LOG_TYPE LogType, const std::string& LogMsg)
{
    if (m_pServer)
        m_pServer->SENDCLUBLOG(m_ClubDbNum, LogType, LogMsg);
}

void GLCharAG::SENDCLUBLOG(DWORD ClubDbNum, club::EM_LOG_TYPE LogType, const std::string& LogMsg)
{
    if (m_pServer)
        m_pServer->SENDCLUBLOG(ClubDbNum, LogType, LogMsg);
}

void GLCharAG::SetMapPosition(const D3DXVECTOR3& fieldPosition, bool Notify)
{
	m_vFieldPosition = fieldPosition;
    
    if (Notify)
    {
        GLMSG::NET_CHAR_POSITION_AC SendData;
        SendData.m_ChaDbNum = m_CharDbNum;
		m_pServer->ConvertWorldToMap(m_mapIDCurrent, m_vFieldPosition, SendData.m_PosX, SendData.m_PosY);

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);

        std::set<DWORD> NotifyList;
        m_pFriend->GetValidFriend(NotifyList);

        std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(m_ClubDbNum);
        if (spClub)
        {
            spClub->SetMemberPos(m_CharDbNum, m_vFieldPosition);
            spClub->GetMemberDbNumList(NotifyList);
        }

        for (std::set<DWORD>::iterator iter=NotifyList.begin(); iter!=NotifyList.end(); ++iter)
        {
            GLCharAG* pNotifyChar = m_pServer->GetCharByDbNum(*iter);
            if (pNotifyChar)
                m_pServer->SENDTOCLIENT(pNotifyChar->ClientSlot(), NET_MSG_CHAR_POSITION_AC, SendBuffer);
        }
    }
}

void GLCharAG::SetPartyID(const GLPartyID& PartyID, const bool bNotify)
{
	m_PartyID = PartyID;

/*    if (bNotify)*/
    {
        // 친구나 클럽원에게 알림
        std::set<DWORD> NotifyList;
        m_pFriend->GetValidFriend(NotifyList);
        std::tr1::shared_ptr<GLClubAgent> spClub = m_pServer->GetClub(m_ClubDbNum);
        if (spClub)
            spClub->GetMemberDbNumList(NotifyList);

        GLMSG::NET_OTHER_CHAR_PARTY_CHANGE_AC SendData(m_CharDbNum, PartyID);
        
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);

        for (std::set<DWORD>::iterator iter=NotifyList.begin(); iter!=NotifyList.end(); ++iter)
        {
            GLCharAG* pNotifyChar = m_pServer->GetCharByDbNum(*iter);
            if (pNotifyChar)
                m_pServer->SENDTOCLIENT(pNotifyChar->ClientSlot(), NET_MSG_OTHER_CHAR_PARTY_CHANGE_AC, SendBuffer);
        }
    }
}

void GLCharAG::ResetPartyID( const bool bNotify /*= false*/ )
{
    SetPartyID(GLPartyID(), bNotify );    
}

void GLCharAG::ResetPartyOption()
{
	m_sReqPartyOpt.Reset();
}

void GLCharAG::SetCurrentMap(const MapID& mapID)
{
    if ( m_PartyID.isValidParty() && m_mapIDCurrent != mapID )
    {
        // 파티원에게 맵이동을 알림
        GLMSG::SNET_PARTY_MBR_MOVEMAP NetMsgMap(m_PartyID, m_dwGaeaID, mapID);
        m_pServer->SENDTOPARTY(m_PartyID, &NetMsgMap);

        // 각각 필드 서버에 파티 멤버원의 정보 변경을 알림
        m_pServer->SENDTOALLCHANNEL(&NetMsgMap);
    }

	m_mapIDCurrent = mapID;
}

void GLCharAG::registTimerEvent(const callbackTimerFunction& _functionCallbackTimer, Timer::Parameter& _parameter, const float _fTimeInterval)
{
	_parameter.valueInteger2 = this->m_CharDbNum;

	Timer::Manager& _managerTimer(Timer::Manager::getInstance());
	const Timer::handle _handleTimer(_managerTimer.registTimer(_functionCallbackTimer, _parameter, _fTimeInterval));
	this->m_listHandleTimer.push_front(_handleTimer);
}
void GLCharAG::eraseTimerEvent(const Timer::handle _handleTimer)
{
	Timer::HandleList& _listHandleTimer(this->m_listHandleTimer);
	Timer::HandleListConstIterator _iteratorEventTimer(std::find(_listHandleTimer.begin(), _listHandleTimer.end(), _handleTimer));
	if ( _iteratorEventTimer == _listHandleTimer.end() )
		return;

	_listHandleTimer.erase(_iteratorEventTimer);
}

bool GLCharAG::IsFieldChange()
{
	if( m_mapIDNext.isValid() && m_mapIDNext != m_mapIDCurrent )
	{
		return true;
	}

	return false;
}