#include "pch.h"

#include "../../SigmaCore/File/SerialFile.h"

#include "../../VisualMaterialLib/Manager/NSVMRender.h"

#include "../DxEffect/TexEff/NSTexEffVisualMaterial.h"
#include "../DxEffect/TextureSetDXT.h"
#include "../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../DxTools/PostProcessing/NSLightShafts.h"
#include "../DxTools/Material/NSMaterialManager.h"
#include "../DxTools/SkyBox/DxSkyBoxMan.h"
#include "../DxTools/Light/NSShadowLight.h"
#include "../DxTools/Light/DxLightMan.h"
#include "../DxTools/RENDERPARAM.h"
#include "../DxTools/DxWeatherMan.h"
#include "../DxTools/DxViewPort.h"
#include "../DxSound/DxSoundMan.h"
#include "../DxSound/BgmSound.h"
#include "../Common/SUBPATH.h"
#include "../Archive/Archive.h"
#include "../Water/NSWaterMain.h"
#include "../GlobalParam.h"
#include "./LightMap/NSLightMapFX.h"
#include "./DxPiece/NSPieceQuickSort.h"
#include "./DxPiece/DxPieceContainer.h"
#include "./DxPiece/DxPieceManagerPSF.h"
#include "./DxPiece/DxPieceView.h"
#include "./DxLandSkinObj.h"
#include "./DxPrefabMan.h"

#include "DxSetLandMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


BOOL	g_bLANDEFF_DSPNAME = FALSE;
BOOL    g_bWIREFRAME_BLACK = FALSE;
BOOL	g_bSKINOBJECT_RENDER_NAME = FALSE;

extern BOOL g_bWIREFRAME_VIEW;

////////////////////////////////////////////////////////////////////////////////////
//	$31 2006-02-24 T14:45	[0x32�� ���� �ø�. DxMesh�� LightMap�� �߰��ϸ鼭 ������ �ٲ�.]
//	$30 2006-02-24 T14:45	[0x31�� ���� �ø�. m_PieceManager�� �ε��� ������.]
//	$30 2005-06-11 T11:10	[FrameMesh�� TextureEff �߰� �ϸ鼭 ������ �ٲ�]
//	$29 2005-05-12 T11:10	[m_PieceManager �߰�]
//	$28 2005-03-24 T13:40	[TextureSetDXT data �߰�]
//	$27 2004-03-03 T15:00	[SKY data �߰�]
//	$26 2003-12-04 T15:00	[FOG data �߰�]
//	$25	2003-04-21 T15:12	[LightMan ���� �� �������� ( �㳷 ��ȭ �߰� ), ��ȿ�� ����, �׸��� ����. ]
//	$24	2003-04-18 T18:18	[Export ���� ���� �� ġȯ ������Ʈ ���� ����.]
//	$23	2003-04-18 T16:18	[��ȿ�� ����.]
//	$22	2003-04-17 T21:40	[ġȯ ���� ���� ���� ����.] 
//	$21	2003-04-08 T15:06	[�̰����� �� ����.]
//	$20	2003-03-25 T20:20	[���� ���� �޼��� �߰�, ��Ÿ��� ������.]
//	$19	2003-03-05 T13:20	[������� Ŭ���� �߰���]
//	$18	2003-03-03 T18:02	[�� �� ����.]
//	$17	2003-02-19 T16:06	[���۰� �Ѿ��ٰ� GASSERT�� ���� ���� ��������]
//	$16	2003-02-19 T15:20	[�� �� ����,���缱��� ��û��.. �Ͼ�� ��������]
//	$15	2003-02-05 T11:34	[�׺���̼� �޽�, ã�ư��� �� �����ֱ�]
//	$14	2003-02-03 T11:46	[WorldSoundPage ����, ���۸� 0�� ����� ���� ����]
//	$13	2003-01-29 T22:41	[Land Gate ���� ����]
//							[RenderState �ε� ����]
//	$12	2003-01-29 T16:26	[�� ���� ��� ���� ����]
//	$11	2003-01-29 T14:33	[ġȯ ������Ʈ ȿ�� ��ġ ����]
//	$10	2003-01-29 T10:23	[�б� �κ� psf���� �ε� ���� ����]
//	$9	2003-01-22 T11:15	[�׺���̼� �޽�, ����Ÿ ���Ἲ üũ ���� ����
//							���丮 �ε� ��ΰ� Ʋ�Ⱦ���]
//							[��������]
//	$8	2003-01-16 T14:10	[¥���� ���׵� ����]
//	$7	2003-01-10 T00:23	[�� ������ ���� �߰��� ������Ȳ ����(ReleaseWav�� �Ͼ�
//							���� �ٸ� ������ ����ϴ� ���۸� ������)]
//	$6.	2003-01-09 T21:04	[ġȯ������Ʈ ���� ��/LandGate ��� �߰�/���� �۾�
//							�������̽� ����"��/���"]
//	$5.	2003-01-04 T11:39	[�ܵ�/Ȧ�� ����]
//	$4.	2002-12-10 T19:15	[�ջ� ����]
//	$3.	2002-12-04 T16:31	[�� ���� ��� ����, ���� �����߰� �� ���� ����]
//	$2.	2002-11-30 T12:01	[ĳ���� ������ ���� �̵� ����]
//	$1.	2002-11-29 Txx:xx	[���� ����]
////////////////////////////////////////////////////////////////////////////////////


DxSetLandMan::DxSetLandMan ( BOOL bPrefab )
    : m_bPrefab(bPrefab)
	, m_dwNumLandEff(0)
	, m_pLandEffList(NULL)
	, m_pLandEffTree(NULL)
	, m_dwNumLandSkinObj(NULL)
	, m_pLandSkinObjList(NULL)
	, m_pLandSkinObjTree(NULL)
	, m_pNaviFrame(NULL)
    , m_strLightMapName(_T(""))     // LightMap Name
    , m_fLightMapDetail(0.25f)      // LightMap Detail
	, m_bPrevSaveFile_120712(FALSE)

	, m_scpPrefabMan(new DxPrefabMan)
	, m_scpShadowLightData(new ShadowLightData)
{
	m_strFileName = _T("");
	memset( m_szMapName, 0, sizeof(char)*MAXLANDNAME );

	m_pFrameMesh = new DxFrameMesh(FALSE);

    m_pLightMan = new DxLightMan;

	// [12.12.18] Prefab ������ ���ܼ� Prefab �� �ƴ� ��츸 �����ϵ��� ���ش�.
	//
	//	Note : DxSetLandMan�� ���������� Instance�� �����ϰ� �����Ѵٴ� �����Ͽ�
	//			�ʱ�ȭ�ÿ� LightMan �� Active Instance�� �����Ѵ�.	
	//
	if ( !bPrefab )
	{
		DxLightMan::SetInstance ( m_pLightMan );
	}

	DxWeatherMan::SetInstance ( &m_WeatherMan );

	//	Note : �ʱ�ȭ �̿ܿ� �ٸ� ó���� �ʿ����� �ʾ�,
	//		   �����ڿ� ������.
	m_CollisionMap.SetDxFrameMesh ( m_pFrameMesh );

    m_pPieceManager = new DxPieceManagerPSF;
    m_pPieceView = new DxPieceView;
}

DxSetLandMan::~DxSetLandMan (void)
{
	SAFE_DELETE( m_pFrameMesh );
    SAFE_DELETE( m_pLightMan );
    SAFE_DELETE( m_pPieceManager );
    SAFE_DELETE( m_pPieceView );
}

HRESULT DxSetLandMan::ActiveMap ()
{
	DxLightMan::SetInstance( m_pLightMan );

	DxWeatherMan::SetInstance ( &m_WeatherMan );

	DxFogMan::GetInstance().SetProperty ( m_FOG_PROPERTY );

	DxSkyMan::GetInstance().SetProperty ( m_SKY_PROPERTY );

	NSShadowLight::SetActive( m_scpShadowLightData.get() );

	return S_OK;
}

HRESULT DxSetLandMan::OneTimeSceneInit ()
{
	HRESULT hr = S_OK;

	hr = m_pFrameMesh->OneTimeSceneInit ();
	if ( FAILED(hr) )	return hr;	

	return S_OK;
}

HRESULT DxSetLandMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_pFrameMesh->GetFileName()!=NULL )
	{
		hr = m_pFrameMesh->InitDeviceObjects ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->InitDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}

		m_CollisionMap.CleanUp();
		m_CollisionMap.Import( TRUE );
	}

	m_pLightMan->InitDeviceObjects ( pd3dDevice );
    m_pPieceView->OnCreateDevice( pd3dDevice );

	return S_OK;
}

HRESULT DxSetLandMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	hr = m_pFrameMesh->RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->RestoreDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}
	}
	
	{
		PLANDSKINOBJ pCur = m_pLandSkinObjList;
		while ( pCur )
		{
			pCur->RestoreDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}
	}

	return S_OK;
}

HRESULT DxSetLandMan::InvalidateDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	hr = m_pFrameMesh->InvalidateDeviceObjects( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->InvalidateDeviceObjects();

			pCur = pCur->m_pNext;
		}
	}

	{
		PLANDSKINOBJ pCur = m_pLandSkinObjList;
		while ( pCur )
		{
			pCur->InvalidateDeviceObjects();

			pCur = pCur->m_pNext;
		}
	}

	return S_OK;
}

HRESULT DxSetLandMan::DeleteDeviceObjects ()
{
	HRESULT hr = S_OK;

	hr = m_pFrameMesh->DeleteDeviceObjects ();
	if ( FAILED(hr) )	return hr;

	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->DeleteDeviceObjects ();

		pCur = pCur->m_pNext;
	}

	m_pLightMan->DeleteDeviceObjects ();

	CTextureSetDXT::GetInstance().DeleteDeviceObjects();

	return S_OK;
}

HRESULT DxSetLandMan::FinalCleanup ()
{
	HRESULT hr = S_OK;

	hr = m_pFrameMesh->FinalCleanup();
	if ( FAILED(hr) )	return hr;

	m_pNaviFrame = NULL;
	m_NaviMesh.Clear ();

	hr = m_StaticSoundMan.FinalCleanup ();
	if ( FAILED(hr) )	return hr;

	m_dwNumLandEff = 0;
	SAFE_DELETE(m_pLandEffList);
	SAFE_DELETE(m_pLandEffTree);

	m_dwNumLandSkinObj = 0;
	SAFE_DELETE(m_pLandSkinObjList);
	SAFE_DELETE(m_pLandSkinObjTree);

	m_pLightMan->CleanUp ();
	m_CameraAniMan.Cleanup ();
	m_LandGateMan.CleanUp ();
	m_CollisionMap.CleanUp ();

	m_pPieceManager->CleanUp ();
    DxSkyBoxMan::GetInstance().CleanUp();

	m_bPrevSaveFile_120712 = FALSE;
	//DeleteLightMapUV();

	m_scpPrefabMan->CleanUp();

	m_strFileName = _T("");

	return S_OK;
}

void DxSetLandMan::SetFogProperty ( FOG_PROPERTY& Property )
{
	m_FOG_PROPERTY = Property;
	DxFogMan::GetInstance().SetProperty ( m_FOG_PROPERTY );
}

void DxSetLandMan::SetSkyProperty ( SKY_PROPERTY& Property )
{
	m_SKY_PROPERTY = Property;
	DxSkyMan::GetInstance().SetProperty ( m_SKY_PROPERTY );
}

HRESULT DxSetLandMan::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	// �ϴ� ���ϳ��� ��ġ�� �س��´�.
	RENDERPARAM::g_emGroundQulity = TnL_PS_3_0_CSM;

	hr = m_pFrameMesh->FrameMove ( fTime, fElapsedTime );
	if ( FAILED(hr) )	return hr;

	//	Note : ī�޶� ���⿡ ���� �Ҹ���ȭ �ݿ�
	//		   ���� ���� Ŭ����
	//
	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	
	PROFILE_BEGIN("StaticSound");
	m_StaticSoundMan.FrameMove ( vLookatPt, fElapsedTime );
	PROFILE_END("StaticSound");

	DXLANDEFF::FrameMove ( fTime, fElapsedTime );
	DXLANDSKINOBJ::FrameMove ( fTime, fElapsedTime );

	m_pPieceManager->FrameMove( fTime, fElapsedTime );

	m_CameraAniMan.FrameMove ( fTime, fElapsedTime );

    DxSkyBoxMan::GetInstance().FrameMove( fElapsedTime );

	// ���� �� ������
	NSWaterMainPSF::FrameMove( fElapsedTime );

	// Prefab
	m_scpPrefabMan->FrameMove( fTime, fElapsedTime );

	// Skin������ ������ FrameMove �� �ʿ��ϴ�.
	CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();
	COLLISION::FrameMoveAABBTree( m_pLandSkinObjTree, fElapsedTime, cv, NULL );

	return S_OK;
}

HRESULT DxSetLandMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pmatPrefab, BOOL bFrameMove )
{
	HRESULT hr = S_OK;

	// �ϴ� ���ϳ��� ��ġ�� �س��´�.
	RENDERPARAM::g_emGroundQulity = TnL_PS_3_0_CSM;

	CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();

	// Prefab �̶�� �Ϻθ� �۾��ȴ�.
	if ( m_bPrefab )
	{
		// Prefab ���� Matrix �� ������ PieceManager �� �����Ѵ�.
		if ( pmatPrefab )
		{
			m_pPieceManager->SetPrefabMatrix( *pmatPrefab );
		}

		// Note : ���� ���� ������.
		m_pPieceManager->Render( pd3dDevice, cv, TRUE, m_fLightMapDetail, bFrameMove );

		// Note : Camera Pick Object    AlphaBlending
		m_pPieceManager->RenderForLodBlend( pd3dDevice );
		//m_pPieceManager->RenderPickAlpha( pd3dDevice, NULL, m_pFrameMesh, TRUE, bFrameMove );


		return S_OK;
	}

    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return S_OK;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

    // ī�޶� ������ �۵��Ǿ�� �Ѵ�.
    NSLightMapFX::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj(), m_fLightMapDetail );

	//NSPieceQuickSort::InitPickAlpha();		// Note : Init Static
	//NSPieceQuickSort::Reset();

	NSPieceQuickSort::Reset();
	NSPieceQuickSort::InitPickAlpha();

	if ( m_pLandSkinObjTree )
	{
		const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		const D3DXVECTOR3& vLookatPt = DxViewPort::GetInstance().GetLookatPt();
		D3DXVECTOR3 vDir = vFromPt - vLookatPt;
		float fLengthSq = D3DXVec3LengthSq( &vDir );
		COLLISION::InsertData( m_pLandSkinObjTree, cv, vFromPt, vLookatPt, fLengthSq );
	}

    // Psf_FX �� ����Ѵ�.
    NSVERTEXNORTEX_FX::SetEnable();
	NSVERTEXNORTEX_FX::FrameMove();
    {

		float fFogStart(1.f);
		float fFogEnd(2.f);
		float fBias = -0.001f;
		DWORD dwBiasPrev(0);
		DWORD dwFogStartPrev(0);
		DWORD dwFogEndPrev(0);
		DWORD dwFogColor(0L);
		DWORD dwFogEnable(0L);
		if ( g_bWIREFRAME_VIEW )
		{
			pd3dDevice->GetRenderState ( D3DRS_DEPTHBIAS,	&dwBiasPrev );
			pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	&dwFogStartPrev );
			pd3dDevice->GetRenderState ( D3DRS_FOGEND,	&dwFogEndPrev );
			pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
			pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,	&dwFogEnable );
			pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
			pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
			pd3dDevice->SetRenderState ( D3DRS_FOGEND,	*((DWORD *)(&fFogEnd)) );
			pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );
			pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	TRUE );

            NSMaterialManager::SetFogColorForEditor( 0xff000000 );
		}

	    CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();

		//if ( m_pFrameMesh && NSLightMapFX::g_emRenderType == NSLIGHTMAPDEF::LMRT_CHECKER )
		//{
		//	// ī�޶� ������ �۵��Ǿ�� �Ѵ�.
		//	NSLightMapFX::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj(), 1.f );
		//	NSLightMapFX::SetLightMapUV_Offset( D3DXVECTOR2(0.f,0.f) );

		//	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
		//	m_pFrameMesh->RenderLightMapChecker( pd3dDevice, m_pFrameMesh->GetFrameRoot(), &cv );
		//}
		//else
		{
			hr = m_pFrameMesh->Render_Deffered_GeometryPass ( pd3dDevice, NULL, &cv, FALSE );
			if ( FAILED(hr) )	
				return hr;
		}

		if ( g_bWIREFRAME_VIEW )
		{
			pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff00ffff );

            NSMaterialManager::SetFogColorForEditor( 0xff00ffff );
		}

		// Prefab �������� m_pPieceManager ���� ���� �̷������ �Ѵ�.
		m_scpPrefabMan->Render( pd3dDevice, bFrameMove );

	    // Note : ���� ���� ������.
	    m_pPieceManager->Render( pd3dDevice, cv, TRUE, m_fLightMapDetail, bFrameMove );

		//// ����
		//{
		//	// Note : Camera Pick Object    AlphaBlending
		//	m_pPieceManager->RenderOpaqueForLodBlend( pd3dDevice );
		//	//m_pPieceManager->RenderPickAlpha( pd3dDevice, NULL, m_pFrameMesh, TRUE, bFrameMove );
		//}

		if ( g_bWIREFRAME_VIEW )
		{
			pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	dwBiasPrev );
			pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	dwFogStartPrev );
			pd3dDevice->SetRenderState ( D3DRS_FOGEND,	dwFogEndPrev );
			pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );
			pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	dwFogEnable );

            NSMaterialManager::SetFogColorForEditor( dwFogColor );
		}

	    //	Note : ���� ���� ���� �׸���
	    m_StaticSoundMan.Render ( pd3dDevice );

	    if ( DxLandGateMan::m_bREND )
            m_LandGateMan.Render ( pd3dDevice );
    }
    NSVERTEXNORTEX_FX::SetDisable();

	return S_OK;
}

void DxSetLandMan::RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Camera Pick Object    AlphaBlending
	m_pPieceManager->RenderForLodBlend( pd3dDevice );
}

void DxSetLandMan::RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Camera Pick Object    AlphaBlending
	m_pPieceManager->RenderSoftAlphaForLodBlend( pd3dDevice );
}

HRESULT DxSetLandMan::RenderSoftAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pmatPrefab, BOOL bFrameMove, BOOL bInverseOrder )
{
	HRESULT hr = S_OK;

	// Prefab �̶�� Piece�� �׸���.
	if ( m_bPrefab )
	{
		//// Note : Camera Pick Object    AlphaBlending
		//m_pPieceManager->RenderPickAlpha( pd3dDevice, NULL, m_pFrameMesh, TRUE, bFrameMove );

		return S_OK;
	}

	CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();


	float fFogStart(1.f);
	float fFogEnd(2.f);
	float fBias = -0.001f;
	DWORD dwBiasPrev(0);
	DWORD dwFogStartPrev(0);
	DWORD dwFogEndPrev(0);
	DWORD dwFogColor(0L);
	DWORD dwFogEnable(0L);
	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->GetRenderState ( D3DRS_DEPTHBIAS,	&dwBiasPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	&dwFogStartPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGEND,	&dwFogEndPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
		pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,	&dwFogEnable );
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
		pd3dDevice->SetRenderState ( D3DRS_FOGEND,	*((DWORD *)(&fFogEnd)) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	TRUE );

		NSMaterialManager::SetFogColorForEditor( 0xff00ffff );
	}

	// ����
	if( GLOBAL_PARAM::IsValue( GP_PIECE ) && bInverseOrder )
	{
		m_pPieceManager->RenderSoftAlpha( pd3dDevice, cv );

		// Note : ���� ���� ������.
		//		m_pPieceManager->RenderEff( pd3dDevice, NULL, m_pFrameMesh, TRUE, m_fLightMapDetail );
	}

	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff00ffff );

		NSMaterialManager::SetFogColorForEditor( 0xff000000 );
	}

	hr = m_pFrameMesh->RenderSoftAlpha_Deffered_GeometryPass ( pd3dDevice, bInverseOrder );
	//hr = m_pFrameMesh->Render_Forward_SoftAlpha( pd3dDevice, NULL, &cv );
	if ( FAILED(hr) )	return hr;

	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );

		NSMaterialManager::SetFogColorForEditor( 0xff000000 );
	}

	//// ����
	//{
	//	// Note : Camera Pick Object    AlphaBlending
	//	m_pPieceManager->RenderPickAlpha( pd3dDevice, NULL, m_pFrameMesh, TRUE, bFrameMove );
	//}

	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	dwBiasPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	dwFogStartPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGEND,	dwFogEndPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	dwFogEnable );

		NSMaterialManager::SetFogColorForEditor( dwFogColor );
	}

	return S_OK;
}

HRESULT DxSetLandMan::Render_DecalPass2_WaterCustomSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Prefab �̶�� Piece�� �׸���.
	if ( m_bPrefab )
	{
		m_pPieceManager->Render_DecalPass2_WaterCustomSoftAlpha();

		return S_OK;
	}

	CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();
	HRESULT hr = m_pFrameMesh->Render_DecalPass2_WaterCustomSoftAlpha( pd3dDevice, &cv );
	if ( FAILED(hr) )	return hr;

	m_pPieceManager->Render_DecalPass2_WaterCustomSoftAlpha();

	return S_OK;
}

HRESULT DxSetLandMan::Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFrameMove )
{
	HRESULT hr = S_OK;

	// Prefab �̶�� Piece�� �׸���.
	if ( m_bPrefab )
	{
		// Note : ���� ���� ������.
		m_pPieceManager->RenderEff( pd3dDevice, NULL, m_pFrameMesh, TRUE, m_fLightMapDetail );

		return S_OK;
	}

    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return S_OK;

	CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();

    // Psf_FX �� ����Ѵ�.
    NSVERTEXNORTEX_FX::SetEnable();
    {
		float fFogStart(1.f);
		float fFogEnd(2.f);
		float fBias = -0.001f;
		DWORD dwBiasPrev(0);
		DWORD dwFogStartPrev(0);
		DWORD dwFogEndPrev(0);
		DWORD dwFogColor(0L);
		DWORD dwFogEnable(0L);
		if ( g_bWIREFRAME_VIEW )
		{
			pd3dDevice->GetRenderState ( D3DRS_DEPTHBIAS,	&dwBiasPrev );
			pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	&dwFogStartPrev );
			pd3dDevice->GetRenderState ( D3DRS_FOGEND,	&dwFogEndPrev );
			pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
			pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,	&dwFogEnable );
			pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
			pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
			pd3dDevice->SetRenderState ( D3DRS_FOGEND,	*((DWORD *)(&fFogEnd)) );
			pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );
			pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	TRUE );

            NSMaterialManager::SetFogColorForEditor( 0xff000000 );
		}

		m_pPieceManager->RenderEff( pd3dDevice, NULL, m_pFrameMesh, TRUE, m_fLightMapDetail );

		if ( g_bWIREFRAME_VIEW )
		{
			pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );

            NSMaterialManager::SetFogColorForEditor( 0xff000000 );
		}

	    if ( m_pLandEffTree )
	    {
		    //COLLISION::dwRendAABB = 0;
		    hr = COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );
		    if ( FAILED(hr) )	return hr;

		    //CDebugSet::ToView ( 8, "COLLISION::dwRendAABB = %d", COLLISION::dwRendAABB );
	    }
	    else
	    {
		    PLANDEFF pCur = m_pLandEffList;
		    while ( pCur )
		    {
			    pCur->Render ( pd3dDevice, &cv );

			    pCur = pCur->m_pNext;
		    }
	    }

	    if( g_bLANDEFF_DSPNAME )
	    {
		    PLANDEFF pCur = m_pLandEffList;
		    while ( pCur )
		    {
			    pCur->RenderName( pd3dDevice );
			    pCur = pCur->m_pNext;
		    }
	    }

		if ( g_bSKINOBJECT_RENDER_NAME && m_pLandSkinObjList )
		{
			m_pLandSkinObjList->RenderName( pd3dDevice );
		}

		if ( g_bWIREFRAME_VIEW )
		{
			pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	dwBiasPrev );
			pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	dwFogStartPrev );
			pd3dDevice->SetRenderState ( D3DRS_FOGEND,	dwFogEndPrev );
			pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );
			pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	dwFogEnable );

            NSMaterialManager::SetFogColorForEditor( dwFogColor );
		}
    }
    NSVERTEXNORTEX_FX::SetDisable();

	// ���� �� ������
	NSWaterMainPSF::Render( pd3dDevice );

	return S_OK;
}

void DxSetLandMan::RenderStarGlow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// VisualMaterial�� StarGlow ���͸��� ������
	NSTexEffVisualMaterial::Render( pd3dDevice, &NSVMRender::g_sStarGlow );
}

void DxSetLandMan::Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice )
{
	LPDIRECT3DSURFACEQ	pSrcZBuffer(NULL);
	pd3dDevice->GetDepthStencilSurface( &pSrcZBuffer );

	if( NSLightShafts::OnRenderStart( pd3dDevice ) )
	{
		DxSkyBoxMan::GetInstance().Render_LightShafts( pd3dDevice );
		//m_pFogLocalMan->Render_LightShafts( pd3dDevice );

		NSLightShafts::OnRenderEnd( pd3dDevice, pSrcZBuffer );
	}

	SAFE_RELEASE ( pSrcZBuffer );
}

HRESULT DxSetLandMan::DrawWireFrame ( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return S_OK;

    DWORD OldFillMode;
	pd3dDevice->GetRenderState(D3DRS_FILLMODE, &OldFillMode );
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );


    float fFogStart(1.f);
    float fFogEnd(2.f);
    float fBias = -0.001f;
    DWORD dwBiasPrev(0);
    DWORD dwFogStartPrev(0);
    DWORD dwFogEndPrev(0);
    pd3dDevice->GetRenderState ( D3DRS_DEPTHBIAS,	&dwBiasPrev );
    pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	&dwFogStartPrev );
	pd3dDevice->GetRenderState ( D3DRS_FOGEND,		&dwFogEndPrev );
	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
    pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
	pd3dDevice->SetRenderState ( D3DRS_FOGEND,		*((DWORD *)(&fFogEnd)) );

    //	Note : ���͸���, �ؽ��� ����.
	//
	D3DMATERIALQ DarkMaterial;
	memset(&DarkMaterial, 0, sizeof(D3DMATERIALQ));
	DarkMaterial.Diffuse.r = 0.0f;
	DarkMaterial.Diffuse.g = 0.0f;
	DarkMaterial.Diffuse.b = 0.0f;
	DarkMaterial.Specular = DarkMaterial.Diffuse;

	pd3dDevice->SetMaterial( &DarkMaterial );
	pd3dDevice->SetTexture( 0, NULL );

    // Fog
    NSLightMapFX::ShortBlackFog();
    pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0L );

    // ������.~!!
    {
        g_bWIREFRAME_BLACK = TRUE;

        CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();

		if ( m_pFrameMesh->GetFrameRoot() )
		{
			HRESULT hr = m_pFrameMesh->DrawWireFrame ( pd3dDevice, m_pFrameMesh->GetFrameRoot(), &cv );
		}

        pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
        pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
	    pd3dDevice->SetRenderState ( D3DRS_FOGEND,		*((DWORD *)(&fFogEnd)) );

        //	Note : ���͸���, �ؽ��� ����.
	    pd3dDevice->SetMaterial( &DarkMaterial );

        // Fog
        NSLightMapFX::ShortBlackFog();
        pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0L );

        m_pPieceManager->DrawWireFrame( pd3dDevice, &cv );

        g_bWIREFRAME_BLACK = FALSE;
    }

	pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	dwBiasPrev );
    pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	dwFogStartPrev );
	pd3dDevice->SetRenderState ( D3DRS_FOGEND,		dwFogEndPrev );

    pd3dDevice->SetRenderState(D3DRS_FILLMODE, OldFillMode );

    return S_OK;
}

void DxSetLandMan::SetNaviFrame ( DxFrame *pNaviMesh )
{
	GASSERT ( pNaviMesh && "�׺���̼� �޽� �������� ����" );

	if ( m_pNaviFrame ) m_pNaviFrame->bFlag &= ~DXFRM_NAVIFRAME;
	
	m_pNaviFrame = pNaviMesh;
	m_pNaviFrame->bFlag |= DXFRM_NAVIFRAME;
}

BOOL DxSetLandMan::CreateNaviMesh ( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

	m_NaviMesh.Clear();
	if ( m_pNaviFrame )
	{
		hr = m_NaviMesh.CreateNaviMesh ( m_pNaviFrame, pd3dDevice );
		if ( FAILED(hr) )	goto _ERROR;
		
		hr = m_NaviMesh.MakeAABBTree ();
		if ( FAILED(hr) )	goto _ERROR;
		
		m_NaviMesh.LinkCells ();
	}

	return TRUE;

_ERROR:
	m_NaviMesh.Clear();

	return FALSE;
}

void DxSetLandMan::BuildSingleEffTree ()
{
	PLANDEFF pCur = m_pLandEffList;
	while( pCur )
	{
		pCur->ReSetAABBBox();
		pCur = pCur->m_pNext;
	}
	COLLISION::MakeAABBTree ( m_pLandEffTree, m_pLandEffList );
}

PLANDEFF DxSetLandMan::FindLandEff ( char* szName )
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

PLANDEFF DxSetLandMan::FindLandEff( D3DXVECTOR3& vCenter, D3DXVECTOR3& vTarget )
{
	D3DXVECTOR3 vColl, vNor;
	POBJAABB pObj = NULL;
	COLLISION::CollisionLineToTree( m_pLandEffTree, vCenter, vTarget, pObj, vColl, vNor );

	PLANDEFF pEff = (PLANDEFF)pObj;
	return pEff;
}

void DxSetLandMan::AddLandEff ( PLANDEFF pLandEff )
{
	m_dwNumLandEff++;
	pLandEff->m_pNext = m_pLandEffList;
	m_pLandEffList = pLandEff;
}

void DxSetLandMan::DelLandEff ( PLANDEFF pLandEff )
{
	GASSERT(pLandEff);
	PLANDEFF pCur, pBack;

	if ( m_pLandEffList==pLandEff )
	{
		pCur = m_pLandEffList;
		m_pLandEffList = m_pLandEffList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandEff--;
		return;
	}

	pCur = m_pLandEffList;
	while ( pCur )
	{
		if ( pCur==pLandEff )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandEff--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

void DxSetLandMan::ReverseListLandEff()
{
	PLANDEFF pNewRoot = NULL;
	PLANDEFF pChange = NULL;
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pChange = pCur;
		pCur = pCur->m_pNext;

		pChange->m_pNext = pNewRoot;
		pNewRoot = pChange;
	}

	m_pLandEffList = pNewRoot;
}

void DxSetLandMan::Clone_MouseShift( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX& pMatrix )
{
	PLANDEFF pCur = m_pLandEffList;
	while (pCur)
	{
		DWORD_PTR dwBuffer = (DWORD_PTR) &pCur->m_matWorld;
		if (dwBuffer == (DWORD_PTR) pMatrix)
		{
			DXLANDEFF* pLandEff;
			pLandEff = new DXLANDEFF;

			char szNewName[DXLANDEFF::LANDEFF_MAXSZ] = "";
			
			int i=0;
			do
			{
				//sprintf( szNewName, "%s[%d]", pCur->m_szFileName, i++ );
				StringCchPrintf( szNewName, DXLANDEFF::LANDEFF_MAXSZ,"%s[%d]", pCur->m_szFileName, i++ );
			} while( FindLandEff(szNewName) );
			pLandEff->SetEffect
            ( 
                szNewName, 
                pCur->m_matWorld, 
                pCur->m_szFileName, 
                pCur->m_fColorPowerDay,
                pCur->m_fColorPowerNight,
                pd3dDevice 
            );

			pMatrix = &pLandEff->m_matWorld;	// DxObjectMRS Ŀ���� �ٲ��ش�. < �߿� >
			AddLandEff( pLandEff );

			BuildSingleEffTree ();

			return;
		}
		pCur = pCur->m_pNext;
	}

	DxLightMan::GetInstance()->Clone_MouseShift( pMatrix );

	m_scpShadowLightData->Clone_MouseShift( pMatrix );

	if ( m_pPieceManager )
	{
		m_pPieceManager->Clone_MouseShift( pd3dDevice, pMatrix );
	}

	m_StaticSoundMan.Clone_MouseShift( pMatrix );
}

void DxSetLandMan::SetObjRotate90()
{
    float fRotate(90.f);
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*fRotate/180.f );

	// Note : ����Ʈ
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		D3DXMatrixMultiply( &pCur->m_matWorld, &pCur->m_matWorld, &matRotate );
		pCur = pCur->m_pNext;
	}
	BuildSingleEffTree();

	// Note : ��Ų ��������
	PLANDSKINOBJ pSkin = m_pLandSkinObjList;
	while( pSkin )
	{
		D3DXMatrixMultiply( &pSkin->m_matWorld, &pSkin->m_matWorld, &matRotate );
		pSkin = pSkin->m_pNext;
	}
	BuildSkinObjTree();

	// Note : ����Ʈ
	DxLightMan::GetInstance()->SetObjRotate90( matRotate );
	DxLightMan::GetInstance()->MakeAABBTree();

	// Note : ����
	m_StaticSoundMan.SetObjRotate90();

	// Note : ����Ʈ�� ��ȯ
	m_LandGateMan.SetObjRotate90();

    // �ϴ� ȸ��.
    DxSkyBoxMan::GetInstance().SetObjRotate90( fRotate );
}

void DxSetLandMan::AddLandSkinObj ( PLANDSKINOBJ pLandSkinObj )
{
	m_dwNumLandSkinObj++;
	pLandSkinObj->m_pNext = m_pLandSkinObjList;
	m_pLandSkinObjList = pLandSkinObj;
}

void DxSetLandMan::DelLandSkinObj ( PLANDSKINOBJ pLandSkinObj )
{
	GASSERT(pLandSkinObj);
	PLANDSKINOBJ pCur, pBack;

	if ( m_pLandSkinObjList==pLandSkinObj )
	{
		pCur = m_pLandSkinObjList;
		m_pLandSkinObjList = m_pLandSkinObjList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandSkinObj--;
		return;
	}

	pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( pCur==pLandSkinObj )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandSkinObj--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

PLANDSKINOBJ DxSetLandMan::FindLandSkinObj ( char* szName )
{
	PLANDSKINOBJ pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

void DxSetLandMan::BuildSkinObjTree ()	
{
	COLLISION::MakeAABBTree ( m_pLandSkinObjTree, m_pLandSkinObjList );
}

// Effect�� �˻��Ѵ�.
BOOL DxSetLandMan::IsCollisionLineEffect( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, POBJAABB& pOBJ )
{
    D3DXVECTOR3 vNor(0.f,1.f,0.f);
	COLLISION::CollisionLineToTree( m_pLandEffTree, vStart, vEnd, pOBJ, vEnd, vNor );
	if( pOBJ )
	{
		return TRUE;
	}
	return FALSE;
}

// �浹üũ
BOOL DxSetLandMan::IsCollisionLine_NoneConst( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vNor, EMCOLLMODE emCullMode )
{
	BOOL bColl(FALSE);
	DxPiece* pPIECE(NULL);
	char szCollTex[MAX_PATH] = {0};
	if( GetCollisionMap()->IsCollision_NoneConst( vStart, vEnd, &vNor, TRUE, NULL, szCollTex, emCullMode ) )
	{
		bColl = TRUE;
	}

	D3DXVECTOR3 vColl(0.f,0.f,0.f);
	GetPieceManager()->IsCollisionLine2( vStart, vEnd, vNor, vColl, &pPIECE, emCullMode );			// PieceManager Picking
	if( pPIECE )
	{
		bColl = TRUE;
		vEnd = vColl;
	}

	return bColl;
}

// �浹üũ
BOOL DxSetLandMan::IsCollisionLineExceptPiece( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vNor, EMCOLLMODE emCullMode, DxPiece* pExceptPiece )
{
	BOOL bColl(FALSE);
	DxPiece* pPIECE(pExceptPiece);
	char szCollTex[MAX_PATH] = {0};
	if( GetCollisionMap()->IsCollision_NoneConst( vStart, vEnd, &vNor, TRUE, NULL, szCollTex, emCullMode ) )
	{
		bColl = TRUE;
	}
	return bColl;

	// �����Ŵ� �浹ó�� ���ϵ��� �Ѵ�. pPIECE(pExceptPiece);
	D3DXVECTOR3 vColl(0.f,0.f,0.f);
	GetPieceManager()->IsCollisionLine2( vStart, vEnd, vNor, vColl, &pPIECE, emCullMode );			// PieceManager Picking
	if( pPIECE )
	{
		bColl = TRUE;
		vEnd = vColl;
	}

	return bColl;
}

void DxSetLandMan::LoadSet ( LPDIRECT3DDEVICEQ pd3dDevice, const char *pFileName, const SUBPATH* pPath )
{
	CDebugSet::ClearFile ( "LoadSet.log" );

	NSWaterMainPSF::CleanUp();
	m_scpShadowLightData->CleanUp();

	BOOL	bResult = FALSE;

    m_strLightMapName = _T("");
    m_fLightMapDetail = 0.25f;
	
	TSTRING strPath;
	strPath = pPath->AppRoot().c_str();
	strPath += pPath->ObjFileRoot().c_str();

	TSTRING strFullName;
	strFullName = strPath;
	strFullName += pFileName;

	sc::SerialFile	SFile;
	BOOL bOpened = SFile.OpenFile ( FOT_READ, strFullName.c_str() );
	if ( !bOpened )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. error File Open..." );
		return;
	}

	char szFileType[FILETYPESIZE] = "";
	DWORD dwFileVer;
	SFile.GetFileType( szFileType, FILETYPESIZE, dwFileVer );

	if ( dwFileVer > DxSetLandMan::VERSION )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. ERROR �� ���� ����(PSF) �� ���� ���� �������� ������ �����Դϴ�." );
		return;
	}

	//	Note : Light
	//
	CDebugSet::ToFile ( "LoadSet.log", "-. [LightMan Loading] - Start" );
	m_pLightMan->Load ( SFile, dwFileVer, TRUE );
	DxLightMan::SetInstance ( m_pLightMan );
	CDebugSet::ToFile ( "LoadSet.log", "-. [LightMan Loading] - End" );

	//	Note : FrameMesh
	//
	m_bPrevSaveFile_120712 = FALSE;
	CDebugSet::ToFile ( "LoadSet.log", "-. [FrameMesh Loading] - Start" );
	if ( dwFileVer >= 0x00000030 )
	{
		switch ( dwFileVer )
		{
			//case 0x00000034:
			//	m_bPrevSaveFile_120712 = FALSE;	// 0x00000034 �������� �ö󰡸� TRUE �� �ʿ� ����.
			//	break;

            case 0x00000033:
				m_bPrevSaveFile_120712 = TRUE;	// 0x00000034 �������� �ö󰡸� TRUE �� �ʿ� ����. 
				if ( !m_pFrameMesh->LoadSet_VER103( SFile, pd3dDevice, strPath ) )
				{
					CDebugSet::ToFile ( "LoadSet.log", "-. ������ �޽��ε忡 �����Ͽ����ϴ�." );
					return;
				}
				m_CollisionMap.CleanUp();
				m_CollisionMap.Import( TRUE );
				break;

            case 0x00000032:
				m_bPrevSaveFile_120712 = TRUE;
				if ( !m_pFrameMesh->LoadSet_VER102( SFile, pd3dDevice, strPath ) )
				{
					CDebugSet::ToFile ( "LoadSet.log", "-. ������ �޽��ε忡 �����Ͽ����ϴ�." );
					return;
				}
				m_CollisionMap.CleanUp();
				m_CollisionMap.Import( TRUE );
				break;

			case 0x00000031:
				m_bPrevSaveFile_120712 = TRUE;
				if ( !m_pFrameMesh->LoadSet_VER101( SFile, pd3dDevice, strPath ) )
				{
					CDebugSet::ToFile ( "LoadSet.log", "-. ������ �޽��ε忡 �����Ͽ����ϴ�." );
					return;
				}
				m_CollisionMap.CleanUp();
				m_CollisionMap.Import( TRUE );
				break;

            case 0x00000030:
				m_bPrevSaveFile_120712 = TRUE;
				if ( !m_pFrameMesh->LoadSet_VER100( SFile, pd3dDevice, strPath ) )
				{
					CDebugSet::ToFile ( "LoadSet.log", "-. ������ �޽��ε忡 �����Ͽ����ϴ�." );
					return;
				}
				m_CollisionMap.CleanUp();
				m_CollisionMap.Import( TRUE );
				break;
		}
	}
	else
	{
		m_bPrevSaveFile_120712 = TRUE;
		if ( !m_pFrameMesh->LoadSet_Prev( SFile, pd3dDevice, strPath ) )
		{
			CDebugSet::ToFile ( "LoadSet.log", "-. ������ �޽��ε忡 �����Ͽ����ϴ�." );
			return;
		}
		m_CollisionMap.CleanUp();
		m_CollisionMap.Import( TRUE );
	}
	CDebugSet::ToFile ( "LoadSet.log", "-. [FrameMesh Loading] - End" );

	SFile >> bResult;
	if ( bResult )
	{
		int StrLength = 0;
		char* szNaviName = NULL;
		
		SFile >> StrLength;
		szNaviName = new char [ StrLength ];
		SFile.ReadBuffer ( szNaviName, StrLength );

		DxFrame *pNaviFrame = NULL;
        pNaviFrame = m_pFrameMesh->FindFrame ( szNaviName );
		if ( pNaviFrame )	SetNaviFrame ( pNaviFrame );
		else
		{
			CDebugSet::ToFile ( "LoadSet.log", "   [%s]...Not Found Navigation Mesh..", szNaviName );
		}

		SAFE_DELETE_ARRAY ( szNaviName );
	}

	//	Note : ViewPort
	//
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [CameraAniMan Loading] - Start" );
		m_CameraAniMan.LoadSet ( SFile, m_pFrameMesh );
		CDebugSet::ToFile ( "LoadSet.log", "-. [CameraAniMan Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxSoundMan Loading] - Start" );
	
		//	Note : Sound Log
#ifdef	_DEBUG
		CDebugSet::ToFile ( "UseSoundList.txt", "" );
		CDebugSet::ToFile ( "UseSoundList.txt", "[-------------------------------------------------------------]" );
		CDebugSet::ToFile ( "UseSoundList.txt", "[!.%s]", pFileName );	
#endif	//	_DEBUG

		DxSoundMan::GetInstance().LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxSoundMan Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [StaticSoundMan Loading] - Start" );
		m_StaticSoundMan.LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [StaticSoundMan Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDEFF Loading] - Start" );

		bResult = TRUE;
		while ( bResult )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( SFile );
			AddLandEff ( pLandEff );
			
			SFile >> bResult;
		}
	
		ReverseListLandEff();
		BuildSingleEffTree ();

		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDEFF Loading] - End" );
	}

	//	Note : �� ID �б�.
	//
	SFile >> bResult;
	if ( bResult )
	{
		SFile >> m_MapID.dwID;
	}

	SFile >> bResult;
	if ( bResult )
	{
		SFile.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDSKINOBJ Loading] - Start" );

		bResult = TRUE;
		while ( bResult )
		{
			PLANDSKINOBJ pLandSkinObj = new DXLANDSKINOBJ( pd3dDevice );
			pLandSkinObj->Load ( SFile, pd3dDevice );
			AddLandSkinObj ( pLandSkinObj );
			
			SFile >> bResult;
		}
	
		BuildSkinObjTree ();

		CDebugSet::ToFile ( "LoadSet.log", "-. [DXLANDSKINOBJ Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		m_LandGateMan.Load ( SFile );
	}

	//	WeatherMan
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [WeatherMan Loading] - Start" );
		m_WeatherMan.LoadSet ( SFile, pd3dDevice );
		CDebugSet::ToFile ( "LoadSet.log", "-. [WeatherMan Loading] - End" );
	}

	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxBgmSound Loading] - Start" );
		m_BGMDATA.LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxBgmSound Loading] - End" );
	}

	//	DxFogMan
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [FOG_PROPERTY Loading] - Start" );
		m_FOG_PROPERTY.LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [FOG_PROPERTY Loading] - End" );
	}
	else 
	{
		goto _RETURN;
	}

	//	Sky
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [SKY_PROPERTY Loading] - Start" );
		m_SKY_PROPERTY.LoadSet ( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [SKY_PROPERTY Loading] - End" );
	}
	else 
	{
		goto _RETURN;
	}

	//	TextureSetDXT
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [TextureSetDXT Loading] - Start" );
		CTextureSetDXT::GetInstance().LoadSet( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [TextureSetDXT Loading] - End" );
	}
	else 
	{
		goto _RETURN;
	}

	//	DxPieceManager
	SFile >> bResult;
	if ( bResult )
	{
        // ������ �ִ� ������ �����.
        DxPieceContainer::GetInstancePSF().CleanUp();

		CDebugSet::ToFile ( "LoadSet.log", "-. [DxPieceManager Loading] - Start" );
		if ( 0x30 >= dwFileVer )
		{
			m_pPieceManager->LoadPSF_100( pd3dDevice, SFile, m_pFrameMesh );	// 
		}
		else
		{
			m_pPieceManager->LoadPSF( pd3dDevice, SFile, m_pFrameMesh );	// 0x31 �Ǹ鼭 �ٲ����.
		}
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxPieceManager Loading] - End" );
	}
	else 
	{
		goto _RETURN;
	}

    //	LightMap
	SFile >> bResult;
	if ( bResult )
	{
		SFile >> m_strLightMapName;
        SFile >> m_fLightMapDetail;
	}
	else 
	{
		goto _RETURN;
	}

    //	Note : DxPieceView, DxSpeedTreeViewer
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxPieceView Loading] - Start" );
		m_pPieceView->Load( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxPieceView Loading] - End" );
	}
	else 
	{
		goto _RETURN;
	}

    //	DxSkyBoxMan
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxSkyBoxMan Loading] - Start" );
		DxSkyBoxMan::GetInstance().Load( pd3dDevice, SFile, TRUE );
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxSkyBoxMan Loading] - End" );
	}
	else 
	{
		goto _RETURN;
	}

	//	Field Water Surf
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [Field Water Surf Loading] - Start" );
		NSWaterMainPSF::LoadSurf( SFile, pd3dDevice );
		CDebugSet::ToFile ( "LoadSet.log", "-. [Field Water Surf Loading] - End" );
	}
	else 
	{
		goto _RETURN;
	}

	// X-File LightMap
	SFile >> bResult;
	if ( bResult )
	{
		TSTRING strTemp;
		SFile >> strTemp;
		// �ƹ��ǹ� ���� �۾��� ��.

		// 0x00000033 ���� ���õǾ�����, �ֽ����� ������ X-File LightMap �� ������� ���� �����̱� ������,
		// ���� ���ٸ� �ֽ������� Save �� �����̶�� �ν��ϵ��� �Ѵ�.
		if ( strTemp.empty() )
		{
			m_bPrevSaveFile_120712 = FALSE;
		}
	}
	else 
	{
		goto _RETURN;
	}

	//	Prefab
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [Prefab Loading] - Start" );
		m_scpPrefabMan->Load( pd3dDevice, SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [Prefab Loading] - End" );
	}
	else return;

	//	NSShadowLight
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [NSShadowLight Loading] - Start" );
		m_scpShadowLightData->Load( SFile );
		CDebugSet::ToFile ( "LoadSet.log", "-. [NSShadowLight Loading] - End" );
	}
	else return;

	return;

_RETURN:

	// ������ �� ���� psf ��� �̸� �˷��ش�.
	if ( m_bPrevSaveFile_120712 )
	{
		MessageBox ( NULL, "���� Tool���� ������ psf �����Դϴ�. ������ �� �����ϴ�.", "ERROR", MB_OK );
	}
}

void DxSetLandMan::SaveSet ( LPDIRECT3DDEVICEQ pd3dDevice, const char *pFileName, boost::function<void (const TCHAR*)> pArchive )
{
	if ( m_bPrevSaveFile_120712 )
	{
		MessageBox ( NULL, "���� Tool���� ������ psf �����Դϴ�. ������ �� �����ϴ�.", "ERROR", MB_OK );
		return;
	}

    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return ;
    }

    sc::SerialFile& SFile = *spFile;

	char szFileType[FILETYPESIZE] = "";
	SFile.SetFileType ( szFileType, DxSetLandMan::VERSION );

	BOOL bOpened = SFile.OpenFile ( FOT_WRITE, pFileName );
	if ( !bOpened )
	{
		MessageBox ( NULL, "File Creation", "ERROR", MB_OK );
	}

	//	Note : Light
	//
	m_pLightMan->Save ( SFile );

	//	Note : FrameMesh
	//
	m_pFrameMesh->SaveSet ( SFile );

	//	Note : ������̼� �޽�.
	//
	if ( m_pNaviFrame )		SFile << BOOL ( TRUE );
	else					SFile << BOOL ( FALSE );

	if( m_pNaviFrame && m_pNaviFrame->szName )
	{
		int	StrLength = (int)strlen(m_pNaviFrame->szName)+1;
		SFile << StrLength;
		SFile.WriteBuffer ( m_pNaviFrame->szName, StrLength );
	}

	//	Note : ViewPort
	//
	SFile << BOOL ( TRUE );
	m_CameraAniMan.SaveSet ( SFile );

	SFile << BOOL ( TRUE );
	DxSoundMan::GetInstance().SaveSet ( SFile );

	SFile << BOOL ( TRUE );
	m_StaticSoundMan.SaveSet ( SFile );

	if ( m_pLandEffList )	SFile << BOOL ( TRUE );
	else					SFile << BOOL ( FALSE );

	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->Save ( SFile );

		pCur = pCur->m_pNext;
		if ( pCur )		SFile << BOOL ( TRUE );
		else			SFile << BOOL ( FALSE );
	}

	//	Note : �� ID ����.
	//
	SFile << BOOL ( TRUE );
	SFile << m_MapID.dwID;

	SFile << BOOL ( TRUE );
	SFile.WriteBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );


	//	Note : Land �� ���Ե� SkinObject
	//
	if ( m_pLandSkinObjList )	SFile << BOOL ( TRUE );
	else						SFile << BOOL ( FALSE );

	PLANDSKINOBJ pLandSkinObj = m_pLandSkinObjList;
	while ( pLandSkinObj )
	{
		pLandSkinObj->Save ( SFile );
			
		pLandSkinObj = pLandSkinObj->m_pNext;
		if ( pLandSkinObj )		SFile << BOOL ( TRUE );
		else					SFile << BOOL ( FALSE );
	}

	SFile << BOOL ( TRUE );
	m_LandGateMan.Save ( SFile );


	//	Note : Weather
	//
	SFile << BOOL ( TRUE );
	m_WeatherMan.SaveSet ( SFile );

	//	Note : BGM ���� �̸� [ 1���� ���� ]
	SFile << BOOL ( TRUE );
	m_BGMDATA.SaveSet ( SFile );	

	//	Note : Fog
	SFile << BOOL ( TRUE );
	m_FOG_PROPERTY.SaveSet ( SFile );

	//	Note : Sky
	SFile << BOOL ( TRUE );
	m_SKY_PROPERTY.SaveSet ( SFile );

	//	Note : TextureSetDXT
	SFile << BOOL ( TRUE );
	CTextureSetDXT::GetInstance().SaveSet( SFile );

	//	Note : DxPieceManager
	SFile << BOOL ( TRUE );
	m_pPieceManager->SavePSF( pd3dDevice, SFile, m_pFrameMesh );

    //	Note : LightMap
    SFile << BOOL ( TRUE );
    SFile << m_strLightMapName;
    SFile << m_fLightMapDetail;

    //	Note : DxPieceView
	SFile << BOOL ( TRUE );
	m_pPieceView->Save( SFile );

    // Note : DxSkyBoxMan
	SFile << BOOL ( TRUE );
	DxSkyBoxMan::GetInstance().Save( SFile );

	//	Note : Field Water surf
	SFile << BOOL ( TRUE );
	NSWaterMainPSF::SaveSurf( SFile );

	// X-File LightMap
	TSTRING strTemp(_T(""));
	SFile << BOOL ( TRUE );
	SFile << strTemp;

	//	Note : Prefab
	SFile << BOOL ( TRUE );
	m_scpPrefabMan->Save( SFile );

	//	Note : NSShadowLight
	SFile << BOOL ( TRUE );
	m_scpShadowLightData->Save( SFile );

	//	Note : ������ �߰��� �����͸� ���ؼ� ��ŷ ------
	//
	SFile << BOOL ( FALSE );

    SFile.CloseFile();
}

void DxSetLandMan::CheckPieceForRadiosity( VEC_TSTRING& vecErrorPiece )
{
	m_pPieceManager->CheckPieceForRadiosity( vecErrorPiece );
}

void DxSetLandMan::LoadSetPrefab( const TCHAR	*pFileName, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL bResult;
	sc::SerialFile	SFile;
	BOOL bOpened = SFile.OpenFile( FOT_READ, pFileName );
	if ( !bOpened )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. error File Open..." );
	}

	TCHAR szFileType[FILETYPESIZE] = _T("");
	DWORD dwFileVer;
	SFile.GetFileType( szFileType, FILETYPESIZE, dwFileVer );

	if ( dwFileVer > VERSION_PREFAB )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. ERROR �� ���� ����(prefab) �� ���� ���� �������� ������ �����Դϴ�." );
		return;
	}

	SFile >> bResult;
	if ( bResult )
	{
		SFile >> m_strFileName;
	}

	//	DxPieceManagerPSF
	SFile >> bResult;
	if ( bResult )
	{
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxPieceManagerPSF Loading] - Start" );
		m_pPieceManager->LoadPSF( pd3dDevice, SFile, NULL );
		CDebugSet::ToFile ( "LoadSet.log", "-. [DxPieceManagerPSF Loading] - End" );
	}
	else return;
}

void DxSetLandMan::SaveSetPrefab( const TCHAR	*pFileName, LPDIRECT3DDEVICEQ pd3dDevice )
{
	sc::SerialFile	SFile;

	TCHAR szFileType[FILETYPESIZE] = _T("");
	SFile.SetFileType ( szFileType, VERSION_PREFAB );

	BOOL bOpened = SFile.OpenFile( FOT_WRITE, pFileName );
	if ( !bOpened )
	{
		MessageBox ( NULL, _T("File Creation"), _T("ERROR"), MB_OK );
	}

	// Ȯ���ڸ� ������ ���� �̸��� �����ش�
	sc::string::getFullFileName( TSTRING( pFileName ), m_strFileName );

	SFile << BOOL ( TRUE );
	SFile << m_strFileName;

	//	Note : DxPieceManagerPSF
	SFile << BOOL ( TRUE );
	m_pPieceManager->SavePSF( pd3dDevice, SFile, m_pFrameMesh );
}

void DxSetLandMan::ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece )
{
	m_pPieceManager->ExportMAPNAMEPIECE( pd3dDevice, mapNamePiece );
	m_scpPrefabMan->ExportMAPNAMEPIECE( pd3dDevice, mapNamePiece );
}

void DxSetLandMan::ChangeNamePIE15()
{
	// ���� ���ϴ� ���� Ǯ������.
	m_bPrevSaveFile_120712 = FALSE;

	m_pPieceManager->ChangeNamePIE15();
}

void DxSetLandMan::SaveSetNavi( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR *pFileName, const TCHAR* szDebugFullDir )
{
	if ( !m_pNaviFrame )
		return;

	if ( CreateNaviMesh ( pd3dDevice ) )
	{
		if( m_NaviMesh.CheckIntegrity ( szDebugFullDir ) )
		{
			m_NaviMesh.SaveFile( pFileName );
		}
		else
		{
			AfxMessageBox(_T("���Ἲ�˻翡�� ������ �߻��Ͽ����ϴ�."));
		}
	}
	else
	{
		AfxMessageBox(_T("NavigationMesh ���� ����"));
	}
}