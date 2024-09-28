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
	BOOL			m_bEnablePL_Day;       // ����Ʈ ����Ʈ ���� Ȱ��ȭ
    BOOL			m_bEnablePL_Night;     // ����Ʈ ����Ʈ �㿡 Ȱ��ȭ
	D3DCOLORVALUE	m_BaseDiffuse;
	D3DXMATRIX		m_matWorld;
	
	D3DLIGHTQ		m_Light;

    int             m_nAttenuation0;
    int             m_nAttenuation1;
    int             m_nAttenuation2;

	BOOL			m_bEditCheck;

	float			m_fOverLighting;		// ������ ��� ���ٰ��ΰ�~?

	//	Note : �ܹ��� ����Ʈ.
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

		// pNext �� ���� ���꿡�� ���ܵȴ�.
	}

	void SetRange ( float fRange );
	virtual void SerialData ();
    BOOL IsEnablePL( BOOL bWhenDayPointOff ) const;

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	//	<--	����Ÿ Save/Load
	void	SaveSet ( sc::SerialFile &SFile );
	void	LoadSet ( sc::SerialFile &SFile );
	void	LoadSet_Ver100( sc::SerialFile &SFile );
	void	LoadSet_Ver101_2( sc::SerialFile &SFile );
    void	LoadSet_Ver103_5( sc::SerialFile &SFile );
    void	LoadSet_Ver106_8( sc::SerialFile &SFile );
	//	-->	����Ÿ Save/Load
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

	//	Note : ���� Ʈ����.
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
	//	<--	����Ÿ Save/Load
	void	SaveSet ( sc::SerialFile &SFile );
    void	LoadSet ( sc::SerialFile &SFile, const LIST_DXLIGHT& listPontLight );
	//	-->	����Ÿ Save/Load
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

protected:	// SAVE / LOAD �ʿ�.
	BOOL		m_bLighting;
	BOOL		m_bWhenDayPointOff;		// ���϶� ����Ʈ ����Ʈ ��.
	BOOL		m_bNightAndDay;			// ������ �ִ�. FALSE ��� ������ �ð��� �����.
	DWORD		m_dwForceStopTime;		// ������ �ð��� ���߾��� ���� �ð�.
										// 0x00109 ���Ϲ������� m_dwForceStopTime ������ ������. m_dwForceStopTime ���� 12 �� �ϵ��� ����.
										// 0x00110 �̻���������� Tool ���� ������ ������ m_dwForceStopTime �� �������.
	DXLIGHT		m_LtDirectNight;
	DXLIGHT		m_LtDirectNoon;

    float       m_fLM_DDPower;      // LightMap������ ���. Direction Light Power �� ������.
    D3DXVECTOR3 m_vShadowColor[2];  // 0-��, 1-��, LightMap������ ���. �׸��� �÷��� ������.
	
	LIST_DXLIGHT	m_listPontLight;
	DXLIGHTNODE	    *m_pTreeHead;

	float		m_fElapsedTime;

private:
	typedef std::vector<DXLIGHT*>	VECDXLIGHT;
	typedef VECDXLIGHT::iterator	VECDXLIGHT_ITER;

	VECDXLIGHT	m_vecPointLight;	// ������ �ϱ� ������ ���� �����͸� Delete �� �ʿ�� ����.

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
	BOOL		ISColsedOnePL ( D3DXVECTOR3 vPos );										// ���� ��ġ�� ������ �ִ� ���� ����Ʈ�� �ִ°� ?
	BOOL		ISDynamicLight ()						{ return m_dwDPLight_Count; }	// ���� ����Ʈ�� �ִ°� ?
	void		SetDPLight ( D3DLIGHTQ sLight );		// �����ϸ� ȿ���� ���� �����ӿ� ��Ÿ����.
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
	float GetThunderPOWER ()	{ return m_fThunderPOWER; }		// ���� ���� ����

	// Note : �������̽�
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

	// Note : ĳ���� ����
public:
    void SetCharPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, BOOL bModulate2X );	// ĳ���͸� �������� ������ ȿ�� Light�� ����
	void AddDffiuse_PointLight ( D3DXVECTOR3& vPointLight OUT, const D3DXVECTOR3& vPos, const DxLandMan& sLandMan ) const;

private:
	void SetCharStaticPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, BOOL bModulate2X );	// ���� Light�� �߽����� ����
	void SetCharDynamicPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, BOOL bModulate2X );	// ȿ�� Light�� �߽����� ����
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

	void ReSetLight( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUnLimitPL=FALSE );	// Render�� �� �� ���߿� ���� ���
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
	void	Load_104 ( sc::SerialFile &SFile );	//	������ ���� ���� ����
    void	Load_105 ( sc::SerialFile &SFile );	//	m_listPontLight �� ����
    void	Load_106 ( sc::SerialFile &SFile );	//	Attenuation ����
    void	Load_107 ( sc::SerialFile &SFile );	//	Showdow Color ����
    void	Load_108 ( sc::SerialFile &SFile );	//	LightMap Direction Diffuse Power ���� ����
    void	Load_109 ( sc::SerialFile &SFile );	//	PointLight Day/Night ���� �Ѱ� ���� �ְ� ��.
	void	Load_110 ( sc::SerialFile &SFile );	//	m_dwForceStopTime ����� ��.
	

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
