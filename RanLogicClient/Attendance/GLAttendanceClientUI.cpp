#include "pch.h"

#include "../../SigmaCore/Xml/2.5.3/ticpp.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/gassert.h"

#include "../../=LuaPlus/src/LuaPlus/LuaPlus.h"

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Attendance/DayN.h"

#include "../GLGaeaClient.h"
#include "./GLAttendanceClient.h"

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/mem_fn.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace attendance;

//--------------------------------------------------------------------------------------------//

SOBJECTTEXT::SOBJECTTEXT() 
: nID(0)
{
    funcCAPTION_1 = SOBJECTTEXT::GETNULLFUNCSTR ();
    funcCAPTION_2 = SOBJECTTEXT::GETNULLFUNCSTR ();
    funcPROGRESS  = SOBJECTTEXT::GETNULLFUNCSTR ();
}

SOBJECTTEXT::FUNCSTR SOBJECTTEXT::GETNULLFUNCSTR ()
{
    using namespace boost::lambda;
    return boost::lambda::constant( std::string("(null)") );
}

SOBJECTTEXT::FUNCSTR SOBJECTTEXT::GETDEFAULTFUNCSTR ( const std::string& strFormat )
{
    using namespace boost::lambda;
    return boost::lambda::bind(&sc::string::format<UINT,UINT>, strFormat, 
        boost::lambda::bind( &TaskN::GetValue,     boost::lambda::_1 ),
        boost::lambda::bind( &TaskN::GetValueBase, boost::lambda::_1 ) );
}

SOBJECTTEXT::FUNCSTR SOBJECTTEXT::GETFUNCSTR ( const std::string& strFormat, FUNCSTR& funcStr )
{
    using namespace boost::lambda;
    return strFormat + boost::lambda::bind(funcStr, boost::lambda::_1);
}

SOBJECTTEXT::FUNCSTR SOBJECTTEXT::GETFUNCSTR ( FUNCSTR& funcStr1, FUNCSTR& funcStr2 )
{
    using namespace boost::lambda;
    return boost::lambda::bind(funcStr1, boost::lambda::_1) + boost::lambda::bind(funcStr2, boost::lambda::_1);
}

SOBJECTTEXT::FUNCSTR SOBJECTTEXT::GETFUNCSTR ( FUNCSTR& funcStr1, FUNCWSTR& funcWStr2 )
{
    using namespace boost::lambda;
    return boost::lambda::bind(funcStr1, boost::lambda::_1)
           + boost::lambda::bind( &sc::string::unicodeToAnsi, boost::lambda::bind(funcWStr2, boost::lambda::_1) );
}

SOBJECTTEXT::FUNCSTR SOBJECTTEXT::GETFUNCSTR ( FUNCSTR& funcStr1, FUNCLUAWSTR& funcLuaWStr2 )
{
    using namespace boost::lambda;
    return GETFUNCSTR ( funcStr1, FUNCWSTR(boost::lambda::ll_reinterpret_cast<const wchar_t*>(boost::lambda::bind(funcLuaWStr2, boost::lambda::_1) ) ) );
}

SOBJECTTEXT::FUNCSTR SOBJECTTEXT::MAKE_TYPE_FUNC ( const std::string& strType, const EMSTYLE emStyle )
{
    SOBJECTTEXT::FUNCSTR FuncStr;

    // 예외처리 //
    // 시간 타입일 경우
    if ( strType == "TIME" )
    {
        switch ( emStyle )
        {
        case EMSTYLE_CAPTION:  FuncStr = &SOBJECTTEXT::CAPTION_TYPE_TIME;  break;
        case EMSTYLE_PROGRESS: FuncStr = &SOBJECTTEXT::PROGRESS_TYPE_TIME; break;
        };
    }
    // 포인트 상점 타입일 경우
    else if ( strType == "CASH" )
    {
        switch ( emStyle )
        {
        case EMSTYLE_CAPTION:  FuncStr = &SOBJECTTEXT::CAPTION_TYPE_CASH; break;
        case EMSTYLE_PROGRESS: FuncStr = &SOBJECTTEXT::PROGRESS_TYPE_CASH; break;
        };
    }
    // 아이템 타입일 경우
    else if ( strType == "ITEM" )
    {
        switch ( emStyle )
        {
        case EMSTYLE_CAPTION:  FuncStr = &SOBJECTTEXT::CAPTION_TYPE_ITEM;  break;
        case EMSTYLE_PROGRESS: FuncStr = &SOBJECTTEXT::PROGRESS_TYPE_ITEM; break;
        };
    }
    // 아이템과 갯수 혼용타입일 경우
    else if ( strType == "ITEM_NUMS" )
    {
        switch ( emStyle )
        {
        case EMSTYLE_CAPTION:  FuncStr = &SOBJECTTEXT::CAPTION_TYPE_ITEM_NUMS;  break;
        case EMSTYLE_PROGRESS: FuncStr = &SOBJECTTEXT::PROGRESS_TYPE_ITEM_NUMS; break;
        };
    }
    else
    {
        std::string strTYPEID  = ( std::string("ATTENDANCE_TASK_VALUETYPE_") + strType );
        FuncStr = SOBJECTTEXT::GETDEFAULTFUNCSTR( ID2GAMEWORD( strTYPEID .c_str(), emStyle ) );
    }

    return FuncStr;
}

std::string SOBJECTTEXT::CAPTION_TYPE_TIME ( TaskN* pTask )
{
    std::string strTime = "";

    UINT nMin  = pTask->GetValueBase()%60;
    UINT nHour = pTask->GetValueBase()/60;

    if ( nHour          ) strTime += sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_TIME", 0), nHour ) + " ";
    if ( nMin || !nHour ) strTime += sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_TIME", 1), nMin  );
    
    return strTime;
}

std::string SOBJECTTEXT::PROGRESS_TYPE_TIME ( TaskN* pTask )
{
    std::string strTime = "";

    if ( pTask->IsCompleted() )
    {
        strTime = ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_TIME", 2);
    }
    else
    {
        UINT nMin  = pTask->GetValue()%60;
        UINT nHour = pTask->GetValue()/60;

        if ( nHour          ) strTime += sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_TIME", 0), nHour ) + " ";
        if ( nMin || !nHour ) strTime += sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_TIME", 1), nMin  );
    }

    return strTime;
}

std::string SOBJECTTEXT::CAPTION_TYPE_CASH ( TaskN* pTask )
{
    return sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_CASH", 0), pTask->GetValueBase() );
}

std::string SOBJECTTEXT::PROGRESS_TYPE_CASH ( TaskN* pTask )
{
    if ( pTask->IsCompleted() || (pTask->GetValueBase() <= pTask->GetValue()) )
    {
        return ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_CASH", 2);
    }

    return sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_CASH", 1), pTask->GetValue(), pTask->GetValueBase() );
}

std::string SOBJECTTEXT::CAPTION_TYPE_ITEM ( TaskN* pTask )
{
    return sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_ITEM", 0), "임시 아이템" );
}

std::string SOBJECTTEXT::PROGRESS_TYPE_ITEM ( TaskN* pTask )
{
    if ( pTask->IsCompleted() )
    {
        return ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_ITEM", 2);
    }

    return sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_ITEM", 1), "임시 아이템" );
}

std::string SOBJECTTEXT::CAPTION_TYPE_ITEM_NUMS ( TaskN* pTask )
{
    return sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_ITEM_NUMS", 0), pTask->GetValue(), pTask->GetValueBase() );
}

std::string SOBJECTTEXT::PROGRESS_TYPE_ITEM_NUMS ( TaskN* pTask )
{
    if ( pTask->IsCompleted() )
    {
        return ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_ITEM_NUMS", 2);
    }

    return sc::string::format( ID2GAMEWORD("ATTENDANCE_TASK_VALUETYPE_ITEM_NUMS", 0), pTask->GetValue(), pTask->GetValueBase() );
}

//--------------------------------------------------------------------------------------------//

const bool GLAttendanceClient::UILoad ()
{
    // bjju.studentrecord
    const SUBPATH* pPath = m_pGaeaClient->GetSubPath();
    std::string strFile(sc::getAppPath());
    strFile.append(pPath->GLogicPath()); //SUBPATH::GLogicPath());
    strFile.append(GLogicData::GetInstance().GetAttendanceUiOptionFileName());
    m_strUIFilePath = strFile;

    if ( m_strUIFilePath.empty() )
    {
        GASSERT( !"GLAttendanceClient::LoadUI() - m_strUIFilePath is empty");
        return false;
    }

    USHORT nPos = (USHORT)m_strUIFilePath.find_last_of(".");
    std::string strExtension( (nPos == std::string::npos) ? "" : m_strUIFilePath.substr( nPos ) );
    std::transform( strExtension.begin(), strExtension.end(), strExtension.begin(), std::toupper );

    bool bSuccess = false;
    if( strExtension == ".LUA" )
    {
        bSuccess = UIParseLua ();
    }
    else if ( strExtension == ".XML")
    {
        bSuccess = UIParseXML ();
    }
    
    if ( !bSuccess )
    {
        UIClear ();

        sc::writeLogError(std::string("GLAttendanceClient::LoadUI () - parse failed"));
        return false;
    }

    return true;
}

void GLAttendanceClient::UIClear ()
{
    m_ObjectTextMap.clear();
    m_strUIFilePath.clear();

    if ( m_pLuaStateUi )
    {
        LuaPlus::LuaState::Destroy(m_pLuaStateUi);
        m_pLuaStateUi = NULL;
    }

    if ( m_pXmlDoc )
    {
        SAFE_DELETE( m_pXmlDoc );
    }
}

const bool GLAttendanceClient::UIParseXML ()
{
    try
    {
        if ( m_pXmlDoc )
        {
            SAFE_DELETE( m_pXmlDoc );
        }

        // OpenFile
        m_pXmlDoc = new ticpp::Document;
        m_pXmlDoc->LoadFile(m_strUIFilePath, TIXML_ENCODING_UTF8);
     
        ticpp::Element* pObjectText = (ticpp::Element*) m_pXmlDoc->FirstChild( "OBJECTTEXT" );
        ticpp::Element* pTask       = (ticpp::Element*) pObjectText->FirstChild( "TASK" );

        UINT nCurID = 0;

        do
        {
            SOBJECTTEXT sObjectText;

            // [ID] ----------------------------------------
            ticpp::Element* pID = (ticpp::Element*) pTask->FirstChild( "ID" );
            pID->GetTextOrDefault( &nCurID, nCurID );
            sObjectText.nID = nCurID;
            
            // [XML] ----------------------------------------
            ticpp::Element* pXMLCAPTIONID   = (ticpp::Element*) pTask->FirstChild( "XMLCAPTIONID", false );
            std::string     strXMLCAPTIONID = "ATTENDANCE_TASK_CAPTION";

            if ( pXMLCAPTIONID )
            {
                strXMLCAPTIONID = pXMLCAPTIONID->GetText();
            }

            ticpp::Element* pXMLPROGRESSID   = (ticpp::Element*) pTask->FirstChild( "XMLPROGRESSID", false );
            std::string     strXMLPROGRESSID = "ATTENDANCE_TASK_PROGRESS";

            if ( pXMLPROGRESSID )
            {
                strXMLPROGRESSID = pXMLPROGRESSID->GetText();
            }

            ticpp::Element* pXMLINDEX = (ticpp::Element*) pTask->FirstChild( "XMLINDEX", false );
            UINT            nXMLIndex = nCurID;

            if ( pXMLINDEX )
            {
                pID->GetTextOrDefault( &nXMLIndex, nCurID );
            }

            // [TYPE] ----------------------------------------
            ticpp::Element* pTYPECAPTION  = (ticpp::Element*) pTask->FirstChild( "TYPE_CAPTION", false  );
            ticpp::Element* pTYPEPROGRESS = (ticpp::Element*) pTask->FirstChild( "TYPE_PROGRESS", false );

            std::string strTYPECAPTION;
            std::string strTYPEPROGRESS;

            if ( pTYPECAPTION && pTYPEPROGRESS )
            {
                strTYPECAPTION  = pTYPECAPTION->GetText();
                strTYPEPROGRESS = pTYPEPROGRESS->GetText();
            }
            else
            {
                ticpp::Element* pTYPE = (ticpp::Element*) pTask->FirstChild( "TYPE" );
                strTYPECAPTION  = pTYPE->GetText();
                strTYPEPROGRESS = pTYPE->GetText();
            }

            SOBJECTTEXT::FUNCSTR FuncCaptionType  = SOBJECTTEXT::MAKE_TYPE_FUNC( strTYPECAPTION,  SOBJECTTEXT::EMSTYLE_CAPTION );
            SOBJECTTEXT::FUNCSTR FuncProgressType = SOBJECTTEXT::MAKE_TYPE_FUNC( strTYPEPROGRESS, SOBJECTTEXT::EMSTYLE_PROGRESS );

            std::string strCAPTION  = ID2GAMEWORD( strXMLCAPTIONID.c_str() , nXMLIndex );
            std::string strPROGRESS = ID2GAMEWORD( strXMLPROGRESSID.c_str(), nXMLIndex );

            sObjectText.funcCAPTION_1 = SOBJECTTEXT::GETDEFAULTFUNCSTR( strCAPTION + " " );
            sObjectText.funcCAPTION_2 = FuncCaptionType;

            sObjectText.funcPROGRESS  = SOBJECTTEXT::GETFUNCSTR( strPROGRESS + " ", FuncProgressType );

            // Insert
            std::pair<SOBJTEXTMAP_ITER,bool> retiter = m_ObjectTextMap.insert( std::make_pair(nCurID, sObjectText) );

            if ( retiter.second==false )
            {
                ::MessageBoxA(
                    NULL,
                    sc::string::format("nID = %1%, ID '%1%' already existed", nCurID).c_str(),
                    m_strUIFilePath.c_str(),
                    MB_OK);
            }

            ++nCurID;

        } while ( pTask = (ticpp::Element*)pTask->NextSiblingElement(false) );
    }
    catch (ticpp::Exception& ex)
    {
        // Report the error
        ::MessageBoxA(
            NULL,
            ex.what(),
            m_strUIFilePath.c_str(),
            MB_OK);
   
        return false;
    }

    return true;
}

const bool GLAttendanceClient::UIParseLua ()
{
    // OpenFile
    try {
        if ( m_pLuaStateUi )
        {
            LuaPlus::LuaState::Destroy(m_pLuaStateUi);
            m_pLuaStateUi = NULL;
        }

        m_pLuaStateUi = LuaPlus::LuaState::Create(true);

        if ( !m_pLuaStateUi )
        {
            ::MessageBoxA(
                NULL,
                LuaPlus::LuaStackObject(m_pLuaStateUi, -1).GetString(),
                m_strUIFilePath.c_str(),
                MB_OK);

            sc::writeLogError(std::string("GLAttendanceClient::LoadUI () - create luastate failed"));
            return false;
        }

        if ( LUA_ERRFILE == m_pLuaStateUi->LoadFile( m_strUIFilePath.c_str() ) )
        {
            ::MessageBoxA(
                NULL,
                LuaPlus::LuaStackObject(m_pLuaStateUi, -1).GetString(),
                m_strUIFilePath.c_str(),
                MB_OK);

            sc::writeLogError(std::string("GLAttendanceClient::LoadUI () - loadfile failed"));
            return false;
        }
    }
    catch (const LuaPlus::LuaException& e)
    {        
        ::MessageBoxA(
            NULL,
            e.GetErrorMessage(),
            m_strUIFilePath.c_str(),
            MB_OK);

        return false;
    }

    // Parse
    try {
        // (unused)
        //LuaPlus::LuaObject ObjectTextTable = m_pLuaStateUi->GetGlobals()["ObjectText"];
        //for (LuaPlus::LuaTableIterator it(ObjectTextTable); it; it.Next())
        //{
        //    LuaPlus::LuaObject objText = it.GetValue();
        //    SOBJECTTEXT sObjectText;
        //    bool        bComplete = false;

        //    // [ID] ----------------------------------------
        //    int nID = objText["ID"].GetInteger();
        //    sObjectText.nID = (UINT)nID;

        //    // [XML] ----------------------------------------
        //    LuaPlus::LuaObject objXMLCAPTIONID = objText["XMLCAPTIONID"];
        //    std::string        strXMLCAPTIONID = "ATTENDANCE_TASK_CAPTION";

        //    if ( !objXMLCAPTIONID.IsNil() )
        //    {
        //        std::wstring wstrText( (wchar_t*)objXMLCAPTIONID.GetWString() );
        //        strXMLCAPTIONID = sc::string::unicodeToAnsi(wstrText);
        //    }

        //    LuaPlus::LuaObject objXMLPROGRESSID = objText["XMLPROGRESSID"];
        //    std::string        strXMLPROGRESSID = "ATTENDANCE_TASK_PROGRESS";

        //    if ( !objXMLPROGRESSID.IsNil() )
        //    {
        //        std::wstring wstrText( (wchar_t*)objXMLPROGRESSID.GetWString() );
        //        strXMLPROGRESSID = sc::string::unicodeToAnsi(wstrText);
        //    }

        //    LuaPlus::LuaObject objXMLINDEX = objText["XMLINDEX"];
        //    UINT               nXMLIndex   = nID;

        //    if ( !objXMLINDEX.IsNil() )
        //    {
        //        nXMLIndex = (UINT)objXMLINDEX.GetInteger();
        //    }

        //    // [TYPE] ----------------------------------------
        //    LuaPlus::LuaObject objType = objText["TYPE"];

        //    if ( !objType.IsNil() )
        //    {
        //        std::wstring wstrText  ( (wchar_t*)objType.GetWString() );
        //        std::string  strType   ( sc::string::unicodeToAnsi(wstrText) );
        //        std::string  strWORDID ( std::string("ATTENDANCE_TASK_VALUETYPE_") + strType );

        //        SOBJECTTEXT::FUNCSTR FuncCaptionType;
        //        SOBJECTTEXT::FUNCSTR FuncProgressType;

        //        // 예외처리 ----------------------------------
        //        // 시간 타입일 경우
        //        if ( strType == "TIME" )
        //        {
        //            FuncCaptionType  = &SOBJECTTEXT::CAPTION_TYPE_TIME;
        //            FuncProgressType = &SOBJECTTEXT::PROGRESS_TYPE_TIME;
        //        }
        //        else
        //        {
        //            FuncCaptionType  = SOBJECTTEXT::GETDEFAULTFUNCSTR( ID2GAMEWORD( strWORDID.c_str(), 0 ) );
        //            FuncProgressType = SOBJECTTEXT::GETDEFAULTFUNCSTR( ID2GAMEWORD( strWORDID.c_str(), 1 ) );
        //        }

        //        std::string strCAPTION  = ID2GAMEWORD( strXMLCAPTIONID.c_str() , nXMLIndex );
        //        std::string strPROGRESS = ID2GAMEWORD( strXMLPROGRESSID.c_str(), nXMLIndex );

        //        sObjectText.funcCAPTION_1 = SOBJECTTEXT::GETDEFAULTFUNCSTR( strCAPTION + " " );
        //        sObjectText.funcCAPTION_2 = FuncCaptionType;

        //        sObjectText.funcPROGRESS  = SOBJECTTEXT::GETFUNCSTR( strPROGRESS + " ", FuncProgressType );
        //    }
        //    else
        //    {
        //        ::MessageBoxA(
        //            NULL,
        //            sc::string::format("nID = %1%, TYPE is not exist", nID).c_str(),
        //            m_strUIFilePath.c_str(),
        //            MB_OK);
        //    }

        //    std::pair<SOBJTEXTMAP_ITER,bool> retiter = m_ObjectTextMap.insert( std::make_pair(nID, sObjectText) );

        //    if ( retiter.second==false )
        //    {
        //        ::MessageBoxA(
        //            NULL,
        //            sc::string::format("nID = %1%, ID '%1%' already existed", nID).c_str(),
        //            m_strUIFilePath.c_str(),
        //            MB_OK);
        //    }

        //    continue;

            //// (unused) [CAPTION] ----------------------------------------
            //LuaPlus::LuaObject objCaption = objText["CAPTION"];
            //if ( !objCaption.IsNil() )
            //{
            //    UINT nTableCount = (UINT)objCaption.GetTableCount();

            //    // caption first string ---------------- //
            //    {
            //        LuaPlus::LuaObject obj = objCaption[1];
            //        SOBJECTTEXT::FUNCSTR funcStr = SOBJECTTEXT::GETDEFAULTFUNCSTR("");

            //        if ( obj.IsFunction() )
            //        {
            //            LuaFunction<const lua_WChar*> luaFunc( obj );
            //            funcStr = SOBJECTTEXT::GETFUNCSTR( funcStr, SOBJECTTEXT::FUNCLUAWSTR( luaFunc ) );
            //        }
            //        else
            //        {
            //            std::wstring wstrText( (wchar_t*)obj.GetWString() );
            //            std::string  strText = sc::string::unicodeToAnsi(wstrText);

            //            funcStr = SOBJECTTEXT::GETFUNCSTR( funcStr, SOBJECTTEXT::GETDEFAULTFUNCSTR(strText) );
            //        }

            //        sObjectText.funcCAPTION_1 = funcStr;
            //    }

            //    // caption second string ---------------- //
            //    {
            //        SOBJECTTEXT::FUNCSTR funcStr = SOBJECTTEXT::GETDEFAULTFUNCSTR("");
            //        for ( UINT i=2; i<=nTableCount; ++i )
            //        {
            //            LuaPlus::LuaObject obj = objCaption[i];
            //            if ( obj.IsNil() )
            //            {
            //                continue;
            //            }

            //            if ( obj.IsFunction() )
            //            {
            //                LuaFunction<const lua_WChar*> luaFunc( obj );
            //                funcStr = SOBJECTTEXT::GETFUNCSTR( funcStr, SOBJECTTEXT::FUNCLUAWSTR( luaFunc ) );
            //            }
            //            else
            //            {
            //                std::wstring wstrText( (wchar_t*)obj.GetWString() );
            //                std::string  strText = sc::string::unicodeToAnsi(wstrText);

            //                funcStr = SOBJECTTEXT::GETFUNCSTR( funcStr, SOBJECTTEXT::GETDEFAULTFUNCSTR(strText) );
            //            }
            //        }

            //        sObjectText.funcCAPTION_2 = funcStr;
            //    }
            //}
            //else
            //{
            //    ::MessageBoxA(
            //        NULL,
            //        sc::string::format("nID = %1%, CAPTION is not exist", nID).c_str(),
            //        m_strUIFilePath.c_str(),
            //        MB_OK);

            //    sObjectText.funcCAPTION_1 = SOBJECTTEXT::GETNULLFUNCSTR();
            //    sObjectText.funcCAPTION_2 = SOBJECTTEXT::GETNULLFUNCSTR();
            //}

            //// (unused) [PROGRESS] ----------------------------------------
            //LuaPlus::LuaObject objProgress = objText["PROGRESS"];
            //if ( !objProgress.IsNil() )
            //{
            //    UINT nTableCount = (UINT)objProgress.GetTableCount();

            //    SOBJECTTEXT::FUNCSTR funcStr = SOBJECTTEXT::GETDEFAULTFUNCSTR("");
            //    for ( UINT i=1; i<=nTableCount; ++i )
            //    {
            //        LuaPlus::LuaObject obj = objProgress[i];
            //        if ( obj.IsNil() )
            //        {
            //            continue;
            //        }

            //        if ( obj.IsFunction() )
            //        {
            //            LuaFunction<const lua_WChar*> luaFunc( obj );
            //            funcStr = SOBJECTTEXT::GETFUNCSTR( funcStr, SOBJECTTEXT::FUNCLUAWSTR( luaFunc ) );
            //        }
            //        else
            //        {
            //            std::wstring wstrText( (wchar_t*)obj.GetWString() );
            //            std::string  strText = sc::string::unicodeToAnsi(wstrText);

            //            funcStr = SOBJECTTEXT::GETFUNCSTR( funcStr, SOBJECTTEXT::GETDEFAULTFUNCSTR(strText) );
            //        }
            //    }

            //    sObjectText.funcPROGRESS = funcStr;
            //}
            //else
            //{
            //    ::MessageBoxA(
            //        NULL,
            //        sc::string::format("nID = %1%, PROGRESS is not exist", nID).c_str(),
            //        m_strUIFilePath.c_str(),
            //        MB_OK);

            //    sObjectText.funcPROGRESS = SOBJECTTEXT::GETNULLFUNCSTR();
            //}
        //}
    }
    catch (const LuaPlus::LuaException& e)
    {        
        ::MessageBoxA(
            NULL,
            e.GetErrorMessage(),
            m_strUIFilePath.c_str(),
            MB_OK);

        return false;
    }

    return true;
}

SOBJECTTEXT* GLAttendanceClient::UIFindObjectText ( const UINT nID )
{
    SOBJTEXTMAP_ITER finditer = m_ObjectTextMap.find( nID );
    if ( finditer == m_ObjectTextMap.end() )
    {
        return NULL;
    }

    return &(finditer->second);
}

std::string GLAttendanceClient::UIGetStringCaption_1 ( TaskN* pTask )
{
    if ( !pTask )
    {
        return "('pTask' is null)";
    }

    SOBJECTTEXT* pText = UIFindObjectText( pTask->GetType() );
    if (pText)
        return pText->funcCAPTION_1( pTask );
    else
        return std::string("UIGetStringCaption_1 pText is null");
}

std::string GLAttendanceClient::UIGetStringCaption_2 ( TaskN* pTask )
{
    if ( !pTask )
    {
        return "('pTask' is null)";
    }

    SOBJECTTEXT* pText = UIFindObjectText( pTask->GetType() );
    if (pText)
        return pText->funcCAPTION_2( pTask );
    else
        return std::string("UIGetStringCaption_2 pText is null");
}

std::string GLAttendanceClient::UIGetStringProgress ( TaskN* pTask )
{
    if ( !pTask )
    {
        return "('pTask' is null)";
    }

    SOBJECTTEXT* pText = UIFindObjectText( pTask->GetType() );
    if (pText)
        return pText->funcPROGRESS( pTask );
    else
        return std::string("UIGetStringProgress pText is null");
}

//--------------------------------------------------------------------------------------------//
