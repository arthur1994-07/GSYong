#pragma once

#include "./DxSkyBox.h"

//class CSerialFile;
class DxSimMesh;

class DxSkyBoxMan
{
private:
	static DWORD VERSION;

public:
	VEC_SKYBOX*	m_pvecSKYBOX;

	float		m_fLightShaftsWeight;
	float		m_fLightShaftsDecay;
	float		m_fLightShaftsLast;
	D3DXVECTOR3	m_vLightShaftsDir;

	float	m_fAlpha;				// 실시간으로 변하는 값.

public:
	void FrameMove( float fElapsedTime );
	void Render_Local( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pmatReflect=NULL );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pmatReflect=NULL );
	void Render_DOF( LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice );
    void Render_LightShafts_Local( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void InsertSkyBox();
	void DeleteSkyBox( int nIndex );

	DxSkyBox* GetSkyBox( int nIndex );
	void Reload( LPDIRECT3DDEVICEQ pd3dDevice );
	void Reload( LPDIRECT3DDEVICEQ pd3dDevice, int nIndex );

	//BOOL IsCollision( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd ) const;

	void SetAlpha( float fAlpha );
	void SetSkyBoxRotateY( const VEC_SKYBOX* pSrc );	// pSrc을 기준으로 Y 값을 셋팅한다.

	void ItemUp( DxSkyBoxMan* pSkyBoxMan, int nIndex );
	void ItemDown( DxSkyBoxMan* pSkyBoxMan, int nIndex );

    void SetObjRotate90( float fRotate );

public:
	VOID CreateClone( LPDIRECT3DDEVICEQ pd3dDevice, VEC_SKYBOX& refSkyBox );
	VOID RenderClone( LPDIRECT3DDEVICEQ pd3dDevice, VEC_SKYBOX& refSkyBox, float fElapsedTime );
	VOID DeleteClone( VEC_SKYBOX& refSkyBox );
	void GetOnlyData( VEC_SKYBOX& refSkyBox );

public:
	void Save( sc::SerialFile &SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, BOOL bTexThread );

	void CleanUp();

	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxSkyBoxMan* pSrc );

	// SceneManager에서만 쓰임.
	void GetSkyBoxMan_SCENE( VEC_SKYBOX** ppvecSkyBox );

	// SceneManager에서만 쓰임.
	void SetSkyBoxMan_SCENE( VEC_SKYBOX* pvecSkyBox );

public:
	DxSkyBoxMan();
	~DxSkyBoxMan();

public:
	static DxSkyBoxMan& GetInstance();
};