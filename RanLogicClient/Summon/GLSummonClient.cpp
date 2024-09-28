#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxAttBoneLink.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/Occlusion/NSOcclusionCulling.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/GlobalParam.h"
#include "../../EngineLib/DxTools/EditMeshs.h"

#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/TransformSet/GLTransformSet.h"

#include "../GLGaeaClient.h"
#include "../GfxInterfaceBase.h"
#include "../GlogicExClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Strike/GLStrikeM.h"
#include "../Land/GLLandManClient.h"
#include "../Char/GLCharacter.h"
#include "../Char/SkinCharMulti.h"
#include "./GLSummonClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLSummonClient::GLSummonClient(GLGaeaClient* pGaeaClient) :
	ClientActor(pGaeaClient, CROW_SUMMON),	
	m_pd3dDevice(NULL),
	m_pSkinChar(NULL),
	m_pSkinCharMulti(NULL),
	m_pTransformSet(NULL),
    m_bVisible(TRUE),

	m_pLandManClient(NULL),

	m_wTARNUM(0),
	m_vTARPOS(D3DXVECTOR3(0,0,0)),

	m_vMax(6,20,6),
	m_vMin(-6,0,-6),
	m_fHeight(20.f),

	m_vMaxOrg(6,20,6),
	m_vMinOrg(-6,0,-6),

	m_fattTIMER(0.0f),
	m_nattSTEP(0),

	m_wAniSub(1),
	m_pAttackProp(NULL),

	m_bBeginAlpha(FALSE)

	, m_spWorldLight(new ActorWorldLight)
	, m_spNameCollision(new ActorNameCollision(EMCC_CULL_NONE))
	, m_spCollisionByObject(new ActorNameCollision(EMCC_CULL_NONE_ONLY_WALL))
{
	m_sTargetID.RESET();
	//SetActState(EM_SUMMONACT_STOP);	

	//m_spNameCollision = std::tr1::shared_ptr<ActorNameCollision>(new ActorNameCollision(EMCC_CULL_NONE));
	//m_spCollisionByObject = std::tr1::shared_ptr<ActorNameCollision>(new ActorNameCollision(EMCC_CULL_NONE_ONLY_WALL));
}

GLSummonClient::~GLSummonClient(void)
{
	DeleteDeviceObjects ();
}

void GLSummonClient::ASSIGN ( PSDROPSUMMON pDropSummon )
{
	m_emTYPE			 = pDropSummon->m_emTYPE;

	ClientActor::SetGaeaID(pDropSummon->m_dwGUID);
	m_sSummonID			 = pDropSummon->m_sSummonID;
	m_dwOwnerGaeaID			 = pDropSummon->m_dwOwner;	
	ClientActor::SetPosition(pDropSummon->m_vPos);
	ClientActor::SetDirect(pDropSummon->m_vDir);	
	m_fCurSpeed			 = pDropSummon->m_fSpeed;

	m_fRemainedTime      = pDropSummon->m_fRemainedTime;

	m_fOwnerIncHP        = pDropSummon->m_fOwnerIncHP;
	m_wOwnerAttack       = pDropSummon->m_wOwnerAttack;
	m_wOwnerDefense      = pDropSummon->m_wOwnerDefense;
	m_wOwnerSkillLevel   = pDropSummon->m_wOwnerSkillLevel;
	m_cOwnerWeaponsGrade = pDropSummon->m_cOwnerWeaponsGrade;
    m_sOwnerBlow         = pDropSummon->m_sOwnerBlow;
    m_wOwnerSchool       = pDropSummon->m_wOwnerSchool;

	//m_dwCeID		 	 = pDropSummon->m_dwCellID;

	ReSetActState(~pDropSummon->m_dwActionFlag);
	SetActState(pDropSummon->m_dwActionFlag);
		
	m_wAniSub			 = pDropSummon->m_wAniSub;

	m_dwOwnerGaeaID			 = pDropSummon->m_dwOwner;
 
	m_dwNowHP			 = pDropSummon->m_dwNowHP;
	m_wNowMP			 = pDropSummon->m_wNowMP;	

	m_sTargetID          = pDropSummon->m_sTarget;

	m_emGestureState     = pDropSummon->m_emGestureState;

	m_bBeginAlpha		 = FALSE;
}

HRESULT GLSummonClient::Create ( GLLandManClient *pLandManClient, PSDROPSUMMON pDropSummon, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	GASSERT(pLandManClient);
	GASSERT(pDropSummon);

	m_fattTIMER  = 0.0f;
	m_nattSTEP   = 0;

	// 기본 데이터 설정
	RESET_DATA();
	ASSIGN ( pDropSummon );	

	m_pLandManClient = pLandManClient;
	m_pd3dDevice     = pd3dDevice;

	// 몹의 설정 정보를 가져옴.
	m_pSummonCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pDropSummon->m_sSummonID );
	if( !m_pSummonCrowData )
	{
		SNATIVEID sCrowID = pDropSummon->m_sSummonID;
		sc::writeLogError(
            sc::string::format(
                "GLSummonClient::Create()-GLCrowDataMan::GetCrowData() [%1%/%2%] 가져오기 실패.",
                sCrowID.wMainID,
                sCrowID.wSubID));
		return E_FAIL;
	}

	// 이동 제어 초기화.
	ClientActor::InitializeActorMove(pDropSummon->m_vPos, m_pLandManClient->GetNaviMesh(), pDropSummon->m_vDir);
	ClientActor::SetMaxSpeed(m_pSummonCrowData->m_sAction.m_fWalkVelo);
	m_fCurSpeed = pDropSummon->m_fSpeed;	

	DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pSummonCrowData->GetSkinObjFile(), m_pd3dDevice );
	if ( !pCharData )
	{
		if( strlen( m_pSummonCrowData->GetSkinObjFile() ) )
		{
			sc::writeLogError(
                sc::string::format(
                    "GLSummonClient::Create()-DxSkinCharDataContainer::LoadData() [%1%] 가져오기 실패.",
                    m_pSummonCrowData->GetSkinObjFile()));
		}
		return E_FAIL;
	}

	SAFE_DELETE(m_pSkinChar);
	SAFE_DELETE(m_pTransformSet);
	m_pTransformSet = new GLTransformSet( pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	m_pSkinChar = new DxSkinChar;

	m_pSkinChar->SetCharData ( pCharData, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

	if ( m_pSkinCharMulti )
	{
		m_pSkinCharMulti->ResetSkinCharMulti();
		SAFE_DELETE(m_pSkinCharMulti);
	}
	m_pSkinCharMulti = new SkinCharMulti( pCharData, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	//
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	// Note : 사용가능한 제스쳐를 체크한다. (클라이언트는 해당 인덱스에 가능 여부만 저장한다)
	//
	m_dwGestureNum = 0;
	for ( int i=0; i<EM_SUMMONGESTURE_MAX; ++i )
	{
		if (  !m_pSkinChar->GETANI( AN_GESTURE, EMANI_SUBTYPE(i) ) )
		{
			m_emGestureableArray[i] = EM_SUMMONGESTURE_NONE;
			continue;
		}

		m_emGestureableArray[i] = EMSUMMONGESTURESTATE(i);
		++m_dwGestureNum;
	}

	// 상태 적용.
	//
	switch ( pDropSummon->m_emAction )
	{

	case GLAT_IDLE:
		{
			TurnAction ( GLAT_IDLE );

			// Note : 상태를 생성중으로 바꾼다.
			if ( pDropSummon->m_dwActionFlag&EM_ACT_CREATE )
			{
				if ( m_pSkinChar->GETANI( AN_CREATE, AN_SUB_00 ) )
				{
					m_emGestureState = EM_SUMMONGESTURE_NONE;
					SELECTANI ( m_pSkinChar,  AN_CREATE, AN_SUB_00 );
					//m_pSkinChar->SETCURTIME( pDropSummon->m_fIdleTime );	// 할 필요 없다.
					SetActState( EM_ACT_CREATE );
				}
			}
			// Note : 상태를 제스쳐 중으로 바꾼다.
			else if ( pDropSummon->m_dwActionFlag&EM_ACT_GESTURE )
			{
				if ( pDropSummon->m_emGestureState != EM_SUMMONGESTURE_NONE && m_emGestureableArray[pDropSummon->m_emGestureState] != EM_SUMMONGESTURE_NONE )
				{
					m_emGestureState = pDropSummon->m_emGestureState;
					SELECTANI ( m_pSkinChar,  AN_GESTURE, EMANI_SUBTYPE(m_emGestureState) );
					//m_pSkinChar->SETCURTIME( pDropSummon->m_fIdleTime );	// 할 필요 없다.
					SetActState( EM_ACT_GESTURE );
				}
			}
		}
		break;

	case GLAT_MOVE:
		{
			// 생성시 이동중인 위치에서 서버와 클라이언트가 서로 다른
			// 루트를 통해 이동하는 문제가 생기므로 수정함.
			if ( pDropSummon->m_dwActionFlag&EM_ACT_RUN )
				SetActState   ( EM_ACT_RUN );
			else
				ReSetActState ( EM_ACT_RUN );

			TurnAction ( GLAT_MOVE );

			ClientActor::SetPositionActor(pDropSummon->vStartPos); // 시작위치를 지정해주고
			ClientActor::GotoLocation(m_sTargetID.vPos, 10.0f); // 웨이포인트를 설정하고
			ClientActor::SetPositionActor(pDropSummon->m_vPos); // 현재의 위치를 지정해주고
			ClientActor::SetNextWayPoint(pDropSummon->sNextWaypoint); // 다음 웨이포인트를 설정해주고			

			// 방향을 업데이트 한다.
			if ( !m_pSummonCrowData->IsDirHold() )
			{
				const D3DXVECTOR3& vMovement = ClientActor::GetNextPosition();
				if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
				{
					D3DXVECTOR3 vDirection = vMovement - pDropSummon->m_vPos;
					if ( !DxIsMinVector(vDirection,0.2f) )
					{
						D3DXVec3Normalize ( &vDirection, &vDirection );
						ClientActor::SetDirect(vDirection);
					}
				}
			}
		}
		break;

	case GLAT_FALLING:
		TurnAction ( GLAT_FALLING );
		break;

	case GLAT_DIE:
		TurnAction ( GLAT_DIE );
		break;
	};

	// 현재 스킬 FACT 업데이트.
	//
	for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )
	{
		SDROP_SKILLFACT &sDropFact = pDropSummon->m_sSKILLFACT[i];
		if ( sDropFact.sNATIVEID==NATIVEID_NULL() )	
			continue;

		RECEIVE_SKILLFACT ( sDropFact.sNATIVEID, static_cast<WORD>(sDropFact.cLEVEL), sDropFact.wSLOT, sDropFact.sTargetID, SSKILLFACT::STIME(sDropFact) );
	}
	
	// 현재 상태이상 업데이트.
	//
	for ( int i=0; i<EMBLOW_MULTI; ++i )
	{
		SSTATEBLOW& sDropBlow = pDropSummon->m_sSTATEBLOWS[i];
		if (sDropBlow.emBLOW == EMBLOW_NONE)
			continue;

		SSTATEBLOW* pSTATEBLOW = NULL;
		if (sDropBlow.emBLOW <= EMBLOW_SINGLE)
        {
            pSTATEBLOW = &m_sSTATEBLOWS[0];
        }
		else
        {
            DWORD Index = sDropBlow.emBLOW-EMBLOW_SINGLE;
            if (Index < EMBLOW_MULTI)
                pSTATEBLOW = &m_sSTATEBLOWS[Index];
        }

        // Assign itself
        if (pSTATEBLOW)
            pSTATEBLOW->SetData(sDropBlow);
	}
	
	DISABLEALLLANDEFF();
	DISABLEALLMAPEFF();

    if ( IsVisibleDetect() == FALSE ) //컬링에 의해 보여지지않을 때는 이펙트 생성 안함
    {
	    // 스킬 버프, 상태이상 효과 생성.
	    FACTEFF::ReNewEffect(
		    STARGETID(CROW_SUMMON,m_dwGaeaID, pDropSummon->m_vPos),
		    m_pSkinChar, m_sSKILLFACT, CROW_SKILLFACT_SIZE, m_sSTATEBLOWS, GetTransMatrix(), GetDirect() );
    }
	if ( !m_pSkinChar->GETCURANIMNODE() )
	{
		DEFAULTANI( m_pSkinChar );
 	}

	// 현재 로딩중 일수도 있다.
	//if ( !m_pSkinChar->GETCURANIMNODE() )
	//{
	//	sc::writeLogError(
	//		sc::string::format(
	//		"current animation node null point error [%1%] [M %2% S %3%]",
	//		m_pSkinChar->GetFileName(),
	//		m_pSkinChar->GETCURMTYPE(),
	//		m_pSkinChar->GETCURSTYPE()));
	//}

	// Note : 현재 주인의 강화 횟수에 따라 스펙큘러 업데이트
	SITEMCLIENT sItemClient;
	sItemClient.cGRADE = m_cOwnerWeaponsGrade;

	int nGrindLevel = sItemClient.GETGRADE_EFFECT();
	for ( int i=0; i<(int)PIECE_SAVE_SIZE; ++i )
	{
		m_pSkinChar->SetPieceGrindLevel(i, nGrindLevel);
	}

	m_bBeginAlpha = FALSE;

	return S_OK;
}

float GLSummonClient::GetDirection ()
{
	return DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
}

float GLSummonClient::GetMoveVelo ()
{
    return GLOGICEX::GetMoveVelocity(
        TRUE,
        m_fCurSpeed,
        m_fCurSpeed,
        false,
        GLSUMMON::GETMOVEVELO(),
        0.0f,
        m_fMoveVeloLimit_Min,
        m_fMoveVeloLimit_Max);
	//float fVELO = m_fCurSpeed;
	//return fVELO * GLSUMMON::GETMOVEVELO();
}

void GLSummonClient::TurnAction ( EMACTIONTYPE toAction )
{
	// 이전 액션 취소(쓰러질때와 죽을때는 제외)
	//
	switch ( GetAction() )
	{
	case GLAT_IDLE:
		//ReSetActState(EM_SUMMONACT_STOP);
		ReSetActState(EM_ACT_CREATE);
		ReSetActState(EM_ACT_GESTURE);
		break;

	case GLAT_MOVE:
		//ReSetActState(EM_SUMMONACT_TRACING);
		//ReSetActState(EM_SUMMONACT_MOVE);
		break;

	case GLAT_ATTACK:
		//ReSetActState(EM_SUMMONACT_ATTACK);
		break;

	case GLAT_SKILL:
		//ReSetActState(EM_SUMMONACT_SKILL);
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

	// 액션 초기화.
	//	
	SetAction(toAction);

	switch ( toAction )
	{
	case GLAT_IDLE:		
		if ( ClientActor::PathIsActive() )
		{
			SetAction(GLAT_MOVE);			
			//ReSetActState(EM_SUMMONACT_STOP);
			//SetActState(EM_SUMMONACT_MOVE);
		}
		break;

	case GLAT_MOVE:
		//ReSetActState(EM_SUMMONACT_STOP);
		//SetActState(EM_SUMMONACT_MOVE);
		break;

	case GLAT_ATTACK:

		// 같은 애니메이션으로 공격하면 애니메이션이 중간 중간 안나오는 경우가 생겨서, 리셋을 해준다.
		if ( m_pSkinChar )
		{
			SELECTANI( m_pSkinChar, AN_GUARD_N );
		}

		//SetActState(EM_SUMMONACT_ATTACK);
		StartAttackProc ();
		break;

	case GLAT_SKILL_WAIT:
		// 같은 애니메이션으로 공격하면 애니메이션이 중간 중간 안나오는 경우가 생겨서, 리셋을 해준다.
		if ( m_pSkinChar )
		{
			SELECTANI( m_pSkinChar, AN_GUARD_N );
		}
		break;

	case GLAT_SKILL:
		//SetActState(EM_SUMMONACT_SKILL);
		StartSkillProc ();
		break;

	case GLAT_FALLING:
		m_dwNowHP = 0;
		FactEffectProcessing( EMFACTEFFECTPROCESSING_DEAD );
		break;

	case GLAT_DIE:
		{
			m_dwNowHP = 0;
			FactEffectProcessing( EMFACTEFFECTPROCESSING_DEAD );

// Note : 블러드 이펙트 삭제
/*
D3DXVECTOR3		vNORMAL(0.f,1.f,0.f);
D3DXVECTOR3		vSTRIKE(0.f,0.f,0.f);

			bool bBLOOD_TRACE(false);
			for ( int i=0; i<PIECE_REV01; ++i )
			{
				PDXCHARPART pCharPart = m_pSkinChar->GetPiece(i);
				if ( !pCharPart )	continue;

				if ( pCharPart->GetTracePos(STRACE_BLOOD01) )
				{
					bBLOOD_TRACE = true;
					pCharPart->CalculateVertexInflu ( STRACE_BLOOD01, vSTRIKE, vNORMAL );
				}
			}

			if ( !bBLOOD_TRACE )		vSTRIKE = m_vPos;

			D3DXMATRIX matEffect;
			D3DXMatrixTranslation ( &matEffect, vSTRIKE.x, vSTRIKE.y, vSTRIKE.z );

			STARGETID sTargetID(CROW_SUMMON,m_dwGlobID,m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
				(
				GLCONST_CHAR::strMOB_DIE_BLOOD.c_str(),
				matEffect,
				&sTargetID
				);
*/

		}
		break;

	default:
		GASSERT("GLCharacter::TurnAction() 준비되지 않은 ACTION 이 들어왔습니다.");
		break;
	};

	if ( ClientActor::PathIsActive() )
	{
		if ( !isAction(GLAT_MOVE, GLAT_PUSHPULL) )
		{
			//ReSetActState( EM_SUMMONACT_TRACING );
			//ReSetActState( EM_SUMMONACT_MOVE );
			//SetActState ( EM_SUMMONACT_STOP );
			ClientActor::StopActorMove();
		}
	}

	// UI 갱신;
	if ( m_pGaeaClient->GetCharacter()->GetGaeaID() == m_dwOwnerGaeaID )
	{
		switch ( toAction )
		{
		case GLAT_SKILL:
		case GLAT_ATTACK:
		case GLAT_IDLE:
		case GLAT_DIE:
			// UI on/off;
			GLWidgetScript::GetInstance().LuaCallFunc<void>( 
				NSWIDGET_SCRIPT::g_strFunc_UpdateSummonAction,
				"-n, -b", (int) toAction, false );
			break;
		}
	}
}


HRESULT GLSummonClient::UpdateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	BOOL bLowSP(false);
	BOOL bFreeze = FALSE;

	DxSkinChar* pRenderChar = NULL;
	if ( ( pRenderChar = m_pTransformSet->GetTransformSkinChar(
		m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform ) ) == NULL )
		pRenderChar = m_pSkinChar;

	if ( pRenderChar == NULL )
		return E_FAIL;

	switch ( GetAction() )
	{
	case GLAT_IDLE:
		{
			// Note: 생성중이면
			if ( IsActState(EM_ACT_CREATE) )
			{
				if ( pRenderChar->GETANI( AN_CREATE, AN_SUB_00 ) )
				{
					SELECTANI ( pRenderChar, AN_CREATE, AN_SUB_00 );

					if ( pRenderChar->IsANI_ISENDANIM( AN_CREATE, AN_SUB_00 ) )
					{
						m_emGestureState = EM_SUMMONGESTURE_NONE;
						ReSetActState(EM_ACT_CREATE);
					}
				}
			}
			// Note : 제스쳐중 이라면
			else if ( IsActState(EM_ACT_GESTURE) )
			{
				if ( m_emGestureableArray[m_emGestureState] != EM_SUMMONGESTURE_NONE )
				{
					if ( pRenderChar->GETANI( AN_GESTURE, EMANI_SUBTYPE(m_emGestureState) ) )
					{
						SELECTANI ( pRenderChar, AN_GESTURE, EMANI_SUBTYPE(m_emGestureState) );

						if ( pRenderChar->IsANI_ISENDANIM( AN_GESTURE, EMANI_SUBTYPE(m_emGestureState) ) )
						{
							m_emGestureState = EM_SUMMONGESTURE_NONE;
							ReSetActState(EM_ACT_GESTURE);
						}
					}
				}
			}
			// Note : 일반 상태라면
			else 
			{
				BOOL bPANT(FALSE); // 헐떡거림.
				EMANI_MAINTYPE emMType;
				EMANI_SUBTYPE emSType;

				if ( pRenderChar->GETANI ( AN_GUARD_L, AN_SUB_NONE ) )
				{
					bPANT = bLowSP || m_bSTATE_PANT;
					for ( int i=0; i<EMBLOW_MULTI; ++i )
					{
						if ( m_sSTATEBLOWS[i].emBLOW == EMBLOW_STUN || m_sSTATEBLOWS[i].emBLOW == EMBLOW_MAD )		bPANT = TRUE;
					}
				}

				if ( bPANT )
				{
					emMType = AN_GUARD_L;
					emSType = AN_SUB_NONE;
				}
				else
				{
					emMType = AN_GUARD_N;
					emSType = AN_SUB_NONE;
				}

				SELECTANI ( pRenderChar, AN_GUARD_N, AN_SUB_NONE );
			}
		}
		break;

	case GLAT_TALK:
		SELECTANI ( pRenderChar, AN_TALK, AN_SUB_NONE );
		if ( pRenderChar->IsANI_ISENDANIM( AN_TALK, AN_SUB_NONE ) )			
			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_MOVE:
		if ( IsActState(EM_ACT_RUN) )		SELECTANI ( pRenderChar, AN_RUN, AN_SUB_NONE );
		else									SELECTANI ( pRenderChar, AN_WALK, AN_SUB_NONE );
		break;

	case GLAT_ATTACK:
		if ( m_pAttackProp )
			SELECTANI ( pRenderChar, m_pAttackProp->strAniFile.c_str() );
		if ( pRenderChar->IsANI_ISENDANIM( m_pAttackProp->strAniFile.c_str() ) )			
			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_SKILL:
		if ( m_pAttackProp )
			SELECTANI ( pRenderChar, m_pAttackProp->strAniFile.c_str() );
		if ( pRenderChar->IsANI_ISENDANIM( m_pAttackProp->strAniFile.c_str() ) )			
			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_SHOCK:
		SELECTANI ( pRenderChar, AN_SHOCK, AN_SUB_NONE );
//		if ( m_pSkinChar->IsANI_ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:
		if ( !IsANI_SELECTANI ( pRenderChar, AN_SPEC, (EMANI_SUBTYPE)m_wAniSub ) )
		{
			SELECTANI ( pRenderChar, AN_GUARD_N, AN_SUB_NONE );
		}
		break;

	case GLAT_FALLING:
		SELECTANI ( pRenderChar, AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		if ( pRenderChar->IsANI_ISENDANIM( AN_DIE, AN_SUB_NONE ) )
			TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		bFreeze = TRUE;
		SELECTANI ( pRenderChar, AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		TOENDTIME ( pRenderChar );
		//DeleteDeviceObjects();
		break;

	case GLAT_GATHERING:
		SELECTANI ( pRenderChar, AN_GATHERING, AN_SUB_NONE );
		break;
	};

	// Note : 사망 이펙트
	//
	if ( !m_bBeginAlpha && pRenderChar->GETCURMTYPE() == AN_DIE )
	{
		DWORD fREMAINEDTIME = (DWORD)(pRenderChar->GETENDTIME() - pRenderChar->GETCURTIME());

		if ( fREMAINEDTIME <= (GLCONST_CHAR::fERASE_SUMMON_BEGIN_TIME*4800.0f) )
		{
			DxEffcharDataMan::GetInstance().PutEffect(
				pRenderChar, GLCONST_CHAR::strERASE_SUMMON_EFFECT.c_str(), &GetDirect());

			m_bBeginAlpha = TRUE;
		}
	}

	// Mob 스킨 업데이트.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( GetAction() )
	{
	case GLAT_MOVE:
		fSkinAniElap *= GLSUMMON::GETMOVEVELO();
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		fSkinAniElap *= GLSUMMON::GETATTVELO();
		break;
	};

	SetPositionSkin( pRenderChar, GetPosition() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f) );
	FrameMoveSkin ( pRenderChar, fTime, fSkinAniElap, FALSE, m_matTrans, TRUE, bFreeze );

	return S_OK;
}

HRESULT GLSummonClient::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sSummonID );
	if ( !pCrowData )
        return E_FAIL;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		pd3dDevice );

	if ( !pSkinChar )
        return E_FAIL;

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	
	SELECTANI ( m_pSkinChar,  AN_GUARD_N, (EMANI_SUBTYPE)0 );
	if ( m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, (EMANI_SUBTYPE)0 ) == 0 )
	{
		DEFAULTANI( m_pSkinChar );
	}

	m_pSkinChar->SetHairColor( 0 );

	return S_OK;
}

void GLSummonClient::UPDATE_DATA_END ( const DWORD dwSkillfactIndex )
{
	if ( dwSkillfactIndex >= CROW_SKILLFACT_SIZE )
		return;

	SSKILLFACT& sSKILLFACT = m_sSKILLFACT[dwSkillfactIndex];
	if ( sSKILLFACT.sNATIVEID==NATIVEID_NULL() )
		return;
	if ( !sSKILLFACT.bEnable )
		return;

	const size_t nSize = sSKILLFACT.vecSPEC.size();
	for ( unsigned int _i = 0; _i < nSize; ++_i )
	{
		const SKILL::SSPEC_ADDON_LEVEL& spec = sSKILLFACT.vecSPEC[_i];
		switch ( spec.emSPEC )
		{		
		case EMSPECA_COUNTATTACK :
			{	
				m_SkillCAIndex.AllReset();
			}
			break;
		case EMSPECA_HALLUCINATE:
			{
				DxSkinChar* pRenderChar = NULL;
				if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) != NULL )
				{
					pRenderChar->DEFAULTANI();
					pRenderChar->StopLoopSound();
				}
			}
			break;
		case EMSPECA_PROVOKE :
			RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
			RF_ACTIVE( m_EffSkillVarSet ).ResetValue();
			break;
		}
	}

	FACTEFF::DeleteSkillFactEffect(STARGETID(CROW_SUMMON, m_dwGaeaID, GetPosition()), m_pSkinChar, sSKILLFACT.sNATIVEID);
	sSKILLFACT.RESET();
}

HRESULT	GLSummonClient::UpdateTurnAction ( float fTime, float fElapsedTime )
{
	HRESULT hr(S_OK);

	DxSkinChar* pRenderChar = NULL;
	if ( ( pRenderChar = m_pTransformSet->GetTransformSkinChar(
		m_sHALLUCINATE.dwTransformID,
		m_sHALLUCINATE.bTransform ) ) == NULL )
		pRenderChar = m_pSkinChar;

	switch ( GetAction() )
	{
	case GLAT_IDLE:
		break;

	case GLAT_MOVE:
		{
			MoveProc ( fElapsedTime );

			// 현재 위치 업데이트			
			//const D3DXVECTOR3& vPosition = ClientActor::RefreshPosition(0.0f);

			if ( !ClientActor::PathIsActive() )
			{
				//ReSetActState( EM_SUMMONACT_MOVE );
				//SetActState ( EM_SUMMONACT_STOP );
				TurnAction ( GLAT_IDLE );
			}

			if ( !m_pSummonCrowData->IsDirHold() )
			{
				// 방향 업데이트
				const D3DXVECTOR3& vMovement = ClientActor::GetNextPosition();
				if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
				{
					D3DXVECTOR3 vDirection = vMovement - ClientActor::GetPositionActor();	//vPosition;
					if ( !DxIsMinVector(vDirection,0.2f) )
					{
						D3DXVec3Normalize ( &vDirection, &vDirection );
						ClientActor::SetDirect(vDirection);
					}
				}
			}
		}
		break;
	case GLAT_ATTACK:
		{
			AttackProc ( fElapsedTime );

			if ( !m_pSummonCrowData->IsDirHold() )
			{
				// 공격 방향으로 회전.
				//
				ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( m_sTargetID );
				if ( pTarget )
				{
					D3DXVECTOR3 vDirection = pTarget->GetPosition() - GetPosition();
					D3DXVec3Normalize ( &vDirection, &vDirection );
					ClientActor::SetDirect(vDirection);
				}
			}	
		}
		break;

	case GLAT_SKILL:
		{
			SkillProc ( fElapsedTime );

			if ( !m_pSummonCrowData->IsDirHold() )
			{
				// 공격 방향으로 회전.
				UpdateSkillDirection(m_pGaeaClient, GetPosition(), GetDirect(), m_idACTIVESKILL, m_vTARPOS, m_sTARIDS);
			}
		};
		break;

	case GLAT_SHOCK:
		TurnAction( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:
		ClientActor::Update( m_pLandManClient->GetvecLayerCollision(), fElapsedTime); 		
		if ( !ClientActor::PathIsActive() )
		{			
			if ( pRenderChar->IsCurAniConInfo(ACF_LOOP) || pRenderChar->ISENDANIM () )
			{
				ClientActor::StopActorMove();
				TurnAction ( GLAT_IDLE );
			}
		}
		break;
	}
	
	return S_OK;
}

void GLSummonClient::UpdateMapEffect()
{
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if( !pLand ) return;

	if( pLand->m_bClubBattle ) 
	{
		if ( !m_pGaeaClient->IsClubBattle() ) 
		{
			DISABLEALLMAPEFF();
			return;
		}
	}
	else
	{
		DISABLEALLMAPEFF();
		return;
	}
}

void GLSummonClient::UpdateLandEffect()
{
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if( !pLand ) return;

	const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();
	if( !pLand->GetUseLevelEtcFunc(EMETCFUNC_LANDEFFECT) ) return;

	const D3DXVECTOR3& vPosition = GetPosition();

	// 모든 버프를 취소해야할 경우의 처리
	for( DWORD i = 0; i < m_vecLandEffect.size(); i++ )
	{
		SLANDEFFECT_VALUE landEffect = m_vecLandEffect[i];
		if( !landEffect.IsUse() ) continue;
		if( landEffect.emLandEffectType == EMLANDEFFECT_CANCEL_ALLBUFF )
		{
			for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
			{
                if (i < CROW_SKILLFACT_SIZE)
                {
				    FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_SUMMON, m_dwGaeaID, vPosition), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );
				    RESETSKEFF(i);
                }
			}			
			return;
		}
	}
}

HRESULT GLSummonClient::FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime )
{
	if ( !m_pSummonCrowData )
        return S_OK;

	HRESULT hr(S_OK);

	if ( !m_bUsingEventSkill )
	{
		DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();
		if ( m_dwAType < dwSize &&
			m_dwAType >= 0 )
		{
			m_pAttackProp = &(m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[m_dwAType]);
		}
	}
	else
	{
		DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage ).size();
		if ( m_dwAType < dwSize &&
			m_dwAType >= 0 )
		{
			m_pAttackProp = &(m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage )[m_dwAType]);
		}
	}

	m_sAirborne.update(fElapsedTime);
	if ( IsValidBody () )
	{
		if ( m_dwNowHP!=0 )
		{
			// Summon 의 LandEffect를 Update 한다
			//UpdateLandEffect();

			//UpdateMapEffect();

			// Summon 의 현재 상태값들을 Update 한다
			UPDATE_DATA ( fTime, fElapsedTime, TRUE );
		}
		else{
			//	행위 변화.
			TurnAction ( GLAT_FALLING );
		}
	}

	// ---------------------------------- 지배효과; //
	switch ( m_sDOMINATION.emController )
	{	
	case EMSPEC_DOMINATE_TYPE_FIX:
		if ( !isAction(GLAT_FALLING, GLAT_DIE) )
			TurnAction(GLAT_IDLE);
		break;
	}
	// --------------------------------------------- //

	DxSkinChar* pRenderChar = NULL;
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	if( m_pSkinCharMulti )
	{
		BOOL bUpdateSuit = FALSE;
		BOOL bIsCloneSkill = IsCheckedSkillFlagSpecial(EMSPECA_CLONE);
		bUpdateSuit = m_pSkinCharMulti->SetUseSkinCharMulti( bIsCloneSkill, m_sClone.GetClonNum());

		if ( !bIsCloneSkill )
			m_sClone.Reset();
	}

	UpdateAnimation( fTime, fElapsedTime );

	UpdateTurnAction ( fTime, fElapsedTime );

	// [shhan][2015.01.27] RefreshPosition 를 안해주니 PUSHPULL 될 시 캐릭터 형상과 이름, 타켓팅 되는 위치가 따로 분리되는 문제가 생겼다.
	//						m_vPosition 것을 없앨 경우는 PUSHPULL Test 도 같이 해봐야 할것 같음.
	ClientActor::RefreshPosition(0.f);

    //이펙트 컬링처리;
    {
        const BOOL bPrevState = m_bVisible;
        m_bVisible = CalcVisibleDetect( fElapsedTime );	
        if ( m_bVisible == FALSE )
            DxEffGroupPlayer::GetInstance().RenderEnable(STARGETID(CROW_SUMMON, m_dwGaeaID), false);
        else if ( m_bVisible != bPrevState )
            DxEffGroupPlayer::GetInstance().RenderEnable(STARGETID(CROW_SUMMON, m_dwGaeaID), true);
    }

	// 스킬 이펙트 업데이트.
	//
	FACTEFF::UpdateSkillEffect(
		m_pGaeaClient,
		STARGETID(CROW_SUMMON, m_dwGaeaID, ClientActor::GetPositionActor()),
		m_pSkinChar,
		m_sSKILLFACT,
		CROW_SKILLFACT_SIZE,
		m_sSTATEBLOWS);



	//////////////////////////////////////////////////////////////////////////
	// Skin 관련 위치를 사용한다. ( vPositionOfSkin ) ( airbone 적용된 값 )
	//////////////////////////////////////////////////////////////////////////

	// Note : 현재 위치 업데이트
	//
	//const D3DXVECTOR3& vPosition = ClientActor::RefreshPosition(m_pSkinChar->m_fHeightYPos) + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f);
	const D3DXVECTOR3& vPositionOfSkin = ClientActor::GetPositionActor() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f);

	// 현재 위치와 방향으로 Transform 메트릭스 계산.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, vPositionOfSkin.x, vPositionOfSkin.y, vPositionOfSkin.z );
	float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	SetTransMatrix(D3DXMATRIX(matYRot * matTrans));

	// AABB 계산.
	//
	m_vMax = vPositionOfSkin + m_vMaxOrg;
	m_vMin = vPositionOfSkin + m_vMinOrg;


	//////////////////////////////////////////////////////////////////////////
	//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spNameCollision->InitData( spLandMan );
	NSSkinAniThread::AddNameCollision( m_spNameCollision, DxViewPort::GetInstance().GetFromPt(), vPositionOfSkin + D3DXVECTOR3(0.0f, 8.0f, 0.0f) );

	if ( m_pLandManClient->IsCullByObjectMap() == true )
	{
		//////////////////////////////////////////////////////////////////////////
		//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
		//				airbone 값이 적용된 것을 사용하지는 않는다.
		// Thread 상으로 넘긴다.
		m_spCollisionByObject->InitData( spLandMan );
		NSSkinAniThread::AddCollisionByObject( m_spCollisionByObject, m_pGaeaClient->GetCharacter()->GetPositionActor() + D3DXVECTOR3(0.0f, 12.0f, 0.0f), GetPositionActor() + D3DXVECTOR3(0.0f, 12.0f, 0.0f) );
	}


	//////////////////////////////////////////////////////////////////////////
	//			ActorWorldLight 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spWorldLight->InitData( spLandMan );
	NSSkinAniThread::AddActorWorldLight( m_spWorldLight, vPositionOfSkin );


	return S_OK;
}

HRESULT GLSummonClient::Render( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
    /*if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	if ( m_spWorldLight->IsNotCalculateColor() )
		return S_OK;

	NSMaterialSkinManager::SetMob( TRUE );
	m_pSkinChar->SetWorldLight( m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult );

	DxSkinChar* pRenderChar = NULL;		
	if ( ( pRenderChar = m_pTransformSet->GetTransformSkinChar(
		m_sHALLUCINATE.dwTransformID,
		m_sHALLUCINATE.bTransform ) ) == NULL )
		pRenderChar = m_pSkinChar;

	if ( pRenderChar )
	{
		const D3DXMATRIX& matWorld = GetTransMatrix();
        //m_pSkinChar->Render( pd3dDevice, TRUE, 1.f, matWorld );

		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
			pRenderChar->RenderFixed( pd3dDevice, TRUE, 1.f );
		}
		else
		{
			pRenderChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
			pRenderChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
		}

		DxLandMan* pLandMan(NULL);
		if ( m_pLandManClient )
		{
			pLandMan = m_pLandManClient->GetLandMan();
		}

		//if ( pLandMan )
		//{
		//	pRenderChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(matWorld._41,matWorld._42,matWorld._43) );
		//}
	}

	//if ( m_pSkinCharMulti )
	//	m_pSkinCharMulti->RenderMyChar( pd3dDevice, TRUE, FALSE, m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult );
*/

	const D3DXVECTOR3& vPosition( GetPosition() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f) );
	const D3DXVECTOR3& vDirect(GetDirect());
	const D3DXVECTOR3& vDirectOrig(GetDirectOrig());

	DxLandMan* pLandMan(NULL);
	if ( m_pLandManClient )
	{
		pLandMan = m_pLandManClient->GetLandMan();
	}

	if ( !IsVisibleDetect() )
	{
		D3DXMATRIX matYRot, matTrans;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));		

		return S_FALSE;
	}

	// Occlusion Culling 
	if ( !NSOcclusionCulling::VisibleOcclusionCharZone( m_vMax, m_vMin ) )
		return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
	{
		D3DXMATRIX matYRot, matTrans;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));

		return S_OK;
	}

	//UpdateQuestNpcState();


	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE);

	//if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		if ( bHALF_VISIBLE )
		{
			DxEffcharDataMan::GetInstance().PutPassiveEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &vDirect );
		}
		else
		{
			DxEffcharDataMan::GetInstance().OutEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
		}
	}

	//// m_bINVISIBLE 가 참일 경우 반투명하게 표현해야함.
	////		
	//if ( IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) )
	//	DxEffcharDataMan::GetInstance().PutPassiveEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &vDirect );
	//else
	//	DxEffcharDataMan::GetInstance().OutEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
	//   
	const D3DXMATRIX& matTrans = GetTransMatrix();

	if ( pRenderChar )
	{
		//if ( FAILED(hr = pRenderChar->Render ( pd3dDevice, TRUE, 1.f, matTrans )) )
		//	return hr;

		RenderMyChar( pd3dDevice, pRenderChar, TRUE, bHALF_VISIBLE );

		//if ( pLandMan )
		//{
		//	pRenderChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(matTrans._41,matTrans._42,matTrans._43) );
		//}
	}

	if(m_pSummonCrowData->IsBarrier())
	{
		EDITMESHS::RENDERSPHERE(
			m_pd3dDevice,
			ClientActor::GetPositionActor(),
			D3DXVECTOR3( 50.0f, 50.0f, 50.0f ),
			FALSE,
			NULL,
			D3DCOLOR_ARGB( 255, 255, 0, 0 ) );
	}

	return S_OK;
}

HRESULT GLSummonClient::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	/*if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	DxSkinChar* pRenderChar = NULL;		
	if ( ( pRenderChar = m_pTransformSet->GetTransformSkinChar(
		m_sHALLUCINATE.dwTransformID,
		m_sHALLUCINATE.bTransform ) ) == NULL )
		pRenderChar = m_pSkinChar;

	if ( pRenderChar )
	{
		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
		}
		else
		{
			pRenderChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
		}
	}*/

	const D3DXVECTOR3& vPosition = GetPosition();
	const D3DXVECTOR3& vDirect = GetDirect();
	const D3DXVECTOR3& vDirectOrig = GetDirectOrig();

	if ( !IsVisibleDetect() )
	{
		D3DXMATRIX matYRot, matTrans;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));		

		return S_FALSE;
	}

	// Occlusion Culling 
	if ( !NSOcclusionCulling::VisibleOcclusionCharZone( m_vMax, m_vMin ) )
		return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
	{
		D3DXMATRIX matYRot, matTrans;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));

		return S_OK;
	}


	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE);

	if ( pRenderChar )
	{
		RenderMyChar( pd3dDevice, pRenderChar, FALSE, bHALF_VISIBLE );
	}

	return S_OK;
}

HRESULT GLSummonClient::RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	/*if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	DxSkinChar* pRenderChar = NULL;		
	if ( ( pRenderChar = m_pTransformSet->GetTransformSkinChar(
		m_sHALLUCINATE.dwTransformID,
		m_sHALLUCINATE.bTransform ) ) == NULL )
		pRenderChar = m_pSkinChar;

	if ( pRenderChar )
	{
		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
		}
		else
		{
			pRenderChar->RenderMaterialEffect ( pd3dDevice, 1.f );
		}
	}*/

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		return S_OK;

	const D3DXVECTOR3& vPosition = GetPosition();
	const D3DXVECTOR3& vDirect = GetDirect();
	const D3DXVECTOR3& vDirectOrig = GetDirectOrig();

	if ( !IsVisibleDetect() )
	{
		D3DXMATRIX matYRot, matTrans;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));		

		return S_FALSE;
	}

	// Occlusion Culling 
	if ( !NSOcclusionCulling::VisibleOcclusionCharZone( m_vMax, m_vMin ) )
		return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
	{
		D3DXMATRIX matYRot, matTrans;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));

		return S_OK;
	}


	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE);

	if ( pRenderChar )
	{
		if ( bHALF_VISIBLE )	// 캐릭터 반투명 렌더
		{
			pRenderChar->RenderMaterialEffect ( pd3dDevice, 0.5f );
		}
		else		// 정상렌더
		{
			pRenderChar->RenderMaterialEffect ( pd3dDevice, 1.f );
		}
	}

	return S_OK;
}

HRESULT GLSummonClient::RenderShadow( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD )
{
    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	HRESULT hr;
	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
        return S_OK;

	// 그림자 랜더링.
	//
	DxSkinChar* pRenderChar = NULL;		
	if ( ( pRenderChar = m_pTransformSet->GetTransformSkinChar(
		m_sHALLUCINATE.dwTransformID,
		m_sHALLUCINATE.bTransform ) ) == NULL )
		pRenderChar = m_pSkinChar;

	hr = DxShadowMap::GetInstance().RenderShadowCharMob ( pRenderChar, GetTransMatrix(), pd3dDevice, bWorldSystemOLD );
	if ( FAILED(hr) )
        return hr;

	return S_OK;
}

HRESULT GLSummonClient::RenderShadow_SpotLight( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	HRESULT hr;
	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
		return S_OK;

	// 그림자 랜더링.
	//
	DxSkinChar* pRenderChar = NULL;		
	if ( ( pRenderChar = m_pTransformSet->GetTransformSkinChar(
		m_sHALLUCINATE.dwTransformID,
		m_sHALLUCINATE.bTransform ) ) == NULL )
		pRenderChar = m_pSkinChar;

	hr = DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight ( pRenderChar, GetTransMatrix(), pd3dDevice, bUsedMaterialSystem, dwSlot );
	if ( FAILED(hr) )
		return hr;

	return S_OK;
}

HRESULT GLSummonClient::RenderAniEff( LPDIRECT3DDEVICEQ pd3dDevice )
{
    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	if ( m_pSkinChar )
        m_pSkinChar->RenderAniEff( pd3dDevice );

	return S_OK;
}

void GLSummonClient::RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pRenderChar, BOOL bOpaqueState, BOOL bHALF_VISIBLE )
{
	if ( m_spWorldLight->IsNotCalculateColor() )
		return;

	if ( GetCrow() == CROW_NPC )
	{
		// 음영을 밝게 한다.
		NSMaterialSkinManager::SetMob( FALSE );
	}
	else
	{
		// 음영을 어둡게 한다.
		NSMaterialSkinManager::SetMob( TRUE );
	}
	pRenderChar->SetWorldLight( m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult );

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
				pRenderChar->RenderFixed( pd3dDevice, TRUE, 0.5f );
			}
			else
			{
				pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL  ? TRUE : FALSE );
				pRenderChar->RenderMaterialOpaque ( pd3dDevice, 0.5f );
				pRenderChar->RenderMaterialHardAlpha ( pd3dDevice, 0.5f );
				pRenderChar->RenderMaterialSoftAlpha ( pd3dDevice, 0.5f );
			}
		}		
	}
	else		// 정상렌더
	{
		if ( bOpaqueState )
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				pRenderChar->RenderFixed( pd3dDevice, TRUE, 1.f );
			}
			else
			{
				pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL  ? TRUE : FALSE );
				pRenderChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
				pRenderChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
			}
		}
		else
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
			}
			else
			{
				pRenderChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
			}
		}
	}

	if ( m_pSkinCharMulti )
	{
		m_pSkinCharMulti->RenderMyChar
		( 
			pd3dDevice, 
			TRUE, 
			bHALF_VISIBLE, 
			m_spWorldLight->m_vDiffuseResult, 
			m_spWorldLight->m_vPointLightResult, 
			RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL ? TRUE : FALSE 
		);
	}
}

void GLSummonClient::StartAttackProc ()
{
	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;

	if( m_pSkinChar )  m_pSkinChar->ResetStrikeAttachBone();
}

BOOL GLSummonClient::AttackProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime;/* * GETATTVELO();*/

	const DxSkinAnimation* pAnimCont = m_pSkinChar->GETCURANIM();

	//GASSERT(pAnimCont);
	if ( !pAnimCont )
		return FALSE;	

	if ( pAnimCont->m_wStrikeCount == 0 )
		return FALSE;

	DWORD dwThisKey = DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

	DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		m_pSkinChar->UpdateStrikeAttachBone( dwThisKey, m_nattSTEP );
		if ( dwThisKey>=dwStrikeKey )
		{
			if ( m_pGaeaClient->IsVisibleCV(m_sTargetID) )
			{
				// 공격시 이팩트 발휘..
				//
				AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
			}

			m_nattSTEP++;
		}
	}

	return TRUE;
}

void GLSummonClient::AttackEffect ( const SANIMSTRIKE &sStrikeEff )
{
	BOOL bOk = m_pGaeaClient->ValidCheckTarget ( m_sTargetID );
	if ( !bOk )	return;

	ClientActor* pActor = m_pGaeaClient->GetCopyActor ( m_sTargetID );
	if ( pActor )	pActor->ReceiveSwing ();

	//	타겟의 위치.
	D3DXVECTOR3 vTARPOS = m_pGaeaClient->GetTargetPos ( m_sTargetID );
	const D3DXVECTOR3& vPosition = GetPosition();

	D3DXVECTOR3 vDir = vTARPOS - vPosition;
	vDir.y = 0.f;
	D3DXVec3Normalize ( &vDir, &vDir );

	vTARPOS.y += 15.0f;

	D3DXVECTOR3 vTarDir = vTARPOS - vPosition;
	float fLength = D3DXVec3Length ( &vTarDir );

	D3DXVec3Normalize ( &vTarDir, &vTarDir );
	D3DXVECTOR3 vTARPAR = vPosition + vTarDir*fLength * 10.0f;

	//	타격 위치를 알아냄.
	STRIKE::SSTRIKE sStrike;

	bool bAttBone = false;
	if ( m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
	{
		bAttBone = !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->m_strAttBoneName.empty();
	}

	bOk = FALSE;

	if ( bAttBone )
	{		
		if ( !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
			return;

		PDXATTBONE pDxAttBone = m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->GetAttBone();
		if ( !pDxAttBone ) 
			return;

		STRIKE::CSELECTOR Selector ( vTARPAR, pDxAttBone->m_AttBoneArray );
		bOk = Selector.SELECT ( (EMPIECECHAR)ATTBONE_BODY, sStrike );
	}
	else
	{
		STRIKE::CSELECTOR Selector ( vTARPAR, m_pSkinChar->m_PartArray );
		bOk = Selector.SELECT ( sStrikeEff.m_emPiece, sStrike );
	}

	if ( !bOk )	return;

	//EMSLOT emSlot = PIECE_2_SLOT(sStrikeEff.m_emPiece);
	//if ( emSlot==SLOT_TSIZE )	return;

	D3DXMATRIX matEffect;
	D3DXMatrixTranslation ( &matEffect, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
	STARGETID sTargetID(m_sTargetID.emCrow,m_sTargetID.GaeaId,vTARPOS);

	EMELEMENT emELEMENT = STATE_TO_ELEMENT ( m_sOwnerBlow.emTYPE );

	if ( m_pAttackProp && m_pAttackProp->emAttRgType == EMATT_LONG )
	{
		if ( !m_pAttackProp->strTargetEffect.empty() )
		{
			SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup(
				m_pAttackProp->strTargetEffect.c_str(),
				matEffect,
				&sTargetID,
				TRUE, 
				FALSE );

			if ( spEffSingleG.get() )
			{
				spEffSingleG->AddEffAfter ( EFFASINGLE, GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT)  );

				//if ( !m_pAttackProp->strTargBodyEffect.empty() )
				//	spEffSingleG->AddEffAfter ( EFFABODY, std::string(m_pAttackProp->strTargBodyEffect.c_str()) );

				//spEffSingleG->AddEffAfter ( EFFABODY, GLCONST_CHAR::strAMBIENT_EFFECT );
			}
		}
	}
	else
	{
		DxEffGroupPlayer::GetInstance().NewEffGroup(
			GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT),
			matEffect,
			&sTargetID,
			TRUE, 
			FALSE );

		DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
	}
}

void GLSummonClient::MoveProc ( float fElpaseTime )
{
	ClientActor::SetMaxSpeed ( GetMoveVelo () );
	ClientActor::Update( m_pLandManClient->GetvecLayerCollision(), fElpaseTime );	
}

void GLSummonClient::Move ( const D3DXVECTOR3& vPos )
{
	D3DXVECTOR3 vDist = GetPosition() - vPos;
	float fDist = D3DXVec3Length ( &vDist );
	if ( fDist < 1.0f )
	{
		return;
	}

	const BOOL bSucceed = ClientActor::GotoLocation(vPos, 10.0f);

	if ( bSucceed )
	{
		//SetActState ( EM_SUMMONACT_MOVE );

		m_sTargetID.vPos = vPos;

		if ( !IsActState ( EM_ACT_RUN ) )
		{		
			ClientActor::SetMaxSpeed ( m_pSummonCrowData->m_sAction.m_fWalkVelo );
		}
		else
		{
			SetActState ( EM_ACT_RUN );
			ClientActor::SetMaxSpeed ( m_pSummonCrowData->m_sAction.m_fRunVelo );
		}

		TurnAction( GLAT_MOVE );
	}
}

void GLSummonClient::Move_TargetTracing ( const STARGETID& sTarget )
{
	ReSetActState(~0);
	//SetActState ( EM_SUMMONACT_TRACING );
	SetActState ( EM_ACT_RUN );

	ClientActor *pTARGET = m_pGaeaClient->GetCopyActor( sTarget );

	m_sTargetID = sTarget;

	// Note : 타겟을 찾지 못하면 지점만 이동하게 설정한다.
	//
	if ( !pTARGET )
	{
		Move ( sTarget.vPos );
		return;
	}

	Move ( pTARGET->GetPosition() );	
}

void GLSummonClient::StartSkillProc ()
{
	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )							return;

	// 목표물에 따라 방향을 미리 회전한다.
	//
	if ( !m_pSummonCrowData->IsDirHold() )
	{
		const D3DXVECTOR3 vReDirect = 
			UpdateSkillDirection(m_pGaeaClient, GetPosition(), GetDirect(), m_idACTIVESKILL, m_vTARPOS, m_sTARIDS);
		ClientActor::SetDirect(vReDirect);
	}

	const ANIMCONTNODE* pAniContNode = NULL;

	if ( !m_bUsingEventSkill )
	{
		DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage ).size();
		if ( m_dwAType >= dwSize ||
			m_dwAType < 0 )
			return;

		SCROWATTACK &sATTACK = m_pSummonCrowData->m_vecCrowAttackPage.at( m_dwCurSkillPage )[m_dwAType];
		pAniContNode = m_pSkinChar->findanicontnode ( sATTACK.strAniFile.c_str() );
		if ( !pAniContNode )					
			return;

	}
	else
	{
		DWORD dwSize = (DWORD)m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage ).size();
		if ( m_dwAType >= dwSize ||
			m_dwAType < 0 )
			return;

		SCROWATTACK &sATTACK = m_pSummonCrowData->m_vecCrowAttackEventPage.at( m_dwCurSkillPage )[m_dwAType];
		pAniContNode = m_pSkinChar->findanicontnode ( sATTACK.strAniFile.c_str() );
		if ( !pAniContNode )					
			return;

	}

	if ( pAniContNode == NULL )
		return;

	EMPIECECHAR emStrikePiece = pAniContNode->m_pDxSkinAni->m_sStrikeEff[0].m_emPiece;
	STARGETID sTARG(GETCROW(), m_dwGaeaID);

	EMELEMENT emELMT(EMELEMENT_SPIRIT);

	// 스킬 시작과 동시에 나타나는 이팩트. 발동.
	//
	if ( pSkill->m_sEXT_DATA.emSELFBODY==SKILL::EMTIME_FIRST )
		DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &GetDirect() );

	SANIMSTRIKE sStrike;
	sStrike.m_emPiece = emStrikePiece;
	sStrike.m_emEffect = EMSF_TARGET;
	sStrike.m_dwFrame = 0;

	if ( pSkill->m_sEXT_DATA.emSELFZONE01==SKILL::EMTIME_FIRST )
		SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE02==SKILL::EMTIME_FIRST )
		SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SPEC )
	{
		EMELEMENT emELMT(EMELEMENT_SPIRIT);

		for ( WORD i=0; i<m_wTARNUM; ++i )
		{
			STARGETID sTARGET;
			sTARGET.emCrow = m_sTARIDS[i].GETCROW();
			sTARGET.GaeaId = m_sTARIDS[i].GETID();

			if ( !m_pGaeaClient->IsVisibleCV(sTARGET) )	continue;
			sTARGET.vPos = m_pGaeaClient->GetTargetPos(sTARGET);

			if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_FIRST )
				SK_EFF_TARZONE ( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_FIRST )
				SK_EFF_TARZONE ( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );
		}
	}

}

void GLSummonClient::SKT_EFF_HOLDOUT ( STARGETID sTarget , DWORD dwDamageFlag )
{
	for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
		if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKEFF.sNATIVEID.wMainID, sSKEFF.sNATIVEID.wSubID );
		if ( !pSkill )								continue;

		if ( !pSkill->m_sEXT_DATA.strHOLDOUT.empty() )
		{

			bool bEffect = false;

			for ( DWORD j = 0; j < pSkill->m_sAPPLY.vecSPEC.size(); ++j )
			{

				if ( (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_PSY_DAMAGE_REDUCE && 
					dwDamageFlag & DAMAGE_TYPE_PSY_REDUCE ) )
				{
					bEffect = true;
					break;
				}

				if ( (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_MAGIC_DAMAGE_REDUCE && 
					dwDamageFlag & DAMAGE_TYPE_MAGIC_REDUCE ) )
				{
					bEffect = true;
					break;
				}

				if ( (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_PSY_DAMAGE_REFLECTION && 
					dwDamageFlag & DAMAGE_TYPE_PSY_REFLECTION ) )
				{
					bEffect = true;
					break;
				}

				if ( (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_MAGIC_DAMAGE_REFLECTION && 
					dwDamageFlag & DAMAGE_TYPE_MAGIC_REFLECTION ) )
				{
					bEffect = true;
					break;
				}
			}

			if ( !bEffect ) continue;


			const char* szZONE_EFF = pSkill->m_sEXT_DATA.strHOLDOUT.c_str();

			const D3DXVECTOR3& vPosition = GetPosition();

			D3DXVECTOR3 vDIR = sTarget.vPos - vPosition;

			D3DXVECTOR3 vDIR_ORG(1,0,0);
			float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

			D3DXMATRIX matTrans;
			D3DXMatrixRotationY ( &matTrans, fdir_y );
			matTrans._41 = vPosition.x;
			matTrans._42 = vPosition.y;
			matTrans._43 = vPosition.z;

			// 자기 위치 이펙트 발생시킴.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &sTarget, TRUE, FALSE );
		}
	}
}

void GLSummonClient::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
{
	D3DXMATRIX matTrans;

	//	스트라이크 지점에서.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		//	타겟의 위치.
		D3DXVECTOR3 vTARPOS = GetPosition() + GetDirect() * 60.0f;

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
		matTrans = matYRot * GetTransMatrix();
	}

	// 자기 위치 이펙트 발생시킴.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, pTarget, TRUE, FALSE );
}

void GLSummonClient::SK_EFF_TARZONE ( const STARGETID &_sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF )
{
	D3DXMATRIX matTrans;
	STARGETID sTarget = _sTarget;

	const D3DXVECTOR3& vPosition = GetPosition();
	//	방향 회전
	D3DXVECTOR3 vDIR = vPosition - sTarget.vPos;

	D3DXVECTOR3 vDIR_ORG(1,0,0);
	float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

	D3DXMatrixRotationY ( &matTrans, fdir_y );

	//	스트라이크 지점에서.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		DxSkinChar* pSkin = m_pGaeaClient->GetSkinChar ( sTarget );

		//	타겟의 위치.
		D3DXVECTOR3 vTARPOS ( vPosition.x, vPosition.y + 15.0f, vPosition.z );;

		//	타격 위치를 알아냄.
		IMPACT_ARROW::SSTRIKE sStrike;
		IMPACT_ARROW::CSELECTOR Selector ( vTARPOS, pSkin->m_PartArray );
		BOOL bOk = Selector.SELECT ( sStrike );
		if ( bOk )
		{
			bTRANS = TRUE;

//			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
			matTrans._41 = sStrike.vPos.x;
			matTrans._42 = sStrike.vPos.y;
			matTrans._43 = sStrike.vPos.z;
			sTarget.vPos = vTARPOS;
		}
	}

	//	지면기준 목표점.
	if ( !bTRANS )
	{
		bTRANS = TRUE;
//		D3DXMatrixTranslation ( &matTrans, sTarget.vPos.x, sTarget.vPos.y, sTarget.vPos.z );
		matTrans._41 = sTarget.vPos.x;
		matTrans._42 = sTarget.vPos.y;
		matTrans._43 = sTarget.vPos.z;
	}

	// 목표 위치 이펙트 발생시킴.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &sTarget, TRUE, FALSE );
}

void GLSummonClient::SK_EFF_TARG ( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &_sTarget )
{
	D3DXMATRIX matTrans;
	STARGETID sTarget = _sTarget;

	const D3DXVECTOR3& vPosition = GetPosition();
	//	스트라이크 지점에서.
	BOOL bTRANS(FALSE);
	if ( pSkill->m_sEXT_DATA.emTARG_POSA == SKILL::EMPOS_STRIKE )
	{
		//	타겟의 위치.
		D3DXVECTOR3 vSTRPOS ( sTarget.vPos.x, sTarget.vPos.y+15, sTarget.vPos.z );;

		D3DXVECTOR3 vDir = vSTRPOS - vPosition;
		float fLength = D3DXVec3Length ( &vDir );

		D3DXVec3Normalize ( &vDir, &vDir );
		D3DXVECTOR3 vSTRPAR = vPosition + vDir*fLength * 10.0f;

		//	스트라이크 위치를 알아냄.
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

			STRIKE::CSELECTOR Selector ( vSTRPAR, pDxAttBone->m_AttBoneArray );
			bOk = Selector.SELECT ( (EMPIECECHAR)ATTBONE_BODY, sStrike );
		}
		else
		{
			STRIKE::CSELECTOR Selector ( vSTRPAR, m_pSkinChar->m_PartArray );
			bOk = Selector.SELECT ( sStrikeEff.m_emPiece, sStrike );
		}

		if ( bOk )
		{
			bTRANS = TRUE;
			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
		}
	}

	//	지면기준 회전 속성 넣어서.
	if ( !bTRANS )
	{
		bTRANS = TRUE;

		D3DXMATRIX matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * GetTransMatrix();
	}

	//	목표 지점 선택
	BOOL bIMPACT(FALSE);
	if ( pSkill->m_sEXT_DATA.emTARG_POSB == SKILL::EMPOS_STRIKE )
	{
		DxSkinChar* pSkin = m_pGaeaClient->GetSkinChar ( sTarget );
		if ( !pSkin )	return;

		//	타겟의 위치.
		D3DXVECTOR3 vTARPOS ( vPosition.x, vPosition.y+15, vPosition.z );;

		//	타격 위치를 알아냄.
		IMPACT_ARROW::SSTRIKE sImpact;
		IMPACT_ARROW::CSELECTOR Selmpact ( vTARPOS, pSkin->m_PartArray );
		BOOL bOk = Selmpact.SELECT ( sImpact );
		if ( bOk )
		{
			bIMPACT = TRUE;
			sTarget.vPos = sImpact.vPos;
		}
	}

    // V607 Ownerless expression 'sTarget.vPos'. glsummonclient.cpp 1451
	//if ( !bIMPACT )
	//{
	//	sTarget.vPos;
	//}

	EMELEMENT emELMT(EMELEMENT_SPIRIT);
	SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARG(emELMT), matTrans, &sTarget, TRUE, FALSE );
	if ( spEffSingleG.get() )
	{
		// 목표 위치 이펙트.
		if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

		// 목표 몸 이펙트.
		if ( pSkill->m_sEXT_DATA.emTARGBODY01==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT) );

		if ( pSkill->m_sEXT_DATA.emTARGBODY02==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT) );
	}
	else
	{
		// 목표 위치 이펙트.
		if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_TARG_OVR )
			DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, TRUE );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_TARG_OVR )
			DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, TRUE );

		//// 목표 몸 이펙트.
		//if ( pSkill->m_sEXT_DATA.emTARGBODY01==SKILL::EMTIME_TARG_OVR )
		//	spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT) );

		//if ( pSkill->m_sEXT_DATA.emTARGBODY02==SKILL::EMTIME_TARG_OVR )
		//	spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT) );
	}
}

void GLSummonClient::SkillEffect ( const SANIMSTRIKE &sStrikeEff )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )							return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	// 타겟 이펙트가 지면을 향해 발사되는 경우.
	//
	EMELEMENT emELMT(EMELEMENT_SPIRIT);
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_ZONE )
	{
		if ( pSkill->m_sEXT_DATA.emTARG == EMTIME_IMPACT )
		{
			D3DXMatrixTranslation ( &matTrans, m_vTARPOS.x, m_vTARPOS.y, m_vTARPOS.z );
			SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE  );
			if ( spEffSingleG.get() )
			{
				if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_TARG_OVR )
					spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_TARG_OVR )
					spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );
			}
			else
			{
				if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_TARG_OVR )
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, TRUE );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_TARG_OVR )
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, TRUE );
			}
		}

		if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE  );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE  );
	}
	// 타겟 이펙트가 지면을 향해 발사되는 경우.
	//		자기 위치에서 대상 위치까지. ( 화살 관통 )
	else if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
	{
		if ( m_wTARNUM == 0 )	return;

		STARGETID sTarget(m_sTARIDS[m_wTARNUM-1].emCrow, m_sTARIDS[m_wTARNUM-1].dwID);
		if ( !m_pGaeaClient->IsVisibleCV(sTarget) )		return;
		sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

		D3DXVECTOR3 vDir = sTarget.vPos - GetPosition();
		vDir.y = 0.f;
		D3DXVec3Normalize ( &vDir, &vDir );

		// 목표 지향 이펙트에 실어 보내는 이펙트.
		//		( 실어 보낼때는 목표 위치 이펙트의 위치는 자동으로 지향 이팩트의 종료 지점으로 고정. )
		if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
		{
			SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
		}
		//	단독으로 목표 지점에 발생.
		else
		{
			// 목표 위치 이펙트.
			if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

			// 목표 몸 이펙트.
			if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
			if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
		}
	}
	// 타겟 이펙트가 유닛을 향해 발사되는 경우.
	//
	else
	{
		// 목표 유닛이 있는 경우.
		//
		if ( m_wTARNUM )
		{
			for ( WORD i=0; i<m_wTARNUM; ++i )
			{
				STARGETID sTarget(m_sTARIDS[i].emCrow, m_sTARIDS[i].dwID);
				if ( !m_pGaeaClient->IsVisibleCV(sTarget) )	continue;
				sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

				D3DXVECTOR3 vDir = sTarget.vPos - GetPosition();
				vDir.y = 0.f;
				D3DXVec3Normalize ( &vDir, &vDir );

				// 목표 지향 이펙트에 실어 보내는 이펙트.
				//		( 실어 보낼때는 목표 위치 이펙트의 위치는 자동으로 지향 이팩트의 종료 지점으로 고정. )
				if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
				{
					SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
				}
				//	단독으로 목표 지점에 발생.
				else
				{
					// 목표 위치 이펙트.
					if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

					if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

					// 목표 몸 이펙트.
					if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
					if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
				}

				if ( pSkill->m_sEXT_DATA.bTARG_ONE )	break;
			}
		}
	}


	STARGETID sTARG(GETCROW(), m_dwGaeaID);

	if ( pSkill->m_sEXT_DATA.emSELFZONE01 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE02 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFBODY == EMTIME_IMPACT )
		DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &GetDirect() );
}

BOOL GLSummonClient::SkillProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime * GETATTVELO();
	//GASSERT(m_pSkinChar->GETCURANIMNODE());

	//PANIMCONTNODE pAnicontNode = m_pSkinChar->GETCURANIMNODE();
	//GASSERT(pAnicontNode);
	//DxSkinAnimation* pAnimCont = pAnicontNode->m_pDxSkinAni;

	const DxSkinAnimation* pAnimCont = m_pSkinChar->GETCURANIM();
	if ( !pAnimCont )
		return FALSE;

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	DWORD dwThisKey = pAnimCont->m_dwSTime + DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

	DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		if ( dwThisKey>=dwStrikeKey )
		{
			// 스킬 이펙트 발동.
			//
			SkillEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );

			m_nattSTEP++;
		}
	}

	return TRUE;
}

HRESULT GLSummonClient::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}


HRESULT GLSummonClient::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT GLSummonClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	if ( m_pTransformSet )
		m_pTransformSet->RestoreDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT GLSummonClient::InvalidateDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}

	if ( m_pTransformSet )
		m_pTransformSet->InvalidateDeviceObjects();

	return S_OK;
}

HRESULT GLSummonClient::DeleteDeviceObjects ()
{
	FactEffectProcessing( EMFACTEFFECTPROCESSING_DESTRUCTOR );

	SAFE_DELETE(m_pTransformSet);
	SAFE_DELETE(m_pSkinChar);

	if ( m_pSkinCharMulti )
	{
		m_pSkinCharMulti->ResetSkinCharMulti();
		SAFE_DELETE(m_pSkinCharMulti);
	}
	
	m_sTargetID.RESET();

	ClientActor::Reset();
	GLSUMMON::RESET ();

	return S_OK;
}

WORD GLSummonClient::GetBodyRadius ()
{
	return GETBODYRADIUS();
}

void GLSummonClient::ReceiveSwing ()
{
	m_pSkinChar->DOSHOCKMIX();
}

const BOOL GLSummonClient::CalcVisibleDetect ( const float fTime )
{
	if ( IsActState(EM_REQ_VISIBLENONE) || IsActState(EM_REQ_VISIBLEOFF) )
		return FALSE;

	if( GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) == FALSE )
		return FALSE;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	const D3DXVECTOR3 vMyPosition(pCharacter->GetPosition() + D3DXVECTOR3(0.0f, 12.0f, 0.0f));
	const D3DXVECTOR3 vPosition(GetPosition() + D3DXVECTOR3(0.0f, 12.0f, 0.0f));

	// 	if ( m_pLandManager->IsCullByObjectMap() == true )
	// 	{ // 기획팀 요청으로 CROW/NPC는 케릭터 시야 컬링에서 제외;
	// 		// 플레이어만 컬링 대상에 포함됨;
	// 		if ( m_pGaeaClient->IsCollision(vMyPosition, vPosition) == true )
	// 			return FALSE; // 내 케릭터 시야에 없다면 안보임;
	// 	}	

	if ( IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) == FALSE )
		return TRUE;	

	const SRECVISIBLE& RECVISIBLE = pCharacter->m_sRECVISIBLE;		

	const D3DXVECTOR3 vDistance(vPosition - vMyPosition);	
	const float fDistance = ::D3DXVec3Length(&vDistance);

	if ( IsCheckedSkillFlagSpecial(EMSPECA_RECVISIBLE) &&
		(m_sINVISIBLE.dwLevelINVISIBLE <= RECVISIBLE.dwLevelRECVISIBLE) &&
		(fDistance < RECVISIBLE.fRadiusRECVISIBLE) )
		return TRUE;

	return CalcVisibleDetectAuto(fTime, fDistance, vDistance);
}

const BOOL GLSummonClient::CalcVisibleDetectAuto (
	const float fTime,
	const float fDistance,
	const D3DXVECTOR3& vDistance )
{
	if ( fDistance > GLHIDESET::fInvisiblePerceiveRadius )
		m_sRECVISIBLE.bFLAG = false;
	else if ( (fTime - m_sRECVISIBLE.fLatestCheckTime) > GLHIDESET::fInvisibleUpdateTime )
	{
		m_sRECVISIBLE.fLatestCheckTime = fTime;

		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		const int iGapLevel = pCharacter->m_wLevel - GETLEVEL();
		const int nConstPercentage = 
			(iGapLevel > 0 ? int(float(iGapLevel) * GLHIDESET::fInvisiblePerceiveLevel) : 5);

		// 좌표값 x + y + z + fDistance에서 소숫점 두자리를 취해 확률 값으로 사용;
		// 동일한 위치에서 발견되고 서로 움직이지 않으면 계속 보임;		
		const float fPercentage = abs(vDistance.x + vDistance.y + vDistance.z + fDistance);
		const int nPercentage = int((fPercentage - float(int(fPercentage))) * 100.0f);		

		D3DXVECTOR3 vDistanceNormal;
		::D3DXVec3Normalize(&vDistanceNormal, &vDistance);;
		const D3DXVECTOR3& vDirectionNormal = pCharacter->GetDirect();
		const float fDot = ::D3DXVec3Dot(&vDistanceNormal, &vDirectionNormal);

		if ( fDot > GLHIDESET::fInvisiblePerceiveAngle )
			m_sRECVISIBLE.bFLAG = nPercentage < (nConstPercentage * (2.0f - fDistance * GLHIDESET::fInvisiblePerceiveURadius));
		else
			m_sRECVISIBLE.bFLAG = false;
	}	
	return m_sRECVISIBLE.bFLAG;
}

BOOL GLSummonClient::IsVisibleDetect(void) const
{
	const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
	if ( m_dwOwnerGaeaID == dwMyGaeaID )
		return TRUE;

	if ( m_pLandManClient->IsCullByObjectMap() == true )
	{
		// 내 케릭터 시야에 없다면 안보임;
		if ( m_spCollisionByObject->GetCollsion() )
			return FALSE;

		//if ( m_pGaeaClient->IsCollision(GetPosition(), m_pGaeaClient->GetCharacter()->GetPosition(), EMCC_CULL_NONE_ONLY_WALL) )
		//	return FALSE; // 케릭터 시야에 없다면 보임;
	}

	if ( IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) )
		return FALSE;

	return TRUE;
}

BOOL GLSummonClient::IsCollisionVolume ()
{
	const CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume();
	if ( COLLISION::IsCollisionVolume ( CV, m_vMax, m_vMin ) )	return TRUE;

	return FALSE;
}

void GLSummonClient::ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR)
{
	if ( dwDamageFlag & DAMAGE_TYPE_SHOCK )	
	{
		//		m_pSkinChar->SELECTANI ( AN_SHOCK, AN_SUB_NONE );
		//		if ( m_pSkinChar->IsANI_ISENDANIM() )			SetSTATE ( EM_SUMMONACT_STOP );
		//		SetSTATE ( EM_SUMMONACT_SHOCK );
		TurnAction ( GLAT_SHOCK );
	}

	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 
			nDamage, dwDamageFlag, UI_ATTACK, sACTOR, m_sTargetID );

		if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
		{
			const D3DXVECTOR3& vPosition = GetPosition();

			// 강한타격 이펙트
			D3DXVECTOR3 vDIR = sACTOR.vPos - vPosition;

			D3DXVECTOR3 vDIR_ORG(1,0,0);
			float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

			D3DXMATRIX matTrans;
			D3DXMatrixRotationY ( &matTrans, fdir_y );
			matTrans._41 = vPosition.x;
			matTrans._42 = vPosition.y + 10.0f;
			matTrans._43 = vPosition.z;

			// 자기 위치 이펙트 발생시킴.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR, TRUE, FALSE );
		}
	}

	RECEIVE_DAMAGE(nDamage);

	SKT_EFF_HOLDOUT ( sACTOR, dwDamageFlag );
}

void GLSummonClient::ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR)
{
	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, nHeal, dwHealFlag, UI_UNDERATTACK, sACTOR, m_sTargetID );
	}

	RECEIVE_HEAL(nHeal);
}

void GLSummonClient::ReceiveBlocking(const STARGETID& sACTOR)
{
	BOOL bCollision = FALSE;

	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, DAMAGE_TYPE_NONE, UI_ATTACK, sACTOR, m_sTargetID );
		bCollision = TRUE;
	}
}

void GLSummonClient::ReceiveAVoid ( const STARGETID& sACTOR )
{
	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, FALSE, UI_ATTACK, sACTOR, m_sTargetID );
	}
}

// BOOL GLSummonClient::IsValidBody () const
// {
// 	return ( m_Action < GLAT_FALLING );
// }
void GLSummonClient::AddSkillFact_SpecialAddon_Client(const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF)
{
	SKILL::SSPEC_ADDON_LEVEL sAddon;

	sAddon.emSPEC			= emAddon;
	sAddon.sSPEC.fVAR1 	= sSPEC.fVAR1;
	sAddon.sSPEC.fVAR2 	= sSPEC.fVAR2;
	sAddon.sSPEC.fRate 		= sSPEC.fRate;
	sAddon.sSPEC.fRate2 	= sSPEC.fRate2;
	sAddon.sSPEC.dwFLAG 	= sSPEC.dwFLAG;
	sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

	sSKILLEF.vecSPEC.push_back( sAddon );
}

/*void GLSummonClient::RECEIVE_SKILLFACT_Invisible(const float fVAR2)
{
	m_sINVISIBLE.bPrevStateRun = IsActState(EM_ACT_RUN);
	switch ( EMSPEC_INVISIBLE_ANI_TYPE(DWORD(fVAR2)) ){
	case EMSPEC_INVISIBLE_ANI_TYPE_WALK:
		ReSetActState(EM_ACT_RUN);
		break;
	case EMSPEC_INVISIBLE_ANI_TYPE_RUN:
		SetActState(EM_ACT_RUN);
		break;
	}
}*/

const BOOL GLSummonClient::RECEIVE_SKILLFACT_AIRBORNE(const SKILL::SSPEC& sSPEC, const SKILL::CDATA_LVL& sSKILL_DATA)
{
	if ( (this->m_bSkillFactsSpecial[EMSPECA_AIRBORNE] == true) && (sSPEC.dwFLAG == 0) )
		return FALSE;

	const float _fTimeLife(sSKILL_DATA.fLIFE);
	const float _fTimeLevitate(sSPEC.fVAR1);
	const float _fHeightMax(sSPEC.fVAR2);	
	this->m_sAirborne.push_back(_fTimeLife, _fTimeLevitate, _fHeightMax);

	return TRUE;
}

const BOOL GLSummonClient::RECEIVE_SKILLFACT( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg )
{
	// Note : Normal 타입이 아니면 튕긴다.
	if ( EMSKILLFACTTYPE_BY_INDEX(pMsg->wSELSLOT) != EMSKILLFACT_TYPE_NORMAL )
	{
		return FALSE;
	}

		//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pMsg->skill_id );
	if ( !pSkill )
		return FALSE;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[pMsg->wLEVEL];

	SSKILLFACT sSKILLEF;

	BOOL bHOLD = FALSE;

	GLSKILL tempSkill = *pSkill;

	tempSkill.m_sAPPLY.vecADDON.clear();
	tempSkill.m_sAPPLY.vecSPEC.clear();

	std::vector<SKILL::SSPEC_ADDON>::iterator   _viterI;
	std::vector<SKILL::SSPEC_ADDON> vAddon= pSkill->m_sAPPLY.vecSPEC;

	std::vector<SKILL::SIMPACT_ADDON>::iterator _viterII;
	
	for( int i = 0; i < SKILL::MAX_ADDON; ++i)
	{
		_viterI = vAddon.begin();

		while( _viterI != vAddon.end() )
		{
			if( _viterI->emSPEC == static_cast<EMSPEC_ADDON>(pMsg->sEffInfo.iRecvEAIdx[i]) )
			{
				tempSkill.m_sAPPLY.vecSPEC.push_back( *_viterI );

				vAddon.erase(_viterI);
				break;
			}

			++_viterI;
		}

		_viterII = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( static_cast<EMIMPACT_ADDON>(pMsg->sEffInfo.iRecvSAIdx[i]) ));

		if( _viterII != pSkill->m_sAPPLY.vecADDON.end() )
		{
			tempSkill.m_sAPPLY.vecADDON.push_back( *_viterII );
		}

	}

	switch ( tempSkill.m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_VARHP:
	case SKILL::EMFOR_VARMP:
	case SKILL::EMFOR_VARSP:
	case SKILL::EMFOR_DEFENSE:
	case SKILL::EMFOR_HITRATE:
	case SKILL::EMFOR_AVOIDRATE:
	case SKILL::EMFOR_VARAP:
	case SKILL::EMFOR_VARDAMAGE:
	case SKILL::EMFOR_VARDEFENSE:
	case SKILL::EMFOR_PA:
	case SKILL::EMFOR_SA:
	case SKILL::EMFOR_MA:
	case SKILL::EMFOR_RESIST:
	case SKILL::EMFOR_SUMMON_ACTIVETIME:
		bHOLD = TRUE;
		sSKILLEF.emTYPE = tempSkill.m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = sSKILL_DATA.fBASIC_VAR;
		break;
	};

	for ( DWORD i = 0; i < tempSkill.m_sAPPLY.vecADDON.size(); ++i )
	{
		bHOLD = TRUE;
		SKILL::SIMPACT_ADDON_LEVEL sAddon;
		
		sAddon.emADDON = tempSkill.m_sAPPLY.vecADDON[i].emADDON;
		sAddon.fADDON_VAR = tempSkill.m_sAPPLY.vecADDON[i].fADDON_VAR[pMsg->wLEVEL];
		sAddon.fRate = tempSkill.m_sAPPLY.vecADDON[i].fRate[pMsg->wLEVEL]; // 효과 비율 적용. by luxes.
		sSKILLEF.vecADDON.push_back( sAddon );
	}

	for ( DWORD i = 0; i < tempSkill.m_sAPPLY.vecSPEC.size(); ++i )
	{
		const EMSPEC_ADDON emAddon(tempSkill.m_sAPPLY.vecSPEC[i].emSPEC);
		const SKILL::SSPEC& sSPEC(tempSkill.m_sAPPLY.vecSPEC[i].sSPEC[pMsg->wLEVEL]);

		switch ( emAddon )
		{
		case EMSPECA_ACTIVATE_DAMAGE :
		case EMSPECA_SPECIAL_IMMUNE :
			{
				bHOLD = TRUE;
				AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_PROVOKE :
			{
				bHOLD = TRUE;
				AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_COUNTATTACK :
			bHOLD = TRUE;
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;
		case EMSPECA_AIRBORNE:
			if ( RECEIVE_SKILLFACT_AIRBORNE(sSPEC, tempSkill.m_sAPPLY.sDATA_LVL[pMsg->wLEVEL]) == TRUE )
			{
				bHOLD = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		/*case EMSPECA_INVISIBLE:
			bHOLD = true;
			RECEIVE_SKILLFACT_Invisible(sSPEC.fVAR2);
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;*/
		case EMSPECA_IGNORED_DAMAGE:
		case EMSPECA_REFDAMAGE:
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_ATTACKVELO:
		case EMSPECA_SKILLDELAY:
		case EMSPECA_PSY_DAMAGE_REDUCE:
		case EMSPECA_MAGIC_DAMAGE_REDUCE:
		case EMSPECA_PSY_DAMAGE_REFLECTION:
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
		case EMSPECA_DURATION_DAMAGE:
		case EMSPECA_CURSE:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
        case EMSPECA_TAUNT:
		case EMSPECA_SKILL_LINK:
		case EMSPECA_DAMAGE_VALUE:
		case EMSPECA_DAMAGE_RATE:
		case EMSPECA_DEFENSE_VALUE	:
		case EMSPECA_DEFENSE_RATE:
		case EMSPECA_HIT_VALUE:
		case EMSPECA_HIT_RATE:
		case EMSPECA_AVOID_VALUE:
		case EMSPECA_AVOID_RATE:
		case EMSPECA_HP_VALUE:
		case EMSPECA_MP_VALUE:
		case EMSPECA_SP_VALUE:
		case EMSPECA_CP_VALUE:
		case EMSPECA_HP_VARIATION_RATE:
		case EMSPECA_MP_VARIATION_RATE:
		case EMSPECA_SP_VARIATION_RATE:
		case EMSPECA_HMS_VARIATION_RATE:
		case EMSPECA_ATTACK_RANGE_VALUE:
		case EMSPECA_DIS_SP_VALUE:
		case EMSPECA_RESIST_VALUE:
		case EMSPECA_STATS_POW_VALUE:
		case EMSPECA_STATS_STR_VALUE	:
		case EMSPECA_STATS_SPI_VALUE:
		case EMSPECA_STATS_DEX_VALUE	:
		case EMSPECA_STATS_STA_VALUE:
		case EMSPECA_PA_VALUE:
		case EMSPECA_SA_VALUE	:
		case EMSPECA_MA_VALUE:
		case EMSPECA_HP_POTION_INC_VALUE:
		case EMSPECA_MP_POTION_INC_VALUE:
		case EMSPECA_SP_POTION_INC_VALUE:
		case EMSPECA_CP_INC_VALUE:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_RECVISIBLE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_STIGMA:
		case EMSPECA_INCREASE_EFF:
		case EMSPECA_DOMINATION:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
        case EMSPECA_MOVEVELO_LIMIT:
			bHOLD = TRUE;
			AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			break;
		};
	}

	// 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//
	if ( bHOLD /*&& !m_pSummonCrowData->m_bNoAllPenalty*/ )
	{
		switch ( tempSkill.m_sBASIC.emACTION_TYPE )
		{
		case SKILL::EMACTION_TYPE_NORMAL: if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_NORMAL ) goto NOPENALTY; break;
		case SKILL::EMACTION_TYPE_DEBUFF: if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_DEBUFF ) goto NOPENALTY; break;
		case SKILL::EMACTION_TYPE_LIMIT:  if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_LIMIT  ) goto NOPENALTY; break;
		}

		sSKILLEF.sNATIVEID	= pMsg->skill_id;
		sSKILLEF.wLEVEL		= pMsg->wLEVEL;
		sSKILLEF.bEnable    = TRUE;

        if ( pMsg->sTIME.IsEXPIREDTIME )
        {
            sSKILLEF.SetAgeInfinite();
            sSKILLEF.nEXPIREDTIME = pMsg->sTIME.nEXPIREDTIME;
        }
        else
        {
            if ( pMsg->sTIME.fAGE == _SKILLFACT_DEFAULTTIME )	sSKILLEF.fAGE = sSKILL_DATA.fLIFE;
            else	sSKILLEF.fAGE = pMsg->sTIME.fAGE;
        }


		if ( pMsg->wSELSLOT < CROW_SKILLFACT_SIZE )
		{
			m_sSKILLFACT[pMsg->wSELSLOT] = sSKILLEF;
		}

		return TRUE;
	}

NOPENALTY:


	return FALSE;
}
const BOOL GLSummonClient::RECEIVE_SKILLFACT ( const SNATIVEID& skill_id, const WORD wlevel, const DWORD dwSELECT, const STARGETID& sID, const SSKILLFACT::STIME sTime, const DWORD dwCount )
{
	// Note : Normal 타입이 아니면 튕긴다.
	if ( EMSKILLFACTTYPE_BY_INDEX(dwSELECT) != EMSKILLFACT_TYPE_NORMAL )
	{
		return FALSE;
	}

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if ( !pSkill )				return FALSE;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wlevel];

	BOOL bHOLD = FALSE;
	SSKILLFACT sSKILLEF;
	switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
	{
	case SKILL::EMFOR_VARHP:
	case SKILL::EMFOR_VARMP:
	case SKILL::EMFOR_VARSP:
	case SKILL::EMFOR_DEFENSE:
	case SKILL::EMFOR_HITRATE:
	case SKILL::EMFOR_AVOIDRATE:
	case SKILL::EMFOR_VARAP:
	case SKILL::EMFOR_VARDAMAGE:
	case SKILL::EMFOR_VARDEFENSE:
	case SKILL::EMFOR_PA:
	case SKILL::EMFOR_SA:
	case SKILL::EMFOR_MA:
	case SKILL::EMFOR_RESIST:
	case SKILL::EMFOR_SUMMON_ACTIVETIME:
		bHOLD = TRUE;
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = sSKILL_DATA.fBASIC_VAR;
		break;
	};

	for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecADDON.size(); ++i )
	{
		bHOLD = TRUE;
		SKILL::SIMPACT_ADDON_LEVEL sAddon;
		
		sAddon.emADDON = pSkill->m_sAPPLY.vecADDON[i].emADDON;
		sAddon.fADDON_VAR = pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel];
		sAddon.fRate = pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel]; // 효과 비율 적용. by luxes.
		sSKILLEF.vecADDON.push_back( sAddon );
	}

	for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
	{
		const EMSPEC_ADDON emAddon(pSkill->m_sAPPLY.vecSPEC[i].emSPEC);
		const SKILL::SSPEC& sSPEC(pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wlevel]);

		switch ( emAddon )
		{
		case EMSPECA_COUNTATTACK :
			bHOLD = TRUE;
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;
		case EMSPECA_AIRBORNE:
			if ( RECEIVE_SKILLFACT_AIRBORNE(sSPEC, pSkill->m_sAPPLY.sDATA_LVL[wlevel]) == TRUE )
			{
				bHOLD = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		/*case EMSPECA_INVISIBLE:
			bHOLD = true;
			RECEIVE_SKILLFACT_Invisible(sSPEC.fVAR2);
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;*/
		case EMSPECA_IGNORED_DAMAGE:
		case EMSPECA_REFDAMAGE:
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_ATTACKVELO:
		case EMSPECA_SKILLDELAY:
		case EMSPECA_PSY_DAMAGE_REDUCE:
		case EMSPECA_MAGIC_DAMAGE_REDUCE:
		case EMSPECA_PSY_DAMAGE_REFLECTION:
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
		case EMSPECA_DURATION_DAMAGE:
		case EMSPECA_CURSE:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
        case EMSPECA_TAUNT:
		case EMSPECA_SKILL_LINK:
		case EMSPECA_DAMAGE_VALUE:
		case EMSPECA_DAMAGE_RATE:
		case EMSPECA_DEFENSE_VALUE	:
		case EMSPECA_DEFENSE_RATE:
		case EMSPECA_HIT_VALUE:
		case EMSPECA_HIT_RATE:
		case EMSPECA_AVOID_VALUE:
		case EMSPECA_AVOID_RATE:
		case EMSPECA_HP_VALUE:
		case EMSPECA_MP_VALUE:
		case EMSPECA_SP_VALUE:
		case EMSPECA_CP_VALUE:
		case EMSPECA_HP_VARIATION_RATE:
		case EMSPECA_MP_VARIATION_RATE:
		case EMSPECA_SP_VARIATION_RATE:
		case EMSPECA_HMS_VARIATION_RATE:
		case EMSPECA_ATTACK_RANGE_VALUE:
		case EMSPECA_DIS_SP_VALUE:
		case EMSPECA_RESIST_VALUE:
		case EMSPECA_STATS_POW_VALUE:
		case EMSPECA_STATS_STR_VALUE	:
		case EMSPECA_STATS_SPI_VALUE:
		case EMSPECA_STATS_DEX_VALUE	:
		case EMSPECA_STATS_STA_VALUE:
		case EMSPECA_PA_VALUE:
		case EMSPECA_SA_VALUE	:
		case EMSPECA_MA_VALUE:
		case EMSPECA_HP_POTION_INC_VALUE:
		case EMSPECA_MP_POTION_INC_VALUE:
		case EMSPECA_SP_POTION_INC_VALUE:
		case EMSPECA_CP_INC_VALUE:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_RECVISIBLE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_STIGMA:
		case EMSPECA_INCREASE_EFF:
		case EMSPECA_DOMINATION:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
        case EMSPECA_MOVEVELO_LIMIT:
			bHOLD = TRUE;
			AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			break;
		};
	}

	// 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//
	if ( bHOLD && !m_pSummonCrowData->m_bNoAllPenalty )
	{
		switch ( pSkill->m_sBASIC.emACTION_TYPE )
		{
		case SKILL::EMACTION_TYPE_NORMAL: if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_NORMAL ) goto NOPENALTY; break;
		case SKILL::EMACTION_TYPE_DEBUFF: if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_DEBUFF ) goto NOPENALTY; break;
		case SKILL::EMACTION_TYPE_LIMIT:  if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_LIMIT  ) goto NOPENALTY; break;
		}

		sSKILLEF.sNATIVEID	= skill_id;
		sSKILLEF.wLEVEL		= wlevel;
		sSKILLEF.bEnable    = TRUE;

        if ( sTime.IsEXPIREDTIME )
        {
            sSKILLEF.SetAgeInfinite();
            sSKILLEF.nEXPIREDTIME = sTime.nEXPIREDTIME;
        }
        else
        {
            if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )	sSKILLEF.fAGE = sSKILL_DATA.fLIFE;
            else									    sSKILLEF.fAGE = sTime.fAGE;
        }


		if ( dwSELECT < CROW_SKILLFACT_SIZE )
		{
			m_sSKILLFACT[dwSELECT] = sSKILLEF;
		}

		return TRUE;
	}

NOPENALTY:

	//GASSERT ( 0 && "지속 스킬정보가 유효하지 않음." );

    return FALSE;
}

const BOOL GLSummonClient::RECEIVE_SKILLCA( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg )
{	
		// Note : Normal 타입이 아니면 튕긴다.
	if ( EMSKILLFACTTYPE_BY_INDEX(pMsg->wSELSLOT) != EMSKILLFACT_TYPE_NORMAL )
	{
		return FALSE;
	}

		//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pMsg->skill_id );
	if ( !pSkill )
		return FALSE;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[pMsg->wLEVEL];

	SSKILLFACT sSKILLEF;

	BOOL bHOLD = FALSE;

	GLSKILL tempSkill = *pSkill;

	tempSkill.m_sAPPLY.vecADDON.clear();
	tempSkill.m_sAPPLY.vecSPEC.clear();

	std::vector<SKILL::SSPEC_ADDON>::iterator   _viterI;
	std::vector<SKILL::SSPEC_ADDON> vAddon = pSkill->m_sAPPLY.vecSPEC;

	std::vector<SKILL::SIMPACT_ADDON>::iterator _viterII;

	for( int i = 0; i < SKILL::MAX_ADDON; ++i)
	{
		_viterI = vAddon.begin();

		while( _viterI != vAddon.end() )
		{
			if( _viterI->emSPEC == static_cast<EMSPEC_ADDON>(pMsg->sEffInfo.iRecvEAIdx[i]) )
			{
				tempSkill.m_sAPPLY.vecSPEC.push_back( *_viterI );

				vAddon.erase(_viterI);
				break;
			}

			++_viterI;
		}

		_viterII = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( static_cast<EMIMPACT_ADDON>(pMsg->sEffInfo.iRecvSAIdx[i]) ));

		if( _viterII != pSkill->m_sAPPLY.vecADDON.end() )
		{
			tempSkill.m_sAPPLY.vecADDON.push_back( *_viterII );
		}

	}

	for ( DWORD i = 0; i < tempSkill.m_sAPPLY.vecADDON.size(); ++i )
	{
		bHOLD = TRUE;
		SKILL::SIMPACT_ADDON_LEVEL sAddon;
		
		sAddon.emADDON = tempSkill.m_sAPPLY.vecADDON[i].emADDON;
		sAddon.fADDON_VAR = tempSkill.m_sAPPLY.vecADDON[i].fADDON_VAR[pMsg->wLEVEL];
		sAddon.fRate = tempSkill.m_sAPPLY.vecADDON[i].fRate[pMsg->wLEVEL]; // 효과 비율 적용. by luxes.
		sSKILLEF.vecADDON.push_back( sAddon );
	}

	for ( DWORD i = 0; i < tempSkill.m_sAPPLY.vecSPEC.size(); ++i )
	{
		const EMSPEC_ADDON emAddon(tempSkill.m_sAPPLY.vecSPEC[i].emSPEC);
		const SKILL::SSPEC& sSPEC(tempSkill.m_sAPPLY.vecSPEC[i].sSPEC[pMsg->wLEVEL]);

		switch ( emAddon )
		{
		case EMSPECA_AIRBORNE:
			if ( RECEIVE_SKILLFACT_AIRBORNE(sSPEC, tempSkill.m_sAPPLY.sDATA_LVL[pMsg->wLEVEL]) == TRUE )
			{
				bHOLD = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		/*case EMSPECA_INVISIBLE:
			bHOLD = true;
			RECEIVE_SKILLFACT_Invisible(sSPEC.fVAR2);
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;*/
		case EMSPECA_IGNORED_DAMAGE:
		case EMSPECA_REFDAMAGE:
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_ATTACKVELO:
		case EMSPECA_SKILLDELAY:
		case EMSPECA_PSY_DAMAGE_REDUCE:
		case EMSPECA_MAGIC_DAMAGE_REDUCE:
		case EMSPECA_PSY_DAMAGE_REFLECTION:
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
		case EMSPECA_DURATION_DAMAGE:
		case EMSPECA_CURSE:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
        case EMSPECA_TAUNT:
		case EMSPECA_SKILL_LINK:
		case EMSPECA_DAMAGE_VALUE:
		case EMSPECA_DAMAGE_RATE:
		case EMSPECA_DEFENSE_VALUE	:
		case EMSPECA_DEFENSE_RATE:
		case EMSPECA_HIT_VALUE:
		case EMSPECA_HIT_RATE:
		case EMSPECA_AVOID_VALUE:
		case EMSPECA_AVOID_RATE:
		case EMSPECA_HP_VALUE:
		case EMSPECA_MP_VALUE:
		case EMSPECA_SP_VALUE:
		case EMSPECA_CP_VALUE:
		case EMSPECA_HP_VARIATION_RATE:
		case EMSPECA_MP_VARIATION_RATE:
		case EMSPECA_SP_VARIATION_RATE:
		case EMSPECA_HMS_VARIATION_RATE:
		case EMSPECA_ATTACK_RANGE_VALUE:
		case EMSPECA_DIS_SP_VALUE:
		case EMSPECA_RESIST_VALUE:
		case EMSPECA_STATS_POW_VALUE:
		case EMSPECA_STATS_STR_VALUE	:
		case EMSPECA_STATS_SPI_VALUE:
		case EMSPECA_STATS_DEX_VALUE	:
		case EMSPECA_STATS_STA_VALUE:
		case EMSPECA_PA_VALUE:
		case EMSPECA_SA_VALUE	:
		case EMSPECA_MA_VALUE:
		case EMSPECA_HP_POTION_INC_VALUE:
		case EMSPECA_MP_POTION_INC_VALUE:
		case EMSPECA_SP_POTION_INC_VALUE:
		case EMSPECA_CP_INC_VALUE:
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_RECVISIBLE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_STIGMA:
		case EMSPECA_INCREASE_EFF:
		case EMSPECA_DOMINATION:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
        case EMSPECA_MOVEVELO_LIMIT:
		//case EMSPECA_SCREEN_THROW :
			bHOLD = TRUE;
			AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			break;
		};
	}

	// 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//
	if ( bHOLD && !m_pSummonCrowData->m_bNoAllPenalty )
	{
		switch ( tempSkill.m_sBASIC.emACTION_TYPE )
		{
		case SKILL::EMACTION_TYPE_NORMAL: if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_NORMAL ) goto NOPENALTY; break;
		case SKILL::EMACTION_TYPE_DEBUFF: if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_DEBUFF ) goto NOPENALTY; break;
		case SKILL::EMACTION_TYPE_LIMIT:  if ( m_pSummonCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_LIMIT  ) goto NOPENALTY; break;
		}

		sSKILLEF.sNATIVEID	= pMsg->skill_id;
		sSKILLEF.wLEVEL		= pMsg->wLEVEL;
		sSKILLEF.bEnable    = TRUE;

        if ( pMsg->sTIME.IsEXPIREDTIME )
        {
            sSKILLEF.SetAgeInfinite();
            sSKILLEF.nEXPIREDTIME = pMsg->sTIME.nEXPIREDTIME;
        }
        else
        {
            if ( pMsg->sTIME.fAGE == _SKILLFACT_DEFAULTTIME )	sSKILLEF.fAGE = sSKILL_DATA.fLIFE;
            else	sSKILLEF.fAGE = pMsg->sTIME.fAGE;
        }


		if ( pMsg->wSELSLOT < CROW_SKILLFACT_SIZE )
		{
			m_sSKILLFACT[pMsg->wSELSLOT] = sSKILLEF;
		}

		return TRUE;
	}

NOPENALTY:


	return FALSE;

}
// Note : bjju. 상황에 따라 Effect를 처리하는 함수
void GLSummonClient::FactEffectProcessing ( EMFACTEFFPROCESSING emFactEffCase )
{
    FACTEFF::DeleteAllEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), GetSkinChar(), m_sSKILLFACT, CROW_SKILLFACT_SIZE, m_sSTATEBLOWS );

    for ( int i=0; i<EMBLOW_MULTI;   ++i )												
        m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;

    for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )												
        m_sSKILLFACT[i].RESET();
}

BOOL GLSummonClient::IsVisibleDisplayName() const
{
	if ( m_spNameCollision->GetCollsion() )
		return FALSE;

	return TRUE;
}

const BOOL GLSummonClient::IsNonTargetable(void) const
{
	return  m_bVisible == FALSE ||
		m_sHALLUCINATE.bNonTargetable ||
		( m_pSummonCrowData && m_pSummonCrowData->m_bNonTargeting );
}

void GLSummonClient::SELECTANI( DxSkinChar* pRenderChar, const TCHAR* szAnim, DWORD dwFlags/*=NULL*/ )
{
	if ( pRenderChar )
		pRenderChar->SELECTANI ( szAnim, dwFlags );

	if ( m_pSkinCharMulti ) 
		m_pSkinCharMulti->SELECTANI ( szAnim, dwFlags );
}

void GLSummonClient::SELECTANI( DxSkinChar* pRenderChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0*/ )
{
	if ( pRenderChar )
		pRenderChar->SELECTANI ( MType, SType, dwFlags, dwSelect );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SELECTANI ( MType, SType, dwFlags, dwSelect );
}

void GLSummonClient::DEFAULTANI( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->DEFAULTANI();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->DEFAULTANI();
}

void GLSummonClient::TOSTARTTIME( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->TOSTARTTIME();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->TOSTARTTIME();
}

void GLSummonClient::TOENDTIME( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->TOENDTIME();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->TOENDTIME();
}

void GLSummonClient::SELECTSKILLANI( DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE */ )
{
	pSkinChar->SELECTSKILLANI ( MType, SType );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SELECTSKILLANI( MType, SType );
}

BOOL GLSummonClient::IsANI_SELECTANI( DxSkinChar *pSkinChar, const TCHAR* szAnim, DWORD dwFlags/*=NULL */ )
{
	BOOL bOK = FALSE;
	bOK = pSkinChar->IsANI_SELECTANI ( szAnim, dwFlags );

	//if ( m_pSkinCharMulti )
	//	bOK = m_pSkinCharMulti->IsANI_SELECTANI ( szAnim, dwFlags );

	return bOK;
}

BOOL GLSummonClient::IsANI_SELECTANI( DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0 */ )
{
	BOOL bOK = FALSE;
	bOK = pSkinChar->IsANI_SELECTANI ( MType, SType, dwFlags, dwSelect );

	//if ( m_pSkinCharMulti )
	//	bOK = m_pSkinCharMulti->IsANI_SELECTANI (  MType, SType, dwFlags, dwSelect  );

	return bOK;
}

void GLSummonClient::SetPositionSkin( DxSkinChar *pSkinChar, const D3DXVECTOR3& vPos )
{
	pSkinChar->SetPosition ( vPos );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SetPosition( vPos );
}

HRESULT GLSummonClient::FrameMoveSkin( DxSkinChar *pSkinChar, const float fTime, const float fElapsedTime, BOOL bNoneOverFrame, const D3DXMATRIX &matRot, const BOOL bContinue/*=TRUE*/, BOOL const bFreeze/*=FALSE*/, const BOOL bAttackMode/*=TRUE*/, const BOOL bBoardRender /*= FALSE */ )
{
	HRESULT hr = S_FALSE;
	hr = pSkinChar->FrameMove ( fTime, fElapsedTime, bNoneOverFrame, matRot, bContinue, bFreeze, bAttackMode, bBoardRender );
	if ( E_FAIL == hr )
		return hr;

	// 메인의 세팅에 실패하면 분신의 세팅에는 자동으로 실패한다;
	if ( m_pSkinCharMulti )
		hr = m_pSkinCharMulti->FrameMove( fTime, fElapsedTime, bNoneOverFrame, matRot , bContinue, bFreeze, bAttackMode, bBoardRender );

	return hr;
}
