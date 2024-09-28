#pragma once

#include "../TextureManager.h"

class DxEffCharOverlay;
class DxEffCharUserColor;

namespace NSTexCombine
{
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

	void CreateOverlay( LPDIRECT3DDEVICEQ pd3dDevice, 
						int nMaterial,
						TextureResource* ptextureRes, 
						DxEffCharOverlay* pOverlay, 
						DWORD dwUserColor1, 
						DWORD dwUserColor2, 
						LPDIRECT3DTEXTUREQ& pResultTex );

	void CreateOverlayForEdit( LPDIRECT3DDEVICEQ pd3dDevice, 
								int nMaterial,
								TextureResource* ptextureRes, 
								DxEffCharOverlay* pOverlay,
								LPDIRECT3DTEXTUREQ& pResultTex );

	void CreateUserColor( LPDIRECT3DDEVICEQ pd3dDevice, 
						int nMaterial,
						TextureResource* ptextureRes, 
						DxEffCharUserColor* pUserColor, 
						DWORD dwUserColor, 
						LPDIRECT3DTEXTUREQ& pResultTex );

	void CreateUserColorForEdit( LPDIRECT3DDEVICEQ pd3dDevice, 
								int nMaterial,
								TextureResource* ptextureRes, 
								DxEffCharUserColor* pUserColor,
								LPDIRECT3DTEXTUREQ& pResultTex );
}