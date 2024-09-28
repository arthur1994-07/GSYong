#include "pch.h"

#include <algorithm>

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Util/Util.h"

#include "../../DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "../../DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../DxLand/LightMap/NSLightMap.h"
#include "../../DxLand/DxLandMan.h"
#include "../../Common/StlFunctions.h"
#include "../../Common/profile.h"
#include "../../DxCommon9/DXUTmisc.h"
#include "../../GlobalParam.h"

#include "./DxPiececontainer.h"
#include "./DxPieceEdit.h"
#include "./DxPiece.h"

#include "DxPieceManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


DxPieceManager::DxPieceManager()
{

}

DxPieceManager::~DxPieceManager()
{

}

//// -----------------------------------------------------------------------------------------------------------------------------------------
////											D	x		P	i	e	c	e		A	A	B	B
//// -----------------------------------------------------------------------------------------------------------------------------------------
//DxPieceAABB::DxPieceAABB() :
//	m_pPieceHead(NULL),
//	m_pPieceAABBTree(NULL),
//	m_dwPieceNUM(0L)
//{
//    m_pQuickSort = new DxPieceQuickSort;
//}
//
//DxPieceAABB::~DxPieceAABB()
//{
//	m_dwPieceNUM = 0L;
//	m_pPieceHead = NULL;
//    SAFE_DELETE( m_pQuickSort );
//	SAFE_DELETE( m_pPieceAABBTree );
//}
//
//void DxPieceAABB::ClearTree()
//{	
//	m_pPieceAABBTree->CleanUp(); 
//}
//
//void DxPieceAABB::ClearOnlyTree()
//{	
//	SAFE_DELETE( m_pPieceAABBTree ); 
//}
//
//void DxPieceAABB::SetPiece( DxPiece* pPiece )
//{
//	// List로 연결
//	pPiece->m_pNext = m_pPieceHead;
//	m_pPieceHead = pPiece;
//}
//
//void DxPieceAABB::FrameMove( const float fTime, const float fElapsedTime )
//{
//}
//
//void DxPieceAABB::Render( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pPiecePick, const CLIPVOLUME &sCV, const float fTime, const float fElapsedTime, BOOL bPSF )
//{
//	PROFILE_BEGIN("DxPieceAABB::Render");
//
//	float fDistance = DxViewPort::GetInstance().GetDistance();
//	fDistance = fDistance*fDistance;
//
//	// Note : Quick Sort 를 한다.		이넘을 저기서도 쓰려면... 데이터를 공유하면 안될 듯 하네.
//	m_pQuickSort->Reset();
//	NSPIECETREE::InsertQuickSort( pd3dDevice, m_pPieceAABBTree, m_pQuickSort, pPiecePick, sCV, 
//								DxViewPort::GetInstance().GetFromPt(), DxViewPort::GetInstance().GetLookatPt(), fDistance );
//	m_pQuickSort->QuickSortProcess();
//
//	m_pQuickSort->FrameMove( fTime, fElapsedTime );
//	m_pQuickSort->Render( pd3dDevice, bPSF );
//
//	PROFILE_END("DxPieceAABB::Render");
//}
//
//void DxPieceAABB::RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
//{
//	PROFILE_BEGIN("DxPieceAABB::RenderAlpha");
//	m_pQuickSort->RenderAlpha( pd3dDevice, bPSF );
//	PROFILE_END("DxPieceAABB::RenderAlpha");
//}
//
//void DxPieceAABB::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF )
//{
//	PROFILE_BEGIN("DxPieceAABB::RenderEff");
//	m_pQuickSort->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bPSF );
//	PROFILE_END("DxPieceAABB::RenderEff");
//}
//
//void DxPieceAABB::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF )
//{
//	PROFILE_BEGIN("DxPieceAABB::RenderPickAlpha");
//	m_pQuickSort->RenderPickAlpha( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bPSF );
//	PROFILE_END("DxPieceAABB::RenderPickAlpha");
//}
//
//void DxPieceAABB::Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bPSF )
//{
//	NSPIECETREE::Render_Reflect( pd3dDevice, m_pPieceAABBTree, sCV, bPSF );
//}
//
//void DxPieceAABB::MakeTree()
//{
//	// Note : 현재 조각들의 AABB Box를 구한다.
//	DxPiece* pCur = m_pPieceHead;
//	while( pCur )
//	{
//		pCur->SetAABBBox();
//		pCur = pCur->m_pNext;
//	}
//
//	// Note : 현재 조각파일의 전체 갯수
//	DWORD dwCount = 0;
//	pCur = m_pPieceHead;
//	while( pCur )
//	{
//		++dwCount;
//		pCur = pCur->m_pNext;
//	}
//
//	m_dwPieceNUM = dwCount;
//
//	// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
//	m_pQuickSort->SetArrayMaxSize( dwCount );
//
//	// Note : Tree 를 만든다.
//	SAFE_DELETE( m_pPieceAABBTree );
//	NSPIECETREE::MakeTree( m_pPieceAABBTree, m_pPieceHead );
//}
//
//void DxPieceAABB::CreateLightMapPiece( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, BOOL bRan_1_0 )
//{
//    NSPIECETREE::CreateLightMapPiece( pd3dDevice, m_pPieceAABBTree, strLightMapName );
//
//	// 공유되는것을 작업하기 때문에 하나만 건들도록 한다.
//	DxPiece* pCur = m_pPieceHead;
//	if ( pCur )
//	{
//		pCur->CreateLightMapPieceObject( pd3dDevice, strLightMapName, bRan_1_0 );
//	}
//}
//
//void DxPieceAABB::LoadLightMap( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//    NSPIECETREE::LoadLightMap( pd3dDevice, m_pPieceAABBTree );
//}
//
//void DxPieceAABB::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//    DxPiece* pCur = m_pPieceHead;
//	while( pCur )
//	{
//		pCur->CreateAlphaTexture32Bit( pd3dDevice );
//		pCur = pCur->m_pNext;
//	}
//}
//
//BOOL DxPieceAABB::IsUserLightMapUV()	// User가 만든 LightMapUV를 사용하는가~?
//{
//	DxPiece* pCur = m_pPieceHead;
//	if ( pCur )
//	{
//		if ( !(pCur->m_dwFlags & EMPO_UNSUPPORT_RAD) )
//			return TRUE;
//	}
//
//	return FALSE;
//}
//
//BOOL DxPieceAABB::Clone_MouseShift( LPD3DXMATRIX& pMatrix )
//{
//	DxPiece* pCur = m_pPieceHead;
//	while( pCur )
//	{
//		DWORD_PTR dwBuffer = (DWORD_PTR) &pCur->m_matWorld;
//		if( dwBuffer == (DWORD_PTR) pMatrix )
//		{
//			return TRUE;
//		}
//		pCur = pCur->m_pNext;
//	}
//
//	return FALSE;
//}
//
//void DxPieceAABB::Save( sc::SerialFile& SFile, const char* pPieceName )
//{
//    SFile << static_cast<DWORD>( VERSION );
//
//    SFile.BeginBlock();
//    {
//	    SFile << m_dwPieceNUM;
//
//        //----
//        BOOL bLightMapPiece(FALSE);
//        DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().FindPiece( pPieceName );
//        if ( pPieceEdit )
//        {
//            if ( pPieceEdit->IsLightMapPiece() )
//            {
//                bLightMapPiece = TRUE;
//            }
//        }
//
//        //----
//        if ( bLightMapPiece )
//        {
//            SFile << static_cast<BOOL>( bLightMapPiece );
//            SFile.BeginBlock();
//            {
//                pPieceEdit->SaveLightMapPiece( SFile );
//            }
//            SFile.EndBlock();
//        }
//        else
//        {
//            SFile << static_cast<BOOL>( bLightMapPiece );
//        }
//
//        //----
//	    NSPIECETREE::Save( m_pPieceAABBTree, SFile );
//    }
//    SFile.EndBlock();
//}
//
//void DxPieceAABB::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const char* pPieceName )
//{
//    DWORD dwVer, dwBufferSize;
//    SFile >> dwVer;
//    SFile >> dwBufferSize;
//
//	SFile >> m_dwPieceNUM;
//
//	// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
//	m_pQuickSort->SetArrayMaxSize( m_dwPieceNUM );
//
//	DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, pPieceName );
//
//    BOOL bLightMapPiece(FALSE);
//    SFile >> bLightMapPiece;
//    if ( bLightMapPiece )
//    {
//        DWORD dwBufferSize_2;
//        SFile >> dwBufferSize_2;
//        if ( pPieceEdit )
//        {
//            pPieceEdit->LoadLightMapPiece( pd3dDevice, SFile );
//        }
//        else
//        {
//            SFile.SetOffSet( SFile.GetfTell() + dwBufferSize_2 );
//        }
//    }
//
//	NSPIECETREE::Load( pd3dDevice, m_pPieceAABBTree, SFile, pPieceEdit );
//}
//
//void DxPieceAABB::Load_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, const char* pPieceName )
//{
//	SFile >> m_dwPieceNUM;
//
//	// Note : 전체 조각들이 모두 사용 할수 있도록 최대 배열을 잡을수 있게 해 준다.
//	m_pQuickSort->SetArrayMaxSize( m_dwPieceNUM );
//
//	DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, pPieceName );
//
//	NSPIECETREE::Load_PREV( pd3dDevice, m_pPieceAABBTree, SFile, pPieceEdit );
//}
//
//void DxPieceAABB::CloneData( LPDIRECT3DDEVICEQ pd3dDevice, 
//                            DxPieceAABB* pSrc, 
//                            const DxPieceEdit* pPieceEdit,
//                            BOOL bLightMap, 
//                            const TSTRING& strLightMapName, 
//                            DxStaticMesh* pStaticMesh, 
//                            DxPieceManager* pPieceManager,
//                            DWORD& dwStartTextureSize,
//                            float fLightMapDetail )
//{
//	SAFE_DELETE( m_pPieceAABBTree );
//	SAFE_DELETE( m_pPieceHead );
//
//	DxPiece* pCur = pSrc->m_pPieceHead;
//
//    DWORD dwCountMAX(0);
//    while( pCur )
//	{
//        ++dwCountMAX;
//		pCur = pCur->m_pNext;
//	}
//
//    int nCount(1);
//    pCur = pSrc->m_pPieceHead;
//	while( pCur )
//	{
//        // 기록
//        {
//            TSTRINGSTREAM strStringStream;
//            strStringStream << "Piece LightMap - A - ";
//            strStringStream << nCount++;
//            strStringStream << "/";
//            strStringStream << dwCountMAX;
//            ExportProgress::SetStateString( strStringStream.str().c_str() );
//        }
//
//		// fLightMapDetail 는 여기에서 부터 참조로 넘기도록 한다.
//		DxPiece* pNew = new DxPiece;
//		pNew->CloneData( pd3dDevice, pPieceEdit, pCur, bLightMap, strLightMapName, pStaticMesh, pPieceManager, dwStartTextureSize, fLightMapDetail );
//
//		pCur = pCur->m_pNext;
//
//		// 이제 연결한다.
//		pNew->m_pNext = m_pPieceHead;
//		m_pPieceHead = pNew;
//	}
//
//	// Note : Tree를 만든다.
//	MakeTree();
//}
//
//BOOL DxPieceAABB::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision )
//{
//	BOOL bCollision(FALSE);
//	CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume();
//	NSPIECETREE::CollisionLine( m_pPieceAABBTree, cv, vStart, vEnd, vCollision, bCollision );
//
//	return bCollision;
//}
//
//BOOL DxPieceAABB::IsCollisionLine2( const D3DXVECTOR3& vStart, 
//									D3DXVECTOR3& vEnd, 
//									D3DXVECTOR3& vCollision, 
//									DxPiece** rrPiece,
//                                    EMCOLLMODE emCullMode )
//{
//	BOOL bCollision(FALSE);
//	if( m_pPieceAABBTree )
//	{
//		NSPIECETREE::CollisionLine2( m_pPieceAABBTree, vStart, vEnd, vCollision, bCollision, *rrPiece, emCullMode );
//	}
//
//	return bCollision;
//}
//
//// Note : 충돌이 되면 바로 리턴한다.
//BOOL DxPieceAABB::IsCollisionLinePixel( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode )
//{
//    BOOL bCollision(FALSE);
//	return NSPIECETREE::IsCollisionLinePixel( m_pPieceAABBTree, vStart, vEnd, emCullMode );
//}
//
//void DxPieceAABB::ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//    DxPiece* pCur = m_pPieceHead;
//	while( pCur )
//	{
//		pCur->ReloadTexEffVMFX( pd3dDevice );
//		pCur = pCur->m_pNext;
//	}
//}
//
//void DxPieceAABB::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot, const TSTRING& strPieceName )
//{
//	NSPIECETREE::ExportXFile( m_pPieceAABBTree, pxofsave, pDataObjectRoot, strPieceName );
//}
//
//// -----------------------------------------------------------------------------------------------------------------------------------------
////											M	A	T	R	I	X		P	I	S	N	A	M	E
//// -----------------------------------------------------------------------------------------------------------------------------------------
//DxPieceManager::MATRIXPISNAME::MATRIXPISNAME() :
//	m_pPisName(NULL),
//	m_pPiece(NULL)
//{
//}
//
//DxPieceManager::MATRIXPISNAME::~MATRIXPISNAME()
//{
//	CleanUp();
//}
//
//void DxPieceManager::MATRIXPISNAME::CleanUp()
//{
//	m_pPiece = NULL;	// m_mapNamePiece 에 넘겨주기 위해서 존재 했었다.
//	SAFE_DELETE_ARRAY( m_pPisName );
//}
//
//void DxPieceManager::MATRIXPISNAME::Save( sc::SerialFile& SFile, const D3DXMATRIX& matLocal, const D3DXMATRIX& matFrame )
//{
//	// Note : PisEdit * 자신 * Frame = 값.
//	//		
//	D3DXMATRIX matInverse;
//	D3DXMatrixInverse( &matInverse, NULL, &matFrame );
//	D3DXMatrixMultiply( &m_matWorld, &m_pPiece->m_matWorld, &matInverse );
//
//	D3DXMatrixInverse( &matInverse, NULL, &matLocal );
//	D3DXMatrixMultiply( &m_matWorld, &matInverse, &m_matWorld );
//	
//	SFile.WriteBuffer( m_matWorld, sizeof(D3DXMATRIX) );
//
//	DWORD dwSize = 0;
//	if( m_pPisName )	dwSize = (DWORD)strlen(m_pPisName) + 1;
//	else				dwSize = 0;
//	SFile << dwSize;
//	if( dwSize )
//	{
//		SFile.WriteBuffer( m_pPisName, sizeof(char)*dwSize );
//	}
//}
//
//void DxPieceManager::MATRIXPISNAME::Load( sc::SerialFile& SFile )
//{
//	CleanUp();
//
//	SFile.ReadBuffer( m_matWorld, sizeof(D3DXMATRIX) );
//
//	DWORD dwSize;
//	SFile >> dwSize;
//	if( dwSize )
//	{
//		m_pPisName = new char[dwSize];
//		SFile.ReadBuffer( m_pPisName, sizeof(char)*dwSize );
//	}
//}
//
//// -----------------------------------------------------------------------------------------------------------------------------------------
////										D	x		P	i	e	c	e		M	a	n	a	g	e	r
//// -----------------------------------------------------------------------------------------------------------------------------------------
//const DWORD DxPieceManager::VERSION_PSF = 0x101;
//const DWORD DxPieceManager::VERSION_WLD = 0x101;
//
//DxPieceManager::EDITMODE DxPieceManager::g_emEDITMODE(DxPieceManager::MODE_EDIT);
//
//DxPieceManager::DxPieceManager()
//	: m_fTime(0.f)
//	, m_fElapsedTime(0.f)
//	, m_pPiecePick(NULL)
//	, m_bUseLightMap(FALSE)
//{
//}
//
//DxPieceManager::~DxPieceManager()
//{
//	CleanUp();
//}
//
//void DxPieceManager::CleanUp()
//{
//	m_fTime = 0.f;
//	m_fElapsedTime = 0.f;
//
//	m_bUseLightMap = FALSE;
//
//	std::for_each( m_mapChangePis.begin(), m_mapChangePis.end(), std_afunc::DeleteMapObject() );
//	m_mapChangePis.clear();
//
//	std::for_each( m_vecPointPis.begin(), m_vecPointPis.end(), std_afunc::DeleteObject() );
//	m_vecPointPis.clear();
//
//	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		(*iter).second->ClearTree();
//	}
//
//	std::for_each( m_mapNamePiece.begin(), m_mapNamePiece.end(), std_afunc::DeleteMapObject() );
//	m_mapNamePiece.clear();
//}
//
//void DxPieceManager::FrameMove( const float fTime, const float fElapsedTime )
//{
//	m_fTime = fTime;
//	m_fElapsedTime = fElapsedTime;
//}
//
//void DxPieceManager::Render( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, BOOL bPSF, float fLightMapDetail )
//{
//    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
//		return;
//
//	// LightMap 셋팅이라면 LightMap 빠진 Piece 라도 OverLighting 되도록 Shader 로 렌더링 되도록 한다.
//	if ( m_bUseLightMap )
//	{
//		// 카메라 동작후 작동되어야 한다.
//		NSVERTEXNORTEX_FX::SetEnable();
//	}
//
//	m_pPiecePick = NULL;
//
//	pd3dDevice->LightEnable( 3, FALSE );
//	pd3dDevice->LightEnable( 4, FALSE );
//	pd3dDevice->LightEnable( 5, FALSE );
//	pd3dDevice->LightEnable( 6, FALSE );
//	pd3dDevice->LightEnable( 7, FALSE );
//
//	DxPieceQuickSort::InitPickAlpha();		// Note : Init Static
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
//	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		if ( (*iter).second->IsUserLightMapUV() )
//		{
//			NSLightMapFX::LightMapDetail( 1.f );
//		}
//		else
//		{
//			NSLightMapFX::LightMapDetail( fLightMapDetail );
//		}
//
//		(*iter).second->Render( pd3dDevice, m_pPiecePick, sCV, m_fTime, m_fElapsedTime, bPSF );
//	}
//
//	DxPieceQuickSort::FrameMoveONLY( m_fTime, m_fElapsedTime );	// Note : FrameMove Static
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
//	// Note : 나중 테스트 할때 써먹자
//	//CLIPVOLUME	sCV;
//
//	//MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
//	//for( ; iter!=m_mapChangePis.end(); ++iter )
//	//{
//	//	DxPiece* pPiece = (*iter).second->m_pPiece;
//	//	if( pPiece )	pPiece->Render( pd3dDevice, sCV, (*iter).second->m_matWorld );
//	//}
//
//    // LightMap 렌더 시 위치좌표를 맞추기 위함.
//	if ( m_bUseLightMap )
//    {
//        D3DXMATRIX matIdentity;
//        D3DXMatrixIdentity( &matIdentity );
//        NSLightMapFX::SetWorld( matIdentity, 1.f );
//        NSVERTEXNORTEX_FX::SetWorld( matIdentity, 1.f );
//		NSVERTEXNORTEX_FX::SetDisable();
//    }
//}
//
//void DxPieceManager::RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF, float fLightMapDetail )
//{
//    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
//		return;
//
//	// LightMap 셋팅이라면 LightMap 빠진 Piece 라도 OverLighting 되도록 Shader 로 렌더링 되도록 한다.
//	if ( m_bUseLightMap )
//	{
//		NSVERTEXNORTEX_FX::SetEnable();
//	}
//
//	pd3dDevice->LightEnable( 3, FALSE );
//	pd3dDevice->LightEnable( 4, FALSE );
//	pd3dDevice->LightEnable( 5, FALSE );
//	pd3dDevice->LightEnable( 6, FALSE );
//	pd3dDevice->LightEnable( 7, FALSE );
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
//	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		if ( (*iter).second->IsUserLightMapUV() )
//		{
//			NSLightMapFX::LightMapDetail( 1.f );
//		}
//		else
//		{
//			NSLightMapFX::LightMapDetail( fLightMapDetail );
//		}
//
//		(*iter).second->RenderAlpha( pd3dDevice, bPSF );
//	}
//
//	iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		(*iter).second->RenderEff( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bPSF );
//	}
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
//		NSVERTEXNORTEX_FX::SetDisable();
//    }
//}
//
//void DxPieceManager::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshLightMap* pStaticMeshLightMap, DxFrameMesh* pFrameMesh, BOOL bPSF )
//{
//    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
//		return;
//
//	// LightMap 셋팅이라면 LightMap 빠진 Piece 라도 OverLighting 되도록 Shader 로 렌더링 되도록 한다.
//	if ( m_bUseLightMap )
//	{
//		NSVERTEXNORTEX_FX::SetEnable();
//	}
//
//	DxPieceQuickSort::RenderPickAlpha( pd3dDevice, pStaticMeshLightMap, pFrameMesh, bPSF );
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
//
//void DxPieceManager::Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPSF )
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
//	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		(*iter).second->Render_Reflect( pd3dDevice, sCV, bPSF );
//	}
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
//
//HRESULT DxPieceManager::DrawWireFrame ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME *pCV )        //	editer 용. 
//{
//    if( !GLOBAL_PARAM::IsValue( GP_PIECE ) )
//		return S_OK;
//
//    MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		(*iter).second->Render( pd3dDevice, m_pPiecePick, *pCV, m_fTime, m_fElapsedTime, TRUE );
//        (*iter).second->RenderAlpha( pd3dDevice, TRUE );
//	}
//
//    DxPieceQuickSort::RenderPickAlpha( pd3dDevice, NULL, NULL, TRUE );
//
//    return S_OK;
//}
//
//BOOL DxPieceManager::IsGetName( DWORD nNUM, LPCSTR& pPieceName, LPCSTR& pFrameName )
//{
//	if( nNUM >= m_mapChangePis.size()+m_vecPointPis.size() )
//        return FALSE;
//
//	if( m_vecPointPis.size() > nNUM )
//	{
//		VECPOINTPIS_ITER iter = m_vecPointPis.begin();
//		for( DWORD i=0; i<nNUM; ++i )		++iter;
//
//		pPieceName = (*iter)->m_pPisName;
//		pFrameName = NULL;
//
//		return TRUE;
//	}
//	else
//	{
//		MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
//		for( DWORD i=(DWORD)m_vecPointPis.size(); i<nNUM; ++i )	++iter;
//
//		pPieceName = (*iter).second->m_pPisName;
//		pFrameName = (*iter).first.c_str();
//
//		return TRUE;
//	}
//}
//
//// m_mapChangePis 에 관련된 일만 한다.
//void DxPieceManager::SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh, const char* szFrameName, const char* szPisName )
//{
//	if( !szPisName )	return;
//
//	DxFrame* pFrame = pFrameMesh->FindFrame( szFrameName );
//	if( !pFrame )		return;
//
//	// Note : Edit를 위한 구조체에 데이터 삽입
//	MAPCHANGEPIS_ITER iter = m_mapChangePis.find( szFrameName );
//	if( iter!=m_mapChangePis.end() )
//	{
//		// Note : 변환 Pis가 없거나 이름이 똑같다. ㅡㅡ;
//		if( !strcmp( (*iter).second->m_pPisName, szPisName ) )	return;
//
//		// Note : PieceContainer의 Count를 줄인다.
//		DxPieceContainer::GetInstance().ReleasePiece( (*iter).second->m_pPisName );
//		
//		// Note : Piece File 이름을 바꾼다.
//		size_t nCount = strlen( szPisName ) + 1;
//		SAFE_DELETE_ARRAY( (*iter).second->m_pPisName );
//		(*iter).second->m_pPisName = new char[ nCount ];
//		StringCchCopy( (*iter).second->m_pPisName, nCount, szPisName );
//
//		// PieceEdit 로드 후 Piece에 셋팅
//		DxPieceEdit*	pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, szPisName );
//		(*iter).second->m_pPiece->Import( pd3dDevice, pPieceEdit, (*iter).second->m_matWorld, pFrame->matCombined );
//	}
//	else
//	{
//		MATRIXPISNAME* pNew = new MATRIXPISNAME;
//		D3DXMatrixIdentity( &pNew->m_matWorld );
//
//		size_t nCount = strlen( szPisName ) + 1;
//		SAFE_DELETE_ARRAY( pNew->m_pPisName );
//		pNew->m_pPisName = new char[nCount];
//		StringCchCopy( pNew->m_pPisName, nCount, szPisName );
//
//		// PieceEdit 로드 후 Piece에 셋팅
//		DxPieceEdit*	pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, szPisName );
//		pNew->m_pPiece = new DxPiece;
//		pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matWorld, pFrame->matCombined );
//
//		m_mapChangePis.insert( std::make_pair( szFrameName, pNew ) );
//	}
//
//	MakeTree();
//
//	// Note : Frame이 Piece로 사용한다고 전한다.
//	pFrameMesh->AddPiece2( szFrameName );
//}
//
//void DxPieceManager::DeletePiece( DxFrameMesh* pFrameMesh, const char* szFrameName )
//{
//	// Note : 데이터 삭제
//	MAPCHANGEPIS_ITER iter_DEL;
//	MAPCHANGEPIS_ITER iter = m_mapChangePis.find( szFrameName );
//	if( iter!=m_mapChangePis.end() )
//	{
//		// Release
//		DxPieceContainer::GetInstance().ReleasePiece( (*iter).second->m_pPisName );
//
//		SAFE_DELETE( (*iter).second );
//
//		m_mapChangePis.erase( iter );
//	}
//
//	MakeTree();
//
//	// Note : Frame에 Piece로 사용이 해제 되었다고 전한다.
//	pFrameMesh->DelPiece2( szFrameName );
//}
//
//// m_vecPointPis에 관련된 일만 한다.
//BOOL DxPieceManager::SetPiece( LPDIRECT3DDEVICEQ pd3dDevice, const char* szPisName, const D3DXMATRIX& matWorld )
//{
//	if( !szPisName )	
//        return FALSE;
//
//    if( _tcslen(szPisName)==0 )	
//        return FALSE;
//
//	if ( !m_mapChangePis.empty() && sc::string::getFileExt( szPisName ) == _T("pie15") )
//	{
//		AfxMessageBox( _T("ERROR - pis, pie15 가 동시에 쓰이고 있습니다.") );
//		return FALSE;
//	}
//
//	for ( DWORD i=0; i<m_vecPointPis.size(); ++i )
//	{
//		if ( sc::string::getFileExt( szPisName ) != sc::string::getFileExt( m_vecPointPis[i]->m_pPisName ) )
//		{
//			AfxMessageBox( _T("ERROR - pie, pie15 가 동시에 쓰이고 있습니다.") );
//			return FALSE;
//		}
//	}
//
//	MATRIXPISNAME* pNew = new MATRIXPISNAME;
//	pNew->m_matWorld = matWorld;
//
//	size_t nCount = strlen( szPisName ) + 1;
//	SAFE_DELETE_ARRAY( pNew->m_pPisName );
//	pNew->m_pPisName = new char[nCount];
//	StringCchCopy( pNew->m_pPisName, nCount, szPisName );
//
//	// PieceEdit 로드 후 Piece에 셋팅
//	DxPieceEdit*	pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, szPisName );
//	pNew->m_pPiece = new DxPiece;
//	D3DXMATRIX matIdentity;
//	D3DXMatrixIdentity( &matIdentity );
//	pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matWorld, matIdentity );
//
//	// vector 에 삽입을 해 놓는다.
//	m_vecPointPis.push_back( pNew );
//
//	MakeTree();
//
//    return TRUE;
//}
//
//void DxPieceManager::DeletePiece( DWORD dwNumber )
//{
//	VECPOINTPIS_ITER iter = m_vecPointPis.begin();
//	for( DWORD i=0; i<dwNumber; ++i )	++iter;
//
//	MATRIXPISNAME* pDelete = (*iter);
//	SAFE_DELETE( pDelete );
//
//	m_vecPointPis.erase( iter );
//
//	MakeTree();
//}
//
//void DxPieceManager::MakeTree()
//{
//	// Note : Tree 를 삭제. List는 참조한 것이므로 삭제하면 안됨.
//	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		(*iter).second->ClearOnlyTree();
//	}
//	m_mapNamePiece.clear();
//
//	// Note : All Setting
//	MAPCHANGEPIS_ITER iter_base = m_mapChangePis.begin();
//	for( ; iter_base!=m_mapChangePis.end(); ++iter_base )
//	{
//		// Note : 없다면 작업 취소
//		if( !(*iter_base).second->m_pPiece )	continue;
//
//		iter = m_mapNamePiece.find( (*iter_base).second->m_pPisName );
//		if( iter!=m_mapNamePiece.end() )
//		{
//			(*iter).second->SetPiece( (*iter_base).second->m_pPiece );
//		}
//		else
//		{
//			// Note : 새로운 넘이다.
//			DxPieceAABB* pNew = new DxPieceAABB;
//
//			pNew->SetPiece( (*iter_base).second->m_pPiece );
//
//			m_mapNamePiece.insert( std::make_pair( (*iter_base).second->m_pPisName, pNew ) );
//		}
//	}
//	VECPOINTPIS_ITER iter_point = m_vecPointPis.begin();
//	for( ; iter_point!=m_vecPointPis.end(); ++iter_point )
//	{
//		// Note : 없다면 작업 취소
//		if( !(*iter_point)->m_pPiece )	continue;
//
//		iter = m_mapNamePiece.find( (*iter_point)->m_pPisName );
//		if( iter!=m_mapNamePiece.end() )
//		{
//			(*iter).second->SetPiece( (*iter_point)->m_pPiece );
//		}
//		else
//		{
//			// Note : 새로운 넘이다.
//			DxPieceAABB* pNew = new DxPieceAABB;
//
//			pNew->SetPiece( (*iter_point)->m_pPiece );
//
//			m_mapNamePiece.insert( std::make_pair( (*iter_point)->m_pPisName, pNew ) );
//		}
//	}
//
//	// Note : Create AABB Tree
//	iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		(*iter).second->MakeTree();
//	}
//}
//
//D3DXMATRIX* DxPieceManager::MoveCamera( DxFrame* pFrameRoot, const char* szFrameName )
//{
//	MAPCHANGEPIS_ITER iter = m_mapChangePis.find( szFrameName );
//	if( iter!=m_mapChangePis.end() )
//	{
//		if ( !(*iter).second->m_pPiece )
//			return NULL;
//
//		D3DXVECTOR3 vPos;
//		vPos.x = (*iter).second->m_pPiece->m_matWorld._41;
//		vPos.y = (*iter).second->m_pPiece->m_matWorld._42;
//		vPos.z = (*iter).second->m_pPiece->m_matWorld._43;
//
//		// Note : 카메라를 이동해야지.
//		DxViewPort::GetInstance().CameraJump( vPos );
//
//		return &(*iter).second->m_pPiece->m_matWorld;
//	}
//	
//	return NULL;
//}
//
//D3DXMATRIX* DxPieceManager::MoveCamera( DWORD dwNumber )
//{
//	VECPOINTPIS_ITER iter = m_vecPointPis.begin();
//	for( DWORD i=0; i<dwNumber; ++i )	
//		++iter;
//
//	if ( !(*iter)->m_pPiece )
//		return NULL;
//
//	D3DXVECTOR3 vPos;
//	vPos.x = (*iter)->m_pPiece->m_matWorld._41;
//	vPos.y = (*iter)->m_pPiece->m_matWorld._42;
//	vPos.z = (*iter)->m_pPiece->m_matWorld._43;
//
//	// Note : 카메라를 이동해야지.
//	DxViewPort::GetInstance().CameraJump( vPos );
//
//	return &(*iter)->m_pPiece->m_matWorld;
//}
//
//DxPiece* DxPieceManager::GetPiece( DWORD dwNumber )
//{
//    VECPOINTPIS_ITER iter = m_vecPointPis.begin();
//    for( DWORD i=0; i<dwNumber; ++i )	++iter;
//    return (*iter)->m_pPiece;
//}
//
//DxPieceManager::MATRIXPISNAME* DxPieceManager::GetMATRIXPISNAME( DWORD dwNumber )
//{
//	VECPOINTPIS_ITER iter = m_vecPointPis.begin();
//	for( DWORD i=0; i<dwNumber; ++i )	++iter;
//	return (*iter);
//}
//
//DxPieceManager::MATRIXPISNAME* DxPieceManager::GetChangeMATRIXPISNAME( DWORD dwNumber )
//{
//	MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
//
//	std::advance(iter, dwNumber);
//
//	if (iter != m_mapChangePis.end())
//	{
//		return iter->second;
//	}
//
//	return 0;
//}
//
//BOOL DxPieceManager::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision )
//{
//	BOOL bCollision(FALSE);
//
//	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		if( (*iter).second->IsCollisionLine( vStart, vEnd, vCollision ) )	bCollision = TRUE;
//	}
//
//	return bCollision;
//}
//
//BOOL DxPieceManager::IsCollisionLine2( const D3DXVECTOR3& vStart, 
//										D3DXVECTOR3& vEnd, 
//										D3DXVECTOR3& vCollision, 
//										DxPiece** rrPiece,
//                                        EMCOLLMODE emCullMode )
//{
//	*rrPiece = NULL;
//	BOOL bCollision(FALSE);
//	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		if( (*iter).second->IsCollisionLine2( vStart, vEnd, vCollision, rrPiece, emCullMode ) )
//			bCollision = TRUE;
//	}
//
//	return bCollision;
//}
//
//// Note : 충돌이 되면 바로 리턴한다.
//BOOL DxPieceManager::IsCollisionLinePixel( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, EMCOLLMODE emCullMode )
//{
//    MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		if( (*iter).second->IsCollisionLinePixel( vStart, vEnd, emCullMode ) )
//            return TRUE;
//	}
//    return FALSE;
//}
//
//int DxPieceManager::GetIndex( DxPiece* pPiece )
//{
//	int nCount(0);
//
//    // m_vecPointPis
//    {
//	    VECPOINTPIS_ITER iter = m_vecPointPis.begin();
//	    for( ; iter!=m_vecPointPis.end(); ++iter, ++nCount )
//	    {
//		    if( (*iter)->m_pPiece == pPiece )
//		    {
//			    return nCount;
//		    }
//	    }
//    }
//
//    // m_mapChangePis
//    {
//        MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
//	    for( ; iter!=m_mapChangePis.end(); ++iter, ++nCount )
//	    {
//		    if( (*iter).second->m_pPiece == pPiece )
//		    {
//			    return nCount;
//		    }
//	    }
//    }
//
//	return -1;
//}
//
//void DxPieceManager::ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//    MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		(*iter).second->ReloadTexEffVMFX( pd3dDevice );
//	}
//}
//
//void DxPieceManager::CreateAlphaTexture32Bit( LPDIRECT3DDEVICEQ pd3dDevice )
//{
//    MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		(*iter).second->CreateAlphaTexture32Bit( pd3dDevice );
//	}
//}
//
//void DxPieceManager::Clone_MouseShift( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX& pMatrix )
//{
//	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		if( (*iter).second->Clone_MouseShift( pMatrix ) )
//		{
//			SetPiece( pd3dDevice, (*iter).first.c_str(), *pMatrix );
//
//			MATRIXPISNAME* pMatrixPisName = m_vecPointPis[ m_vecPointPis.size()-1 ];
//
//			// Note : DxObjectMRS 커서를 바꿔준다. < 중요 >
//			pMatrix = &pMatrixPisName->m_pPiece->m_matWorld;
//
//			return;
//		}
//	}
//}
//
//void DxPieceManager::SavePSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh )
//{
//	D3DXMATRIX matIdentity;
//	D3DXMATRIX* pmatPisEdit, *pmatFrame;
//	D3DXMatrixIdentity( &matIdentity );
//
//	DxFrame*		pFrame(NULL);
//	DxPieceEdit*	pPieceEdit(NULL);
//
//	SFile << VERSION_PSF;
//
//	SFile.BeginBlock();
//	{
//		DWORD dwSize = (DWORD)m_vecPointPis.size();
//		SFile << dwSize;
//
//		VECPOINTPIS_ITER iter = m_vecPointPis.begin();
//		for( ; iter!=m_vecPointPis.end(); ++iter )
//		{
//			pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, (*iter)->m_pPisName );
//			{
//				if( pPieceEdit )	pmatPisEdit = pPieceEdit->GetMatrixLocal();
//				else				pmatPisEdit = &matIdentity;
//
//				(*iter)->Save( SFile, *pmatPisEdit, matIdentity );
//			}
//			DxPieceContainer::GetInstance().ReleasePiece( (*iter)->m_pPisName );
//		}
//	}
//	{
//		DWORD dwSize = (DWORD)m_mapChangePis.size();
//		SFile << dwSize;
//
//		MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
//		for( ; iter!=m_mapChangePis.end(); ++iter )
//		{
//			SFile << (*iter).first;
//
//			pFrame = pFrameMesh->FindFrame( (*iter).first.c_str() );
//			pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, (*iter).second->m_pPisName );
//			{
//				if( pPieceEdit )	pmatPisEdit = pPieceEdit->GetMatrixLocal();
//				else				pmatPisEdit = &matIdentity;
//
//				if( pFrame )		pmatFrame = &pFrame->matCombined;
//				else				pmatFrame = &matIdentity;
//
//				(*iter).second->Save( SFile, *pmatPisEdit, *pmatFrame );
//			}
//			DxPieceContainer::GetInstance().ReleasePiece( (*iter).second->m_pPisName );
//		}
//	}
//	SFile.EndBlock();
//}
//
//void DxPieceManager::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh )
//{
//	CleanUp();
//
//	D3DXMATRIX matIdentity;
//	D3DXMatrixIdentity( &matIdentity );
//
//	DWORD dwVer, dwBufferSize;
//	SFile >> dwVer;
//	SFile >> dwBufferSize;
//
//	DWORD dwSize = 0;
//	{
//		SFile >> dwSize;
//
//		for( DWORD i=0; i<dwSize; ++i )
//		{
//			MATRIXPISNAME* pNew = new MATRIXPISNAME;
//			pNew->Load( SFile );
//			m_vecPointPis.push_back( pNew );
//
//			DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, pNew->m_pPisName );
//			if( !pPieceEdit )
//			{
//				std::string strName = "조각파일 : ";
//				strName += pNew->m_pPisName;
//				strName += " 이 없습니다.";
//				MessageBox( NULL, strName.c_str(), "ERROR", MB_OK );
//				continue;
//			}
//
//			pNew->m_pPiece = new DxPiece;
//			pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matWorld, matIdentity );
//		}
//	}
//	{
//		SFile >> dwSize;
//
//		std::string szFrameName;
//		for( DWORD i=0; i<dwSize; ++i )
//		{
//			SFile >> szFrameName;
//
//			// Note : szFrameName이 있는지 없는지를 검사한다. 
//			//			만약 없다면 로딩만 한 후 건너띤다.
//			DxFrame* pFrame = pFrameMesh->FindFrame( szFrameName.c_str() );
//			if( !pFrame )
//			{
//				MATRIXPISNAME sNew;
//				sNew.Load( SFile );
//				continue;
//			}
//
//			MATRIXPISNAME* pNew = new MATRIXPISNAME;
//			pNew->Load( SFile );
//			m_mapChangePis.insert( std::make_pair( szFrameName, pNew ) );
//
//			DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, pNew->m_pPisName );
//			if( !pPieceEdit )
//			{
//				std::string strName = "조각파일 : ";
//				strName += pNew->m_pPisName;
//				strName += " 이 없습니다.";
//				MessageBox( NULL, strName.c_str(), "ERROR", MB_OK );
//				continue;
//			}
//
//			pNew->m_pPiece = new DxPiece;
//			pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matWorld, pFrame->matCombined );
//		}
//	}
//
//	MakeTree();
//
//	// Note : 깔끔하게 지운다.
//	pFrameMesh->SetPieceUseFALSE();
//
//	// Note : Src Frame에 Piece를 사용하고 있다고 알린다.
//	MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
//	for( ; iter!=m_mapChangePis.end(); ++iter )
//	{
//		pFrameMesh->AddPiece2( (*iter).first.c_str() );
//	}
//}
//
//void DxPieceManager::LoadPSF_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, DxFrameMesh* pFrameMesh )
//{
//	CleanUp();
//
//	D3DXMATRIX matIdentity;
//	D3DXMatrixIdentity( &matIdentity );
//
//	DWORD dwSize = 0;
//	{
//		SFile >> dwSize;
//
//		for( DWORD i=0; i<dwSize; ++i )
//		{
//			MATRIXPISNAME* pNew = new MATRIXPISNAME;
//			pNew->Load( SFile );
//			m_vecPointPis.push_back( pNew );
//
//			DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, pNew->m_pPisName );
//			if( !pPieceEdit )
//			{
//				std::string strName = "조각파일 : ";
//				strName += pNew->m_pPisName;
//				strName += " 이 없습니다.";
//				MessageBox( NULL, strName.c_str(), "ERROR", MB_OK );
//				continue;
//			}
//
//			pNew->m_pPiece = new DxPiece;
//			pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matWorld, matIdentity );
//		}
//	}
//	{
//		SFile >> dwSize;
//
//		std::string szFrameName;
//		for( DWORD i=0; i<dwSize; ++i )
//		{
//			SFile >> szFrameName;
//
//			// Note : szFrameName이 있는지 없는지를 검사한다. 
//			//			만약 없다면 로딩만 한 후 건너띤다.
//			DxFrame* pFrame = pFrameMesh->FindFrame( szFrameName.c_str() );
//			if( !pFrame )
//			{
//				MATRIXPISNAME sNew;
//				sNew.Load( SFile );
//				continue;
//			}
//
//			MATRIXPISNAME* pNew = new MATRIXPISNAME;
//			pNew->Load( SFile );
//			pNew->m_matWorld = matIdentity;
//			m_mapChangePis.insert( std::make_pair( szFrameName, pNew ) );
//
//			DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, pNew->m_pPisName );
//			if( !pPieceEdit )
//			{
//				std::string strName = "조각파일 : ";
//				strName += pNew->m_pPisName;
//				strName += " 이 없습니다.";
//				MessageBox( NULL, strName.c_str(), "ERROR", MB_OK );
//				continue;
//			}
//
//			pNew->m_pPiece = new DxPiece;
//			pNew->m_pPiece->Import( pd3dDevice, pPieceEdit, pNew->m_matWorld, pFrame->matCombined );
//		}
//	}
//
//	MakeTree();
//
//	// Note : 깔끔하게 지운다.
//	pFrameMesh->SetPieceUseFALSE();
//
//	// Note : Src Frame에 Piece를 사용하고 있다고 알린다.
//	MAPCHANGEPIS_ITER iter = m_mapChangePis.begin();
//	for( ; iter!=m_mapChangePis.end(); ++iter )
//	{
//		pFrameMesh->AddPiece2( (*iter).first.c_str() );
//	}
//}
//
//void DxPieceManager::SaveWLD( sc::SerialFile& SFile )
//{
//    SFile << static_cast<DWORD>( VERSION_WLD );
//    SFile.BeginBlock();
//    {
//		SFile << m_bUseLightMap;
//
//	    DWORD dwSize = (DWORD)m_mapNamePiece.size();
//	    SFile << dwSize;
//
//	    MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	    for( ; iter!=m_mapNamePiece.end(); ++iter )
//	    {
//		    SFile << (*iter).first;
//		    (*iter).second->Save( SFile, (*iter).first.c_str() );
//	    }
//    }
//    SFile.EndBlock();
//}
//
//void DxPieceManager::LoadWLD( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
//{
//	CleanUp();
//
//    DWORD dwVer, dwBufferSize;
//    SFile >> dwVer;
//    SFile >> dwBufferSize;
//
//	if ( dwVer == 0x101 )
//	{
//		SFile >> m_bUseLightMap;
//
//		DWORD dwSize = 0;
//		SFile >> dwSize;
//
//		std::string szPieceName;
//		for( DWORD i=0; i<dwSize; ++i )
//		{
//			SFile >> szPieceName;
//
//			DxPieceAABB* pNew = new DxPieceAABB;
//			pNew->Load( pd3dDevice, SFile, szPieceName.c_str() );
//
//			m_mapNamePiece.insert( std::make_pair( szPieceName.c_str(), pNew ) );
//		}
//	}
//	else if( dwVer == 0x100 )
//	{
//		DWORD dwSize = 0;
//		SFile >> dwSize;
//
//		std::string szPieceName;
//		for( DWORD i=0; i<dwSize; ++i )
//		{
//			SFile >> szPieceName;
//
//			DxPieceAABB* pNew = new DxPieceAABB;
//			pNew->Load( pd3dDevice, SFile, szPieceName.c_str() );
//
//			m_mapNamePiece.insert( std::make_pair( szPieceName.c_str(), pNew ) );
//		}
//	}
//	else
//	{
//		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
//	}
//}
//
//void DxPieceManager::LoadWLD_PREV( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
//{
//	CleanUp();
//
//	DWORD dwSize = 0;
//	SFile >> dwSize;
//
//	std::string szPieceName;
//	for( DWORD i=0; i<dwSize; ++i )
//	{
//		SFile >> szPieceName;
//
//		DxPieceAABB* pNew = new DxPieceAABB;
//		pNew->Load_PREV( pd3dDevice, SFile, szPieceName.c_str() );
//
//		m_mapNamePiece.insert( std::make_pair( szPieceName.c_str(), pNew ) );
//	}
//}
//
//void DxPieceManager::ClonePSFtoWLD( LPDIRECT3DDEVICEQ pd3dDevice, 
//                                   DxPieceManager* pSrc, 
//                                   BOOL bLightMap, 
//                                   const TSTRING& strLightMapName, 
//                                   DxStaticMesh* pStaticMesh, 
//                                   float fLightMapDetail, 
//								   BOOL bRan_1_0 )
//{
//	CleanUp();
//
//	m_bUseLightMap = bLightMap;
//
//    // Texture Number Count 를 해야한다.
//    // Piece 인지 아닌지도 확인해야 한다.
//    NSLightMap::Init( TRUE );
//
//    int nCount(1);
//    DWORD dwStartTextureSize(512);
//	MAPNAMEPIECE_ITER iter = pSrc->m_mapNamePiece.begin();
//	for( ; iter!=pSrc->m_mapNamePiece.end(); ++iter, ++nCount )
//	{
//        // 기록
//        {
//            TSTRINGSTREAM strStringStream;
//            strStringStream << "Piece LightMap - ";
//            strStringStream << nCount;
//            strStringStream << "/";
//            strStringStream << pSrc->m_mapNamePiece.size();
//            ExportProgress::SetStateString( strStringStream.str().c_str() );
//        }
//
//        const DxPieceEdit* pPieceEdit = DxPieceContainer::GetInstance().LoadPiece( pd3dDevice, (*iter).first.c_str() );
//
//		// LightMap 을 만든다면, 여기서 하는 일은 Mesh 분리와 Texel 들의 컬러계산이 작업된다.
//		// LightMap 을 안 만든다면, Piece 만 잘 분리하는 작업만하게 된다.
//		DxPieceAABB* pNew = new DxPieceAABB;
//		pNew->CloneData( pd3dDevice, (*iter).second, pPieceEdit, bLightMap, strLightMapName, pStaticMesh, pSrc, dwStartTextureSize, fLightMapDetail );
//
//		m_mapNamePiece.insert( std::make_pair( (*iter).first.c_str(), pNew ) );
//	}
//
//    // LightMap 일 경우 2단계 작업이 들어간다.
//    if ( bLightMap )
//    {
//        {
//			// Piece 는 Fit 하게 Resize 하면 오류가 생기는 경우가 있다.
//			// 1,2,3,4,5(마지막텍스쳐) 에서 4,5 가 이어져 있고 5가 Fit 하게 줄여지면 UV에 문제가 생긴다.
//            //// LightMap Texture를 Resize 한다
//            //NSLightMap::ResizeLightMapTexture();
//
//            // 텍스쳐 정리.
//            TextureManager::GetInstance().RescanPath();
//        }
//
//		// Texture 관리자(m_vecTextureData)에 각각 Piece에서 계산된 컬러를 넣어준다.
//		// LightMapRenderMan를 생성하는 작업도 해준다.
//        MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//        for( ; iter!=m_mapNamePiece.end(); ++iter )
//	    {
//            (*iter).second->CreateLightMapPiece( pd3dDevice, strLightMapName, bRan_1_0 );
//        }
//
//		// 실제 텍스쳐를 만든다.
//        // 마지막 정리작업.
//        NSLightMap::CreateLightMapFinal_PIECE( pd3dDevice, strLightMapName, _T("_P") );
//
//        // NSLightMap의 텍스쳐 정리.
//        TextureManager::GetInstance().RescanPath();
//
//        // SingleTexMesh 에서 LightMap Texture 로딩 함.
//        iter = m_mapNamePiece.begin();
//        for( ; iter!=m_mapNamePiece.end(); ++iter )
//	    {
//            (*iter).second->LoadLightMap( pd3dDevice );
//        }
//    }
//}
//
//void DxPieceManager::ExportXFile( LPDIRECTXFILESAVEOBJECT pxofsave, LPDIRECTXFILEDATA pDataObjectRoot )
//{
//	MAPNAMEPIECE_ITER iter = m_mapNamePiece.begin();
//	for( ; iter!=m_mapNamePiece.end(); ++iter )
//	{
//		TSTRING strNewName("");
//		TSTRING strPieceName = sc::util::GetName_ExtErase( (*iter).first );
//
//		while( strNewName != strPieceName )
//		{
//			strPieceName = sc::util::GetChangeTCHAR( strPieceName, ' ', '_' );	// 작업함.
//			strNewName = sc::util::GetChangeTCHAR( strPieceName, ' ', '_' );	// 또 공백이 있는지 확인 후 while 문 비교를 위해 작업함.
//		}
//
//		(*iter).second->ExportXFile( pxofsave, pDataObjectRoot, strPieceName );
//	}
//}
//
//void DxPieceManager::Archive( boost::function<void (const TCHAR*)> fun )
//{
//    BOOST_FOREACH(MAPCHANGEPIS_VALUE& it, m_mapChangePis)
//    {
//        fun( (TSTRING(it.second->m_pPisName) + DxPieceContainer::GetInstance().GetPath()).c_str() );
//    }
//
//    BOOST_FOREACH(VECPOINTPIS::value_type& it, m_vecPointPis)
//    {
//        fun( (TSTRING(it->m_pPisName) + DxPieceContainer::GetInstance().GetPath()).c_str() );
//    }
//}
