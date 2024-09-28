#include "pch.h"

#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"

#include "../../VisualMaterialLib/Manager/NSVMRender.h"

#include "../NaviMesh/NavigationMesh.h"
#include "../DxEffect/TexEff/NSTexEffVisualMaterial.h"
#include "../DxEffect/Single/DxEffSingleMan.h"
#include "../DxEffect/Frame/DxEffectFrame.h"
#include "../DxEffect/Frame/DxEffectMan.h"
#include "../DxEffect/Mesh/DxStaticMeshLightMap.h"
#include "../DxEffect/NSEffectThread.h"
#include "../DxTools/PostProcessing/NSLightShafts.h"
#include "../DxTools/Material/DxStaticMaterialMeshLightMap.h"
#include "../DxTools/Material/DxStaticMaterialMesh.h"
#include "../DxTools/Material/NSShadowRenderForMaterial.h"
#include "../DxTools/Material/NSMaterialManager.h"
#include "../DxTools/Occlusion/NSOcclusionQueryFastRender.h"
#include "../DxTools/Occlusion/NSOcclusionCulling.h"
#include "../DxTools/Occlusion/NSOcclusionOctree.h"
#include "../DxTools/Deffered/NSDeffered.h"
#include "../DxTools/Texture/NSTexture16Bit.h"
#include "../DxTools/Texture/NSTexCompress.h"
#include "../DxTools/CubeMap/NSCubeMapUtil.h"
#include "../DxTools/SkyBox/DxSkyBoxMan.h"
#include "../DxTools/Light/NSShadowLight.h"
#include "../DxTools/DxBackUpRendTarget.h"
#include "../DxTools/DxRenderStates.h"
#include "../DxTools/DxEnvironment.h"
#include "../DxTools/DxSurfaceTex.h"
#include "../DxTools/DxShadowMap.h"
#include "../DxTools/RENDERPARAM.h"
#include "../DxSound/StaticSoundMan.h"
#include "../DxSound/BgmSound.h"
#include "../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../Common/SUBPATH.h"
#include "../DxLand/DxPiece/NSOctreeRenderList.h"
#include "../DxLand/DxPiece/DxPieceManagerWLD.h"
#include "../DxLand/DxPiece/DxPieceManagerPSF.h"
#include "../DxLand/DxPiece/NSPieceQuickSort.h"
#include "../DxLand/EntityRenderList.h"
#include "../DxLand/NSLandThread.h"
#include "../Water/NSWaterMain.h"
#include "../GlobalParam.h"
#include "./LightMap/NSLightMapFX.h"
#include "./LightMap/NSLightMap.h"
#include "./LightMap/NSRadiosity.h"
#include "./DxSetLandMan.h"

#include "DxLandMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bWIREFRAME_VIEW;

char DxLandMan::m_szPath[MAX_PATH];

namespace	ExportProgress
{
	int		EndPos;
	int		CurPos;
	BOOL	bValid = FALSE;
	char	szState[MAX_PATH] = "";

	void GetStateString ( char *szDesString )
	{
		if ( bValid )
		{
			size_t nStrLen = strlen(szState)+1;
			StringCchCopy( szDesString, nStrLen, szState );
			bValid = FALSE;
		}
		else
		{
			szDesString[0] = NULL;
		}
	}

	void SetStateString ( const char *szString )
	{
		while ( bValid )
		{
			//	Note : ������ ��忡�� while()���� ��������
			//		������ ���ѷ����� ������ ����. ( ������ �� �ڵ忡�� �߻�. )
			Sleep( 1 );
		}

        //if ( true == bTime )
	    {
		    time_t ltime;
		    struct tm *today;
		    time( &ltime );
		    today = localtime( &ltime );
		    char szTime[MAX_PATH] = {0};
		    ::strftime( szTime, MAX_PATH, _T("[%H:%M:%S] "), today );

            TSTRING strName;
            strName = szTime;
            strName += szString;
            StringCchCopy( szState, MAX_PATH, strName.c_str() );
	    }

		//StringCchCopy( szState, MAX_PATH, szString );
		bValid = TRUE;
	}
}

DxLandMan::DxLandMan ()
	//m_pStaticMesh(NULL),
	: m_pd3dDevice(NULL)
	, m_pPieceObjTree(NULL),
	m_pPieceObjHead(NULL),
	m_pAniManHead(NULL),
	m_pAniManTree(NULL),
	m_dwEffectCount(0),
	m_pEffectList(NULL),
	m_pEffectTree(NULL),
	m_dwEffectCount_AFTER(0),
	m_pEffectList_AFTER(NULL),
	m_pEffectTree_AFTER(NULL),
	m_dwEffectCount_AFTER_1(0),
	m_pEffectList_AFTER_1(NULL),
	m_pEffectTree_AFTER_1(NULL),
	m_dwCount_Weather(0),
	m_pList_Weather(NULL),
	m_pTree_Weather(NULL),
	m_pEffectFrameList(NULL),
	m_pStaticSoundMan(NULL),
	m_dwNumLandEff(0),
	m_pLandEffList(NULL),
	m_pLandEffTree(NULL),

	m_bCamCollision(FALSE)

	, m_bUsedMaterialSystem(TRUE)
	, m_bUsedMaterialSystemPS_3_0(TRUE)
	, m_dwRAD_Pass(3)
	, m_fRAD_Reflectance(1.f)
	, m_fRAD_FogBlend(0.45f)

	, m_scpShadowLightData(new ShadowLightData)
	, m_spPieceManager(new DxPieceManagerWLD)
	, m_spLandSkinManager(new DxLandSkinManager)
{
	m_strNameWLD = _T("");

    m_pLightMan = new DxLightMan;

	for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
	{
		m_spEntityRenderListSwap[i] = boost::shared_ptr<EntityRenderListSwap>(new EntityRenderListSwap());
	}
}

DxLandMan::~DxLandMan()
{
	CleanUp();

    SAFE_DELETE( m_pLightMan );
}

void DxLandMan::ActiveMapIn ()
{
	DxLightMan::SetInstance( m_pLightMan );
	DxWeatherMan::SetInstance ( &m_WeatherMan );

	// BGM �� �ٸ� ��츸 Stop �� Play �� �ϵ��� �Ѵ�.
	if ( m_BGMDATA.strFileName != DxBgmSound::GetInstance().GetFile() )
	{
		DxBgmSound::GetInstance().ForceStop ();
		DxBgmSound::GetInstance().ClearFile ();

		DxBgmSound::GetInstance().SetFile ( m_BGMDATA.strFileName );
		DxBgmSound::GetInstance().Play();
	}

	DxFogMan::GetInstance().SetProperty ( m_FOG_PROPERTY );
	DxSkyMan::GetInstance().SetProperty ( m_SKY_PROPERTY );

	DxEnvironment::GetInstance().ChangeMapInitData();

	NSShadowLight::SetActive( m_scpShadowLightData.get() );

	m_WeatherMan.SetMapID( m_MapID );
}

HRESULT DxLandMan::ActiveMap( boost::shared_ptr<DxLandMan>& spDxLandManMe )
{
	ActiveMapIn ();

	if ( spDxLandManMe && spDxLandManMe.get() == this )
	{
		// ���ڽ��� shared_ptr �� ����.
		DxEffSingleMan::GetInstance().SetLandMan ( spDxLandManMe );	
	}
	else
	{
		DxEffSingleMan::GetInstance().SetLandMan ( boost::shared_ptr<DxLandMan>() );
	}

	return S_OK;
}

HRESULT DxLandMan::ActiveMap_Tool( LPDIRECT3DDEVICEQ pd3dDevice, boost::shared_ptr<DxLandMan>& spDxLandManMe )
{
	ActiveMap ( spDxLandManMe );

	// Material �� ���Ѵ�.
	Active_MATERIAL_DATA( pd3dDevice, FALSE );

	return S_OK;
}

// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
void DxLandMan::SetNSLandThread()
{
	// Thread�� ��� ���� �ɶ����� while �� ���ư���.
	NSSkinAniThread::DataCleanUp();
	NSEffectThread::DataCleanUp();
	NSLandThread::DataCleanUp();

	// ���� Effect ��
	{
		DxEffectBase *pEffectCur;
		pEffectCur = m_pEffectList;
		while ( pEffectCur != NULL )
		{
			pEffectCur->SetNSLandThread();
			pEffectCur = pEffectCur->pEffectNext;
		}
	}

	// Field ��
	if ( m_spStaticMeshLightMap.get() )
	{
		m_spStaticMeshLightMap->SetNSLandThread();
	}
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->SetNSLandThread();
	}

	// LandSkin ��
	NSLandThread::SetLandSkinManager( m_spLandSkinManager );

	// Piece ��
	m_spPieceManager->SetNSLandThread( m_pd3dDevice );
}

void DxLandMan::BuildSingleEffTree ()
{
	COLLISION::MakeAABBTree ( m_pLandEffTree, m_pLandEffList );
}

PLANDEFF DxLandMan::FindLandEff ( char* szName )
{
	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

void DxLandMan::AddLandEff ( PLANDEFF pLandEff )
{
	m_dwNumLandEff++;
	pLandEff->m_pNext = m_pLandEffList;
	m_pLandEffList = pLandEff;
}

void DxLandMan::DelLandEff ( PLANDEFF pLandEff )
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

HRESULT	DxLandMan::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;

	OctreeDebugInfo::EndNodeCount = 0;
	OctreeDebugInfo::AmountDxFrame = 0;

	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		pCur->InitDeviceObjects ( pd3dDevice );

        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		pCur = pCur->m_pNext;
	}

	m_pLightMan->InitDeviceObjects ( pd3dDevice );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	HRESULT hr = m_DxOctree.InitDeviceObjects ( pd3dDevice );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    return hr;
}

HRESULT DxLandMan::RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	DxEffectBase *pEffectCur;

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectFrameList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER_1;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pList_Weather;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->RestoreDeviceObjects ( pd3dDevice );

			pCur = pCur->m_pNext;
		}
	}

	m_spLandSkinManager->OnResetDevice( pd3dDevice );
	m_spPieceManager->OnResetDevice( pd3dDevice );

	//////////////////////////////////////////////////////////////////////////
	// �ܺο��� SetWorldBattleFlag( const TCHAR* pWorldBattleFlag ) ������ ����� Resize ���� ���� Flag �� ����� ���´�.
	//////////////////////////////////////////////////////////////////////////

	return S_OK;
}

HRESULT DxLandMan::InvalidateDeviceObjects()
{
	DxEffectBase *pEffectCur;

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectFrameList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER_1;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pList_Weather;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	m_DxOctree.InvalidateDeviceObjects ();

	{
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->InvalidateDeviceObjects ();

			pCur = pCur->m_pNext;
		}
	}

	m_spLandSkinManager->OnLostDevice();
	m_spPieceManager->OnLostDevice();

	return S_OK;
}

HRESULT	DxLandMan::DeleteDeviceObjects()
{
	DxEffectBase *pEffectCur;

	//	Note : Frame ���� ȿ����.
	//
	pEffectCur = m_pEffectFrameList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	//	Note : Frame ������ ȿ����.
	//
	pEffectCur = m_pEffectList;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER_1;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pList_Weather;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	m_DxOctree.DeleteDeviceObjects ();

	PLANDEFF pCur = m_pLandEffList;
	while ( pCur )
	{
		pCur->DeleteDeviceObjects ();
	
		pCur = pCur->m_pNext;
	}

	m_pLightMan->DeleteDeviceObjects ();
	//DxBgmSound::GetInstance().ForceStop ();

	CleanUp();

	return S_OK;
}

void DxLandMan::SetFogProperty ( FOG_PROPERTY& Property )
{
	m_FOG_PROPERTY = Property;
	DxFogMan::GetInstance().SetProperty ( m_FOG_PROPERTY );
}

void DxLandMan::SetSkyProperty ( SKY_PROPERTY& Property )
{
	m_SKY_PROPERTY = Property;
	DxSkyMan::GetInstance().SetProperty ( m_SKY_PROPERTY );
}

void DxLandMan::AddEffectFrameList ( DxEffectBase* pEffect )
{
	pEffect->pEffectNext = m_pEffectFrameList;
	m_pEffectFrameList = pEffect;
}

HRESULT DxLandMan::EffectAdaptToFrame ( DxOctree &OcNode, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
							PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc, DxTextureEffMan* pTexEff )
{
	HRESULT hr S_OK;

	DxFrame	*pDxFrameResult = NULL;

	if(OcNode.IsSubDivision())
	{
		for ( int i=0; i<8; i++ )
		{
			DxOctree *pOctree = OcNode.GetOctreeNode(i);
			if ( pOctree )
			{
				hr = EffectAdaptToFrame ( *pOctree, TypeID, szFrame, pd3dDevice, pProperty, dwSize, dwVer, pAffineParts, pEffectSrc, pTexEff );
				if ( FAILED(hr) )	return E_FAIL;
			}
		}
	}
	else
	{	
		DxFrame* pDxFrameHead = OcNode.GetDxFrameHead ();
		if ( !pDxFrameHead )		return S_OK;

		DxFrame *pDxFrame = pDxFrameHead->FindFrame ( szFrame );
		if ( !pDxFrame )			return S_OK;

		DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance ( TypeID );
		if ( !pEffect ) return NULL;

		//	Note : Property, Affine  ���� ����.
		//
		if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
		if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

		//	Note : �뷮�� ���۸� �����ϱ� ���� �޼ҵ�.
		//
		if ( pEffectSrc )
		{
			pEffect->CloneData ( pEffectSrc, pd3dDevice, pTexEff );
		}

		hr = pEffect->Create ( pd3dDevice );
		if ( FAILED(hr) )
		{
			SAFE_DELETE(pEffect);
			return E_FAIL;
		}

		pEffect->AdaptToDxFrame ( pDxFrame, pd3dDevice );

		AddEffectFrameList ( pEffect );	// DxLandMan���� ����Ʈ ���������� ���.
	}

	return S_OK;
}

HRESULT DxLandMan::EffectAdaptToList ( DxFrame *pFrameRoot, DWORD TypeID, char* szFrame, LPDIRECT3DDEVICEQ pd3dDevice,
							PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts, DxEffectBase* pEffectSrc, 
                            DxTextureEffMan* pTexEff, const TSTRING& strLightMapName, VEC_LIGHTMAPUV_MAIN& vecLightMapUV_MAIN )
{
	HRESULT hr;

	DxFrame *pDxFrame = pFrameRoot->FindFrame ( szFrame );

	DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance ( TypeID );
	if ( !pEffect ) return NULL;

	hr = pEffect->AdaptToEffList ( pDxFrame, pd3dDevice );

	if ( hr == S_FALSE )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "�ܵ� ȿ��(%s)�� Adapt �޼ҵ尡 �������� �ʽ��ϴ�.", pEffect->GetName() );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		return E_FAIL;
	}

	if ( FAILED(hr) )
	{
		char szMsg[256] = "";
		StringCchPrintf( szMsg, 256, "�ܵ� ȿ��(%s)�� Adapt ���� ������ �߻��Ͽ����ϴ�.", pEffect->GetName() );
		MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		return E_FAIL;
	}

	//	Note : Property, Affine  ���� ����.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )		pEffect->SetAffineValue ( pAffineParts );

	//	Note : �뷮�� ���۸� �����ϱ� ���� �޼ҵ�.
	//
	if ( pEffectSrc )
    {
        pEffect->CloneData ( pEffectSrc, pd3dDevice, pTexEff );
        pEffect->CloneDataLightMap( pEffectSrc, pd3dDevice, pTexEff, strLightMapName, vecLightMapUV_MAIN );
    }

	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);
		return E_FAIL;
	}

	if( pEffect->GetFlag()&(_EFF_REPLACE_AFTER|_EFF_SINGLE_AFTER_0) )
	{
		m_dwEffectCount_AFTER++;
		pEffect->pEffectNext = m_pEffectList_AFTER;
		m_pEffectList_AFTER = pEffect;
	}
	else if( pEffect->GetFlag()&(_EFF_REPLACE_AFTER_1|_EFF_SINGLE_AFTER_1) )
	{
		m_dwEffectCount_AFTER_1++;
		pEffect->pEffectNext = m_pEffectList_AFTER_1;
		m_pEffectList_AFTER_1 = pEffect;
	}
	else if( pEffect->GetFlag()&_EFF_SINGLE_AFTER_2 )
	{
		m_dwCount_Weather++;
		pEffect->pEffectNext = m_pList_Weather;
		m_pList_Weather = pEffect;
	}
	else
	{
		m_dwEffectCount++;
		if( pEffect->GetTypeID() == DEF_EFFECT_SHADOW )
		{
			if ( !m_pEffectList )					// ����ִٸ� �ʱ�ȭ��. .
			{
				m_pEffectList = pEffect;
				return S_OK;
			}

			DxEffectBase* pCur = m_pEffectList;
			while ( pCur )							// ����
			{
				if ( !(pCur->pEffectNext) )
				{
					pCur->pEffectNext = pEffect;	// �׸��ڴ� ������ �������� �д�.
					return S_OK;
				}
				pCur = pCur->pEffectNext;
			}
		}
		else
		{
			pEffect->pEffectNext = m_pEffectList;
			m_pEffectList = pEffect;
		}
	}

	return S_OK;
}

HRESULT	DxLandMan::Import_Ran_1_0( int MaxTriangles, 
									int MaxSubdivisions, 
									DxSetLandMan *pSetLandMan, 
									LPDIRECT3DDEVICEQ pd3dDevice, 
									char *szDebugFullDir, 
									BOOL bCreatLightMap,
									const TSTRING& strLightMapName,
									float fLightMapDetail,
									const SUBPATH* pPath )
{	
	//HRESULT hr;

	//DxFrameMesh *pFrameMesh = pSetLandMan->GetFrameMesh();
	//float fUNITTIME = pFrameMesh->GetUnitTime();
	//DxFrame *pDxFrameRoot = pFrameMesh->GetFrameRoot();
	//if (!pDxFrameRoot) return E_FAIL;


	//OctreeDebugInfo::EndNodeCount = 0;
	//OctreeDebugInfo::AmountDxFrame = 0;
	//
	////	Note : �� ����Ÿ ��� ����.
	////
 //   ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = 1;
	//ExportProgress::SetStateString ( "���� ����Ÿ ����..." );
	//CleanUp ();

	////	Note : �� ��� ����.
	////
	//m_MapID = pSetLandMan->GetMapID();
	//memcpy ( m_szMapName, pSetLandMan->GetMapName(), sizeof(char)*MAXLANDNAME );

	//////	Note : ���� Obj ��Ʈ�� �������� ���� ����, EndPos�� DxOctree Import �߰��κп��� ����
	//////
 ////   ExportProgress::CurPos = 0;
	////ExportProgress::EndPos = 1;
	////ExportProgress::SetStateString ( "������ �޽� ����Ÿ ������..." );	
	////
	////m_CollisionMap.SetDxFrameMesh ( pFrameMesh );
	////m_CollisionMap.Import ();	

	//////	Note : ���� Obj ��Ʈ�� �������� ���� ����, EndPos�� DxOctree Import �߰��κп��� ����
	//////
 ////   ExportProgress::CurPos = 0;	
	////ExportProgress::SetStateString ( "���� Obj ��Ʈ�� �������� ������..." );
	////m_DxOctree.SetLoadState ( TRUE );
	////m_DxOctree.Import ( MaxTriangles, MaxSubdivisions, &m_CollisionMap );

	//////	Note : AABBƮ���� ����
	//////
	////ExportProgress::CurPos = 0;		
	////ExportProgress::SetStateString ( "AABB Ʈ�� ������..." );	
	////m_DxOctree.MakeAABBTree ();

	////	Note : ���Ἲ�� ���� ����� ���� �Լ������� �����Ȳ�� üũ��
	//ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = 6;	//	������ ȣ��� �Լ��� ����	
	//
	////	Note : ���ϸ��̼� ������ ��������.
	////
	//ExportProgress::SetStateString ( "���ϸ��̼� ������ ����������..." );	
	//DxAnimationMan::m_dwAniManCount = 0;
	//hr = pDxFrameRoot->GetAniFrameMesh ( m_pAniManHead, fUNITTIME, pd3dDevice );
	//if ( FAILED(hr) )
	//{
	//	OctreeDebugInfo::EndNodeCount = 0;
	//	OctreeDebugInfo::AmountDxFrame = 0;

	//	return E_FAIL;
	//}
	//ExportProgress::CurPos++;
	//

	////	Note : ���ϸ��̼� �޴���, ���� �����ϱ�.
	////
	//ExportProgress::SetStateString ( "���ϸ��̼� �޴���, ���� ������..." );	

	//DWORD dwNodeCount = 0;
	//POBJAABB pNodeCur = m_pAniManHead;
	//while ( pNodeCur )
	//{
	//	dwNodeCount++;
	//	pNodeCur = pNodeCur->GetNext();
	//}

	//ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = dwNodeCount;

	//COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );
	//ExportProgress::CurPos++;

	////	Note : ġȯ Obj ���� ����.
	////
	//ExportProgress::SetStateString ( "ġȯ Obj ���� ������..." );
	//DXPIECEOBJ* pPieceCur = pFrameMesh->GetPieceList ();
	//while ( pPieceCur )
	//{
	//	//	Note : ġȯ ���� ��������.
	//	//
	//	DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( pPieceCur->szFileName );
	//	if (!pPiece)
 //           return FALSE;

	//	DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
	//	pNewPieceObj->SetPiece ( pPiece );
	//	pNewPieceObj->matComb = pPieceCur->matComb;

	//	if ( pPiece->bRendAni )
	//	{
	//		srand(GetTickCount());
	//		pNewPieceObj->fCurTime = (rand()%1000) * 0.01f;
	//	}

	//	AddPieceObj ( pNewPieceObj );

	//	pPieceCur = pPieceCur->pNext;
	//}
	//ExportProgress::CurPos++;


	////	Note : ġȯ Obj AABB-Tree ����.
	////
	//ExportProgress::SetStateString ( "ġȯ Obj AABB-Tree ������..." );	
	//DxReplaceContainer::MakeAABBTree ( m_pPieceObjTree, m_pPieceObjHead );
	//ExportProgress::CurPos++;


	//// Prefab ���� ������� ���ϵ��� ������.
	//std::tr1::shared_ptr<DxPieceManagerPSF> spNewPieceManagerPSF = std::tr1::shared_ptr<DxPieceManagerPSF>(new DxPieceManagerPSF);
	//spNewPieceManagerPSF->Import( pd3dDevice, pSetLandMan->GetPieceManager(), pSetLandMan->GetPrefabMan() );

	//// LightMap ����
	//VEC_LIGHTMAPUV_MAIN vecLightMapUV_MAIN;
	//if ( bCreatLightMap )
	//{
	//	// Note : �ʱ�ȭ �Ѵ�.
	//	if ( pFrameMesh )
	//	{
	//		if ( pFrameMesh->GetFrameRoot() )
	//		{
	//			// LightMap�� �ʿ��� ������ ������ �´�. FrameName / UV_Data
	//			MAP_LIGHTMAPUV mapLightMapUV;
	//			pFrameMesh->ExportLightMapUV( mapLightMapUV, pFrameMesh->GetFrameRoot() );

	//			// UV ���� �����Ѵ�. UV�� 0 ~ 1 ������ ������ �����.
	//			int  nTextureID(0);
	//			while ( !mapLightMapUV.empty() )
	//			{
	//				SP_LIGHTMAPUV_MAIN spLightMapUV_Main = SP_LIGHTMAPUV_MAIN(new LIGHTMAPUV_MAIN);
	//				NSLightMap::RemakeLightMapUserUV_STATICMESH( mapLightMapUV, spLightMapUV_Main );
	//				spLightMapUV_Main->m_dwTextureID = nTextureID++;
	//				vecLightMapUV_MAIN.push_back( spLightMapUV_Main );
	//			}
	//		}
	//	}
	//}

	//m_spStaticMeshLightMap = std::tr1::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
	//m_spStaticMeshLightMap->StartThread( pd3dDevice );

	//for ( DWORD i=0; i<vecLightMapUV_MAIN.size(); ++i )
	//{
	//	SP_LIGHTMAPUV_MAIN spLightMapUV_MAIN = vecLightMapUV_MAIN[i];

	//	//	Note : Optimum Octree..
	//	std::tr1::shared_ptr<DxStaticMesh> pStaticMesh = std::tr1::shared_ptr<DxStaticMesh>(new DxStaticMesh);
	//	pStaticMesh->MakeAABBOCTree
	//	( 
	//		pd3dDevice, 
	//		pFrameMesh, 
	//		spNewPieceManagerPSF.get(),
	//		spLightMapUV_MAIN.get(), 
	//		TRUE, 
	//		FALSE, 
	//		bCreatLightMap, 
	//		strLightMapName, 
	//		NULL,
	//		TRUE
	//	);	// Modify	

	//	m_spStaticMeshLightMap->Insert( pStaticMesh );

	//	// ������ �����۾�.
	//	NSLightMap::CreateLightMapFinal_STATICMESH( pd3dDevice, strLightMapName, _T("_S"), i, 0, _T("") );
	//}

	//// �ؽ��� ����.
	//TextureManager::GetInstance().RescanPath();

	//m_spStaticMeshLightMap->LoadLightMapTexture( pd3dDevice );

 ////   //	Note : Optimum Octree..
	//////
	////SAFE_DELETE ( m_pStaticMesh );
	////m_pStaticMesh = new DxStaticMesh;
	////m_pStaticMesh->StartThread( pd3dDevice );
	////m_pStaticMesh->MakeAABBOCTree( pd3dDevice, pFrameMesh, pSetLandMan->GetPieceManager(), sLightMapUV_MAIN, TRUE, FALSE, bCreatLightMap, strLightMapName, fLightMapDetail, NULL, FALSE );	// Modify

	//// �浹ó���� ���� ��. spStaticMesh
	//std::tr1::shared_ptr<DxStaticMesh> spStaticMesh = std::tr1::shared_ptr<DxStaticMesh>(new DxStaticMesh);
	//spStaticMesh->MakeAABBOCTree
	//( 
	//	pd3dDevice, 
	//	pFrameMesh, 
	//	spNewPieceManagerPSF.get(), 
	//	NULL, 
	//	FALSE, 
	//	FALSE, 
	//	FALSE, 
	//	strLightMapName, 
	//	NULL,
	//	TRUE
	//);

	//// LightMap �� �ƴ� ��� �⺻���� StaticMesh
	//if ( !bCreatLightMap )
	//{
	//	m_spStaticMeshLightMap->Insert( spStaticMesh );
	//}


 //   {
 //       // Texture Number Count �� �ؾ��Ѵ�.
 //       // Piece ���� �ƴ����� Ȯ���ؾ� �Ѵ�.
 //       NSLightMap::Init( FALSE );

	//    //	Note : Effect ȿ�� �����ϱ�.
	//    ExportProgress::SetStateString ( "Effect ȿ�� ������..." );	
	//    m_dwEffectCount = 0;
	//    m_dwEffectCount_AFTER = 0;
	//    DxEffectBase* pEffCur = pFrameMesh->GetEffectList ();
	//    while ( pEffCur )
	//    {
	//	    DWORD TypeID = pEffCur->GetTypeID();
	//	    char* szFrameName = pEffCur->GetAdaptFrameName();

	//	    PBYTE pProperty;
	//	    DWORD dwSize;
	//	    DWORD dwVer;
	//	    pEffCur->GetProperty ( pProperty, dwSize, dwVer );
	//	    LPDXAFFINEPARTS pAffineParts = pEffCur->m_pAffineParts;

	//	    if ( pEffCur->GetFlag()&(_EFF_REPLACE|_EFF_SINGLE|_EFF_REPLACE_AFTER|_EFF_SINGLE_AFTER_0|_EFF_REPLACE_AFTER_1|_EFF_SINGLE_AFTER_1|_EFF_SINGLE_AFTER_2) )
	//	    {
 //               EffectAdaptToList ( pDxFrameRoot, 
 //                                   TypeID, 
 //                                   szFrameName, 
 //                                   pd3dDevice, 
 //                                   pProperty, 
 //                                   dwSize, 
 //                                   dwVer, 
 //                                   pAffineParts, 
 //                                   pEffCur, 
 //                                   pFrameMesh->GetTextureEffMan(),
	//								strLightMapName,
	//								vecLightMapUV_MAIN );
	//	    }
	//	    else
	//	    {
	//		    EffectAdaptToFrame ( m_DxOctree, TypeID, szFrameName, pd3dDevice, pProperty, dwSize, dwVer, pAffineParts, pEffCur, pFrameMesh->GetTextureEffMan() );
	//	    }

	//	    pEffCur = pEffCur->pEffectNext;
	//    }
	//    ExportProgress::CurPos++;

 //       //{
 //       //    // LightMap Texture�� Resize �Ѵ�
 //       //    NSLightMap::ResizeLightMapTexture();

 //       //    // LightMap 2���۾�
 //       //    DxEffectBase* pEffCur = m_pEffectList;
	//       // while ( pEffCur )
	//       // {
 //       //        pEffCur->CreateLightMap2( pd3dDevice, pFrameMesh->GetTextureEffMan(), bCreatLightMap, strLightMapName );
	//	      //  pEffCur = pEffCur->pEffectNext;
	//       // }

 //       //    // ������ �����۾�.
 //       //    NSLightMap::CreateLightMapFinal( pd3dDevice, strLightMapName, _T("_T") );

 //       //    // �ؽ��� ����.
 //       //    TextureManager::GetInstance().RescanPath();

 //       //    // LightMap �� �ε��Ѵ�.
 //       //    pEffCur = m_pEffectList;
	//       // while ( pEffCur )
	//       // {
 //       //        pEffCur->LoadLightMap( pd3dDevice );
	//	      //  pEffCur = pEffCur->pEffectNext;
	//       // }
 //       //}
 //   }


	//// Note : ���� ���� �Ŵ���
 //   if ( bCreatLightMap )
 //   {
 //       ExportProgress::SetStateString ( "LightMap ������ - PieceManager" );
 //   }
	//m_spPieceManager->ClonePSFtoWLD( pd3dDevice, spStaticMesh.get(), spNewPieceManagerPSF.get(), NULL, TRUE, bCreatLightMap, strLightMapName );

	////	Note : ���ϸ��̼� �޴���, ���� �����ϱ�.
	////
	//ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������..." );
	//COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
	//ExportProgress::CurPos++;

	//ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������...AFTER" );
	//COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	//ExportProgress::CurPos++;

	//ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������...AFTER_1" );
	//COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	//ExportProgress::CurPos++;

	//ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������...Weather" );
	//COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	//ExportProgress::CurPos++;


	////	Note : �׺���̼� �޽� �����ϱ�
	////	
	//DxFrame	*pNaviFrame = NULL;
	//pSetLandMan->GetNaviFrame ( &pNaviFrame );

	//if ( pNaviFrame && pNaviFrame->pmsMeshs )
	//{		
	//	ULONG	NumOfFaces = pNaviFrame->pmsMeshs->m_pSysMemMesh->GetNumFaces();

	//	ExportProgress::CurPos = 0;
	//	ExportProgress::EndPos = NumOfFaces;
	//	ExportProgress::SetStateString ( "�׺���̼� �޽� ������..." );	

	//	m_pNaviMesh = new NavigationMesh;
	//    m_pNaviMesh->Clear();

	//	hr = m_pNaviMesh->CreateNaviMesh ( pNaviFrame, pd3dDevice );
	//	if ( FAILED ( hr ) )
	//	{
	//		ExportProgress::SetStateString ( "����!! �׺���̼� �޽� ������ �����Ͽ����ϴ�." );
	//		m_pNaviMesh->Clear();
	//		SAFE_DELETE ( m_pNaviMesh );
	//	}
	//	else
	//	{
	//		ExportProgress::CurPos = 0;
	//		ExportProgress::EndPos = NumOfFaces;
	//		ExportProgress::SetStateString ( "�׺���̼� �޽� AABB Ʈ�� ������..." );
	//		m_pNaviMesh->MakeAABBTree ();

	//		//	<--	NavigationMesh ���� ����			
	//		ExportProgress::CurPos = 0;
	//		ExportProgress::EndPos = NumOfFaces;
	//		ExportProgress::SetStateString ( "�׺���̼� �޽� ��ũ��..." );	
	//		m_pNaviMesh->LinkCells ();
	//		//	-->	NavigationMesh ���� ����

	//		ExportProgress::CurPos = 0;
	//		ExportProgress::EndPos = 1;
	//		ExportProgress::SetStateString ( "�׺���̼� �޽� ��ũ ���Ἲ �˻���..." );
	//		m_bIntegrity = m_pNaviMesh->CheckIntegrity ( szDebugFullDir );
	//	}
	//}
	//else
	//{
	//	ExportProgress::EndPos = 1;
	//	ExportProgress::SetStateString ( "���!! �׺���̼� �޽��� �������� �ʾҽ��ϴ�." );			
	//	ExportProgress::CurPos++;
	//}

	////	Note : ���� ��ü �����ϱ� ( DxSetLandMan --> DxLandMan )
	////
	//ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = 1;
	//ExportProgress::SetStateString ( "���� ��ü ������..." );

	//m_pStaticSoundMan = new CStaticSoundMan;
	//CStaticSoundMan* pStaticSoundMan = pSetLandMan->GetStaticSoundMan();
	//pStaticSoundMan->Import ( m_pStaticSoundMan );	
	//ExportProgress::CurPos++;

	////	Note : Single Effect �����ϱ� ( DxSetLandMan --> DxLandMan )
	////
	//ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = pSetLandMan->GetNumLandEff ();
	//ExportProgress::SetStateString ( "Single Effect ȿ�� ������..." );

	//PLANDEFF pCurLandEff = pSetLandMan->GetLandEffList ();
	//while ( pCurLandEff )
	//{
	//	PLANDEFF pNewLandEff = new DXLANDEFF;
	//	pNewLandEff->SetEffect
 //       ( 
 //           pCurLandEff->m_szName, 
 //           pCurLandEff->m_matWorld, 
 //           pCurLandEff->m_szFileName, 
 //           pCurLandEff->m_fColorPowerDay, 
 //           pCurLandEff->m_fColorPowerNight, 
 //           pd3dDevice 
 //       );

	//	ExportProgress::CurPos++;
	//	AddLandEff ( pNewLandEff );

	//	pCurLandEff = pCurLandEff->m_pNext;
	//}
	//
	//BuildSingleEffTree ();

	////	Note : Light �Ӽ� ����.
	////
	//ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = 1;
	//ExportProgress::SetStateString ( "Light �Ӽ� ����..." );
	//DxLightMan* pLightMan = pSetLandMan->GetLightMan ();
	//pLightMan->CloneInstance ( m_pLightMan );

	////	Note : pWeatherMan �Ӽ� ����.
	////
	//ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = 1;
	//ExportProgress::SetStateString ( "Weather �Ӽ� ����..." );
	//DxWeatherMan* pWeatherMan = pSetLandMan->GetWeatherMan ();
	//pWeatherMan->CloneInstance ( &m_WeatherMan, pd3dDevice );

	////	Note : Camera Animation ����.
	////
	//ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = 1;
	//ExportProgress::SetStateString ( "Camera Animation �Ӽ� ����..." );
	//m_CameraAniMan.Import ( pSetLandMan->GetCameraAniMan() );

	////	Note : Land Skin_Object ����.
	////
	//ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = 1;
	//ExportProgress::SetStateString ( "Land Skin_Object ����..." );

	//PLANDSKINOBJ pCurSkinObj = pSetLandMan->GetLandSkinObjList ();
	//while ( pCurSkinObj )
	//{
	//	PLANDSKINOBJ pNewSkinObj = new DXLANDSKINOBJ;
	//	pNewSkinObj->SetSkinChar ( pCurSkinObj->m_szName, pCurSkinObj->m_matWorld, pCurSkinObj->m_szFileName, pd3dDevice );

	//	AddLandSkinObj ( pNewSkinObj );

	//	pCurSkinObj = pCurSkinObj->m_pNext;
	//}

	//BuildSkinObjTree ();
	//
	////	Note : Land Gate.
	////
	//ExportProgress::CurPos = 0;
	//ExportProgress::EndPos = 1;
	//ExportProgress::SetStateString ( "Land Gate ����..." );

	//m_LandGateMan = *pSetLandMan->GetLandGateMan();

	////	Note : fog.
	//m_FOG_PROPERTY = pSetLandMan->GetFogProperty();

	////	Note : sky.
	//m_SKY_PROPERTY = pSetLandMan->GetSkyProperty();

	//m_BGMDATA = *pSetLandMan->GetBGMDATA ();

	//ExportProgress::SetStateString ( "----------------------------------------------------" );

	return S_OK;
}

HRESULT	DxLandMan::Import( int MaxTriangles, 
						  BOOL bOneCell, 
						  DxSetLandMan *pSetLandMan, 
						  LPDIRECT3DDEVICEQ pd3dDevice, 
						  char *szDebugFullDir, 
						  const SUBPATH* pPath,
						  BOOL bCreatLightMap,
						  const TSTRING& strLightMapName,
						  BOOL bRadiosity,
						  int nRadiosityPass,
						  float fRadiosityReflectance,
						  float fRadiosityFogBlend,
						  DWORD dwRadiosityFogLength )
{	
	HRESULT hr;

	// �������� ����� �༮���� Material System �� ����ϴ� �͵��̴�.
	m_bUsedMaterialSystem = TRUE;
	m_bUsedMaterialSystemPS_3_0 = TRUE;

	m_dwRAD_Pass = nRadiosityPass;
	m_fRAD_Reflectance = fRadiosityReflectance;
	m_fRAD_FogBlend = fRadiosityFogBlend;
	m_dwRAD_FogLength = dwRadiosityFogLength;

	DxFrameMesh *pFrameMesh = pSetLandMan->GetFrameMesh();
	float fUNITTIME = pFrameMesh->GetUnitTime();
	DxFrame *pDxFrameRoot = pFrameMesh->GetFrameRoot();
	if (!pDxFrameRoot) return E_FAIL;


	OctreeDebugInfo::EndNodeCount = 0;
	OctreeDebugInfo::AmountDxFrame = 0;

	//	Note : �� ����Ÿ ��� ����.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "���� ����Ÿ ����..." );
	CleanUp ();

	//	Note : �� ��� ����.
	//
	m_MapID = pSetLandMan->GetMapID();
	memcpy ( m_szMapName, pSetLandMan->GetMapName(), sizeof(char)*MAXLANDNAME );

	//	Note : ���Ἲ�� ���� ����� ���� �Լ������� �����Ȳ�� üũ��
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 6;	//	������ ȣ��� �Լ��� ����	

	//	Note : ���ϸ��̼� ������ ��������.
	//
	ExportProgress::SetStateString ( "���ϸ��̼� ������ ����������..." );	
	DxAnimationMan::m_dwAniManCount = 0;
	hr = pDxFrameRoot->GetAniFrameMesh ( m_pAniManHead, fUNITTIME, pd3dDevice );
	if ( FAILED(hr) )
	{
		OctreeDebugInfo::EndNodeCount = 0;
		OctreeDebugInfo::AmountDxFrame = 0;

		return E_FAIL;
	}
	ExportProgress::CurPos++;


	//	Note : ���ϸ��̼� �޴���, ���� �����ϱ�.
	//
	ExportProgress::SetStateString ( "���ϸ��̼� �޴���, ���� ������..." );	

	DWORD dwNodeCount = 0;
	POBJAABB pNodeCur = m_pAniManHead;
	while ( pNodeCur )
	{
		dwNodeCount++;
		pNodeCur = pNodeCur->GetNext();
	}

	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = dwNodeCount;

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );
	ExportProgress::CurPos++;

	// CellSize �� ���ϴ� ���
	// Fov �� ���� �þ߰Ÿ��� �������� ������ ���Ѵ�.
	// �ű⼭ 50% ���� ������ �����Ѵ�.
	// �ű⼭ 10% ���� ������ �ø���.
	float fCellSize = (DxFogMan::GetInstance().GetGameFogHighEnd( GetFogProperty() ) / cosf( DxViewPort::GetInstance().GetFOV() * 0.5f )) * 0.5f * 1.1f;

	// �� Cell �� ���鵵�� �Ѵ�.
	if ( bOneCell )
	{
		fCellSize = 900000.f;
	}

	// �ӽ÷� Light�� �����Ѵ�. Octree ��忡�� LightMap�� ������ ���ð��� �����Ѱ� ����ϱ� ������.
	//	EditMode �� Light ���ð��� ����� �� �ֵ��� �������ش�.
	DxLightMan::SetInstance( pSetLandMan->GetLightMan() );

	// Prefab ���� ������� ���ϵ��� ������.
	std::tr1::shared_ptr<DxPieceManagerPSF> spNewPieceManagerPSF = std::tr1::shared_ptr<DxPieceManagerPSF>(new DxPieceManagerPSF);
	spNewPieceManagerPSF->Import( pd3dDevice, pSetLandMan->GetPieceManager(), pSetLandMan->GetPrefabMan() );

	// �浹ó���� ���� ��. spStaticMesh
	std::tr1::shared_ptr<DxStaticMesh> spStaticMesh = std::tr1::shared_ptr<DxStaticMesh>(new DxStaticMesh);
	spStaticMesh->MakeAABBOCTree
	( 
		pd3dDevice, 
		pFrameMesh, 
		spNewPieceManagerPSF.get(), 
		NULL, 
		FALSE, 
		FALSE, 
		FALSE, 
		strLightMapName, 
		NULL,
		pFrameMesh ? pFrameMesh->GetMaterialSetting_const() : NULL,
		FALSE,
		fCellSize
	);

	//	Note : 
	ExportProgress::SetStateString ( "DxPieceManagerPSF::CreateTriangleLinkLight" );	

	// [CascadeShadow] �۾�������
	// Piece�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
	SET_DWORD setEraseLightID;
	spNewPieceManagerPSF->CreateTriangleLinkLight( pd3dDevice, spStaticMesh.get(), setEraseLightID, pSetLandMan->GetShadowLightData()->m_bDirectionLight );

	//	Note : 
	ExportProgress::SetStateString ( "DxFrameMesh::CreateTriangleLinkLight" );	

	// Field�� TriangleLinkLight �� �����ϵ��� �Ѵ�.
	pFrameMesh->CreateTriangleLinkLight( pd3dDevice, spStaticMesh.get(), spNewPieceManagerPSF.get(), setEraseLightID, pSetLandMan->GetShadowLightData()->m_bDirectionLight );

	//	Note : 
	ExportProgress::SetStateString ( "DxPieceManagerPSF::EraseLightID4TriangleLinkLight" );	

	// LightID �� ���������� ȸ���ؼ� WORDWORD �������� LightID �� �ֵ��� �Ѵ�.
	{
		DWORD dwLightNumber = NSShadowLight::GetLightNumberALL();

		// dwLightNumber+1 �� �� ������ Direction Light�� 0 ���� ���̰� �ֱ� �����̴�.
		// 0 - Direction Light
		// 1~(dwLightNumber+1) - SpotLight
		for ( DWORD i=0; i<dwLightNumber+1; ++i )
		{
			// ���� ����Ʈ�� ������ �ﰢ������ ù��° ���Կ� �ڸ��� ������ Ȯ��.
			if ( pFrameMesh->IsTriangleLightID_InsertSlot( i, pFrameMesh->GetFrameRoot(), 0 ) && spNewPieceManagerPSF->IsLightID_InsertSlot( i, 0 ) )
			{
				// ���� ����Ʈ�� ������ �ﰢ������ ù��° ���Կ� ���� LightID �� ����.
				pFrameMesh->InsertSlotTriangleLightID( i, pFrameMesh->GetFrameRoot(), 0 );
				spNewPieceManagerPSF->InsertSlotLightID( i, 0 );
				continue;
			}

			// ���� ����Ʈ�� ������ �ﰢ������ �ι�° ���Կ� �ڸ��� ������ Ȯ��.
			if ( pFrameMesh->IsTriangleLightID_InsertSlot( i, pFrameMesh->GetFrameRoot(), 1 ) && spNewPieceManagerPSF->IsLightID_InsertSlot( i, 1 ) )
			{
				// ���� ����Ʈ�� ������ �ﰢ������ �ι�° ���Կ� ���� LightID �� ����.
				pFrameMesh->InsertSlotTriangleLightID( i, pFrameMesh->GetFrameRoot(), 1 );
				spNewPieceManagerPSF->InsertSlotLightID( i, 1 );
				continue;
			}
		}
	}

	// Triangle Link Light - Light�� 3�� �̻��� �� ã�� �� Light ���� 2���� ����
	pFrameMesh->EraseLightID4LightOver3( setEraseLightID );

	// Light�� 3�� �̻��� �� ã�� �� Light ���� 2���� ����
	spNewPieceManagerPSF->EraseLightID4LightOver3( setEraseLightID );

	// setEraseLightID �� �������� FrameMesh, �� LightID�� ���� �ϵ��� �Ѵ�.
	pFrameMesh->EraseLightID4TriangleLinkLight( setEraseLightID );

	// setEraseLightID �� �������� Piece �� LightID�� ���� �ϵ��� �Ѵ�.
	spNewPieceManagerPSF->EraseLightID4TriangleLinkLight( setEraseLightID );

	// Piece �� PS_3_0�� ����Ÿ���� �����ϵ��� �Ѵ�.
	spNewPieceManagerPSF->SetType_PS_3_0();

	// RealTime ���� ������ �ʴ� ���� ���쵵�� �Ѵ�.
	// ShadowLight�� �ִ´�.
	m_scpShadowLightData->Import( pSetLandMan->GetShadowLightData(), setEraseLightID );
	NSShadowLight::SetActive( m_scpShadowLightData.get() );

	// LightMap ����
	VEC_LIGHTMAPUV_MAIN vecLightMapUV_MAIN;
	if ( bCreatLightMap )
	{
		// Texture Number Count �� �ؾ��Ѵ�.
		// Piece ���� �ƴ����� Ȯ���ؾ� �Ѵ�.
		NSLightMap::Init( FALSE );

		// Note : �ʱ�ȭ �Ѵ�.
		if ( pFrameMesh )
		{
			if ( pFrameMesh->GetFrameRoot() )
			{
				// LightMap�� �ʿ��� ������ ������ �´�. FrameName / UV_Data
				MAP_LIGHTMAPUV mapLightMapUV;
				pFrameMesh->ExportLightMapUV( mapLightMapUV, pFrameMesh->GetFrameRoot() );

				// UV ���� �����Ѵ�. UV�� 0 ~ 1 ������ ������ �����.
				int  nTextureID(0);
				while ( !mapLightMapUV.empty() )
				{
					SP_LIGHTMAPUV_MAIN spLightMapUV_Main = SP_LIGHTMAPUV_MAIN(new LIGHTMAPUV_MAIN);
					NSLightMap::RemakeLightMapUserUV_STATICMESH( mapLightMapUV, spLightMapUV_Main );
					spLightMapUV_Main->m_dwTextureID = nTextureID++;
					vecLightMapUV_MAIN.push_back( spLightMapUV_Main );
				}
			}
		}
	}

	m_spStaticMaterialMeshLightMap = boost::shared_ptr<DxStaticMaterialMeshLightMap>(new DxStaticMaterialMeshLightMap);

	m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);

	for ( DWORD i=0; i<vecLightMapUV_MAIN.size(); ++i )
	{
		SP_LIGHTMAPUV_MAIN spLightMapUV_MAIN = vecLightMapUV_MAIN[i];

		//	Note : Optimum Octree..
		std::tr1::shared_ptr<DxStaticMesh> pStaticMesh = std::tr1::shared_ptr<DxStaticMesh>(new DxStaticMesh);
		pStaticMesh->MakeAABBOCTree
		( 
			pd3dDevice, 
			pFrameMesh, 
			spNewPieceManagerPSF.get(),
			spLightMapUV_MAIN.get(), 
			TRUE, 
			FALSE, 
			bCreatLightMap, 
			strLightMapName, 
			NULL,
			pFrameMesh ? pFrameMesh->GetMaterialSetting_const() : NULL,
			FALSE,
			fCellSize
		);	// Modify	

		// DxStaticMaterialMesh �� ���� �� m_spStaticMaterialMeshLightMap �� �ִ´�.
		std::tr1::shared_ptr<DxStaticMaterialMesh> pStaticMaterialMesh = std::tr1::shared_ptr<DxStaticMaterialMesh>(new DxStaticMaterialMesh);
		pStaticMaterialMesh->Import( pd3dDevice, pStaticMesh.get(), fCellSize );
		pStaticMaterialMesh->SetLightMapName( strLightMapName, _T("_S"), i );	// Main �ʿ� LightMap�� �ټ��� ��� Name �� �� ���� �ȵǴ� ������ ������ �ڵ尡 �߰���.
		m_spStaticMaterialMeshLightMap->Insert_SMMLM( pStaticMaterialMesh );

		// ������ �����۾�.
		NSLightMap::CreateLightMapFinal_STATICMESH( pd3dDevice, strLightMapName, _T("_S"), i, 0, _T("") );
	}

	NSLightMap::InitTextureDataBackup();
	NSLightMap::SetTextureDataBackupIntoUsed( 0 );

	//// �ؽ��� ����.
	//TextureManager::GetInstance().RescanPath();

	//m_spStaticMaterialMeshLightMap->LoadLightMapTexture( pd3dDevice );

	// LightMap �� �ƴ� ��� �⺻���� StaticMesh
	if ( !bCreatLightMap )
	{
		m_spStaticMeshLightMap->Insert( spStaticMesh );
	}

	TSTRING strErrorMsg(_T(""));

	// Note : ���� ���� �Ŵ���
	if ( bCreatLightMap )
	{
		ExportProgress::SetStateString ( "LightMap ������ - PieceManager" );
	}
	m_spPieceManager->ClonePSFtoWLD
	(
		pd3dDevice, 
		spStaticMesh.get(),
		spNewPieceManagerPSF.get(),
		NULL, 
		FALSE,
		bCreatLightMap, 
		strLightMapName,
		strErrorMsg,
		fCellSize
	);
	NSLightMap::SetTextureDataBackupIntoUsed( 1 );

	// �ؽ��� �����۾�.
	{
		// NSLightMap�� �ؽ��� ����.
		TextureManager::GetInstance().RescanPath();

		// LightMap Texture �ε� ��.
		m_spStaticMaterialMeshLightMap->RefreshLightMap( pd3dDevice );
		m_spPieceManager->RefreshLightMap( pd3dDevice );
	}

	// Collsion�� ���� Texture ������ �����ص� �ȴ�.
	NSTexture16Bit::CleanUp();

	// SkinObject �� �ֺ����� ���� �� �ֵ��� �۾��Ѵ�.
	PLANDSKINOBJ pCurSkinObj = pSetLandMan->GetLandSkinObjList ();
	while ( pCurSkinObj )
	{
		pCurSkinObj->LightMapCalculationBoxColor( pd3dDevice, spStaticMesh.get(), spNewPieceManagerPSF.get() );
		pCurSkinObj = pCurSkinObj->m_pNext;
	}

	SetNSLandThread();

	// Radiosity �۾�
	// 
	if ( bRadiosity )
	{
		// FIXED �� ���� �� ������ ������ ���δ�.
		D3DLIGHTQ	sSrcLight00;
		pd3dDevice->GetLight ( 0, &sSrcLight00 );
		{
			D3DLIGHTQ	sDestLight00;
			sDestLight00 = sSrcLight00;

			sDestLight00.Ambient.r = sSrcLight00.Ambient.r * 0.5f;
			sDestLight00.Ambient.g = sSrcLight00.Ambient.g * 0.5f;
			sDestLight00.Ambient.b = sSrcLight00.Ambient.b * 0.5f;
			sDestLight00.Diffuse.r = sSrcLight00.Diffuse.r * 0.5f;
			sDestLight00.Diffuse.g = sSrcLight00.Diffuse.g * 0.5f;
			sDestLight00.Diffuse.b = sSrcLight00.Diffuse.b * 0.5f;

			pd3dDevice->SetLight ( 0, &sDestLight00 );
		}

		// [shhan][2013.04.24] ����Ʈ�� ������ ����ȴٴ� �̾߱Ⱑ �־ �־.
		//				�̰� � Ư���� ��Ȳ���� ResetDevice, LostDevice, FrameMove, Render �� ������ ���� ���� ��Ȱ�� ��.
		NSOcclusionCulling::g_bBaking = TRUE;

		NSRadiosity::CreateSurface( pd3dDevice );

		HRESULT hr(S_OK);
		LPDIRECT3DTEXTUREQ pTextureSystem(NULL);
		if( !pTextureSystem )
		{
			hr = pd3dDevice->CreateTexture(
				128,
				128,
				1,
				0,
				D3DFMT_A8R8G8B8,
				D3DPOOL_SYSTEMMEM,
				&pTextureSystem,
				NULL );
		}

		// VoxelColor�� Ambient Cube ���� �����ؾ� �Ѵ�.
		if ( m_spStaticMaterialMeshLightMap.get() )
			m_spStaticMaterialMeshLightMap->FrameMove( 0.f );

		if ( pTextureSystem )
		{
			for ( int i=0; i<nRadiosityPass; ++i )
			{
				NSLightMap::SetTextureDataUsedIntoBackup( 0 );

				for ( DWORD z=0; z<vecLightMapUV_MAIN.size(); ++z )
				{
					SP_LIGHTMAPUV_MAIN spLightMapUV_MAIN = vecLightMapUV_MAIN[z];
					NSSTATICMESH::RenderRadiosityForFrameMesh
					( 
						pd3dDevice, 
						pFrameMesh, 
						this, 
						spLightMapUV_MAIN.get(),
						pTextureSystem,
						fRadiosityReflectance,
						fRadiosityFogBlend,
						dwRadiosityFogLength
					);

					// ���� �ؽ��ĸ� �����.
					// ������ �����۾�.
					NSLightMap::CreateLightMapFinal_STATICMESH( pd3dDevice, strLightMapName, _T("_S"), z, i+1, _T("") );
				}

				NSLightMap::SetTextureDataUsedIntoBackup( 1 );
				m_spPieceManager->RenderRadiosity
				( 
					pd3dDevice, 
					this,
					pTextureSystem,
					strLightMapName,
					i,
					fRadiosityReflectance,
					fRadiosityFogBlend,
					dwRadiosityFogLength
				);

				// �ؽ��� �����۾�.
				m_spStaticMaterialMeshLightMap->RefreshLightMap( pd3dDevice );
				m_spPieceManager->RefreshLightMap( pd3dDevice );
			}

			// SkinObject �� �ֺ����� ���� �� �ֵ��� �۾��Ѵ�.
			PLANDSKINOBJ pCurSkinObj = pSetLandMan->GetLandSkinObjList ();
			while ( pCurSkinObj )
			{
				pCurSkinObj->AddRadiosityColorForVoxel( pd3dDevice, this, pTextureSystem, fRadiosityReflectance, fRadiosityFogBlend, dwRadiosityFogLength );
				pCurSkinObj = pCurSkinObj->m_pNext;
			}

			//// Radiosity Normal Map �� ������.
			////	���⼺ 3����.
			//for ( int i=0; i<3; ++i )
			//{
			//	// Object ������
			//	{
			//		iter = mapNamePiece.begin();
			//		for( ; iter!=mapNamePiece.end(); ++iter )
			//		{
			//			// �������� ���Ǵ� �����Ϳ� ���� �ִ´�.
			//			(*iter).second->BakeRadiosity( pd3dDevice, pLandMan, pTextureSystem, fRadiosityReflectance, fRadiosityFogBlend, i );
			//		}

			//		TSTRINGSTREAM ssADDName;
			//		ssADDName << _T("_RC");
			//		ssADDName << i;

			//		// ���� �ؽ��ĸ� �����.
			//		// ������ �����۾�.
			//		NSLightMap::CreateLightMapFinal_PIECE( pd3dDevice, strLightMapName, _T("_"), -1, ssADDName.str() );
			//	}
			//}

			SAFE_RELEASE( pTextureSystem );

			// �ؽ��� �����۾�.
			{
				// NSLightMap�� �ؽ��� ����.
				TextureManager::GetInstance().RescanPath();

				// LightMap Texture �ε� ��.
				m_spStaticMaterialMeshLightMap->RefreshLightMap( pd3dDevice );
				m_spPieceManager->RefreshLightMap( pd3dDevice );
			}
		}

		NSRadiosity::DeleteSurface();

		NSOcclusionCulling::g_bBaking = FALSE;

		// FIXED �� ���� �� ������ ������ ���� ���� �ǵ�����.
		{
			pd3dDevice->SetLight ( 0, &sSrcLight00 );
		}
	}

	//m_spPieceManager->ClonePSFtoWLD( pd3dDevice, pSetLandMan->GetPieceManager(), bCreatLightMap, strLightMapName, spStaticMesh.get(), fLightMapDetail, FALSE );

	//	Note : ���ϸ��̼� �޴���, ���� �����ϱ�.
	//
	ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������..." );
	COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
	ExportProgress::CurPos++;

	ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������...AFTER" );
	COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	ExportProgress::CurPos++;

	ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������...AFTER_1" );
	COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	ExportProgress::CurPos++;

	ExportProgress::SetStateString ( "�ִϸ��̼� �޴���, ���� ������...Weather" );
	COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	ExportProgress::CurPos++;


	//	Note : �׺���̼� �޽� �����ϱ�
	//	
	m_sNaviMeshCapsule.CreateNaviMesh( pSetLandMan, pd3dDevice, szDebugFullDir );


	//	Note : ���� ��ü �����ϱ� ( DxSetLandMan --> DxLandMan )
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "���� ��ü ������..." );

	m_pStaticSoundMan = new CStaticSoundMan;
	CStaticSoundMan* pStaticSoundMan = pSetLandMan->GetStaticSoundMan();
	pStaticSoundMan->Import ( m_pStaticSoundMan );	
	ExportProgress::CurPos++;

	//	Note : Single Effect �����ϱ� ( DxSetLandMan --> DxLandMan )
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = pSetLandMan->GetNumLandEff ();
	ExportProgress::SetStateString ( "Single Effect ȿ�� ������..." );

	PLANDEFF pCurLandEff = pSetLandMan->GetLandEffList ();
	while ( pCurLandEff )
	{
		PLANDEFF pNewLandEff = new DXLANDEFF;
		pNewLandEff->SetEffect
			( 
			pCurLandEff->m_szName, 
			pCurLandEff->m_matWorld, 
			pCurLandEff->m_szFileName, 
			pCurLandEff->m_fColorPowerDay, 
			pCurLandEff->m_fColorPowerNight, 
			pd3dDevice 
			);

		ExportProgress::CurPos++;
		AddLandEff ( pNewLandEff );

		pCurLandEff = pCurLandEff->m_pNext;
	}

	BuildSingleEffTree ();

	//	Note : Light �Ӽ� ����.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Light �Ӽ� ����..." );
	DxLightMan* pLightMan = pSetLandMan->GetLightMan ();
	pLightMan->CloneInstance ( m_pLightMan, !NSShadowLight::IsCascadeShadow() );

	//	Note : pWeatherMan �Ӽ� ����.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Weather �Ӽ� ����..." );
	DxWeatherMan* pWeatherMan = pSetLandMan->GetWeatherMan ();
	pWeatherMan->CloneInstance ( &m_WeatherMan, pd3dDevice );

	//	Note : Camera Animation ����.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Camera Animation �Ӽ� ����..." );
	m_CameraAniMan.Import ( pSetLandMan->GetCameraAniMan() );

	//	Note : Land Skin_Object ����.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Land Skin_Object ����..." );

	m_spLandSkinManager->Import( pd3dDevice, pSetLandMan->GetLandSkinObjList() );

	//	Note : Land Gate.
	//
	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;
	ExportProgress::SetStateString ( "Land Gate ����..." );

	m_LandGateMan = *pSetLandMan->GetLandGateMan();

	//	Note : fog.
	m_FOG_PROPERTY = pSetLandMan->GetFogProperty();

	//	Note : sky.
	m_SKY_PROPERTY = pSetLandMan->GetSkyProperty();

	m_BGMDATA = *pSetLandMan->GetBGMDATA ();

	// ���� �� ������
	NSWaterMain::Import ( pd3dDevice, NSWaterMainPSF::GetWaterSurf() );

	SetNSLandThread();

	ExportProgress::SetStateString ( "----------------------------------------------------" );


	if ( !strErrorMsg.empty() )
	{
		AfxMessageBox( strErrorMsg.c_str() );
	}

	return S_OK;
}

BOOL DxLandMan::AddPieceObj ( DXPIECEOBJ *pNewPieceObj )
{
	//	Note : ����Ʈ�� ���.
	//
	pNewPieceObj->pNext = m_pPieceObjHead;
	m_pPieceObjHead = pNewPieceObj;

	return TRUE;
}

static TnL_MODE	g_emTnLMode_Prev(TnL_FIXED);

HRESULT DxLandMan::FrameMove( float fTime,
							 float fElapsedTime,
							 const CLIPVOLUME &cv,
							 BOOL bNotRendering )
{
	//////////////////////////////////////////////////////////////////////////
	// �Ϸ�� ������ ����Ѵ�.
	NSLandThread::Wait_FrameMove
	( 
		m_spEntityRenderListSwap,
		cv, 
		fTime, 
		fElapsedTime,
		m_bUsedMaterialSystem, 
		m_bUsedMaterialSystemPS_3_0, 
		IsStaticMaterialMesh() ? GetRealTimeWaterHeight() :DxEnvironment::GetInstance().GetCenter().y,
		bNotRendering
	);







	//OverLight�� ������ ��ġ�� �����ϴ� ��
	NSMaterialManager::SetOverLighting_Values( !m_bUsedMaterialSystemPS_3_0 );

	// ������ ���� Spot ���� ������ ��Ƴ��� �� ������.
	// ���� Spot �����͸� ����� ���� �ƴ�.
	NSShadowLight::ClearRenderSpotID();

	DxAnimationMan *pAnimManCur = m_pAniManHead;
	while(pAnimManCur)
	{
		pAnimManCur->FrameMove ( fTime, fElapsedTime );
		pAnimManCur = pAnimManCur->m_pNext;
	}

	DxEffectBase *pEffectCur;

	pEffectCur = m_pEffectFrameList;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );
		pEffectCur = pEffectCur->pEffectNext;
	}

	pEffectCur = m_pEffectList;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pEffectList_AFTER_1;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );
		pEffectCur = pEffectCur->pEffectNext;
	}
	pEffectCur = m_pList_Weather;
	while ( pEffectCur )
	{
		pEffectCur->FrameMove ( fTime, fElapsedTime );
		pEffectCur = pEffectCur->pEffectNext;
	}

	if ( m_pStaticSoundMan )
	{
		//	Note : ī�޶� ���⿡ ���� �Ҹ���ȭ �ݿ�
		//		   ���� ���� Ŭ����
		//
		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt();
		D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt();
		m_pStaticSoundMan->FrameMove ( vLookatPt, fElapsedTime );
	}

	DXLANDEFF::FrameMove ( fTime, fElapsedTime );
	DXLANDSKINOBJ::FrameMove ( fTime, fElapsedTime );

	m_CameraAniMan.FrameMove ( fTime, fElapsedTime );

	// Note : ���� ���� �Ŵ���
	m_spPieceManager->FrameMove( fTime, fElapsedTime, m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );

	// Note : m_pStaticMesh
	if ( m_spStaticMeshLightMap.get() )
		m_spStaticMeshLightMap->FrameMove( fElapsedTime );

	if ( m_spStaticMaterialMeshLightMap.get() )
		m_spStaticMaterialMeshLightMap->FrameMove( fElapsedTime );

	//// Note : m_pStaticMesh
	//if ( m_pStaticMesh )
	//       m_pStaticMesh->FrameMove( fElapsedTime );

	DxSkyBoxMan::GetInstance().FrameMove( fElapsedTime );

	// ���� �� ������
	NSWaterMain::FrameMove( fElapsedTime );

	NSMaterialManager::SetTime( fTime );

	//m_spLandSkinManager->FrameMove( fElapsedTime, cv,  GetRealTimeWaterHeight() );

	m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL]->FrameMoveLandSkin( fElapsedTime );

	return S_OK;
}





//////////////////////////////////////////////////////////////////////////
// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
HRESULT DxLandMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bFrustom /* = TRUE */ )
{
	HRESULT hr = S_OK;

	if ( (g_emTnLMode_Prev != RENDERPARAM::g_emGroundQulity) )
	{
		g_emTnLMode_Prev = RENDERPARAM::g_emGroundQulity;
		Active_MATERIAL_DATA( pd3dDevice, FALSE );
	}

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	NSMaterialManager::FrameMove
	( 
		DxViewPort::GetInstance().GetMatView(), 
		DxViewPort::GetInstance().GetMatProj(), 
		NULL, 
		NULL, 
		NULL,
		NULL
	);

    // ī�޶� ������ �۵��Ǿ�� �Ѵ�.
    NSLightMapFX::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj(), 1.f );

	// �⺻���� Octree �� Shader�� ������� �ʵ��� �Ѵ�. ���������� ����.
	NSVERTEXNORTEX_FX::SetEnable();
	NSVERTEXNORTEX_FX::FrameMove();
	NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
	NSVERTEXNORTEX_FX::SetDisable();

	PROFILE_BEGIN("DxLandMan::Render");

	OctreeDebugInfo::TotalNodesDrawn = 0;
	OctreeDebugInfo::FaceViewCount = 0;
	OctreeDebugInfo::VertexViewCount = 0;
	OctreeDebugInfo::TodalDxFrameDrawn = 0;

	float fFogStart(1.f);
	float fFogEnd(2.f);
	//float fBias = -0.001f;
	//DWORD dwBiasPrev(0);
	DWORD dwFogStartPrev(0);
	DWORD dwFogEndPrev(0);
	DWORD dwFogColor(0L);
	if ( g_bWIREFRAME_VIEW )
	{
		//pd3dDevice->GetRenderState ( D3DRS_DEPTHBIAS,	&dwBiasPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	&dwFogStartPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGEND,	&dwFogEndPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
		//pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
		pd3dDevice->SetRenderState ( D3DRS_FOGEND,	*((DWORD *)(&fFogEnd)) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );

        NSMaterialManager::SetFogColorForEditor( 0xff000000 );
	}

	if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM && m_bUsedMaterialSystem )
	{
	}
	else
	{
		DxSkyBoxMan::GetInstance().Render( pd3dDevice );

		// �ϴ� ������..!! ��ü
		DxSkyMan::GetInstance().Render( pd3dDevice, DxViewPort::GetInstance().GetFOV(), DxViewPort::GetInstance().GetAspect() );
	}

	//	Note : ���� Ʈ������ �ʱ�ȭ.
	//
	pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

	// �ӵ�üũ�� �ȵ�. FALSE �ϸ� �ȵ�.
	//COLLISION::bCollisionCheck = bFrustom;
	
	const VEC_DWORD* pvecPieceIndex(NULL);
	if ( NSOcclusionCulling::IsActive() )
	{
		const VEC_DWORD* pvecOctreeIndex(NULL);
		NSOcclusionCulling::GetRenderIndex( DxViewPort::GetInstance().GetLookatPt(), &pvecPieceIndex, &pvecOctreeIndex );
		NSOcclusionOctree::InsertIndex( pvecOctreeIndex );
	}

	//	Note : �⺻ ���� ������.
	//
	PROFILE_BEGIN("DxOctree::Render");
	{
		DxLightMan::GetInstance()->ReSetLight( pd3dDevice );

		m_DxOctree.Render ( cv, m_SFileMap, pd3dDevice, m_pEffectFrameList, this );

		DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
	}
	PROFILE_END("DxOctree::Render");

	// RenderList Reset
	NSOctreeRenderList::ClearRenderIndex();

	//	Note : Optimum Octree
	//
	PROFILE_BEGIN("DxOptimumOctree::Render");
	if ( m_spStaticMeshLightMap.get() )
	{
        if( GLOBAL_PARAM::IsValue( GP_PIECE ) )
        {
		    D3DMATERIALQ	sMaterial;
		    sMaterial.Diffuse.r = 1.f;
		    sMaterial.Diffuse.g = 1.f;
		    sMaterial.Diffuse.b = 1.f;
		    sMaterial.Diffuse.a = 1.f;

		    sMaterial.Ambient.r = 1.f;
		    sMaterial.Ambient.g = 1.f;
		    sMaterial.Ambient.b = 1.f;
		    sMaterial.Ambient.a = 1.f;

		    sMaterial.Specular.r = 1.f;
		    sMaterial.Specular.g = 1.f;
		    sMaterial.Specular.b = 1.f;
		    sMaterial.Specular.a = 1.f;

		    sMaterial.Emissive.r = 0.f;
		    sMaterial.Emissive.g = 0.f;
		    sMaterial.Emissive.b = 0.f;
		    sMaterial.Emissive.a = 0.f;

		    sMaterial.Power	= 1.f;

		    pd3dDevice->SetMaterial ( &sMaterial );

		    pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

			m_spStaticMeshLightMap->Render_THREAD_( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL, cv );
			m_spStaticMeshLightMap->Render_THREAD_Alpha ( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL, cv );

		    pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
        }
	}
	PROFILE_END("DxOptimumOctree::Render");

	//if ( g_bWIREFRAME_VIEW )
	//{
	//	pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );
	//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );

	//	NSMaterialManager::SetFogColorForEditor( 0xff000000 );
	//}

	// mat �� ����
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->RenderOrderSetting( pd3dDevice, cv );
		m_spStaticMaterialMeshLightMap->Render( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL, FALSE, matIdentity );
		m_spStaticMaterialMeshLightMap->RenderHardAlpha( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL, FALSE, matIdentity );
		m_spStaticMaterialMeshLightMap->RenderDecal( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL, matIdentity );
	}

	BOOL bOcclusionCullingRendering = NSOcclusionCulling::g_bRendering;
	NSOcclusionCulling::g_bRendering = FALSE;
	{
		//	Note : ���� ȿ�� ������.	SINGLE, REPLACE, .....
		PROFILE_BEGIN("DxLandMan::Render-m_pEffectTree");
	//	COLLISION::dwRendAABB = 0;
		COLLISION::RenderAABBTreeFrame ( pd3dDevice, &cv, m_pEffectTree, FALSE, this );	
		PROFILE_END("DxLandMan::Render-m_pEffectTree");
	}
	NSOcclusionCulling::g_bRendering = bOcclusionCullingRendering;

	//	Note : ġȯ ������Ʈ ������.
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*fDistance;
	DxReplaceContainer::EMREFLECTOPTION emReflect = DxReplaceContainer::EM_NULL;
	DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjTree, &cv, emReflect, fDistance );

	//CDebugSet::ToView ( 3, "���� Octree %d, DxFrame %d", OctreeDebugInfo::EndNodeCount, OctreeDebugInfo::AmountDxFrame );
	//CDebugSet::ToView ( 4, "������ Octree %d, DxFrame %d", OctreeDebugInfo::TotalNodesDrawn, OctreeDebugInfo::TodalDxFrameDrawn );
	//CDebugSet::ToView ( 5, "������ ���̽� %d, ���ؽ� %d", OctreeDebugInfo::FaceViewCount, OctreeDebugInfo::VertexViewCount );
	//CDebugSet::ToView ( 6, "������ ���� %d", COLLISION::dwRendAABB );
	//CDebugSet::ToView ( 7, "Effect Num %d", m_dwEffectCount );

	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff00ffff );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );

        NSMaterialManager::SetFogColorForEditor( 0xff00ffff );
	}

	//NSPieceQuickSort::InitPickAlpha();		// Note : Init Static
	//NSPieceQuickSort::Reset();

	// Note : ���� ���� �Ŵ���
	//m_spPieceManager->SetRenderOrder( pd3dDevice, TRUE );
	m_spPieceManager->RenderOpaque
	( 
		pd3dDevice,
		pvecPieceIndex, 
		FALSE, 
		m_bUsedMaterialSystem,
		FALSE,
		0.f,
		m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL]
	);
	m_spPieceManager->RenderHardAlpha( pd3dDevice, 1.f, FALSE, m_bUsedMaterialSystem, FALSE, m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );
	m_spPieceManager->RenderEffFrame( m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );
	m_spPieceManager->RenderDecal( m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );


	//m_spEntityRenderListSwap->RenderOpaque( pd3dDevice, FALSE, m_bUsedMaterialSystem, FALSE, 0.f, TRUE );
	//m_spEntityRenderListSwap->RenderHardAlpha( pd3dDevice, FALSE, m_bUsedMaterialSystem, TRUE );


	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );
        NSMaterialManager::SetFogColorForEditor( 0xff000000 );
	}

	// ���� �� ������
	NSWaterMain::Render( pd3dDevice );


	if ( g_bWIREFRAME_VIEW )
	{
		//pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	dwBiasPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	dwFogStartPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGEND,	dwFogEndPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );

        NSMaterialManager::SetFogColorForEditor( dwFogColor );
	}

	PROFILE_END("DxLandMan::Render");
	return S_OK;
}


HRESULT DxLandMan::Render_LOW_SoftAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	PROFILE_BEGIN("DxLandMan::Render_EFF");

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	float fFogStart(1.f);
	float fFogEnd(2.f);
	float fBias = -0.001f;
	DWORD dwBiasPrev(0);
	DWORD dwFogStartPrev(0);
	DWORD dwFogEndPrev(0);
	DWORD dwFogColor(0L);
	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->GetRenderState ( D3DRS_DEPTHBIAS,	&dwBiasPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	&dwFogStartPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGEND,	&dwFogEndPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
		pd3dDevice->SetRenderState ( D3DRS_FOGEND,	*((DWORD *)(&fFogEnd)) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );

        NSMaterialManager::SetFogColorForEditor( 0xff000000 );
	}

	//	Note : ���� ȿ�� ������.	SINGLE, REPLACE, .....
	//
	PROFILE_BEGIN("DxLandMan::Render_EFF-m_pEffectTree_1");
//	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTreeFrame ( pd3dDevice, &cv, m_pEffectTree_AFTER, FALSE, this );	
	PROFILE_END("DxLandMan::Render_EFF-m_pEffectTree_1");

	//	Note : ���ϸ��̼� �ִ� ������Ʈ ������.
	//
//	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pAniManTree, FALSE );

	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );

		NSMaterialManager::SetFogColorForEditor( 0xff000000 );
	}

    //	Note : ���� �� �Ӽ��� Frame ������.
    //
    DrawAlphaMap ( pd3dDevice, &cv );

	//	Note : ���� ȿ�� ������.	SINGLE, REPLACE, .....
	//
	PROFILE_BEGIN("DxLandMan::Render_EFF-m_pEffectTree");
//	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTreeFrame ( pd3dDevice, &cv, m_pEffectTree_AFTER_1, FALSE, this );	
	PROFILE_END("DxLandMan::Render_EFF-m_pEffectTree");

	// mat �� ����
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->RenderDecalPass2( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL, matIdentity );
	}

	{
		// - water ������.
		// mat �� ����
		if ( m_spStaticMaterialMeshLightMap.get() )
		{
			pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
			m_spStaticMaterialMeshLightMap->RenderWater_CustomSoftAlpha( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL );
			m_spStaticMaterialMeshLightMap->RenderSoftAlpha( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL, FALSE, matIdentity );
		}

		m_spPieceManager->RenderSoftAlpha( pd3dDevice, 1.f, FALSE, m_bUsedMaterialSystem, m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );

		//m_spEntityRenderListSwap->RenderSoftAlpha( pd3dDevice, TRUE, FALSE, m_bUsedMaterialSystem );

//		//	Note : ��Ų ������Ʈ ������.
//		//
//		PROFILE_BEGIN("DxLandMan::Render-m_pLandSkinObjTree");
////		COLLISION::dwRendAABB = 0;
//		COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandSkinObjTree, FALSE );
//		PROFILE_END("DxLandMan::Render-m_pLandSkinObjTree");

		m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL]->RenderSkinObject( FALSE, 0.f );
	}

    // ����
    {
	    //	Note : ������ ���� ����Ʈ �߿��� ���� ��� �Ѹ��� Render �ϴ� ����Ʈ.
	    //
	    PROFILE_BEGIN("m_AfterRender.Render");
        if( GLOBAL_PARAM::IsValue( GP_PIECE ) )
    	    m_AfterRender.Render ( pd3dDevice );
	    m_AfterRender.Clear ();
	    PROFILE_END("m_AfterRender.Render");

	    //	Note : ġȯ ������Ʈ ������.
	    DxReplaceContainer::EMREFLECTOPTION emReflect = DxReplaceContainer::EM_NULL;
	    DxReplaceContainer::DrawRePlacePiece_ALPHA( pd3dDevice, m_pPieceObjTree, &cv, emReflect );

        // VisualMaterial�� Translucent ���͸��� ������
		NSTexEffVisualMaterial::Render( pd3dDevice, &NSVMRender::g_sTranslucent );
    }

	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	dwBiasPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	dwFogStartPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGEND,	dwFogEndPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );

        NSMaterialManager::SetFogColorForEditor( dwFogColor );
	}

	PROFILE_END("DxLandMan::Render_EFF");

	return S_OK;
}

HRESULT DxLandMan::Render_LOW_SingleEff ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	RenderPickAlpha( pd3dDevice, FALSE );

	// Note : ���� ���� �Ŵ���
	m_spPieceManager->RenderEff( pd3dDevice, m_spStaticMeshLightMap.get(), NULL, 1.f, m_bUsedMaterialSystem, m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );

	//	Note : �ܵ� ����Ʈ ( single effect ) ������.
	//
	//	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );

	//	Note : Weather..
	PROFILE_BEGIN("Weather");
	//	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTreeFrame( pd3dDevice, &cv, m_pTree_Weather, FALSE, this );	
	PROFILE_END("Weather");

	return S_OK;
}

void DxLandMan::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem )
{
	LPDIRECT3DSURFACEQ	pColorBuffer(NULL);
	if ( RENDERPARAM::g_bEnable_HDR )
	{
		pColorBuffer = DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR;
	}
	else
	{
		pColorBuffer = DxSurfaceTex::GetInstance().m_pColorBuffer;
	}

	RenderPickAlpha
	( 
		pd3dDevice,
		FALSE, 
		0.f 
	);
}

void DxLandMan::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice,
								BOOL bReflect,
								float fReflectHeight )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	float fFogStart(1.f);
	float fFogEnd(2.f);
	float fBias = -0.001f;
	DWORD dwBiasPrev(0);
	DWORD dwFogStartPrev(0);
	DWORD dwFogEndPrev(0);
	DWORD dwFogColor(0L);
	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->GetRenderState ( D3DRS_DEPTHBIAS,	&dwBiasPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	&dwFogStartPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGEND,	&dwFogEndPrev );
		pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
		pd3dDevice->SetRenderState ( D3DRS_FOGEND,	*((DWORD *)(&fFogEnd)) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff00ffff );

		NSMaterialManager::SetFogColorForEditor( 0xff00ffff );
	}

	BOOL bOcclusionCullingRendering = NSOcclusionCulling::g_bRendering;
	NSOcclusionCulling::g_bRendering = TRUE;

	DxReplaceContainer::DrawRePlacePiecePickAlpha( pd3dDevice );		// Note : Prev PieceManager
	m_spPieceManager->RenderPickAlpha
	( 
		pd3dDevice, 
		m_spStaticMeshLightMap.get(), 
		NULL,
		bReflect, 
		fReflectHeight,
		m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL]
	);	// Note : Recent PieceManager

	// VisualMaterial�� Translucent ���͸��� ������
	NSTexEffVisualMaterial::Render( pd3dDevice, &NSVMRender::g_sTranslucent );

	NSOcclusionCulling::g_bRendering = bOcclusionCullingRendering;

	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	dwBiasPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	dwFogStartPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGEND,	dwFogEndPrev );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );

		NSMaterialManager::SetFogColorForEditor( dwFogColor );
	}
}
// TnL_PS_1_1 ���Ͽ����� ȣ�� �ȴ�.
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// Deffered ��,��,�ֻ� �ɼ�	
// DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_DF && m_bUsedMaterialSystem ��ܿ��� Ȯ���ʿ�.
HRESULT DxLandMan::Render_Opaque_SM30DF( LPDIRECT3DDEVICEQ pd3dDevice, 
										const CLIPVOLUME &cv, 
										NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
										BOOL bReflect,
										float fReflectHeight,
										LPDIRECT3DTEXTUREQ pColorTex_1st,
										LPDIRECT3DTEXTUREQ pColorTex_2nd, 
										LPDIRECT3DTEXTUREQ pColorTex_3rd,
										LPDIRECT3DTEXTUREQ pColorTex_4th,
										const D3DXMATRIX& matWorld,
										const D3DXMATRIX& matView,
										const D3DXMATRIX& matProj,
										BOOL bMaterialFrameMove )
{
	HRESULT hr = S_OK;

	if ( (g_emTnLMode_Prev != RENDERPARAM::g_emGroundQulity) )
	{
		g_emTnLMode_Prev = RENDERPARAM::g_emGroundQulity;
		Active_MATERIAL_DATA( pd3dDevice, FALSE );
	}

	if ( bMaterialFrameMove )
	{
		NSMaterialManager::FrameMove
		( 
			matView, 
			matProj, 
			pColorTex_1st,
			pColorTex_2nd, 
			pColorTex_3rd,
			pColorTex_4th
		);
	}

	PROFILE_BEGIN("DxLandMan::Render");

	DWORD dwFogColor(0L);
	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
		NSMaterialManager::SetFogColorForEditor( 0xff000000 );
	}

	//	Note : ���� Ʈ������ �ʱ�ȭ.
	//
	pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );

	// �ӵ�üũ�� �ȵ�. FALSE �ϸ� �ȵ�.
	//COLLISION::bCollisionCheck = bFrustom;

	const VEC_DWORD* pvecPieceIndex(NULL);
	if ( NSOcclusionCulling::IsActive() )
	{
		const VEC_DWORD* pvecOctreeIndex(NULL);
		NSOcclusionCulling::GetRenderIndex( DxViewPort::GetInstance().GetLookatPt(), &pvecPieceIndex, &pvecOctreeIndex );
		NSOcclusionOctree::InsertIndex( pvecOctreeIndex );
	}

	DWORD dwFogEnable(FALSE);
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	// mat �� ����
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->RenderOrderSetting( pd3dDevice, cv );
		m_spStaticMaterialMeshLightMap->Render( pd3dDevice, emCullingRenderType, FALSE, matWorld );
		m_spStaticMaterialMeshLightMap->RenderHardAlpha( pd3dDevice, emCullingRenderType, FALSE, matWorld );
	}

	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff00ffff );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );

		NSMaterialManager::SetFogColorForEditor( 0xff00ffff );
	}

	//NSPieceQuickSort::InitPickAlpha();		// Note : Init Static
	//NSPieceQuickSort::Reset();

	////	Note : ��Ų ������Ʈ ���
	//{
	//	const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
	//	const D3DXVECTOR3& vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	//	D3DXVECTOR3 vDir = vFromPt - vLookatPt;
	//	float fLengthSq = D3DXVec3LengthSq( &vDir );
	//	COLLISION::InsertData( m_pLandSkinObjTree, cv, vFromPt, vLookatPt, fLengthSq );
	//}

	// Note : ���� ���� �Ŵ���
	//m_spPieceManager->SetRenderOrder( pd3dDevice, TRUE );
	m_spPieceManager->RenderOpaque
	( 
		pd3dDevice, 
		//DxViewPort::GetInstance().GetFromPt(), 
		//DxViewPort::GetInstance().GetLookatPt(),
		pvecPieceIndex,
		FALSE, 
		m_bUsedMaterialSystem,
		bReflect,
		fReflectHeight,
		m_spEntityRenderListSwap[emCullingRenderType]
	);
	m_spPieceManager->RenderHardAlpha( pd3dDevice, 1.f, FALSE, m_bUsedMaterialSystem, bReflect, m_spEntityRenderListSwap[emCullingRenderType] );

	// õ ������ �ݻ綧 ������ ���� ����.
	//if ( !bReflect )
	{
		m_spPieceManager->RenderEffFrameForDeffered( FALSE, bReflect, m_spEntityRenderListSwap[emCullingRenderType] );
	}

	//m_spEntityRenderListSwap->RenderOpaque( pd3dDevice, FALSE, m_bUsedMaterialSystem, FALSE, 0.f, TRUE );
	//m_spEntityRenderListSwap->RenderHardAlpha( pd3dDevice, FALSE, m_bUsedMaterialSystem, TRUE );


	if ( g_bWIREFRAME_VIEW )
	{
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xff000000 );
		NSMaterialManager::SetFogColorForEditor( 0xff000000 );
	}

		
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );


	if ( g_bWIREFRAME_VIEW )
	{
		NSMaterialManager::SetFogColorForEditor( dwFogColor );
	}

	PROFILE_END("DxLandMan::Render");
	return S_OK;
}

void DxLandMan::RenderMS_FrameAnimation_Will_Erase( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
{
	//////////////////////////////////////////////////////////////////////////
	//		�ʱ� Material System ���� �����ߴ� Frame�ִϸ��̼� ������Ʈ
	//				��¿ �� ���� ����־� �Ѵ�.
	//			������ ���ʿ����� �� �� ���� ��.
	if ( m_bUsedMaterialSystem && !m_bUsedMaterialSystemPS_3_0 )
	{
		//	Note : ���ϸ��̼� �ִ� ������Ʈ ������.
		//
//		COLLISION::dwRendAABB = 0;
		COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pAniManTree, FALSE );
	}
}

HRESULT DxLandMan::RenderDecal_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, 
										NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
										BOOL bReflect, 
										float fReflectHeight, 
										const D3DXMATRIX& matWorld )
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->RenderDecal( pd3dDevice, emCullingRenderType, matIdentity );
	}
	m_spPieceManager->RenderDecalAndLodBlendDecal( pd3dDevice, bReflect, m_spEntityRenderListSwap[emCullingRenderType] );

	return S_OK;
}

HRESULT DxLandMan::RenderWater_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, 
										NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{	
	//NSPieceQuickSort::RenderSkinObject( FALSE, 0.f,		TRUE );

	// - water ������.
	// mat �� ����
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
		m_spStaticMaterialMeshLightMap->RenderWater_CustomSoftAlpha( pd3dDevice, emCullingRenderType );
	}

	//m_spEntityRenderListSwap[emCullingRenderType]->RenderSkinObject( FALSE, 0.f, TRUE );

	//m_spPieceManager->RenderWater_CustomSoftAlpha( pd3dDevice, m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );
	//m_spEntityRenderListSwap[emCullingRenderType]->RenderWater_CustomSoftAlpha( pd3dDevice );

	return S_OK;
}

HRESULT DxLandMan::Render_SoftAlpha_SM30DF( LPDIRECT3DDEVICEQ pd3dDevice, 
										   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
										   BOOL bReflect, 
										   float fReflectHeight, 
										   const D3DXMATRIX& matWorld )
{
	HRESULT hr = S_OK;

	PROFILE_BEGIN("DxLandMan::Render_EFF");

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	{
		// mat �� ����
		if ( m_spStaticMaterialMeshLightMap.get() )
		{
			m_spStaticMaterialMeshLightMap->RenderSoftAlpha_Deffered( pd3dDevice, emCullingRenderType, matIdentity );
		}

		m_spPieceManager->RenderSoftAlpha_Deffered( pd3dDevice, m_spEntityRenderListSwap[emCullingRenderType] );

		//m_spEntityRenderListSwap[emCullingRenderType]->RenderSoftAlpha( pd3dDevice, TRUE, FALSE, m_bUsedMaterialSystem );
		//m_spEntityRenderListSwap->RenderPickAlpha( )
	}
	PROFILE_END("DxLandMan::Render_EFF");

	return S_OK;
}

void DxLandMan::Render_Opaque_Spot_LBuffer( LPDIRECT3DDEVICEQ pd3dDevice, 
										   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	const VEC_DWORD* pvecPieceIndex(NULL);
	if ( NSOcclusionCulling::IsActive() )
	{
		const VEC_DWORD* pvecOctreeIndex(NULL);
		NSOcclusionCulling::GetRenderIndex( DxViewPort::GetInstance().GetLookatPt(), &pvecPieceIndex, &pvecOctreeIndex );
	}

	DWORD dwFogEnable(FALSE);
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	// mat �� ����
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		m_spStaticMaterialMeshLightMap->Render( pd3dDevice, emCullingRenderType, TRUE, matIdentity );
		m_spStaticMaterialMeshLightMap->RenderHardAlpha( pd3dDevice, emCullingRenderType, TRUE, matIdentity );
	}

	// Note : ���� ���� �Ŵ���
	//m_spPieceManager->SetRenderOrder( pd3dDevice, TRUE );
	m_spPieceManager->RenderOpaque
	( 
		pd3dDevice,
		//cv, 
		//DxViewPort::GetInstance().GetFromPt(), 
		//DxViewPort::GetInstance().GetLookatPt(),
		pvecPieceIndex,
		TRUE, 
		TRUE,
		FALSE,
		0.f, 
		m_spEntityRenderListSwap[emCullingRenderType]
	);
	m_spPieceManager->RenderHardAlpha( pd3dDevice, 1.f, TRUE, m_bUsedMaterialSystem, FALSE, m_spEntityRenderListSwap[emCullingRenderType] );
	m_spPieceManager->RenderEffFrameForDeffered( TRUE, FALSE, m_spEntityRenderListSwap[emCullingRenderType] );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
}

void DxLandMan::Render_SoftAlpha_Spot_LBuffer( LPDIRECT3DDEVICEQ pd3dDevice, 
											  NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	//D3DXMATRIX matIdentity;
	//D3DXMatrixIdentity( &matIdentity );

	//{
	//	DWORD dwZWriteEnable(FALSE);
	//	DWORD dwFogEnable(FALSE);
	//	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,	&dwZWriteEnable );
	//	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,		&dwFogEnable );

	//	// Z �� �ʿ� ����.
	//	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,	FALSE );


	//	// mat �� ����
	//	if ( m_spStaticMaterialMeshLightMap.get() )
	//	{
	//		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,		FALSE );
	//		m_spStaticMaterialMeshLightMap->RenderSoftAlpha_Deffered( pd3dDevice, cv, matIdentity );
	//		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,		dwFogEnable );
	//	}

	//	if ( g_bWIREFRAME_VIEW )
	//	{
	//		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,	0xff00ffff );
	//		NSMaterialManager::SetFogColorForEditor( 0xff00ffff );
	//	}

	//	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,		FALSE );
	//	m_spPieceManager->RenderSoftAlpha_Deffered( pd3dDevice );
	//	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,		dwFogEnable );

	//	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,	dwZWriteEnable );
	//}
}

HRESULT DxLandMan::RenderEff_Deffered( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
{
	m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL]->RenderSkinObject( FALSE, 0.f );

	// PickBlend �Ǵ� Piece�� Rendering
	m_spPieceManager->RenderEff_PickBlend_Deffered( pd3dDevice, m_spStaticMeshLightMap.get(), m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );

	// Note : ���� ���� �Ŵ���
	m_spPieceManager->RenderEff( pd3dDevice, m_spStaticMeshLightMap.get(), NULL, 1.f, TRUE, m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );

	//	Note : �ܵ� ����Ʈ ( single effect ) ������.
	//
//	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );

	return S_OK;
}

void DxLandMan::RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice,
								  NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
								  BOOL bReflect, 
								  float fReflectHeight, 
								  const D3DXMATRIX& matWorld )
{
	// Note : Camera Pick Object    AlphaBlending
	m_spPieceManager->RenderForLodBlend( pd3dDevice, m_spEntityRenderListSwap[emCullingRenderType], bReflect, fReflectHeight );
}

void DxLandMan::RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice, 
										   NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
										   BOOL bReflect, 
										   float fReflectHeight, 
										   const D3DXMATRIX& matWorld )
{
	// Note : Camera Pick Object    AlphaBlending
	m_spPieceManager->RenderSoftAlphaForLodBlend( pd3dDevice, m_spEntityRenderListSwap[emCullingRenderType] );
}

void DxLandMan::RenderDecalPass2( LPDIRECT3DDEVICEQ pd3dDevice, 
								 NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType,
								 BOOL bReflect )
{
	if ( bReflect )
	{
		m_spEntityRenderListSwap[emCullingRenderType]->RenderSkinObject( bReflect, GetRealTimeWaterHeight() );
	}


	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	DWORD dwFogColor(0L);
	DWORD dwZWriteEnable(FALSE);
	//DWORD dwFogEnable(FALSE);
	pd3dDevice->GetRenderState( D3DRS_FOGCOLOR,		&dwFogColor );
	pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,	&dwZWriteEnable );
	//pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,		&dwFogEnable );

	pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,		0 );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );

	// mat �� ����
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->RenderDecalPass2( pd3dDevice, emCullingRenderType, matIdentity );
	}

	m_spPieceManager->RenderDecalPass2( pd3dDevice, bReflect, m_spEntityRenderListSwap[emCullingRenderType] );

	pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,		dwFogColor );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	dwZWriteEnable );
	//pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,		dwFogEnable );

//	//////////////////////////////////////////////////////////////////////////
//	//	Note : ��Ų ������Ʈ ������.	
//	//			���۾� �ʿ伺�� �ִ�.
//	PROFILE_BEGIN("DxLandMan::Render-m_pLandSkinObjTree");
////	COLLISION::dwRendAABB = 0;
//	if ( bReflect )
//	{
//		COLLISION::RenderAABBTreeReflect ( pd3dDevice, &cv, m_pLandSkinObjTree, GetRealTimeWaterHeight() );
//	}
//	else
//	{
//		COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandSkinObjTree, FALSE );
//	}
//	PROFILE_END("DxLandMan::Render-m_pLandSkinObjTree");
}
// Deffered ��,��,�ֻ� �ɼ�
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// CascadeShadowMap �� SpotShadowMap �����ڵ�
void DxLandMan::RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// WorldEdit ���� Direction Light�� Shadow�� �Ⱦ��ڴٰ� �Ͽ� CascadeShadow �� ��� �Ұ���.
	if ( !NSShadowLight::IsCascadeShadow() )
	{
		return;
	}

	// ���� Qulity �ɼǿ����� CascadeShadow �� ���� ���� ����.
	// ������ ���� ���̶� ���� �� ���� ���� �ְ�,
	// �ɼ��� ���缭 ���� �� ���� ���� �ִ�.
	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
	{
		// Shadow�� View��ġ�� ���������� �����Ͽ� �������ؾ� �ϹǷ� FrameMove �� �� �����.
		// �̰� ������ ������ g_vLightPosWorldSpace ���� ������ �ȵǿ� ���� Frame ���� ������ �ȴ�.
		NSMaterialManager::FrameMove
		( 
			DxViewPort::GetInstance().GetMatView(), 
			DxViewPort::GetInstance().GetMatProj(), 
			NULL, 
			NULL, 
			NULL, 
			NULL 
		);

		NSOcclusionQueryFastRender::g_bRendering = TRUE;

		DWORD dwCullModePrev, dwFogEnable;
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,		&dwCullModePrev );
		pd3dDevice->GetRenderState( D3DRS_FOGENABLE,	&dwFogEnable );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	FALSE );

		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );	

		// ����� ���� _ 2048
		{
			pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_CSM );

			const CLIPVOLUME& sCV = DxShadowMap::GetInstance().GetClipVolume();
			const D3DXMATRIX& matVP = DxShadowMap::GetInstance().GetMatrix_D();

			NSShadowRenderForMaterial::SetBaseFX( matVP );

			if ( NSShadowRenderForMaterial::FX_Begin() )
			{
				if ( m_spStaticMaterialMeshLightMap.get() )
				{
					m_spStaticMaterialMeshLightMap->RenderCascadeShadow( pd3dDevice, NSLandThread::TYPE_SHADOW_FIRST_CULL );
				}

				m_spPieceManager->RenderCascadeShadow
				( 
					pd3dDevice, 
					NSLandThread::TYPE_SHADOW_FIRST_CULL, 
					m_spEntityRenderListSwap[NSLandThread::TYPE_SHADOW_FIRST_CULL] 
				);

				NSShadowRenderForMaterial::FX_End();
			}


			//DxShadowMap::GetInstance().LastImageBlurForVSM
			//( 
			//	pd3dDevice, 
			//	DxSurfaceTex::GetInstance().m_pShadowTexR32F_CSM, 
			//	DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM,
			//	DxSurfaceTex::GetInstance().m_pShadowTexR32F_2048, 
			//	DxSurfaceTex::GetInstance().m_pShadowSufR32F_2048,
			//	1024
			//);

			//DxShadowMap::GetInstance().LastImageBlurForCascadeShadow
			//( 
			//	pd3dDevice, 
			//	DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM, 
			//	DxSurfaceTex::GetInstance().m_pShadowTexR32F_2048, 
			//	static_cast<DWORD>( DxSurfaceTex::CASCADESHADOWSIZE.x ) 
			//);
		}

		// Mode ����.
		NSShadowRenderForMaterial::SetMode_G32R32F();

		// �� ���� _ 1024
		{
			pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufG32R32F_VSM_CSM2 );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_1024 );
			HRESULT hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

			const CLIPVOLUME& sCV = DxShadowMap::GetInstance().GetClipVolume_2nd();
			const D3DXMATRIX& matVP = DxShadowMap::GetInstance().GetMatrix_D_2nd();

			NSShadowRenderForMaterial::SetBaseFX( matVP );

			if ( NSShadowRenderForMaterial::FX_Begin() )
			{
				if ( m_spStaticMaterialMeshLightMap.get() )
				{
					m_spStaticMaterialMeshLightMap->RenderCascadeShadow( pd3dDevice, NSLandThread::TYPE_SHADOW_SECOND_CULL );
				}

				m_spPieceManager->RenderCascadeShadow
				( 
					pd3dDevice, 
					NSLandThread::TYPE_SHADOW_SECOND_CULL, 
					m_spEntityRenderListSwap[NSLandThread::TYPE_SHADOW_SECOND_CULL] 
				);

				//m_spPieceManager->RenderCascadeShadow( pd3dDevice, sCV );

				NSShadowRenderForMaterial::FX_End();
			}

			//DxShadowMap::GetInstance().LastImageBlurForVSM
			//( 
			//	pd3dDevice, 
			//	DxSurfaceTex::GetInstance().m_pShadowTexG32R32F, 
			//	DxSurfaceTex::GetInstance().m_pShadowSufG32R32F,
			//	DxSurfaceTex::GetInstance().m_pShadowTexG32R32F_1024, 
			//	DxSurfaceTex::GetInstance().m_pShadowSufG32R32F_1024,
			//	1024
			//);

			//DxShadowMap::GetInstance().LastImageBlurForCascadeShadow ( pd3dDevice, DxSurfaceTex::GetInstance().m_pShadowSufR32F, DxSurfaceTex::GetInstance().m_pShadowTexR32F_1024, 1024 );
		}

		// Mode ���������� ���̴� R32F �� �ǵ���.
		NSShadowRenderForMaterial::SetMode_R32F();

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	dwFogEnable );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,		dwCullModePrev );

		NSOcclusionQueryFastRender::g_bRendering = FALSE;
	}
	else if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
	{
		// Shadow�� View��ġ�� ���������� �����Ͽ� �������ؾ� �ϹǷ� FrameMove �� �� �����.
		// �̰� ������ ������ g_vLightPosWorldSpace ���� ������ �ȵǿ� ���� Frame ���� ������ �ȴ�.
		NSMaterialManager::FrameMove
		( 
			DxViewPort::GetInstance().GetMatView(), 
			DxViewPort::GetInstance().GetMatProj(), 
			NULL, 
			NULL, 
			NULL, 
			NULL 
		);

		NSOcclusionQueryFastRender::g_bRendering = TRUE;

		DWORD dwCullModePrev, dwFogEnable;
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,		&dwCullModePrev );
		pd3dDevice->GetRenderState( D3DRS_FOGENABLE,	&dwFogEnable );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	FALSE );

		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice );	

		//// Mode ����.
		//if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 )
		//{
		//	NSShadowRenderForMaterial::SetMode_G32R32F();
		//}

		{
			pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_1024 );

			const CLIPVOLUME& sCV = DxShadowMap::GetInstance().GetClipVolume();
			const D3DXMATRIX& matVP = DxShadowMap::GetInstance().GetMatrix_D();

			NSShadowRenderForMaterial::SetBaseFX( matVP );

			if ( NSShadowRenderForMaterial::FX_Begin() )
			{
				if ( m_spStaticMaterialMeshLightMap.get() )
				{
					m_spStaticMaterialMeshLightMap->RenderCascadeShadow( pd3dDevice, NSLandThread::TYPE_SHADOW_FIRST_CULL );
				}

				m_spPieceManager->RenderCascadeShadow
				( 
					pd3dDevice, 
					NSLandThread::TYPE_SHADOW_FIRST_CULL, 
					m_spEntityRenderListSwap[NSLandThread::TYPE_SHADOW_FIRST_CULL] 
				);

				//m_spPieceManager->RenderCascadeShadow( pd3dDevice, sCV );

				NSShadowRenderForMaterial::FX_End();
			}


			//DxShadowMap::GetInstance().LastImageBlurForVSM
			//( 
			//	pd3dDevice, 
			//	DxSurfaceTex::GetInstance().m_pShadowTexR32F_CSM, 
			//	DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM,
			//	DxSurfaceTex::GetInstance().m_pShadowTexR32F_2048, 
			//	DxSurfaceTex::GetInstance().m_pShadowSufR32F_2048,
			//	1024
			//);

			//DxShadowMap::GetInstance().LastImageBlurForCascadeShadow
			//( 
			//	pd3dDevice, 
			//	DxSurfaceTex::GetInstance().m_pShadowSufR32F, 
			//	DxSurfaceTex::GetInstance().m_pShadowTexR32F_1024, 
			//	static_cast<DWORD>( DxSurfaceTex::CASCADESHADOWSIZE.z )  
			//);
		}

		//// Mode ���������� ���̴� R32F �� �ǵ���.
		//NSShadowRenderForMaterial::SetMode_R32F();

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	dwFogEnable );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,		dwCullModePrev );

		NSOcclusionQueryFastRender::g_bRendering = FALSE;
	}
}

void DxLandMan::RenderShadow_SpotLight1( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// ���� Qulity �ɼǿ����� CascadeShadow �� ���� ���� ����.
	// ������ ���� ���̶� ���� �� ���� ���� �ְ�,
	// �ɼ��� ���缭 ���� �� ���� ���� �ִ�.
	if ( RENDERPARAM::g_emGroundQulity < TnL_PS_3_0_CSM )
	{
		return;
	}

	// ���� ��ġ������ SpotLightShadow �� �۵����� �ʴ´�.
	if ( DxShadowMap::GetInstance().GetSpotLightID_PL1() == static_cast<DWORD>(-1) )
	{
		return;
	}

	NSOcclusionQueryFastRender::g_bRendering = TRUE;

	DWORD dwCullModePrev, dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_CULLMODE,		&dwCullModePrev );
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE,	&dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	FALSE );

	DxBackupTarget sBackupTarget;
	sBackupTarget.Backup( pd3dDevice );

	// ZBuffer ����
	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_PL1 );

	// PL1
	{
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F_1024 );
		HRESULT hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

		const CLIPVOLUME& sCV = DxShadowMap::GetInstance().GetClipVolume_PL1();
		const D3DXMATRIX& matVP = DxShadowMap::GetInstance().GetMatrix_PL1();

		NSShadowRenderForMaterial::SetBaseFX( matVP );

		if ( NSShadowRenderForMaterial::FX_Begin() )
		{
			if ( m_spStaticMaterialMeshLightMap.get() )
			{
				m_spStaticMaterialMeshLightMap->RenderCascadeShadow( pd3dDevice, NSLandThread::TYPE_SPOTSHADOW_FIRST_CULL );
			}

			//m_spPieceManager->RenderCascadeShadow( pd3dDevice, sCV );

			m_spPieceManager->RenderCascadeShadow
			( 
				pd3dDevice, 
				NSLandThread::TYPE_SPOTSHADOW_FIRST_CULL, 
				m_spEntityRenderListSwap[NSLandThread::TYPE_SPOTSHADOW_FIRST_CULL] 
			);

			NSShadowRenderForMaterial::FX_End();
		}

		DxShadowMap::GetInstance().LastImageBlurForCascadeShadow ( pd3dDevice, DxSurfaceTex::GetInstance().m_pShadowSufR32F_PL1, DxSurfaceTex::GetInstance().m_pShadowTexR32F_1024, 1024 );
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,		dwCullModePrev );

	NSOcclusionQueryFastRender::g_bRendering = FALSE;
}

void DxLandMan::RenderShadow_SpotLight2( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// ���� Qulity �ɼǿ����� CascadeShadow �� ���� ���� ����.
	// ������ ���� ���̶� ���� �� ���� ���� �ְ�,
	// �ɼ��� ���缭 ���� �� ���� ���� �ִ�.
	if ( RENDERPARAM::g_emGroundQulity < TnL_PS_3_0_CSM )
	{
		return;
	}

	// ���� ��ġ������ SpotLightShadow �� �۵����� �ʴ´�.
	if ( DxShadowMap::GetInstance().GetSpotLightID_PL2() == static_cast<DWORD>(-1) )
	{
		return;
	}

	NSOcclusionQueryFastRender::g_bRendering = TRUE;

	DWORD dwCullModePrev, dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_CULLMODE,		&dwCullModePrev );
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE,	&dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	FALSE );

	DxBackupTarget sBackupTarget;
	sBackupTarget.Backup( pd3dDevice );

	// ZBuffer ����
	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_PL2 );

	// PL2
	{
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F_1024 );
		HRESULT hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

		const CLIPVOLUME& sCV = DxShadowMap::GetInstance().GetClipVolume_PL2();
		const D3DXMATRIX& matVP = DxShadowMap::GetInstance().GetMatrix_PL2();

		NSShadowRenderForMaterial::SetBaseFX( matVP );

		if ( NSShadowRenderForMaterial::FX_Begin() )
		{
			if ( m_spStaticMaterialMeshLightMap.get() )
			{
				m_spStaticMaterialMeshLightMap->RenderCascadeShadow( pd3dDevice, NSLandThread::TYPE_SPOTSHADOW_SECOND_CULL );
			}

			//m_spPieceManager->RenderCascadeShadow( pd3dDevice, sCV );

			m_spPieceManager->RenderCascadeShadow
			( 
				pd3dDevice, 
				NSLandThread::TYPE_SPOTSHADOW_SECOND_CULL, 
				m_spEntityRenderListSwap[NSLandThread::TYPE_SPOTSHADOW_SECOND_CULL] 
			);

			NSShadowRenderForMaterial::FX_End();
		}

		DxShadowMap::GetInstance().LastImageBlurForCascadeShadow ( pd3dDevice, DxSurfaceTex::GetInstance().m_pShadowSufR32F_PL2, DxSurfaceTex::GetInstance().m_pShadowTexR32F_1024, 1024 );
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,		dwCullModePrev );

	NSOcclusionQueryFastRender::g_bRendering = FALSE;
}
// CascadeShadowMap �� SpotShadowMap �����ڵ�
//////////////////////////////////////////////////////////////////////////



void DxLandMan::RenderStarGlow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	// VisualMaterial�� StarGlow ���͸��� ������
	NSTexEffVisualMaterial::Render( pd3dDevice, &NSVMRender::g_sStarGlow );
}

// �ǽð� �ݻ� ��ġ�� ���Ѵ�.
// ���� FLT_MAX �� ���� �۵����� �ʴ´�.
float DxLandMan::GetRealTimeWaterHeight()
{
	if ( !m_spStaticMaterialMeshLightMap.get() )
		return FLT_MAX;

	return m_spStaticMaterialMeshLightMap->GetRealTimeWaterHeight();
}

HRESULT	DxLandMan::Render_ReflectOLD( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, float fHeight, const D3DXMATRIX& matReflectVP )
{
	HRESULT hr = S_OK;

	if( DxSurfaceTex::GetInstance().GetMaxUserClipPlanes() <= 0 )
		return S_OK;

	D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

	// Clip Plane ����
	{
		D3DXPLANE sPlane;
		sPlane.a = 0.f;
		sPlane.b = 1.f;
		sPlane.c = 0.f;
		sPlane.d = -vCenter.y;
		D3DXPlaneNormalize( &sPlane, &sPlane );

		//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	D3DCLIPPLANE0 );
		//pd3dDevice->SetClipPlane( 0, sPlane );

		//// OFF
		//// Note : Clip Plane Shader��
		//D3DXPLANE sPlane_Shader;
		//D3DXPlaneNormalize( &sPlane, &sPlane );
		//D3DXMATRIXA16 mViewProjection;
		//D3DXMATRIXA16 mViewProjectionInvTrans;
		//mViewProjection = matReflectVP;
		////D3DXMatrixMultiply( &mViewProjection, &DxViewPort::GetInstance().GetMatView(), &DxViewPort::GetInstance().GetMatProj() );
		//D3DXMatrixInverse( &mViewProjectionInvTrans, 0, &mViewProjection );
		//D3DXMatrixTranspose( &mViewProjectionInvTrans, &mViewProjectionInvTrans );
		//D3DXPlaneTransform( &sPlane_Shader, &sPlane, &mViewProjectionInvTrans );

		//pd3dDevice->SetClipPlane( 0, sPlane_Shader );

		NSENVIRONMENT::SetReflectONOFF( pd3dDevice, TRUE );
		NSENVIRONMENT::SetReflect( sPlane );
		NSENVIRONMENT::SetReflectShader( FALSE ,sPlane );
		NSENVIRONMENT::SetReflectClip( pd3dDevice );
	}

    // ����
    {
        //DxSkyBoxMan::GetInstance().Render( pd3dDevice, &matWorld );

	    //	Note : �⺻ ���� ������.
	    {
		    DxLightMan::GetInstance()->ReSetLight( pd3dDevice );

		    m_DxOctree.Render_NOEFF ( cv, m_SFileMap, pd3dDevice, m_pEffectFrameList );

		    DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
	    }

	    //	Note : Optimum Octree
	    //
	    if ( m_spStaticMeshLightMap.get() )
	    {
		    D3DMATERIALQ	sMaterial;
		    sMaterial.Diffuse.r = 1.f;
		    sMaterial.Diffuse.g = 1.f;
		    sMaterial.Diffuse.b = 1.f;
		    sMaterial.Diffuse.a = 1.f;

		    sMaterial.Ambient.r = 1.f;
		    sMaterial.Ambient.g = 1.f;
		    sMaterial.Ambient.b = 1.f;
		    sMaterial.Ambient.a = 1.f;

		    sMaterial.Specular.r = 1.f;
		    sMaterial.Specular.g = 1.f;
		    sMaterial.Specular.b = 1.f;
		    sMaterial.Specular.a = 1.f;

		    sMaterial.Emissive.r = 0.f;
		    sMaterial.Emissive.g = 0.f;
		    sMaterial.Emissive.b = 0.f;
		    sMaterial.Emissive.a = 0.f;

		    sMaterial.Power	= 1.f;

		    pd3dDevice->SetMaterial ( &sMaterial );

		    m_spStaticMeshLightMap->Render_THREAD_ ( pd3dDevice, NSLandThread::TYPE_REFLECTION_CULL, cv );
	    }

	    //	Note : ���� �� �Ӽ��� Frame ������.
	    //
	    DrawAlphaMap ( pd3dDevice, &cv );
    }

    // ����
    {
	    //	Note : ġȯ ������Ʈ ������.
	    //			�ݻ������� FALSE�� �ذ��� �ݻ� ����� Ʋ���� �׷���.
	    float fDistance = DxViewPort::GetInstance().GetDistance();
	    fDistance = fDistance*fDistance;
	    DxReplaceContainer::EMREFLECTOPTION emRelflect = DxReplaceContainer::EM_REFLECT_OLD;
	    DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, &cv, emRelflect, fDistance );

		// Note : ���� ���� �Ŵ���
		const VEC_DWORD* pvecPieceIndex(NULL);
		m_spPieceManager->RenderOpaque
		( 
			pd3dDevice, 
			pvecPieceIndex,
			FALSE, 
			m_bUsedMaterialSystem,
			FALSE,
			vCenter.y,
			m_spEntityRenderListSwap[NSLandThread::TYPE_REFLECTION_CULL]
		);
		m_spPieceManager->RenderHardAlpha( pd3dDevice, 1.f, FALSE, m_bUsedMaterialSystem, FALSE, m_spEntityRenderListSwap[NSLandThread::TYPE_REFLECTION_CULL] );
		m_spPieceManager->RenderSoftAlpha( pd3dDevice, 1.f, FALSE, m_bUsedMaterialSystem, m_spEntityRenderListSwap[NSLandThread::TYPE_REFLECTION_CULL] );

        // VisualMaterial�� Translucent ���͸��� ������
        NSTexEffVisualMaterial::Render( pd3dDevice, &NSVMRender::g_sTranslucent );
    }

	//	Note : �ܵ� ����Ʈ ( single effect ) ������.
	//
//	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );

	// Clip Plane ����
	//pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	0 );
	NSENVIRONMENT::SetReflectONOFF( pd3dDevice, FALSE );

	return hr;
}

HRESULT	DxLandMan::Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;

	// �� ������������ ���ؼ� ������ ����.
	DWORD	dwCullMode;
	pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );	

	D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

	// Think : �⺻ ������ Matrix�� Identity �̰�, �� ���̰� vCenter.y �̰�, 
	//		������ ������ �� ���̺��� -vCenter.y�� �־�����,
	//		������ �Ŀ��� �� ���̺��� +vCenter.y�� �־�� �Ѵ�.
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	matWorld._42 = -(vCenter.y*2.f);	
	D3DXMatrixMultiply_MIRROR( matWorld );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

    // ����
    {
	    //	Note : �⺻ ���� ������.
	    {
		    DxLightMan::GetInstance()->ReSetLight( pd3dDevice );

		    m_DxOctree.Render_NOEFF ( cv, m_SFileMap, pd3dDevice, m_pEffectFrameList );

		    DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
	    }

	    //	Note : Optimum Octree
	    if ( m_spStaticMeshLightMap.get() )
	    {
		    D3DMATERIALQ	sMaterial;
		    sMaterial.Diffuse.r = 1.f;
		    sMaterial.Diffuse.g = 1.f;
		    sMaterial.Diffuse.b = 1.f;
		    sMaterial.Diffuse.a = 1.f;

		    sMaterial.Ambient.r = 1.f;
		    sMaterial.Ambient.g = 1.f;
		    sMaterial.Ambient.b = 1.f;
		    sMaterial.Ambient.a = 1.f;

		    sMaterial.Specular.r = 1.f;
		    sMaterial.Specular.g = 1.f;
		    sMaterial.Specular.b = 1.f;
		    sMaterial.Specular.a = 1.f;

		    sMaterial.Emissive.r = 0.f;
		    sMaterial.Emissive.g = 0.f;
		    sMaterial.Emissive.b = 0.f;
		    sMaterial.Emissive.a = 0.f;

		    sMaterial.Power	= 1.f;

		    pd3dDevice->SetMaterial( &sMaterial );

		    pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	// m_pStaticMesh�� Diffuse Color�� ����ϹǷ� Light�� ����.

		    m_spStaticMeshLightMap->Render_THREAD_( pd3dDevice, NSLandThread::TYPE_REFLECTION_CULL, cv );
	    }

	    //	Note : ���� �� �Ӽ��� Frame ������.
	    DrawAlphaMap( pd3dDevice, &cv, TRUE );
    }

    // ����
    {
	    //	Note : ġȯ ������Ʈ ������.
	    float fDistance = DxViewPort::GetInstance().GetDistance();
	    fDistance = fDistance*fDistance;
	    DxReplaceContainer::EMREFLECTOPTION emReflect = DxReplaceContainer::EM_REFLECT;
	    DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, &cv, emReflect, fDistance );

	    // Note : ���� ���� �Ŵ���
		const VEC_DWORD* pvecPieceIndex(NULL);
		m_spPieceManager->RenderOpaque
		( 
			pd3dDevice, 
			pvecPieceIndex,
			FALSE, 
			m_bUsedMaterialSystem,
			TRUE,
			vCenter.y,
			m_spEntityRenderListSwap[NSLandThread::TYPE_REFLECTION_CULL]
		);
		m_spPieceManager->RenderHardAlpha( pd3dDevice, 1.f, FALSE, m_bUsedMaterialSystem, TRUE, m_spEntityRenderListSwap[NSLandThread::TYPE_REFLECTION_CULL] );
		m_spPieceManager->RenderSoftAlpha( pd3dDevice, 1.f, FALSE, m_bUsedMaterialSystem, m_spEntityRenderListSwap[NSLandThread::TYPE_REFLECTION_CULL] );

        // VisualMaterial�� Translucent ���͸��� ������
        NSTexEffVisualMaterial::Render( pd3dDevice, &NSVMRender::g_sTranslucent );
    }

	pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );

	//	Note : �ܵ� ����Ʈ ( single effect ) ������.
//	COLLISION::dwRendAABB = 0;
	COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandEffTree, FALSE );

	return hr;
}

//HRESULT DxLandMan::Render_NOSKIN ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
//{
//	HRESULT hr = S_OK;
//
//	PROFILE_BEGIN("DxLandMan::Render");
//
//	OctreeDebugInfo::TotalNodesDrawn = 0;
//	OctreeDebugInfo::FaceViewCount = 0;
//	OctreeDebugInfo::VertexViewCount = 0;
//	OctreeDebugInfo::TodalDxFrameDrawn = 0;
//
//	//	Note : ���� Ʈ������ �ʱ�ȭ.
//	//
//	D3DXMATRIX matIdentity;
//	D3DXMatrixIdentity( &matIdentity );
//	pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
//	
//	//	Note : �⺻ ���� ������.
//	//
//	PROFILE_BEGIN("DxOctree::Render");
//	{
//		DxLightMan::GetInstance()->ReSetLight( pd3dDevice );
//
//		m_DxOctree.Render ( cv, m_SFileMap, pd3dDevice, m_pEffectFrameList, this );
//
//		DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
//	}
//	PROFILE_END("DxOctree::Render");
//
//	//	Note : Optimum Octree
//	//
//	if ( m_spStaticMeshLightMap.get() )
//	{
//		D3DMATERIALQ	sMaterial;
//		sMaterial.Diffuse.r = 1.f;
//		sMaterial.Diffuse.g = 1.f;
//		sMaterial.Diffuse.b = 1.f;
//		sMaterial.Diffuse.a = 1.f;
//
//		sMaterial.Ambient.r = 1.f;
//		sMaterial.Ambient.g = 1.f;
//		sMaterial.Ambient.b = 1.f;
//		sMaterial.Ambient.a = 1.f;
//
//		sMaterial.Specular.r = 1.f;
//		sMaterial.Specular.g = 1.f;
//		sMaterial.Specular.b = 1.f;
//		sMaterial.Specular.a = 1.f;
//
//		sMaterial.Emissive.r = 0.f;
//		sMaterial.Emissive.g = 0.f;
//		sMaterial.Emissive.b = 0.f;
//		sMaterial.Emissive.a = 0.f;
//
//		sMaterial.Power	= 1.f;
//
//		pd3dDevice->SetMaterial ( &sMaterial );
//
//		m_spStaticMeshLightMap->Render_THREAD_ ( pd3dDevice, cv );
//	}
//
//	//	Note : ������ ���� ����Ʈ �߿��� ���� ��� �Ѹ��� Render �ϴ� ����Ʈ.
//	//
//	PROFILE_BEGIN("m_AfterRender.Render");
//	m_AfterRender.Render ( pd3dDevice );
//	m_AfterRender.Clear ();
//	PROFILE_END("m_AfterRender.Render");
//
//    // VisualMaterial�� Translucent ���͸��� ������
//    NSTexEffVisualMaterial::Render( pd3dDevice, &NSVMRender::g_sTranslucent );
//
//	PROFILE_END("DxLandMan::Render");
//	return hr;
//}

HRESULT DxLandMan::DrawAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, BOOL bReflect )
{
    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return E_FAIL;

	BOOL bOcclusionCullingRendering = NSOcclusionCulling::g_bRendering;
	NSOcclusionCulling::g_bRendering = FALSE;

	PROFILE_BEGIN("DxLandMan::DrawAlphaMap");

	//	Note : Ʈ������ ����.
	//
	if ( !bReflect )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
	}

	{
		DxLightMan::GetInstance()->ReSetLight( pd3dDevice );

		m_DxOctree.RenderAlpha( *pCV );

		DxLightMan::GetInstance()->LightDisable34567( pd3dDevice );
	}

	//	Note : Optimum Octree
	if ( m_spStaticMeshLightMap.get() )
	{
		D3DMATERIALQ	sMaterial;
		sMaterial.Diffuse.r = 1.f;
		sMaterial.Diffuse.g = 1.f;
		sMaterial.Diffuse.b = 1.f;
		sMaterial.Diffuse.a = 1.f;

		sMaterial.Ambient.r = 1.f;
		sMaterial.Ambient.g = 1.f;
		sMaterial.Ambient.b = 1.f;
		sMaterial.Ambient.a = 1.f;

		sMaterial.Specular.r = 1.f;
		sMaterial.Specular.g = 1.f;
		sMaterial.Specular.b = 1.f;
		sMaterial.Specular.a = 1.f;

		sMaterial.Emissive.r = 0.f;
		sMaterial.Emissive.g = 0.f;
		sMaterial.Emissive.b = 0.f;
		sMaterial.Emissive.a = 0.f;

		sMaterial.Power	= 1.f;

		pd3dDevice->SetMaterial ( &sMaterial );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	// m_pStaticMesh�� Diffuse Color�� ����ϹǷ� Light�� ����.

		m_spStaticMeshLightMap->Render_THREAD_SoftAlpha ( pd3dDevice, NSLandThread::TYPE_BASEMESH_CULL, *pCV );

		pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	}

	PROFILE_END("DxLandMan::DrawAlphaMap");

	NSOcclusionCulling::g_bRendering = bOcclusionCullingRendering;

	return S_OK;
}

void DxLandMan::Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice )
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

void DxLandMan::Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFieldOcclusion, BOOL bPieceOcclusion, DWORD dwCullMode )
{
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matVP;
	pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
	D3DXMatrixMultiply( &matVP, &matView, &matProj );

	NSOcclusionQueryFastRender::g_bRendering = TRUE;

	const CLIPVOLUME& sCV = DxViewPort::GetInstance().GetClipVolume();

	// �⺻������ �Ѵ�.
	{
		DWORD dwCullModePrev;
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,		&dwCullModePrev );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,		dwCullMode );

		NSOcclusionQueryFastRender::SetWVP( matVP );
		NSOcclusionQueryFastRender::SetVP( matVP );

		if ( m_spStaticMaterialMeshLightMap.get() )
		{
			//m_spStaticMaterialMeshLightMap->RenderOrderSetting( pd3dDevice, sCV );
			m_spStaticMaterialMeshLightMap->Render_CharShadow( pd3dDevice, sCV );
		}
    
		//NSPieceQuickSort::Render_Query_Prepare( pd3dDevice, sCV );

		pd3dDevice->SetRenderState( D3DRS_CULLMODE,		dwCullModePrev );
	}

	// Occlusion Query 
	{
		NSOcclusionQueryFastRender::SetWVP( matVP );
		NSOcclusionQueryFastRender::SetVP( matVP );

		if ( bFieldOcclusion )
		{
			if ( m_spStaticMaterialMeshLightMap.get() )
			{
				m_spStaticMaterialMeshLightMap->Render_Query( pd3dDevice, sCV );
			}
		}

		if ( bPieceOcclusion )
		{
		//	NSPieceQuickSort::Render_Query( pd3dDevice, sCV );
		}
	}

	NSOcclusionQueryFastRender::SetWVP( matVP );
	NSOcclusionQueryFastRender::SetVP( matVP );

	NSOcclusionQueryFastRender::g_bRendering = FALSE;
}

void DxLandMan::PieceSort( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt )
{
	m_spPieceManager->PieceSort( sCV, vFromPt, vLookatPt );
}

void DxLandMan::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_DxOctree.CreateQuery( pd3dDevice );

	if ( m_spStaticMeshLightMap.get() )
	{
		m_spStaticMeshLightMap->CreateQuery( pd3dDevice );
	}
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->CreateQuery( pd3dDevice );
	}

	m_spPieceManager->CreateQuery( pd3dDevice );
}

void DxLandMan::DestroyQuery()
{
	m_DxOctree.DestroyQuery();

	if ( m_spStaticMeshLightMap.get() )
	{
		m_spStaticMeshLightMap->DestroyQuery();
	}
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->DestroyQuery();
	}

	m_spPieceManager->DestroyQuery();
}

void DxLandMan::ResetOcclusionQuery()
{
	m_DxOctree.ResetOcclusionQuery();

	if ( m_spStaticMeshLightMap.get() )
	{
		m_spStaticMeshLightMap->ResetOcclusionQuery();
	}
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->ResetOcclusionQuery();
	}

	m_spPieceManager->ResetOcclusionQuery();
}

void DxLandMan::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, VEC_DWORD& vecOctreeIndex, VEC_DWORD& vecPieceIndex )
{
	DWORD dwZEnable, ZWriteEnable;
	pd3dDevice->GetRenderState( D3DRS_ZENABLE,		&dwZEnable );
	pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE, &ZWriteEnable );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	const CLIPVOLUME& sCV = DxViewPort::GetInstance().GetClipVolume();

	m_DxOctree.VisibleOcclusionQueryTest( sCV, pd3dDevice, vecOctreeIndex );

	if ( m_spStaticMeshLightMap.get() )
	{
		m_spStaticMeshLightMap->FindOcclusionRenderIndex( pd3dDevice, sCV, vecOctreeIndex );
	}
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->FindOcclusionRenderIndex( pd3dDevice, sCV, vecOctreeIndex );
	}

	m_spPieceManager->FindOcclusionRenderPieceIndex( pd3dDevice, vecPieceIndex );

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,		dwZEnable );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, ZWriteEnable );
}

// �ܼ� �浹 �ߴ��� �� �ߴ��� Ȯ�ο�.
BOOL DxLandMan::IsCollisionSimple( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, EMCOLLMODE emCullMode ) const
{
	if ( m_spStaticMeshLightMap.get() || m_spStaticMaterialMeshLightMap.get() )
	{
		if ( m_spStaticMeshLightMap.get() )
		{
			if( m_spStaticMeshLightMap->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
				return TRUE;
		}
		if ( m_spStaticMaterialMeshLightMap.get() )
		{
			if( m_spStaticMaterialMeshLightMap->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
				return TRUE;
		}

		// Note : ���� ���� �ʿ����� �浹ó��
		if( m_spPieceManager->IsCollisionLineSimple( vStart, vEnd, emCullMode ) ) 
			return TRUE;
	}
	else
	{
		if ( m_DxOctree.IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
			return TRUE;
	}

	return FALSE;
}

void DxLandMan::IsCollision ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
						LPDXFRAME &pDxFrame, EMCOLLMODE emCullMode, BOOL bOnlyCamColl ) const
{
	bCollision = FALSE;
	vCollision = vEnd;

	//COLLISION::vColTestStart = vStart;
	//COLLISION::vColTestEnd = vEnd;

	D3DXVECTOR3	vNewEnd		= vEnd;

	const char* szName = NULL;
	D3DXVECTOR3 vNewNormal(0.f,1.f,0.f);
	if ( m_spStaticMeshLightMap.get() || m_spStaticMaterialMeshLightMap.get() )
	{
		bCollision = FALSE;
		if ( m_spStaticMeshLightMap.get() )
		{
			if( m_spStaticMeshLightMap->IsCollisionLine( vStart, vNewEnd, vCollision, vNewNormal, szName, emCullMode, bOnlyCamColl ) )
				bCollision = TRUE;
		}
		if ( m_spStaticMaterialMeshLightMap.get() )
		{
			if( m_spStaticMaterialMeshLightMap->IsCollisionLine( vStart, vNewEnd, vCollision, vNewNormal, szName, emCullMode, bOnlyCamColl ) )
				bCollision = TRUE;
		}

		// Note : ���� ���� �ʿ����� �浹ó��
		if( m_spPieceManager->IsCollisionLine( vStart, vNewEnd, vCollision, emCullMode, bOnlyCamColl ) ) 
			bCollision = TRUE;
	}
	else
	{
		m_DxOctree.IsCollision_OCTREE( vStart, vStart, vEnd, vCollision, vNewNormal, bCollision, pDxFrame, szName, emCullMode );
	}

	CDebugSet::ToView( 4, "%s", szName );
}

void DxLandMan::IsCollisionEX ( const D3DXVECTOR3 &vStart, D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
							LPDXFRAME &pDxFrame, LPCSTR& szName, D3DXVECTOR3 *vNormal, BOOL bOnlyCamColl )
{
	bCollision = FALSE;
	vCollision = vEnd;

	//COLLISION::vColTestStart = vStart;
	//COLLISION::vColTestEnd = vEnd;

	D3DXVECTOR3	vNewEnd		= vEnd;

	D3DXVECTOR3 vNewNormal (0.f, 1.f, 0.f);
	if ( vNormal )	vNewNormal = *vNormal;

	if ( m_spStaticMeshLightMap.get() || m_spStaticMaterialMeshLightMap.get() )
	{
		bCollision = FALSE;
		if ( m_spStaticMeshLightMap.get() )
		{
			if( m_spStaticMeshLightMap->IsCollisionLine( vStart, vNewEnd, vCollision, vNewNormal, szName, EMCC_CULL_CCW, bOnlyCamColl ) )
				bCollision = TRUE;
		}
		if ( m_spStaticMaterialMeshLightMap.get() )
		{
			if( m_spStaticMaterialMeshLightMap->IsCollisionLine( vStart, vNewEnd, vCollision, vNewNormal, szName, EMCC_CULL_CCW, bOnlyCamColl ) )
				bCollision = TRUE;
		}
	}
	else
	{
		m_DxOctree.IsCollision_OCTREE( vStart, vStart, vNewEnd, vCollision, vNewNormal, bCollision, pDxFrame, szName, EMCC_CULL_CCW );
	}

	CDebugSet::ToView( 10, "%s", szName );

	if ( vNormal )	*vNormal = vNewNormal;
}

void DxLandMan::IsCollisionNEW( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
								EMCOLLMODE emCullMode, const BOOL bPiece, BOOL bOnlyCamColl )
{
	if( !m_spStaticMeshLightMap.get() )	
		return;

	bCollision = FALSE;
	vCollision = vEnd;

	//COLLISION::vColTestStart = vStart;
	//COLLISION::vColTestEnd = vEnd;

	D3DXVECTOR3	vNewEnd		= vEnd;

	// Note : �Ϲ� �޽� �浹 ó��
	const char* szName = NULL;
	D3DXVECTOR3 vNewNormal(0.f,1.f,0.f);
	if ( m_spStaticMeshLightMap.get() )
	{
		if( m_spStaticMeshLightMap->IsCollisionLine( vStart, vNewEnd, vCollision, vNewNormal, szName, emCullMode, bOnlyCamColl ) )	
			bCollision = TRUE;
	}

	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		if( m_spStaticMaterialMeshLightMap->IsCollisionLine( vStart, vNewEnd, vCollision, vNewNormal, szName, emCullMode, bOnlyCamColl ) )	
			bCollision = TRUE;
	}

	// Note : ���� ���� �ʿ����� �浹ó��
	if ( bPiece )
	{
		if( m_spPieceManager->IsCollisionLine( vStart, vNewEnd, vCollision, emCullMode, bOnlyCamColl ) ) 
			bCollision = TRUE;
	}


	CDebugSet::ToView( 4, "%s", szName );
}

void DxLandMan::GetCollisionLineInTriListExceptY ( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList )  // �ֺ��� �߰� �޼ҵ�
{	
	const char* szName = NULL;

	if ( m_spStaticMeshLightMap.get() || m_spStaticMaterialMeshLightMap.get() )
	{
		if ( m_spStaticMeshLightMap.get() )
		{
			m_spStaticMeshLightMap->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
		}
		if ( m_spStaticMaterialMeshLightMap.get() )
		{
			m_spStaticMaterialMeshLightMap->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
		}
	}
	else
	{
		m_DxOctree.GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
	}

	DxReplaceContainer::GetCollisionLineInTriListExceptY( m_pPieceObjTree, vStart, vEnd, TriList );
}

BOOL DxLandMan::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEndSRC, DWORD& dwColor )
{
	D3DXVECTOR3 vEnd = vEndSRC;

	BOOL bExist(FALSE);
	if ( m_spStaticMeshLightMap.get() )
	{
		bExist = m_spStaticMeshLightMap->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor );
	}

	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		BOOL bBackUp = m_spStaticMaterialMeshLightMap->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor );
		if ( bBackUp )
		{
			bExist = TRUE;
		}
	}

	//if( m_pPieceManager )
	//{
	//	BOOL bBackUp = m_spPieceManager->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor );
	//	if ( bBackUp )
	//	{
	//		bExist = TRUE;
	//	}
	//}

	return bExist;
}

// �α��� ȭ�鿡���� MainThread ���� �ϰ�
// ������ ����ȭ�鿡���� Ÿ Thread ���� ���۵ȴ�.
void DxLandMan::CalculateActorWorldLight_Thread( D3DXVECTOR3 &vDiffuse OUT, D3DXVECTOR3 &vPointLight OUT, const D3DXVECTOR3 &vPos ) const
{
	if ( !m_pLightMan )
		return;

	//////////////////////////////////////////////////////////////////////////
	//					Direction Light �� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////
	vDiffuse.x = m_pLightMan->GetDirectLight()->m_Light.Diffuse.r;
	vDiffuse.y = m_pLightMan->GetDirectLight()->m_Light.Diffuse.g;
	vDiffuse.z = m_pLightMan->GetDirectLight()->m_Light.Diffuse.b;

	D3DXVECTOR3 vStart(vPos);
	vStart.y += 10.f;

	// �ǽð��� ��� ���� ������� �ش�.
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_PS_2_0 || RENDERPARAM::g_emGroundQulity <= TnL_PS_3_0_DF )
	{	
		D3DXVECTOR3 vEnd(vStart);
		vEnd.x -= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction.x * 1000.f;
		vEnd.y -= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction.y * 1000.f;
		vEnd.z -= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction.z * 1000.f;

		D3DXVECTOR3 vCollision;
		LPDXFRAME pDxFrame(NULL);
		if ( IsCollisionSimple( vStart, vEnd, EMCC_CULL_NONE_EXCEPT_ALPHATEX ) )
		{
			// ���ݸ� �޵��� �Ѵ�.
			vDiffuse.x *= 0.5f;
			vDiffuse.y *= 0.5f;
			vDiffuse.z *= 0.5f;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//					Point Light �� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////
	m_pLightMan->AddDffiuse_PointLight( vPointLight, vStart, *this );
}

BOOL DxLandMan::IsLightMap()
{
	BOOL bExist(FALSE);
	if ( m_spStaticMeshLightMap.get() )
	{
		bExist = m_spStaticMeshLightMap->IsLightMap();
	}

	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		return TRUE;
	}

	return bExist;
}

void DxLandMan::CleanUp ()
{
	m_strNameWLD = _T("");
	m_strMakeTimeWLD = _T("");

	m_WeatherMan.ReSetSound ();

	NSCubeMapUtil::NSCubeMapBlend::CleanUp();
	m_scpShadowLightData->CleanUp();

	DxWeatherMan::SetInstance( NULL );

	// Thread�� ��� ���� �ɶ����� while �� ���ư���.
	NSSkinAniThread::DataCleanUp();
	NSEffectThread::DataCleanUp();
	NSLandThread::DataCleanUp();

	for ( DWORD i=0; i<NSLandThread::TYPE_CULL_MODE_SIZE; ++i )
	{
		m_spEntityRenderListSwap[i]->CleanUp();
	}

	NSPieceQuickSort::CleanUp();

	if( m_spStaticMeshLightMap.get() )
	{
		m_spStaticMeshLightMap.reset();
		//SAFE_DELETE( m_pStaticMesh );
	}

	if( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap.reset();
	}

	//	Note : �� ����Ÿ ��� ����.
	//
	m_SFileMap.CloseFile ();

	OctreeDebugInfo::EndNodeCount = 0;
	OctreeDebugInfo::AmountDxFrame = 0;

	m_DxOctree.CleanUp ();
	SAFE_DELETE(m_pAniManHead);
	SAFE_DELETE(m_pAniManTree);
	SAFE_DELETE(m_pPieceObjHead);
	SAFE_DELETE(m_pPieceObjTree);
	
	SAFE_DELETE(m_pEffectFrameList);
	SAFE_DELETE(m_pEffectList);
	SAFE_DELETE(m_pEffectTree);
	SAFE_DELETE(m_pEffectList_AFTER);
	SAFE_DELETE(m_pEffectTree_AFTER);
	SAFE_DELETE(m_pEffectList_AFTER_1);
	SAFE_DELETE(m_pEffectTree_AFTER_1);
	SAFE_DELETE(m_pList_Weather);
	SAFE_DELETE(m_pTree_Weather);

	m_sNaviMeshCapsule.CleanUp();

	SAFE_DELETE(m_pStaticSoundMan);

	m_dwNumLandEff = 0;
	SAFE_DELETE(m_pLandEffList);
	SAFE_DELETE(m_pLandEffTree);

	m_spLandSkinManager->CleanUp();

	m_pLightMan->CleanUp ();
	m_CameraAniMan.Cleanup ();	
	m_LandGateMan.CleanUp ();

	m_CollisionMap.CleanUp ();

	m_spPieceManager->CleanUp();

	NSWaterMain::CleanUp();

	m_bUsedMaterialSystem = TRUE;
	m_bUsedMaterialSystemPS_3_0 = TRUE;
}

void DxLandMan::MakeMiniMap( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxSetLandMan* pSetLandMan,
							D3DXVECTOR2& vMin, 
							D3DXVECTOR2& vMax, 
							DWORD dwTexSize,  
							const TCHAR* pName)
{
	BOOL bPieceInstancing = NSPieceQuickSort::g_bPieceInstancing;
	NSPieceQuickSort::g_bPieceInstancing = FALSE;
	TnL_MODE emGroundQulityBackUp( RENDERPARAM::g_emGroundQulity );
	RENDERPARAM::g_emGroundQulity = TnL_PS_3_0_DF;
	BOOL bFORCE_TnL_PS_2_0_BACKUP = RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF;
	RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF = TRUE;
	// Sort �� �˷��༭ LOD ����� ���� ���ϵ��� �Ѵ�.
	BOOL bViewAlphaLOD_BackUp = NSPieceQuickSort::g_bViewAlphaLOD;
	NSPieceQuickSort::g_bViewAlphaLOD = FALSE;

	// ���� ���ϴ� ����.
	float fAngle = D3DX_PI/2;

	D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3 vFromPtOrig = DxViewPort::GetInstance().GetFromPt();

	D3DXVECTOR3 vDir = vFromPtOrig - vLookatPt;
	float fLength = D3DXVec3Length( &vDir );

	// Note : ������ �Ǵ� �κ� ������
	vMin.x = vLookatPt.x - fLength*0.5f;
	vMin.y = vLookatPt.z - fLength*0.5f;
	vMax.x = vLookatPt.x + fLength*0.5f;
	vMax.y = vLookatPt.z + fLength*0.5f;

	// Note : View Port�� �����Ѵ�.
	D3DXMATRIX matView;
	D3DXVECTOR3 vFromPt = vLookatPt + D3DXVECTOR3( 0.f, fLength, 0.f );
	D3DXVECTOR3 vUpVec( 0.f, 0.f, 1.f );
	D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );

	// Note : Projection�� �����Ѵ�.
	D3DXMATRIX matProj;
	float fNearPlane = 10.f;
	float fFarPlane = 100000.f;
	D3DXMatrixOrthoLH( &matProj, fLength, fLength, fNearPlane, fFarPlane );	// D3DXMatrixPerspectiveFovLH

	DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
	DxViewPort::GetInstance().SetProjectionOrthoLH ( fLength, fLength, fNearPlane, fFarPlane );

	// Note : Ŭ���� ������ �����.
	CLIPVOLUME sCV = DxViewPort::GetInstance().ComputeClipVolume
	( 
		vLookatPt, 
		vFromPt, 
		vUpVec, 
		fNearPlane, 
		fFarPlane, 
		1.f, 
		1.f, 
		fAngle 
	);

	// CV Thread �۾��� ���۵ǰ���. 
	{
		NSLandThread::Wait_FrameMoveRAD( GetArrayEntityRenderListSwap(), sCV );

		// �Ϸ�� ������ ����Ѵ�.
		while ( !NSLandThread::IsCompleteData() )
		{
		}
		NSLandThread::Swap();
	}

	// Note : SetRenderState
	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	// Note : ����� ����
	D3DXMATRIX matViewPREV, matProjPREV;
	pd3dDevice->GetTransform( D3DTS_VIEW,		&matViewPREV );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjPREV );
	pd3dDevice->SetTransform( D3DTS_VIEW,		&matView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	UINT uiWidth( dwTexSize );
	UINT uiHeight( dwTexSize );
	D3DFORMAT emFormat( D3DFMT_A8R8G8B8 );
	IDirect3DTexture9*	pTexture(NULL);
	LPDIRECT3DSURFACEQ	pSurface;
	LPDIRECT3DSURFACEQ	pZBuffer;
	TextureManager::GetInstance().CreateTexture(
		pd3dDevice,
		uiWidth, 
		uiHeight, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		emFormat,
		D3DPOOL_DEFAULT,
		&pTexture,
		TEXTURE_RESIZE_NONE,
		TEXTURE_CATEGORY_SIZE );

	pTexture->GetSurfaceLevel( 0, &pSurface );



	LPDIRECT3DSURFACEQ	pSurfaceBackUp_1st;
	LPDIRECT3DTEXTUREQ	pTextureBackUp_1st;
	TextureManager::GetInstance().CreateTexture(
		pd3dDevice,
		uiWidth, 
		uiHeight, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		emFormat,
		D3DPOOL_DEFAULT,
		&pTextureBackUp_1st,
		TEXTURE_RESIZE_NONE,
		TEXTURE_CATEGORY_SIZE );

	pTextureBackUp_1st->GetSurfaceLevel( 0, &pSurfaceBackUp_1st );



	LPDIRECT3DSURFACEQ	pSurfaceBackUp_2nd;
	LPDIRECT3DTEXTUREQ	pTextureBackUp_2nd;
	TextureManager::GetInstance().CreateTexture(
		pd3dDevice,
		uiWidth, 
		uiHeight, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		emFormat,
		D3DPOOL_DEFAULT,
		&pTextureBackUp_2nd,
		TEXTURE_RESIZE_NONE,
		TEXTURE_CATEGORY_SIZE );

	pTextureBackUp_2nd->GetSurfaceLevel( 0, &pSurfaceBackUp_2nd );



	D3DFORMAT sFMT161616(D3DFMT_A16B16G16R16);
	LPDIRECT3DSURFACEQ	pSurfaceBackUp_3rd;
	LPDIRECT3DTEXTUREQ	pTextureBackUp_3rd;
	TextureManager::GetInstance().CreateTexture(
		pd3dDevice,
		uiWidth, 
		uiHeight, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		sFMT161616,
		D3DPOOL_DEFAULT,
		&pTextureBackUp_3rd,
		TEXTURE_RESIZE_NONE,
		TEXTURE_CATEGORY_SIZE );

	pTextureBackUp_3rd->GetSurfaceLevel( 0, &pSurfaceBackUp_3rd );



	pd3dDevice->CreateDepthStencilSurface( 
		uiWidth,
		uiHeight,
		DxSurfaceTex::GetInstance().GetZFormat(),
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&pZBuffer,
		NULL );



	// ���� ������
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice, DxBackupTarget::ALL );

		pd3dDevice->SetRenderTarget( 0, pSurface );
		pd3dDevice->SetDepthStencilSurface( pZBuffer );
		pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

		//if ( pSurfaceBackUp && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_bUsedMaterialSystem )
		//{
		//	pd3dDevice->SetRenderTarget( 0, pSurfaceBackUp );
		//	pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );
		//}

		pd3dDevice->BeginScene();
		{
			NSMaterialManager::FrameMove( matView, matProj, pTextureBackUp_1st, pTextureBackUp_2nd, pTextureBackUp_3rd, NULL );

			// Note : ���� ���� �Ŵ���
			m_spPieceManager->FrameMove( 0.f, 0.f, m_spEntityRenderListSwap[NSLandThread::TYPE_BASEMESH_CULL] );

			// Note : m_pStaticMesh
			// Note : m_pStaticMesh
			if ( m_spStaticMeshLightMap.get() )
				m_spStaticMeshLightMap->FrameMove( 0.f );

			if ( m_spStaticMaterialMeshLightMap.get() )
				m_spStaticMaterialMeshLightMap->FrameMove( 0.f );

			// ���� �� ������
			NSWaterMain::FrameMove( 0.f );

			D3DMATERIALQ	sMaterial;
			sMaterial.Diffuse.r = 1.f;
			sMaterial.Diffuse.g = 1.f;
			sMaterial.Diffuse.b = 1.f;
			sMaterial.Diffuse.a = 1.f;

			sMaterial.Ambient.r = 1.f;
			sMaterial.Ambient.g = 1.f;
			sMaterial.Ambient.b = 1.f;
			sMaterial.Ambient.a = 1.f;

			sMaterial.Specular.r = 1.f;
			sMaterial.Specular.g = 1.f;
			sMaterial.Specular.b = 1.f;
			sMaterial.Specular.a = 1.f;

			sMaterial.Emissive.r = 0.f;
			sMaterial.Emissive.g = 0.f;
			sMaterial.Emissive.b = 0.f;
			sMaterial.Emissive.a = 0.f;

			sMaterial.Power	= 1.f;

			pd3dDevice->SetMaterial ( &sMaterial );

			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );

			// ����� ��� ������ ���Ͽ� ����Ǿ��ִ� ���� ����ϱ� ����;
			if ( emGroundQulityBackUp >= TnL_PS_3_0_DF )//&& m_RenderType == RENDER_TEXTURE_LIGHTMAP )
			{
				Render_Radiosity
					(
					pd3dDevice, 
					sCV,
					matView,
					matProj,
					TRUE,
					D3DCOLOR_XRGB(0,0,0),
					uiWidth,
					uiHeight,
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
					FALSE,
					NSLandThread::TYPE_BASEMESH_CULL
					);
			}
			else
			{
				Render( pd3dDevice, sCV );
				RenderPickAlpha( pd3dDevice, TRUE );
				pSetLandMan->Render_EFF( pd3dDevice, FALSE );
			}
		}
		pd3dDevice->EndScene();

		sBackupTarget.Restore();

		// Note : �ؽ��ĸ� �����Ѵ�.
		LPDIRECT3DBASETEXTURE9 pTexRGB(NULL);
		NSTexCompress::Compress
		(
			pd3dDevice,
			dwTexSize,
			dwTexSize,
			0,
			1,
			D3DFMT_X8R8G8B8,
			pTexture,
			pTexRGB
		);

		LPDIRECT3DBASETEXTURE9 pTexNew(NULL);
		NSTexCompress::Compress
		(
			pd3dDevice,
			dwTexSize,
			dwTexSize,
			0,
			1,
			D3DFMT_DXT1,
			pTexRGB,
			pTexNew
		);

		// Note : ���ۿ� �׷��� ���� �ؽ��ķ� �̴´�.
		TSTRING strTexture = NSLightMap::g_strPath + pName + _T(".dds");
		D3DXSaveTextureToFile( strTexture.c_str(), D3DXIFF_DDS, pTexNew, NULL );

		SAFE_RELEASE( pTexRGB );
		SAFE_RELEASE( pTexNew );
	}

	// Naviagtion ������
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( pd3dDevice, DxBackupTarget::ALL );

		pd3dDevice->SetRenderTarget( 0, pSurface );
		pd3dDevice->SetDepthStencilSurface( pZBuffer );
		pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

		pd3dDevice->BeginScene();
		{
			m_sNaviMeshCapsule.RenderMiniMapMode( pd3dDevice );
		}
		pd3dDevice->EndScene();

		sBackupTarget.Restore();

		// Note : �ؽ��ĸ� �����Ѵ�.
		LPDIRECT3DBASETEXTURE9 pTexRGB(NULL);
		NSTexCompress::Compress
			(
			pd3dDevice,
			dwTexSize,
			dwTexSize,
			0,
			1,
			D3DFMT_X8R8G8B8,
			pTexture,
			pTexRGB
			);

		LPDIRECT3DBASETEXTURE9 pTexNew(NULL);
		NSTexCompress::Compress
			(
			pd3dDevice,
			dwTexSize,
			dwTexSize,
			0,
			1,
			D3DFMT_DXT1,
			pTexRGB,
			pTexNew
			);

		// Note : ���ۿ� �׷��� ���� �ؽ��ķ� �̴´�.
		TSTRING strTexture = NSLightMap::g_strPath + pName + _T("_navi.dds");
		D3DXSaveTextureToFile( strTexture.c_str(), D3DXIFF_DDS, pTexNew, NULL );

		SAFE_RELEASE( pTexRGB );
		SAFE_RELEASE( pTexNew );
	}

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	// Note : �ѹ�
	pd3dDevice->SetTransform( D3DTS_VIEW,		&matViewPREV );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjPREV );

	SAFE_RELEASE( pTextureBackUp_1st );
	SAFE_RELEASE( pSurfaceBackUp_1st );
	SAFE_RELEASE( pTextureBackUp_2nd );
	SAFE_RELEASE( pSurfaceBackUp_2nd );
	SAFE_RELEASE( pTextureBackUp_3rd );
	SAFE_RELEASE( pSurfaceBackUp_3rd );
	SAFE_RELEASE( pTexture );
	SAFE_RELEASE( pSurface );
	SAFE_RELEASE( pZBuffer );

	// RollBack
	NSPieceQuickSort::g_bViewAlphaLOD = bViewAlphaLOD_BackUp;
	RENDERPARAM::g_emGroundQulity = emGroundQulityBackUp;
	RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF = bFORCE_TnL_PS_2_0_BACKUP;
	NSPieceQuickSort::g_bPieceInstancing = bPieceInstancing;
}

void DxLandMan::ClipStaticMesh( DxDecal* pDecal )
{
	if ( m_spStaticMeshLightMap.get() )
	{
		m_spStaticMeshLightMap.get()->ClipStaticMesh( pDecal );
	}
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap.get()->ClipStaticMesh( pDecal );
	}
}

void DxLandMan::GetClipVolumeRAD( CLIPVOLUME& sOUT, D3DXMATRIX& matViewOUT, D3DXMATRIX& matProjOUT, D3DXVECTOR3& vLookatPtOUT, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vNor )
{
	float fAngle = D3DX_PI*0.8f;

	// Up Vector ������ Normal ������ Y ��� ��ġ�ϸ� ������ �� �� �ִ�. (Rendering �� �ȵȴ�.)
	// �׷��� ������ġ��ŭ ������ Ʋ���ش�.
	D3DXVECTOR3 vNormalize;
	D3DXVec3Normalize( &vNormalize, &vNor );
	if ( vNormalize.y >= 0.99f || vNormalize.y <= -0.99f )
	{
		vNormalize.z = 0.01f;
	}

	//DxFogMan::GetInstance().RenderFogSB ( pd3dDevice, FALSE );

	D3DXVECTOR3 vPosNew = vPos;
	D3DXVECTOR3 vColl = vPos;
	//if ( NSHfMain::IsCollisionY( vColl ) )
	//{
	//	// �������� �������� �ؿ� �ִٸ� �������� �÷��ִ� �۾��� �Ѵ�.
	//	vPosNew.y = vColl.y + 1.f;
	//}

	// Note : View Port�� �����Ѵ�.
	D3DXVECTOR3 vFromPt = vPosNew;
	vLookatPtOUT = vPosNew + vNormalize;
	D3DXVECTOR3 vUpVec( 0.f, 1.f, 0.f );
	D3DXMatrixLookAtLH( &matViewOUT, &vFromPt, &vLookatPtOUT, &vUpVec );

	// Note : Projection�� �����Ѵ�.
	float fNearPlane = 0.1f;
	float fFarPlane = 300.f;
	D3DXMatrixPerspectiveFovLH( &matProjOUT, fAngle, 1.f, fNearPlane, fFarPlane );	// D3DXMatrixPerspectiveFovLH

	// Note : Ŭ���� ������ �����.
	sOUT = DxViewPort::GetInstance().ComputeClipVolume
	( 
		vLookatPtOUT, 
		vFromPt, 
		vUpVec, 
		fNearPlane, 
		fFarPlane, 
		1.f, 
		1.f, 
		fAngle 
	);
}

// Radiosity Render
DWORD DxLandMan::Render_Radiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
								  const D3DXVECTOR3& vPos, 
								  const D3DXVECTOR3& vNor, 
								  LPDIRECT3DTEXTUREQ pTextureSystem, 
								  float fReflectance,
								  float fFogBlend,
								  DWORD dwFogLength,
								  BOOL bDay )
{
	// 
	CLIPVOLUME sCV;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXVECTOR3 vLookatPtOUT;
	GetClipVolumeRAD( sCV, matView, matProj, vLookatPtOUT, vPos, vNor );

	// Note : SetRenderState
	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );

	// Note : ����� ����
	D3DXMATRIX matViewPREV, matProjPREV;
	pd3dDevice->GetTransform( D3DTS_VIEW,		&matViewPREV );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjPREV );
	pd3dDevice->SetTransform( D3DTS_VIEW,		&matView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	// Fog ������ ���� �۾���.
	DWORD dwFogColor = 0xff000000;
	DWORD dwNearPlanePrev(0L);
	DWORD dwFarPlanePrev(0L);
	{
		//FrameMove( 0.f, 0.f, matView, matProj );
		NSMaterialManager::FrameMove( matView, matProj, NSRadiosity::g_pTexture_1st, NSRadiosity::g_pTexture_2nd, NSRadiosity::g_pTexture_3rd, NULL );

		// Fog ��ġ ����
		{
			float fNearPlane = 0.f;
			float fFarPlane = static_cast<float>(dwFogLength);
			pd3dDevice->GetRenderState( D3DRS_FOGSTART, &dwNearPlanePrev );
			pd3dDevice->GetRenderState( D3DRS_FOGEND,	&dwFarPlanePrev );
			pd3dDevice->SetRenderState( D3DRS_FOGSTART,	*((DWORD *)(&fNearPlane)) );
			pd3dDevice->SetRenderState( D3DRS_FOGEND,	*((DWORD *)(&fFarPlane)) );
			DxFogMan::GetInstance().SetFogStartFogEnd( fNearPlane, fFarPlane );
			NSMaterialManager::SetFogParam( fNearPlane, fFarPlane );
		}

		D3DXVECTOR3 vDirectLightAmbient(0.f,0.f,0.f);
		const FOG_PROPERTY& sFogProperty = DxFogMan::GetInstance().GetProperty();
		if ( sFogProperty.m_bSkyFogEnable )
		{
			if ( bDay )
			{
				dwFogColor = sFogProperty.m_cFogColor_Day;

				D3DLIGHTQ* pDirectLight = &DxLightMan::GetInstance()->GetDirectNoonLight()->m_Light;
				vDirectLightAmbient = D3DXVECTOR3( pDirectLight->Ambient.r, pDirectLight->Ambient.g, pDirectLight->Ambient.b );
			}
			else
			{
				dwFogColor = sFogProperty.m_cFogColor_Night;
				D3DLIGHTQ* pDirectLight = &DxLightMan::GetInstance()->GetDirectNightLight()->m_Light;
				vDirectLightAmbient = D3DXVECTOR3( pDirectLight->Ambient.r, pDirectLight->Ambient.g, pDirectLight->Ambient.b );
			}
		}
		else
		{
			dwFogColor = sFogProperty.m_cFogColor;

			D3DLIGHTQ* pDirectLight = &DxLightMan::GetInstance()->GetDirectNoonLight()->m_Light;
			vDirectLightAmbient = D3DXVECTOR3( pDirectLight->Ambient.r, pDirectLight->Ambient.g, pDirectLight->Ambient.b );
		}
		
		// Fog ���� ���Ѵ�.
		D3DXCOLOR colorClear;
		colorClear.r = static_cast<float>( (dwFogColor&0xff0000) >> 16 );
		colorClear.g = static_cast<float>( (dwFogColor&0xff00) >> 8 );
		colorClear.b = static_cast<float>( (dwFogColor&0xff) );

		colorClear.r *= 0.3333f;
		colorClear.g *= 0.3333f;
		colorClear.b *= 0.3333f;

		D3DXVECTOR3 vDirectLightAmbientDiv3(vDirectLightAmbient);
		vDirectLightAmbientDiv3.x *= 0.3333f;
		vDirectLightAmbientDiv3.y *= 0.3333f;
		vDirectLightAmbientDiv3.z *= 0.3333f;

		D3DXVECTOR3 vFogColor(colorClear.r/255.f,colorClear.g/255.f,colorClear.b/255.f);
		float fAmbientALL = vDirectLightAmbientDiv3.x + vDirectLightAmbientDiv3.y + vDirectLightAmbientDiv3.z;
		float fFogColorALL = vFogColor.x + vFogColor.y + vFogColor.z;

		if ( fFogColorALL > 0.0001f )
		{
			vFogColor.x *= fAmbientALL / fFogColorALL;
			vFogColor.y *= fAmbientALL / fFogColorALL;
			vFogColor.z *= fAmbientALL / fFogColorALL;
		}
		else
		{
			vFogColor.x = vDirectLightAmbient.x;
			vFogColor.y = vDirectLightAmbient.y;
			vFogColor.z = vDirectLightAmbient.z;
		}


		float fFogPower = 1.f;
		{
			float fR = vFogColor.x*255.f;	//static_cast<DWORD>(((colorClear&0xff0000) >> 16));
			float fG = vFogColor.y*255.f;	//static_cast<DWORD>(((colorClear&0xff00) >> 8));
			float fB = vFogColor.z*255.f;	//static_cast<DWORD>(((colorClear&0xff)));
			float fGray = (fR + fG + fB) * (1.f-fFogBlend) * 0.333f;
			fR = fR * fFogBlend + fGray;
			fG = fG * fFogBlend + fGray;
			fB = fB * fFogBlend + fGray;
			DWORD dwR = static_cast<DWORD>( fR * fFogPower );
			DWORD dwG = static_cast<DWORD>( fG * fFogPower );
			DWORD dwB = static_cast<DWORD>( fB * fFogPower );
			if ( dwR > 255 )	dwR = 255;
			if ( dwG > 255 )	dwG = 255;
			if ( dwB > 255 )	dwB = 255;

			dwFogColor = 0xff000000;
			dwFogColor += dwR << 16;
			dwFogColor += dwG << 8;
			dwFogColor += dwB;
		}
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );

		NSMaterialManager::SetFogColorForEditor( dwFogColor );
	}

	UINT uiWidth( 128 );
	UINT uiHeight( 128 );

	NSLightMapFX::SetRenderMode( NSLIGHTMAPDEF::LMRT_TEXTURE_LIGHTMAP );

	DxBackupTarget sBackupTarget;
	sBackupTarget.Backup( pd3dDevice, DxBackupTarget::ALL );

	pd3dDevice->SetRenderTarget( 0, NSRadiosity::g_pSurface );
	pd3dDevice->SetDepthStencilSurface( NSRadiosity::g_pZBuffer );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, dwFogColor, 1.0f, 0L);

	//if ( NSRadiosity::g_pSurfaceBackUp && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_bUsedMaterialSystem )
	//{
	//	pd3dDevice->SetRenderTarget( 0, NSRadiosity::g_pSurfaceBackUp );
	//	pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, dwFogColor, 1.0f, 0L );
	//}

	// OverLight ������ ���� ����.
	NSMaterialManager::SetOverLightPower_PS_2_0( 0.3333f );

	// ȭ�� ����� ��������.
	NSMaterialManager::SetWindowSize( 128, 128 );

	// Sort �� �˷��༭ LOD ����� ���� ���ϵ��� �Ѵ�.
	NSPieceQuickSort::g_bBakeRAD = TRUE;

	pd3dDevice->BeginScene();
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		Render_Radiosity
		(
			pd3dDevice, 
			sCV,
			matView,
			matProj,
			bDay,
			dwFogColor,
			uiWidth,
			uiHeight,
			NSRadiosity::g_pSurface,
			NSRadiosity::g_pSurface_1st,
			NSRadiosity::g_pSurface_2nd,
			NSRadiosity::g_pSurface_3rd,
			NSRadiosity::g_pTexture_1st,
			NSRadiosity::g_pTexture_2nd,
			NSRadiosity::g_pTexture_3rd,
			FALSE,
			0.f,
			matIdentity,
			FALSE,
			NSLandThread::TYPE_BASEMESH_CULL
		);
	}
	pd3dDevice->EndScene();

	pd3dDevice->SetRenderState( D3DRS_FOGSTART, dwNearPlanePrev );
	pd3dDevice->SetRenderState( D3DRS_FOGEND,	dwFarPlanePrev );

	NSPieceQuickSort::g_bBakeRAD = FALSE;

	// ȭ�� ����� ��������.
	NSMaterialManager::SetWindowSize( DxSurfaceTex::GetInstance().GetWidth(), DxSurfaceTex::GetInstance().GetHeight() );

	sBackupTarget.Restore();

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	// Note : �ѹ�
	pd3dDevice->SetTransform( D3DTS_VIEW,		&matViewPREV );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjPREV );

	HRESULT hr = NSTexCompress::GetRenderTargetData( pd3dDevice, NSRadiosity::g_pSurface, pTextureSystem );
	if( FAILED( hr ) )
	{
		GASSERT( 0 && _T("HRESULT hr = NSTexCompress::GetRenderTargetData( pd3dDevice, NSRadiosity::g_pSurface, pTextureSystem );") );
		return hr;
	}

	// �迭
	float arrayfSquare[128][128];

#pragma omp parallel for
	for( int y=0; y<static_cast<int>(uiHeight); ++y )
	{
		float fRateY = static_cast<float>(y)/(uiHeight-1);	// 0~1
		fRateY = fRateY*2.f;		// 0~2
		fRateY -= 1.f;				// -1~1
		fRateY = fabsf( fRateY );	// 1~0~1
		fRateY = 1.f - fRateY;		// 0~1~0
		fRateY = 0.1f + fRateY*0.9f;

		for( int x=0; x<static_cast<int>(uiWidth); ++x )
		{
			float fRateX = static_cast<float>(x)/(uiWidth-1);	// 0~1
			fRateX = fRateX*2.f;		// 0~2
			fRateX -= 1.f;				// -1~1
			fRateX = fabsf( fRateX );	// 1~0~1
			fRateX = 1.f - fRateX;		// 0~1~0
			fRateX = 0.1f + fRateX*0.9f;

			// Perspective �� ���� �̹����� �ְ�Ǳ� ������ �װ��� �����ϱ� ���� ���̴�.
			//arrayfSquare[y][x] = fRateX  *  fRateY;
			arrayfSquare[y][x] = fRateX * fRateX * fRateY * fRateY;
		}
	}

	float fR(0.f);
	float fG(0.f);
	float fB(0.f);

	// �� ���.
	float fSquareADD(0.f);

	D3DLOCKED_RECT stLocked;
	hr = pTextureSystem->LockRect( 0, &stLocked, NULL, 0 );
	if( FAILED(hr) )
	{
		GASSERT( 0 && _T("hr = pTextureSystem->LockRect( 0, &stLocked, NULL, 0 );") );
		return hr;
	}

	DWORD* pData = (DWORD*)stLocked.pBits;
	INT iPitch = stLocked.Pitch / 4;

	for( int y=0; y<static_cast<int>(uiHeight); ++y )
	{
		for( int x=0; x<static_cast<int>(uiWidth); ++x )
		{
			DWORD dwColor = pData[ x + y*iPitch ];

			float fSquare = arrayfSquare[y][x];
			fR += fSquare * ((dwColor & 0xff0000) >> 16);
			fG += fSquare * ((dwColor & 0xff00) >> 8);
			fB += fSquare * ((dwColor & 0xff));

			fSquareADD += fSquare;
		}
	}

	pTextureSystem->UnlockRect( 0 );

	// �ݻ� ��ġ�� ����Ѵ�.
	fR *= fReflectance;
	fG *= fReflectance;
	fB *= fReflectance;

	fR /= fSquareADD;
	fG /= fSquareADD;
	fB /= fSquareADD;

	DWORD dwR = static_cast<DWORD>(fR);
	DWORD dwG = static_cast<DWORD>(fG);
	DWORD dwB = static_cast<DWORD>(fB);

	if ( dwR > 255 )	dwR = 255;
	if ( dwG > 255 )	dwG = 255;
	if ( dwB > 255 )	dwB = 255;

	//if ( dwR > 30 )
	//{
	//	TSTRING strTexture = NSMaterialManager::g_strPath + _T("_AAA") + _T(".dds");
	//	D3DXSaveTextureToFile( strTexture.c_str(), D3DXIFF_DDS, NSRadiosity::g_pTexture_1st, NULL );
	//}

	DWORD dwColor = 0xff000000 + (dwR<<16) + (dwG<<8) + dwB;
	return dwColor;
}

// �ݻ� ����Ʈ�� ���� �� ����. ������������ ���ȴ�.
HRESULT DxLandMan::Render_Radiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
									CLIPVOLUME &cv,
									const D3DXMATRIX& matView,
									const D3DXMATRIX& matProj,
									BOOL bDay,
									const D3DCOLOR& colorClear,
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
									BOOL bSky,
									NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	HRESULT hr = S_OK;

	// BackUp �� TnL_PS_3_0_DF ���� ���۵ǵ��� ����.
	BOOL bFORCE_TnL_PS_2_0_BACKUP = RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF;
	RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF = TRUE;
	BOOL bOcclusionCullingRendering = NSOcclusionCulling::g_bRendering;
	NSOcclusionCulling::g_bRendering = FALSE;

	// Render
	{
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

		const VEC_DWORD* pvecPieceIndex(NULL);
		if ( NSOcclusionCulling::IsActive() )
		{
			const VEC_DWORD* pvecOctreeIndex(NULL);
			NSOcclusionCulling::GetRenderIndex( DxViewPort::GetInstance().GetLookatPt(), &pvecPieceIndex, &pvecOctreeIndex );
			NSOcclusionOctree::InsertIndex( pvecOctreeIndex );
		}

		// RenderList Reset
		NSOctreeRenderList::ClearRenderIndex();


		// 
		m_spEntityRenderListSwap[emCullingRenderType]->SetData_Force_TnL_PS_3_0_DF();
		m_spPieceManager->SetData_Force_TnL_PS_3_0_DF();


		//////////////////////////////////////////////////////////////////////////
		//					RenderDefferedOpaqueForPSF
		NSDeffered::RenderDefferedForSM30DF
		(
			pd3dDevice,
			colorClear,
			this,
			dwWidth,
			dwHeight,
			pColorSuf_Main,
			pColorSuf_1st,
			pColorSuf_2nd,
			pColorSuf_3rd,
			pColorTex_1st,
			pColorTex_2nd, 
			pColorTex_3rd,
			bReflect,
			fReflectHeight,
			matWorld,
			matView,
			matProj,
			cv,
			FALSE,
			FALSE,
			bSky,
			FALSE
		);

		// ���� �� ������
		if ( !bReflect )
		{
			NSWaterMain::Render( pd3dDevice );
		}



		//////////////////////////////////////////////////////////////////////////
		//					RenderDefferedOpaqueForPSF
		NSDeffered::RenderDefferedForSM30DF
		(
			pd3dDevice,
			colorClear,
			this,
			dwWidth,
			dwHeight,
			pColorSuf_Main,
			pColorSuf_1st,
			pColorSuf_2nd,
			pColorSuf_3rd,
			pColorTex_1st,
			pColorTex_2nd, 
			pColorTex_3rd,
			bReflect,
			fReflectHeight,
			matWorld,
			matView,
			matProj,
			cv,
			TRUE,
			FALSE,
			bSky,
			FALSE
		);


		// 
		//m_spPieceManager->Active_MATERIAL_DATA( pd3dDevice, FALSE );
	}	

	// RollBack
	RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF = bFORCE_TnL_PS_2_0_BACKUP;
	NSOcclusionCulling::g_bRendering = bOcclusionCullingRendering;

	return S_OK;
}

void DxLandMan::ExportXFileForLandSkin( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strSkinName, const TSTRING& strTexName )
{
	m_spLandSkinManager->ExportXFile( pd3dDevice, pxofsave, pDataObjectRoot, strSkinName, strTexName );
}

boost::shared_ptr<NavigationMesh> DxLandMan::NavigationMeshCapsule::GetNaviMesh()
{
	if ( m_spNaviMeshBypass )
	{
		// ����� Navigation�� �ִٸ� �װ��� ����ϵ��� �Ѵ�.
		// LevelEdit ���� �����ؾ����� �ȴ�.
		// LevelEdit, Game ��� ���� ������ �� �ִ�.
		return m_spNaviMeshBypass;
	}
	else
	{
		return m_spNaviMeshInWld;
	}
}

void DxLandMan::NavigationMeshCapsule::CreateNaviMesh( DxSetLandMan* pSetLandMan, LPDIRECT3DDEVICEQ pd3dDevice, char *szDebugFullDir )
{
	DxFrame	*pNaviFrame = NULL;
	pSetLandMan->GetNaviFrame ( &pNaviFrame );

	if ( pNaviFrame && pNaviFrame->pmsMeshs )
	{		
		ULONG	NumOfFaces = pNaviFrame->pmsMeshs->m_pSysMemMesh->GetNumFaces();

		ExportProgress::CurPos = 0;
		ExportProgress::EndPos = NumOfFaces;
		ExportProgress::SetStateString ( "�׺���̼� �޽� ������..." );	

		m_spNaviMeshInWld = boost::shared_ptr<NavigationMesh>(new NavigationMesh);
		m_spNaviMeshInWld->Clear();

		HRESULT hr = m_spNaviMeshInWld->CreateNaviMesh ( pNaviFrame, pd3dDevice );
		if ( FAILED ( hr ) )
		{
			ExportProgress::SetStateString ( "����!! �׺���̼� �޽� ������ �����Ͽ����ϴ�." );
			m_spNaviMeshInWld->Clear();
			m_spNaviMeshInWld.reset();
		}
		else
		{
			ExportProgress::CurPos = 0;
			ExportProgress::EndPos = NumOfFaces;
			ExportProgress::SetStateString ( "�׺���̼� �޽� AABB Ʈ�� ������..." );
			m_spNaviMeshInWld->MakeAABBTree ();

			//	<--	NavigationMesh ���� ����			
			ExportProgress::CurPos = 0;
			ExportProgress::EndPos = NumOfFaces;
			ExportProgress::SetStateString ( "�׺���̼� �޽� ��ũ��..." );	
			m_spNaviMeshInWld->LinkCells ();
			//	-->	NavigationMesh ���� ����

			ExportProgress::CurPos = 0;
			ExportProgress::EndPos = 1;
			ExportProgress::SetStateString ( "�׺���̼� �޽� ��ũ ���Ἲ �˻���..." );
			m_bIntegrity = m_spNaviMeshInWld->CheckIntegrity ( szDebugFullDir );
		}
	}
	else
	{
		ExportProgress::EndPos = 1;
		ExportProgress::SetStateString ( "���!! �׺���̼� �޽��� �������� �ʾҽ��ϴ�." );			
		ExportProgress::CurPos++;
	}
}

void DxLandMan::NavigationMeshCapsule::RenderMiniMapMode( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_spNaviMeshInWld.get() )
		return;

	m_spNaviMeshInWld->RenderNavigationForEditVerySlow( pd3dDevice );
}

void DxLandMan::NavigationMeshCapsule::RenderNavigationForEditVerySlow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	boost::shared_ptr<NavigationMesh> spNavigationMesh = GetNaviMesh();
	if ( !spNavigationMesh )
		return;

	spNavigationMesh->RenderNavigationForEditVerySlow( pd3dDevice );
}

BOOL DxLandMan::NavigationMeshCapsule::IsNaviMeshIntegrity()
{
	return m_bIntegrity;
}

void DxLandMan::NavigationMeshCapsule::CleanUp()
{
	m_bIntegrity = TRUE;
	m_spNaviMeshInWld.reset();
	m_spNaviMeshBypass.reset();
}

boost::shared_ptr<NavigationMesh> DxLandMan::GetNaviMesh()
{
	return m_sNaviMeshCapsule.GetNaviMesh();
}

BOOL DxLandMan::IsNaviMeshIntegrity()
{
	return m_sNaviMeshCapsule.IsNaviMeshIntegrity();
}

void DxLandMan::RenderNavigationForEditVerySlow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sNaviMeshCapsule.RenderNavigationForEditVerySlow( pd3dDevice );
}

boost::shared_ptr<EntityRenderListSwap>* DxLandMan::GetArrayEntityRenderListSwap()
{
	return m_spEntityRenderListSwap;
}

// 
void DxLandMan::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	m_spPieceManager->Active_MATERIAL_DATA( pd3dDevice, bTool );
	if ( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->Active_MATERIAL_DATA( pd3dDevice, bTool );
	}
}

void DxLandMan::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	if ( !pWorldBattleFlag )
		return;

	m_spLandSkinManager->SetWorldBattleFlag( pWorldBattleFlag );
}