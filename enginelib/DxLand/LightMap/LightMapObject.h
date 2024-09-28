#pragma once

#include <boost/tr1/memory.hpp>

#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/TextureManager.h"
#include "./LightMapDef.h"

class DxLandMan;
class DxStaticMeshCollBase;
class DxPieceManager;

class LightMapObject_Data
{
public:
    BOOL                            m_bChangeUV_XZ;     // UV X,Z 를 Change 했는가~?
    WORDWORD                        m_wwBaseTexelXY;    // Texel  그려지는 위치
    WORDWORD                        m_wwTexelSize;      // Texel  Width, Height - 그려야되는 범위
    WORDWORD                        m_wwTextureSize;    // Texture  Width, Height - 그려지는 텍스쳐의 사이즈
    D3DXVECTOR2                     m_vMaxUV;           // UV 최대값
    D3DXVECTOR2                     m_vMinUV;           // UV 최소값

    TSTRING                         m_strTextureCombined;			// LightMap 이름
    TSTRING                         m_strTextureDirect_Shadow_1;	// LightMap 이름
	TSTRING                         m_strTextureDirect_Shadow_2;	// LightMap 이름
    TSTRING                         m_strTextureIndirect;			// LightMap 이름

public:
    LightMapObject_Data();
};

struct RadiosityRenderData
{
	int x;
	int y;
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNor;
};

struct listTriangleID
{
	int nFaceNUM;
	DWORD* pTriangleID;

	listTriangleID()
		: nFaceNUM(0)
		, pTriangleID(NULL)
	{

	}

	~listTriangleID()
	{
		SAFE_DELETE_ARRAY ( pTriangleID );
	}
};

class LightMapObject
{
private:
    BOOL                            m_bChangeUV_XZ;     // UV X,Z 를 Change 했는가~?
    std::vector<VERTEX_WORLD>		m_vecVertices;      // 정점 정보
	std::vector<D3DXVECTOR2>		m_vecLightMapUV;    // LightMapUV
    VEC_DWORD                       m_vecFaces;         // Face List
    WORDWORD                        m_wwBaseTexelXY;    // Texel  그려지는 위치
    WORDWORD                        m_wwTexelSize;      // Texel  Width, Height - 그려야되는 범위
    WORDWORD                        m_wwTextureSize;    // Texture  Width, Height - 그려지는 텍스쳐의 사이즈
    D3DXVECTOR2                     m_vMaxUV;           // UV 최대값
    D3DXVECTOR2                     m_vMinUV;           // UV 최소값
    WORDWORD                        m_wwBaseTexelPieceXY;   // BaseTexel Piece용
    
    BOOL                            m_bGaussBlurSetUV;		// GaussBlur와 UV Setting을 하였나~?
    DWORD                           m_dwTextureID;			// TextureID
	bool*							m_pInTriangle;			// 삼각형 안에 있는지 확인 
    DWORD*                          m_pTexelCombined;		// Texel 컬러
    DWORD*                          m_pTexelDirect_Shadow;  // Texel 컬러
	DWORD*                          m_pTexelIndirect;		// Texel 컬러
	DWORD*                          m_pTexelCombinedRAD;	// Texel 컬러
	DWORD*                          m_pTexelNoDynamicLight;	// Texel 컬러 - DynamicLight 가 될 수 없는 것들의 Light 모임.
	listTriangleID*					m_pTexelTriangleID;		// 그 텍셀에 연결된 TriangleID
	WORDWORD						m_wwTexelTriangleSize;	// Triangle 

	std::vector<WORDWORD>			m_vecTriangle_wwLightID;	// 현재삼각형이 어떤 라이트에 영향을 받는가~?			

	std::list<RadiosityRenderData>	m_listRadiosityRenderData;	// RadiosityRender 정보

	TSTRING                         m_strTextureCombined;			// LightMap 이름
	TSTRING                         m_strTextureDirect_Shadow_1;  // LightMap 이름
	TSTRING                         m_strTextureDirect_Shadow_2;  // LightMap 이름
	TSTRING                         m_strTextureIndirect;		// LightMap 이름
    // Texture
    // 기본 Mesh ID
    // 분리 Mesh 정보
    // UV 범위
    // UV 위치
    // Texel별 Color 셋팅

    // Get
public:
    DWORD GetTextureID() const;
    DWORD GetTexelSizeX() const;
    DWORD GetTexelSizeY() const;
	void GetLightMapUV_RATE( D3DXVECTOR2& vRate ) const;
    BOOL IsGaussBlurSetUV() const;
    DWORD* GetTexelColorCombined() const;
    DWORD* GetTexelColorDirect_Shadow() const;
	DWORD* GetTexelColorIndirect() const;
	DWORD* GetTexelColorCombinedRAD() const;
	DWORD* GetTexelColorNoDynamicLight() const;
    const WORDWORD& GetBaseTexelXY() const;
    const WORDWORD& GetBaseTexelPieceXY() const;
    TextureResource GetTexture( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strAddName ) const;
	void GetLightMapName( TSTRING& strTextureCombined,
							TSTRING& strTextureDirect_Shadow_1,
							TSTRING& strTextureDirect_Shadow_2,
							TSTRING& strTextureIndirect );
    void Export( LightMapObject_Data& sData );

    // Set
public:
    void SetBaseTexelXY( WORDWORD& sTexelXY );
    void SetTextureSizeXY( WORDWORD& sSizeXY );
    void SetBaseTexelPieceXY( WORDWORD& sTexelXY );
    void SetTextureID( DWORD dwID );
	void SetLightMapName( const TSTRING& strTextureCombined,
							const TSTRING& strTextureDirect_Shadow_1,
							const TSTRING& strTextureDirect_Shadow_2,
							const TSTRING& strTextureIndirect );

	void SetTriangle_vecLightID( const std::vector<WORDWORD>& vecTriangle_wwLightID );

	// 텍셀을 셋팅해준다.
	void SetTexelScale( WORDWORD wwTexelSize );

	void SetLightMapUV( const std::vector<D3DXVECTOR2>& vecUV, WORDWORD wwTexelSize );

	void CreateTexelTriangleID();
	void DeleteTexelTriangleID();

    // 
public:
    // 버텍스를 얻어옴.
 //   void AddThreeVerties( const VERTEX* pVerticesSrc, NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType, DWORD dwFace );
 //   void AddThreeVerties( const VERTEXNORCOLORTEX* pVerticesSrc, NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType, DWORD dwFace );
	//void AddThreeVerties( const VERTEXNORTEX2* pVerticesSrc, NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType, DWORD dwFace );
	void AddThreeVerties( const VERTEXNORCOLORTEX4* pVerticesSrc, DWORD dwFace );
	void AddThreeVerties( const VERTEX_WORLD* pVerticesSrc, DWORD dwFace );

    // 알맞을 텍셀 크기를 구함.
    void CreateTexelScale( float fLengthRate );

    // 텍셀을 만들고 계산하고..
    void CreateTexel( LPDIRECT3DDEVICEQ pd3dDevice, 
					 DxStaticMeshCollBase* pStaticMesh, 
                     DxPieceManager* pPieceManager, 
                     const D3DXMATRIX& matWorld,
                     BOOL bDontMakeShadowLM,
					 const std::vector<BOOL>* pvecReceiveShadowLM,
					 int nSampling,
					 const WORDWORD* pwwLightMapUserUV_TEXEL,
					 const WORDWORD* pwwLightID4Piece,
					 BOOL& bEnoughUsedPixelLightMap );

	// RAD Normal 을 위한 LightMap 정보
	void CreateTexelForRADNormal( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager,
									std::vector<VERTEX_WORLD>& vecVerticesMatrix,
									BOOL _bDontMakeShadowLM,
									const std::vector<BOOL>* pvecReceiveShadowLM,
									int nSampling,
									WORDWORD wwBaseTexelXY,
									WORDWORD wwLightMapUserUV_TEXEL,
									const WORDWORD* pwwLightID4Piece );

	// RAD Normal 을 위한 Vertex 정보 셋팅.
	void SetVertexForRADNormal( LPDIRECT3DDEVICEQ pd3dDevice, std::vector<VERTEX_WORLD>& vecVerticesMatrix, DWORD dwRADNormalIndex, const D3DXMATRIX& matWorld );

	// Texture에 맞게 UV를 재계산한다.
	void CaleUV( std::vector<D3DXVECTOR2>& vecLightMapUV, DWORD dwOffsetX, DWORD dwOffsetY );

    // Texture에 맞게 UV를 재계산한다.
    void CaleUV( DWORD dwOffsetX, DWORD dwOffsetY );

	// UV에 - 연산을 해준다.
	void SubUV( std::vector<D3DXVECTOR2>& vecLightMapUV, WORDWORD wwBaseTexelXY );

    // UV에 - 연산을 해준다.
    void SubUV( WORDWORD wwBaseTexelXY );

    //// VERTEXNORTEX2 정보를 얻어온다.
    //void GetVerticesData( VERTEXNORTEX2* pVertices, DWORD dwFaceStartNum, DWORD& dwFaceNum );

    //// VERTEXCOLORNORTEX2 정보를 얻어온다.
    //void GetVerticesData( VERTEXNORCOLORTEX2* pVertices, DWORD dwFaceStartNum, DWORD& dwFaceNum );

    // LightMap UV 셋팅
    void SetLightMapUV( std::vector<VERTEX_WORLD>& vecVertices, DWORD* pIndices, DWORD dwLightMapUVIndex );

    // 텍셀 정보를 삭제함.
    void DeleteTexelColor();

	// Radiosity 를 굽는다.
	void BakeRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxLandMan* pLandMan, 
						const D3DXMATRIX& matWorld, 
						LPDIRECT3DTEXTUREQ pTextureSystem, 
						float fReflectance, 
						float fFogBlend, 
						DWORD dwFogLength,
						DWORD dwRADNormalIndex,		// RAD Normal 생성시 사용. -1 일 경우 사용하지 않음.
						int nSampling,
						BOOL bPiece );

private:
	// 슈퍼샘플링
	BOOL SuperSampling( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxStaticMeshCollBase* pStaticMesh, 
						DxPieceManager* pPieceManager, 
						DWORD x, 
						DWORD y, 
						int nTexelDetail,
						const std::vector<D3DXVECTOR2>& vecPointRate,
						D3DXVECTOR3& vColorDay, 
						D3DXVECTOR4& vColorDirect_Shadow,
						D3DXVECTOR3& vColorIndirect,
						D3DXVECTOR3& vColorNoDynamicLight,
						const std::vector<D3DXVECTOR2>& vecUV,
						const std::vector<VERTEX_WORLD>& vecVerticesMatrix,
						BOOL _bDontMakeShadowLM,
						const std::vector<BOOL>* pvecReceiveShadowLM,
						WORDWORD wwBaseTexelXY,
						WORDWORD wwLightMapUserUV_TEXEL,
						const WORDWORD* pwwLightID4Piece,
						WORDWORD* pwwInTriangleSpot );

	// 슈퍼샘플링
	BOOL SuperSamplingMain( LPDIRECT3DDEVICEQ pd3dDevice,
							DxStaticMeshCollBase* pStaticMesh, 
							DxPieceManager* pPieceManager, 
							DWORD x, 
							DWORD y, 
							int nSampling, 
							D3DXVECTOR3& vColorDay, 
							D3DXVECTOR4& vColorDirect_Shadow,
							D3DXVECTOR3& vColorIndirect,
							D3DXVECTOR3& vColorNoDynamicLight,
							const std::vector<D3DXVECTOR2>& vecUV,
							const std::vector<VERTEX_WORLD>& vecVerticesMatrix,
							BOOL _bDontMakeShadowLM,
							const std::vector<BOOL>* pvecReceiveShadowLM,
							WORDWORD wwBaseTexelXY,
							WORDWORD wwLightMapUserUV_TEXEL,
							const WORDWORD* pwwLightID4Piece,
							WORDWORD* pwwInTriangleSpot );

	// Edge Color 를 적용한다.
	void EdgeColorApply( DWORD* pTexelColor );

	// Edge Color 를 적용한다.
	void EdgeColorApply_SDSD( DWORD* pTexelColor, const WORDWORD* pwwInTriangleSpot );

	// Radiosity Render
	DWORD RadiosityRender( LPDIRECT3DDEVICEQ pd3dDevice, 
		DxLandMan* pLandMan, 
		const D3DXVECTOR3& vPos, 
		const D3DXVECTOR3& vNor, 
		LPDIRECT3DTEXTUREQ pTextureSystem,
		float fReflectance, 
		float fFogBlend,
		DWORD dwFogLength,
		BOOL bDay );

public:
    LightMapObject();
    ~LightMapObject();
};
