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
	//// [shhan][2014.12.31] �̰����� EndThread�� ���� ������ GLGaeaClient:: ���� SAFE_DELETE(m_pLandMClient); �� ȣ��Ǹ鼭,
	////						( �������� EndThread �� ���� �ٸ������� ȣ���ϴ� ��������. )
	////
	////						1.DxStaticMesh_THREAD::EndThread(); ȣ�� ��ȸ�� �Ҿ������, DxStaticMeshLightMap:: ���� m_SFile.CloseFile(); �� ȣ��Ǵ� ����� ����,
	////						 Thread �� SFile �� �����ϸ鼭 ������ �߻��Ѵ�.
	////
	////						2. DxStaticMeshLightMap ��ü�� ������� �Ǿ� DxStaticMesh_THREAD::LoadThread �� plistLoadingData�� OcTree �� ���� ���ɼ��� ũ��.
	////						
	////						3. DxStaticMeshColor_THREAD::GetInstance().DeleteList(); ��ȸ�� �Ҿ������, 
	////						 DxStaticMeshLightMap ��ü�� ������� �Ǿ� DxStaticMeshColor_THREAD::LoadThread �� plistLoadingData�� OcTree �� ���� ���ɼ��� ũ��.
	////
	//EndThread();

	// ������ �ݴ´�.
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
//	DxStaticMeshColor_THREAD::GetInstance().DeleteList();	//	���� List�� �����.
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

// Note : �浹�� �Ǹ� �ٷ� �����Ѵ�.
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
		strName += '0';		// wld0 �� ����.

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
	CleanUp();	// ��� �ʱ�ȭ

	std::string strName = szName;

	std::string::size_type idx;
	idx = strName.find( ".wld" );
	if( idx!=std::string::npos )
		strName += '0';		// wld0 �� ����.

	sc::SerialFile		SFile;
	if ( SFile.OpenFile ( FOT_READ, strName.c_str() ) )
		Load( pd3dDevice, SFile );

	SFile.CloseFile();
}

void DxStaticMeshLightMap::Load_Prev( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	CleanUp();	// ��� �ʱ�ȭ

	std::string strName = szName;

	std::string::size_type idx;
	idx = strName.find( ".wld" );
	if( idx!=std::string::npos )
		strName += '0';		// wld0 �� ����.

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

// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
void DxStaticMeshLightMap::SetNSLandThread()
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->SetNSLandThread();
	}
}