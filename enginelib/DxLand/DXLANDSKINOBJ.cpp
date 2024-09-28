#include "pch.h"
#include "../../SigmaCore/File/SerialFile.h"

#include "../DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../DxTools/DxMethods.h"
#include "../DxTools/DxShadowMap.h"
#include "../DxTools/RENDERPARAM.h"
#include "../DxTools/DxEnvironment.h"
#include "../G-Logic/GLPeriod.h"
#include "./ExportXFile/NSExportXFile.h"
#include "./LightMap/LightMapBoxColor.h"
#include "./LightMap/NSLightMapTools.h"
#include "./DxPiece/NSPieceQuickSort.h"
#include "./DxPiece/DxPieceDef.h"

#include "DxLandSkinObj.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

float		DXLANDSKINOBJ::m_fTime = 0.0f;
float		DXLANDSKINOBJ::m_fElapsedTime = 0.0f;

DXLANDSKINOBJ::DXLANDSKINOBJ( LPDIRECT3DDEVICE9 pd3dDevice )
	: m_pd3dDevice(pd3dDevice)
	, m_vCenter(0.f,0.f,0.f)
	, m_fRadius(1.f)
	, m_vMax(3,18,3)
	, m_vMin(-3,0,-3)
	, m_vMaxOrg(3,18,3)
	, m_vMinOrg(-3,0,-3)
	, m_pSkinCharData(NULL)
	, m_pSkinChar(NULL)
	, m_pNext(NULL)
	, m_bRender(TRUE)
	, m_bOnlyFixedForMaterial(FALSE)
	, m_pLightMapBoxColor(NULL)
	, m_fVisibleRadius(DEFAULT_VISIBLE_RADIUS)
	, m_fInvisibleRadius(DEFAULT_INVISIBLE_RADIUS)
{
	StringCchCopy( m_szName, LANDSKINOBJ_MAXSZ, "이름없음" );
	memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

	D3DXMatrixIdentity( &m_matWorld );

	m_pLightMapBoxColorFullData = new LightMapBoxColorFullData;
}

DXLANDSKINOBJ::~DXLANDSKINOBJ(void)
{
	SAFE_DELETE(m_pSkinChar);
	SAFE_DELETE( m_pLightMapBoxColor );
	SAFE_DELETE( m_pLightMapBoxColorFullData );

	SAFE_DELETE(m_pNext);
}

HRESULT DXLANDSKINOBJ::PlaySkinChar ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pSkinCharData )
	{
		// [shhan][2014.12.16] 문자가 없다면 작업안함.
		//						WorldEdit 에서 LandSkinObj 를 여러개 만들면 문제가 발생하였다.
		size_t nSize(0);
		StringCchLength( m_szFileName, MAX_PATH, &nSize );
		if ( nSize == 0 )
			return E_FAIL;

		m_pSkinCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_szFileName, pd3dDevice );
		if ( !m_pSkinCharData )
            return E_FAIL;
	}
	
	if ( !m_pSkinChar )
	{
		m_pSkinChar = new DxSkinChar;
		m_pSkinChar->SetCharData ( m_pSkinCharData, pd3dDevice, FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	}

	return S_OK;
}

HRESULT DXLANDSKINOBJ::SetSkinChar ( const char* szName, const D3DXMATRIX& matWorld, const char* szFile, LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;
	GASSERT(szName);
	GASSERT(szFile);

	StringCchCopy( m_szName, LANDSKINOBJ_MAXSZ, szName );
	StringCchCopy( m_szFileName, MAX_PATH, szFile );

	m_vMax = m_vMaxOrg;
	m_vMin = m_vMinOrg;
	m_matWorld = matWorld;
	COLLISION::TransformAABB( m_vMax, m_vMin, m_matWorld );

	hr = PlaySkinChar ( pd3dDevice );
	if (FAILED(hr) )	return hr;

	SetAABBBOX( m_vMaxOrg, m_vMinOrg );

	m_vMax = m_vMaxOrg;
	m_vMin = m_vMinOrg;
	COLLISION::TransformAABB ( m_vMax, m_vMin, m_matWorld );

	// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
	m_vCenter = (m_vMax+m_vMin)*0.5f;
	D3DXVECTOR3 vHalf = m_vMax - m_vCenter;
	m_fRadius = D3DXVec3Length( &vHalf );

	return S_OK;
}

HRESULT DXLANDSKINOBJ::Import ( const DXLANDSKINOBJ* pSrc, LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_bOnlyFixedForMaterial = TRUE;

	SetSkinChar( pSrc->m_szName, pSrc->m_matWorld, pSrc->m_szFileName, pd3dDevice );
	*m_pLightMapBoxColorFullData = *pSrc->m_pLightMapBoxColorFullData;

	m_fVisibleRadius = pSrc->m_fVisibleRadius;
	m_fInvisibleRadius = pSrc->m_fInvisibleRadius;

	return S_OK;
}

void DXLANDSKINOBJ::SetAABBBOX( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	if( m_pSkinChar )
	{
		vMax = m_pSkinChar->m_vMax;
		vMin = m_pSkinChar->m_vMin;
	}
}

HRESULT DXLANDSKINOBJ::Load ( sc::SerialFile	&SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DWORD dwVer, dwSize;
	
	SFile >> dwVer;
	SFile >> dwSize;
	
	if ( dwVer == VERSION )
	{
		m_bOnlyFixedForMaterial = TRUE;

		SFile.ReadBuffer( m_szName, sizeof(char)*LANDSKINOBJ_MAXSZ );
		SFile.ReadBuffer( m_szFileName, sizeof(char)*MAX_PATH );

		SFile >> m_vMax;
		SFile >> m_vMaxOrg;

		SFile >> m_vMin;
		SFile >> m_vMinOrg;

		SFile.ReadBuffer( m_matWorld, sizeof(D3DXMATRIX) );

		SFile >> m_fVisibleRadius;   // Piece의 alpha가 1이 되는 반경.
		SFile >> m_fInvisibleRadius; // Piece의 alpha가 0이 되는 반경.

		m_pLightMapBoxColorFullData->Load( SFile );
	}
	else if ( dwVer == 0x0103 )	//	데이터 포맷 변경 진행
	{
		SFile.ReadBuffer( m_szName, sizeof(char)*LANDSKINOBJ_MAXSZ );
		SFile.ReadBuffer( m_szFileName, sizeof(char)*MAX_PATH );

		SFile >> m_vMax;
		SFile >> m_vMaxOrg;

		SFile >> m_vMin;
		SFile >> m_vMinOrg;

		SFile.ReadBuffer( m_matWorld, sizeof(D3DXMATRIX) );
	}
	else if ( dwVer == 0x0102 )
	{
		SFile.ReadBuffer( m_szName, sizeof(char)*LANDSKINOBJ_MAXSZ );
		SFile.ReadBuffer( m_szFileName, sizeof(char)*MAX_PATH );
		SFile.ReadBuffer( m_matWorld, sizeof(D3DXMATRIX) );

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile >> m_vMaxOrg;
		SFile >> m_vMinOrg;
	}
	else if ( dwVer == 0x0101 )
	{
		DXAFFINEPARTS m_sAffineParts;

		SFile.ReadBuffer ( m_szName, sizeof(char)*LANDSKINOBJ_MAXSZ );
		SFile.ReadBuffer ( m_szFileName, sizeof(char)*MAX_PATH );
		SFile.ReadBuffer ( &m_sAffineParts, sizeof(DXAFFINEPARTS) );

		D3DXMatrixCompX( m_matWorld, m_sAffineParts );

		SFile >> m_vMax;
		SFile >> m_vMin;

		SFile >> m_vMaxOrg;
		SFile >> m_vMinOrg;
	}
	else
	{
		CDebugSet::ToFile ( "LoadSet.log", "[%s/%s] 단독 이팩트의 버전이 틀려 로드가 생략되었습니다.",
			m_szName, m_szFileName );

		SFile.SetOffSet ( SFile.GetfTell() + dwSize );
		hr = E_FAIL;
	}

	// 로딩 후 해줘야 하는 작업.
	// [목적] Export 다 된 wld에서 SkinChar 의 Max Min 이 변경 되었을 시. 다시 Export 할 필요 없도록 하기 위함.
	// [TODO] m_vMaxOrg, m_vMinOrg 은 Save, Load 를 할 필요가 없다.
	//			나중에 시간이 남으면 작업하도록 하자.
	{
		hr = PlaySkinChar ( pd3dDevice );
		if (FAILED(hr) )
			return hr;

		SetAABBBOX( m_vMaxOrg, m_vMinOrg );

		m_vMax = m_vMaxOrg;
		m_vMin = m_vMinOrg;
		COLLISION::TransformAABB ( m_vMax, m_vMin, m_matWorld );

		// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
		m_vCenter = (m_vMax+m_vMin)*0.5f;
		D3DXVECTOR3 vHalf = m_vMax - m_vCenter;
		m_fRadius = D3DXVec3Length( &vHalf );
	}

	return hr;
}

HRESULT DXLANDSKINOBJ::Save ( sc::SerialFile	&SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile.WriteBuffer ( m_szName, sizeof(char)*LANDSKINOBJ_MAXSZ );
		SFile.WriteBuffer ( m_szFileName, sizeof(char)*MAX_PATH );

		SFile << m_vMax;
		SFile << m_vMaxOrg;

		SFile << m_vMin;
		SFile << m_vMinOrg;

		SFile.WriteBuffer ( m_matWorld, sizeof(D3DXMATRIX) );

		SFile << m_fVisibleRadius;   // Piece의 alpha가 1이 되는 반경.
		SFile << m_fInvisibleRadius; // Piece의 alpha가 0이 되는 반경.

		m_pLightMapBoxColorFullData->Save( SFile );
	}
	SFile.EndBlock();

	return S_OK;
}

void DXLANDSKINOBJ::InsertData ( const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength )
{
	// 등록을 하도록 한다.
	BOOL bLodBlending = m_bOnlyFixedForMaterial;	// 새맵일 경우 LOD Blending 도 작동된다.
	NSPieceQuickSort::InsertData( this, vFromPt, vLookatPt, fLength, bLodBlending, FALSE );
}

void DXLANDSKINOBJ::InsertDataForShadow()
{
	// 등록을 하도록 한다.
	NSPieceQuickSort::InsertDataForShadow( this );
}

void DXLANDSKINOBJ::FrameMove( const float fElapsedTime )
{
	if ( !m_pSkinChar )
		return;

	HRESULT hr = m_pSkinChar->FrameMove ( m_fTime, fElapsedTime, FALSE, m_matWorld );
	if ( FAILED(hr) )
		return;
}

void DXLANDSKINOBJ::FrameMoveEntity( float fElapsedTime )
{
	FrameMove( fElapsedTime );
}

void DXLANDSKINOBJ::RenderSkinObject( BOOL bReflect, float fReflectHeight )
{
	DWORD dwFVF(0L);
	m_pd3dDevice->GetFVF( &dwFVF );
	m_pd3dDevice->SetFVF( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );

	Render ( m_pd3dDevice, NULL, bReflect, fReflectHeight );

	m_pd3dDevice->SetFVF( dwFVF );
}

HRESULT DXLANDSKINOBJ::Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV )
{
	return Render ( pd3dDevice, pCV, FALSE, 0.f );
}

void DXLANDSKINOBJ::Render2( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect, float fReflectHeight, BOOL bPointLightRT )
{
	if ( bReflect )
	{
		if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_1_1 )
		{
			if ( DxEnvironment::GetInstance().IsRecentReflect() )
			{
				D3DXMATRIX matReflect, matReflectView;
				D3DXMatrixIdentity( &matReflect );
				matReflect._42 = -(fReflectHeight*2.f);
				D3DXMatrixMultiply_MIRROR( matReflect );
				D3DXMatrixMultiply( &matReflectView, &matReflect, &DxViewPort::GetInstance().GetMatView() );

				NSMaterialSkinManager::FrameMove( matReflectView, DxViewPort::GetInstance().GetMatProj() );
			}
			else
			{
				NSMaterialSkinManager::FrameMove( DxEnvironment::GetInstance().GetMatrix_ReflectView(), DxViewPort::GetInstance().GetMatProj() );
			}

			DWORD	dwCullMode;
			pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

			m_pSkinChar->RenderReflect ( pd3dDevice, m_fAlpha );

			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );

			{
				NSMaterialSkinManager::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj() );
			}
		}
	}
	else
	{
		//if ( bFrameMove )
		//{
		//	HRESULT hr = m_pSkinChar->FrameMove ( m_fTime, m_fElapsedTime, matWorld );
		//	if ( FAILED(hr) )
		//		return;
		//}

		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
			m_pSkinChar->RenderFixed( pd3dDevice, bPointLightRT, m_fAlpha, FALSE, TRUE, FALSE, FALSE, TRUE );
		}
		else
		{
			m_pSkinChar->SetPhysX_Cloth( TRUE );
			m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, m_fAlpha );
			m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, m_fAlpha );
			m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, m_fAlpha );
			m_pSkinChar->RenderMaterialEffect ( pd3dDevice, m_fAlpha );
		}
	}
}

HRESULT DXLANDSKINOBJ::Render ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV, BOOL bReflect, float fReflectHeight )	//, BOOL bFrameMove )
{
	if( !m_bRender ) 
		return S_OK;

	HRESULT hr;

	if ( !m_pSkinChar )
	{
		hr = PlaySkinChar ( pd3dDevice );
		if (FAILED(hr) )	
			return S_OK;	// return hr 을 주고 싶지만, 그렇게 되면 밑에 Tree에 있는게 렌더링이 안되서 바뀜
	}

	if ( !m_pSkinChar )
	{
		return S_OK;	// return hr 을 주고 싶지만, 그렇게 되면 밑에 Tree에 있는게 렌더링이 안되서 바뀜
	}

	if ( m_bOnlyFixedForMaterial )
	{
		D3DXVECTOR4*	pAmbientCube(NULL);
		float*			fAmbientCubeSpot1(NULL);
		float*			fAmbientCubeSpot2(NULL);
		D3DXVECTOR4*	pVoxelColor(NULL);
		D3DXVECTOR2*	pSpot1_Spot2(NULL);
		m_pLightMapBoxColorFullData->FrameMoveStep
		( 
			&pAmbientCube,
			&fAmbientCubeSpot1,
			&fAmbientCubeSpot2,
			&pVoxelColor,
			&pSpot1_Spot2
		);

		//m_pLightMapBoxColorFullData->FrameMoveStep( m_vVoxelColorUsed, &m_avAmbientCube[0] );

		D3DLIGHTQ sLightBackUp;
		pd3dDevice->GetLight( 0, &sLightBackUp );

		D3DLIGHTQ sLight;
		sLight = sLightBackUp;
		sLight.Diffuse.r = 0.f;
		sLight.Diffuse.g = 0.f;
		sLight.Diffuse.b = 0.f;
		sLight.Ambient.r = pVoxelColor->x;
		sLight.Ambient.g = pVoxelColor->y;
		sLight.Ambient.b = pVoxelColor->z;
		pd3dDevice->SetLight( 0, &sLight );

		Render2( pd3dDevice, bReflect, fReflectHeight, FALSE );

		pd3dDevice->SetLight( 0, &sLightBackUp );

		return S_OK;
	}



	D3DLIGHTQ sLightBackUp;
	BOOL bLightEnable1(FALSE);
	BOOL bLightEnable2(FALSE);
	if ( m_pSkinChar->IsUseWorldColor() )
	{
		pd3dDevice->GetLight( 0, &sLightBackUp );
		pd3dDevice->GetLightEnable( 1, &bLightEnable1 );
		pd3dDevice->GetLightEnable( 2, &bLightEnable2 );

		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNor(0.f,1.f,0.f);
		D3DXVECTOR3 vColorALL = D3DXVECTOR3 (0.f,0.f,0.f);
		D3DXVECTOR3 vColorIndirect = D3DXVECTOR3 (0.f,0.f,0.f);
		D3DXVECTOR3 vColorNoDynamicLight = D3DXVECTOR3 (0.f,0.f,0.f);
		D3DXVECTOR3 vColorDirectionDirect(0.f,0.f,0.f);
		float fSpot1Color(0.f);
		float fSpot2Color(0.f);
		vPos.x = m_matWorld._41;
		vPos.y = m_matWorld._42;
		vPos.z = m_matWorld._43;

		// Color 를 얻어온다.
		NSLightMapTools::GetColorForObject
		( 
			pd3dDevice, 
			NULL, 
			NULL, 
			vPos, 
			vNor, 
			FALSE, 
			vColorALL, 
			vColorIndirect,
			fSpot1Color, 
			fSpot2Color,
			vColorNoDynamicLight,
			vColorDirectionDirect,
			WORDWORD(-1,-1) 
		);

		float fOverLightLimit(4.f);
		if ( vColorALL.x > fOverLightLimit )
			vColorALL.x = fOverLightLimit;
		if ( vColorALL.y > fOverLightLimit )
			vColorALL.y = fOverLightLimit;
		if ( vColorALL.z > fOverLightLimit )
			vColorALL.z = fOverLightLimit;

		D3DLIGHTQ sLight;
		sLight = sLightBackUp;
		sLight.Diffuse.r = 0.f;
		sLight.Diffuse.g = 0.f;
		sLight.Diffuse.b = 0.f;
		sLight.Ambient.r = vColorALL.x;
		sLight.Ambient.g = vColorALL.y;
		sLight.Ambient.b = vColorALL.z;
		pd3dDevice->SetLight( 0, &sLight );
		pd3dDevice->LightEnable( 1, FALSE );
		pd3dDevice->LightEnable( 2, FALSE );
	}

	Render2( pd3dDevice, bReflect, fReflectHeight, TRUE );

	if ( m_pSkinChar->IsUseWorldColor() )
	{
		pd3dDevice->SetLight( 0, &sLightBackUp );
		pd3dDevice->LightEnable( 1, bLightEnable1 );
		pd3dDevice->LightEnable( 2, bLightEnable2 );
	}

	return S_OK;
}

HRESULT DXLANDSKINOBJ::RenderName ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3 vOut;
	D3DXVECTOR3 vSrc( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	D3DXVec3Project( &vOut, &vSrc, &DxViewPort::GetInstance().GetViewPort(), &DxViewPort::GetInstance().GetMatProj(), 
		&DxViewPort::GetInstance().GetMatView(), &matIdentity );
	CDebugSet::ToPos( vOut.x, vOut.y, "%s", m_szName );

	if ( m_pNext )
	{
		return m_pNext->RenderName ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DXLANDSKINOBJ::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	if ( m_pSkinChar )
	{
		return m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DXLANDSKINOBJ::InvalidateDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		return m_pSkinChar->InvalidateDeviceObjects ();
	}

	return S_OK;
}

void DXLANDSKINOBJ::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->SetWorldBattleFlag( pWorldBattleFlag );
	}
}

void DXLANDSKINOBJ::ExportXFile( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strSkinName, const TSTRING& strTexName )
{
	// Mesh 정보를 얻어온다.
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	std::vector<VERTEXNORCOLORTEX4>	vecVertices;

	if ( !m_pSkinChar )
	{
		PlaySkinChar( pd3dDevice );
	}

	if ( m_pSkinChar )
	{
		m_pSkinChar->GetStaticVertexData( vecVertices, m_matWorld );
	}

	if ( vecVertices.empty() )
		goto _RETURN;

	NSExportXFile::ExportDxStaticMesh_SingleTexMesh
	( 
		pxofsave, 
		pDataObjectRoot, 
		vecVertices,
		strSkinName, 
		strTexName,
		matIdentity 
	);

_RETURN:
	if ( m_pNext )
	{
		m_pNext->ExportXFile( pd3dDevice, pxofsave, pDataObjectRoot, strSkinName, strTexName );
	}
}

void DXLANDSKINOBJ::LightMapCalculationBoxColor( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshCollBase* pStaticMesh, DxPieceManager* pPieceManager )
{
	m_bOnlyFixedForMaterial = TRUE;

	SAFE_DELETE ( m_pLightMapBoxColor );
	m_pLightMapBoxColor = new LightMapBoxColor( m_vMin, m_vMax, m_matWorld );
	m_pLightMapBoxColor->LightMapCalculationBoxColor( pd3dDevice, pStaticMesh, pPieceManager, WORDWORD(-1,-1) );

	m_pLightMapBoxColorFullData->LightMapCalculationBoxColor( m_pLightMapBoxColor );
}

void DXLANDSKINOBJ::AddRadiosityColorForVoxel( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, LPDIRECT3DTEXTUREQ pTextureSystem, float fReflectance, float fFogBlend, DWORD dwFogLength )
{
	m_bOnlyFixedForMaterial = TRUE;

	m_pLightMapBoxColor->BakeRadiosity( pd3dDevice, pLandMan, pTextureSystem, fReflectance, fFogBlend, dwFogLength );

	m_pLightMapBoxColorFullData->AddRadiosityColorForVoxel( m_pLightMapBoxColor );
}












//////////////////////////////////////////////////////////////////////////
//						DxLandSkinManager
DxLandSkinManager::DxLandSkinManager()
	: m_dwNumLandSkinObj(NULL)
	, m_pLandSkinObjList(NULL)
	, m_pLandSkinObjTree(NULL)
{

}

DxLandSkinManager::~DxLandSkinManager()
{
	CleanUp();
}

void DxLandSkinManager::CleanUp()
{
	m_dwNumLandSkinObj = 0;
	SAFE_DELETE(m_pLandSkinObjList);
	SAFE_DELETE(m_pLandSkinObjTree);
}

void DxLandSkinManager::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	PLANDSKINOBJ pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		pCur->RestoreDeviceObjects ( pd3dDevice );

		pCur = pCur->m_pNext;
	}
}

void DxLandSkinManager::OnLostDevice()
{
	PLANDSKINOBJ pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		pCur->InvalidateDeviceObjects();

		pCur = pCur->m_pNext;
	}
}

void DxLandSkinManager::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	PLANDSKINOBJ pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		pCur->SetWorldBattleFlag( pWorldBattleFlag );

		pCur = pCur->m_pNext;
	}
}
//
//void DxLandSkinManager::FrameMove( float fElapsedTime, const CLIPVOLUME &cv, float fRealTimeWaterHeight )
//{
//	////	Note : 렌더에 필요한 스킨Object 를 걸러낸다.
//	////			일반 카메라와 반사쪽 카메라도 같이 넣어줘서 걸러내도록 한다.
//	//if ( fRealTimeWaterHeight == FLT_MAX )
//	//{
//	//	COLLISION::FrameMoveAABBTree( m_pLandSkinObjTree, fElapsedTime, cv, NULL );
//	//}
//	//else
//	//{
//	//	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
//	//	D3DXVECTOR3&	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
//
//	//	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
//	//	D3DXVECTOR3		vTempFromPt		= vFromPt;
//	//	D3DXVECTOR3		vTempLookatPt	= vLookatPt;
//
//	//	vTempFromPt.y = (2.f*fRealTimeWaterHeight) - vTempFromPt.y;
//	//	vTempLookatPt.y = (2.f*fRealTimeWaterHeight) - vTempLookatPt.y;
//
//	//	CLIPVOLUME sReflectCV = DxViewPort::GetInstance().ComputeClipVolumeGameSetting( vTempLookatPt, vTempFromPt, vUpVec );
//	//	COLLISION::FrameMoveAABBTree( m_pLandSkinObjTree, fElapsedTime, cv, &sReflectCV );
//	//}
//}
//
//void DxLandSkinManager::RenderAABBTree( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
//{
//	//COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pLandSkinObjTree, FALSE );
//}
//
//void DxLandSkinManager::RenderSkinObject( BOOL bReflect, float fReflectHeight, BOOL bFrameMove )
//{
//
//}
//
//void DxLandSkinManager::RenderAABBTreeReflect( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, float fRealTimeWaterHeight )
//{
//
//}
//
//void DxLandSkinManager::Render_Query_Prepare( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
//{
//
//}
//
//void DxLandSkinManager::Render_Query( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv )
//{
//
//}

void DxLandSkinManager::AddLandSkinObj ( PLANDSKINOBJ pLandSkinObj )
{
	m_dwNumLandSkinObj++;
	pLandSkinObj->m_pNext = m_pLandSkinObjList;
	m_pLandSkinObjList = pLandSkinObj;
}

void DxLandSkinManager::DelLandSkinObj ( PLANDSKINOBJ pLandSkinObj )
{
	GASSERT(pLandSkinObj);
	PLANDSKINOBJ pCur, pBack;

	if ( m_pLandSkinObjList==pLandSkinObj )
	{
		pCur = m_pLandSkinObjList;
		m_pLandSkinObjList = m_pLandSkinObjList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandSkinObj--;
		return;
	}

	pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( pCur==pLandSkinObj )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandSkinObj--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

PLANDSKINOBJ DxLandSkinManager::FindLandSkinObj ( char* szName )
{
	PLANDSKINOBJ pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szName,szName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

PLANDSKINOBJ DxLandSkinManager::FindLandSkinObjByFileName ( const char* szFileName )
{
	PLANDSKINOBJ pCur = m_pLandSkinObjList;
	while ( pCur )
	{
		if ( !strcmp(pCur->m_szFileName,szFileName) )	return pCur;

		pCur = pCur->m_pNext;
	};

	return NULL;
}

void DxLandSkinManager::BuildSkinObjTree ()
{
	COLLISION::MakeAABBTree ( m_pLandSkinObjTree, m_pLandSkinObjList );
}

void DxLandSkinManager::Import( LPDIRECT3DDEVICEQ pd3dDevice, const DXLANDSKINOBJ* pCurSkinObj )
{
	CleanUp();

	while ( pCurSkinObj )
	{
		PLANDSKINOBJ pNewSkinObj = new DXLANDSKINOBJ( pd3dDevice );
		pNewSkinObj->Import ( pCurSkinObj, pd3dDevice );
		AddLandSkinObj ( pNewSkinObj );

		pCurSkinObj = pCurSkinObj->m_pNext;
	}

	BuildSkinObjTree ();
}

void DxLandSkinManager::ExportXFile( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strSkinName, const TSTRING& strTexName )
{
	if ( !m_pLandSkinObjList )
		return;

	m_pLandSkinObjList->ExportXFile( pd3dDevice, pxofsave, pDataObjectRoot, strSkinName, strTexName );
}

void DxLandSkinManager::SaveFile( sc::SerialFile& SFile )
{
	SFile << BOOL ( m_pLandSkinObjList!=NULL );

	PLANDSKINOBJ pLandSkinObj = m_pLandSkinObjList;
	while ( pLandSkinObj )
	{
		pLandSkinObj->Save ( SFile );

		pLandSkinObj = pLandSkinObj->m_pNext;
		SFile << BOOL ( pLandSkinObj!=NULL );
	}
}

void DxLandSkinManager::LoadFile( sc::SerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL bExist(FALSE);
	SFile >> bExist;
	if ( bExist )
	{
		bExist = TRUE;
		while ( bExist )
		{
			PLANDSKINOBJ pLandSkinObj = new DXLANDSKINOBJ( pd3dDevice );
			pLandSkinObj->Load ( SFile, pd3dDevice );
			AddLandSkinObj ( pLandSkinObj );

			SFile >> bExist;
		}

		BuildSkinObjTree ();
	}
}

//////////////////////////////////////////////////////////////////////////
// 전체 다 호출된다.
void DxLandSkinManager::InsertQuickSort( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend, BOOL bReflect )
{
	if ( !m_pLandSkinObjTree )
		return;

	D3DXVECTOR3 vDir = vFromPt - vLookatPt;
	float fLength = D3DXVec3Length( &vDir );

	COLLISION::InsertData( m_pLandSkinObjTree, sCV, vFromPt, vLookatPt, fLength );
}

void DxLandSkinManager::InsertQuickSortForShadow( const CLIPVOLUME &sCV )
{
	if ( !m_pLandSkinObjTree )
		return;

	COLLISION::InsertDataForShadow( m_pLandSkinObjTree, sCV );
}

//void DxLandSkinManager::InsertQuickSortPSF( const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, BOOL bLodBlending, BOOL bTimeBlend )
//{
//	if ( !m_pAllPieceTree || !m_pOldMapAndCollisionPieceTree )
//		return;
//
//	PROFILE_BEGIN2("DxPieceAABB::Render");
//
//	D3DXVECTOR3 vDir = vFromPt - vLookatPt;
//	float fLength = D3DXVec3Length( &vDir );
//
//	// Note : Quick Sort 를 한다.		이넘을 저기서도 쓰려면... 데이터를 공유하면 안될 듯 하네.
//	//			Sort 정보만 
//	//
//	// RENDERPARAM::g_emTnLMode 를 쓰는 건 PSF 상에서 확인되야 하기 때문이다.
//	// PSF 상 RENDERPARAM::g_emGroundQulity 값은 최고값이 셋팅된다.
//	if ( RENDERPARAM::g_emTnLMode >= TnL_PS_3_0_DF )
//	{
//		NSPIECETREE::InsertQuickSortALL( m_pAllPieceTree, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
//	}
//	else
//	{
//		NSPIECETREE::InsertQuickSortALL( m_pOldMapAndCollisionPieceTree, sCV, vFromPt, vLookatPt, fLength, bLodBlending, bTimeBlend );
//	}
//
//
//	PROFILE_END2("DxPieceAABB::Render");
//}
// 전체 다 호출된다.
//////////////////////////////////////////////////////////////////////////