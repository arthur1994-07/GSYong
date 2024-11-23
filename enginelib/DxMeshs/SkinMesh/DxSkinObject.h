#pragma once

//#include "DxSkeletonManager.h"
//#include "DxSkinAnimationManager.h"
#include "./DxSkeletonManager.h"
#include "./DxSkinMeshManager.h"
#include "./DxSkinAniControl.h"

class DxSkinMesh9;

class DxSkinObject : public DxSkinAniControl
{
protected:
	static char		m_szPath[MAX_PATH];

public:
	static void		SetPath ( char* szPath );
	static char*	GetPath ()	{ return m_szPath; }

protected:
	char			    m_szSkinMesh[MAX_PATH];
	DxSkinMeshResource	m_skinMeshRes;
	DxSkeletonResource  m_skeletonRes;

public:
	DxSkinMesh9*	GetSkinMesh ()		{ return m_skinMeshRes.get(); }
	char*			GetSkinMeshName ()	{ return m_szSkinMesh; }

public:
	HRESULT FrameMove ( float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot );

public:
	DxSkinMesh9* CreateSkinMesh ( const char* szFile, const char *szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bThread );

public:
	BOOL LoadObject ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread );
	BOOL SaveObject ( const char* szFile );

	void ClearAll ();

public:
	DxSkinObject(void);
	~DxSkinObject(void);
};
