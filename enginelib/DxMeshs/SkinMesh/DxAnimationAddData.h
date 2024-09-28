#pragma once

struct DxSkeleton;

struct ANIMATION_ADD_DATA
{
	DxSkeleton* m_pSkeleton;

	ANIMATION_ADD_DATA()
		: m_pSkeleton(NULL)
	{
	}
};