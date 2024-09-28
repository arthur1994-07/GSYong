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
    //! VERSION 
    //! VERSION  compile 
    //! 0x0002 m_Country m_MasterLevel 
    enum { VERSION=0x0007, };

    GLClub();
    virtual ~GLClub();

    GLClub(const GLClub& value)
    {
        operator=(value);
    }

public:
    DWORD			m_DbNum; //! Club DB ��ȣ
    wb::EM_COUNTRY  m_Country; //! ����. 2012-02-13 jgkim
    char			m_szName[CLUB_NAME_LENGTH]; //! Club �̸�

    DWORD			m_MasterCharDbNum; //! Club ������ Char DB ��ȣ
    char			m_szMasterName[CHAR_SZNAME]; //! Club �������� char name
    int             m_MasterLevel; //! Club ������ Level 2012-02-13 jgkim
    DWORD			m_dwCDCertifior; //! ���� Club ���� ���� Club ���.
    
    DWORD			m_dwRank; //! Club ���

    DWORD			m_dwMarkVER;								//!	Club ��ũ ����
    DWORD			m_aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];	//!	Club ��ũ �̹��� RGB.

    __time64_t		m_tOrganize;								//!	Club â�� �ð�.
    __time64_t		m_tDissolution;								//!	Club ��ü ���� �ð�.
    __time64_t		m_tAuthority;								//!	Club������ ���� ���� �ð�

    __time64_t		m_tAllianceSec;								//!	���� Ż�� Ȥ�� ���� �ð� + ( ���� ���ѽð� )
    __time64_t		m_tAllianceDis;								//!	���� �ػ� �ð� + ( â�� ���� �ð� )

    char			m_szNotice[EMCLUB_NOTICE_LEN];	//! Club ����
    char 		    m_szNoticeChaName[CHAR_SZNAME]; //! �����ۼ� ĳ���� �̸�
    __time64_t	    m_NoticeDate; //! �����ۼ� ��¥/�ð�

    char            m_szNoticeNewbie[EMCLUB_NOTICE_LEN]; //! ���Ի��� ���� �Ұ���

protected:
    CLUBMEMBERS		m_mapMembers;								//! Club��.
    club::AuthGrade* m_pGrade;

public:
    CLUB_CDM_MEM	m_listCDMMembers;							//! CDM Club��.

    DWORD			m_dwAlliance;								//! ���� Club��.
    CLUB_ALLIANCE	m_setAlliance;								//! ���� Club��.

    CLUBCONFT		m_setConftSTART;							//! Club ��� ���۽� �ɹ�.
    CLUBCONFT		m_setConftCURRENT;							//! Club ��� ���� �ɹ�.
    SCONFT_OPTION	m_conftOPT;									//! Club ��� �ɼ�.
    DWORD			m_dwconftCLUB;								//! Club ��� ��� Club.
    float			m_fconftTIMER;								//! Club ��� ��� �ð�.

    float			m_fTIMER_BATTLE;
        
    DWORD			m_dwBattleWin;								//! ��Ʋ ����Ʈ ( �� )
    DWORD			m_dwBattleLose;								//! ��Ʋ ����Ʈ ( �� )
    DWORD			m_dwBattleDraw;								//! ��Ʋ ����Ʈ ( ��)
    __time64_t		m_tLastBattle;								//! ������ ��Ʋ ����ð�

    DWORD			m_dwAllianceBattleWin;						//! ���͹�Ʋ ����Ʈ ( �� )
    DWORD			m_dwAllianceBattleLose;						//! ���͹�Ʋ ����Ʈ ( �� )
    DWORD			m_dwAllianceBattleDraw;						//! ���͹�Ʋ ����Ʈ ( ��)

    CLUB_BATTLE		m_mapBattleReady;							//! ������� ��Ʋ����
    CLUB_BATTLE		m_mapBattle;								//! ��Ʋ ����
    CLUB_BATTLE_DEL	m_mapBattleDel;								//! ���� ������ ��Ʋ ����

    DWORD			m_dwBattleTime;								//! ��Ʋ Club ����ð�. (temp)
    DWORD			m_dwBattleArmisticeReq;						//! ��Ʋ ���� ��û�� Club ID ( temp )

    DWORD			m_dwReqClubID;								//! ��û Club ID

protected:
    //! ����/����� ����
    DWORD m_PublicType; //! Ŭ�� ���� Ÿ�� EM_PUBLIC_TYPE ����

    //! ���ͱݾ�. ����! Field Server ������ ������� ���ÿ�
    LONGLONG m_lnIncomeMoney;  

    //! â��ݾ�. ����! Field Server ������ ������� ���ÿ�
    LONGLONG m_lnStorageMoney; 

    club::AUTO_KICK_OUT m_AutoKickOut; //! �ڵ� �߹�

public:
    const GLClub& operator = (const GLClub& rhs);
    //virtual void SetInfo(const GLCLUBINFO& sINFO);
    BOOL GETSTORAGE_BYBUF(se::ByteStream& ByteStream) const;

public:
    //! Club DB ��ȣ
    inline DWORD DbNum() const { return m_DbNum; }
    inline bool IsValid() const { return m_DbNum != CLUB_NULL ? true : false; }
    inline void SetDbNum(DWORD DbNum) { m_DbNum=DbNum; }

    //! ����
    inline wb::EM_COUNTRY GetCountry() const { return m_Country; }
    void SetCountry(wb::EM_COUNTRY Country) { m_Country=Country; }
    
    //! Club �̸�
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

    //! Club ������ Char DB ��ȣ
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
    
    //! ���� �ػ� �ð� + ( â�� ���� �ð� )
    void SetAllianceDis();    
    void SetAllianceDis(const __time64_t& Time) { m_tAllianceDis=Time; }
    __time64_t GetAllianceDisTime() const { return m_tAllianceDis; }    
    
    //!	���� Ż�� Ȥ�� ���� �ð� + ( ���� ���ѽð� )
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

    //	���� Club�̰ų� �������� üũ�ϴ� �Լ�
    //	Ŭ���̾�Ʈ�� �ΰ��� �Լ��� ��밡��
    //	������ ���� ���͸���Ʈ�� ������ Club�� ������ �����Ƿ� 
    //	IsAllianceGuild_CHIEF() �ش� �Լ��� ������ Club�� ��밡���ϴ�.
    //	Common�Լ��� ����ID( AllianceID )�� ������ üũ�ϹǷ� �ƹ������� ��밡���ϴ�.
    bool IsAllianceGuild_CHIEF ( DWORD dwGuildID );
    bool IsAllianceGuild_COMMON ( DWORD dwClubID, DWORD dwAllianceID );

    bool IsAuthority();
    //! ���� �Ἲ ����ð� ���� üũ
    bool IsAllianceDis();
    //! ���� ���� ����ð� ���� üũ
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
    bool IsCDMEnter(DWORD dwCharID); //! ���� ��������

public:
    bool IsMember(DWORD dwCharID) const;
    DWORD GetMemberNum() const { return (DWORD) m_mapMembers.size(); }
    DWORD GetAllianceNum() const { return (DWORD) m_setAlliance.size(); }
    //! ���� Club��
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

    //! Club ����
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

    //! ���Ի��� ���� �Ұ���
    const char* GetNoticeNewbie() const { return m_szNoticeNewbie; }

    //! ���Ի��� ���� �Ұ���
    void SetNoticeNewbie(const char* szMsg)
    {
        if (szMsg)
            StringCchCopy(m_szNoticeNewbie, EMCLUB_NOTICE_LEN, szMsg);
    }

    void SetNoticeNewbie(const std::string& Msg)
    {
        SetNoticeNewbie(Msg.c_str());
    }

    //! Club ������ Level 2012-02-13 jgkim
    inline void SetMasterCharLevel(WORD ChaLevel)
    {
        m_MasterLevel = ChaLevel;
    }

    inline int GetMasterLevel() const { return m_MasterLevel; }

    //! Club �������� char name
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

    //! Club ���
    inline DWORD GetRank() const { return m_dwRank; }

    //! Club â�� �ð�
    inline __time64_t GetOrganizeDate() const { return m_tOrganize; }

    //! Club ����
protected:
    bool CheckAuth(DWORD ChaDbNum, club::AUTHORITY Auth) const;

public:
    virtual bool AddAuth(size_t Index, const club::AUTH_GRADE& Grade);
    virtual bool ChangeAuth(size_t Index, club::AUTH_GRADE& Grade);
    virtual bool ChangeAuth(size_t IndexA, size_t IndexB);

    club::AUTH_GRADE GetAuth(size_t Index) const;
    
    //! Club ��� ����
    WORD GetGradeSize() const;

    //! Club ��� ���� ���ڿ�
    std::string GetGradeName(size_t Index) const;
    bool HaveSameGradeName(const std::string& GradeName) const;
    virtual bool ChangeGradeName(size_t GradeIndex, const std::string& GradeName, DWORD ReqChaDbNum);

    bool HaveMemberJoinAuthority(DWORD ChaDbNum) const;
    bool HaveMemberKickAuthority(DWORD ChaDbNum) const;
    bool HaveNoticeChangeAuthority(DWORD ChaDbNum) const;
    bool HaveMarkChangeAuthority(DWORD ChaDbNum) const;
    bool HaveDeathMatchAuthority(DWORD ChaDbNum) const;
    //! Ŭ�� ���� ������ �����Ѱ�?
    bool HaveAuthChangeAuthority(DWORD ChaDbNum) const;
    bool HaveRankUpAuthority(DWORD ChaDbNum) const;
    bool HaveStorageAuthority(DWORD ChaDbNum) const;
    bool HaveMoneyAuthority(DWORD ChaDbNum) const;
    bool HaveAllianceAuthority(DWORD ChaDbNum) const;
    bool HaveAllianceDisAuthority(DWORD ChaDbNum) const;
    bool HaveClubBattleAuthority(DWORD ChaDbNum) const;

    //! Club member ��� ����
    virtual bool ChangeMemberGrade(DWORD MemberDbNum, size_t Grade);

    //! ���� Club ���� ���� Club ���.
    DWORD GetCdCertifier() const { return m_dwCDCertifior; }
    void SetCdCertifier(DWORD MemberDbNum) { m_dwCDCertifior=MemberDbNum; }

    //! ��Ʋ ����Ʈ (��)
    DWORD GetBattleWin() const { return m_dwBattleWin; }
    //! ��Ʋ ����Ʈ (��)
    DWORD GetBattleLost() const { return m_dwBattleLose; }
    //! ��Ʋ ����Ʈ (��)
    DWORD GetBattleDraw() const { return m_dwBattleDraw; }

    //! Ŭ�� ��Ʋ/club battle
    void GetClubBattleData(std::vector<GLCLUBBATTLE>& vData) const;

    //!	Club ��ũ ����
    DWORD GetMarkVersion() const { return m_dwMarkVER; }

    //! ����
    DWORD GetGradeFlag(size_t Index) const;
    bool SetGradeFlag(size_t Index, DWORD Flag);
    bool SetGradeFlag(const std::vector<club::GRADE_FLAG>& Data);

    //! Login time
    void SetMemberLastPlayTime(DWORD ChaDbNum, const __time64_t& LoginTime);

    //! ����/����� ���� ----------------------------------------------------    
    //! ���Ի� �鿡�� Ŭ�� ����
    bool IsPublicForNewbie() const { return m_PublicType & club::PUBLIC_FOR_NEWBIE; }

    //! Ÿ Ŭ�����鿡�� Ŭ�� ����
    bool IsPublicForOtherClubMember() const { return m_PublicType & club::PUBLIC_FOR_OTHER_CLUB_MEMBER; }

    void PublicSet(DWORD PublicType) { m_PublicType = PublicType; }
    DWORD GetPublicFlag() const { return m_PublicType; }

    //! �ڵ� �߹�
    bool AutoKickOutUse() const { return m_AutoKickOut.m_Use; }
    const club::AUTO_KICK_OUT& GetAutoKickOut() const { return m_AutoKickOut; }
    void AutoKickOutSet(const club::AUTO_KICK_OUT& AutoKickOut) { m_AutoKickOut = AutoKickOut; }

    //! ���ͱݾ�. ����! Field Server ������ ������� ���ÿ�
    void SetIncomeMoney( LONGLONG llMoney );

    //! â��ݾ�. ����! Field Server ������ ������� ���ÿ�
    void SetStorageMoney( LONGLONG llMoney );

    //! ��û Club ID
    void  SetReqClubId(DWORD ClubDbNum) { m_dwReqClubID = ClubDbNum; }
    DWORD GetReqClubId() const { return m_dwReqClubID; }

    //! ���� Club��
    void SetAlliance(DWORD ClubDbNum) { m_dwAlliance = ClubDbNum; }

    //! ���͹�Ʋ ����Ʈ (��)
    void SetAllianceBattleWin(DWORD Win) { m_dwAllianceBattleWin = Win; }
    DWORD GetAllianceBattleWin() const { return m_dwAllianceBattleWin; }

    //! ���͹�Ʋ ����Ʈ (��)
    void SetAllianceBattleLose(DWORD Lose) { m_dwAllianceBattleLose = Lose; }
    DWORD GetAllianceBattleLose() const { return m_dwAllianceBattleLose; }

    //! ���͹�Ʋ ����Ʈ (��)
    void SetAllianceBattleDraw(DWORD Draw) { m_dwAllianceBattleDraw = Draw; }
    DWORD GetAllianceBattleDraw() const { return m_dwAllianceBattleDraw; }
};

//typedef stdext::hash_map<std::string,DWORD>	CLUBSNAME;
typedef std::tr1::unordered_map<std::string, DWORD> CLUBSNAME;
typedef CLUBSNAME::iterator					     CLUBSNAME_ITER;
typedef CLUBSNAME::const_iterator                CLUBSNAME_CITER;
typedef CLUBSNAME::value_type                    CLUBSNAME_VALUE;

#endif // _GL_CLUB_H_
