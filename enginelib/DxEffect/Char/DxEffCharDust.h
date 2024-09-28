#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

struct EFFCHAR_PROPERTY_DUST_100 //	데이터 포맷 변경 진행	
{
	DWORD			m_dwFlag;
	char			m_szTexture[MAX_PATH];
};


struct EFFCHAR_PROPERTY_DUST : public EFFCHAR_PROPERTY
{
	char			m_szTexture[MAX_PATH];
	DWORD			m_dwFlag;

	EFFCHAR_PROPERTY_DUST ()
	{
		m_dwFlag = 0L;
		StringCchCopy( m_szTexture, MAX_PATH, "Rain.dds" );	//
	}

	void Assign( EFFCHAR_PROPERTY_DUST_100& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		StringCchCopy( m_szTexture, MAX_PATH, sOldData.m_szTexture );

	}
};

class DxEffCharDust : public DxEffChar
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
			EFFCHAR_PROPERTY_DUST m_Property;
		};
		
		struct
		{
			char			m_szTexture[MAX_PATH];
			DWORD			m_dwFlag;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_DUST*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedBaseSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBaseSB;

protected:
	DxSkinMesh9*			m_pSkinMesh;
	PSMESHCONTAINER			m_pmcMesh;

protected:
	TextureResource m_textureRes;
	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DWORD					m_dwMaterials;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh();

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행

public:
	DxEffCharDust(void);
	virtual ~DxEffCharDust(void);
};
