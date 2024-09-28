#pragma once

#include "../../DxEffect/Frame/DxEffectFrame.h"
#include "../../DxEffect/Mesh/DxStaticMeshBase.h"

class DxAnimationMan;
class DxFrameAniMan;
class DxFrameMesh;
class DxStaticMesh;
class DxStaticMeshBase;
class DxPieceManager;
class DxPieceManagerPSF;
class DxStaticMaterialMeshAniMan;
class DxStaticMaterialMeshFrameAni;
struct PieceParameter;

namespace NSTexture16Bit
{
	struct DATA;
};

//----------------------------------------------------------------------------------------------------------------------
//								D	X		R	E	P	L	A	C	E	P	I	E	C	E		E	X
//----------------------------------------------------------------------------------------------------------------------
class DxStaticAniFrame
{
private:
    enum
    {
        VERSION = 0x0104,
        VERSION_LM = 0x0101,
    };

protected:
	BOOL			m_bFrameMoveLock;
	DWORD			m_dwTexType;			// Opaque, HardAlpha, SoftAlpha �� �ִ��� Ȯ���ϴ� �ڵ�. [TEXTURE_TYPE_FLAG]

protected:
	DWORD			m_dwEffectCount;
	PDXEFFECTBASE	m_pEffectHead;

protected:
	DxFrameAniMan*		m_pFrameAniMan;		// ����
	DxStaticMeshBase*	m_pStaticMesh;		// ����

	std::tr1::shared_ptr<DxStaticMaterialMeshAniMan>	m_spStaticMaterialMeshAniMan;
	std::tr1::shared_ptr<DxStaticMaterialMeshFrameAni>	m_spStaticMaterialMeshFrameAni;



	//////////////////////////////////////////////////////////////////////////
	//	Animation �� Pointer �� ������ �־ �ٸ������� ������ �� �ֵ��� ��.
private:
	std::vector<D3DXMATRIX*>	m_vecAnimationMatrixPointer;	// Animation�� ���� Matrix �� Pointer�� ������ �ִ´�.

public:
	const std::vector<D3DXMATRIX*>& GetAnimationMatrixPointer() const;
	void SetAnimationMatrix( const std::vector<D3DXMATRIX>& vecAnimationMatrix );


public:
	// Material �� ���Ѵ�.
	void Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool );


protected:
	int					m_nRefCount;
	DxStaticAniFrame*	m_pNext;

public:
    DxFrameAniMan*	GetFrameAniMan() { return m_pFrameAniMan; }
	DxFrame*		GetAniFrame( const char* szName );
	DxAnimationMan*	GetAniManHead();
	DxStaticMesh*	GetStaticMesh();

public:
	void SetTime( const float fAniTime );

	void MakeAniMan( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh );

	void MergeLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
							const TSTRING& strLightMapName, 
							DxStaticMesh* pStaticMesh, 
							DxPieceManagerPSF* pPieceManager,
							const D3DXMATRIX& matWorld,
							VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
							DWORD& dwStartTextureSize,
							WORDWORD& wwBaseTexelXY,
							BOOL& bEnoughUsedPixelLightMap,
							const WORDWORD& wwLightID4Shadow );

	void CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
								const TSTRING& strLightMapName,
								TSTRING& strTextureCombined, 
								TSTRING& strTextureDirect_Shadow_1,
								TSTRING& strTextureDirect_Shadow_2,
								TSTRING& strTextureIndirect,
								VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
								WORDWORD wwBaseTexelXY );

	void CreateLightMapUVPieceObject( LPDIRECT3DDEVICEQ pd3dDevice, 
										const TSTRING& strLightMapName,
										TSTRING& strTextureCombined, 
										TSTRING& strTextureDirect_Shadow_1,
										TSTRING& strTextureDirect_Shadow_2,
										TSTRING& strTextureIndirect,
										VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										WORDWORD wwBaseTexelXY,
										BOOL bRan_1_0, 
										float fCellSize );

	void SetLightMapUV_Rate( const D3DXVECTOR2& vLightMapUV_Rate );

    void LoadLightMapOLD( LPDIRECT3DDEVICEQ pd3dDevice );

    void FlipLightMapRenderMan( DxStaticAniFrame* pStaticAniFrame );

    void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL IsMaterialMesh() const;

	void BakeRadiosityForPiece( LPDIRECT3DDEVICEQ pd3dDevice,
								DxLandMan* pLandMan,
								VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
								const D3DXMATRIX& matWorld, 
								LPDIRECT3DTEXTUREQ pTextureSystem,
								float fReflectance, 
								float fFogBlend,
								DWORD dwFogLength,
								DWORD dwRadiosityIndex );

	BOOL CheckOnlyVoxel();

	// TriangleLinkLight
public:
	// Piece�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
	BOOL CreateTriangleLinkLightPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxStaticMeshCollBase* pStaticMeshColl, 
										DxPieceManager* pPieceManager, 
										std::vector<DWORD>& vecLightID, 
										const D3DXMATRIX& matWorld,
										const D3DLIGHTQ& sLight,
										float fAttEndLineValue ) const;

protected:
	HRESULT EffectAdaptToList( DxFrame *pFrameRoot, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
									PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc, const CMaterialSetting* pMaterialSetting );
	HRESULT EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, PDXEFFECTBASE &pEffList );

	// Edit
public:
	void GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3&  vMin );
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3* pNor, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const;

	BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const;

    // Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
    BOOL IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV );

	BOOL GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor, const NSTexture16Bit::DATA* pDataDay );

    void GetClosedPoint( D3DXVECTOR3& vCollisionInOut );

	void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld );

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

public:
	BOOL IsTexType( DWORD dwTexType ) const	{ return (m_dwTexType & dwTexType) ? TRUE : FALSE; }

	void FrameMoveTOOL( LPDIRECT3DDEVICEQ pd3dDevice, const float fElapsedTime );
	void FrameMoveMAT( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWLD );

	void Render( LPDIRECT3DDEVICEQ pd3dDevice, 
                  const D3DXMATRIX& matWorld, 
                  const float fAniTime, 
                  DxTextureEffMan::MAPTEXEFF* pmapTexEff,
                  const D3DXMATRIX& matTexScale,
                  float fAlpha,
				  int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
				  LPDIRECT3DTEXTUREQ pTextureDay, 
				  LPDIRECT3DTEXTUREQ pTextureNight,
				  BOOL bMPShading );

	void Render_HardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
							const D3DXMATRIX& matWorld, 
							DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
							const D3DXMATRIX& matTexScale,
							float fAlpha,
							int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
							LPDIRECT3DTEXTUREQ pTextureDay, 
							LPDIRECT3DTEXTUREQ pTextureNight,
							BOOL bMPShading );

	void Render_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
                        const D3DXMATRIX& matWorld, 
                        DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
                        const D3DXMATRIX& matTexScale,
						float fAlpha,
						int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
						LPDIRECT3DTEXTUREQ pTextureDay, 
						LPDIRECT3DTEXTUREQ pTextureNight,
						BOOL bMPShading );

	//void RenderOrderSettingPiece( PieceParameter& sParameter, BOOL bMPShading );
	void RenderMS_Opaque( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend );
	void RenderMS_Opaque_Animation( PieceParameter& sParameter, BOOL bMPShading, BOOL bReflect, float fHeight, BOOL bBlend, bool bAniLoop=true );
	void RenderMS_HardAlpha( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend );
	void RenderMS_HardAlpha_Animation( PieceParameter& sParameter, BOOL bMPShading, BOOL bBlend );
	void RenderDecal( PieceParameter& sParameter );
	void RenderMS_SoftAlpha( PieceParameter& sParameter, BOOL bMPShading );
	void RenderMS_SoftAlphaForPSF( PieceParameter& sParameter );
	void RenderMS_DecalPass2( PieceParameter& sParameter );
	void Render_PIECE_Custom_UNLIT_UNFOG( PieceParameter& sParameter );		// ���� Water_CustomSoftAlpha�� Instancing ���� �ϰ� �ʹٸ� �۾��� �ʿ��ϴ�.
	void Render_PIECE_Water_CustomSoftAlpha( PieceParameter& sParameter );		// ���� Water_CustomSoftAlpha�� Instancing ���� �ϰ� �ʹٸ� �۾��� �ʿ��ϴ�.

	void RenderOpaque_Instancing( const PieceParameter& sParameter );
	void RenderHardAlpha_Instancing( const PieceParameter& sParameter );
	void RenderDecal_Instancing( const PieceParameter& sParameter );
	void RenderDecalPass2_Instancing( const PieceParameter& sParameter );

	void Render_CharShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV );

	void RenderCascadeShadowOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowOpaque_Animation( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderCascadeShadowHardAlpha_Animation( LPDIRECT3DDEVICEQ pd3dDevice );

	void Render_EDIT( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld );

	void Render_Query( PieceParameter& sParameter );

public:
	void Save( sc::SerialFile& SFile );	// Note : Save, Load �� DxPieceEdit ������ ���δ�. 
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, boost::function<void (const TCHAR*)> pArchive );
	void Export( LPDIRECT3DDEVICEQ pd3dDevice, 
				DxFrameMesh* pFrameMesh, 
				LIGHTMAPUV_MAIN* pLightMapUV_MAIN, 
				const DxTextureEffMan* pTextureEffMan, 
				const CMaterialSetting* pMaterialSetting, 
				BOOL bRan_1_0,
				float fCellSize );

    BOOL IsLightMapPiece();
    void SaveLightMapPiece( sc::SerialFile& SFile );
    void LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	void Load_0100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
	void Load_0101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
	void Load_0102( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );	//	������ ���� ���� ����
	void Load_0103( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, boost::function<void (const TCHAR*)> pArchive );
	void Load_0104( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, boost::function<void (const TCHAR*)> pArchive );

	void LoadBasicAABB( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL ChangeDxStaticMaterialMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fCellSize );

protected:
	void CleanUp();

public:
	DxStaticAniFrame();
	~DxStaticAniFrame();
};