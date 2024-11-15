#pragma once

#include "../TexEff/DxTextureEffMan.h"

struct DxFrame;
class DxFrameMesh;
class DxAnimationMan;
class DxFrameAni;
class DxTexEffBase;

#include "../../DxMeshs/FrameMesh/DxAnimationManager.h"

class DxFrameAniMan
{
protected:
	DxFrameAni*		m_pFrameAni;
	DxAnimationMan*	m_pAniManHead;

public:
    DxFrameAni*	GetFrameAni() { return m_pFrameAni; }
	DxAnimationMan*	GetAniManHead()				{ return m_pAniManHead; }
	DxFrame* GetAniFrame( const char* szName );
	void MakeAniMan( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh );

protected:
	void MakeFrameAni( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEffMan );
	void LinkFrameAni( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame );
	void AniManMeshDelete();
	void AniManMeshDeleteTree( DxFrame* pFrame );

public:
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, const float fAniTime, DxTextureEffMan::MAPTEXEFF* pTexEffList );
	void Render_EDIT( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxFrameAniMan();
	~DxFrameAniMan();

	void Save( sc::SerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
};

//---------------------------------------------------------------------------------------------------------
//						N	S		O	P	T	I	M	I	Z	E		A	N	I
//---------------------------------------------------------------------------------------------------------
namespace NSOPTIMIZEANI
{
	void OptimizeFrameAni( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrameRoot, LPDXANIMATIONMAN &pDxAnimationMan, float fUNITTIME );
}