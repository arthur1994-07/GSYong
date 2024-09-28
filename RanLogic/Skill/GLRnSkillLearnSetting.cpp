#include "pch.h"
#include <boost/foreach.hpp>

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Memory/SmartPtr.h"

#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"

#include "../GLogicDataMan.h"

#include "GLRnSkillLearnSetting.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool GLRnSkillLearnSetting::LoadFile( std::string strFileName )
{    
    if( strFileName.empty() == TRUE )
        return false;

    std::string strPath( GLOGIC::GetPath() );
    strPath.append( strFileName );

    CString strExt = sc::string::getFileExt( strFileName.c_str() );

    // 루아 파일 열기
    if( strExt.CompareNoCase( _T( "lua" ) ) == 0 )
    {

        char szName[ _MAX_FNAME ] = _T( "" );
        char szExt[ _MAX_EXT ] = _T( "" );
        _splitpath( strPath.c_str(), NULL, NULL, szName, szExt );
        StringCchCat( szName, _MAX_FNAME, szExt );

        std::auto_ptr< sc::BaseStream > pStream(
            GLOGIC::openfile_basestream(
            GLOGIC::UseLogicZipFile(),
            GLogicData::GetInstance().GetGlogicZipFile(),
            strPath.c_str(), 
            szName, 
            false,
            GLOGIC::UseLogicPackFile() ) );

        if( !pStream.get() )
        {
            sc::writeLogError(
                sc::string::format(
                "GLRnSkillLearnSetting::LoadFile(), File Not Found! Default Setting Use %1%", strPath ) );

            return false;
        }

        size_t nSize = static_cast< size_t >( pStream->GetFileSize() );
        void* pBuffer = malloc( nSize );
        pStream->ReadBuffer( pBuffer, 0 );
        CTypeSmartPtr cPtrData( pBuffer );

        try
        {
            sc::lua_init();

            if( !sc::lua_doBuffer( static_cast< char* >( pBuffer ), nSize, NULL ) )
            {
                MessageBox( NULL, sc::lua_getError().c_str(), strPath.c_str(), MB_OK );

                return false;
            }



            // 루아 파싱
            LuaPlus::LuaObject OptionList = sc::lua_getGlobalFromName("RnSkillLearnSetting");
            for (LuaPlus::LuaTableIterator OptionIter(OptionList); OptionIter; OptionIter.Next())
            {
                SLEARN_SETTING sSetting = {0};

                for (LuaPlus::LuaTableIterator IDiter(OptionIter.GetValue()); IDiter; IDiter.Next())
                {
                    sSetting.wMID = ( IDiter.GetValue().GetInteger() ); // 스킬의 MID
                    IDiter.Next();
                    sSetting.wSID = ( IDiter.GetValue().GetInteger() ); // 스킬의 SID
                    IDiter.Next();
                    for (LuaPlus::LuaTableIterator SettingIter(IDiter.GetValue()); SettingIter; SettingIter.Next())
                    {
                        sSetting.bUseScrool = SettingIter.GetValue().GetBoolean();  // 스크롤 사용여부
                        SettingIter.Next();
                        sSetting.dwUseMoney = SettingIter.GetValue().GetInteger(); // 게임머니 사용량
                    }

                    m_vecSetting.push_back( sSetting );
                }
            }
            return true;
        }
        catch( const LuaPlus::LuaException& e )
        {        
            MessageBox( NULL, e.GetErrorMessage(), strPath.c_str(), MB_OK );

            return false;
        }

        return false;
    }

    return false;

}


const DWORD GLRnSkillLearnSetting::GetUseMoney( WORD wMID, WORD wSID )
{
    BOOST_FOREACH( SLEARN_SETTING& sSetting, m_vecSetting  )
    {
        if( sSetting.wMID == wMID && sSetting.wSID == wSID )
            return sSetting.dwUseMoney;
    }

    return 0;
}

const bool GLRnSkillLearnSetting::GetUseScrool( WORD wMID, WORD wSID )
{
    BOOST_FOREACH( SLEARN_SETTING& sSetting, m_vecSetting  )
    {
        if( sSetting.wMID == wMID && sSetting.wSID == wSID )
            return sSetting.bUseScrool;
    }

    return false;
}
