#pragma once

#include "./DxVertexFVF.h"

class DxGlowMan
{
protected:
	float		m_fMicro_256;
	float		m_fElapsedTime;
	D3DXVECTOR3	m_vDirection;

protected:
	VERTEXRHW			m_sVert_256[4];
	VERTEXRHW			m_sVert_512[4];
	VERTEXRHW			m_sVert_Full[4];
	VERTEXCOLORRHW		m_sVert_Color_256[4];

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedGlowSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectGlowSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedBurnSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBurnSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedBlurSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectBlurSB;

protected:
	BOOL			m_bProjectActive;		// ������Ʈ���� ����/�̵���
	BOOL			m_bActive;				// �� �����ӿ��� ����/�̵���
	BOOL			m_bActive_Burn;			// �� �����ӿ��� ����/�̵���
	BOOL			m_bOptionEnable;		// �ɼǿ����� Ȱ��ȭ

public:
	void	SetProjectActiveON()			{ m_bProjectActive=TRUE; }
	void	SetGlowON()						{ m_bActive=TRUE; }
	void	SetGlowON_Burn ()				{ m_bActive_Burn=TRUE; }
	void	SetOptionEnable( BOOL bUse )	{ m_bOptionEnable=bUse; }
	BOOL	IsOptionEnable()				{ return m_bOptionEnable; }

	// Glow
public:
	void RenderTex ( const LPDIRECT3DDEVICEQ pd3dDevice );
	void Render ( const LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	BOOL m_bTEX4;

protected:
	void Render2W( const LPDIRECT3DDEVICEQ pd3dDevice );
	void Render2H( const LPDIRECT3DDEVICEQ pd3dDevice );
	void Render4 ( const LPDIRECT3DDEVICEQ pd3dDevice );

	void TexRender2( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vOffSet1, const D3DXVECTOR2& vOffSet2, const float fRate );
	void TexRender4();

	// Burn
public:
	void RenderBURN( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void GlowRender_Burn( LPDIRECT3DDEVICEQ pd3dDevice );
	void TexOneRender( LPDIRECT3DDEVICEQ pd3dDevice, VERTEXRHW* sVertices );
	void TexOneRender( LPDIRECT3DDEVICEQ pd3dDevice, VERTEXCOLORRHW* sVertices, float fMicro, DWORD cColor, int nPixelW, int nPixelH );

public:
	void FrameMove ( float fTime, float fElapsedTime );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxGlowMan();

public:
	virtual ~DxGlowMan();

public:
	static DxGlowMan& GetInstance();
};
