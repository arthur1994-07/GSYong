#pragma once

#include <queue>

#include "../../../SigmaCore/Memory/ObjectPool.h"

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxTools/DxVertexFVF.h"

#define		NOUSEORIGIN		0x0001
#define		USEREFRACT		0x0002
#define		NOUSE_BEZIER	0x0004	// 이젠 사용 하지 않는다.
#define		NOUSE_MIDDLE	0x0008	// 이젠 사용 하지 않는다.
#define		USEALLACTION	0x0010	// 모든 것에 다 나온다.

#define		TEXTURE_LENGTH	128

struct EFFCHAR_PROPERTY_BLUR_100 : public EFFCHAR_PROPERTY
{
	BOOL			m_bUse;
	BOOL			m_bBezier;
	BOOL			m_bAlphaVel;
	float			m_fAlphaVel;
	int				m_nBlend;
	DWORD			m_dwTraceIndexUP;			//	추적 포인터 인덱스.
	DWORD			m_dwTraceIndexDOWN;			//	추적 포인터 인덱스.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_100()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_BLUR_101 : public EFFCHAR_PROPERTY
{
	BOOL			m_bUse;						// 세이브 할 필요는 없어 보인다.. 다음 버전에는 밑으로 내리자.
	BOOL			m_bBezier;
	int				m_nBlend;
	DWORD			m_dwTraceIndexUP;			//	추적 포인터 인덱스.
	DWORD			m_dwTraceIndexDOWN;			//	추적 포인터 인덱스.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;
	BOOL			m_bIndexMID;
	float			m_fRateMID;
	D3DCOLOR		m_vColorMID;

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_101()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_BLUR_102 : public EFFCHAR_PROPERTY
{
	BOOL			m_bBezier;
	int				m_nBlend;
	DWORD			m_dwTraceIndexUP;			//	추적 포인터 인덱스.
	DWORD			m_dwTraceIndexDOWN;			//	추적 포인터 인덱스.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	BOOL			m_bIndexMID;				//	중간 컬러
	float			m_fRateMID;					//
	D3DCOLOR		m_vColorMID;				//

	BOOL			m_bAllMType;				//	타이밍 및 필요한 부분에 넣는 것
	BOOL			m_bAllSType;				//
	BOOL			m_bAllTime;					//
	DWORD			m_dwKeyStartTime;			//
	DWORD			m_dwKeyEndTime;				//
	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_102()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_BLUR_103 : public EFFCHAR_PROPERTY
{
	BOOL			m_bBezier;
	int				m_nBlend;
	char			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	BOOL			m_bIndexMID;				//	중간 컬러
	float			m_fRateMID;					//
	D3DCOLOR		m_vColorMID;				//

	BOOL			m_bAllMType;				//	타이밍 및 필요한 부분에 넣는 것
	BOOL			m_bAllSType;				//
	BOOL			m_bAllTime;					//
	DWORD			m_dwKeyStartTime;			//
	DWORD			m_dwKeyEndTime;				//
	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_103()
	{
		memset( m_szTraceUP, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceDOWN, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_BLUR_104 : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	char			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	float			m_fRateMID;					//
	D3DCOLOR		m_vColorMID;				//

	BOOL			m_bAllMType;				//	타이밍 및 필요한 부분에 넣는 것
	BOOL			m_bAllSType;				//
	BOOL			m_bAllTime;					//
	DWORD			m_dwKeyStartTime;			//
	DWORD			m_dwKeyEndTime;				//
	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_BLUR_104()
	{
		memset( m_szTraceUP, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceDOWN, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_BLUR_105	//	데이터 포맷 변경 진행	
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	float			m_fFullLifeTemp;
	TCHAR			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
	TCHAR			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	TCHAR			m_szTexture[ TEXTURE_LENGTH ];
};

struct EFFCHAR_PROPERTY_BLUR : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	float			m_fFullLifeTemp;
	D3DCOLOR		m_vColorUPTemp;
	D3DCOLOR		m_vColorDOWNTemp;

	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	TCHAR			m_szTexture[ TEXTURE_LENGTH ];
	TCHAR			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
	TCHAR			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.

	EFFCHAR_PROPERTY_BLUR () :
		m_nBlend(1), 
		m_fFullLifeTemp(0.25f),
		m_vColorUPTemp(D3DCOLOR_ARGB(255,255,255,255)),
		m_vColorDOWNTemp(D3DCOLOR_ARGB(0,255,255,255)),
		m_SetAniMType(AN_ATTACK),
		m_SetAniSType(AN_SUB_NONE)
	{
		m_dwFlag = 0L;

		StringCchCopy( m_szTexture, TEXTURE_LENGTH, "1d_Lighting.bmp" );

		StringCchCopy( m_szTraceUP,		STRACE_NSIZE, STRACE_IMAGE01 );
		StringCchCopy( m_szTraceDOWN,	STRACE_NSIZE, STRACE_IMAGE02 );
	}

	void Assign( EFFCHAR_PROPERTY_BLUR_105& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;

		m_nBlend = sOldData.m_nBlend ;
		m_fFullLifeTemp = sOldData.m_fFullLifeTemp;
		m_vColorUPTemp = sOldData.m_vColorUPTemp;
		m_vColorDOWNTemp = sOldData.m_vColorDOWNTemp;

		m_SetAniMType = sOldData.m_SetAniMType;
		m_SetAniSType = sOldData.m_SetAniSType;

		StringCchCopy( m_szTexture, TEXTURE_LENGTH, sOldData.m_szTexture );
		StringCchCopy( m_szTraceUP, STRACE_NSIZE, sOldData.m_szTraceUP );
		StringCchCopy( m_szTraceDOWN, STRACE_NSIZE, sOldData.m_szTraceDOWN );
	}
};

class DxEffCharBlur : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_BLUR m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;

			int				m_nBlend;
			float			m_fFullLifeTemp;
			D3DCOLOR		m_vColorUPTemp;
			D3DCOLOR		m_vColorDOWNTemp;

			EMANI_MAINTYPE	m_SetAniMType;				//
			EMANI_SUBTYPE	m_SetAniSType;				//

			TCHAR			m_szTexture[ TEXTURE_LENGTH ];
			TCHAR			m_szTraceUP[STRACE_NSIZE];		//	추적 포인터 인덱스.
			TCHAR			m_szTraceDOWN[STRACE_NSIZE];	//	추적 포인터 인덱스.
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_BLUR*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	TextureResource m_textureRes;

private:
	float		m_fTime;
	float		m_fElapsedTime;
	D3DXMATRIX	m_matIdentity;

	// Note : 애니메이션에 의해 생성된 점.
private:
	struct VERTEX_UPDOWN : public sc::PoolAllocMgr
	{
		D3DXVECTOR3	sPos[2];
	};

	typedef std::deque<VERTEX_UPDOWN*>	DEQ_STOREPOINT;

	DWORD			m_dwKeyPointCOUNT;
	DEQ_STOREPOINT	m_deqKeyPoint;

	// Note : 잔상을 만들기 위해서 계속 기록,삭제되는 점.
private:
	struct BLUR_UPDOWN : public sc::PoolAllocMgr
	{
		VERTEXCOLORTEX	sVertex[2];

		float			fLife;
	};

	typedef std::deque<BLUR_UPDOWN*>	DEQ_BLURPOINT;
	typedef DEQ_BLURPOINT::iterator		DEQ_BLURPOINT_ITER;

	DEQ_BLURPOINT	m_deqBlurPoint;

	// Note : 칼에 붙어 있는 잔상
private:
	BOOL			m_bBaseBlurRender;
	VERTEXCOLORTEX	m_sBaseBlur[4];

private:
	float		m_fOneDiveFullLife;	// 1.f / FullLife
	DWORD		m_dwPrevKeyFrame;	// 이전 키 프레임을 저장 시켜 놓는다.

private:
	float			m_fFullLife;
	D3DCOLOR		m_vColorUP;
	D3DCOLOR		m_vColorDOWN;

	float	m_fAlpha[2];	// 셋팅된 알파값
	DWORD	m_dwColor[2];	// 셋팅된 컬러 값

private:
	BOOL IsUnionKey( D3DXVECTOR3& vUp, D3DXVECTOR3& vDown );
	void CreateNewKeyFrame( D3DXVECTOR3& vUp, D3DXVECTOR3& vDown );
	void FrameMoveKeyFrame();
	void ClearKeyFrame();
	void ClearBlurDara();
	void CreateBlur();
	void FrameMoveBlur();

private:
	int SeparateNumOfLine();				// Line의 분할 갯수
	void DxInsertBlurPoint( float fRate );

private:
	void RenderBlur( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnCreateDevice_STATIC();
	static void OnDestroyDevice_STATIC();

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

private:
	void	PrecalculateBeforeRenderEff();
	void	OnlyRenderEff( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_SINGLE_EFF; }

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:
	virtual void PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix ) OVERRIDE;

	virtual void RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
											const D3DXMATRIX* pMatrix, 
											const float fScale );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행	

public:
	DxEffCharBlur(void);
	virtual ~DxEffCharBlur(void);
};
