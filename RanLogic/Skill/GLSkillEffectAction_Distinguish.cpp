#include "pch.h"
#include "../GLogicEx.h"
#include "../TransformSet/GLTransformSet.h"
#include "GLSkillEffectAction.h"

CSkillDistinguish::CSkillDistinguish() : m_iDistinguishIndex(0)
{

}

CSkillDistinguish::~CSkillDistinguish()
{

}

BOOL CSkillDistinguish::GetDistinguishCharData( const UINT iDisIndex, const bool bDistinguish )
{
	if( bDistinguish && m_iDistinguishIndex != iDisIndex )
	{
		if ( iDisIndex > (GLTRANSFORMSET::g_nTransformChaSet - 1) )
			return FALSE;

		//DxSkinCharData* const _pSkinChar = GLTransformSkinManager::GetInstance().GetSkinCharSet(iDisIndex);

		//if( _pSkinChar == NULL )	return NULL;

		m_iDistinguishIndex = iDisIndex;

		return TRUE;
	}

	return FALSE;
}

BOOL CSkillDistinguish::AutoResetDisting( const UINT iDisIndex, const bool bDistinguish )
{
	if( !bDistinguish && m_iDistinguishIndex > 0 )
	{
		m_iDistinguishIndex = 0;

		return TRUE;
	}

	return FALSE;
}