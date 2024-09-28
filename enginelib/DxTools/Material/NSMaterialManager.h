#pragma once

#include <boost/function.hpp>

#include "../DxVertexFVF.h"
#include "./MaterialFieldDef.h"

struct MATERIAL_DATA;
struct MaterialValue;

namespace NSMaterialManager
{
	extern TSTRING				g_strPath;

	//enum EMBLEND_OPTION
	//{
	//	EMBLEND_OPAQUE = 0,
	//	EMBLEND_HARD_ALPHA = 1,
	//	EMBLEND_SOFT_ALPHA_0 = 2,
	//	EMBLEND_SOFT_ALPHA_1 = 3,
	//	EMBLEND_SOFT_ALPHA_2 = 4,
	//};

	extern D3DXVECTOR4 g_vFog;
	extern D3DXVECTOR4 g_vLightPosWorldSpace;

	enum FX_TYPE
	{
		FX_TYPE_DEFAULT		= 0,
		FX_TYPE_2_0_LM		= 1,
		FX_TYPE_2_0_VC		= 2,
		FX_TYPE_2_0_AC		= 3,
		FX_TYPE_2_0_DECAL	= 4,
		FX_TYPE_LBUFFER		= 5,
		FX_TYPE_3_0_LM		= 6,
		FX_TYPE_3_0_VC		= 7,
		FX_TYPE_3_0_AC		= 8,
		FX_TYPE_3_0_DECAL	= 9,	// TnL_PS_3_0_NM �̻� Decal
		FX_TYPE_CUSTOM		= 10,
	};

	enum EMDECAL_BLEND_TYPE
	{
		EMDECAL_BLEND_NONE			= 0,
		EMDECAL_BLEND_ALPHABLEND	= 1,
		EMDECAL_BLEND_ALPHATEST		= 2,
		EMDECAL_BLEND_MULTIPLY		= 3,
		EMDECAL_BLEND_ADD			= 4,
	};

	enum EMSURFACEPROP
	{
		EMSURFACEPROP_NULL		= 0,
		EMSURFACEPROP_WATER		= 1,
		EMSURFACEPROP_CUSTOM_SOFT_ALPHA		= 2,
		EMSURFACEPROP_DECAL					= 3,
		EMSURFACEPROP_CUSTOM_UNLIT_UNFOG	= 4,
	};

	enum RENDER_MODE
	{
		EMRM_NULL = 0,
		EMRM_TOOL = 1,
		EMRM_MATERIAL = 2,
		EMRM_TEST = 3,
		EMRM_TEST_TOOL = 4,
	};

	enum UNLM_RENDER_MODE
	{
		EMUNLM_RM_NULL = 0,
		EMUNLM_RM_LIGHTMAP = 1,
		EMUNLM_RM_AMBIENTCUBE = 2,
		EMUNLM_RM_VOXELCOLOR = 3,
	};

	void SetPath( const TSTRING& path );
	void Init( LPDIRECT3DDEVICEQ pd3dDevice );
	void Delete();

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	//void Reload( LPDIRECT3DDEVICEQ pd3dDevice );
	void SimpleReload( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnlyVersionUpForTool();

	//EMBLEND_OPTION GetBlendType( const TSTRING& strMaterialName );
	//int IsDecal( const TSTRING& strMaterialName );
	//int IsDecalRenderType( const TSTRING& strMaterialName );		// return EMDECAL_BLEND_TYPE
	//int IsDecalOrder( const TSTRING& strMaterialName );				// 1, 2, 3 // 1 �� ���� �����Ѵ�.
	//int IsCastShadowLightMap( const TSTRING& strMaterialName );		// (-1) Material ��ü�� ����. (0) CastShadow ���Ѵ�. (1) CastShadow �Ѵ�.
	//BOOL IsReceiveShadowLM( const TSTRING& strMaterialName );
	//BOOL Is2SideMesh( const TSTRING& strMaterialName );
	BOOL IsWater( const TSTRING& strMaterialName );
	BOOL IsCustomSoftAlpha( const TSTRING& strMaterialName );
	BOOL IsCustomUNLIT_UNFOG( const TSTRING& strMaterialName );
	//int GetLightMapIndex( const TSTRING& strMaterialName );

	void SetTime( float fTime );
	void FrameMove( const D3DXMATRIX& matView, 
					const D3DXMATRIX& matProj, 
					LPDIRECT3DTEXTUREQ pBackBufferTex, 
					LPDIRECT3DTEXTUREQ pBackBufferTex_2nd, 
					LPDIRECT3DTEXTUREQ pBackBufferTex_3rd,
					LPDIRECT3DTEXTUREQ pBackBufferTex_4th );
	void SetViewProjection( LPDIRECT3DDEVICEQ pd3dDevice );		// WorldEdit ������ ����.
	void SetOverLightPower_PS_2_0( float fOverLightPower );	// WorldEdit ������ ����.
    void SetFogColorForEditor( DWORD dwColor );
	void SetWindowSize( DWORD dwWidth, DWORD dwHeight );		// WorldEdit ������ ����.
	void SetEditLightMapPower( float fPower );					// WorldEdit ������ ����.
	float GetEditLightMapPower();								// WorldEdit ������ ����.
	void SetFogParam( float fNearPlane, float fFarPlane );
	void SetOverLighting_Values( BOOL bOldMaterialMap_2_0 );		// OverLight�� ������ ��ġ�� �����ϴ� ��

	//BOOL ExistMaterial( const TSTRING& strMaterialName );

	// sMaterial_Data.m_rMaterial, sMaterial_Data.m_rMaterial->m_pFX ���� ��ȿ�ؾ� TRUE �� �ǳٴ�.
	BOOL Active( LPDIRECT3DDEVICEQ pd3dDevice, 
				const TSTRING& strMaterialName, 
				RENDER_MODE emRenderMode, 
				UNLM_RENDER_MODE emUnLMRenderMode,
				MATERIAL_DATA& sMaterial_Data,
				BOOL bDecal,
				const D3DXMATRIX& matWorld, 
				float fAlpha,
				BOOL bVertexColor,
				const MaterialValue& sMaterialValue,
				BOOL bNormalMap );

	// Tool ���� ���ǰ�, Deffered Render ���̴�.
	BOOL ActiveDefferedForTool( LPDIRECT3DDEVICEQ pd3dDevice, 
								const TSTRING& strMaterialName, 
								RENDER_MODE emRenderMode, 
								UNLM_RENDER_MODE emUnLMRenderMode,
								MATERIAL_DATA& sMaterial_Data,
								BOOL bDecal,
								const D3DXMATRIX& matWorld, 
								float fAlpha,
								BOOL bVertexColor,
								const MaterialValue& sMaterialValue,
								BOOL bNormalMap );

	// sMaterial_Data.m_rMaterial, sMaterial_Data.m_rMaterial->m_pFX ���� ��ȿ�ؾ� TRUE �� �ǳٴ�.
	BOOL Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, 
								const TSTRING& strMaterialName, 
								UNLM_RENDER_MODE emUnLMRenderMode,
								MATERIAL_DATA& sMaterial_Data,
								BOOL bDecal,
								BOOL bVertexColor,
								int nRenderMode,	// nRenderMode = 0-Default, 1-CubeMap, 2-Specular, 3-CubeMap&Specular
								BOOL bNormalMap );

	void SetFX_CommonParameter( LPD3DXEFFECT pFX, FX_TYPE emFxType );
	void SetFX_CommonParameter_Field( LPD3DXEFFECT pFX, FX_TYPE emFxType, D3DXVECTOR4* pvAmbientCube, D3DXVECTOR4& vVoxelColor );
	void SetFX_CommonParameter_Reset();

	void SetFX_CommonParameter_Small( LPD3DXEFFECT pFX, float fAlpha, const D3DXMATRIX& matWorld );
	void SetFX_CommonParameter_Small_Fog( LPD3DXEFFECT pFX );

	void SetFX_Deffered( LPD3DXEFFECT pFX );
	void SetFX_DefferedTool( LPD3DXEFFECT pFX );

	void SetFX_SpotLight( const WORDWORD& wwLightID, const D3DXMATRIX& matWorld, LPD3DXEFFECT pFX );

	void SetFX_SpotLightEDITMODE( LPD3DXEFFECT pFX );

	void SetFX_Parameter( const MATERIAL_DATA& sMaterial_Data, LPD3DXEFFECT pFX );

	void Archive( const TSTRING& strName, boost::function<void (const TCHAR*)> pArchive );

	void ChangeSimpleFullPath( TSTRING& strSrc );

	BOOL LoadTexture_NonThread( const TSTRING& strMaterialName );

	LPDIRECT3DTEXTUREQ GetWhiteTexture();
	LPDIRECT3DTEXTUREQ GetBlackTexture();
	LPDIRECT3DTEXTUREQ GetMaskTexture();

	const LPD3DXEFFECT GetDefferedFX_Tool();
	const LPD3DXEFFECT GetDefferedFX();

	// FX - Tool
	void SetDefaultMaterialTool_Deffered( std::string& strFX );

	// FX - Game
	void SetDefaultMaterial_Deffered( std::string& strFX );

	// FX - Tool
	void SetLightMapMaterialTool( std::string& strFX );
	void SetVoxelColorMaterialTool( std::string& strFX );
	void SetAmbientCubeMaterialTool( std::string& strFX );
	void SetDecalMaterialTool( std::string& strFX );

	// FX - Tool. NormalMap
	void SetLightMapMaterialTool_NM( std::string& strFX );
	void SetVoxelColorMaterialTool_NM( std::string& strFX );
	void SetAmbientCubeMaterialTool_NM( std::string& strFX );
	void SetDecalMaterialTool_NM( std::string& strFX );

	// FX - [Albedo] ShaderModel.2.0
	void SetDefaultFX( std::string& strFX );
	void SetVoxelColorFX( std::string& strFX );
	void SetVoxelColorVertexColorFX( std::string& strFX );
	void SetAmbientCubeFX( std::string& strFX );
	void SetAmbientCubeVertexColorFX( std::string& strFX );
	void SetDecalFX( std::string& strFX );

	// FX - [Albedo] ShaderModel.3.0
	void SetDefault_PS30_FX( std::string& strFX );
	void SetVoxelColor_PS30_FX( std::string& strFX );
	void SetVoxelColorVertexColor_PS30_FX( std::string& strFX );
	void SetAmbientCube_PS30_FX( std::string& strFX );
	void SetAmbientCubeVertexColor_PS30_FX( std::string& strFX );
	void SetDecal_PS30_FX( std::string& strFX );

	// FX - [Albedo] ShaderModel.3.0.NormalMap
	void SetDefaultNM_FX( std::string& strFX );
	void SetVoxelColorNM_FX( std::string& strFX );
	void SetVoxelColorVertexColorNM_FX( std::string& strFX );
	void SetAmbientCubeNM_FX( std::string& strFX );
	void SetAmbientCubeVertexColorNM_FX( std::string& strFX );
	void SetDecalNM_FX( std::string& strFX );

	//// FX - [Shading] ShaderModel.2.0
	//void SetLightMapMaterial_PS20_Shading( std::string& strFX );
	//void SetVoxelColorMaterial_PS20_Shading( std::string& strFX );
	//void SetAmbientCubeFX_PS20_Shading( std::string& strFX );

	// FX - [Shading] ShaderModel.2.0.ShadowOff
	void SetLightMapMaterial_PS20_Shading_ShadowOff( std::string& strFX );
	void SetVoxelColorMaterial_PS20_Shading_ShadowOff( std::string& strFX );
	void SetAmbientCubeFX_PS20_Shading_ShadowOff( std::string& strFX );

	// FX - [Shading] ShaderModel.3.0
	void SetLightMapMaterial_PS30_Shading( std::string& strFX );
	void SetVoxelColorMaterial_PS30_Shading( std::string& strFX );
	void SetAmbientCubeFX_PS30_Shading( std::string& strFX );

	// FX - [Shading] ShaderModel.3.0.CascadeShadow
	void SetLightMapMaterial_POS_NOR_TEX2_PS30_CSM( std::string& strFX );
	void SetVoxelColorMaterial_PS30_CSM( std::string& strFX );
	void SetAmbientCubeMaterial_PS30_CSM( std::string& strFX );

	//// FX - [Shading] ShaderModel.3.0.CascadeShadow.NormalMap
	//void SetLightMapMaterial_POS_NOR_TEX2_PS30_CSM_NM( std::string& strFX );
	//void SetVoxelColorMaterial_PS30_CSM_NM( std::string& strFX );
	//void SetAmbientCubeMaterial_PS30_CSM_NM( std::string& strFX );

	// User �� ���� FX �� �����ϴ� ����
	extern std::map<TSTRING,ID3DXEffect*>	g_mapCustomFX;

	// System ���� ���� FX �� �����ϴ� ����
	extern std::vector<ID3DXEffect*>		g_vecBaseFX;
};

#include "../../DxTools/TextureManager.h"

struct MaterialFXnTYPE
{
	ID3DXEffect*					m_rFX;
	NSMaterialManager::FX_TYPE		m_emFxType;
	D3DXHANDLE						m_hTechnique_runtime_1;
	D3DXHANDLE						m_hTechnique_runtime_2;
	D3DXHANDLE						m_hTechnique_runtime_3;
	D3DXHANDLE						m_hTechnique_Instancing;
	LPDIRECT3DVERTEXDECLARATION9	m_pInstancingDCRT;			// Instancing LPDIRECT3DVERTEXDECLARATION9

	MaterialFXnTYPE();
	~MaterialFXnTYPE();
};

struct Material
{
	BOOL	m_bLoad;

	// --------------------- ���� 6���� ������ ����ü�̴�.
	TSTRING							m_strFX;
	MaterialFXnTYPE					m_sFXnType;
	MaterialFXnTYPE*				m_rFXnType_PS30_DF;	// Mesh�� TnL_PS_3_0_NM �̻����� ����������� TnL_PS_3_0_DF ������ ����ϰ� �; ����. (Radiosity)(�ݻ�) < ������ �ϵ��� �Ѵ�. >
	MaterialFXnTYPE*				m_rFXnType_LBuffer;	// LBuffer ������ ���Ǿ�����. < ������ �ϵ��� �Ѵ�. >
	MaterialSmall					m_sMaterialGAME;
	MaterialSmall					m_sMaterialTOOL;
	BOOL							m_bLightMap;
	BOOL							m_bLightMap_float4;
	int								m_nLightMapIndex;
	DWORD							m_dwVerSection;		// 32 bit �� ��� ������ �ø���. 0xffffffff �� f �ϳ��� �ϳ��� ������ ���δ�.
	

	//int 	m_nDecalBlendType;		// EMDECAL_BLEND_TYPE
	//int		m_nDecalOrder;
	//BOOL	m_bCastShadowLightMap;
	//BOOL	m_bReceiveShadowLM;
	//BOOL	m_b2SideMesh;
	NSMaterialManager::EMSURFACEPROP	m_emSurfaceProp;		// EMSURFACEPROP
	TSTRING								m_strSurfaceProp;

	std::vector<Constants>	m_vecConstants;
	std::vector<Textures>	m_vecTextures;
	std::vector<Textures>	m_vecCubeTextures;

	Material();
	~Material();

	void OnlyVersionUpForTool();
	void RefreshFX( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwVerSection );
	void RefreshFX_Default( LPDIRECT3DDEVICEQ pd3dDevice, std::string& strFX, int nRenderMode, DWORD dwVerSection, NSMaterialManager::FX_TYPE emFxType );	// Ư�� ��Ʈ�� ���ǵ��� ��.
	void OnResetDevice_Reload( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFile, std::string& strFX, int nRenderMode, DWORD dwVerSection );

	void ResetTechniqueHandle();
	//void OnResetDevice();
	//void OnLostDevice();
	void CreateData( LPDIRECT3DDEVICEQ pd3dDevice );
	void LoadTexture( bool bThreadLoading );
	bool ParseCommentLua( LPDIRECT3DDEVICEQ pd3dDevice, const sc::CFileFindTree& sfileTree );
	HRESULT LuaLoad( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& FullPath, const std::string& _FileName, const sc::CFileFindTree& sfileTree );
};
