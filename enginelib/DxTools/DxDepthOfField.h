#pragma once

//--------------------------------------------------------------------------------------
// Vertex format
//--------------------------------------------------------------------------------------
struct DOFVERTEX 
{
	D3DXVECTOR4 pos;
	DWORD       clr;
	D3DXVECTOR2 tex1;
	D3DXVECTOR2 tex2;
	D3DXVECTOR2 tex3;
	D3DXVECTOR2 tex4;
	D3DXVECTOR2 tex5;
	D3DXVECTOR2 tex6;

	static const DWORD FVF;
};

class DxDepthOfField
{
public:
	DxDepthOfField(void);
	~DxDepthOfField(void);

	HRESULT BeginDOFRender(D3DXMATRIX matWorld);
	HRESULT BeginDOFRender();
	HRESULT	EndDOFRender();
	HRESULT BeginDOF();
	HRESULT	EndDOF();
	HRESULT RenderDOF();

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FrameMove ();

	void	SetupQuad();

private:
	IDirect3DDevice9		*m_pd3dDevice;

	LPDIRECT3DTEXTURE9      m_pFullScreenTexture;
	LPD3DXRENDERTOSURFACE   m_pRenderToSurface;
	LPDIRECT3DSURFACE9      m_pFullScreenTextureSurf;

	DOFVERTEX               m_Vertex[4];

	LPD3DXMESH              m_pTestMesh;
	LPDIRECT3DTEXTURE9      m_pTestTexture;


	LPD3DXEFFECT			m_pEffect;

	D3DXVECTOR4             m_vFocalPlane;
	int                     m_nShowMode;

	D3DVIEWPORT9            m_ViewportFB;
	D3DVIEWPORT9            m_ViewportOffscreen;

	FLOAT                   m_fBlurConst;

	float					m_fTestRotate;

	D3DXHANDLE              m_hFocalPlane;
	D3DXHANDLE              m_hWorld;
	D3DXHANDLE              m_hWorldView;
	D3DXHANDLE              m_hWorldViewProjection;
	D3DXHANDLE              m_hMeshTexture;
	D3DXHANDLE              m_hTechWorldWithBlurFactor;
	D3DXHANDLE              m_hTechShowBlurFactor;
	D3DXHANDLE              m_hTechDepthOfField;
	D3DXHANDLE				m_hLightDir;
	D3DXHANDLE				m_hLightAmbient;
	D3DXHANDLE				m_hLightDiffuse;

public:
	static DxDepthOfField& GetInstance();
};
