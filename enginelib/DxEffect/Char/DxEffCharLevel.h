#pragma once

#include "DxEffChar.h"

#include "../../DxTools/MaterialSkin/MaterialSkinDef.h"
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

struct EFFCHAR_PROPERTY_LEVEL_100
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];
	char			m_szSrcTex_4[MAX_PATH];	

	char			m_szTexture_1[MAX_PATH];
	char			m_szTexture_2[MAX_PATH];
	char			m_szTexture_3[MAX_PATH];
	char			m_szTexture_4[MAX_PATH];

	EFFCHAR_PROPERTY_LEVEL_100()
	{
		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_4, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_4, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_LEVEL_101
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;

	D3DXCOLOR		m_cSpecular;

	char			m_szSrcTex_1[MAX_PATH];
	char			m_szSrcTex_2[MAX_PATH];
	char			m_szSrcTex_3[MAX_PATH];
	char			m_szSrcTex_4[MAX_PATH];		//지워야함

	char			m_szTexture_1[MAX_PATH];
	char			m_szTexture_2[MAX_PATH];
	char			m_szTexture_3[MAX_PATH];
	char			m_szTexture_4[MAX_PATH];	//지워야함

	EFFCHAR_PROPERTY_LEVEL_101()
	{
		memset( m_szSrcTex_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szSrcTex_4, 0, sizeof(char)*MAX_PATH );

		memset( m_szTexture_1, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_2, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_3, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture_4, 0, sizeof(char)*MAX_PATH );
	};
};

struct EFFCHAR_PROPERTY_LEVEL_102_103
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;
	DWORD			m_dwMaterials;

	D3DXCOLOR		m_cSpecular;
};

struct EFFCHAR_PROPERTY_LEVEL_104_105	//	데이터 포맷 변경 진행
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;
	DWORD			m_dwMaterials;

	float			m_fPower;
	D3DXCOLOR		m_cSpecular;

	char			m_szTexture[MAX_PATH];
};

struct EFFCHAR_PROPERTY_LEVEL : public EFFCHAR_PROPERTY	//Ver.106
{
	DWORD			m_dwFlag;
	DWORD			m_dwColorOP;
	DWORD			m_dwMaterials;
	float			m_fPower;
	char			m_szTexture[MAX_PATH];
	D3DXCOLOR		m_cSpecular;

	EFFCHAR_PROPERTY_LEVEL () :
		m_dwColorOP(D3DTOP_MODULATE),
		m_dwMaterials(0L),
		m_fPower(14.f),
		m_cSpecular(0.8f,0.8f,0.8f,1.0f)
	{
		m_dwFlag = USETEXTURE1 | USETEXTURE2 | USETEXTURE3 | USETEXTURE4;

		//StringCchCopy( m_szTexture, MAX_PATH, "_Wa_water5.bmp" );
		StringCchCopy( m_szTexture, MAX_PATH, "Rain.dds" );
	}

	void Assign ( EFFCHAR_PROPERTY_LEVEL_104_105& sOldData )
	{
		m_dwFlag = sOldData.m_dwFlag;
		m_dwColorOP = sOldData.m_dwColorOP;
		m_dwMaterials = sOldData.m_dwMaterials;
		m_fPower = sOldData.m_fPower;
		m_cSpecular = sOldData.m_cSpecular;

		StringCchCopy( m_szTexture, MAX_PATH, sOldData.m_szTexture );
	}
};

class DxEffCharLevel : public DxEffChar
{
public:
	static DWORD		TYPEID;
	const static DWORD	VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return EMECF_LEVEL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_LEVEL m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			DWORD			m_dwColorOP;
			DWORD			m_dwMaterials;
			float			m_fPower;
			char			m_szTexture[MAX_PATH];
			D3DXCOLOR		m_cSpecular;			
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_LEVEL*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_A;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_A;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_S;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_S;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_R;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_R;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_F;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_F;

	static LPDIRECT3DSTATEBLOCK9		m_pSavedSB_G;
	static LPDIRECT3DSTATEBLOCK9		m_pDrawSB_G;

protected:
	struct	DXAMBIENT
	{
		BOOL			m_bUse;
		float			m_fBaseColor;
		float			m_fDynamicColor;
		D3DXCOLOR		m_cColor;

		DXAMBIENT () :
			m_bUse(FALSE),
			m_fBaseColor(1.f),
			m_fDynamicColor(0.f)
		{
			m_cColor.a = 1.f;
			m_cColor.r = 1.f;
			m_cColor.g = 1.f;
			m_cColor.b = 1.f;
		}
	};

	struct	DXSPECULAR
	{
		BOOL			m_bUse;
		float			m_fPower;
		DWORD			m_dwColorOP;
		D3DXCOLOR		m_cColor;

		DXSPECULAR()
            : m_bUse(FALSE)
            , m_fPower(5.f)
            , m_dwColorOP(D3DTOP_MODULATE)
            , m_cColor(D3DCOLOR_ARGB(255, 255, 255, 255))
		{
			//D3DXCOLOR ( D3DCOLOR_ARGB ( 255, 255, 255, 255 ) );
		}
	};

	struct	DXFLOW
	{
		BOOL			m_bUse;
		D3DXCOLOR		m_cColor;

		DXFLOW () :
			m_bUse(FALSE)
		{
			m_cColor.a = 1.f;
			m_cColor.r = 1.f;
			m_cColor.g = 1.f;
			m_cColor.b = 1.f;
		}
	};

	struct	DXGLOW
	{
		BOOL			m_bUse;
		D3DXCOLOR		m_cColor;

		DXGLOW () :
			m_bUse(FALSE)
		{
			m_cColor.a = 1.f;
			m_cColor.r = 1.f;
			m_cColor.g = 1.f;
			m_cColor.b = 1.f;
		}
	};

	struct	DXOPTION
	{
		DXAMBIENT		m_Ambient;
		DXSPECULAR		m_Specular;
		DXSPECULAR		m_Reflect;
		DXFLOW			m_Flow;
		DXGLOW			m_Glow;
	};

	enum
	{
		LEVEL_LIMIT = 11,	//5, 
		LEVEL_ARRAY = 12,	//6,	// LEVEL_LIMIT+1
	};

	DXOPTION			m_OpLEVEL [ LEVEL_ARRAY ];	// 기본 0 LEVEL 1~5 까지

protected:
	int					m_nTestLEVEL;

protected:
	DWORD				m_dwOldAmbient;

protected:
	float				m_fTime;
	float				m_fLevelTime;

protected:
	D3DXVECTOR2			m_vSumTex01;
	D3DXVECTOR2			m_vTexUV01;
	D3DXVECTOR2			m_vTex01;
	D3DXVECTOR2			m_vMove;
	BOOL				m_bFlow_USE;

protected:
	TextureResource m_textureRes;
	std::string m_szLevelFlowTex;
	TextureResource m_textureResFlow;

protected:
	static BOOL			m_bFlowReflectUSE;

protected:
	BOOL						m_bMaterialSystem;
	DXMATERIAL_CHAR_EFF*		m_pMaterials;
	DXMATERIAL_CHAR_EFF*		m_pTempMaterials;
	DWORD						m_dwTempMaterials;
    SKINEFF_LEVEL_DATA			m_sSkinEffLevelData;		// VisualMaterial에서 사용됨.
	EffectMaterialData_Level    m_sEffMaterialData_Level;   // MaterialSystem OLD 에서 사용됨.

protected:
	DxSkinMesh9*				m_pSkinMesh;
	PSMESHCONTAINER				m_pmcMesh;

public:
	void	SetTestLEVEL ( const int nLevel )		{ m_nTestLEVEL = nLevel; }

public:
	virtual void SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterials );
	virtual DXMATERIAL_CHAR_EFF*		GetMaterials () { return m_pMaterials; }
    virtual void    GetLevelData( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFF_LEVEL_DATA** ppLevelData );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	RenderGlow( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender, BOOL bActiveVisualMaterial );

	virtual DWORD GetStateOrder () const					{ return EMEFFSO_LEVEL; }

protected:
	HRESULT	Render_EX( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, int nLevel, const BOOL bPieceRender, const D3DXMATRIX& matStaticLinkBone, const D3DXMATRIX* arrayBoneMatrices );
	HRESULT	Render_Ambient(const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXAMBIENT& cData, const BOOL bPieceRender, const D3DXMATRIX& matStaticLinkBone, const D3DXMATRIX* arrayBoneMatrices );
	HRESULT	Render_Specular(const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXSPECULAR& cData, const BOOL bPieceRender, const D3DXMATRIX& matStaticLinkBone, const D3DXMATRIX* arrayBoneMatrices );
	HRESULT	Render_Reflect( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXSPECULAR& cData, const BOOL bPieceRender, const D3DXMATRIX& matStaticLinkBone, const D3DXMATRIX* arrayBoneMatrices );
	HRESULT	Render_Flow( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXFLOW& cData, const BOOL bPieceRender, const D3DXMATRIX& matStaticLinkBone, const D3DXMATRIX* arrayBoneMatrices );
	HRESULT	Render_Glow( const LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialPiece, const DXGLOW& cData, const BOOL bPieceRender, const D3DXMATRIX& matStaticLinkBone, const D3DXMATRIX* arrayBoneMatrices );

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:

	// 프로그램상 MaskMap 을 만들기 위한 작업.
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

	// 
	virtual void RenderMaterialGlow ( LPDIRECT3DDEVICEQ pd3dDevice, SMATERIAL_PIECE* pMaterialSystem, const D3DXMATRIX& matStaticLinkBone, DxSkinMesh9_Material& sSkinMesh, const DxPhysXCloth* pPhysXCloth );

	// ThreadLoading 으로 인해 m_pmcMesh 정보가 없을 수 있어서 사용하는 곳에서 매번 체크하도록 한다.
private:
	BOOL IsExist_Copy_pmcMesh();

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
	HRESULT LoadFile_0107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// Material System 적용에 따른 Texture 정보 필요성 사라짐.

public:
	DxEffCharLevel(void);
	virtual ~DxEffCharLevel(void);
};
