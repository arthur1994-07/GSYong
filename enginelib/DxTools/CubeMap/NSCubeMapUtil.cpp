#include "pch.h"

#include "../../DxLand/DxPiece/NSPieceQuickSort.h"
#include "../../DxLand/DxSetLandMan.h"
#include "../../DxLand/NSLandThread.h"
#include "../../DxLand/DxLandMan.h"
#include "../Material/NSMaterialManager.h"
#include "../Deffered/NSDeffered.h"
#include "../Texture/NSTexCompress.h"
#include "../SkyBox/DxSkyBoxMan.h"
#include "../DxBackUpRendTarget.h"
#include "../DxCustomTypes.h"
#include "../DxSurfaceTex.h"
#include "../DxViewPort.h"
#include "../DxFogMan.h"

#include "./NSCubeMapCulling.h"

#include "NSCubeMapUtil.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bWORLD_TOOL;

namespace NSCubeMapUtil
{
	D3DXMATRIX D3DUtil_GetCubeMapViewMatrix_New( DWORD dwFace, D3DXVECTOR3& vLookDir, D3DXVECTOR3& vUpDir )
	{
		D3DXVECTOR3 vEyePt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

		switch( dwFace )
		{
		case D3DCUBEMAP_FACE_POSITIVE_X:
			vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
			vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_X:
			vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
			vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Y:
			vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:
			vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
			vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Z:
			vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
			vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:
			vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
			vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
			break;
		}

		// Set the view transform for this cubemap surface
		D3DXMATRIX matView;
		D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookDir, &vUpDir );
		return matView;
	}

	void SeamlessLine( LPDIRECT3DTEXTUREQ pTextureSeamless1, LPDIRECT3DTEXTUREQ pTextureSeamless2, DWORD dwSize, int nMode, BOOL bPlus1, BOOL bPlus2 )
	{
		D3DLOCKED_RECT	m_stLocked1;
		D3DLOCKED_RECT	m_stLocked2;
		HRESULT hr = pTextureSeamless1->LockRect( 0, &m_stLocked1, NULL, 0 );
		if( FAILED(hr) )
		{
			GASSERT( 0 && _T("HRESULT hr = pTextureSeamless1->LockRect( 0, &m_stLocked1, NULL, 0 );") );
			return;
		}

		hr = pTextureSeamless2->LockRect( 0, &m_stLocked2, NULL, 0 );
		if( FAILED(hr) )
		{
			GASSERT( 0 && _T("hr = pTextureSeamless2->LockRect( 0, &m_stLocked2, NULL, 0 );") );
			pTextureSeamless1->UnlockRect( 0 );
			return;
		}

		INT iPitch = m_stLocked1.Pitch / 4;

		for( DWORD i = 0; i < dwSize; ++i )
		{
			DWORD *dwReturn1;
			D3DXCOLOR cColor1;
			DWORD *dwReturn2;
			D3DXCOLOR cColor2;

			if ( nMode == 0 )
			{
				if ( bPlus1 == FALSE )
				{
					dwReturn1 = ( DWORD* )m_stLocked1.pBits;
					dwReturn1 = &dwReturn1[ (dwSize-1-i) + 0 * iPitch ];
					cColor1 = D3DXCOLOR( *dwReturn1 );

					dwReturn2 = ( DWORD* )m_stLocked2.pBits;
					dwReturn2 = &dwReturn2[ i + 0 * iPitch ];
					cColor2 = D3DXCOLOR( *dwReturn2 );
				}
				else if ( bPlus2 == FALSE )
				{
					dwReturn1 = ( DWORD* )m_stLocked1.pBits;
					dwReturn1 = &dwReturn1[ i + (dwSize-1) * iPitch ];
					cColor1 = D3DXCOLOR( *dwReturn1 );

					dwReturn2 = ( DWORD* )m_stLocked2.pBits;
					dwReturn2 = &dwReturn2[ (dwSize-1-i) + (dwSize-1) * iPitch ];
					cColor2 = D3DXCOLOR( *dwReturn2 );
				}
				else
				{
					dwReturn1 = ( DWORD* )m_stLocked1.pBits;
					dwReturn1 = &dwReturn1[ i + (dwSize-1) * iPitch ];
					cColor1 = D3DXCOLOR( *dwReturn1 );

					dwReturn2 = ( DWORD* )m_stLocked2.pBits;
					dwReturn2 = &dwReturn2[ i + 0 * iPitch ];
					cColor2 = D3DXCOLOR( *dwReturn2 );
				}
			}
			else if ( nMode == 1 )
			{
				dwReturn1 = ( DWORD* )m_stLocked1.pBits;
				dwReturn1 = &dwReturn1[ (dwSize-1) + i * iPitch ];
				cColor1 = D3DXCOLOR( *dwReturn1 );

				dwReturn2 = ( DWORD* )m_stLocked2.pBits;
				dwReturn2 = &dwReturn2[ 0 + i * iPitch ];
				cColor2 = D3DXCOLOR( *dwReturn2 );
			}
			else if ( nMode == 2 )
			{
				dwReturn1 = ( DWORD* )m_stLocked1.pBits;
				dwReturn1 = &dwReturn1[ i + (dwSize-1) * iPitch ];
				cColor1 = D3DXCOLOR( *dwReturn1 );

				dwReturn2 = ( DWORD* )m_stLocked2.pBits;
				dwReturn2 = &dwReturn2[ (dwSize-1) + i * iPitch ];
				cColor2 = D3DXCOLOR( *dwReturn2 );
			}
			else if ( nMode == 3 )
			{
				dwReturn1 = ( DWORD* )m_stLocked1.pBits;
				dwReturn1 = &dwReturn1[ i + (dwSize-1) * iPitch ];
				cColor1 = D3DXCOLOR( *dwReturn1 );

				dwReturn2 = ( DWORD* )m_stLocked2.pBits;
				dwReturn2 = &dwReturn2[ 0 + i * iPitch ];
				cColor2 = D3DXCOLOR( *dwReturn2 );
			}
			else if ( nMode == 4 )
			{
				dwReturn1 = ( DWORD* )m_stLocked1.pBits;
				dwReturn1 = &dwReturn1[ i + 0 * iPitch ];
				cColor1 = D3DXCOLOR( *dwReturn1 );

				dwReturn2 = ( DWORD* )m_stLocked2.pBits;
				dwReturn2 = &dwReturn2[ (dwSize-1) + (dwSize-1-i) * iPitch ];
				cColor2 = D3DXCOLOR( *dwReturn2 );
			}
			else if ( nMode == 5 )
			{
				dwReturn1 = ( DWORD* )m_stLocked1.pBits;
				dwReturn1 = &dwReturn1[ i + 0 * iPitch ];
				cColor1 = D3DXCOLOR( *dwReturn1 );

				dwReturn2 = ( DWORD* )m_stLocked2.pBits;
				dwReturn2 = &dwReturn2[ 0 + i * iPitch ];
				cColor2 = D3DXCOLOR( *dwReturn2 );
			}

			cColor1.r = (cColor1.r + cColor2.r) * 0.5f;
			cColor1.g = (cColor1.g + cColor2.g) * 0.5f;
			cColor1.b = (cColor1.b + cColor2.b) * 0.5f;

			DWORD dwR = static_cast<DWORD>( cColor1.r*255 );
			DWORD dwG = static_cast<DWORD>( cColor1.g*255 );
			DWORD dwB = static_cast<DWORD>( cColor1.b*255 );

			if ( dwR > 255 )	dwR = 255;
			if ( dwG > 255 )	dwG = 255;
			if ( dwB > 255 )	dwB = 255;

			DWORD dwColor = 0xff000000 + (dwR<<16) + (dwG<<8) + dwB;
			*dwReturn1 = dwColor;
			*dwReturn2 = dwColor;
		}

		pTextureSeamless1->UnlockRect( 0 );
		pTextureSeamless2->UnlockRect( 0 );
	}

	void CreateLocalCubeMapDetail( LPDIRECT3DDEVICEQ pd3dDevice, 
									DWORD dwSize, 
									const D3DXVECTOR3& vPos, 
									DxLandMan* pLandMan,
									LPDIRECT3DCUBETEXTUREQ	pCubeTex,
									LPDIRECT3DSURFACEQ	pSurfaceBackUp_1st,
									LPDIRECT3DSURFACEQ	pSurfaceBackUp_2nd,
									LPDIRECT3DSURFACEQ	pSurfaceBackUp_3rd,
									LPDIRECT3DTEXTUREQ	pTextureBackUp_1st,
									LPDIRECT3DTEXTUREQ	pTextureBackUp_2nd,
									LPDIRECT3DTEXTUREQ	pTextureBackUp_3rd,
									LPDIRECT3DSURFACEQ	pZBuffer )
	{
		BOOL bPieceInstancing = NSPieceQuickSort::g_bPieceInstancing;
		NSPieceQuickSort::g_bPieceInstancing = FALSE;

		D3DXMATRIX	matOldView, matView;
		D3DXMATRIX	matOldProj, matProj;

		// 약간 넓혀서 끝라인 부분이 비슷한 곳을 볼수 있도록 작업함.
		float fSize = static_cast<float>(dwSize) * 0.5f;
		float fNewFOV = 2.0f * atan(fSize / (fSize - 0.5f));
		//float fNewFOV = D3DX_PI * 0.5f;

		//D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI * 0.5f, 1.0f, 1.f, DxFogMan::GetInstance().GetFogEnd_RealTime() );
		// CubeMap 만드는 높이도(1.5m) 생각해서 10.f(1m) 정도 안에 들어오는것은 안 보이도록 한다.
		D3DXMatrixPerspectiveFovLH( &matProj, fNewFOV, 1.0f, 10.f, DxFogMan::GetInstance().GetFogEnd_RealTime() );

		pd3dDevice->GetTransform ( D3DTS_VIEW, &matOldView );
		pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matOldProj );

		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );

		LPDIRECT3DSURFACEQ	pSurfaceSeamless[6];
		LPDIRECT3DTEXTUREQ	pTextureSeamless[6];
		for( UINT i = 0; i < 6; i++ )
		{
			HRESULT hr = pd3dDevice->CreateTexture ( dwSize, dwSize, 1, 0L, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTextureSeamless[i], NULL );
			if( FAILED(hr) )
				return;

			hr = pTextureSeamless[i]->GetSurfaceLevel( 0, &pSurfaceSeamless[i] );
			if( FAILED(hr) )
				return;
		}

		LPDIRECT3DSURFACEQ pSurface;

		for( UINT i = 0; i < 6; i++ )
		{
			pCubeTex->GetCubeMapSurface ( (D3DCUBEMAP_FACES)i, 0, &pSurface );

			HRESULT hr(S_OK);
			hr = pd3dDevice->SetRenderTarget( 0, pSurface );
			hr = pd3dDevice->SetDepthStencilSurface( pZBuffer );

			// Clear the viewport
			D3DCOLOR colorClear = DxFogMan::GetInstance().GetFogColor();
			hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, colorClear, 1.0f, 0L );

			//{
			//	hr = pd3dDevice->SetRenderTarget( 0, pSurfaceBackUp );
			//	HRESULT hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, colorClear, 1.0f, 0L );
			//}

			D3DXVECTOR3 vLookatPt;
			D3DXVECTOR3 vUpVec;
			matView = D3DUtil_GetCubeMapViewMatrix_New( (D3DCUBEMAP_FACES) i, vLookatPt, vUpVec );					// View 가져오기

			vLookatPt += vPos;
			D3DXVECTOR3 vFromPt = vPos;

			D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
			pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );

			// Note : 클리핑 정보를 만든다.
			CLIPVOLUME sCV = DxViewPort::GetInstance().ComputeClipVolume
			( 
				vLookatPt, 
				vFromPt, 
				vUpVec, 
				DxFogMan::GetInstance().GetFogStart_RealTime(), 
				DxFogMan::GetInstance().GetFogEnd_RealTime(), 
				1.f, 
				1.f, 
				fNewFOV	//D3DX_PI * 0.5f 
			);

			// CV Thread 작업이 시작되겠음. 
			{
				NSLandThread::Wait_FrameMoveRAD( pLandMan->GetArrayEntityRenderListSwap(), sCV );

				// 완료될 때까지 대기한다.
				while ( !NSLandThread::IsCompleteData() )
				{
				}
				NSLandThread::Swap();
			}

			// 화면 사이즈를 설정해줌.
			NSMaterialManager::SetWindowSize( dwSize, dwSize );

			// 렌더링 위치.
			pd3dDevice->BeginScene();
			{
				NSMaterialManager::FrameMove( matView, matProj, pTextureBackUp_1st, pTextureBackUp_2nd, pTextureBackUp_3rd, NULL );

				// Sort 에 알려줘서 LOD 기능이 동작 못하도록 한다.
				BOOL bViewAlphaLOD_BackUp = NSPieceQuickSort::g_bViewAlphaLOD;
				NSPieceQuickSort::g_bViewAlphaLOD = FALSE;

				// 하늘 먼저 렌더링 -------------- 1
				pd3dDevice->SetRenderTarget( 0, pSurface );

				DxSkyBoxMan::GetInstance().Render( pd3dDevice );
				DxSkyMan::GetInstance().Render( pd3dDevice, fNewFOV, 1.f );

				//// 지형 렌더링 ------------------- 2
				//pd3dDevice->SetRenderTarget( 0, pSurfaceBackUp );

				D3DXMATRIX matIdentity;
				D3DXMatrixIdentity( &matIdentity );
				pLandMan->Render_Radiosity
				(
					pd3dDevice, 
					sCV,
					matView,
					matProj,
					TRUE,
					colorClear,
					dwSize,
					dwSize,
					pSurface,
					pSurfaceBackUp_1st,
					pSurfaceBackUp_2nd,
					pSurfaceBackUp_3rd,
					pTextureBackUp_1st,
					pTextureBackUp_2nd,
					pTextureBackUp_3rd,
					FALSE,
					0.f,
					matIdentity,
					TRUE,
					NSLandThread::TYPE_BASEMESH_CULL
				);

				NSPieceQuickSort::g_bViewAlphaLOD = bViewAlphaLOD_BackUp;
			}
			pd3dDevice->EndScene();

			// 화면 사이즈를 설정해줌.
			NSMaterialManager::SetWindowSize( DxSurfaceTex::GetInstance().GetWidth(), DxSurfaceTex::GetInstance().GetHeight() );

			// 복제
			D3DXLoadSurfaceFromSurface ( pSurfaceSeamless[i], NULL, NULL, pSurface, NULL, NULL, D3DX_FILTER_POINT, 0L );

			SAFE_RELEASE ( pSurface );
		}

		// seamless 하게 만들자.
		{
			SeamlessLine( pTextureSeamless[4], pTextureSeamless[3], dwSize, 0, TRUE, TRUE );
			SeamlessLine( pTextureSeamless[3], pTextureSeamless[5], dwSize, 0, TRUE, FALSE );
			SeamlessLine( pTextureSeamless[5], pTextureSeamless[2], dwSize, 0, FALSE, TRUE );
			SeamlessLine( pTextureSeamless[2], pTextureSeamless[4], dwSize, 0, TRUE, TRUE );

			SeamlessLine( pTextureSeamless[1], pTextureSeamless[4], dwSize, 1, TRUE, TRUE );
			SeamlessLine( pTextureSeamless[4], pTextureSeamless[0], dwSize, 1, TRUE, TRUE );
			SeamlessLine( pTextureSeamless[0], pTextureSeamless[5], dwSize, 1, TRUE, TRUE );
			SeamlessLine( pTextureSeamless[5], pTextureSeamless[1], dwSize, 1, TRUE, TRUE );

			SeamlessLine( pTextureSeamless[0], pTextureSeamless[3], dwSize, 2, TRUE, TRUE );
			SeamlessLine( pTextureSeamless[1], pTextureSeamless[3], dwSize, 3, TRUE, TRUE );
			SeamlessLine( pTextureSeamless[0], pTextureSeamless[2], dwSize, 4, TRUE, TRUE );
			SeamlessLine( pTextureSeamless[1], pTextureSeamless[2], dwSize, 5, TRUE, TRUE );
		}

		// 복제
		for( UINT i = 0; i < 6; i++ )
		{
			pCubeTex->GetCubeMapSurface ( (D3DCUBEMAP_FACES)i, 0, &pSurface );
			D3DXLoadSurfaceFromSurface ( pSurface, NULL, NULL, pSurfaceSeamless[i], NULL, NULL, D3DX_FILTER_POINT, 0L );
			SAFE_RELEASE ( pSurface );
		}

		// 정리
		for( UINT i = 0; i < 6; i++ )
		{
			SAFE_RELEASE ( pTextureSeamless[i] );
			SAFE_RELEASE ( pSurfaceSeamless[i] );
		}

		sBackupTarget.Restore();

		pd3dDevice->SetTransform ( D3DTS_VIEW, &matOldView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matOldProj );

		NSPieceQuickSort::g_bPieceInstancing = bPieceInstancing;
	}

	void CreateLocalCubeMapOutputFile( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, DWORD dwSize, const D3DXVECTOR3& vPos, DxLandMan* pLandMan )
	{
		LPDIRECT3DCUBETEXTUREQ	pCubeTex(NULL);
		LPDIRECT3DSURFACEQ		pZBuffer(NULL);

		//	Note : 큐브 텍스쳐 만들기
		D3DFORMAT colorFormat = D3DFMT_A8R8G8B8;
		HRESULT hr = pd3dDevice->CreateCubeTexture ( dwSize, 1, D3DUSAGE_RENDERTARGET,			// 4. A8R8G8B8
			colorFormat, D3DPOOL_DEFAULT, &pCubeTex, NULL );
		if( FAILED(hr) )
		{
			return;
		}

		LPDIRECT3DSURFACEQ	pSurfaceBackUp_1st(NULL);
		LPDIRECT3DTEXTUREQ	pTextureBackUp_1st(NULL);
		hr = pd3dDevice->CreateTexture ( dwSize, dwSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, D3DPOOL_DEFAULT, &pTextureBackUp_1st, NULL );
		if( FAILED(hr) )
			return;

		hr = pTextureBackUp_1st->GetSurfaceLevel( 0, &pSurfaceBackUp_1st );
		if( FAILED(hr) )
			return;


		LPDIRECT3DSURFACEQ	pSurfaceBackUp_2nd(NULL);
		LPDIRECT3DTEXTUREQ	pTextureBackUp_2nd(NULL);
		hr = pd3dDevice->CreateTexture ( dwSize, dwSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, D3DPOOL_DEFAULT, &pTextureBackUp_2nd, NULL );
		if( FAILED(hr) )
			return;

		hr = pTextureBackUp_2nd->GetSurfaceLevel( 0, &pSurfaceBackUp_2nd );
		if( FAILED(hr) )
			return;


		LPDIRECT3DSURFACEQ	pSurfaceBackUp_3rd(NULL);
		LPDIRECT3DTEXTUREQ	pTextureBackUp_3rd(NULL);
		hr = pd3dDevice->CreateTexture ( dwSize, dwSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &pTextureBackUp_3rd, NULL );
		if( FAILED(hr) )
			return;

		hr = pTextureBackUp_3rd->GetSurfaceLevel( 0, &pSurfaceBackUp_3rd );
		if( FAILED(hr) )
			return;


		D3DFORMAT cZFormat(D3DFMT_D16);
		hr = pd3dDevice->CreateDepthStencilSurface( dwSize, dwSize, cZFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &pZBuffer, NULL );
		if( FAILED(hr) )
		{
			return;
		}

		// CubeMap 을 생성한다.
		CreateLocalCubeMapDetail
		( 
			pd3dDevice, 
			dwSize, 
			vPos, 
			pLandMan, 
			pCubeTex, 
			pSurfaceBackUp_1st, 
			pSurfaceBackUp_2nd,
			pSurfaceBackUp_3rd, 
			pTextureBackUp_1st, 
			pTextureBackUp_2nd, 
			pTextureBackUp_3rd, 
			pZBuffer 
		);

		// 파일로 저장한다.
		{
			// Note : 텍스쳐를 압축한다.
			LPDIRECT3DBASETEXTURE9 pTexRGB(NULL);
			NSTexCompress::Compress
			(
				pd3dDevice,
				dwSize,
				dwSize,
				0,
				1,
				D3DFMT_X8R8G8B8,
				pCubeTex,
				pTexRGB,
				0,
				TRUE
			);

			// 이런식으로 변환 후 변환 해야 이미지가 덜 손상된다.
			LPDIRECT3DBASETEXTURE9 pTexNew(NULL);
			NSTexCompress::Compress
			(
				pd3dDevice,
				dwSize,
				dwSize,
				0,
				1,
				D3DFMT_DXT1,
				pTexRGB,
				pTexNew,
				0,
				TRUE
			);

			// Note : 버퍼에 그려진 것을 텍스쳐로 뽑는다.
			TSTRING strTexture = strName;
			D3DXSaveTextureToFile( strTexture.c_str(), D3DXIFF_DDS, pTexNew, NULL );

			SAFE_RELEASE( pTexRGB );
			SAFE_RELEASE( pTexNew );
		}

		SAFE_RELEASE ( pSurfaceBackUp_1st );
		SAFE_RELEASE ( pTextureBackUp_1st );
		SAFE_RELEASE ( pSurfaceBackUp_2nd );
		SAFE_RELEASE ( pTextureBackUp_2nd );
		SAFE_RELEASE ( pSurfaceBackUp_3rd );
		SAFE_RELEASE ( pTextureBackUp_3rd );
		SAFE_RELEASE ( pCubeTex );
		SAFE_RELEASE ( pZBuffer );
	}

	void CreateLocalCubeMapForGlobal( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwSize, const D3DXVECTOR3& vPos, DxLandMan* pLandMan, LPDIRECT3DCUBETEXTUREQ& pCubeTexOUT )
	{
		// 크게만들어서 줄일 필요가 없다. 작은사이즈로 해도 되겠다.
		//DWORD dwRenderSize(256);
		//if ( dwSize > dwRenderSize )
		//	dwRenderSize = dwSize;

		DWORD dwRenderSize = dwSize;

		LPDIRECT3DCUBETEXTUREQ	pCubeTex(NULL);
		LPDIRECT3DSURFACEQ		pZBuffer(NULL);

		//	Note : 큐브 텍스쳐 만들기
		D3DFORMAT colorFormat = D3DFMT_A8R8G8B8;
		HRESULT hr = pd3dDevice->CreateCubeTexture ( dwRenderSize, 1, D3DUSAGE_RENDERTARGET,			// 4. A8R8G8B8
														colorFormat, D3DPOOL_DEFAULT, &pCubeTex, NULL );
		if( FAILED(hr) )
		{
			return;
		}


		LPDIRECT3DSURFACEQ	pSurfaceBackUp_1st(NULL);
		LPDIRECT3DTEXTUREQ	pTextureBackUp_1st(NULL);
		hr = pd3dDevice->CreateTexture ( dwRenderSize, dwRenderSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, D3DPOOL_DEFAULT, &pTextureBackUp_1st, NULL );
		if( FAILED(hr) )
			return;

		hr = pTextureBackUp_1st->GetSurfaceLevel( 0, &pSurfaceBackUp_1st );
		if( FAILED(hr) )
			return;


		LPDIRECT3DSURFACEQ	pSurfaceBackUp_2nd(NULL);
		LPDIRECT3DTEXTUREQ	pTextureBackUp_2nd(NULL);
		hr = pd3dDevice->CreateTexture ( dwRenderSize, dwRenderSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, D3DPOOL_DEFAULT, &pTextureBackUp_2nd, NULL );
		if( FAILED(hr) )
			return;

		hr = pTextureBackUp_2nd->GetSurfaceLevel( 0, &pSurfaceBackUp_2nd );
		if( FAILED(hr) )
			return;


		LPDIRECT3DSURFACEQ	pSurfaceBackUp_3rd(NULL);
		LPDIRECT3DTEXTUREQ	pTextureBackUp_3rd(NULL);
		hr = pd3dDevice->CreateTexture ( dwRenderSize, dwRenderSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &pTextureBackUp_3rd, NULL );
		if( FAILED(hr) )
			return;

		hr = pTextureBackUp_3rd->GetSurfaceLevel( 0, &pSurfaceBackUp_3rd );
		if( FAILED(hr) )
			return;


		D3DFORMAT cZFormat(D3DFMT_D16);
		hr = pd3dDevice->CreateDepthStencilSurface( dwRenderSize, dwRenderSize, cZFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &pZBuffer, NULL );
		if( FAILED(hr) )
		{
			return;
		}

		// CubeMap 을 생성한다.
		CreateLocalCubeMapDetail
		( 
			pd3dDevice, 
			dwRenderSize, 
			vPos, 
			pLandMan, 
			pCubeTex, 
			pSurfaceBackUp_1st, 
			pSurfaceBackUp_2nd,
			pSurfaceBackUp_3rd, 
			pTextureBackUp_1st, 
			pTextureBackUp_2nd, 
			pTextureBackUp_3rd, 
			pZBuffer 
		);



		// 파일로 저장한다.
		{
			// Note : 텍스쳐를 압축한다.
			LPDIRECT3DBASETEXTURE9 pTexRGB(NULL);
			NSTexCompress::Compress
			(
				pd3dDevice,
				dwSize,
				dwSize,
				0,
				1,
				D3DFMT_X8R8G8B8,
				pCubeTex,
				pTexRGB,
				0,
				TRUE
			);

			// 이런식으로 변환 후 변환 해야 이미지가 덜 손상된다.
			LPDIRECT3DBASETEXTURE9 pTexNew(NULL);
			NSTexCompress::Compress
			(
				pd3dDevice,
				dwSize,
				dwSize,
				0,
				1,
				D3DFMT_DXT1,
				pTexRGB,
				pTexNew,
				0,
				TRUE
			);

			SAFE_RELEASE( pTexRGB );

			SAFE_RELEASE( pCubeTexOUT );
			pCubeTexOUT = (LPDIRECT3DCUBETEXTURE9)pTexNew;
			if ( !pCubeTexOUT )
			{
				SAFE_RELEASE( pTexNew );
			}
		}

		SAFE_RELEASE ( pSurfaceBackUp_1st );
		SAFE_RELEASE ( pTextureBackUp_1st );
		SAFE_RELEASE ( pSurfaceBackUp_2nd );
		SAFE_RELEASE ( pTextureBackUp_2nd );
		SAFE_RELEASE ( pSurfaceBackUp_3rd );
		SAFE_RELEASE ( pTextureBackUp_3rd );
		SAFE_RELEASE ( pCubeTex );
		SAFE_RELEASE ( pZBuffer );
	}


	// ---------------------------------------------------- 임시 CubeMap
	//
	TSTRING			g_strTempCubeMapName;
	TextureResource g_textureResTempCubeMap;

	void SetTempCubeMapName( const TSTRING& strName )
	{
		g_strTempCubeMapName = strName;

		if ( !g_strTempCubeMapName.empty() )
		{
			g_textureResTempCubeMap = TextureManager::GetInstance().LoadTexture( g_strTempCubeMapName,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE,
				1,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_ETC,
				false,
				FALSE );
		}
	}

	LPDIRECT3DCUBETEXTUREQ GetTempCubeMapTexture()
	{
		return g_textureResTempCubeMap.GetCubeTexture();
	}

	//
	// ---------------------------------------------------- 임시 CubeMap

	// NSCubeMapBlend, NSCubeMapForPSF 공용으로 필요한 데이터
	struct VERTEXRHW_CUBE
	{
		D3DXVECTOR4 vPos;
		D3DXVECTOR3 vTex;
		static const DWORD FVF = D3DFVF_XYZRHW|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0);
	};
	VERTEXRHW				g_sVertBase[4];

	void CrateCube( VERTEXRHW_CUBE* pVertCube, DWORD dwSize )
	{
		float fMicroOffset = -0.5f;

		for ( DWORD i=0; i<6; ++i )
		{
			pVertCube[i*4+0].vPos = D3DXVECTOR4 ( fMicroOffset,					fMicroOffset,				1.f, 1.f );
			pVertCube[i*4+1].vPos = D3DXVECTOR4 ( (float)dwSize+fMicroOffset,	fMicroOffset,				1.f, 1.f );
			pVertCube[i*4+2].vPos = D3DXVECTOR4 ( fMicroOffset,					(float)dwSize+fMicroOffset,	1.f, 1.f );
			pVertCube[i*4+3].vPos = D3DXVECTOR4 ( (float)dwSize+fMicroOffset,	(float)dwSize+fMicroOffset,	1.f, 1.f );
		}

		// x+
		pVertCube[0*4+0].vTex = D3DXVECTOR3 ( 0.5f, 0.5f, 0.5f );
		pVertCube[0*4+1].vTex = D3DXVECTOR3 ( 0.5f, 0.5f, -0.5f );
		pVertCube[0*4+2].vTex = D3DXVECTOR3 ( 0.5f, -0.5f, 0.5f );
		pVertCube[0*4+3].vTex = D3DXVECTOR3 ( 0.5f, -0.5f, -0.5f );

		// x-
		pVertCube[1*4+0].vTex = D3DXVECTOR3 ( -0.5f, 0.5f, -0.5f );
		pVertCube[1*4+1].vTex = D3DXVECTOR3 ( -0.5f, 0.5f, 0.5f );
		pVertCube[1*4+2].vTex = D3DXVECTOR3 ( -0.5f, -0.5f, -0.5f );
		pVertCube[1*4+3].vTex = D3DXVECTOR3 ( -0.5f, -0.5f, 0.5f );

		// y+
		pVertCube[2*4+0].vTex = D3DXVECTOR3 ( -0.5f, 0.5f, -0.5f );
		pVertCube[2*4+1].vTex = D3DXVECTOR3 ( 0.5f, 0.5f, -0.5f );
		pVertCube[2*4+2].vTex = D3DXVECTOR3 ( -0.5f, 0.5f, 0.5f );
		pVertCube[2*4+3].vTex = D3DXVECTOR3 ( 0.5f, 0.5f, 0.5f );

		// y-
		pVertCube[3*4+0].vTex = D3DXVECTOR3 ( -0.5f, -0.5f, 0.5f );
		pVertCube[3*4+1].vTex = D3DXVECTOR3 ( 0.5f, -0.5f, 0.5f );
		pVertCube[3*4+2].vTex = D3DXVECTOR3 ( -0.5f, -0.5f, -0.5f );
		pVertCube[3*4+3].vTex = D3DXVECTOR3 ( 0.5f, -0.5f, -0.5f );

		// z+
		pVertCube[4*4+0].vTex = D3DXVECTOR3 ( -0.5f, 0.5f, 0.5f );
		pVertCube[4*4+1].vTex = D3DXVECTOR3 ( 0.5f, 0.5f, 0.5f );
		pVertCube[4*4+2].vTex = D3DXVECTOR3 ( -0.5f, -0.5f, 0.5f );
		pVertCube[4*4+3].vTex = D3DXVECTOR3 ( 0.5f, -0.5f, 0.5f );

		// z-
		pVertCube[5*4+0].vTex = D3DXVECTOR3 ( 0.5f, 0.5f, -0.5f );
		pVertCube[5*4+1].vTex = D3DXVECTOR3 ( -0.5f, 0.5f, -0.5f );
		pVertCube[5*4+2].vTex = D3DXVECTOR3 ( 0.5f, -0.5f, -0.5f );
		pVertCube[5*4+3].vTex = D3DXVECTOR3 ( -0.5f, -0.5f, -0.5f );
	}

	// CubeMap을 Blend 시켜준다.
	namespace NSCubeMapBlend
	{
		int						g_nCubeMapIndex(0);	// 현재 변경되고 있는 CubeMapIndex
		float					g_fAddTime(0.f);	// 텍스쳐가 변한 후 지난 시간
		float					g_fMaxTime(1.f);	// 텍스쳐가 변하는 시간
		LPDIRECT3DCUBETEXTUREQ	g_rThisTex(NULL);	// 현재 텍스쳐
		LPDIRECT3DCUBETEXTUREQ	g_rPrevTex(NULL);	// 이전 텍스쳐를 기록해서 텍스쳐가 변하는지 찾는다.
		LPDIRECT3DCUBETEXTUREQ	g_pCubeTex(NULL);
		LPDIRECT3DSTATEBLOCK9	g_pSavedSB(NULL);
		LPDIRECT3DSTATEBLOCK9	g_pDrawSB(NULL);
		LPDIRECT3DSTATEBLOCK9	g_pSavedEditSB(NULL);
		LPDIRECT3DSTATEBLOCK9	g_pDrawEditSB(NULL);
		DWORD					g_dwTextureSize(32);
		VERTEXRHW_CUBE			g_sVertCube[6][4];

		void CleanUp()
		{
			g_rThisTex = g_rPrevTex = NULL;
		}

		void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
		{
			for( UINT which=0; which<2; which++ )
			{
				pd3dDevice->BeginStateBlock();

				// Note : SetRenderState()
				pd3dDevice->SetRenderState( D3DRS_LIGHTING,		    FALSE );
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    FALSE );
				pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    FALSE );
				pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

				// Note : SetTextureStageState()
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TFACTOR );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT3 );

				// 이곳에서는 LINEAR 쓰면 경계가 심하게 보인다. 
				// 특수한 곳. Cube 맵의 각 Surface 를 복사하는 곳이다.
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

				if( which==0 )	pd3dDevice->EndStateBlock ( &g_pSavedSB );
				else			pd3dDevice->EndStateBlock ( &g_pDrawSB );
			}

			for( UINT which=0; which<2; which++ )
			{
				pd3dDevice->BeginStateBlock();

				// Note : SetRenderState()
				pd3dDevice->SetRenderState( D3DRS_LIGHTING,		    FALSE );
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    FALSE );
				pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    FALSE );
				pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );

				// Note : SetTextureStageState()
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

				// 이곳에서는 LINEAR 쓰면 경계가 심하게 보인다. 
				// 특수한 곳. Cube 맵의 각 Surface 를 복사하는 곳이다.
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

				if( which==0 )	pd3dDevice->EndStateBlock ( &g_pSavedEditSB );
				else			pd3dDevice->EndStateBlock ( &g_pDrawEditSB );
			}

			OnNewSizeDevice( pd3dDevice, g_dwTextureSize );
		}

		void OnNewSizeDevice( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwSize )
		{
			HRESULT hr(S_OK);
			D3DFORMAT	colorFormat = D3DFMT_X1R5G5B5;
			D3DFORMAT	zFormat		= D3DFMT_D16;

			// 사이즈를 셋팅해놓는다.
			g_dwTextureSize = dwSize;

			//	Note : 큐브 텍스쳐 만들기
			//
			SAFE_RELEASE( g_pCubeTex );
			if( FAILED ( pd3dDevice->CreateCubeTexture ( dwSize, 1, D3DUSAGE_RENDERTARGET,				// 1. X1R5G5B5
				colorFormat, D3DPOOL_DEFAULT, &g_pCubeTex, NULL ) ) )
			{
				colorFormat = D3DFMT_A1R5G5B5;
				if( FAILED ( pd3dDevice->CreateCubeTexture ( dwSize, 1, D3DUSAGE_RENDERTARGET,			// 2. A1R5G5B5
					colorFormat, D3DPOOL_DEFAULT, &g_pCubeTex, NULL ) ) )	
				{
					colorFormat = D3DFMT_A4R4G4B4;
					if( FAILED ( pd3dDevice->CreateCubeTexture ( dwSize, 1, D3DUSAGE_RENDERTARGET,		// 3. A4R4G4B4
						colorFormat, D3DPOOL_DEFAULT, &g_pCubeTex, NULL ) ) )
					{
						colorFormat = D3DFMT_A8R8G8B8;
						hr = pd3dDevice->CreateCubeTexture ( dwSize, 1, D3DUSAGE_RENDERTARGET,			// 4. A8R8G8B8
							colorFormat, D3DPOOL_DEFAULT, &g_pCubeTex, NULL );
						if( FAILED(hr) )
						{
							//					GASSERT ( 0 && "<DxCubeMap> 텍스쳐 포맷 설정 불가능함" );
							return;
						}
					}
				}
			}

			CrateCube( &g_sVertCube[0][0], dwSize );

			float fMicroOffset = 0.5f/(float)dwSize;

			// Test 용
			g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,			0.f,			1.f, 1.f );
			g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)dwSize,	0.f,			1.f, 1.f );
			g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,			(float)dwSize,	1.f, 1.f );
			g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)dwSize,	(float)dwSize,	1.f, 1.f );

			// Test 용
			g_sVertBase[0].vTex = D3DXVECTOR2 ( 0.f+fMicroOffset, 0.f+fMicroOffset );
			g_sVertBase[1].vTex = D3DXVECTOR2 ( 1.f+fMicroOffset, 0.f+fMicroOffset );
			g_sVertBase[2].vTex = D3DXVECTOR2 ( 0.f+fMicroOffset, 1.f+fMicroOffset );
			g_sVertBase[3].vTex = D3DXVECTOR2 ( 1.f+fMicroOffset, 1.f+fMicroOffset );
		}

		void OnLostDevice()
		{
			SAFE_RELEASE ( g_pSavedSB );
			SAFE_RELEASE ( g_pDrawSB );
			SAFE_RELEASE ( g_pSavedEditSB );
			SAFE_RELEASE ( g_pDrawEditSB );

			SAFE_RELEASE ( g_pCubeTex );

			g_rThisTex = g_rPrevTex = NULL;
		}

		void OnFrameMove( float fElapsedTime )
		{
			g_fAddTime += fElapsedTime;
		}

		void OnRender( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DCUBETEXTUREQ pSrcCubeMap )
		{
			if ( !g_pCubeTex )
				return;

			if ( !pSrcCubeMap )
				return;

			// 처음 들어오는 거라면 공통으로 셋팅한다.
			if ( !g_rPrevTex )
			{
				g_rPrevTex = g_rThisTex = pSrcCubeMap;
			}

			float fRate = g_fAddTime / g_fMaxTime;

			DWORD dwAlpha = static_cast<DWORD>( 255 * fRate );
			if ( dwAlpha > 255 )
				dwAlpha = 255;

			DWORD dwColor = (dwAlpha<<24) + 0xffffff;
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwColor );

			DxBackupTarget sBackupTarget;
			sBackupTarget.Backup( pd3dDevice );

			LPDIRECT3DSURFACEQ pSurface;

			g_pSavedSB->Capture();
			g_pDrawSB->Apply();

			//for( UINT i = 0; i < 6; i++ )
			UINT i = g_nCubeMapIndex;
			{
				g_pCubeTex->GetCubeMapSurface ( (D3DCUBEMAP_FACES)i, 0, &pSurface );

				HRESULT hr(S_OK);
				hr = pd3dDevice->SetRenderTarget( 0, pSurface );

				// 이미지를 블렌딩해야한다.

				// 1
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );

				pd3dDevice->SetFVF( VERTEXRHW_CUBE::FVF );
				pd3dDevice->SetTexture( 0, g_rPrevTex );
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertCube[i], sizeof(VERTEXRHW_CUBE) );

				// 2
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

				pd3dDevice->SetTexture( 0, g_rThisTex );
				pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertCube[i], sizeof(VERTEXRHW_CUBE) );

				SAFE_RELEASE ( pSurface );
			}

			g_pSavedSB->Apply();

			sBackupTarget.Restore();

			// 돌아갈 큐브맵
			++g_nCubeMapIndex;
			if ( g_nCubeMapIndex >= 6 )
				g_nCubeMapIndex = 0;

			// 텍스쳐를 변경할 수 있는 상황이 되었다.
			if ( dwAlpha == 255 )
			{
				// 텍스쳐가 다르다면,
				if ( g_rThisTex != pSrcCubeMap )
				{
					g_fAddTime = 0.f;
					g_rPrevTex = g_rThisTex;
					g_rThisTex = pSrcCubeMap;
				}
			}
		}

		void OnRenderForPieceEditor( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture )
		{
			if ( !g_pCubeTex )
				return;

			if ( !pTexture )
				return;

			DxBackupTarget sBackupTarget;
			sBackupTarget.Backup( pd3dDevice );

			LPDIRECT3DSURFACEQ pSurface;

			g_pSavedEditSB->Capture();
			g_pDrawEditSB->Apply();

			UINT i = g_nCubeMapIndex;
			{
				g_pCubeTex->GetCubeMapSurface ( (D3DCUBEMAP_FACES)i, 0, &pSurface );

				HRESULT hr(S_OK);
				hr = pd3dDevice->SetRenderTarget( 0, pSurface );

				if ( g_textureResTempCubeMap.GetTexture() )
				{
					pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT3 );

					pd3dDevice->SetFVF( VERTEXRHW_CUBE::FVF );
					pd3dDevice->SetTexture( 0, g_textureResTempCubeMap.GetTexture() );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertCube[i], sizeof(VERTEXRHW_CUBE) );
				}
				else
				{
					pd3dDevice->SetFVF( VERTEXRHW::FVF );
					pd3dDevice->SetTexture( 0, pTexture );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );
				}

				SAFE_RELEASE ( pSurface );
			}

			g_pSavedEditSB->Apply();

			sBackupTarget.Restore();

			// 돌아갈 큐브맵
			++g_nCubeMapIndex;
			if ( g_nCubeMapIndex >= 6 )
				g_nCubeMapIndex = 0;
		}

		LPDIRECT3DCUBETEXTUREQ GetCubeMapTexture()
		{
			return g_pCubeTex;
		}
	};

	// RealTime 으로 CubeMap 을 생성한다. PSF 모드에서 필요함.
	// WorldEdit 에서만 쓰임.
	namespace NSCubeMapForPSF
	{
		BOOL					g_bMyCubeMapRender(FALSE);
		DWORD					g_dwCubeMapSize(64);
		int						g_nCubeMapIndex(0);	// 현재 변경되고 있는 CubeMapIndex
		LPDIRECT3DCUBETEXTUREQ	g_pCubeTex(NULL);
		LPDIRECT3DSTATEBLOCK9	g_pSavedEditSB(NULL);
		LPDIRECT3DSTATEBLOCK9	g_pDrawEditSB(NULL);
		VERTEXRHW_CUBE			g_sVertCube[6][4];

		LPDIRECT3DTEXTUREQ		g_pColorTex_1st(NULL);		// Deffered - Buffer Size	기본 렌더링을 위한 것.
		LPDIRECT3DSURFACEQ		g_pColorSuf_1st(NULL);		// Deffered
		LPDIRECT3DTEXTUREQ		g_pColorTex_2nd(NULL);		// Deffered - Buffer Size	deffered 용 Normal 값을 기록하기 위해 존재한다.
		LPDIRECT3DSURFACEQ		g_pColorSuf_2nd(NULL);		// Deffered
		LPDIRECT3DTEXTUREQ		g_pColorTex_3rd(NULL);		// Deffered - Buffer Size	deffered 용 SpecularFactor 값을 기록하기 위해 존재한다.
		LPDIRECT3DSURFACEQ		g_pColorSuf_3rd(NULL);		// Deffered
		LPDIRECT3DTEXTUREQ		g_pColorTex_4th(NULL);		// Deffered - Buffer Size	deffered 용 
		LPDIRECT3DSURFACEQ		g_pColorSuf_4th(NULL);		// Deffered
		LPDIRECT3DTEXTUREQ		g_pTexFourCC_INTZ(NULL);		// Deffered - Buffer Size	deffered 용 Depth 값을 기록하기 위해 존재한다.
		LPDIRECT3DSURFACEQ		g_pSufFourCC_INTZ(NULL);		// Deffered
		LPDIRECT3DTEXTUREQ		g_pColorTex_LBuffer(NULL);	// Deffered - Buffer Size	Light Accumulation Buffer
		LPDIRECT3DSURFACEQ		g_pColorSuf_LBuffer(NULL);	// Deffered
		

		void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
		{
			// WorldEdit 일 경우만 동작된다.
			if ( !g_bWORLD_TOOL )
				return;

			for( UINT which=0; which<2; which++ )
			{
				pd3dDevice->BeginStateBlock();

				// Note : SetRenderState()
				pd3dDevice->SetRenderState( D3DRS_LIGHTING,		    FALSE );
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    FALSE );
				pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    FALSE );
				pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );

				// Note : SetTextureStageState()
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

				// 이곳에서는 LINEAR 쓰면 경계가 심하게 보인다. 
				// 특수한 곳. Cube 맵의 각 Surface 를 복사하는 곳이다.
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

				if( which==0 )	pd3dDevice->EndStateBlock ( &g_pSavedEditSB );
				else			pd3dDevice->EndStateBlock ( &g_pDrawEditSB );
			}

			//	Note : 큐브 텍스쳐 만들기
			D3DFORMAT colorFormat = D3DFMT_A8R8G8B8;
			HRESULT hr = pd3dDevice->CreateCubeTexture ( g_dwCubeMapSize, 1, D3DUSAGE_RENDERTARGET,			// 4. A8R8G8B8
															colorFormat, D3DPOOL_DEFAULT, &g_pCubeTex, NULL );
			if( FAILED(hr) )
			{
				return;
			}

			hr = pd3dDevice->CreateTexture( g_dwCubeMapSize, g_dwCubeMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pColorTex_1st, NULL );
			if( FAILED(hr) )
				return;
			hr = g_pColorTex_1st->GetSurfaceLevel( 0, &g_pColorSuf_1st );
			if( FAILED(hr) )
				return;
			pd3dDevice->SetRenderTarget( 0, g_pColorSuf_1st );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );


			hr = pd3dDevice->CreateTexture( g_dwCubeMapSize, g_dwCubeMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pColorTex_2nd, NULL );
			if( FAILED(hr) )
				return;
			hr = g_pColorTex_2nd->GetSurfaceLevel( 0, &g_pColorSuf_2nd );
			if( FAILED(hr) )
				return;
			pd3dDevice->SetRenderTarget( 0, g_pColorSuf_2nd );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );


			hr = pd3dDevice->CreateTexture( g_dwCubeMapSize, g_dwCubeMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pColorTex_3rd, NULL );
			if( FAILED(hr) )
				return;
			hr = g_pColorTex_3rd->GetSurfaceLevel( 0, &g_pColorSuf_3rd );
			if( FAILED(hr) )
				return;
			pd3dDevice->SetRenderTarget( 0, g_pColorSuf_3rd );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );


			hr = pd3dDevice->CreateTexture( g_dwCubeMapSize, g_dwCubeMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pColorTex_4th, NULL );
			if( FAILED(hr) )
				return;
			hr = g_pColorTex_4th->GetSurfaceLevel( 0, &g_pColorSuf_4th );
			if( FAILED(hr) )
				return;
			pd3dDevice->SetRenderTarget( 0, g_pColorSuf_4th );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );


			D3DFORMAT m_FormatINTZ = (D3DFORMAT)MAKEFOURCC('I','N','T','Z');
			hr = pd3dDevice->CreateTexture( g_dwCubeMapSize, g_dwCubeMapSize, 1, D3DUSAGE_DEPTHSTENCIL, m_FormatINTZ, D3DPOOL_DEFAULT, &g_pTexFourCC_INTZ, NULL );
			if( FAILED(hr) )
				return;
			hr = g_pTexFourCC_INTZ->GetSurfaceLevel( 0, &g_pSufFourCC_INTZ );
			if( FAILED(hr) )
				return;


			hr = pd3dDevice->CreateTexture( g_dwCubeMapSize, g_dwCubeMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pColorTex_LBuffer, NULL );
			if( FAILED(hr) )
				return;
			hr = g_pColorTex_LBuffer->GetSurfaceLevel( 0, &g_pColorSuf_LBuffer );
			if( FAILED(hr) )
				return;
			pd3dDevice->SetRenderTarget( 0, g_pColorSuf_LBuffer );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );


			CrateCube( &g_sVertCube[0][0], g_dwCubeMapSize );
		}

		void OnLostDevice()
		{
			SAFE_RELEASE ( g_pSavedEditSB );
			SAFE_RELEASE ( g_pDrawEditSB );

			SAFE_RELEASE ( g_pCubeTex );

			SAFE_RELEASE ( g_pColorTex_1st );
			SAFE_RELEASE ( g_pColorSuf_1st );
			SAFE_RELEASE ( g_pColorTex_2nd );
			SAFE_RELEASE ( g_pColorSuf_2nd );
			SAFE_RELEASE ( g_pColorTex_3rd );
			SAFE_RELEASE ( g_pColorSuf_3rd );
			SAFE_RELEASE ( g_pColorTex_4th );
			SAFE_RELEASE ( g_pColorSuf_4th );
			SAFE_RELEASE ( g_pTexFourCC_INTZ );
			SAFE_RELEASE ( g_pSufFourCC_INTZ );
			SAFE_RELEASE ( g_pColorTex_LBuffer );
			SAFE_RELEASE ( g_pColorSuf_LBuffer );
		}

		void OnFrameMove( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos, DxSetLandMan* pSetLandMan )
		{
			if ( !g_pCubeTex )
				return;

			g_bMyCubeMapRender = TRUE;

			{
				D3DXMATRIX	matOldView, matView;
				D3DXMATRIX	matOldProj, matProj;

				// 약간 넓혀서 끝라인 부분이 비슷한 곳을 볼수 있도록 작업함.
				float fSize = static_cast<float>(g_dwCubeMapSize) * 0.5f;
				float fNewFOV = 2.0f * atan(fSize / (fSize - 0.5f));

				//D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI * 0.5f, 1.0f, 1.f, DxFogMan::GetInstance().GetFogEnd_RealTime() );
				// CubeMap 만드는 높이도(1.5m) 생각해서 10.f(1m) 정도 안에 들어오는것은 안 보이도록 한다.
				float fNearPlane(10.f);
				D3DXMatrixPerspectiveFovLH( &matProj, fNewFOV, 1.0f, fNearPlane, DxFogMan::GetInstance().GetFogEnd_RealTime() );

				pd3dDevice->GetTransform ( D3DTS_VIEW, &matOldView );
				pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matOldProj );

				pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

				DxBackupTarget sBackupTarget;
				sBackupTarget.Backup( pd3dDevice );


				LPDIRECT3DSURFACEQ pSurface;

				//for( UINT i = 0; i < 6; i++ )
				UINT i = g_nCubeMapIndex;
				{
					g_pCubeTex->GetCubeMapSurface ( (D3DCUBEMAP_FACES)i, 0, &pSurface );

					HRESULT hr(S_OK);
					hr = pd3dDevice->SetRenderTarget( 0, pSurface );
					hr = pd3dDevice->SetDepthStencilSurface( g_pSufFourCC_INTZ );

					// Clear the viewport
					D3DCOLOR colorClear = DxFogMan::GetInstance().GetFogColor();
					hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, colorClear, 1.0f, 0L );

					D3DXVECTOR3 vLookatPt;
					D3DXVECTOR3 vUpVec;
					matView = D3DUtil_GetCubeMapViewMatrix_New( (D3DCUBEMAP_FACES) i, vLookatPt, vUpVec );					// View 가져오기

					vLookatPt += vPos;
					D3DXVECTOR3 vFromPt = vPos;

					D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
					pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );

					// Note : 클리핑 정보를 만든다.
					CLIPVOLUME sCV = DxViewPort::GetInstance().ComputeClipVolume
					( 
						vLookatPt, 
						vFromPt, 
						vUpVec, 
						DxFogMan::GetInstance().GetFogStart_RealTime(), 
						DxFogMan::GetInstance().GetFogEnd_RealTime(), 
						1.f, 
						1.f, 
						fNewFOV	//D3DX_PI * 0.5f 
					);

					// 렌더링 위치.
					//pd3dDevice->BeginScene();
					{
						NSMaterialManager::FrameMove( matView, matProj, g_pColorTex_1st, g_pColorTex_2nd, g_pColorTex_3rd, g_pColorTex_4th );

						// 현재 순서상 NSMaterialManager::FrameMove 다음에 되어야 한다.
						NSMaterialManager::SetWindowSize( g_dwCubeMapSize, g_dwCubeMapSize );

						CLIPVOLUME sCameraCV;
						NSDeffered::ComputeClipVolume( sCameraCV, vLookatPt, vFromPt, vUpVec, fNearPlane, static_cast<float>(g_dwCubeMapSize), static_cast<float>(g_dwCubeMapSize), fNewFOV );

						NSDeffered::RenderDefferedForPSF
						( 
							pd3dDevice, 
							colorClear,
							pSetLandMan,
							g_dwCubeMapSize,
							g_dwCubeMapSize,
							pSurface,
							g_pColorSuf_1st,
							g_pColorSuf_2nd,
							g_pColorSuf_3rd,
							g_pColorSuf_4th,
							g_pColorSuf_LBuffer,
							g_pTexFourCC_INTZ,
							g_pColorTex_1st,
							g_pColorTex_2nd, 
							g_pColorTex_3rd,
							g_pColorTex_4th,
							g_pColorTex_LBuffer,
							matView,
							matProj,
							sCameraCV,
							FALSE
						);

						NSDeffered::RenderDefferedForPSF
						( 
							pd3dDevice, 
							colorClear,
							pSetLandMan,
							g_dwCubeMapSize,
							g_dwCubeMapSize,
							pSurface,
							g_pColorSuf_1st,
							g_pColorSuf_2nd,
							g_pColorSuf_3rd,
							g_pColorSuf_4th,
							g_pColorSuf_LBuffer,
							g_pTexFourCC_INTZ,
							g_pColorTex_1st,
							g_pColorTex_2nd, 
							g_pColorTex_3rd,
							g_pColorTex_4th,
							g_pColorTex_LBuffer,
							matView,
							matProj,
							sCameraCV,
							TRUE
						);

						//pSetLandMan->RenderSoftAlpha( pd3dDevice, NULL, TRUE );
						pSetLandMan->Render_EFF ( pd3dDevice, FALSE );

						//pSetLandMan->Render ( pd3dDevice, NULL, FALSE );
						//pSetLandMan->Render_EFF ( pd3dDevice, FALSE );

						// 하늘 렌더링..!! 후
						DxSkyMan::GetInstance().Render_AFTER( pd3dDevice, fNewFOV, 1.f );
					}
					//pd3dDevice->EndScene();

					SAFE_RELEASE ( pSurface );
				}

				sBackupTarget.Restore();

				pd3dDevice->SetTransform ( D3DTS_VIEW, &matOldView );
				pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matOldProj );

				NSMaterialManager::FrameMove( matOldView, matOldProj, NULL, NULL, NULL, NULL );

				// 돌아갈 큐브맵
				++g_nCubeMapIndex;
				if ( g_nCubeMapIndex >= 6 )
					g_nCubeMapIndex = 0;
			}

			g_bMyCubeMapRender = FALSE;
		}

		void OnRenderForPieceEditor( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture )
		{
			if ( !g_pCubeTex )
				return;

			if ( !pTexture )
				return;

			DxBackupTarget sBackupTarget;
			sBackupTarget.Backup( pd3dDevice );

			LPDIRECT3DSURFACEQ pSurface;

			g_pSavedEditSB->Capture();
			g_pDrawEditSB->Apply();

			UINT i = g_nCubeMapIndex;
			{
				g_pCubeTex->GetCubeMapSurface ( (D3DCUBEMAP_FACES)i, 0, &pSurface );

				HRESULT hr(S_OK);
				hr = pd3dDevice->SetRenderTarget( 0, pSurface );

				if ( g_textureResTempCubeMap.GetTexture() )
				{
					pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,	D3DTTFF_COUNT3 );

					pd3dDevice->SetFVF( VERTEXRHW_CUBE::FVF );
					pd3dDevice->SetTexture( 0, g_textureResTempCubeMap.GetTexture() );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertCube[i], sizeof(VERTEXRHW_CUBE) );
				}
				else
				{
					pd3dDevice->SetFVF( VERTEXRHW::FVF );
					pd3dDevice->SetTexture( 0, pTexture );
					pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );
				}

				SAFE_RELEASE ( pSurface );
			}

			g_pSavedEditSB->Apply();

			sBackupTarget.Restore();

			// 돌아갈 큐브맵
			++g_nCubeMapIndex;
			if ( g_nCubeMapIndex >= 6 )
				g_nCubeMapIndex = 0;
		}

		LPDIRECT3DCUBETEXTUREQ GetCubeMapTexture()
		{
			if ( g_bMyCubeMapRender )
				return NULL;
			else
				return g_pCubeTex;
		}
	}
};