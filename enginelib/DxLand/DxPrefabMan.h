#ifndef _DX_PREFAB_MAN_H_
#define _DX_PREFAB_MAN_H_

#pragma once

#include "./DxPiece/DxPieceDef.h"

class DxLightMan;
class DxPieceManagerPSF;
class DxSetLandMan;

// ---------------------------------------------------------------- Prefab
//
class DxPrefab
{
private:
	enum
	{
		VERSION = 0x100,
	};

public:
	TSTRING		m_strFileName;
	D3DXMATRIX	m_matWorld;
	boost::scoped_ptr<DxSetLandMan>			m_scpSetLandMan;

	void FrameMove( float fTime, float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFrameMove );

	BOOL IsCollision ( D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3* pNor ) const;
	void MakeTree();
	void RefreshPrefab( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strPath );
	void ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece );

	void Load( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, const D3DXMATRIX& matWorld );

public:
	DxPrefab(void);
	~DxPrefab(void);
};

// ---------------------------------------------------------------- PrefabMan
//
class DxPrefabMan
{
private:
	enum
	{
		VERSION = 0x100,
	};

private:
	static TSTRING	m_strPath;

public:
	static void SetPath( const TSTRING& strPath )	{ m_strPath = strPath; }

public:
	typedef std::vector<std::tr1::shared_ptr<DxPrefab>>	VEC_PREFAB;
	typedef VEC_PREFAB::iterator						VEC_PREFAB_ITER;
	VEC_PREFAB	m_vecPrefab;

	void FrameMove( float fTime, float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFrameMove );

	// Edit
	void Insert( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, const D3DXMATRIX& matWorld );
	void Delete( int nIndex );
	size_t GetSize();
	BOOL IsGetName( size_t i, TSTRING& pPieceName );
	BOOL IsCollision ( const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vTargetPt, DxPrefab** ppPrefab ) const;
	D3DXMATRIX* MoveCamera( DWORD dwItem );
	DxPrefab* GetPrefab( DWORD dwItem );
	int GetIndex( DxPrefab* pSrc );
	void MakeTree();
	void RefreshPrefab( LPDIRECT3DDEVICEQ pd3dDevice );
	void ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece );

	void Save( sc::SerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
	void CleanUp();

public:
	DxPrefabMan(void);
	~DxPrefabMan(void);
};

#endif // _DX_PREFAB_MAN_H_