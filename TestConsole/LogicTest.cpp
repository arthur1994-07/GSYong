#include "stdafx.h"
#include <boost/crc.hpp>
#include <boost/tr1/memory.hpp>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/regex.hpp>
#include <WinSock2.h>
#include "../=CryptoPP/sha.h"
#include "../=CryptoPP/iterhash.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Math/Random.h"

#include "../EngineLib/Common/SUBPATH.h"

#include "../RanLogic/WorldBattle/WorldBattleCountry.h"
#include "../RanLogic/GLogicFile.h"
#include "../RanLogic/Club/ClubAuthData.h"

#include "../RanLogicClient/Command/dxincommand.h"

#include "../RanLogicServer/Util/GLItemLimit.h"
#include "../RanLogicServer/Util/IpAddressChecker.h"
#include "../RanLogicServer/Database/ODBC/s_COdbcManager.h"
#include "../RanLogicServer/Database/ODBC/s_COdbcSupervisor.h"
#include "../RanLogicServer/Database/ODBC/OdbcStmt.h"
#include "../RanLogicServer/Club/ClubLogMan.h"

#include "./LogicTest.h"

LogicTest::LogicTest()
    : m_pPath(NULL)
    , m_pLogicFile(NULL)
    , m_pCountry(NULL)
    , m_pClubAuthData(NULL)
	, m_pGameCommand(NULL)
    , m_pClubLog(NULL)
{   
}

LogicTest::~LogicTest()
{
    delete m_pClubLog;
	delete m_pGameCommand;
    delete m_pClubAuthData;
    delete m_pCountry;
    delete m_pLogicFile;
    delete m_pPath;
}

void LogicTest::Init()
{
    m_pPath = new SUBPATH(SP_WORLD_BATTLE); 

    m_pLogicFile = new GLogicFile;
    m_pLogicFile->LoadCountryFile();
    m_pCountry = new wb::Country;
    m_pClubAuthData = new club::ClubDefineData;
	m_pGameCommand = new GameCommand(NULL, NULL);
    m_pClubLog = new club::LogManServer;
}

void LogicTest::DoTest()
{
    WorldBattleCountryLoad();
    ClubAutorityDataLoad();
	GameCommandTest();
    ClubLogTest();
}

void LogicTest::ClubLogTest()
{
    for (int i=0; i<1000; ++i)
    {
        WORD ClubDbNum = sc::Random::getInstance().get();
        ClubDbNum = ClubDbNum % 100;
        WORD LogType = sc::Random::getInstance().getPercent();
        LogType = LogType % 5;
        UUID Uuid;
        sc::string::getUUID(Uuid);
        std::string LogStr = sc::string::uuidToString(Uuid);
        m_pClubLog->Insert(ClubDbNum, static_cast<club::EM_LOG_TYPE> (LogType), LogStr);
    }

    m_pClubLog->DeleteOneDayData(1333520689);
}

void LogicTest::GameCommandTest()
{
	m_pGameCommand->command(std::string("/month"));
}

void LogicTest::ClubAutorityDataLoad()
{
    std::string FileName(sc::getAppPath());
    FileName.append(m_pPath->GLogicPath());
    FileName.append(m_pLogicFile->GetClubDefineFile());
    m_pClubAuthData->Load(FileName);
    std::cout << "DescSize " << m_pClubAuthData->GetDescSize() << std::endl;
    for (size_t i=0; i<m_pClubAuthData->GetDescSize(); ++i)
    {
        std::cout << i << " Desc:" << m_pClubAuthData->GetDesc(i) << std::endl;
    }

    for (size_t i=0; i<m_pClubAuthData->GetGradeSize()+1; ++i)
    {
        club::AUTH_GRADE Grade = m_pClubAuthData->GetGrade(i);
        std::cout << i << " Grade Desc:" << Grade.m_Desc << " " << Grade.m_Flag << std::endl;
    }

    std::cout << club::ClubLogDeleteTime << std::endl;
}

void LogicTest::WorldBattleCountryLoad()
{    
    std::string WorldBattleCountyFile(sc::getAppPath());
    WorldBattleCountyFile.append(m_pPath->GLogicPath());
    WorldBattleCountyFile.append(m_pLogicFile->GetWorldBattleCountryFile());
    
    m_pCountry->Load(SP_WORLD_BATTLE, WorldBattleCountyFile);

    std::vector<wb::COUNTRY_INFO> vResult;
    m_pCountry->Search("ko", vResult);

    for (size_t i=0; i<vResult.size(); ++i)
    {
        const wb::COUNTRY_INFO& Code = vResult[i];
        std::cout << Code.m_Code << " " << Code.m_FullName << " " << Code.m_Alpha2Code << " " << Code.m_Alpha3Code << " " << Code.m_Desc << " " << std::endl;
    }
}
