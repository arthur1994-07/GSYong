
#include "pch.h"

#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxTools/NsLoadingLagTester.h"
#include "../../DxTools/RENDERPARAM.h"
//#include "./_new/DxSkinMesh9_CPU.h"
#include "./_new/DxSkinMesh9_NORMAL.h"
#include "./_new/DxSkinMesh9_HLSL.h"
#include "./_new/DxSkinMesh9_Material.h"

#include "DxSkinMeshManager.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////

DxSkinMeshManager& DxSkinMeshManager::GetInstance()
{
	static DxSkinMeshManager SM; return SM;
}

DxSkinMeshManager::DxSkinMeshManager()
{
	m_pd3dDevice = NULL;	
}

void DxSkinMeshManager::SetPath(const TSTRING& path)
{ 
	m_path = path; 
	m_sFileTree.CreateTree( path );
}

DxSkinMeshManager::Resource DxSkinMeshManager::LoadDxSkinMesh( const TSTRING& szSkinMesh, const TSTRING& szSkeleton, BOOL bMeshThread, BOOL bTexThread )
{
	SKINMESH_ADD_DATA sAddData;
	sAddData.m_strName = szSkeleton;
	sAddData.m_bTexThread = bTexThread;	// Texture 도 Thread Loading 을 하는지 체크코드
	return LoadData( GetPath() + szSkinMesh, false, sAddData, bMeshThread?true:false );
}

bool DxSkinMeshManager::ReLoadDxSkinMesh( const TSTRING& szSkinMesh, const TSTRING& szSkeleton, Resource& res, DxSkeletonResource& sres )
{
	if (res.IsValid() == false || sres.IsValid() == false)
	{
		return false;
	}

	SKINMESH_ADD_DATA sAddData;
	sAddData.m_strName = szSkeleton;
	sAddData.m_bTexThread = false;
	return ReLoadData(szSkinMesh, false, sAddData, res, sres);
}

bool DxSkinMeshManager::ReLoadData(const TSTRING& szSkinMesh, bool bMustDirectory, const SKINMESH_ADD_DATA& addData, Resource& res, DxSkeletonResource& sres)
{
	UINT nDataSize = 0;
	CTypeSmartPtr spCTypeRawData = CFileSystem::getInstance().getFile( GetPath() + szSkinMesh, nDataSize );

	COMPLTED_DATA_MAP::iterator it = m_completedDataMap.find( szSkinMesh );

	if( m_completedDataMap.end() == it )
	{
		DxSkinMesh9* pSkinMesh = NULL;
		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
			pSkinMesh = new DxSkinMesh9_NORMAL;
		}
		else
		{
			pSkinMesh = new DxSkinMesh9_Material;	
		}

		Resource sNew(pSkinMesh);
		res = sNew;

		res.get()->DataExchange( *LoadDxSkinMesh( szSkinMesh, addData.m_strName, FALSE, addData.m_bTexThread ).get() );
		return true;
		//return false;
	}

	if( spCTypeRawData != NULL )
	{
		SP_RAW_DATA& spRawData = spCTypeRawData.GetSPMemory();		

		SP_QUEUE_DATA spQueueData( new QUEUE_DATA( szSkinMesh, bMustDirectory, addData ) );
		spQueueData->m_rawDataSize = nDataSize;
		spQueueData->m_spRawData = spRawData;

		ResourceQueue<SKINMESH_ADD_DATA>::SP_QUEUE_DATA_VECTOR completedRawDataVec;
		completedRawDataVec.push_back( spQueueData );

		DWORD nWorkTime = ULONG_MAX;
		NAME_N_SP_COMPLETED_DATA_VECTOR completedDataVec; 
		completedDataVec.reserve( completedRawDataVec.size() );
		
		ProcessCompleteData( completedRawDataVec, &completedDataVec, 0, nWorkTime );		

		if (completedDataVec.empty() == false)
		{
			DxSkinMesh9* pSkinMesh = NULL;
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				pSkinMesh = new DxSkinMesh9_NORMAL;
			}
			else
			{
				pSkinMesh = new DxSkinMesh9_Material;	
			}

			Resource sNew(pSkinMesh);
			res = sNew;

			res.get()->DataExchange( *(completedDataVec[0].second.get()) );
			return true;
		}
	}

	return false;
}

void DxSkinMeshManager::ProcessCompleteData(
    SP_QUEUE_DATA_VECTOR& dataVec, 
    NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
	DWORD dwPrevTick, 
	DWORD nWorkTime )
{
	SP_QUEUE_DATA_VECTOR::iterator it = dataVec.begin();
    while( it != dataVec.end() )
    {
		DWORD dwTick( timeGetTime() );

        const SP_QUEUE_DATA& spResData = *it;
        ++it;

		MIN_ASSERT(!spResData->m_fileName.empty());
		MIN_ASSERT(spResData->m_spRawData != NULL);
		MIN_ASSERT(m_pd3dDevice);

		DxSkinMesh9* pSkinMesh = NULL;

		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
			pSkinMesh = new DxSkinMesh9_NORMAL;
		}
		else
		{
			pSkinMesh = new DxSkinMesh9_Material;	
		}

		pSkinMesh->SetFile(spResData->GetFileName().c_str(),spResData->m_addData.m_strName.c_str());

		pSkinMesh->OnCreateSkin_Bone(m_pd3dDevice);

		pSkinMesh->OnCreateSkin_Memory(m_pd3dDevice,spResData->m_spRawData,spResData->m_rawDataSize );

		pSkinMesh->OnCreateSkin_Skin(m_pd3dDevice, spResData->m_addData.m_bTexThread);

		pSkinMesh->OnCreateSkin_Final(m_pd3dDevice,FALSE);

		pCompletedDataVec->push_back( std::make_pair(spResData->GetFileName(),SP_COMPLETED_DATA(pSkinMesh)) );

#ifdef _RELEASED
		if ( NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData < timeGetTime() - dwTick )
		{
			NSLoadingLagTester::g_DxSkinMeshManager_ProcessCompleteData = timeGetTime() - dwTick;
			NSLoadingLagTester::g_strDxSkinMeshManager_ProcessCompleteData = spResData->m_fileName;
		}
#endif

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxSkinMeshManager Load %1% - %2%", spResData->m_fileName, dwTick ) );

		if ( timeGetTime() - dwPrevTick > nWorkTime )
		{
			break;
		}
    }

    if( !dataVec.empty() )
        dataVec.erase( dataVec.begin(), it );
}

BOOL DxSkinMeshManager::ExistFile( const TSTRING& strName )
{
	TSTRING strFullPath = m_sFileTree.FindPathName( strName );
	if ( strFullPath.empty() )
		return FALSE;
	else
		return TRUE;
}

//
static void FindInverseScale( const TCHAR* pSkinMesh )
{
	TSTRING strSkeleton("b_ball.x");
	DxSkinMeshManager::GetInstance().LoadDxSkinMesh( pSkinMesh, strSkeleton, FALSE, FALSE );
}

void DxSkinMeshManager::OnFindInverseScale()
{
	m_sFileTree.SomeWork( FindInverseScale );
}


void DxSkinMeshManager::Archive( boost::function<void (const TCHAR*)> fun )
{
    BOOST_FOREACH(COMPLTED_DATA_MAP::value_type& it, m_completedDataMap)
    {
        fun( (m_path + it.first).c_str() );
    }

    BOOST_FOREACH(COMPLTED_DATA_MAP::value_type& it, m_backupCacheCompletedDataMap)
    {
        fun( (m_path + it.first).c_str() );
    }
}
