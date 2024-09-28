#include "pch.h"

#include "NSRadiosity.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSRadiosity
{
	LPDIRECT3DTEXTUREQ	g_pTexture(NULL);
	LPDIRECT3DSURFACEQ	g_pSurface(NULL);
	LPDIRECT3DTEXTUREQ	g_pTexture_1st(NULL);
	LPDIRECT3DSURFACEQ	g_pSurface_1st(NULL);
	LPDIRECT3DTEXTUREQ	g_pTexture_2nd(NULL);
	LPDIRECT3DSURFACEQ	g_pSurface_2nd(NULL);
	LPDIRECT3DTEXTUREQ	g_pTexture_3rd(NULL);
	LPDIRECT3DSURFACEQ	g_pSurface_3rd(NULL);
	LPDIRECT3DSURFACEQ	g_pZBuffer(NULL);

	void CreateSurface( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		HRESULT hr(S_OK);

		hr = pd3dDevice->CreateTexture ( 128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexture, NULL );
		if( FAILED(hr) )
			return;

		hr = g_pTexture->GetSurfaceLevel( 0, &g_pSurface );
		if( FAILED(hr) )
			return;

		hr = pd3dDevice->CreateTexture ( 128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexture_1st, NULL );
		if( FAILED(hr) )
			return;

		hr = g_pTexture_1st->GetSurfaceLevel( 0, &g_pSurface_1st );
		if( FAILED(hr) )
			return;

		hr = pd3dDevice->CreateTexture ( 128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexture_2nd, NULL );
		if( FAILED(hr) )
			return;

		hr = g_pTexture_2nd->GetSurfaceLevel( 0, &g_pSurface_2nd );
		if( FAILED(hr) )
			return;

		hr = pd3dDevice->CreateTexture ( 128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &g_pTexture_3rd, NULL );
		if( FAILED(hr) )
			return;

		hr = g_pTexture_3rd->GetSurfaceLevel( 0, &g_pSurface_3rd );
		if( FAILED(hr) )
			return;

		hr = pd3dDevice->CreateDepthStencilSurface( 128, 128, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pZBuffer, NULL );
		if( FAILED(hr) )
			return;
	}

	void DeleteSurface()
	{
		SAFE_RELEASE ( g_pTexture );
		SAFE_RELEASE ( g_pSurface );
		SAFE_RELEASE ( g_pTexture_1st );
		SAFE_RELEASE ( g_pSurface_1st );
		SAFE_RELEASE ( g_pTexture_2nd );
		SAFE_RELEASE ( g_pSurface_2nd );
		SAFE_RELEASE ( g_pTexture_3rd );
		SAFE_RELEASE ( g_pSurface_3rd );
		SAFE_RELEASE ( g_pZBuffer );
	}
};