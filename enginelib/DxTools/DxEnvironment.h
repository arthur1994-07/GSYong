#ifndef _DX_ENVIRONMENT_H_
#define _DX_ENVIRONMENT_H_

#pragma once

#include "DxViewPort.h"
#include "./TextureManager.h"

class DxFrameMesh;
class DxLandMan;
class DxOctree;
class DxSkinCharBase;
class DxVehicle;

// 예전 버젼 반사 굴절 때문에 이런게 생겨남.
namespace NSENVIRONMENT
{
	void SetReflectONOFF( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOn );
	void SetReflect( const D3DXPLANE& sPlane );
	void SetReflectShader( BOOL bUse, const D3DXPLANE& sPlane );
	void SetReflectClip( const LPDIRECT3DDEVICEQ pd3dDevice );
	void SetReflectClipShader( const LPDIRECT3DDEVICEQ pd3dDevice );

	void SetRefractONOFF( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOn );
	void SetRefract( const D3DXPLANE& sPlane );
	void SetRefractShader( BOOL bUse, const D3DXPLANE& sPlane );
	void SetRefractClip( const LPDIRECT3DDEVICEQ pd3dDevice );
	void SetRefractClipShader( const LPDIRECT3DDEVICEQ pd3dDevice );
};

class DxEnvironment
{
protected:
	static LPDIRECT3DSTATEBLOCK9		m_pSavedBasePosTexSB;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawBasePosTexSB;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedWaveSB;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawWaveSB;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedDualSB;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawDualSB;

	static	LPDIRECT3DSTATEBLOCK9		m_pSavedWaveTexSB;
	static	LPDIRECT3DSTATEBLOCK9		m_pDrawWaveTexSB;

	static	LPDIRECT3DSTATEBLOCK9		m_pSavedNightViewSB;
	static	LPDIRECT3DSTATEBLOCK9		m_pDrawNightViewSB;

protected:
	D3DXMATRIX			m_matReflectView;
	D3DXMATRIX			m_matReflectProj;
	D3DXMATRIX			m_matReflection;
	D3DXMATRIX			m_matRefraction;

	D3DXVECTOR3			m_vCenter;
	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;
	BOOL				m_bFar;

protected:
	enum	EM_USEREFLECT
	{
		EMUR_PREV	= 0,		// 예전 View 포트를 돌려서 만든 것		- 예전에 만듬
		EMUR_RECENT	= 1,		// 최근 World 데이터를 돌려서 만든 것	- 최근에 만듬
	};

	enum	EM_HEIGHT_STATS
	{
		EMHS_NODATA	= 0,
		EMHS_INIT	= 1,
		EMHS_END	= 2,
	};

	BOOL			m_bHeightSAME;
	EM_HEIGHT_STATS	m_emHeightStats;

protected:
	BOOL				m_bUseTexture;
	LPDIRECT3DTEXTUREQ	m_pTexture;

protected:
    TextureResource m_textureResSunSpecular;

protected:
	BOOL				m_bRefractPlay;
	BOOL				m_bReflectPlay;
	BOOL				m_bReflectPlayEX;

	BOOL				m_bFullTwistPlay;

public:
	void SetRefractPlayON()		{ m_bRefractPlay = TRUE; }

protected:
	DWORD				m_dwTexDetail;
	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProj;
	D3DXVECTOR3			m_vViewMax;
	D3DXVECTOR3			m_vViewMin;

	D3DXMATRIX			m_matLastVP;

protected:
	float				m_fTime;

protected:
	struct VERTEX
	{
		D3DXVECTOR3	vPos;
		D3DCOLOR	cDiffuse;
		D3DXVECTOR2	vTex1;
		D3DXVECTOR2	vTex2;
		static const	DWORD	FVF;

		VERTEX () :
			vPos(0.f,0.f,0.f),
			cDiffuse(0xffffffff),
			vTex1(0.f,0.f),
			vTex2(0.f,0.f)
		{
		}
	};

	struct POS_DIFF_TEX1
	{
		D3DXVECTOR3	vPos;
		D3DCOLOR	cDiffuse;
		D3DXVECTOR2	vTex;
		static const	DWORD	FVF;

		POS_DIFF_TEX1 () :
			vPos(0.f,0.f,0.f),
			cDiffuse(0xffffffff),
			vTex(0.f,0.f)
		{
		}
	};

	struct VERTEX_POS_TEX1
	{
		D3DXVECTOR3	vPos;
		D3DXVECTOR2	vTex;
		static const	DWORD	FVF;

		VERTEX_POS_TEX1 () :
			vPos(0.f,0.f,0.f),
			vTex(0.f,0.f)
		{
		}
	};

	struct D3DWAVETEX
	{
		D3DXVECTOR4			vPos;
		D3DXVECTOR2			vTex;
		static const	DWORD	FVF;
	};	

protected:
	static	LPDIRECT3DVERTEXBUFFERQ	m_pWaveTexVB;
	static	LPDIRECT3DINDEXBUFFERQ	m_pWaveTexIB;

	static	LPDIRECT3DVERTEXBUFFERQ	m_pTwistVB;
	static	LPDIRECT3DINDEXBUFFERQ	m_pTwistIB;

	enum { TWIST_VERTEX = 21, TWIST_FACES = 28 };	//36 };

	class DxWave
	{
	public:
		enum { REFRACT_VERTEX = 8, REFRACT_FACES = 6 };

		static POS_DIFF_TEX1	aRefract[REFRACT_VERTEX];	// 생성할때 초기화를 부른다. OK
		static VERTEX_POS_TEX1	aTwist[TWIST_VERTEX];		// 일그러트릴 표면을 읽어온다.

	public:
		BOOL			bBiil;		// 빌보드			ON / OFF
		DWORD			dwType;		// 타입			0(VBIB)	1(흔듬)	2(Sine)	3(퍼짐)	4(모임)
		D3DXVECTOR3		vPos;		// 중심점			X		O		O		O		O
		D3DXMATRIX		matWorld;	// 월드 M			O		X		X		X		X
		DWORD			dwVertics;	// 점 갯수			O		X		X		X		X
		DWORD			dwFaces;	// 면 갯수			O		X		X		X		X
		float			fSize;		// 크기				X		O		O		O		O
		float			fAlpha;		// 알파				O		O		O		O		O
		float			fBumpMat;	// 범프상수			O		O		O		X		X
		float			fLifeTime;	// 생명력			X		X		X		O		O
		float			fSpeed;		// 속도				X		X		O		X		X

		LPDIRECT3DVERTEXBUFFERQ pVBuffer;	//			O		X		X		X		X
		LPDIRECT3DINDEXBUFFERQ	pIBuffer;	//			O		X		X		X		X

		LPDIRECT3DTEXTUREQ		pTexture;

		DxWave*			pNext;

		DxWave () :
			bBiil(TRUE),
			dwType(0),
			vPos(0.f,0.f,0.f),
			dwVertics(0),
			dwFaces(0),
			fSize(0),
			fAlpha(0),
			fBumpMat(3.f),
			fLifeTime(1.f),
			pVBuffer(NULL),
			pIBuffer(NULL),
			pTexture(NULL),
			pNext(NULL)
		{
			D3DXMatrixIdentity ( &matWorld );
		}

		~DxWave ()
		{
			SAFE_DELETE ( pNext );
		}

		HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
		HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

		HRESULT	RenderVB ( LPDIRECT3DDEVICEQ pd3dDevice, float fTime, D3DXVECTOR3 vViewMax, D3DXVECTOR3 vViewMin, LPDIRECT3DTEXTUREQ pBumpTex, LPDIRECT3DTEXTUREQ pUserTex );
		HRESULT	RenderBlur ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTwistTex );
		HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, float fTime, LPDIRECT3DTEXTUREQ pUserTex, LPDIRECT3DSURFACEQ pTwistSurface, LPDIRECT3DTEXTUREQ pTwistTex );
		HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice );
		void	DxTextureTwist ( LPDIRECT3DDEVICEQ pd3dDevice, float fTime, LPDIRECT3DTEXTUREQ pUserTex, LPDIRECT3DSURFACEQ pTwistSurface );

		void	DxInitTwistBillboard ();
		void	DxInitTwistGround ();
	};

protected:
	DxWave*		m_pWaveList;

public:
	void	SetWave ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBiil, DWORD dwType, D3DXVECTOR3& vPos, float fSize, float fAlpha, float fBumpMat, float fLifeTime, float fSpeed );
	void	SetWave ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwType, LPDIRECT3DTEXTUREQ pTex );
	void	SetWave ( LPDIRECT3DDEVICEQ pd3dDevice, VERTEX* pData, DWORD dwFaces, float fAlpha );
	void	SetWaveWater ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matWorld, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwVertics, DWORD dwFaces, float fAlpha );
	void	SetWaveBlur ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, DWORD dwVertics, DWORD dwFaces );
	HRESULT	RenderWave ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pBumpTex );

	void	DxUpdateWaveTex ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pUserTex, LPDIRECT3DSURFACEQ pTwistSurface );

public:
	const D3DXVECTOR3&	GetViewMax () const			{ return m_vViewMax; }
	const D3DXVECTOR3&	GetViewMin () const			{ return m_vViewMin; }
	const D3DXVECTOR3&	GetCenter () const			{ return m_vCenter; }			// 이건 이전맵에서 쓰이는 것. MaterialMap 은 이값을 사용하지 않는다. DxLandMan::GetRealTimeWaterHeight() 사용함.
	BOOL				IsRecentReflect () const	{ return m_bHeightSAME; }

public:
	const D3DXMATRIX&			GetMatrix_ReflectView() const	{ return m_matReflectView; }
	const D3DXMATRIX&			GetMatrix_Reflection() const		{ return m_matReflection; }

public:
	void		UserToRefraction ( LPDIRECT3DDEVICEQ pd3dDevice );

	// 이전맵에서 꼭 불려져야할 함수
	HRESULT		SetReflection ( D3DXVECTOR3 vCenter, BOOL bHeight=FALSE, BOOL bFar=FALSE, D3DXVECTOR3 vMax = D3DXVECTOR3(0.f,0.f,0.f), D3DXVECTOR3 vMin = D3DXVECTOR3(0.f,0.f,0.f) );

	// 캐릭이 다 불려지면 m_bReflectPlay 를 FALSE 한다.
	// 이전 맵은 물이 그려지면 SetReflection 을 불러서 m_bReflectPlay 를 TRUE 로 만든다.
	// 최근 맵은 무조건 m_bReflectPlay 를 TRUE 로 만든다. (MaterialSystem)
	void		EndReflection( BOOL bUsedMaterialSystem );

protected:
	void	NightViewRender ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT RenderReflection ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );
	HRESULT RenderReflectionEX ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

public:
	HRESULT RenderRefelctChar ( DxSkinCharBase *pSkinObj, LPDIRECT3DDEVICEQ pd3dDevice, float fWaterHeight );
	//HRESULT RenderRefelctChar ( DxVehicle *pSkinObj, const D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );
	void RenderReflectionMaterial( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan );

public:
	void ChangeMapInitData();

public:
	enum EDIT_WATER_TYPE
	{
		RENDER_WATER_ZERO		     = 0,	// EDIT 모드가 아니다. 게임 셋팅대로 작동된다.
		RENDER_WATER_RT_REFLECTION   = 1,	// 실시간 반사 모드
		RENDER_WATER_CUBEMAP		 = 2,	// CubeMap 모드
		RENDER_WATER_DIFFUSE_TEXTURE = 3,	// Diffuse Texture 모드
	};
	void SetRenderEditMode( EDIT_WATER_TYPE emEditWaterType )	{ m_emEditWaterType = emEditWaterType; }

	BOOL IsRealTimeReflection();		// 옵션과 m_emEditWaterType 를 동시에 본다.
	BOOL IsEditModeDiffuseTexture();	// WorldEdit 상 Water를 DiffuseTexture 로 렌더링 할 것인가~?

private:
	EDIT_WATER_TYPE	m_emEditWaterType;

private:
	BOOL		m_bReflectionCV;	// 반사가 동작되는지 셋팅해 놓는다.
	CLIPVOLUME	m_sReflectionCV;	// 반사 CV 를 저장해 놓는다.

public:
	void				ResetReflectionCV();						//
	void				SetReflectionCV( const CLIPVOLUME& sCV );	//
	BOOL				IsReflectionCV() const;
	const CLIPVOLUME&	GetReflectionCV() const;

protected:
	DxEnvironment();

public:
	virtual ~DxEnvironment();

public:
	static DxEnvironment& GetInstance();
};

#endif // _DX_ENVIRONMENT_H_