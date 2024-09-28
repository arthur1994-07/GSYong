#pragma once

#include "../../DxTools/Collision/CollisionDef.h"
#include "../../DxTools/Material/MaterialDef.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/TextureManager.h"
#include "../LightMap/LightMapDef.h"
#include "../LightMap/LightMapBoxColor.h"
#include "../NSLandThreadDef.h"
#include "../Entity.h"
#include "./DxPieceDef.h"

class DxPieceEdit;
class DxPieceEff;
class DxStaticAniFrame;
class DxPieceTexEff;
class DxPieceManager;
class DxPieceManagerPSF;
class DxStaticMesh;
class DxStaticMeshLightMap;
class DxFrameMesh;
class LightMapBoxColor;
class DxSetLandMan;
class DxLandMan;
class DxStaticMeshCollBase;
class DxPieceInstancing;
class DxPieceInstancingForShadow;
struct DxPieceNode;
struct CLIPVOLUME;

template <typename T>
class PieceInstancingSwap;

namespace sc
{	
    class SerialFile;
};

//----------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e
//	Note : 
//----------------------------------------------------------------------------------------------------------------------
class DxPiece : public Entity
{
private:
    enum
    {
        VERSION = 0x110,
    };

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	float		m_fTime;
	float		m_fAniTime;
	float		m_ElapsedTime;

private:
	//DxPieceEdit*		m_rPieceEdit;		// RefCount로 1개만 생성후 Pointer 사용으로 중복생성을 안함. Container에서 관리
	DxStaticAniFrame*	m_rStaticAniFrame;	// RefCount로 1개만 생성후 Pointer 사용으로 중복생성을 안함. Container에서 관리
	DxPieceEff*			m_pPieceEff;		// RefCount 없음. 여러개 생성. 확실히 지워줘야 함.
	DxPieceTexEff*		m_pAniTexEff;		// Animation용 Texture Effect. 여러개 생성. 확실히 지워줘야 함.

	// Animation 계산을 적게 하기 위한 것.
	std::vector<D3DXMATRIX>		m_vecAnimationMatrixBackUp;		// 이전에 계산된 Animation을 위한 Matrix 를 가지고 있는다. 같은 프레임에서 그림자를 위해 다시 계산할 필요가 없다.


    // LightMap 관련 데이터
private:
    VEC_SVEC_LIGHTMAPOBJ*   m_pvecsvecLightMapObj;		// LightMap 생성을 위한 임시 객체
    WORDWORD                m_wwStartTexel;				// Save,Load 필요   시작 Texel
    WORDWORD                m_wwTextureTexel;			// Save,Load 필요   Width, Height ( Texture )
	D3DXVECTOR2				m_vLightMapUV_Rate;			// Save,Load 필요
    TSTRING                 m_strTextureCombined;		// Save,Load 필요
    TSTRING                 m_strTextureDirect_Shadow_1;// Save,Load 필요
	TSTRING                 m_strTextureDirect_Shadow_2;// Save,Load 필요
	TSTRING                 m_strTextureIndirect;		// Save,Load 필요
    TextureResource			m_textureResCombined;
    TextureResource			m_textureResDirect_Shadow_1;
	TextureResource			m_textureResDirect_Shadow_2;
	TextureResource			m_textureResIndirect;

	D3DXVECTOR4					m_vVoxelColorUsed;				// 사용되는 VoxelColor
	D3DXVECTOR4					m_avAmbientCube[LightMapBoxColor::TEST_COUNT];
	LightMapBoxColor*			m_pLightMapBoxColor;			// 라이트맵 or Radiosity를 만들 때 잠시 쓰이고 지운다.
	LightMapBoxColorFullData*	m_pLightMapBoxColorFullData;	// 실제 정보를 가지고 있음.

public:
	const TSTRING&	GetLightMapCombined() const	{ return m_strTextureCombined; }


	// LOD 관련 데이터
private:
	float               m_fVisibleRadius;   // Piece의 alpha가 1이 되는 반경.
	float               m_fInvisibleRadius; // Piece의 alpha가 0이 되는 반경.

public:
	virtual float	GetVisibleRadius()    { return m_fVisibleRadius; }
	virtual float   GetInvisibleRadius()  { return m_fInvisibleRadius; }


	// Instancing 관련
private:
	BOOL		m_bExceptLowOption;	// 저사양에서는 렌더링을 하지 않는다.

public:
	BOOL IsExceptLowOption() const	{ return m_bExceptLowOption; }


public:
	DWORD		m_dwID;				// Load 할 경우 참조를 위하여 쓰인다.
	TSTRING		m_strName;			// Load 할 경우 쓰여야 한다.
	D3DXVECTOR3	m_vStaticMax;	    // StaticMesh 만의 한정. < 캐릭터를 가릴경우를 체크하기 위함 >
	D3DXVECTOR3	m_vStaticMin;	    // StaticMesh 만의 한정. < 캐릭터를 가릴경우를 체크하기 위함 >
	D3DXVECTOR3 m_vCenter;		    // 물체의 중점. 						< 1차 체크를 위함 >
    float       m_fRadius;          // 중점에서 원을 그렸을 경우의 반지름.	< 1차 체크를 위함 >
	float		m_fRadiusSquare;	// 물체의 반지름 제곱
    DWORD		m_dwFlags;		    // PIECE_OPTION

public:
	BOOL		m_bAddAlpha;	// Visible 안에 들어갔는가~?
	BOOL		m_bBlendAlpha;	// LOD 가 Blend 로 처리가 되는 것인가~? 아니면 점묘법으로 되는것 인가~?

	// DxOcclusionQuery
private:
	LPDIRECT3DQUERY9	m_pd3dQuery;
	BOOL				m_bQueryTest;

private:
	VEC_DWORD	m_vecPointIndex;				// 자신에게 영향을 주는 Point Light Index

	// TriangleLinkLight
private:
	TriangleLinkLightForPiece	m_sTriangleLinkLight;

private:
	PieceParameter	m_sParameter;

public:
	D3DXVECTOR3	m_vMax;			// 전체의 Max
	D3DXVECTOR3	m_vMin;			// 전체의 Min
	D3DXMATRIX	m_matWorldOrig;
	D3DXMATRIX	m_matWorld;
	D3DXMATRIX	m_matWorld_Mirror;
    D3DXMATRIX	m_matInverse;   

	DxPiece*	m_pNext;		// List
	DxPiece*	m_pTreeNext;	// Tree 안에서의 List 이다.
	DxPiece*	m_pSortNext;	// Quick Sort 할 때의 List 이다.

    DxPiece*	m_rPiecePSF;	// PSF 용 Piece 이다.   - 주의깊게 사용해야 한다.

	// friend class ~
public:
	friend class CPiecePointEdit;

	// DxOcclusionQuery
public:
	VOID CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID DestroyQuery();
	HRESULT GetDataQuery( DWORD& dwCullPixels );
	void ResetOcclusionQuery();
	BOOL VisibleOcclusionQueryTest( LPDIRECT3DDEVICEQ pd3dDevice, DWORD& dwCullPixels );

	// TriangleLinkLight
public:
	BOOL IS_PS_3_0_TYPE_ZERO_OR_CSM() const;	// Spot Light 에 영향을 안 받는가~?

public:
	// 강제로 DF 로 렌더된다면 그에 맞는 옵션값들을 변화시킨다.
	void SetData_Force_TnL_PS_3_0_DF() override;

	// Tool 에서 사용
	virtual void FrameMoveTOOL( const float fTime, const float fElapsedTime );

	// Game에서 사용., Only 는 FrameMoveMAT 에서 부분부분 분리한것.
	void FrameMoveMAT( const float fTime, const float fElapsedTime ) override;
	void FrameMoveMAT_OnlyStaticAniFrame( float fElapsedTime ) override;
	void FrameMoveMAT_OnlyPieceEff( const float fTime, const float fElapsedTime ) override;

	// Thread 상에서 돌아감.
	virtual void FrameMoveTHREAD( const float fElapsedTime );

	//void FrameMoveStep( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );
	//void FrameMove_Instancing( float fElapsedTime );
	virtual void Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );
	virtual void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );
	virtual void RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );
	void RenderMS_Opaque( BOOL bMPShading, BOOL bReflect, float fHeight, BOOL bBlend ) override;
	void RenderMS_Opaque_Animation( BOOL bMPShading, BOOL bReflect, float fHeight, BOOL bBlend ) override;
	virtual void RenderMS_Fixed();
	void RenderMS_HardAlpha( BOOL bMPShading, BOOL bBlend );
	void RenderMS_HardAlpha_Animation( BOOL bMPShading, BOOL bBlend );
	virtual void RenderEffFrame();
	virtual void RenderEffFrameForDeffered( BOOL bLBuffer, BOOL bReflect );
	virtual void RenderDecal();
	virtual void RenderMS_SoftAlpha( BOOL bMPShading );
	virtual void RenderMS_SoftAlphaForDeffered();
	virtual void RenderMS_SoftAlphaForPSF();
	virtual void RenderMS_DecalPass2();
	virtual void Render_PIECE_Custom_UNLIT_UNFOG();
	virtual void Render_PIECE_Water_CustomSoftAlpha();
	virtual void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF );

	void RenderEff_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap );

	void RenderCascadeShadowOpaque() override;
	void RenderCascadeShadowHardAlpha() override;
	void RenderCascadeShadowReplace() override;

	void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );
	virtual void Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	virtual void Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void Render_Query2( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );

    BOOL		GetOnlyAmbient() const		{ return m_dwFlags&EMPO_ONLY_AMBIENT; }
    BOOL		GetTalkAlphaBlent() const	{ return m_dwFlags&EMPO_TALK_ALPHA_BLEND; }
	BOOL		GetMousePicking() const		{ return m_dwFlags&EMPO_MOUSE_PICKING; }
	BOOL		GetShadowRender() const		{ return m_dwFlags&EMPO_SHADOW_RENDER; }
	BOOL		GetFarRender() const		{ return m_dwFlags&EMPO_FAR_RENDER; }

	DxStaticAniFrame* GetStaticAniFrame() const		{ return m_rStaticAniFrame; }
	const PieceParameter* GetPieceParameter() const	{ return &m_sParameter; }
	PieceParameter* GetPieceParameterNoneConst()	{ return &m_sParameter; }

	// Edit
	DWORD GetID()					{ return m_dwID; }
	void SetID( DWORD dwID )		{ m_dwID = dwID; }
	void SetAABBBox();
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3* pNor, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

    // Note : 충돌이 되면 바로 리턴한다.
    BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor );

    void ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

    void CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName );
	void CreateLightMapPieceObject( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, BOOL bRan_1_0, float fCellSize );
	void LightMapCalculationBoxColor( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshCollBase* pStaticMesh, DxPieceManager* pPieceManager );
    void LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice );
	void LoadLightMapForRAD( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReleaseLightMap();
	void ResetAnimationTime_DisableLoopType();
    void SetLightMapWidthHeight( WORDWORD wwStartTexel, WORDWORD wwTextureTexel );
    void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice );

    void GetClosedPoint( D3DXVECTOR3& vCollisionInOut );

	void SetPrefabMatrix( const D3DXMATRIX& matPrefab );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot );

	// Radiosity
	void BakeRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxLandMan* pLandMan, 
						LPDIRECT3DTEXTUREQ pTextureSystem, 
						float fReflectance, 
						float fFogBlend,
						DWORD dwFogLength,
						DWORD dwRadiosityIndex );

	void AddRadiosityColorForVoxel( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, LPDIRECT3DTEXTUREQ pTextureSystem, float fReflectance, float fFogBlend, DWORD dwFogLength );

	virtual BOOL IsVoxelColor() const;
	virtual BOOL IsMaterialMesh() const;

	virtual void				SetBlendAlpha( BOOL bBlend )	{ m_bBlendAlpha = bBlend; }
	virtual BOOL				IsBlendAlpha() const			{ return m_bBlendAlpha; }
	virtual BOOL				GetPickAlpha() const			{ return m_dwFlags&EMPO_PICK_ALPHA; }
	virtual float				GetRadius() const				{ return m_fRadius; }
	virtual const D3DXVECTOR3&	GetCenter() const				{ return m_vCenter; }
	virtual const D3DXVECTOR3&	GetStaticMax() const			{ return m_vStaticMax; }
	virtual const D3DXVECTOR3&	GetStaticMin() const			{ return m_vStaticMin; }

	virtual void SetAddAlpha( float fTime, BOOL bAddAlpha );
	virtual BOOL IsAddAlpha()					{ return m_bAddAlpha; }

	virtual BOOL CheckOcclusionCulling( const VEC_DWORD* pvecDWORD ) const;

	// LightMap UV 를 디폴트 값에서 현재 맵의 LightMap Texture 에 맞춘 Size 로 변경함
	void SetLightMapUV_Rate();

	// TriangleLinkLight
public:
	// Piece의 TriangleLinkLight 를 생성하도록 한다.
	void CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager,
									BOOL bDirectionLight );

	//  현재 라이트와 연관된 삼각형들의 특정 슬롯에 자리가 남는지 확인.
	BOOL IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const;

	// 현재 라이트와 연관된 삼각형들의 특정 슬롯에 현재 LightID 를 셋팅.
	void InsertSlotLightID( DWORD dwLightID, DWORD dwSlot );

	// 한 삼각형에서 Light를 3개 이상 쓰고 있는 것을 찾는다.
	BOOL FindTriangleLimitLightNumOver3( DWORD& dwLightID ) const;

	// 특정 Light를 줄여주도록 한다.
	void EraseLightID( DWORD dwLightID );

	// Piece 의 PS_3_0의 렌더타입을 설정하도록 한다. < 구조적인 것 >
	void SetType_PS_3_0();

	// Piece 의 PS_3_0의 렌더타입을 설정하도록 한다. < 실제 렌더시 사용되는 것 >
	//void SetType_PS_3_0_USED();

public:
	void Save( sc::SerialFile& SFile );	// Tree 에서의 Save Load
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DWORD dwID, BOOL bWLD );
    void Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxPieceEdit* pSrc );
	void Load_PREV2( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxPieceEdit* pSrc );

	void LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Import( LPDIRECT3DDEVICEQ pd3dDevice, 
				DxPieceEdit* pSrc, 
				const D3DXMATRIX& matLocal, 
				const D3DXMATRIX& matFrame,
				float fVisibleRadius,
				float fInvisibleRadius,
				BOOL bShader_3_0 );	                    // DxPieceEdit에서 값을 가져올 때 호출

	void CloneData( LPDIRECT3DDEVICEQ pd3dDevice, 
                    DxPiece* pSrc, 
                    BOOL bLightMap, 
                    const TSTRING& strLightMapName, 
                    DxStaticMesh* pStaticMesh, 
                    DxPieceManagerPSF* pPieceManager,
                    DWORD& dwStartTextureSize,
                    BOOL& bEnoughUsedPixelLightMap );   // DxPiece 녀석들 끼리의 복제
	void CleanUp();

	void CloneData( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pSrc );		// DxPiece 녀석들 끼리의 복제	< SpeedPiece작업후 삭제 데이터 >

	void ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece );

	BOOL IsSupportRAD();

	void CalculateSlopeMatrix( DxSetLandMan* pSetLandMan, float fRate );	// fRate ( 기울기 1<->0 위쪽방향 )

private:
	void CheckPointLight();									            // 자신과 맞닿아 있는 빛을 체크를 한다.	< 기본적으로 빛은 로딩되어 있어야 함 >
	void RenderPointLight( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );	// Point Light를 적용시킨다.

public:
	// Material 을 구한다.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );


	//////////////////////////////////////////////////////////////////////////
	//							Instancing
private:
	PieceInstancingSwap<DxPieceInstancing>*				m_rPieceInstancingSwap[NSLandThread::TYPE_INSTANCING_ALL_SIZE];
	PieceInstancingSwap<DxPieceInstancingForShadow>*	m_rPieceInstancingSwapForShadow[NSLandThread::TYPE_INSTANCING_ALL_SIZE];

public:
	void ResetInstancingLink();
	void SetInstancingLink( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, PieceInstancingSwap<DxPieceInstancing>* pInstancing );
	void SetInstancingLink( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, PieceInstancingSwap<DxPieceInstancingForShadow>* pInstancing );
	PieceInstancingSwap<DxPieceInstancing>*				GetInstancingLink( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType ) const;
	PieceInstancingSwap<DxPieceInstancingForShadow>*	GetInstancingLinkForShadow( NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType ) const;
	BOOL IsInstancingDynamicOption() const;



	//////////////////////////////////////////////////////////////////////////
	const D3DXVECTOR4* GetAmbientCube_LightMapBoxColorFullData() const;


public:
	DxPiece( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual ~DxPiece();
};

typedef DxPiece*		PDXPIECE;
