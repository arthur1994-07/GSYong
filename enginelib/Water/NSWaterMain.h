#pragma once

//#include "../../CoreCommon/String/SerialFile.h"

struct CLIPVOLUME;
class WaterSurf;

namespace sc
{
	class SerialFile;
};

/**
 * \namespace	NSWaterMain
 * \date		2006/11/07
 * \author		Sung-Hwan Han
 * \brief		CWaterMain의 Init, SaveLoad, Redering 등을 관리 적용함.
 */
namespace NSWaterMain
{
	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID RestoreDeviceObjects();

	VOID FrameMove( FLOAT fElapsedTime );
	VOID Render( LPDIRECT3DDEVICEQ pd3dDevice );

	VOID InvalidateDeviceObjects();
	VOID DeleteDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID FinalCleanUp();

	VOID SaveSurf( sc::SerialFile &SFile );
	VOID LoadSurf( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	void Import ( LPDIRECT3DDEVICEQ pd3dDevice, WaterSurf* pWaterSurf );

	void CleanUp();
}

namespace NSWaterMainPSF
{
	VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID RestoreDeviceObjects();

	VOID FrameMove( FLOAT fElapsedTime );
	VOID Render( LPDIRECT3DDEVICEQ pd3dDevice );

	VOID RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice );

	VOID InvalidateDeviceObjects();
	VOID DeleteDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	VOID FinalCleanUp();

	VOID SaveSurf( sc::SerialFile &SFile );
	VOID LoadSurf( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	void SetObjRotate90( const D3DXMATRIX& matRotate );

	void CleanUp();

	WaterSurf* GetWaterSurf();
}