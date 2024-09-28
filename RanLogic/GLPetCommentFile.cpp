#include "pch.h"

#include "../SigmaCore/File/DepthWithTextToFile.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/Lua/MinLua.h"

#include "../EngineLib/Common/IniLoader.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxMeshs/DxAniKeys.h"
#include "../EngineLib/G-Logic/GLogic.h"

#include "./NpcTalk/NpcTalk.h"

#include "./GLogicDataMan.h"
#include "./Item/GLItem.h"
#include "./GLLevelFile.h"
#include "./Crow/GLMobSchedule.h"
#include "./Skill/GLSkill.h"
#include "./Quest/GLQuest.h"
#include "./Post/GLPostData.h"
#include "./Activity/Activity.h"
#include "./GLPetCommentFile.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool CGLPetCommentFile::LOADFILE( const std::string& strName, bool bToolMode )
{
    std::string strPath( GLOGIC::GetPath() );
    strPath.append( strName );

    CString strExt = sc::string::getFileExt( strName.c_str() );
    if( strExt.CompareNoCase( _T( "lua" ) ) == 0 )
        return LOADFILE_LUA( strPath );

    return false;
}

void CGLPetCommentFile::LoadHelp(
              const std::string& strTitle,
              std::string* pList,
              int nCount )
{
    std::wstring ws;

    try
    {
        int i = 0;
        LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( strTitle.c_str() );
        for( LuaPlus::LuaTableIterator it( ob ); it.IsValid(); it.Next(), ++i )
        {
            if( i >= nCount )
                break;

            ws = reinterpret_cast< const wchar_t* >( it.GetValue().GetWString() );
            pList[ i ] = sc::string::unicodeToAnsi( ws );
        }
    }
    catch( const LuaPlus::LuaException& e )
    {        
        MessageBox( NULL, e.GetErrorMessage(), strTitle.c_str(), MB_OK );
    }
}

bool CGLPetCommentFile::ParseCommentLua()
{
    std::wstring ws;

    //==========================================================================

    try
    {
        LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "PET_DEFINE" ) );
        for( LuaPlus::LuaTableIterator ListIter( ob ); ListIter.IsValid(); ListIter.Next() )
        {
            for( LuaPlus::LuaTableIterator ValIter( ListIter.GetValue() ); ValIter.IsValid(); ValIter.Next() )
            {
                int IntType = ValIter.GetValue().GetInteger();
                COMMENT::PET_DEFINE.push_back( IntType );
                ValIter.Next();

                ws = reinterpret_cast< const wchar_t* >( ValIter.GetValue().GetWString() );
                COMMENT::PET_TYPE.push_back( sc::string::unicodeToAnsi( ws ) );
                ValIter.Next();

                ws = reinterpret_cast< const wchar_t* >( ValIter.GetValue().GetWString() );
                COMMENT::szPetInitFile.push_back( sc::string::unicodeToAnsi( ws ) );
            }
        }
    }
    catch( const LuaPlus::LuaException& e )
    {        
        MessageBox( NULL, e.GetErrorMessage(), _T( "PET_DEFINE" ), MB_OK );
    }

    //==========================================================================

    try
    {
        LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "PET_STYLECARD_TEXT" ) );
        for( LuaPlus::LuaTableIterator ListIter( ob ); ListIter.IsValid(); ListIter.Next() )
        {
            ws = reinterpret_cast< const wchar_t* >( ListIter.GetValue().GetWString() );
            COMMENT::szPetStyleCardText.push_back( sc::string::unicodeToAnsi( ws ) );
        }
    }
    catch( const LuaPlus::LuaException& e )
    {        
        MessageBox( NULL, e.GetErrorMessage(), _T( "PET_STYLECARD_TEXT" ), MB_OK );
    }

    //==========================================================================

    try
    {
        LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "PET_STYLECARD_UI" ) );
        for( LuaPlus::LuaTableIterator ListIter( ob ); ListIter.IsValid(); ListIter.Next() )
        {
            for( LuaPlus::LuaTableIterator ValIter( ListIter.GetValue() ); ValIter.IsValid(); ValIter.Next() )
            {
                WORD wPetType, wPetStyle;

                WORD wWindowPosX, wWindowPosY, wWindowWidth, wWindowHeight;

                std::string strTexture;
                WORD wTextureSizeX, wTextureSizeY;
                WORD wTexturePosX, wTexturePosY, wTextureWidth, wTextureHeight;

                wPetType = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wPetStyle = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wWindowPosX = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wWindowPosY = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wWindowWidth = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wWindowHeight = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                ws = reinterpret_cast< const wchar_t* >( ValIter.GetValue().GetWString() );
                strTexture = sc::string::unicodeToAnsi( ws );
                ValIter.Next();

                wTextureSizeX = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wTextureSizeY = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wTexturePosX = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wTexturePosY = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wTextureWidth = static_cast<WORD>(ValIter.GetValue().GetInteger());
                ValIter.Next();

                wTextureHeight = static_cast<WORD>(ValIter.GetValue().GetInteger());

                PET_STYLECARD_UI sPetCardUI( wPetType, wPetStyle,
                    wWindowPosX, wWindowPosY, wWindowWidth, wWindowHeight,
                    strTexture, wTextureSizeX, wTextureSizeY,
                    wTexturePosX, wTexturePosY, wTextureWidth, wTextureHeight );

                COMMENT::PetStyleCardUI.push_back( sPetCardUI );
            }
        }
    }
    catch( const LuaPlus::LuaException& e )
    {        
        MessageBox( NULL, e.GetErrorMessage(), _T( "PET_STYLECARD_UI" ), MB_OK );
    }

    //==========================================================================


    return true;
}

bool CGLPetCommentFile::LOADFILE_LUA( const std::string& strPath )
{
    if( strPath.empty() )
        return false;

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
            "CGLPetCommentFile::LOADFILE_LUA(), File Open %1%", strPath ) );

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

        return ParseCommentLua();
    }
    catch( const LuaPlus::LuaException& e )
    {        
        MessageBox( NULL, e.GetErrorMessage(), strPath.c_str(), MB_OK );

        return false;
    }

    return false;
}