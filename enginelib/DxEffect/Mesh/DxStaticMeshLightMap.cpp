#include "pch.h"

#include <algorithm>

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../Common/StlFunctions.h"
#include "../../DxMeshs/DxDecal.h"
#include "../../Archive/Archive.h"
#include "./DxStaticMesh.h"

#include "DxStaticMeshLightMap.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//						D	x		S	t	a	t	i	c		M	e	s	h		L	i	g	h	t	M	a	p
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMeshLightMap::DxStaticMeshLightMap()
{
	//InitializeCriticalSection(&m_CSLockLoading);
}

DxStaticMeshLightMap::~DxStaticMeshLightMap()
{
	CleanUp();

	//DeleteCriticalSection(&m_CSLockLoading);
}

void DxStaticMeshLightMap::CleanUp()
{
	//// [shhan][2014.12.31] 이곳에서 EndThread를 하지 않으면 GLGaeaClient:: 에서 SAFE_DELETE(m_pLandMClient); 가 호출되면서,
	////						( 이전에는 EndThread 를 직접 다른곳에서 호출하는 구조였다. )
	////
	////						1.DxStaticMesh_THREAD::EndThread(); 호출 기회를 잃어버리고, DxStaticMeshLightMap:: 에서 m_SFile.CloseFile(); 만 호출되는 결과가 생겨,
	////						 Thread 상 SFile 에 접근하면서 문제가 발생한다.
	////
	////						2. DxStaticMeshLightMap 자체가 사라지게 되어 DxStaticMesh_THREAD::LoadThread 상 plistLoadingData의 OcTree 가 없을 가능성도 크다.
	////						
	////						3. DxStaticMeshColor_THREAD::GetInstance().DeleteList(); 기회를 잃어버리고, 
	////						 DxStaticMeshLightMap 자체가 사라지게 되어 DxStaticMeshColor_THREAD::LoadThread 상 plistLoadingData의 OcTree 가 없을 가능성도 크다.
	////
	//EndThread();

	// 파일을 닫는다.
	//m_SFile.CloseFile();

	m_vecStaticMesh.clear();
}

//void DxStaticMeshLightMap::StartThread( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//	//m_sThread.StartThread( pd3dDevice, &m_SFile, &m_listLoadingData, &m_CSLockLoading );
//}
//
//void DxStaticMeshLightMap::EndThread()
//{
//	DxStaticMeshColor_THREAD::GetInstance().DeleteList();	//	관련 List를 지운다.
//
//	//m_sThread.EndThread();
//}

void DxStaticMeshLightMap::Insert( std::tr1::shared_ptr<DxStaticMesh> pStaticMesh )
{
	m_vecStaticMesh.push_back( pStaticMesh );
}

void DxStaticMeshLightMap::FrameMove( float fElapsedTime )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->FrameMove( fElapsedTime );
	}
}

// Thread Loading Render
void DxStaticMeshLightMap::Render_THREAD_( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const CLIPVOLUME &sCV )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->Render_THREAD_( pd3dDevice, emCullingRenderType, sCV );	//, m_listLoadingData, m_CSLockLoading );
	}
}

void DxStaticMeshLightMap::Render_THREAD_Alpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const CLIPVOLUME &sCV )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->Render_THREAD_Alpha( pd3dDevice, emCullingRenderType, sCV );	//, m_listLoadingData, m_CSLockLoading );
	}
}

void DxStaticMeshLightMap::Render_THREAD_SoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const CLIPVOLUME &sCV )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->Render_THREAD_SoftAlpha( pd3dDevice, emCullingRenderType, sCV );	//, m_listLoadingData, m_CSLockLoading );
	}
}

void DxStaticMeshLightMap::Render_CharShadow( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME* pCV )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->Render_CharShadow( pd3dDevice, pCV );
	}
}

void DxStaticMeshLightMap::Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->Render_Query( pd3dDevice, sCV );
	}
}

VOID DxStaticMeshLightMap::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->CreateQuery( pd3dDevice );
	}
}

VOID DxStaticMeshLightMap::DestroyQuery()
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->DestroyQuery();
	}
}

void DxStaticMeshLightMap::ResetOcclusionQuery()
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->ResetOcclusionQuery();
	}
}

void DxStaticMeshLightMap::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->FindOcclusionRenderIndex( pd3dDevice, sCV, vecOctreeIndex );
	}
}

BOOL DxStaticMeshLightMap::IsCollisionLine( const D3DXVECTOR3& vStart, 
											 D3DXVECTOR3& vEnd, 
											 D3DXVECTOR3 &vColl,
											 D3DXVECTOR3 &vNor, 
											 LPCSTR& szName, 
											 EMCOLLMODE emCullMode,
											 BOOL bOnlyCamColl ) const
{
	BOOL bColl(FALSE);
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		if ( m_vecStaticMesh[i]->IsCollisionLine( vStart, 
													vEnd, 
													vColl,
													vNor, 
													szName, 
													emCullMode,
													bOnlyCamColl ) )
		{
			bColl = TRUE;
		}
	}
	return bColl;
}

BOOL DxStaticMeshLightMap::IsCollisionLine( const D3DXVECTOR3& vStart, 
											   D3DXVECTOR3& vEnd, 
											   D3DXVECTOR3 &vColl,
											   D3DXVECTOR3 &vNor,  
											   EMCOLLMODE emCullMode,
											   BOOL bOnlyCamColl ) const
{
	BOOL bColl(FALSE);
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		if ( m_vecStaticMesh[i]->IsCollisionLine( vStart, 
													vEnd, 
													vColl,
													vNor, 
													emCullMode,
													bOnlyCamColl ) )
		{
			bColl = TRUE;
		}
	}
	return bColl;
}

BOOL DxStaticMeshLightMap::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		if ( m_vecStaticMesh[i]->IsCollisionLineSimple( vStart, vEnd, emCullMode ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

// Note : 충돌이 되면 바로 리턴한다.
BOOL DxStaticMeshLightMap::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
	BOOL bColl(FALSE);
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		if ( m_vecStaticMesh[i]->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
		{
			bColl = TRUE;
		}
	}
	return bColl;
}

void DxStaticMeshLightMap::GetCollisionLineInTriListExceptY( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
	}
}

BOOL DxStaticMeshLightMap::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor )
{
	BOOL bExist(FALSE);
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		if ( m_vecStaticMesh[i]->GetLightMapColor( pd3dDevice, vStart, vEnd, dwColor ) )
		{
			bExist = TRUE;
		}
	}

	return bExist;
}

BOOL DxStaticMeshLightMap::IsLightMap()
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		if ( m_vecStaticMesh[i]->IsLightMap() )
		{
			return TRUE;
		}
	}
	return FALSE;
}

void DxStaticMeshLightMap::ClipStaticMesh( DxDecal* pDecal )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		pDecal->ClipStaticMesh( m_vecStaticMesh[i].get() );
	}
}

void DxStaticMeshLightMap::LoadLightMapTexture( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->LoadLightMapTexture( pd3dDevice );
	}
}

void DxStaticMeshLightMap::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->ExportXFile( pxofsave, pDataObjectRoot, strPieceName, matWorld );
	}
}

void DxStaticMeshLightMap::GetTexEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffNewFile& sTexEffNewFile )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->GetTexEff( pd3dDevice, sTexEffNewFile );
	}
}

void DxStaticMeshLightMap::SetTextureEffNewFile( LPDIRECT3DDEVICEQ pd3dDevice, const DxTextureEffNewFile& sTexEffNewFile )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->SetTextureEffNewFile( pd3dDevice, sTexEffNewFile );
	}
}

void DxStaticMeshLightMap::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		//m_vecStaticMesh[i]->BaseLoad( pd3dDevice, vPos, &m_SFile );
		m_vecStaticMesh[i]->BaseLoad( pd3dDevice, vPos, NULL );
	}
}

void DxStaticMeshLightMap::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		//m_vecStaticMesh[i]->BaseLoad( pd3dDevice, vMax, vMin, &m_SFile );
		m_vecStaticMesh[i]->BaseLoad( pd3dDevice, vMax, vMin, NULL );
	}
}

void DxStaticMeshLightMap::Save ( const char* szName, boost::function<void (const TCHAR*)> pArchive )
{
	std::string strName = szName;

	std::string::size_type idx;
	idx = strName.find( ".wld" );
	if( idx!=std::string::npos )	
		strName += '0';		// wld0 로 만듬.

    if (pArchive)
    {
        SPSerialFile spFile;

        if (Archive::CreateSerialFile(pArchive, spFile) == false)
        {
            return;
        }

        sc::SerialFile& SFile = *spFile;

        if ( SFile.OpenFile ( FOT_WRITE, strName.c_str() ) )	
            Save( SFile );

        SFile.CloseFile();
    }
    else
    {
		sc::SerialFile		SFile;
	    if ( SFile.OpenFile ( FOT_WRITE, strName.c_str() ) )	
		    Save( SFile );

    	SFile.CloseFile();
    }
}

void DxStaticMeshLightMap::Load ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	CleanUp();	// 모두 초기화

	std::string strName = szName;

	std::string::size_type idx;
	idx = strName.find( ".wld" );
	if( idx!=std::string::npos )
		strName += '0';		// wld0 로 만듬.

	sc::SerialFile		SFile;
	if ( SFile.OpenFile ( FOT_READ, strName.c_str() ) )
		Load( pd3dDevice, SFile );

	SFile.CloseFile();
}

void DxStaticMeshLightMap::Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	CleanUp();	// 모두 초기화

	std::string strName = szName;

	std::string::size_type idx;
	idx = strName.find( ".wld" );
	if( idx!=std::string::npos )
		strName += '0';		// wld0 로 만듬.

	sc::SerialFile		SFile;
	if ( SFile.OpenFile ( FOT_READ, strName.c_str() ) )
		Load_Prev( pd3dDevice, SFile );

	SFile.CloseFile();
}

void DxStaticMeshLightMap::Save ( sc::SerialFile& SFile )
{
	DWORD	dwSize = 0;

	SFile << static_cast<DWORD>( VERSION );

	SFile.BeginBlock();
	{
		DWORD dwSize = m_vecStaticMesh.size();
		SFile << dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			m_vecStaticMesh[i]->Save ( SFile, FALSE );
		}
	}
	SFile.EndBlock();
}

void DxStaticMeshLightMap::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwSize(0);
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		std::tr1::shared_ptr<DxStaticMesh> spStaticMesh = std::tr1::shared_ptr<DxStaticMesh>(new DxStaticMesh);
		spStaticMesh->Load( pd3dDevice, SFile, TRUE, FALSE );

		m_vecStaticMesh.push_back( spStaticMesh );
	}

	//StartThread( pd3dDevice );
}

void DxStaticMeshLightMap::Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	std::tr1::shared_ptr<DxStaticMesh> spStaticMesh = std::tr1::shared_ptr<DxStaticMesh>(new DxStaticMesh);
	spStaticMesh->Load( pd3dDevice, SFile, TRUE, FALSE );

	m_vecStaticMesh.push_back( spStaticMesh );

	//StartThread( pd3dDevice );
}

// 로그인 화면 같은 경우 다중으로 맵이 로딩이 되어 있고, 맵 전환이 되기 때문에 NSLandThread 정보값도 변경이 되야한다.
void DxStaticMeshLightMap::SetNSLandThread()
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->SetNSLandThread();
	}
}