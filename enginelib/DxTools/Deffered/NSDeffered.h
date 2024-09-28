/**
 * \date    2013/11/18
 * \author	shhan
 *
 * Deffered Render 를 위한 작업.
 */
#pragma once

#include "../../DxLand/NSLandThreadDef.h"

class DxSetLandMan;
class DxLandMan;
class DxPieceEdit;
struct CLIPVOLUME;

namespace NSDeffered
{
	extern LPDIRECT3DTEXTUREQ	g_pTextureLodBlend;

	HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT OnLostDevice();

	void RenderDefferedForPSF( IDirect3DDevice9* pd3dDevice, 
								const D3DCOLOR& colorClear,
								DxSetLandMan* pSetLandMan,
								DWORD dwWidth,
								DWORD dwHeight,
								LPDIRECT3DSURFACEQ pColorSuf_Main,
								LPDIRECT3DSURFACEQ pColorSuf_1st,
								LPDIRECT3DSURFACEQ pColorSuf_2nd,
								LPDIRECT3DSURFACEQ pColorSuf_3rd,
								LPDIRECT3DSURFACEQ pColorSuf_4th,
								LPDIRECT3DSURFACEQ pColorSuf_LBuffer,
								LPDIRECT3DTEXTUREQ pColorTex_FourCC_INTZ,
								LPDIRECT3DTEXTUREQ pColorTex_1st,
								LPDIRECT3DTEXTUREQ pColorTex_2nd, 
								LPDIRECT3DTEXTUREQ pColorTex_3rd,
								LPDIRECT3DTEXTUREQ pColorTex_4th,
								LPDIRECT3DTEXTUREQ pColorTex_LBuffer,
								const D3DXMATRIX& matView,
								const D3DXMATRIX& matProj,
								const CLIPVOLUME& sCameraCV,
								BOOL bSoftAlpha );

	void RenderDefferedForPiece( IDirect3DDevice9* pd3dDevice, 
								const D3DCOLOR& colorClear,
								DxPieceEdit* pPieceEdit,
								DWORD dwWidth,
								DWORD dwHeight,
								LPDIRECT3DSURFACEQ pColorSuf_Main,
								LPDIRECT3DSURFACEQ pColorSuf_1st,
								LPDIRECT3DSURFACEQ pColorSuf_2nd,
								LPDIRECT3DSURFACEQ pColorSuf_3rd,
								LPDIRECT3DSURFACEQ pColorSuf_4th,
								LPDIRECT3DSURFACEQ pColorSuf_LBuffer,
								LPDIRECT3DTEXTUREQ pColorTex_FourCC_INTZ,
								LPDIRECT3DTEXTUREQ pColorTex_1st,
								LPDIRECT3DTEXTUREQ pColorTex_2nd, 
								LPDIRECT3DTEXTUREQ pColorTex_3rd,
								LPDIRECT3DTEXTUREQ pColorTex_4th,
								LPDIRECT3DTEXTUREQ pColorTex_LBuffer,
								const D3DXMATRIX& matWorld,
								const D3DXMATRIX& matView,
								const D3DXMATRIX& matProj,
								const CLIPVOLUME& sCameraCV,
								BOOL bSoftAlpha );

	void RenderDefferedForSM30DF( IDirect3DDevice9* pd3dDevice, 
								const D3DCOLOR& colorClear,
								DxLandMan* pLandMan,
								DWORD dwWidth,
								DWORD dwHeight,
								LPDIRECT3DSURFACEQ pColorSuf_Main,
								LPDIRECT3DSURFACEQ pColorSuf_1st,
								LPDIRECT3DSURFACEQ pColorSuf_2nd,
								LPDIRECT3DSURFACEQ pColorSuf_3rd,
								LPDIRECT3DTEXTUREQ pColorTex_1st,
								LPDIRECT3DTEXTUREQ pColorTex_2nd, 
								LPDIRECT3DTEXTUREQ pColorTex_3rd,
								BOOL bReflect,
								float fReflectHeight,
								const D3DXMATRIX& matWorld,
								const D3DXMATRIX& matView,
								const D3DXMATRIX& matProj,
								const CLIPVOLUME& sCameraCV,
								BOOL bSoftAlpha,
								BOOL bCharShadow,
								BOOL bSky,
								BOOL bMaterialFrameMove );

	void RenderDefferedForSM30NM( IDirect3DDevice9* pd3dDevice, 
									const D3DCOLOR& colorClear,
									DxLandMan* pLandMan,
									DWORD dwWidth,
									DWORD dwHeight,
									LPDIRECT3DSURFACEQ pColorSuf_Main,
									LPDIRECT3DSURFACEQ pColorSuf_1st,
									LPDIRECT3DSURFACEQ pColorSuf_2nd,
									LPDIRECT3DSURFACEQ pColorSuf_3rd,
									LPDIRECT3DSURFACEQ pColorSuf_4th,
									LPDIRECT3DSURFACEQ pColorSuf_LBuffer,
									LPDIRECT3DSURFACEQ pColorSuf_FourCC_INTZ,
									LPDIRECT3DTEXTUREQ pColorTex_1st,
									LPDIRECT3DTEXTUREQ pColorTex_2nd, 
									LPDIRECT3DTEXTUREQ pColorTex_3rd,
									LPDIRECT3DTEXTUREQ pColorTex_4th,
									LPDIRECT3DTEXTUREQ pColorTex_LBuffer,
									LPDIRECT3DTEXTUREQ pColorTex_FourCC_INTZ,
									const D3DXMATRIX& matWorld,
									const D3DXMATRIX& matView,
									const D3DXMATRIX& matProj,
									const CLIPVOLUME& sCameraCV,
									BOOL bSoftAlpha );

	void ComputeClipVolume( CLIPVOLUME& sCameraCV,
							D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec, 
							float fNearPlane, float fWidth, float fHeight, float fFOV);
};