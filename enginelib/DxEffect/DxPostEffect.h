#pragma once

//--------------------------------------------------------------------------------------
// DxPostEffect by luxes.
//--------------------------------------------------------------------------------------

#include "../DxTools/DxVertexFVF.h"

// vertex format.

struct VERTEX_OF_POSTEFFECT
{
	enum { FVF=D3DFVF_XYZRHW | D3DFVF_TEX1 };
	D3DXVECTOR4 pos;
	D3DXVECTOR2 tex;
};

 class DxPostEffect
 {
 public:
	 DxPostEffect(void);
	 virtual ~DxPostEffect(void);

	 HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	 HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	 HRESULT InvalidateDeviceObjects();

 private:
	bool						m_bEnableHW;			// 하드웨어상 가능한지 불가능한지 체크
	BOOL						m_bActivePostSurface;	// 후처리 Surface 동작 하는지 확인.

	 VERTEX_OF_POSTEFFECT		m_sScreenVB[ 4 ];
	 LPDIRECT3DSURFACEQ			m_pBaseSurface;
	 LPDIRECT3DSURFACEQ			m_pBaseSurfaceZ;
	 LPDIRECT3DSURFACEQ			m_pCurrentSurface;
	 LPDIRECT3DSURFACEQ			m_pCurrentSurfaceZ;

	 LPDIRECT3DTEXTUREQ			m_pTexFinal;
	 LPDIRECT3DSURFACEQ			m_pSurFinal;
	 LPDIRECT3DSURFACEQ			m_pSurFinalZ;

	 LPDIRECT3DSTATEBLOCK9		m_pSavedSB;
	 LPDIRECT3DSTATEBLOCK9		m_pDrawSB;

	 D3DSURFACE_DESC			m_d3dsdBackBuffer;

	 UINT						m_nWidth;
	 UINT						m_nHeight;

	 DWORD						m_dwSate[ 30 ];
	 float						m_fState[ 5 ];
	 D3DCOLOR					m_dwColor;

	 HRESULT RenderFinal( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSourceTexture ); // render fianl texture.

 public:
	 HRESULT InitFinalTarget( LPDIRECT3DDEVICEQ pd3dDevice ); // init radial blur. by luxes.
	 HRESULT InitShader( LPDIRECT3DDEVICEQ pd3dDevice );

	 LPDIRECT3DTEXTUREQ GetFinalTarget()	{ return m_pTexFinal; }
	 LPDIRECT3DSURFACEQ GetFinalSurface()	{ return m_pSurFinal; }

	 bool IsPostEffectEnable();
	 BOOL IsActivePostSurface() const		{ return m_bActivePostSurface; }

	 //void PostEffectActiveOn();
	 //void PostEffectActiveOff();

	 //void SetTargetNull( LPDIRECT3DDEVICEQ pd3dDevice );

	 void ChangeFinalTarget( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bZClear ); // change target for post effect.
	 void RecoveryTargetFromFinal( LPDIRECT3DDEVICEQ pd3dDevice ); // recovery target from post effect.

	 HRESULT RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice ); // render post effect.

	 // for radial blur.
 private:
	 VERTEX_OF_POSTEFFECT		m_sRadialBlurVB[ 4 ];
	 LPD3DXEFFECT				m_pRadialBlurEFF;
	 D3DXHANDLE					m_hTechRadialBlur;
	 D3DXHANDLE					m_hTexRadialBlur;
	 D3DXHANDLE					m_hVecCenter;
	 D3DXHANDLE					m_hRadialBlurStart;
	 D3DXHANDLE					m_hRadialBlurWidth;
	 D3DXHANDLE					m_hRadialBlurStrength;
	 //LPDIRECT3DTEXTUREQ			m_pTexRadialBlur;
	 //LPDIRECT3DSURFACEQ			m_pSurRadialBlur;
	 //LPDIRECT3DSURFACEQ			m_pSurRadialBlurZ;

	 bool						m_bRadialBlurSwitch;
	 bool						m_bRadialBlurFade;			// false = fade in, true = fade out.
	 D3DXVECTOR2				m_vecRadialBlurCenter;		// 모든 원소는 0~1사이여야 한다. default = D3DXVECTOR2( 0.5f, 0.5f );
	 float						m_fRadialBlurStart;			// 샘플링할 벡터 위치. default = 0.0f;
	 float						m_fRadialBlurWidth;			// 현재 샘플링할 벡터 길이. default = 0.0f;
	 float						m_fRadialBlurWidthMAX;		// 샘플링할 벡터 길이 최대값. default = 0.1f;
	 float						m_fRadialBlurStrength;		// 현재 radial blur의 정도. default = 2.2f;
	 float						m_fRadialBlurStrengthMAX;	// radial blur의 정도 최대값. default = 2.2f;
	 float						m_fRadialBlurRenderSpeed;	// render speed. default = 1.0f;

	 bool						m_bFadeConinue;			// radial blur가 계속 켜져 있을때, 어느정도 지속적 변화를 주기 위한 변수. false = 감소, true = 증가.

	 HRESULT RenderRadialBlur( LPDIRECT3DDEVICEQ pd3dDevice,
								 LPDIRECT3DTEXTUREQ pSourceTexture,
								 LPDIRECT3DSURFACEQ pBackUpSurface,
								 LPDIRECT3DTEXTUREQ pBackUpTexture ); // render radial blur. by luxes.

 public:
	 //HRESULT InitRadialBlur( LPDIRECT3DDEVICEQ pd3dDevice ); // init radial blur. by luxes.

	 void RadialBlurActiveOn( float fStart = 0.0f
		 , float fWidthMAX = 0.1f, float fStrength = 1.5f, float fSpeed = 1.0f );
	 void RadialBlurActiveOff();

	 bool IsRadialBlurON();

	 HRESULT FrameMoveRadialBlur( float m_fTime, float m_fElapsedTime );

 private:
	 bool IsRadialBlurActiveON();
	 bool IsRadialBlurActiveOff();

 public:
	 void Check_SetDeactivePostSurface();

 public:
	 static DxPostEffect& GetInstance();
 };