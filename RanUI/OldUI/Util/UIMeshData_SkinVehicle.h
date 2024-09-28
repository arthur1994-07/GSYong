
#pragma once


#include "./UIMeshData.h"
#include "../../../EngineLib/DxMeshs/DxAniKeys.h"
#include "../../../EngineLib/DxMeshs/SkinMesh/DxPieceDefine.h"

class DxVehicle;
class DxVehicleData;

class CUIMeshData_SkinVehicle : public IUIMeshData
{
public:
	CUIMeshData_SkinVehicle();
	~CUIMeshData_SkinVehicle();

public:
	virtual HRESULT InitDeviceObjects       ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects    ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();                              
	virtual HRESULT DeleteDeviceObjects     ();                             

public:
	virtual const BOOL Load      ( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& szFile );
	virtual void       FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );
	virtual void       Render    ( LPDIRECT3DDEVICEQ pd3dDevice );

	

	HRESULT SetPart(const TCHAR* szFile,LPDIRECT3DDEVICEQ pd3dDevice);
	void ResetVehiclePart( EMPIECEVEHICLE i );
	

	const BOOL         SelectAni     ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType=AN_SUB_NONE, DWORD dwFlags=NULL, DWORD dwSelect=0 );
	const BOOL         SetMainColor  ( int emPart, const DWORD dwColor );
	const BOOL         SetSubColor   ( int emPart, const DWORD dwColor );

	void               GetDefaultOverlayColor( int emPart, DWORD& dwMainColor, DWORD& dwSubColor );
	const float        GetHeight ();
	void GetDriverMatrix( D3DXMATRIX& matCombinedScaleInv );

private:
	float			m_fTime;
	DxVehicle*		m_pVehicle;
	DxVehicleData*	m_pVehicleData;
};