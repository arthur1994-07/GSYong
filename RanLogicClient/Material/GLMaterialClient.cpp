#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/GlobalParam.h"

#include "../../RanLogic/Crow/GLCrowDataMan.h"

#include "../Land/GLLandManClient.h"
#include "../Effect/GLFactEffect.h"
#include "../GLGaeaClient.h"
#include "./GLMaterialClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLMaterialClient::GLMaterialClient(GLGaeaClient* const pGaeaClient)
	: ClientActor(pGaeaClient, CROW_MATERIAL)
	, m_pd3dDevice(NULL)
	, m_pSkinChar(NULL)

	, m_vMaxOrg(8,20,8)
	, m_vMinOrg(-8,0,-8)

	, m_vMax(8,20,8)
	, m_vMin(-8,0,-8)
	, m_fHeight(20.f)

	, m_dwCeID(0)
	, m_pLandManClient(NULL)

	, m_fAge(0.0f)
	, m_fIdleTime(0.0f)

	, m_spWorldLight(new ActorWorldLight)
	, m_spNameCollision(new ActorNameCollision(EMCC_CULL_NONE))
{
}

GLMaterialClient::~GLMaterialClient(void)
{
	m_pd3dDevice = NULL;

	ClientActor::Reset();	

	m_pLandManClient = NULL;
	//m_pGlobNode = NULL;
	//m_pQuadNode = NULL;
	//m_pCellNode = NULL;
	
	SAFE_DELETE(m_pSkinChar);
}

HRESULT GLMaterialClient::CreateMaterial ( GLLandManClient *pLandManClient, SDROP_MATERIAL *pCrowDrop, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	GASSERT(pLandManClient);

	m_pLandManClient = pLandManClient;
	m_pd3dDevice = pd3dDevice;

	//	Note : 몹의 설정 정보를 가져옴.
	m_pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pCrowDrop->sNativeID );
	if( !m_pCrowData )
	{
		SNATIVEID sMobID = pCrowDrop->sNativeID;
		sc::writeLogError(
            sc::string::format(
                "GLMaterialClient::CreateMaterial()-GLCrowDataMan::GetCrowData() [%1%/%2%] 가져오기 실패.", sMobID.wMainID, sMobID.wSubID));
		return E_FAIL;
	}

	m_sNativeID = m_pCrowData->sNativeID;

	//	Note : 이동 제어 초기화.	
	ClientActor::InitializeActorMove(pCrowDrop->vPos, m_pLandManClient->GetNaviMesh(), pCrowDrop->vDir);	

	DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pCrowData->GetSkinObjFile(), m_pd3dDevice );
	if ( !pCharData )
	{
		if( strlen( m_pCrowData->GetSkinObjFile() ) )
		{
			sc::writeLogError(
                sc::string::format(
                    "GLCrowClient::CreateCrow()-DxSkinCharDataContainer::LoadData() [%1%] 가져오기 실패.", m_pCrowData->GetSkinObjFile()));
		}
		return E_FAIL;
	}

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;

	m_pSkinChar->SetCharData ( pCharData, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	//	Note : 현정보 적용.
	m_dwGaeaID = pCrowDrop->dwGlobID;
	m_dwCeID = pCrowDrop->dwCeID;

	switch ( pCrowDrop->emAction )
	{
	case GLAT_MOVE:
		break;

	case GLAT_FALLING:
		TurnAction ( GLAT_FALLING );
		break;

	case GLAT_DIE:
		TurnAction ( GLAT_DIE );
		break;
	};
	
	//	Note : 생성 효과 발생
	//
	if ( pCrowDrop->dwFLAGS&SDROP_MATERIAL::CROW_GEM )
	{
		D3DXMATRIX matEffect;
		D3DXMatrixTranslation ( &matEffect, pCrowDrop->vPos.x, pCrowDrop->vPos.y, pCrowDrop->vPos.z );

		std::string strGEM_EFFECT = GLCONST_CHAR::strMOB_GEM_EFFECT.c_str();
		if ( !m_pCrowData->m_sAction.m_strBirthEffect.empty() )	strGEM_EFFECT = m_pCrowData->m_sAction.m_strBirthEffect;

		STARGETID sTargetID(m_pCrowData->m_emCrow, m_dwGaeaID, pCrowDrop->vPos);
		DxEffGroupPlayer::GetInstance().NewEffGroup
		(
			strGEM_EFFECT.c_str(),
			matEffect,
			&sTargetID,
			TRUE,
			FALSE
		);
	}

	if ( !m_pSkinChar->GETCURANIMNODE() )
	{
		m_pSkinChar->DEFAULTANI();
 	}

	// 현재 로딩중 일 수도 있다.
	//if ( !m_pSkinChar->GETCURANIMNODE() )
	//{
	//	sc::writeLogError(
	//		sc::string::format(
	//		"current animation node null point error [%1%] [M %2% S %3%]",
	//		m_pSkinChar->GetFileName(),
	//		m_pSkinChar->GETCURMTYPE(),
	//		m_pSkinChar->GETCURSTYPE()));
	//}

	return S_OK;
}

WORD GLMaterialClient::GetBodyRadius ()
{
	return m_pCrowData->m_sAction.m_wBodyRadius;
}

// BOOL GLMaterialClient::IsValidBody ()
// {
// 	return ( m_Action < GLAT_FALLING );
// }
// 
// BOOL GLMaterialClient::IsDie ()
// {
// 	if ( IsACTION(GLAT_DIE) )
// 		return TRUE;
// 
// 	return FALSE;
// }

BOOL GLMaterialClient::IsHaveVisibleBody ()
{
	return ( m_pCrowData->GetSkinObjFile() && (strlen(m_pCrowData->GetSkinObjFile())!=0) );
}

void GLMaterialClient::TurnAction ( EMACTIONTYPE toAction )
{
	//	Note : 이전 액션 취소(쓰러질때와 죽을때는 제외)
	//
	switch ( GetAction() )
	{
	case GLAT_IDLE:
		break;

	case GLAT_FALLING:
		if ( toAction==GLAT_FALLING )	return;
		break;
		
	case GLAT_DIE:
		if ( toAction==GLAT_FALLING || toAction==GLAT_DIE )	return;
		break;

	default:
		break;
	};

	//	Note : 액션 초기화.
	//
	SetAction(toAction);	

	switch ( toAction )
	{
	case GLAT_IDLE:
		m_fIdleTime = 0.0f;
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	case GLAT_FALLING:
		break;

	case GLAT_DIE:
		{
			D3DXVECTOR3		vNORMAL(0.f,1.f,0.f);
			D3DXVECTOR3		vSTRIKE(0.f,0.f,0.f);

			bool bBLOOD_TRACE(false);
			for ( int i=0; i<PIECE_SIZE; ++i )
			{
				PDXCHARPART pCharPart = m_pSkinChar->GetPiece(i);
				if ( !pCharPart )	continue;

				if ( pCharPart->GetTracePos(STRACE_BLOOD01) )
				{
					bBLOOD_TRACE = true;
					pCharPart->CalculateVertexInflu ( STRACE_BLOOD01, vSTRIKE, vNORMAL );
				}
			}

			const D3DXVECTOR3& vPosition = GetPosition();
			if ( !bBLOOD_TRACE )
				vSTRIKE = vPosition;

			D3DXMATRIX matEffect;
			D3DXMatrixTranslation ( &matEffect, vSTRIKE.x, vSTRIKE.y, vSTRIKE.z );

			STARGETID sTargetID(m_pCrowData->m_emCrow, m_dwGaeaID, vPosition);
			DxEffGroupPlayer::GetInstance().NewEffGroup
			(
				GLCONST_CHAR::strMOB_DIE_BLOOD.c_str(),
				matEffect,
				&sTargetID,
				TRUE,
				FALSE
			);

			DxEffcharDataMan::GetInstance().PutEffect(
				m_pSkinChar, GLCONST_CHAR::strERASE_EFFECT.c_str(), &GetDirect());
		}
		break;

	default:
		GASSERT("GLCharacter::TurnAction() 준비되지 않은 ACTION 이 들어왔습니다.");
		break;
	};

}


HRESULT GLMaterialClient::UpdateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	BOOL bLowSP(false);

	BOOL bFreeze = FALSE;
	switch ( GetAction() )
	{
	case GLAT_IDLE:
		{
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_PUSHPULL:
		break;

	case GLAT_FALLING:
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		if ( m_pSkinChar->IsANI_ISENDANIM( AN_DIE, AN_SUB_NONE ) )
			TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		bFreeze = TRUE;
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		m_pSkinChar->TOENDTIME();
		break;
	case GLAT_GATHERING:
		break;
	};

	//	Note : Mob 스킨 업데이트.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( GetAction() )
	{
	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		break;
	};

	m_pSkinChar->SetPosition ( GetPosition() );
	m_pSkinChar->FrameMove ( fTime, fSkinAniElap, FALSE, m_matTrans, TRUE, bFreeze );

	return S_OK;
}

BOOL GLMaterialClient::IsCollisionVolume ()
{
	const CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume();
	if ( COLLISION::IsCollisionVolume ( CV, m_vMax, m_vMin ) )	return TRUE;

	return FALSE;
}

HRESULT GLMaterialClient::FrameMove ( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;
	GASSERT(m_pCrowData);
	GASSERT(m_pLandManClient);

	// 현재 위치 업데이트	
	const D3DXVECTOR3& vPosition = ClientActor::RefreshPosition(0.0f);
	
	//	Note : 현재 위치와 방향으로 Transform 메트릭스 계산.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
	float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	SetTransMatrix(D3DXMATRIX(matYRot * matTrans));	

	//	Note : 에니메이션, 스킨 업데이트.
	//
	UpdateAnimation ( fTime, fElapsedTime );

	//	Note : AABB 계산.
	//
	m_vMax = vPosition + m_vMaxOrg;
	m_vMin = vPosition + m_vMinOrg;
	

	//////////////////////////////////////////////////////////////////////////
	//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spNameCollision->InitData( spLandMan );
	NSSkinAniThread::AddNameCollision( m_spNameCollision, DxViewPort::GetInstance().GetFromPt(), vPosition + D3DXVECTOR3(0.0f, 8.0f, 0.0f) );


	//////////////////////////////////////////////////////////////////////////
	//			ActorWorldLight 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spWorldLight->InitData( spLandMan );
	NSSkinAniThread::AddActorWorldLight( m_spWorldLight, vPosition );


	return S_OK;
}

float GLMaterialClient::GetDirection ()
{
	return DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
}

HRESULT GLMaterialClient::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
        return S_OK;

	if ( m_spWorldLight->IsNotCalculateColor() )
		return S_OK;

	//DxEffcharDataMan::GetInstance().OutEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );

	const D3DXMATRIX& matWorld = GetTransMatrix();
	//hr = m_pSkinChar->Render ( pd3dDevice, TRUE, 1.f, matWorld );
	//if ( FAILED(hr) )
 //       return hr;

	NSMaterialSkinManager::SetMob( TRUE );

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		m_pSkinChar->RenderFixed( pd3dDevice, TRUE, 1.f );
	}
	else
	{
		m_pSkinChar->SetWorldLight( m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult );
		m_pSkinChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL  ? TRUE : FALSE );
		m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
		m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
	}

	DxLandMan* pLandMan(NULL);
	if ( m_pLandManClient )
	{
		pLandMan = m_pLandManClient->GetLandMan();
	}

	//if ( pLandMan )
	//{
	//	m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(matWorld._41,matWorld._42,matWorld._43) );
	//}

	return S_OK;
}

HRESULT GLMaterialClient::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
		return S_OK;

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
	}
	else
	{
		m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
	}

	return S_OK;
}

HRESULT GLMaterialClient::RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
		return S_OK;

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
	}
	else
	{
		m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 1.f );
	}

	return S_OK;
}

HRESULT GLMaterialClient::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD )
{
    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	HRESULT hr;
	if ( isAction(GLAT_DIE) )
        return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
        return S_OK;
	
	//	Note : 그림자 랜더링.
	//
	hr = DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, GetTransMatrix(), pd3dDevice, bWorldSystemOLD );
	if ( FAILED(hr) )
        return hr;

	return S_OK;
}

HRESULT GLMaterialClient::RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	HRESULT hr;
	if ( isAction(GLAT_DIE) )
		return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
		return S_OK;

	//	Note : 그림자 랜더링.
	//
	hr = DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight ( m_pSkinChar, GetTransMatrix(), pd3dDevice, bUsedMaterialSystem, dwSlot );
	if ( FAILED(hr) )
		return hr;

	return S_OK;
}

HRESULT GLMaterialClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	return S_OK;
}

HRESULT GLMaterialClient::InvalidateDeviceObjects ()
{
	m_pSkinChar->InvalidateDeviceObjects ();
	return S_OK;
}

void GLMaterialClient::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_ACTION_BRD:
		{
			GLMSG::SNET_ACTION_BRD *pNetMsg = (GLMSG::SNET_ACTION_BRD *)nmg;
			TurnAction ( pNetMsg->emAction );
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCrowClient::MsgProcess unknown message type %d/%d", nmg->nType, nmg->nType-NET_MSG_GCTRL );
		break;
	};
}

BOOL GLMaterialClient::IsVisibleDisplayName() const
{
	if ( m_spNameCollision->GetCollsion() )
		return FALSE;

	return TRUE;
}


