#ifndef _DX_SURFACE_TEX_H_
#define _DX_SURFACE_TEX_H_

#pragma once

#include "DxViewPort.h"

class DxSurfaceTex
{
protected:
	enum	EMGC
	{
		EMGC_DEFAULT			= 0,
		EMGC_SAVAGE_ETC			= 1,
		EMGC_INTEL_82810E		= 2,
		EMGC_INTEL_82845G		= 3,
		EMGC_INTEL_82865G		= 4,
		EMGC_INTEL_G33_G31		= 5,
		EMGC_INTEL_ETC			= 6,
		EMGC_RIVA_TNT2			= 7,
		EMGC_GEFORCE2_MX		= 8,
		EMGC_GEFORCE4_MX_440	= 9,
		EMGC_RADEON_IGP			= 10,
	};

	EMGC	m_GRAPHIC_CARD;

protected:
	struct	VERTEX
	{
		D3DXVECTOR3			vPos;
		D3DCOLOR			cColor;
		D3DXVECTOR2			vTex;
		static const DWORD	FVF;
	};

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedPShadowSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectPShadowSB;

protected:
	static		LPDIRECT3DVERTEXBUFFERQ		m_pObjectVB;
	static		LPDIRECT3DINDEXBUFFERQ		m_pObjectIB;

	const		static	DWORD				POINTSHADOWVERTEX;
	const		static	DWORD				POINTSHADOWFACES;
	const		static	float				EYE_TO_LOOKAT;			// 사각형을 뿌릴때 카메라의 거리 (Point Light)

public:
	const		static	D3DXVECTOR4			CASCADESHADOWSIZE;

protected:
	DWORD			m_dwUseSwShader;	// 그래픽 카드가 Shader 를 지원 하는가?
	BOOL			m_bUseMIXED;		// MIXED 인가 ? 아니면 SW 인가.. 확인하라?
	BOOL			m_bUseZTexture;		// 텍스쳐에 Z 버퍼를 지원 하도록 하는가 ?
	BOOL			m_bSameZBufLastRender;		// 같은Z값에 렌더링 될경우 마지막에 렌더링되는 것이 그려지는가 ?
    DWORD           m_dwMaxUserClipPlanes;      // d3dCaps.MaxUserClipPlanes

public:
	LPDIRECT3DSURFACEQ	m_pColorBuffer;			
	LPDIRECT3DSURFACEQ	m_pZBuffer;

	LPDIRECT3DTEXTUREQ	m_pShadowTex;		// Shadow - 512
	LPDIRECT3DSURFACEQ	m_pShadowSuf;		// Shadow
	LPDIRECT3DSURFACEQ	m_pShadowZBuf;		// Shadow

	LPDIRECT3DTEXTUREQ	m_pShadowTexR32F;		// Shadow - 1024 ( R32F )
	LPDIRECT3DSURFACEQ	m_pShadowSufR32F;		// Shadow
	LPDIRECT3DSURFACEQ	m_pShadowZBuf_1024;		// Shadow

	LPDIRECT3DTEXTUREQ	m_pShadowTexG32R32F_VSM_CSM2;		// Shadow - 1024 ( G32R32F ) - cascade shadow #2
	LPDIRECT3DSURFACEQ	m_pShadowSufG32R32F_VSM_CSM2;		// Shadow

	LPDIRECT3DTEXTUREQ	m_pShadowTexR32F_1024;	// Shadow - 1024 ( R32F ) - cascade shadow #2
	LPDIRECT3DSURFACEQ	m_pShadowSufR32F_1024;	// Shadow

	LPDIRECT3DTEXTUREQ	m_pShadowTexR32F_CSM;	// Shadow - 2048 ( R32F ) - cascade shadow
	LPDIRECT3DSURFACEQ	m_pShadowSufR32F_CSM;	// Shadow
	LPDIRECT3DSURFACEQ	m_pShadowZBuf_CSM;		// Shadow

	LPDIRECT3DTEXTUREQ	m_pShadowTexR32F_PL1;	// Shadow - 1024 ( R32F ) - cascade shadow
	LPDIRECT3DSURFACEQ	m_pShadowSufR32F_PL1;	// Shadow
	LPDIRECT3DSURFACEQ	m_pShadowZBuf_PL1;		// Shadow

	LPDIRECT3DTEXTUREQ	m_pShadowTexR32F_PL2;	// Shadow - 1024 ( R32F ) - cascade shadow
	LPDIRECT3DSURFACEQ	m_pShadowSufR32F_PL2;	// Shadow
	LPDIRECT3DSURFACEQ	m_pShadowZBuf_PL2;		// Shadow

	LPDIRECT3DTEXTUREQ	m_pReflectTex;		// Reflect - 512	TEMP< Glow >	- Relfect_Deffered_Src
	LPDIRECT3DSURFACEQ	m_pReflectSuf;		// Reflect
	LPDIRECT3DSURFACEQ	m_pReflectZBuf;		// Reflect

	LPDIRECT3DTEXTUREQ	m_pTempTex512;		// Temp - 512	( 임시 그림자 )		- Relfect_Deffered_1st
	LPDIRECT3DSURFACEQ	m_pTempSuf512;		// Temp

	LPDIRECT3DTEXTUREQ	m_pTempTex256;		// Temp - 256	( 임시 굴절 )
	LPDIRECT3DSURFACEQ	m_pTempSuf256;		// Temp

	LPDIRECT3DTEXTUREQ	m_pTempTex128;		// Temp - 128	( 임시 이미지 )
	LPDIRECT3DSURFACEQ	m_pTempSuf128;		// Temp

	LPDIRECT3DTEXTUREQ	m_pTempTex64;		// Temp - 64	( 임시 이미지 )
	LPDIRECT3DSURFACEQ	m_pTempSuf64;		// Temp

	// 예전 프레임 정보를 가지고 있음. 건들면 안됨.
	LPDIRECT3DTEXTUREQ	m_pGlowTex_SRC;		// Glow - ( Src Insert )
	LPDIRECT3DSURFACEQ	m_pGlowSuf_SRC;		// Glow

	LPDIRECT3DTEXTUREQ	m_pGlowTex;			// Glow - 512	( 렌더용 글로우)
	LPDIRECT3DSURFACEQ	m_pGlowSuf;			// Glow

	LPDIRECT3DTEXTUREQ	m_pGlowTex_TEMP;	// Temp - 512	Glow 				- Relfect_Deffered_2nd
	LPDIRECT3DSURFACEQ	m_pGlowSuf_TEMP;	// Temp

	LPDIRECT3DTEXTUREQ	m_pWaveTex;			// Temp - 512	( Wave )
	LPDIRECT3DSURFACEQ	m_pWaveSuf;			// Temp

	LPDIRECT3DTEXTUREQ	m_pFlowTex;			// Flow - 128	( 흐르는 텍스쳐 )
	LPDIRECT3DSURFACEQ	m_pFlowSuf;			// Flow

	LPDIRECT3DTEXTUREQ	m_pBurnResultTex;	// Burn
	LPDIRECT3DSURFACEQ	m_pBurnResultSuf;	// Burn

	LPDIRECT3DTEXTUREQ	m_pBurnNewTex;		// Burn
	LPDIRECT3DSURFACEQ	m_pBurnNewSuf;		// Burn

	LPDIRECT3DTEXTUREQ	m_pColorTex_1st;	// Deffered - Buffer Size	기본 렌더링을 위한 것.
	LPDIRECT3DSURFACEQ	m_pColorSuf_1st;	// Deffered

	LPDIRECT3DTEXTUREQ	m_pColorTex_2nd;	// Deffered - Buffer Size	deffered 용 Normal 값을 기록하기 위해 존재한다.
	LPDIRECT3DSURFACEQ	m_pColorSuf_2nd;	// Deffered

	LPDIRECT3DTEXTUREQ	m_pColorTex_3rd;	// Deffered - Buffer Size	deffered 용 SpecularFactor 값을 기록하기 위해 존재한다.
	LPDIRECT3DSURFACEQ	m_pColorSuf_3rd;	// Deffered

	LPDIRECT3DTEXTUREQ	m_pColorTex_4th;	// Deffered - Buffer Size	deffered 용 Position 값을 기록하기 위해 존재한다.
	LPDIRECT3DSURFACEQ	m_pColorSuf_4th;	// Deffered

	LPDIRECT3DTEXTUREQ	m_pColorTex_LBuffer;// Deffered - Buffer Size	Lbuffer 용 
	LPDIRECT3DSURFACEQ	m_pColorSuf_LBuffer;// Deffered

	LPDIRECT3DTEXTUREQ	m_p512Tex_4th;		// Deffered - 512	deffered 용 Position 값을 기록하기 위해 존재한다.
	LPDIRECT3DSURFACEQ	m_p512Suf_4th;		// Deffered

	LPDIRECT3DTEXTUREQ	m_pTexFourCC_INTZ;	// Deffered - Buffer Size	deffered 용 Z 값을 기록하기 위해 존재한다.
	LPDIRECT3DSURFACEQ	m_pSufFourCC_INTZ;	// 

    LPDIRECT3DTEXTUREQ	m_pColorTex_Post;	// Temp - Buffer Size	DxPostEffect 때문에 쓰이고 있다. 탈것으로 이동시 잔상이 생긴다고 한다. & LightShaft
	LPDIRECT3DSURFACEQ	m_pColorSuf_Post;	// Temp

    LPDIRECT3DTEXTUREQ	m_pTempTex16F_256_1st;	// Temp - 256 ( 16F ) ( 임시 이미지 ) ( LightShafts )
	LPDIRECT3DSURFACEQ	m_pTempSuf16F_256_1st;	// Temp 

	LPDIRECT3DTEXTUREQ	m_pTempTex16F_256_2nd;	// Temp - 256 ( 16F ) ( 임시 이미지 ) ( LightShafts )
	LPDIRECT3DSURFACEQ	m_pTempSuf16F_256_2nd;	// Temp 

	LPDIRECT3DTEXTUREQ	m_pColorTex16F_HDR;	// HDR - Buffer Size ( 16F )
	LPDIRECT3DSURFACEQ	m_pColorSuf16F_HDR;	// HDR

	LPDIRECT3DTEXTUREQ	m_pBloomBrightTex;	// Bloom - Buffer Size / 2 ( 16F )
	LPDIRECT3DSURFACEQ	m_pBloomBrightSuf;	// Bloom

	LPDIRECT3DTEXTUREQ	m_pBloomDownSampleTex;	// Bloom - Buffer Size / 8
	LPDIRECT3DSURFACEQ	m_pBloomDownSampleSuf;	// Bloom

	LPDIRECT3DTEXTUREQ	m_pBloomDownSampleTexH;	// Bloom - Buffer Size / 8
	LPDIRECT3DSURFACEQ	m_pBloomDownSampleSufH;	// Bloom

	DWORD				m_dwSetWidth;
	DWORD				m_dwSetHeight;


protected:
	DWORD				m_dwWidth;
	DWORD				m_dwHeight;
	D3DFORMAT			m_cMainFormat;
	D3DFORMAT			m_cColorFormat;
	D3DFORMAT			m_cZFormat;

	BOOL				m_bDeviceEnable;		// 개인이 만든 서페이스로 그려야 하는지 체크
	BOOL				m_bOptionEnable;		// 옵션에서의 활성화 ( 굴절옵션임 )
	BOOL				m_bEnable;				// 굴절 사용 가능 ?

	float				m_fTime;
	int					m_nNum;

protected:
	BOOL				m_bFlowOneDraw;

public:
	DWORD		GetWidth()			{ return m_dwWidth; }		// 전체화면 크기
	DWORD		GetHeight()			{ return m_dwHeight; }		// 전체화면 크기
	D3DFORMAT	GetMainFormat()		{ return m_cMainFormat; }	// 전체화면 크기
	D3DFORMAT	GetColorFormat()	{ return m_cColorFormat; }	// 전체화면 크기
	D3DFORMAT	GetZFormat()		{ return m_cZFormat; }		// 전체화면 크기

	void		SetOptionEnable ( BOOL bUse )	{ m_bOptionEnable=bUse; }	// 옵션 용
	BOOL		IsOptionEnable ()				{ return m_bOptionEnable; }	// 굴절옵션
	BOOL		IsEnable()						{ return m_bEnable; }

	BOOL		IsFlowOneDraw ()	{ return m_bFlowOneDraw; }
	void		SetFlowUSE ()		{ m_bFlowOneDraw = TRUE; }

    DWORD       GetMaxUserClipPlanes() { return m_dwMaxUserClipPlanes; }

	BOOL		IsCorrectZBuffer()		{ return m_bSameZBufLastRender; }		// 정확한 ZBuffer

	BOOL		IsLowLowGPU()		{ return m_GRAPHIC_CARD!=EMGC_DEFAULT ? TRUE : FALSE; }

//	Note : View, Projection 설정, 텍스쳐 만들기
public:
	void	InitViewProjection ( D3DXVECTOR3& vBaseViewMax, D3DXVECTOR3& vBaseViewMin, D3DXMATRIX& matProj, const float fDistance, 
								const float fTexWidth, const float fTexHeight );
	void	SetViewProjection ( D3DXVECTOR3& _vViewMax, D3DXVECTOR3& _vViewMin, D3DXMATRIX& _matView, D3DXMATRIX& _matProj, 
								DWORD dwTexDetail, const float _fDistance, const float _fHeight );
	HRESULT	RenderPointShadow ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vViewMax, D3DXVECTOR3 vViewMin, D3DXMATRIX matLastVP, DWORD dwTexDetail );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove ( float fTime, float fElapsedTime );

	void MakeSurfaceBlackColor( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxSurfaceTex();

public:
	virtual ~DxSurfaceTex();

public:
	static DxSurfaceTex& GetInstance();
};

#endif // _DX_SURFACE_TEX_H_