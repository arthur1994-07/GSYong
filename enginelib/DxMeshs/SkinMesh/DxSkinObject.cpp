#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "DxSkinObject.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

char DxSkinObject::m_szPath[MAX_PATH] = "";

void DxSkinObject::SetPath ( char* szPath )
{
	StringCchCopy( m_szPath, MAX_PATH, szPath );
}

DxSkinObject::DxSkinObject()
{
	memset( m_szSkinMesh, 0, sizeof(char)*MAX_PATH );
}

DxSkinObject::~DxSkinObject(void)
{
}

DxSkinMesh9* DxSkinObject::CreateSkinMesh ( const char* szFile, const char* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bThread )
{
	//MIN_ASSERT(szFile);
	//MIN_ASSERT(szSkeleton);
	//MIN_ASSERT(pd3dDevice);

	//if( !DxSkinAniControl::LoadSkeletonForEdit ( szSkeleton, pd3dDevice ) )
	//{
	//	return NULL;
	//}

	//StringCchCopy( m_szSkinMesh, MAX_PATH, szFile );
	//m_skinMeshRes = DxSkinMeshManager::GetInstance().LoadDxSkinMesh( m_szSkinMesh, m_strSkeleton, bThread, bThread );
	//return m_skinMeshRes.get();

	return NULL;
}

BOOL DxSkinObject::LoadObject ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread )
{
    // [ Thread Work shhan ]

	//GASSERT(szFile);
	//GASSERT(m_szPath[0]!=NULL);
	//if ( !pd3dDevice )	return FALSE;

	////	Note : 앳 대이터 초기화.
	////
	//DxSkinAniControl::ClearAll ();

	//char szPathName[MAX_PATH] = "";
	//StringCchCopy( szPathName, MAX_PATH, m_szPath );
	//StringCchCat( szPathName, MAX_PATH, szFile );

	//sc::SerialFile SFile;
	//if ( !SFile.OpenFile ( FOT_READ, szPathName ) )		return FALSE;

	//SFile.ReadBuffer ( m_szSkinMesh, sizeof(char)*MAX_PATH );

	//char szFILE[MAX_PATH] = "";
	//SFile.ReadBuffer ( szFILE, sizeof(char)*MAX_PATH );
	//m_strSkeleton = szFILE;

	//m_skeletonRes = DxSkeletonManager::GetInstance().LoadSkeleton( m_strSkeleton );
	//m_pSkinMesh = DxSkinMeshManager::GetInstance().LoadSkinMesh(m_szSkinMesh, m_strSkeleton, pd3dDevice, bThread);
	//if (!m_pSkeleton || !m_pSkinMesh)
 //       return FALSE;

	////	Note :에니메이션 읽기.
	////
	//DWORD dwLenght;
	//char szAniName[128] = "";

	//SFile >> dwLenght;
	//for ( DWORD i=0; i<dwLenght; ++i )
	//{
	//	SFile.ReadBuffer ( szAniName, sizeof(char)*128 );

	//	DxSkinAniControl::LoadAnimation ( szAniName, pd3dDevice );
	//}

	//DxSkinAniControl::ClassifyAnimation ();

	////	Note : 초기 에니메이션 설정.
	////
	//DxSkinAniControl::SELECTANI ( AN_GUARD_N );

	return TRUE;
}

BOOL DxSkinObject::SaveObject ( const char* szFile )
{
    // [ Thread Work shhan ]

	//if ( !m_pSkeleton || !m_pSkinMesh )	return FALSE;

	//char szPathName[MAX_PATH] = "";
	//StringCchCopy( szPathName, MAX_PATH, m_szPath );
	//StringCchCat( szPathName, MAX_PATH, szFile );

	//sc::SerialFile SFile;
	//if ( !SFile.OpenFile ( FOT_WRITE, szPathName ) )		return FALSE;

	//char szSkeleton[MAX_PATH] = "";
	//StringCchCopy( szSkeleton, MAX_PATH, m_strSkeleton.c_str() );

	//SFile.WriteBuffer ( m_szSkinMesh, sizeof(char)*MAX_PATH );
	//SFile.WriteBuffer ( szSkeleton, sizeof(char)*MAX_PATH );

	//DWORD dwLenght;

	//dwLenght = 0;

	//SFile << GETANIAMOUNT();
	//
	//for (size_t n=0; n<m_vecAnim.size(); ++n)
	//{
 //       char szName[ACF_SZNAME] = {0};
 //       StringCchCopy(szName, ACF_SZNAME, m_vecAnim[n]->m_pDxSkinAni->m_strCfgFile.c_str());
	//	SFile.WriteBuffer(szName, sizeof(char)*ACF_SZNAME);
	//}

	return TRUE;
}

void DxSkinObject::ClearAll ()
{
	//memset(m_szSkinMesh, 0, sizeof(char) * (MAX_PATH));	
	//m_skinMeshRes.Clear();

	//DxSkinAniControl::ClearAll ();
}

HRESULT DxSkinObject::FrameMove ( float fElapsedTime )
{
	//DxSkinAniControl::FrameMove ( fElapsedTime );

	return S_OK;
}

HRESULT DxSkinObject::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot )
{
	if ( !m_skinMeshRes.IsValid() )	
        return S_FALSE;

	////	Note : Skin Animation.
	////
	//DxSkinAniControl::Render ( matRot );

	//	Note : SkinMesh 그리기.
	//
	//UINT cTriangles(0);
	
	//	TODO::
	//m_pSkinMesh->DrawMeshClobal ( pd3dDevice, cTriangles );

	return S_OK;
}
