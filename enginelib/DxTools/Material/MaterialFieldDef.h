#pragma once

#include <vector>
#include <map>

struct Material;
class DxSingleMaterialMesh;

#include "./MaterialDef.h"

struct MATERIAL_DATA
{
	Material*			m_rMaterial;			// ���� ����
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
	PS_3_0_TYPE_ZERO	= 0,	// �׸��ڴ� �����ȵ�
	PS_3_0_TYPE_CSM		= 1,	// CascadeShadowMap �� ����.
	PS_3_0_TYPE_CSM_PL	= 2,	// CascadeShadowMap, PointLight ����.
	PS_3_0_TYPE_PL1		= 3,	// PointLight�� ����.
	PS_3_0_TYPE_PL2		= 4,	// PointLight�� ����.
	PS_3_0_TYPE_PL_PL	= 5,	// PointLight, PointLight ����.
};

enum PS_3_0_TYPE_USED	// RT - RealTime Shadow�� �����. ON - �̹��� ������� �������Ѵ�.
{
	PS_3_0_TYPE_USED_ZERO			= 0,	// �׸��ڴ� �����ȵ�
	PS_3_0_TYPE_USED_CSM			= 1,	// CascadeShadowMap �� ����.
	PS_3_0_TYPE_USED_CSM_PL2_RT		= 2,	// CascadeShadowMap, PointLight ����.
	PS_3_0_TYPE_USED_CSM_PL2_ON		= 3,	// CascadeShadowMap, PointLight ����.
	PS_3_0_TYPE_USED_PL1_RT			= 4,	// PointLight�� ����.
	PS_3_0_TYPE_USED_PL1_ON			= 5,	// PointLight�� ����.
	PS_3_0_TYPE_USED_PL2_RT			= 6,	// PointLight�� ����.
	PS_3_0_TYPE_USED_PL2_ON			= 7,	// PointLight�� ����.
	PS_3_0_TYPE_USED_PL1_RT_PL2_RT	= 8,	// PointLight, PointLight ����.
	PS_3_0_TYPE_USED_PL1_RT_PL2_ON	= 9,	// PointLight, PointLight ����.
	PS_3_0_TYPE_USED_PL1_ON_PL2_RT	= 10,	// PointLight, PointLight ����.
	PS_3_0_TYPE_USED_PL1_ON_PL2_ON	= 11,	// PointLight, PointLight ����.
};

typedef std::vector<DxSingleMaterialMesh*>							VEC_SINGLE_MATERIAL_MESH;					
typedef std::map<DWORD,VEC_SINGLE_MATERIAL_MESH>					MAP_TYPE_VEC_SINGLE_MATERIAL_MESH;			// first - PS_3_0_TYPE, second - DxSingleMaterialMesh
typedef MAP_TYPE_VEC_SINGLE_MATERIAL_MESH::iterator					MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER;

typedef std::map<Material*,MAP_TYPE_VEC_SINGLE_MATERIAL_MESH>		MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH;
typedef MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH::iterator	MAP_MATERIAL_MAP_TYPE_VEC_SINGLE_MATERIAL_MESH_ITER;

typedef std::map<Material*,VEC_SINGLE_MATERIAL_MESH>	MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH;
typedef MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH::iterator	MAP_MATERIAL_VEC_SINGLE_MATERIAL_MESH_ITER;