#pragma once

#include <map>

#include "../../DxEffect/TexEff/DxTextureEffMan.h"

namespace sc {
    class SerialFile;
}
class DxTexEffBase;
class DxFrameMesh;

//----------------------------------------------------------------------------------------------------------------------
//								D	x		P	i	e	c	e		T	e	x		E	f	f
//	Note : 
//----------------------------------------------------------------------------------------------------------------------
class DxPieceTexEff
{
protected:
	typedef DxTextureEffMan::MAPTEXEFF	MAPTEXEFF;
	typedef MAPTEXEFF::iterator			MAPTEXEFF_ITER;
	MAPTEXEFF	m_mapTexEff;

public:
	DxTextureEffMan::MAPTEXEFF*	GetMapTexEff()		{ return &m_mapTexEff; }

public:
	void FrameMove( const float fElapsedTime );
    void ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void Save( sc::SerialFile& SFile );	// Tree ¿¡¼­ÀÇ Save Load
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

public:
	void Import( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh );
	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceTexEff* pSrc );
	void CleanUp();

public:
	DxPieceTexEff();
	~DxPieceTexEff();
};
