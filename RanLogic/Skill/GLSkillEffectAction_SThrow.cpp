#include "pch.h"
#include "../GLogicEx.h"
#include "GLSkillEffectAction.h"

CSkillSThrow::CSkillSThrow()
{
	
}

CSkillSThrow::~CSkillSThrow()
{
	
}

BOOL CSkillSThrow::GetEnableCheck( const STARGETID& sTarget, const UINT iIndex, const bool bEnable )
{
	if ( iIndex >= GLCONST_CHAR::SCREEN_EFFECT_NUM )
		return FALSE;

	if( bEnable && m_sEffID[iIndex].bEffID != bEnable )
	{	
		m_sEffID[iIndex].bEffID   = TRUE;
		m_sEffID[iIndex].sTagetID = sTarget;

		return TRUE;
	}

	return FALSE;
}

BOOL CSkillSThrow::AutoReset( const UINT iIndex, const bool bEnable )
{
	if ( iIndex >= GLCONST_CHAR::SCREEN_EFFECT_NUM )
		return FALSE;

	if( !bEnable && m_sEffID[iIndex].bEffID )
	{
		m_sEffID[iIndex].Reset();

		return TRUE;
	}

	return FALSE;
}