#pragma once

class DxLandMan;
class DxSetLandMan;

namespace NSCubeMapUtil
{
	void CreateLocalCubeMapOutputFile( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, DWORD dwSize, const D3DXVECTOR3& vPos, DxLandMan* pLandMan );

	void CreateLocalCubeMapForGlobal( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwSize, const D3DXVECTOR3& vPos, DxLandMan* pLandMan, LPDIRECT3DCUBETEXTUREQ& pCubeTex );

	// 임시 CubeMap 관련 함수
	void SetTempCubeMapName( const TSTRING& strName );
	LPDIRECT3DCUBETEXTUREQ GetTempCubeMapTexture();

	// CubeMap을 Blend 시켜준다.
	namespace NSCubeMapBlend
	{
		void CleanUp();
		void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
		void OnNewSizeDevice( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwSize );
		void OnLostDevice();
		void OnFrameMove( float fElapsedTime );
		void OnRender( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DCUBETEXTUREQ pSrcCubeMap );
		void OnRenderForPieceEditor( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture );
		LPDIRECT3DCUBETEXTUREQ GetCubeMapTexture();
	};

	// RealTime 으로 CubeMap 을 생성한다. PSF 모드에서 필요함.
	// WorldEdit 에서만 쓰임.
	namespace NSCubeMapForPSF
	{
		void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
		void OnLostDevice();
		void OnFrameMove( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, DxSetLandMan* pSetLandMan );
		void OnRenderForPieceEditor( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture );
		LPDIRECT3DCUBETEXTUREQ GetCubeMapTexture();
	}
};