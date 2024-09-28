#pragma once

namespace NSRadiosity
{
	extern LPDIRECT3DSURFACEQ	g_pSurface;
	extern LPDIRECT3DSURFACEQ	g_pSurface_1st;
	extern LPDIRECT3DTEXTUREQ	g_pTexture_1st;
	extern LPDIRECT3DSURFACEQ	g_pSurface_2nd;
	extern LPDIRECT3DTEXTUREQ	g_pTexture_2nd;
	extern LPDIRECT3DSURFACEQ	g_pSurface_3rd;
	extern LPDIRECT3DTEXTUREQ	g_pTexture_3rd;
	extern LPDIRECT3DSURFACEQ	g_pZBuffer;

	void CreateSurface( LPDIRECT3DDEVICEQ pd3dDevice );
	void DeleteSurface();
};