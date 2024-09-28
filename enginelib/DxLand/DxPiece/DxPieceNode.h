#pragma once

#include "../../DxTools/Collision/CollisionDef.h"
#include "../../DxTools/Material/MaterialDef.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/TextureManager.h"
#include "../LightMap/LightMapDef.h"
#include "../LightMap/LightMapBoxColor.h"
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
struct DxPieceNode;
struct CLIPVOLUME;

namespace sc
{	
    class SerialFile;
};

//----------------------------------------------------------------------------------------------------------------------
//										D	x		P	i	e	c	e		T	r	e	e
//----------------------------------------------------------------------------------------------------------------------
struct DxPieceNode
{
	enum
	{
		VERSION = 0x102,
	};

	union
	{
		struct { D3DXVECTOR3 m_vMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 m_vMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	D3DXVECTOR3	m_vCenter;
	D3DXVECTOR3	m_vCenterPoint;		// 분할시 쓰이는 데이터
	D3DXVECTOR3	m_vHalf;
	float		m_fRadius;			// 물체의 반지름

	//BOOL		m_bDeletePIECE;

	// 전체 Piece 들.
	VEC_PIECE	m_vecPIECE;	// Piece 모음	// 이 데이터는 참조값이다. 삭제시 알맹이를 Delete 하면 안된다. 껍데기만 지워주면 된다.

	// 중옵 이상에서는 이 2개만 렌더하면 된다.
	MAP_PIECE_INSTANCING	m_mapPieceInstancing;
	//VEC_PIECE				m_vecInstancingExceptPiece;	// Instancing 에 포함되지 않은 Piece 들.

	// 저사양에서는 이것만 렌더하면 된다.
	//VEC_PIECE				m_vecUnderShader_2_0;		// Shader.2.0 이하에서만 동작되는 Piece 들

	DxPieceNode*	m_pLeftChild;
	DxPieceNode*	m_pRightChild;

	void	InitInstancing( VEC_PIECE& vecInstancingExceptPiece );
	void	CreateVB_ForInstancing( LPDIRECT3DDEVICEQ pd3dDevice );
	//void	CreateUnderShader_2_0();
	//void	FrameMove_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime, VEC_PIECE& vecInstancingExceptPiece );
	void	RenderOpaque_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void	RenderHardAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void	RenderEffFrame_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bLBuffer );
	void	RenderDecal_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void	RenderDecalPass2_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void	RenderWaterCustomSoftAlpha_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV );
	void	RenderEff_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, DxStaticMeshLightMap* pStaticMeshLightMap );

	void	Save( sc::SerialFile& SFile );
	void	Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const VEC_PIECE& vecPiece );

	void	Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxPieceEdit* pSrc, VEC_PIECE& vecPiece );
	void	Load_PREV2( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxPieceEdit* pSrc, VEC_PIECE& vecPiece );

	void	ReleasePIECE();

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	DxPieceNode ()
		: m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX)
		, m_vMin(FLT_MAX,FLT_MAX,FLT_MAX)
		, m_vCenter(0.f,0.f,0.f)
		, m_vHalf(0.f,0.f,0.f)
		, m_fRadius(0.f)
		//, m_bDeletePIECE( FALSE )
		, m_pLeftChild( NULL )
		, m_pRightChild( NULL )
	{
	};

	~DxPieceNode ()
	{
		ReleasePIECE();
		SAFE_DELETE( m_pLeftChild );
		SAFE_DELETE( m_pRightChild );
	};
};
typedef DxPieceNode*	PDXPIECENODE;
