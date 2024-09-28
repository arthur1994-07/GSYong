#include "pch.h"

#include <algorithm>

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxPrefabMan.h"
#include "../../DxLand/DxLandMan.h"
#include "../../Common/StlFunctions.h"
#include "../../Common/profile.h"
#include "../../DxCommon9/DXUTmisc.h"
#include "../../GlobalParam.h"

#include "./NSPieceQuickSort.h"
#include "./DxPiececontainer.h"
#include "./DxPieceEdit.h"
#include "./DxPieceAABB.h"
#include "./DxPiece.h"

#include "DxPieceManagerPSF.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


static const float g_fCELL_SIZE_PSF = 10.f;

// -----------------------------------------------------------------------------------------------------------------------------------------
//											M	A	T	R	I	X		P	I	S	N	A	M	E
// -----------------------------------------------------------------------------------------------------------------------------------------
DxPieceManagerPSF::MATRIXPISNAME::MATRIXPISNAME()
	: m_pPisName(NULL)
	, m_pPiece(NULL)
{
}

DxPieceManagerPSF::MATRIXPISNAME::~MATRIXPISNAME()
{
	CleanUp();
}

void DxPieceManagerPSF::MATRIXPISNAME::CleanUp()
{
	m_pPiece = NULL;	// m_mapNamePiece 에 넘겨주기 위해서 존재 했었다.
	SAFE_DELETE_ARRAY( m_pPisName );
}

void DxPieceManagerPSF::MATRIXPISNAME::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile.WriteBuffer( m_pPiece->m_matWorld, sizeof(D3DXMATRIX) );

		DWORD dwSize = 0;
		if( m_pPisName )	dwSize = (DWORD)strlen(m_pPisName) + 1;
		else				dwSize = 0;
		SFile << dwSize;
		if( dwSize )
		{
			SFile.WriteBuffer( m_pPisName, sizeof(char)*dwSize );
		}

		SFile << m_pPiece->GetVisibleRadius();
		SFile << m_pPiece->GetInvisibleRadius();

		SFile << m_pPiece->IsExceptLowOption();
	}
	SFile.EndBlock();
}

void DxPieceManagerPSF::MATRIXPISNAME::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	BOOL bShader_3_0(FALSE);
	float fVisibleRadius(DEFAULT_VISIBLE_RADIUS);
	float fInvisibleRadius(DEFAULT_INVISIBLE_RADIUS);
	if( dwVer == VERSION )
	{
		SFile.ReadBuffer( matWorld, sizeof(D3DXMATRIX) );

		DWORD dwSize;
		SFile >> dwSize;
		if( dwSize )
		{
			m_pPisName = new char[dwSize];
			SFile.ReadBuffer( m_pPisName, sizeof(char)*dwSize );
		}

		SFile >> fVisibleRadius;
		SFile >> fInvisibleRadius;

		SFile >> bShader_3_0;
	}
	else if( dwVer == 0x0100 )
	{
		SFile.ReadBuffer( matWorld, sizeof(D3DXMATRIX) );

		DWORD dwSize;
		SFile >> dwSize;
		if( dwSize )
		{
			m_pPisName = new char[dwSize];
			SFile.ReadBuffer( m_pPisName, sizeof(char)*dwSize );
		}

		SFile >> fVisibleRadius;
		SFile >> fInvisibleRadius;
	}

	DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, m_pPisName );
	if( !pPieceEdit )
	{
		std::string strName = "조각파일 : ";
		strName += m_pPisName;
		strName += " 이 없습니다.";
		MessageBox( NULL, strName.c_str(), "ERROR", MB_OK );
		return;
	}

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	m_pPiece = new DxPiece(pd3dDevice);
	m_pPiece->Import( pd3dDevice, pPieceEdit, matWorld, matIdentity, fVisibleRadius, fInvisibleRadius, bShader_3_0 );
}

void DxPieceManagerPSF::MATRIXPISNAME::Load_Prev( sc::SerialFile& SFile )
{
	CleanUp();

	SFile.ReadBuffer( m_matFrame, sizeof(D3DXMATRIX) );

	DWORD dwSize;
	SFile >> dwSize;
	if( dwSize )
	{
		m_pPisName = new char[dwSize];
		SFile.ReadBuffer( m_pPisName, sizeof(char)*dwSize );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//										D	x		P	i	e	c	e		M	a	n	a	g	e	r
// -----------------------------------------------------------------------------------------------------------------------------------------
const DWORD DxPieceManagerPSF::VERSION_PSF = 0x102;

DxPieceManagerPSF::EDITMODE DxPieceManagerPSF::g_emEDITMODE(DxPieceManagerPSF::MODE_EDIT);

DxPieceManagerPSF::DxPieceManagerPSF()
	: m_fTime(0.f)
	, m_fElapsedTime(0.f)
	, m_pPiecePick(NULL)
	, m_bUseLightMap(FALSE)
{
}

DxPieceManagerPSF::~DxPieceManagerPSF()
{
	CleanUp();
}

void DxPieceManagerPSF::CleanUp()
{
	m_fTime = 0.f;
	m_fElapsedTime = 0.f;

	m_bUseLightMap = FALSE;

	std::for_each( m_mapChangePis.begin(), m_mapChangePis.end(), std_afunc::DeleteMapObject() );
	m_mapChangePis.clear();

	std::for_each( m_vecPointPis.begin(), m_vecPointPis.end(), std_afunc::DeleteObject() );
	m_vecPointPis.clear();

	//MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	//for( ; iter!=m_mapNamePiece.end(); ++iter )
	//{
	//	(*iter).second->ClearTree();
	//}

	std::for_each( m_mapNamePiece.begin(), m_mapNamePiece.end(), std_afunc::DeleteMapObject() );
	m_mapNamePiece.clear();
}

void DxPieceManagerPSF::FrameMove( const float fTime, const float fElapsedTime )
{
	m_bFrameMoveOnce = true;
	m_fTime = fTime;
	m_fElapsedTime = fElapsedTime;
}

void DxPieceManagerPSF::Render( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bPSF, float fLightMapDetail, BOOL bFrameMove )
{
    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	// LightMap 셋팅이라면 LightMap 빠진 Piece 라도 OverLighting 되도록 Shader 로 렌더링 되도록 한다.
	if ( m_bUseLightMap )
	{
		// 카메라 동작후 작동되어야 한다.
		NSVERTEXNORTEX_FX::SetEnable();
	}

	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->InsertQuickSortPSF( sCV, DxViewPort::GetInstance().GetFromPt(), DxViewPort::GetInstance().GetLookatPt(), TRUE, TRUE );
		(*iter).second->Active_MATERIAL_DATA( pd3dDevice, TRUE );
		//(*iter).second->FrameMoveStep( pd3dDevice, TRUE );
	}

	// 애니메이션 관련 Object
	{
		NSPieceQuickSort::QuickSortProcess();

		if ( m_bFrameMoveOnce )
		{
			m_bFrameMoveOnce = false;
			NSPieceQuickSort::FrameMoveTOOL( m_fTime, m_fElapsedTime );
			NSPieceQuickSort::FrameMoveONLY( m_fTime, m_fElapsedTime );	// Note : FrameMove Static
		}
		NSPieceQuickSort::RenderOpaquePSF( pd3dDevice, sCV, FALSE, FALSE, TRUE, FALSE, 0.f );
		NSPieceQuickSort::RenderHardAlphaPSF( pd3dDevice, sCV, FALSE, FALSE, TRUE );
		NSPieceQuickSort::RenderDecalPSF();
	}

    // LightMap 렌더 시 위치좌표를 맞추기 위함.
	if ( m_bUseLightMap )
    {
        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity( &matIdentity );
        NSLightMapFX::SetWorld( matIdentity, 1.f );
        NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
		NSVERTEXNORTEX_FX::SetDisable();
    }
}

void DxPieceManagerPSF::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;
	
	NSPieceQuickSort::RenderSoftAlphaPSF( pd3dDevice, sCV, FALSE, FALSE, TRUE );
}

void DxPieceManagerPSF::Render_DecalPass2_WaterCustomSoftAlpha()
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	NSPieceQuickSort::RenderSkinObjectPSF( FALSE, 0.f );

	NSPieceQuickSort::RenderDecalPass2_PSF();
	NSPieceQuickSort::RenderWaterCustomSoftAlphaPSF();
}

void DxPieceManagerPSF::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF, float fLightMapDetail )
{
    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	// LightMap 셋팅이라면 LightMap 빠진 Piece 라도 OverLighting 되도록 Shader 로 렌더링 되도록 한다.
	if ( m_bUseLightMap )
	{
		NSVERTEXNORTEX_FX::SetEnable();
	}

	pd3dDevice->LightEnable( 3, FALSE );
	pd3dDevice->LightEnable( 4, FALSE );
	pd3dDevice->LightEnable( 5, FALSE );
	pd3dDevice->LightEnable( 6, FALSE );
	pd3dDevice->LightEnable( 7, FALSE );

	D3DMATERIALQ sMaterial;
	D3DUtil_InitMaterial( sMaterial, 1.f, 1.f, 1.0f, 1.f );
	switch( DxMeshes::g_emMeshMeterial )
	{
	case EM_MESH_MATERIAL_RED:
		sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
		sMaterial.Ambient.g = sMaterial.Diffuse.g = DxMeshes::g_fAlpha;
		sMaterial.Ambient.b = sMaterial.Diffuse.b = DxMeshes::g_fAlpha;
		pd3dDevice->SetMaterial( &sMaterial );
		break;
	};

	//MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	//for( ; iter!=m_mapNamePiece.end(); ++iter )
	//{
	//	//if ( (*iter).second->IsUserLightMapUV() )
	//	//{
	//	//	NSLightMapFX::LightMapDetail( 1.f );
	//	//}
	//	//else
	//	//{
	//	//	NSLightMapFX::LightMapDetail( fLightMapDetail );
	//	//}

	//	(*iter).second->RenderAlpha( pd3dDevice, bPSF );
	//}

	NSPieceQuickSort::RenderEff_PSF( pd3dDevice, NULL, NULL, FALSE, TRUE );

	//iter = m_mapNamePiece.begin();
	//for( ; iter!=m_mapNamePiece.end(); ++iter )
	//{
	//	(*iter).second->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bPSF );
	//}

	switch( DxMeshes::g_emMeshMeterial )
	{
	case EM_MESH_MATERIAL_RED:
		sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
		sMaterial.Ambient.g = sMaterial.Diffuse.g = 1.f;
		sMaterial.Ambient.b = sMaterial.Diffuse.b = 1.f;
		pd3dDevice->SetMaterial( &sMaterial );
		break;
	};

    // LightMap 렌더 시 위치좌표를 맞추기 위함.
	if ( m_bUseLightMap )
    {
        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity( &matIdentity );
        NSLightMapFX::SetWorld( matIdentity, 1.f );
        NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
		NSVERTEXNORTEX_FX::SetDisable();
    }
}

void DxPieceManagerPSF::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF, BOOL bFrameMove )
{
 //   if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
	//	return;

	//// LightMap 셋팅이라면 LightMap 빠진 Piece 라도 OverLighting 되도록 Shader 로 렌더링 되도록 한다.
	//if ( m_bUseLightMap )
	//{
	//	NSVERTEXNORTEX_FX::SetEnable();
	//}

	//NSPieceQuickSort::RenderPickAlpha_PSF( pd3dDevice );
	////CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume();
	////NSPieceQuickSort::RenderPickAlpha
	////( 
	////	pd3dDevice, 
	////	sCV, 
	////	pStaticMeshLightMap, 
	////	pFrameMesh, 
	////	bPSF, 
	////	FALSE, 
	////	DxSurfaceTex::GetInstance().m_pColorSuf_1st,
	////	DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
	////	DxSurfaceTex::GetInstance().m_pColorSuf_3rd,
	////	DxSurfaceTex::GetInstance().m_pColorBuffer,
	////	FALSE,
	////	0.f,
	////	bFrameMove
	////);

 //   // LightMap 렌더 시 위치좌표를 맞추기 위함.
	//if ( m_bUseLightMap )
 //   {
 //       D3DXMATRIX matIdentity;
 //       D3DXMatrixIdentity( &matIdentity );
 //       NSLightMapFX::SetWorld( matIdentity, 1.f );
 //       NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
 //   }
}

void DxPieceManagerPSF::RenderForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	NSPieceQuickSort::RenderForLodBlend_PSF( pd3dDevice );
}

void DxPieceManagerPSF::RenderSoftAlphaForLodBlend( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return;

	NSPieceQuickSort::RenderSoftAlphaForLodBlend_PSF( pd3dDevice );
}

//void DxPieceManagerPSF::Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
//{
//    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
//		return;
//
//	if ( m_bUseLightMap )
//	{
//		NSVERTEXNORTEX_FX::SetEnable();
//	}
//
//	D3DMATERIALQ sMaterial;
//	D3DUtil_InitMaterial( sMaterial, 1.f, 1.f, 1.0f, 1.f );
//	switch( DxMeshes::g_emMeshMeterial )
//	{
//	case EM_MESH_MATERIAL_RED:
//		sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
//		sMaterial.Ambient.g = sMaterial.Diffuse.g = DxMeshes::g_fAlpha;
//		sMaterial.Ambient.b = sMaterial.Diffuse.b = DxMeshes::g_fAlpha;
//		pd3dDevice->SetMaterial( &sMaterial );
//		break;
//	};
//
//	// Note : View Volume을 변환 시킨다.
//	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
//	D3DXVECTOR3&	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
//
//	D3DXVECTOR3	vUpVec ( 0.f, 1.f, 0.f );
//	D3DXVECTOR3	vTempFromPt		= vFromPt;
//	D3DXVECTOR3	vTempLookatPt	= vLookatPt;
//
//	vTempFromPt.y = vTempFromPt.y - (DxEnvironment::GetInstance().GetCenter().y*2.f);
//	vTempLookatPt.y = vTempLookatPt.y - (DxEnvironment::GetInstance().GetCenter().y*2.f);
//
//	CLIPVOLUME sCV = DxViewPort::GetInstance().ComputeClipVolume( vTempLookatPt, vTempFromPt, vUpVec, 1.f, FALSE );
//
//	//MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	//for( ; iter!=m_mapNamePiece.end(); ++iter )
//	//{
//	//	(*iter).second->Render_Reflect( pd3dDevice, sCV, bPSF );
//	//}
//
//	switch( DxMeshes::g_emMeshMeterial )
//	{
//	case EM_MESH_MATERIAL_RED:
//		sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
//		sMaterial.Ambient.g = sMaterial.Diffuse.g = 1.f;
//		sMaterial.Ambient.b = sMaterial.Diffuse.b = 1.f;
//		pd3dDevice->SetMaterial( &sMaterial );
//		break;
//	};
//
//    // LightMap 렌더 시 위치좌표를 맞추기 위함.
//	if ( m_bUseLightMap )
//    {
//        D3DXMATRIX matIdentity;
//        D3DXMatrixIdentity( &matIdentity );
//        NSLightMapFX::SetWorld( matIdentity, 1.f );
//        NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
//    }
//}

HRESULT DxPieceManagerPSF::DrawWireFrame ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV )        //	editer 용. 
{
    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
		return S_OK;

 //   MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	//for( ; iter!=m_mapNamePiece.end(); ++iter )
	//{
	//	(*iter).second->Render( pd3dDevice, m_pPiecePick, *pCV, m_fTime, m_fElapsedTime, TRUE );
 //       (*iter).second->RenderAlpha( pd3dDevice, TRUE );
	//}

    return S_OK;
}

BOOL DxPieceManagerPSF::IsGetName( DWORD nNUM, LPCSTR& pPieceName, LPCSTR& pFrameName )
{
	if( nNUM >= m_mapChangePis.size()+m_vecPointPis.size() )
        return FALSE;

	if( m_vecPointPis.size() > nNUM )
	{
		VECPOINTPIS_ITER iter = m_vecPointPis.begin();
		for( DWORD i=0; i<nNUM; ++i )		++iter;

		pPieceName = (*iter)->m_pPisName;
		pFrameName = NULL;

		return TRUE;
	}
	else
	{
		MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
		for( DWORD i=(DWORD)m_vecPointPis.size(); i<nNUM; ++i )	++iter;

		pPieceName = (*iter).second->m_pPisName;
		pFrameName = (*iter).first.c_str();

		return TRUE;
	}
}

// m_mapChangePis 에 관련된 일만 한다.
void DxPieceManagerPSF::SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh, const char* szFrameName, const char* szPisName )
{
	if( !szPisName )	return;

	DxFrame* pFrame = pFrameMesh->FindFrame( szFrameName );
	if( !pFrame )		return;

	// Note : Edit를 위한 구조체에 데이터 삽입
	MAPCHANGEPIS_ITER iter = m_mapChangePis.find( szFrameName );
	if( iter!=m_mapChangePis.end() )
	{
		// Note : 변환 Pis가 없거나 이름이 똑같다. ㅡㅡ;
		if( !strcmp( (*iter).second->m_pPisName, szPisName ) )	return;

		// Note : PieceContainer의 Count를 줄인다.
		DxPieceContainer::GetInstancePSF().ReleasePiece( (*iter).second->m_pPisName );
		
		// Note : Piece File 이름을 바꾼다.
		size_t nCount = strlen( szPisName ) + 1;
		SAFE_DELETE_ARRAY( (*iter).second->m_pPisName );
		(*iter).second->m_pPisName = new char[ nCount ];
		StringCchCopy( (*iter).second->m_pPisName, nCount, szPisName );

		// PieceEdit 로드 후 Piece에 셋팅
		DxPieceEdit*	pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, szPisName );
		(*iter).second->m_pPiece->Import( pd3dDevice, pPieceEdit, (*iter).second->m_matFrame, pFrame->matCombined, DEFAULT_VISIBLE_RADIUS, DEFAULT_INVISIBLE_RADIUS, FALSE );
	}
	else
	{
		MATRIXPISNAME* pNew = new MATRIXPISNAME;
		D3DXMatrixIdentity( &pNew->m_matFrame );

		size_t nCount = strlen( szPisName ) + 1;
		SAFE_DELETE_ARRAY( pNew->m_pPisName );
		pNew->m_pPisName = new char[nCount];
		StringCchCopy( pNew->m_pPisName, nCount, szPisName );

		// PieceEdit 로드 후 Piece에 셋팅
		DxPieceEdit*	pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, szPisName );
		pNew->m_pPiece = new DxPiece(pd3dDevice);
		pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matFrame, pFrame->matCombined, DEFAULT_VISIBLE_RADIUS, DEFAULT_INVISIBLE_RADIUS, FALSE );

		m_mapChangePis.insert( std::make_pair( szFrameName, pNew ) );
	}

	MakeTree();

	// Note : Frame이 Piece로 사용한다고 전한다.
	pFrameMesh->AddPiece2( szFrameName );
}

void DxPieceManagerPSF::DeletePiece( DxFrameMesh* pFrameMesh, const char* szFrameName )
{
	// Note : 데이터 삭제
	MAPCHANGEPIS_ITER iter_DEL;
	MAPCHANGEPIS_ITER iter = m_mapChangePis.find( szFrameName );
	if( iter!=m_mapChangePis.end() )
	{
		// Release
		DxPieceContainer::GetInstancePSF().ReleasePiece( (*iter).second->m_pPisName );

		SAFE_DELETE( (*iter).second );

		m_mapChangePis.erase( iter );
	}

	MakeTree();

	// Note : Frame에 Piece로 사용이 해제 되었다고 전한다.
	pFrameMesh->DelPiece2( szFrameName );
}

BOOL DxPieceManagerPSF::SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, const char* szPisName, const D3DXMATRIX& matWorld, DxSetLandMan* pSetLandMan, BOOL bSlope, float fSlopeRate )
{
	BOOL bResult = SetPiece( pd3dDevice, szPisName, matWorld );
	if ( !bResult )
		return FALSE;

	if ( !bSlope )
		return TRUE;

	m_vecPointPis[m_vecPointPis.size()-1]->m_pPiece->CalculateSlopeMatrix( pSetLandMan, fSlopeRate );

	MakeTree();

	return TRUE;
}

// m_vecPointPis에 관련된 일만 한다.
BOOL DxPieceManagerPSF::SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, const char* szPisName, const D3DXMATRIX& matWorld )
{
	if( !szPisName )	
        return FALSE;

    if( _tcslen(szPisName)==0 )	
        return FALSE;

	if ( !m_mapChangePis.empty() && sc::string::getFileExt( szPisName ) == _T("pie15") )
	{
		AfxMessageBox( _T("ERROR - pis, pie15 가 동시에 쓰이고 있습니다.") );
		return FALSE;
	}

	for ( DWORD i=0; i<m_vecPointPis.size(); ++i )
	{
		if ( sc::string::getFileExt( szPisName ) != sc::string::getFileExt( m_vecPointPis[i]->m_pPisName ) )
		{
			AfxMessageBox( _T("ERROR - pie, pie15 가 동시에 쓰이고 있습니다.") );
			return FALSE;
		}
	}

	MATRIXPISNAME* pNew = new MATRIXPISNAME;

	size_t nCount = strlen( szPisName ) + 1;
	SAFE_DELETE_ARRAY( pNew->m_pPisName );
	pNew->m_pPisName = new char[nCount];
	StringCchCopy( pNew->m_pPisName, nCount, szPisName );

	// PieceEdit 로드 후 Piece에 셋팅
	DxPieceEdit*	pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, szPisName );
	pNew->m_pPiece = new DxPiece(pd3dDevice);
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, matWorld, matIdentity, pPieceEdit->GetVisibleRadius(), pPieceEdit->GetInvisibleRadius(), FALSE );

	// vector 에 삽입을 해 놓는다.
	m_vecPointPis.push_back( pNew );

	MakeTree();

    return TRUE;
}

void DxPieceManagerPSF::DeletePiece( DWORD dwNumber )
{
	VECPOINTPIS_ITER iter = m_vecPointPis.begin();
	for( DWORD i=0; i<dwNumber; ++i )	++iter;

	MATRIXPISNAME* pDelete = (*iter);
	SAFE_DELETE( pDelete );

	m_vecPointPis.erase( iter );

	MakeTree();
}

void DxPieceManagerPSF::MakeTree()
{
	// Note : Tree 를 삭제. List는 참조한 것이므로 삭제하면 안됨.
	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->SetPiece_NULL();
	}
	std::for_each( m_mapNamePiece.begin(), m_mapNamePiece.end(), std_afunc::DeleteMapObject() );
	m_mapNamePiece.clear();

	// Note : All Setting
	MAPCHANGEPIS_ITER iter_base = m_mapChangePis.begin();
	for( ; iter_base!=m_mapChangePis.end(); ++iter_base )
	{
		// Note : 없다면 작업 취소
		if( !(*iter_base).second->m_pPiece )	continue;

		iter = m_mapNamePiece.find( (*iter_base).second->m_pPisName );
		if( iter!=m_mapNamePiece.end() )
		{
			(*iter).second->SetPiece( (*iter_base).second->m_pPiece );
		}
		else
		{
			// Note : 새로운 넘이다.
			DxPieceAABB* pNew = new DxPieceAABB;

			pNew->SetPiece( (*iter_base).second->m_pPiece );

			m_mapNamePiece.insert( std::make_pair( (*iter_base).second->m_pPisName, pNew ) );
		}
	}
	VECPOINTPIS_ITER iter_point = m_vecPointPis.begin();
	for( ; iter_point!=m_vecPointPis.end(); ++iter_point )
	{
		// Note : 없다면 작업 취소
		if( !(*iter_point)->m_pPiece )	continue;

		iter = m_mapNamePiece.find( (*iter_point)->m_pPisName );
		if( iter!=m_mapNamePiece.end() )
		{
			(*iter).second->SetPiece( (*iter_point)->m_pPiece );
		}
		else
		{
			// Note : 새로운 넘이다.
			DxPieceAABB* pNew = new DxPieceAABB;

			pNew->SetPiece( (*iter_point)->m_pPiece );

			m_mapNamePiece.insert( std::make_pair( (*iter_point)->m_pPisName, pNew ) );
		}
	}

	// Note : Create AABB Tree
	iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->MakeTree();
	}
}

// Etc
D3DXMATRIX* DxPieceManagerPSF::MoveCamera( DxFrame* pFrameRoot, const char* szFrameName )
{
	MAPCHANGEPIS_ITER iter = m_mapChangePis.find( szFrameName );
	if( iter!=m_mapChangePis.end() )
	{
		if ( !(*iter).second->m_pPiece )
			return NULL;

		D3DXVECTOR3 vPos;
		vPos.x = (*iter).second->m_pPiece->m_matWorld._41;
		vPos.y = (*iter).second->m_pPiece->m_matWorld._42;
		vPos.z = (*iter).second->m_pPiece->m_matWorld._43;

		// Note : 카메라를 이동해야지.
		DxViewPort::GetInstance().CameraJump( vPos );

		return &(*iter).second->m_pPiece->m_matWorld;
	}
	
	return NULL;
}

D3DXMATRIX* DxPieceManagerPSF::MoveCamera( DWORD dwNumber )
{
	VECPOINTPIS_ITER iter = m_vecPointPis.begin();
	for( DWORD i=0; i<dwNumber; ++i )	
		++iter;

	if ( !(*iter)->m_pPiece )
		return NULL;

	D3DXVECTOR3 vPos;
	vPos.x = (*iter)->m_pPiece->m_matWorld._41;
	vPos.y = (*iter)->m_pPiece->m_matWorld._42;
	vPos.z = (*iter)->m_pPiece->m_matWorld._43;

	// Note : 카메라를 이동해야지.
	DxViewPort::GetInstance().CameraJump( vPos );

	return &(*iter)->m_pPiece->m_matWorld;
}

DxPiece* DxPieceManagerPSF::GetPiece( DWORD dwNumber )
{
    VECPOINTPIS_ITER iter = m_vecPointPis.begin();
    for( DWORD i=0; i<dwNumber; ++i )	++iter;
    return (*iter)->m_pPiece;
}

DxPieceManagerPSF::MATRIXPISNAME* DxPieceManagerPSF::GetMATRIXPISNAME( DWORD dwNumber )
{
	VECPOINTPIS_ITER iter = m_vecPointPis.begin();
	for( DWORD i=0; i<dwNumber; ++i )	++iter;
	return (*iter);
}

DxPieceManagerPSF::MATRIXPISNAME* DxPieceManagerPSF::GetChangeMATRIXPISNAME( DWORD dwNumber )
{
	MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();

	std::advance(iter, dwNumber);

	if (iter != m_mapChangePis.end())
	{
		return iter->second;
	}

	return 0;
}

BOOL DxPieceManagerPSF::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision ) const
{
	BOOL bCollision(FALSE);

	MAPNAMEPIECE_CITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine( vStart, vEnd, NULL, vCollision, EMCC_CULL_CCW, FALSE ) )	bCollision = TRUE;
	}

	return bCollision;
}

BOOL DxPieceManagerPSF::IsCollisionLine2( const D3DXVECTOR3& vStart, 
										D3DXVECTOR3& vEnd, 
										D3DXVECTOR3& vNor,
										D3DXVECTOR3& vCollision, 
										DxPiece** rrPiece,
                                        EMCOLLMODE emCullMode ) const
{
	*rrPiece = NULL;
	BOOL bCollision(FALSE);
	MAPNAMEPIECE_CITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLine2( vStart, vEnd, &vNor, vCollision, rrPiece, emCullMode, FALSE ) )
			bCollision = TRUE;
	}

	return bCollision;
}

// Note : 충돌이 되면 바로 리턴한다.
BOOL DxPieceManagerPSF::IsCollisionLinePixel( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode, int nIndexUV )
{
    MAPNAMEPIECE_CITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		if( (*iter).second->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, emCullMode, nIndexUV ) )
            return TRUE;
	}
    return FALSE;
}

int DxPieceManagerPSF::GetIndex( DxPiece* pPiece )
{
	int nCount(0);

    // m_vecPointPis
    {
	    VECPOINTPIS_ITER iter = m_vecPointPis.begin();
	    for( ; iter!=m_vecPointPis.end(); ++iter, ++nCount )
	    {
		    if( (*iter)->m_pPiece == pPiece )
		    {
			    return nCount;
		    }
	    }
    }

    // m_mapChangePis
    {
        MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
	    for( ; iter!=m_mapChangePis.end(); ++iter, ++nCount )
	    {
		    if( (*iter).second->m_pPiece == pPiece )
		    {
			    return nCount;
		    }
	    }
    }

	return -1;
}

void DxPieceManagerPSF::ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
    MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->ReloadTexEffVMFX( pd3dDevice );
	}
}

void DxPieceManagerPSF::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
{
    MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->CreateAlphaTexture32Bit( pd3dDevice );
	}
}

void DxPieceManagerPSF::Clone_MouseShift( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX& pMatrix )
{
	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		if( (*iter).second->Clone_MouseShift( pMatrix ) )
		{
			SetPiece( pd3dDevice, (*iter).first.c_str(), *pMatrix );

			MATRIXPISNAME* pMatrixPisName = m_vecPointPis[ m_vecPointPis.size()-1 ];

			// Note : DxObjectMRS 커서를 바꿔준다. < 중요 >
			pMatrix = &pMatrixPisName->m_pPiece->m_matWorld;

			return;
		}
	}
}

void DxPieceManagerPSF::SavePSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh )
{
	//D3DXMATRIX matIdentity;
	//D3DXMATRIX* pmatPisEdit;
	//D3DXMatrixIdentity( &matIdentity );

	//DxFrame*		pFrame(NULL);
	//DxPieceEdit*	pPieceEdit(NULL);

	SFile << VERSION_PSF;

	SFile.BeginBlock();
	{
		DWORD dwSize = (DWORD)m_vecPointPis.size();
		SFile << dwSize;

		VECPOINTPIS_ITER iter = m_vecPointPis.begin();
		for( ; iter!=m_vecPointPis.end(); ++iter )
		{
			//pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, (*iter)->m_pPisName );
			//{
			//	if( pPieceEdit )	pmatPisEdit = pPieceEdit->GetMatrixLocal();
			//	else				pmatPisEdit = &matIdentity;

			(*iter)->Save( SFile );	//, *pmatPisEdit, matIdentity );
			//}
			//DxPieceContainer::GetInstancePSF().ReleasePiece( (*iter)->m_pPisName );
		}
	}
	SFile.EndBlock();
}

void DxPieceManagerPSF::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh )
{
	CleanUp();

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( 0x102 == dwVer )
	{
		DWORD dwSize = 0;
		{
			SFile >> dwSize;

			for( DWORD i=0; i<dwSize; ++i )
			{
				MATRIXPISNAME* pNew = new MATRIXPISNAME;
				pNew->Load( pd3dDevice, SFile );
				m_vecPointPis.push_back( pNew );
			}
		}
	}
	else if ( 0x101 == dwVer )
	{
		DWORD dwSize = 0;
		{
			SFile >> dwSize;

			for( DWORD i=0; i<dwSize; ++i )
			{
				MATRIXPISNAME* pNew = new MATRIXPISNAME;
				pNew->Load_Prev( SFile );
				m_vecPointPis.push_back( pNew );

				DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, pNew->m_pPisName );
				if( !pPieceEdit )
				{
					std::string strName = "조각파일 : ";
					strName += pNew->m_pPisName;
					strName += " 이 없습니다.";
					MessageBox( NULL, strName.c_str(), "ERROR", MB_OK );
					continue;
				}

				pNew->m_pPiece = new DxPiece(pd3dDevice);
				pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matFrame, matIdentity, DEFAULT_VISIBLE_RADIUS, DEFAULT_INVISIBLE_RADIUS, FALSE );
			}
		}
		{
			SFile >> dwSize;

			std::string szFrameName;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> szFrameName;

				// Note : szFrameName이 있는지 없는지를 검사한다. 
				//			만약 없다면 로딩만 한 후 건너띤다.
				DxFrame* pFrame = pFrameMesh->FindFrame( szFrameName.c_str() );
				if( !pFrame )
				{
					MATRIXPISNAME sNew;
					sNew.Load_Prev( SFile );
					continue;
				}

				MATRIXPISNAME* pNew = new MATRIXPISNAME;
				pNew->Load_Prev( SFile );
				m_mapChangePis.insert( std::make_pair( szFrameName, pNew ) );

				DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, pNew->m_pPisName );
				if( !pPieceEdit )
				{
					std::string strName = "조각파일 : ";
					strName += pNew->m_pPisName;
					strName += " 이 없습니다.";
					MessageBox( NULL, strName.c_str(), "ERROR", MB_OK );
					continue;
				}

				pNew->m_pPiece = new DxPiece(pd3dDevice);
				pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matFrame, pFrame->matCombined, DEFAULT_VISIBLE_RADIUS, DEFAULT_INVISIBLE_RADIUS, FALSE );
			}
		}
	}

	// 이름순으로 sort 되도록 한다.
	SortPointPis();

	MakeTree();

	// Prefab 일 경우, pFrameMesh 는 NULL 값이 넘어온다.
	if ( pFrameMesh )
	{
		// Note : 깔끔하게 지운다.
		pFrameMesh->SetPieceUseFALSE();

		// Note : Src Frame에 Piece를 사용하고 있다고 알린다.
		MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
		for( ; iter!=m_mapChangePis.end(); ++iter )
		{
			pFrameMesh->AddPiece2( (*iter).first.c_str() );
		}
	}
}

void DxPieceManagerPSF::LoadPSF_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh )
{
	CleanUp();

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	DWORD dwSize = 0;
	{
		SFile >> dwSize;

		for( DWORD i=0; i<dwSize; ++i )
		{
			MATRIXPISNAME* pNew = new MATRIXPISNAME;
			pNew->Load_Prev( SFile );
			m_vecPointPis.push_back( pNew );

			DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, pNew->m_pPisName );
			if( !pPieceEdit )
			{
				std::string strName = "조각파일 : ";
				strName += pNew->m_pPisName;
				strName += " 이 없습니다.";
				MessageBox( NULL, strName.c_str(), "ERROR", MB_OK );
				continue;
			}

			pNew->m_pPiece = new DxPiece(pd3dDevice);
			pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matFrame, matIdentity, DEFAULT_VISIBLE_RADIUS, DEFAULT_INVISIBLE_RADIUS, FALSE );
		}
	}
	{
		SFile >> dwSize;

		std::string szFrameName;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> szFrameName;

			// Note : szFrameName이 있는지 없는지를 검사한다. 
			//			만약 없다면 로딩만 한 후 건너띤다.
			DxFrame* pFrame = pFrameMesh->FindFrame( szFrameName.c_str() );
			if( !pFrame )
			{
				MATRIXPISNAME sNew;
				sNew.Load_Prev( SFile );
				continue;
			}

			MATRIXPISNAME* pNew = new MATRIXPISNAME;
			pNew->Load_Prev( SFile );
			pNew->m_matFrame = matIdentity;
			m_mapChangePis.insert( std::make_pair( szFrameName, pNew ) );

			DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstancePSF().LoadPiece( pd3dDevice, pNew->m_pPisName );
			if( !pPieceEdit )
			{
				std::string strName = "조각파일 : ";
				strName += pNew->m_pPisName;
				strName += " 이 없습니다.";
				MessageBox( NULL, strName.c_str(), "ERROR", MB_OK );
				continue;
			}

			pNew->m_pPiece = new DxPiece(pd3dDevice);
			pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matFrame, pFrame->matCombined, DEFAULT_VISIBLE_RADIUS, DEFAULT_INVISIBLE_RADIUS, FALSE );
		}
	}

	// 이름순으로 sort 되도록 한다.
	SortPointPis();

	MakeTree();

	// Note : 깔끔하게 지운다.
	pFrameMesh->SetPieceUseFALSE();

	// Note : Src Frame에 Piece를 사용하고 있다고 알린다.
	MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
	for( ; iter!=m_mapChangePis.end(); ++iter )
	{
		pFrameMesh->AddPiece2( (*iter).first.c_str() );
	}
}

void DxPieceManagerPSF::Archive( boost::function<void (const TCHAR*)> fun )
{
    BOOST_FOREACH(MAPCHANGEPIS_VALUE& it, m_mapChangePis)
    {
        fun( (TSTRING(DxPieceContainer::GetInstancePSF().GetPath()) + it.second->m_pPisName).c_str() );
    }

    BOOST_FOREACH(VECPOINTPIS::value_type& it, m_vecPointPis)
    {
        fun( (TSTRING(DxPieceContainer::GetInstancePSF().GetPath()) + it->m_pPisName).c_str() );
    }
}

// Radiosity
void DxPieceManagerPSF::CheckPieceForRadiosity( VEC_TSTRING& vecErrorPiece )
{
	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		if ( !(*iter).second->IsSupportRAD() )
		{
			vecErrorPiece.push_back( (*iter).first );
		}
	}
}

// Prefab
void DxPieceManagerPSF::MakeTree( const D3DXMATRIX& matWorld )
{
	// Note : Create AABB Tree
	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->MakeTree( matWorld );
	}
}

void DxPieceManagerPSF::SetPrefabMatrix( const D3DXMATRIX& matPrefab )
{
	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->SetPrefabMatrix( matPrefab );
	}
}

void DxPieceManagerPSF::Import( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceManagerPSF* pSrc, DxPrefabMan* pPrefabMan )
{
	MAPNAMEPIECE_ITER iter = pSrc->m_mapNamePiece.begin();
	for( ; iter!=pSrc->m_mapNamePiece.end(); ++iter )
	{
		DxPieceAABB* pNew = new DxPieceAABB;
		pNew->CloneData( pd3dDevice, (*iter).second, g_fCELL_SIZE_PSF, FALSE );

		m_mapNamePiece.insert( std::make_pair( (*iter).first.c_str(), pNew ) );
	}

	pPrefabMan->ExportMAPNAMEPIECE( pd3dDevice, m_mapNamePiece );

	// Note : Create AABB Tree
	iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->MakeTree();
	}
}

void DxPieceManagerPSF::ExportMAPNAMEPIECE( LPDIRECT3DDEVICEQ pd3dDevice, MAPNAMEPIECE& mapNamePiece )
{
	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->ExportMAPNAMEPIECE( pd3dDevice, mapNamePiece, g_fCELL_SIZE_PSF );
	}
}

void DxPieceManagerPSF::ChangeNamePIE15()
{
	for ( DWORD i=0; i<m_vecPointPis.size(); ++i )
	{
		TSTRING strName = m_vecPointPis[i]->m_pPisName;

		// pie15는 작업할 필요가 없습니다.
		if ( sc::string::getFileExt( strName.c_str() ) == _T("pie15") )
		{
			continue;
		}

		// 이름 변경.
		strName += _T("15");
		size_t nCount = strName.size() + 1;
		SAFE_DELETE_ARRAY( m_vecPointPis[i]->m_pPisName );
		m_vecPointPis[i]->m_pPisName = new char[nCount];
		StringCchCopy( m_vecPointPis[i]->m_pPisName, nCount, strName.c_str() );
	}
}

void DxPieceManagerPSF::ResetAnimationTime_DisableLoopType()
{
	MAPNAMEPIECE_CITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->ResetAnimationTime_DisableLoopType();
	}
}

void DxPieceManagerPSF::SortPointPis()
{
	typedef std::map<TSTRING,VECPOINTPIS>	MAPVECPOINTPIS;
	typedef MAPVECPOINTPIS::iterator		MAPVECPOINTPIS_ITER;
	MAPVECPOINTPIS	mapPointPis;

	for ( DWORD i=0; i<m_vecPointPis.size(); ++i )
	{
		MAPVECPOINTPIS_ITER iter = mapPointPis.find( m_vecPointPis[i]->m_pPisName );
		if ( iter == mapPointPis.end() )
		{
			VECPOINTPIS vecPoint;
			vecPoint.push_back( m_vecPointPis[i] );
			mapPointPis.insert( std::make_pair(m_vecPointPis[i]->m_pPisName,vecPoint) );
		}
		else
		{
			(*iter).second.push_back( m_vecPointPis[i] );
		}
	}

	m_vecPointPis.clear();

	MAPVECPOINTPIS_ITER iter = mapPointPis.begin();
	for ( ; iter!=mapPointPis.end(); ++iter )
	{
		for ( DWORD i=0; i<(*iter).second.size(); ++i )
		{
			m_vecPointPis.push_back( (*iter).second[i] );
		}
	}

	mapPointPis.clear();
}

// Piece의 TriangleLinkLight 를 생성하도록 한다.
void DxPieceManagerPSF::CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
												DxStaticMeshCollBase* pStaticMesh,
												SET_DWORD& setEraseLightID,
												BOOL bDirectionLight )
{
	// 닿는 Light는 다 연결시켜준다.
	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->CreateTriangleLinkLight( pd3dDevice, pStaticMesh, this, bDirectionLight );
	}

	// 2개가 초과되면 그 이상은 삭제시킨다.
	iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		// 문제되는 녀석을 찾는다.
		DWORD dwLightID=0;
		if ( (*iter).second->FindTriangleLimitLightNumOver3( dwLightID ) )
		{
			// Erease 되는 Light 등록
			setEraseLightID.insert( dwLightID );

			// 실제 Erease 시킨다.
			MAPNAMEPIECE_ITER iterE = m_mapNamePiece.begin();
			for( ; iterE!=m_mapNamePiece.end(); ++iterE )
			{
				(*iterE).second->EraseLightID( dwLightID );
			}
		}
	}
}

//  현재 라이트와 연관된 삼각형들의 특정 슬롯에 자리가 남는지 확인.
BOOL DxPieceManagerPSF::IsLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot ) const
{
	MAPNAMEPIECE_CITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		if ( !(*iter).second->IsLightID_InsertSlot( dwLightID, dwSlot ) )
			return FALSE;
	}

	return TRUE;
}

// 현재 라이트와 연관된 삼각형들의 특정 슬롯에 현재 LightID 를 셋팅.
void DxPieceManagerPSF::InsertSlotLightID( DWORD dwLightID, DWORD dwSlot )
{
	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
	for( ; iter!=m_mapNamePiece.end(); ++iter )
	{
		(*iter).second->InsertSlotLightID( dwLightID, dwSlot );
	}
}

// Triangle Link Light - Light가 3개 이상인 것 찾기 및 Light 갯수 2개로 제한
void DxPieceManagerPSF::EraseLightID4LightOver3( SET_DWORD& setEraseLightID )
{
	// Triangle Link Light - Light가 3개 이상인 것 찾기 및 Light 갯수 2개로 제한
	BOOL bLightOver3(TRUE);
	while ( bLightOver3 )
	{
		DWORD dwLightID(0);
		bLightOver3 = FALSE;

		// 실제 Erease 시킨다.
		MAPNAMEPIECE_ITER iterE = m_mapNamePiece.begin();
		for( ; iterE!=m_mapNamePiece.end(); ++iterE )
		{
			if ( (*iterE).second->FindTriangleLimitLightNumOver3( dwLightID ) )
			{
				// Erease 되는 Light 등록
				setEraseLightID.insert( dwLightID );

				// 특정 라이트를 줄여준다.
				EraseLightID( dwLightID );

				bLightOver3 = TRUE;
				break;
			}
		}
	}
}

// 특정 LightID 를 삭제하도록 한다.
void DxPieceManagerPSF::EraseLightID( DWORD dwLightID )
{
	// 실제 Erease 시킨다.
	MAPNAMEPIECE_ITER iterE = m_mapNamePiece.begin();
	for( ; iterE!=m_mapNamePiece.end(); ++iterE )
	{
		(*iterE).second->EraseLightID( dwLightID );
	}
}

// setEraseLightID 을 바탕으로 Piece 쪽 LightID도 삭제 하도록 한다.
void DxPieceManagerPSF::EraseLightID4TriangleLinkLight( const SET_DWORD& setEraseLightID )
{
	// 지워야될 LightID 들
	SET_DWORD_CITER citer = setEraseLightID.begin();
	for ( ; citer!=setEraseLightID.end(); ++citer )
	{
		// 실제 Erease 시킨다.
		EraseLightID( (*citer) );
	}
}

// Piece 의 PS_3_0의 렌더타입을 설정하도록 한다.
void DxPieceManagerPSF::SetType_PS_3_0()
{
	// 실제 Erease 시킨다.
	MAPNAMEPIECE_ITER iterE = m_mapNamePiece.begin();
	for( ; iterE!=m_mapNamePiece.end(); ++iterE )
	{
		(*iterE).second->SetType_PS_3_0();
	}
}