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

    //BOOL                m_bUsedPMarketSeacrhCard;   // ���λ��� �˻� ī�� ��뿩��

	//
	//mjeon.CaptureTheField
	//
	BOOL				m_bCTFPlayer;	//CaptureTheField�� �������ΰ�?

	DWORD				m_dwKey;		//InstanceKey

	
	DWORD				m_ClientSlot; //! ��� ���� ��ȣ
	int					m_nChannel;
	int					m_nOriginChannel;

	DWORD				m_dwLastPostGetData;	    // Time when the character got the last post data.
    DWORD				m_dwLastPostGetState;	    // Time when the character got the last post state.
    DWORD				m_dwLastPostGetSendState;	// Time when the character got the last send post state.
    DWORD				m_dwLastPostGetRecvState;	// Time when the character got the last recv post state.

	DWORD				m_dwGaeaID; //! GaeaID ������ �ο��� ID, DB ��ȣ�� Ʋ��	
	GLPartyID			m_PartyID;

	DWORD				m_dwActState;							//	���� �ൿ ����.
protected:
	MapID				m_mapIDPrevious;	// ���� ��;
	MapID				m_mapIDCurrent;		// ���� ��;
	MapID				m_mapIDNext;			// �ʵ� ���� ����� ���� ������ �� ID.

public:
	BOOL				m_bMapMoveFCard;
	DWORD				m_dwCurFieldSvr;						//	���� �ʵ� ����

	D3DXVECTOR3		m_vFieldPosition; //! Field ���������� ��ġ (��Ȯ���� ����)

	DWORD				m_dwNextGateID;							//	�ʵ� ���� ����� ���� ������ ����Ʈ ID.
	D3DXVECTOR3			m_vNextPos;								//	�ʵ� ���� ����� ���ο� ��ġ.
	DWORD				m_dwNextFieldSvr;						//	�ʵ� ���� ���� ID.
	DWORD				m_dwNextChannel;						//	ä�� ���� ID.
	BOOL				m_bIsInstance;							//  ���� InstanceServer�� �ִ°�?

	//sc::SGLNODE<GLCharAG*>*	m_pPCNode;								//	PC ����Ʈ ���.

    std::vector<SGETWHISPERLIST> m_vecGetWhisperList;			    // �ӼӸ��� ����� GM����Ʈ

	SGETWHISPERLIST			m_GmWhisperList;				    // �ڽ��� GM�� ��� ����� ĳ���� ����
	bool					m_bViewWhisperMSG;


	bool				m_bFIELDCK_OK;							//	�̹� üũ�� �Ϸ��Ͽ��°�?
	DWORD				m_dwFIELDCK_NUM;						//	�ʵ� ���� üũ ��.
	//DWORD				m_dwFIELDCK_BEING;						//	�ʵ� ������ ���� �ִ� ������.
	bool				m_bFIELD_CK[MAX_CHANNEL_NUMBER][FIELDSERVER_MAX];//	�ʵ� ���� üũ ����.

	SPartyOption			m_sReqPartyOpt;							//	��û�� ��Ƽ �ɼ�.


	CONFT_MAP			m_mapCONFT;								//	�ֱ� ��� ����.
	SCONFTING			m_sCONFTING;							//	���� ��� ����.
	SCONFTING			m_sREQCONFTING;							//	��û�� ��� ����.

	//float				m_fFRIEND_TIMER;

	bool				m_bReciveEventTime;						// �̺�Ʈ �ð��� �޾Ҵ��� �ƴ���.
	bool				m_bTracingUser;							// ���� �������� �������� �ƴ���
    bool                m_bWhisperIgnore;                       // �ӼӸ� ���� �������� �ƴ���.

	DWORD				m_dwComboAttend;						// �⼮ ���� ����.

	float				m_fCyberClassCheck;					// ���̹� ī�� üũŸ��
	float				m_fCheckValidMapTime;

	__time64_t			m_tLoginTime; //! ���� �ð�

    RebuyItem::REBUY_VEC m_vecRebuy;    // ����� ����Ʈ.
    bool m_bCheckRebuySupplies;         // NPC ����� ui���� �Ҹ�ǰ ������ üũ.

    wb::EM_COUNTRY m_Country; //! ��������
	Timer::HandleList m_listHandleTimer;

protected:
    GLAgentServer* m_pServer;
	//bool m_bFriendWindowOpen;			// ģ��â ���� ����
	//bool m_bClubWindowOpen;				// Ŭ��â ���� ����

protected:
	CRYPT_KEY m_cKey;	// ClientManager �� ���� �׸��� ������ ���� ������� �ʰ� �ְ�, �����ʿ��� ����Ϸ��� ���⿡ �߰��Ѵ�.

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
	BOOL IsInInstance()	//���� InstanceServer�� ���ӵǾ� �ִ°�?
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
    
    //! ��� ���� ��ȣ
    inline DWORD ClientSlot() const { return m_ClientSlot; }
    
    //! GaeaID ������ �ο��� ID, DB ��ȣ�� Ʋ��

    inline DWORD GaeaId() const { return m_dwGaeaID; }

    inline int GetChannel() const { return m_nChannel; }
	inline int GetOriginChannel() const { return m_nOriginChannel; }

    inline DWORD GetCurrentFieldServer() const { return m_dwCurFieldSvr; }

    //! ���� ��
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
	//	�ʵ忡 ĳ���͸� �������� �ʵ��� char ���Ἲ ���� �׽�Ʈ �Ϸ� ���� ����.
	//	( �Ϸ��Ĵ� ���ѹ��� ȣ��Ǿ���. )
	bool IsFieldCheckComplete ( bool *pFieldSvr );

	//	������ ���� ĳ�������� �˻�.
	//	(������ ���� ĳ���ʹ� �ʵ弭������ ������ ��ȿ�ؾ��Ѵ�.)
	bool IsDoneFieldCheck ();

public:
	bool IsCHATBLOCK ();

public:
	void SetCurrentField(DWORD dwChannel, DWORD dwSvr, const MapID& mapID, BOOL bIsInstance = FALSE);

	void ResetNextFieldSvr();
	void SetNextFieldSvr(const MapID&  sMID, DWORD dwGateID, D3DXVECTOR3 vPos, DWORD dwFieldSvr, DWORD dwChannel, BOOL bIsInstance = FALSE/*Instance�����ΰ�?*/);

public:
	//	�ֱ� ��� ���� ��û.
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

    //! ĳ���� Action Log
	// ��� �� �α� ����
    void LogCharAction(
        DWORD dwCI, gamelog::EMACTION emACT, 
        gamelog::EMIDTYPE emTAR, DWORD dwTAR, 
        __int64 nEXP, int nBRIGHT, int nLIFE, int nMONEY, const MapID &sMapID);

    //! ��������
    wb::EM_COUNTRY GetCountry() const { return m_Country; }

    //! Field ���������� ��ġ (��Ȯ���� ����)    
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
    //! Club master ���� ����
	HRESULT MsgClubAuthorityReq(NET_MSG_GENERIC* nmg);	
	HRESULT MsgClubAuthorityReqAns(NET_MSG_GENERIC* nmg);	
	HRESULT MsgClubMarkInfo(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubMarkChange(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubRank2Agt(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubGuidCommission(NET_MSG_GENERIC* nmg);
	//HRESULT MsgClubNoticeReq(NET_MSG_GENERIC* nmg);
	//HRESULT MsgClubSubMaster(NET_MSG_GENERIC* nmg);
    
    //! ���Ϳ��� Ŭ�� �����û ó��
	HRESULT MsgClubAllianceDelReq(NET_MSG_GENERIC* nmg);
	HRESULT MsgClubAllianceSecReq(NET_MSG_GENERIC* nmg);
    //! ���� ��ü
	HRESULT MsgClubAllianceDisReq(NET_MSG_GENERIC* nmg);

	//! Ŭ�� ��Ʋ�� ��û��
    HRESULT	MsgClubBattleReq(NET_MSG_GENERIC* nmg);
    
    //! Ŭ�� ��Ʋ��û ��� FeedBack.
	HRESULT	MsgClubBattleReqAns(NET_MSG_GENERIC* nmg);
    
    //! Ŭ�� ��Ʋ ������û Client->Agent
	HRESULT	MsgClubBattleReqAnsAlliance(NET_MSG_GENERIC* nmg);
	
    HRESULT	MsgClubBattleArmisticeReq(NET_MSG_GENERIC* nmg);
    
    //! Ŭ�� ��Ʋ ������û ���. Ŭ�� ��Ʋ ���� �亯 Client->Agent
	HRESULT	MsgClubBattleArmisticeReqAns(NET_MSG_GENERIC* nmg);
    
    //! Ŭ�� ��Ʋ �׺���û 
	HRESULT	MsgClubBattleSubmissionReq(NET_MSG_GENERIC* nmg);

    //! Ŭ�� ��Ʋ�� ��û��. ( ���� )
	HRESULT	MsgAllianceBattleReq(NET_MSG_GENERIC* nmg);
	
    HRESULT	MsgAllianceBattleReqAns(NET_MSG_GENERIC* nmg);
    
    //! Ŭ�� ��Ʋ ������û (����) Client->Agent
	HRESULT	MsgAllianceBattleArmisticeReq(NET_MSG_GENERIC* nmg);

    //! Ŭ�� ��Ʋ ������û ��� FeedBack. (����)
	HRESULT	MsgAllianceBattleArmisticeReqAns(NET_MSG_GENERIC* nmg);

    //! Ŭ�� ��Ʋ �׺���û ( ���� ) 
	HRESULT	MsgAllianceBattleSubmissionReq(NET_MSG_GENERIC* nmg);

	//! Ŭ�� ������ �̸� ��û
	HRESULT MsgClubId2MasterNameReqCA(NET_MSG_GENERIC* nmg);

	HRESULT MsgRenameBrd(NET_MSG_GENERIC* nmg);
	//HRESULT MsgPhoneNumberBrd(NET_MSG_GENERIC* nmg);
	HRESULT	MsgUpdateStartMap(NET_MSG_GENERIC* nmg);

	
	//	Memo :	��/���� ���� ����(Field->Agent)
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
    // ���� �޼���
    bool MsgReqProduction( NET_MSG_GENERIC* nmg );
    bool MsgReqProductIsLearnBook( NET_MSG_GENERIC* nmg );

    // �ű�
    bool MsgReqProductBookList( NET_MSG_GENERIC* nmg );
    bool MsgReqProductTypeList( NET_MSG_GENERIC* nmg );
    bool MsgReqProductTypeAdd( NET_MSG_GENERIC* nmg );
    bool MsgReqProductTypeDel( NET_MSG_GENERIC* nmg );
    bool MsgReqProductIsGetExpList( NET_MSG_GENERIC* nmg );

    bool MsgAckProductFA( NET_MSG_GENERIC* nmg );       // ���� ����
    bool MsgAckLearnBookFA( NET_MSG_GENERIC* nmg );     // �ʵ忡�� ���ռ� ���⸦ �����Ͽ���.
    bool MsgAckTypeDeleteFA( NET_MSG_GENERIC* nmg );    // �ʵ忡�� ĳ��(����) ������ ����� ����

public:
    //! �ʵ� ���� üũ ��
    void FieldCheckNumReset()    { m_dwFIELDCK_NUM = 0; }
    void FieldCheckNumIncrease() { ++m_dwFIELDCK_NUM; }

    //! ���� �ð�
    __time64_t GetLoginTime() const { return m_tLoginTime; }

    void GetValidFriend(std::set<DWORD>& FriendList) const;

    //! CaptureTheField�� �������ΰ�?
    BOOL IsCtfPlayer() const { return m_bCTFPlayer; }

    bool CheckSelectClubCondition();
};

typedef GLCharAG* PGLCHARAG;
typedef sc::CGLLIST<PGLCHARAG>	AGCHARLIST;
typedef sc::SGLNODE<PGLCHARAG>	AGCHARNODE;

#endif // _GL_CHAR_AG_H_

