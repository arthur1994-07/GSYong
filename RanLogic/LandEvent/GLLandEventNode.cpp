#include "../pch.h"

#include "./GLLandEventNode.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CEVENT_NODE::FrameMove( const FLOAT fElapsedTime )
{
	/*if ( m_fTimeEnd <= m_fTimeCurrent ) 
		return;*/

	m_fTimeCurrent += fElapsedTime;

	if ( !m_bInit && (0.0f <= m_fTimeCurrent) )
	{
		FrameStart();

		m_bInit = TRUE;
	}

	if ( !m_bEvent && (m_fTimeEvent <= m_fTimeCurrent) )
	{
		FrameEvent();

		m_bEvent = TRUE;
	}

	FrameProcess();

	if ( m_fTimeEnd <= m_fTimeCurrent )
	{
		FrameEnd();
	}
}