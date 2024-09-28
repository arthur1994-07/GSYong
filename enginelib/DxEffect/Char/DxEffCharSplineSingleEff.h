#pragma once

#include "DxEffChar.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxTools/Math/NonuniformSpline.h"

struct EFF_PROPGROUP;
class DxEffSingleGroup;
class DxBoneTransMaintain;

struct EFFCHAR_PROPERTY_SPLINE_SINGLE_EFF : public EFFCHAR_PROPERTY
{
	float			m_fScale;       // Effect 의 Scale
	float			m_fSpeed;       // Spline 을 이동하는 스피드
    DWORD           m_dwEffNumber;  // 사용되는 Effect 갯수

	EFFCHAR_PROPERTY_SPLINE_SINGLE_EFF()
        : m_fScale(1.f)
		, m_fSpeed(1.f)
        , m_dwEffNumber(3)
	{
	}
};

class DxEffCharSplineSingleEff : public DxEffChar
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
			EFFCHAR_PROPERTY_SPLINE_SINGLE_EFF m_Property;
		};

		struct
		{
	        float			m_fScale;       // Effect 의 Scale
	        float			m_fSpeed;       // Spline 을 이동하는 스피드
            DWORD           m_dwEffNumber;  // 사용되는 Effect 갯수
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_SPLINE_SINGLE_EFF*)pProperty);
	}

	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
    TSTRING             m_strSingleEffect;      // Save, Load
    TSTRING             m_strBoneName;          // Save, Load
    VEC_VECTOR3         m_vecSplinePoint;       // Save, Load

protected:
	DxSkinMesh9*		m_pSkinMesh;
    SPLINE::TNS		    m_sSplinePoint;			// Round Nonuniform Spline

private:
	// m_szBoneName 정보에 의해 바뀌는 값들.
	const DxBoneTransMaintain*		m_rBoneTranMaintain;	//	트랜스 매트릭스.
	int								m_nBoneIndex;

protected:
	DxEffSinglePropGroupResource	m_sPropGroupRes;	//	속성값.

    struct SINGLEEFFDATA
    {
        SPDXEFFSINGLEGROUP  m_spSingleEff;
        float				m_fPlayTime;
    };
    typedef std::vector<SINGLEEFFDATA>      VECSPEFFSINGLEEFFDATA;
    typedef VECSPEFFSINGLEEFFDATA::iterator VECSPEFFSINGLEEFFDATA_ITER;
    VECSPEFFSINGLEEFFDATA   m_vecSingleEffData;

	float				m_fTime;

private:
    float           m_fSplineFullLength;    // 총 거리
    float           m_fSplineStartRate;   // Effect가 시작되어야 하는 비율
    float           m_fSplineEndRate;     // Effect가 이동 마지막 지점 비율. 시작위치로 변경하는 시점.

	D3DXMATRIX		m_matRotateCombine;

    // Edit 에서 사용됨.
public:
    void InsertSplinePoint( int nIndex );
    BOOL DeleteSplinePoint( DWORD dwIndex );
    DWORD UpSplinePoint( DWORD dwIndex );
    DWORD DownSplinePoint( DWORD dwIndex );
    void MakeRNS();

private:
	void	OnlyRenderEff( LPDIRECT3DDEVICEQ pd3dDevice );

private:
    void RenderSplineLine( LPDIRECT3DDEVICEQ pd3dDevice );
    float GetSpline2PointLength( const D3DXVECTOR3& vPos, D3DXVECTOR3& vPrevPos );

	// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxEffSingleGroup가 로딩안되었으면 로딩을 시도한다.
	void CreateDxEffSingleGroup();

	// DxEffSingleGroup 을 정리한다.
	void DeleteDxEffSingleGroup();

public:
    friend class CEffSplineSingleEffPage;

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix, const float fScale );

	virtual DWORD GetStateOrder () const					{ return EMEFFSO_SINGLE_EFF; }

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:
	virtual void PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix ) OVERRIDE;

	virtual void RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
											const D3DXMATRIX* pMatrix, 
											const float fScale );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );
	//virtual void SetDirection ( D3DXVECTOR3 *pDir, float fTarDir )	{ m_vDirection = *pDir; m_fTarDirection = fTarDir; }

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

private:
	HRESULT LoadFile_0100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffCharSplineSingleEff(void);
	virtual ~DxEffCharSplineSingleEff(void);
};

