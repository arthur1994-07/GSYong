#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/Common/gltexfile.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "./QuestionItemGen_060417.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SNATIVEID SQTGENITEM_060417::SELECTITEM () // Question ������ �߻� by ���
{
    float fNowRate = sc::Random::getInstance().GetFloatPercent();
    float fRateL(0);

    for (int i=0; i<m_nItemNum; ++i)
    {
        float fRate = m_sItem[i].m_fGenRate;

        if ((fRateL<=fNowRate) && fNowRate<(fRateL+fRate))
        {
            return m_sItem[i].m_sGenItemID;
        }
        fRateL += fRate;
    }
    return SNATIVEID(false);
}

BOOL SQTGENITEM_060417::LOADFILE ( const char* szFileName ) // by ���
{
    if (!szFileName)
        return FALSE;
    if (strlen(szFileName) == 0)
        return FALSE;

    // Note : �� ������ ;�� �̷���� ������ �����̸��� ���� �ʽ��ϴ�.
    std::string strFirstToken = szFileName;
    strFirstToken = strFirstToken.substr(0, strFirstToken.find_first_of(';'));

    std::string strPath;
    strPath = GLOGIC::GetServerPath();
    strPath += strFirstToken;	

    gltexfile glTextFile;
    glTextFile.reg_sep("\t ,{}[]()|");

    if (GLOGIC::UseLogicZipFile()) // by ���
        glTextFile.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

    if ( !glTextFile.open ( strPath, true ) )
    {
        sc::writeLogError(
            sc::string::format(
                "SQTGENITEM_060417::LOADFILE %1%", strFirstToken));
        return FALSE;
    }

    //WORD dwVERSION = 0;
    //glTextFile.getflag ( "VERSION", 1, 1, dwVERSION, true );

    float fRate(0);

    DWORD dwNUM = glTextFile.getflagnum ( "QTITEM" );
    if ( dwNUM > EMQTMAX )	dwNUM = EMQTMAX;

    for ( DWORD i=0; i<dwNUM; ++i )
    {
        glTextFile.getflag ( i, "QTITEM", 1, 3, m_sItem[i].m_sGenItemID.wMainID, true );
        glTextFile.getflag ( i, "QTITEM", 2, 3, m_sItem[i].m_sGenItemID.wSubID, true );
        glTextFile.getflag ( i, "QTITEM", 3, 3, m_sItem[i].m_fGenRate, true );
        fRate += m_sItem[i].m_fGenRate;
    }

    if( fRate > 100.0 )
    {
        sc::writeLogError(
            sc::string::format(
                "SQTGENITEM_060417::LOADFILE %1%", strFirstToken));
        return FALSE;
    }

    m_nItemNum = dwNUM; // ������ ��� ���� ����

    return TRUE;
}

