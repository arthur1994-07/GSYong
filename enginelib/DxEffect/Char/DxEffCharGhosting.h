#pragma once

#include <list>
#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

struct EFFCHAR_PROPERTY_GHOSTING_100
{
	DWORD			m_dwFlag;
	DWORD			m_dwGhostingNum;		// 고스팅 하는 갯수
	DWORD			m_dwAlphaMax;
	DWORD			m_dwUnit;
	D3DCOLOR		m_dwColor;
};

struct EFFCHAR_PROPERTY_GHOSTING_101	//	데이터 포맷 변경 진행	
{
	DWORD			m_dwFlag;
	DWORD			m_dwGhostingNum;		// 고스팅 하는 갯수
	DWORD			m_dwAlphaMax;
	DWORD			m_dwUnit;
	EMANI_MAINTYPE	m_AniMType;
	EMANI_SUBTYPE	m_AniSType;
	D3DCOLOR		m_dwColor;
};


struct EFFCHAR_PROPERTY_GHOSTING : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	EMANI_MAINTYPE	m_AniMType;
	EMANI_SUBTYPE	m_AniSType;
	D3DCOLOR		m_dwColor;	
	DWORD			m_dwGhostingNum;		// 고스팅 하는 갯수
	DWORD			m_dwUnit;
	DWORD			m_dwAlphaMax;
	

	EFFCHAR_PROPERTY_GHOSTING () :
		m_dwGhostingNum(3),
		m_dwAlphaMax(130),
		m_dwUnit(3),
		m_AniMType(AN_SKILL),
		m_AniSType(AN_SUB_NONE),
		m_dwColor(0x00ffffff)
	{
		m_dwFlag = 0L;
	}

	void Assign( EFFCHAR_PROPERTY_GHOSTING_101& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_AniMType = sOldData.m_AniMType;
		m_AniSType = sOldData.m_AniSType;
		m_dwColor = sOldData.m_dwColor;
		m_dwGhostingNum = sOldData.m_dwGhostingNum;
		m_dwAlphaMax = sOldData.m_dwAlphaMax;
		m_dwUnit = sOldData.m_dwUnit;
	}
};

class DxEffCharGhosting : public DxEffChar
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
			EFFCHAR_PROPERTY_GHOSTING m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			EMANI_MAINTYPE	m_AniMType;
			EMANI_SUBTYPE	m_AniSType;
			D3DCOLOR		m_dwColor;
			DWORD			m_dwGhostingNum;
			DWORD			m_dwUnit;
			DWORD			m_dwAlphaMax;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_GHOSTING*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedBaseSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBaseSB;

protected:
	DxSkinMesh9*		m_pSkinMesh;
	PSMESHCONTAINER		m_pmcMesh;

	DWORD				m_dwPrevKeyTime;


	struct	GHOSTING_INFORMATION
	{
		BOOL		m_Use;
		DWORD		m_UseNum;		// 사용한 횟수
		DWORD		m_dwKeyTime;
		D3DXMATRIX	m_matCurAni;

		GHOSTING_INFORMATION() :
			m_Use(FALSE),
			m_UseNum(0),
			m_dwKeyTime(0)
		{
			D3DXMatrixIdentity ( &m_matCurAni );
		}
	};

	typedef std::list<GHOSTING_INFORMATION>		GHOSTINGLIST;
	typedef GHOSTINGLIST::iterator				GHOSTINGLIST_ITER;

	GHOSTINGLIST								m_listGhosting;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

	virtual DWORD GetStateOrder () const				{ return EMEFFSO_GHOSTING; }

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행

public:
	DxEffCharGhosting(void);
	virtual ~DxEffCharGhosting(void);
};
