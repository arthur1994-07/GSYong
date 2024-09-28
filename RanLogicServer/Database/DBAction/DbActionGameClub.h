#ifndef _DB_ACTION_GAME_CLUB_H_
#define _DB_ACTION_GAME_CLUB_H_

#pragma once

#include "DbAction.h"

class GLClubAgent;

namespace wb
{
    enum EM_COUNTRY;
}

namespace db
{

//! 새로운 클럽을 생성한다.
class ClubCreate : public DbAction
{
public:
	//! 새로운 클럽을 생성한다.
	//! \return -1:이미 클럽마스터로 등록되어 있음 생성불가.
	//!         -2:클럽 생성중 에러발생 (중복된길드이름)
	//!          0이상:생성된 클럽번호임
    ClubCreate(        
        DWORD ClientSlot,
        const std::string& ClubName,
        const std::string& ChaName,
        DWORD ChaDbNum,
        wb::EM_COUNTRY Country,
        bool bLobby);
    virtual ~ClubCreate() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    std::string m_ClubName;
    std::string m_ChaName;
    DWORD m_ChaDbNum;
    wb::EM_COUNTRY m_Country;
    bool m_bLobby; //! Lobby 에서 생성할때
};

//! 클럽을 삭제한다.
class ClubDelete : public DbAction
{
public:
	//! 클럽을 삭제한다.
	//! \param dwClub 클럽번호
	//! \param dwChaNum 클럽을 삭제하려는 캐릭터번호(캐릭터 번호가 클럽 마스터가 아니면 삭제가 되지 않는다)
	//! \return 0:클럽 삭제 실패
    ClubDelete(DWORD dwClub, DWORD dwChaNum);
    virtual ~ClubDelete() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    DWORD m_dwChaNum;
};

//! 클럽랭크를 세팅한다.
class ClubRankSet : public DbAction
{
public:
	//! 클럽랭크를 세팅한다.
	//! \param dwClub 클럽번호
	//! \param dwRank 랭킹
    ClubRankSet(DWORD dwClub, DWORD dwRank);
    virtual ~ClubRankSet() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    DWORD m_dwRank;
};

class ClubMarkImageWrite : public DbAction
{
public:
	//! 클럽 마크 이미지를 저장한다.
	//! \param dwClub 클럽번호
	//! \param dwMarkVer 마크버전
	//! \param pData 이미지데이터 포인터
	//! \param nSize 이미지데이터 사이즈
    ClubMarkImageWrite(DWORD dwClub, DWORD dwMarkVer, BYTE* pData, int nSize);
    virtual ~ClubMarkImageWrite();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    DWORD m_dwMarkVer;
    BYTE  m_aryMark[sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY];
    int   m_nSize;
};

//! 클럽 해체시간을 설정한다.
class ClubDissolutionTimeSet : public DbAction
{
public:
	//! 클럽 해체시간을 설정한다.
	//! \param dwClub 클럽번호
	//! \param tDiss 해체시간
    ClubDissolutionTimeSet(DWORD dwClub, __time64_t tDiss);
    virtual ~ClubDissolutionTimeSet() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    __time64_t m_tDiss;
};

//! 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
class ClubAllianceTimeSet : public DbAction
{
public:
	//! 클럽의 동맹탈퇴 혹은 제명시간을 정한다.
	//! \param dwClub 클럽번호
	//! \param tSec 시간
    ClubAllianceTimeSet(DWORD dwClub, __time64_t tSec);
    virtual ~ClubAllianceTimeSet() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    __time64_t m_tSec;
};

//! 클럽의 동맹해산 시간을 정한다.
class ClubAllianceDisolveTimeSet : public DbAction
{
public:
	//! 클럽의 동맹해산 시간을 정한다.
	//! \param dwClub 클럽번호
	//! \param tDis 해산시간
    ClubAllianceDisolveTimeSet(DWORD dwClub, __time64_t tDis);
    virtual ~ClubAllianceDisolveTimeSet() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    __time64_t m_tDis;
};

//! 클럽의 마스터 위임 시간을 설정한다.
class ClubAuthorityTimeSet : public DbAction
{
public:	
	//! 클럽의 마스터 위임 시간을 설정한다.
	//! \param dwClub 클럽번호
	//! \param tDis 마스터 위임 시간
    ClubAuthorityTimeSet(DWORD dwClub, __time64_t tAuthority);
    virtual ~ClubAuthorityTimeSet() {} ;
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    __time64_t m_tAuthority;
};

//! 새로운 캐릭터를 클럽에 가입시킨다
class ClubMemberAdd : public DbAction
{
public:	
	//! 새로운 캐릭터를 클럽에 가입시킨다
	//! \param dwClub 클럽번호
	//! \param dwChaNum 가입시키려는 캐릭터번호
    ClubMemberAdd(DWORD dwClub, DWORD dwChaNum);
    virtual ~ClubMemberAdd();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    DWORD m_dwChaNum;
};

//! 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
class ClubMemberDelete : public DbAction
{
public:
	//! 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다
	//! \param dwChaNum 캐릭터번호
    ClubMemberDelete(DWORD dwChaNum);
    virtual ~ClubMemberDelete();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwChaNum;
};

//! 가입되어 있는 캐릭터를 클럽에서 탈퇴시킨다(탈퇴 패널티X)
class ClubMemberDeleteWithNoPenalty : public DbAction
{
public:
	ClubMemberDeleteWithNoPenalty( DWORD dwChaNum );
	virtual ~ClubMemberDeleteWithNoPenalty();
	virtual int Execute( NetServer* pServer ) override;

protected:
	DWORD m_dwChaNum;
};

//! 지역과 그지역을 소유한 클럽 세율을 정한다.
class ClubRegionSet : public DbAction
{
public:
	//! 지역과 그지역을 소유한 클럽 세율을 정한다.
	//! \param dwRegionID 지역 ID
	//! \param dwClub 클럽 ID
	//! \param fTax 세율
	ClubRegionSet(DWORD dwRegionID, DWORD dwClub, float fTax);
	virtual ~ClubRegionSet();
	virtual int Execute(NetServer* pServer) override;
	
protected:
	DWORD m_dwRegionID;
	DWORD m_dwClub;
	float m_fTax;
};

//! 해당 지역의 소유 상태를 리셋한다.
class ClubRegionDelete : public DbAction
{
public:	
	//! 해당 지역의 소유 상태를 리셋한다.
	//! \n 주의
	//! \n 해당지역 ID 와 클럽 ID 가 DB 정보와 정확히 일치하지 않으면 취소된다.
	//! \param dwRegionID 지역 ID
	//! \param dwClub 클럽 ID
	ClubRegionDelete(DWORD dwRegionID, DWORD dwClub);
	virtual ~ClubRegionDelete();
	virtual int Execute(NetServer* pServer) override;
	
protected:
	DWORD m_dwRegionID;
	DWORD m_dwClub;
};

//! 해당클럽의 보유금액을 세팅한다. (Storage)
class ClubMoneySet : public DbAction
{
public:
	/**
	* 해당클럽의 보유금액을 세팅한다. (Storage)
	* \param dwClub 클럽번호
	* \param llMoney 보유금액 (금액은 >= 0)
	* \return 
	*/
	ClubMoneySet(DWORD dwClub, LONGLONG llMoney);
	virtual ~ClubMoneySet() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	LONGLONG m_llMoney;
};

//! 해당클럽의 수입금액을 세팅한다. (Income money)
class ClubIncomeMoneySet : public DbAction
{
public:
	/**
	* 해당클럽의 수입금액을 세팅한다. (Income money)
	* \param dwClub 클럽번호
	* \param llMoney 수입금액 (금액은 >= 0)
	* \return 
	*/
	ClubIncomeMoneySet(DWORD dwClub, LONGLONG llMoney);
	virtual ~ClubIncomeMoneySet() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	LONGLONG m_llMoney;
};

//! 해당클럽의 클럽창고를 저장한다. 
class ClubStorageWrite : public DbAction
{
public:
	/**
	* 해당클럽의 클럽창고를 저장한다.
	* \param dwClub 클럽번호
	* \param pData 창고데이터 포인터
	* \param nSize 버퍼크기
	* \return 
	*/
	ClubStorageWrite( DWORD dwClub, GLClubAgent* pClub );	// 포인터 넘겨받아 작업하려니 찜찜, 생성자에서 아이템만 받아오고 더이상 사용하지 않음
	virtual ~ClubStorageWrite();
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	std::vector< SINVENITEM_SAVE > m_vecItems;
};

class ClubStorageGet : public DbAction
{
public:
	ClubStorageGet( DWORD _dwClientID, DWORD _dwCharID, DWORD _dwClubID );
	virtual ~ClubStorageGet() {};
	virtual int Execute( NetServer* pServer ) override;

protected:
	DWORD m_dwClientID;
	DWORD m_dwCharID;
	DWORD m_dwClubID;
};

//! 부 클럽장을 세팅한다
class ClubMasterFlagsSet : public DbAction
{
public:
	/**
	*
	* dwClub : 클럽번호
    * dwChaNum : 캐릭터번호
    * dwSubMasterFlags : 플래그 (0 으로 넣으면 일반길드원이 된다)
	* \param dwClub 
	* \param dwChaNum 
	* \param dwSubMasterFlags 
	* \return 
	*/
	ClubMasterFlagsSet(DWORD dwClub, DWORD dwChaNum, DWORD dwSubMasterFlags);
	virtual ~ClubMasterFlagsSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	DWORD m_dwChaNum;
	DWORD m_dwSubMasterFlags;
};

//! 동맹 클럽을 결성한다.
class ClubAllianceSet : public DbAction
{
public:
	/**
	* 동맹 클럽을 결성한다.
	* \n 주의
    * \n db 는 클럽의 무결성을 체크하지 않는다.
    * \n A 가 B 의 보조인 상태에서 
    * \n B 가 C 의 보조로 들어가고
    * \n C 가 A 의 보조로 들어가면 소유권 체인 문제가 발생한다.
	* \param dwClubP 주 클럽번호 (동맹 클럽의 주)
	* \param dwClubS 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
	* \return 
	*/
	ClubAllianceSet(DWORD dwClubP, DWORD dwClubS);
	virtual ~ClubAllianceSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClubP;
	DWORD m_dwClubS;
};

//! 동맹 클럽을 해체한다.
class ClubAllianceDelete : public DbAction
{
public:
	/**
	* 동맹 클럽을 해체한다.
	* \param dwClubP 주 클럽번호 (동맹 클럽의 주)
	* \param dwClubS 보조 클럽번호 (주 클럽 밑으로 들어가는 하위 클럽)
	* \return 
	*/
	ClubAllianceDelete(DWORD dwClubP, DWORD dwClubS);
	virtual ~ClubAllianceDelete() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClubP;
	DWORD m_dwClubS;
};

class ClubBattleSet : public DbAction
{
public:
	/**
	* 클럽배틀을 시작한다.
	* \param dwClubP 주 클럽번호 (자기클럽)
	* \param dwClubS 보조 클럽번호 (상대클럽)
	* \return 
	*/
	ClubBattleSet(DWORD dwClubP, DWORD dwClubS, DWORD dwEndTime, bool bAlliance = false );
	virtual ~ClubBattleSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClubP;
	DWORD m_dwClubS;
	DWORD m_dwEndTime;
	bool m_bAlliance;
};

class ClubBattleEnd : public DbAction
{
public:
	/**
	* 클럽배틀을 종료한다.
	* \param dwClubP 주 클럽번호 (자기클럽)
	* \param dwClubS 보조 클럽번호 (상대클럽)
	* \param nFlag		종료 Flag
	* \param nGuKillNum	 킬수
	* \param nGuDeathNum 데스수
	* \return 
	*/
	ClubBattleEnd(DWORD dwClubP, DWORD dwClubS, int nFlag, int nGuKillNum, int nGuDeathNum, bool bAlliance = false );
	virtual ~ClubBattleEnd() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD	m_dwClubP;
	DWORD	m_dwClubS;
	int		m_nFlag;
	int		m_nGuKillNum;
	int		m_nGuDeathNum;
	bool	m_bAlliance;
};

class ClubBattleSave : public DbAction
{
public:
	/**
	* 클럽배틀을 진행상황을 저장한다.
	* \param dwClubP 주 클럽번호 (자기클럽)
	* \param dwClubS 보조 클럽번호 (상대클럽)
	* \param nGuKillNum	 킬수
	* \param nGuDeathNum 데스수
	* \return 
	*/
	ClubBattleSave(DWORD dwClubP, DWORD dwClubS, int nGuKillNum, int nGuDeathNum );
	virtual ~ClubBattleSave() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD	m_dwClubP;
	DWORD	m_dwClubS;
	int		m_nGuKillNum;
	int		m_nGuDeathNum;
};

//! 동맹배틀 전적을 초기화 시킨다.
class AllianceBattleReSet : public DbAction
{
public:
	/**
	* 동맹배틀 전적을 초기화 시킨다.	
	* \param dwClub 클럽번호
	* \return 
	*/
    AllianceBattleReSet( DWORD dwClub );
    virtual ~AllianceBattleReSet() {} ;
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
};

//! 클럽의 공지사항을 세팅한다.
class ClubNoticeSet : public DbAction
{
public:	
	//! 클럽의 공지사항을 세팅한다.
	//! \param dwClub 클럽번호
	//! \param szClubNotice 공지사항
    ClubNoticeSet(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice);
	virtual ~ClubNoticeSet();
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
    std::string m_Notice;
    std::string m_ChaName;
	//char  m_szNotice[EMCLUB_NOTICE_LEN+1];
};

//! 클럽의 선도클럽 인증 대리자를 세팅한다.
class ClubDeputySet : public DbAction
{
public:
	/**
	* 클럽의 선도클럽 인증 대리자를 세팅한다.
	* \param dwClub 클럽번호
	* \param dwDeputy 인증대리자 캐릭터번호
	* \return 
	*/
	ClubDeputySet(DWORD dwClub, DWORD dwDeputy);
	virtual ~ClubDeputySet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	DWORD m_dwDeputy;
};

//! 클럽의 마스터 권한을 위임한다.
class ClubAuthoritySet : public DbAction
{
public:
	//! 클럽의 선도클럽 인증 대리자를 세팅한다.
	//! \param dwClub 클럽번호
	//! \param dwDeputy 인증대리자 캐릭터번호
	ClubAuthoritySet(DWORD dwClub, DWORD dwMasterID);
	virtual ~ClubAuthoritySet() {}
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	DWORD m_dwMasterID;
};

//! 클럽 권한 세팅
class InsertClubAuth : public DbAction
{
public:
    InsertClubAuth(
        DWORD ClubDbNum,
        size_t AuthIndex,
        DWORD AuthFlag,
        const std::string& AuthName);
    virtual ~InsertClubAuth();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ClubDbNum;
    size_t m_AuthIndex;
    DWORD m_AuthFlag;
    std::string m_AuthName;
};

//! 클럽 권한 세팅 업데이트
class UpdateClubAuth : public DbAction
{
public:
    UpdateClubAuth(
        DWORD ClubDbNum,
        size_t AuthIndex,
        DWORD AuthFlag,
        const std::string& AuthName);
    virtual ~UpdateClubAuth();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ClubDbNum;
    size_t m_AuthIndex;
    DWORD m_AuthFlag;
    std::string m_AuthName;
};

//! 클럽 권한 인덱스 변경
class ChangeClubAuthIndex : public DbAction
{
public:
    ChangeClubAuthIndex(
        DWORD ClubDbNum,
        size_t IndexA,
        size_t IndexB);
    virtual ~ChangeClubAuthIndex();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ClubDbNum;
    size_t m_IndexA;
    size_t m_IndexB;
};

//! Club member 계급 변경
class ChangeClubMemberGrade : public DbAction
{
public:
    ChangeClubMemberGrade(DWORD ChaDbNum, int Grade);
    virtual ~ChangeClubMemberGrade();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ChaDbNum;
    int m_Grade;
};

//! 클럽 닉네임 업데이트
class ClubNickUpdate : public DbAction
{
public:
    ClubNickUpdate(DWORD ChaDbNum, const std::string& NickName);
    virtual ~ClubNickUpdate();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ChaDbNum;
    std::string m_NickName;
};

//! 클럽 공개 범위 설정
class ClubPublicFlagUpdate : public DbAction
{
public:
    ClubPublicFlagUpdate(DWORD ClubDbNum, DWORD Flag);
    virtual ~ClubPublicFlagUpdate();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ClubDbNum;
    DWORD m_Flag;
};

//! 클럽 자동 멤버 탈퇴 설정
class ClubAutoKickUpdate : public DbAction
{
public:
    ClubAutoKickUpdate(
        DWORD ClubDbNum,
        size_t GradeIndex,
        WORD Day,
        bool AutoKickOut);
    virtual ~ClubAutoKickUpdate();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ClubDbNum;
    size_t m_GradeIndex;  //! 추방할 계급
    WORD   m_Day;         //! 얼마동안 접속하지 않으면 kick 할 것인가
    bool   m_AutoKickOut; //! 자동 추방을 사용/사용하지 않음
};

//! 클럽 신입생 소개글 update
class ClubNewbieNoticeUpdate : public DbAction
{
public:
    ClubNewbieNoticeUpdate(DWORD ClubDbNum, const std::string& Notice);
    virtual ~ClubNewbieNoticeUpdate();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ClubDbNum;
    std::string m_Notice;
};

//! 클럽 가입제한시간 리셋
class ChaClubSecedeTimeReset : public DbAction
{
public:
    ChaClubSecedeTimeReset(DWORD ChaDbNum);
    virtual ~ChaClubSecedeTimeReset();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ChaDbNum;
};

//! 클럽 이름 변경
class ClubRename : public DbAction
{
public:
	// m_ClubName 변경할 클럽 이름
	// m_ChaDbNum 요청한 캐릭터
	ClubRename( DWORD ClubNum, DWORD ChaDbNum, const std::string& ClubName );
	virtual ~ClubRename() {}
	virtual int Execute( NetServer* pServer ) override;

protected:
	DWORD m_ClubNum;
	DWORD m_ChaDbNum;
	std::string m_ClubName;
};

} // namespace db

#endif // _DB_ACTION_GAME_CLUB_H_