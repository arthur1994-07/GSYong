#pragma once

#include <boost/function.hpp>

#include "../Material/MaterialDef.h"
#include "../DxVertexFVF.h"

struct MATERIAL_SKIN_DATA;
struct EffectMaterialData;

namespace NSMaterialSkinManager
{
	enum EMSURFACEPROP_SKIN
	{
		EMSURFACEPROP_SKIN_NULL					= 0,
		EMSURFACEPROP_SKIN_CUSTOM_SOFT_ALPHA	= 1,
	};

	extern TSTRING				g_strPath;

	void SetPath( const TSTRING& path );
	void Init( LPDIRECT3DDEVICEQ pd3dDevice );
	void Delete();

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	void Reload( LPDIRECT3DDEVICEQ pd3dDevice );
	void SimpleReload( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnlyVersionUpForTool();

	BOOL IsCustomSoftAlpha( const TSTRING& strMaterialName );

	void SetTime( float fTime );
	void FrameMove( const D3DXMATRIX& matView, const D3DXMATRIX& matProj );

	const D3DXMATRIX& GetMatrixView();
	const D3DXMATRIX& GetMatrixProjection();
	const D3DXMATRIX& GetMatrixViewProjection();


	// sMaterial_Data.m_rMaterial, sMaterial_Data.m_rMaterial->m_pFX 값이 유효해야 TRUE 를 건넨다.
	BOOL Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, 
								const TSTRING& strMaterialName, 
								MATERIAL_SKIN_DATA& sMaterial_Data,
								BOOL bVertexColor,
								int nRenderMode,
								BOOL bNormalMap,
								int nRenderType );	// 0 - Default, 1 - Glow, 2 - Shadow, 3 - Kajiya-Kay Hair

	void SetFX_CommonParameter( LPD3DXEFFECT pFX );
	void SetFX_CommonParameter_Reset();

	void SetMob( BOOL bMob );
	void SetDiffuseColor( const D3DXVECTOR3& vDiffuseColor, const D3DXVECTOR3& vPointLightColor );
	void GetDiffuseColor( D3DXVECTOR3& vDiffuseColor, D3DXVECTOR3& vPointLightColor );
	void SetColorPower_LooakLength( float fLength );
	void SetPosition_NSMaterialSkinManager( const D3DXVECTOR3& vPos );
	void SetFX_LightParameter( LPD3DXEFFECT pFX, const EffectMaterialData* pEffectMaterialData, float fMultiply=1.f );

	void SetFX_SpotLight( const WORDWORD& wwLightID, const D3DXMATRIX& matWorld, LPD3DXEFFECT pFX );

	//void SetFX_Parameter( const MATERIAL_SKIN_DATA& sMaterial_Data, LPD3DXEFFECT pFX );

	void Archive( const TSTRING& strName, boost::function<void (const TCHAR*)> pArchive );

	void ChangeSimpleFullPath( TSTRING& strSrc );

	void SetDefaultFX_SM1( std::string& strFX );				// 새로운 렌더링 FX
	void SetDefaultFX_Glow( std::string& strFX );				// Glow
	void SetDefaultFX_Shadow_R32F( std::string& strFX );		// Shadow
	void SetDefaultFX_Shadow_G32R32F( std::string& strFX );		// Shadow
	void SetDefaultFX_Kajiya_Kay_Hair( std::string& strFX );	// Kajiya-Kay Hair Shading
	void SetDefaultFX( std::string& strFX );					// 새로운 렌더링 FX
	void SetDefaultFX_NM( std::string& strFX );					// 새로운 렌더링 FX
};

#include "../../DxTools/TextureManager.h"

struct MaterialSkin
{
	BOOL	m_bLoad;

	// --------------------- 이하 6개의 변수는 공동체이다.
	TSTRING							m_strFX;
	ID3DXEffect*					m_pFX;
	D3DXHANDLE						m_hTechnique_runtime_Skin;
	D3DXHANDLE						m_hTechnique_runtime_Object;
	MaterialSmall					m_sMaterialSkin;
	MaterialSmall					m_sMaterialObject;
	DWORD							m_dwVerSection;		// 32 bit 중 어느 부위를 올릴지. 0xffffffff 중 f 하나가 하나의 부위로 쓰인다.
	
	NSMaterialSkinManager::EMSURFACEPROP_SKIN	m_emSurfaceProp;		// EMSURFACEPROP
	TSTRING										m_strSurfaceProp;

	std::vector<Constants>	m_vecConstants;
	std::vector<Textures>	m_vecTextures;
	std::vector<Textures>	m_vecCubeTextures;

	MaterialSkin();
	~MaterialSkin();

	void OnlyVersionUpForTool();
	void RefreshFX( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwVerSection );
	void RefreshFX_Default( LPDIRECT3DDEVICEQ pd3dDevice, std::string& strFX, int nRenderMode, DWORD dwVerSection );	// 특정 비트만 사용되도록 함.
	void OnResetDevice_Reload( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFile, std::string& strFX, int nRenderMode, DWORD dwVerSection );
	void OnResetDevice();
	void OnLostDevice();
	void CreateData( LPDIRECT3DDEVICEQ pd3dDevice );
	void LoadTexture( bool bThreadLoading );
	bool ParseCommentLua( LPDIRECT3DDEVICEQ pd3dDevice, const sc::CFileFindTree& sfileTree );
	HRESULT LuaLoad( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& FullPath, const std::string& _FileName, const sc::CFileFindTree& sfileTree );
};
