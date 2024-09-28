#include "pch.h"
#include "../GLogicEx.h"
#include "GLSkillEffectAction.h"

CSkillCameraHit::CSkillCameraHit()
{
	m_bHit = FALSE;

	m_iIndex = -1;
}

CSkillCameraHit::~CSkillCameraHit()
{

}

BOOL CSkillCameraHit::GetEnableCheck( WORD wEffID, const bool bEnable )
{
	if( bEnable && wEffID != m_iIndex )
	{	
		m_iIndex = wEffID;

		return TRUE;
	}

	return FALSE;
}

BOOL CSkillCameraHit::AutoReset( WORD& wEffID, const bool bEnable )
{
	if( !bEnable && m_iIndex >= 0 )
	{
		wEffID = m_iIndex;

		m_iIndex = -1;

		return TRUE;
	}

	return FALSE;
}