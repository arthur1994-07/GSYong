#include "pch.h"
#include "./NSWaterMain.h"

#include "../GlobalParam.h"

//#include "./WaterMain.h"
#include "./WaterSurf.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSWaterMain
{
	LPDIRECT3DDEVICEQ g_pd3dDevice( NULL );
	WaterSurf* g_pWaterSurf( NULL );


	void CleanUp()
	{
		if( g_pWaterSurf )
			g_pWaterSurf->CleanUp();
	}

	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		g_pd3dDevice = pd3dDevice;

		g_pWaterSurf = new WaterSurf;

		g_pWaterSurf->OnCreateDevice( pd3dDevice );
	}

	VOID RestoreDeviceObjects()
	{
        if( g_pWaterSurf )
            g_pWaterSurf->OnResetDevice( g_pd3dDevice );
	}

	VOID FrameMove( FLOAT fElapsedTime )
	{
		if( !GLOBAL_PARAM::IsValue( GP_WATER ) )
			return;

		if( g_pWaterSurf )
			g_pWaterSurf->FrameMove( fElapsedTime );
	}

	VOID Render( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if( !GLOBAL_PARAM::IsValue( GP_WATER ) )
			return;

		if( g_pWaterSurf )
			g_pWaterSurf->Render( pd3dDevice );
	}

	VOID InvalidateDeviceObjects()
	{
        if( g_pWaterSurf )
            g_pWaterSurf->OnLostDevice();
	}

	VOID DeleteDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if( g_pWaterSurf )
			g_pWaterSurf->OnDestroyDevice( pd3dDevice );
	}

	VOID FinalCleanUp()
	{
		SAFE_DELETE( g_pWaterSurf );
	}

	VOID SaveSurf( sc::SerialFile &SFile )
	{
		if( g_pWaterSurf )
			g_pWaterSurf->Save( SFile );
	}

	VOID LoadSurf( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if( g_pWaterSurf )
			g_pWaterSurf->Load( SFile, pd3dDevice );
	}

	void Import ( LPDIRECT3DDEVICEQ pd3dDevice, WaterSurf* pWaterSurf )
	{
		g_pWaterSurf->Import( pd3dDevice, pWaterSurf );
	}
}

namespace NSWaterMainPSF
{
	LPDIRECT3DDEVICEQ g_pd3dDevice( NULL );
	WaterSurf* g_pWaterSurf( NULL );


	void CleanUp()
	{
		if( g_pWaterSurf )
			g_pWaterSurf->CleanUp();
	}

	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		g_pd3dDevice = pd3dDevice;

		g_pWaterSurf = new WaterSurf;

		g_pWaterSurf->OnCreateDevice( pd3dDevice );
	}

	VOID RestoreDeviceObjects()
	{
		if( g_pWaterSurf )
			g_pWaterSurf->OnResetDevice( g_pd3dDevice );
	}

	VOID FrameMove( FLOAT fElapsedTime )
	{
		if( !GLOBAL_PARAM::IsValue( GP_WATER ) )
			return;

		if( g_pWaterSurf )
			g_pWaterSurf->FrameMove( fElapsedTime );
	}

	VOID Render( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if( !GLOBAL_PARAM::IsValue( GP_WATER ) )
			return;

		if( g_pWaterSurf )
			g_pWaterSurf->Render( pd3dDevice );
	}

	VOID RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if( g_pWaterSurf )
			g_pWaterSurf->RenderEdit( pd3dDevice );
	}

	VOID InvalidateDeviceObjects()
	{
		if( g_pWaterSurf )
			g_pWaterSurf->OnLostDevice();
	}

	VOID DeleteDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if( g_pWaterSurf )
			g_pWaterSurf->OnDestroyDevice( pd3dDevice );
	}

	VOID FinalCleanUp()
	{
		SAFE_DELETE( g_pWaterSurf );
	}

	VOID SaveSurf( sc::SerialFile &SFile )
	{
		if( g_pWaterSurf )
			g_pWaterSurf->Save( SFile );
	}

	VOID LoadSurf( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if( g_pWaterSurf )
			g_pWaterSurf->Load( SFile, pd3dDevice );
	}

	void SetObjRotate90( const D3DXMATRIX& matRotate )
	{
		if( g_pWaterSurf )
			g_pWaterSurf->SetObjRotate90( matRotate );

	}

	WaterSurf* GetWaterSurf()
	{
		return g_pWaterSurf;
	}
}