#ifndef _DX_SHADOW_MAP_H_
#define _DX_SHADOW_MAP_H_

#pragma once

#include "DxViewPort.h"
#include "./TextureManager.h"

class DxSkinChar;
class DxSkinCharBase;
class DxVehicle;

enum ENUM_LAND_QUALITY
{
	LAND_QT_LOW_LOW		= 0,
	LAND_QT_LOW			= 1,
	LAND_QT_MIDDLE		= 2,
	LAND_QT_HIGH		= 3,
	LAND_QT_HIGH_HIGH	= 4,

	LAND_QT_NSIZE		= 5,
};

class DxShadowMap
{
protected:
	struct	VERTEX
	{
		D3DXVECTOR3			p;
		D3DXVECTOR2			t;
		static const DWORD	FVF;
	};

protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedCircleSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectCircleSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedOneShadowSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawOneShadowSB;

protected:
	static		LPDIRECT3DVERTEXBUFFERQ		m_pShadowVB;
	const		static	float				m_fRange;
	const		static	float				EYE_TO_LOOKAT;			// 사각형을 뿌릴때 카메라의 거리 (Point Light)

protected:
	DWORD		m_dwUseSwShader;	// 그래픽 카드가 Shader 를 지원 하는가?
	BOOL		m_bUseMIXED;		// MIXED 인가 ? 아니면 SW 인가.. 확인하라?

	std::string			m_szCircleTex;
	TextureResource m_textureRes;		// 원 텍스쳐

	ENUM_LAND_QUALITY		m_emShadowDetail;
	UINT		m_nCurShadow;

protected:
	float				m_fDistance;
	float				m_fDistanceRate;

	float				m_fTexWidth;
	float				m_fTexHeight;

	D3DXVECTOR3			m_vBaseViewMin;
	D3DXVECTOR3			m_vBaseViewMax;

	D3DXVECTOR3			m_vViewMin;
	D3DXVECTOR3			m_vViewMax;

	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProj;
	D3DXMATRIX			m_matLastWVP_OLD;	// 구맵 직사광선 카메라의 매트릭스
	D3DXMATRIX			m_matLastWVP_D;		// 직사광선 카메라의 매트릭스
	D3DXMATRIX			m_matLastWVP_D_2nd;	// 직사광선 카메라의 매트릭스
	D3DXMATRIX			m_matLastWVP_PL1;	// PL1 카메라의 매트릭스
	D3DXMATRIX			m_matLastWVP_PL2;	// PL2 카메라의 매트릭스
	CLIPVOLUME			m_sOrthoCV;
	CLIPVOLUME			m_sOrthoCV_2nd;
	CLIPVOLUME			m_sOrthoCV_3rd;
	CLIPVOLUME			m_sPerspectiveCV_PL1;
	CLIPVOLUME			m_sPerspectiveCV_PL2;
	DWORD				m_dwSpotLightID_PL1;
	DWORD				m_dwSpotLightID_PL2;

	ID3DXEffect*        m_pFX;

protected:
	float	m_fPointDis1;
	int		m_nPointNum1;

	float	m_fPointDis2;
	int		m_nPointNum2;

	BOOL	m_bCharUse;
	BOOL	m_bImageBlurUse;

public:
	D3DXVECTOR3		GetViewMax ()			{ return m_vViewMax; }
	D3DXVECTOR3		GetViewMin ()			{ return m_vViewMin; }

public:
	LPDIRECT3DTEXTUREQ	GetCircleTexture ()							{ return m_textureRes.GetTexture(); }

	void				GetMultiply_Clip2Tex( D3DXMATRIX& matOut, const D3DXMATRIX matIn );

	const D3DXMATRIX&	GetMatrix_OLD ()								{ return m_matLastWVP_OLD; }

	const D3DXMATRIX&	GetMatrix_D ()								{ return m_matLastWVP_D; }
	const CLIPVOLUME&	GetClipVolume ()							{ return m_sOrthoCV; }

	const D3DXMATRIX&	GetMatrix_D_2nd ()								{ return m_matLastWVP_D_2nd; }
	const CLIPVOLUME&	GetClipVolume_2nd ()							{ return m_sOrthoCV_2nd; }

	const D3DXMATRIX&	GetMatrix_PL1 ()								{ return m_matLastWVP_PL1; }
	const CLIPVOLUME&	GetClipVolume_PL1 ()							{ return m_sPerspectiveCV_PL1; }

	const D3DXMATRIX&	GetMatrix_PL2 ()								{ return m_matLastWVP_PL2; }
	const CLIPVOLUME&	GetClipVolume_PL2 ()							{ return m_sPerspectiveCV_PL2; }

	DWORD				GetSpotLightID_PL1()							{ return m_dwSpotLightID_PL1; }
	DWORD				GetSpotLightID_PL2()							{ return m_dwSpotLightID_PL2; }

	ENUM_LAND_QUALITY		GetShadowDetail ()							{ return m_emShadowDetail; }
	void				SetShadowDetail ( ENUM_LAND_QUALITY Detail );

public:
	HRESULT ClearShadow ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem );
	HRESULT RenderShadow ( DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadow ( DxVehicle *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadowMob ( DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadowMob ( DxVehicle *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT RenderShadowCharMob ( DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD );
	HRESULT RenderShadowCharMob ( DxVehicle *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD );
	HRESULT RenderShadowCharMob_SpotLight( DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot );
	HRESULT RenderShadowCharMob_SpotLight( DxVehicle *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot );
	void	LastImageBlur ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem );
	void	LastImageBlurForVSM( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTex, LPDIRECT3DSURFACEQ pSur, LPDIRECT3DTEXTUREQ pTexTemp, LPDIRECT3DSURFACEQ pSurTemp, DWORD dwSize );
	void	LastImageBlurForCascadeShadow ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ pShadowSufR32F, LPDIRECT3DTEXTUREQ pShadowTexSRC, DWORD dwSize );

	// 로비에서 셋팅이 되어야 한다.
	HRESULT RenderShadowForLobbyStage( BOOL bWorld_MaterialSystem );

	// Shadow 에 필요한 Matrix 와 CV 를 구한다.
	void CaculateViewProjForMaterialSystemALL();

private:
	// CSM 이 아닌 상황에서의 matVP 구하기
	void CaculateViewProjForMaterialSystem( float fLengthMulti );

	// 적당한 카메라 범위를 구한다.
	float GetCameraLengthCascadeShadow();

	// Spot Shadow
	void CaculateViewProjForMaterialSystemSpotShadow( D3DXMATRIX &matWVP, CLIPVOLUME &sCV, const D3DLIGHTQ& sLight );
	
	// 메인 CSM
	void CaculateViewProjForMaterialSystemCascadeShadow( D3DXMATRIX &matWVP, CLIPVOLUME &sOrthoCV, float fDistance );

	// 약간 떨어져 있는 CSM
	void CaculateViewProjForMaterialSystemCascadeShadow2( D3DXMATRIX &matWVP, CLIPVOLUME &sOrthoCV, float fDistance );
	HRESULT RenderShadowForMaterialSystem( DxSkinCharBase *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderShadowForMaterialSystem_SpotLight( DxSkinCharBase *pSkinObj, 
														const D3DXMATRIX &matRot, 
														LPDIRECT3DDEVICEQ pd3dDevice, 
														const CLIPVOLUME& sCV, 
														const D3DXMATRIX& matVP, 
														LPDIRECT3DSURFACEQ pSurface,
														LPDIRECT3DSURFACEQ pZBuffer );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove ( float fTime, float fElapsedTime );

protected:
	void	SetPointLight ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar *pSkinObj, const D3DXMATRIX &matRot, 
							D3DXVECTOR3& vCharPos, D3DXVECTOR3& vLightPos, D3DXVECTOR3& vLightDirect );
	void	SetPointLight ( LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle *pSkinObj, const D3DXMATRIX &matRot, 
		D3DXVECTOR3& vCharPos, D3DXVECTOR3& vLightPos, D3DXVECTOR3& vLightDirect );

	void	SetViewProjection ();
	HRESULT	Shadow_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar *pSkinObj, const D3DXMATRIX &matRot );
	HRESULT	Shadow_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle *pSkinObj, const D3DXMATRIX &matRot );
	HRESULT	Circle_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, DWORD dwTextureFactor );
	void	ReSetClosedPL ( D3DLIGHTQ &sLight, D3DXVECTOR3 vCharPos, int nNum );

	void SetFX( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxShadowMap();

public:
	virtual ~DxShadowMap();

public:
	static DxShadowMap& GetInstance();
};

#endif // _DX_SHADOW_MAP_H_