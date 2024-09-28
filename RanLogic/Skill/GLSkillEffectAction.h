#pragma once

#include "GLSkillEffVarSet.h"

using namespace NSSKILL_EFFSET;

class CSkillSThrow
{
public :
	CSkillSThrow();
	~CSkillSThrow();

private :
	_sScreenThrow::sInfo m_sEffID[GLCONST_CHAR::SCREEN_EFFECT_NUM];
public :
	BOOL GetEnableCheck( const STARGETID& sTarget, const UINT iIndex, const bool bEnable );
	BOOL AutoReset( const UINT iIndex, const bool bEnable );	
};

class CSkillCameraHit
{
public :
	CSkillCameraHit();
	~CSkillCameraHit();

private :
	BOOL m_bHit;

	int m_iIndex;

public :
	BOOL GetEnableCheck( WORD wEffID, const bool bEnable );
	BOOL AutoReset( WORD& wEffID, const bool bEnable );

};

class CSkillDistinguish
{
public :
	CSkillDistinguish();
	~CSkillDistinguish();

	// 연극부 변장 관련
	BOOL AutoResetDisting( const UINT iDisIndex, const bool bDistinguish );
	BOOL GetDistinguishCharData( const UINT iDisIndex, const bool bDistinguish );

private:
	UINT m_iDistinguishIndex;
};