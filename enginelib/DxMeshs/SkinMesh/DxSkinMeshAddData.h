#pragma once

#include "../../../InternalCommonLib/dxstdafx.h"

struct DxSkeleton;

struct SKINMESH_ADD_DATA
{
	TSTRING m_strName;
	BOOL	m_bTexThread;

	SKINMESH_ADD_DATA()
		: m_bTexThread(TRUE)
	{
	}
};