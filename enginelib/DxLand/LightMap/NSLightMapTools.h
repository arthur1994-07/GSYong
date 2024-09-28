#pragma once

#include "../../DxTools/DxVertexFVF.h"

class DxStaticMeshCollBase;
class DxPieceManager;


namespace NSLightMapTools
{
    BOOL FindNearTriangle( const std::vector<D3DXVECTOR2>& vecUV, 
							const D3DXVECTOR2& vUV, 
							VEC_DWORD& vecFaceNUM, 
							DWORD& dwFaceOut, 
							float& u, 
							float& v, 
							DWORD& dwMainIndex );

    void CalcPositionNormal( D3DXVECTOR3& vPos_Result, 
                            D3DXVECTOR3& vNor_Result, 
                            const D3DXVECTOR2 vUV_Array[], 
                            const D3DXVECTOR3 vPos_Array[], 
                            const D3DXVECTOR3 vNor_Array[],
                            const D3DXVECTOR2& vUV, 
                            DWORD dwTexelSizeX, 
                            DWORD dwTexelSizeY,
							int nTexelDetail );

    D3DXVECTOR3 GetColor( const D3DXVECTOR3& vPos,
                            const D3DXVECTOR3& vNor, 
                            const D3DLIGHTQ& sLight,
                            float fDot,
							float fDiffusePower,
							BOOL bAmbientADD );

	void GetColorForObject( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxStaticMeshCollBase* pStaticMesh, 
							DxPieceManager* pPieceManager,
							const D3DXVECTOR3& vPos, 
							const D3DXVECTOR3& vNor, 
							BOOL bDontMakeShadowLM,
							D3DXVECTOR3& vColorALL,
							D3DXVECTOR3& vColorIndirect,
							float& fSpot1Color,
							float& fSpot2Color,
							D3DXVECTOR3& vColorNoDynamicLight,
							D3DXVECTOR3& vColorDirectionDirect,
							const WORDWORD& wwLight );

	void GetColor( LPDIRECT3DDEVICEQ pd3dDevice, 
					DxStaticMeshCollBase* pStaticMesh, 
					DxPieceManager* pPieceManager,
					const D3DXVECTOR3& vPos, 
					const D3DXVECTOR3& vNor, 
					BOOL bDontMakeShadowLM,
					D3DXVECTOR3& vColorALL,
					D3DXVECTOR4& vColorDirect_Shadow,
					D3DXVECTOR3& vColorIndirect,
					D3DXVECTOR3& vColorNoDynamicLight,
					const WORDWORD& wwLight,
					WORDWORD* pwwInTriangleSpot );

	void LinkTriangleLight( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxStaticMeshCollBase* pStaticMesh, 
							DxPieceManager* pPieceManager,
							const D3DXVECTOR3& vPos, 
							const D3DXVECTOR3& vNor, 
							std::vector<DWORD>& vecLightForTriangle,
							BOOL bDirectionLight );

	// 하나의 삼각형에 영향받는 라이트 갯수
	void OneTriangleIncludeLight( std::vector<DWORD>& vecLightID_OUT,
									LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager, 
									const std::vector<VERTEXNOR>& vecVerticesS,
									BOOL bDirectionLight );	// 삼각형 하나가 필요함. 그럼으로 정점은 3개

	// 하나의 삼각형에 영향받는 라이트 갯수
	BOOL OneTriangleIncludeLightPiece( std::vector<DWORD>& vecLightID_OUT,
										LPDIRECT3DDEVICEQ pd3dDevice, 
										DxStaticMeshCollBase* pStaticMesh, 
										DxPieceManager* pPieceManager, 
										const std::vector<VERTEXNOR>& vecVerticesS,
										const D3DLIGHTQ& sLight,
										float fAttEndLineValue );	// 삼각형 하나가 필요함. 그럼으로 정점은 3개
		

    //D3DXVECTOR3 GetColor( DxStaticMeshCollBase* pStaticMesh, 
    //                        DxPieceManager* pPieceManager,
    //                        const D3DXVECTOR3& vPos,
    //                        const D3DXVECTOR3& vNor, 
    //                        const D3DLIGHTQ& sLight );

	DWORD ConvertColor( const D3DXVECTOR4& vSrc, int nMultiX, int nMultiY, int nMultiZ, int nMultiA );

    DWORD ConvertColor( const D3DXVECTOR3& vSrc, int nMulti );

    DWORD ConvertColor( const D3DXVECTOR3& vSrc, int nMulti, DWORD dwMultiplyColor );

	void GaussBlur4( DWORD* pData, int nWidth, int nHeight, int nSW, int nSH, int nEW, int nEH );

	void SetRadiosityMode( BOOL bRadiosity );
	BOOL IsRadiosityMode();

}