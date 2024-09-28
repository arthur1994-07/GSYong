#include "pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../SigmaCore/String/MinGuid.h"

#include "../../../SigmaCore/Util/DateTime.h"

//#include "../DbDefineLog.h"
#include "./ADOManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{
int AdoManager::InstantDungeonLogTypeInsert(
    const db::EM_INSTANT_LOG_TYPE& nLogType,
    const char* strParamName1,  
    const char* strParamName2,  
    const char* strParamName3,  
    const char* strParamName4,  
    const char* strParamName5,
    const char* strParamName6,  
    const char* strParamName7,  
    const char* strParamName8,  
    const char* strParamName9,  
    const char* strParamName10,
    const char* strParamName11, 
    const char* strParamName12, 
    const char* strParamName13, 
    const char* strParamName14, 
    const char* strParamName15,
    const char* strParamName16, 
    const char* strParamName17, 
    const char* strParamName18, 
    const char* strParamName19, 
    const char* strParamName20 )
{

/*
    @LogType int,
    @ParamName1 varchar(30),
    @ParamName2 varchar(30),
    @ParamName3 varchar(30),
    @ParamName4 varchar(30),
    @ParamName5 varchar(30),
    @ParamName6 varchar(30),
    @ParamName7 varchar(30),
    @ParamName8 varchar(30),
    @ParamName9 varchar(30),
    @ParamName10 varchar(30),
    @ParamName11 varchar(30),
    @ParamName12 varchar(30),
    @ParamName13 varchar(30),
    @ParamName14 varchar(30),
    @ParamName15 varchar(30),
    @ParamName16 varchar(30),
    @ParamName17 varchar(30),
    @ParamName18 varchar(30),
    @ParamName19 varchar(30),
    @ParamName20 varchar(30),
    @nReturn int OUTPUT
*/
    std::vector<std::string> vecTemp;
    vecTemp.reserve(20);
    vecTemp.push_back( (strParamName1?strParamName1:std::string()) ); 
    vecTemp.push_back( (strParamName2?strParamName2:std::string()) );
    vecTemp.push_back( (strParamName3?strParamName3:std::string()) );
    vecTemp.push_back( (strParamName4?strParamName4:std::string()) );
    vecTemp.push_back( (strParamName5?strParamName5:std::string()) );
    vecTemp.push_back( (strParamName6?strParamName6:std::string()) );
    vecTemp.push_back( (strParamName7?strParamName7:std::string()) );
    vecTemp.push_back( (strParamName8?strParamName8:std::string()) );
    vecTemp.push_back( (strParamName9?strParamName9:std::string()) );
    vecTemp.push_back( (strParamName10?strParamName10:std::string()) );
    vecTemp.push_back( (strParamName11?strParamName11:std::string()) ); 
    vecTemp.push_back( (strParamName12?strParamName12:std::string()) );
    vecTemp.push_back( (strParamName13?strParamName13:std::string()) );
    vecTemp.push_back( (strParamName14?strParamName14:std::string()) );
    vecTemp.push_back( (strParamName15?strParamName15:std::string()) );
    vecTemp.push_back( (strParamName16?strParamName16:std::string()) );
    vecTemp.push_back( (strParamName17?strParamName17:std::string()) );
    vecTemp.push_back( (strParamName18?strParamName18:std::string()) );
    vecTemp.push_back( (strParamName19?strParamName19:std::string()) );
    vecTemp.push_back( (strParamName20?strParamName20:std::string()) );

    sc::db::AdoExt Ado(m_LogDBConnString);
    APPEND_IPARAM_INT( Ado, "@LogType", nLogType );

    for( int i=0; i<20; i++ )
    {
        std::string strName = sc::string::format( "%s%d", "@ParamName", i+1 );
        std::string& strInput = vecTemp[i];

        if( strInput.empty() )
            APPEND_IPARAM_INT( Ado, strName.c_str(), NULL );
        else
            APPEND_IPARAM_VARCHAR( Ado, strName.c_str(), strInput.c_str(), strInput.length() );
    }
    APPEND_OPARAM_INT(Ado, "@nReturn");

    return Ado.ExecuteStoredProcedureIntReturn("dbo.InstantDungeonLogTypeInsert", "@nReturn");
}

int AdoManager::InstantDungeonLogInsert(
    const db::EM_INSTANT_LOG_TYPE& nLogType,
    const char* strParam1,  
    const char* strParam2,  
    const char* strParam3,  
    const char* strParam4,  
    const char* strParam5,
    const char* strParam6,  
    const char* strParam7,  
    const char* strParam8,  
    const char* strParam9,  
    const char* strParam10,
    const char* strParam11, 
    const char* strParam12, 
    const char* strParam13, 
    const char* strParam14, 
    const char* strParam15,
    const char* strParam16, 
    const char* strParam17, 
    const char* strParam18, 
    const char* strParam19, 
    const char* strParam20 )
{
/*
    @LogType int,
    @Parameter1 varchar(30),
    @Parameter2 varchar(30),
    @Parameter3 varchar(30),
    @Parameter4 varchar(30),
    @Parameter5 varchar(30),
    @Parameter6 varchar(30),
    @Parameter7 varchar(30),
    @Parameter8 varchar(30),
    @Parameter9 varchar(30),
    @Parameter10 varchar(30),
    @Parameter11 varchar(30),
    @Parameter12 varchar(30),
    @Parameter13 varchar(30),
    @Parameter14 varchar(30),
    @Parameter15 varchar(30),
    @Parameter16 varchar(30),
    @Parameter17 varchar(30),
    @Parameter18 varchar(30),
    @Parameter19 varchar(30),
    @Parameter20 varchar(30),
    @nReturn int OUTPUT
*/
    std::vector<std::string> vecTemp;
    vecTemp.reserve(20);
    vecTemp.push_back( (strParam1?strParam1:std::string()) ); 
    vecTemp.push_back( (strParam2?strParam2:std::string()) );
    vecTemp.push_back( (strParam3?strParam3:std::string()) );
    vecTemp.push_back( (strParam4?strParam4:std::string()) );
    vecTemp.push_back( (strParam5?strParam5:std::string()) );
    vecTemp.push_back( (strParam6?strParam6:std::string()) );
    vecTemp.push_back( (strParam7?strParam7:std::string()) );
    vecTemp.push_back( (strParam8?strParam8:std::string()) );
    vecTemp.push_back( (strParam9?strParam9:std::string()) );
    vecTemp.push_back( (strParam10?strParam10:std::string()) );
    vecTemp.push_back( (strParam11?strParam11:std::string()) ); 
    vecTemp.push_back( (strParam12?strParam12:std::string()) );
    vecTemp.push_back( (strParam13?strParam13:std::string()) );
    vecTemp.push_back( (strParam14?strParam14:std::string()) );
    vecTemp.push_back( (strParam15?strParam15:std::string()) );
    vecTemp.push_back( (strParam16?strParam16:std::string()) );
    vecTemp.push_back( (strParam17?strParam17:std::string()) );
    vecTemp.push_back( (strParam18?strParam18:std::string()) );
    vecTemp.push_back( (strParam19?strParam19:std::string()) );
    vecTemp.push_back( (strParam20?strParam20:std::string()) );

    sc::db::AdoExt Ado(m_LogDBConnString);

    // 시작 로그를 남긴다.
    __time64_t currentTime  = CTime::GetCurrentTime().GetTime();
    std::string StrDate     = sc::time::DateTimeFormat(currentTime, true, true);
    APPEND_IPARAM_VARCHAR(Ado, "@LogDate", StrDate.c_str(), StrDate.length());

    APPEND_IPARAM_INT( Ado, "@LogType", nLogType );

    for( int i=0; i<20; i++ )
    {
        std::string strName = sc::string::format( "%s%d", "@Parameter", i+1 );
        std::string& strInput = vecTemp[i];
        
            
        if( strInput.empty() )
            APPEND_IPARAM_INT( Ado, strName.c_str(), NULL );
        else
            APPEND_IPARAM_VARCHAR( Ado, strName.c_str(), strInput.c_str(), strInput.length() );
    }
    APPEND_OPARAM_INT(Ado, "@nReturn");
  
    return Ado.ExecuteStoredProcedureIntReturn("dbo.InstantDungeonLogInsert", "@nReturn");
}




}