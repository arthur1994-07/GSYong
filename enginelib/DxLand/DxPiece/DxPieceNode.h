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
	D3DXVECTOR3	m_vCenterPoint;		// ���ҽ� ���̴� ������
	D3DXVECTOR3	m_vHalf;
	float		m_fRadius;			// ��ü�� ������

	//BOOL		m_bDeletePIECE;

	// ��ü Piece ��.
	VEC_PIECE	m_vecPIECE;	// Piece ����	// �� �����ʹ� �������̴�. ������ �˸��̸� Delete �ϸ� �ȵȴ�. �����⸸ �����ָ� �ȴ�.

	// �߿� �̻󿡼��� �� 2���� �����ϸ� �ȴ�.
	MAP_PIECE_INSTANCING	m_mapPieceInstancing;
	//VEC_PIECE				m_vecInstancingExceptPiece;	// Instancing �� ���Ե��� ���� Piece ��.

	// ����翡���� �̰͸� �����ϸ� �ȴ�.
	//VEC_PIECE				m_vecUnderShader_2_0;		// Shader.2.0 ���Ͽ����� ���۵Ǵ� Piece ��

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
