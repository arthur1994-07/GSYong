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
    // 전장 점수 얻어옴;
    int AdoManager::CompetitionRatingGetInfo(
                                         const std::string& strContentType,
                                         const DWORD charDbNum,
                                         int& Rating,
                                         int& WinningStreak,
                                         int& PlayCount,
                                         __time64_t& LastPlayDate
                                         )
    {
        /*
        @ChaNum			[int],
        @ContentType	[varchar](50),
        */

        sc::db::AdoExt Ado(m_GameDBConnString);
        APPEND_IPARAM_INT( Ado, "@ChaNum", charDbNum );
        APPEND_IPARAM_VARCHAR( Ado, "@ContentType", strContentType.c_str(), strContentType.length() );
        if ( Ado.ExecuteStoredProcedure("dbo.sp_CompetitionRatingGetInfo") == false )
        {
            return sc::db::DB_ERROR;
        }

        if( Ado.GetEOF() || Ado.GetBOF() )
        {
            return sc::db::DB_ERROR;
        }

        int nReturn;
        Ado.GetCollect( 0, nReturn);
        if ( ( nReturn == -1 ) || ( nReturn == -2 ) )
            return sc::db::DB_ERROR;

        Ado.GetCollect( 1, Rating );
        Ado.GetCollect( 2, WinningStreak );
        Ado.GetCollect( 3, PlayCount );

        _variant_t varLastDate;  // ExpirationDate
        Ado.GetCollect( 4, varLastDate );
        if( varLastDate.GetVARIANT().vt == VT_DATE )
            LastPlayDate = sc::time::GetTime(varLastDate);

        return sc::db::DB_OK;
    }

    // 전장 점수 저장;
    int AdoManager::CompetitionRatingUpdate(
                                        const std::string& strContentType,
                                        const DWORD charDbNum,
                                        const int Rating,
                                        const int WinningStreak,
                                        const int PlayCount,
                                        const __time64_t tLastPlayDate
                                        )
    {
        /*
        @ChaNum			[int],
        @ContentType	[varchar](50),
        @Rating			[int]
        @WinningStreak	[int]
        @PlayCount		[int]
        */

        sc::db::AdoExt Ado(m_GameDBConnString);
        APPEND_IPARAM_INT( Ado, "@ChaNum", charDbNum );
        APPEND_IPARAM_VARCHAR( Ado, "@ContentType", strContentType.c_str(), strContentType.length() ); 
        APPEND_IPARAM_INT( Ado, "@Rating", Rating );
        APPEND_IPARAM_INT( Ado, "@WinningStreak", WinningStreak );
        APPEND_IPARAM_INT( Ado, "@PlayCount", PlayCount );

        std::string strDate     = sc::time::DateTimeFormat(tLastPlayDate, true, true);
        APPEND_IPARAM_VARCHAR(Ado, "@LastPlayDate" , strDate.c_str(), static_cast<long> (strDate.length()));

        return Ado.Execute4Cmd( "dbo.sp_CompetitionRatingUpdate", adCmdStoredProc );
    }

    // 전장 시즌 업데이트;
    int AdoManager::CompetitionSeasonUpdate(
                                        const std::string& strContentType,
                                        const int seasonNum,
                                        const int baseRating
                                        )
    {
        /*
        [ContentType]	[varchar](50),
        [SeasonNum]		[int],
        [BaseRating]	[int]
        */

        sc::db::AdoExt Ado(m_GameDBConnString);
        APPEND_IPARAM_VARCHAR( Ado, "@ContentType", strContentType.c_str(), strContentType.length() );
        APPEND_IPARAM_INT( Ado, "@SeasonNum", seasonNum );
        APPEND_IPARAM_INT( Ado, "@BaseRating", baseRating );

        return Ado.Execute4Cmd( "dbo.sp_CompetitionSeasonUpdate", adCmdStoredProc );
    }

}