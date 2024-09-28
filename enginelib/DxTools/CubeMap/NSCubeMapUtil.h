#pragma once

class DxLandMan;
class DxSetLandMan;

namespace NSCubeMapUtil
{
	void CreateLocalCubeMapOutputFile( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, DWORD dwSize, const D3DXVECTOR3& vPos, DxLandMan* pLandMan );

	void CreateLocalCubeMapForGlobal( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwSize, const D3DXVECTOR3& vPos, DxLandMan* pLandMan, LPDIRECT3DCUBETEXTUREQ& pCubeTex );

	// �ӽ� CubeMap ���� �Լ�
	void SetTempCubeMapName( const TSTRING& strName );
	LPDIRECT3DCUBETEXTUREQ GetTempCubeMapTexture();

	// CubeMap�� Blend �����ش�.
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

	// RealTime ���� CubeMap �� �����Ѵ�. PSF ��忡�� �ʿ���.
	// WorldEdit ������ ����.
	namespace NSCubeMapForPSF
	{
		void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
		void OnLostDevice();
		void OnFrameMove( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, DxSetLandMan* pSetLandMan );
		void OnRenderForPieceEditor( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture );
		LPDIRECT3DCUBETEXTUREQ GetCubeMapTexture();
	}
};