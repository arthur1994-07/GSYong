#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxMeshs/SkinMesh/DxMaterialHLSL.h"

struct EFFCHAR_PROPERTY_NORMAL_100_101
{

	char m_szNormalMap[MAX_PATH];
	char m_szSpecularTex[MAX_PATH];
	char m_szGlossinessTex[MAX_PATH];

	bool		m_bUseSpecularTex;
	bool		m_bUseGlossinessTex;

	D3DXCOLOR	m_cAmbient;
	D3DXCOLOR	m_cSpecular;
	D3DXCOLOR	m_cDiffuse;

	float		m_fSpecularPower;

};

struct EFFCHAR_PROPERTY_NORMAL : public EFFCHAR_PROPERTY
{

	char m_szNormalMap[MAX_PATH];
	char m_szSpecularTex[MAX_PATH];
	char m_szGlossinessTex[MAX_PATH];
	char m_szGlowTex[MAX_PATH];

	bool		m_bUseSpecularTex;
	bool		m_bUseGlossinessTex;
	bool		m_bUseGlowTex;

	D3DXCOLOR	m_cAmbient;
	D3DXCOLOR	m_cSpecular;
	D3DXCOLOR	m_cDiffuse;

	float		m_fSpecularPower;

	float		m_fGlowBright;
	float		m_fGlowAlpha;
	float		m_fGlowAniSpeed;

	void Assign( EFFCHAR_PROPERTY_NORMAL_100_101& sOldData )
	{
		StringCchCopy( m_szNormalMap, MAX_PATH, sOldData.m_szNormalMap );
		StringCchCopy( m_szSpecularTex, MAX_PATH, sOldData.m_szSpecularTex );
		StringCchCopy( m_szGlossinessTex, MAX_PATH, sOldData.m_szGlossinessTex );

		m_bUseSpecularTex	= sOldData.m_bUseSpecularTex;
		m_bUseGlossinessTex	= sOldData.m_bUseGlossinessTex;

		m_cAmbient	= sOldData.m_cAmbient;
		m_cSpecular	= sOldData.m_cSpecular;
		m_cDiffuse	= sOldData.m_cDiffuse;

		m_fSpecularPower	= sOldData.m_fSpecularPower;

	}
	


	EFFCHAR_PROPERTY_NORMAL() :
		m_cAmbient(0.5f,0.5f,0.5f,1.0f),
		m_cSpecular(0.5f,0.5f,0.5f,1.0f),
		m_cDiffuse(0.5f,0.5f,0.5f,1.0f),
		m_fSpecularPower(1.0f),
		m_bUseSpecularTex(FALSE),
		m_bUseGlossinessTex(FALSE),	
		m_bUseGlowTex(FALSE),
		m_fGlowBright(1.0f),
		m_fGlowAlpha(1.0f),
		m_fGlowAniSpeed(1.0f)
	{
		ZeroMemory( m_szNormalMap, MAX_PATH );
		ZeroMemory( m_szSpecularTex, MAX_PATH );
		ZeroMemory( m_szGlossinessTex, MAX_PATH );
		ZeroMemory( m_szGlowTex, MAX_PATH );
	}
};

class DxEffCharNormal : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_NORMALMAP; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_NORMAL m_Property;
		};
		
		struct
		{
			char m_szNormalMap[MAX_PATH];
			char m_szSpecularTex[MAX_PATH];
			char m_szGlossinessTex[MAX_PATH];
			char m_szGlowTex[MAX_PATH];

			bool		m_bUseSpecularTex;
			bool		m_bUseGlossinessTex;
			bool		m_bUseGlowTex;

			D3DXCOLOR	m_cAmbient;
			D3DXCOLOR	m_cSpecular;
			D3DXCOLOR	m_cDiffuse;

			float		m_fSpecularPower;

			float		m_fGlowBright;
			float		m_fGlowAlpha;
			float		m_fGlowAniSpeed;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_NORMAL*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}


protected:
	DxSkinMesh9*		m_pSkinMesh;
	PSMESHCONTAINER		m_pmcMesh;

    TextureResource m_textureResNormal;
    TextureResource m_textureResSpecular;
    TextureResource m_textureResGlossiness;
    TextureResource m_textureResGlow;
	LPDIRECT3DVERTEXDECLARATION9 m_pNormalDCRT;
	ID3DXEffect*				 m_pEffect;
	

	float						 m_fAniGlowAlpha;
	float						 m_fAniValue;
	D3DXHANDLE					 m_hTechnique;
	D3DXHANDLE					 m_hvLightDir;
	D3DXHANDLE					 m_hvEyePos;
	D3DXHANDLE					 m_htNormalMap;
	D3DXHANDLE					 m_htSpecularTex;
	D3DXHANDLE					 m_htGlossinessTex;
	D3DXHANDLE					 m_htGlowTex;

	D3DXHANDLE					 m_htWorld;
	D3DXHANDLE					 m_htView;
	D3DXHANDLE					 m_htProj;

	D3DXHANDLE					 m_htAmbient;
	D3DXHANDLE					 m_htDiffuse;
	D3DXHANDLE					 m_htSpecular;
    D3DXHANDLE					 m_htbUseDirectionLight;

	D3DXHANDLE					 m_htGlowAlpha;

	D3DXVECTOR3					 m_vAddLight;
	
	BOOL						 m_bPossibleEffect;



public:

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//  Glow 텍스쳐 추가

public:
	DxEffCharNormal(void);
	virtual ~DxEffCharNormal(void);
};
