#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

#define USE_OBLIQUE		0x0001	// 빗금 렌더링 사용함.
#define TOON_TEX_SIZE	32


struct EFFCHAR_PROPERTY_TOON_100	//	데이터 포맷 변경 진행
{
	DWORD			m_dwFlag;
	float			m_fSize;
	float			m_fSizeADD;			//  사이즈 추가 < 랜덤 수치 >
	char			m_szToonTex[TOON_TEX_SIZE];
	char			m_szObliqueTex[TOON_TEX_SIZE];	// 사선 
};

struct EFFCHAR_PROPERTY_TOON : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	char			m_szToonTex[TOON_TEX_SIZE];
	char			m_szObliqueTex[TOON_TEX_SIZE];	// 사선 
	float			m_fSize;
	float			m_fSizeADD;			//  사이즈 추가 < 랜덤 수치 >

	EFFCHAR_PROPERTY_TOON () :
		m_dwFlag(0L),
		m_fSize(1.f),
		m_fSizeADD(1.f)
	{
		StringCchCopy( m_szToonTex, TOON_TEX_SIZE, "Toon.dds" );
		StringCchCopy( m_szObliqueTex, TOON_TEX_SIZE, "Oblique.dds" );
	}

	void Assign ( EFFCHAR_PROPERTY_TOON_100& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		StringCchCopy( m_szToonTex, TOON_TEX_SIZE, sOldData.m_szToonTex );
		StringCchCopy( m_szObliqueTex, TOON_TEX_SIZE, sOldData.m_szObliqueTex );
		m_fSize = sOldData.m_fSize;
		m_fSizeADD = sOldData.m_fSizeADD;
	}
};

class DxEffCharToon : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_TOON; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_TOON m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			char			m_szToonTex[TOON_TEX_SIZE];
			char			m_szObliqueTex[TOON_TEX_SIZE];	// 사선 
			float			m_fSize;
			float			m_fSizeADD;			//  사이즈 추가 < 랜덤 수치 >
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_TOON*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static DWORD		m_dwToonVS_G;		// 일반 Mesh
	static DWORD		m_dwToonVSA_G[];	// 일반 Mesh
	static DWORD		m_dwToonVS[4];
	static DWORD		m_dwToonVSA_1[];
	static DWORD		m_dwToonVSA_2[];
	static DWORD		m_dwToonVSA_3[];
	static DWORD		m_dwToonVSA_4[];

	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedEdgeSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawEdgeSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedObliqueSB_1;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawObliqueSB_1;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedObliqueSB_2;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawObliqueSB_2;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedObliqueSB_3;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawObliqueSB_3;
	
    TextureResource m_textureRes;
    TextureResource m_textureResOblique;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

private:
	float		m_fObliqueTEX;

public:
	static HRESULT StaticCreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT StaticResetDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT StaticLostDevice();
	static void StaticDestroyDevice();

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );

	virtual DWORD GetStateOrder () const				{ return EMEFFSO_TOON; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA );
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice );

private:
	void RenderSHADOW( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE *pmtrlPiece );
	void RenderOBLIQUE( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE *pmtrlPiece );
	void RenderBASE( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE *pmtrlPiece );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행

public:
	DxEffCharToon(void);
	virtual ~DxEffCharToon(void);
};
