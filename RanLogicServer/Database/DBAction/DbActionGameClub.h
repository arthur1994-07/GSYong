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

//! ���ο� Ŭ���� �����Ѵ�.
class ClubCreate : public DbAction
{
public:
	//! ���ο� Ŭ���� �����Ѵ�.
	//! \return -1:�̹� Ŭ�������ͷ� ��ϵǾ� ���� �����Ұ�.
	//!         -2:Ŭ�� ������ �����߻� (�ߺ��ȱ���̸�)
	//!          0�̻�:������ Ŭ����ȣ��
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
    bool m_bLobby; //! Lobby ���� �����Ҷ�
};

//! Ŭ���� �����Ѵ�.
class ClubDelete : public DbAction
{
public:
	//! Ŭ���� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param dwChaNum Ŭ���� �����Ϸ��� ĳ���͹�ȣ(ĳ���� ��ȣ�� Ŭ�� �����Ͱ� �ƴϸ� ������ ���� �ʴ´�)
	//! \return 0:Ŭ�� ���� ����
    ClubDelete(DWORD dwClub, DWORD dwChaNum);
    virtual ~ClubDelete() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    DWORD m_dwChaNum;
};

//! Ŭ����ũ�� �����Ѵ�.
class ClubRankSet : public DbAction
{
public:
	//! Ŭ����ũ�� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param dwRank ��ŷ
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
	//! Ŭ�� ��ũ �̹����� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param dwMarkVer ��ũ����
	//! \param pData �̹��������� ������
	//! \param nSize �̹��������� ������
    ClubMarkImageWrite(DWORD dwClub, DWORD dwMarkVer, BYTE* pData, int nSize);
    virtual ~ClubMarkImageWrite();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    DWORD m_dwMarkVer;
    BYTE  m_aryMark[sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY];
    int   m_nSize;
};

//! Ŭ�� ��ü�ð��� �����Ѵ�.
class ClubDissolutionTimeSet : public DbAction
{
public:
	//! Ŭ�� ��ü�ð��� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param tDiss ��ü�ð�
    ClubDissolutionTimeSet(DWORD dwClub, __time64_t tDiss);
    virtual ~ClubDissolutionTimeSet() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    __time64_t m_tDiss;
};

//! Ŭ���� ����Ż�� Ȥ�� ����ð��� ���Ѵ�.
class ClubAllianceTimeSet : public DbAction
{
public:
	//! Ŭ���� ����Ż�� Ȥ�� ����ð��� ���Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param tSec �ð�
    ClubAllianceTimeSet(DWORD dwClub, __time64_t tSec);
    virtual ~ClubAllianceTimeSet() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    __time64_t m_tSec;
};

//! Ŭ���� �����ػ� �ð��� ���Ѵ�.
class ClubAllianceDisolveTimeSet : public DbAction
{
public:
	//! Ŭ���� �����ػ� �ð��� ���Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param tDis �ػ�ð�
    ClubAllianceDisolveTimeSet(DWORD dwClub, __time64_t tDis);
    virtual ~ClubAllianceDisolveTimeSet() {}
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    __time64_t m_tDis;
};

//! Ŭ���� ������ ���� �ð��� �����Ѵ�.
class ClubAuthorityTimeSet : public DbAction
{
public:	
	//! Ŭ���� ������ ���� �ð��� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param tDis ������ ���� �ð�
    ClubAuthorityTimeSet(DWORD dwClub, __time64_t tAuthority);
    virtual ~ClubAuthorityTimeSet() {} ;
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    __time64_t m_tAuthority;
};

//! ���ο� ĳ���͸� Ŭ���� ���Խ�Ų��
class ClubMemberAdd : public DbAction
{
public:	
	//! ���ο� ĳ���͸� Ŭ���� ���Խ�Ų��
	//! \param dwClub Ŭ����ȣ
	//! \param dwChaNum ���Խ�Ű���� ĳ���͹�ȣ
    ClubMemberAdd(DWORD dwClub, DWORD dwChaNum);
    virtual ~ClubMemberAdd();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
    DWORD m_dwChaNum;
};

//! ���ԵǾ� �ִ� ĳ���͸� Ŭ������ Ż���Ų��
class ClubMemberDelete : public DbAction
{
public:
	//! ���ԵǾ� �ִ� ĳ���͸� Ŭ������ Ż���Ų��
	//! \param dwChaNum ĳ���͹�ȣ
    ClubMemberDelete(DWORD dwChaNum);
    virtual ~ClubMemberDelete();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwChaNum;
};

//! ���ԵǾ� �ִ� ĳ���͸� Ŭ������ Ż���Ų��(Ż�� �г�ƼX)
class ClubMemberDeleteWithNoPenalty : public DbAction
{
public:
	ClubMemberDeleteWithNoPenalty( DWORD dwChaNum );
	virtual ~ClubMemberDeleteWithNoPenalty();
	virtual int Execute( NetServer* pServer ) override;

protected:
	DWORD m_dwChaNum;
};

//! ������ �������� ������ Ŭ�� ������ ���Ѵ�.
class ClubRegionSet : public DbAction
{
public:
	//! ������ �������� ������ Ŭ�� ������ ���Ѵ�.
	//! \param dwRegionID ���� ID
	//! \param dwClub Ŭ�� ID
	//! \param fTax ����
	ClubRegionSet(DWORD dwRegionID, DWORD dwClub, float fTax);
	virtual ~ClubRegionSet();
	virtual int Execute(NetServer* pServer) override;
	
protected:
	DWORD m_dwRegionID;
	DWORD m_dwClub;
	float m_fTax;
};

//! �ش� ������ ���� ���¸� �����Ѵ�.
class ClubRegionDelete : public DbAction
{
public:	
	//! �ش� ������ ���� ���¸� �����Ѵ�.
	//! \n ����
	//! \n �ش����� ID �� Ŭ�� ID �� DB ������ ��Ȯ�� ��ġ���� ������ ��ҵȴ�.
	//! \param dwRegionID ���� ID
	//! \param dwClub Ŭ�� ID
	ClubRegionDelete(DWORD dwRegionID, DWORD dwClub);
	virtual ~ClubRegionDelete();
	virtual int Execute(NetServer* pServer) override;
	
protected:
	DWORD m_dwRegionID;
	DWORD m_dwClub;
};

//! �ش�Ŭ���� �����ݾ��� �����Ѵ�. (Storage)
class ClubMoneySet : public DbAction
{
public:
	/**
	* �ش�Ŭ���� �����ݾ��� �����Ѵ�. (Storage)
	* \param dwClub Ŭ����ȣ
	* \param llMoney �����ݾ� (�ݾ��� >= 0)
	* \return 
	*/
	ClubMoneySet(DWORD dwClub, LONGLONG llMoney);
	virtual ~ClubMoneySet() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	LONGLONG m_llMoney;
};

//! �ش�Ŭ���� ���Աݾ��� �����Ѵ�. (Income money)
class ClubIncomeMoneySet : public DbAction
{
public:
	/**
	* �ش�Ŭ���� ���Աݾ��� �����Ѵ�. (Income money)
	* \param dwClub Ŭ����ȣ
	* \param llMoney ���Աݾ� (�ݾ��� >= 0)
	* \return 
	*/
	ClubIncomeMoneySet(DWORD dwClub, LONGLONG llMoney);
	virtual ~ClubIncomeMoneySet() {} ;
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	LONGLONG m_llMoney;
};

//! �ش�Ŭ���� Ŭ��â�� �����Ѵ�. 
class ClubStorageWrite : public DbAction
{
public:
	/**
	* �ش�Ŭ���� Ŭ��â�� �����Ѵ�.
	* \param dwClub Ŭ����ȣ
	* \param pData â������ ������
	* \param nSize ����ũ��
	* \return 
	*/
	ClubStorageWrite( DWORD dwClub, GLClubAgent* pClub );	// ������ �Ѱܹ޾� �۾��Ϸ��� ����, �����ڿ��� �����۸� �޾ƿ��� ���̻� ������� ����
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

//! �� Ŭ������ �����Ѵ�
class ClubMasterFlagsSet : public DbAction
{
public:
	/**
	*
	* dwClub : Ŭ����ȣ
    * dwChaNum : ĳ���͹�ȣ
    * dwSubMasterFlags : �÷��� (0 ���� ������ �Ϲݱ����� �ȴ�)
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

//! ���� Ŭ���� �Ἲ�Ѵ�.
class ClubAllianceSet : public DbAction
{
public:
	/**
	* ���� Ŭ���� �Ἲ�Ѵ�.
	* \n ����
    * \n db �� Ŭ���� ���Ἲ�� üũ���� �ʴ´�.
    * \n A �� B �� ������ ���¿��� 
    * \n B �� C �� ������ ����
    * \n C �� A �� ������ ���� ������ ü�� ������ �߻��Ѵ�.
	* \param dwClubP �� Ŭ����ȣ (���� Ŭ���� ��)
	* \param dwClubS ���� Ŭ����ȣ (�� Ŭ�� ������ ���� ���� Ŭ��)
	* \return 
	*/
	ClubAllianceSet(DWORD dwClubP, DWORD dwClubS);
	virtual ~ClubAllianceSet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClubP;
	DWORD m_dwClubS;
};

//! ���� Ŭ���� ��ü�Ѵ�.
class ClubAllianceDelete : public DbAction
{
public:
	/**
	* ���� Ŭ���� ��ü�Ѵ�.
	* \param dwClubP �� Ŭ����ȣ (���� Ŭ���� ��)
	* \param dwClubS ���� Ŭ����ȣ (�� Ŭ�� ������ ���� ���� Ŭ��)
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
	* Ŭ����Ʋ�� �����Ѵ�.
	* \param dwClubP �� Ŭ����ȣ (�ڱ�Ŭ��)
	* \param dwClubS ���� Ŭ����ȣ (���Ŭ��)
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
	* Ŭ����Ʋ�� �����Ѵ�.
	* \param dwClubP �� Ŭ����ȣ (�ڱ�Ŭ��)
	* \param dwClubS ���� Ŭ����ȣ (���Ŭ��)
	* \param nFlag		���� Flag
	* \param nGuKillNum	 ų��
	* \param nGuDeathNum ������
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
	* Ŭ����Ʋ�� �����Ȳ�� �����Ѵ�.
	* \param dwClubP �� Ŭ����ȣ (�ڱ�Ŭ��)
	* \param dwClubS ���� Ŭ����ȣ (���Ŭ��)
	* \param nGuKillNum	 ų��
	* \param nGuDeathNum ������
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

//! ���͹�Ʋ ������ �ʱ�ȭ ��Ų��.
class AllianceBattleReSet : public DbAction
{
public:
	/**
	* ���͹�Ʋ ������ �ʱ�ȭ ��Ų��.	
	* \param dwClub Ŭ����ȣ
	* \return 
	*/
    AllianceBattleReSet( DWORD dwClub );
    virtual ~AllianceBattleReSet() {} ;
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_dwClub;
};

//! Ŭ���� ���������� �����Ѵ�.
class ClubNoticeSet : public DbAction
{
public:	
	//! Ŭ���� ���������� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param szClubNotice ��������
    ClubNoticeSet(const std::string& ChaName, DWORD dwClub, const std::string& ClubNotice);
	virtual ~ClubNoticeSet();
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
    std::string m_Notice;
    std::string m_ChaName;
	//char  m_szNotice[EMCLUB_NOTICE_LEN+1];
};

//! Ŭ���� ����Ŭ�� ���� �븮�ڸ� �����Ѵ�.
class ClubDeputySet : public DbAction
{
public:
	/**
	* Ŭ���� ����Ŭ�� ���� �븮�ڸ� �����Ѵ�.
	* \param dwClub Ŭ����ȣ
	* \param dwDeputy �����븮�� ĳ���͹�ȣ
	* \return 
	*/
	ClubDeputySet(DWORD dwClub, DWORD dwDeputy);
	virtual ~ClubDeputySet() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	DWORD m_dwDeputy;
};

//! Ŭ���� ������ ������ �����Ѵ�.
class ClubAuthoritySet : public DbAction
{
public:
	//! Ŭ���� ����Ŭ�� ���� �븮�ڸ� �����Ѵ�.
	//! \param dwClub Ŭ����ȣ
	//! \param dwDeputy �����븮�� ĳ���͹�ȣ
	ClubAuthoritySet(DWORD dwClub, DWORD dwMasterID);
	virtual ~ClubAuthoritySet() {}
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwClub;
	DWORD m_dwMasterID;
};

//! Ŭ�� ���� ����
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

//! Ŭ�� ���� ���� ������Ʈ
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

//! Ŭ�� ���� �ε��� ����
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

//! Club member ��� ����
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

//! Ŭ�� �г��� ������Ʈ
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

//! Ŭ�� ���� ���� ����
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

//! Ŭ�� �ڵ� ��� Ż�� ����
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
    size_t m_GradeIndex;  //! �߹��� ���
    WORD   m_Day;         //! �󸶵��� �������� ������ kick �� ���ΰ�
    bool   m_AutoKickOut; //! �ڵ� �߹��� ���/������� ����
};

//! Ŭ�� ���Ի� �Ұ��� update
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

//! Ŭ�� �������ѽð� ����
class ChaClubSecedeTimeReset : public DbAction
{
public:
    ChaClubSecedeTimeReset(DWORD ChaDbNum);
    virtual ~ChaClubSecedeTimeReset();
    virtual int Execute(NetServer* pServer) override;

protected:
    DWORD m_ChaDbNum;
};

//! Ŭ�� �̸� ����
class ClubRename : public DbAction
{
public:
	// m_ClubName ������ Ŭ�� �̸�
	// m_ChaDbNum ��û�� ĳ����
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