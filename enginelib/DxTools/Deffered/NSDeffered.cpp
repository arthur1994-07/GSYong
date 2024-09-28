#include "pch.h"

#include "../../../SigmaCore/Math/Random.h"

#include "../../DxLand/DxPiece/DxPieceEdit.h"
#include "../../DxLand/DxSetLandMan.h"
#include "../../DxLand/DxLandMan.h"
#include "../Material/NSMaterialManager.h"
#include "../CubeMap/NSCubeMapUtil.h"
#include "../SkyBox/DxSkyBoxMan.h"
#include "../Light/NSShadowLight.h"
#include "../Light/DxShadowLight.h"
#include "../DxEnvironment.h"
#include "../DxSurfaceTex.h"
#include "../RENDERPARAM.h"
#include "../DxShadowMap.h"
#include "../DxViewPort.h"
#include "../EditMeshs.h"

#include "NSDeffered.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/*

*/

namespace NSDeffered
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														Common
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LPDIRECT3DTEXTUREQ					g_pTextureLodBlend(NULL);



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														OnResetDevice
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	HRESULT OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		HRESULT hr(S_OK);

		DWORD dwSizeX = DxSurfaceTex::GetInstance().GetWidth();
		DWORD dwSizeY = DxSurfaceTex::GetInstance().GetHeight();

		hr = pd3dDevice->CreateTexture ( dwSizeX, dwSizeY, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &g_pTextureLodBlend, NULL );
		if( FAILED(hr) )
			return hr;

		D3DLOCKED_RECT stLocked;
		hr = g_pTextureLodBlend->LockRect( 0, &stLocked, NULL, 0 );
		if ( FAILED(hr) )
			return hr;	// intel 칩의 구형 그래픽드라이버 사용되면 오류가 발생할 수 있음.

		DWORD* pData = (DWORD*)stLocked.pBits;
		INT iPitch = stLocked.Pitch / 4;
		if ( pData )
		{
			for( DWORD y=0; y<dwSizeY; ++y )
			{
				for( DWORD x=0; x<dwSizeX; ++x )
				{
					DWORD dwColor = static_cast<DWORD>( sc::Random::RANDOM_POS()*255 );
					
					pData[ x + y * iPitch ] = (dwColor<<16) + (dwColor<<8) + dwColor;
				}
			}
		}
		g_pTextureLodBlend->UnlockRect( 0 );

		return hr;
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														OnLostDevice
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	HRESULT OnLostDevice()
	{
		HRESULT hr(S_OK);

		SAFE_RELEASE( g_pTextureLodBlend );

		return hr;
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														RenderDefferedForPSF
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
								BOOL bSoftAlpha ) 
	{
		//////////////////////////////////////////////////////////////////////////
		// Check FX
		LPD3DXEFFECT pFX = NSMaterialManager::GetDefferedFX_Tool();
		if ( !pFX )
			return;

		D3DXMATRIX matInverseView;
		D3DXMatrixInverse( &matInverseView, NULL,		&matView );
		pFX->SetMatrixTranspose( "g_matInverseView",	&matInverseView );



		//////////////////////////////////////////////////////////////////////////
		// Create Quad
		struct VERTEXRHW_CUBE
		{
			D3DXVECTOR4 vPos;
			D3DXVECTOR2 vTex;
		};
		VERTEXRHW				g_sVertBase[4];
		g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				1.f, 1.f );
		g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,		0.f,				1.f, 1.f );
		g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	1.f, 1.f );
		g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,		(float)dwHeight,	1.f, 1.f );
		g_sVertBase[0].vTex = D3DXVECTOR2 ( 0.5f/dwWidth,		0.5f/dwHeight );
		g_sVertBase[1].vTex = D3DXVECTOR2 ( 1.f+0.5f/dwWidth,	0.5f/dwHeight );
		g_sVertBase[2].vTex = D3DXVECTOR2 ( 0.5f/dwWidth,		1.f+0.5f/dwHeight );
		g_sVertBase[3].vTex = D3DXVECTOR2 ( 1.f+0.5f/dwWidth,	1.f+0.5f/dwHeight );



		//////////////////////////////////////////////////////////////////////////
		// RenderTarget Setting
		pd3dDevice->SetRenderTarget( 0, pColorSuf_1st );
		pd3dDevice->SetRenderTarget( 1, pColorSuf_2nd );
		pd3dDevice->SetRenderTarget( 2, pColorSuf_3rd );
		pd3dDevice->SetRenderTarget( 3, pColorSuf_4th );
		if ( bSoftAlpha )
		{
			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 1 );
		}
		else
		{
			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, colorClear, 1.0f, 1 );
		}



		//////////////////////////////////////////////////////////////////////////
		// Geometry Pass
		DWORD dwZEnable, dwZFunc, dwZWriteEnable, dwAlphaBlendEnable;
		pd3dDevice->GetRenderState( D3DRS_ZENABLE,			&dwZEnable );
		pd3dDevice->GetRenderState( D3DRS_ZFUNC,			&dwZFunc );
		pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );

		if ( bSoftAlpha )
		{
			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,					D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_INVSRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA,			D3DBLEND_ONE );
				pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA,			D3DBLEND_INVSRCALPHA );
			}

			//pFX->SetTexture( "g_pTextureLodBlend",			NSMaterialManager::GetBlackTexture() );
			pFX->SetTexture( "g_pTextureLodBlend",			g_pTextureLodBlend );
			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
			pSetLandMan->RenderSoftAlpha( pd3dDevice, NULL, FALSE, TRUE );
			pSetLandMan->RenderSoftAlphaForLodBlend( pd3dDevice );

			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			FALSE );
				pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	FALSE );
			}
		}
		else
		{
			pFX->SetTexture( "g_pTextureLodBlend",			g_pTextureLodBlend );
			pSetLandMan->Render( pd3dDevice, NULL, TRUE );	// Opaque, HardAlpha, Decal
			pSetLandMan->RenderForLodBlend( pd3dDevice );	// Opaque, HardAlpha, Decal
		}
		


		//////////////////////////////////////////////////////////////////////////
		// Light Accumulation Pass
		pd3dDevice->SetRenderTarget( 0, pColorSuf_LBuffer );
		pd3dDevice->SetRenderTarget( 1, NULL );
		pd3dDevice->SetRenderTarget( 2, NULL );
		pd3dDevice->SetRenderTarget( 3, NULL );
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

		pFX->SetTexture( "g_BackBufferTexture_2nd", pColorTex_2nd );
		pFX->SetTexture( "g_BackBufferTexture_3rd", pColorTex_3rd );
		pFX->SetTexture( "g_BackBufferTexture_4th", pColorTex_4th );
		pFX->SetTexture( "g_BackBufferTexture_Depth", pColorTex_FourCC_INTZ );

		D3DXHANDLE hTechniqueHandle = pFX->GetTechniqueByName("tool_deffered");
		if ( hTechniqueHandle )
		{
			//////////////////////////////////////////////////////////////////////////
			//						SetTechnique
			pFX->SetTechnique(hTechniqueHandle);

			// 이 부분 깔끔하게 처리할 필요가 있다.
			NSMaterialManager::SetFX_Deffered( pFX );



			//////////////////////////////////////////////////////////////////////////
			//			Determine Lit Pixels (DirectionLight)
			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( 3 < numPasses )
			{
				pFX->BeginPass( 3 );
				{
					// FX 마무리
					pFX->CommitChanges();

					pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );
				}
				pFX->EndPass();
			}
			pFX->End();



			//////////////////////////////////////////////////////////////////////////
			//						Common
			pd3dDevice->SetRenderState( D3DRS_STENCILREF,		0 );
			pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,			TRUE );



			//////////////////////////////////////////////////////////////////////////
			//				Determine Lit Pixels (PointLight)
			{
				D3DXMATRIX matWorld;
				LIST_DXLIGHT& listPointLight = DxLightMan::GetInstance()->GetLIST_DXLIGHT();
				LIST_DXLIGHT_ITER iter = listPointLight.begin();
				for( ; iter!=listPointLight.end(); ++iter )
				{
					//////////////////////////////////////////////////////////////////////////
					//							Common
					D3DXMatrixScaling( &matWorld, (*iter)->m_Light.Range, (*iter)->m_Light.Range, (*iter)->m_Light.Range );
					matWorld._41 = (*iter)->m_Light.Position.x;
					matWorld._42 = (*iter)->m_Light.Position.y;
					matWorld._43 = (*iter)->m_Light.Position.z;
					pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

					D3DXVECTOR3 vPos((*iter)->m_Light.Position);
					D3DXVec3TransformCoord( &vPos, &vPos, &matView );
					D3DXVECTOR4 vPos_Range_PL_TOOL( vPos, (*iter)->m_Light.Range );
					D3DXVECTOR4 vDiff_OverLighting_PL_TOOL( (*iter)->m_Light.Diffuse.r, (*iter)->m_Light.Diffuse.g, (*iter)->m_Light.Diffuse.b, (*iter)->m_fOverLighting );
					D3DXVECTOR4 vAtt_PL_TOOL( (*iter)->m_Light.Attenuation0, (*iter)->m_Light.Attenuation1, (*iter)->m_Light.Attenuation2, 1.f );

					D3DXMATRIX matWV, matWVP, matInverseProj;
					D3DXMatrixMultiply( &matWV, &matWorld,		&matView );
					D3DXMatrixMultiply( &matWVP, &matWV,		&matProj );
					D3DXMatrixInverse( &matInverseProj, NULL,	&matProj );
					pFX->SetMatrixTranspose( "g_matLightObjectWV",		&matWV );
					pFX->SetMatrixTranspose( "g_matLightObjectWVP",		&matWVP );
					pFX->SetMatrixTranspose( "g_matInverseProj",		&matInverseProj );
					pFX->SetVector( "g_vPos_Range_PL_TOOL", &vPos_Range_PL_TOOL );
					pFX->SetVector( "g_vDiff_OverLighting_PL_TOOL", &vDiff_OverLighting_PL_TOOL );
					pFX->SetVector( "g_vAtt_PL_TOOL", &vAtt_PL_TOOL );

					//////////////////////////////////////////////////////////////////////////
					//					구안에 들어갔다면... 혹은 밖에 있다면
					if ( COLLISION::IsCollisionVolume( sCameraCV, (*iter)->m_Light.Position, (*iter)->m_Light.Range ) )
					{
						//////////////////////////////////////////////////////////////////////////
						//				Determine Lit Pixels (PointLight) #1
						pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
						pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
						pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
						pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );
						pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_GREATEREQUAL );
						pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	FALSE );

						UINT numPasses;
						pFX->Begin( &numPasses, 0 );
						if( 4 < numPasses )
						{
							pFX->BeginPass( 4 );
							{
								// FX 마무리
								pFX->CommitChanges();
								EDITMESHS::DrawMeshSphere();
							}
							pFX->EndPass();
						}
						pFX->End();
					}
					else
					{
						//////////////////////////////////////////////////////////////////////////
						//				Determine Lit Pixels (PointLight) #1
						pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,	0L );
						pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );
						pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_GREATEREQUAL );
						pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	TRUE );
						pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_ALWAYS );
						pd3dDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_ZERO );
						pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3DSTENCILOP_INCR );
						EDITMESHS::DrawMeshSphere();



						//////////////////////////////////////////////////////////////////////////
						//				Determine Lit Pixels (PointLight) #2
						pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
						pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
						pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
						pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
						pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );
						pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,	0x0000000f );
						pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_EQUAL );
						pd3dDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP );
						pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP );

						UINT numPasses;
						pFX->Begin( &numPasses, 0 );
						if( 4 < numPasses )
						{
							pFX->BeginPass( 4 );
							{
								// FX 마무리
								pFX->CommitChanges();
								EDITMESHS::DrawMeshSphere();
							}
							pFX->EndPass();
						}
						pFX->End();
					}

					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
					pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
					pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );
				}
			}



			//////////////////////////////////////////////////////////////////////////
			//				Determine Lit Pixels (SpotLight)
			{
				D3DXQUATERNION	vQuat;
				D3DXVECTOR3 vScale, vPos;
				D3DXMATRIX matWorld, matRotate;
				const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& vecShadowSpotLight = NSShadowLight::GetVecShadowSpotLight();
				std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::const_iterator citer = vecShadowSpotLight.begin();
				for( ; citer!=vecShadowSpotLight.end(); ++citer )
				{
					//////////////////////////////////////////////////////////////////////////
					//							Common
					//D3DXMatrixToSQT( vScale, vQuat, vPos, (*citer)->m_matWorld );
					//D3DXMatrixRotationQuaternion( &matRotate, &vQuat );
					//D3DXMatrixScaling( &matWorld, (*citer)->m_sLight.Range, (*citer)->m_sLight.Range, (*citer)->m_sLight.Range );
					//D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );
					//matWorld._41 = (*citer)->m_sLight.Position.x;
					//matWorld._42 = (*citer)->m_sLight.Position.y;
					//matWorld._43 = (*citer)->m_sLight.Position.z;
					//pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
					D3DXMatrixScaling( &matWorld, (*citer)->m_sLight.Range, (*citer)->m_sLight.Range, (*citer)->m_sLight.Range );
					matWorld._41 = (*citer)->m_sLight.Position.x;
					matWorld._42 = (*citer)->m_sLight.Position.y;
					matWorld._43 = (*citer)->m_sLight.Position.z;
					pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

					D3DXVECTOR3 vPos((*citer)->m_sLight.Position);
					D3DXVec3TransformCoord( &vPos, &vPos, &matView );
					D3DXVECTOR4 vPos_Range_CL_TOOL( vPos, (*citer)->m_sLight.Range );
					D3DXVECTOR4 vDiff_CosPhiHalf_CL_TOOL( (*citer)->m_sLight.Diffuse.r, (*citer)->m_sLight.Diffuse.g, (*citer)->m_sLight.Diffuse.b, cosf(D3DX_PI*(*citer)->m_sLight.Phi/180.f*0.5f) );
					D3DXVECTOR4 vAtt_CosHalfThetaSubPhi_CL_TOOL( (*citer)->m_sLight.Attenuation0, (*citer)->m_sLight.Attenuation1, (*citer)->m_sLight.Attenuation2, cosf(D3DX_PI*(*citer)->m_sLight.Theta/180.f*0.5f)-cosf(D3DX_PI*(*citer)->m_sLight.Phi/180.f*0.5f) );
					D3DXVECTOR3 vDir(-(*citer)->m_sLight.Direction.x,-(*citer)->m_sLight.Direction.y,-(*citer)->m_sLight.Direction.z);
					D3DXVec3TransformNormal( &vDir, &vDir, &matView );
					D3DXVECTOR4 vDirect_Falloff_CL_TOOL( vDir, (*citer)->m_sLight.Falloff );
					float fAttEndLineValue_CL_TOOL((*citer)->m_fAttEndLineValue);
					float fOverLighting_CL_TOOL((*citer)->m_fOverLighting);

					D3DXMATRIX matWV, matWVP, matInverseProj;
					D3DXMatrixMultiply( &matWV, &matWorld,		&matView );
					D3DXMatrixMultiply( &matWVP, &matWV,		&matProj );
					D3DXMatrixInverse( &matInverseProj, NULL,	&matProj );
					pFX->SetMatrixTranspose( "g_matLightObjectWV",		&matWV );
					pFX->SetMatrixTranspose( "g_matLightObjectWVP",		&matWVP );
					pFX->SetMatrixTranspose( "g_matInverseProj",		&matInverseProj );
					pFX->SetVector( "g_vPos_Range_CL_TOOL",					&vPos_Range_CL_TOOL );
					pFX->SetVector( "g_vDiff_CosPhiHalf_CL_TOOL",			&vDiff_CosPhiHalf_CL_TOOL );
					pFX->SetVector( "g_vAtt_CosHalfThetaSubPhi_CL_TOOL",	&vAtt_CosHalfThetaSubPhi_CL_TOOL );
					pFX->SetVector( "g_vDirect_Falloff_CL_TOOL",			&vDirect_Falloff_CL_TOOL );
					pFX->SetFloat( "g_fAttEndLineValue_CL_TOOL",			fAttEndLineValue_CL_TOOL );
					pFX->SetFloat( "g_fOverLighting_CL_TOOL",				fOverLighting_CL_TOOL );



					//////////////////////////////////////////////////////////////////////////
					//					구안에 들어갔다면... 혹은 밖에 있다면
					if ( COLLISION::IsCollisionVolume( sCameraCV, (*citer)->m_sLight.Position, (*citer)->m_sLight.Range ) )
					{
						//////////////////////////////////////////////////////////////////////////
						//				Determine Lit Pixels (PointLight) #1
						pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
						pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
						pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
						pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );
						pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_GREATEREQUAL );
						pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	FALSE );

						UINT numPasses;
						pFX->Begin( &numPasses, 0 );
						if( 5 < numPasses )
						{
							pFX->BeginPass( 5 );
							{
								// FX 마무리
								pFX->CommitChanges();
								EDITMESHS::DrawMeshSphere();
							}
							pFX->EndPass();
						}
						pFX->End();
					}
					else
					{
						//////////////////////////////////////////////////////////////////////////
						//				Determine Lit Pixels (PointLight) #1
						pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,	0L );
						pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );
						pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_GREATEREQUAL );
						pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	TRUE );
						pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_ALWAYS );
						pd3dDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_ZERO );
						pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3DSTENCILOP_INCR );
						EDITMESHS::DrawMeshSphere();



						//////////////////////////////////////////////////////////////////////////
						//				Determine Lit Pixels (PointLight) #2
						pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
						pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
						pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
						pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
						pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );
						pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,	0x0000000f );
						pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_EQUAL );
						pd3dDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP );
						pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP );

						UINT numPasses;
						pFX->Begin( &numPasses, 0 );
						if( 5 < numPasses )
						{
							pFX->BeginPass( 5 );
							{
								// FX 마무리
								pFX->CommitChanges();
								EDITMESHS::DrawMeshSphere();
							}
							pFX->EndPass();
						}
						pFX->End();
					}

					pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
					pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
					pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );
				}
			}


			//////////////////////////////////////////////////////////////////////////
			//						Common
			pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,		FALSE );
		}



		//////////////////////////////////////////////////////////////////////////
		// Material Pass (deffered)
		pd3dDevice->SetRenderTarget( 0, pColorSuf_Main );

		pFX->SetTexture( "g_BackBufferTexture",			pColorTex_1st );
		pFX->SetTexture( "g_BackBufferTexture_2nd",		pColorTex_2nd );
		pFX->SetTexture( "g_BackBufferTexture_3rd",		pColorTex_3rd );
		pFX->SetTexture( "g_BackBufferTexture_Depth",	pColorTex_FourCC_INTZ );
		pFX->SetTexture( "g_LightAccumulateTexture",	pColorTex_LBuffer );
		pFX->SetTexture( "g_GlobalCubeTexture",			NSCubeMapUtil::NSCubeMapForPSF::GetCubeMapTexture() );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

		UINT unPass = 6;
		if ( bSoftAlpha )
		{
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				unPass = 7;
			}
		}
		else
		{
			////////////////////////////////////////////////////////////////////////////
			//				Sky 때문에 Z 값의 검사를 하자.
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_GREATEREQUAL );
		}

		if ( hTechniqueHandle )
		{
			pFX->SetTechnique(hTechniqueHandle);

			// 이 부분 깔끔하게 처리할 필요가 있다.
			NSMaterialManager::SetFX_CommonParameter_Small_Fog( pFX );
			NSMaterialManager::SetFX_DefferedTool( pFX );

			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( unPass < numPasses )
			{
				pFX->BeginPass( unPass );
				{
					g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				0.99999f, 1.f );
					g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,		0.f,				0.99999f, 1.f );
					g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	0.99999f, 1.f );
					g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,		(float)dwHeight,	0.99999f, 1.f );

					// FX 마무리
					pFX->CommitChanges();

					pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );
				}
				pFX->EndPass();
			}
			pFX->End();
		}



		//////////////////////////////////////////////////////////////////////////
		//							마무리
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			dwZEnable );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,			dwZFunc );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );



		//////////////////////////////////////////////////////////////////////////
		if ( bSoftAlpha )
		{
			pSetLandMan->Render_DecalPass2_WaterCustomSoftAlpha( pd3dDevice );
		}
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														RenderDefferedForPiece
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
								BOOL bSoftAlpha )
	{
		//////////////////////////////////////////////////////////////////////////
		// Check FX
		LPD3DXEFFECT pFX = NSMaterialManager::GetDefferedFX_Tool();
		if ( !pFX )
			return;

		D3DXMATRIX matInverseView;
		D3DXMatrixInverse( &matInverseView, NULL,		&matView );
		pFX->SetMatrixTranspose( "g_matInverseView",	&matInverseView );



		//////////////////////////////////////////////////////////////////////////
		// Create Quad
		struct VERTEXRHW_CUBE
		{
			D3DXVECTOR4 vPos;
			D3DXVECTOR2 vTex;
		};
		VERTEXRHW				g_sVertBase[4];
		g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				1.f, 1.f );
		g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,		0.f,				1.f, 1.f );
		g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	1.f, 1.f );
		g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,		(float)dwHeight,	1.f, 1.f );
		g_sVertBase[0].vTex = D3DXVECTOR2 ( 0.5f/dwWidth,		0.5f/dwHeight );
		g_sVertBase[1].vTex = D3DXVECTOR2 ( 1.f+0.5f/dwWidth,	0.5f/dwHeight );
		g_sVertBase[2].vTex = D3DXVECTOR2 ( 0.5f/dwWidth,		1.f+0.5f/dwHeight );
		g_sVertBase[3].vTex = D3DXVECTOR2 ( 1.f+0.5f/dwWidth,	1.f+0.5f/dwHeight );



		//////////////////////////////////////////////////////////////////////////
		// RenderTarget Setting
		pd3dDevice->SetRenderTarget( 0, pColorSuf_1st );
		pd3dDevice->SetRenderTarget( 1, pColorSuf_2nd );
		pd3dDevice->SetRenderTarget( 2, pColorSuf_3rd );
		pd3dDevice->SetRenderTarget( 3, pColorSuf_4th );
		if ( bSoftAlpha )
		{
			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 1 );
		}
		else
		{
			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, colorClear, 1.0f, 1 );
		}



		//////////////////////////////////////////////////////////////////////////
		// Geometry Pass
		DWORD dwZEnable, dwZFunc, dwZWriteEnable, dwAlphaBlendEnable;
		pd3dDevice->GetRenderState( D3DRS_ZENABLE,			&dwZEnable );
		pd3dDevice->GetRenderState( D3DRS_ZFUNC,			&dwZFunc );
		pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );

		if ( bSoftAlpha )
		{
			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,					D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_INVSRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA,			D3DBLEND_ONE );
				pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA,			D3DBLEND_INVSRCALPHA );
			}

			pPieceEdit->RenderDefferedSoftAlpha( pd3dDevice, matWorld );

			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			FALSE );
				pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	FALSE );
			}
		}
		else
		{
			pPieceEdit->RenderDeffered( pd3dDevice, matWorld );
			pPieceEdit->RenderDefferedDecal( pd3dDevice, matWorld );	// Decal
		}



		//////////////////////////////////////////////////////////////////////////
		// Light Accumulation Pass
		pd3dDevice->SetRenderTarget( 0, pColorSuf_LBuffer );
		pd3dDevice->SetRenderTarget( 1, NULL );
		pd3dDevice->SetRenderTarget( 2, NULL );
		pd3dDevice->SetRenderTarget( 3, NULL );
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

		pFX->SetTexture( "g_BackBufferTexture_2nd", pColorTex_2nd );
		pFX->SetTexture( "g_BackBufferTexture_3rd", pColorTex_3rd );
		pFX->SetTexture( "g_BackBufferTexture_4th", pColorTex_4th );
		pFX->SetTexture( "g_BackBufferTexture_Depth", pColorTex_FourCC_INTZ );

		D3DXHANDLE hTechniqueHandle = pFX->GetTechniqueByName("tool_deffered");
		if ( hTechniqueHandle )
		{
			//////////////////////////////////////////////////////////////////////////
			//						SetTechnique
			pFX->SetTechnique(hTechniqueHandle);

			// 이 부분 깔끔하게 처리할 필요가 있다.
			NSMaterialManager::SetFX_Deffered( pFX );



			//////////////////////////////////////////////////////////////////////////
			//			Determine Lit Pixels (DirectionLight)
			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( 3 < numPasses )
			{
				pFX->BeginPass( 3 );
				{
					// FX 마무리
					pFX->CommitChanges();

					pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );
				}
				pFX->EndPass();
			}
			pFX->End();



			//////////////////////////////////////////////////////////////////////////
			//						Common
			pd3dDevice->SetRenderState( D3DRS_STENCILREF,		0 );
			pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,			TRUE );



			//////////////////////////////////////////////////////////////////////////
			//						Common
			pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,		FALSE );
		}



		//////////////////////////////////////////////////////////////////////////
		// Material Pass (deffered)
		pd3dDevice->SetRenderTarget( 0, pColorSuf_Main );

		pFX->SetTexture( "g_BackBufferTexture",			pColorTex_1st );
		pFX->SetTexture( "g_BackBufferTexture_2nd",		pColorTex_2nd );
		pFX->SetTexture( "g_BackBufferTexture_3rd",		pColorTex_3rd );
		pFX->SetTexture( "g_BackBufferTexture_Depth",	pColorTex_FourCC_INTZ );
		pFX->SetTexture( "g_LightAccumulateTexture",	pColorTex_LBuffer );
		pFX->SetTexture( "g_GlobalCubeTexture",			NSCubeMapUtil::NSCubeMapForPSF::GetCubeMapTexture() );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

		UINT unPass = 6;
		if ( bSoftAlpha )
		{
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				unPass = 7;
			}
		}
		else
		{
			////////////////////////////////////////////////////////////////////////////
			//				Sky 때문에 Z 값의 검사를 하자.
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_GREATEREQUAL );
		}

		if ( hTechniqueHandle )
		{
			pFX->SetTechnique(hTechniqueHandle);

			// 이 부분 깔끔하게 처리할 필요가 있다.
			NSMaterialManager::SetFX_CommonParameter_Small_Fog( pFX );
			NSMaterialManager::SetFX_DefferedTool( pFX );

			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( unPass < numPasses )
			{
				pFX->BeginPass( unPass );
				{
					g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				0.99999f, 1.f );
					g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,		0.f,				0.99999f, 1.f );
					g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	0.99999f, 1.f );
					g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,		(float)dwHeight,	0.99999f, 1.f );

					// FX 마무리
					pFX->CommitChanges();

					pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );
				}
				pFX->EndPass();
			}
			pFX->End();
		}



		//////////////////////////////////////////////////////////////////////////
		//							마무리
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			dwZEnable );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,			dwZFunc );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );



		//////////////////////////////////////////////////////////////////////////
		//
		if ( bSoftAlpha )
		{
			pPieceEdit->RenderDecalPass2( pd3dDevice, matWorld );
		}
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														RenderDefferedForSM30DF
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
									BOOL bMaterialFrameMove )
	{
		// NM 쪽에서는 반사나 그림자가 렌더링 못한다.
		// 기본 Surface 에서 사용하는 것만 렌더링 된다.
		NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType( bReflect ? NSLandThread::TYPE_REFLECTION_CULL : NSLandThread::TYPE_BASEMESH_CULL );




		//////////////////////////////////////////////////////////////////////////
		// Clip Plane
		if ( bReflect )
		{
			// Note : Clip Plane
			NSENVIRONMENT::SetReflectONOFF( pd3dDevice, TRUE );
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
		}



		//////////////////////////////////////////////////////////////////////////
		//						Sky Render
		if ( !bReflect && !bSoftAlpha && bSky )
		{
			pd3dDevice->SetRenderTarget ( 0, pColorSuf_Main );

			DxSkyBoxMan::GetInstance().Render( pd3dDevice );

			// 하늘 렌더링..!! 본체
			DxSkyMan::GetInstance().Render( pd3dDevice, DxViewPort::GetInstance().GetFOV(), DxViewPort::GetInstance().GetAspect() );
		}



		//////////////////////////////////////////////////////////////////////////
		// Check FX
		LPD3DXEFFECT pFX = NSMaterialManager::GetDefferedFX();
		if ( !pFX )
			return;

		D3DXMATRIX matInverseView;
		D3DXMatrixInverse( &matInverseView, NULL,		&matView );
		pFX->SetMatrixTranspose( "g_matInverseView",	&matInverseView );



		//////////////////////////////////////////////////////////////////////////
		// Create Quad
		struct VERTEXRHW_CUBE
		{
			D3DXVECTOR4 vPos;
			D3DXVECTOR2 vTex;
		};
		VERTEXRHW				g_sVertBase[4];
		g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				1.f, 1.f );
		g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,		0.f,				1.f, 1.f );
		g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	1.f, 1.f );
		g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,		(float)dwHeight,	1.f, 1.f );
		g_sVertBase[0].vTex = D3DXVECTOR2 ( 0.5f/dwWidth,		0.5f/dwHeight );
		g_sVertBase[1].vTex = D3DXVECTOR2 ( 1.f+0.5f/dwWidth,	0.5f/dwHeight );
		g_sVertBase[2].vTex = D3DXVECTOR2 ( 0.5f/dwWidth,		1.f+0.5f/dwHeight );
		g_sVertBase[3].vTex = D3DXVECTOR2 ( 1.f+0.5f/dwWidth,	1.f+0.5f/dwHeight );



		//////////////////////////////////////////////////////////////////////////
		// water 렌더링.
		if ( bSoftAlpha && !bReflect )
		{
			pLandMan->RenderWater_Deffered( pd3dDevice, emCullingRenderType );
		}




		//////////////////////////////////////////////////////////////////////////
		// RenderTarget Setting
		pd3dDevice->SetRenderTarget( 0, pColorSuf_1st );
		pd3dDevice->SetRenderTarget( 1, pColorSuf_2nd );
		pd3dDevice->SetRenderTarget( 2, pColorSuf_3rd );
		if ( bSoftAlpha )
		{
			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 1 );
		}
		else
		{
			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, colorClear, 1.0f, 1 );
		}



		//////////////////////////////////////////////////////////////////////////
		// Geometry Pass
		DWORD dwZEnable, dwZFunc, dwZWriteEnable, dwAlphaBlendEnable;
		pd3dDevice->GetRenderState( D3DRS_ZENABLE,			&dwZEnable );
		pd3dDevice->GetRenderState( D3DRS_ZFUNC,			&dwZFunc );
		pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );

		if ( bSoftAlpha )
		{
			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,					D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_INVSRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA,			D3DBLEND_ONE );
				pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA,			D3DBLEND_INVSRCALPHA );
			}

			pLandMan->Render_SoftAlpha_SM30DF( pd3dDevice, emCullingRenderType, bReflect, fReflectHeight, matWorld );
			pLandMan->RenderSoftAlphaForLodBlend( pd3dDevice, emCullingRenderType, bReflect, fReflectHeight, matWorld );

			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			FALSE );
				pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	FALSE );
			}
		}
		else
		{
			pLandMan->Render_Opaque_SM30DF
			( 
				pd3dDevice, 
				sCameraCV,
				emCullingRenderType,
				bReflect,
				fReflectHeight, 
				pColorTex_1st,
				pColorTex_2nd,
				pColorTex_3rd,
				NULL,
				matWorld, 
				matView, 
				matProj,
				bMaterialFrameMove
			);	// Opaque, HardAlpha
			pLandMan->RenderForLodBlend( pd3dDevice, emCullingRenderType, bReflect, fReflectHeight, matWorld );					// Opaque, HardAlpha

			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );
			pd3dDevice->SetRenderTarget( 1, NULL );
			pd3dDevice->SetRenderTarget( 2, NULL );
			pLandMan->RenderDecal_Deffered( pd3dDevice, emCullingRenderType, bReflect, fReflectHeight, matWorld );	// Decal
		}
		


		//////////////////////////////////////////////////////////////////////////
		// Material Pass (deffered)
		pd3dDevice->SetRenderTarget( 0, pColorSuf_Main );
		pd3dDevice->SetRenderTarget( 1, NULL );
		pd3dDevice->SetRenderTarget( 2, NULL );

		//if ( bSoftAlpha && !bReflect && bSky )
		//{
		//	pLandMan->RenderEff_Deffered( pd3dDevice, sCameraCV );
		//}

		pFX->SetTexture( "g_BackBufferTexture",			pColorTex_1st );
		pFX->SetTexture( "g_BackBufferTexture_2nd",		pColorTex_2nd );
		pFX->SetTexture( "g_BackBufferTexture_3rd",		pColorTex_3rd );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

		// 이 부분 깔끔하게 처리할 필요가 있다.
		// g_matInvView_Shadow
		// g_vShadow
		// g_ShadowTexture
		D3DXMATRIX matInvView_Shadow;
		D3DXMatrixMultiply( &matInvView_Shadow, &matInverseView, &DxShadowMap::GetInstance().GetMatrix_D() );

		D3DXMATRIX matLightViewProjClip2Tex;
		DxShadowMap::GetInstance().GetMultiply_Clip2Tex( matLightViewProjClip2Tex, matInvView_Shadow );

		pFX->SetMatrixTranspose( "g_matInvView_Shadow_Clip2Tex",	&matLightViewProjClip2Tex );

		UINT unPass = 2;
		if ( bSoftAlpha )
		{
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				unPass = 3;
			}
		}
		else
		{
			////////////////////////////////////////////////////////////////////////////
			//				Sky 때문에 Z 값의 검사를 하자.
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_GREATEREQUAL );
		}

		if ( bReflect )
		{
			// Note : Clip Plane
			NSENVIRONMENT::SetReflectONOFF( pd3dDevice, FALSE );
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
		}

		if ( bCharShadow )
		{
			unPass += 2;

			//if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 )
			//{
			//	unPass += 2;
			//}
		}

		//D3DXVECTOR4 vShadow( DxSurfaceTex::CASCADESHADOWSIZE.z, 0.001f, 0.4f, 0.f );	// x-SIZE(해상도), y-EPSILON(Z-Fighting때문에), z-COLOR(낮은 값일 수록 어두움)
		//pFX->SetVector( "g_vShadow",	&vShadow );
		//pFX->SetTexture( "g_ShadowTexture",	DxSurfaceTex::GetInstance().m_pShadowTexR32F );

		//if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 )
		//{
		//	D3DXVECTOR4 vShadow( DxSurfaceTex::CASCADESHADOWSIZE.z, 0.5f, 1.f/DxSurfaceTex::CASCADESHADOWSIZE.z, 0.4f );	// x-SIZE(해상도)
		//	pFX->SetVector( "g_vShadow", &vShadow );
		//	pFX->SetTexture( "g_ShadowTexture",	DxSurfaceTex::GetInstance().m_pShadowTexG32R32F_VSM_CSM2 );

		//	pFX->SetVector( "g_vLightPosWorldSpace",	&NSMaterialManager::g_vLightPosWorldSpace );
		//}
		//else
		{
			D3DXVECTOR4 vShadow( DxSurfaceTex::CASCADESHADOWSIZE.z, 0.001f, 1.f/DxSurfaceTex::CASCADESHADOWSIZE.z, 0.4f );	// x-SIZE(해상도), y-EPSILON(Z-Fighting때문에)
			pFX->SetVector( "g_vShadow", &vShadow );
			pFX->SetTexture( "g_ShadowTexture",	DxSurfaceTex::GetInstance().m_pShadowTexR32F );
		}

		D3DXHANDLE hTechniqueHandle = pFX->GetTechniqueByName("game");
		if ( hTechniqueHandle )
		{
			pFX->SetTechnique(hTechniqueHandle);

			// 이 부분 깔끔하게 처리할 필요가 있다.
			NSMaterialManager::SetFX_CommonParameter_Small_Fog( pFX );

			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( unPass < numPasses )
			{
				pFX->BeginPass( unPass );
				{
					g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				0.99999f, 1.f );
					g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,		0.f,				0.99999f, 1.f );
					g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	0.99999f, 1.f );
					g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,		(float)dwHeight,	0.99999f, 1.f );

					// FX 마무리
					pFX->CommitChanges();

					pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );
				}
				pFX->EndPass();
			}
			pFX->End();
		}



		//////////////////////////////////////////////////////////////////////////
		//							마무리
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			dwZEnable );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,			dwZFunc );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );



		//////////////////////////////////////////////////////////////////////////
		//							Decal ADD
		if ( bSoftAlpha )
		{
			//pLandMan->RenderDecalPass2( pd3dDevice, sCameraCV );
		}
		else
		{
			pLandMan->RenderDecalPass2( pd3dDevice, emCullingRenderType, bReflect );

			// 현재 렌더모드에서만 확인을 하면 된다.
			// 어쩔 수 없이 이전 렌더링에 맞추기 위한 잘못된 코드. 로그인화면이 바뀌면 이 코드는 삭제해 버리자.
			pLandMan->RenderMS_FrameAnimation_Will_Erase( pd3dDevice, sCameraCV );
		}
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														RenderDefferedForSM30NM
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
									BOOL bSoftAlpha )
	{
		// NM 쪽에서는 반사나 그림자가 렌더링 못한다.
		// 기본 Surface 에서 사용하는 것만 렌더링 된다.
		NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType(NSLandThread::TYPE_BASEMESH_CULL);

		// 그림자 렌더전에 NSShadowLight::FrameMove 를 호출해줘야한다.
		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
		{
			NSShadowLight::PreRender( DxViewPort::GetInstance().GetMatView() );
		}



		//////////////////////////////////////////////////////////////////////////
		//						Sky Render
		if ( !bSoftAlpha )
		{
			pd3dDevice->SetRenderTarget ( 0, pColorSuf_Main );

			DxSkyBoxMan::GetInstance().Render( pd3dDevice );

			// 하늘 렌더링..!! 본체
			DxSkyMan::GetInstance().Render( pd3dDevice, DxViewPort::GetInstance().GetFOV(), DxViewPort::GetInstance().GetAspect() );
		}



		//////////////////////////////////////////////////////////////////////////
		// Check FX
		LPD3DXEFFECT pFX = NSMaterialManager::GetDefferedFX();
		if ( !pFX )
			return;

		D3DXMATRIX matInverseView;
		D3DXMatrixInverse( &matInverseView, NULL,		&matView );
		pFX->SetMatrixTranspose( "g_matInverseView",	&matInverseView );



		//////////////////////////////////////////////////////////////////////////
		// Create Quad
		struct VERTEXRHW_CUBE
		{
			D3DXVECTOR4 vPos;
			D3DXVECTOR2 vTex;
		};
		VERTEXRHW				g_sVertBase[4];
		g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				0.99999f, 1.f );
		g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,		0.f,				0.99999f, 1.f );
		g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	0.99999f, 1.f );
		g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,		(float)dwHeight,	0.99999f, 1.f );
		g_sVertBase[0].vTex = D3DXVECTOR2 ( 0.5f/dwWidth,		0.5f/dwHeight );
		g_sVertBase[1].vTex = D3DXVECTOR2 ( 1.f+0.5f/dwWidth,	0.5f/dwHeight );
		g_sVertBase[2].vTex = D3DXVECTOR2 ( 0.5f/dwWidth,		1.f+0.5f/dwHeight );
		g_sVertBase[3].vTex = D3DXVECTOR2 ( 1.f+0.5f/dwWidth,	1.f+0.5f/dwHeight );



		//////////////////////////////////////////////////////////////////////////
		// water 렌더링.
		if ( bSoftAlpha )
		{
			pLandMan->RenderWater_Deffered( pd3dDevice, emCullingRenderType );
		}




		//////////////////////////////////////////////////////////////////////////
		// RenderTarget Setting
		pd3dDevice->SetRenderTarget( 0, pColorSuf_1st );
		pd3dDevice->SetRenderTarget( 1, pColorSuf_2nd );
		pd3dDevice->SetRenderTarget( 2, pColorSuf_3rd );
		pd3dDevice->SetRenderTarget( 3, pColorSuf_4th );
		if ( bSoftAlpha )
		{
			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 1 );
		}
		else
		{
			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, colorClear, 1.0f, 1 );
		}



		//////////////////////////////////////////////////////////////////////////
		// Geometry Pass
		DWORD dwZEnable, dwZFunc, dwZWriteEnable, dwAlphaBlendEnable;
		pd3dDevice->GetRenderState( D3DRS_ZENABLE,			&dwZEnable );
		pd3dDevice->GetRenderState( D3DRS_ZFUNC,			&dwZFunc );
		pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );

		//pd3dDevice->SetRenderState( D3DRS_STENCILREF,		0 );
		//pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	TRUE );
		//pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_ALWAYS );
		//pd3dDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_ZERO );
		//pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP );

		if ( bSoftAlpha )
		{
			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,					D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_INVSRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA,			D3DBLEND_ONE );
				pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA,			D3DBLEND_INVSRCALPHA );
			}

			pLandMan->Render_SoftAlpha_SM30DF( pd3dDevice, emCullingRenderType, FALSE, 0.f, matWorld );
			pLandMan->RenderSoftAlphaForLodBlend( pd3dDevice, emCullingRenderType, FALSE, 0.f, matWorld );

			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			FALSE );
				pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,	FALSE );
			}
		}
		else
		{
			pLandMan->Render_Opaque_SM30DF
			( 
				pd3dDevice, 
				sCameraCV,
				emCullingRenderType,
				FALSE, 
				0.f, 
				pColorTex_1st,
				pColorTex_2nd, 
				pColorTex_3rd,
				pColorTex_4th,
				matWorld, 
				matView, 
				matProj,
				TRUE
			);																			// Opaque, HardAlpha
			pLandMan->RenderForLodBlend( pd3dDevice, emCullingRenderType, FALSE, 0.f, matWorld );			// Opaque, HardAlpha

			pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );
			pd3dDevice->SetRenderTarget( 3, NULL );
			pLandMan->RenderDecal_Deffered( pd3dDevice, emCullingRenderType, FALSE, 0.f, matWorld );	// Decal
		}



		//////////////////////////////////////////////////////////////////////////
		// Light Accumulation Pass (DirectionLight) (CSM)
		pd3dDevice->SetRenderTarget( 0, pColorSuf_LBuffer );
		pd3dDevice->SetRenderTarget( 1, NULL );
		pd3dDevice->SetRenderTarget( 2, NULL );
		pd3dDevice->SetRenderTarget( 3, NULL );
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

		pFX->SetTexture( "g_BackBufferTexture_2nd", pColorTex_2nd );
		pFX->SetTexture( "g_BackBufferTexture_3rd", pColorTex_3rd );
		pFX->SetTexture( "g_BackBufferTexture_4th", pColorTex_4th );
		pFX->SetTexture( "g_DepthBuffer",			pColorTex_FourCC_INTZ );

		D3DXHANDLE hTechniqueHandle = pFX->GetTechniqueByName("game");
		if ( hTechniqueHandle )
		{
			//////////////////////////////////////////////////////////////////////////
			//						SetTechnique
			pFX->SetTechnique(hTechniqueHandle);

			NSMaterialManager::SetFX_Deffered( pFX );

			if ( NSShadowLight::IsCascadeShadow() )
			{
				//////////////////////////////////////////////////////////////////////////
				//			Determine Lit Pixels (DirectionLight)
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
				//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED );
				//pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_EQUAL );

				////////////////////////////////////////////////////////////////////////////
				//				Sky 때문에 Z 값의 검사를 하자.
				pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
				pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_GREATEREQUAL );

				if ( !bSoftAlpha )
				{
					UINT unDirectionLightPass(0);
					if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
					{
						unDirectionLightPass = 1;
					}
					UINT numPasses;
					pFX->Begin( &numPasses, 0 );
					if( unDirectionLightPass < numPasses )
					{
						pFX->BeginPass( unDirectionLightPass );
						{
							// FX 마무리
							pFX->CommitChanges();

							pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
							pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );
						}
						pFX->EndPass();
					}
					pFX->End();
				}

				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
				//pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,	0xf );
				pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	FALSE );
				pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );



				//////////////////////////////////////////////////////////////////////////
				//						Common
				pd3dDevice->SetRenderState( D3DRS_STENCILREF,		0 );
				pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	TRUE );
				pd3dDevice->SetRenderState( D3DRS_ZENABLE,			TRUE );

				////////////////////////////////////////////////////////////////////////////
				////				Determine Lit Pixels (SpotLight)
				//if ( RENDERPARAM::g_bDefferedRendering_INTZ )
				//{
				//	D3DXQUATERNION	vQuat;
				//	D3DXVECTOR3 vScale, vPos;
				//	D3DXMATRIX matWorld, matRotate;
				//	const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& vecShadowSpotLight = NSShadowLight::GetVecShadowSpotLight();
				//	std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::const_iterator citer = vecShadowSpotLight.begin();
				//	for( ; citer!=vecShadowSpotLight.end(); ++citer )
				//	{
				//		//////////////////////////////////////////////////////////////////////////
				//		//							Common
				//		//D3DXMatrixToSQT( vScale, vQuat, vPos, (*citer)->m_matWorld );
				//		//D3DXMatrixRotationQuaternion( &matRotate, &vQuat );
				//		//D3DXMatrixScaling( &matWorld, (*citer)->m_sLight.Range, (*citer)->m_sLight.Range, (*citer)->m_sLight.Range );
				//		//D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );
				//		//matWorld._41 = (*citer)->m_sLight.Position.x;
				//		//matWorld._42 = (*citer)->m_sLight.Position.y;
				//		//matWorld._43 = (*citer)->m_sLight.Position.z;
				//		//pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
				//		D3DXMatrixScaling( &matWorld, (*citer)->m_sLight.Range, (*citer)->m_sLight.Range, (*citer)->m_sLight.Range );
				//		matWorld._41 = (*citer)->m_sLight.Position.x;
				//		matWorld._42 = (*citer)->m_sLight.Position.y;
				//		matWorld._43 = (*citer)->m_sLight.Position.z;
				//		pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

				//		D3DXVECTOR3 vPos((*citer)->m_sLight.Position);
				//		D3DXVec3TransformCoord( &vPos, &vPos, &matView );
				//		D3DXVECTOR4 vPos_Range_CL_TOOL( vPos, (*citer)->m_sLight.Range );
				//		D3DXVECTOR4 vDiff_CosPhiHalf_CL_TOOL( (*citer)->m_sLight.Diffuse.r, (*citer)->m_sLight.Diffuse.g, (*citer)->m_sLight.Diffuse.b, cosf(D3DX_PI*(*citer)->m_sLight.Phi/180.f*0.5f) );
				//		D3DXVECTOR4 vAtt_CosHalfThetaSubPhi_CL_TOOL( (*citer)->m_sLight.Attenuation0, (*citer)->m_sLight.Attenuation1, (*citer)->m_sLight.Attenuation2, cosf(D3DX_PI*(*citer)->m_sLight.Theta/180.f*0.5f)-cosf(D3DX_PI*(*citer)->m_sLight.Phi/180.f*0.5f) );
				//		D3DXVECTOR3 vDir(-(*citer)->m_sLight.Direction.x,-(*citer)->m_sLight.Direction.y,-(*citer)->m_sLight.Direction.z);
				//		D3DXVec3TransformNormal( &vDir, &vDir, &matView );
				//		D3DXVECTOR4 vDirect_Falloff_CL_TOOL( vDir, (*citer)->m_sLight.Falloff );
				//		float fAttEndLineValue_CL_TOOL((*citer)->m_fAttEndLineValue);
				//		float fOverLighting_CL_TOOL((*citer)->m_fOverLighting);

				//		D3DXMATRIX matWV, matWVP, matInverseProj;
				//		D3DXMatrixMultiply( &matWV, &matWorld,		&matView );
				//		D3DXMatrixMultiply( &matWVP, &matWV,		&matProj );
				//		D3DXMatrixInverse( &matInverseProj, NULL,	&matProj );
				//		pFX->SetMatrixTranspose( "g_matLightObjectWV",		&matWV );
				//		pFX->SetMatrixTranspose( "g_matLightObjectWVP",		&matWVP );
				//		pFX->SetMatrixTranspose( "g_matInverseProj",		&matInverseProj );
				//		pFX->SetVector( "g_vPos_Range_CL_TOOL",					&vPos_Range_CL_TOOL );
				//		pFX->SetVector( "g_vDiff_CosPhiHalf_CL_TOOL",			&vDiff_CosPhiHalf_CL_TOOL );
				//		pFX->SetVector( "g_vAtt_CosHalfThetaSubPhi_CL_TOOL",	&vAtt_CosHalfThetaSubPhi_CL_TOOL );
				//		pFX->SetVector( "g_vDirect_Falloff_CL_TOOL",			&vDirect_Falloff_CL_TOOL );
				//		pFX->SetFloat( "g_fAttEndLineValue_CL_TOOL",			fAttEndLineValue_CL_TOOL );
				//		pFX->SetFloat( "g_fOverLighting_CL_TOOL",				fOverLighting_CL_TOOL );



				//		//////////////////////////////////////////////////////////////////////////
				//		//					구안에 들어갔다면... 혹은 밖에 있다면
				//		if ( COLLISION::IsCollisionVolume( sCameraCV, (*citer)->m_sLight.Position, (*citer)->m_sLight.Range ) )
				//		{
				//			//////////////////////////////////////////////////////////////////////////
				//			//				Determine Lit Pixels (PointLight) #1
				//			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
				//			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
				//			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
				//			pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );
				//			pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_GREATEREQUAL );
				//			pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	FALSE );

				//			UINT numPasses;
				//			pFX->Begin( &numPasses, 0 );
				//			if( 1 < numPasses )
				//			{
				//				pFX->BeginPass( 1 );
				//				{
				//					// FX 마무리
				//					pFX->CommitChanges();
				//					EDITMESHS::DrawMeshSphere();
				//				}
				//				pFX->EndPass();
				//			}
				//			pFX->End();
				//		}
				//		else
				//		{
				//			//////////////////////////////////////////////////////////////////////////
				//			//				Determine Lit Pixels (PointLight) #1
				//			pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,	0L );
				//			pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );
				//			pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_GREATEREQUAL );
				//			pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,	TRUE );
				//			pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_ALWAYS );
				//			pd3dDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_ZERO );
				//			pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3DSTENCILOP_INCR );
				//			EDITMESHS::DrawMeshSphere();



				//			//////////////////////////////////////////////////////////////////////////
				//			//				Determine Lit Pixels (PointLight) #2
				//			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
				//			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
				//			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
				//			pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
				//			pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );
				//			pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,	0x0000000f );
				//			pd3dDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_EQUAL );
				//			pd3dDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP );
				//			pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP );

				//			UINT numPasses;
				//			pFX->Begin( &numPasses, 0 );
				//			if( 1 < numPasses )
				//			{
				//				pFX->BeginPass( 1 );
				//				{
				//					// FX 마무리
				//					pFX->CommitChanges();
				//					EDITMESHS::DrawMeshSphere();
				//				}
				//				pFX->EndPass();
				//			}
				//			pFX->End();
				//		}

				//		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
				//		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
				//		pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );
				//	}
				//}


				//////////////////////////////////////////////////////////////////////////
				//						Common
				pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,		FALSE );
			}
		}



		//////////////////////////////////////////////////////////////////////////
		// Light Accumulation Pass ( Geometry )
		pd3dDevice->SetRenderTarget( 0, pColorSuf_LBuffer );
		pd3dDevice->SetRenderTarget( 1, NULL );
		pd3dDevice->SetRenderTarget( 2, NULL );
		pd3dDevice->SetRenderTarget( 3, NULL );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,					D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_ONE );

		if ( bSoftAlpha )
		{
			pLandMan->Render_SoftAlpha_Spot_LBuffer( pd3dDevice, emCullingRenderType );
		}
		else
		{
			pLandMan->Render_Opaque_Spot_LBuffer( pd3dDevice, emCullingRenderType );	// Opaque, HardAlpha
		}

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,			FALSE );



		//////////////////////////////////////////////////////////////////////////
		// Material Pass (deffered)
		pd3dDevice->SetRenderTarget( 0, pColorSuf_Main );
		pd3dDevice->SetRenderTarget( 1, NULL );
		pd3dDevice->SetRenderTarget( 2, NULL );
		pd3dDevice->SetRenderTarget( 3, NULL );

		//if ( bSoftAlpha )
		//{
		//	pLandMan->RenderEff_Deffered( pd3dDevice, sCameraCV );
		//}

		pFX->SetTexture( "g_BackBufferTexture",			pColorTex_1st );
		pFX->SetTexture( "g_BackBufferTexture_2nd",		pColorTex_2nd );
		pFX->SetTexture( "g_BackBufferTexture_3rd",		pColorTex_3rd );
		pFX->SetTexture( "g_BackBufferTexture_4th",		pColorTex_4th );
		pFX->SetTexture( "g_BackBufferTexture_LBuffer",	pColorTex_LBuffer );
		pFX->SetTexture( "g_DepthBuffer",				pColorTex_FourCC_INTZ );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

		UINT unPass = 6;
		if ( bSoftAlpha )
		{
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

			if ( RENDERPARAM::g_bSeparateAlphaBlend )
			{
				unPass = 7;
			}
		}
		else
		{
			////////////////////////////////////////////////////////////////////////////
			//				Sky 때문에 Z 값의 검사를 하자.
			pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
			pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_GREATEREQUAL );
		}

		// CSM 이 안되므로 단순한 단일 Shadow 라도 렌더링 해야한다.
		if ( !NSShadowLight::IsCascadeShadow() )
		{
			unPass += 2;

			// CSM 일 경우는 Spot Light 에 들어갈 경우 기본 그림자를 FadeOut 시켜준다.
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
			{
				unPass += 2;
			}

			//// 이 부분 깔끔하게 처리할 필요가 있다.
			//// g_matInvView_Shadow
			//// g_vShadow
			//// g_ShadowTexture
			//D3DXMATRIX matInvView_Shadow;
			//D3DXMatrixMultiply( &matInvView_Shadow, &matInverseView, &DxShadowMap::GetInstance().GetMatrix_D() );

			//D3DXMATRIX matLightViewProjClip2Tex;
			//DxShadowMap::GetInstance().GetMultiply_Clip2Tex( matLightViewProjClip2Tex, matInvView_Shadow );

			//pFX->SetMatrixTranspose( "g_matInvView_Shadow_Clip2Tex",	&matLightViewProjClip2Tex );

			//{
				D3DXVECTOR4 vShadow( DxSurfaceTex::CASCADESHADOWSIZE.z, 0.001f, 1.f/DxSurfaceTex::CASCADESHADOWSIZE.z, 0.4f );	// x-SIZE(해상도), y-EPSILON(Z-Fighting때문에)
				pFX->SetVector( "g_vShadow", &vShadow );
			//	pFX->SetTexture( "g_ShadowTexture",	DxSurfaceTex::GetInstance().m_pShadowTexR32F );
			//}
		}

		if ( hTechniqueHandle )
		{
			pFX->SetTechnique(hTechniqueHandle);

			// 이 부분 깔끔하게 처리할 필요가 있다.
			NSMaterialManager::SetFX_CommonParameter_Small_Fog( pFX );

			UINT numPasses;
			pFX->Begin( &numPasses, 0 );
			if( unPass < numPasses )
			{
				pFX->BeginPass( unPass );
				{
					g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				0.99999f, 1.f );
					g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwWidth,		0.f,				0.99999f, 1.f );
					g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	0.99999f, 1.f );
					g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwWidth,		(float)dwHeight,	0.99999f, 1.f );

					// FX 마무리
					pFX->CommitChanges();

					pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );
				}
				pFX->EndPass();
			}
			pFX->End();
		}



		//////////////////////////////////////////////////////////////////////////
		//							마무리
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			dwZEnable );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,			dwZFunc );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );



		//////////////////////////////////////////////////////////////////////////
		//							Decal ADD
		if ( !bSoftAlpha )
		{
			pLandMan->RenderDecalPass2( pd3dDevice, emCullingRenderType, FALSE );
		}
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//														ComputeClipVolume
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void ComputeClipVolume( CLIPVOLUME& sCameraCV,
							D3DXVECTOR3	&vLookatPt, D3DXVECTOR3 &vFromPt, D3DXVECTOR3 &vUpVec, 
							float fNearPlane, float fWidth, float fHeight, float fFOV)
	{
		sCameraCV = DxViewPort::GetInstance().ComputeClipVolume( vLookatPt,
																	vFromPt,
																	vUpVec,
																	0.0001f,
																	fNearPlane,
																	fWidth,
																	fHeight,
																	fFOV );
	}
};
