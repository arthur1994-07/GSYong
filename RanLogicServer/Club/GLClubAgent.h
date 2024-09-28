#pragma once

#include <boost/logic/tribool.hpp>
#include "../../RanLogic/Club/GLClub.h"

class GLAgentServer;
class GLCharAG;

class GLClubAgent : public GLClub
{
public:
    GLClubAgent(GLAgentServer* pServer);
    virtual ~GLClubAgent();

    const GLClubAgent& operator = (const GLClubAgent& value);
    const GLClubAgent& operator = (const GLClub& value);
    //virtual void SetInfo(const GLCLUBINFO& sINFO) override;

private:
    GLAgentServer* m_pServer;
    GLInventoryStorage m_cStorage[MAX_CLUBSTORAGE]; //! 클럽 창고 인벤토리
    //LONGLONG m_lnStorageMoney; //! 클럽 창고 금액
    boost::logic::tribool m_bStorageLoadFromDb; //! 클럽 창고가 DB 에서 부텅 로딩되었는가?
    //LONGLONG m_lnIncomeMoney; //! 클럽 수익 금액
    bool m_bStorageUpdate; //! 클럽의 Storage 정보가 업데이트 되었는가?. 업데이트 되었을때만 DB 에 저장된다.

    DWORD m_ChangeClubMarkTime; //! 클럽 마크 변경한 시간.

	DWORD m_CurrentClubStorageUser;	// 현재 클럽창고 사용중인 캐릭터번호, default GAEAID_NULL
	__time64_t m_LastClubStorageUseTime;	// 클럽창고 마지막 사용시간
	DWORD m_CurrentClubStorageMoneyUser;	// 현재 클럽창고 게임머니 사용중인 캐릭터번호, default GAEAID_NULL
	__time64_t m_LastClubStorageMoneyUseTime;	// 클럽창고 게임머니 마지막 사용시간

public:
    bool SetAuthorityMaster(DWORD dwCharID);
    void SetLastBattlePoint(DWORD dwClubID, WORD dwKillPoint, WORD dwDeathPoint);
    void SendClubClient(NET_MSG_GENERIC* pMsg);
    void SendClubClient(EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);

    //! 클럽 창고 금액
    LONGLONG GetStorageMoney() const;
    void SetStorageMoney(LONGLONG Money);
    void AddStorageMoney(LONGLONG Money);
    void SubtractStorageMoney(LONGLONG Money);

    BOOL SETSTORAGE_BYBUF(se::ByteStream& ByteStream);
	BOOL SETSTORAGE_BYBUF_FOR_MIGRATION(se::ByteStream& ByteStream);
    BOOL GETSTORAGE_BYBUF(se::ByteStream& ByteStream) const;

	BOOL SETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems );
	BOOL GETSTORAGE_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItems );

    virtual void RESET() override;
    void RESET_STORAGE();

    //! 클럽 수익 금액
    LONGLONG GetIncomeMoney() const;
    void SetIncomeMoney(LONGLONG Money);
    void AddIncomeMoney(LONGLONG AddMoney);

    //! 클럽 마크 변경 시간.
    DWORD GetChangeClubMarkTime();
    void SetChangeClubMarkTime( DWORD CurTime );

public:
    virtual bool AddAuth(size_t Index, const club::AUTH_GRADE& Grade) override;    
    virtual bool ChangeAuth(size_t Index, club::AUTH_GRADE& Grade) override;
    virtual bool ChangeAuth(size_t IndexA, size_t IndexB) override;
    bool AddAuth(const club::AUTH_GRADE_DB& Grade);

    //! Club member 계급 변경
    virtual bool ChangeMemberGrade(DWORD MemberDbNum, size_t Grade) override;
    virtual bool ChangeGradeName(size_t GradeIndex, const std::string& GradeName, DWORD ReqChaDbNum) override;

public:
    BOOL InsertItem(const SITEMCUSTOM& Item);
    BOOL InsertItem(const SITEMCUSTOM& Item, DWORD Channel, WORD PosX, WORD PosY, bool bSendClient = true);
	BOOL InsertItem( SINVENITEM_SAVE& sItem );
    bool DeleteItem(DWORD Channel, WORD PosX, WORD PosY);
    const SINVENITEM* GetItem(DWORD Channel, WORD PosX, WORD PosY);
    BOOL IsInsertable(DWORD Channel, WORD PosX, WORD PosY);
    //BOOL SplitItem(DWORD Channel, WORD PosX, WORD PosY, WORD Split);

	void ClubStorageUpdateItem( DWORD Channel, WORD PosX, WORD PosY, bool JustInvenTypeUpdate = false );
	void ClubStorageUpdateItem( DWORD Channel, SINVENITEM* pInvenItem, bool JustInvenTypeUpdate = false );

    void SaveStorageToDB();

    boost::logic::tribool IsValidStorage() const { return m_bStorageLoadFromDb; }
    void SetValidStorage(boost::logic::tribool Valid) { m_bStorageLoadFromDb = Valid; }

    void SendClubStorage(DWORD ClientSlot);

    bool IsStorageUpdate() const { return m_bStorageUpdate; }

    bool SetGradeFlag(GLCharAG* pChar, size_t Index, DWORD Flag);
    bool SetGradeFlag(GLCharAG* pChar, const std::vector<club::GRADE_FLAG>& Data);

    //! 공개 범위 설정
    using GLClub::PublicSet;
    bool PublicSet(DWORD Flag, GLCharAG* pChar);

    //! 자동 member kick out
    void AutoKickOut();

    bool DelMember(DWORD dwCharID, DWORD ActionChaDbNum, const std::string& ActionChaName, bool bIsKick = false);

    using GLClub::AutoKickOutSet;
    bool AutoKickOutSet(const club::AUTO_KICK_OUT& AutoKickOut, GLCharAG* pChar);

    //! 신입생을 위한 소개말 
    using GLClub::SetNoticeNewbie;
    void SetNoticeNewbie(const std::string& Notice, GLCharAG* pChar);

    virtual void SetMemberState(DWORD ChaDbNum, const MapID& mapId) override;
    virtual void SetMemberState(DWORD dwCharID, bool bONLINE, int nCHANNEL, const MapID& nidMAP) override;
    virtual void SetMemberState(DWORD ChaDbNum, WORD ChaLevel) override;
    virtual void SetMemberPos(DWORD dwCharID, const D3DXVECTOR3& vecPOS) override;

    void SendClubInfoToAllMember();

    void SendClubInfo(DWORD ClientSlot);
    void SendClubInfoBase(DWORD ClientSlot);
    void SendClubInfoMember(DWORD ClientSlot);
    void SendClubInfoAlliance(DWORD ClientSlot);
    void SendClubInfoBattle(DWORD ClientSlot);
    void SendClubInfoAuth(DWORD ClientSlot);

	DWORD GetClubStorageUser() { return m_CurrentClubStorageUser; }
	__time64_t GetLastClubStorageUseTime() { return m_LastClubStorageUseTime; }
	bool ReqClubStorageUse( DWORD ChaDbNum );	// 클럽창고 사용 요청(아이템) true 면 사용가능, 아니면 누군가 사용중
	void ResetClubStorageUser();
	std::string GetClubStorageUserName();

	DWORD GetClubStorageMoneyUser() { return m_CurrentClubStorageMoneyUser; }
	__time64_t GetLastClubStorageMoneyUseTime() { return m_LastClubStorageMoneyUseTime; }
	bool ReqClubStorageMoneyUse( DWORD ChaDbNum );	// 클럽창고게임머니 사용 요청 true 면 사용가능, 아니면 누군가 사용중
	void ResetClubStorageMoneyUser();
	std::string GetClubStorageMoneyUserName();

public:
	void CopyStorage( DWORD Channel, GLInventoryStorage& Target );
	void PasteStorage( DWORD Channel, GLInventoryStorage& Source );

	SINVENITEM* InsertItemNew( const SITEMCUSTOM& Item, DWORD Channel, WORD PosX, WORD PosY, bool bSendClient = true );

public:
	BOOL ValidateVehicleRandomOption();
	BOOL ValidateWrappedItem( std::vector< SITEMCUSTOM >& vecPostItems, std::vector< SINVENITEM* >& vecUnWrappedItems );
	BOOL ValidateRandomOption( std::vector< SITEMCUSTOM >& vecDeleteItems );

};

class AutoResetClubStorageUser
{
public:
	AutoResetClubStorageUser( std::tr1::shared_ptr< GLClubAgent > pClub )
	{
		m_pClub = pClub;
	}
	~AutoResetClubStorageUser()
	{
		if ( m_pClub )
		{
			m_pClub->ResetClubStorageUser();
		}
	}
protected:
	std::tr1::shared_ptr< GLClubAgent > m_pClub;
};

class AutoResetClubStorageMoneyUser
{
public:
	AutoResetClubStorageMoneyUser( std::tr1::shared_ptr< GLClubAgent > pClub )
	{
		m_pClub = pClub;
	}
	~AutoResetClubStorageMoneyUser()
	{
		if ( m_pClub )
		{
			m_pClub->ResetClubStorageMoneyUser();
		}
	}
protected:
	std::tr1::shared_ptr< GLClubAgent > m_pClub;
};