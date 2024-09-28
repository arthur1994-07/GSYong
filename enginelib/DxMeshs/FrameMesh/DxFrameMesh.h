// DxFrameMesh.h: interface for the DxFrameMesh class.
//
//
//
//
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXFRAMEMESH_H__ED2134D1_E13E_4E71_8619_C435D1E2403B__INCLUDED_)
#define AFX_DXFRAMEMESH_H__ED2134D1_E13E_4E71_8619_C435D1E2403B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <boost/scoped_ptr.hpp>
#include <boost/tr1/memory.hpp>

#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxObject.h"
#include "../../DxTools/DxCustomTypes.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxEffect/Frame/DxEffectFrame.h"
#include "../../DxLand/DxOctree/DxOctreeMesh.h"
#include "../../DxLand/LightMap/LightMapDef.h"
#include "./DxAnimationManager.h"

namespace sc {
    class SerialFile;
}

class   LightMapObject_Data;
struct	DxEffFrameBase;
class	DxEffectBase;
struct	DXPIECEOBJ;
struct	DXPIECEOBJNODE;
struct	TEXEFF_PROPERTY;
class	DxTextureEffMan;
class   DxTangentMesh;
class	MaterialMeshArray;
class CMaterialSetting;
class DxStaticMeshCollBase;
class DxPieceManager;

//	Note : ���͸��� (Attrib Table) ���� �ٸ� ������ ������ �ϱ� ���ؼ�.
//
enum DXMTRLTYPE
{
	MATERIAL_NORMAL			= 0,
	MATERIAL_ALPHA			= 1,
	MATERIAL_ALPHA_NOZWRI	= 2,
	MATERIAL_ALPHA_NOTEST	= 3,
	MATERIAL_ALPHA_HARD		= 4,
	MATERIAL_ALPHA_SOFT		= 5,

	MATERIAL_NSIZE			= 6
};

struct D3DEXMATERIAL_100
{

	DXMTRLTYPE	MtrlType;
	BOOL		bSubSetTriRend;

	D3DEXMATERIAL_100 () :
	MtrlType(MATERIAL_NORMAL)
	{
		bSubSetTriRend = FALSE;
	}

	static char szMtrlTypes[MATERIAL_NSIZE][24];
	static DWORD VERSION;
};

struct D3DEXMATERIAL
{

	DXMTRLTYPE	MtrlType;
	BOOL		bSubSetTriRend;
	BOOL        bCamColl;

	D3DEXMATERIAL () :
		MtrlType(MATERIAL_NORMAL)
	{
		bSubSetTriRend = FALSE;
		bCamColl       = FALSE;
	}

	static char szMtrlTypes[MATERIAL_NSIZE][24];
	static DWORD VERSION;
};

enum MESH_METERIAL
{
	EM_MESH_MATERIAL = 0,
	EM_MESH_MATERIAL_RED = 1,
};

struct DATA_FOR_TRIANGLE_LINK_LIGHT
{
	VERTEXNOR			m_sVertexNor;	// ����
	DWORD				m_dwFaceID;		// ���ؽ� ����
};

struct DATA_FOR_TRIANGLE_LINK_LIGHT_SORT
{
	bool operator() ( const DATA_FOR_TRIANGLE_LINK_LIGHT &lvalue, const DATA_FOR_TRIANGLE_LINK_LIGHT &rvalue )
	{
		return bool( lvalue.m_sVertexNor.vPos.x < rvalue.m_sVertexNor.vPos.x );
	}
};

struct DATA_FOR_TRIANGLE_LINK_LIGHT_FIND
{
	float m_fPoint;
	DATA_FOR_TRIANGLE_LINK_LIGHT_FIND( float fPoint ) 
		: m_fPoint(fPoint) 
	{
	};

	bool operator() ( const DATA_FOR_TRIANGLE_LINK_LIGHT &value )
	{
		if ( m_fPoint + 0.001f > value.m_sVertexNor.vPos.x &&
			m_fPoint < value.m_sVertexNor.vPos.x + 0.001f )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct DxMeshes
{
private:
    enum
    {
        VERSION = 0x103,
		VERSION_B = 0x100,
    };

public:
	static BOOL					g_bAlphaUP;
	static float				g_fAlpha;
	static MESH_METERIAL		g_emMeshMeterial;		// ��Ʈ���� ���� ����.

public:
	static void FrameMove( float fElapsedTime );

public:
	char				*m_szName;

	LPD3DXMESH			m_pSysMemMesh;
	LPD3DXMESH			m_pLocalMesh;
	DxOctreeMesh		*m_pDxOctreeMesh;

	DWORD					m_dwVert;
	DWORD					m_dwFace;
	DWORD					m_dwAttrib;
	D3DXVECTOR3				m_vMax;
	D3DXVECTOR3				m_vMin;

	VERTEX*					m_pVertSrc;
	LPDIRECT3DVERTEXBUFFERQ	m_pEffVB;
	LPDIRECT3DINDEXBUFFERQ	m_pEffIB;
	LPD3DXATTRIBUTERANGE	m_pAttrib;

    std::tr1::shared_ptr<DxTangentMesh>    m_spTangentMesh;

    VEC_LIGHTMAPOBJ			m_vecLightMapObject;
	std::vector<VEC_DWORD>	m_vecTriangle_vecLightID;	// �� �ﰢ������ ���� �޴� Light�� ���ϵ��� �Ѵ�. - �߰��� ( ����޴°� �� �޴´�. )
	std::vector<WORDWORD>	m_vecTriangle_wwLightID;	// �� �ﰢ������ ���� �޴� Light�� ���ϵ��� �Ѵ�. - ������ ( �ִ� 2���� ��󳽴�. )

	DWORD				m_cMaterials;
	D3DMATERIALQ		*m_rgMaterials;
	D3DEXMATERIAL		*m_exMaterials;
	TEXEFF_PROPERTY		*m_pTexEff;
	TextureResource		*m_pTextureRes;
	TextureResource		*m_pMaskTexRes;
	CString				*m_strTextureFiles;
	std::tr1::shared_ptr<MaterialMeshArray>    m_spMaterialMeshArray;
	VEC_TSTRING			m_vecMaterialName;		// Material �̸�

    // Note : ����Ʈ�� Ÿ��. & Detail
	BOOL				m_bEnableLightMap;				// mat Format�� �� �� ���, LightMap �� ���� �� ���� �˾ƾ� �ϱ� ������ �ʿ���.
	DWORD				m_dwLightMapIndex;
    float               m_fLightMapDetail;				// 
	WORDWORD			m_wwLightMapUserUV_TextureSize;	// UserUV�� TextureSize.

	BOOL				m_bVertexColor;

	DxMeshes			*m_pMeshNext;

public:
	DxMeshes();
	~DxMeshes();

public:
	BOOL MakeEffectMeshs( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF );

	HRESULT MakeLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF );
	HRESULT SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF );
    HRESULT CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice );              // TangentMesh�� �����.
    HRESULT CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice );            // ����ȭ�� TangetMesh�� �����.

	void CalculateMaxMin();

public:
    LightMapObject_Data             GetLightMapData();  // ����Ʈ�� ����
	HRESULT ExportLightMapUV( MAP_LIGHTMAPUV& mapLightMapUV, CMaterialSetting* pMaterialSetting );

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RenderAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bShadow=FALSE );
	
	HRESULT	RenderOctree( LPDIRECT3DDEVICEQ pd3dDevice, const float fAlpha = 1.f );
	HRESULT	RenderOctreeAlphaMap( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bShadow=FALSE, const float fAlpha = 1.f );

	HRESULT RenderTriangle ( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL RenderLightMapChecker( LPDIRECT3DDEVICEQ pd3dDevice, WORDWORD wwLightMapUserUV_TextureSize );
	BOOL RenderLightMapCheckerInexact( LPDIRECT3DDEVICEQ pd3dDevice );

    HRESULT RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXDECLARATION9 pDCRT );
    void DrawMeshFX( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, LPDIRECT3DVERTEXDECLARATION9 pDCRT );

	// TriangleLinkLight
public:
	// Field�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
	void CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager,
									SET_DWORD& setEraseLightID,
									std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT>& vecDataForTriangleLinkLight,
									std::vector<VEC_DWORD>& vecTriangle_vecLightID,
									const D3DXMATRIX& matWorld,
									BOOL bDirectionLight );

	// Triangle Link Light - ����� �ﰢ�� Light �����ϱ�
	void PermeateTriangleLinkLight( std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT>& vecDataForTriangleLinkLight,
									std::vector<VEC_DWORD>& vecTriangle_vecLightID,
									const D3DXMATRIX& matWorld );

	//  ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� �ڸ��� ������ Ȯ��.
	BOOL IsTriangleLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot );

	// ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� ���� LightID �� ����.
	void InsertSlotTriangleLightID( DWORD dwLightID, DWORD dwSlot );

	// m_vecTriangle_vecLightID �� m_vecTriangle_wwLightID �� ���Ͽ� m_vecTriangle_wwLightID �� ���� ���� ������ �� �ְ� dwLightID �� �ѱ��.
	BOOL FindTriangleLimitLightNumOver3( DWORD& dwLightID ) const;

	// Ư�� Light�� �ٿ��ֵ��� �Ѵ�.
	void EraseLightID( DWORD dwLightID );

public:
	BOOL		SaveFile ( sc::SerialFile &SFile );
	BOOL		LoadFile_NEW ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL		LoadFile_OLD ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void		SaveSet( sc::SerialFile &SFile );
    void        LoadSet( sc::SerialFile &SFile );

	//	Note : �浹 üũ
	//
public:
	void		IsCollision_DxMeshes ( const D3DXMATRIX &matCom, const D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3* pNor, D3DXVECTOR3 &vCollision, 
							BOOL &bCollision, char* pName, EMCOLLMODE emCullMode );
	void		GetClosedPointIndex ( const D3DXMATRIX &matCom, const D3DXVECTOR3 &vCollision, DWORD &dwIndex, D3DXVECTOR3 &vVert );
};

#define __DXFRAMEROOT	("__ROOT")

struct CLIPVOLUME;
struct DxAnimation;
class  DxOctree;
class DxFrameMesh;

enum FRAMETYPES
{
	FRAME_COMMON,
	FRAME_DOOR
};

#define DXFRM_ALPHAMAP	   (0x00000001)
#define DXFRM_NAVIFRAME	   (0x00000010)

struct DxFrame
{
    enum
    {
        VERSION = 0x100,
		VERSION_B = 0x100,
    };

	typedef DxFrame* PDXFRAME;

	//	Note : ���� �ɹ�..
	//
	static DWORD	dwFrameTypeNum;		//	������ Ÿ�� ����.
	static char		szFrameType[2][24];	//	������ Ÿ�� ���ڿ�.	- editer ��.


	//	Note : Frame �з�.
	//
	char*			szName;				//	������ �̸�.
	FRAMETYPES		eFrameType;			//	������ Ÿ��.

	//	Note : ������ ����.
	//
	DWORD			bFlag;

	//	Note : �޽� ����.
	//
	DxMeshes		*pmsMeshs;

	union
	{
		struct { D3DXVECTOR3 vTreeMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 vTreeMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	DXPIECEOBJ*		pRePlacePiece;		//	ġȯ ����.
	BOOL			bPieceUSE;			// DxPiece�� ���� ���̴�. DXPIECEOBJ ���� Ʋ�� �༮��.

	//	Note : Ʈ������ ����.
	//
	D3DXMATRIX		matRot;				//	�ش�ð�(���ϰ� ������)�� Ʈ������.
	D3DXMATRIX		matRotOrig;			//	���� �������� �ƴ� ���� Ʈ������.
	D3DXMATRIX		matCombined;		//	�θ� ���� ������ ���� Ʈ������.

	DxAnimation		*pframeFromAnimate;	//	�� �������� �����ϴ� ���ϸ��̼�.
	BOOL			bParentMatrixUSE;	// ����� ���� �ʰ� �θ��� Matrix�� �׳� �ᵵ �Ǵ� �� �ΰ�?

	//	Note : ����Ʈ ó����.
	//
	DxEffectBase	*pEffectPrev;
	DxEffectBase	*pEffect;
	DxEffectBase	*pEffectNext;

	DxEffFrameBase	*pEffFrame;

	DxFrame			*pNextAlphaMapFrm;

	//	Note : �ڽ� ��� �� ���� ��� ����.
	//
	DxFrame			*pframeSibling;		//	���� ����
	DxFrame			*pframeFirstChild;	//	�ڽ� ����

	DxFrame () :
		pmsMeshs(NULL),
		bFlag(NULL),
		eFrameType(FRAME_COMMON),
		vTreeMax(0,0,0),
		vTreeMin(0,0,0),
		pRePlacePiece(NULL),
		pframeFromAnimate(NULL),
		pframeSibling(NULL),
		pframeFirstChild(NULL),
		szName(NULL),
		pEffectPrev(NULL),
		pEffect(NULL),
		pEffectNext(NULL),
		pEffFrame(NULL),
		pNextAlphaMapFrm(NULL),
		bPieceUSE(FALSE)
	{
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixIdentity(&matRotOrig);
		D3DXMatrixIdentity(&matCombined);
	}

	DxFrame::~DxFrame ();

	void AddFrame ( DxFrame *pframe )
	{
		if ( pframeFirstChild == NULL )
		{
			pframeFirstChild = pframe;
		}
		else
		{
			pframe->pframeSibling = pframeFirstChild->pframeSibling;
			pframeFirstChild->pframeSibling = pframe;
		}
	}

	void AddMesh ( DxMeshes *pms )
	{
		pms->m_pMeshNext = pmsMeshs;
		pmsMeshs = pms;
	}

	DxFrame* FindFrame ( const char *szFrame )
	{
		DxFrame *pframe;
		if ( ( szName != NULL ) && ( strcmp ( szName, szFrame ) == 0 ) )
			return this;

		if ( pframeFirstChild != NULL )
		{
			pframe = pframeFirstChild->FindFrame ( szFrame );
			if ( pframe != NULL )	return pframe;
		}
		
		if ( pframeSibling != NULL )
		{
			pframe = pframeSibling->FindFrame ( szFrame );
			if ( pframe != NULL )	return pframe;
		}

		return NULL;
	}

	DxFrame* FindParentFrame ( DxFrame* pDxFindFrame )
	{
		DxFrame *pframeChild = pframeFirstChild;
		while ( pframeChild != NULL )
		{
			if ( pframeChild == pDxFindFrame )	return this;

			DxFrame *pframe = pframeChild->FindParentFrame ( pDxFindFrame );
			if ( pframe != NULL ) return pframe;

			pframeChild = pframeChild->pframeSibling;
		}

		return NULL;
	}

public:
	BOOL	                        IsAlphaMapMesh ();
	BOOL	                        IsReplace ();	    // ȿ����ü�� ġȯ ���� ���� ��ü���� ���� ��� ��� �ؾ���.
    float                           GetLightMapDetail();// ����Ʈ�� ������ ����
    LightMapObject_Data             GetLightMapData();  // ����Ʈ�� ����

public:
	D3DXVECTOR3		GetTreeCenter() { return (vTreeMax+vTreeMin)*(0.5f); };
	float			GetTreeXWidth() { return vTreeMax.x-vTreeMin.x; };
	float			GetTreeYWidth() { return vTreeMax.y-vTreeMin.y; };
	float			GetTreeZWidth() { return vTreeMax.z-vTreeMin.z; };

public:
	HRESULT CalculateBoundingBox ( D3DXMATRIX *pmatCur, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, float fUnitTime, BOOL bAnimate=FALSE );
	HRESULT CalculateBoundingBoxKey ( D3DXMATRIX *pmatCur, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, float dwTime );
	HRESULT CalculateBoundingSphere ( D3DXMATRIX *pmatCur, float fUnitTime );

public:
	void		GetAllNodeData ( DWORD* pIndices, DWORD* pPrevIndexPos,
					VERTEX_WORLD* pVertices, DWORD* pPrevVertexPos,
					DxFrame** ppDxFrames, DWORD* pPrevDxFramePos,
					DxFrame* pDxFrameThis, bool breplace_in, const BOOL bEdit );
	HRESULT		FrameMeshToOctree	( DxOctree *pDxOctree, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax, LPDIRECT3DDEVICEQ pd3dDevice, int debug, int debug2 );

public:
	BOOL		SaveFile ( sc::SerialFile &SFile );
	BOOL		LoadFile_NEW ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, PDXFRAME &pAlphaMapHead );
	BOOL		LoadFile_OLD( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, PDXFRAME &pAlphaMapHead );
	DxEffectBase* LoadEffect ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void		SaveSet ( sc::SerialFile &SFile );
    void		LoadSet ( sc::SerialFile &SFile, DWORD nNodeCount );

protected:	
	//	Note : Octree�� �ޱ� ���� DxFrame�� �Ʒ��� �޼ҵ带 �̿��ؼ�
	//		   ��ȯ�ؼ� ����(��带 ����)�ϴ� �Լ�
	HRESULT		DxFrameNodeCopy ( DxFrame *pDst, DxFrame *pSrc, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax,
		LPDIRECT3DDEVICEQ pd3dDevice, char *pParentName );

	//	Note : DxMeshes�� ID3DXMESH�� DxOctreeMesh�� �ٲ��ִ� �ֿ� �޽��	
	HRESULT		ConvertMesh		( DxMeshes **ppDstMesh, DxMeshes *pSrcMesh, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT		GetMeshRawData	( BYTE	**ppIndices, BOOL b32BitMesh, VERTEX_WORLD	**ppVertices, LPD3DXMESH pMeshData, int NumOfIndices, int NumOfVertices );
	HRESULT		CheckInTheBox	( int *pNumOfInBoxFaces, BOOL **ppbInBoxFaces,
										int *pNumOfInBoxVertices, BOOL **ppbInBoxVertices,
										BYTE *pIndices, BOOL b32BitMesh, VERTEX_WORLD *pVertices,
										int NumOfIndices, int NumOfVertices, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax );

	HRESULT		MakeOctreeMeshData ( DxMeshes **ppDstMesh, DxMeshes *pSrcMesh,
										 int NumOfInBoxFaces, BOOL *pbInBoxFaces,
										 int NumOfInBoxVertices, BOOL *pbInBoxVertices,
										 BYTE *pIndices, BOOL b32BitMesh, VERTEX_WORLD	*pVertices,
										 LPDIRECT3DDEVICEQ pd3dDevice );

	void		DelMeshRawData ( BYTE *pIndices, BOOL b32BitMesh, VERTEX_WORLD *pVertices, BOOL *pbInBoxFaces, BOOL *pbInBoxVertices );	

public:
	//	Note : ���ϸ��̼� Frame ����.
	//
	HRESULT		AniFrameTreeCopy ( DxFrame *pDst, DxFrame *pSrc, DxAnimationMan *pDxAnimationMan,
								LPDIRECT3DDEVICEQ pd3dDevice, char *pParentName );
	HRESULT		GetAniFrameMesh ( LPDXANIMATIONMAN &pDxAnimationMan,
								float fUNITTIME, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : �浹 üũ
	//
public:
	BOOL		IsCollision ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXMATRIX* pMat=NULL ) const;
	BOOL		BoxIsInTheFrustum ( const D3DXMATRIX* pMat, const CLIPVOLUME &cv, LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL		CollisionDetect_NoneConst( const D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3* pNor, D3DXVECTOR3 &vCollision, char* pName, EMCOLLMODE emCullMode );

protected:
	void		CollisionDetect_NoneConst( DxFrame *pframeCur, const D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3* pNor,
								BOOL &bCollision, D3DXVECTOR3 &vCollision, char* pName, EMCOLLMODE emCullMode );

public:	
	void		DxFrameNodeCount ( DWORD &nNodeCount );
};

typedef DxFrame* LPDXFRAME;

class DxFrameMesh : public DxObject
{
public:
	static BOOL	m_bNaviDraw;
	static DWORD VERSION;

	//	Note : ���� ����.
protected:
	BOOL		m_bMeshOptimize;
	DWORD		m_dwFVF;
	DxFrame*	m_pDxFrame;
	char*		m_pszFileName;
	TSTRING		m_strPath;

	union
	{
		struct { D3DXVECTOR3	vTreeMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3	vTreeMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	DWORD		m_dwVertices;	//	�� ���ؽ���.
	DWORD		m_dwTriangles;	//	�� ���̽���.

	//	Note : Alpha Map ������ ���.
	DxFrame			*m_pDxAlphaMapFrameHead;

	//	Note : ġȯ ���� ����.
	DXPIECEOBJ		*m_pPieceObjHead;	//	ġȯ ���� ����Ʈ.
	DXPIECEOBJNODE	*m_pPieceObjTree;	//	ġȯ ���� Ʈ��.


	//	Note : ���ϸ���Ʈ
protected:
	float				fCurTime;		//	���ϸ���Ʈ �ð�.
	float				m_UNITTIME;		//	���ϸ��̼� ���� �ð�.
	DxAnimation*		pAnimHead;		//	���ϸ��̼� ������ ���.
	DxAnimationMan*		pAnimManHead;	//	���ϸ��̼� ��� ������ ����.

	//	Note : ����Ʈ ó����.
	DxEffectBase*		m_pEffectHead;

	//	Note : �浹 üũ
protected:
	D3DXVECTOR3			m_vCollionPos;
	LPDXFRAME			m_pCollDetectFrame;	//	�浹 üũ�� ����� FRAME.

	// Note : TextureEff
protected:
	DxTextureEffMan*	m_pTextureEff;

	// Material
private:
	boost::scoped_ptr<CMaterialSetting>	m_scpMaterialSetting;

	// ������ ������. BoxIsInTheFrustum �˻� �ѹ��� �˻�, �� ������ Rendering �� �����ϰ� �ϱ� ����.
private:
	std::vector<DxFrame*> m_vecframeCur;

public:
	const CMaterialSetting* GetMaterialSetting_const()	{ return m_scpMaterialSetting.get(); }
	CMaterialSetting* GetMaterialSetting()				{ return m_scpMaterialSetting.get(); }

	// Etc
protected:
	void CollisionDetect_NoneConst( DxFrame *pframeCur, const D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3* pNor, BOOL &bCollision, char* pName, EMCOLLMODE emCullMode );

public:
	BOOL IsCollision_NoneConst( const D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3* pNor, char* pName, EMCOLLMODE emCullMode );
	LPDXFRAME const		GetCollisionDetectedFrame ()	{ return m_pCollDetectFrame; }
	void				SetCollisionDetectedFrame ( LPDXFRAME pDxFrame )	{ m_pCollDetectFrame = pDxFrame; }
	const D3DXVECTOR3	GetCollisionPos ()				{ return m_vCollionPos; }

public:
	DxEffectBase*		GetEffectList () { return m_pEffectHead; }

	void AddEffect ( DxEffectBase *pEffectAdd );
	BOOL DelEffect ( DxFrame *pDxFrame, DxEffectBase *pEffectDel );

public:
	void AddAlphaMapFrameAll ( DxFrame *pDxFrame );
	void DelAlphaMapFrameAll ( DxFrame *pDxFrame );

	void AddAlphaMapFrame ( DxFrame *pDxFrame );
	void DelAlphaMapFrame ( DxFrame *pDxFrame );

public:
	BOOL AddPieceObj ( char *szFrameName, const char *szPieceName );
	void DelPieceObj ( char *szFrameName );

	DXPIECEOBJ* GetPieceList () { return m_pPieceObjHead; }

	void MakePieceAABBTree ();
	void CleanPieceAABBTree ();

public:
	void ModiflyAlphaMapFrame ( DxFrame *pDxFrame )
	{
		if ( pDxFrame->IsAlphaMapMesh() )
		{
			pDxFrame->bFlag |= DXFRM_ALPHAMAP;
			AddAlphaMapFrame ( pDxFrame );
		}
		else
		{
			pDxFrame->bFlag &= ~DXFRM_ALPHAMAP;
			DelAlphaMapFrame ( pDxFrame );
		}
	}

public:
	void			SetFVF ( DWORD dwFVF, BOOL bNow=FALSE, LPDIRECT3DDEVICEQ pd3dDevice=NULL );
	HRESULT			SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, DWORD *pdwFVF = NULL );

public:
	void			SetCurTime ( float fCur ) { fCurTime = fCur; }

public:
	char*			GetFileName () { return m_pszFileName; }
	DxFrame*		GetFrameRoot () { return m_pDxFrame; }
	DWORD			GetVertices()	{ return m_dwVertices; }
	DWORD			GetTriangles()	{ return m_dwTriangles;}

	float			GetUnitTime ()	{ return m_UNITTIME; }
	float			GetCurTime()	{ return fCurTime; }

public:
	const D3DXVECTOR3&	GetTreeMax()	{ return vTreeMax; }
	const D3DXVECTOR3&	GetTreeMin()	{ return vTreeMin; }
	const D3DXVECTOR3	GetTreeCenter() { return (vTreeMax+vTreeMin)*(0.5f); };
	float				GetTreeXWidth() { return vTreeMax.x-vTreeMin.x; };
	float				GetTreeYWidth() { return vTreeMax.y-vTreeMin.y; };
	float				GetTreeZWidth() { return vTreeMax.z-vTreeMin.z; };

protected:
	void AddAnimationFrame ( DxAnimation *pAnim )
	{
		pAnim->pAnimNext = pAnimHead;
		pAnimHead = pAnim;
	}

	void MakeAnimationList ();
	void MakeAnimationList ( DxFrame * pDxFrame );

	// Key Animation �� �ϳ��� �ִ� ���� �ִϸ��̼� ����.
	void OneKeyAnimationLinkOff();

public:
	DxAnimation* GetAnimationList ()	{ return pAnimHead; }
	DxAnimation* FindAnimation ( char *szName )
	{
		DxAnimation* pCur = pAnimHead;
		while ( pCur )
		{
			if ( !strcmp(pCur->szName,szName) )
			{
				return pCur;
			}

			pCur = pCur->pAnimNext;
		}

		return NULL;
	}

	DWORD GetAniManCount ()
	{
		DWORD	nCount = 0;
		DxAnimationMan*		pDxAniMan = pAnimManHead;
		while ( pDxAniMan )
		{
			nCount++;
			pDxAniMan = pDxAniMan->m_pNext;
		}
		return nCount;
	}
	DxAnimationMan* FindAniRootFrame ( DxFrame *pDxFrame ); // ������ ��ɿ��� �ʿ�.

	DxFrame* GetAniFrame( const char*szFrameName );

    // DxTextureEffMan
public:
	DxTextureEffMan* GetTextureEffMan()		                { return m_pTextureEff; }
    const DxTextureEffMan* GetTextureEffMan_const() const   { return m_pTextureEff; }
    void    ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxFrame* FindFrame ( const char *szName )
	{
		if ( m_pDxFrame == NULL )	return NULL;
		else						return m_pDxFrame->FindFrame ( szName );
	}

	DxFrame* FindParentFrame ( DxFrame* pDxFrame )
	{
		if ( m_pDxFrame == NULL || pDxFrame == NULL )	return NULL;
		else	return m_pDxFrame->FindParentFrame ( pDxFrame );
	}

	void CalculateBoundingSphere ()
	{
		D3DXMATRIX mCur;
		D3DXMatrixIdentity ( &mCur );
		m_pDxFrame->CalculateBoundingSphere ( &mCur, m_UNITTIME );

		vTreeMax = m_pDxFrame->vTreeMax;
		vTreeMin = m_pDxFrame->vTreeMin;
	}

protected:
	HRESULT LoadMeshHierarchy ( LPDIRECT3DDEVICEQ pd3dDevice );
	
	HRESULT LoadMesh ( LPD3DXFILEDATA pxofobjCur, DWORD fvf, LPDIRECT3DDEVICEQ pD3DDevice,
					DxFrame *pframeParent);
	HRESULT LoadFrames ( LPD3DXFILEDATA pxofobjCur, DWORD fvf, LPDIRECT3DDEVICEQ pD3DDevice,
					DxFrame *pframeParent);
	HRESULT LoadAnimation ( LPD3DXFILEDATA pxofobjCur, DWORD fvf, LPDIRECT3DDEVICEQ pD3DDevice );
	HRESULT LoadAnimationSet ( LPD3DXFILEDATA pxofobjCur, DWORD fvf, LPDIRECT3DDEVICEQ pD3DDevice );

	HRESULT MakeLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur );
	HRESULT DeleteLocalMeshs ( DxFrame *pframeCur );

private:
    HRESULT CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur );
    HRESULT CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur );
    HRESULT RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
                                DxFrame *pframeCur, 
                                ID3DXEffect* pFX,
                                LPDIRECT3DVERTEXDECLARATION9 pDCRT,
                                const D3DXMATRIX& matView, 
                                const D3DXMATRIX& matProj );

public:
    virtual BOOL ExistXFile()                                         { return GetFrameRoot() ? TRUE : FALSE; }
    virtual HRESULT CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual HRESULT CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual HRESULT RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
                                            ID3DXEffect* pFX,
                                            LPDIRECT3DVERTEXDECLARATION9 pDCRT,
                                            const D3DXMATRIX& matView, 
                                            const D3DXMATRIX& matProj );

public:
	HRESULT ExportLightMapUV( MAP_LIGHTMAPUV& mapLightMapUV, DxFrame *pframeCur );
	void CheckLightMapUV( DxFrame *pframeCur );

	// TriangleLinkLight
public:
	// Field�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
	void CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager,
									SET_DWORD& setEraseLightID,
									BOOL bDirectionLight );

	//  ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� �ڸ��� ������ Ȯ��.
	BOOL IsTriangleLightID_InsertSlot( DWORD dwLightID, DxFrame *pframeCur, DWORD dwSlot );

	// ���� ����Ʈ�� ������ �ﰢ������ Ư�� ���Կ� ���� LightID �� ����.
	void InsertSlotTriangleLightID( DWORD dwLightID, DxFrame *pframeCur, DWORD dwSlot );

	// Triangle Link Light - Light�� 3�� �̻��� �� ã�� �� Light ���� 2���� ����
	void EraseLightID4LightOver3( SET_DWORD& setEraseLightID );

	// setEraseLightID �� �������� FrameMesh, �� LightID�� ���� �ϵ��� �Ѵ�.
	void EraseLightID4TriangleLinkLight( const SET_DWORD& setEraseLightID );

	// TriangleLinkLight
private:
	// Field�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
	void CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager,
									SET_DWORD& setEraseLightID,												// OUT
									std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT>& vecDataForTriangleLinkLight,	// OUT
									std::vector<VEC_DWORD>& vecTriangle_vecLightID,							// OUT
									DxFrame *pframeCur,
									BOOL bDirectionLight );

	// Triangle Link Light - ����� �ﰢ�� Light �����ϱ�
	void PermeateTriangleLinkLight( std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT>& vecDataForTriangleLinkLight,
									std::vector<VEC_DWORD>& vecTriangle_vecLightID, 
									DxFrame *pframeCur );

	// m_vecTriangle_vecLightID �� m_vecTriangle_wwLightID �� ���Ͽ� m_vecTriangle_wwLightID �� ���� ���� ������ �� �ְ� dwLightID �� �ѱ��.
	BOOL FindTriangleLimitLightNumOver3( DWORD& dwLightID, DxFrame *pframeCur ) const;

	// Ư�� Light�� �ٿ��ֵ��� �Ѵ�.
	void EraseLightID( DWORD dwLightID, DxFrame *pframeCur );

protected:
	float m_fTime, m_fElapsedTime;

protected:
	HRESULT UpdateFrames ( DxFrame *pframeCur, D3DXMATRIX &matCur );
	HRESULT DrawFrames ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld );
	HRESULT DrawFrames_RenderAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld );
	HRESULT DrawFrames_Decal1 ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld );
	HRESULT DrawFrames_Decal2 ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld );
	HRESULT DrawFrames_Decal3 ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld );
	HRESULT RenderWaterCustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld );
	HRESULT DrawFrames_NOEFF ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV );
	HRESULT DrawFrames_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV );

	HRESULT DrawAlphaMapFrames ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, BOOL bRefelct=FALSE );

public:
	HRESULT DrawSelectedFrame ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, const D3DXMATRIX* pmatWorld );	//	editer ��.
	HRESULT DrawSelectedMesh  ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV );	//	editer ��.
	HRESULT DrawSelectedPoint ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vVert );	//	editer ��. 
    HRESULT DrawWireFrame ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV );        //	editer ��. 

protected:
	void    DrawSelectedMesh1 ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, DxFrame* pCurFrame ); //	editer ��.

public:
	void SetFile ( const char* szPath, const char *szFileName );

	virtual HRESULT OneTimeSceneInit();
	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);
	virtual HRESULT InvalidateDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT FinalCleanup();

	void	SetPieceUseFALSE();
	void	SetPieceUseChild( DxFrame* pFrame, const BOOL bUse );
	void	AddPiece2( const char* pName );
	void	DelPiece2( const char* pName );
	void	SetAnimationTimeZero();
	HRESULT FrameMovePiece ( float fTime, float fElapsedTime );
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render_Independence( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, D3DMATERIAL9* pMaterial = NULL, LPDIRECT3DTEXTUREQ pTexture = NULL );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX *pWorld, const CLIPVOLUME *pCV );
	HRESULT Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX *pWorld, const CLIPVOLUME *pCV );
	HRESULT Render_NOEFF ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV );
	HRESULT Render_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV );
	HRESULT RenderLightMapChecker( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pframeCur, const CLIPVOLUME *pCV );

	HRESULT UpdateFrames ()
	{
		D3DXMATRIXA16 mCur;
		D3DXMatrixIdentity ( &mCur );
		return UpdateFrames ( m_pDxFrame, mCur );		
	}

	// Deffered
public:
	HRESULT Render_Deffered_GeometryPass( LPDIRECT3DDEVICEQ pd3dDevice, 
										const D3DXMATRIX *pWorld, 
										const CLIPVOLUME *pCV, 
										BOOL bForwardPass );	// bForwardPass : TRUE - Pass2,Water,CustomAlpha �� Deffered �� ������ ���ϴ� �͵�.. );

	HRESULT RenderSoftAlpha_Deffered_GeometryPass( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bInverseOrder );
	HRESULT Render_Deffered_MaterialPass( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX *pWorld, const CLIPVOLUME *pCV );
	HRESULT Render_DecalPass2_WaterCustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV );
	//HRESULT Render_Forward_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX *pWorld, const CLIPVOLUME *pCV );

	// Deffered
private:
	HRESULT DrawFrames_Deffered_GPass( LPDIRECT3DDEVICEQ pd3dDevice, 
										DxFrame *pframeCur, 
										const CLIPVOLUME *pCV, 
										const D3DXMATRIX& matWorld, 
										DWORD dwPass, 
										std::vector<DxFrame*>* pvecframeCur,
										BOOL bForwardPass );	// bForwardPass : TRUE - Pass2,Water,CustomAlpha �� Deffered �� ������ ���ϴ� �͵�..

	HRESULT DrawFrames_Deffered_GPass_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, 
												const std::vector<DxFrame*>& vecframeCur, 
												BOOL bInverseOrder,
												BOOL bForwardPass )	;// bForwardPass : TRUE - Pass2,Water,CustomAlpha �� Deffered �� ������ ���ϴ� �͵�..)

	HRESULT DrawFrames_Deffered_GPass_Decal( LPDIRECT3DDEVICEQ pd3dDevice, 
												const std::vector<DxFrame*>& vecframeCur, 
												DWORD dwOrder, 
												BOOL bForwardPass );	// bForwardPass : TRUE - Pass2,Water,CustomAlpha �� Deffered �� ������ ���ϴ� �͵�.. );

	//HRESULT DrawFrames_Forward_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, const CLIPVOLUME *pCV, const D3DXMATRIX& matWorld );

public:
	BOOL LoadSet( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strPath );
	BOOL LoadSet_VER100( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strPath );
    BOOL LoadSet_VER101( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strPath );
    BOOL LoadSet_VER102( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strPath );
    BOOL LoadSet_VER103( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strPath );

	void SaveSet( sc::SerialFile &SFile );
    BOOL LoadSet_Prev( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strPath );

	void SaveFrameEff( sc::SerialFile &SFile );

private:
    BOOL LoadSet_Prev2( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strPath );

public:
	DxFrameMesh( BOOL bMeshOptimize );
	virtual ~DxFrameMesh();
};

#endif // !defined(AFX_DXFRAMEMESH_H__ED2134D1_E13E_4E71_8619_C435D1E2403B__INCLUDED_)
