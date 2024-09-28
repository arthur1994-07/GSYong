/**
 * \date    2012/09/03
 * \author	shhan
 *
 * m_pd3dQuery->Issue( D3DISSUE_BEGIN );, m_pd3dQuery->Issue( D3DISSUE_END ); 
 * 를 할 때, 빠른 렌더링을 하기 위한 namespace
 */
#pragma once

namespace NSOcclusionQueryFastRender
{
	extern BOOL g_bRendering;		// 현재 OcclusionQuery 렌더링 중인가~?

	void CleanUp();

	HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT OnLostDevice();

	HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetWorld( const D3DXMATRIX& matWorld );
	void SetVP( const D3DXMATRIX& matVP );
	void SetWVP( const D3DXMATRIX& matWVP );

	void Render( LPD3DXMESH pMesh );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, 
				LPDIRECT3DVERTEXBUFFERQ	pVB,
				LPDIRECT3DINDEXBUFFERQ pIB,
				int nFVFSize,
				DWORD dwVertNum, 
				DWORD dwFaceNum );

}