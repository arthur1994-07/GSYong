#pragma once

#include "../../DxTools/DxVertexFVF.h"
#include "./LightMapDef.h"

class DxStaticMeshCollBase;
class DxPieceManagerPSF;

namespace NSLightMap
{
    extern TSTRING g_strPath;
    extern LPDIRECT3DTEXTUREQ g_pGlobalTexture;
	extern BOOL g_bNoDXT1;
	extern int	g_nSampling;

    void Init( BOOL bPiece );

    //// CreateLightMap 한 후
    //BOOL CreateLightMap(
    //    LPDIRECT3DDEVICEQ pd3dDevice, 
    //    const TSTRING& strLightMapName, 
    //    const TSTRING& strAddName, 
    //    BOOL bAlphaChange,
    //    BOOL bPiece,
    //    VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
    //    DWORD FaceNUM, 
    //    BYTE* pVerticesSrc, 
    //    DWORD dwFVF,
    //    DxStaticMeshCollBase* pStaticMesh,
    //    DxPieceManager* pPieceManager,
    //    const D3DXMATRIX& matWorld,
    //    DWORD& dwStartTextureSize,
    //    float fLightMapDetail,
    //    NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType,
    //    BOOL bDontMakeShadowLM);

	// CreateLightMap 한 후
	BOOL CreateFieldLightMapUserUV(
		LPDIRECT3DDEVICEQ pd3dDevice, 
		const TSTRING& strLightMapName, 
		const TSTRING& strAddName, 
		BOOL bAlphaChange,
		VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
		const std::vector<WORDWORD>& vecTriangle_wwLightID,
		DWORD FaceNUM, 
		BYTE* pVerticesSrc, 
		DWORD dwFVF,
		DxStaticMeshCollBase* pStaticMesh,
		DxPieceManagerPSF* pPieceManager,
		const D3DXMATRIX& matWorld,
		const std::vector<BOOL>& vecReceiveShadowLM,
		DWORD dwTextureID,
		WORDWORD wwLightMapUserUV_TEXEL,
		LIGHTMAPUV* pLightMapUV );

	// CreateLightMap 한 후
	BOOL CreatePieceLightMapUserUV(
		LPDIRECT3DDEVICEQ pd3dDevice, 
		const TSTRING& strLightMapName, 
		const TSTRING& strAddName, 
		BOOL bAlphaChange,
		BOOL bPiece,
		VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
		DWORD FaceNUM, 
		BYTE* pVerticesSrc, 
		DWORD dwFVF,
		DxStaticMeshCollBase* pStaticMesh,
		DxPieceManagerPSF* pPieceManager,
		const D3DXMATRIX& matWorld,
		DWORD& dwStartTextureSize,
		BOOL& bEnoughUsedPixelLightMap,
		const std::vector<BOOL>& vecReceiveShadowLM,
		WORDWORD wwTexelSize,
		const std::vector<D3DXVECTOR2>& vecUV,
		const WORDWORD& wwLightID4Piece );

	// User 가 만든 LightMapUV 를 잘 사용가능하도록 변경한다.
	BOOL RemakeLightMapUserUV_STATICMESH( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT );

	// User 가 만든 LightMapUV 를 잘 사용가능하도록 변경한다.
	BOOL RemakeLightMapUserUV_PIECE( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT );

    // LightMap Texture를 Resize 한다
    void ResizeLightMapTexture();

    // 1개의 Width 라도 사용된다면 그 라인은 Block 한다.
    void OneUseLineBlock();

	// 모든 라인을 블럭한다.
	void AllLineBlock();

    // Texture 사이즈를 얻어온다.
    WORDWORD GetTextureSize( VEC_LIGHTMAPOBJ& vecLightMapObjectDest );

    // 
    void CreateLightMap2_Detail(
        LightMapObject* pLightMapObject,
		DWORD dwTextureID,
        BOOL bAlphaChange,
        BOOL bPiece,
        WORDWORD wwBaseTexelXY,
        BOOL bCreateTextureDataLowerEmpty,
		BOOL bUserUV );

    // 
    void CreateLightMap2_PIECE(
        VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
        const TSTRING& strLightMapName, 
        const TSTRING& strAddName, 
        BOOL bAlphaChange,
        BOOL bPiece,
		TSTRING& strTextureCombined, 
		TSTRING& strTextureDirect_Shadow_1,
		TSTRING& strTextureDirect_Shadow_2,
		TSTRING& strTextureIndirect,
        WORDWORD wwBaseTexelXY,
        BOOL bCreateTextureDataLowerEmpty,
		BOOL bUserUV );

	// 
	void CreateLightMap2_STATICMESH(
		VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
		const TSTRING& strLightMapName, 
		const TSTRING& strAddName, 
		BOOL bAlphaChange,
		BOOL bPiece,
		TSTRING& strTextureCombined, 
		TSTRING& strTextureDirect_Shadow_1,
		TSTRING& strTextureDirect_Shadow_2,
		TSTRING& strTextureIndirect,
		WORDWORD wwBaseTexelXY );

    // CreateLightMapFinal 을 해야 한다.
    // Texture 만드는 작업, UV 셋팅하는 작업등이 들어 있다.
	// nBackUpNum - 0 이면 만들지 않는다. 1 부터는 이름을 붙여서 텍스쳐를 하나 더 만든다.
    void CreateLightMapFinal_PIECE(LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, const TSTRING& strAddName, int nBackUpNum, const TSTRING& strFinalName );
	void CreateLightMapFinal_STATICMESH( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, const TSTRING& strAddName, DWORD dwTextureID, int nBackUpNum, const TSTRING& strFinalName );

    void CalcMinMax( D3DXVECTOR2& vMin, D3DXVECTOR2& vMax, const D3DXVECTOR2& vSrc );
    void CalcMinMax( D3DXVECTOR3& vMin, D3DXVECTOR3& vMax, const D3DXVECTOR3& vSrc );

    void BackUp_Piece();
    void RollBack_Piece();

    void CleanUp();

	// 백업을 사용하기 위한 초기화
	void InitTextureDataBackup();

	// 사용되고 있는 것을 백업 하도록 한다.
	// g_mapTextureData_BackUP <- m_vecTextureData 
	void SetTextureDataBackupIntoUsed( DWORD dwIndex );

	// 백업된 것을 사용하도록 한다.
	// m_vecTextureData <- g_mapTextureData_BackUP
	void SetTextureDataUsedIntoBackup( DWORD dwIndex );

	// Radiosity 를 굽는다. 내부에서 CreateLightMapInRadiosity 도 호출된다.
	void BakeRadiosityForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
								DxLandMan* pLandMan, 
								VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
								const D3DXMATRIX& matWorld,
								LPDIRECT3DTEXTUREQ pTextureSystem,
								float fReflectance, 
								float fFogBlend,
								DWORD dwFogLength,
								DWORD dwRadiosityIndex );

	// Texel 정보를 모으는 작업을 한다.
	void CreateLightMapInRadiosity( VEC_LIGHTMAPOBJ& vecLightMapObjectDest );
}