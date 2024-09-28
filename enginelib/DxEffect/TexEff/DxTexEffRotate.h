#pragma once

#include <string>

#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/TextureManager.h"
#include "DxTexEff.h"

class sc::SerialFile;
struct DxMeshes;

struct TEXEFF_ROTATE_PROPERTY
{
	// Note : Save, Load, Edit
	float				fSpeed;
	std::string			strTex;
	TextureResource m_textureRes;

	void Save( sc::SerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );
	
	void Load_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	TEXEFF_ROTATE_PROPERTY();
	~TEXEFF_ROTATE_PROPERTY();
};

class DxTexEffRotate : public DxTexEffBase
{
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static char	NAME[];

protected:
	TEXEFF_ROTATE_PROPERTY	m_sProp;

	// Note : 내부 사용
	float	m_fTime;
	float	m_fRotate_ValueEND;

	DWORD	m_dwColor;
	float	m_fSin_Value;
	float	m_fCos_Value;

public:
	virtual DWORD GetTypeID()				{ return TYPEID; }
	virtual const char* GetName()			{ return NAME; }
	virtual void GetProperty( PBYTE &pProp ) { pProp = (PBYTE)&m_sProp; }
	virtual void SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp );

public:
	virtual void FrameMove( const float fElapsedTime );

public:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawSB;
	static void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void SetRenderBegin( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice );

	// Note : FrameMesh Mode
public:
	void FrameMoveMultiTex( LPDIRECT3DVERTEXBUFFERQ pVB, DWORD dwVert, VERTEX* pVertSrc );
	virtual void Render( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, 
				        LPD3DXATTRIBUTERANGE pAttrib, const DWORD dwAttrib, VERTEX* pVertSrc, DWORD dwVert );

public:
	// Note : Octree 메쉬에서의 개개의 VB를 사용하지 않고 공용 VB 사용하려고 이렇게 만들어 놓았다.
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces, VERTEXNORCOLORTEX* pVertSrc ) override;

public:
	TEXEFF_ROTATE_PROPERTY* GetProperty()	{ return &m_sProp; }

public:
	virtual void SavePSF( sc::SerialFile& SFile );
	virtual BOOL LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

public:
	DxTexEffRotate();
	virtual ~DxTexEffRotate();
};


