#pragma once

#include <vector>
#include <map>

struct MaterialSkin;
struct SKINEFF_LEVEL_DATA;

struct MATERIAL_SKIN_DATA
{
	MaterialSkin*		m_rMaterial;			// 순수 참조
	BOOL				m_bDefaultMaterial;

	MATERIAL_SKIN_DATA::MATERIAL_SKIN_DATA()
		: m_rMaterial(NULL)
		, m_bDefaultMaterial(FALSE)
	{
	}

	void Init()
	{
		m_rMaterial = NULL;
		m_bDefaultMaterial = FALSE;
	}
};

struct EffectMaterialData_Level
{
	BOOL m_bLevel;
	BOOL m_bLevelFlow;
	D3DXVECTOR4 m_vLevelColor;
	D3DXVECTOR4 m_vLevelFlowColor;
	LPDIRECT3DTEXTUREQ m_rLevelFlowTex;

	EffectMaterialData_Level()
		: m_bLevel(FALSE)
		, m_bLevelFlow(FALSE)
		, m_rLevelFlowTex(NULL)
	{
	}
};

//struct EffectViewMatrix
//{
//	D3DXMATRIX	m_matView;
//	D3DXMATRIX	m_matViewProj;
//	D3DXMATRIX	m_matInverseView;
//	D3DXVECTOR4 m_vLightDir4;
//	D3DXVECTOR4 m_vSpecularDir4;
//
//	void Compute( const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXVECTOR3& vLightDir, const D3DXVECTOR3& vCameraDir )
//	{
//		m_matView = matView;
//		D3DXMatrixMultiply( &m_matViewProj, &matView, &matProj );
//		D3DXMatrixInverse( &m_matInverseView, NULL, &matView );
//
//		// Set Light for vertex shader
//		D3DXVECTOR3 vLightDirTemp;
//		D3DXVec3TransformNormal( &vLightDirTemp, &vLightDir, &matView );
//		m_vLightDir4 = D3DXVECTOR4( -vLightDirTemp, 1.0f );
//
//		D3DXVECTOR3 vSpecularDir = vCameraDir;	// + (vCross * 0.5f) + (vUp * 0.5f);
//		D3DXVec3Normalize( &vSpecularDir, &vSpecularDir );
//		D3DXVec3TransformNormal( &vSpecularDir, &vSpecularDir, &matView );
//		m_vSpecularDir4 = D3DXVECTOR4( -vSpecularDir, 1.f );
//	}
//};

struct EffectMaterialData
{
	INT							m_nEnableHairIndex;					// Hair 가 활성화된 Index
	//const D3DXVECTOR4*			m_rHairSpecColor_TangentOffsetTRT;	// xyz - SpecColor,		w - TangentOffsetTRT
	//const D3DXVECTOR4*			m_rHairShiftUV_SpecExp_R_TRT;		// xy - ShiftTangentUV, z - SpecExp_R,			w - SpecExp_TRT

	std::vector<BOOL>			m_vecEnableNormalMap;

	BOOL						m_bMultiTex;			// m_vecEnableMultiTex 있지만 m_bMultiTex 이 또 있는건 m_vecEnableMultiTex 을 매번 재 설정 하지 않으려고 임.
	int							m_nMultiTex_ADD;
	std::vector<BOOL>			m_vecEnableMultiTex;
	LPDIRECT3DTEXTUREQ			m_rMultiTex;

	BOOL						m_bCubeBlend;			// m_vecEnableCubeBlend 중 하나라도 TRUE 가 있을 경우 m_bCubeBlend 는 TRUE 가 됨.
	std::vector<BOOL>			m_vecEnableCubeBlend;
	LPDIRECT3DTEXTUREQ			m_pCubeMapTexture;

	BOOL							m_bTexDiffColor;
	std::vector<float>				m_vecTexDiffColor;
	std::vector<LPDIRECT3DTEXTUREQ>	m_vecTexUpDownTexture;

	BOOL						m_bAmbient;				// 0-FALSE, 1-USETEXCOLOR, 2-Ambient
	D3DXVECTOR3					m_vEffAmbient_DiffuseMulti;
	D3DXVECTOR3					m_vEffAmbient_AmbientMulti;

	EffectMaterialData_Level*	m_rLevelData;

	EffectMaterialData::EffectMaterialData()
		: m_nEnableHairIndex(-1)
		//, m_rHairSpecColor_TangentOffsetTRT(NULL)
		//, m_rHairShiftUV_SpecExp_R_TRT(NULL)
		, m_bMultiTex(FALSE)
		, m_nMultiTex_ADD(0)
		, m_rMultiTex(NULL)
		, m_bCubeBlend(FALSE)
		, m_pCubeMapTexture(NULL)
		, m_bTexDiffColor(FALSE)
		, m_bAmbient(FALSE)
		, m_vEffAmbient_DiffuseMulti(1.f,1.f,1.f)
		, m_vEffAmbient_AmbientMulti(1.f,1.f,1.f)
		, m_rLevelData(NULL)
	{
	}

	// FrameMove 를 돌면서 Init 이 필요한 것.
	void InitForPrecalculateForMaterialSystem()
	{
		m_nEnableHairIndex = -1;
		m_bMultiTex = FALSE;
		m_bCubeBlend = FALSE;
		m_pCubeMapTexture = NULL;
		m_bAmbient = FALSE;
		m_bTexDiffColor = FALSE;
		m_rLevelData = NULL;
	}

	// 값이 변동될수 있는 Tool 상에서 해야하는 작업.
	void InitForTool()
	{
		m_vecEnableNormalMap.clear();
		m_vecEnableMultiTex.clear();
		m_vecEnableCubeBlend.clear();
		m_vecTexDiffColor.clear();
		m_vecTexUpDownTexture.clear();

		InitForPrecalculateForMaterialSystem();
	}
};