#ifndef _LOGIC_TEST_H_
#define _LOGIC_TEST_H_

#pragma once

class SUBPATH;
class GLogicFile;
class GameCommand;

namespace wb
{
    class Country;
}

namespace club
{
    class ClubDefineData;
    class LogManServer;
}

class LogicTest
{
public:
    LogicTest();
    ~LogicTest();

protected:
    SUBPATH* m_pPath;
    GLogicFile* m_pLogicFile;
    wb::Country* m_pCountry;
    club::ClubDefineData* m_pClubAuthData;
	GameCommand* m_pGameCommand;
    club::LogManServer* m_pClubLog;

public:
    void Init();
    void DoTest();
    
protected:
    void WorldBattleCountryLoad();
    void ClubAutorityDataLoad();
	void GameCommandTest();
    void ClubLogTest();
};

#endif // _LOGIC_TEST_H_