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

    //// CreateLightMap �� ��
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

	// CreateLightMap �� ��
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

	// CreateLightMap �� ��
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

	// User �� ���� LightMapUV �� �� ��밡���ϵ��� �����Ѵ�.
	BOOL RemakeLightMapUserUV_STATICMESH( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT );

	// User �� ���� LightMapUV �� �� ��밡���ϵ��� �����Ѵ�.
	BOOL RemakeLightMapUserUV_PIECE( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT );

    // LightMap Texture�� Resize �Ѵ�
    void ResizeLightMapTexture();

    // 1���� Width �� ���ȴٸ� �� ������ Block �Ѵ�.
    void OneUseLineBlock();

	// ��� ������ ���Ѵ�.
	void AllLineBlock();

    // Texture ����� ���´�.
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

    // CreateLightMapFinal �� �ؾ� �Ѵ�.
    // Texture ����� �۾�, UV �����ϴ� �۾����� ��� �ִ�.
	// nBackUpNum - 0 �̸� ������ �ʴ´�. 1 ���ʹ� �̸��� �ٿ��� �ؽ��ĸ� �ϳ� �� �����.
    void CreateLightMapFinal_PIECE(LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, const TSTRING& strAddName, int nBackUpNum, const TSTRING& strFinalName );
	void CreateLightMapFinal_STATICMESH( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, const TSTRING& strAddName, DWORD dwTextureID, int nBackUpNum, const TSTRING& strFinalName );

    void CalcMinMax( D3DXVECTOR2& vMin, D3DXVECTOR2& vMax, const D3DXVECTOR2& vSrc );
    void CalcMinMax( D3DXVECTOR3& vMin, D3DXVECTOR3& vMax, const D3DXVECTOR3& vSrc );

    void BackUp_Piece();
    void RollBack_Piece();

    void CleanUp();

	// ����� ����ϱ� ���� �ʱ�ȭ
	void InitTextureDataBackup();

	// ���ǰ� �ִ� ���� ��� �ϵ��� �Ѵ�.
	// g_mapTextureData_BackUP <- m_vecTextureData 
	void SetTextureDataBackupIntoUsed( DWORD dwIndex );

	// ����� ���� ����ϵ��� �Ѵ�.
	// m_vecTextureData <- g_mapTextureData_BackUP
	void SetTextureDataUsedIntoBackup( DWORD dwIndex );

	// Radiosity �� ���´�. ���ο��� CreateLightMapInRadiosity �� ȣ��ȴ�.
	void BakeRadiosityForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
								DxLandMan* pLandMan, 
								VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
								const D3DXMATRIX& matWorld,
								LPDIRECT3DTEXTUREQ pTextureSystem,
								float fReflectance, 
								float fFogBlend,
								DWORD dwFogLength,
								DWORD dwRadiosityIndex );

	// Texel ������ ������ �۾��� �Ѵ�.
	void CreateLightMapInRadiosity( VEC_LIGHTMAPOBJ& vecLightMapObjectDest );
}