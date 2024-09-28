/**
 * \date    2013/7/19
 * \author	shhan
 *
 * CascadeShadow �� ������ ������ �ϱ����� �۾���.
 * Material System �� ����� Mesh���� �۾��ȴ�.
 */
#pragma once

namespace NSShadowRenderForMaterial
{
	extern LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_ONLYPOS;
	extern LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_POSTEX;
	extern LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_ONLYPOS_INSTANCING;
	extern LPDIRECT3DVERTEXDECLARATION9    g_pDCRT_POSTEX_INSTANCING;

	void CleanUp();

	HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT OnLostDevice();

	HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetWorld( const D3DXMATRIX& matWorld );
	void SetBaseFX( const D3DXMATRIX& matVP );

	void SetMode_R32F();
	void SetMode_G32R32F();

	void SetTexture( LPDIRECT3DTEXTUREQ pTexture );

	void CommitChanges();

	BOOL FX_Begin();
	BOOL FX_BeginPass( UINT unPass );
	void FX_EndPass();
	void FX_End();
}