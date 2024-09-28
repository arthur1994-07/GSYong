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
	bool						m_bEnableHW;			// �ϵ����� �������� �Ұ������� üũ
	BOOL						m_bActivePostSurface;	// ��ó�� Surface ���� �ϴ��� Ȯ��.

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
	 D3DXVECTOR2				m_vecRadialBlurCenter;		// ��� ���Ҵ� 0~1���̿��� �Ѵ�. default = D3DXVECTOR2( 0.5f, 0.5f );
	 float						m_fRadialBlurStart;			// ���ø��� ���� ��ġ. default = 0.0f;
	 float						m_fRadialBlurWidth;			// ���� ���ø��� ���� ����. default = 0.0f;
	 float						m_fRadialBlurWidthMAX;		// ���ø��� ���� ���� �ִ밪. default = 0.1f;
	 float						m_fRadialBlurStrength;		// ���� radial blur�� ����. default = 2.2f;
	 float						m_fRadialBlurStrengthMAX;	// radial blur�� ���� �ִ밪. default = 2.2f;
	 float						m_fRadialBlurRenderSpeed;	// render speed. default = 1.0f;

	 bool						m_bFadeConinue;			// radial blur�� ��� ���� ������, ������� ������ ��ȭ�� �ֱ� ���� ����. false = ����, true = ����.

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