#include "pch.h"

#include "../../SigmaCore/File/SerialFile.h"

//#include "../HeightField/NSHfMain.h"
#include "../DxTools/TextureManager.h"
#include "../DxTools/DxViewPort.h"
#include "../DxTools/EditMeshs.h"
#include "../DxTools/Collision.h"
#include "../DxLand/DxSetLandMan.h"

//#include "./WaterSurfPlane.h"
#include "./WaterSurfPlaneGroup.h"
#include "./NSWaterMain.h"

#include "./WaterSurfPointMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
//		W	a	t	e	r		S	u	r	f		P	o	i	n	t		M	a	n		
//---------------------------------------------------------------------------------------------
WaterSurfPointMan::WaterSurfPointMan()
    : m_strName( _T( "_Wa_water_Surf.dds" ) )

    , m_nVertexMax( 0 )
{
	m_pWaterSurfPlaneGroup[0] = new WaterSurfPlaneGroup;
	m_pWaterSurfPlaneGroup[1] = new WaterSurfPlaneGroup;
}

WaterSurfPointMan::~WaterSurfPointMan()
{
	CleanUp();

	SAFE_DELETE( m_pWaterSurfPlaneGroup[0] );
	SAFE_DELETE( m_pWaterSurfPlaneGroup[1] );
}

void WaterSurfPointMan::CleanUp()
{
	OnLostDevice();
	OnDestroyDevice();

	m_vecWaterSurfPos[0].clear();
	m_vecWaterSurfPos[1].clear();

    m_nVertexMax = 0;
}

void WaterSurfPointMan::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_strName,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        0,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_ETC,
		false,
		FALSE );
}

void WaterSurfPointMan::OnDestroyDevice()
{
	m_textureRes.Clear();
}

void WaterSurfPointMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if( m_nVertexMax <= 0 )
        return;

    m_sVertexBuffer.dwFVF = WATERSURFVERTEX::FVF;
	m_sVertexBuffer.nVertexSize = D3DXGetFVFVertexSize( m_sVertexBuffer.dwFVF );
	m_sVertexBuffer.nFullByte = m_sVertexBuffer.nVertexSize * m_nVertexMax * 3;
    m_sVertexBuffer.CreateVB( pd3dDevice, FALSE );
}

void WaterSurfPointMan::OnLostDevice()
{
    m_sVertexBuffer.ReleaseVB();
}

void WaterSurfPointMan::Create( DxSetLandMan* pSetLandMan, D3DXVECTOR3 vPos, float fRadius, BOOL bLeft )
{
	CleanUp();

	//CWaterMain* pWaterMain = NSWaterMain::GetWaterMain();
	//float fHeight = pWaterMain->GetHeight();

	//vPos.y = fHeight;

	// Note : Point 넣기
	VEC_VECTOR3	vecWaterSurfPos;
	vecWaterSurfPos.push_back( vPos );

	// Note : 계속적으로 새로운 점을 만든다.
	D3DXVECTOR3 vDir( 1.f, 0.f, 0.f );
	if( bLeft )
	{
		vDir = D3DXVECTOR3( -1.f, 0.f, 0.f );
	}
	D3DXVECTOR3 vBasePos[4];
	Create( pSetLandMan, TRUE, vecWaterSurfPos, &vBasePos[0], vPos, vDir, fRadius );

	// Note : 
	for( size_t i=0; i<vecWaterSurfPos.size(); ++i )
	{
		m_vecWaterSurfPos[0].push_back( vecWaterSurfPos[i] );
	}
	// Note : vecWaterSurfPos 데이터를 반으로 나눈다.
	for( size_t i=0; i<vecWaterSurfPos.size()-1; ++i )
	{
		m_vecWaterSurfPos[1].push_back( (vecWaterSurfPos[i]+vecWaterSurfPos[i+1])*0.5f );
	}

	// Note : Plane을 생성한다.
	PlaneRemake( pSetLandMan );
}

void WaterSurfPointMan::Create( DxSetLandMan* pSetLandMan, BOOL bFirst, VEC_VECTOR3& vecWaterSurfPos, D3DXVECTOR3* pBasePos, const D3DXVECTOR3& vPos, D3DXVECTOR3 vDir, float fRadius )
{
	m_fPlaneWidth	= fRadius;
	m_fPlaneHeight	= fRadius;

	// 4방향으로 검사를 한다.
	struct CHECKDATA
	{
		BOOL bColl;
		D3DXVECTOR3 vPos;
	};
	CHECKDATA sCheckData[3];

	D3DXVECTOR3 vCross(0.f,0.f,0.f);
	D3DXVECTOR3 vUp(0.f,1.f,0.f);
	D3DXVec3Cross( &vCross, &vDir, &vUp );
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXVec3Normalize( &vCross, &vCross );
	vDir *= fRadius;
	vCross *= fRadius;

	D3DXVECTOR3 vNor;
	D3DXVECTOR3 vPos11 = vPos + vCross;
	D3DXVECTOR3 vPos12 = vPos + vCross + vDir;
	D3DXVECTOR3 vPos21 = vPos - vCross;
	D3DXVECTOR3 vPos22 = vPos - vCross + vDir;

	sCheckData[0].vPos = vPos12;
	sCheckData[0].bColl = FALSE;
	//if( NSHfMain::IsCollision( vPos11, sCheckData[0].vPos, TRUE, FALSE ) )
	if ( pSetLandMan->IsCollisionLine_NoneConst( vPos11, sCheckData[0].vPos, vNor, EMCC_CULL_NONE ) )
	{
		sCheckData[0].bColl = TRUE;
	}

	sCheckData[1].vPos = vPos22;
	sCheckData[1].bColl = FALSE;
	//if( NSHfMain::IsCollision( vPos12, sCheckData[1].vPos, TRUE, FALSE ) )
	if ( pSetLandMan->IsCollisionLine_NoneConst( vPos12, sCheckData[1].vPos, vNor, EMCC_CULL_NONE ) )
	{
		sCheckData[1].bColl = TRUE;
	}

	sCheckData[2].vPos = vPos22;
	sCheckData[2].bColl = FALSE;
	//if( NSHfMain::IsCollision( vPos21, sCheckData[2].vPos, TRUE, FALSE ) )
	if ( pSetLandMan->IsCollisionLine_NoneConst( vPos21, sCheckData[2].vPos, vNor, EMCC_CULL_NONE ) )
	{
		sCheckData[2].bColl = TRUE;
	}

	int nCount(0);
	D3DXVECTOR3 vPoint(0.f,0.f,0.f);
	for( DWORD i=0; i<3; ++i )
	{
		if( sCheckData[i].bColl )
		{
			++nCount;
			vPoint += sCheckData[i].vPos;
		}
	}

	if( nCount >= 1 )
	{
		vPoint = vPoint / static_cast<float>(nCount);
		vecWaterSurfPos.push_back( vPoint );

		// Note : 처음 생성시에는 Base Plane만 만들고,
		//		무한루프 검사를 위한 체크는 하지 않는다.
		if( bFirst )
		{
			// Note : Base Plane 만들기.
			D3DXVECTOR3 vCross(0.f,0.f,0.f);
			D3DXVECTOR3 vUp(0.f,1.f,0.f);
			vDir = vPoint - vPos;
			D3DXVec3Cross( &vCross, &vDir, &vUp );
			D3DXVec3Normalize( &vCross, &vCross );
			vUp *= fRadius;
			vCross *= fRadius;

			pBasePos[0] = vPos + vUp - vCross;
			pBasePos[1] = vPos + vUp + vCross;
			pBasePos[2] = vPos - vUp - vCross;
			pBasePos[3] = vPos - vUp + vCross;
		}
		else
		{
			// Note : 무한루프를 막기위한 검사.
			D3DXVECTOR3 vColl(0.f,0.f,0.f);
			size_t dwSize = vecWaterSurfPos.size();
			if( COLLISION::IsLineTriangleCollision(
				&pBasePos[0],
				&pBasePos[1],
				&pBasePos[2],
				&vecWaterSurfPos[ dwSize-2 ],
				&vecWaterSurfPos[ dwSize-1 ],
				&vColl,
				NULL,
				EMCC_CULL_NONE,
				FALSE ) )
			{
				return;
			}
			if( COLLISION::IsLineTriangleCollision(
				&pBasePos[1],
				&pBasePos[2],
				&pBasePos[3],
				&vecWaterSurfPos[ dwSize-2 ],
				&vecWaterSurfPos[ dwSize-1 ],
				&vColl,
				NULL,
				EMCC_CULL_NONE,
				FALSE ) )
			{
				return;
			}
		}

		// Note : 계속적으로 새로운 점을 만든다.
		size_t dwSize = vecWaterSurfPos.size();
		vDir = vPoint - vPos;
		Create( pSetLandMan, FALSE, vecWaterSurfPos, pBasePos, vPoint, vDir, fRadius );
	}
}

void WaterSurfPointMan::PlaneRemake( DxSetLandMan* pSetLandMan )
{
	// Note : Plane을 생성한다.
	m_pWaterSurfPlaneGroup[0]->Create( pSetLandMan, m_Property, m_vecWaterSurfPos[0], 10 );
	m_pWaterSurfPlaneGroup[1]->Create( pSetLandMan, m_Property, m_vecWaterSurfPos[1], 10 );

    CountVertexMax();
}

void WaterSurfPointMan::FrameMove( float fElapsedTime )
{
	m_pWaterSurfPlaneGroup[0]->FrameMove( m_Property, fElapsedTime );
	m_pWaterSurfPlaneGroup[1]->FrameMove( m_Property, fElapsedTime );
}

void WaterSurfPointMan::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
    UINT nVertexRender = 0;
    m_pWaterSurfPlaneGroup[0]->CountVertexRender( nVertexRender );
    m_pWaterSurfPlaneGroup[1]->CountVertexRender( nVertexRender );

    if( nVertexRender <= 0 )
        return;

    // 한방에 뿌릴려고 렌더링 되지 않는 버텍스 추가한다
    // 그런데 처음에는 추가하지 않으므로 처음에것은 빼줘야함
    nVertexRender -= PLANE_LINK_VERTEX;

    UINT nLockSize = sizeof( WATERSURFVERTEX ) * nVertexRender;
    VOID* pLockVoid = m_sVertexBuffer.LockVB( nLockSize );
    if( !pLockVoid )
        return;

    WATERSURFVERTEX* pLocked = static_cast< WATERSURFVERTEX* >( pLockVoid );

    bool bFirst = true;
	m_pWaterSurfPlaneGroup[0]->Render( pLocked, bFirst );
	m_pWaterSurfPlaneGroup[1]->Render( pLocked, bFirst );

    UINT uiStartVertex = m_sVertexBuffer.UnlockVB( nLockSize, nVertexRender );

    pd3dDevice->SetTexture( 0, m_textureRes.GetTexture() );
    pd3dDevice->SetStreamSource( 0, m_sVertexBuffer.pVB, 0, sizeof( WATERSURFVERTEX ) );
    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, uiStartVertex, nVertexRender - 2 );
}

void WaterSurfPointMan::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, float fSize, DWORD dwColor )
{
	for( DWORD i=0; i<m_vecWaterSurfPos[0].size(); ++i )
	{
		EDITMESHS::RENDERSPHERE( pd3dDevice, m_vecWaterSurfPos[0][i], fSize, NULL, dwColor );
	}
}

void WaterSurfPointMan::ResetData()
{
	m_pWaterSurfPlaneGroup[0]->ResetData( m_Property );
	m_pWaterSurfPlaneGroup[1]->ResetData( m_Property );
}

void WaterSurfPointMan::AddHeight( float fHeight )
{
	m_pWaterSurfPlaneGroup[0]->AddHeight( fHeight );
	m_pWaterSurfPlaneGroup[1]->AddHeight( fHeight );
}

void WaterSurfPointMan::MoveCamera( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_vecWaterSurfPos[0].empty() )
		return;

	DxViewPort::GetInstance().CameraJump( m_vecWaterSurfPos[0][0] );
}

void WaterSurfPointMan::SetObjRotate90( const D3DXMATRIX& matRotate )
{
    m_pWaterSurfPlaneGroup[0]->SetObjRotate90( matRotate );
	m_pWaterSurfPlaneGroup[1]->SetObjRotate90( matRotate );
}

void WaterSurfPointMan::CountVertexMax()
{
    m_nVertexMax = 0;
    m_pWaterSurfPlaneGroup[0]->CountVertexMax( m_nVertexMax );
    m_pWaterSurfPlaneGroup[1]->CountVertexMax( m_nVertexMax );

    if( m_nVertexMax <= 0 )
        return;

    // 한방에 뿌릴려고 렌더링 되지 않는 버텍스 추가한다
    // 그런데 처음에는 추가하지 않으므로 처음에것은 빼줘야함
    m_nVertexMax -= PLANE_LINK_VERTEX;
}

void WaterSurfPointMan::Save( sc::SerialFile &SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile << m_strName;
		m_Property.Save( SFile );

		DWORD dwSize = (DWORD) m_vecWaterSurfPos[0].size();
		SFile << dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile << m_vecWaterSurfPos[0][i];
		}

		dwSize = (DWORD) m_vecWaterSurfPos[1].size();
		SFile << dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile << m_vecWaterSurfPos[1][i];
		}

		m_pWaterSurfPlaneGroup[0]->Save( SFile );
		m_pWaterSurfPlaneGroup[1]->Save( SFile );
	}
	SFile.EndBlock();
}

void WaterSurfPointMan::Load( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile >> m_strName;
	m_Property.Load( SFile );

	DWORD dwSize;
	D3DXVECTOR3 vPos(0.f,0.f,0.f);

	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> vPos;
		m_vecWaterSurfPos[0].push_back( vPos );
	}

	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> vPos;
		m_vecWaterSurfPos[1].push_back( vPos );
	}

	m_pWaterSurfPlaneGroup[0]->Load( SFile, m_Property );
	m_pWaterSurfPlaneGroup[1]->Load( SFile, m_Property );

    CountVertexMax();

    OnLostDevice();
	OnDestroyDevice();
	OnCreateDevice( pd3dDevice );
    OnResetDevice( pd3dDevice );
}

void WaterSurfPointMan::Import( LPDIRECT3DDEVICEQ pd3dDevice, const WaterSurfPointMan* pSrc )
{
	CleanUp();

	m_strName = pSrc->m_strName;
	m_Property = pSrc->m_Property;

	for( DWORD i=0; i<pSrc->m_vecWaterSurfPos[0].size(); ++i )
	{
		m_vecWaterSurfPos[0].push_back( pSrc->m_vecWaterSurfPos[0][i] );
	}

	for( DWORD i=0; i<pSrc->m_vecWaterSurfPos[1].size(); ++i )
	{
		m_vecWaterSurfPos[1].push_back( pSrc->m_vecWaterSurfPos[1][i] );
	}

	m_pWaterSurfPlaneGroup[0]->Import( pSrc->m_pWaterSurfPlaneGroup[0], m_Property );
	m_pWaterSurfPlaneGroup[1]->Import( pSrc->m_pWaterSurfPlaneGroup[1], m_Property );

	CountVertexMax();

	OnLostDevice();
	OnDestroyDevice();
	OnCreateDevice( pd3dDevice );
	OnResetDevice( pd3dDevice );
}