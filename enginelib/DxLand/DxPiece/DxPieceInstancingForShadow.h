#pragma once

#include <vector>
#include "./DxPieceDef.h"

struct PieceParameter;
class DxPiece;
class DxStaticAniFrame;

class DxPieceInstancingForShadow
{
private:
	struct INSTANCEDATA_POS
	{
		D3DXVECTOR3 m_vMatrix1;
		D3DXVECTOR3 m_vMatrix2;
		D3DXVECTOR3 m_vMatrix3;
		D3DXVECTOR4 m_vMatrix4;
	};

	IDirect3DVertexBuffer9* m_pInstanceDataVB;
	sc::DefaultLock			m_sDefaultLock_VB;

private:
	const DxStaticAniFrame*			m_rStaticAniFrame;		// 참조만 한다.
	std::vector<const DxPiece*>		m_vecrPiece;			// 참조만 한다.
	DWORD							m_dwPieceMax;			// 이 함수에서 가능한 최대 Piece 갯수

public:
	void CreateVB( LPDIRECT3DDEVICEQ pd3dDevice );

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();



	//////////////////////////////////////////////////////////////////////////
	//							Thread
public:
	void Insert_THREAD( const DxPiece* pPiece );
	void Reset_THREAD();

	// m_vecrPiece 갯수가 1개라면 vecExceptInstancing 으로 보낸다.
	void InsertDataToVB_ExceptPiece_THREAD( VEC_PIECE& vecExceptInstancing );

private:
	BOOL IsInsertDataToVB_THREAD();
	//							Thread
	//////////////////////////////////////////////////////////////////////////



	// FrameMove
public:
	BOOL FrameMoveStep();

	// Render
public:
	void RenderOpaque( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxPieceInstancingForShadow( LPDIRECT3DDEVICEQ pd3dDevice, const DxStaticAniFrame* pStaticAniFrame, DWORD dwPieceMax );
	~DxPieceInstancingForShadow();
};