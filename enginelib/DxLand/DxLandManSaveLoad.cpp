#include "pch.h"

#include <iomanip>

#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#include "../DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "../DxEffect/TexEff/DxTextureEffNewFile.h"
#include "../DxEffect/Frame/DxEffectFrame.h"
#include "../DxEffect/Frame/DxEffectMan.h"
#include "../DxEffect/Mesh/DxStaticMeshLightMap.h"
#include "../DxTools/IrradianceVolume/NSLightMapAmbientColor.h"
#include "../DxTools/Material/DxStaticMaterialMeshLightMap.h"
#include "../DxTools/Occlusion/NSOcclusionCulling.h"
#include "../DxTools/CubeMap/NSCubeMapCulling.h"
#include "../DxTools/SkyBox/DxSkyBoxMan.h"
#include "../DxTools/Light/NSShadowLight.h"
#include "../DxTools/Light/DxLightMan.h"
#include "../Water/NSWaterMain.h"
#include "../NaviMesh/NavigationMesh.h"
#include "../DxSound/DxSoundMan.h"
#include "../DxSound/StaticSoundMan.h"
#include "../DxSound/BgmSound.h"
#include "../DxLand/DxPiece/DxPieceContainer.h"
#include "../DxLand/DxPiece/DxPieceManagerWLD.h"
#include "../Archive/Archive.h"
#include "./DxPiece/NSOctreeRenderList.h"
#include "./NSLandSettingLua.h"
#include "./DXLANDSKINOBJ.h"
#include "./NSLandThread.h"

#include "DxLandMan.h"

//
//	LAND.MAN 파일 버전 변화.
//	ver.113 - Weather를 마지막에 뿌리기 위해 변경
//	ver.112 - DxPieceManager 추가
//	ver.111 - Replace_Single After 추가
//	ver.110 - Replace_Single After 추가
//	ver.109 - Octree 최적화로 데이터 변경.
//	ver.108 - Fog 추가로 변화. Sky 추가로 변화.
//	ver.107 - 날씨 file mark 위치 추가로 인한 변화.
//
//
//

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD	DxLandMan::VERSION = 0x0123;
const char	DxLandMan::FILEMARK[128] = "LAND.MAN";

BOOL DxLandMan::SaveFile ( LPDIRECT3DDEVICEQ pd3dDevice, const char *szFile, boost::function<void (const TCHAR*)> pArchive )
{
    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return FALSE;
    }

    sc::SerialFile& SFile = *spFile;
	BOOL	hr = FALSE;

	SFile.SetFileType ( (char*)FILEMARK, DxLandMan::VERSION );

	if ( hr = SFile.OpenFile ( FOT_WRITE, szFile ) )
	{
		//	Note : 맵 ID 저장.
		SFile << m_MapID.dwID;
		SFile.WriteBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

		//	Note : 파일중 일부 데이터를 랜덤 접근하기 위한 파일 마크 위치 저장.
		//
		long nFILEMARK = SFile.GetfTell ();
		m_sFILEMARK.SaveSet ( SFile );

		//	Note	:	세이브
		//
		hr = m_DxOctree.SaveFile ( SFile );
		if ( !hr )
		{
			return hr;
		}

		SFile << BOOL ( m_pPieceObjHead!=NULL );
        if ( m_pPieceObjHead )
		{
			m_pPieceObjHead->SaveFile ( SFile );
		}

		SFile << BOOL ( m_pAniManHead!=NULL );
		if ( m_pAniManHead )
		{
			m_pAniManHead->SaveFile ( SFile );
		}

		// Replace
		{
			DxEffectBase* pEffCur;
			DWORD dwEffCount = 0;
			
			pEffCur = m_pEffectList;
			while ( pEffCur )
			{
				dwEffCount++;
				pEffCur = pEffCur->pEffectNext;
			}

			SFile << dwEffCount;

			pEffCur = m_pEffectList;
			while ( pEffCur )
			{
				pEffCur->SaveFile ( SFile );
				pEffCur = pEffCur->pEffectNext;
			}
		}

		// Replace - AFTER
		{
			DxEffectBase* pEffCur;
			DWORD dwEffCount = 0;
			
			pEffCur = m_pEffectList_AFTER;
			while ( pEffCur )
			{
				dwEffCount++;
				pEffCur = pEffCur->pEffectNext;
			}

			SFile << dwEffCount;

			pEffCur = m_pEffectList_AFTER;
			while ( pEffCur )
			{
				pEffCur->SaveFile ( SFile );
				pEffCur = pEffCur->pEffectNext;
			}
		}

		// Replace - AFTER_1
		{
			DxEffectBase* pEffCur;
			DWORD dwEffCount = 0;
			
			pEffCur = m_pEffectList_AFTER_1;
			while ( pEffCur )
			{
				dwEffCount++;
				pEffCur = pEffCur->pEffectNext;
			}

			SFile << dwEffCount;

			pEffCur = m_pEffectList_AFTER_1;
			while ( pEffCur )
			{
				pEffCur->SaveFile ( SFile );
				pEffCur = pEffCur->pEffectNext;
			}
		}

		m_sFILEMARK.dwNAVI_MARK = SFile.GetfTell ();
		m_sNaviMeshCapsule.SaveForNaviMeshInWld( SFile );


		//	Note : Land 에 삽입된 SkinObject
		//
		m_spLandSkinManager->SaveFile( SFile );
		

		//	Note : StaticMesh
		SFile << BOOL ( TRUE );
		if ( m_spStaticMeshLightMap.get() )
        {
            // 형상 자체 저장
            {
                m_spStaticMeshLightMap->Save( szFile, pArchive );
            }

            // TextureEffect 를 위해 자체 저장.
            {
                DxTextureEffNewFile sTexEffNewFile;
                m_spStaticMeshLightMap->GetTexEff( pd3dDevice, sTexEffNewFile );
                sTexEffNewFile.Save( szFile, pArchive );
            }
        }

		SFile << BOOL ( TRUE );
		if ( m_spStaticMaterialMeshLightMap.get() )
		{
			// 형상 자체 저장
			m_spStaticMaterialMeshLightMap->Save( szFile, pArchive );
		}

        // Note : LightMan 
        m_pLightMan->Save ( SFile );

		// Note : DxPieceManager
		SFile << BOOL ( TRUE );
		{
			// CellSize 를 구하는 방법
			// Fov 로 인한 시야거리를 기준으로 빗변을 구한다.
			// 거기서 50% 정도 값으로 변경한다.
			// 거기서 10% 정도 값으로 늘린다.
			float fCellSize = (DxFogMan::GetInstance().GetGameFogHighEnd( GetFogProperty() ) / cosf( DxViewPort::GetInstance().GetFOV() * 0.5f )) * 0.5f * 1.1f;
			m_spPieceManager->SaveWLD( SFile, fCellSize );
		}


		SFile << BOOL ( m_pLandEffList!=NULL );
		PLANDEFF pCur = m_pLandEffList;
		while ( pCur )
		{
			pCur->Save ( SFile );

			pCur = pCur->m_pNext;
			SFile << BOOL ( pCur!=NULL );
		}

		SFile << BOOL ( TRUE );
		m_CameraAniMan.Save ( SFile );

		
		SFile << BOOL ( TRUE );
		m_sFILEMARK.dwGATE_MARK = SFile.GetfTell ();
		m_LandGateMan.Save ( SFile );



		// Replace - Weather
		{
			DxEffectBase* pEffCur;
			DWORD dwEffCount = 0;
			
			pEffCur = m_pList_Weather;
			while ( pEffCur )
			{
				dwEffCount++;
				pEffCur = pEffCur->pEffectNext;
			}

			SFile << dwEffCount;

			pEffCur = m_pList_Weather;
			while ( pEffCur )
			{
				pEffCur->SaveFile ( SFile );
				pEffCur = pEffCur->pEffectNext;
			}
		}

		//	Note : Weather
		//
		SFile << BOOL ( TRUE );
		m_sFILEMARK.dwWEATHER_MARK = SFile.GetfTell ();
		m_WeatherMan.SaveSet ( SFile );

		//	Note : ColiisonMap
		//
		SFile << BOOL ( TRUE );
		m_sFILEMARK.dwCOLL_MARK = SFile.GetfTell ();
		SFile.BeginBlock();
		//	m_CollisionMap.SaveFile ( SFile );		// 사용되지 않는다.
		SFile.EndBlock();

		//	Note : Fog
		SFile << BOOL ( TRUE );
		m_FOG_PROPERTY.SaveSet ( SFile );

		//	Note : Sky
		SFile << BOOL ( TRUE );
		m_SKY_PROPERTY.SaveSet ( SFile );	


		DxSoundMan::GetInstance().SaveSet ( SFile );

		SFile << BOOL ( m_pStaticSoundMan!=NULL );
		if ( m_pStaticSoundMan )
		{
			m_pStaticSoundMan->SaveSet ( SFile );
		}

		//	Note : Bgm Sound
		//
		SFile << BOOL ( TRUE );
		m_BGMDATA.SaveSet ( SFile );	

		SFile << m_bCamCollision;

        // Note : DxSkyBoxMan
		SFile << BOOL ( TRUE );
		DxSkyBoxMan::GetInstance().Save( SFile );

		//	Note : Field Water
		SFile << BOOL ( TRUE );
		NSWaterMain::SaveSurf( SFile );

		//	Note : 만들어진 시간 셋팅.
		SFile << BOOL ( TRUE );
		{
			CTime sTime = CTime::GetCurrentTime();
			CString strTime;
			strTime.Format ( "%d_%02d_%02d_%02d_%02d_%02d", sTime.GetYear(), sTime.GetMonth(), sTime.GetDay(),
				sTime.GetHour(), sTime.GetMinute(), sTime.GetSecond() );
			m_strMakeTimeWLD = strTime.GetString();
			SFile << m_strMakeTimeWLD;
		}

		//	Note : RAD 정보
		SFile << BOOL ( TRUE );
		SFile << m_dwRAD_Pass;
		SFile << m_fRAD_Reflectance;
		SFile << m_fRAD_FogBlend;

		//	Note : RAD 정보
		SFile << BOOL ( TRUE );
		SFile << m_dwRAD_FogLength;

		//	Note : NSShadowLight 정보
		SFile << BOOL ( TRUE );
		m_scpShadowLightData->Save( SFile );

		//	Note : 다음에 추가될 데이터를 위해서 마킹.
		//
		SFile << BOOL ( FALSE );

		//	Note : 각종 Data의 파일저장 위치를 파일 헤더 부분에 저장.
		//		( 주의 ) 모든 파일의 정보를 저장후 가장 마지막에 행한다.
		//
		SFile.SetOffSet ( nFILEMARK );
		m_sFILEMARK.SaveSet ( SFile );
	}

	return hr;
}
//
////	Note : 치환 조각으로 저장하기 위해서 사용하는 Save 함수.
////		치환 조각은 형상 + 종속효과 + 에니메이션 으로 이루어진다.
////
//BOOL DxLandMan::SavePieceFile ( const char *szFile )
//{
//	sc::SerialFile SFile;
//	BOOL	hr = FALSE;
//
//	if ( !m_pStaticMesh && !m_pAniManHead )
//	{
//		MessageBox ( NULL, "형상이 존재 하지 않습니다.\nExport 후에 저장하세요.", "치환조각 저장 - 오류", MB_OK );
//		return FALSE;
//	}
//
//	D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
//	D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);
//
//	D3DXVECTOR3 vMaxCur, vMinCur;
//
//	if ( m_pStaticMesh )
//	{
//		vMax = m_pStaticMesh->GetAABBMax();
//		vMin = m_pStaticMesh->GetAABBMin();
//	}
//
//	DxAnimationMan *pAinManCur = m_pAniManHead;
//	while(pAinManCur)
//	{
//		DxFrame *pFrame = pAinManCur->GetDxFrameRoot ();
//		if ( pFrame )
//		{
//			vMaxCur = pFrame->vTreeMax;
//			vMinCur = pFrame->vTreeMin;
//
//			if ( vMax.x < vMaxCur.x )	vMax.x = vMaxCur.x;
//			if ( vMin.x > vMinCur.x )	vMin.x = vMinCur.x;
//
//			if ( vMax.y < vMaxCur.y )	vMax.y = vMaxCur.y;
//			if ( vMin.y > vMinCur.y )	vMin.y = vMinCur.y;
//
//			if ( vMax.z < vMaxCur.z )	vMax.z = vMaxCur.z;
//			if ( vMin.z > vMinCur.z )	vMin.z = vMinCur.z;
//		}
//
//		pAinManCur = pAinManCur->m_pNext;
//	}
//
//	DxEffectBase* pEffCur = m_pEffectList;
//	while ( pEffCur )
//	{
//		pEffCur->GetAABBSize ( vMaxCur, vMinCur );
//
//		if ( vMax.x < vMaxCur.x )	vMax.x = vMaxCur.x;
//		if ( vMin.x > vMinCur.x )	vMin.x = vMinCur.x;
//
//		if ( vMax.y < vMaxCur.y )	vMax.y = vMaxCur.y;
//		if ( vMin.y > vMinCur.y )	vMin.y = vMinCur.y;
//
//		if ( vMax.z < vMaxCur.z )	vMax.z = vMaxCur.z;
//		if ( vMin.z > vMinCur.z )	vMin.z = vMinCur.z;
//
//		pEffCur = pEffCur->pEffectNext;
//	}
//
//	if ( hr = SFile.OpenFile ( FOT_WRITE, szFile ) )
//	{
//		SFile << vMax;
//		SFile << vMin;
//
//
//		// 원래는 옥트리 형상이었는데 Version Up 으로 안 쓰인다.
//		SFile << FALSE;
//
//		if ( m_pAniManHead )
//		{
//			SFile << BOOL ( TRUE );
//			m_pAniManHead->SaveFile ( SFile );
//		}
//		else
//		{
//			SFile << BOOL ( FALSE );
//		}
//
//		SFile << TRUE; // bRendAni;
//
//		DxEffectBase* pEffCur = NULL;
//		DWORD dwEffCount = 0;
//
//		pEffCur = m_pEffectList;
//		while ( pEffCur )
//		{
//			dwEffCount++;
//			pEffCur = pEffCur->pEffectNext;
//		}
//
//		SFile << dwEffCount;
//
//		pEffCur = m_pEffectList;
//		while ( pEffCur )
//		{
//			pEffCur->SaveFile ( SFile );
//			pEffCur = pEffCur->pEffectNext;
//		}
//
//		if ( m_pLandEffList )	SFile << BOOL ( TRUE );
//		else					SFile << BOOL ( FALSE );
//
//		PLANDEFF pCur = m_pLandEffList;
//		while ( pCur )
//		{
//			pCur->Save ( SFile );
//
//			pCur = pCur->m_pNext;
//			if ( pCur )		SFile << BOOL ( TRUE );
//			else			SFile << BOOL ( FALSE );
//		}
//
//		if ( m_pStaticMesh )
//		{
//			SFile << TRUE;
//			m_pStaticMesh->Save( SFile, TRUE );
//		}
//		else
//		{
//			SFile << FALSE;
//		}
//
//		//	Note : 다음 새이브 될 
//		//
//		SFile << BOOL ( FALSE );
//	}
//	//sc::SerialFile SFile;
//	//BOOL	hr = FALSE;
//
//	//if ( m_DxOctree.IsSubDivision() )
//	//{
//	//	MessageBox ( NULL, "Export 시에 페이스 분할 개수가 과소하게 정해졌습니다.\n"
//	//		"분할 개수를 더 크게 설정하여 하나로 분할되게 하십시요.", "치환조각 저장 - 오류", MB_OK );
//	//	return FALSE;
//	//}
//	//
//	//if ( !m_DxOctree.GetDxFrameHead() && !m_pAniManHead )
//	//{
//	//	MessageBox ( NULL, "형상이 존재 하지 않습니다.\nExport 후에 저장하세요.", "치환조각 저장 - 오류", MB_OK );
//	//	return FALSE;
//	//}
//
//	//D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
//	//D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);
//
//	//D3DXVECTOR3 vMaxCur, vMinCur;
//
//	//if ( m_DxOctree.GetDxFrameHead() )
//	//{
//	//	vMax = m_DxOctree.GetMax();
//	//	vMin = m_DxOctree.GetMin();
//	//}
//
//	//DxAnimationMan *pAinManCur = m_pAniManHead;
//	//while(pAinManCur)
//	//{
//	//	DxFrame *pFrame = pAinManCur->GetDxFrameRoot ();
//	//	if ( pFrame )
//	//	{
//	//		vMaxCur = pFrame->vTreeMax;
//	//		vMinCur = pFrame->vTreeMin;
//
//	//		if ( vMax.x < vMaxCur.x )	vMax.x = vMaxCur.x;
//	//		if ( vMin.x > vMinCur.x )	vMin.x = vMinCur.x;
//
//	//		if ( vMax.y < vMaxCur.y )	vMax.y = vMaxCur.y;
//	//		if ( vMin.y > vMinCur.y )	vMin.y = vMinCur.y;
//
//	//		if ( vMax.z < vMaxCur.z )	vMax.z = vMaxCur.z;
//	//		if ( vMin.z > vMinCur.z )	vMin.z = vMinCur.z;
//	//	}
//
//	//	pAinManCur = pAinManCur->m_pNext;
//	//}
//
//	//DxEffectBase* pEffCur = m_pEffectList;
//	//while ( pEffCur )
//	//{
//	//	pEffCur->GetAABBSize ( vMaxCur, vMinCur );
//
//	//	if ( vMax.x < vMaxCur.x )	vMax.x = vMaxCur.x;
//	//	if ( vMin.x > vMinCur.x )	vMin.x = vMinCur.x;
//
//	//	if ( vMax.y < vMaxCur.y )	vMax.y = vMaxCur.y;
//	//	if ( vMin.y > vMinCur.y )	vMin.y = vMinCur.y;
//
//	//	if ( vMax.z < vMaxCur.z )	vMax.z = vMaxCur.z;
//	//	if ( vMin.z > vMinCur.z )	vMin.z = vMinCur.z;
//
//	//	pEffCur = pEffCur->pEffectNext;
//	//}
//
//	//if ( hr = SFile.OpenFile ( FOT_WRITE, szFile ) )
//	//{
//	//	SFile << vMax;
//	//	SFile << vMin;
//
//	//	DxFrame* pDxFrameHead = m_DxOctree.GetDxFrameHead();
//
//	//	if ( pDxFrameHead )
//	//	{
//	//		SFile << TRUE;
//	//		pDxFrameHead->SaveFile(SFile);
//	//	}
//	//	else
//	//	{
//	//		SFile << FALSE;
//	//	}
//
//	//	if ( m_pAniManHead )
//	//	{
//	//		SFile << BOOL ( TRUE );
//	//		m_pAniManHead->SaveFile ( SFile );
//	//	}
//	//	else
//	//	{
//	//		SFile << BOOL ( FALSE );
//	//	}
//
//	//	SFile << TRUE; // bRendAni;
//
//	//	DxEffectBase* pEffCur = NULL;
//	//	DWORD dwEffCount = 0;
//
//	//	pEffCur = m_pEffectList;
//	//	while ( pEffCur )
//	//	{
//	//		dwEffCount++;
//	//		pEffCur = pEffCur->pEffectNext;
//	//	}
//
//	//	SFile << dwEffCount;
//
//	//	pEffCur = m_pEffectList;
//	//	while ( pEffCur )
//	//	{
//	//		pEffCur->SaveFile ( SFile );
//	//		pEffCur = pEffCur->pEffectNext;
//	//	}
//
//	//	if ( m_pLandEffList )	SFile << BOOL ( TRUE );
//	//	else					SFile << BOOL ( FALSE );
//
//	//	PLANDEFF pCur = m_pLandEffList;
//	//	while ( pCur )
//	//	{
//	//		pCur->Save ( SFile );
//
//	//		pCur = pCur->m_pNext;
//	//		if ( pCur )		SFile << BOOL ( TRUE );
//	//		else			SFile << BOOL ( FALSE );
//	//	}
//
//	//	//	Note : 다음 새이브 될 
//	//	//
//	//	SFile << BOOL ( FALSE );
//	//}
//
//	return TRUE;
//}

HRESULT DxLandMan::EffectLoadToList ( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, PDXEFFECTBASE &pEffList, BOOL bDummy )
{
	BOOL	bResult = FALSE;
	HRESULT	hr = E_FAIL;
	char	*szName = NULL;
	int		StrLength;

	SFile >> bResult;
	if ( bResult )
	{
		SFile >> StrLength;
		szName = new char [ StrLength ];
		SFile.ReadBuffer ( szName, StrLength );
	}

    DWORD	TypeID;
	SFile >> TypeID;
	
	DWORD	dwSize = 0;
	BYTE	*pProperty = NULL;
	DWORD	dwVer = 0;

	SFile >> dwVer;
	SFile >> dwSize;
	if ( dwSize )
	{
		pProperty = new BYTE [ dwSize ];
		SFile.ReadBuffer ( pProperty, sizeof ( BYTE ) * dwSize );
	}

	LPDXAFFINEPARTS	pAffineParts = NULL;
	SFile >> bResult;
	if ( bResult )
	{
		pAffineParts = new DXAFFINEPARTS;
		SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
	}

	DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance ( TypeID );
	if ( !pEffect )
	{
		SAFE_DELETE_ARRAY ( pProperty );
		return E_FAIL;
	}

	//	Note : Property, Affine  값을 대입.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

	//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
	//
	//pEffect->LoadBuffer ( SFile, dwVer, pd3dDevice );

	if ( bDummy && pEffect->GetTypeID()==DEF_EFFECT_TILING )
	{
		pEffect->LoadBuffer_NEW( SFile, dwVer, pd3dDevice, TRUE );
		SAFE_DELETE ( pEffect );

		SAFE_DELETE_ARRAY ( pProperty );
		SAFE_DELETE ( pAffineParts );
		SAFE_DELETE_ARRAY ( szName );

		return S_OK;
	}
	else
	{
		//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
		//
		pEffect->LoadBuffer ( SFile, dwVer, pd3dDevice );
	}

	//	Note : Create Device.
	//
	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);

		SAFE_DELETE_ARRAY ( pProperty );
		SAFE_DELETE ( pAffineParts );
		SAFE_DELETE_ARRAY ( szName );

		return E_FAIL;
	}

    SAFE_DELETE_ARRAY ( pProperty );
	SAFE_DELETE ( pAffineParts );
	SAFE_DELETE_ARRAY ( szName );

	//	Note : Insert To List.
	//
	pEffect->pEffectNext = pEffList;
	pEffList = pEffect;

	// Note : 값 뒤집기	< List 순서를 유지하기 위해서 >
	DxEffectBase* pTemp = NULL;
	DxEffectBase* pCur	= pEffList;
	DxEffectBase* pThis = pEffList;
	while ( pCur )
	{
		pThis = pCur;
		pCur = pCur->pEffectNext;

		pThis->pEffectNext = pTemp;
		pTemp = pThis;
	}
	pEffList = pTemp;

	return S_OK;
}

HRESULT DxLandMan::EffectLoadToList( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, PDXEFFECTBASE &pEffList, PDXEFFECTBASE &pEffList_AFTER,
									 DWORD& dwCount, DWORD& dwCount_AFTER, BOOL bDummy )
{
	BOOL	bResult = FALSE;
	HRESULT	hr = E_FAIL;
	char	*szName = NULL;
	int		StrLength;

	SFile >> bResult;
	if ( bResult )
	{
		SFile >> StrLength;
		szName = new char [ StrLength ];
		SFile.ReadBuffer ( szName, StrLength );
	}

    DWORD	TypeID;
	SFile >> TypeID;
	
	DWORD	dwSize = 0;
	BYTE	*pProperty = NULL;
	DWORD	dwVer = 0;

	SFile >> dwVer;
	SFile >> dwSize;
	if ( dwSize )
	{
		pProperty = new BYTE [ dwSize ];
		SFile.ReadBuffer ( pProperty, sizeof ( BYTE ) * dwSize );
	}

	LPDXAFFINEPARTS	pAffineParts = NULL;
	SFile >> bResult;
	if ( bResult )
	{
		pAffineParts = new DXAFFINEPARTS;
		SFile.ReadBuffer ( pAffineParts, sizeof ( DXAFFINEPARTS ) );
	}

	DxEffectBase *pEffect = DxEffectMan::GetInstance().CreateEffInstance ( TypeID );
	if ( !pEffect )
	{
		SAFE_DELETE_ARRAY ( pProperty );
		SAFE_DELETE ( pAffineParts );
		SAFE_DELETE_ARRAY ( szName );
		return E_FAIL;
	}

	//	Note : Property, Affine  값을 대입.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

	//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
	//
	//pEffect->LoadBuffer ( SFile, dwVer, pd3dDevice, bDummy );

	if ( bDummy && pEffect->GetTypeID()==DEF_EFFECT_TILING )
	{
		pEffect->LoadBuffer_NEW( SFile, dwVer, pd3dDevice, TRUE );
		SAFE_DELETE ( pEffect );

		SAFE_DELETE_ARRAY ( pProperty );
		SAFE_DELETE ( pAffineParts );
		SAFE_DELETE_ARRAY ( szName );

		return S_OK;
	}
	else
	{
		//	Note : LoadBuffer(), 특정 Effect 에서만 사용되는 메모리 버퍼를 읽는다.
		//
		pEffect->LoadBuffer ( SFile, dwVer, pd3dDevice );
	}

	//	Note : Create Device.
	//
	hr = pEffect->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffect);

		SAFE_DELETE_ARRAY ( pProperty );
		SAFE_DELETE ( pAffineParts );
		SAFE_DELETE_ARRAY ( szName );

		return E_FAIL;
	}

    SAFE_DELETE_ARRAY ( pProperty );
	SAFE_DELETE ( pAffineParts );
	SAFE_DELETE_ARRAY ( szName );

	if( pEffect->GetFlag()&(_EFF_REPLACE_AFTER|_EFF_SINGLE_AFTER_0|_EFF_REPLACE_AFTER_1|_EFF_SINGLE_AFTER_1|_EFF_SINGLE_AFTER_2) )
	{
		pEffect->pEffectNext = pEffList_AFTER;
		pEffList_AFTER = pEffect;

		++dwCount;
	}
	else
	{
		//	Note : Insert To List.
		//
		pEffect->pEffectNext = pEffList;
		pEffList = pEffect;

		// Note : 값 뒤집기	< List 순서를 유지하기 위해서 >
		DxEffectBase* pTemp = NULL;
		DxEffectBase* pCur	= pEffList;
		DxEffectBase* pThis = pEffList;
		while ( pCur )
		{
			pThis = pCur;
			pCur = pCur->pEffectNext;

			pThis->pEffectNext = pTemp;
			pTemp = pThis;
		}
		pEffList = pTemp;

		++dwCount_AFTER;
	}

	return S_OK;
}

BOOL DxLandMan::LoadFile ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad )
{
	//	Note : 앳 데이타 모두 제거.
	CleanUp();

	m_pd3dDevice = pd3dDevice;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// Note : 현재 맵의 설정으로 바꿈
	ActiveMapIn();

	OctreeDebugInfo::EndNodeCount = 0;
	OctreeDebugInfo::AmountDxFrame = 0;

	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, m_szPath );
	StringCchCat( szPathName, MAX_PATH, szFile );
	
	char szFileType[FILETYPESIZE] = "";
	DWORD dwVersion;

	if ( hr = m_SFileMap.OpenFile ( FOT_READ, szPathName ) )
	{
		//	Note : 파일 형식 버전확인.
		//
		m_SFileMap.GetFileType( szFileType, FILETYPESIZE, dwVersion );

		if ( strcmp(FILEMARK,szFileType) )
		{
			CString Str;
			Str.Format ( "[%s]  맵 파일 형식이 아닙니다.", szFile );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		if ( dwVersion>=0x0108 && dwVersion<=DxLandMan::VERSION )
        {
        }
		else
		{
			CString Str;
			Str.Format ( "[%s]  Error Load Map File!!!", szFile );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

        // 기존에 있던 정보를 지운다.
        DxPieceContainer::GetInstance().CleanUp();

		// 
		NSOctreeRenderList::g_nRenderIndex = 0;
		NSOcclusionCulling::Load( szPathName );				// 자체 파일 로드
		NSCubeMapCulling::Load( pd3dDevice, szPathName );	// 자체 파일 로드

		// ZoomOut 디폴트 값
		DxViewPort::GetInstance().SetZoomOutDefaultGameSetting();

        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		m_strNameWLD = szFile;

		if ( dwVersion >= 0x0120 )
		{
			// LightMapAmbientColor는 버전 120 에서부터 사용 가능하다.
			NSLightMapAmbientColor::g_bActiveLightMapAmbientColor = TRUE;
		}
		else
		{
			NSLightMapAmbientColor::g_bActiveLightMapAmbientColor = FALSE;
		}

		if ( dwVersion >= 0x0121 )
		{
			// MaterialSystem ps.2.0 은 버전 121 에서부터 사용 가능하다.
			m_bUsedMaterialSystem = TRUE;
		}
		else
		{
			m_bUsedMaterialSystem = FALSE;
		}

		if ( dwVersion >= 0x0123 )
		{
			// MaterialSystem ps.3.0 은 버전 123 에서부터 사용 가능하다.
			m_bUsedMaterialSystemPS_3_0 = TRUE;
		}
		else
		{
			m_bUsedMaterialSystemPS_3_0 = FALSE;
		}

		//////////////////////////////////////////////////////////////////////////
		// [shhan][2014.09.16] 실내맵 라이트 문제가 있는데, 그래픽팀에서 작업을 하면서 프로그램쪽도 잘되는지 확인을 해야 한다.
		//					그래서 임시로 ps.3.0 버젼은 막도록 한다.
		// [shhan][2015.03.17] 실내맵으로 동작되도록 변경함.
		BOOL bForceIndoor(FALSE);
		std::transform( m_strNameWLD.begin(), m_strNameWLD.end(), m_strNameWLD.begin(), tolower );
		if ( m_strNameWLD == _T("pvptournament.wld" ) || 
			m_strNameWLD == _T("arcade_ground01.wld" ) ||
			m_strNameWLD == _T("fs_main01.wld" ) ||
			m_strNameWLD == _T("fs_main02.wld" ) )
			//||
			//m_strNameWLD == _T("log_in.wld" ) )			// log_in.wld 는 그림자를 SoftShadow 했더니 최상옵에서 그림자가 이상하게 보이는 현상이 있어서 이곳에 추가됨.
		{
			bForceIndoor = TRUE;
			//m_bUsedMaterialSystemPS_3_0 = FALSE;
		}
		//////////////////////////////////////////////////////////////////////////

		BOOL bResult(FALSE);
		switch ( dwVersion )
		{
		case 0x0108:
			bResult = LoadFile_VER108 ( szFile, pd3dDevice, bUseDynamicLoad );
			break;
		case 0x0109:
			bResult = LoadFile_VER109 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
		case 0x0110:
			bResult = LoadFile_VER110 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
		case 0x0111:
		case 0x0112:
			bResult = LoadFile_VER112 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
		case 0x0113:
		case 0x0114:
			bResult = LoadFile_VER114 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
		case 0x0115:
			bResult = LoadFile_VER115 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
		case 0x0116:
		case 0x0117:
			bResult = LoadFile_VER117 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
        case 0x0118:
			bResult = LoadFile_VER118 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
        case 0x0119:
			bResult = LoadFile_VER119 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
		case 0x0120:
			bResult = LoadFile_VER120 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
		case 0x0121:
			bResult = LoadFile_VER121 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName );	
			break;
		case 0x0122:
			bResult = LoadFile_VER122_123 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName, bForceIndoor );	
			break;
		case 0x0123:
			bResult = LoadFile_VER122_123 ( szFile, pd3dDevice, bUseDynamicLoad, szPathName, bForceIndoor );
			break;
		default:
            {
                std::string ErrorMsg(
                    sc::string::format(
                        "DxLandMan::LoadFile unknown version. %1% %2%",
                        szFile, dwVersion));
                sc::ErrorMessageBox(ErrorMsg, false);
                return FALSE;
            }            
			break;
		};




		//////////////////////////////////////////////////////////////////////////
		//		위쪽에서 로딩 된 것을 다시 한번 새로운 정보로 덮어쓴다.
		NSLandSettingLua::Load( pd3dDevice, szPathName );	// 자체 파일 로드


		// 
		// Material 을 구한다.
		Active_MATERIAL_DATA( pd3dDevice, FALSE );


		SetNSLandThread();
		//NSLandThread::SetLandSkinManager( m_spLandSkinManager );


        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//



		return bResult;
	}

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER122_123( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName, BOOL bForceIndoor )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )
        return hr;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		

		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
            return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			//srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;

        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_NEW( m_SFileMap, pd3dDevice );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	// 타일 관련 문제가 이곳에서는 생기지 않는다. 사용자체를 하지 않기 때문임.
	// Replace
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );
	

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load( pd3dDevice, szPathName );	// 자체 파일 로드

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
        {
            // m_pStaticMesh 에 적용을 한다.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	// 
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMaterialMeshLightMap = boost::shared_ptr<DxStaticMaterialMeshLightMap>(new DxStaticMaterialMeshLightMap);
		m_spStaticMaterialMeshLightMap->Load( pd3dDevice, szPathName );	// 자체 파일 로드
	}
	else return TRUE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    //	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE, TRUE );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );

			m_SFileMap >> bExist;
		}

		BuildSingleEffTree ();
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// Replace - Weather
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pList_Weather );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwCount_Weather++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}
    else return TRUE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}
    else return TRUE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	m_SFileMap >> m_bCamCollision;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    //	Note : DxSkyBoxMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		DxSkyBoxMan::GetInstance().Load( pd3dDevice, m_SFileMap, TRUE );
	}
	else return TRUE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// Note : NSWaterMain Surf
	m_SFileMap >> bExist;
	if ( bExist )
	{
		NSWaterMain::LoadSurf( m_SFileMap, pd3dDevice );
	}
	else return TRUE;

	//	Note : 만들어진 시간 셋팅.
	m_SFileMap >> bExist;
	if ( bExist )
	{
		TSTRING strTime;
		m_SFileMap >> m_strMakeTimeWLD;

		// Occlusion Culling Data 가 정상데이터인지 확인한다.
		NSOcclusionCulling::IntegrityCheck( m_strMakeTimeWLD );

		// CubeMap Culling Data 가 정상데이터인지 확인한다.
		NSCubeMapCulling::IntegrityCheck( m_strMakeTimeWLD );
	}
	else return TRUE;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SFileMap >> m_dwRAD_Pass;
		m_SFileMap >> m_fRAD_Reflectance;
		m_SFileMap >> m_fRAD_FogBlend;
	}
	else return TRUE;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SFileMap >> m_dwRAD_FogLength;
	}
	else return TRUE;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_scpShadowLightData->Load( m_SFileMap );
		if ( bForceIndoor )
		{
			m_scpShadowLightData->m_bDirectionLight = FALSE;
		}
		NSShadowLight::SetActive( m_scpShadowLightData.get() );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER121( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )
		return hr;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		

		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
			return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			//srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;

		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	// 타일 관련 문제가 이곳에서는 생기지 않는다. 사용자체를 하지 않기 때문임.
	// Replace
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load( pd3dDevice, szPathName );	// 자체 파일 로드

		DxTextureEffNewFile sTexEffNewFile;
		if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
		{
			// m_pStaticMesh 에 적용을 한다.
			m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
		}
	}
	else return TRUE;

	// 
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMaterialMeshLightMap = boost::shared_ptr<DxStaticMaterialMeshLightMap>(new DxStaticMaterialMeshLightMap);
		m_spStaticMaterialMeshLightMap->Load( pd3dDevice, szPathName );	// 자체 파일 로드
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE, TRUE );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );

			m_SFileMap >> bExist;
		}

		BuildSingleEffTree ();
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Replace - Weather
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pList_Weather );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwCount_Weather++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> m_bCamCollision;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : DxSkyBoxMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		DxSkyBoxMan::GetInstance().Load( pd3dDevice, m_SFileMap, TRUE );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Note : NSWaterMain Surf
	m_SFileMap >> bExist;
	if ( bExist )
	{
		NSWaterMain::LoadSurf( m_SFileMap, pd3dDevice );
	}
	else return TRUE;

	//	Note : 만들어진 시간 셋팅.
	m_SFileMap >> bExist;
	if ( bExist )
	{
		TSTRING strTime;
		m_SFileMap >> m_strMakeTimeWLD;

		// Occlusion Culling Data 가 정상데이터인지 확인한다.
		NSOcclusionCulling::IntegrityCheck( m_strMakeTimeWLD );
	}
	else return TRUE;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SFileMap >> m_dwRAD_Pass;
		m_SFileMap >> m_fRAD_Reflectance;
		m_SFileMap >> m_fRAD_FogBlend;
	}
	else return TRUE;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SFileMap >> m_dwRAD_FogLength;
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER120( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )
		return hr;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		

		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
			return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			//srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;

		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	// Replace
	DWORD dwEffectListStartMark = EffectLoadToList_Dummy( pd3dDevice,m_SFileMap );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load( pd3dDevice, szPathName );	// 자체 파일 로드

		DxTextureEffNewFile sTexEffNewFile;
		if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
		{
			// m_pStaticMesh 에 적용을 한다.
			m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
		}
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );

			m_SFileMap >> bExist;
		}

		BuildSingleEffTree ();
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Replace - Weather
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pList_Weather );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwCount_Weather++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> m_bCamCollision;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : DxSkyBoxMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		DxSkyBoxMan::GetInstance().Load( pd3dDevice, m_SFileMap, TRUE );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Note : NSWaterMain Surf
	m_SFileMap >> bExist;
	if ( bExist )
	{
		NSWaterMain::LoadSurf( m_SFileMap, pd3dDevice );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER119( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )
		return hr;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		

		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
			return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			//srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;

		//---------------------------------------------------//
		NSLoadingDirect::FrameMoveRender( FALSE );
		//---------------------------------------------------//
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	// Replace
	DWORD dwEffectListStartMark = EffectLoadToList_Dummy( pd3dDevice,m_SFileMap );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// 자체 파일 로드

		DxTextureEffNewFile sTexEffNewFile;
		if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
		{
			// m_pStaticMesh 에 적용을 한다.
			m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
		}
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );

			m_SFileMap >> bExist;
		}

		BuildSingleEffTree ();
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Replace - Weather
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pList_Weather );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwCount_Weather++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	m_SFileMap >> m_bCamCollision;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : DxSkyBoxMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		DxSkyBoxMan::GetInstance().Load( pd3dDevice, m_SFileMap, TRUE );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Note : NSWaterMain Surf
	m_SFileMap >> bExist;
	if ( bExist )
	{
		NSWaterMain::LoadSurf( m_SFileMap, pd3dDevice );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER118( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		

		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( szFileName );
		if (!pPiece)
            return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			//srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );



	// Replace
	DWORD dwEffectListStartMark = EffectLoadToList_Dummy( pd3dDevice,m_SFileMap );

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}


	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// 자체 파일 로드

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
        {
            // m_pStaticMesh 에 적용을 한다.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

    //	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );

			m_SFileMap >> bExist;
		}

		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	// Replace - Weather
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pList_Weather );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwCount_Weather++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}


	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;



	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	m_SFileMap >> m_bCamCollision;

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER117( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		

		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
            return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			//srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );



	// Replace
	DWORD dwEffectListStartMark = EffectLoadToList_Dummy( pd3dDevice,m_SFileMap );

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}


	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// 자체 파일 로드

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
        {
            // m_pStaticMesh 에 적용을 한다.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;


	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD_PREV( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );

			m_SFileMap >> bExist;
		}

		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	// Replace - Weather
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pList_Weather );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwCount_Weather++;
		}

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}


	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;



	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	m_SFileMap >> m_bCamCollision;

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER115( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
            return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			//srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );



	// Replace
	DWORD dwEffectListStartMark = EffectLoadToList_Dummy( pd3dDevice,m_SFileMap );

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}
	
		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}


	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// 자체 파일 로드

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
        {
            // m_pStaticMesh 에 적용을 한다.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;


	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD_PREV( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	// Replace - Weather
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pList_Weather );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwCount_Weather++;
		}
	
		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}


	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;



	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER114( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
            return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			//srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	// Replace
	DWORD dwEffectListStartMark = EffectLoadToList_Dummy( pd3dDevice,m_SFileMap );

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}
	
		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}

	// Replace - Weather
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pList_Weather );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwCount_Weather++;
		}
	
		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	}

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// 자체 파일 로드

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
        {
            // m_pStaticMesh 에 적용을 한다.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD_PREV( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	return TRUE;
}


BOOL DxLandMan::LoadFile_VER112( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
            return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
			//srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	// Replace
	DWORD dwEffectListStartMark = EffectLoadToList_Dummy( pd3dDevice,m_SFileMap );

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		// Note : Weather 효과가 있는 것을 빼온다.
		SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
		COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );
	
		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	// Replace - AFTER_1
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER_1 );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// 자체 파일 로드

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
        {
            // m_pStaticMesh 에 적용을 한다.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	//	DxPieceManager	ver.112 에서 추가 됨
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD_PREV( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER110( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
            return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
		//	srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	// Replace
	DWORD dwEffectListStartMark = EffectLoadToList_Dummy( pd3dDevice,m_SFileMap );

	// Replace - AFTER
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList_AFTER );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		// Note : Weather 효과가 있는 것을 빼온다.
		SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
		COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );


	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// 자체 파일 로드

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
        {
            // m_pStaticMesh 에 적용을 한다.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER109 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad, const char* szPathName )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
            return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
		//	srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	// EffectLoadToList Dummy
	DWORD dwEffectListStartMark = m_SFileMap.GetfTell();

	// OOC 를 위함.
	int nNSOctreeRenderList_RenderIndex = NSOctreeRenderList::g_nRenderIndex;

	DWORD dwMaxCount = 0;
	m_SFileMap >> dwMaxCount;
	for ( DWORD i=0; i<dwMaxCount; i++ )
	{
		hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pEffectList, m_pEffectList_AFTER, m_dwEffectCount, m_dwEffectCount_AFTER, TRUE );
		if ( FAILED(hr) )
			MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );
	}

	// OOC 를 위함.
	NSOctreeRenderList::g_nRenderIndex = nNSOctreeRenderList_RenderIndex;

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );

	// EffectLoadToList 재 로딩.
	{
		DWORD dwLightManEndMark = m_SFileMap.GetfTell();

		m_SFileMap.SetOffSet( dwEffectListStartMark );

		m_dwEffectCount = 0;
		m_dwEffectCount_AFTER = 0;
		SAFE_DELETE ( m_pEffectList );
		SAFE_DELETE ( m_pEffectList_AFTER );

		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pEffectList, m_pEffectList_AFTER, m_dwEffectCount, m_dwEffectCount_AFTER, FALSE );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );
		}

		// Note : Weather 효과가 있는 것을 빼온다.
		SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
		COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );

		m_SFileMap.SetOffSet( dwLightManEndMark );
	}

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// 자체 파일 로드

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// 자체 파일 로드
        {
            // m_pStaticMesh 에 적용을 한다.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	return TRUE;
}

BOOL DxLandMan::LoadFile_VER108 ( const char *szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUseDynamicLoad )
{
	BOOL	hr = FALSE;
	BOOL	bExist = FALSE;

	m_SFileMap >> m_MapID.dwID;
	m_SFileMap.ReadBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

	m_sFILEMARK.LoadSet ( m_SFileMap );

	//	Note	:	로드
	//
	hr = m_DxOctree.LoadFile ( m_SFileMap, pd3dDevice, m_pEffectFrameList, bUseDynamicLoad );
	if ( !hr )	return hr;

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pAniManHead = new DxAnimationMan;
		m_pAniManHead->LoadFile_OLD ( m_SFileMap, pd3dDevice );
	}

	COLLISION::MakeAABBTree ( m_pAniManTree, m_pAniManHead );

	m_SFileMap >> bExist;
	while ( bExist )
	{
		float	fCurTime;
		m_SFileMap >> fCurTime;		
		
		char*	szFileName = NULL;
		m_SFileMap >> bExist;
		if ( bExist )
		{
            int	StrLength = 0;
			m_SFileMap >> StrLength;
			szFileName = new char [ StrLength ];
			m_SFileMap.ReadBuffer ( szFileName, sizeof ( char ) * StrLength );
		}

		char*	szFrameName = NULL;
        m_SFileMap >> bExist;
		if ( bExist )
		{
			m_SFileMap >> bExist;
			if ( bExist )
			{
				int StrLength = 0;
				m_SFileMap >> StrLength;
				szFrameName = new char [ StrLength ];
				m_SFileMap.ReadBuffer ( szFrameName, sizeof ( char ) * StrLength );
			}
		}

		D3DXMATRIX		matComb;
		m_SFileMap.ReadBuffer ( &matComb, sizeof ( D3DXMATRIX ) );

		//	Note : 치환 조각 가져오기.
		//
		DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece(szFileName);
		if (!pPiece)
            return FALSE;

		DXPIECEOBJ *pNewPieceObj = new DXPIECEOBJ;
		pNewPieceObj->SetPiece ( pPiece );
		pNewPieceObj->matComb = matComb;

		if ( pPiece->bRendAni )
		{
		//	srand(GetTickCount());
			pNewPieceObj->fCurTime = fCurTime;
		}

		AddPieceObj ( pNewPieceObj );

		SAFE_DELETE_ARRAY ( szFileName );
		SAFE_DELETE_ARRAY ( szFrameName );

		m_SFileMap >> bExist;
	}

	// EffectLoadToList Dummy
	DWORD dwEffectListStartMark = m_SFileMap.GetfTell();

	// OOC 를 위함.
	int nNSOctreeRenderList_RenderIndex = NSOctreeRenderList::g_nRenderIndex;

	DWORD dwMaxCount = 0;
	m_SFileMap >> dwMaxCount;
	for ( DWORD i=0; i<dwMaxCount; i++ )
	{
		hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pEffectList, m_pEffectList_AFTER, m_dwEffectCount, m_dwEffectCount_AFTER, TRUE );
		if ( FAILED(hr) )
			MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );
	}

	// OOC 를 위함.
	NSOctreeRenderList::g_nRenderIndex = nNSOctreeRenderList_RenderIndex;

	//	Note : Light 설정.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );

	// EffectLoadToList 재 로딩.
	{
		DWORD dwLightManEndMark = m_SFileMap.GetfTell();

		m_SFileMap.SetOffSet( dwEffectListStartMark );
		
		m_dwEffectCount = 0;
		m_dwEffectCount_AFTER = 0;
		SAFE_DELETE ( m_pEffectList );
		SAFE_DELETE ( m_pEffectList_AFTER );

		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pEffectList, m_pEffectList_AFTER, m_dwEffectCount, m_dwEffectCount_AFTER, FALSE );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );
		}

		// Note : Weather 효과가 있는 것을 빼온다.
		SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
		COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );

		m_SFileMap.SetOffSet( dwLightManEndMark );
	}

	// Note : 조각파일의 Tree를 Light Load 후 한다.
	DxReplaceContainer::MakeAABBTree( m_pPieceObjTree, m_pPieceObjHead );

	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );

	DxSoundMan::GetInstance().LoadSet ( m_SFileMap );

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_pStaticSoundMan = new CStaticSoundMan;
		m_pStaticSoundMan->LoadSet ( m_SFileMap );
	}	

	m_SFileMap >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDEFF pLandEff = new DXLANDEFF;
			pLandEff->Load ( m_SFileMap );
			AddLandEff ( pLandEff );
			
			m_SFileMap >> bExist;
		}
	
		BuildSingleEffTree ();
	}

	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_CameraAniMan.Load ( m_SFileMap );
	}


	//	Note : Land 에 삽입된 SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_LandGateMan.Load ( m_SFileMap );
	}

	//	Note : m_WeatherMan
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_WeatherMan.LoadSet ( m_SFileMap, pd3dDevice );
	}

	//	Note : Bgm Sound
	//
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_BGMDATA.LoadSet ( m_SFileMap );
	}

	//	Note : Collision Map Skip ( By-Pass )
	//
	m_SFileMap >> bExist;
	DWORD dwBlockSize = m_SFileMap.ReadBlockSize();
	//m_CollisionMap.LoadFile ( m_SFileMap );
	m_SFileMap.SetOffSet ( m_SFileMap.GetfTell()+dwBlockSize );

	//	DxFogMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_FOG_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	//	DxSkyMan
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_SKY_PROPERTY.LoadSet ( m_SFileMap );
	}
	else return TRUE;

	return TRUE;
}

DWORD DxLandMan::EffectLoadToList_Dummy( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile )
{
	// OOC 를 위함.
	int nNSOctreeRenderList_RenderIndex = NSOctreeRenderList::g_nRenderIndex;

	DWORD dwEffectListStartMark = m_SFileMap.GetfTell();
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			HRESULT hr = EffectLoadToList ( pd3dDevice, SFile, m_pEffectList, TRUE );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}
	}

	// OOC 를 위함.
	NSOctreeRenderList::g_nRenderIndex = nNSOctreeRenderList_RenderIndex;

	return dwEffectListStartMark;
}

void DxLandMan::EffectLoadToList_ModifyTileColor( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile, DWORD dwEffectLoadToListStartMark )
{
	DWORD dwLightManEndMark = m_SFileMap.GetfTell();

	// Replace
	{
		m_SFileMap.SetOffSet( dwEffectLoadToListStartMark );
		m_dwEffectCount = 0;
		SAFE_DELETE( m_pEffectList );

		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			HRESULT hr = EffectLoadToList ( pd3dDevice, SFile, m_pEffectList, FALSE );
			if ( FAILED(hr) )
				MessageBox ( NULL, "이팩트를 로드중에 오류가 발생", "ERROR", MB_OK );

			m_dwEffectCount++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );

		m_SFileMap.SetOffSet( dwLightManEndMark );
	}
}

BOOL DxLandMan::LoadBasicPos( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 &vPos )
{
	float fDis = DxFogMan::GetInstance().GetFogRangeFar();
	D3DXVECTOR3 vMax = vPos + D3DXVECTOR3( fDis, fDis, fDis );
	D3DXVECTOR3 vMin = vPos - D3DXVECTOR3( fDis, fDis, fDis );
	//if ( m_pPieceManager )
	{
		m_spPieceManager->LoadBasicAABB( pd3dDevice, vMax, vMin );
	}

	if( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->BaseLoad( pd3dDevice, vPos );
		return TRUE;
	}
	else 
	if( m_spStaticMeshLightMap.get() )
	{
		m_spStaticMeshLightMap->BaseLoad( pd3dDevice, vPos );
		return TRUE;
	}

	return m_DxOctree.LoadBasicPos ( m_SFileMap, pd3dDevice, m_pEffectFrameList, vMax, vMin );
}

BOOL DxLandMan::LoadBasicPos( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin )
{
	//if ( m_pPieceManager )
	{
		m_spPieceManager->LoadBasicAABB( pd3dDevice, vMax, vMin );
	}

	if( m_spStaticMaterialMeshLightMap.get() )
	{
		m_spStaticMaterialMeshLightMap->BaseLoad( pd3dDevice, vMax, vMin );
		return TRUE;
	}
	else if( m_spStaticMeshLightMap.get() )
	{
		m_spStaticMeshLightMap->BaseLoad( pd3dDevice, vMax, vMin );
		return TRUE;
	}
	return m_DxOctree.LoadBasicPos ( m_SFileMap, pd3dDevice, m_pEffectFrameList, vMax, vMin );
}

void DxLandMan::SwitchWeatherEffect( PDXEFFECTBASE& pSrc, DWORD& dwSrc, PDXEFFECTBASE& pDest, DWORD& dwDest )
{
	dwDest = 0;
	SAFE_DELETE( pDest );

	DxEffectBase*	pPrev = NULL;
	DxEffectBase*	pChange = NULL;
	DxEffectBase*	pCurSRC = pSrc;
	while( pCurSRC )
	{
		if( pCurSRC->GetTypeID()==DEF_EFFECT_RAINPOINT )
		{
			pChange = pCurSRC;				// 움직이기 위한 포인트 셋팅.
			pCurSRC = pCurSRC->pEffectNext;	// 다음것을 돌리기 위한 셋팅.
			--dwSrc;						// 소스에 것은 하나 빠진다.

			pChange->pEffectNext = pDest;	// pDest에 값을 삽입.
			pDest = pChange;				// pDest에 값을 삽입.
			++dwDest;						// 목적에 있는 것은 하나 더한다.

			if( pPrev )	pPrev->pEffectNext = pCurSRC;	// 빠진 것을 이어주기 위한 셋팅.
			else		pSrc = pCurSRC;					// Src 가 가르키는 값이 변하였다.

			// 뛰어 넘는다.
			continue;
		}

		pPrev = pCurSRC;
		pCurSRC = pCurSRC->pEffectNext;
	}
}

void DxLandMan::LoadNaviBypassFile ( const char *szFile )
{
	m_sNaviMeshCapsule.LoadNaviBypassFile( szFile );
}

void DxLandMan::ClearNaviBypassFile()
{
	m_sNaviMeshCapsule.ClearNaviBypassFile();
}

void DxLandMan::NavigationMeshCapsule::SaveForNaviMeshInWld( sc::SerialFile& SFile )
{
	SFile << static_cast<BOOL>( m_spNaviMeshInWld.get() ? TRUE : FALSE );
	if ( m_spNaviMeshInWld.get() )
	{
		m_spNaviMeshInWld->SaveFile ( SFile );
	}
}

void DxLandMan::NavigationMeshCapsule::LoadForNaviMeshInWld( sc::SerialFile& SFile )
{
	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		m_spNaviMeshInWld = boost::shared_ptr<NavigationMesh>(new NavigationMesh);
		m_spNaviMeshInWld->LoadFile ( SFile );
	}
}

void DxLandMan::NavigationMeshCapsule::LoadNaviBypassFile( const char *szFile )
{
	m_spNaviMeshBypass = boost::shared_ptr<NavigationMesh>(new NavigationMesh);
	m_spNaviMeshBypass->LoadFile ( szFile );
}

void DxLandMan::NavigationMeshCapsule::ClearNaviBypassFile()
{
	m_spNaviMeshBypass.reset();
}