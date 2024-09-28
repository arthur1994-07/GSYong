#pragma once

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/TextureManager.h"
#include "../../DxMeshs/DxSimpleMeshMan.h"
#include "./DxSkyBoxDef.h"

class DxSimMesh;

struct DxSkyBox
{
private:
	enum
	{
		VERSION = 0x105,
	};

	enum BLEND
	{
		BLEND_NONE			= 0,
		BLEND_ADD			= 1,
		BLEND_ALPHA			= 2,
		BLEND_LIGHT_SHAFTS	= 3,
	};

private:
	float	m_fRotationY;		// 회전되어있는 각도.

public:
	float	m_fRotationBaseY;
	float	m_fRotationSPEED;
	float	m_fScale;
	float	m_fHeight;
	BLEND	m_emBlend;	// Blend~
	float	m_fPower;	// Texture 강도.	
	TSTRING	m_strName;

private:
	DxSimMeshResource	m_SimMeshRes;

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTexThread );
	void OnDestroyDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, D3DXMATRIX* pmatReflect, float fAlpha );
	void Render_DOF( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );
	void Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );

	BOOL IsHaveLightShafts();
	//BOOL IsCollision( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd );

public:
	DxSkyBox* CreateClone( LPDIRECT3DDEVICEQ pd3dDevice );
	void GetOnlyData( const DxSkyBox* pSkyBox );

	void SetSkyBoxRotateY( const DxSkyBox* pSrc );

    void SetObjRotate90( float fRotate );

public:
	friend class CSkyBoxManPage;

public:
	void Save( sc::SerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread );

	void Load_Ver103( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread );
	void Load_Ver102( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread );
	void Load_Ver101( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread );
	void Load_Ver100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bTexThread );

	void Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxSkyBox* pSrc, BOOL bTexThread );

public:
	DxSkyBox();
	~DxSkyBox();
};