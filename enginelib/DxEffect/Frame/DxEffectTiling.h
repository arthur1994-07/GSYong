// Terrain0.h: interface for the DxEffectWater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DXEFFECTTILING__INCLUDED__)
#define __DXEFFECTTILING__INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <hash_map>
#include <map>
#include <algorithm>
#include <boost/tr1/memory.hpp> 

#include "../../../VisualMaterialLib/Define/DxVMDefine.h"

#include "../../DxLand/LightMap/LightMapDef.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/Collision.h"
#include "../../DxTools/TextureManager.h"
#include "./DxEffectFrame.h"

#define USE_TEX_DRAW		0x0001
#define USE_COLOR_DRAW		0x0002

struct DxMeshes;
struct DXMATERIAL;
class DxSingleTexMesh;
class DxTextureEffMan;
class DxTangentMesh;
class DxPieceManagerPSF;
class LightMapObject_Data;

struct TILING_PROPERTY_OLD				// Ver.102 이전
{
	DWORD			m_dwFlag;

	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

	DWORD			m_dwVertices;
	DWORD			m_dwFaces;

	DWORD			m_dwUNIQUE_NUMBER;
	DWORD			m_dwMaterials_B;

	D3DXMATRIX		m_matFrameComb;

};

struct TILING_PROPERTY				// Ver.103 Ver.104
{
	DWORD			m_dwFlag;

	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

	DWORD			m_dwVertices;
	DWORD			m_dwFaces;

	DWORD			m_dwUNIQUE_NUMBER;
	DWORD			m_dwMaterials_B;

	D3DXMATRIX		m_matFrameComb;

	BYTE			m_dwRenderPriority;

	TILING_PROPERTY () :
		m_vMax(0.f,0.f,0.f),
		m_vMin(0.f,0.f,0.f),
		m_dwVertices(0),
		m_dwFaces(0),
		m_dwMaterials_B(0),
		m_dwUNIQUE_NUMBER(1),
		m_dwRenderPriority(0)
	{
		m_dwFlag = USE_TEX_DRAW;
	};

	VOID Assign( const TILING_PROPERTY_OLD &oldProperty )
	{
		m_dwFlag		  = oldProperty.m_dwFlag;
		m_vMax			  = oldProperty.m_vMax;
		m_vMin			  = oldProperty.m_vMin;
		m_dwVertices	  = oldProperty.m_dwVertices;
		m_dwFaces		  = oldProperty.m_dwFaces;
		m_dwUNIQUE_NUMBER = oldProperty.m_dwUNIQUE_NUMBER;
		m_matFrameComb    = oldProperty.m_matFrameComb;
	}
};


class DxEffectTiling : public DxEffectBase
{
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

	CString m_strPriorityName;

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ();

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

public:
	void SetProperty ( TILING_PROPERTY& Property )
	{
		m_Property = Property;
	}
	TILING_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			TILING_PROPERTY		m_Property;
		};

		struct
		{
			DWORD			m_dwFlag;

			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;

			DWORD			m_dwVertices;
			DWORD			m_dwFaces;

			DWORD			m_dwUNIQUE_NUMBER;
			DWORD			m_dwMaterials_B;

			D3DXMATRIX		m_matFrameComb;

			BYTE			m_dwRenderPriority;
		};
	};

	//	Note : 랜더링 데이터
	//
protected:
	static	LPDIRECT3DSTATEBLOCK9		m_pSavedSB;
	static	LPDIRECT3DSTATEBLOCK9		m_pDrawSB;

	static	LPDIRECT3DSTATEBLOCK9		m_pSavedSB_Select;
	static	LPDIRECT3DSTATEBLOCK9		m_pDrawSB_Select;

protected:

	struct POINT
	{
		D3DXVECTOR3		vPos;			// 위치
		D3DXVECTOR3		vNor;			// Normal
		D3DXVECTOR2		vTex;			// Tex 좌표
        D3DXVECTOR3		vTan;			// Tangent
		DWORD			dwFace_NUM;		// 자신이 속해 있는 Face Number
		DWORD			dwPointEx;		// 녀석의 음...부모격 포인트
	};

	struct POINTA
	{
		DWORD	dwIndex;
		POINTA*	pNext;

		POINTA() :
			pNext(NULL)
		{
		}

		~POINTA()
		{
			SAFE_DELETE ( pNext );
		}
	};

	struct POINTEX_PREV
	{
		POINTA*		pPoint;		// 원본 Point
		D3DXVECTOR3	vPos;		// 위치
		DWORD		dwMaterial;
		DWORD		dwColor;

		POINTEX_PREV()
			: pPoint(NULL)
			, vPos(0.f,0.f,0.f)
			, dwMaterial(0)
			, dwColor(0x00ffffff)
		{
		}
	};

	struct POINTEX
	{
		POINTA*		pPoint;		// 원본 Point
		D3DXVECTOR3	vPos;		// 위치

		POINTEX()
			: pPoint(NULL)
			, vPos(0.f,0.f,0.f)
		{
		}

		~POINTEX()
		{
			SAFE_DELETE ( pPoint );
		}
	};

	struct POINT_USE_COLOR
	{
		BOOL	bUse;
		DWORD	dwColor;

		POINT_USE_COLOR()
			: bUse(FALSE)
			, dwColor(0x00ffffff)
		{
		}
	};

protected:
    struct SEPARATEOBJ
	{
		VERTEXNORCOLORTEX	m_sVertex[3];			// Save, Load
        D3DXVECTOR3			m_vTangetArray[3];		// Don`t Save Load
		D3DXVECTOR2			m_avLightMapUV[3];	// Don`t Save, Load

        void Save( sc::SerialFile &SFile );
        void Load( sc::SerialFile &SFile );
	};

	struct TRIANGLE
	{
		BOOL		bBase;			// 기본 삼각형을 쓸 것인가 ?
		DWORD		dwMaterial;		// 무슨 종류의 텍스쳐를 쓰는지 알기만 하면 된다.

		TRIANGLE() :
			bBase(TRUE)
		{
		}
	};

	typedef std::list<DWORD>			TRIANGLELIST;
	typedef TRIANGLELIST::iterator		TRIANGLELIST_ITER;

	typedef std::list<SEPARATEOBJ>		FACELIST;
	typedef FACELIST::iterator			FACELIST_ITER;

	//-------------------------------------------------------------------------------------------------------------
	// Note : m_pPointEX 을 바탕으로 하여서, m_pPointUseColor 과 listTriangle 을 조합하여 listFace 을 만들어 낸다.
	//
	struct MATERIALEX
	{
		TextureResource		m_textureRes;			//
		std::string			strTexture;				//	SAVE

		POINT_USE_COLOR*	m_pPointUseColor;		// 전점을 가지고 있고 사용 중인 위치도 가지고 있는다. SAVE
													// 디퓨즈값과 사용함 안함을 가지고 있음

		TRIANGLELIST		listTriangle;			// 삼각형 List를 가지고 있다.
		FACELIST			listFace;				// 렌더링 용으로 가지고 있다.

		BOOL				m_bAlpha;
		DxSingleTexMesh*	m_pMeshList;			// Save

        VEC_LIGHTMAPOBJ     m_vecLightMapObject;    // 

		MATERIALEX( DWORD dwPointEX_NUM );
		~MATERIALEX();
	};

protected:
	void CreateAABBOctree( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX, DxTextureEffMan* pTexEff );

	//void CreateAABBOctreeLM_1( LPDIRECT3DDEVICEQ pd3dDevice, 
	//							MATERIALEX* pMaterialEX,
	//							DxStaticMesh* pStaticMesh, 
	//							DxPieceManager* pPieceManager, 
	//							LIGHTMAPUV* pLightMapUV, 
	//							DWORD dwTextureID );

    //void CreateAABBOctreeLM_2( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX, DxTextureEffMan* pTexEff, const TSTRING& strLightMapName );

    void CreateAABBOctreeLM( LPDIRECT3DDEVICEQ pd3dDevice, 
								MATERIALEX* pMaterialEX,
								DxTextureEffMan* pTexEff,
								const TSTRING& strLightMapName, 
								LIGHTMAPUV* pLightMapUV, 
								DWORD dwTextureID );

	struct VERTEXDATA
	{
		BOOL		bEqual;
		DWORD		dwNUM;
		DWORD		dwNUM_EX;
	};

protected:
	POINT*			m_pPoint;

	DWORD			m_dwPointEX;
	POINTEX*		m_pPointEX;

	DWORD			m_dwPointEX_Load;
	POINTEX_PREV*	m_pPointEX_Load;

	TRIANGLE*		m_pTriangle;

	DWORD			m_dwMaterials;
	DXMATERIAL*		m_pMaterials;

    LightMapObject_Data*            m_pLightMapData;    // LightMap Data

protected:
	DWORD			m_dwCurPOINT;			// 현재 셋팅할 점 번호
	DWORD			m_dwCurPOINT_PREV;		// 예전에 선택된 점 번호
	TRIANGLELIST	m_listSelectTriangle;	// 선택된 삼각형을 표시 나도록 해주기 위해서.

protected:
	DWORD			m_dwCurTILE;			// 현재 셋팅할 타일 번호 (텍스쳐 종류)
	D3DXCOLOR		m_cCurCOLOR;			// 현재 셋팅할 점 색
	D3DXCOLOR		m_cCurRANDOM;			// 현재 셋팅할 점 색의 랜덤성을 주기 위해서.
	BOOL			m_bSelectDRAW;			// 강조를 할 것인가 ?

public:
    typedef std::map<DWORD,std::tr1::shared_ptr<MATERIALEX>>	MATERIALMAP;
	typedef MATERIALMAP::iterator					        MATERIALMAP_ITER;

	MATERIALMAP		m_mapMaterials;		// 블렌딩에 쓰이는 삼각형

protected:
	struct FINDEX
	{
		DWORD		dwEX;
		D3DXVECTOR3 vPos;
	};
	typedef std::multimap<float,FINDEX>			FINDEXMAP;
	typedef FINDEXMAP::iterator					FINDEXMAP_ITER;

	FINDEXMAP			m_mapFindEX;

protected:
	virtual HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );
public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup();

//public:
//    virtual void CreateLightMap2( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff, BOOL bCreatLightMap, const TSTRING& strLightMapName );
//    virtual void LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
    virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

	HRESULT Render_Select ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCenter );
    HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan, DxTextureEffMan* pTextureEff );

protected:
	//		생	성	부	분
	void	SetBaseQuad( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );

	void	SetPointData ( VERTEXNORTEXTANGENT* pVertices, WORD* pIndices );
	void	SetTriangleData ( LPD3DXMESH pSrcMesh );
    void	SetMaterial ( DxMeshes *pmsMeshs );

	BOOL	DxFindPoint ( D3DXVECTOR3 vPos, DWORD _dwIndex );

	BOOL	CheckTriangleUSE ( MATERIALMAP_ITER& iter, DWORD dwFaceNUM );

	void	CreateTiles ( LPDIRECT3DDEVICEQ pd3dDevice, LIGHTMAPUV* pLightMapUV );
	void	CreateTile ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALMAP_ITER& iterMaterial, LIGHTMAPUV* pLightMapUV );

	void	OneSrcTriangle ( VERTEXDATA* pData, const MATERIALEX* pMaterialEX, FACELIST& pRoot, LIGHTMAPUV* pLightMapUV );
	void	TwoSrcTriangle ( VERTEXDATA* pData, const MATERIALEX* pMaterialEX, FACELIST& pRoot, LIGHTMAPUV* pLightMapUV );
	void	ThrSrcTriangle ( VERTEXDATA* pData, const MATERIALEX* pMaterialEX, FACELIST& pRoot, LIGHTMAPUV* pLightMapUV );
    void	SetSeparateList ( FACELIST& pRoot, const vm::VERTEXNORCOLORTEXTANGENT& sVertex_0, 
                            const vm::VERTEXNORCOLORTEXTANGENT& sVertex_1, const vm::VERTEXNORCOLORTEXTANGENT& sVertex_2, D3DXVECTOR2 avLightMapUV[] );

	void	BaseTileSetting ();

	void	BlendTileSetting ();

	void	RenderTiles ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALMAP& mapMaterials, DxTextureEffMan* pTextureEff );
	void	RenderTiles ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX, DxTextureEffMan* pTextureEff );

public:
	BOOL	IsTriangle ( D3DXVECTOR3 vPickRayOrig, D3DXVECTOR3 vPickRayDir );
	void	SetListBox ( LPDIRECT3DDEVICEQ pd3dDevice );

	void	TileEdit ( D3DXVECTOR3 vPickRayOrig, D3DXVECTOR3 vPickRayDir );
	void	DxLeftMouseClick ();
	void	DeleteClick ();

public:
	char*	GetStrList( DWORD i );
	BOOL	SetStrList( LPDIRECT3DDEVICEQ pd3dDevice, const char* cName );

	void		SetCurMaterial ( DWORD i )				{ m_dwCurTILE=i; }			// 선택된 Material 
	DWORD		GetCurMaterial ()						{ return m_dwCurTILE; }		//
	void		SetCurColor ( D3DXCOLOR cColor )		{ m_cCurCOLOR=cColor; }		// 색으로 칠하기. 
	D3DXCOLOR	GetCurColor ()							{ return m_cCurCOLOR; }		//
	void		SetColorRandom ( D3DXCOLOR cRandom )	{ m_cCurRANDOM=cRandom; }	// 색 랜덤성
	D3DXCOLOR	GetColorRandom ()						{ return m_cCurRANDOM; }	//
	void		SetSelectDraw ( BOOL bUse )				{ m_bSelectDRAW=bUse; }		// 선택된 Material 강조하기.
	BOOL		GetSelectDraw ()						{ return m_bSelectDRAW; }	//

	void DeleteColorALL();

	// 정렬을 위한 함수
	bool		SortRenderPriority( const DxEffectTiling* effect ) const
	{
		return m_dwRenderPriority > effect->m_dwRenderPriority;
	}

	void ClonePointEX( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMakeTriangle );

public:
    friend class CTilingEffEdit;
    friend class CommandTiling_TileChange;
    friend class CommandTiling_MaterialChange;
	
public:
	DxEffectTiling ();
	virtual ~DxEffectTiling ();

protected:
	virtual void SaveBuffer ( sc::SerialFile &SFile );
	virtual void LoadBufferSet ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	void LoadBuffer_NEW( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bDummy ) override;

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	//virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEff );
    virtual void CloneDataLightMap( DxEffectBase* pSrcEffect, 
                                    LPDIRECT3DDEVICEQ pd3dDevice, 
                                    DxTextureEffMan* pTexEff, 
                                    const TSTRING& strLightMapName, 
									VEC_LIGHTMAPUV_MAIN& vecLightMapUV_MAIN );

public:
	void SetNSLandThread() override;
};

#endif // !defined(__DXEFFECTTILING__INCLUDED__)
