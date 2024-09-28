#pragma once

#include <vector>
#include <map>

struct Material;
class DxSingleMaterialMesh;

#include "./MaterialDef.h"

struct MATERIAL_DATA
{
	Material*			m_rMaterial;			// 순수 참조
	BOOL				m_bDefaultMaterial;
	BOOL				m_bGameMode;

	MATERIAL_DATA::MATERIAL_DATA()
		: m_rMaterial(NULL)
		, m_bDefaultMaterial(FALSE)
		, m_bGameMode(TRUE)
	{
	}

	void Init()
	{
		m_rMaterial = NULL;
		m_bDefaultMaterial = FALSE;
		m_bGameMode = TRUE;
	}
};

enum PS_3_0_TYPE
{
	PS_3_0_TYPE_ZERO	= 0,	// 그림자는 생성안됨
	PS_3_0_TYPE_CSM		= 1,	// CascadeShadowMap 만 보임.
	PS_3_0_TYPE_CSM_PL	= 2,	// CascadeShadowMap, PointLight 보임.
	PS_3_0_TYPE_PL1		= 3,	// PointLight만 보임.
	PS_3_0_TYPE_PL2		= 4,	// PointLight만 보임.
	PS_3_0_TYPE_PL_PL	= 5,	// PointLight, PointLight 보임.
};

enum PS_3_0_TYPE_USED	// RT - RealTime Shadow를 사용함. ON - 이미지 기반으로 렌더링한다.
{
	PS_3_0_TYPE_USED_ZERO			= 0,	// 그림자는 생성안됨
	PS_3_0_TYPE_USED_CSM			= 1,	// CascadeShadowMap 만 보임.
	PS_3_0_TYPE_USED_CSM_PL2_RT		= 2,	// CascadeShadowMap, PointLight 보임.
	PS_3_0_TYPE_USED_CSM_PL2_ON		= 3,	// CascadeShadowMap, PointLight 보임.
	PS_3_0_TYPE_USED_PL1_RT			= 4,	// PointLight만 보임.
	PS_3_0_TYPE_USED_PL1_ON			= 5,	// PointLight만 보임.
	PS_3_0_TYPE_USED_PL2_RT			= 6,	// PointLight만 보임.
	PS_3_0_TYPE_USED_PL2_ON			= 7,	// PointLight만 보임.
	PS_3_0_TYPE_USED_PL1_RT_PL2_RT	= 8,	// PointLight, PointLight 보임.
	PS_3_0_TYPE_USED_PL1_RT_PL2_ON	= 9,	// PointLight, PointLight 보임.
	PS_3_0_TYPE_USED_PL1_ON_PL2_RT	= 10,	// PointLight, PointLight 보임.
	PS_3_0_TYPE_USED_PL1_ON_PL2_ON	= 11,	// PointLight, PointLight 보임.
};

typedef std::vector<DxSingleMaterialMesh*>							VEC_SINGLE_MATERIAL_MESH;					
typedef std::map<DWORD,VEC_SINGLE_MATERIAL_MESH>					MAP_TYPE_VEC_SINGLE_MATERIAL_MESH;			// first - PS_3_0_TYPE, second - DxSingleMaterialMesh
typedef MAP_TYPE_VEC_SINGLE_MATERIAL_MESH::iterator					MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER;

typedef std::map<Material*,MAP_TYPE_VEC_SINGLE_MATERIAL_MESH>		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH;
typedef MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH::iterator	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER;

typedef std::map<Material*,VEC_SINGLE_MATERIAL_MESH>	MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH;
typedef MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH::iterator	MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH_ITER;