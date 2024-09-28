#include "../pch.h"

#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/GlobalParam.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Char/GLCharClient.h"
#include "../Char/GLCharacter.h"
#include "../Land/GLLandManClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Strike/GLStrikeM.h"
#include "../GLGaeaClient.h"
#include "./GLAnyPet.h"

GLAnyPet::GLAnyPet(GLGaeaClient* pGaeaClient) :
    m_pGaeaClient(pGaeaClient),
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),

	m_vDir(D3DXVECTOR3(0,0,-1)),
	m_vDirOrig(D3DXVECTOR3(0,0,-1)),
	m_vPos(D3DXVECTOR3(0,0,0)),
	m_dwActionFlag(EM_PETACT_STOP),

	m_vTarPos(D3DXVECTOR3(0,0,0)),

	m_vMax(6,20,6),
	m_vMin(-6,0,-6),
	m_fHeight(20.f),

	m_vMaxOrg(6,20,6),
	m_vMinOrg(-6,0,-6),

	m_wAniSub(1),
	m_fAttackTimer( 0.0f ),
	m_fFunyTimer( 0.0f )

	, m_spWorldLight(new ActorWorldLight)
	, m_spNameCollision(new ActorNameCollision(EMCC_CULL_NONE))
{
	m_sPetSkinPackState.Init();
}

GLAnyPet::~GLAnyPet(void)
{
	DeleteDeviceObjects ();
}

void GLAnyPet::ASSIGN ( PSDROPPET pDropPet )
{
	m_emTYPE			= pDropPet->m_emTYPE;
	m_dwGUID			= pDropPet->m_dwGUID;
	m_sPetID			= pDropPet->m_sPetID;
	m_dwOwner		= pDropPet->m_dwOwner;
	m_wStyle			= pDropPet->m_wStyle;
	m_wColor			= pDropPet->m_wColor;
	m_nFull				= pDropPet->m_nFull;
	m_vPos				= pDropPet->m_vPos;
	m_vDir				= pDropPet->m_vDir;
	m_vTarPos			= pDropPet->m_vTarPos;

	m_sMapID			= pDropPet->m_sMapID;
	m_dwCellID			= pDropPet->m_dwCellID;

	m_fRunSpeed			= pDropPet->m_fRunSpeed;
	m_fWalkSpeed		= pDropPet->m_fWalkSpeed;

	m_dwActionFlag		= pDropPet->m_dwActionFlag;
		
	m_wAniSub			= pDropPet->m_wAniSub;

	m_DbNum			= pDropPet->m_dwPetID;

	m_dwOwner			= pDropPet->m_dwOwner;

	m_wMaxSkillSlot		= pDropPet->m_dwMaxSkillSlot;

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = pDropPet->m_PutOnItems[i];
	}

	for ( WORD i = 0; i < MAX_PETSKILLSLOT; ++i )
	{
		m_sActiveSkillID[i] = pDropPet->m_sActiveSkillID[i];
	}

	SetName(pDropPet->m_szName);
}

HRESULT GLAnyPet::Create ( PSDROPPET pDropPet, boost::shared_ptr<NavigationMesh> spNavi, LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 기본 데이터 설정
	ASSIGN ( pDropPet );

	m_pd3dDevice = pd3dDevice;

	m_sPetSkinPackState.bUsePetSkinPack = pDropPet->m_petSkinPackData.bUsePetSkinPack;
	m_sPetSkinPackState.petSkinMobID    = pDropPet->m_petSkinPackData.sMobID;
	m_sPetSkinPackState.fPetScale	    = pDropPet->m_petSkinPackData.fScale;

	// 초기 위치와 방향 설정
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );
	m_matTrans = matYRot * matTrans;

	if ( spNavi )
	{
		m_actorMove.Create(spNavi, m_vPos);
	}

	if ( FAILED ( SkinLoad ( pd3dDevice ) ) )
		return E_FAIL;

	UpdateSuit ();

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	// 만약 움직이고 있다면... 내부메시지 발생
	if ( IsSTATE ( EM_PETACT_MOVE ) )
	{
		GLMSG::SNETPET_GOTO_BRD NetMsgBrd(m_vPos,m_vTarPos,m_dwActionFlag);
		NetMsgBrd.dwGUID =  m_dwGUID;
		MsgGoto ( ( NET_MSG_GENERIC* ) &NetMsgBrd );
	}

	// 퍼니액션중이면...
	if ( IsSTATE ( EM_PETACT_FUNNY ) )
	{
		m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub );
	}

	// 스킬 발동중이면 이펙트 표시
	for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
	{
		if ( m_sActiveSkillID[i] != NATIVEID_NULL () )
		{
			std::tr1::shared_ptr<GLCharClient> pTarChar = m_pGaeaClient->GetChar(m_dwOwner);
			// 주인에게 붙는 이펙트 발동(변경)
			if ( pTarChar )
			{
				STARGETID sTargetID( CROW_PC, pTarChar->GetGaeaID(), pTarChar->GetPosition () );
				FACTEFF::NewSkillFactEffect ( sTargetID, pTarChar->GetCharSkin (), m_sActiveSkillID[i], pTarChar->GetTransMatrix(), sTargetID.vPos );
			}
		}
	}

	// SAD 액션중이면
	if ( IsSTATE ( EM_PETACT_SAD ) )
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
		std::tr1::shared_ptr<GLCharClient> pChar = pLandMan->GetChar(m_dwOwner);
		if (pChar)
		{
			// SAD 애니메이션
			m_pSkinChar->SELECTANI ( AN_GESTURE, AN_SUB_04 );

			// 방향 전환
			D3DXVECTOR3 vDirection = pChar->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;
		}
	}

	return S_OK;
}

HRESULT GLAnyPet::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SNATIVEID loadPetID = m_sPetID;
	if( IsUsePetSkinPack() )
	{
		loadPetID = m_sPetSkinPackState.petSkinMobID;
	}

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( loadPetID );
	if (!pCrowData)
        return E_FAIL;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		pd3dDevice );

	if ( !pSkinChar )	return E_FAIL;

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	
	m_pSkinChar->SELECTANI ( AN_GUARD_N, (EMANI_SUBTYPE)0 );

	if ( m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, (EMANI_SUBTYPE)0 ) == 0 )
	{
		m_pSkinChar->DEFAULTANI();
	}

	// 현재 펫 에니메이션중에 플레이가 불가능한 애니메이션을 찾는다.
	if( IsUsePetSkinPack() )
	{
		if( !m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, AN_SUB_NONE )  ) m_sPetSkinPackState.bNotFoundGuard = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_RUN, AN_SUB_NONE ) )		 m_sPetSkinPackState.bNotFoundRun = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_WALK, AN_SUB_NONE ) )	 m_sPetSkinPackState.bNotFoundWalk = TRUE;	
		//		m_pSkinChar->GETANIAMOUNT( AN_GESTURE, AN_SUB_NONE ) )
		if( !m_pSkinChar->GETANIAMOUNT( AN_ATTACK, AN_SUB_NONE ) )	 m_sPetSkinPackState.bNotFoundAttack = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_GESTURE, AN_SUB_04 ) )	 m_sPetSkinPackState.bNotFoundSad	 = TRUE;
	}


	return S_OK;
}

HRESULT GLAnyPet::UpdateAnimation ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT	GLAnyPet::UpdatePetState ( float fTime, float fElapsedTime )
{
	// 팻이 공격모션 중이면
	if( IsSTATE ( EM_PETACT_ATTACK ) )
	{
		if( true == m_pSkinChar->IsAni( AN_ATTACK ) )
		{
			m_fAttackTimer += fElapsedTime;
			if( m_pSkinChar->GETCURANITIME() < m_fAttackTimer )
			{
				ReSetAllSTATE();
				SetSTATE( EM_PETACT_STOP );
				m_actorMove.Stop ();
				m_pSkinChar->SELECTANI( AN_GUARD_N, AN_SUB_NONE );
			}
		}
	}

	if ( IsSTATE ( EM_PETACT_STOP ) )
	{
		// 이동 메시지를 기다린다.
	}

	if ( IsSTATE ( EM_PETACT_MOVE ) )
	{
		if ( !m_actorMove.PathIsActive() )
		{
			ReSetSTATE ( EM_PETACT_MOVE );
			SetSTATE ( EM_PETACT_STOP );
			m_actorMove.Stop ();
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}
	}

	if( IsSTATE( EM_PETACT_FUNNY ) )
	{
		if( true == m_pSkinChar->IsAni( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub ) )
		{
			m_fFunyTimer += fElapsedTime;
			if( m_pSkinChar->GETCURANITIME() < m_fFunyTimer )
			{
				ReSetAllSTATE();
				SetSTATE( EM_PETACT_STOP );
				m_actorMove.Stop ();
				m_pSkinChar->SELECTANI( AN_GUARD_N, AN_SUB_NONE );
			}
		}
	}

	if ( IsSTATE ( EM_PETACT_WAIT ) )
	{
		// 다음 메시지를 기다린다.
	}
	
	return S_OK;
}

HRESULT GLAnyPet::FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime )
{
	HRESULT hr(S_OK);

	GLLandManClient* pLandManClient = m_pGaeaClient->GetLandManClient();
	if ( !pLandManClient )
		return S_OK;

	UpdatePetState ( fTime, fElapsedTime );
	
	// ============== Update about Actor (Begin) ============== /

	hr = m_actorMove.Update ( pLandManClient->GetvecLayerCollision(), fElapsedTime );
	if ( FAILED(hr) )	return E_FAIL;

	// ============== Update about Actor (end)   ============== /

	// ============== Update about Skin (Begin) ============== /

	D3DXMATRIX matTrans, matYRot, matScale;

	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	if( IsUsePetSkinPack() )
	{
		D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
		m_matTrans = matScale * matYRot * matTrans;
	}
	else
	{
		m_matTrans = matYRot * matTrans;
	}

	m_pSkinChar->SetPosition ( m_vPos );

	if( IsUsePetSkinPack() && IsSTATE ( EM_PETACT_MOVE ) )
	{
		float fSpeedGap1, fSpeedGap2, fTempTime, fTempElapsedTime;

		fSpeedGap1 = 1.0f / m_sPetSkinPackState.fPetScale;
		if( IsSTATE( EM_PETACT_RUN ) && m_sPetSkinPackState.bNotFoundRun ) fSpeedGap2 = m_fRunSpeed / m_fWalkSpeed;
		else															  fSpeedGap2 = 1.0f;

		fTempTime		 = fTime * fSpeedGap1 * fSpeedGap2;
		fTempElapsedTime = fElapsedTime * fSpeedGap1 * fSpeedGap2;

		m_pSkinChar->FrameMove ( fTempTime, fTempElapsedTime, FALSE, m_matTrans );
	}
	else
	{
		m_pSkinChar->FrameMove ( fTime, fElapsedTime, FALSE, m_matTrans );
	}

	// ============== Update about Skin (End)   ============== /

	// 현재 위치 업데이트
	m_vPos = m_actorMove.Position();

	// 방향 업데이트
	D3DXVECTOR3 vMovement = m_actorMove.NextPosition();
	if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
	{
		D3DXVECTOR3 vDirection = vMovement - m_vPos;
		if ( !DxIsMinVector(vDirection,0.2f) )
		{
			D3DXVec3Normalize ( &vDirection, &vDirection );
			m_vDir = vDirection;
		}
	}

	//	Note : AABB 계산.
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;


	//////////////////////////////////////////////////////////////////////////
	//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spNameCollision->InitData( spLandMan );
	NSSkinAniThread::AddNameCollision( m_spNameCollision, DxViewPort::GetInstance().GetFromPt(), m_vPos + D3DXVECTOR3(0.0f, 8.0f, 0.0f) );


	//////////////////////////////////////////////////////////////////////////
	//			ActorWorldLight 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spWorldLight->InitData( spLandMan );
	NSSkinAniThread::AddActorWorldLight( m_spWorldLight, m_vPos );


	return S_OK;
}

HRESULT GLAnyPet::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	std::tr1::shared_ptr<GLCharClient>  ptrOwner = m_pGaeaClient->GetChar(m_dwOwner);	
	if ( ptrOwner == NULL )
		return S_OK;

	if ( m_pSkinChar == NULL )
		return S_OK;

	const BOOL bVisible = ptrOwner->IsVisibleDetect(); // 보여지는 상태인가;		
	if ( bVisible == FALSE )
		return S_OK;

	bool bHALF_VISIBLE = ptrOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // 은신 중인가;

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		if ( bHALF_VISIBLE )
		{
			DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &m_vDir );
		}
		else
		{
			DxEffcharDataMan::GetInstance().OutEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
		}
	}

	//const bool bHalfVisible = ptrOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // 은신 중인가;
	//if ( bHalfVisible )
	//{
	//	DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &m_vDir );
	//}
	//else
	//{
	//	DxEffcharDataMan::GetInstance().OutEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
	//}

	GLLandManClient* pLandManClient = m_pGaeaClient->GetLandManClient();
	DxLandMan* pLandMan(NULL);
	if ( pLandManClient )
	{
		pLandMan = pLandManClient->GetLandMan();
	}

	if( IsUsePetSkinPack() )
	{
		D3DXMATRIX matScale, matRender;
		D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
		matRender = matScale * m_matTrans;
		//m_pSkinChar->Render( pd3dDevice, TRUE, 1.f, matRender, FALSE, TRUE, TRUE );

		RenderMyChar( pd3dDevice, matRender, TRUE, bHALF_VISIBLE );

		//if ( pLandMan )
		//{
		//	m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(matRender._41,matRender._42,matRender._43) );
		//}
	}
	else
	{
		//m_pSkinChar->Render( pd3dDevice, TRUE, 1.f, m_matTrans );

		RenderMyChar( pd3dDevice, m_matTrans, TRUE, bHALF_VISIBLE );

		//if ( pLandMan )
		//{
		//	m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(m_matTrans._41,m_matTrans._42,m_matTrans._43) );
		//}
	}
	
	return S_OK;
}

HRESULT GLAnyPet::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	std::tr1::shared_ptr<GLCharClient>  ptrOwner = m_pGaeaClient->GetChar(m_dwOwner);	
	if ( ptrOwner == NULL )
		return S_OK;

	if ( m_pSkinChar == NULL )
		return S_OK;

	const BOOL bVisible = ptrOwner->IsVisibleDetect(); // 보여지는 상태인가;		
	if ( bVisible == FALSE )
		return S_OK;

	bool bHALF_VISIBLE = ptrOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // 은신 중인가;

	if( IsUsePetSkinPack() )
	{
		D3DXMATRIX matScale, matRender;
		D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
		matRender = matScale * m_matTrans;

		RenderMyChar( pd3dDevice, matRender, FALSE, bHALF_VISIBLE );
	}
	else
	{
		//m_pSkinChar->Render( pd3dDevice, TRUE, 1.f, m_matTrans );

		RenderMyChar( pd3dDevice, m_matTrans, FALSE, bHALF_VISIBLE );
	}

	return S_OK;
}

HRESULT GLAnyPet::RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		return S_OK;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	std::tr1::shared_ptr<GLCharClient>  ptrOwner = m_pGaeaClient->GetChar(m_dwOwner);	
	if ( ptrOwner == NULL )
		return S_OK;

	if ( m_pSkinChar == NULL )
		return S_OK;

	const BOOL bVisible = ptrOwner->IsVisibleDetect(); // 보여지는 상태인가;		
	if ( bVisible == FALSE )
		return S_OK;

	bool bHALF_VISIBLE = ptrOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // 은신 중인가;

	NSMaterialSkinManager::SetMob( FALSE );

	if ( bHALF_VISIBLE )	// 캐릭터 반투명 렌더
	{
		m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 0.5f );
	}
	else		// 정상렌더
	{
		m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 1.f );
	}

	return S_OK;
}

void GLAnyPet::RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, BOOL bOpaqueState, BOOL bHALF_VISIBLE )
{
	if ( m_spWorldLight->IsNotCalculateColor() )
		return;

	NSMaterialSkinManager::SetMob( FALSE );
	m_pSkinChar->SetWorldLight( m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult );

	if ( bHALF_VISIBLE )	// 캐릭터 반투명 렌더
	{
		if ( bOpaqueState )
		{
			// 아무것도 하지 않는다.
		}
		else
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				m_pSkinChar->RenderFixed( pd3dDevice, TRUE, 0.5f );
			}
			else
			{
				m_pSkinChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL  ? TRUE : FALSE );
				m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, 0.5f );
				m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, 0.5f );
				m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, 0.5f );
			}
		}		
	}
	else		// 정상렌더
	{
		if ( bOpaqueState )
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				m_pSkinChar->RenderFixed( pd3dDevice, TRUE, 1.f );
			}
			else
			{
				m_pSkinChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL  ? TRUE : FALSE );
				m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
				m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
			}
		}
		else
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
			}
			else
			{
				m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
			}
		}
	}
}

HRESULT GLAnyPet::RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD )
{
	HRESULT hr;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
        return S_OK;

	//	Note : 그림자 랜더링.
	//
	hr = DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice, bWorldSystemOLD );
	if ( FAILED(hr) )
        return hr;

	return S_OK;
}

HRESULT GLAnyPet::RenderShadow_SpotLight( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	HRESULT hr;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
		return S_OK;

	//	Note : 그림자 랜더링.
	//
	hr = DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight ( m_pSkinChar, m_matTrans, pd3dDevice, bUsedMaterialSystem, dwSlot );
	if ( FAILED(hr) )
		return hr;

	return S_OK;
}

HRESULT GLAnyPet::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}


HRESULT GLAnyPet::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT GLAnyPet::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT GLAnyPet::InvalidateDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}

	return S_OK;
}

HRESULT GLAnyPet::DeleteDeviceObjects ()
{
	SAFE_DELETE(m_pSkinChar);

	m_actorMove.Stop();
	m_actorMove.Release();

	RESET ();

	return S_OK;
}

HRESULT GLAnyPet::UpdateSuit ()
{
	if( IsUsePetSkinPack() ) return S_OK;

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return E_FAIL;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice );

	if ( !pSkinChar )					  return E_FAIL;

	const PETSTYLE &sSTYLE = GLCONST_PET::GetPetStyle ( m_emTYPE );

	// 스타일 갱신

	if ( sSTYLE.wSTYLENum > m_wStyle )
	{
		std::string strSTYLE_CPS = sSTYLE.strSTYLE_CPS[m_wStyle];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	현재 장착 스킨.

		if ( pCharPart && strcmp( strSTYLE_CPS.c_str(), pCharPart->GetFileName() ) )
		{
			m_pSkinChar->SetPiece( strSTYLE_CPS.c_str(), m_pd3dDevice, 0L, 0, EMSCD_ALL_THREAD );
		}
	}

	// 컬러 갱신
	m_pSkinChar->SetHairColor( m_wColor );

	// 악세서리 모양 갱신
	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		SITEM* pSlotItem = GLogicData::GetInstance().GetItem( m_PutOnItems[i].GetNativeID() );
		if ( pSlotItem )
            m_pSkinChar->SetPiece( pSlotItem->GetPetWearingFile(), m_pd3dDevice, 0L, 0, EMSCD_ALL_THREAD );
	}

	return S_OK;
}

void GLAnyPet::SetAttackState( const D3DXVECTOR3& vTargetPos )
{
	// 방향 전환
	D3DXVECTOR3 vDirection = vTargetPos - m_vPos;
	D3DXVec3Normalize( &m_vDir, &vDirection );
	m_vDir = vDirection;

	if( FALSE == IsSTATE( EM_PETACT_ATTACK ) )
	{
		ReSetAllSTATE();
		SetSTATE( EM_PETACT_ATTACK );
		m_pSkinChar->SELECTANI( AN_ATTACK, AN_SUB_NONE );
	}
	else
	{
		m_actorMove.Stop();
		m_pSkinChar->SELECTANI( AN_GUARD_N, AN_SUB_NONE );
	}

	m_fAttackTimer = 0.0f;
}

const bool GLAnyPet::IsVisibleDetect(void) const
{
	std::tr1::shared_ptr<GLCharClient> pOwner = m_pGaeaClient->GetChar(m_dwOwner);
	if ( pOwner == NULL )
		return false;

	return pOwner->IsVisibleDetect();
}

void GLAnyPet::MsgGoto ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_GOTO_BRD* pNetMsg = ( GLMSG::SNETPET_GOTO_BRD* ) nmg;

	m_dwActionFlag = pNetMsg->dwFlag;

	BOOL bSucceed = m_actorMove.GotoLocation
	(
		pNetMsg->vTarPos+D3DXVECTOR3(0,+10,0),
		pNetMsg->vTarPos+D3DXVECTOR3(0,-10,0)
	);
	
	BOOL bReqRun = pNetMsg->dwFlag&EM_PETACT_RUN;

	if ( IsSTATE ( EM_PETACT_RUN ) )
	{
		if( IsUsePetSkinPack() && m_sPetSkinPackState.bNotFoundRun  )
		{
			m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		}else{
			m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
		}
		m_actorMove.SetMaxSpeed ( m_fRunSpeed );
	}
	else
	{
		m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
	}
}

void GLAnyPet::MsgStop ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_STOP_BRD* pNetMsg = ( GLMSG::SNETPET_STOP_BRD* ) nmg;

	// 멈춰야 한다면 현재 네비게이션을 끝까지 가게 해줘야 한다.
	BOOL bReqStop = pNetMsg->dwFlag&EM_PETACT_STOP;
	if ( bReqStop && !pNetMsg->bStopAttack )	return;
	
	m_dwActionFlag = pNetMsg->dwFlag;
	
	// 기다려야 하거나 어텍모션을 멈춰야 한다면
	if ( IsSTATE ( EM_PETACT_WAIT ) || pNetMsg->bStopAttack )
	{
		m_actorMove.Stop ();
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
	}
}

void GLAnyPet::MsgSkillChange ( NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_PET_REQ_SKILL_CHANGE_FC* pNetMsg = ( GLMSG::NET_PET_REQ_SKILL_CHANGE_FC* ) nmg;

	for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
	{
		// 이전 이펙트 제거
		std::tr1::shared_ptr<GLCharClient> pTarChar = m_pGaeaClient->GetChar(pNetMsg->sSKILL[i].dwTarID);
		if (pTarChar)
		{
			FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC, pTarChar->GetGaeaID(), pTarChar->GetPosition () ), pTarChar->GetCharSkin (), m_sActiveSkillID[i] );
		}

		// 스킬변경
		m_sActiveSkillID[i] = pNetMsg->sSKILL[i].sSkillID;

		// 이펙트 처리
		if ( m_sActiveSkillID[i] != SNATIVEID(false) )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID[i].wMainID, m_sActiveSkillID[i].wSubID );
			if ( !pSkill )								return;

			// 자신에게 발동되는 이펙트 발동(변경)
			STARGETID sTARG( CROW_PET,m_dwGUID, m_vPos );

			EMELEMENT emELMT = EMELEMENT_SPIRIT;

			if ( pSkill->m_sEXT_DATA.emSELFBODY==SKILL::EMTIME_FIRST )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &m_vDir );

			SANIMSTRIKE sStrike;
			sStrike.m_emPiece = PIECE_HAIR;
			sStrike.m_emEffect = EMSF_TARGET;
			sStrike.m_dwFrame = 0;

			if ( pSkill->m_sEXT_DATA.emSELFZONE01==SKILL::EMTIME_FIRST )
				SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

			if ( pSkill->m_sEXT_DATA.emSELFZONE02==SKILL::EMTIME_FIRST )
				SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

			if ( pSkill->m_sEXT_DATA.emSELFZONE03==SKILL::EMTIME_FIRST )
				SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE03_POS, pSkill->m_sEXT_DATA.GETSELFZONE03(emELMT), &sTARG );

			// 주인에게 붙는 이펙트 발동(변경)
			if ( pTarChar )
			{
				STARGETID sTargetID( CROW_PC, pTarChar->GetGaeaID(), pTarChar->GetPosition () );
				FACTEFF::NewSkillFactEffect ( sTargetID, pTarChar->GetCharSkin (), m_sActiveSkillID[i], pTarChar->GetTransMatrix(), sTargetID.vPos );
			}
		}

	}
}

void GLAnyPet::MsgUpdateMoveState ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_UPDATE_MOVESTATE_BRD* pNetMsg = (GLMSG::SNETPET_REQ_UPDATE_MOVESTATE_BRD*) nmg;

	m_dwActionFlag = pNetMsg->dwFlag;

	if (IsSTATE(EM_PETACT_RUN))
        m_actorMove.SetMaxSpeed(m_fRunSpeed);
	else
        m_actorMove.SetMaxSpeed(m_fWalkSpeed);

	if (IsSTATE(EM_PETACT_MOVE))
	{
		if (IsSTATE(EM_PETACT_RUN))
            m_pSkinChar->SELECTANI(AN_RUN, AN_SUB_NONE);
		else
            m_pSkinChar->SELECTANI(AN_WALK, AN_SUB_NONE);
	}
}

void GLAnyPet::MsgRename ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_RENAME_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_RENAME_BRD* ) nmg;
	SetName(pNetMsg->szName);
}

void GLAnyPet::MsgChangeColor ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_CHANGE_COLOR_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_CHANGE_COLOR_BRD* ) nmg;
	m_wColor = pNetMsg->wColor;

	UpdateSuit ();
}

void GLAnyPet::MsgChangeStyle ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_CHANGE_STYLE_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_CHANGE_STYLE_BRD* ) nmg;

	m_wStyle = pNetMsg->wStyle;
	m_wColor = pNetMsg->wColor;
	UpdateSuit ();
}

void GLAnyPet::MsgChangeAccessory ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD* ) nmg;

	SetSlotItem ( pNetMsg->emSuit, pNetMsg->sItemCustom );
	UpdateSuit ();
}

void GLAnyPet::MsgLearnSkill ( NET_MSG_GENERIC* nmg )
{
	// 구동 스킬에 따른 이펙트 표시
	STARGETID sTargetID(CROW_PET, m_dwGUID, m_vPos);
	DxEffGroupPlayer::GetInstance().NewEffGroup
	(
		GLCONST_CHAR::strSKILL_LEARN_EFFECT.c_str(),
		m_matTrans,
		&sTargetID,
		FALSE,
		FALSE
	);
}

void GLAnyPet::MsgSkillSlotExpansion ( NET_MSG_GENERIC* nmg )
{
	// 구동 스킬에 따른 이펙트 표시
	STARGETID sTargetID(CROW_PET, m_dwGUID, m_vPos);
	DxEffGroupPlayer::GetInstance().NewEffGroup
		(
		GLCONST_CHAR::strSKILL_LEARN_EFFECT.c_str(),
		m_matTrans,
		&sTargetID,
		FALSE,
		FALSE
		);
}

void GLAnyPet::MsgRemoveSlotItem ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_BRD* ) nmg;
	ReSetSlotItem( pNetMsg->emSuit );

    SkinLoad( m_pd3dDevice );
	UpdateSuit();
}

void GLAnyPet::MsgAttack ( NET_MSG_GENERIC* nmg )
{
	// 이동중이면 이동을 마친후 공격 모션을 취한다.
	if ( m_actorMove.PathIsActive () ) return;

	GLMSG::SNETPET_ATTACK_BRD* pNetMsg = ( GLMSG::SNETPET_ATTACK_BRD* ) nmg;
	
	D3DXVECTOR3 vTargetPos;

	if( pNetMsg->sTarID.emCrow == CROW_PC )
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
		std::tr1::shared_ptr<GLCharClient> pChar = pLandMan->GetChar ( pNetMsg->sTarID.GaeaId );
		if( pChar )
			vTargetPos = pChar->GetPosition();
		else // 다른 PC 가 아니면 플레이어 케릭터이다
			vTargetPos = m_pGaeaClient->GetCharacter()->GetPosition();
	}
	else if ( pNetMsg->sTarID.emCrow == CROW_MOB )
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(pNetMsg->sTarID.GaeaId);
		if( NULL == spCrow )
			return;

		vTargetPos = spCrow->GetPosition();
	}
	else if ( pNetMsg->sTarID.emCrow == CROW_SUMMON )
	{
		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
		std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(pNetMsg->sTarID.GaeaId);
		if( NULL == spCrow )
			return;

		vTargetPos = spCrow->GetPosition();
	}

	SetAttackState( vTargetPos );
}

void GLAnyPet::MsgSad ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_SAD_BRD* pNetMsg = ( GLMSG::SNETPET_SAD_BRD* ) nmg;
	
	GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
	std::tr1::shared_ptr<GLCharClient> pChar = pLandMan->GetChar ( pNetMsg->dwOwner );
	if ( pChar )
	{
		// 플레이어가 죽어서 요청된 메세지다;
		if ( pNetMsg->bReset == false )
		{
			m_dwActionFlag = EM_PETACT_SAD;

			// SAD 애니메이션
			if( IsUsePetSkinPack() && m_sPetSkinPackState.bNotFoundSad )
			{
				m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
			}else{
				m_pSkinChar->SELECTANI ( AN_GESTURE, AN_SUB_04 );
			}

			// 방향 전환
			D3DXVECTOR3 vDirection = pChar->GetPosition () - m_vPos;
			D3DXVec3Normalize ( &m_vDir, &vDirection );
			m_vDir = vDirection;
		}
		else
		{
			ReSetSTATE(EM_PETACT_SAD);
			SetSTATE (EM_PETACT_STOP);
			m_actorMove.Stop ();
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}
	}
}

void GLAnyPet::MsgFunny ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_FUNNY_BRD* pNetMsg = ( GLMSG::SNETPET_REQ_FUNNY_BRD* ) nmg;
	m_wAniSub = pNetMsg->wFunny;

	SetSTATE( EM_PETACT_FUNNY );
	m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub );

	m_fFunyTimer = 0.0f;
}

void GLAnyPet::MsgAccessoryDelete ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_ACCESSORY_DELETE_BRD* pNetMsg = (GLMSG::SNETPET_ACCESSORY_DELETE_BRD*) nmg;
	m_PutOnItems[pNetMsg->accetype] = SITEMCUSTOM ();

    SkinLoad( m_pd3dDevice );
	UpdateSuit();
}

void GLAnyPet::MsgPetSkinPack ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_SKINPACKOPEN_BRD* pNetMsg = (GLMSG::SNETPET_SKINPACKOPEN_BRD*) nmg;

	m_sPetSkinPackState.bUsePetSkinPack = pNetMsg->sPetSkinPackData.bUsePetSkinPack;
	m_sPetSkinPackState.petSkinMobID	   = pNetMsg->sPetSkinPackData.sMobID;
	m_sPetSkinPackState.fPetScale	   = pNetMsg->sPetSkinPackData.fScale;

	// 생성 효과
	D3DXMATRIX matEffect;
	D3DXVECTOR3 vPos = GetPosition ();
	D3DXMatrixTranslation ( &matEffect, vPos.x, vPos.y, vPos.z );

	std::string strGEN_EFFECT = GLCONST_CHAR::strPET_GEN_EFFECT.c_str();
	STARGETID sTargetID(CROW_PET,m_dwGUID,vPos);
	DxEffGroupPlayer::GetInstance().NewEffGroup( strGEN_EFFECT.c_str(), matEffect, &sTargetID, FALSE, FALSE );				
	SkinLoad( m_pd3dDevice );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	GetSkinChar()->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = ( m_vMaxOrg.y - m_vMinOrg.y ) * m_sPetSkinPackState.fPetScale;

	if( !IsUsePetSkinPack() ) UpdateSuit ();
}

void GLAnyPet::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
{
	D3DXMATRIX matTrans;

	//	스트라이크 지점에서.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		//	타겟의 위치.
		D3DXVECTOR3 vTARPOS = m_vPos + m_vDir * 60.0f;

		//	타격 위치를 알아냄.
		STRIKE::SSTRIKE sStrike;

		bool bAttBone = false;
		if ( m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
		{
			bAttBone = !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->m_strAttBoneName.empty();
		}

		BOOL bOk = FALSE;

		if ( bAttBone )
		{
			if ( !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
				return;

			PDXATTBONE pDxAttBone = m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->GetAttBone();
			if ( !pDxAttBone ) 
				return;
			
			STRIKE::CSELECTOR Selector ( vTARPOS, pDxAttBone->m_AttBoneArray );
			bOk = Selector.SELECT ( (EMPIECECHAR)ATTBONE_BODY, sStrike );
		}
		else
		{
			STRIKE::CSELECTOR Selector ( vTARPOS, m_pSkinChar->m_PartArray );
			bOk = Selector.SELECT ( sStrikeEff.m_emPiece, sStrike );
		}

		if ( bOk )
		{
			bTRANS = TRUE;

			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
		}
	}

	//	지면기준 목표점에 회전 속성 넣어서.
	if ( !bTRANS )
	{
		bTRANS = TRUE;

		D3DXMATRIX matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * m_matTrans;
	}

	//	Note : 자기 위치 이펙트 발생시킴.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, pTarget, TRUE, FALSE );
}

void GLAnyPet::MsgJumpPos(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_JUMP_FB* pNetMsg = (GLMSG::SNETPET_REQ_JUMP_FB*)nmg;

	m_vPos = pNetMsg->vJumpPos;

	GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

	m_actorMove.Create ( pLandMan->GetNaviMesh(), m_vPos );
	m_actorMove.Stop ();

	m_vTarPos = m_vPos;

	ReSetSTATE ( EM_PETACT_MOVE );
	SetSTATE ( EM_PETACT_STOP );	
}

void GLAnyPet::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_PET_GOTO_BRD:						MsgGoto ( nmg );				break;
	case NET_MSG_PET_REQ_UPDATE_MOVE_STATE_BRD:		MsgUpdateMoveState ( nmg );		break;
	case NET_MSG_PET_STOP_BRD:						MsgStop ( nmg );				break;
	case NET_MSG_PET_REQ_SKILL_CHANGE_FC:			MsgSkillChange ( nmg );			break;
	case NET_MSG_PET_REQ_RENAME_BRD:				MsgRename ( nmg );				break;
	case NET_MSG_PET_REQ_CHANGE_COLOR_BRD:			MsgChangeColor ( nmg );			break;
	case NET_MSG_PET_REQ_CHANGE_STYLE_BRD:			MsgChangeStyle ( nmg );			break;
	case NET_MSG_PET_REQ_SLOT_EX_HOLD_BRD:			MsgChangeAccessory ( nmg );		break;
	case NET_MSG_PET_REQ_LEARNSKILL_BRD:			MsgLearnSkill ( nmg );			break;
	case NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_BRD:	MsgSkillSlotExpansion ( nmg );	break;
	case NET_MSG_PET_REQ_FUNNY_BRD:					MsgFunny ( nmg );				break;
	case NET_MSG_PET_REMOVE_SLOTITEM_BRD:			MsgRemoveSlotItem ( nmg );		break;
	case NET_MSG_PET_ATTACK_BRD:					MsgAttack ( nmg );				break;
	case NET_MSG_PET_SAD_BRD:						MsgSad ( nmg );					break;
	case NET_MSG_PET_ACCESSORY_DELETE_BRD:			MsgAccessoryDelete ( nmg );		break;
	case NET_MSG_PET_PETSKINPACKOPEN_BRD:			MsgPetSkinPack ( nmg );			break;
	case NET_MSG_GCTRL_PET_JUMP_POS_FB:				MsgJumpPos( nmg );				break;

	default:
		CDebugSet::ToListView ( "[WARNING]GLPetField::MsgProcess Illigal Message(%d)", nmg->nType );
	  	break;
	};
}

BOOL GLAnyPet::IsVisibleDisplayName() const
{
	if ( m_spNameCollision->GetCollsion() )
		return FALSE;

	return TRUE;
}