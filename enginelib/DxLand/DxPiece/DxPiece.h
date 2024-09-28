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
	//DxPieceEdit*		m_rPieceEdit;		// RefCount�� 1���� ������ Pointer ������� �ߺ������� ����. Container���� ����
	DxStaticAniFrame*	m_rStaticAniFrame;	// RefCount�� 1���� ������ Pointer ������� �ߺ������� ����. Container���� ����
	DxPieceEff*			m_pPieceEff;		// RefCount ����. ������ ����. Ȯ���� ������� ��.
	DxPieceTexEff*		m_pAniTexEff;		// Animation�� Texture Effect. ������ ����. Ȯ���� ������� ��.

	// Animation ����� ���� �ϱ� ���� ��.
	std::vector<D3DXMATRIX>		m_vecAnimationMatrixBackUp;		// ������ ���� Animation�� ���� Matrix �� ������ �ִ´�. ���� �����ӿ��� �׸��ڸ� ���� �ٽ� ����� �ʿ䰡 ����.


    // LightMap ���� ������
private:
    VEC_SVEC_LIGHTMAPOBJ*   m_pvecsvecLightMapObj;		// LightMap ������ ���� �ӽ� ��ü
    WORDWORD                m_wwStartTexel;				// Save,Load �ʿ�   ���� Texel
    WORDWORD                m_wwTextureTexel;			// Save,Load �ʿ�   Width, Height ( Texture )
	D3DXVECTOR2				m_vLightMapUV_Rate;			// Save,Load �ʿ�
    TSTRING                 m_strTextureCombined;		// Save,Load �ʿ�
    TSTRING                 m_strTextureDirect_Shadow_1;// Save,Load �ʿ�
	TSTRING                 m_strTextureDirect_Shadow_2;// Save,Load �ʿ�
	TSTRING                 m_strTextureIndirect;		// Save,Load �ʿ�
    TextureResource			m_textureResCombined;
    TextureResource			m_textureResDirect_Shadow_1;
	TextureResource			m_textureResDirect_Shadow_2;
	TextureResource			m_textureResIndirect;

	D3DXVECTOR4					m_vVoxelColorUsed;				// ���Ǵ� VoxelColor
	D3DXVECTOR4					m_avAmbientCube[LightMapBoxColor::TEST_COUNT];
	LightMapBoxColor*			m_pLightMapBoxColor;			// ����Ʈ�� or Radiosity�� ���� �� ��� ���̰� �����.
	LightMapBoxColorFullData*	m_pLightMapBoxColorFullData;	// ���� ������ ������ ����.

public:
	const TSTRING&	GetLightMapCombined() const	{ return m_strTextureCombined; }


	// LOD ���� ������
private:
	float               m_fVisibleRadius;   // Piece�� alpha�� 1�� �Ǵ� �ݰ�.
	float               m_fInvisibleRadius; // Piece�� alpha�� 0�� �Ǵ� �ݰ�.

public:
	virtual float	GetVisibleRadius()    { return m_fVisibleRadius; }
	virtual float   GetInvisibleRadius()  { return m_fInvisibleRadius; }


	// Instancing ����
private:
	BOOL		m_bExceptLowOption;	// ����翡���� �������� ���� �ʴ´�.

public:
	BOOL IsExceptLowOption() const	{ return m_bExceptLowOption; }


public:
	DWORD		m_dwID;				// Load �� ��� ������ ���Ͽ� ���δ�.
	TSTRING		m_strName;			// Load �� ��� ������ �Ѵ�.
	D3DXVECTOR3	m_vStaticMax;	    // StaticMesh ���� ����. < ĳ���͸� ������츦 üũ�ϱ� ���� >
	D3DXVECTOR3	m_vStaticMin;	    // StaticMesh ���� ����. < ĳ���͸� ������츦 üũ�ϱ� ���� >
	D3DXVECTOR3 m_vCenter;		    // ��ü�� ����. 						< 1�� üũ�� ���� >
    float       m_fRadius;          // �������� ���� �׷��� ����� ������.	< 1�� üũ�� ���� >
	float		m_fRadiusSquare;	// ��ü�� ������ ����
    DWORD		m_dwFlags;		    // PIECE_OPTION

public:
	BOOL		m_bAddAlpha;	// Visible �ȿ� ���°�~?
	BOOL		m_bBlendAlpha;	// LOD �� Blend �� ó���� �Ǵ� ���ΰ�~? �ƴϸ� ���������� �Ǵ°� �ΰ�~?

	// DxOcclusionQuery
private:
	LPDIRECT3DQUERY9	m_pd3dQuery;
	BOOL				m_bQueryTest;

private:
	VEC_DWORD	m_vecPointIndex;				// �ڽſ��� ������ �ִ� Point Light Index

	// TriangleLinkLight
private:
	TriangleLinkLightForPiece	m_sTriangleLinkLight;

private:
	PieceParameter	m_sParameter;

public:
	D3DXVECTOR3	m_vMax;			// ��ü�� Max
	D3DXVECTOR3	m_vMin;			// ��ü�� Min
	D3DXMATRIX	m_matWorldOrig;
	D3DXMATRIX	m_matWorld;
	D3DXMATRIX	m_matWorld_Mirror;
    D3DXMATRIX	m_matInverse;   

	DxPiece*	m_pNext;		// List
	DxPiece*	m_pTreeNext;	// Tree �ȿ����� List �̴�.
	DxPiece*	m_pSortNext;	// Quick Sort �� ���� List �̴�.

    DxPiece*	m_rPiecePSF;	// PSF �� Piece �̴�.   - ���Ǳ�� ����ؾ� �Ѵ�.

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
	BOOL IS_PS_3_0_TYPE_ZERO_OR_CSM() const;	// Spot Light �� ������ �� �޴°�~?

public:
	// ������ DF �� �����ȴٸ� �׿� �´� �ɼǰ����� ��ȭ��Ų��.
	void SetData_Force_TnL_PS_3_0_DF() override;

	// Tool ���� ���
	virtual void FrameMoveTOOL( const float fTime, const float fElapsedTime );

	// Game���� ���., Only �� FrameMoveMAT ���� �κкκ� �и��Ѱ�.
	void FrameMoveMAT( const float fTime, const float fElapsedTime ) override;
	void FrameMoveMAT_OnlyStaticAniFrame( float fElapsedTime ) override;
	void FrameMoveMAT_OnlyPieceEff( const float fTime, const float fElapsedTime ) override;

	// Thread �󿡼� ���ư�.
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

    // Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
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

	// LightMap UV �� ����Ʈ ������ ���� ���� LightMap Texture �� ���� Size �� ������
	void SetLightMapUV_Rate();

	// TriangleLinkLight
public:
	// Piece�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
	void CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager,
									BOOL bDirectionLight );

	//  ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� �ڸ��� ������ Ȯ��.
	BOOL IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const;

	// ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� ���� LightID �� ����.
	void InsertSlotLightID( DWORD dwLightID, DWORD dwSlot );

	// �� �ﰢ������ Light�� 3�� �̻� ���� �ִ� ���� ã�´�.
	BOOL FindTriangleLimitLightNumOver3( DWORD& dwLightID ) const;

	// Ư�� Light�� �ٿ��ֵ��� �Ѵ�.
	void EraseLightID( DWORD dwLightID );

	// Piece �� PS_3_0�� ����Ÿ���� �����ϵ��� �Ѵ�. < �������� �� >
	void SetType_PS_3_0();

	// Piece �� PS_3_0�� ����Ÿ���� �����ϵ��� �Ѵ�. < ���� ������ ���Ǵ� �� >
	//void SetType_PS_3_0_USED();

public:
	void Save( sc::SerialFile& SFile );	// Tree ������ Save Load
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
				BOOL bShader_3_0 );	                    // DxPieceEdit���� ���� ������ �� ȣ��

	void CloneData( LPDIRECT3DDEVICEQ pd3dDevice, 
                    DxPiece* pSrc, 
                    BOOL bLightMap, 
                    const TSTRING& strLightMapName, 
                    DxStaticMesh* pStaticMesh, 
                    DxPieceManagerPSF* pPieceManager,
                    DWORD& dwStartTextureSize,
                    BOOL& bEnoughUsedPixelLightMap );   // DxPiece �༮�� ������ ����
	void CleanUp();

	void CloneData( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pSrc );		// DxPiece �༮�� ������ ����	< SpeedPiece�۾��� ���� ������ >

	void ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece );

	BOOL IsSupportRAD();

	void CalculateSlopeMatrix( DxSetLandMan* pSetLandMan, float fRate );	// fRate ( ���� 1<->0 ���ʹ��� )

private:
	void CheckPointLight();									            // �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.	< �⺻������ ���� �ε��Ǿ� �־�� �� >
	void RenderPointLight( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF );	// Point Light�� �����Ų��.

public:
	// Material �� ���Ѵ�.
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
