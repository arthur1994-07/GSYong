#pragma once

#include <vector>
#include <map>
#include "../../../SigmaCore/Container/MList.h"
#include "../DxCustomTypes.h"
#include "../DxVertexFVF.h"

class DxLandMan;

namespace sc {
    class SerialFile;
}

#define STRING_LENGTH_256	256

class DXLIGHT
{
private:
    enum
    {
        VERSION = 0x102,
    };

public:
	char			m_szLightName[STRING_LENGTH_256];
	BOOL			m_bEnablePL_Day;       // 포인트 라이트 낮에 활성화
    BOOL			m_bEnablePL_Night;     // 포인트 라이트 밤에 활성화
	D3DCOLORVALUE	m_BaseDiffuse;
	D3DXMATRIX		m_matWorld;
	
	D3DLIGHTQ		m_Light;

    int             m_nAttenuation0;
    int             m_nAttenuation1;
    int             m_nAttenuation2;

	BOOL			m_bEditCheck;

	float			m_fOverLighting;		// 몇배까지 밝게 해줄것인가~?

	//	Note : 단방향 리스트.
	DXLIGHT			*pNext;

public:
	DXLIGHT ()
        : m_bEnablePL_Day(FALSE)
        , m_bEnablePL_Night(TRUE)
		, m_bEditCheck(FALSE)
		, m_fOverLighting(3.f)
		, pNext(NULL)
	{
		memset( m_szLightName, 0, sizeof(char)*STRING_LENGTH_256 );
		D3DXMatrixIdentity( &m_matWorld );
	}

	virtual ~DXLIGHT ()
	{
		//SAFE_DELETE(pNext);
	}

	void SetDefault ()
	{
		StringCchCopy( m_szLightName, STRING_LENGTH_256, "UNNAMED" );

		SecureZeroMemory( &m_Light, sizeof(D3DLIGHTQ) );
		m_Light.Type        = D3DLIGHT_DIRECTIONAL;
		m_Light.Diffuse.r   = 1.f;
		m_Light.Diffuse.g   = 1.f;
		m_Light.Diffuse.b   = 1.f;
		m_Light.Ambient.r	= 0.4f;
		m_Light.Ambient.g	= 0.4f;
		m_Light.Ambient.b	= 0.4f;
		m_Light.Specular.r   = 1.0f;
		m_Light.Specular.g   = 1.0f;
		m_Light.Specular.b   = 1.0f;

		D3DXVECTOR3 vDirection = D3DXVECTOR3(-1.0f, -2.0f, 2.0f);
		D3DXVec3Normalize ( &vDirection, &vDirection );
		m_Light.Direction = vDirection;

		m_Light.Range		= 80.0f;

		m_Light.Attenuation0 = 1.0f;
		m_Light.Attenuation1 = 0.0f;
		m_Light.Attenuation2 = 0.0f;

        m_nAttenuation0 = 100;
        m_nAttenuation1 = 0;
        m_nAttenuation2 = 0;

		//m_Ambient.a = 0.0f;
		m_BaseDiffuse.r = 1.0f;
		m_BaseDiffuse.g = 1.0f;
		m_BaseDiffuse.b = 1.0f;
	}

	void operator= ( const DXLIGHT& sLight )
	{
		StringCchCopy( m_szLightName, STRING_LENGTH_256, sLight.m_szLightName );
		m_bEnablePL_Day     = sLight.m_bEnablePL_Day;
        m_bEnablePL_Night   = sLight.m_bEnablePL_Night;
		m_BaseDiffuse	    = sLight.m_BaseDiffuse;
		m_Light			    = sLight.m_Light;
		m_matWorld		    = sLight.m_matWorld;

        m_nAttenuation0 = sLight.m_nAttenuation0;
        m_nAttenuation1 = sLight.m_nAttenuation1;
        m_nAttenuation2 = sLight.m_nAttenuation2;

		m_fOverLighting = sLight.m_fOverLighting;

		// pNext 는 대입 연산에서 제외된다.
	}

	void SetRange ( float fRange );
	virtual void SerialData ();
    BOOL IsEnablePL( BOOL bWhenDayPointOff ) const;

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	//	<--	데이타 Save/Load
	void	SaveSet ( sc::SerialFile &SFile );
	void	LoadSet ( sc::SerialFile &SFile );
	void	LoadSet_Ver100( sc::SerialFile &SFile );
	void	LoadSet_Ver101_2( sc::SerialFile &SFile );
    void	LoadSet_Ver103_5( sc::SerialFile &SFile );
    void	LoadSet_Ver106_8( sc::SerialFile &SFile );
	//	-->	데이타 Save/Load
};

typedef DXLIGHT* PDXLIGHT;

typedef std::list<DXLIGHT*>					LIST_DXLIGHT;
typedef LIST_DXLIGHT::iterator				LIST_DXLIGHT_ITER;
typedef LIST_DXLIGHT::const_iterator		LIST_DXLIGHT_CITER;

struct DXLIGHTNODE
{
	union
	{
		struct { float fMaxX, fMaxY, fMaxZ; };
		struct { D3DXVECTOR3 vMax; };
	};

	union
	{
		struct { float fMinX, fMinY, fMinZ; };
		struct { D3DXVECTOR3 vMin; };
	};

	DXLIGHT			*pLight;

	//	Note : 정렬 트리용.
	//
	DXLIGHTNODE		*pLeftChild;
	DXLIGHTNODE		*pRightChild;

	BOOL IsCollisionVolume ( const CLIPVOLUME & cv );

	DXLIGHTNODE () :
		fMaxX(0.0f),
		fMaxY(0.0f),
		fMaxZ(0.0f),
		fMinX(0.0f),
		fMinY(0.0f),
		fMinZ(0.0f),
		pLight(NULL),
		pLeftChild(NULL),
		pRightChild(NULL)
	{
	}

	~DXLIGHTNODE ()
	{
		SAFE_DELETE(pLeftChild);
		SAFE_DELETE(pRightChild);
	}

public:
	//	<--	데이타 Save/Load
	void	SaveSet ( sc::SerialFile &SFile );
    void	LoadSet ( sc::SerialFile &SFile, const LIST_DXLIGHT& listPontLight );
	//	-->	데이타 Save/Load
};

class DxLightMan
{
public:
	typedef std::multimap<float,DXLIGHT*>		CLMAN;
	typedef CLMAN::iterator						CLMAN_ITER;

	typedef std::multimap<float,D3DLIGHT9*>		MANPOINTLIGHT;
	typedef MANPOINTLIGHT::iterator				MANPOINTLIGHT_ITER;

public:
	enum { VERSION = 0x00110, _MAX_LIGHT = 8};

protected:	// SAVE / LOAD 필요.
	BOOL		m_bLighting;
	BOOL		m_bWhenDayPointOff;		// 낮일때 포인트 라이트 끔.
	BOOL		m_bNightAndDay;			// 낮밤이 있다. FALSE 라면 강제로 시간을 멈춘다.
	DWORD		m_dwForceStopTime;		// 강제로 시간을 멈추었을 때의 시간.
										// 0x00109 이하버젼에서 m_dwForceStopTime 개념이 없었다. m_dwForceStopTime 값은 12 로 하도록 하자.
										// 0x00110 이상버젼에서는 Tool 에서 셋팅한 값으로 m_dwForceStopTime 를 사용하지.
	DXLIGHT		m_LtDirectNight;
	DXLIGHT		m_LtDirectNoon;

    float       m_fLM_DDPower;      // LightMap에서만 사용. Direction Light Power 를 설정함.
    D3DXVECTOR3 m_vShadowColor[2];  // 0-낮, 1-밤, LightMap에서만 사용. 그림자 컬러를 설정함.
	
	LIST_DXLIGHT	m_listPontLight;
	DXLIGHTNODE	    *m_pTreeHead;

	float		m_fElapsedTime;

private:
	typedef std::vector<DXLIGHT*>	VECDXLIGHT;
	typedef VECDXLIGHT::iterator	VECDXLIGHT_ITER;

	VECDXLIGHT	m_vecPointLight;	// 참조만 하기 때문에 삭제 포인터를 Delete 할 필요는 없다.

	void ListToVectorLIGHT();

protected:
	DXLIGHT		*m_pLtTest;

	DWORD		m_dwRenderCount;
	DWORD		m_dwBackRenderCount;
	DWORD		m_dwRegistCount;

	BOOL		m_bRendRange;

	DXLIGHT		m_LtDirectNow;
	CLMAN		m_mmapClosedLight;


	float		m_fThunderTimer;
	int			m_nThunder;
	int			m_nThunderFULL;
	float		m_fThunderPOWER;
	D3DLIGHTQ	m_sThunderLight;

	BOOL		m_bDSPName;

protected:
	D3DLIGHTQ	m_sDPLight[ _MAX_LIGHT ];
	DWORD		m_dwDPLight_Count;

	D3DLIGHTQ	m_sSaveDPLight[ _MAX_LIGHT ];
	DWORD		m_dwSaveDPLight_Count;

	DWORD		m_dwDPLight;
	

public:
	BOOL		ISColsedOnePL ( D3DXVECTOR3 vPos );										// 지금 위치에 영향을 주는 동적 라이트가 있는가 ?
	BOOL		ISDynamicLight ()						{ return m_dwDPLight_Count; }	// 동적 라이트가 있는가 ?
	void		SetDPLight ( D3DLIGHTQ sLight );		// 셋팅하면 효과는 다음 프레임에 나타난다.
	D3DLIGHTQ*	GetClosedDPLight ()						{ return &m_sDPLight[m_dwDPLight]; }
	void		Clone2DPLight ();
	void		SetDPLight ( LPDIRECT3DDEVICEQ pd3dDevice );
    DXLIGHT*    GetDirectionNoon() { return &m_LtDirectNoon; }
    DXLIGHT*    GetDirectionNight() { return &m_LtDirectNight; }

public:
	void SetPiecePointIndex( const D3DXVECTOR3& vPos, const float& fLength, VEC_DWORD& vecPointIndex );
    const DXLIGHT* GetDXLIGHT( DWORD dwIndex );
	D3DLIGHTQ* GetPointLight( const DWORD& dwIndex );
	char* GetStrPointLight( const DWORD& dwIndex );

public:
	LPDIRECT3DDEVICEQ m_pd3dDevice;

public:
	void SetDefault ();

	void SetForceStopTime( DWORD dwTime )	{ m_dwForceStopTime = dwTime;}
	DWORD GetForceStopTime()				{ return m_dwForceStopTime; }

	void EnableLighting ( BOOL bEnable )	{ m_bLighting = bEnable; }
	BOOL IsEnableLighting ()				{ return m_bLighting; }

	void EnableNightAndDay ( BOOL bEnable )	{ m_bNightAndDay = bEnable; }
	BOOL IsNightAndDay ()					{ return m_bNightAndDay; }

	void EnableWhenDayPointOff ( BOOL bEnable )	{ m_bWhenDayPointOff = bEnable; }
	BOOL IsWhenDayPointOff ()					{ return m_bWhenDayPointOff; }

	void SetDirectNoonLight ( const DXLIGHT& ltDirectional );
	void SetDirectNightLight ( const DXLIGHT& ltDirectional );

	DXLIGHT* GetDirectLight ();

	DXLIGHT* GetDirectNoonLight ();
	DXLIGHT* GetDirectNightLight ();

	DXLIGHT* GetClosedLight ()
	{
		if ( m_mmapClosedLight.empty() )	return NULL;
		return (*m_mmapClosedLight.begin()).second;
	};

	DXLIGHT* GetClosed2Light ()
	{
		if ( m_mmapClosedLight.size() < 2 )	return NULL;
		
		CLMAN_ITER iter = m_mmapClosedLight.begin();
		++iter;
		return (*iter).second;
	};

	DXLIGHT* GetClosedLight ( int nPoint )
	{
		if ( m_mmapClosedLight.size() < (DWORD)nPoint )	return NULL;
		
		CLMAN_ITER iter = m_mmapClosedLight.begin();
		for ( int i=0; i<(nPoint-1); i++ )
		{
			++iter;
		}
		return (*iter).second;
	};

    const D3DXVECTOR3& GetShadowColorDay()      { return m_vShadowColor[0]; }
    const D3DXVECTOR3& GetShadowColorNight()    { return m_vShadowColor[1]; }
    void SetShadowColorDay( const D3DCOLORVALUE& sColor )   
    { 
        m_vShadowColor[0].x = sColor.r; 
        m_vShadowColor[0].y = sColor.g;
        m_vShadowColor[0].z = sColor.b;
    }
    void SetShadowColorNight( const D3DCOLORVALUE& sColor )
    { 
        m_vShadowColor[1].x = sColor.r; 
        m_vShadowColor[1].y = sColor.g;
        m_vShadowColor[1].z = sColor.b;
    }
    float GetLightMapDirectionDiffusePower()                { return m_fLM_DDPower; }
    void SetLightMapDirectionDiffusePower( float fPower )   { m_fLM_DDPower = fPower; }

	void EnableRangeRend ( BOOL bRend )	{ m_bRendRange = bRend; }
	BOOL IsEnableRangeRend ()			{ return m_bRendRange; }

	void Thunder ();
	float GetThunderPOWER ()	{ return m_fThunderPOWER; }		// 번개 빛의 세기

	// Note : 인터페이스
public:
	void Clone_MouseShift( LPD3DXMATRIX& pMatrix );
	void SetDSPName( BOOL bUse )						{ m_bDSPName = bUse; }

public:
	DXLIGHT*	GetLight ( const char* szLightName );
	void		AddLight ( DXLIGHT* pLight );
	void		DelLight ( const char* szLightName );

	LIST_DXLIGHT&	GetLIST_DXLIGHT()			{ return m_listPontLight; }

	void		TestLight ( DXLIGHT* pLight );
	void		ResetTestLight ();

	DWORD		ComputeClosedPointLight( D3DXVECTOR3* pPos, D3DXVECTOR3* pNor );

    void        SetObjRotate90( const D3DXMATRIX& matRotate );

protected:
	void		ComputePointLight( DXLIGHTNODE *pLightNode, D3DXVECTOR3* pPos, D3DXVECTOR3* pNor, D3DXVECTOR3& vColor );

	// Note : 캐릭터 관련
public:
    void SetCharPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, BOOL bModulate2X );	// 캐릭터를 기준으로 지형과 효과 Light를 셋팅
	void AddDffiuse_PointLight ( D3DXVECTOR3& vPointLight OUT, const D3DXVECTOR3& vPos, const DxLandMan& sLandMan ) const;

private:
	void SetCharStaticPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, BOOL bModulate2X );	// 지형 Light를 중심으로 셋팅
	void SetCharDynamicPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, BOOL bModulate2X );	// 효과 Light를 중심으로 셋팅
	void AddDffiuse_PointLight_Tree ( const DXLIGHTNODE &sLightNode, IN OUT D3DXVECTOR3& vPointLight, const D3DXVECTOR3& vPos, const DxLandMan& sLandMan ) const;

protected:
	void MakeAABBTree ( PDXLIGHT *ppLightArray, DWORD dwArraySize, DXLIGHTNODE* pLightNode,
		const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin );

public:
	void MakeAABBTree ();

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects ();

public:
	void CleanUp ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUnLimitPL, BOOL bUsedMaterialSystem );
	HRESULT Render_Sphere_LightName( LPDIRECT3DDEVICEQ pd3dDevice );

	void TreeRender ( DXLIGHTNODE *pLightNode, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME& cv );
	void Tree_Sphere_LightName ( DXLIGHTNODE *pLightNode, LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME& cv );

	void ReSetLight( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUnLimitPL=FALSE );	// Render을 한 후 나중에 변할 경우
	void LightDisable34567( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	SaveSet ( sc::SerialFile &SFile, DWORD dwSetLandVer );
	void	LoadSet ( sc::SerialFile &SFile, DWORD dwSetLandVer );

public:
	void	Save ( sc::SerialFile &SFile );
	void	Load ( sc::SerialFile &SFile, DWORD dwLandVer, BOOL bSETLAND, BOOL bMaterialSystem=FALSE );

	void	Load_100 ( sc::SerialFile &SFile );
	void	Load_101 ( sc::SerialFile &SFile );
	void	Load_102 ( sc::SerialFile &SFile );
	void	Load_103 ( sc::SerialFile &SFile );
	void	Load_104 ( sc::SerialFile &SFile );	//	데이터 포맷 변경 진행
    void	Load_105 ( sc::SerialFile &SFile );	//	m_listPontLight 로 변경
    void	Load_106 ( sc::SerialFile &SFile );	//	Attenuation 변경
    void	Load_107 ( sc::SerialFile &SFile );	//	Showdow Color 설정
    void	Load_108 ( sc::SerialFile &SFile );	//	LightMap Direction Diffuse Power 설정 가능
    void	Load_109 ( sc::SerialFile &SFile );	//	PointLight Day/Night 각각 켜고 끌수 있게 함.
	void	Load_110 ( sc::SerialFile &SFile );	//	m_dwForceStopTime 기능이 들어감.
	

public:
	void	CloneInstance ( DxLightMan* pLightMan, BOOL bDirectionLightOFF );

public:
	DxLightMan(void);
	virtual ~DxLightMan(void);

public:
	static void SetDefDirect ( const DXLIGHT& ltDirectional );

protected:
	static DxLightMan*	g_pInstanceDefault;
	static DxLightMan*	g_pInstance;

public:
	static DxLightMan* GetInstance ();

	static void SetInstance ( DxLightMan* pLightMan )
	{
		if ( pLightMan==NULL )	g_pInstance = NULL;
		else					g_pInstance = pLightMan;
	}

    static void ReleaseInstance();
};
