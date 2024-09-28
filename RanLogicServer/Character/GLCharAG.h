#ifndef _GL_CHAR_AG_H_
#define _GL_CHAR_AG_H_

#pragma once

#include <vector>

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Timer/Timer.h"
#include "../../RanLogic/MatchSystem/MatchDefine.h"
#include "../../RanLogic/Country/GLCountryDefine.h"
//#include "../../RanLogic/Msg/GLContrlCharJoinMsg.h"
#include "../Msg/AgentMsg.h"
#include "../Database/DBAction/DbAction.h"

#include <boost/tr1/memory.hpp>

class GLAgentServer;

class GLCharAG : public GLCHARAG_DATA
{
protected:
	typedef std::map<DWORD, SCONFT_LOG> CONFT_MAP;
	typedef CONFT_MAP::iterator         CONFT_MAP_ITER;
    typedef CONFT_MAP::const_iterator   CONFT_MAP_CITER;
    typedef CONFT_MAP::value_type       CONFT_MAP_VALUE;

public:
    GLCharAG(GLAgentServer* pServer, wb::EM_COUNTRY Country);
    virtual ~GLCharAG();

public:
	//
	//mjeon.Post.Get
	//
	typedef std::vector<SAPOST>   VPOST;
	typedef VPOST::iterator       VPOST_ITER;
    typedef VPOST::const_iterator VPOST_CITER;

	//
	//mjeon.sns
	//
	SFACEBOOK			m_AUTHFB;
	STWITTER			m_AUTHTW;

	//VPOST				m_vPost;

	std::tr1::shared_ptr<VPOST> m_pvPost;

	BOOL				m_bPostGotData;
    BOOL				m_bPostGotState;
    BOOL				m_bPostGotRecvState;
    BOOL				m_bPostGotSendState;

    //BOOL                m_bUsedPMarketSeacrhCard;   // 개인상점 검색 카드 사용여부

	//
	//mjeon.CaptureTheField
	//
	BOOL				m_bCTFPlayer;	//CaptureTheField에 참여중인가?

	DWORD				m_dwKey;		//InstanceKey

	
	DWORD				m_ClientSlot; //! 통신 슬롯 번호
	int					m_nChannel;
	int					m_nOriginChannel;

	DWORD				m_dwLastPostGetData;	    // Time when the character got the last post data.
    DWORD				m_dwLastPostGetState;	    // Time when the character got the last post state.
    DWORD				m_dwLastPostGetSendState;	// Time when the character got the last send post state.
    DWORD				m_dwLastPostGetRecvState;	// Time when the character got the last recv post state.

	DWORD				m_dwGaeaID; //! GaeaID 서버가 부여한 ID, DB 번호와 틀림	
	GLPartyID			m_PartyID;

	DWORD				m_dwActState;							//	현재 행동 설정.
protected:
	MapID				m_mapIDPrevious;	// 이전 맵;
	MapID				m_mapIDCurrent;		// 현재 맵;
	MapID				m_mapIDNext;			// 필드 서버 변경시 새로 진입할 맵 ID.

public:
	BOOL				m_bMapMoveFCard;
	DWORD				m_dwCurFieldSvr;						//	현재 필드 서버

	D3DXVECTOR3		m_vFieldPosition; //! Field 서버에서의 위치 (정확하지 않음)

	DWORD				m_dwNextGateID;							//	필드 서버 변경시 새로 진입할 게이트 ID.
	D3DXVECTOR3			m_vNextPos;								//	필드 서버 변경시 새로운 위치.
	DWORD				m_dwNextFieldSvr;						//	필드 서버 변경 ID.
	DWORD				m_dwNextChannel;						//	채널 변경 ID.
	BOOL				m_bIsInstance;							//  현재 InstanceServer에 있는가?

	//sc::SGLNODE<GLCharAG*>*	m_pPCNode;								//	PC 리스트 노드.

    std::vector<SGETWHISPERLIST> m_vecGetWhisperList;			    // 귓속말을 엿듣는 GM리스트

	SGETWHISPERLIST			m_GmWhisperList;				    // 자신이 GM일 경우 엿듣는 캐릭터 정보
	bool					m_bViewWhisperMSG;


	bool				m_bFIELDCK_OK;							//	이미 체크를 완료하였는가?
	DWORD				m_dwFIELDCK_NUM;						//	필드 서버 체크 수.
	//DWORD				m_dwFIELDCK_BEING;						//	필드 서버에 남아 있던 잔존량.
	bool				m_bFIELD_CK[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX];//	필드 서버 체크 여부.

	SPartyOption			m_sReqPartyOpt;							//	요청한 파티 옵션.


	CONFT_MAP			m_mapCONFT;								//	최근 대련 정보.
	SCONFTING			m_sCONFTING;							//	현재 대련 정보.
	SCONFTING			m_sREQCONFTING;							//	요청한 대련 정보.

	//float				m_fFRIEND_TIMER;

	bool				m_bReciveEventTime;						// 이벤트 시간을 받았는지 아닌지.
	bool				m_bTracingUser;							// 현재 추적중인 유저인지 아닌지
    bool                m_bWhisperIgnore;                       // 귓속말 무시 상태인지 아닌지.

	DWORD				m_dwComboAttend;						// 출석 연속 일자.

	float				m_fCyberClassCheck;					// 사이버 카페 체크타임
	float				m_fCheckValidMapTime;

	__time64_t			m_tLoginTime; //! 접속 시간

    RebuyItem::REBUY_VEC m_vecRebuy;    // 재매입 리스트.
    bool m_bCheckRebuySupplies;         // NPC 재매입 ui에서 소모품 아이템 체크.

    wb::EM_COUNTRY m_Country; //! 국가정보
	Timer::HandleList m_listHandleTimer;

protected:
    GLAgentServer* m_pServer;
	//bool m_bFriendWindowOpen;			// 친구창 갱신 여부
	//bool m_bClubWindowOpen;				// 클럽창 갱신 여부

protected:
	CRYPT_KEY m_cKey;	// ClientManager 에 같은 항목이 있으나 지금 사용하지 않고 있고, 로직쪽에서 사용하려고 여기에 추가한다.

	// Country
	Country::SCOUNTRY_INFO m_sCountryInfo;

public:
	// Country
	void SetCountryInfo ( const Country::SCOUNTRY_INFO& sCountryInfo ) { m_sCountryInfo = sCountryInfo; }
	const Country::SCOUNTRY_INFO& GetCountryInfo () const { return m_sCountryInfo; }

public:
	void SetCryptKey( CRYPT_KEY cKey ) { m_cKey = cKey; }
	CRYPT_KEY GetCryptKey() { return m_cKey; }

	void registTimerEvent(const callbackTimerFunction& _functionCallbackTimer, Timer::Parameter& _parameter, const float _fTimeInterval);
	void eraseTimerEvent(const Timer::handle _handleTimer);

public:
	BOOL IsInInstance()	//현재 InstanceServer에 접속되어 있는가?
	{
		return m_bIsInstance;
	}

	BOOL IsOnMatching()
	{
		return (m_dwKey != INVALID_INSTANCE_KEY);
	}

	void  SetInstanceKey(DWORD dwKey)
	{
		GASSERT(dwKey != INVALID_INSTANCE_KEY);
		m_dwKey = dwKey;
	}

	DWORD GetInstanceKey() {return m_dwKey;}

	void ResetInstanceKey() {m_dwKey = INVALID_INSTANCE_KEY;}
	bool IsFieldChange();

protected:
	BOOL IsSTATE(DWORD dwState) const { return m_dwActState&dwState; }
	
public:
    void AddGameDbJob(db::DbActionPtr spJob);
	//mjeon.ado
	void AddGameAdoJob(db::DbActionPtr spJob);	
	void AddLogAdoJob(db::DbActionPtr spJob);
	
	void SetSTATE(DWORD dwState) { m_dwActState |= dwState; }
	void ReSetSTATE(DWORD dwState) { m_dwActState &= ~dwState; }
    
    //! 통신 슬롯 번호
    inline DWORD ClientSlot() const { return m_ClientSlot; }
    
    //! GaeaID 서버가 부여한 ID, DB 번호와 틀림

    inline DWORD GaeaId() const { return m_dwGaeaID; }

    inline int GetChannel() const { return m_nChannel; }
	inline int GetOriginChannel() const { return m_nOriginChannel; }

    inline DWORD GetCurrentFieldServer() const { return m_dwCurFieldSvr; }

    //! 현재 맵
    const MapID& GetCurrentMap(void) const { return m_mapIDCurrent; }
	const MapID& GetPreviousMap(void) const { return m_mapIDPrevious; }
	const MapID& GetNextMap(void) const { return m_mapIDNext; }
    void SetCurrentMap(const MapID& mapID); // { m_mapIDCurrent = mapID; }

    void SetMapPosition(const D3DXVECTOR3& fieldPosition, bool Notify);

public:
	void SetPartyID(const GLPartyID& PartyID, const bool bNotify = false);
	void ResetPartyID(const bool bNotify = false);	
	const GLPartyID& GetPartyID(void) const { return m_PartyID; }

	void ResetPartyOption();
	
public:
	HRESULT CreateChar(GLCHARAG_DATA* pchar_data);

public:
	//	필드에 캐릭터를 생성전에 필드의 char 무결성 점검 테스트 완료 여부 점검.
	//	( 완료후는 단한번만 호출되야함. )
	bool IsFieldCheckComplete ( bool *pFieldSvr );

	//	점검이 끝난 캐릭터인지 검사.
	//	(점검이 끝난 캐릭터는 필드서버와의 연결이 유효해야한다.)
	bool IsDoneFieldCheck ();

public:
	bool IsCHATBLOCK ();

public:
	void SetCurrentField(DWORD dwChannel, DWORD dwSvr, const MapID& mapID, BOOL bIsInstance = FALSE);

	void ResetNextFieldSvr();
	void SetNextFieldSvr(const MapID&  sMID, DWORD dwGateID, D3DXVECTOR3 vPos, DWORD dwFieldSvr, DWORD dwChannel, BOOL bIsInstance = FALSE/*Instance서버인가?*/);

public:
	//	최근 대련 정보 요청.
	const SCONFT_LOG* GETCONFRONT_LOG(DWORD dwCHARID);
	const SCONFTING& GETCONFRONTING() const { return m_sCONFTING; }

	void SaveLogConfront ();

	void StartConfront ( EMCONFT_TYPE emType, DWORD dwTAR_ID, const SCONFT_OPTION &sOption );
	void ResetConfront ( EMCONFRONT_END emEND );
	void ResetConfrontOk ();

public:
	void AddFriendAndGroup(const char* szFriend);    
	bool DelFriend(char* szFriend);
    void DelFriend2(char* szFriend);
    size_t GetFriendSize() const;
	void BlockFriend(const char* szFriend);
	void SetStateFriend(const char *szFriend, DWORD dwFlag);
	EMFRIEND_FLAGS GetStateFriend(char *szFriend);
    EMFRIEND_FLAGS GetStateFriend(DWORD FriendDbNum);
    bool IsFriend(const std::string& ChaName);
    bool IsBlock( const std::string& ChaName );
    std::tr1::shared_ptr<SFRIEND> GetFriend(const char* szName);
    //void SendMyLevelToFriend();
    
    void Online();
    void Offline();

	void CarcAttendCombo();

    //! 캐릭터 Action Log
	// 통계 및 로그 수정
    void LogCharAction(
        DWORD dwCI, gamelog::EMACTION emACT, 
        gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
        __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID);

    //! 국가정보
    wb::EM_COUNTRY GetCountry() const { return m_Country; }

    //! Field 서버에서의 위치 (정확하지 않음)    
	const D3DXVECTOR3& GetFieldMapPosition(void) const { return m_vFieldPosition; }

public:
	HRESULT MsgActState(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqFriendList(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqFriendAdd(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqFriendAddAns(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqFriendDel(NET_MSG_GENERIC* nmg);
	HRESULT MsgReqFriendBlock(NET_MSG_GENERIC* nmg);

	//HRESULT MsgClubMemberReqAgt(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubMemberDel(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubMemberSecede(NET_MSG_GENERIC* nmg);
    //! Club master 권한 위임
	HRESULT MsgClubAuthorityReq(NET_MSG_GENERIC* nmg);	
	HRESULT MsgClubAuthorityReqAns(NET_MSG_GENERIC* nmg);	
	HRESULT MsgClubMarkInfo(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubMarkChange(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubRank2Agt(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubGuidCommission(NET_MSG_GENERIC* nmg);
	//HRESULT MsgClubNoticeReq(NET_MSG_GENERIC* nmg);
	//HRESULT MsgClubSubMaster(NET_MSG_GENERIC* nmg);
    
    //! 동맹에서 클럽 제명요청 처리
	HRESULT MsgClubAllianceDelReq(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubAllianceSecReq(NET_MSG_GENERIC* nmg);
    //! 동맹 해체
	HRESULT MsgClubAllianceDisReq(NET_MSG_GENERIC* nmg);

	//! 클럽 배틀을 요청함
    HRESULT	MsgClubBattleReq(NET_MSG_GENERIC* nmg);
    
    //! 클럽 배틀요청 결과 FeedBack.
	HRESULT	MsgClubBattleReqAns(NET_MSG_GENERIC* nmg);
    
    //! 클럽 배틀 휴전요청 Client->Agent
	HRESULT	MsgClubBattleReqAnsAlliance(NET_MSG_GENERIC* nmg);
	
    HRESULT	MsgClubBattleArmisticeReq(NET_MSG_GENERIC* nmg);
    
    //! 클럽 배틀 휴전요청 결과. 클럽 배틀 휴전 답변 Client->Agent
	HRESULT	MsgClubBattleArmisticeReqAns(NET_MSG_GENERIC* nmg);
    
    //! 클럽 배틀 항복요청 
	HRESULT	MsgClubBattleSubmissionReq(NET_MSG_GENERIC* nmg);

    //! 클럽 배틀을 요청함. ( 동맹 )
	HRESULT	MsgAllianceBattleReq(NET_MSG_GENERIC* nmg);
	
    HRESULT	MsgAllianceBattleReqAns(NET_MSG_GENERIC* nmg);
    
    //! 클럽 배틀 휴전요청 (동맹) Client->Agent
	HRESULT	MsgAllianceBattleArmisticeReq(NET_MSG_GENERIC* nmg);

    //! 클럽 배틀 휴전요청 결과 FeedBack. (동맹)
	HRESULT	MsgAllianceBattleArmisticeReqAns(NET_MSG_GENERIC* nmg);

    //! 클럽 배틀 항복요청 ( 동맹 ) 
	HRESULT	MsgAllianceBattleSubmissionReq(NET_MSG_GENERIC* nmg);

	//! 클럽 마스터 이름 요청
	HRESULT MsgClubId2MasterNameReqCA(NET_MSG_GENERIC* nmg);

	HRESULT MsgRenameBrd(NET_MSG_GENERIC* nmg);
	//HRESULT MsgPhoneNumberBrd(NET_MSG_GENERIC* nmg);
	HRESULT	MsgUpdateStartMap(NET_MSG_GENERIC* nmg);

	
	//	Memo :	주/보조 무기 변경(Field->Agent)
	HRESULT MsgChangeArm( NET_MSG_GENERIC* nmg );

	//HRESULT MsgFriendClubWindowOpen( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqQboxOption( NET_MSG_GENERIC* nmg );

	HRESULT	MsgReqAttendList( NET_MSG_GENERIC* nmg );

	HRESULT MsgReqAttendance( NET_MSG_GENERIC* nmg );

    HRESULT MsgWhisperState( NET_MSG_GENERIC *nmg );

    // Rebuy Item
    HRESULT MsgAddRebuyItem( NET_MSG_GENERIC* nmg );
    HRESULT MsgBuyRebuyItem( NET_MSG_GENERIC* nmg );
    HRESULT MsgBuyRebuyItemFB( NET_MSG_GENERIC* nmg );
    HRESULT MsgCheckRebuySupplies( NET_MSG_GENERIC* nmg );

public:
	//HRESULT MsgProcess(NET_MSG_GENERIC* nmg);
	HRESULT FrameMove(float fTime, float fElapsedTime);
	void UpdateIPBonus(float fTime, float fElapsedTime);

    void SENDTOCACHE(NET_MSG_GENERIC* pMsg);
    void SENDCLUBLOG(club::EM_LOG_TYPE LogType, const std::string& LogMsg);
    void SENDCLUBLOG(DWORD ClubDbNum, club::EM_LOG_TYPE LogType, const std::string& LogMsg);

    bool AddFriendGroup(const std::string& GroupName);
    bool DelFriendGroup(WORD GroupNum);
    bool SetFriendGroup(DWORD FriendDbNum, WORD GroupNum);
    bool FriendGroupRename(WORD GroupNum, const std::string& NewName);
    void MsgFriendGroupReq();

public:
    //! Notify ----------------------------------------------------------------
    void NotifyReqCA();

public:
    //! Product ----------------------------------------------------------------
    // 기존 메세지
    bool MsgReqProduction( NET_MSG_GENERIC* nmg );
    bool MsgReqProductIsLearnBook( NET_MSG_GENERIC* nmg );

    // 신규
    bool MsgReqProductBookList( NET_MSG_GENERIC* nmg );
    bool MsgReqProductTypeList( NET_MSG_GENERIC* nmg );
    bool MsgReqProductTypeAdd( NET_MSG_GENERIC* nmg );
    bool MsgReqProductTypeDel( NET_MSG_GENERIC* nmg );
    bool MsgReqProductIsGetExpList( NET_MSG_GENERIC* nmg );

    bool MsgAckProductFA( NET_MSG_GENERIC* nmg );       // 조합 성공
    bool MsgAckLearnBookFA( NET_MSG_GENERIC* nmg );     // 필드에서 조합서 배우기를 성공하였다.
    bool MsgAckTypeDeleteFA( NET_MSG_GENERIC* nmg );    // 필드에서 캐쉬(리셋) 아이템 사용을 성공

public:
    //! 필드 서버 체크 수
    void FieldCheckNumReset()    { m_dwFIELDCK_NUM = 0; }
    void FieldCheckNumIncrease() { ++m_dwFIELDCK_NUM; }

    //! 접속 시간
    __time64_t GetLoginTime() const { return m_tLoginTime; }

    void GetValidFriend(std::set<DWORD>& FriendList) const;

    //! CaptureTheField에 참여중인가?
    BOOL IsCtfPlayer() const { return m_bCTFPlayer; }

    bool CheckSelectClubCondition();
};

typedef GLCharAG* PGLCHARAG;
typedef sc::CGLLIST<PGLCHARAG>	AGCHARLIST;
typedef sc::SGLNODE<PGLCHARAG>	AGCHARNODE;

#endif // _GL_CHAR_AG_H_

