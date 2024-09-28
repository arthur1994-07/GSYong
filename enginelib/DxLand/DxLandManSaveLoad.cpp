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
//	LAND.MAN ���� ���� ��ȭ.
//	ver.113 - Weather�� �������� �Ѹ��� ���� ����
//	ver.112 - DxPieceManager �߰�
//	ver.111 - Replace_Single After �߰�
//	ver.110 - Replace_Single After �߰�
//	ver.109 - Octree ����ȭ�� ������ ����.
//	ver.108 - Fog �߰��� ��ȭ. Sky �߰��� ��ȭ.
//	ver.107 - ���� file mark ��ġ �߰��� ���� ��ȭ.
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
		//	Note : �� ID ����.
		SFile << m_MapID.dwID;
		SFile.WriteBuffer ( m_szMapName, sizeof(char)*MAXLANDNAME );

		//	Note : ������ �Ϻ� �����͸� ���� �����ϱ� ���� ���� ��ũ ��ġ ����.
		//
		long nFILEMARK = SFile.GetfTell ();
		m_sFILEMARK.SaveSet ( SFile );

		//	Note	:	���̺�
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


		//	Note : Land �� ���Ե� SkinObject
		//
		m_spLandSkinManager->SaveFile( SFile );
		

		//	Note : StaticMesh
		SFile << BOOL ( TRUE );
		if ( m_spStaticMeshLightMap.get() )
        {
            // ���� ��ü ����
            {
                m_spStaticMeshLightMap->Save( szFile, pArchive );
            }

            // TextureEffect �� ���� ��ü ����.
            {
                DxTextureEffNewFile sTexEffNewFile;
                m_spStaticMeshLightMap->GetTexEff( pd3dDevice, sTexEffNewFile );
                sTexEffNewFile.Save( szFile, pArchive );
            }
        }

		SFile << BOOL ( TRUE );
		if ( m_spStaticMaterialMeshLightMap.get() )
		{
			// ���� ��ü ����
			m_spStaticMaterialMeshLightMap->Save( szFile, pArchive );
		}

        // Note : LightMan 
        m_pLightMan->Save ( SFile );

		// Note : DxPieceManager
		SFile << BOOL ( TRUE );
		{
			// CellSize �� ���ϴ� ���
			// Fov �� ���� �þ߰Ÿ��� �������� ������ ���Ѵ�.
			// �ű⼭ 50% ���� ������ �����Ѵ�.
			// �ű⼭ 10% ���� ������ �ø���.
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
		//	m_CollisionMap.SaveFile ( SFile );		// ������ �ʴ´�.
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

		//	Note : ������� �ð� ����.
		SFile << BOOL ( TRUE );
		{
			CTime sTime = CTime::GetCurrentTime();
			CString strTime;
			strTime.Format ( "%d_%02d_%02d_%02d_%02d_%02d", sTime.GetYear(), sTime.GetMonth(), sTime.GetDay(),
				sTime.GetHour(), sTime.GetMinute(), sTime.GetSecond() );
			m_strMakeTimeWLD = strTime.GetString();
			SFile << m_strMakeTimeWLD;
		}

		//	Note : RAD ����
		SFile << BOOL ( TRUE );
		SFile << m_dwRAD_Pass;
		SFile << m_fRAD_Reflectance;
		SFile << m_fRAD_FogBlend;

		//	Note : RAD ����
		SFile << BOOL ( TRUE );
		SFile << m_dwRAD_FogLength;

		//	Note : NSShadowLight ����
		SFile << BOOL ( TRUE );
		m_scpShadowLightData->Save( SFile );

		//	Note : ������ �߰��� �����͸� ���ؼ� ��ŷ.
		//
		SFile << BOOL ( FALSE );

		//	Note : ���� Data�� �������� ��ġ�� ���� ��� �κп� ����.
		//		( ���� ) ��� ������ ������ ������ ���� �������� ���Ѵ�.
		//
		SFile.SetOffSet ( nFILEMARK );
		m_sFILEMARK.SaveSet ( SFile );
	}

	return hr;
}
//
////	Note : ġȯ �������� �����ϱ� ���ؼ� ����ϴ� Save �Լ�.
////		ġȯ ������ ���� + ����ȿ�� + ���ϸ��̼� ���� �̷������.
////
//BOOL DxLandMan::SavePieceFile ( const char *szFile )
//{
//	sc::SerialFile SFile;
//	BOOL	hr = FALSE;
//
//	if ( !m_pStaticMesh && !m_pAniManHead )
//	{
//		MessageBox ( NULL, "������ ���� ���� �ʽ��ϴ�.\nExport �Ŀ� �����ϼ���.", "ġȯ���� ���� - ����", MB_OK );
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
//		// ������ ��Ʈ�� �����̾��µ� Version Up ���� �� ���δ�.
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
//		//	Note : ���� ���̺� �� 
//		//
//		SFile << BOOL ( FALSE );
//	}
//	//sc::SerialFile SFile;
//	//BOOL	hr = FALSE;
//
//	//if ( m_DxOctree.IsSubDivision() )
//	//{
//	//	MessageBox ( NULL, "Export �ÿ� ���̽� ���� ������ �����ϰ� ���������ϴ�.\n"
//	//		"���� ������ �� ũ�� �����Ͽ� �ϳ��� ���ҵǰ� �Ͻʽÿ�.", "ġȯ���� ���� - ����", MB_OK );
//	//	return FALSE;
//	//}
//	//
//	//if ( !m_DxOctree.GetDxFrameHead() && !m_pAniManHead )
//	//{
//	//	MessageBox ( NULL, "������ ���� ���� �ʽ��ϴ�.\nExport �Ŀ� �����ϼ���.", "ġȯ���� ���� - ����", MB_OK );
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
//	//	//	Note : ���� ���̺� �� 
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

	//	Note : Property, Affine  ���� ����.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

	//	Note : LoadBuffer(), Ư�� Effect ������ ���Ǵ� �޸� ���۸� �д´�.
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
		//	Note : LoadBuffer(), Ư�� Effect ������ ���Ǵ� �޸� ���۸� �д´�.
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

	// Note : �� ������	< List ������ �����ϱ� ���ؼ� >
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

	//	Note : Property, Affine  ���� ����.
	//
	if ( pProperty )	pEffect->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffect->IsUseAffineMatrix() )	pEffect->SetAffineValue ( pAffineParts );

	//	Note : LoadBuffer(), Ư�� Effect ������ ���Ǵ� �޸� ���۸� �д´�.
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
		//	Note : LoadBuffer(), Ư�� Effect ������ ���Ǵ� �޸� ���۸� �д´�.
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

		// Note : �� ������	< List ������ �����ϱ� ���ؼ� >
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
	//	Note : �� ����Ÿ ��� ����.
	CleanUp();

	m_pd3dDevice = pd3dDevice;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// Note : ���� ���� �������� �ٲ�
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
		//	Note : ���� ���� ����Ȯ��.
		//
		m_SFileMap.GetFileType( szFileType, FILETYPESIZE, dwVersion );

		if ( strcmp(FILEMARK,szFileType) )
		{
			CString Str;
			Str.Format ( "[%s]  �� ���� ������ �ƴմϴ�.", szFile );
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

        // ������ �ִ� ������ �����.
        DxPieceContainer::GetInstance().CleanUp();

		// 
		NSOctreeRenderList::g_nRenderIndex = 0;
		NSOcclusionCulling::Load( szPathName );				// ��ü ���� �ε�
		NSCubeMapCulling::Load( pd3dDevice, szPathName );	// ��ü ���� �ε�

		// ZoomOut ����Ʈ ��
		DxViewPort::GetInstance().SetZoomOutDefaultGameSetting();

        //---------------------------------------------------//
        NSLoadingDirect::FrameMoveRender( FALSE );
        //---------------------------------------------------//

		m_strNameWLD = szFile;

		if ( dwVersion >= 0x0120 )
		{
			// LightMapAmbientColor�� ���� 120 �������� ��� �����ϴ�.
			NSLightMapAmbientColor::g_bActiveLightMapAmbientColor = TRUE;
		}
		else
		{
			NSLightMapAmbientColor::g_bActiveLightMapAmbientColor = FALSE;
		}

		if ( dwVersion >= 0x0121 )
		{
			// MaterialSystem ps.2.0 �� ���� 121 �������� ��� �����ϴ�.
			m_bUsedMaterialSystem = TRUE;
		}
		else
		{
			m_bUsedMaterialSystem = FALSE;
		}

		if ( dwVersion >= 0x0123 )
		{
			// MaterialSystem ps.3.0 �� ���� 123 �������� ��� �����ϴ�.
			m_bUsedMaterialSystemPS_3_0 = TRUE;
		}
		else
		{
			m_bUsedMaterialSystemPS_3_0 = FALSE;
		}

		//////////////////////////////////////////////////////////////////////////
		// [shhan][2014.09.16] �ǳ��� ����Ʈ ������ �ִµ�, �׷��������� �۾��� �ϸ鼭 ���α׷��ʵ� �ߵǴ��� Ȯ���� �ؾ� �Ѵ�.
		//					�׷��� �ӽ÷� ps.3.0 ������ ������ �Ѵ�.
		// [shhan][2015.03.17] �ǳ������� ���۵ǵ��� ������.
		BOOL bForceIndoor(FALSE);
		std::transform( m_strNameWLD.begin(), m_strNameWLD.end(), m_strNameWLD.begin(), tolower );
		if ( m_strNameWLD == _T("pvptournament.wld" ) || 
			m_strNameWLD == _T("arcade_ground01.wld" ) ||
			m_strNameWLD == _T("fs_main01.wld" ) ||
			m_strNameWLD == _T("fs_main02.wld" ) )
			//||
			//m_strNameWLD == _T("log_in.wld" ) )			// log_in.wld �� �׸��ڸ� SoftShadow �ߴ��� �ֻ�ɿ��� �׸��ڰ� �̻��ϰ� ���̴� ������ �־ �̰��� �߰���.
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
		//		���ʿ��� �ε� �� ���� �ٽ� �ѹ� ���ο� ������ �����.
		NSLandSettingLua::Load( pd3dDevice, szPathName );	// ��ü ���� �ε�


		// 
		// Material �� ���Ѵ�.
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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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

	// Ÿ�� ���� ������ �̰������� ������ �ʴ´�. �����ü�� ���� �ʱ� ������.
	// Replace
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList );
			if ( FAILED(hr) )
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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


	//	Note : Land �� ���Ե� SkinObject
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
		m_spStaticMeshLightMap->Load( pd3dDevice, szPathName );	// ��ü ���� �ε�

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
        {
            // m_pStaticMesh �� ������ �Ѵ�.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	// 
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMaterialMeshLightMap = boost::shared_ptr<DxStaticMaterialMeshLightMap>(new DxStaticMaterialMeshLightMap);
		m_spStaticMaterialMeshLightMap->Load( pd3dDevice, szPathName );	// ��ü ���� �ε�
	}
	else return TRUE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

    //	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE, TRUE );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	DxPieceManager	ver.112 ���� �߰� ��
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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

	//	Note : ������� �ð� ����.
	m_SFileMap >> bExist;
	if ( bExist )
	{
		TSTRING strTime;
		m_SFileMap >> m_strMakeTimeWLD;

		// Occlusion Culling Data �� ������������ Ȯ���Ѵ�.
		NSOcclusionCulling::IntegrityCheck( m_strMakeTimeWLD );

		// CubeMap Culling Data �� ������������ Ȯ���Ѵ�.
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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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

	// Ÿ�� ���� ������ �̰������� ������ �ʴ´�. �����ü�� ���� �ʱ� ������.
	// Replace
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			hr = EffectLoadToList ( pd3dDevice, m_SFileMap, m_pEffectList );
			if ( FAILED(hr) )
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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


	//	Note : Land �� ���Ե� SkinObject
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
		m_spStaticMeshLightMap->Load( pd3dDevice, szPathName );	// ��ü ���� �ε�

		DxTextureEffNewFile sTexEffNewFile;
		if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
		{
			// m_pStaticMesh �� ������ �Ѵ�.
			m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
		}
	}
	else return TRUE;

	// 
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMaterialMeshLightMap = boost::shared_ptr<DxStaticMaterialMeshLightMap>(new DxStaticMaterialMeshLightMap);
		m_spStaticMaterialMeshLightMap->Load( pd3dDevice, szPathName );	// ��ü ���� �ε�
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE, TRUE );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxPieceManager	ver.112 ���� �߰� ��
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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

	//	Note : ������� �ð� ����.
	m_SFileMap >> bExist;
	if ( bExist )
	{
		TSTRING strTime;
		m_SFileMap >> m_strMakeTimeWLD;

		// Occlusion Culling Data �� ������������ Ȯ���Ѵ�.
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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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


	//	Note : Land �� ���Ե� SkinObject
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
		m_spStaticMeshLightMap->Load( pd3dDevice, szPathName );	// ��ü ���� �ε�

		DxTextureEffNewFile sTexEffNewFile;
		if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
		{
			// m_pStaticMesh �� ������ �Ѵ�.
			m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
		}
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxPieceManager	ver.112 ���� �߰� ��
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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


	//	Note : Land �� ���Ե� SkinObject
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
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// ��ü ���� �ε�

		DxTextureEffNewFile sTexEffNewFile;
		if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
		{
			// m_pStaticMesh �� ������ �Ѵ�.
			m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
		}
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//	DxPieceManager	ver.112 ���� �߰� ��
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}


	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );


	//	Note : Land �� ���Ե� SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// ��ü ���� �ε�

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
        {
            // m_pStaticMesh �� ������ �Ѵ�.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	//	DxPieceManager	ver.112 ���� �߰� ��
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

    //	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}

		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}


	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );


	//	Note : Land �� ���Ե� SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// ��ü ���� �ε�

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
        {
            // m_pStaticMesh �� ������ �Ѵ�.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;


	//	DxPieceManager	ver.112 ���� �߰� ��
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD_PREV( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}


	m_sNaviMeshCapsule.LoadForNaviMeshInWld( m_SFileMap );


	//	Note : Land �� ���Ե� SkinObject
	//
	m_spLandSkinManager->LoadFile( m_SFileMap, pd3dDevice );


	//	StaticMesh
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spStaticMeshLightMap = boost::shared_ptr<DxStaticMeshLightMap>(new DxStaticMeshLightMap);
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// ��ü ���� �ε�

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
        {
            // m_pStaticMesh �� ������ �Ѵ�.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;


	//	DxPieceManager	ver.112 ���� �߰� ��
	m_SFileMap >> bExist;
	if ( bExist )
	{
		m_spPieceManager->LoadWLD_PREV( pd3dDevice, m_SFileMap );
	}
	else return TRUE;

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

			m_dwCount_Weather++;
		}
	
		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pTree_Weather, m_pList_Weather );
	}

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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


	//	Note : Land �� ���Ե� SkinObject
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
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// ��ü ���� �ε�

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
        {
            // m_pStaticMesh �� ������ �Ѵ�.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	//	DxPieceManager	ver.112 ���� �߰� ��
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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		// Note : Weather ȿ���� �ִ� ���� ���´�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

			m_dwEffectCount_AFTER_1++;
		}
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER_1, m_pEffectList_AFTER_1 );
	}

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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


	//	Note : Land �� ���Ե� SkinObject
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
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// ��ü ���� �ε�

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
        {
            // m_pStaticMesh �� ������ �Ѵ�.
            m_spStaticMeshLightMap->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
        }
	}
	else return TRUE;

	//	DxPieceManager	ver.112 ���� �߰� ��
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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

			m_dwEffectCount_AFTER++;
		}

		// Note : Weather ȿ���� �ִ� ���� ���´�.
		SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
		COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );
	
		//	Note : AABB Tree make
		//
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );
	}

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );
	EffectLoadToList_ModifyTileColor( pd3dDevice, m_SFileMap, dwEffectListStartMark );


	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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


	//	Note : Land �� ���Ե� SkinObject
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
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// ��ü ���� �ε�

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
        {
            // m_pStaticMesh �� ������ �Ѵ�.
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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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

	// OOC �� ����.
	int nNSOctreeRenderList_RenderIndex = NSOctreeRenderList::g_nRenderIndex;

	DWORD dwMaxCount = 0;
	m_SFileMap >> dwMaxCount;
	for ( DWORD i=0; i<dwMaxCount; i++ )
	{
		hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pEffectList, m_pEffectList_AFTER, m_dwEffectCount, m_dwEffectCount_AFTER, TRUE );
		if ( FAILED(hr) )
			MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );
	}

	// OOC �� ����.
	NSOctreeRenderList::g_nRenderIndex = nNSOctreeRenderList_RenderIndex;

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );

	// EffectLoadToList �� �ε�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );
		}

		// Note : Weather ȿ���� �ִ� ���� ���´�.
		SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
		COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );

		m_SFileMap.SetOffSet( dwLightManEndMark );
	}

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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


	//	Note : Land �� ���Ե� SkinObject
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
		m_spStaticMeshLightMap->Load_Prev( pd3dDevice, szPathName );	// ��ü ���� �ε�

        DxTextureEffNewFile sTexEffNewFile;
        if ( sTexEffNewFile.Load( pd3dDevice, szPathName ) )	// ��ü ���� �ε�
        {
            // m_pStaticMesh �� ������ �Ѵ�.
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

	//	Note	:	�ε�
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

		//	Note : ġȯ ���� ��������.
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

	// OOC �� ����.
	int nNSOctreeRenderList_RenderIndex = NSOctreeRenderList::g_nRenderIndex;

	DWORD dwMaxCount = 0;
	m_SFileMap >> dwMaxCount;
	for ( DWORD i=0; i<dwMaxCount; i++ )
	{
		hr = EffectLoadToList( pd3dDevice, m_SFileMap, m_pEffectList, m_pEffectList_AFTER, m_dwEffectCount, m_dwEffectCount_AFTER, TRUE );
		if ( FAILED(hr) )
			MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );
	}

	// OOC �� ����.
	NSOctreeRenderList::g_nRenderIndex = nNSOctreeRenderList_RenderIndex;

	//	Note : Light ����.
	m_pLightMan->Load( m_SFileMap, VERSION, FALSE );

	// EffectLoadToList �� �ε�.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );
		}

		// Note : Weather ȿ���� �ִ� ���� ���´�.
		SwitchWeatherEffect( m_pEffectList_AFTER, m_dwEffectCount_AFTER, m_pList_Weather, m_dwCount_Weather );
		COLLISION::MakeAABBTree( m_pTree_Weather, m_pList_Weather );

		//	Note : AABB Tree make
		COLLISION::MakeAABBTree ( m_pEffectTree, m_pEffectList );
		COLLISION::MakeAABBTree ( m_pEffectTree_AFTER, m_pEffectList_AFTER );

		m_SFileMap.SetOffSet( dwLightManEndMark );
	}

	// Note : ���������� Tree�� Light Load �� �Ѵ�.
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


	//	Note : Land �� ���Ե� SkinObject
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
	// OOC �� ����.
	int nNSOctreeRenderList_RenderIndex = NSOctreeRenderList::g_nRenderIndex;

	DWORD dwEffectListStartMark = m_SFileMap.GetfTell();
	{
		DWORD dwMaxCount = 0;
		m_SFileMap >> dwMaxCount;
		for ( DWORD i=0; i<dwMaxCount; i++ )
		{
			HRESULT hr = EffectLoadToList ( pd3dDevice, SFile, m_pEffectList, TRUE );
			if ( FAILED(hr) )
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

			m_dwEffectCount++;
		}
	}

	// OOC �� ����.
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
				MessageBox ( NULL, "����Ʈ�� �ε��߿� ������ �߻�", "ERROR", MB_OK );

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
			pChange = pCurSRC;				// �����̱� ���� ����Ʈ ����.
			pCurSRC = pCurSRC->pEffectNext;	// �������� ������ ���� ����.
			--dwSrc;						// �ҽ��� ���� �ϳ� ������.

			pChange->pEffectNext = pDest;	// pDest�� ���� ����.
			pDest = pChange;				// pDest�� ���� ����.
			++dwDest;						// ������ �ִ� ���� �ϳ� ���Ѵ�.

			if( pPrev )	pPrev->pEffectNext = pCurSRC;	// ���� ���� �̾��ֱ� ���� ����.
			else		pSrc = pCurSRC;					// Src �� ����Ű�� ���� ���Ͽ���.

			// �پ� �Ѵ´�.
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