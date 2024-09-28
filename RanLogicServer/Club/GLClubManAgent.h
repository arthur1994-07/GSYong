#pragma once

#include <boost/tr1/memory.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/noncopyable.hpp>

#include "../../RanLogic/WorldBattle/WorldBattleDefine.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Club/GLClub.h"

namespace db
{
    class IDbManager;
}

namespace wb
{    
    class ClubCountry;
}

namespace jm
{
    enum EM_MSG_TYPE;
}

class GLClubAgent;
class GLAgentServer;

typedef std::tr1::unordered_map<DWORD, std::tr1::shared_ptr<GLClubAgent> > CLUB_AGENT;
typedef CLUB_AGENT::iterator                      CLUB_AGENT_ITER;
typedef CLUB_AGENT::const_iterator                CLUB_AGENT_CITER;
typedef CLUB_AGENT::value_type                    CLUB_AGENT_VALUE;

class GLClubManAgent : private boost::noncopyable
{
public:
    GLClubManAgent(GLAgentServer* pServer);
    ~GLClubManAgent();

private:
    enum { STORAGE_SAVE_FREQUENCY = 60, };

    CLUB_AGENT m_mapCLUB;
    CLUBSNAME  m_mapCLUBNAME;
    GLAgentServer* m_pServer;
    wb::ClubCountry* m_pClubCountry;
    int m_LastStorageSaveMin; //! 마지막으로 창고정보가 저장된 시간(분)
    DWORD m_AutoMemberKickTimer; //! 자동 멤버 탈퇴 시간 타이머

public:
    DWORD FindClub(const std::string& strName);
    std::tr1::shared_ptr<GLClubAgent> GetClub(DWORD dwID);
    std::tr1::shared_ptr<GLClubAgent> GetClub(const std::string& ClubName);
    void AddClub(std::tr1::shared_ptr<GLClubAgent> pClub);
    bool DelClub(DWORD dwID);
    bool LoadFromDB(db::IDbManager* pDBMan, db::IDbManager* pAdoMan);

protected:
    void LoadClubAuthFromDB(
        db::IDbManager* pAdoMan,
        DWORD ClubDbNum,
        std::tr1::shared_ptr<GLClubAgent> pClub);

public:
    HRESULT FrameMoveAgent(float fTime, float fElapsedTime, const CTime& CurrentTime);
    void FrameMoveDissolution(std::tr1::shared_ptr<GLClubAgent> pClub, std::vector<DWORD>& vClubDel);
    void FrameMoveClubBattle(std::tr1::shared_ptr<GLClubAgent> pClub, float fElapsedTime);
    void FrameMoveConfting(std::tr1::shared_ptr<GLClubAgent> pClub, float fElapsedTime);
    void FrameMoveAutoKick();

    void FrameMoveSaveClubStorage(const CTime& CurrentTime);
	void FrameMoveSaveClubStorageUser( std::tr1::shared_ptr< GLClubAgent > pClub, const CTime& CurrentTime );

    HRESULT EndClubBattle(DWORD dwClubID_P, DWORD dwClubID_S);
    bool InitClubBattle();
    HRESULT EndClubBattleAgent(DWORD dwClubID_P, DWORD dwClubID_S);
    void InitAllianceBattle();
    void DelAllianceBattle(DWORD dwClub_P, DWORD dwClub_S);
    HRESULT EndAllianceBattle(DWORD dwClubID_P, DWORD dwClubID_S);
    //void ChaLevelUp(DWORD ClubDbNum, DWORD ChaDbNum, WORD ChaLevel);

    const std::vector<wb::COUNTRY_CLUB_COUNT_INFO>& GetCountry();
    void SendCountryClubInfo(DWORD ClientSlot, wb::EM_COUNTRY CountryCode);
    void SendCountryClubInfo(std::tr1::shared_ptr<GLClubAgent> pClub, DWORD ClientSlot, jm::EM_MSG_TYPE MsgType);
    
    void SendClubClient(DWORD ClubDbNum, NET_MSG_GENERIC* pMsg);
    void SendClubClient(DWORD dwClubID, EMNET_MSG MsgType, const msgpack::sbuffer& PackBuffer, bool UseCrc=false);

    //void SetMemberMap(DWORD ClubDbNum, DWORD ChaDbNum, const SNATIVEID& MapId);
    void SAVESTORAGE2DB(DWORD ClubDbNum);

    void SaveStorage2DbAll();

    void SendClubInfo(DWORD ClientSlot, DWORD ClubDbNum);
    void SendClubInfoBase(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot);
    void SendClubInfoMember(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot);
    void SendClubInfoAlliance(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot);
    void SendClubInfoBattle(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot);
    void SendClubInfoAuth(std::tr1::shared_ptr<GLClubAgent> spClub, DWORD ClientSlot);

    void SendClubInfoToAllMember(DWORD ClubDbNum);

	bool ClubRename( DWORD ClubDbNum, const std::string& strNewName );
};