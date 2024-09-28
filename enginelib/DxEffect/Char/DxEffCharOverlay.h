#pragma once
#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"
#include "../../DxMeshs/SkinMesh/DxMaterialHLSL.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_NORMAL.h"

struct EFFCHAR_PROPERTY_OVERLAY_TEXTURE;

struct EFFCHAR_PROPERTY_OVERLAY_100
{

	char		m_szOverlayTex[MAX_PATH];
	D3DXCOLOR	m_cColor1;
	D3DXCOLOR	m_cColor2;

	D3DXCOLOR	m_cAmbient;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cSpecular;

	float		m_fSpecularPower;
	float		m_fSpecularLevel;

	DWORD		m_dwSelectMaterial;

	char		m_szGlowTex[MAX_PATH];
	BOOL		m_bUseGlowTex;
	float		m_fGlowBright;
	float		m_fGlowAlpha;
	float		m_fGlowAniSpeed;

};

struct EFFCHAR_PROPERTY_OVERLAY_101
{
	char		m_szOverlayTex[MAX_PATH];
	D3DXCOLOR	m_cColor1;
	D3DXCOLOR	m_cColor2;

	D3DXCOLOR	m_cAmbient;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cSpecular;

	float		m_fSpecularPower;
	float		m_fSpecularLevel;

	DWORD		m_dwSelectMaterial;

	char		m_szGlowTex[MAX_PATH];
	BOOL		m_bUseGlowTex;
	float		m_fGlowBright;
	float		m_fGlowAlpha;
	float		m_fGlowAniSpeed;

	char		m_szReflectTex[MAX_PATH];
	char		m_szReflectAlphaTex[MAX_PATH];
	BOOL		m_bUseReflect;
	float		m_fReflectActivity;
	BOOL		m_bUseAlphaReflect;
};

const int MAX_MATERIAL_OVERLAY = 6; // by luxes.

struct EFFCHAR_PROPERTY_OVERLAY_102
{
	char		m_szOverlayTex[MAX_PATH];

	BOOL		m_bUseOverlay[ MAX_MATERIAL_OVERLAY ]; // 현재 선택된 material에서 overlay 사용하기. by luxes.

	BOOL		m_bUseSpecular; // by luxes.
	D3DXCOLOR	m_cColor1;
	D3DXCOLOR	m_cColor2;

	D3DXCOLOR	m_cAmbient;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cSpecular;

	float		m_fSpecularPower;
	float		m_fSpecularLevel;

	float		m_fAlphaRef; // by luxes.

	DWORD		m_dwSelectMaterial;

	char		m_szGlowTex[MAX_PATH];
	BOOL		m_bUseGlowTex;
	float		m_fGlowBright;
	float		m_fGlowAlpha;
	float		m_fGlowAniSpeed;

	char		m_szReflectTex[MAX_PATH];
	char		m_szReflectAlphaTex[MAX_PATH];
	BOOL		m_bUseReflect;
	float		m_fReflectActivity;
	BOOL		m_bUseAlphaReflect;
};

struct EFFCHAR_PROPERTY_OVERLAY : public EFFCHAR_PROPERTY
{
	char		m_szOverlayTex[MAX_PATH];

	BOOL		m_bUseOverlay[ MAX_MATERIAL_OVERLAY ]; // 현재 선택된 material에서 overlay 사용하기. by luxes.

	BOOL		m_bUseSpecular; // by luxes.
	D3DXCOLOR	m_cColor1;
	D3DXCOLOR	m_cColor2;

	D3DXCOLOR	m_cAmbient;
	D3DXCOLOR	m_cDiffuse;
	D3DXCOLOR	m_cSpecular;

	float		m_fSpecularPower;
	float		m_fSpecularLevel;
	
	float		m_fAlphaRef; // by luxes.

	DWORD		m_dwSelectMaterial;
	int			m_nSelectOverlay; // 오버레이 적용할 매테리얼 넘버. by luxes.

	char		m_szGlowTex[MAX_PATH];
	BOOL		m_bUseGlowTex;
	float		m_fGlowBright;
	float		m_fGlowAlpha;
	float		m_fGlowAniSpeed;

	char		m_szReflectTex[MAX_PATH];
	char		m_szReflectAlphaTex[MAX_PATH];
	BOOL		m_bUseReflect;
	float		m_fReflectActivity;
	BOOL		m_bUseAlphaReflect;

	EFFCHAR_PROPERTY_OVERLAY () :
		m_bUseSpecular(FALSE),
		m_cColor1(D3DCOLOR_ARGB(255,128,128,128)),
		m_cColor2(D3DCOLOR_ARGB(255,128,128,128)),
		m_cDiffuse(D3DCOLOR_ARGB(0,60,60,60)),
		m_cSpecular(D3DCOLOR_ARGB(0,70,70,70)),
		m_cAmbient(D3DCOLOR_ARGB(0,200,200,200)),
		m_fSpecularPower(98.0f),
		m_fSpecularLevel(2.0f),
		m_fAlphaRef(128.0f),
		m_dwSelectMaterial(0),
		m_nSelectOverlay(0),
		m_bUseGlowTex(FALSE),
		m_fGlowBright(1.0f),
		m_fGlowAlpha(1.0f),
		m_fGlowAniSpeed(1.0f),
		m_bUseReflect(false),
		m_bUseAlphaReflect(false),
		m_fReflectActivity(0.0f)
	{
		ZeroMemory( m_szReflectAlphaTex, MAX_PATH );
		ZeroMemory( m_szReflectTex, MAX_PATH );
		ZeroMemory( m_szOverlayTex, MAX_PATH );
		ZeroMemory( m_szGlowTex, MAX_PATH );

		for ( int i= 0; i < MAX_MATERIAL_OVERLAY; i++ )
		{
			m_bUseOverlay[ i ] = TRUE;
		}
	}

	void Assign( EFFCHAR_PROPERTY_OVERLAY_100& sOldData )
	{
		StringCchCopy( m_szOverlayTex, MAX_PATH, sOldData.m_szOverlayTex );
		StringCchCopy( m_szGlowTex, MAX_PATH, sOldData.m_szGlowTex );

		m_cColor1	= sOldData.m_cColor1;
		m_cColor2	= sOldData.m_cColor2;

		m_cAmbient	= sOldData.m_cAmbient;
		m_cDiffuse	= sOldData.m_cDiffuse;
		m_cSpecular = sOldData.m_cSpecular;

		m_fSpecularPower = sOldData.m_fSpecularPower;
		m_fSpecularLevel = sOldData.m_fSpecularLevel;

		m_dwSelectMaterial = sOldData.m_dwSelectMaterial;;

		m_bUseGlowTex	   = sOldData.m_bUseGlowTex;
		m_fGlowBright	   = sOldData.m_fGlowBright;
		m_fGlowAlpha	   = sOldData.m_fGlowAlpha;
		m_fGlowAniSpeed	   = sOldData.m_fGlowAniSpeed;


	}

	void Assign( EFFCHAR_PROPERTY_OVERLAY_101& sOldData )
	{
		StringCchCopy( m_szOverlayTex, MAX_PATH, sOldData.m_szOverlayTex );
		m_cColor1	= sOldData.m_cColor1;
		m_cColor2	= sOldData.m_cColor2;

		m_cAmbient	= sOldData.m_cAmbient;
		m_cDiffuse	= sOldData.m_cDiffuse;
		m_cSpecular = sOldData.m_cSpecular;

		m_fSpecularPower = sOldData.m_fSpecularPower;
		m_fSpecularLevel = sOldData.m_fSpecularLevel;

		m_dwSelectMaterial = sOldData.m_dwSelectMaterial;

		StringCchCopy( m_szGlowTex, MAX_PATH, sOldData.m_szGlowTex );
		m_bUseGlowTex	   = sOldData.m_bUseGlowTex;
		m_fGlowBright	   = sOldData.m_fGlowBright;
		m_fGlowAlpha	   = sOldData.m_fGlowAlpha;
		m_fGlowAniSpeed	   = sOldData.m_fGlowAniSpeed;

		StringCchCopy( m_szReflectTex, MAX_PATH, sOldData.m_szReflectTex );
		StringCchCopy( m_szReflectAlphaTex, MAX_PATH, sOldData.m_szReflectAlphaTex );
		m_bUseReflect = sOldData.m_bUseReflect;
		m_fReflectActivity = sOldData.m_fReflectActivity;
		m_bUseAlphaReflect = sOldData.m_bUseAlphaReflect;
	}

	void Assign( EFFCHAR_PROPERTY_OVERLAY_102& sOldData )
	{
		StringCchCopy( m_szOverlayTex, MAX_PATH, sOldData.m_szOverlayTex );
		m_cColor1	= sOldData.m_cColor1;
		m_cColor2	= sOldData.m_cColor2;

		m_cAmbient	= sOldData.m_cAmbient;
		m_cDiffuse	= sOldData.m_cDiffuse;
		m_cSpecular = sOldData.m_cSpecular;

		m_fSpecularPower = sOldData.m_fSpecularPower;
		m_fSpecularLevel = sOldData.m_fSpecularLevel;

		m_dwSelectMaterial = sOldData.m_dwSelectMaterial;

		StringCchCopy( m_szGlowTex, MAX_PATH, sOldData.m_szGlowTex );
		m_bUseGlowTex	   = sOldData.m_bUseGlowTex;
		m_fGlowBright	   = sOldData.m_fGlowBright;
		m_fGlowAlpha	   = sOldData.m_fGlowAlpha;
		m_fGlowAniSpeed	   = sOldData.m_fGlowAniSpeed;

		StringCchCopy( m_szReflectTex, MAX_PATH, sOldData.m_szReflectTex );
		StringCchCopy( m_szReflectAlphaTex, MAX_PATH, sOldData.m_szReflectAlphaTex );
		m_bUseReflect = sOldData.m_bUseReflect;
		m_fReflectActivity = sOldData.m_fReflectActivity;
		m_bUseAlphaReflect = sOldData.m_bUseAlphaReflect;
	}

	void Assign( EFFCHAR_PROPERTY_OVERLAY_TEXTURE& sOldData, int dwSelect )
	{
		StringCchCopy( m_szOverlayTex, MAX_PATH, sOldData.m_szOverlayTex );

		m_bUseOverlay[ dwSelect ] = sOldData.m_bOverlay; // 현재 선택된 material에서 overlay 사용하기. by luxes.

		m_bUseSpecular = sOldData.m_bUseSpecular; // by luxes.

		m_cColor1 = D3DCOLOR_COLORVALUE( sOldData.m_vColor1.x, sOldData.m_vColor1.y
			, sOldData.m_vColor1.z, sOldData.m_vColor1.w );
		m_cColor2 =  D3DCOLOR_COLORVALUE( sOldData.m_vColor2.x, sOldData.m_vColor2.y
			, sOldData.m_vColor2.z, sOldData.m_vColor2.w );

		m_cAmbient = D3DCOLOR_COLORVALUE( sOldData.m_vAmbient.x, sOldData.m_vAmbient.y
			, sOldData.m_vAmbient.z, sOldData.m_vAmbient.w );
		m_cDiffuse = D3DCOLOR_COLORVALUE( sOldData.m_vDiffuse.x, sOldData.m_vDiffuse.y
			, sOldData.m_vDiffuse.z, sOldData.m_vDiffuse.w );
		m_cSpecular = D3DCOLOR_COLORVALUE( sOldData.m_vSpecular.x, sOldData.m_vSpecular.y
			, sOldData.m_vSpecular.z, sOldData.m_vSpecular.w );

		m_fSpecularPower = sOldData.m_fSpecularPower;
		m_fSpecularLevel = sOldData.m_fSpecularLevel;

		m_fAlphaRef = sOldData.m_fAlphaRef; // by luxes.

		m_dwSelectMaterial = dwSelect;

		StringCchCopy( m_szGlowTex, MAX_PATH, sOldData.m_szGlowTex );
		m_bUseGlowTex = sOldData.m_bUseGlowTex;
		m_fGlowBright = sOldData.m_fGlowBright;
		m_fGlowAlpha = sOldData.m_fGlowAlpha;
		m_fGlowAniSpeed = m_fGlowAniSpeed;

		StringCchCopy( m_szReflectTex, MAX_PATH, sOldData.m_szReflectTex );
		StringCchCopy( m_szReflectAlphaTex, MAX_PATH, sOldData.m_szReflectAlphaTex );
		m_bUseReflect = sOldData.m_bUseReflectTex;
		m_fReflectActivity = sOldData.m_fReflectActivity;
		m_bUseAlphaReflect = sOldData.m_bUseAlphaReflectTex;
	}
};

class DxEffCharOverlay : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_OVERLAY; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_OVERLAY m_Property;
		};

		struct
		{
			char		m_szOverlayTex[MAX_PATH];

			BOOL		m_bUseOverlay[ MAX_MATERIAL_OVERLAY]; // 현재 선택된 material에서 overlay 사용하기. by luxes.

			BOOL		m_bUseSpecular; // by luxes.
			D3DXCOLOR	m_cColor1;
			D3DXCOLOR	m_cColor2;

			D3DXCOLOR	m_cAmbient;
			D3DXCOLOR	m_cDiffuse;
			D3DXCOLOR	m_cSpecular;

			float		m_fSpecularPower;
			float		m_fSpecularLevel;

			float		m_fAlphaRef; // by luxes.

			DWORD		m_dwSelectMaterial;
			int			m_nSelectOverlay;

			char		m_szGlowTex[MAX_PATH];
			BOOL		m_bUseGlowTex;
			float		m_fGlowBright;
			float		m_fGlowAlpha;
			float		m_fGlowAniSpeed;

			char		m_szReflectTex[MAX_PATH];
			char		m_szReflectAlphaTex[MAX_PATH];
			BOOL		m_bUseReflect;
			float		m_fReflectActivity;
			BOOL		m_bUseAlphaReflect;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_OVERLAY*)pProperty);
	}

	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
    static ID3DXEffect*          s_pEffect;

    TextureResource m_textureResOverlay;
    TextureResource m_textureResGlow;
    TextureResource m_textureResReflect;
    TextureResource m_textureResAlphaReflect;
	DxSkinMesh9*				 m_pSkinMesh;
	PSMESHCONTAINER				 m_pmcMesh;
	BOOL						 m_bPossibleEffect;
	LPDIRECT3DVERTEXDECLARATION9 m_pDecl;// 정점선언

	EFFCHAR_PROPERTY_OVERLAY_TEXTURE	m_PropertyOverlay[ MAX_MATERIAL_OVERLAY ];

	D3DXVECTOR3					 m_vAddLight;

	D3DXHANDLE					 m_hTechnique;
	D3DXHANDLE					 m_hmWVP;
	D3DXHANDLE					 m_htMeshTex;
	D3DXHANDLE					 m_hvLightDir;
	D3DXHANDLE					 m_htColorTex;

	D3DXHANDLE					 m_htWorld;
	D3DXHANDLE					 m_htProj;
	D3DXHANDLE					 m_htView;

	D3DXHANDLE					 m_hbUseDirectionLight;

	D3DXHANDLE					 m_hfSpecularLevel;
	D3DXHANDLE					 m_hfSpecularPower;
	D3DXHANDLE					 m_hfAlphaRef; // by luxes.
	D3DXHANDLE					 m_hvInputColor1;
	D3DXHANDLE					 m_hvInputColor2;
	D3DXHANDLE					 m_hvAmbient;
	D3DXHANDLE					 m_hvDiffuse;
	D3DXHANDLE					 m_hvSpecular;

	D3DXHANDLE					 m_htAlphaReflectTex;
	D3DXHANDLE					 m_htGlowTex;
	D3DXHANDLE					 m_htGlowAlpha;

	D3DXHANDLE					 m_htReflectTex;
	D3DXHANDLE					 m_hvEyePos;

	D3DXHANDLE					 m_hvWorldInv;
	D3DXHANDLE					 m_hvWorldInvTrans;
	D3DXHANDLE					 m_hvViewInv;

	float						 m_fAniGlowAlpha;
	float						 m_fAniValue;

	BOOL						m_bNewMaterialMode;

public:
    static HRESULT StaticInitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT StaticRestoreDeviceObjects();
	static HRESULT StaticInvalidateDeviceObjects();
	static HRESULT StaticDeleteDeviceObjects();

	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum );

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( sc::BaseStream &SFile );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	void	PrepareOverlayValues( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwSelectMaterial );
	EFFCHAR_PROPERTY_OVERLAY_TEXTURE GetPropertyOverlay( int nIndex );
	void	OldDataSync( EFFCHAR_PROPERTY_OVERLAY_TEXTURE& val1, EFFCHAR_PROPERTY_OVERLAY& val2 ); // by luxes.
	void	SetOverlayProperty( EFFCHAR_PROPERTY_OVERLAY_TEXTURE* PropertyOverlay, int nIndex ); // by luxes.
	void	SetOverlayColor( D3DXCOLOR hairColor, D3DXCOLOR subColor ); // by luxes.

	// Color 를 MaterialSystem 에서 사용하기 위한 작업.
	void LoadOverlayMaskTexture( int nMaterial, BOOL bTexThread=TRUE );
	LPDIRECT3DTEXTUREQ GetOverlayMaskTexture( int nMaterial );
	void GetOverlayColor_1_2( int nMaterial, const D3DXVECTOR4** ppColor1, const D3DXVECTOR4** ppColor2 );

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:

	void ExportEffTexture( LPDIRECT3DDEVICEQ pd3dDevice, 
							DWORD dwMaterialID, 
							LPDIRECT3DTEXTUREQ* ppLevelTex, 
							LPDIRECT3DTEXTUREQ* ppCubeTex,
							bool bTexThread,
							const TCHAR* pTextureName_NoHave ) OVERRIDE;

	// Effect Texture 들이 다 로딩이 되었는가~?
	BOOL IsEnableEffTexture( BOOL bTexThread ) OVERRIDE;

	// PrecalculateForMaterialSystem의 2번 중 값만 미리 백업해 놓는거라면, 이곳에서 렌더가 된다.
	virtual void PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
												EffectMaterialData* pEffMaterialData, 
												const D3DXMATRIX* pMatrix, 
												const float fScale );

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh();

public:
	static DxEffCharOverlay& GetInstance();

public:
	DxEffCharOverlay(void);
	virtual ~DxEffCharOverlay(void);
};
