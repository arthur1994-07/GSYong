#include "pch.h"

#include "../../SigmaCore/File/SerialFile.h"

#include "../Common/StlFunctions.h"
#include "./WaterSurfPlane.h"

#include "WaterSurfPlaneGroup.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
//					S	U	R	F	P	L	A	N	G	R	O	U	P					
//---------------------------------------------------------------------------------------------
WaterSurfPlaneGroup::WaterSurfPlaneGroup()
{
}

WaterSurfPlaneGroup::~WaterSurfPlaneGroup()
{
	CleanUp();
}

void WaterSurfPlaneGroup::CleanUp()
{
	std::for_each( m_vecWaterSurfPlane.begin(), m_vecWaterSurfPlane.end(), std_afunc::DeleteObject() );
	m_vecWaterSurfPlane.clear();
}

// nPointNum - Plane 이 그룹지어지는 개수
void WaterSurfPlaneGroup::Create( DxSetLandMan* pSetLandMan, const WATERSURF_PROPERTY& sProp, const VEC_VECTOR3& vecWaterSurfPOS, int nPointNum )
{
	CleanUp();

	int nHalf = nPointNum/2;

	DWORD dwSize = (DWORD) vecWaterSurfPOS.size();
	for( DWORD i=0; i<dwSize; i+=nHalf )
	{
		// Note : 셋팅되어야 할 점을 얻는다.
		VEC_VECTOR3 vecPos;
		for( DWORD j=i; j<i+nPointNum; ++j )
		{
			if( j < dwSize )
			{
				vecPos.push_back( vecWaterSurfPOS[j] );
			}
		}

		// Note : WaterSurfPlane를 생성한다.
		WaterSurfPlane* pNew = new WaterSurfPlane;
		if( pNew->Create( pSetLandMan, sProp, vecPos ) )
		{
			// Note : m_vecWaterSurfPlane에 넣는다.
			m_vecWaterSurfPlane.push_back( pNew );
		}
		else
		{
			SAFE_DELETE( pNew );
		}
	}
}

void WaterSurfPlaneGroup::FrameMove( const WATERSURF_PROPERTY& sProp, float fElapsedTime )
{
	for( DWORD i=0; i<m_vecWaterSurfPlane.size(); ++i )
	{
		m_vecWaterSurfPlane[i]->FrameMove( sProp, fElapsedTime );
	}
}

void WaterSurfPlaneGroup::Render( WATERSURFVERTEX*& pLocked, bool& bFirst )
{
	for( DWORD i=0; i<m_vecWaterSurfPlane.size(); ++i )
	{
		m_vecWaterSurfPlane[i]->Render( pLocked, bFirst );
	}
}

void WaterSurfPlaneGroup::ResetData( const WATERSURF_PROPERTY& sProp )
{
	for( DWORD i=0; i<m_vecWaterSurfPlane.size(); ++i )
	{
		m_vecWaterSurfPlane[i]->ResetData( sProp );
	}
}

void WaterSurfPlaneGroup::AddHeight( float fHeight )
{
	for( DWORD i=0; i<m_vecWaterSurfPlane.size(); ++i )
	{
		m_vecWaterSurfPlane[i]->AddHeight( fHeight );
	}
}

void WaterSurfPlaneGroup::SetObjRotate90( const D3DXMATRIX& matRotate )
{
    for( DWORD i=0; i<m_vecWaterSurfPlane.size(); ++i )
	{
		m_vecWaterSurfPlane[i]->SetObjRotate90( matRotate );
	}
}

void WaterSurfPlaneGroup::CountVertexMax( UINT& nVertexMax )
{
    for( DWORD i=0; i<m_vecWaterSurfPlane.size(); ++i )
	{
		m_vecWaterSurfPlane[i]->CountVertexMax( nVertexMax );
	}
}

void WaterSurfPlaneGroup::CountVertexRender( UINT& nVertexMax )
{
    for( DWORD i=0; i<m_vecWaterSurfPlane.size(); ++i )
	{
		m_vecWaterSurfPlane[i]->CountVertexRender( nVertexMax );
	}
}

void WaterSurfPlaneGroup::Save( sc::SerialFile &SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		DWORD dwSize = (DWORD) m_vecWaterSurfPlane.size();
		SFile << dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			m_vecWaterSurfPlane[i]->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void WaterSurfPlaneGroup::Load( sc::SerialFile &SFile, const WATERSURF_PROPERTY& sProp )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwSize;
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		WaterSurfPlane* pNew = new WaterSurfPlane;
		pNew->Load( SFile, sProp );
		m_vecWaterSurfPlane.push_back( pNew );
	}
}

void WaterSurfPlaneGroup::Import( const WaterSurfPlaneGroup* pSrc, const WATERSURF_PROPERTY& sProp )
{
	CleanUp();

	for( DWORD i=0; i<pSrc->m_vecWaterSurfPlane.size(); ++i )
	{
		WaterSurfPlane* pNew = new WaterSurfPlane;
		pNew->Import( pSrc->m_vecWaterSurfPlane[i], sProp );
		m_vecWaterSurfPlane.push_back( pNew );
	}
}