#pragma once

#include "DxEffChar.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxMethods.h"

#define		ISTEXTURE1		0x0001
#define		ISTEXTURE2		0x0002
#define		ISTEXTURE3		0x0004
#define		ISTEXTURE4		0x0008
#define		USETEXTURE1		0x0010
#define		USETEXTURE2		0x0020
#define		USETEXTURE3		0x0040
#define		USETEXTURE4		0x0080
#define		USETEXMAP		0x0100

#define		USE_ALL_TEX		0x1000
#define		USE_SELECT_TEX	0x2000


struct EFFCHAR_PROPERTY_MULTITEX_100_1
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	int				m_nTexNum;

	D3DXCOLOR		m_cDiffuse1;
	D3DXCOLOR		m_cDiffuse2;

	D3DXVECTOR2		m_vTex01;
	D3DXVECTOR2		m_vTex02;

	char			m_szTex1[MAX_PATH];
	char			m_szTex2[MAX_PATH];

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_MULTITEX_100_1()
	{
		memset( m_szTex1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTex2, 0, sizeof(char)*MAX_PATH );

		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_MULTITEX_102
{
	DWORD			m_dwFlag;

	int				m_nBlend;
	int				m_nTexNum;

	D3DXCOLOR		m_cDiffuse1;
	D3DXCOLOR		m_cDiffuse2;

	D3DXVECTOR2		m_vTex01;
	D3DXVECTOR2		m_vTex02;

	D3DXVECTOR2		m_vTexUV01;
	D3DXVECTOR2		m_vTexUV02;

	char			m_szTex1[MAX_PATH];
	char			m_szTex2[MAX_PATH];

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_MULTITEX_102()
	{
		memset( m_szTex1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTex2, 0, sizeof(char)*MAX_PATH );

		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_MULTITEX_103_105	//	데이터 포맷 변경 진행	
{
	DWORD			m_dwFlag;
	DWORD			m_dwMaterials;

	int				m_nBlend;
	int				m_nTexNum;

	D3DXCOLOR		m_cDiffuse1;
	D3DXCOLOR		m_cDiffuse2;

	D3DXVECTOR2		m_vTex01;
	D3DXVECTOR2		m_vTex02;

	D3DXVECTOR2		m_vTexUV01;
	D3DXVECTOR2		m_vTexUV02;

	char			m_szTex1[MAX_PATH];
	char			m_szTex2[MAX_PATH];

	char			m_szTexture[MAX_PATH];
};

struct EFFCHAR_PROPERTY_MULTITEX : public EFFCHAR_PROPERTY		// Ver.106
{
	DWORD			m_dwFlag;
	DWORD			m_dwMaterials;

	char			m_szTex1[MAX_PATH];
	char			m_szTex2[MAX_PATH];

	char			m_szTexture[MAX_PATH];

	int				m_nBlend;
	int				m_nTexNum;

	D3DXCOLOR		m_cDiffuse1;
	D3DXCOLOR		m_cDiffuse2;

	D3DXVECTOR2		m_vTex01;
	D3DXVECTOR2		m_vTex02;

	D3DXVECTOR2		m_vTexUV01;
	D3DXVECTOR2		m_vTexUV02;

	EFFCHAR_PROPERTY_MULTITEX () :
		m_nBlend(0),
		m_dwMaterials(0),
		m_nTexNum(1),
		m_vTex01(0.1f,0.f),
		m_vTex02(0.f,0.1f),
		m_vTexUV01(1.f,1.f),
		m_vTexUV02(1.f,1.f),
		m_cDiffuse1(0xffdddddd),
		m_cDiffuse2(0xffdddddd)
	{
		m_dwFlag = 0L;
		m_dwFlag |= USE_ALL_TEX;

		StringCchCopy( m_szTexture, MAX_PATH, "1d_Lighting.bmp" );

		StringCchCopy( m_szTex1, MAX_PATH, "Rain.dds" );	//
		StringCchCopy( m_szTex2, MAX_PATH, "Rain.dds" );	//
	}

	void Assign ( EFFCHAR_PROPERTY_MULTITEX_103_105& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_dwMaterials = sOldData.m_dwMaterials;
		StringCchCopy( m_szTex1, MAX_PATH, sOldData.m_szTex1 );
		StringCchCopy( m_szTex2, MAX_PATH, sOldData.m_szTex2 );
		StringCchCopy( m_szTexture, MAX_PATH, sOldData.m_szTexture );
		m_nBlend = sOldData.m_nBlend;
		m_nTexNum = sOldData.m_nTexNum;
		m_cDiffuse1 = sOldData.m_cDiffuse1;
		m_cDiffuse2 = sOldData.m_cDiffuse2;
		m_vTex01 = sOldData.m_vTex01;
		m_vTex02 = sOldData.m_vTex02;
		m_vTexUV01 = sOldData.m_vTexUV01;
		m_vTexUV02 = sOldData.m_vTexUV02;
	}
};

class DxEffCharMultiTex : public DxEffChar
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
			EFFCHAR_PROPERTY_MULTITEX m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			DWORD			m_dwMaterials;

			char			m_szTex1[MAX_PATH];
			char			m_szTex2[MAX_PATH];

			char			m_szTexture[MAX_PATH];

			int				m_nBlend;
			int				m_nTexNum;

			D3DXCOLOR		m_cDiffuse1;
			D3DXCOLOR		m_cDiffuse2;

			D3DXVECTOR2		m_vTex01;
			D3DXVECTOR2		m_vTex02;

			D3DXVECTOR2		m_vTexUV01;
			D3DXVECTOR2		m_vTexUV02;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_MULTITEX*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static LPDIRECT3DSTATEBLOCK9		m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9		m_pEffectStateBlock;

protected:
	D3DXVECTOR2			m_vSumTex01;
	D3DXVECTOR2			m_vSumTex02;

    TextureResource m_textureRes;
    TextureResource m_textureRes1;
    TextureResource m_textureRes2;

protected:
	DXMATERIAL_CHAR_EFF*	m_pMaterials;
	DXMATERIAL_CHAR_EFF*	m_pTempMaterials;
	EMCHAR_EFF_TYPE*		m_pMaterials_EffUse;
	DWORD					m_dwTempMaterials;

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

public:
	virtual void SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials );
	virtual DXMATERIAL_CHAR_EFF*		GetMaterials () { return m_pMaterials; }

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_ADD; }

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender );

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:
	virtual void PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
												EffectMaterialData* pEffMaterialData, 
												const D3DXMATRIX* pMatrix, 
												const float fScale ) OVERRIDE;

	// HardAlpha
	virtual void PrecalculateForMaterialSystemHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
														EffectMaterialData* pEffMaterialData ) OVERRIDE;

	// SoftAlpha
	virtual void PrecalculateForMaterialSystemSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
														EffectMaterialData* pEffMaterialData ) OVERRIDE;

	// Add 에서 작업
	virtual void PrecalculateForMaterialSystemAdd( LPDIRECT3DDEVICEQ pd3dDevice, EffectMaterialData* pEffMaterialData ) OVERRIDE;

	virtual void RenderMaterialAdd ( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialSystem, const D3DXMATRIX& matStaticLinkBone, DxSkinMesh9_Material& sSkinMesh, const DxPhysXCloth* pPhysXCloth ) OVERRIDE;

private:
	// RenderMaterial 를 하기전 필요한 정보를 셋팅하기 위한 작업.
	// DxEffCharMultiTex 의 pEffMaterialData->m_rMultiTex 정보가 매번 변하기 때문에 
	// Render 전에 변한걸 가지고 있도록 한다.
	BOOL PrecalculateForMaterialSystemMultiTex( LPDIRECT3DDEVICEQ pd3dDevice, EffectMaterialData* pEffMaterialData, TEXTURE_ALPHA emTextureType );

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh();

private:
	BOOL IsBaseRenderOff();

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
	DxEffCharMultiTex(void);
	virtual ~DxEffCharMultiTex(void);
};
