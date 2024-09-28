#include "pch.h"
#include "../AgentServer/GLAgentServer.h"
#include "./GLAGPeriod.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//------------------------------------------------------------------------------------------//
// Note : 대련 불가능 모드

SNONCONFRONTLIST::SNONCONFRONTLIST ()
{

}

SNONCONFRONTLIST::~SNONCONFRONTLIST ()
{
	CleanUp();
}

void SNONCONFRONTLIST::CleanUp()
{
	sPeriodList.clear();
}

void SNONCONFRONTLIST::FrameMove(GLAgentServer* pServer, float fTime, float fElapsedTime)
{
	DWORD dwElapsedTime = (DWORD)(fElapsedTime*1000.0f);

	LIST_ITER iter = sPeriodList.begin();
	for ( ; iter!=sPeriodList.end(); )
	{
		SNONCONFRONT& sNonconf = (*iter);

		if ( sNonconf.bUNLIMITED )
		{
			 ++iter;

			continue;
		}

		sNonconf.dwCURTIME += dwElapsedTime;

		if ( sNonconf.dwENDTIME <= sNonconf.dwCURTIME )
		{
			sNonconf.dwCURTIME = sNonconf.dwENDTIME;

			// Note : 종료 사실 필드서버에 알림
			//
			GLMSG::SNET_GM_NONCONFRONT_MODE NetMsg;
			NetMsg.bMode    = FALSE;
			NetMsg.sMapID   = sNonconf.sMAPID;
			NetMsg.nChannel = sNonconf.nCHANNEL;

			pServer->SENDTOCHANNEL(&NetMsg, NetMsg.nChannel);
			sPeriodList.erase(iter++);
			continue;
		}

		++iter;
	}
}

void SNONCONFRONTLIST::Insert(GLAgentServer* pServer, const SNATIVEID& sMapID, const INT32 nChannel, const DWORD dwPeriod)
{
	SNONCONFRONT sNonConfront;
	sNonConfront.sMAPID      = sMapID;
	sNonConfront.dwBEGINTIME = timeGetTime();
	sNonConfront.dwCURTIME   = sNonConfront.dwBEGINTIME;

	if ( dwPeriod == -1 )
	{
		sNonConfront.dwENDTIME  = 0;
		sNonConfront.bUNLIMITED = TRUE;
	}
	else
	{
		// Note : dwPeriod 는 분
		sNonConfront.dwENDTIME  = sNonConfront.dwBEGINTIME + (dwPeriod*60000);
		sNonConfront.bUNLIMITED = FALSE;
	}

	if ( nChannel == -1 )
	{
		Delete ( sMapID, nChannel );

		for ( UINT32 i=0; i<MAX_CHANNEL_NUMBER; ++i )
		{
			for ( UINT32 j=0; j<FIELDSERVER_MAX; ++j )
			{
				if (pServer->IS_FIELDSERVER_ONLINE(i,j))
				{
					sNonConfront.nCHANNEL = (INT32)i;
					sPeriodList.insert( sPeriodList.end(), 1, sNonConfront );
				}
			}
		}
	}
	else
	{
		sNonConfront.nCHANNEL = nChannel;

		LIST_ITER founditer = std::find ( sPeriodList.begin(), sPeriodList.end(), SNONCONFRONT(sMapID, nChannel) );
		if ( founditer != sPeriodList.end() )
		{
			(*founditer) = sNonConfront;
		}
		else
		{
			sPeriodList.insert( sPeriodList.end(), 1, sNonConfront );
		}
	}
}

void SNONCONFRONTLIST::Delete_Front ()
{
	sPeriodList.pop_front();
}

void SNONCONFRONTLIST::Delete_Back ()
{
	sPeriodList.pop_back();
}

void SNONCONFRONTLIST::Delete ( const SNATIVEID& sMapID, const INT32 nChannel )
{
	sPeriodList.remove( SNONCONFRONT(sMapID, nChannel) );
}

BOOL SNONCONFRONTLIST::IsNonConfront ( const SNATIVEID& sMapID, const INT32 nChannel )
{
	LIST_ITER founditer = std::find ( sPeriodList.begin(), sPeriodList.end(), SNONCONFRONT(sMapID, nChannel) );
	if ( founditer != sPeriodList.end() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------//

GLAGPeriod::GLAGPeriod ()
{

}

GLAGPeriod::~GLAGPeriod ()
{
	CleanUp();
}

void GLAGPeriod::CleanUp ()
{
	m_NonConfrontList.CleanUp();
}

void GLAGPeriod::FrameMove(GLAgentServer* pServer, float fTime, float fElapsedTime)
{
	// 대련 불가능 지역 관련
	m_NonConfrontList.FrameMove(pServer, fTime, fElapsedTime);
}

//------------------------------------------------------------------------------------------//

