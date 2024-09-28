#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "DxSkinMeshManager.h"
#include "DxSkinDefine.h"

#include "../../DxTools/DxInputDevice.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxLand/DxLandMan.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxDynamicVB.h"

#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/COLLISION.h"

#include "./DxSkinAniControlThreadNeedData.h"
#include "./DxAttBoneLinkThreadNeedData.h"
#include "./NSSkinAniThread.h"
#include "./DxSkeletonMaintain.h"
#include "./DxSkinMeshManager.h"
#include "./DxSkinAnimation.h"
#include "./DxJiggleBoneColl.h"
#include "./DxJiggleBone.h"
#include "./DxClothColl.h"
#include "./SANIMCONINFO.h"

#include "DxAttBone.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


extern BOOL	g_bCHAR_EDIT_RUN;

DWORD	DxAttBone::g_dwLOD = 0;
DWORD	DxAttBone::g_dwHIGHDRAW_NUM = 0;


DxAttBone::DxAttBone(void)
	: m_fScale(1.0f)
	, m_fHeight(20.f)
	, m_bWorldObj(FALSE)
	, m_dwVertexNUM(0)
	, m_spSkinAniControlThread(new DxSkinAniControl)
{
	memset( m_szFileName, 0, FILE_LENGTH );
}

DxAttBone::~DxAttBone(void)
{
}




HRESULT DxAttBone::ClearAll ()
{
	m_fScale = 1.0f;
	m_bWorldObj = FALSE;

	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		//	Note : 만약 이미 읽은 AttBone Type 에 지정된 Piece 가 있다면 지워준다.
		//
		if ( m_AttBoneArray[i].GetFileName() )
		{
			m_AttBoneArray[i].ReSetPart ();
		}
	}

	m_spSkinAniControlThread->ClearAll ();

	return S_OK;
}

void DxAttBone::ResetAttBonePiece ( int i )
{
	GASSERT(i<ATTBONE_SIZE);

	//	Note : 이전에 올려진 Part 가 있다면 이를 내려준다.
	//
	if ( m_AttBoneArray[i].GetFileName() )
	{
		m_AttBoneArray[i].ReSetPart ();
	}
}

void DxAttBone::ResetAttBone()
{
	ClearAll();

	if ( m_szFileName[0]!='\0' )
		DxAttBoneDataContainer::GetInstance().ReleaseAttBoneData ( m_szFileName );
	m_szFileName[0] = '\0';
}

void DxAttBone::SetAttBoneData ( DxAttBoneData* pAttBoneData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP /*= FALSE*/, int nLevel/*=FALSE*/, EMSCD_FLAGS emFlags, const char* pSelfBodyEffect )
{
	// [shhan][2014.09.18]
	// [CRM #3013] 변신 기능으로 인해 DxSkinChar가 유지 되면서 DxSkinCharData 인자값이 다른것들이 넘어 올 수 있다. 그러면서 재 셋팅.
	//				타 쓰레드에서 참조하고 있을 수 있으므로 m_spSkinAniControlThread->ClearAll, m_spSkinAniControlThread->SetCharData 등.. 작업을 직접하지 말고 새로 생성 후 하자.
	//				형상이 바뀌는 경우는 캐릭터가 잠깐 안보이더라도 전부 정리를 해주는 방향으로 한다.
	{
		m_spSkinAniControlThread = std::tr1::shared_ptr<DxSkinAniControl>(new DxSkinAniControl);

		for ( int i=0; i<ATTBONE_SIZE; ++i )
		{
			//	Note : 만약 이미 읽은 AttBone Type 에 지정된 Piece 가 있다면 지워준다.
			//
			if ( m_AttBoneArray[i].GetFileName() )
			{
				m_AttBoneArray[i].ReSetPart ();
			}
		}
	}

	//TSTRING strFileName = pAttBoneData->GetFileName();
	//if ( strFileName.empty() )
	//	return;

	BOOL bSetBaseCPS(FALSE);
	BOOL bJiggleBone(FALSE);
	//if( emFlags & EMSCD_USERCOLOR )
	//{
	//	bUserColor = TRUE;
	//}
	if( emFlags & EMSCD_SET_BASE_CPS )
	{
		bSetBaseCPS = TRUE;
	}
	//if( dwFlags & EMSCD_GAME_PLAYER )
	//{
	//	m_bGamePlayer = TRUE;
	//}

	StringCchCopy( m_szFileName, FILE_LENGTH, pAttBoneData->GetFileName() );

	//EMANI_MAINTYPE MType = m_spSkinAniControlThread->GETCURMTYPE();
	//EMANI_SUBTYPE SType = m_spSkinAniControlThread->GETCURSTYPE();
	//if ( MType == AN_NONE )	MType = AN_GUARD_N;

	m_spSkinAniControlThread->ClearAll ();

	//float fCurTime = m_spSkinAniControlThread->GETCURTIME();

	m_fScale = pAttBoneData->m_fScale;
	m_vMax = pAttBoneData->m_vMax;
	m_vMin = pAttBoneData->m_vMin;
	m_fHeight = pAttBoneData->m_fHeight;


	m_bWorldObj = pAttBoneData->m_bWorldObj;

	INT32 nGRIND = nLevel;

	m_spSkinAniControlThread->SetCharData ( pAttBoneData->GetSkinAniControlData(), pd3dDevice, m_fScale, m_szFileName );

	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		DxSkinPiece* pSkinPiece = pAttBoneData->GetAttBonePiece( pd3dDevice, i, emFlags );
		if ( pSkinPiece )
		{
			m_AttBoneArray[i].SetPart ( this, pSkinPiece, pd3dDevice, bAmbientUP, pSelfBodyEffect );
			m_AttBoneArray[i].SetGrindLevel( nGRIND );
		}
		else
		{
			m_AttBoneArray[i].ReSetPart ();
		}
	}





	////	Note : 에니메이션 복제.
	////
	//m_strSkeleton = pAttBoneData->GetSkeletonName();
 //   m_skeletonResThread = DxSkeletonManager::GetInstance().LoadSkeleton( m_strSkeleton, TRUE );

	//bJiggleBone = TRUE;		// 지글본 무조건 활성화
	//if ( bJiggleBone )
	//{
	//	m_spJiggleBone->Import ( pAttBoneData->GetJiggleBone(), m_skeletonResThread.get(), m_fScale );			// DxJiggleBone
	//	m_spJiggleBoneColl->Import ( pAttBoneData->GetJiggleBoneColl(), m_skeletonResThread.get(), m_fScale );	// DxJiggleBoneColl
	//}

	//// Cloth
	//m_rClothColl = pAttBoneData->GetClothColl();

	//const DxSkinAniControlData::VEC_ANIMATION& vecANI = pAttBoneData->GetAniList();
	//size_t nSize = vecANI.size();
	//for ( size_t n=0; n<nSize; ++n )
	//{
	//	m_spSkinAniControlThread->InitAnimation( pd3dDevice, vecANI[n].get() );
	//}

	//m_spSkinAniControlThread->ClassifyAnimation ();

	//m_spSkinAniControlThread->VECNODE& vecANI = pAttBoneData->GetAniList ();
	//size_t nSize = vecANI.size();
	//for ( size_t n=0; n<nSize; ++n )
	//{
	//	m_spSkinAniControlThread->LoadAnimation ( vecANI[n]->m_pDxSkinAni->m_strCfgFile, pd3dDevice );

	//	m_EffAniPlayer.CheckCreateEff( vecANI[n]->m_pDxSkinAni );	// Aniplayer에 효과가 동작하도록 셋팅
	//}

	//m_spSkinAniControlThread->ClassifyAnimation ();

	////	Note : 초기 에니메이션 설정.
	////
	//m_spSkinAniControlThread->SETCURTIME ( fCurTime );
	//m_spSkinAniControlThread->SELECTANI ( MType, SType );

	//if ( !GETCURANIMNODE() )
	//{
	//	//m_spSkinAniControlThread->SELECTANI ( AN_PLACID, SType );
	//	m_spSkinAniControlThread->DEFAULTANI();
	//}

	//if ( !GETCURANIMNODE() )
	//{
 //       std::string ErrorMsg(
 //           sc::string::format(
 //               "DxAttBone::SetAttBoneData current animation null. %1% %2% [%3%/%4%]",
 //               pAttBoneData->GetFileName(),
 //               m_szFileName,
 //               MType,
 //               SType));
	//	CDebugSet::ToLogFile(ErrorMsg);
	//	if (m_vecAnim.size())
 //           DEFAULTANI();
	//}

	//m_spSkinAniControlThread->SetBoneScaleList(pAttBoneData->GetBoneScaleList());
}

void DxAttBone::SetRadiusHeight( DxAttBoneData* pAttBoneData )
{
	m_vMin = pAttBoneData->m_vMin;
	m_vMax = pAttBoneData->m_vMax;
	m_fHeight = pAttBoneData->m_fHeight;
}

HRESULT DxAttBone::FrameMove( const float fTime, 
							 const float fElapsedTime, 
							 std::tr1::shared_ptr<DxSkinAniControl>& spLinkSkinAniControl,
							 std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>& spThreadNeedData, 
							 BOOL bNoneOverFrame,
							 float fKeyJiggleWeight,
							 const BOOL bContinue/* =TRUE */, 
							 BOOL const bFreeze/* =FALSE */)
{
	if ( bFreeze )
		m_spSkinAniControlThread->FrameMove_AniTime ( 0.0f, bContinue );
	else
		m_spSkinAniControlThread->FrameMove_AniTime ( fElapsedTime, bContinue );

	m_sSkinEffData.Reset();		// Data Reset.
	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].FrameMove
		( 
			fTime, 
			fElapsedTime,
			m_spSkinAniControlThread,
			bNoneOverFrame
		);
	}

	m_spSkinAniControlThread->FrameMove_EffAniPlayer( this, m_sSkinEffData, fTime, fElapsedTime, bFreeze );

	// Note : Edge Color 디폴트 셋팅.
	m_EffKeepPlayer.FrameMove( fElapsedTime, m_sSkinEffData );


	////////////////////////////////////////////////////////////////////////////
	//// Render 에서 쓰였던것.
	//D3DXMATRIXA16	matScale;
	//D3DXMatrixIdentity( &matScale );
	//matScale._11 = m_fScale;
	//matScale._22 = m_fScale;
	//matScale._33 = m_fScale;
	//D3DXMatrixMultiply( &m_spSkinAniControlThread->GetWorld(), &matScale, &matRot );

	// TAG_ProcessAnimationThread_1
	// 등록만 한다.
	NSSkinAniThread::AddAniControl_AttBone( m_spSkinAniControlThread, spLinkSkinAniControl, spThreadNeedData, fKeyJiggleWeight, m_fScale, bNoneOverFrame );

	//if ( !m_spSkinAniControlThread->Render( m_spSkinAniControlThread->GetWorld(), fKeyJiggleWeight ) )
	//	return S_OK;
	//////////////////////////////////////////////////////////////////////////

	// 이전 키등록을 제일 마지막에 해야한다.
	m_spSkinAniControlThread->SetPrevKeyFrame();

	return S_OK;
}

HRESULT DxAttBone::FrameMoveForTool( const float fTime, const float fElapsedTime, const D3DXMATRIX& matWorld )
{
	BOOL bContinue(TRUE);
	BOOL bFreeze(FALSE);

	if ( bFreeze )
		m_spSkinAniControlThread->FrameMove_AniTime ( 0.0f, bContinue );
	else
		m_spSkinAniControlThread->FrameMove_AniTime ( fElapsedTime, bContinue );

	m_sSkinEffData.Reset();		// Data Reset.
	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].FrameMove
		( 
			fTime, 
			fElapsedTime, 
			m_spSkinAniControlThread,
			FALSE
		);
	}

	m_spSkinAniControlThread->FrameMove_EffAniPlayer( this, m_sSkinEffData, fTime, fElapsedTime, bFreeze );

	// Note : Edge Color 디폴트 셋팅.
	m_EffKeepPlayer.FrameMove( fElapsedTime, m_sSkinEffData );


	//////////////////////////////////////////////////////////////////////////
	// Render 에서 쓰였던것.
	D3DXMATRIXA16	matScale, matScaleWorld;
	D3DXMatrixIdentity( &matScale );
	matScale._11 = m_fScale;
	matScale._22 = m_fScale;
	matScale._33 = m_fScale;
	D3DXMatrixMultiply( &matScaleWorld, &matScale, &matWorld );

	std::tr1::shared_ptr<DxSkinAniControlThreadNeedData> spThreadNeedData(new DxSkinAniControlThreadNeedData(1));

	// TAG_ProcessAnimationThread_1
	// 등록만 한다.
	NSSkinAniThread::AddAniControl( m_spSkinAniControlThread, spThreadNeedData, matScaleWorld, FALSE );

	//if ( !m_spSkinAniControlThread->Render( m_spSkinAniControlThread->GetWorld(), fKeyJiggleWeight ) )
	//	return S_OK;
	//////////////////////////////////////////////////////////////////////////

	// 이전 키등록을 제일 마지막에 해야한다.
	m_spSkinAniControlThread->SetPrevKeyFrame();

	return S_OK;
}

// 렌더시 필수 요소가 있는가~?
BOOL DxAttBone::IsValidForRender()
{
	// Thread 작업되면서 Bone이 없을 수 있다.
	if ( !m_spSkinAniControlThread->GetSkeletonMaintain()->IsActiveBone() )
		return FALSE;

	// 캐릭터 툴이 아닐 때..
	if ( !g_bCHAR_EDIT_RUN ) 
	{
		// 기본 애니메이션이 있을 때 렌더링 한다.
		if ( !m_spSkinAniControlThread->IsValid_Skeleton_DefaultAni() )
			return FALSE;
	}

	return TRUE;
}

HRESULT DxAttBone::RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, const D3DXMATRIX &matRot, const BOOL bShadow, const BOOL bEff, const BOOL bReverse )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return S_OK;

    //UpdateLOD(*reinterpret_cast<const D3DXVECTOR3*>(&matRot._41), pd3dDevice);

	g_dwLOD = 0;
#ifdef USE_SKINMESH_LOD
	// Note : 그림자일 경우는 저급으로 뿌린다.
	if( bShadow )
	{
		g_dwLOD = 1;
	}
	else if( g_dwHIGHDRAW_NUM > 10 )
	{
		// Note : 한계가 넘었을 경우 최소한 보장해 줘야 하는 애들을 위함
		D3DXVECTOR3 vPos;
		D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		vPos.x = m_spSkinAniControlThread->GetWorld()._41;
		vPos.y = m_spSkinAniControlThread->GetWorld()._42;
		vPos.z = m_spSkinAniControlThread->GetWorld()._43;

		float fLengthSRC = DxViewPort::GetInstance().GetDistance();
		fLengthSRC += 30.f;
		fLengthSRC = fLengthSRC*fLengthSRC;

		D3DXVECTOR3 vDir = vPos - vFromPt;
		float fLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;
		if( fLength > fLengthSRC )	// 거리가 (카메라거리+30.f) 이상일 경우 LOD 레벨을 바꾼다.
		{
			g_dwLOD = 1;
		}
	}
	else
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		vPos.x = m_spSkinAniControlThread->GetWorld()._41;
		vPos.y = m_spSkinAniControlThread->GetWorld()._42;
		vPos.z = m_spSkinAniControlThread->GetWorld()._43;

		float fLengthSRC = DxViewPort::GetInstance().GetDistance();
		fLengthSRC += 150.f;
		fLengthSRC = fLengthSRC*fLengthSRC;

		D3DXVECTOR3 vDir = vPos - vFromPt;
		float fLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;
		if( fLength > fLengthSRC )	// 거리가 (카메라거리+150.f) 이상일 경우 LOD 레벨을 바꾼다.
		{
			g_dwLOD = 1;
		}
	}

	if( g_dwLOD==0 )
	{
		++g_dwHIGHDRAW_NUM;
	}
#endif

	int nStart=0, nEnd = 0, nAdd = 0;

	// 피스 순서를 거꾸로 그린다. ( 펫 전용 )
	if ( bReverse )
	{
		nStart = ATTBONE_SIZE;
		nEnd = -1;
		nAdd = -1;
	}
	else
	{
		nStart = 0;	
		nEnd = ATTBONE_SIZE;
		nAdd = 1;
	}

	int i = nStart;


	// Note : 조명의 변화는 받지 않는다.
	if( m_bWorldObj )
	{
		DWORD	dwNormalizeNormals;
		pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
		{
			SKIN_SPEC_SSS_INFO sSkinInfo;

			int i = nStart;
			while ( i != nEnd )
			{
				m_AttBoneArray[i].Render ( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), NULL, m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), sSkinInfo, bShadow, bEff );
				i += nAdd;
			}

			//i = nStart;
			//while ( i != nEnd )
			//{
			//	m_AttBoneArray[i].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
			//	i += nAdd;

			//}

			m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, fAlpha );
		}
		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );


		//	Note : Glow 
		for ( int i=0; i<ATTBONE_SIZE; ++i )
		{
			m_AttBoneArray[i].RenderGlow( pd3dDevice );
		}

		//	Note : Single Eff
		for ( int i=0; i<ATTBONE_SIZE; ++i )
		{
			m_AttBoneArray[i].RenderEff( pd3dDevice, m_fScale, &m_spSkinAniControlThread->GetWorld() );
		}

		return S_OK;
	}

	//D3DLIGHTQ	sSrcLight00;
	//D3DLIGHTQ	sDestLight00;

	//pd3dDevice->GetLight ( 0, &sSrcLight00 );

	//sDestLight00 = sSrcLight00;

	//sDestLight00.Ambient.r += sSrcLight00.Diffuse.r * 0.2f;
	//sDestLight00.Ambient.g += sSrcLight00.Diffuse.g * 0.2f;
	//sDestLight00.Ambient.b += sSrcLight00.Diffuse.b * 0.2f;
	//sDestLight00.Ambient.r += sSrcLight00.Ambient.r * 0.2f;
	//sDestLight00.Ambient.g += sSrcLight00.Ambient.g * 0.2f;
	//sDestLight00.Ambient.b += sSrcLight00.Ambient.b * 0.2f;
	//sDestLight00.Diffuse.r = sSrcLight00.Diffuse.r * 0.6f;
	//sDestLight00.Diffuse.g = sSrcLight00.Diffuse.g * 0.6f;
	//sDestLight00.Diffuse.b = sSrcLight00.Diffuse.b * 0.6f;

	////sDestLight01.Ambient.r += sSrcLight01.Diffuse.r * 0.15f;
	////sDestLight01.Ambient.g += sSrcLight01.Diffuse.g * 0.15f;
	////sDestLight01.Ambient.b += sSrcLight01.Diffuse.b * 0.15f;
	////sDestLight01.Diffuse.r = sSrcLight01.Diffuse.r * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.
	////sDestLight01.Diffuse.g = sSrcLight01.Diffuse.g * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.
	////sDestLight01.Diffuse.b = sSrcLight01.Diffuse.b * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.

	////sDestLight02.Ambient.r += sSrcLight02.Diffuse.r * 0.15f;
	////sDestLight02.Ambient.g += sSrcLight02.Diffuse.g * 0.15f;
	////sDestLight02.Ambient.b += sSrcLight02.Diffuse.b * 0.15f;
	////sDestLight02.Diffuse.r = sSrcLight02.Diffuse.r * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.
	////sDestLight02.Diffuse.g = sSrcLight02.Diffuse.g * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.
	////sDestLight02.Diffuse.b = sSrcLight02.Diffuse.b * 0.75f;	//0.9f;	// Item 때문에 색을 낮춘다.

	//pd3dDevice->SetLight ( 0, &sDestLight00 );

	//pd3dDevice->LightEnable( 1, FALSE );
	//pd3dDevice->LightEnable( 2, FALSE );
	//pd3dDevice->LightEnable( 3, FALSE );
	//pd3dDevice->LightEnable( 4, FALSE );
	//pd3dDevice->LightEnable( 5, FALSE );
	//pd3dDevice->LightEnable( 6, FALSE );
	//pd3dDevice->LightEnable( 7, FALSE );

	//D3DXVECTOR3 vCharPos( m_spSkinAniControlThread->GetWorld()._41, m_spSkinAniControlThread->GetWorld()._42, m_spSkinAniControlThread->GetWorld()._43 );
	//DxLightMan::GetInstance()->SetCharPLight( pd3dDevice, vCharPos );

	DWORD	dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	DWORD	dwNormalizeNormals;
	pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

	{
		{
			SKIN_SPEC_SSS_INFO sSkinInfo;

			switch( m_sSkinEffData.m_bNewHeadDRAW )
			{
			case TRUE:

				i = nStart;
				while ( i != nEnd )
				{
					m_AttBoneArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), NULL, m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), sSkinInfo, bShadow, bEff );
					i += nAdd;
				}	
				break;

			case FALSE:
				i = nStart;
				while ( i != nEnd )
				{
					m_AttBoneArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), NULL, m_sSkinEffData, m_spSkinAniControlThread->GetWorld(), sSkinInfo, bShadow, bEff );
					i += nAdd;
				}
				break;
			};

			//for ( int i=0; i<ATTBONE_SIZE; ++i )
			//{
			//	m_AttBoneArray[i].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), m_spSkinAniControlThread->GetWorld() );
			//}
			m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, fAlpha );
			m_EffKeepPlayer.Render( pd3dDevice );

			pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );
		}

		//	Note : Glow 
		for ( int i=0; i<ATTBONE_SIZE; ++i )
		{
			m_AttBoneArray[i].RenderGlow( pd3dDevice );
		}

		//	Note : Single Eff
		for ( int i=0; i<ATTBONE_SIZE; ++i )
		{
			m_AttBoneArray[i].RenderEff( pd3dDevice, m_fScale, &m_spSkinAniControlThread->GetWorld() );
		}
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	//pd3dDevice->SetLight ( 0, &sSrcLight00 );

	//pd3dDevice->LightEnable( 1, FALSE );
	//pd3dDevice->LightEnable( 2, FALSE );
	//pd3dDevice->LightEnable( 3, FALSE );
	//pd3dDevice->LightEnable( 4, FALSE );
	//pd3dDevice->LightEnable( 5, FALSE );



	return S_OK;

	//EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );

	//float	fDirectDiffuseFact = 0.6f;	//0.7f;	// Item 때문에 색을 낮춘다.
	//float	fDirectAmbientFact = 0.25f;	//0.3f;	// Item 때문에 색을 낮춘다.

	////	낮 일 경우와. 지형그림자가 켜져 있을 경우만 <지형에 따라 캐릭터를 어둡게> 한다.
	//if ( GLPeriod::GetInstance().IsOffLight() && DxEffectMan::GetInstance().GetSSDetail() )
	//{
	//	DxLandMan*	pLandMan = DxEffectMan::GetInstance().GetLandMan();
	//	if ( pLandMan )
	//	{
	//		D3DXVECTOR3	vDirect;
	//		vDirect = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
	//		vDirect.x *= -10000.f;
	//		vDirect.y *= -10000.f;
	//		vDirect.z *= -10000.f;

	//		BOOL		bCollision;
	//		D3DXVECTOR3	vCollision;
	//		D3DXVECTOR3	vPoint1(m_spSkinAniControlThread->GetWorld()._41+vDirect.x,m_spSkinAniControlThread->GetWorld()._42+8.f+vDirect.y,m_spSkinAniControlThread->GetWorld()._43+vDirect.z);
	//		D3DXVECTOR3	vPoint2(m_spSkinAniControlThread->GetWorld()._41,m_spSkinAniControlThread->GetWorld()._42+8.f,m_spSkinAniControlThread->GetWorld()._43);
	//		LPDXFRAME	pDxFrame = NULL;

	//		pLandMan->IsCollision ( vPoint1, vPoint2, vCollision, bCollision, pDxFrame );

	//		if ( bCollision )	// 무언가에 닿았다면... 어둡게 만든다.
	//		{
	//			fDirectDiffuseFact *= 0.7f;
	//			fDirectAmbientFact *= 0.7f;
	//		}
	//	}
	//}
}
//
//HRESULT DxAttBone::RenderSKIN( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matLocalRot )
//{
//	// Thread 작업되면서 Bone이 없을 수 있다.
//	if ( !m_spSkinAniControlThread->GetSkeletonMaintain()->IsActiveBone() )
//		return S_OK;
//
//	//D3DLIGHTQ	sSrcLight00;
//	//D3DLIGHTQ	sDestLight00;
//
//	//pd3dDevice->GetLight ( 0, &sSrcLight00 );
//
//	//sDestLight00 = sSrcLight00;
//
//	//sDestLight00.Ambient.r += sSrcLight00.Diffuse.r * 0.2f;
//	//sDestLight00.Ambient.g += sSrcLight00.Diffuse.g * 0.2f;
//	//sDestLight00.Ambient.b += sSrcLight00.Diffuse.b * 0.2f;
//	//sDestLight00.Ambient.r += sSrcLight00.Ambient.r * 0.2f;
//	//sDestLight00.Ambient.g += sSrcLight00.Ambient.g * 0.2f;
//	//sDestLight00.Ambient.b += sSrcLight00.Ambient.b * 0.2f;
//	//sDestLight00.Diffuse.r = sSrcLight00.Diffuse.r * 0.6f;
//	//sDestLight00.Diffuse.g = sSrcLight00.Diffuse.g * 0.6f;
//	//sDestLight00.Diffuse.b = sSrcLight00.Diffuse.b * 0.6f;
//
//	//pd3dDevice->SetLight ( 0, &sDestLight00 );
//
//	//pd3dDevice->LightEnable( 1, FALSE );
//	//pd3dDevice->LightEnable( 2, FALSE );
//	//pd3dDevice->LightEnable( 3, FALSE );
//	//pd3dDevice->LightEnable( 4, FALSE );
//	//pd3dDevice->LightEnable( 5, FALSE );
//	//pd3dDevice->LightEnable( 6, FALSE );
//	//pd3dDevice->LightEnable( 7, FALSE );
//
//	//D3DXVECTOR3 vCharPos( matLocalRot._41, matLocalRot._42, matLocalRot._43 );
//	//DxLightMan::GetInstance()->SetCharPLight( pd3dDevice, vCharPos );
//
//	DWORD	dwFogEnable;
//	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
//
//	{
//		DWORD	dwNormalizeNormals;
//		pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormalizeNormals );
//		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
//
//		SKIN_SPEC_SSS_INFO sSkinInfo;
//
//		switch( m_sSkinEffData.m_bNewHeadDRAW )
//		{
//		case TRUE:
//			for ( int i=1; i<ATTBONE_SIZE; ++i )
//			{
//				m_AttBoneArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), m_sSkinEffData, matLocalRot, sSkinInfo, FALSE, TRUE );
//			}
//			break;
//
//		case FALSE:
//			for ( int i=0; i<ATTBONE_SIZE; ++i )
//			{
//				m_AttBoneArray[i].Render( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), m_sSkinEffData, matLocalRot, sSkinInfo, FALSE, TRUE );
//			}
//			break;
//		};
//
//		//for ( int i=0; i<ATTBONE_SIZE; ++i )
//		//{
//		//	m_AttBoneArray[i].RenderGhosting ( pd3dDevice, m_spSkinAniControlThread.get(), matLocalRot );
//		//}
//		m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, 1.f );
//		m_EffKeepPlayer.Render( pd3dDevice );
//
//		pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormalizeNormals );
//	}
//
//	//	Note : Glow 
//	for ( int i=0; i<ATTBONE_SIZE; ++i )
//	{
//		m_AttBoneArray[i].RenderGlow( pd3dDevice );
//	}
//
//	//	Note : Single Eff
//	for ( int i=0; i<ATTBONE_SIZE; ++i )
//	{
//		m_AttBoneArray[i].RenderEff( pd3dDevice, m_fScale, &matLocalRot );
//	}
//
//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
//
//	//pd3dDevice->SetLight ( 0, &sSrcLight00 );
//
//	//pd3dDevice->LightEnable( 1, FALSE );
//	//pd3dDevice->LightEnable( 2, FALSE );
//	//pd3dDevice->LightEnable( 3, FALSE );
//	//pd3dDevice->LightEnable( 4, FALSE );
//	//pd3dDevice->LightEnable( 5, FALSE );
//
//	return S_OK;
//}

HRESULT DxAttBone::RenderShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, const D3DXMATRIX& matViewProj, const BOOL bDay )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return S_OK;

	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	float fHeight=0.f;
	if( !bDay )
	{
		fHeight = -matRot._42;
	}

	if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_1_1 )	// 캐릭터가 Material System 으로 동작될 때는 이렇게 쓴다.
	{
		for ( int i=0; i<ATTBONE_SIZE; ++i )
		{
			m_AttBoneArray[i].RenderShadowForMaterial( pd3dDevice, matViewProj, FALSE );
		}
	}
	else	// 하드웨어 성능이 무지 나쁜경우.
	{
		for ( int i=0; i<ATTBONE_SIZE; ++i )
		{
			m_AttBoneArray[i].RenderShadow( pd3dDevice, m_spSkinAniControlThread->GetSkeletonMaintain(), fHeight );
		}
	}	

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	return S_OK;
}

HRESULT DxAttBone::RenderShadowForMaterialSystem( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return S_OK;

	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	DWORD dwAlphaTestEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );	

	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].RenderShadowForMaterial( pd3dDevice, matViewProj, TRUE );
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );

	return S_OK;
}

void DxAttBone::RenderBone( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_spSkinAniControlThread->RenderBone( pd3dDevice );
}

void DxAttBone::SetPosOffset( const D3DXVECTOR3& vPosOffset )
{
	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].SetPosOffset( vPosOffset );
	}
}

void DxAttBone::RenderMaterialOpaque( const LPDIRECT3DDEVICEQ pd3dDevice, 
									 float fAlpha, 
									 const BOOL bShadow, 
									 const BOOL bEff,
									 const DxClothColl* pClothColl,
									 const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
									 float fAttParentBoneScale )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	//UpdateLOD(*reinterpret_cast<const D3DXVECTOR3*>(&m_spSkinAniControlThread->GetWorld()._41), pd3dDevice);

	g_dwLOD = 0;
#ifdef USE_SKINMESH_LOD
	// Note : 그림자일 경우는 저급으로 뿌린다.
	if( bShadow )
	{
		g_dwLOD = 1;
	}
	else if( g_dwHIGHDRAW_NUM > 10 )
	{
		// Note : 한계가 넘었을 경우 최소한 보장해 줘야 하는 애들을 위함
		D3DXVECTOR3 vPos;
		D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		vPos.x = m_spSkinAniControlThread->GetWorld()._41;
		vPos.y = m_spSkinAniControlThread->GetWorld()._42;
		vPos.z = m_spSkinAniControlThread->GetWorld()._43;

		float fLengthSRC = DxViewPort::GetInstance().GetDistance();
		fLengthSRC += 30.f;
		fLengthSRC = fLengthSRC*fLengthSRC;

		D3DXVECTOR3 vDir = vPos - vFromPt;
		float fLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;
		if( fLength > fLengthSRC )	// 거리가 (카메라거리+30.f) 이상일 경우 LOD 레벨을 바꾼다.
		{
			g_dwLOD = 1;
		}
	}
	else
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		vPos.x = m_spSkinAniControlThread->GetWorld()._41;
		vPos.y = m_spSkinAniControlThread->GetWorld()._42;
		vPos.z = m_spSkinAniControlThread->GetWorld()._43;

		float fLengthSRC = DxViewPort::GetInstance().GetDistance();
		fLengthSRC += 150.f;
		fLengthSRC = fLengthSRC*fLengthSRC;

		D3DXVECTOR3 vDir = vPos - vFromPt;
		float fLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;
		if( fLength > fLengthSRC )	// 거리가 (카메라거리+150.f) 이상일 경우 LOD 레벨을 바꾼다.
		{
			g_dwLOD = 1;
		}
	}

	if( g_dwLOD==0 )
	{
		++g_dwHIGHDRAW_NUM;
	}
#endif

	int nStart=0, nEnd = 0, nAdd = 0;

	// 피스 순서를 거꾸로 그린다. ( 펫 전용 )
	{
		nStart = 0;	
		nEnd = ATTBONE_SIZE;
		nAdd = 1;
	}

	int i = nStart;


	SKIN_SPEC_SSS_INFO sSkinInfo;

	//////////////////////////////////////////////////////////////////////////
	// [shhan][2014.05.16] Cloth Collision 같은 경우는 상위의 DxSkinChar 것만 사용하도록 한다.
	//////////////////////////////////////////////////////////////////////////
	while ( i != nEnd )
	{
		m_AttBoneArray[i].RenderMaterialOpaque
		( 
			pd3dDevice, 
			*m_spSkinAniControlThread->GetSkeletonMaintain(), 
			NULL,
			m_sSkinEffData,
			fAlpha, 
			sSkinInfo,
			&m_spSkinAniControlThread->GetWorld(), 
			m_spSkinAniControlThread->GetWorld(), 
			m_fScale, 
			pClothColl,
			pvecClothCollSpheres,
			fAttParentBoneScale,
			bEff
		);
		i += nAdd;
	}
}

void DxAttBone::RenderMaterialHardAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	DWORD dwAlphaTestEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );

	int nStart=0, nEnd = 0, nAdd = 0;
	{
		nStart = 0;	
		nEnd = ATTBONE_SIZE;
		nAdd = 1;
	}

	int i = nStart;


	SKIN_SPEC_SSS_INFO sSkinInfo;

	i = nStart;
	while ( i != nEnd )
	{
		m_AttBoneArray[i].RenderMaterialHardAlpha( pd3dDevice, fAlpha, sSkinInfo );
		i += nAdd;
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, dwAlphaTestEnable );
}

void DxAttBone::RenderMaterialSoftAlpha( const LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	DWORD dwAlphaBlendEnable(0L);
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	int nStart=0, nEnd = 0, nAdd = 0;
	{
		nStart = 0;	
		nEnd = ATTBONE_SIZE;
		nAdd = 1;
	}

	int i = nStart;

	SKIN_SPEC_SSS_INFO sSkinInfo;

	i = nStart;
	while ( i != nEnd )
	{
		m_AttBoneArray[i].RenderMaterialSoftAlpha( pd3dDevice, fAlpha, sSkinInfo );
		i += nAdd;
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
}

void DxAttBone::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const float fAlpha )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_1_1 )
		return;

	//	Note : Single Eff
	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].RenderEff( pd3dDevice, m_fScale, &m_spSkinAniControlThread->GetWorld() );
	}

	m_spSkinAniControlThread->Render_EffAniPlayer( pd3dDevice, this, fAlpha );
}

void DxAttBone::RenderMaterialAdd( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	// RenderMaterialAdd 는 옵션과는 영향이 없어야 한다.
	//if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
	//	return;

	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].RenderMaterialAdd( pd3dDevice );
	}
}

void DxAttBone::RenderMaterialGlow( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return;

	if ( RENDERPARAM::g_emCharacterQulity < TnL_CHAR_PS_2_0 )
		return;

	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].RenderMaterialGlow( pd3dDevice );
	}
}

BOOL DxAttBone::RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, size_t unGhostingIndex )
{
	// 렌더시 필수 요소가 있는가~?
	if ( !IsValidForRender() )
		return FALSE;

	SKIN_SPEC_SSS_INFO sSkinInfo;

	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		if ( !m_AttBoneArray[i].RenderMaterialGhosting( pd3dDevice, m_sSkinEffData, sSkinInfo, fAlpha, unGhostingIndex ) )
			return FALSE;
	}

	return TRUE;
}

HRESULT DxAttBone::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : m_AttBoneArray, m_EffAniPlayer 은 다른 경로에서 초기화를 해 준다.

	return S_OK;
}

HRESULT DxAttBone::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].RestoreDeviceObjects ( pd3dDevice );
	}

	m_spSkinAniControlThread->RestoreDeviceObjects();

	return S_OK;
}

HRESULT DxAttBone::InvalidateDeviceObjects ()
{
	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].InvalidateDeviceObjects ();
	}

	m_spSkinAniControlThread->InvalidateDeviceObjects();

	return S_OK;
}

HRESULT DxAttBone::DeleteDeviceObjects ()
{
	for ( int i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].DeleteDeviceObjects ();
	}

	m_spSkinAniControlThread->DeleteDeviceObjects();

	return S_OK;
}

//------------------------------------------------------------
//				E	f	f		K	e	e	p
void DxAttBone::CreateEff( DxEffKeepDataMain* pEff )
{
	m_EffKeepPlayer.CreateEff( pEff );
}

void DxAttBone::DeleteEff( DxEffKeepDataMain* pEff )
{

}

void DxAttBone::Reset()
{
	m_EffKeepPlayer.Reset();
}


void DxAttBone::UpdateLOD(const D3DXVECTOR3& vPos, LPDIRECT3DDEVICEQ pd3dDevice)
{
    //for( DWORD i=0; i<ATTBONE_SIZE; ++i )
    //{
    //    m_AttBoneArray[i].UpdateLOD(vPos, pd3dDevice);
    //}
}

//////////////////////////////////////////////////////////////////////////
void DxAttBone::GhostingON( BOOL bGhosting, float fLifeTime )
{
	for( DWORD i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].GhostingON( bGhosting, fLifeTime );
	}
}

//////////////////////////////////////////////////////////////////////////
void DxAttBone::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	for( DWORD i=0; i<ATTBONE_SIZE; ++i )
	{
		m_AttBoneArray[i].SetWorldBattleFlag( pWorldBattleFlag );
	}
}

//////////////////////////////////////////////////////////////////////////
// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
BOOL DxAttBone::SetPhysX_Cloth( BOOL bOnOff )
{
	for( DWORD i=0; i<ATTBONE_SIZE; ++i )
	{
		if ( !m_AttBoneArray[i].SetPhysX_Cloth( bOnOff ) )
			return FALSE;
	}

	return TRUE;
}

const TCHAR* DxAttBone::GetBoneName() const
{
	if ( !m_spSkinAniControlThread.get() )
		return NULL;

	return m_spSkinAniControlThread->GetSkeletonName();
}