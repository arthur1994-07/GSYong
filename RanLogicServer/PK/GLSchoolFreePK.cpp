#include "pch.h"

#include <algorithm>

#include "./GLSchoolFreePK.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/G-Logic/GLPeriod.h"
#include "../../RanLogic/GLogicData.h"
#include "../AgentServer/GLAgentServer.h"

#include "../../SigmaCore/DebugInclude.h"

GLSchoolFreePK::GLSchoolFreePK()
    : m_bON(false)   
{
}

GLSchoolFreePK::~GLSchoolFreePK()
{
}

bool GLSchoolFreePK::IsON() const
{
	if (GLCONST_CHAR::bSCHOOL_FREE_PK_ALWAYS)
        return true;
    else
	    return m_bON;
}

GLSchoolFreePKAgent::GLSchoolFreePKAgent(GLAgentServer* pServer)
    : m_pServer(pServer)
    , m_dwLAST_DAY(0)
    , m_fTIMER(0)
{
}

GLSchoolFreePKAgent::~GLSchoolFreePKAgent()
{
}

bool GLSchoolFreePKAgent::DoBegin()
{
	m_bON = true;
	m_fTIMER = 0;

/* 
	//	Note : Ŭ���̾�Ʈ�� �˸�.
	GLMSG::SNETPC_SERVER_INFO NetMsgClt;
	NetMsgClt.bSCHOOL_FREEPK = m_bON;
	pServer->SENDTOALLCLIENT ( &NetMsgClt );
*/

	//	Note : �ʵ忡 ���� ��ȯ.
	GLMSG::SNETPC_SCHOOLFREEPK_FLD NetMsgFld;
	NetMsgFld.bSCHOOL_FREEPK = m_bON;
	m_pServer->SENDTOALLCHANNEL ( &NetMsgFld );
	return true;
}

bool GLSchoolFreePKAgent::DoEnd()
{
	m_bON = false;
	m_fTIMER = 0;
/*
	//	Note : Ŭ���̾�Ʈ�� �˸�.
	GLMSG::SNETPC_SERVER_INFO NetMsgClt;
	NetMsgClt.bSCHOOL_FREEPK = m_bON;
	m_pServer->SENDTOALLCLIENT ( &NetMsgClt );
*/
	// �ʵ忡 ���� ��ȯ
	GLMSG::SNETPC_SCHOOLFREEPK_FLD NetMsgFld;
	NetMsgFld.bSCHOOL_FREEPK = m_bON;
	m_pServer->SENDTOALLCHANNEL(&NetMsgFld);
	return true;
}

bool GLSchoolFreePKAgent::DoProgress ( float fElaps )
{
	if (!GLCONST_CHAR::bSCHOOL_FREE_PK)
	{
		DoEnd ();
		return true;
	}

    m_fTIMER += fElaps;

    if ( !GLCONST_CHAR::bSCHOOL_FREE_PK_TIME_REAL )
    {
        if ( m_fTIMER > GLCONST_CHAR::dwSCHOOL_FREE_PK_TIME_TERM*3600.0f/GLPeriod::REALTIME_TO_VBR )
        {
            DoEnd ();
            return true;
        }
    }
    else
    {
        if ( m_fTIMER > GLCONST_CHAR::dwSCHOOL_FREE_PK_TIME_TERM*3600.0f )
        {
            DoEnd ();
            return true;
        }
    }

    return false;
}

bool GLSchoolFreePKAgent::CheckBegin ()
{
    if ( !GLCONST_CHAR::bSCHOOL_FREE_PK )
		return true;

    if ( !GLCONST_CHAR::bSCHOOL_FREE_PK_TIME_REAL )
    {
        DWORD dwYearToday = GLPeriod::GetInstance().GetYearToday();
        DWORD dwHour = GLPeriod::GetInstance().GetHour();

		
		if (m_pServer->IsFreePkGmCommand())
		{
			m_dwLAST_DAY = dwYearToday;
			//	Note : ����.
			DoBegin ();
			m_pServer->SetFreePkGMCommand(FALSE);
		}

        //	Note : ���� ���۵Ǿ����� �˻�.
        if ( m_dwLAST_DAY!=dwYearToday )
        {
            //	Note : ���۽ð����� �˻�.
            if ( GLCONST_CHAR::dwSCHOOL_FREE_PK_TIME_START == dwHour )
            {
                m_dwLAST_DAY = dwYearToday;

                //	Note : ����.
                DoBegin ();
            }
        }
    }
    else
    {
        CTime cTIME = CTime::GetCurrentTime();

        DWORD dwYearToday = (DWORD) cTIME.GetDay();
        DWORD dwHour = (DWORD) cTIME.GetHour();

		if (m_pServer->IsFreePkGmCommand())
		{
			m_dwLAST_DAY = dwYearToday;
			//	Note : ����.
			DoBegin ();
			m_pServer->SetFreePkGMCommand(FALSE);
		}

        //	Note : ���� ���۵Ǿ����� �˻�.
        if ( m_dwLAST_DAY!=dwYearToday )
        {
            //	Note : ���۽ð����� �˻�.
            if ( GLCONST_CHAR::dwSCHOOL_FREE_PK_TIME_START == dwHour )
            {
                m_dwLAST_DAY = dwYearToday;

                //	Note : ����.
                DoBegin ();
            }
        }
    }
    return true;
}

void GLSchoolFreePKAgent::FrameMove ( float fElaps )
{
    if ( m_bON )
    {
        //	Note : ����.
        DoProgress ( fElaps );
    }
    else
    {
        //	Note : ���� ����.
        CheckBegin ();
    }
}

GLSchoolFreePKField::GLSchoolFreePKField()
{
}

GLSchoolFreePKField::~GLSchoolFreePKField()
{
}