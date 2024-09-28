#pragma once

#include "../../../SigmaCore/gassert.h"

#include "../../DxTools/Collision/CollisionDef.h"
#include "../../DxLand/LightMap/LightMapDef.h"
#include "../TexEff/DxTextureEffMan.h"
#include "./DxStaticMeshCollBase.h"

class DxFrameMesh;
class DxPieceEdit;
class DxStaticMesh;
class DxPieceManager;
class DxPieceManagerPSF;
class DxTextureEffMan;
class CMaterialSetting;
class DxLandMan;
struct CLIPVOLUME;
struct LightMapBoxColorData;

class DxStaticMeshBase : public DxStaticMeshCollBase
{
public:
	virtual D3DXVECTOR3 GetAABBMax () = 0;
	virtual D3DXVECTOR3 GetAABBMin () = 0;

	virtual void MakeAABBOCTree( const LPDIRECT3DDEVICEQ pd3dDevice, 
								DxFrameMesh* const pFrameMesh, 
								DxPieceManagerPSF* pPieceManager, 
								LIGHTMAPUV_MAIN* pLightMapUV_MAIN,
								const BOOL bProgress, 
								BOOL bPiece, 
								BOOL bLightMap, 
								const TSTRING& strLightMapName,
								float fLightMapDetail,
								const DxTextureEffMan* pTextureEffMan,
								const CMaterialSetting* pMaterialSetting,
								BOOL Ran_1_0 )								{ GASSERT( 0 && "DxStaticMesh 이 아닌곳에서 호출됨" ); }

	virtual void BakeRadiosityForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxLandMan* pLandMan,
										VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										const D3DXMATRIX& matWorld, 
										LPDIRECT3DTEXTUREQ pTextureSystem,
										float fReflectance,
										float fFogBlend,
										DWORD dwFogLength,
										DWORD dwRadiosityIndex )							{ MIN_ASSERT( 0 && "DxStaticMaterialMesh 이 아닌곳에서 호출됨" ); }

	virtual void MergeLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										const TSTRING& strLightMapName, 
										DxStaticMesh* pStaticMesh, 
										DxPieceManagerPSF* pPieceManager, 
										const D3DXMATRIX& matWorld, 
										VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										DWORD& dwStartTextureSize,
										WORDWORD& wwBaseTexelXY,
										BOOL& bEnoughUsedPixelLightMap,
										const WORDWORD& wwLightID4Shadow ) = 0;

	virtual void CreateLightMapUVPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
										const TSTRING& strLightMapName,
										TSTRING& strTextureCombined, 
										TSTRING& strTextureDirect_Shadow_1,
										TSTRING& strTextureDirect_Shadow_2,
										TSTRING& strTextureIndirect,
										VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
										WORDWORD wwBaseTexelXY ) = 0;

	virtual void CreateLightMapUVPieceObject( LPDIRECT3DDEVICEQ pd3dDevice, 
												const TSTRING& strLightMapName,
												TSTRING& strTextureCombined, 
												TSTRING& strTextureDirect_Shadow_1,
												TSTRING& strTextureDirect_Shadow_2,
												TSTRING& strTextureIndirect,
												VEC_SVEC_LIGHTMAPOBJ& vecsvecLightMapObj,
												WORDWORD wwBaseTexelXY,
												BOOL bRan_1_0, 
												float fCellSize ) = 0;

	virtual void FlipLightMapRenderMan( DxStaticMeshBase* pStaticMesh ) = 0;

	virtual void CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;

	//virtual void GetAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3&  vMin ) = 0;
	//virtual BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision, EMCOLLMODE emCullMode ) const = 0;

	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, 
									LPCSTR& szName, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const = 0;

	virtual BOOL IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const = 0;

	virtual void GetClosedPoint( D3DXVECTOR3& vCollisionInOut ) = 0;

	virtual void ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld ) = 0;

	virtual void FrameMove( const float fElapsedTime ) = 0;

	virtual void Render_PIECE_( const LPDIRECT3DDEVICEQ pd3dDevice, 
								DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
								const D3DXMATRIX& matWorld, 
								const D3DXVECTOR2& vLightMapUV_Offset,
								float fAlpha, 
								int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
								LPDIRECT3DTEXTUREQ pTextureDay, 
								LPDIRECT3DTEXTUREQ pTextureNight,
								BOOL bMPShading ) = 0;

	virtual void Render_PIECE_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
									DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
									const D3DXMATRIX& matWorld, 
									const D3DXVECTOR2& vLightMapUV_Offset,
									float fAlpha, 
									int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
									LPDIRECT3DTEXTUREQ pTextureDay, 
									LPDIRECT3DTEXTUREQ pTextureNight,
									BOOL bMPShading ) = 0;

	virtual void Render_PIECE_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, 
										DxTextureEffMan::MAPTEXEFF* pmapTexEff, 
										const D3DXMATRIX& matWorld, 
										const D3DXVECTOR2& vLightMapUV_Offset,
										float fAlpha, 
										int nRenderMode,					// nRenderMode - NSMaterialManager::RENDER_MODE
										LPDIRECT3DTEXTUREQ pTextureDay, 
										LPDIRECT3DTEXTUREQ pTextureNight,
										BOOL bMPShading ) = 0;

	virtual void Render_CharShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV ) = 0;

	virtual BOOL IsLightMapPiece() = 0;
	virtual void SaveLightMapPiece( sc::SerialFile& SFile ) = 0;
	virtual void LoadLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile ) = 0;

	// TriangleLinkLight
public:
	// Piece의 TriangleLinkLight 를 생성하도록 한다.
	virtual BOOL CreateTriangleLinkLightForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
													DxStaticMeshCollBase* pStaticMesh, 
													DxPieceManager* pPieceManager, 
													std::vector<DWORD>& vecLightID, 
													const D3DXMATRIX& matWorld,
													const D3DLIGHTQ& sLight,
													float fAttEndLineValue ) const = 0;

	BOOL CreateTriangleLinkLightForPieceDetail( LPDIRECT3DDEVICEQ pd3dDevice, 
												DxStaticMeshCollBase* pStaticMesh, 
												DxPieceManager* pPieceManager, 
												std::vector<DWORD>& vecLightID, 
												const D3DXMATRIX& matWorld,
												const std::vector<VERTEXNORCOLORTEX4>& vecVerticesS,
												const D3DLIGHTQ& sLight,
												float fAttEndLineValue ) const;

public:
	DxStaticMeshBase();
	virtual ~DxStaticMeshBase();
};