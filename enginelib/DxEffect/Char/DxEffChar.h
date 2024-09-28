#pragma once
#include "../../../SigmaCore/File/BaseStream.h"

#include "../../DxMeshs/SkinMesh/DxMaterial.h"
#include "../../DxMeshs/SkinMesh/DxSkinEffResult.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../Single/DxEffSinglePropGMan.h"

struct DxCharPart;
struct EffectMaterialData;
class DxMultiMaterialSkinMesh;
class DxSkinMesh9_Material;
class DxPhysXCloth;

//struct DxSkinPiece;
//struct DxSkeleton;
//class DxSkinAniControl;

template < class T > class CMinMax
{
public:
	CMinMax()				
	{	
		min = T();	
		max = T();	
	}
	CMinMax( T min, T max ) 
	{	
		min = min;	
		max = max;	
	}

	virtual ~CMinMax() { }

public:
	T min;
	T max;


	T GetRange()
	{
		return abs( max - min ); 
	}	
};


enum EFFCHAR_TYPES
{
	EMEFFCHAR_SINGLE		    = 0,
	EMEFFCHAR_BLUR			    = 1,
	EMEFFCHAR_CLONEBLUR		    = 2,
	EMEFFCHAR_DUST			    = 3,
	EMEFFCHAR_SHOCK			    = 4,
	EMEFFCHAR_ATTRIBUTE		    = 5,
	EMEFFCHAR_ARROW			    = 6,
	EMEFFCHAR_SPECULAR		    = 7,
	EMEFFCHAR_LEVEL			    = 8,	//	EMEFFCHAR_SPECULAR2
	EMEFFCHAR_EMIT			    = 9,
	EMEFFCHAR_ALPHA			    = 10,
	EMEFFCHAR_NEON			    = 11,
	EMEFFCHAR_MARK			    = 12,
	EMEFFCHAR_NOALPHA		    = 13,
	EMEFFCHAR_REFLECTION2	    = 14,
	EMEFFCHAR_AMBIENT		    = 15,
	EMEFFCHAR_DOT3			    = 16,
	EMEFFCHAR_MULTITEX		    = 17,
	EMEFFCHAR_GHOSTING		    = 18,
	EMEFFCHAR_SPECULAR2		    = 19,	//	EMEFFCHAR_LEVEL
	EMEFFCHAR_TOON			    = 20,
	EMEFFCHAR_TEXDIFF		    = 21,
	EMEFFCHAR_PARTICLE		    = 22,
	EMEFFCHAR_BONEPOSEFF	    = 23,
	EMEFFCHAR_BONELISTEFF	    = 24,
	EMEFFCHAR_USERCOLOR		    = 25,
	EMEFFCHAR_NORMALMAP		    = 26,
	EMEFFCHAR_LINE2BONE		    = 27,
	EMEFFCHAR_AROUNDEFFECT	    = 28,
	EMEFFCHAR_OVERLAY		    = 29,
	EMEFFCHAR_TOTALSHADER	    = 30,
    EMEFFCHAR_VISUAL_MATERIAL   = 31,
    EMEFFCHAR_SPLINE_SINGLE_EFF = 32,
	EMEFFCHAR_WORLD_BATTLE_FLAG = 33,
	EMEFFCHAR_NULL				= 34,
    EMEFFCHAR_SIZE,
};

enum EMEFFCHAR_FLAGS			// 내가 누구인지 확실히 알려야 한다.
{
	EMECF_SPECULAR	    	= 0x00001,
	EMECF_DOT3		    	= 0x00002,
	EMECF_LEVEL		    	= 0x00004,
	EMECF_TOON		    	= 0x00008,
	EMECF_AMBIENT	    	= 0x00010,
	EMECF_NOALPHA	    	= 0x00020,
	EMECF_ALPHA		    	= 0x00040,
	EMECF_SHOCK		    	= 0x00080,
	EMECF_TEXDIFF	    	= 0x00100,
	EMECF_NORMALMAP	    	= 0x00200,
	EMECF_SPECULAR2	    	= 0x00400,	// MulpiPass Rendering
	EMECF_RELFECT		    = 0x00800,	// MulpiPass Rendering
    EMECF_VISUAL_MATERIAL	= 0x01000,
    EMECF_OVERLAY	        = 0x02000,
    EMECF_TOTAL_SHADER      = 0x04000,
	EMECF_WB_FLAG			= 0x08000,
	EMECF_NONE				= 0x10000,

	EMECF_NONHLSL		    = (EMECF_SPECULAR2 | EMECF_RELFECT),	                            // EMECF_NORMALMAP 랑 같이 쓰면 안되는 것.~!!!								
	EMECF_ORGINNON		    = (EMECF_SPECULAR | EMECF_DOT3 | EMECF_TOON | EMECF_VISUAL_MATERIAL
                                |EMECF_OVERLAY | EMECF_TOTAL_SHADER | EMECF_NONE),			// 원본을 뿌리지 않고 내가 원본이 된다.
};

enum EMCHAR_FLAG				// Flag Save, Load
{
	EMCF_SHADOW	= 0x00001,
};

enum CRDT_TYPE					// 이젠 쓸모 없는 거 같다.
{
	CRDT_DEFAULT		= 0,
	CRDT_SPECULAR		= 1,
	CRDT_DOT3_SPECULAR	= 2,
	CRDT_DOT3_REFLECT	= 3,
	CRDT_GLOW			= 4,
};

enum EMEFFCHAR_STATE_ORDER		// State 변화 할때 보통 쓰임
{
	//	renderstate 조정 순서에 따라 정렬.
	EMEFFSO_SHOCK		= 1,
	EMEFFSO_NOALPHA		= 2,
	EMEFFSO_ALPHA		= 3,
	EMEFFSO_MARK		= 4,
	EMEFFSO_AMBIENT		= 5,
	EMEFFSO_TOON		= 6,
	
	EMEFFSO_RENDERSTATE	= 6,	//	여기까지 renderstate 관련 효과.

	EMEFFSO_SINGLE_EFF	= 11,	// MaterialSystem 에서도 사용
	EMEFFSO_ADD			= 12,
	EMEFFSO_GLOW		= 13,
	EMEFFSO_LEVEL		= 14,
	EMEFFSO_GHOSTING	= 15,
	EMEFFSO_NORMAL		= 16,
};

struct EFFCHAR_PROPERTY
{
};

class DxEffChar
{
public:
	enum
	{
		RF_DIE	= 0x0010,
	};

public:
	virtual DWORD GetTypeID() = 0;
	virtual DWORD GetFlag() = 0;
	virtual const char* GetName() = 0;

protected:
	DWORD			m_dwRunFlag;
	DWORD			m_dwFlag;
	std::string		m_strFileName;

	float			m_fAge;

	DxCharPart*		m_pCharPart;
	DxSkinPiece*	m_pSkinPiece;
	BOOL            m_bMyChar;

	DWORD				m_dwKeyTime;
	EMANI_MAINTYPE		m_CurAniMType;
	EMANI_SUBTYPE		m_CurAniSType;
	DxSkinAniControl*	m_rSkinAniControl;	// 참조값
	D3DXMATRIX			m_matCurAni;

	const DxSkeletonMaintain*	m_rSkeleton;

	D3DXMATRIX			m_matWorld;

public:
	void SetEnd()										{ m_dwRunFlag |= RF_DIE; }
	void SetFileName(const std::string& strFileName )	{ m_strFileName = strFileName; }
	std::string GetFileName() const						{ return m_strFileName; }

public:
	BOOL IsEnd() const { return (m_dwRunFlag&RF_DIE); }

public:
	virtual HRESULT OneTimeSceneInit () { return S_OK; }
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) { return S_OK; }
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) { return S_OK; }
	virtual HRESULT InvalidateDeviceObjects () { return S_OK; }
	virtual HRESULT DeleteDeviceObjects () { return S_OK; }
	virtual HRESULT FinalCleanup () { return S_OK; }

public:
	void	Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, const SKINEFFDATA& sSKINEFFDATA );

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime ) = 0;
	virtual void	Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender )					{}
	virtual void	RenderVM( LPDIRECT3DDEVICEQ pd3dDevice, const SKINEFFDATA* pSKINEFFDATA, TEXTURE_TYPE_FLAG emTexType )	{}
    virtual void	RenderGlow( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* arrayBoneMatrices, BOOL bPieceRender, BOOL bActiveVisualMaterial );
	virtual void	RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix, const float fScale )	{}
	virtual void	RenderEDIT( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )        {}
    virtual void    RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIXA16* pBoneMatrices, LPD3DXBONECOMBINATION pBoneCur, int nPaletteEntries, int nInfl, TEXTURE_TYPE_FLAG emTexType )	{}
    virtual void    RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwAttrib, TEXTURE_TYPE_FLAG emTexType, const D3DXMATRIX& matStaticLinkBone )															{}
    virtual void    ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice )                                                                              {}

public:
	virtual void	SetWorldMatrix( const D3DXMATRIX& matWorld ) { m_matWorld = matWorld; }
	virtual DWORD	GetStateOrder () const															{ return EMEFFSO_NORMAL; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA )		{ return S_OK; }
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )									{ return S_OK; }
    virtual void    GetLevelData( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFF_LEVEL_DATA** ppLevelData )	{}

	virtual void ConvertTracePoint()	{}

	// 이곳에서 텍스쳐가 만들어지면, 렌더링 되는 BaseTexture 를 사용하지 않고 이곳에서 만들어진 것을 사용하게 된다.
	virtual void SetWorldBattleFlag( LPDIRECT3DTEXTUREQ apOverlayTexOUT[], const TCHAR* pWorldBattleFlag )	{}

	//////////////////////////////////////////////////////////////////////////
	// Material System 에 현재 효과들을 붙이기 위한 작업.
public:

	// 프로그램상 MaskMap 을 만들기 위한 작업.
	virtual void ExportEffTexture( LPDIRECT3DDEVICEQ pd3dDevice, 
									DWORD dwMaterialID, 
									LPDIRECT3DTEXTUREQ* ppLevelTex, 
									LPDIRECT3DTEXTUREQ* ppCubeTex,
									bool bTexThread,
									const TCHAR* pTextureName_NoHave )	{}

	// Effect Texture 들이 다 로딩이 되었는가~?
	virtual BOOL IsEnableEffTexture( BOOL bTexThread )	{ return TRUE; }

	// 1. EffectMaterialData 정보를 얻어오거나 
	// 2. Effect 렌더시 Skeleton 의 Matrix 값이 변경 되어있기 때문에..(Effect는 렌더순서 나중)
	//		렌더정보를 다 계산해서 OPTMManager 써서 넘겨주거나,
	//		혹은 본 정보의 필요한 값만 미리 백업해놓고 사용하는 방법을 쓰도록 한다.
	virtual void PrecalculateForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
												EffectMaterialData* pEffMaterialData, 
												const D3DXMATRIX* pMatrix, 
												const float fScale )					{}

	// HardAlpha
	virtual void PrecalculateForMaterialSystemHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
														EffectMaterialData* pEffMaterialData )					{}

	// SoftAlpha
	virtual void PrecalculateForMaterialSystemSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
															EffectMaterialData* pEffMaterialDatae )					{}

	// Add 에서 작업
	virtual void PrecalculateForMaterialSystemAdd( LPDIRECT3DDEVICEQ pd3dDevice, EffectMaterialData* pEffMaterialData )	{}

	// Effect 같은 경우는 이걸 호출하게 된다. 반사 같은 경우 제외를 쉽게 하기 위함.
	virtual void PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix )	{}

	// PrecalculateForMaterialSystem의 2번 중 값만 미리 백업해 놓는거라면, 이곳에서 렌더가 된다.
	virtual void RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice,
											const D3DXMATRIX* pMatrix, 
											const float fScale )						{}

	virtual void RenderMaterialGlow ( LPDIRECT3DDEVICEQ pd3dDevice, 
										SMATERIAL_PIECE* pMaterialSystem, 
										const D3DXMATRIX& matStaticLinkBone, 
										DxSkinMesh9_Material& sSkinMesh, 
										const DxPhysXCloth* pPhysXCloth )	{}

	virtual void RenderMaterialAdd ( LPDIRECT3DDEVICEQ pd3dDevice, 
										SMATERIAL_PIECE* pMaterialSystem, 
										const D3DXMATRIX& matStaticLinkBone, 
										DxSkinMesh9_Material& sSkinMesh, 
										const DxPhysXCloth* pPhysXCloth )	{}

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty ) = 0;
	virtual EFFCHAR_PROPERTY* GetProperty () = 0;

	virtual void SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterial )	{}
	virtual DXMATERIAL_CHAR_EFF*	GetMaterials ()					{ return NULL; }

	void SetCurrentAni ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwKeyTime )
	{
		m_CurAniMType	= MType;
		m_CurAniSType	= SType;
		m_dwKeyTime		= dwKeyTime;
	}

	void SetAniControl( DxSkinAniControl* _pSkinAniControl, const D3DXMATRIX &_matCurAni )
	{
		m_rSkinAniControl = _pSkinAniControl;
		m_matCurAni = _matCurAni;
	}

	void SetAniControl( DxSkinAniControl* _pSkinAniControl )
	{
		m_rSkinAniControl = _pSkinAniControl;
	}

	virtual void SetSkeleton( const DxSkeletonMaintain* pSkeleton ) { m_rSkeleton = pSkeleton; }

	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum ) = 0;
	
	virtual void SetDirection ( const D3DXVECTOR3 *pDir, float fTarDir ) { };
	virtual void SetLinkObj ( DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
	{
		m_pCharPart = pCharPart;
		m_pSkinPiece = pSkinPiece;
	}

	void SetMyChar()    { m_bMyChar = TRUE; }

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual HRESULT SaveFile ( sc::BaseStream &SFile ) = 0;

public:
	virtual void SetCharSingleHellMatrixMode( int nMode )		{}

public:
	HRESULT Create ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( FAILED(OneTimeSceneInit()) )				return E_FAIL;
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )	return E_FAIL;
		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )	return E_FAIL;

		return S_OK;
	}

	HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
		FinalCleanup();

		return S_OK;
	}

public:
	DxEffChar(void);
	virtual ~DxEffChar(void);
};


class DXEFFCHAR_OPER
{
public:
	bool operator() ( const DxEffChar* lvalue, const DxEffChar* rvalue )
	{
		return lvalue->GetStateOrder() < rvalue->GetStateOrder();
	}
	
	bool operator() ( const DxEffChar* lvalue, const DWORD rvalue_dwORDER )		// A
	{
		return lvalue->GetStateOrder() < rvalue_dwORDER;
	}

	//
	//mjeon.migration
	//
	// Debug_Iterator checks whether Predicate is commutative.	-> If A is TRUE, B MUST be FALSE;
	//
	
	bool operator() ( const DWORD lvalue_dwORDER, const DxEffChar* rvalue )		// B
	{
		return lvalue_dwORDER < rvalue->GetStateOrder() ;
	}
};


class DXEFFCHAR_DISOPER
{
public:
	bool operator() ( const DxEffChar* lvalue, const DxEffChar* rvalue )
	{
		return lvalue->GetStateOrder() > rvalue->GetStateOrder();
	}

	bool operator() ( const DxEffChar* lvalue, const DWORD rvalue_dwORDER )
	{
		return lvalue->GetStateOrder() > rvalue_dwORDER;
	}

	//
	//mjeon.migration
	//
	// Debug_Iterator checks whether Predicate is commutative.	-> If A is TRUE, B MUST be FALSE;
	//

	bool operator() ( const DWORD lvalue_dwORDER, const DxEffChar* rvalue )
	{
		return lvalue_dwORDER > rvalue->GetStateOrder();
	}
};

struct DxEffCharType
{
	DWORD	TYPEID;
	char	NAME[MAX_PATH];

	DxEffCharType *pNext;

	DxEffCharType() :
		TYPEID(0xffffffff),
		pNext(NULL)
	{
		memset( NAME, 0, sizeof(char)*MAX_PATH );
	}

	~DxEffCharType ()
	{
		SAFE_DELETE(pNext);
	}
};

class DxEffCharMan
{
protected:
	void RegistType ( const DWORD TypeID, const char *Name );

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	D3DXVECTOR3			m_vViewMax; 
	D3DXVECTOR3			m_vViewMin;
	D3DXVECTOR3&		GetViewMax ()	{ return m_vViewMax; }
	D3DXVECTOR3&		GetViewMin ()	{ return m_vViewMin; }
	
protected:
	VOID				SetViewBoard ();

protected:
	DxEffCharType*		m_pEffectTypeList;

public:
	DxEffCharType* GetEffectList ()	{ return m_pEffectTypeList; }

public:
	DxEffChar*	CreateEffInstance ( DWORD TypeID );
	DxEffChar*	CreateEffect ( DWORD TypeID, EFFCHAR_PROPERTY* pProp, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );
	DxEffChar*	CopyEffect ( DxEffChar* pEff, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );
	void		DeleteEffect ( DxEffChar* pEffChar );
	DxEffChar*  AddDefaultAmbient ( DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();

protected:
	DxEffCharMan ();

public:
	~DxEffCharMan ();

public:
	static DxEffCharMan& GetInstance();
};