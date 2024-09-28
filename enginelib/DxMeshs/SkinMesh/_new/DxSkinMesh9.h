#pragma once

#include "../DxSkeletonManager.h"

#include "../../../../SigmaCore/File/BaseStream.h"
#include "../../../DxTools/TextureManager.h"

namespace sc {
    class SerialFile;
}

class DxMaterialHLSL;
//
//// enum for various skinning modes possible
//enum METHOD
//{
//    D3DNONINDEXED,
//    D3DINDEXED,
//    SOFTWARE,
//    D3DINDEXEDVS,
//    D3DINDEXEDHLSLVS,
//    NONE
//};

enum SKIN_LOADING_MODE
{
	SKIN_LOADING_END_BONE	= 0x01,
	SKIN_LOADING_END_MEMORY	= 0x02,
	SKIN_LOADING_END_SKIN	= 0x04,
	SKIN_LOADING_END_FINAL	= 0x08,
	SKIN_COMPLETED			= 0x10,
	SKIN_LOADING_END_ERROR	= 0x20,
};

class DxEffChar;
struct SMeshContainer;
struct DxSkeleton;
struct SVERTEXINFLU;
struct SMATERIAL_PIECE;
struct DXMATERIAL_CHAR_EFF;
class DxSkeletonMaintain;

//typedef struct _D3DXFRAME {
//    LPSTR Name;
//    D3DXMATRIX TransformationMatrix;
//    LPD3DXMESHCONTAINER pMeshContainer;
//    struct _D3DXFRAME *pFrameSibling;
//    struct _D3DXFRAME *pFrameFirstChild;
//} D3DXFRAME, *LPD3DXFRAME;

//--------------------------------------------------------------------------------------
// Name: struct D3DXFRAME_DERIVED
// Desc: Structure derived from D3DXFRAME so we can add some app-specific
//       info that will be stored with each frame
//--------------------------------------------------------------------------------------
struct SFrame : public D3DXFRAME
{
	D3DXMATRIXA16		CombinedTransformationMatrix;

    SFrame()
    {
        Name = NULL;
        D3DXMatrixIdentity( &TransformationMatrix );

        pMeshContainer = NULL;

        pFrameSibling = NULL;
        pFrameFirstChild = NULL;

        D3DXMatrixIdentity( &CombinedTransformationMatrix );
    }
    ~SFrame()
    {
        SAFE_DELETE_ARRAY( Name );
    }

	SFrame *FindFrame ( char *szFrame )
	{
		SFrame *pframe;
		if ( (Name!=NULL) && (strcmp(Name, szFrame) == 0) )
            return this;

		if ( pFrameFirstChild != NULL )
		{
			pframe = ((SFrame*)pFrameFirstChild)->FindFrame ( szFrame );
			if ( pframe != NULL )
                return pframe;
        }

		if ( pFrameSibling != NULL )
		{
			pframe = ((SFrame*)pFrameSibling)->FindFrame ( szFrame );
			if ( pframe != NULL )
                return pframe;
		}

		return NULL;
	}

	SFrame* FindParentFrame ( SFrame* pFindFrame )
	{
		SFrame *pframeChild = (SFrame*)pFrameFirstChild;
		while ( pframeChild != NULL )
		{
			if ( pframeChild == pFindFrame )
                return this;

			SFrame *pframe = pframeChild->FindParentFrame ( pFindFrame );
			if ( pframe != NULL )
                return pframe;

			pframeChild = (SFrame*)pframeChild->pFrameSibling;
		}

		return NULL;
	}
};

enum RENDER_OPT
{
	CTOP_BASE	= 0,
    CTOP_SHADOW = 1,	// Toon - 음영.
    CTOP_EDGE	= 2,	// Toon - Edge
	CTOP_NEWTEX = 3,	// New Textuer
};

struct CHARSETTING
{
	LPD3DXMESHCONTAINER		pMeshContainerBase;
	SMATERIAL_PIECE*		pmtrlPiece;
	DXMATERIAL_CHAR_EFF*	pmtrlSpecular;
	BOOL					bWorldIdentity;
	RENDER_OPT				emRDOP;
	LPDIRECT3DTEXTUREQ*		m_r_apOverlayTex;

	CHARSETTING()
		: pMeshContainerBase(NULL)
		, pmtrlPiece(NULL)
		, pmtrlSpecular(NULL)
		, bWorldIdentity(FALSE)
		, emRDOP(CTOP_BASE)
		, m_r_apOverlayTex(NULL)
	{

	}
};

//-------------------------------------------------------------------------------------------------------
//	Overlay에서 여러장의 텍스쳐를 쓸때, 각 텍스쳐마다 overlay의 속성을 저장하기 위한 구조체. by luxes.
//-------------------------------------------------------------------------------------------------------
struct EFFCHAR_PROPERTY_OVERLAY_TEXTURE
{
	bool						m_bOverlay;				// overlay 적용 유무.

	D3DXVECTOR4					m_vLightPos;			// 광원 벡터.
	D3DXVECTOR4					m_vEyePos;				// 시선 벡터.
	D3DXMATRIX					m_matWorld;				// 월드 행렬.
	D3DXMATRIX					m_matView;				// 뷰 행렬.
	D3DXMATRIX					m_matProj;				// 프로젝션 행렬.
	D3DXMATRIX					m_matWorldInverse;		// 월드 역 행렬.
	D3DXMATRIX					m_matWorldInvTrans;		// 월드 역전치 행렬.
	D3DXMATRIX					m_matViewInverse;		// 뷰 역 행렬.
	D3DXVECTOR4					m_vColor1;				// overlay color1.
	D3DXVECTOR4					m_vColor2;				// overlay color2.
	D3DXVECTOR4					m_vAmbient;				// 환경광.
	D3DXVECTOR4					m_vDiffuse;				// 산란광.
	D3DXVECTOR4					m_vSpecular;			// 반사광.
	char						m_szOverlayTex[MAX_PATH];			// overlay texture.
    TextureResource m_textureResOverlay;
	bool						m_bUseSpecular;			// use specular map.
	bool						m_bUseGlowTex;			// use glow texture.
	char						m_szGlowTex[MAX_PATH];				// glow texture.
    TextureResource m_textureResGlow;
	float						m_fGlowBright;			// glow bright value.
	float						m_fGlowAlpha;			// glow alpha value.
	bool						m_bUseReflectTex;		// use reflection texture.
	char						m_szReflectTex[MAX_PATH];			// reflection texture.
    TextureResource m_textureResReflect;
	float						m_fReflectActivity;		// reflection activity value.
	bool						m_bUseAlphaReflectTex;	// use alpha reflection texture.
	char						m_szReflectAlphaTex[MAX_PATH];	// alpha reflection texture.
    TextureResource m_textureResAlphaReflect;
	bool						m_bUseDirLight;
	float						m_fSpecularPower;		// specular power.
	float						m_fSpecularLevel;		// specular level.
	float						m_fAlphaRef;			// alpha ref.
	D3DCOLORVALUE				m_cDiffuse;				// material diffuse.
	D3DCOLORVALUE				m_cAmbient;				// material ambient.
	D3DXMATRIX					m_matWVP;				// world * view * proj.
	D3DXMATRIX					m_matVP;				// view * proj.

	EFFCHAR_PROPERTY_OVERLAY_TEXTURE() :
		  m_bOverlay(true)
		, m_bUseSpecular(false)
		, m_bUseGlowTex(false)
		, m_bUseReflectTex(false)
		, m_bUseAlphaReflectTex(false)
		, m_bUseDirLight(false)
		, m_vColor1(D3DXVECTOR4(128.0f/255.0f,128.0f/255.0f,128.0f/255,255.0f/255.0f))
		, m_vColor2(D3DXVECTOR4(128.0f/255.0f,128.0f/255.0f,128.0f/255,255.0f/255.0f))
		, m_vDiffuse(D3DXVECTOR4(60.0f/255.0f,60.0f/255.0f,60.0f/255.0f,0.0f/255.0f))
		, m_vAmbient(D3DXVECTOR4(200.0f/255.0f,200.0f/255.0f,200.0f/255.0f,0.0f/255.0f))
		, m_vSpecular(D3DXVECTOR4(70.0f/255.0f,70.0f/255.0f,70.0f/255.0f,0.0f/255.0f))
		, m_fSpecularPower(98.0f)
		, m_fSpecularLevel(2.0f)
		, m_fAlphaRef(128.0f)
		, m_fGlowBright(1.0f)
		, m_fGlowAlpha(1.0f)
		, m_fReflectActivity(0.0f)
	{
		ZeroMemory( m_szOverlayTex, MAX_PATH );
		ZeroMemory( m_szGlowTex, MAX_PATH );
		ZeroMemory( m_szReflectTex, MAX_PATH );
		ZeroMemory( m_szReflectAlphaTex, MAX_PATH );
	}

    void SaveFile( sc::BaseStream &SFile );
    void LoadFile( sc::BaseStream &SFile );
};

class DxSkinMesh9
{
public:
	//static METHOD	m_SkinningMethod;
	static DWORD			m_dwBehaviorFlags;	// Behavior flags of the 3D device
	static D3DXMATRIXA16*	g_pBoneMatrices;
	static DWORD            g_NumBoneMatricesReserve;
	static DWORD			g_NumBoneMatricesMax;
	static D3DXMATRIXA16	g_matIdentity;
	
protected:
	TSTRING			m_strName_SKIN;		//	형상 xFile 이름.
	TSTRING			m_strName_BONE;		//
	TSTRING			m_strFullName_SKIN;	//	형상 xFile 이름.

protected:
	DWORD			m_dwRefCount;
	LPD3DXFRAME		m_pFrameRoot;
	SMeshContainer*	m_pMeshContainerHead;
	//DxSkeleton*		m_pSkeleton;
    DxSkeletonResource		m_skeletonRes;

	D3DXVECTOR3		m_vObjectCenter;
	FLOAT			m_fObjectRadius;	// Radius of bounding sphere of object

	ID3DXAnimationController*	m_pAnimController;

//private:
//	static ID3DXEffect*	g_pShadowFX;
//	static ID3DXEffect*	g_pShadowSkinFX;
//	static LPDIRECT3DVERTEXDECLARATION9	g_pShadowDCRT;
//	static LPDIRECT3DVERTEXDECLARATION9	g_pShadowSkinDCRT;

protected:
	BOOL		m_bOriginDraw;
	BOOL		m_bAlpha;
	BOOL		m_bShadow;
	BOOL		m_bAlphaTex;
	BOOL		m_bLevel;
	float		m_fHeight;

public:
	D3DXMATRIXA16	m_matView;			// View matrix
	D3DXMATRIXA16	m_matProj;			// Projection matrix
	D3DXMATRIXA16	m_matProjT;			// Transpose of projection matrix (for asm shader)

private:
	CTypeSmartPtr	m_pSrcData;
	int		m_nSrcDataSize;

	DWORD	m_dwFlagsEX;				// SKIN_LOADING_MODE

public:
	DxSkinMesh9 ();
    virtual ~DxSkinMesh9();

public:
	void DataExchange(DxSkinMesh9& rhs);
	DWORD AddRef ()							{ return ++m_dwRefCount; }
	DWORD DisRef ()							{ if (m_dwRefCount>0) --m_dwRefCount; return m_dwRefCount; }

	static void CreateBoneMatrices();
	static void DestroyBoneMatrices();

public:
	const D3DXVECTOR3& GetObjectCenter ()	{	return m_vObjectCenter; }
	FLOAT GetObjectRadius ()				{	return m_fObjectRadius; }

protected:
	HRESULT SetupNameOnMeshContainer ( LPD3DXFRAME pFrame );

	HRESULT SetupBoneMatrixPointersOnMesh ( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );
	HRESULT SetupBoneMatrixPointers ( LPD3DXFRAME pFrame );

public:
	//void DrawFrame ( IDirect3DDevice9 *pd3dDevice, LPD3DXFRAME pFrame );
	virtual void DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, 
									const CHARSETTING& sCharSetting, 
									const DxSkeletonMaintain* pSkeleton, 
									const D3DXMATRIX& matStaticLinkBone,
									const D3DXMATRIX* arrayBoneMatrices )		{}

	virtual void DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
							DXMATERIAL_CHAR_EFF* pmtrlSpecular=NULL, BOOL bWorldIdentity=FALSE ) = 0;

	virtual void DrawMeshOVERLAY( IDirect3DDevice9 *pd3dDevice, 
									const D3DXMATRIXA16* pBoneMatrices,
									const CHARSETTING& sCharSetting, 
									EFFCHAR_PROPERTY_OVERLAY_TEXTURE* sOverlayVal, 
									ID3DXEffect* pEffect, 
									DWORD dwOverMaterial, 
									BOOL bWorldIdentity=FALSE )						{}

	void DrawMeshNORMALMAP( IDirect3DDevice9 *pd3dDevice, 
							LPD3DXMESHCONTAINER pMeshContainerBase, 
							const D3DXMATRIXA16* pBoneMatrices, 
							ID3DXEffect* pEffect, 
							SMATERIAL_PIECE *pmtrlPiece, 
							BOOL bWorldIdentity=FALSE );

	void DrawMeshTotalShader( IDirect3DDevice9 *pd3dDevice, 
								LPD3DXMESHCONTAINER pMeshContainerBase, 
								const D3DXMATRIXA16* pBoneMatrices, 
								ID3DXEffect* pEffect, 
								SMATERIAL_PIECE *pmtrlPiece, 
								int nOverlayMatrial, 
								BOOL bWorldIdentity=FALSE );

    void DrawMeshVisualMaterial( IDirect3DDevice9 *pd3dDevice, 
								LPD3DXMESHCONTAINER pMeshContainerBase, 
								const D3DXMATRIXA16* pBoneMatrices, 
								DxEffChar* pEffChar, 
								const DxSkeletonMaintain* pSkeleton,
								TEXTURE_TYPE_FLAG emTexType,
								const D3DXMATRIX& matStaticLinkBone );

	void RenderUseHLSL( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, SMATERIAL_PIECE *pmtrlPiece );

	//void DrawMeshForMaterialSystem( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, const D3DXMATRIX& matViewProj );

	// Note	: 일반메쉬를 렌더링 할 경우 쓰이는 것.
	// Parameters : pMatrix는 NULL 이 오면 절대 안된다.  
	//				에러를 유발한다.
	void DrawFrameMesh( IDirect3DDevice9 *pd3dDevice, SMeshContainer* pMeshContainer, SMATERIAL_PIECE* pmtrlPiece, LPD3DXMATRIX pMatrix, LPDIRECT3DTEXTUREQ* apOverlayTex );

	//void UpdateSkinningMethod ( LPD3DXFRAME pFrameBase );
	void UpdateFrameMatrices ( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix );

protected:
	HRESULT RenderDefault ( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
							SMATERIAL_PIECE *pmtrlPiece, DXMATERIAL_CHAR_EFF* pmtrlSpecular, LPDIRECT3DTEXTUREQ* apOverlayTex );

protected:
	BOOL CheckLoadTexture( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pMeshContainer, DWORD dwThisAttribId );
//
//public:
//	HRESULT OnNewSkinnedMethod ( METHOD NewSkinningMethod );

public:
	void Init_Detail_Texture ();
	void SetDrawState ( BOOL bOriginDraw=TRUE, BOOL bAlpha=FALSE, BOOL bShadow=FALSE, BOOL bAlphaTex=TRUE, BOOL bLevel=FALSE, float fHeight=0.f );

public:
	SMeshContainer* FindMeshContainer ( LPD3DXFRAME pFrame, const char *szName );

	SFrame* GetFrameRoot ()					{ return (SFrame*)m_pFrameRoot; }
	const TCHAR* GetSkinFileName ()			{ return m_strName_SKIN.c_str(); }
	const TCHAR* GetBoneFileName ()			{ return m_strName_BONE.c_str(); }

	D3DXVECTOR3 GetCenter ()				{ return m_vObjectCenter; }
	float GetRadius ()						{ return m_fObjectRadius; }

	SMeshContainer* GetMeshClobalList ()	{ return (SMeshContainer*)m_pMeshContainerHead; }

public:
	void AddMeshGlobal ( SMeshContainer *pmc );

public:
	SFrame* FindFrame ( char *szName );
	SFrame* FindParentFrame ( SFrame* pFrame );
	SMeshContainer* FindMeshContainer ( const char *szName )		{ return FindMeshContainer(m_pFrameRoot,szName); }
	HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer* pmcMesh, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
    void SetFile( const TCHAR* szSkinFile, const TCHAR *szSkeletonFile );
    //HRESULT OnCreateSkin ( IDirect3DDevice9* pd3dDevice, BOOL bThread );
	//HRESULT OnCreateSkin ( IDirect3DDevice9* pd3dDevice, const char* szSkinFile, const char *szSkeletonFile, BOOL bThread );

	HRESULT SetSkin_Bone(DxSkeletonResource skeletonRes);
    HRESULT OnCreateSkin_Bone( IDirect3DDevice9* pd3dDevice);//, BOOL bThread );	// 순차	
	HRESULT OnCreateSkin_Memory( IDirect3DDevice9* pd3dDevice,const SP_RAW_DATA& res , UINT size);	// Thread
	HRESULT OnCreateSkin_Skin( IDirect3DDevice9* pd3dDevice, BOOL bThreadTEX );	// 순차
	HRESULT OnCreateSkin_Final( IDirect3DDevice9* pd3dDevice, BOOL bSleep );	// Thread

    BOOL IsLoadEnd_Memory();
	BOOL IsLoadEnd_Final();
	BOOL IsLoadEnd_Error();

	//void FileReleaseMemory();

public:
	void CreateCartoonMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness );

public:
	static HRESULT StaticCreate (IDirect3DDevice9* pd3dDevice );
	static HRESULT StaticResetDevice ( IDirect3DDevice9* pd3dDevice );
	static void StaticLost ();
	static void StaticDestroy ();

public:
	void OnSkinDestroyDevice ();
	void OnSkinCleanUp ();

public:
	HRESULT OnSkinFrameMove ( double fTime, float fElapsedTime, D3DXMATRIXA16 &matWorld );
	//void OnSkinFrameRender ( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );

public:
	void SaveNORMAL( TCHAR* pName );
	void SaveHLSL( TCHAR* pName );
	void SaveNormalMesh( TCHAR* pName );
	void SaveLOD_SW( TCHAR* pName );
	virtual void Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice ) = 0;

private:
	void SaveNORMAL( sc::SerialFile& SFile );
	void SaveHLSL( sc::SerialFile& SFile );
	void SaveNormalMesh( sc::SerialFile& SFile );
	void SaveLOD_SW( sc::SerialFile& SFile );
	virtual void Load( sc::SerialFile& SFile, IDirect3DDevice9 *pd3dDevice ) = 0;
	void LoadFile( void * param1 );
};

//--------------------------------------------------------------------------------------
// Name: class CAllocateHierarchy
// Desc: Custom version of ID3DXAllocateHierarchy with custom methods to create
//       frames and meshcontainers.
//--------------------------------------------------------------------------------------
class CAllocateHierarchy: public ID3DXAllocateHierarchy
{
private:
	TSTRING			m_strSkinFile;
	BOOL			m_bThread;
	DxSkinMesh9*	m_pSkinMesh;

public:
    STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
    STDMETHOD(CreateMeshContainer)(THIS_ 
        LPCSTR Name, 
        CONST D3DXMESHDATA *pMeshData,
        CONST D3DXMATERIAL *pMaterials, 
        CONST D3DXEFFECTINSTANCE *pEffectInstances, 
        DWORD NumMaterials, 
        CONST DWORD *pAdjacency, 
        LPD3DXSKININFO pSkinInfo, 
        LPD3DXMESHCONTAINER *ppNewMeshContainer);
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

	CAllocateHierarchy ( DxSkinMesh9* pSkinMesh, const TSTRING& strSkinFile, BOOL bThread )
        : m_pSkinMesh( pSkinMesh )
        , m_bThread( bThread )
		, m_strSkinFile( strSkinFile )
	{}
};