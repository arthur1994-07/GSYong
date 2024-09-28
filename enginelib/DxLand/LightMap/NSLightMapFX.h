#pragma once

#include "./LightMapDef.h"

class DxTangentMesh;

namespace NSLightMapFX
{
	enum RENDER_MODE
	{
		ENRM_ZERO       = 0,
		ENRM_VS         = 1,
		ENRM_VS_PS_1_1  = 2,
		ENRM_VS_PS_1_4  = 3,
	};

	extern RENDER_MODE							g_emMode;
	extern NSLIGHTMAPDEF::LIGHTMAP_RENDERTYPE	g_emRenderType;

    void CleanUp();

    HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT OnLostDevice();

    HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice );

    void RenderGeneral( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh );
	void RenderColor( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh );
	BOOL RenderTest( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bVertexColor, DxTangentMesh* pTangentMesh, const DWORD dwAttrib );
	BOOL RenderVERTEX( LPDIRECT3DDEVICEQ pd3dDevice,  LPD3DXMESH pMesh, DWORD dwMaterials );
	BOOL Render( LPDIRECT3DDEVICEQ pd3dDevice, 
				LPDIRECT3DVERTEXBUFFERQ	pVB,
				LPDIRECT3DINDEXBUFFERQ pIB,
				int nFVFSize,
				DWORD dwFVF,
				DWORD dwVertNum, 
				DWORD dwFaceNum );
	BOOL RenderInexact( LPDIRECT3DDEVICEQ pd3dDevice,  LPD3DXMESH pMesh, DWORD dwMaterials, int nIndex );

    // FX Param 셋팅.
    void FrameMove( const D3DXMATRIX& matView, const D3DXMATRIX& matProj, float fLightMapDetail );
    void SetBaseTexture( LPDIRECT3DTEXTUREQ pTexture );
    void SetLightMapTexture( LPDIRECT3DTEXTUREQ pTextureDay, LPDIRECT3DTEXTUREQ pTextureNight );
    void SetLightMapUV_Offset( const D3DXVECTOR2& vLightMapUV_Offset );
	void SetLightMapUV_Multiply( WORDWORD wwLightMapUserUV_TextureSize );	// Piece에서 Export를 안하고 Checker 를 보기 위해 셋팅하는 것.
    void SetWorld( const D3DXMATRIX& matWorld, float fAlpha );
	void LightMapDetail( float fLightMapDetail );

    // Edit
    void ShortBlackFog();
	void SetRenderMode( NSLIGHTMAPDEF::LIGHTMAP_RENDERTYPE emType );		// Rendering 모드
	void SetLightMapCheckerName( const TSTRING& strName );
	void GetLightMapCheker( LPDIRECT3DTEXTUREQ& pTextureCombined, D3DXVECTOR4& vUV_Multiply );
	void GetLightMapUV_Multiply( D3DXVECTOR4& vUV_Multiply );
	LPDIRECT3DTEXTUREQ GetTextureChecker();
}