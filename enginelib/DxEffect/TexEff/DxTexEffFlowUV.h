#pragma once

#include <string>

#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/TextureManager.h"
#include "DxTexEff.h"

class sc::SerialFile;
struct DxMeshes;

#define	TEXEFFFLOWUV_NEWUV			0x0001
#define	TEXEFFFLOWUV_RANDOMSPEED	0x0002
#define	TEXEFFFLOWUV_MODULATE2X		0x0004

struct TEXEFF_FLOWUV_PROPERTY
{
	DWORD		dwFlag;
	float		fScale;
	float		fAlpha;
	D3DXVECTOR2	vSpeed;
	std::string	strAlphaTex;
	std::string	strFlowTex;

	float		fSpeed;		// Save�� ������ Edit ������ �� ���δ�.
	D3DXVECTOR2	vDirect;	// Save�� ������ Edit ������ �� ���δ�.

    TextureResource m_textureResAlpha;
    TextureResource m_textureResFlow;

	void Save( sc::SerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	void Load_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

	TEXEFF_FLOWUV_PROPERTY();
	~TEXEFF_FLOWUV_PROPERTY();
};

class DxTexEffFlowUV : public DxTexEffBase
{
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static char	NAME[];

protected:
	TEXEFF_FLOWUV_PROPERTY	m_sProp;

	BOOL		m_bUp;
	float		m_fTime;
	float		m_fRandSpeed;
	D3DXVECTOR2	m_vLastSpeed;
	DWORD		m_dwColor;

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
	// Note : Octree �޽������� ������ VB�� ������� �ʰ� ���� VB ����Ϸ��� �̷��� ����� ���Ҵ�.
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces, VERTEXNORCOLORTEX* pVertSrc ) override;

public:
	TEXEFF_FLOWUV_PROPERTY* GetProperty()	{ return &m_sProp; }

public:
	virtual void SavePSF( sc::SerialFile& SFile );
	virtual BOOL LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

public:
	DxTexEffFlowUV();
	virtual ~DxTexEffFlowUV();
};


