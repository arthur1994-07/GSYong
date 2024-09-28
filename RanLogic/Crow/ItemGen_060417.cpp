#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/Common/gltexfile.h"
#include "../GLogicData.h"
#include "../Item/GLItemMan.h"
#include "./ItemGen_060417.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// 발생 확율
BOOL SGENITEM_060417::ISGEN(float fGenRate)
{
    if (m_fGenRate==0)
        return FALSE;

    float fGEN_RATE = m_fGenRate*GLCONST_CHAR::GET_ITEM_SCALE();
    if (fGenRate != 0.0f)
    {
        fGEN_RATE *= fGenRate;
        return sc::Random::getInstance().IsSucceed(fGEN_RATE);
    }
    else 
    {
        return FALSE;
    }
}

// 발생 갯수.
WORD SGENITEM_060417::SELECTNUM()
{
    WORD wNum = 0;
    float fNowRate = sc::Random::getInstance().GetFloatPercent();
    for ( int i=0; i<EMGNUM; ++i )
    {
        if ( m_sGenNum[i].wNum==0 )		break;

        if ( (m_sGenNum[i].fRateL<=fNowRate) && (fNowRate<m_sGenNum[i].fRateH) )
        {
            wNum = m_sGenNum[i].wNum;
            break;
        }
    }

    return wNum;
}

DWORD SGENITEM_060417::SELECTSPECID ( float fGEN_RATE, bool& bSPECID )	//	발생 군.
{
    bool bSCALE = GLCONST_CHAR::fITEM_DROP_LOWER_SCALE!=1.0f || fGEN_RATE!=1.0f;

    float fNowRate = sc::Random::getInstance().GetFloatPercent();
    DWORD dwSpecID = EMSPEC_NULL;
    float fRateL = m_sItemSpec[0].fRateL;
    for ( int i=0; i<EMGSPEC; ++i )
    {
        float fRate = (m_sItemSpec[i].fRateH-m_sItemSpec[i].fRateL);
        if ( bSCALE && fRate < 2.0f )
        {
            fRate = fRate * (GLCONST_CHAR::fITEM_DROP_LOWER_SCALE*fGEN_RATE);
        }

        if ( (fRateL<=fNowRate) && fNowRate<(fRateL+fRate) )
        {
            dwSpecID = m_sItemSpec[i].dwSPECID;
            bSPECID = m_sItemSpec[i].bSPECID;
            break;
        }

        fRateL += fRate;
    }

    return dwSpecID;
}

BOOL SGENITEM_060417::LOADFILE ( const char* szFileName )
{
    if( !szFileName )					return FALSE;
    if( strlen( szFileName ) == 0 )		return FALSE;

    std::string strPath;
    strPath = GLOGIC::GetServerPath();
    strPath += szFileName;	

    gltexfile glTextFile;
    glTextFile.reg_sep("\t ,{}[]()|");

    if (GLOGIC::UseLogicZipFile()) // by 경대
        glTextFile.SetZipFile(GLOGIC::strGLOGIC_SERVER_ZIPFILE);

    if ( !glTextFile.open ( strPath, true ) )
    {
        sc::writeLogError(
            sc::string::format(
                "SGENITEM::LOADFILE() %1%", szFileName));
        return FALSE;
    }

    WORD dwVERSION = 0;
    glTextFile.getflag ( "VERSION", 1, 1, dwVERSION, true );

    glTextFile.getflag ( "GENRATE", 1, 1, m_fGenRate, true );

    WORD wNum=0;
    float fRate=0, fRateL=0;

    DWORD dwNUM = glTextFile.getflagnum ( "GENNUM" );
    for ( DWORD i=0; i<dwNUM; ++i )
    {
        glTextFile.getflag ( i, "GENNUM", 1, 2, wNum, true );
        glTextFile.getflag ( i, "GENNUM", 2, 2, fRate, true );

        m_sGenNum[i].wNum = wNum;
        m_sGenNum[i].fRateL = fRateL;
        m_sGenNum[i].fRateH = fRateL+fRate;
        fRateL = m_sGenNum[i].fRateH;
    }

    DWORD dwSpecID=0;
    fRate=0, fRateL=0;

    typedef std::multimap<float,SGITEMSPEC> MAP_ITEMGEN;
    typedef MAP_ITEMGEN::iterator			MAP_ITEMGEN_ITER;

    MAP_ITEMGEN mapGENITEMS;

    DWORD dwSpecNum = glTextFile.getflagnum ( "ITEMSPEC" );

    if ( dwSpecNum > SGENITEM_060417::EMGSPEC )
    {
        CString strErr;
        strErr.Format ( "GenItem : %s Overflow.",
            szFileName );
        MessageBox ( NULL, strErr.GetString(), "ERROR", MB_OK );
        return FALSE;
    }

    for ( DWORD i=0; i<dwSpecNum; ++i )
    {
        glTextFile.getflag ( i, "ITEMSPEC", 1, 2, dwSpecID, true );
        glTextFile.getflag ( i, "ITEMSPEC", 2, 2, fRate, true );

        if ( dwSpecID >= GLItemMan::SPECID_NUM )
        {
            CString strErr;
            strErr.Format ( "ITEMSPEC : %s Overflow. %d >= %d",
                szFileName, dwSpecID, GLItemMan::SPECID_NUM );
            MessageBox ( NULL, strErr.GetString(), "ERROR", MB_OK );

            dwSpecID = 0;
        }

        //	Note : 
        SGITEMSPEC sSPEC;
        sSPEC.dwSPECID = dwSpecID;
        sSPEC.fRateL = fRate;
        sSPEC.fRateH = 0.0f;
        sSPEC.bSPECID = true;

        mapGENITEMS.insert ( std::make_pair(fRate,sSPEC) );
    }

    DWORD dwIDNUM = glTextFile.getflagnum ( "ITEMID" );

    if ( dwSpecNum+dwIDNUM > SGENITEM_060417::EMGSPEC )
    {
        CString strErr;
        strErr.Format ( "GenItem : %s Overflow.",
            szFileName );
        MessageBox ( NULL, strErr.GetString(), "ERROR", MB_OK );
        return FALSE;
    }


    for ( DWORD i=0; i<dwIDNUM; ++i )
    {
        SNATIVEID sNativeID;
        glTextFile.getflag ( i, "ITEMID", 1, 3, sNativeID.wMainID, true );
        glTextFile.getflag ( i, "ITEMID", 2, 3, sNativeID.wSubID, true );
        dwSpecID = sNativeID.dwID;

        glTextFile.getflag ( i, "ITEMID", 3, 3, fRate, true );

        if ( sNativeID == NATIVEID_NULL() )
        {
            CString strErr;
            strErr.Format ( "ITEMID : %s NativeID_NULL. %d",
                szFileName, dwNUM );
            MessageBox ( NULL, strErr.GetString(), "ERROR", MB_OK );

            dwSpecID = 0;
        }

        //	Note : 
        SGITEMSPEC sSPEC;
        sSPEC.dwSPECID = dwSpecID;
        sSPEC.fRateL = fRate;
        sSPEC.fRateH = 0.0f;
        sSPEC.bSPECID = false;

        mapGENITEMS.insert ( std::make_pair(fRate,sSPEC) );
    }


    //	Note : 희귀성 순서대로 입력.
    //
    int GenCount = 0;
    fRate=0, fRateL=0;
    MAP_ITEMGEN_ITER pos = mapGENITEMS.begin();
    MAP_ITEMGEN_ITER end = mapGENITEMS.end();
    for ( ; pos!=end; ++pos )
    {
        SGITEMSPEC sSPEC = (*pos).second;

        float fRate = sSPEC.fRateL;
        sSPEC.fRateL = fRateL;
        sSPEC.fRateH = fRateL + fRate;
        fRateL = sSPEC.fRateH;

        m_sItemSpec[GenCount++] = sSPEC;
    }

    return TRUE;
}

