#pragma once

struct WORDWORD;

namespace NSCOMBINETEXTOOLS
{
	BOOL GetTexelAndTexelMarking( WORDWORD& sTexelXY,
									int nIndexX, 
									int nIndexY, 
									bool abTexelState[], 
									WORDWORD* pTexelEnable,
									WORD* pTexelLast,
									DWORD dwTexelUseNUM, 
									DWORD dwTexWidth, 
									DWORD dwTexHeight );

    void SetFitHeight( bool* pbTexelState, 
						DWORD dwTexWidth, 
						DWORD& dwTexHeight );

	void SetFitHeightOfTexture( LPDIRECT3DDEVICEQ pd3dDevice, 
								bool* pbTexelState, 
								DWORD dwTexWidth, 
								DWORD& dwTexHeight, 
								IDirect3DTexture9** ppTextureDAY,
								IDirect3DTexture9** ppTextureDAY_NEW );

	void DrawTexel_Tex2Array( DWORD* pTexData, 
							int iPitch, 
							DWORD* pColor,
							WORDWORD sTexelXY,
							int nSizeX, 
							int nSizeY,
							int OffsetX, 
							int OffsetY );

	void DrawTexel_Tex2Array( DWORD* pTexData, 
								int iPitch, 
								DWORD* pColor,
								WORDWORD sTexelXY,
								int nSizeX, 
								int nSizeY,
								int OffsetX, 
								int OffsetY,
								DWORD dwBitShading,
								DWORD dwOffsetShading,
								DWORD dwBitShadow,
								DWORD dwOffsetShadow );

	void DrawTexel_Tex2Tex( DWORD* pTexData, 
							int iPitch, 
							DWORD* pTexDataSrc,
							int iPitchSrc, 
							WORDWORD sTexelXY,
							int nSizeX, 
							int nSizeY,
							int OffsetX, 
							int OffsetY );

    void DrawTexel_CheckTile( DWORD* pTexData, 
					        int iPitch,
					        int nSizeX, 
					        int nSizeY );

	void DrawTexel_Dot( DWORD* pTexData, 
						int iPitch,
						int nSizeX, 
						int nSizeY );
}