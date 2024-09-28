#include "pch.h"

#include "../../DxLand/DxPiece/NSPieceQuickSort.h"
#include "../../DxLand/DxLandMan.h"
#include "../Material/NSMaterialManager.h"
#include "../Texture/NSTexCompress.h"
#include "../SkyBox/DxSkyBoxMan.h"
#include "../DxBackUpRendTarget.h"
#include "../DxCustomTypes.h"
#include "../DxSurfaceTex.h"
#include "../DxViewPort.h"
#include "../DxFogMan.h"

#include "NSCubeMap.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSCubeMap
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

	void CreateLocalCubeMap( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, DWORD dwSize, const D3DXVECTOR3& vPos, DxLandMan* pLandMan )
	{
		//////////////////////////////////////////////////////////////////////////
		//						현재 사용 안함.
		//LPDIRECT3DCUBETEXTUREQ	pCubeTex(NULL);
		//LPDIRECT3DSURFACEQ		pZBuffer(NULL);

		////	Note : 큐브 텍스쳐 만들기
		//D3DFORMAT colorFormat = D3DFMT_A8R8G8B8;
		//HRESULT hr = pd3dDevice->CreateCubeTexture ( dwSize, 1, D3DUSAGE_RENDERTARGET,			// 4. A8R8G8B8
		//												colorFormat, D3DPOOL_DEFAULT, &pCubeTex, NULL );
		//if( FAILED(hr) )
		//{
		//	return;
		//}


		//LPDIRECT3DSURFACEQ	pSurfaceBackUp_1st(NULL);
		//LPDIRECT3DTEXTUREQ	pTextureBackUp_1st(NULL);
		//hr = pd3dDevice->CreateTexture ( dwSize, dwSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, D3DPOOL_DEFAULT, &pTextureBackUp_1st, NULL );
		//if( FAILED(hr) )
		//	return;

		//hr = pTextureBackUp_1st->GetSurfaceLevel( 0, &pSurfaceBackUp_1st );
		//if( FAILED(hr) )
		//	return;


		//LPDIRECT3DSURFACEQ	pSurfaceBackUp_2nd(NULL);
		//LPDIRECT3DTEXTUREQ	pTextureBackUp_2nd(NULL);
		//hr = pd3dDevice->CreateTexture ( dwSize, dwSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, D3DPOOL_DEFAULT, &pTextureBackUp_2nd, NULL );
		//if( FAILED(hr) )
		//	return;

		//hr = pTextureBackUp_2nd->GetSurfaceLevel( 0, &pSurfaceBackUp_2nd );
		//if( FAILED(hr) )
		//	return;


		//LPDIRECT3DSURFACEQ	pSurfaceBackUp_3rd(NULL);
		//LPDIRECT3DTEXTUREQ	pTextureBackUp_3rd(NULL);
		//hr = pd3dDevice->CreateTexture ( dwSize, dwSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &pTextureBackUp_3rd, NULL );
		//if( FAILED(hr) )
		//	return;

		//hr = pTextureBackUp_2nd->GetSurfaceLevel( 0, &pSurfaceBackUp_3rd );
		//if( FAILED(hr) )
		//	return;


		//D3DFORMAT cZFormat(D3DFMT_D16);
		//hr = pd3dDevice->CreateDepthStencilSurface( dwSize, dwSize, cZFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &pZBuffer, NULL );
		//if( FAILED(hr) )
		//{
		//	return;
		//}


		//D3DXMATRIX	matOldView, matView;
		//D3DXMATRIX	matOldProj, matProj;

		//D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI * 0.5f, 1.0f, 1.f, DxFogMan::GetInstance().GetFogEnd_RealTime() );

		//pd3dDevice->GetTransform ( D3DTS_VIEW, &matOldView );
		//pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matOldProj );

		//pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

		//DxBackupTarget sBackupTarget;
		//sBackupTarget.Backup( pd3dDevice );

		//LPDIRECT3DSURFACEQ pSurface;

		//for( UINT i = 0; i < 6; i++ )
		//{
		//	pCubeTex->GetCubeMapSurface ( (D3DCUBEMAP_FACES)i, 0, &pSurface );

		//	HRESULT hr(S_OK);
		//	hr = pd3dDevice->SetRenderTarget( 0, pSurface );
		//	hr = pd3dDevice->SetDepthStencilSurface( pZBuffer );

		//	// Clear the viewport
		//	D3DCOLOR colorClear = DxFogMan::GetInstance().GetFogColor();
		//	hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, colorClear, 1.0f, 0L );

		//	//{
		//	//	hr = pd3dDevice->SetRenderTarget( 0, pSurfaceBackUp );
		//	//	HRESULT hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, colorClear, 1.0f, 0L );
		//	//}

		//	D3DXVECTOR3 vLookatPt;
		//	D3DXVECTOR3 vUpVec;
		//	matView = D3DUtil_GetCubeMapViewMatrix_New( (D3DCUBEMAP_FACES) i, vLookatPt, vUpVec );					// View 가져오기

		//	vLookatPt += vPos;
		//	D3DXVECTOR3 vFromPt = vPos;

		//	D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
		//	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );

		//	// Note : 클리핑 정보를 만든다.
		//	CLIPVOLUME sCV = DxViewPort::GetInstance().ComputeClipVolume
		//	( 
		//		vLookatPt, 
		//		vFromPt, 
		//		vUpVec, 
		//		DxFogMan::GetInstance().GetFogStart_RealTime(), 
		//		DxFogMan::GetInstance().GetFogEnd_RealTime(), 
		//		1.f, 
		//		1.f, 
		//		D3DX_PI * 0.5f 
		//	);

		//	// 화면 사이즈를 설정해줌.
		//	NSMaterialManager::SetWindowSize( dwSize, dwSize );

		//	// 렌더링 위치.
		//	pd3dDevice->BeginScene();
		//	{
		//		NSMaterialManager::FrameMove( matView, matProj, pTextureBackUp_1st, pTextureBackUp_2nd, pTextureBackUp_3rd, NULL );

		//		// Sort 에 알려줘서 LOD 기능이 동작 못하도록 한다.
		//		BOOL bViewAlphaLOD_BackUp = NSPieceQuickSort::g_bViewAlphaLOD;
		//		NSPieceQuickSort::g_bViewAlphaLOD = FALSE;

		//		// 하늘 먼저 렌더링 -------------- 1
		//		pd3dDevice->SetRenderTarget( 0, pSurface );

		//		DxSkyBoxMan::GetInstance().Render( pd3dDevice );
		//		DxSkyMan::GetInstance().Render( pd3dDevice, D3DX_PI * 0.5f, 1.f );

		//		D3DXMATRIX matIdentity;
		//		D3DXMatrixIdentity( &matIdentity );
		//		pLandMan->Render_Radiosity
		//		(
		//			pd3dDevice, 
		//			sCV,
		//			vFromPt,
		//			vLookatPt,
		//			vUpVec,
		//			matView,
		//			matProj,
		//			TRUE,
		//			colorClear,
		//			dwSize,
		//			dwSize,
		//			pSurface,
		//			pSurfaceBackUp_1st,
		//			pSurfaceBackUp_2nd,
		//			pSurfaceBackUp_3rd,
		//			pTextureBackUp_1st,
		//			pTextureBackUp_2nd,
		//			pTextureBackUp_3rd,
		//			FALSE,
		//			0.f,
		//			matIdentity,
		//			TRUE
		//		);

		//		NSPieceQuickSort::g_bViewAlphaLOD = bViewAlphaLOD_BackUp;
		//	}
		//	pd3dDevice->EndScene();

		//	// 화면 사이즈를 설정해줌.
		//	NSMaterialManager::SetWindowSize( DxSurfaceTex::GetInstance().GetWidth(), DxSurfaceTex::GetInstance().GetHeight() );

		//	SAFE_RELEASE ( pSurface );
		//}

		//sBackupTarget.Restore();

		//pd3dDevice->SetTransform ( D3DTS_VIEW, &matOldView );
		//pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matOldProj );

		//// 파일로 저장한다.
		//{
		//	// Note : 텍스쳐를 압축한다.
		//	LPDIRECT3DBASETEXTURE9 pTexRGB(NULL);
		//	NSTexCompress::Compress
		//	(
		//		pd3dDevice,
		//		dwSize,
		//		dwSize,
		//		0,
		//		1,
		//		D3DFMT_X8R8G8B8,
		//		pCubeTex,
		//		pTexRGB,
		//		0,
		//		TRUE
		//	);

		//	LPDIRECT3DBASETEXTURE9 pTexNew(NULL);
		//	NSTexCompress::Compress
		//	(
		//		pd3dDevice,
		//		dwSize,
		//		dwSize,
		//		0,
		//		1,
		//		D3DFMT_DXT1,
		//		pTexRGB,
		//		pTexNew,
		//		0,
		//		TRUE
		//	);

		//	// Note : 버퍼에 그려진 것을 텍스쳐로 뽑는다.
		//	TSTRING strTexture = strName;
		//	D3DXSaveTextureToFile( strTexture.c_str(), D3DXIFF_DDS, pTexNew, NULL );

		//	SAFE_RELEASE( pTexRGB );
		//	SAFE_RELEASE( pTexNew );
		//}

		//SAFE_RELEASE ( pSurfaceBackUp_1st );
		//SAFE_RELEASE ( pTextureBackUp_1st );
		//SAFE_RELEASE ( pSurfaceBackUp_2nd );
		//SAFE_RELEASE ( pTextureBackUp_2nd );
		//SAFE_RELEASE ( pSurfaceBackUp_3rd );
		//SAFE_RELEASE ( pTextureBackUp_3rd );
		//SAFE_RELEASE ( pCubeTex );
		//SAFE_RELEASE ( pZBuffer );
	}


	struct CUBEMAP_DATA
	{
		
	};

	typedef std::map<DWORD,std::tr1::shared_ptr<CUBEMAP_DATA>>	MAP_CUBEMAP_DATA;
	typedef MAP_CUBEMAP_DATA::iterator						MAP_CUBEMAP_DATA_ITER;

	MAP_CUBEMAP_DATA g_mapCubeMapData;


	void CleanUp();
};