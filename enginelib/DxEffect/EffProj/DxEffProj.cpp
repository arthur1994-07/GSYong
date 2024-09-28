#include "pch.h"

#include "../../DxTools/DxSurfaceTex.h"

#include "./DxEffProjTexture.h"

#include "DxEffProj.h"


DxEffProjMan& DxEffProjMan::GetInstance()
{
	static DxEffProjMan Instance;
	return Instance;
}

DxEffProjMan::DxEffProjMan()
{

}

DxEffProjMan::~DxEffProjMan()
{

}

void DxEffProjMan::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sGetItemEFF.OnCreateDevice( pd3dDevice );
	m_sDoublePointEFF.OnCreateDevice( pd3dDevice );
}

void DxEffProjMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sGetItemEFF.OnResetDevice( pd3dDevice );
	m_sDoublePointEFF.OnResetDevice( pd3dDevice );
}

void DxEffProjMan::OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sGetItemEFF.OnLostDevice( pd3dDevice );
	m_sDoublePointEFF.OnLostDevice( pd3dDevice );
}

void DxEffProjMan::OnDestroyDevice()
{
	m_sGetItemEFF.OnDestroyDevice();
	m_sDoublePointEFF.OnDestroyDevice();
}

void DxEffProjMan::FrameMove( float fElapsedTime )
{
	if( !DxSurfaceTex::GetInstance().IsEnable() )	return;

	m_sGetItemEFF.FrameMove( fElapsedTime );
	m_sDoublePointEFF.FrameMove( fElapsedTime );
}

void DxEffProjMan::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !DxSurfaceTex::GetInstance().IsEnable() )	return;

	m_sGetItemEFF.Render( pd3dDevice );
	m_sDoublePointEFF.Render( pd3dDevice );
}

