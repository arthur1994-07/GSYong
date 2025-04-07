#ifndef _GL_CLUB_H_
#define _GL_CLUB_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include "../GLogicData.h"
#include "../Inventory/GLInventory.h"
#include "./GLClubDefine.h"


namespace wb
{
    enum EM_COUNTRY;
}

namespace club
{
    class AuthGrade;
}


class GLClub
{
public:
    //! 구조를 변경시키면 VERSION 을 올리십시오.
    //! VERSION 을 올리시면 compile 에러가 나는곳이 수정해야 할 부분입니다.
    //! 0x0002 m_Country m_MasterLevel 추가
    enum { VERSION=0x0007, };

    GLClub();
    virtual ~GLClub();

    GLClub(const GLClub& value)
    {
        operator=(value);
    }

public:
    DWORD			m_DbNum; //! Club DB 번호
    wb::EM_COUNTRY  m_Country; //! 국가. 2012-02-13 jgkim
    char			m_szName[CLUB_NAME_LENGTH]; //! Club 이름

    DWORD			m_MasterCharDbNum; //! Club 마스터 Char DB 번호
    char			m_szMasterName[CHAR_SZNAME]; //! Club 마스터의 char name
    int             m_MasterLevel; //! Club 마스터 Level 2012-02-13 jgkim
    DWORD			m_dwCDCertifior; //! 선도 Club 인증 가능 Club 멤버.
    
    DWORD			m_dwRank; //! Club 등급

    DWORD			m_dwMarkVER;								//!	Club 마크 버전
    DWORD			m_aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];	//!	Club 마크 이미지 RGB.

    __time64_t		m_tOrganize;								//!	Club 창설 시간.
    __time64_t		m_tDissolution;								//!	Club 해체 예정 시간.
    __time64_t		m_tAuthority;								//!	Club마스터 위임 제한 시간

    __time64_t		m_tAllianceSec;								//!	동맹 탈퇴 혹은 제명 시간 + ( 가입 제한시간 )
    __time64_t		m_tAllianceDis;								//!	동맹 해산 시간 + ( 창설 제한 시간 )

    char			m_szNotice[EMCLUB_NOTICE_LEN];	//! Club 공지
    char 		    m_szNoticeChaName[CHAR_SZNAME]; //! 공지작성 캐릭터 이름
    __time64_t	    m_NoticeDate; //! 공지작성 날짜/시간

    char            m_szNoticeNewbie[EMCLUB_NOTICE_LEN]; //! 신입생을 위한 소개말

protected:
    CLUBMEMBERS		m_mapMembers;								//! Club원.
    club::AuthGrade* m_pGrade;

public:
    CLUB_CDM_MEM	m_listCDMMembers;							//! CDM Club원.

    DWORD			m_dwAlliance;								//! 동맹 Club장.
    CLUB_ALLIANCE	m_setAlliance;								//! 동맹 Club들.

    CLUBCONFT		m_setConftSTART;							//! Club 대련 시작시 맴버.
    CLUBCONFT		m_setConftCURRENT;							//! Club 대련 진행 맴버.
    SCONFT_OPTION	m_conftOPT;									//! Club 대련 옵션.
    DWORD			m_dwconftCLUB;								//! Club 대련 상대 Club.
    float			m_fconftTIMER;								//! Club 대련 경과 시간.

    float			m_fTIMER_BATTLE;
        
    DWORD			m_dwBattleWin;								//! 배틀 포인트 ( 승 )
    DWORD			m_dwBattleLose;								//! 배틀 포인트 ( 패 )
    DWORD			m_dwBattleDraw;								//! 배틀 포인트 ( 무)
    __time64_t		m_tLastBattle;								//! 마지막 배틀 종료시간

    DWORD			m_dwAllianceBattleWin;						//! 동맹배틀 포인트 ( 승 )
    DWORD			m_dwAllianceBattleLose;						//! 동맹배틀 포인트 ( 패 )
    DWORD			m_dwAllianceBattleDraw;						//! 동맹배틀 포인트 ( 무)

    CLUB_BATTLE		m_mapBattleReady;							//! 대기중인 배틀정보
    CLUB_BATTLE		m_mapBattle;								//! 배틀 정보
    CLUB_BATTLE_DEL	m_mapBattleDel;								//! 삭제 예정인 배틀 정보

    DWORD			m_dwBattleTime;								//! 배틀 Club 진행시간. (temp)
    DWORD			m_dwBattleArmisticeReq;						//! 배틀 휴전 요청한 Club ID ( temp )

    DWORD			m_dwReqClubID;								//! 요청 Club ID

protected:
    //! 공개/비공개 설정
    DWORD m_PublicType; //! 클럽 공개 타입 EM_PUBLIC_TYPE 참고

    //! 수익금액. 주의! Field Server 에서는 사용하지 마시오
    LONGLONG m_lnIncomeMoney;  

    //! 창고금액. 주의! Field Server 에서는 사용하지 마시오
    LONGLONG m_lnStorageMoney; 

    club::AUTO_KICK_OUT m_AutoKickOut; //! 자동 추방

public:
    const GLClub& operator = (const GLClub& rhs);
    //virtual void SetInfo(const GLCLUBINFO& sINFO);
    BOOL GETSTORAGE_BYBUF(se::ByteStream& ByteStream) const;

public:
    //! Club DB 번호
    inline DWORD DbNum() const { return m_DbNum; }
    inline bool IsValid() const { return m_DbNum != CLUB_NULL ? true : false; }
    inline void SetDbNum(DWORD DbNum) { m_DbNum=DbNum; }

    //! 국가
    inline wb::EM_COUNTRY GetCountry() const { return m_Country; }
    void SetCountry(wb::EM_COUNTRY Country) { m_Country=Country; }
    
    //! Club 이름
    inline const char* Name() const { return m_szName; }
    inline void SetName(const std::string& Name)
    {
        SetName(Name.c_str());
    }

    inline void SetName(const char* szName)
    {
        if (szName)
            StringCchCopy(m_szName, CLUB_NAME_LENGTH, szName);
    }

    //! Club 마스터 Char DB 번호
    inline DWORD MasterCharDbNum() const { return m_MasterCharDbNum; }
    inline void SetMasterCharDbNum(DWORD DbNum) { m_MasterCharDbNum=DbNum; }

    bool DoDissolution(__time64_t tDISSOLUTION);
    bool DoDissolutionCancel();

public:
    bool IsRegDissolution();
    bool CheckDissolution();
	bool GetDissolution( CTime &tRemaining );

public:
    virtual bool AddMember(
        DWORD CharDbNum,
        const char* szCharName,
        DWORD dwFlags,
        WORD ChaLevel,
        const __time64_t& LastPlayTime,
        bool bOnline,
        EMCHARCLASS ChaClass);
    virtual bool AddMemberEx(DWORD ChaDbNum, const GLCLUBMEMBER& Member);
    virtual bool DelMember(DWORD dwCharID);

public:
    virtual bool AddAlliance(DWORD dwAlliance, const char* szClubName, const char* szMasterName, DWORD MarkVersion);
    bool DelAlliance(DWORD dwAlliance);
    bool ClearAlliance();
	bool UpdateAllianceClubName( DWORD dwAlliance, const char* szClubName );

    void SetAuthority();
    
    //! 동맹 해산 시간 + ( 창설 제한 시간 )
    void SetAllianceDis();    
    void SetAllianceDis(const __time64_t& Time) { m_tAllianceDis=Time; }
    __time64_t GetAllianceDisTime() const { return m_tAllianceDis; }    
    
    //!	동맹 탈퇴 혹은 제명 시간 + ( 가입 제한시간 )
    void SetAllianceSec();
    void SetAllianceSec(const __time64_t& Time) { m_tAllianceSec=Time; }
    __time64_t GetAllianceSec() const { return m_tAllianceSec; }
    
public:
    GLCLUBMEMBER* GetMember(DWORD dwCharID);
    const GLCLUBMEMBER* GetMember(DWORD dwCharID) const;
    virtual void SetMember(const GLCLUBMEMBER& Member);
    std::string GetMemberName(DWORD ChaDbNum) const;

public:
	virtual void SetMemberName(DWORD ChaDbNum, std::string& chaName);
    virtual void SetMemberState(DWORD dwCharID, bool bONLINE, int nCHANNEL, const MapID& mapID);
    virtual void SetMemberState(DWORD ChaDbNum, const MapID& mapID);
    virtual void SetMemberState(DWORD ChaDbNum, WORD ChaLevel);
    virtual void SetMemberPos(DWORD dwCharID, const D3DXVECTOR3& vecPOS);
    virtual bool SetMemberOffline(DWORD ChaDbNum);

public:
    //virtual bool SetMemberFlag(DWORD dwCharID, DWORD dwFlags);
    DWORD GetMemberFlag(DWORD dwCharID);
    bool SetMemberCDMFlag(DWORD dwCharID, BOOL bSet);
    
public:
    bool IsMaster(DWORD dwCharID) const;
    bool IsSubMaster(DWORD dwCharID) const;

    bool IsAlliance() const;
    bool IsChief() const;

    //	같은 Club이거나 동맹인지 체크하는 함수
    //	클라이언트는 두개의 함수를 사용가능
    //	서버의 경우는 동맹리스트를 마스터 Club만 가지고 있으므로 
    //	IsAllianceGuild_CHIEF() 해당 함수는 마스터 Club만 사용가능하다.
    //	Common함수는 동맹ID( AllianceID )를 가지고 체크하므로 아무데서나 사용가능하다.
    bool IsAllianceGuild_CHIEF ( DWORD dwGuildID );
    bool IsAllianceGuild_COMMON ( DWORD dwClubID, DWORD dwAllianceID );

    bool IsAuthority();
    //! 동맹 결성 제약시간 유무 체크
    bool IsAllianceDis();
    //! 동맹 가입 제약시간 유무 체크
    bool IsAllianceSec();

public:
    //bool IsMemberFlgJoin(DWORD dwCharID);
    //bool IsMemberFlgKick ( DWORD dwCharID );
    bool IsMemberFlgNotice(DWORD dwCharID);
    bool IsMemberFlgCDCertify(DWORD dwCharID);
    bool IsMemberFlgMarkChange(DWORD dwCharID);
    bool IsMemberFlgCDM(DWORD dwCharID);

public:
    bool EnableCDMFlag(DWORD dwCharID);
    bool IsCDMEnter(DWORD dwCharID); //! 입장 가능한지

public:
    bool IsMember(DWORD dwCharID) const;
    DWORD GetMemberNum() const { return (DWORD) m_mapMembers.size(); }
    DWORD GetAllianceNum() const { return (DWORD) m_setAlliance.size(); }
    //! 동맹 Club들
	const DWORD GetChiefID() const { return m_dwAlliance; }
    const CLUB_ALLIANCE& GetAlliance() const { return m_setAlliance; }
    void GetAllianceData(std::vector<GLCLUBALLIANCE>& vData) const;
    CLUBMEMBERS& GetMembers() { return m_mapMembers; }
    void GetMemberDbNumList(std::set<DWORD>& Member) const;

public:
    void CLEARCONFT ();
    void SETCONFT_MEMBER ();
    DWORD GETCONFT_NUM ();

    void ADDCONFT_MEMBER ( DWORD dwID );
    void DELCONFT_MEMBER ( DWORD dwID );
    bool ISCONFT_MEMBER ( DWORD dwID );

    bool ISCONFTING ();
    bool ISCONFT_LOSS ();

public:
    virtual void RESET();
    
public:
    void ADDBATTLECLUB(const GLCLUBBATTLE& sBattleClub);
    void DELBATTLECLUB( DWORD dwClubID );
    void ADDBATTLE_READY_CLUB( const GLCLUBBATTLE& sBattleClub );
    void DELBATTLE_READY_CLUB( DWORD dwClubID );

    bool IsBattle(DWORD dwClubID);
    bool IsBattleAlliance(DWORD dwClubID);
    bool IsBattleReady(DWORD dwClubID);
    bool IsBattleStop(DWORD dwClubID);
    bool IsGuidBattle();

    void KillBattleClubMem(DWORD dwClubID, WORD nKill = 1);
    void DeathBattleClubMem(DWORD dwClubID, WORD nDeath = 1);

    DWORD GetBattleNum() const { return (DWORD)m_mapBattle.size(); }
    DWORD GetBattleReadyNum() const { return (DWORD)m_mapBattleReady.size(); }
    DWORD GetAllBattleNum() const { return GetBattleNum() + GetBattleReadyNum(); }
    GLCLUBBATTLE* GetClubBattle(DWORD dwClubID);
    GLCLUBBATTLE* GetClubBattleReady(DWORD dwClubID);
    GLCLUBBATTLE_DEL* GetClubBattleDel(DWORD dwClubID);

    DWORD GetBattleDelNum() const { return (DWORD)m_mapBattleDel.size(); }    
    void ReSetAllianceBattle();

    //! Club 공지
    void SetNotice(const std::string& Notice)
    {
        SetNotice(Notice.c_str());
    }

    void SetNotice(const char* Notice)
    {
        if (Notice)
            StringCchCopy(m_szNotice, EMCLUB_NOTICE_LEN, Notice);
    }

    const char* GetNotice() const { return m_szNotice; }

    void SetNoticeChaName(const std::string& NoticeChaName)
    {
        SetNoticeChaName(NoticeChaName.c_str());
    }

    void SetNoticeChaName(const char* szNoticeChaName)
    {
        if (szNoticeChaName)
            StringCchCopy(m_szNoticeChaName, CHAR_SZNAME, szNoticeChaName);
    }

    const char* GetNoticeChaName() const { return m_szNoticeChaName; }

    __time64_t GetNoticeDate() const { return m_NoticeDate; }

    void SetNoticeDate(__time64_t NoticeDate)
    {
        m_NoticeDate = NoticeDate;
    }

    //! 신입생을 위한 소개말
    const char* GetNoticeNewbie() const { return m_szNoticeNewbie; }

    //! 신입생을 위한 소개말
    void SetNoticeNewbie(const char* szMsg)
    {
        if (szMsg)
            StringCchCopy(m_szNoticeNewbie, EMCLUB_NOTICE_LEN, szMsg);
    }

    void SetNoticeNewbie(const std::string& Msg)
    {
        SetNoticeNewbie(Msg.c_str());
    }

    //! Club 마스터 Level 2012-02-13 jgkim
    inline void SetMasterCharLevel(WORD ChaLevel)
    {
        m_MasterLevel = ChaLevel;
    }

    inline int GetMasterLevel() const { return m_MasterLevel; }

    //! Club 마스터의 char name
    inline const char* GetMasterName() const { return m_szMasterName; }
    void SetMasterName(const char* szName)
    {
        if (szName)
            StringCchCopy(m_szMasterName, CHAR_SZNAME, szName);
    }
    
    void SetMasterName(const std::string& ChaName)
    {
        StringCchCopy(m_szMasterName, CHAR_SZNAME, ChaName.c_str());
    }

    //! Club 등급
    inline DWORD GetRank() const { return m_dwRank; }

    //! Club 창설 시간
    inline __time64_t GetOrganizeDate() const { return m_tOrganize; }

    //! Club 권한
protected:
    bool CheckAuth(DWORD ChaDbNum, club::AUTHORITY Auth) const;

public:
    virtual bool AddAuth(size_t Index, const club::AUTH_GRADE& Grade);
    virtual bool ChangeAuth(size_t Index, club::AUTH_GRADE& Grade);
    virtual bool ChangeAuth(size_t IndexA, size_t IndexB);

    club::AUTH_GRADE GetAuth(size_t Index) const;
    
    //! Club 계급 갯수
    WORD GetGradeSize() const;

    //! Club 계급 설명 문자열
    std::string GetGradeName(size_t Index) const;
    bool HaveSameGradeName(const std::string& GradeName) const;
    virtual bool ChangeGradeName(size_t GradeIndex, const std::string& GradeName, DWORD ReqChaDbNum);

    bool HaveMemberJoinAuthority(DWORD ChaDbNum) const;
    bool HaveMemberKickAuthority(DWORD ChaDbNum) const;
    bool HaveNoticeChangeAuthority(DWORD ChaDbNum) const;
    bool HaveMarkChangeAuthority(DWORD ChaDbNum) const;
    bool HaveDeathMatchAuthority(DWORD ChaDbNum) const;
    //! 클럽 권한 변경이 가능한가?
    bool HaveAuthChangeAuthority(DWORD ChaDbNum) const;
    bool HaveRankUpAuthority(DWORD ChaDbNum) const;
    bool HaveStorageAuthority(DWORD ChaDbNum) const;
    bool HaveMoneyAuthority(DWORD ChaDbNum) const;
    bool HaveAllianceAuthority(DWORD ChaDbNum) const;
    bool HaveAllianceDisAuthority(DWORD ChaDbNum) const;
    bool HaveClubBattleAuthority(DWORD ChaDbNum) const;

    //! Club member 계급 변경
    virtual bool ChangeMemberGrade(DWORD MemberDbNum, size_t Grade);

    //! 선도 Club 인증 가능 Club 멤버.
    DWORD GetCdCertifier() const { return m_dwCDCertifior; }
    void SetCdCertifier(DWORD MemberDbNum) { m_dwCDCertifior=MemberDbNum; }

    //! 배틀 포인트 (승)
    DWORD GetBattleWin() const { return m_dwBattleWin; }
    //! 배틀 포인트 (패)
    DWORD GetBattleLost() const { return m_dwBattleLose; }
    //! 배틀 포인트 (무)
    DWORD GetBattleDraw() const { return m_dwBattleDraw; }

    //! 클럽 배틀/club battle
    void GetClubBattleData(std::vector<GLCLUBBATTLE>& vData) const;

    //!	Club 마크 버전
    DWORD GetMarkVersion() const { return m_dwMarkVER; }

    //! 권한
    DWORD GetGradeFlag(size_t Index) const;
    bool SetGradeFlag(size_t Index, DWORD Flag);
    bool SetGradeFlag(const std::vector<club::GRADE_FLAG>& Data);

    //! Login time
    void SetMemberLastPlayTime(DWORD ChaDbNum, const __time64_t& LoginTime);

    //! 공개/비공개 설정 ----------------------------------------------------    
    //! 신입생 들에게 클럽 공개
    bool IsPublicForNewbie() const { return m_PublicType & club::PUBLIC_FOR_NEWBIE; }

    //! 타 클럽원들에게 클럽 공개
    bool IsPublicForOtherClubMember() const { return m_PublicType & club::PUBLIC_FOR_OTHER_CLUB_MEMBER; }

    void PublicSet(DWORD PublicType) { m_PublicType = PublicType; }
    DWORD GetPublicFlag() const { return m_PublicType; }

    //! 자동 추방
    bool AutoKickOutUse() const { return m_AutoKickOut.m_Use; }
    const club::AUTO_KICK_OUT& GetAutoKickOut() const { return m_AutoKickOut; }
    void AutoKickOutSet(const club::AUTO_KICK_OUT& AutoKickOut) { m_AutoKickOut = AutoKickOut; }

    //! 수익금액. 주의! Field Server 에서는 사용하지 마시오
    void SetIncomeMoney( LONGLONG llMoney );

    //! 창고금액. 주의! Field Server 에서는 사용하지 마시오
    void SetStorageMoney( LONGLONG llMoney );

    //! 요청 Club ID
    void  SetReqClubId(DWORD ClubDbNum) { m_dwReqClubID = ClubDbNum; }
    DWORD GetReqClubId() const { return m_dwReqClubID; }

    //! 동맹 Club장
    void SetAlliance(DWORD ClubDbNum) { m_dwAlliance = ClubDbNum; }

    //! 동맹배틀 포인트 (승)
    void SetAllianceBattleWin(DWORD Win) { m_dwAllianceBattleWin = Win; }
    DWORD GetAllianceBattleWin() const { return m_dwAllianceBattleWin; }

    //! 동맹배틀 포인트 (패)
    void SetAllianceBattleLose(DWORD Lose) { m_dwAllianceBattleLose = Lose; }
    DWORD GetAllianceBattleLose() const { return m_dwAllianceBattleLose; }

    //! 동맹배틀 포인트 (무)
    void SetAllianceBattleDraw(DWORD Draw) { m_dwAllianceBattleDraw = Draw; }
    DWORD GetAllianceBattleDraw() const { return m_dwAllianceBattleDraw; }
};

//typedef stdext::hash_map<std::string,DWORD>	CLUBSNAME;
typedef std::tr1::unordered_map<std::string, DWORD> CLUBSNAME;
typedef CLUBSNAME::iterator					     CLUBSNAME_ITER;
typedef CLUBSNAME::const_iterator                CLUBSNAME_CITER;
typedef CLUBSNAME::value_type                    CLUBSNAME_VALUE;

#endif // _GL_CLUB_H_
