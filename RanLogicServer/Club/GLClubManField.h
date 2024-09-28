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

class GLClubField;
class GLGaeaServer;

typedef std::tr1::unordered_map<DWORD, std::tr1::shared_ptr<GLClubField> > CLUB_FIELD;
typedef CLUB_FIELD::iterator                      CLUB_FIELD_ITER;
typedef CLUB_FIELD::const_iterator                CLUB_FIELD_CITER;
typedef CLUB_FIELD::value_type                    CLUB_FIELD_VALUE;

class GLClubManField : private boost::noncopyable
{
public:
    GLClubManField(GLGaeaServer* pServer);
    ~GLClubManField();

private:
    CLUB_FIELD m_mapCLUB;
    CLUBSNAME  m_mapCLUBNAME;
    GLGaeaServer* m_pServer;

public:
    std::tr1::shared_ptr<GLClubField> GetClub(DWORD dwID);
    void AddClub(std::tr1::shared_ptr<GLClubField> pClub);
    bool DelClub(DWORD dwID);

    bool LoadFromDB(db::IDbManager* pDBMan, db::IDbManager* pAdoMan);
    void LoadClubAuthFromDB(
        db::IDbManager* pAdoMan,
        DWORD ClubDbNum,
        std::tr1::shared_ptr<GLClubField> pClub,
        bool &OldClub);

    HRESULT FrameMoveField(float fTime, float fElapsedTime);
    bool InitClubBattle();
    HRESULT EndClubBattleField(DWORD dwClubID_P, DWORD dwClubID_S);
    void InitAllianceBattle();
    void SaveClubBattle();
    void ChaLevelUp(DWORD ClubDbNum, DWORD ChaDbNum, WORD ChaLevel);
    void SendClubClient(DWORD ClubDbNum, NET_MSG_GENERIC* pMsg, DWORD ExceptMember);
    void SendClubClient(DWORD ClubDbNum, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, DWORD ExceptMember, bool UseCrc=false);
	DWORD FindClub( const std::string& strName );
	bool ClubRename( DWORD ClubDbNum, const std::string& strNewName );
};

