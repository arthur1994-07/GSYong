#pragma once

#include <vector>
#include "./DxPieceDef.h"
#include "../../DxTools/RENDERPARAM.h"

class DxPiece;
class DxStaticAniFrame;
class DxStaticMeshLightMap;
struct CLIPVOLUME;
struct PieceParameter;


class DxPieceInstancing
{
private:
	struct INSTANCEDATA
	{
		D3DXVECTOR4 m_vVC_AC0XY;				// TEXTURE1
		D3DXVECTOR4 m_vMatrix1_AC0W;			// TEXTURE2
		D3DXVECTOR4 m_vMatrix2_LMOffsetX;		// TEXTURE3
		D3DXVECTOR4 m_vMatrix3_LMOffsetY;		// TEXTURE4
		D3DXVECTOR4 m_vMatrix4;					// TEXTURE5
		D3DXVECTOR4 m_vAmbientCube_0Z_12_3X;	// TEXTURE6
		D3DXVECTOR4 m_vAmbientCube_3YZ_45;		// TEXTURE7
	};

	IDirect3DVertexBuffer9* m_pInstanceDataVB;
	sc::DefaultLock			m_sDefaultLock_VB;

private:
	const DxStaticAniFrame*			m_rStaticAniFrame;	// 참조만 한다.
	std::vector<const DxPiece*>		m_vecrPiece;		// 참조만 한다.
	DWORD							m_dwPieceMax;		// 이 함수에서 가능한 최대 Piece 갯수

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
	void InsertDataToVB_ExceptPiece_Reflect_THREAD( VEC_PIECE& vecExceptInstancing, float fHeight );

private:
	BOOL IsInsertDataToVB_THREAD();
	BOOL IsInsertDataToVB_Reflect_THREAD( float fHeight );
	//							Thread
	//////////////////////////////////////////////////////////////////////////



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
	DxPieceInstancing( LPDIRECT3DDEVICEQ pd3dDevice, const DxStaticAniFrame* pStaticAniFrame, DWORD dwPieceMax );
	~DxPieceInstancing();
};