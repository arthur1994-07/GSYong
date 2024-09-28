#pragma once

#include <vector>
#include "./DxPieceDef.h"
#include "../../DxTools/RENDERPARAM.h"

class DxPiece;
class DxStaticAniFrame;
class DxStaticMeshLightMap;
struct CLIPVOLUME;

template <typename T>
class DxPieceInstancingLodVB
{
private:
	const DxStaticAniFrame*		m_rStaticAniFrame;	// ������ �Ѵ�.
	DWORD						m_dwPieceMax;		// ���� �ʿ� ���õ� ��ü Piece
	DWORD						m_dwPieceCV;		// �׷��� �Ǵ� Piece ������.
	DWORD						m_dwLOD;			// ���Ǵ� LOD��ȣ

	std::vector<boost::shared_ptr<T>>	m_vecInstancingLOD;	// LOD
	std::vector<DWORD>					m_vecPieceSizeLOD;	// PieceSize

public:
	void Import_AddRef();
	BOOL CreateInstancingLOD( LPDIRECT3DDEVICEQ pd3dDevice );

	const size_t GetSize() const;
	//void ExportPiece( VEC_PIECE& vecPIECE ) const;

	// Instancing ������ ������ �ȵǸ� FALSE �� �� ���� �ִ�.
	//BOOL FrameMove_Instancing( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	// Thread
public:
	void ResetRef_THREAD();
	void AddRef_THREAD();
	void Reset_THREAD();
	void Insert_THREAD( const DxPiece* pPiece );
	void InsertDataToVB_ExceptPiece_THREAD( VEC_PIECE& vecExceptInstancing );
	void InsertDataToVB_ExceptPiece_Reflect_THREAD( VEC_PIECE& vecExceptInstancing, float fHeight );

	// FrameMove
public:
	BOOL FrameMoveStep( float fElapsedTime );

	// Render
public:
	void RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderDecal( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxPieceInstancingLodVB( const DxStaticAniFrame* pStaticAniFrame );
	~DxPieceInstancingLodVB();
};


//////////////////////////////////////////////////////////////////////////
#include "./DxPieceInstancingLodVB.inl"

//////////////////////////////////////////////////////////////////////////