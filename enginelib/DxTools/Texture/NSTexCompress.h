#pragma once

namespace NSTexCompress
{
	extern DWORD Filter;

	HRESULT Compress(
		LPDIRECT3DDEVICEQ pd3dDevice,
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD dwDepth,
		DWORD numMips,
		D3DFORMAT fmtTo, 
		LPDIRECT3DBASETEXTURE9 ptexOrig,
		LPDIRECT3DBASETEXTURE9& _ptexNew,
		DWORD dwBltLevel = 0,
		BOOL bCubeMapFlags = FALSE );
}

namespace NSTexCompress
{
	WORD ColorTo565( const BYTE* color );

	WORD ColorTo1555( const BYTE* color );

	HRESULT CopyRenderTargetToManaged(
		LPDIRECT3DDEVICEQ pd3dDevice,
		LPDIRECT3DSURFACEQ pRenderTarget,
		LPDIRECT3DTEXTUREQ& pTextureSystem,
		LPDIRECT3DTEXTUREQ& pTextureManaged );

	HRESULT GetRenderTargetData(
		LPDIRECT3DDEVICEQ pd3dDevice,
		LPDIRECT3DSURFACEQ pRenderTarget,
		LPDIRECT3DTEXTUREQ pTextureSystem );

	HRESULT CopyTexture(
		DWORD dwWidth,
		DWORD dwHeight,
		D3DFORMAT emFormat,
		LPDIRECT3DTEXTUREQ pTextureSystem,
		LPDIRECT3DTEXTUREQ pTextureManaged );
}

namespace NSTexCompress
{
	HRESULT CompressDXT1(
		DWORD dwWidth,
		DWORD dwHeight,
		LPDIRECT3DTEXTUREQ pTextureRGBA,
		LPDIRECT3DTEXTUREQ pTextureDXT1 );
}