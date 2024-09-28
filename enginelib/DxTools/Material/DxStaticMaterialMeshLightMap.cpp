#include "pch.h"

#include <algorithm>

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../Common/StlFunctions.h"
#include "../../DxMeshs/DxDecal.h"
#include "../../Archive/Archive.h"
#include "../Occlusion/NSOcclusionCulling.h"
#include "../DxRenderStates.h"
#include "./NSShadowRenderForMaterial.h"
#include "./DxStaticMaterialMesh.h"
#include "./NSMaterialManager.h"

#include "DxStaticMaterialMeshLightMap.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------------------------
//						D	x		S	t	a	t	i	c		M	e	s	h		L	i	g	h	t	M	a	p
// -----------------------------------------------------------------------------------------------------------------------------------------
DxStaticMaterialMeshLightMap::DxStaticMaterialMeshLightMap()
	: m_fRealTimeWaterHeight(0.f)
{
}

DxStaticMaterialMeshLightMap::~DxStaticMaterialMeshLightMap()
{
	CleanUp();

}

void DxStaticMaterialMeshLightMap::CleanUp()
{
	m_vecStaticMesh.clear();
}

void DxStaticMaterialMeshLightMap::Insert_SMMLM( std::tr1::shared_ptr<DxStaticMaterialMesh> pStaticMesh )
{
	m_vecStaticMesh.push_back( pStaticMesh );
}

void DxStaticMaterialMeshLightMap::FrameMove( float fElapsedTime )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->FrameMove( fElapsedTime );
	}
}



// Material �� ���Ѵ�.
void DxStaticMaterialMeshLightMap::Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTool )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->Active_MATERIAL_DATA( pd3dDevice, bTool );
	}
}


void DxStaticMaterialMeshLightMap::RenderOrderSetting( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	// �ٽ� �����ְ� Render �� ������Ѵ�.
	NSMaterialManager::SetFX_CommonParameter_Reset();

	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->RenderOrderSetting( pd3dDevice, sCV );
	}
}

void DxStaticMaterialMeshLightMap::Render( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld )
{
	// Piece Opaque, Hard Alpha �� �����Ȱ͵��� ���ܼ� �ٽ� ���ش�.
	// �ٽ� �����ְ� Render �� ������Ѵ�.
	// [shhan][2014.04.24] �߰��� LBuffer �� ������ ���� Piece �� ���ð��� �־ ������ �������.
	//						�׷��� �߰���.
	NSMaterialManager::SetFX_CommonParameter_Reset();

	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->Render( pd3dDevice, emCullingRenderType, bMPShading, matWorld );
	}
}

void DxStaticMaterialMeshLightMap::RenderHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->RenderHardAlpha( pd3dDevice, emCullingRenderType, bMPShading, matWorld );
	}
}

void DxStaticMaterialMeshLightMap::RenderDecal( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld )
{
	// [shhan][2013.07.16] Field�� Decal ���� ���� OcculsionCullgin �� ����� �ȵȴ�.
	//						�׷��� NSOcclusionCulling::g_bRendering �� FALSE �� �༭, 
	//						LightMapRenderMat::RenderMaterial �� NSOcclusionOctree::Visible( m_nID ) ���� ������ ���� �ʵ��� �Ѵ�.
	BOOL bOcclusionCullingRendering = NSOcclusionCulling::g_bRendering;
	NSOcclusionCulling::g_bRendering = FALSE;
	{
		for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
		{
			m_vecStaticMesh[i]->RenderDecal( pd3dDevice, emCullingRenderType, FALSE, matWorld );
		}
	}
	NSOcclusionCulling::g_bRendering = bOcclusionCullingRendering;
}

void DxStaticMaterialMeshLightMap::RenderSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, BOOL bMPShading, const D3DXMATRIX& matWorld )
{
	// Piece Opaque, Hard Alpha �� �����Ȱ͵��� ���ܼ� �ٽ� ���ش�.
	// �ٽ� �����ְ� Render �� ������Ѵ�.
	NSMaterialManager::SetFX_CommonParameter_Reset();

	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->RenderSoftAlpha( pd3dDevice, emCullingRenderType, bMPShading, matWorld );
	}
}

void DxStaticMaterialMeshLightMap::RenderSoftAlpha_Deffered( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld )
{
	// Piece Opaque, Hard Alpha �� �����Ȱ͵��� ���ܼ� �ٽ� ���ش�.
	// �ٽ� �����ְ� Render �� ������Ѵ�.
	NSMaterialManager::SetFX_CommonParameter_Reset();

	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->RenderSoftAlpha_Deffered( pd3dDevice, emCullingRenderType, matWorld );
	}
}

void DxStaticMaterialMeshLightMap::RenderDecalPass2( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType, const D3DXMATRIX& matWorld )
{
	// [shhan][2013.07.16] Field�� Decal ���� ���� OcculsionCullgin �� ����� �ȵȴ�.
	//						�׷��� NSOcclusionCulling::g_bRendering �� FALSE �� �༭, 
	//						LightMapRenderMat::RenderMaterial �� NSOcclusionOctree::Visible( m_nID ) ���� ������ ���� �ʵ��� �Ѵ�.
	BOOL bOcclusionCullingRendering = NSOcclusionCulling::g_bRendering;
	NSOcclusionCulling::g_bRendering = FALSE;
	{
		for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
		{
			m_vecStaticMesh[i]->RenderDecalPass2( pd3dDevice, emCullingRenderType, matWorld );
		}
	}
	NSOcclusionCulling::g_bRendering = bOcclusionCullingRendering;
}

void DxStaticMaterialMeshLightMap::RenderWater_CustomSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	{
		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

		DWORD dwFogEnable(0L);
		pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

		for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
		{
			// unlit, unfog
			m_vecStaticMesh[i]->RenderCustomMaterialUNLIT_UNFOG( pd3dDevice, emCullingRenderType );
		}

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

		float fBias = 0.0000f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	}

	{
		DxRenderStates::GetInstance().SetOnSoftAlphaMap ( pd3dDevice );

		// [shhan][2013.07.16] LightMapRenderMat::RenderMaterialWater ���� ����
		//						NSOcclusionOctree::Visible( m_nID ) �� ������ �ʾƼ�
		//						NSOcclusionCulling::g_bRendering = FALSE; �� ���� �ʿ䰡 ����.
		float fPrevHeight(m_fRealTimeWaterHeight);
		float fNearLength(FLT_MAX);
		m_fRealTimeWaterHeight = FLT_MAX;
		for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
		{
			// �ǽð� �ݻ簡 ������ ���� �ִٸ� m_fRealTimeWaterHeight �� FLT_MAX �� �ƴ� ���� �Ѿ�´�.
			// m_fRealTimeWaterHeight �� �������� �ǽð� �ݻ縦 ������ ���� �����ϵ��� ����.
			m_vecStaticMesh[i]->RenderCustomMaterialUNLIT_SoftAlpha( pd3dDevice, emCullingRenderType, fPrevHeight, m_fRealTimeWaterHeight, fNearLength );
		}

		float fBias = 0.0000f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap ( pd3dDevice );
	}
}

void DxStaticMaterialMeshLightMap::RenderCascadeShadow( const LPDIRECT3DDEVICEQ pd3dDevice, NSLandThread::EM_FIELD_CULLING_RENDERING_TYPE emCullingRenderType )
{
	// Opaque
	{
		pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_ONLYPOS );

		if ( NSShadowRenderForMaterial::FX_BeginPass( 0 ) )
		{
			NSShadowRenderForMaterial::CommitChanges();

			for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
			{
				m_vecStaticMesh[i]->RenderCascadeShadowOpaque( pd3dDevice, emCullingRenderType );
			}

			NSShadowRenderForMaterial::FX_EndPass();
		}
	}


	// HardAlpha
	{
		pd3dDevice->SetVertexDeclaration( NSShadowRenderForMaterial::g_pDCRT_POSTEX );

		DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

		if ( NSShadowRenderForMaterial::FX_BeginPass( 1 ) )
		{
			NSShadowRenderForMaterial::CommitChanges();

			for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
			{
				m_vecStaticMesh[i]->RenderCascadeShadowHardAlpha( pd3dDevice, emCullingRenderType );
			}

			NSShadowRenderForMaterial::FX_EndPass();
		}

		DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
	}	
}

void DxStaticMaterialMeshLightMap::Render_CharShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->Render_CharShadow( pd3dDevice, &sCV );
	}
}

void DxStaticMaterialMeshLightMap::Render_Query( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->Render_Query( pd3dDevice, &sCV );
	}
}

VOID DxStaticMaterialMeshLightMap::CreateQuery( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->CreateQuery( pd3dDevice );
	}
}

VOID DxStaticMaterialMeshLightMap::DestroyQuery()
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->DestroyQuery();
	}
}

void DxStaticMaterialMeshLightMap::ResetOcclusionQuery()
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->ResetOcclusionQuery();
	}
}

void DxStaticMaterialMeshLightMap::FindOcclusionRenderIndex( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, VEC_DWORD& vecOctreeIndex )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->FindOcclusionRenderIndex( pd3dDevice, sCV, vecOctreeIndex );
	}
}

BOOL DxStaticMaterialMeshLightMap::IsCollisionLine( const D3DXVECTOR3& vStart, 
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

BOOL DxStaticMaterialMeshLightMap::IsCollisionLine( const D3DXVECTOR3& vStart, 
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

BOOL DxStaticMaterialMeshLightMap::IsCollisionLineSimple( const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode ) const
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

void DxStaticMaterialMeshLightMap::GetCollisionLineInTriListExceptY( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, std::vector<COLLISION::TRIANGLE> &TriList )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		GetCollisionLineInTriListExceptY( vStart, vEnd, TriList );
	}
}

// �ǽð� �ݻ� ��ġ�� ���Ѵ�.
// ���� FLT_MAX �� ���� �۵����� �ʴ´�.
float DxStaticMaterialMeshLightMap::GetRealTimeWaterHeight()
{
	return m_fRealTimeWaterHeight;
}

BOOL DxStaticMaterialMeshLightMap::GetLightMapColor( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, DWORD& dwColor )
{
	BOOL bExist(FALSE);
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		if ( m_vecStaticMesh[i]->GetLightMapColorForFieldMesh( pd3dDevice, vStart, vEnd, dwColor ) )
		{
			bExist = TRUE;
		}
	}

	return bExist;
}

void DxStaticMaterialMeshLightMap::ClipStaticMesh( DxDecal* pDecal )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->ClipStaticMesh( pDecal );
	}
}

void DxStaticMaterialMeshLightMap::LoadLightMapTexture( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->LoadLightMapForFieldMesh( pd3dDevice );
	}
}

void DxStaticMaterialMeshLightMap::RefreshLightMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->RefreshLightMap( pd3dDevice );
	}
}

void DxStaticMaterialMeshLightMap::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName, const D3DXMATRIX& matWorld )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->ExportXFile( pxofsave, pDataObjectRoot, strPieceName, matWorld );
	}
}

void DxStaticMaterialMeshLightMap::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->BaseLoad( pd3dDevice, vPos, &m_SFile );
	}
}

void DxStaticMaterialMeshLightMap::BaseLoad( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->BaseLoad( pd3dDevice, vMax, vMin, &m_SFile );
	}
}

void DxStaticMaterialMeshLightMap::Save ( const char* szName, boost::function<void (const TCHAR*)> pArchive )
{
	std::string strName = szName;

	std::string::size_type idx;
	idx = strName.find( ".wld" );
	if( idx!=std::string::npos )	
		strName += '2';		// wld2 �� ����.

    if (pArchive)
    {
        SPSerialFile spFile;

        if (Archive::CreateSerialFile(pArchive, spFile) == false)
        {
            return ;
        }

        sc::SerialFile& SFile = *spFile;

        if ( SFile.OpenFile ( FOT_WRITE, strName.c_str() ) )	
            Save( SFile, pArchive );

        SFile.CloseFile();
    }
    else
    {
	    if ( m_SFile.OpenFile ( FOT_WRITE, strName.c_str() ) )	
		    Save( m_SFile, pArchive );

	    m_SFile.CloseFile();
    }
}

void DxStaticMaterialMeshLightMap::Load ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
{
	std::string strName = szName;

	std::string::size_type idx;
	idx = strName.find( ".wld" );
	if( idx!=std::string::npos )
		strName += '2';		// wld2 �� ����.

	m_SFile.CloseFile();
	if ( m_SFile.OpenFile ( FOT_READ, strName.c_str() ) )
		Load( pd3dDevice, m_SFile );
	//Load( pd3dDevice, m_SFile, TRUE, FALSE );
}

void DxStaticMaterialMeshLightMap::Save ( sc::SerialFile& SFile, boost::function<void (const TCHAR*)> pArchive )
{
	DWORD	dwSize = 0;

	SFile << static_cast<DWORD>( VERSION );

	SFile.BeginBlock();
	{
		dwSize = m_vecStaticMesh.size();
		SFile << dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			m_vecStaticMesh[i]->Save ( SFile, FALSE, pArchive );
		}
	}
	SFile.EndBlock();
}

void DxStaticMaterialMeshLightMap::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	CleanUp();	// ��� �ʱ�ȭ

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwSize(0);
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		std::tr1::shared_ptr<DxStaticMaterialMesh> spStaticMesh = std::tr1::shared_ptr<DxStaticMaterialMesh>(new DxStaticMaterialMesh);
		spStaticMesh->Load( pd3dDevice, m_SFile, TRUE, FALSE, NULL );

		m_vecStaticMesh.push_back( spStaticMesh );
	}
}

// �α��� ȭ�� ���� ��� �������� ���� �ε��� �Ǿ� �ְ�, �� ��ȯ�� �Ǳ� ������ NSLandThread �������� ������ �Ǿ��Ѵ�.
void DxStaticMaterialMeshLightMap::SetNSLandThread()
{
	for ( DWORD i=0; i<m_vecStaticMesh.size(); ++i )
	{
		m_vecStaticMesh[i]->SetNSLandThread();
	}
}