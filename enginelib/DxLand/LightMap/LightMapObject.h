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
    BOOL                            m_bChangeUV_XZ;     // UV X,Z �� Change �ߴ°�~?
    WORDWORD                        m_wwBaseTexelXY;    // Texel  �׷����� ��ġ
    WORDWORD                        m_wwTexelSize;      // Texel  Width, Height - �׷��ߵǴ� ����
    WORDWORD                        m_wwTextureSize;    // Texture  Width, Height - �׷����� �ؽ����� ������
    D3DXVECTOR2                     m_vMaxUV;           // UV �ִ밪
    D3DXVECTOR2                     m_vMinUV;           // UV �ּҰ�

    TSTRING                         m_strTextureCombined;			// LightMap �̸�
    TSTRING                         m_strTextureDirect_Shadow_1;	// LightMap �̸�
	TSTRING                         m_strTextureDirect_Shadow_2;	// LightMap �̸�
    TSTRING                         m_strTextureIndirect;			// LightMap �̸�

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
    BOOL                            m_bChangeUV_XZ;     // UV X,Z �� Change �ߴ°�~?
    std::vector<VERTEX_WORLD>		m_vecVertices;      // ���� ����
	std::vector<D3DXVECTOR2>		m_vecLightMapUV;    // LightMapUV
    VEC_DWORD                       m_vecFaces;         // Face List
    WORDWORD                        m_wwBaseTexelXY;    // Texel  �׷����� ��ġ
    WORDWORD                        m_wwTexelSize;      // Texel  Width, Height - �׷��ߵǴ� ����
    WORDWORD                        m_wwTextureSize;    // Texture  Width, Height - �׷����� �ؽ����� ������
    D3DXVECTOR2                     m_vMaxUV;           // UV �ִ밪
    D3DXVECTOR2                     m_vMinUV;           // UV �ּҰ�
    WORDWORD                        m_wwBaseTexelPieceXY;   // BaseTexel Piece��
    
    BOOL                            m_bGaussBlurSetUV;		// GaussBlur�� UV Setting�� �Ͽ���~?
    DWORD                           m_dwTextureID;			// TextureID
	bool*							m_pInTriangle;			// �ﰢ�� �ȿ� �ִ��� Ȯ�� 
    DWORD*                          m_pTexelCombined;		// Texel �÷�
    DWORD*                          m_pTexelDirect_Shadow;  // Texel �÷�
	DWORD*                          m_pTexelIndirect;		// Texel �÷�
	DWORD*                          m_pTexelCombinedRAD;	// Texel �÷�
	DWORD*                          m_pTexelNoDynamicLight;	// Texel �÷� - DynamicLight �� �� �� ���� �͵��� Light ����.
	listTriangleID*					m_pTexelTriangleID;		// �� �ؼ��� ����� TriangleID
	WORDWORD						m_wwTexelTriangleSize;	// Triangle 

	std::vector<WORDWORD>			m_vecTriangle_wwLightID;	// ����ﰢ���� � ����Ʈ�� ������ �޴°�~?			

	std::list<RadiosityRenderData>	m_listRadiosityRenderData;	// RadiosityRender ����

	TSTRING                         m_strTextureCombined;			// LightMap �̸�
	TSTRING                         m_strTextureDirect_Shadow_1;  // LightMap �̸�
	TSTRING                         m_strTextureDirect_Shadow_2;  // LightMap �̸�
	TSTRING                         m_strTextureIndirect;		// LightMap �̸�
    // Texture
    // �⺻ Mesh ID
    // �и� Mesh ����
    // UV ����
    // UV ��ġ
    // Texel�� Color ����

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

	// �ؼ��� �������ش�.
	void SetTexelScale( WORDWORD wwTexelSize );

	void SetLightMapUV( const std::vector<D3DXVECTOR2>& vecUV, WORDWORD wwTexelSize );

	void CreateTexelTriangleID();
	void DeleteTexelTriangleID();

    // 
public:
    // ���ؽ��� ����.
 //   void AddThreeVerties( const VERTEX* pVerticesSrc, NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType, DWORD dwFace );
 //   void AddThreeVerties( const VERTEXNORCOLORTEX* pVerticesSrc, NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType, DWORD dwFace );
	//void AddThreeVerties( const VERTEXNORTEX2* pVerticesSrc, NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType, DWORD dwFace );
	void AddThreeVerties( const VERTEXNORCOLORTEX4* pVerticesSrc, DWORD dwFace );
	void AddThreeVerties( const VERTEX_WORLD* pVerticesSrc, DWORD dwFace );

    // �˸��� �ؼ� ũ�⸦ ����.
    void CreateTexelScale( float fLengthRate );

    // �ؼ��� ����� ����ϰ�..
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

	// RAD Normal �� ���� LightMap ����
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

	// RAD Normal �� ���� Vertex ���� ����.
	void SetVertexForRADNormal( LPDIRECT3DDEVICEQ pd3dDevice, std::vector<VERTEX_WORLD>& vecVerticesMatrix, DWORD dwRADNormalIndex, const D3DXMATRIX& matWorld );

	// Texture�� �°� UV�� �����Ѵ�.
	void CaleUV( std::vector<D3DXVECTOR2>& vecLightMapUV, DWORD dwOffsetX, DWORD dwOffsetY );

    // Texture�� �°� UV�� �����Ѵ�.
    void CaleUV( DWORD dwOffsetX, DWORD dwOffsetY );

	// UV�� - ������ ���ش�.
	void SubUV( std::vector<D3DXVECTOR2>& vecLightMapUV, WORDWORD wwBaseTexelXY );

    // UV�� - ������ ���ش�.
    void SubUV( WORDWORD wwBaseTexelXY );

    //// VERTEXNORTEX2 ������ ���´�.
    //void GetVerticesData( VERTEXNORTEX2* pVertices, DWORD dwFaceStartNum, DWORD& dwFaceNum );

    //// VERTEXCOLORNORTEX2 ������ ���´�.
    //void GetVerticesData( VERTEXNORCOLORTEX2* pVertices, DWORD dwFaceStartNum, DWORD& dwFaceNum );

    // LightMap UV ����
    void SetLightMapUV( std::vector<VERTEX_WORLD>& vecVertices, DWORD* pIndices, DWORD dwLightMapUVIndex );

    // �ؼ� ������ ������.
    void DeleteTexelColor();

	// Radiosity �� ���´�.
	void BakeRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
						DxLandMan* pLandMan, 
						const D3DXMATRIX& matWorld, 
						LPDIRECT3DTEXTUREQ pTextureSystem, 
						float fReflectance, 
						float fFogBlend, 
						DWORD dwFogLength,
						DWORD dwRADNormalIndex,		// RAD Normal ������ ���. -1 �� ��� ������� ����.
						int nSampling,
						BOOL bPiece );

private:
	// ���ۻ��ø�
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

	// ���ۻ��ø�
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

	// Edge Color �� �����Ѵ�.
	void EdgeColorApply( DWORD* pTexelColor );

	// Edge Color �� �����Ѵ�.
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
