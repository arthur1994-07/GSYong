#include "../pch.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/DxEnvironment.h"
#include "../../EngineLib/GlobalParam.h"

#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Land/GLLandManClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../Strike/GLStrikeM.h"
#include "../GLGaeaClient.h"
#include "./GLPetClient.h"


#include "../../SigmaCore/DebugInclude.h"

//  아이템 줍기 시도 요청 제한 시간
static const float fPET_PICKUP_ITEM_TIME = 0.5f;

// Q박스 경고 재출력 시간;
static const float fPET_GETQBOX_DISABLE_MSG = 300.0f;

GLPetClient::GLPetClient(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
	, m_pd3dDevice(NULL)
    , m_vDir(D3DXVECTOR3(0,0,-1))
    , m_vDirOrig(D3DXVECTOR3(0,0,-1))
    , m_vPos(D3DXVECTOR3(0,0,0))
    
    ,
	m_vMax(6,20,6),
	m_vMin(-6,0,-6),
	m_fHeight(20.f),

	m_vMaxOrg(6,20,6),
	m_vMinOrg(-6,0,-6),

	m_pOwner(NULL),

	m_bValid(false),
	m_bReadyActive(false),
	m_dwActionFlag(0),
	m_fIdleTimer(0.0f),
	m_fAttackTimer(0.0f),
    m_fPickItemTimer(0.0f),
	m_fQBoxLockNotifyTimer(fPET_GETQBOX_DISABLE_MSG),
	m_fQboxPickableTimer(0.0f),
	m_sPetCrowID(NATIVEID_NULL()),
	m_wColorTEMP(0),
	m_wStyleTEMP(0),
	m_bEnableSytle(FALSE),
	m_bEnableColor(FALSE),
	m_vRandPos(0.0f,0.0f,0.0f)

	, m_spWorldLight(new ActorWorldLight)
	, m_spNameCollision(new ActorNameCollision(EMCC_CULL_NONE))
	, m_pSkinChar(new DxSkinChar)	// m_pSkinChar 는 NULL 은 존재하지 않는다는 가정이 들어간다.
{
	SecureZeroMemory ( m_wAniSub,sizeof(WORD)*3 );
	m_vecDroppedItems.reserve(50);
	m_sSkillTARGET.RESET ();
	m_SKILLDELAY.clear();

	m_sPetSkinPackState.Init();

	m_vecActiveSkillInfo.clear();
}

GLPetClient::~GLPetClient(void)
{
	SAFE_DELETE ( m_pSkinChar );
}

HRESULT GLPetClient::Create ( const PGLPET pPetData, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, boost::shared_ptr<NavigationMesh> spNavi, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !spNavi || !pd3dDevice ) 
		return E_FAIL;

	m_pd3dDevice = pd3dDevice;

	ASSIGN ( *pPetData );

	m_vPos = vPos;
	m_vDir = vDir;
	m_pOwner = m_pGaeaClient->GetCharacter ();

	m_actorMove.Create ( spNavi, m_vPos );

	if ( FAILED ( SkinLoad ( pd3dDevice ) ) )
		return E_FAIL;

	UpdateSuit ( TRUE );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;
	
	m_bValid = true;
	m_bReadyActive = false;

	ReSetAllSTATE ();
	SetSTATE ( EM_PETACT_STOP );

	// Note : 초기화
	m_vecActiveSkillInfo.clear();
	//m_vecActiveSkillInfo._Mylast = m_vecActiveSkillInfo._Myfirst;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_OpenWindow );

	return S_OK;
}

void GLPetClient::Delete()
{
	m_actorMove.Stop ();
	m_actorMove.Release ();

	m_pd3dDevice = NULL;
	m_pOwner	 = NULL;
	m_vDir	     = D3DXVECTOR3 (0,0,-1);
	m_vPos	     = D3DXVECTOR3(0,0,0);
	m_vDirOrig   = D3DXVECTOR3(0,0,-1);
	m_bValid	 = false;
	m_bReadyActive	 = false;
	ReSetAllSTATE ();
	SetSTATE ( EM_PETACT_STOP );

	m_sSkillTARGET.RESET ();
	m_fIdleTimer = 0.0f;
	m_fAttackTimer = 0.0f;
	m_fPickItemTimer = 0.0f;
	SecureZeroMemory ( m_wAniSub,sizeof(WORD)*3 );

	m_sPetCrowID = NATIVEID_NULL();

	m_SKILLDELAY.clear();

	m_vecDroppedItems.clear();

	RESET ();

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_CloseWindow );
}

HRESULT GLPetClient::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_sPetID == NATIVEID_NULL() )
	{
		GASSERT ( 0&&"m_sPetID==NATIVEID_NULL()" );
		return E_FAIL;
	}

	SNATIVEID loadPetID = m_sPetID;

	if( IsUsePetSkinPack() )
	{
		loadPetID = m_sPetSkinPackState.petSkinMobID;
	}
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( loadPetID );
	if ( !pCrowData )
	{
		GASSERT ( pCrowData );
		return E_FAIL;
	}

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		pd3dDevice );

	if ( !pSkinChar )
	{
		GASSERT ( pSkinChar );
		return E_FAIL;
	}

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

	

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

	
	m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );

	if ( m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, AN_SUB_NONE ) == 0 )
	{
		m_pSkinChar->DEFAULTANI();
	}

	return S_OK;
}

void GLPetClient::DeleteSkin()
{
	// [shhan][2014.12.03] 지우고 난 후 비어 있는거라도 넣어 놓는다.
	//						m_pSkinChar 는 NULL 이 없다는 가정이 깔려있다.
	SAFE_DELETE ( m_pSkinChar );
	m_pSkinChar = new DxSkinChar;
}

// 맵을 이동하는 것이니 맵 참조값을 줄이는 방향으로 간다.
void GLPetClient::LandShredPtrRefSub()
{
	// 깔끔하게 새로 만들도록 한다.
	m_spWorldLight = std::tr1::shared_ptr<ActorWorldLight>(new ActorWorldLight);
	m_spNameCollision = std::tr1::shared_ptr<ActorNameCollision>(new ActorNameCollision(EMCC_CULL_NONE));
}

HRESULT GLPetClient::UpdateAnimation ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT GLPetClient::UpdateSuit( BOOL bChangeStyle )
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
	if ( bChangeStyle )
	{
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
	}

	// 컬러 갱신 (컬러변경카드 사용중이면 갱신하지 않는다)
	if ( !m_bEnableColor )
        m_pSkinChar->SetHairColor( m_wColor );

	// 악세서리 모양 갱신
	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		SITEM* pSlotItem = GLogicData::GetInstance().GetItem( m_PutOnItems[i].GetNativeID() );
		if ( pSlotItem )
            m_pSkinChar->SetPiece( pSlotItem->GetPetWearingFile(), m_pd3dDevice, 0, 0, EMSCD_ALL_THREAD );
	}

	return S_OK;
}

void GLPetClient::SetMoveState ( BOOL bRun = TRUE )
{
	if ( bRun ) 
	{
		m_actorMove.SetMaxSpeed ( m_fRunSpeed );
		SetSTATE ( EM_PETACT_RUN );
		if ( IsSTATE ( EM_PETACT_MOVE ) )
		{
			if( IsUsePetSkinPack() && m_sPetSkinPackState.bNotFoundRun )
			{
				m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );				
			}else{
				m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
			}
		}
	}
	else
	{
		m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
		ReSetSTATE ( EM_PETACT_RUN );
		if ( IsSTATE ( EM_PETACT_MOVE ) )
		{
			m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		}
	}

	// 이동상태를 변경하는 메시지 발송요망
	GLMSG::SNETPET_REQ_UPDATE_MOVESTATE NetMsg(m_dwActionFlag);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLPetClient::ReSetAllSTATE (void)
{
	BOOL bIsQboxWait(IsSTATE(EM_PETACT_QBOXPICK_WAIT));
	m_dwActionFlag = 0;
	if ( bIsQboxWait )
		SetSTATE(EM_PETACT_QBOXPICK_WAIT);
}

void GLPetClient::SetAttackState( const D3DXVECTOR3& vTargetPos )
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

HRESULT	GLPetClient::UpdatePetState ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3 vOwnerPos, vRandPos, vDist;
	float fDist;

	static float fTimer(0.0f);
	static WORD wSubAni(0);
	static float fIdleTimer(0.0f);

    m_fPickItemTimer += fElapsedTime;
	if( GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_GetQBoxLock ) == false &&
		this->IsActiveSkill(SKILL::EMFOR_PET_GETQBOX) )
	{
		m_fQBoxLockNotifyTimer += fElapsedTime;
	}

	GLLandManClient* pLAND = m_pGaeaClient->GetActiveMap();
	if ( IsSTATE(EM_PETACT_QBOXPICK_WAIT) )
	{
		m_fQboxPickableTimer += fElapsedTime;
		if ( m_fQboxPickableTimer >= GLCONST_CHAR::fQBoxPickableTime )
		{
			m_fQboxPickableTimer = 0.0f;
			ReSetSTATE(EM_PETACT_QBOXPICK_WAIT);
		}
	}


	// 팻이 공격모션 중이면
	if ( IsSTATE ( EM_PETACT_ATTACK ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			ReqGoto ( GetRandomOwnerTarget (), false );
		}
		else
		{
			if( true == m_pSkinChar->IsAni( AN_ATTACK ) )
			{
				m_fAttackTimer += fElapsedTime;
				if ( m_pSkinChar->GETCURANITIME() < m_fAttackTimer )
					ReqStop ( true );
			}
		}
	}

	// 슬퍼할때
	if ( IsSTATE ( EM_PETACT_SAD ) ) return S_OK;

	// 멈춰있을때
	if ( IsSTATE ( EM_PETACT_STOP ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			ReqGoto ( GetRandomOwnerTarget (), false );
		}
		else
		{
			m_vRandPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

			m_fIdleTimer += fElapsedTime;
			if ( m_fIdleTimer > 2.0f )
			{
				ReSetSTATE ( EM_PETACT_STOP );
				SetSTATE ( EM_PETACT_FUNNY );
				MakeAniSubKey ( fTime );

				// Funny Action
				m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub[wSubAni] );

				GLMSG::SNETPET_REQ_FUNNY NetMsg(m_wAniSub[wSubAni]);
    			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

				m_fIdleTimer = 0.0f;
			}
		}

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	//	[shhan][2016.01.19] 현재 Funny Action 은 셋팅시 Sub 값을 AN_SUB_00, AN_SUB_01, AN_SUB_02 가 아닌 
	//						AN_SUB_01, AN_SUB_02, AN_SUB_03 으로 해야 정상 작동합니다.
	//						
	//						3 중 하나가 빠져있고 AN_SUB_00 에 셋팅이 되면 움찔거리는 현상이 발생합니다.
	//						SELECTANI 에서 AN_SUB_00 로 강제 셋팅해서 렌더링 해주고,
	//						다음 Frame 에서 조건문 검사로 인해 Idle 로 셋팅됨.
	//////////////////////////////////////////////////////////////////////////
	// FUNNY 액션중이면
	if ( IsSTATE ( EM_PETACT_FUNNY ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			ReqGoto ( GetRandomOwnerTarget (), false );
			wSubAni = 0;
		}
		else
		{
			const DxSkinAnimation* pANIMCON = m_pSkinChar->GETCURANIM ();

            if ( !pANIMCON )
            {
                //GASSERT( 0 && "HRESULT GLPetClient::UpdatePetState에서 m_pSkinChar->GETCURANIM()이 NULL입니다." );

				// 엉뚱하게 헤메일 때는 Idle 로 셋팅한다.
				m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );

                return E_FAIL;
            }

			// Funny 애니메이션이 끝났으면 Idle 애니메이션 설정
			if ( pANIMCON->m_MainType == AN_GESTURE && pANIMCON->m_SubType == m_wAniSub[wSubAni] )
			{
				if ( m_pSkinChar->ISENDANIM () )
				{
					m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );

					GLMSG::SNETPET_REQ_STOP NetMsg(m_dwGUID, EM_PETACT_STOP, m_vPos, true );
					m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

					if ( ++wSubAni > 2 ) 
					{
						// Funny 애니메이션 순서 재설정
						wSubAni = 0;
						MakeAniSubKey ( fTime );
					}

					fIdleTimer = 0.0f;
				}
			}
			// Idle 애니메이션이 끝나면 Funny 애니메이션 설정
			else if ( pANIMCON->m_MainType == AN_GUARD_N && pANIMCON->m_SubType == AN_SUB_NONE )
			{
				fIdleTimer += fElapsedTime;
				// 애니메이션 설정에 루프가 설정되어 있으므로 끝남 유무를 알수 없으므로 시간으로 제어함.
				if ( fIdleTimer > 3.0f )
				{
					// Funny Action
					m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub[wSubAni] );

					GLMSG::SNETPET_REQ_FUNNY NetMsg(m_wAniSub[wSubAni]);
					m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

					fIdleTimer = 0.0f;
				}
			}
			else	
			{
				// 엉뚱하게 헤메일 때는 Idle 로 셋팅한다.
				m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
			}
		}
	}

/*
	// 주인이 공격/스킬 중일때
	if ( m_pOwner->IsACTION ( GLAT_ATTACK ) || m_pOwner->IsACTION ( GLAT_SKILL ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPosition ();
			vOwnerPos += vRandPos;
			ReqGoto ( vOwnerPos, false );
		}
	}
*/

	// 이동중 주인을 기다릴때
	if ( IsSTATE ( EM_PETACT_WAIT ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();

		if ( m_pOwner->isAction ( GLAT_IDLE ) )
		{
			D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPosition ();
			vOwnerPos = m_pOwner->GetPosition ();
			vOwnerPos += vRandPos;
			ReqGoto ( vOwnerPos, false );
			return S_OK;
		}

		vDist = vOwnerPos - m_vPos;
		float fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			D3DXVECTOR3 vOwnerBack = m_vPos - vOwnerPos;
			D3DXVec3Normalize ( &vOwnerBack, &vOwnerBack );
			vOwnerPos += (vOwnerBack*GLCONST_PET::fOwnerDistance);

			ReqGoto ( vOwnerPos, false );
		}

		m_fIdleTimer = 0.0f;
	}

	if ( IsSTATE ( EM_PETACT_MOVE ) )
	{
		// 아이템을 주우러 달릴때
		if ( IsSTATE ( EM_PETACT_RUN_PICKUP_ITEM ) )
		{
			vDist = m_vPos - m_sSkillTARGET.vPos;
			fDist = D3DXVec3Length(&vDist);
			if ( !m_actorMove.PathIsActive() )
			{
                if ( m_fPickItemTimer > fPET_PICKUP_ITEM_TIME ) 
                {
				    BOOL bPickup = FALSE;
                    m_fPickItemTimer = 0.0f;
					if ( m_sSkillTARGET.emCrow==CROW_ITEM )
					{
						std::tr1::shared_ptr<CItemClientDrop> spItemDrop;
						//  [2/12/2015 gbgim];
						// 예외처리가 많아져 괄호가 많아짐;; 아래같이 처리함;
						if ( pLAND && (spItemDrop = pLAND->GetItem(m_sSkillTARGET.GaeaId)) )
						{
							SITEM* pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
							if ( pItem && pItem->sBasicOp.emItemType==ITEM_QITEM )
							{
								if ( this->IsQBoxPickableTime() )
								{
									SetSTATE(EM_PETACT_QBOXPICK_WAIT);
									bPickup = TRUE;
								}
							}
							else
							{
								bPickup = TRUE;
							}
						}
					}
					else if ( m_sSkillTARGET.emCrow==CROW_MONEY )
					{
						bPickup = TRUE;
					}

					// 아이템을 줍는다.
					if ( bPickup == TRUE )
						m_pOwner->ReqFieldTo ( m_sSkillTARGET, true );  

					ReqStop ();

				    return S_OK;
                }
			}
		}
		else
		{
			vOwnerPos = m_pOwner->GetPosition ();
			vDist = m_vPos - vOwnerPos;
			fDist = D3DXVec3Length(&vDist);

			STARGETID sTarget = m_pOwner->GetActionID ();

			if ( !m_actorMove.PathIsActive() )
			{
				ReqStop ();
				return S_OK;
			}

			// 2초에 한번씩 이동메시지를 전송한다. (이부분은 수정해봐야 할듯하다)
			fTimer += fElapsedTime;
			if ( fTimer > 2.0f && m_pOwner->isAction ( GLAT_MOVE ) )
			{
				// D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPosition ();
				vOwnerPos = m_pOwner->GetPosition ();

				D3DXVECTOR3 vOwnerBack = m_vPos - vOwnerPos;
				D3DXVec3Normalize ( &vOwnerBack, &vOwnerBack );
				vOwnerPos += (vOwnerBack*GLCONST_PET::fOwnerDistance);

				ReqGoto ( vOwnerPos, false );
				fTimer = 0.0f;
			}
			
			// 주인이 걸으면 거리에 따라 뛰거나 걷는다
			// [주의] 위의 코드에서 이미 팻의 움직임 상태를 변경했으므로 
			//		   주인의 걷는 상태에 대한 처리만 해주면 된다.
			if ( !m_pOwner->IsActState ( EM_ACT_RUN ) )
			{
				vDist = m_vPos - vOwnerPos;
				float fDist = D3DXVec3Length(&vDist);

				// 주인과 거리가 너무 멀어지면 일정거리를 유지하기 위해 달린다
				if ( fDist > GLCONST_PET::fRunArea && !IsSTATE ( EM_PETACT_RUN_CLOSELY ) )
				{
					m_actorMove.SetMaxSpeed ( m_fRunSpeed );
					SetSTATE ( EM_PETACT_RUN_CLOSELY );
					SetMoveState ();
				}
				else if ( fDist <= GLCONST_PET::fWalkArea && IsSTATE ( EM_PETACT_RUN_CLOSELY ) )
				{
					m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
					ReSetSTATE ( EM_PETACT_RUN_CLOSELY );
					SetMoveState ( FALSE );
				}
			}
		}
		m_fIdleTimer = 0.0f;
	}

	return S_OK;
}

EM_FIELDITEM_STATE GLPetClient::IsInsertableInven ( STARGETID sTar )
{
	GLLandManClient* pLAND = m_pGaeaClient->GetActiveMap();
	if (!pLAND)
        return EM_FIELDITEM_STATE_GENERALFAIL;

	if ( m_sSkillTARGET.emCrow==CROW_ITEM )
	{
        std::tr1::shared_ptr<CItemClientDrop> spItemDrop = pLAND->GetItem(m_sSkillTARGET.GaeaId);
		if (!spItemDrop)
            return EM_FIELDITEM_STATE_REMOVED;

		SITEM* pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
		if (!pItem)
            return EM_FIELDITEM_STATE_GENERALFAIL;

		BOOL bOk = m_pOwner->IsInsertToInven(spItemDrop);
		if (!bOk)
            return EM_FIELDITEM_STATE_INSERTFAIL;
	}
	else if ( m_sSkillTARGET.emCrow==CROW_MONEY )
	{
        std::tr1::shared_ptr<CMoneyClientDrop> spMoney = pLAND->GetMoney(m_sSkillTARGET.GaeaId);
		if (!spMoney)
            return EM_FIELDITEM_STATE_REMOVED;
	}

	return EM_FIELDITEM_STATE_INSERTOK;
}

void GLPetClient::ProceedSkill ()
{
	PETACTIVESKILLLIST_VEC& vecActiveSkillList = UpdateActiveSkillInfo();
	DWORD				    dwGetItemFlag      = GetSkillGetItemFlag();

	//	스킬 정보 가져옴.
	for ( WORD i=0; i<vecActiveSkillList.size(); ++i )
	{
		SPETACTIVESKILLINFO& sPetActiveSkillInfo = vecActiveSkillList[i];

		SKILL::EMTYPES emType = sPetActiveSkillInfo.emTypes;
		WORD		   wCount = sPetActiveSkillInfo.wCount;  // Note : 중첩 수
		
		switch ( emType )
		{

		// 큇슬롯에 포션을 먹어준다. (한프레임에 한번만 약을 먹어준다!)
		case SKILL::EMFOR_PET_AUTOMEDISON:
			{
//	서버로 프로세서 이동 시켰다.
//	문제 없음이 확인되면 나중에 지워주자~~
/*
				DWORD dwMaxOwnersHP = m_pOwner->GETMAXHP ();
				DWORD dwMaxOwnersMP = m_pOwner->GETMAXMP ();
				DWORD dwMaxOwnersSP = m_pOwner->GETMAXSP ();
				DWORD dwHPVar = dwMaxOwnersHP - m_pOwner->GETHP ();
				int   nMPVar  = dwMaxOwnersMP - m_pOwner->GETMP ();
				int   nSPVar  = dwMaxOwnersSP - m_pOwner->GETSP ();


				// Note : 소환수의 피를 체크하여 가장 적은 소환수의 피를 따라간다
				//
				if ( !m_pOwner->m_dwSummonGUIDList.empty() )
				{
					FLOAT fOwnerHpRatio = (((FLOAT)dwMaxOwnersHP-dwHPVar)/(FLOAT)dwMaxOwnersHP) * 100.0f;

					SUMMONID_ITER SummonIter    = m_pOwner->m_dwSummonGUIDList.begin();
					SUMMONID_ITER SummonIterEnd = m_pOwner->m_dwSummonGUIDList.end();

					for ( ; SummonIter!=SummonIterEnd; ++SummonIter )
					{
						PGLSUMMONCLIENT pSummon = m_pGaeaClient->GetSummon((*SummonIter));

						if ( !pSummon || !pSummon->IsValidBody() )
							continue;

						DWORD dwMaxSummonHP  = pSummon->GETMAXHP();
						DWORD dwSummonHP     = pSummon->GETHP();
						FLOAT fSummonHpRatio = ((FLOAT)dwSummonHP/(FLOAT)dwMaxSummonHP) * 100.0f;

						if ( fSummonHpRatio < fOwnerHpRatio )
						{
							// Note : 주인의 HP를 덮어 씌운다
							dwMaxOwnersHP = dwMaxSummonHP;
							dwHPVar       = (dwMaxSummonHP - dwSummonHP);
						}
					}
				}

				// Q, W, E, A, S, D
				for ( WORD wSLOT = 0; wSLOT < EMACTIONQUICK_SIZE; ++wSLOT )
				{
					const SACTION_SLOT &sACTION = m_pOwner->m_sACTIONQUICK[wSLOT];
					if ( !sACTION.VALID() )	continue;

					switch ( sACTION.wACT )
					{
					case EMACT_SLOT_DRUG:
						{
							SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindItem ( sACTION.sNID );
							if ( !pInvenItem )	continue;

							SITEM* pITEM = GLogicData::GetInstance().GetItem ( sACTION.sNID );
							if ( !pITEM ) continue;

							if ( pITEM->sBasicOp.emItemType!=ITEM_CURE ) continue;

							if ( m_pOwner->IsCoolTime( pInvenItem->sItemCustom.sNativeID ) ) continue;
							
							// HP 관련이면
							switch ( pITEM->sDrugOp.emDrug )
							{
							case ITEM_DRUG_HP:
							case ITEM_DRUG_HP_MP:
							case ITEM_DRUG_HP_MP_SP:
							case ITEM_DRUG_HP_CURE:
							case ITEM_DRUG_HP_MP_SP_CURE:
								{
									if ( pITEM->sDrugOp.bRatio )
									{
										if ( pITEM->sDrugOp.wCureVolume/100.0f*dwMaxOwnersHP <= (float)dwHPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}
									else
									{
										if ( pITEM->sDrugOp.wCureVolume <= dwHPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}

									// HP가 20% 밑으로 떨어지면 무조건 먹는다.
									if ( (float)(dwMaxOwnersHP-dwHPVar)/dwMaxOwnersHP*100.0f < GLCONST_PET::fMaginotLineHP )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
										return;
									}
								}
							};

							// MP 관련이면
							switch ( pITEM->sDrugOp.emDrug )
							{
							case ITEM_DRUG_MP:
							case ITEM_DRUG_HP_MP:
							case ITEM_DRUG_MP_SP:
							case ITEM_DRUG_HP_MP_SP:
							case ITEM_DRUG_HP_MP_SP_CURE:
								{
									if ( pITEM->sDrugOp.bRatio )
									{
										if ( pITEM->sDrugOp.wCureVolume/100.0f*dwMaxOwnersMP <= float(nMPVar) )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}
									else
									{
										if ( pITEM->sDrugOp.wCureVolume <= nMPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}

									// MP가 20% 밑으로 떨어지면
									if ( (float)(dwMaxOwnersMP-nMPVar)/dwMaxOwnersMP*100.0f < GLCONST_PET::fMaginotLineMP )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
										return;
									}
								}
							};

							// SP 관련이면
							switch ( pITEM->sDrugOp.emDrug )
							{
							case ITEM_DRUG_SP:
							case ITEM_DRUG_MP_SP:
							case ITEM_DRUG_HP_MP_SP:
							case ITEM_DRUG_HP_MP_SP_CURE:
								{
									if ( pITEM->sDrugOp.bRatio )
									{
										if ( pITEM->sDrugOp.wCureVolume/100.0f*dwMaxOwnersSP <= (float)nSPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}
									else
									{
										if ( pITEM->sDrugOp.wCureVolume <= nSPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}

									// SP가 20% 밑으로 떨어지면
									if ( (float)(dwMaxOwnersSP-nSPVar)/dwMaxOwnersSP*100.0f < GLCONST_PET::fMaginotLineSP )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
										return;
									}
								}
							};
							
							// 이상치료
							if ( pITEM->sDrugOp.emDrug == ITEM_DRUG_CURE )
							{
								if ( m_pOwner->ISSTATEBLOW() )
								{
									m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
									return;
								}
							}
						}
					};
				} // end of for
			*/
			}
			break;

			// 아이템을 줍는 스킬
		case SKILL::EMFOR_PET_GETALLITEMS:
		case SKILL::EMFOR_PET_GETRAREITEMS:
		case SKILL::EMFOR_PET_GETPOTIONS:
		case SKILL::EMFOR_PET_GETSTONE:
		case SKILL::EMFOR_PET_GETMONEY:
		case SKILL::EMFOR_PET_GETQBOX:
		case SKILL::EMFOR_PET_GETMATERIALS:
			{
			}			
			break;

			// 각각의 스킬별로 주인의 상태를 갱신해준다.
		case SKILL::EMFOR_PET_HEAL:
		case SKILL::EMFOR_PET_SUPPROT:
		case SKILL::EMFOR_PET_BACKUP_ATK:
		case SKILL::EMFOR_PET_BACKUP_DEF:
			{
			}
			break;

		case SKILL::EMFOR_PET_PROTECTITEMS:
			break;

		default: break;
		};
	}

	// Note : 아이템 줍기에 대한 프로세스
	//		  요청 중이 아니거나 요청이 없을때.
    if ( dwGetItemFlag != EMPETSKILL_NONE && m_pOwner->IsValidBody() )
	{
		GLLandManClient* pLAND = m_pGaeaClient->GetActiveMap();
		if ( pLAND == NULL )
			return;

		// 주변에 아이템이 있으면 달려간다.
		if ( !IsSTATE(EM_PETACT_RUN_PICKUP_ITEM) )
		{
			if ( !m_vecDroppedItems.empty() )
			{
				// 하나 꺼내서
				m_sSkillTARGET = m_vecDroppedItems.back();
				m_vecDroppedItems.pop_back ();

				// 아이템을 인벤에 넣을 수 있는지 검사
				EM_FIELDITEM_STATE emResult = IsInsertableInven ( m_sSkillTARGET );

				// 큐박스는 아이템 획득이 아니기때문에 큐박스를 획득하려고한다면;
				// 인벤토리 검사는 무시한다;
				if ( emResult == EM_FIELDITEM_STATE_INSERTFAIL && (dwGetItemFlag & EMPETSKILL_GETQBOX))
				{
					std::tr1::shared_ptr<CItemClientDrop> spItemDrop;
					if ( (spItemDrop = pLAND->GetItem(m_sSkillTARGET.GaeaId)) )
					{
						SITEM* pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
						if ( pItem && pItem->sBasicOp.emItemType == ITEM_QITEM )
						{
							emResult = EM_FIELDITEM_STATE_INSERTOK;
						}
					}
				}

				switch ( emResult )
				{
					// 네트워크 딜레이로 존재하지 않는 아이템을 들기 시도할경우
				case EM_FIELDITEM_STATE_REMOVED:
					break;

					// 인벤토리 공간 부족
				case EM_FIELDITEM_STATE_INSERTFAIL:
					{
                        std::tr1::shared_ptr<CItemClientDrop> spItemDrop = pLAND->GetItem(m_sSkillTARGET.GaeaId);
						if (!spItemDrop)
                            break;

						SITEM* pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
						if (!pItem)
                            break;

						// Note : 해당되는 습득 스킬을 취소한다.
						for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
						{
							PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID[i] );
							if ( !pSkill )	continue;

							SKILL::EMTYPES emSkillType = pSkill->m_sAPPLY.emBASIC_TYPE;

							bool bCancel = false;

							switch ( emSkillType )
							{
							case SKILL::EMFOR_PET_GETALLITEMS:																												   bCancel = true; break;
							case SKILL::EMFOR_PET_GETRAREITEMS: if (pItem->sBasicOp.emLevel    == EMITEM_LEVEL_HIGH )																    bCancel = true; break;
							case SKILL::EMFOR_PET_GETPOTIONS:	if (pItem->sBasicOp.emItemType == ITEM_CURE	|| pItem->sBasicOp.emItemType == ITEM_EFFECT_RELEASE_CURE )			bCancel = true; break;
							case SKILL::EMFOR_PET_GETSTONE:		if (pItem->sBasicOp.emItemType == ITEM_GRINDING || pItem->sBasicOp.emItemType == ITEM_MATERIALS )	bCancel = true; break;
							case SKILL::EMFOR_PET_GETQBOX:		if (pItem->sBasicOp.emItemType == ITEM_QITEM )												bCancel = true; break;
							case SKILL::EMFOR_PET_GETMATERIALS:	if (pItem->sBasicOp.emItemType == ITEM_MATERIALS )												bCancel = true; break;
							}
	
							if ( bCancel )
							{
								// Note : 스킬을 취소해버린다.
								ReqChangeActiveSkill ( NATIVEID_NULL(), i );

								m_sSkillTARGET.RESET ();
								m_vecDroppedItems.clear();
								m_pGaeaClient->PrintConsoleTextDlg( ID2GAMEINTEXT("EMPET_NOTENOUGHINVEN") );
							}
						}
					}
					break;;
					// 인벤토리 삽입 가능
				case EM_FIELDITEM_STATE_INSERTOK:
					{
						D3DXVECTOR3 vDist, vDir, vTar;
						vDist = m_sSkillTARGET.vPos - m_vPos;
						D3DXVec3Normalize ( &vDir, &vDist );
						float fDist = D3DXVec3Length ( &vDist ) - 10.0f; // 줍기 가능한 거리만큼 거리 설정
						vTar = vDir * fDist;
						vTar = m_vPos + vTar;
						ReqGoto ( vTar, true );
					}
					break;
					// 일반 오류일 경우
				case EM_FIELDITEM_STATE_GENERALFAIL:
					{
					}
					break;
				}
			}
			// 땅에 떨어진 아이템 정보를 요청한다.
			else 
			{
				ReqGetItem ( dwGetItemFlag ); 
			}
		}
	}
	else
	{
		if ( !m_vecDroppedItems.empty() )
		{
			m_vecDroppedItems.clear();
		}
	}
}


bool GLPetClient::CheckSkill ( SNATIVEID sSkillID, WORD wSlot )
{
	PETSKILL_MAP_CITER learniter = m_ExpSkills.find ( sSkillID.dwID );
	if ( learniter==m_ExpSkills.end() )
	{
		// 배우지 않은 스킬
		return false;
	}

	PETDELAY_MAP_ITER delayiter = m_SKILLDELAY.find ( sSkillID.dwID );
	if ( delayiter!=m_SKILLDELAY.end() )
	{
		// 스킬딜레이
		return false;
	}

	if ( wSlot >= m_wMaxSkillSlot || wSlot >= MAX_PETSKILLSLOT )
	{
		// 잘못된 스킬 슬롯
		return false;
	}

	return true;
}

float GLPetClient::GetSkillDelayPercent ( SNATIVEID sSkillID )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
	if ( !pSkill )					     return 0.0f;

	PETDELAY_MAP_ITER delayiter = m_SKILLDELAY.find ( sSkillID.dwID );
	if ( delayiter==m_SKILLDELAY.end() ) return 0.0f;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[0];
	float fSkillDelay = sSKILL_DATA.fDELAYTIME;
	float fCurDelay = (*delayiter).second;

	if ( fSkillDelay == 0.0f ) fSkillDelay = 1.0f;

	return fCurDelay / fSkillDelay;
}

void GLPetClient::AccountSkill ( SNATIVEID sSkillID )
{
	PETSKILL_MAP_ITER iter = m_ExpSkills.find ( sSkillID.dwID );
	if ( iter == m_ExpSkills.end() ) return;
	PETSKILL sPetSkill = (*iter).second;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
	if ( !pSkill )					 return;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sPetSkill.wLevel];

	m_SKILLDELAY.insert ( std::make_pair(sSkillID.dwID, sSKILL_DATA.fDELAYTIME) );
}

void GLPetClient::UpdateSkillDelay ( float fElapsedTime )
{
	PETDELAY_MAP_ITER iter_del;

	PETDELAY_MAP_ITER iter = m_SKILLDELAY.begin ();
	PETDELAY_MAP_ITER iter_end = m_SKILLDELAY.end ();

	for ( ; iter!=iter_end; )
	{
		float &fDelay = (*iter).second;
		iter_del = iter++;

		fDelay -= fElapsedTime;
		if ( fDelay < 0.0f )	m_SKILLDELAY.erase ( iter_del );
	}
}

HRESULT GLPetClient::FrameMove( GLLandManClient* pLandManClient, float fTime, float fElapsedTime )
{
	if ( !m_bValid ) return S_OK;
	
	HRESULT hr(S_OK);

	UpdatePetState ( fTime, fElapsedTime );
	ProceedSkill ();
	// UpdateSkillDelay ( fElapsedTime ); 기획팀 김병욱씨 요청으로 제거 [06.10.09]

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
	//
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;


	//////////////////////////////////////////////////////////////////////////
	//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
	if ( pLandManClient )
	{
		// Thread 상으로 넘긴다.
		m_spNameCollision->InitData( pLandManClient->GetLandManSp() );
		NSSkinAniThread::AddNameCollision( m_spNameCollision, DxViewPort::GetInstance().GetFromPt(), m_vPos + D3DXVECTOR3(0.0f, 8.0f, 0.0f) );



		//////////////////////////////////////////////////////////////////////////
		//			ActorWorldLight 을 Thread 상에서 확인하도록 유도한다.
		// Thread 상으로 넘긴다.
		m_spWorldLight->InitData( pLandManClient->GetLandManSp() );
		NSSkinAniThread::AddActorWorldLight( m_spWorldLight, m_vPos );
	}


	return S_OK;
}

HRESULT GLPetClient::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )
        return S_OK;

    if ( !pd3dDevice )
        return E_FAIL;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	

	// 펫은 피스 순서를 거꾸로 그린다.
	// 케릭터가 은신 중이면 펫도 반투명하게 렌더;
	if ( m_pSkinChar )
	{
		D3DXMATRIX matScale, matRender;
		if( IsUsePetSkinPack() )
		{				
			D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
			matRender = matScale * m_matTrans;			
		}
		else
			matRender = m_matTrans;

		//m_pSkinChar->Render( pd3dDevice, matRender, FALSE, TRUE, TRUE, FALSE, m_pOwner->m_bINVISIBLE );

		bool bHALF_VISIBLE = m_pOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // 은신 중인가;

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

		//if ( m_pOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) )
		//{
		//	DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &m_vDir );
		//}
		//else
		//{
		//	DxEffcharDataMan::GetInstance().OutEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
		//}

		
		//m_pSkinChar->Render( pd3dDevice, TRUE, 1.f, matRender, FALSE, TRUE, TRUE, FALSE);

		RenderMyChar( pd3dDevice, matRender, TRUE, bHALF_VISIBLE );

		DxLandMan* pLandMan(NULL);
		if ( m_pGaeaClient )
		{
			GLLandManClient* pLandManClient = m_pGaeaClient->GetLandManClient();
			if ( pLandManClient )
			{
				pLandMan = pLandManClient->GetLandMan();
			}
		}

		//if ( pLandMan )
		//{
		//	m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(matRender._41,matRender._42,matRender._43) );
		//}
	}
	
	return S_OK;
}

HRESULT GLPetClient::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )
		return S_OK;

	if ( !pd3dDevice )
		return E_FAIL;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;



	// 펫은 피스 순서를 거꾸로 그린다.
	// 케릭터가 은신 중이면 펫도 반투명하게 렌더;
	if ( m_pSkinChar )
	{
		D3DXMATRIX matScale, matRender;
		if( IsUsePetSkinPack() )
		{				
			D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
			matRender = matScale * m_matTrans;			
		}
		else
			matRender = m_matTrans;

		//m_pSkinChar->Render( pd3dDevice, matRender, FALSE, TRUE, TRUE, FALSE, m_pOwner->m_bINVISIBLE );

		bool bHALF_VISIBLE = m_pOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // 은신 중인가;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		RenderMyChar( pd3dDevice, matRender, FALSE, bHALF_VISIBLE );
	}

	return S_OK;
}

HRESULT GLPetClient::RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		return S_OK;

	if ( !m_bValid )
		return S_OK;

	if ( !pd3dDevice )
		return E_FAIL;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	NSMaterialSkinManager::SetMob( FALSE );

	if ( m_pSkinChar )
	{
		bool bHALF_VISIBLE = m_pOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // 은신 중인가;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		if ( bHALF_VISIBLE )	// 캐릭터 반투명 렌더
		{
			m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 0.5f );
		}
		else		// 정상렌더
		{
			m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 1.f );
		}
	}

	return S_OK;
}

void GLPetClient::RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, BOOL bOpaqueState, BOOL bHALF_VISIBLE )
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
				m_pSkinChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY ? TRUE : FALSE );
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
				m_pSkinChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY ? TRUE : FALSE );
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

HRESULT GLPetClient::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD )
{
	if ( !m_bValid )
        return S_OK;

    if ( !pd3dDevice )
        return E_FAIL;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	//	Note : 그림자 랜더링.
	//
	if ( m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice, bWorldSystemOLD );
	}

	return S_OK;
}

HRESULT GLPetClient::RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	if ( !m_bValid )
		return S_OK;

	if ( !pd3dDevice )
		return E_FAIL;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	//	Note : 그림자 랜더링.
	//
	if ( m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight ( m_pSkinChar, m_matTrans, pd3dDevice, bUsedMaterialSystem, dwSlot );
	}

	return S_OK;
}

HRESULT GLPetClient::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, float fWaterHeight )
{
	if ( !m_bValid )
        return S_OK;

    if ( !pd3dDevice )
        return E_FAIL;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	//	Note : 반사 랜더링.
	//
	if ( m_pSkinChar )
	{
		DxEnvironment::GetInstance().RenderRefelctChar( m_pSkinChar, pd3dDevice, fWaterHeight );
	}

	return S_OK;
}

HRESULT GLPetClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}
	
	return S_OK;
}

HRESULT GLPetClient::InvalidateDeviceObjects ()
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}
	
	return S_OK;
}

// Device 와 관련 있는 것들만 삭제를 하기 바람.
HRESULT GLPetClient::DeleteDeviceObjects ()
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->DeleteDeviceObjects ();
		//SAFE_DELETE( m_pSkinChar );
	}

	Delete();

	return S_OK;
}

void GLPetClient::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
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

//----------------------------------------------------------------------------------------------------------------------------------
//								H	a	i	r				S	t	y	l	e	 &&	 C	o	l	o	r
//----------------------------------------------------------------------------------------------------------------------------------
void GLPetClient::StyleInitData()
{
	m_bEnableSytle = TRUE;
	
	m_wStyleTEMP = m_wStyle;

	// 컬러 변화 중일 경우 건들지 않는다.
	if( !m_bEnableColor )
	{
		m_wColorTEMP = m_wColor;
	}
}

void GLPetClient::ColorInitData()
{
	m_bEnableColor = TRUE;

	m_wColorTEMP = m_wColor;

	// 스타일 변화 중일 경우 건들지 않는다.
	if( !m_bEnableSytle )
	{
		m_wStyleTEMP = m_wStyle;
	}
}

void GLPetClient::StyleChange( WORD wStyle )
{
	if( 0 > m_emTYPE )
		return;

	m_wStyleTEMP = wStyle; 

	if ( m_wColor == GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[m_wStyle] && !m_bEnableColor )
	{
		m_wColorTEMP = GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[m_wStyleTEMP];
	}

	StyleUpdate();
	ColorUpdate();
}

void GLPetClient::ColorChange( WORD wColor )
{ 
	m_wColorTEMP = wColor; 

	ColorUpdate();
}

void GLPetClient::StyleUpdate()
{
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice );

	if ( !pSkinChar )					  return;

	const PETSTYLE &sSTYLE = GLCONST_PET::GetPetStyle ( m_emTYPE );

	// 헤어스타일.
	if ( sSTYLE.wSTYLENum > m_wStyleTEMP )
	{
		std::string strHAIR_CPS = sSTYLE.strSTYLE_CPS[m_wStyleTEMP];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	현재 장착 스킨.

		if( pCharPart && strcmp( strHAIR_CPS.c_str(), pCharPart->GetFileName() ) )
		{
			m_pSkinChar->SetPiece( strHAIR_CPS.c_str(), m_pd3dDevice, NULL, NULL, EMSCD_ALL_THREAD );
		}
	}

	// Note : 이것을 호출하면서.. 머리형이 원상태로 바뀌어 버린다.
	//		그래서 FALSE 호출로 머리모양을 안바뀌게 한다.
	//UpdateSuit( FALSE );
}

void GLPetClient::ColorUpdate()
{
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice );

	if ( !pSkinChar )					  return;

	m_pSkinChar->SetHairColor( m_wColorTEMP );
}

void GLPetClient::MakeAniSubKey ( float fTime )
{
	static bool bPlus(true);

	//srand ( unsigned int(fTime) );
	m_wAniSub[0] = rand()%3+1;
	m_wAniSub[1] = rand()%3+1;
	if ( m_wAniSub[0] == m_wAniSub[1] )
	{
		if ( bPlus )
		{
			m_wAniSub[1] += 1;
			if ( m_wAniSub[1] > 3 ) m_wAniSub[1] = 1;
		}
		else
		{
			m_wAniSub[1] -= 1;
			if ( m_wAniSub[1] < 1 ) m_wAniSub[1] = 3;
		}
		bPlus = !bPlus;
	}
	for ( int i = 1; i < 4; ++i )
	{
		if ( m_wAniSub[0] != i && m_wAniSub[1] != i )
		{
			m_wAniSub[2] = i;
			break;
		}
	}
}

// Note : 현재 활성화된 스킬을 정리한다
PETACTIVESKILLLIST_VEC& GLPetClient::UpdateActiveSkillInfo ()
{
	// Note : 초기화
	m_vecActiveSkillInfo.clear();
	//m_vecActiveSkillInfo._Mylast = m_vecActiveSkillInfo._Myfirst;

	for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID[i] );
		if ( !pSkill )	continue;

		SPETACTIVESKILLINFO sInfo;
		sInfo.emTypes = pSkill->m_sAPPLY.emBASIC_TYPE;
		sInfo.wLevel  = 0;

		PETACTIVESKILLLIST_VEC_ITER iterFind = std::find(m_vecActiveSkillInfo.begin(), m_vecActiveSkillInfo.end(), sInfo);

		if ( iterFind != m_vecActiveSkillInfo.end() )
		{
			(*iterFind).wCount++;
		}
		else
		{
			sInfo.wCount = 0;
			m_vecActiveSkillInfo.push_back( sInfo );
		}
	}

	return m_vecActiveSkillInfo;
}

D3DXVECTOR3 GLPetClient::GetRandomOwnerTarget ()
{
	D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPosition ();
	m_vRandPos = vRandPos;

    //  이동중에 Shock상태에 빠지면 주인의 이동이 멈춘다.
    //  이때 주인의 위치와 타겟의 위치가 틀려지면서
    //  펫이 계속 타겟주변의 위치로 이동할려고 하면서 메세지를 계속 보내게된다.
    
    D3DXVECTOR3 vPos;
    if ( m_pOwner->isAction( GLAT_MOVE ) )
    {
        STARGETID sTargetID = m_pOwner->GetActionID ();
        vPos = sTargetID.vPos;
	    vPos += vRandPos;
    }
    else
    {
        vPos = m_pOwner->GetPosition ();
        vPos += vRandPos;
    }

	return vPos;

}

BOOL GLPetClient::IsVisibleDisplayName() const
{
	if ( m_spNameCollision->GetCollsion() )
		return FALSE;

	return TRUE;
}

bool GLPetClient::IsActiveSkill(SKILL::EMTYPES emSkillType)
{
	const size_t dwSize(m_vecActiveSkillInfo.size());
	for ( size_t i = 0; i < dwSize; ++i )
	{
		if ( m_vecActiveSkillInfo[i].emTypes == emSkillType )
			return true;
	}
	return false;
}

bool GLPetClient::IsQBoxLockMessageNotifyTime(void)
{
	if( false == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_GetQBoxLock ) && 
		fPET_GETQBOX_DISABLE_MSG < m_fQBoxLockNotifyTimer)
	{
		m_fQBoxLockNotifyTimer = 0.0f;
		return true;
	}
	return false;
}

bool GLPetClient::IsQBoxPickableTime()
{
	return !IsSTATE(EM_PETACT_QBOXPICK_WAIT);
}

int GLPetClient::GetType() 
{
	return (int) m_emTYPE;
}

std::string GLPetClient::GetTypeString()
{
	if( -1 < m_emTYPE && m_emTYPE < COMMENT::PET_TYPE.size() )
		return COMMENT::PET_TYPE[ m_emTYPE ].c_str();
	else 
		return "";
}

const WORD  GLPetClient::GetMaxSkillSlot() const
{
	return m_wMaxSkillSlot;
}

lua_tinker::table GLPetClient::GetPetCardID()
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	lua_tinker::table tbItem( GLWidgetScript::GetInstance().GetLuaState() );
	if( pChar->GetGaeaID() == m_dwOwner )
	{
		tbItem.set("dwID", pChar->m_sPetCardNativeID.dwID );
		tbItem.set("wMID", pChar->m_sPetCardNativeID.wMainID );
		tbItem.set("wSID", pChar->m_sPetCardNativeID.wSubID );
	}

	return tbItem;
}

lua_tinker::table GLPetClient::GetPetItemList()
{
	lua_tinker::table tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	int nDisplayCnt = 1; //Lua
	for( int i = 0; i < ACCETYPESIZE; ++i )
	{
		lua_tinker::table tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		
		tbItem.set("dwID", m_PutOnItems[i].GetNativeID().dwID );
		tbItem.set("wMID", m_PutOnItems[i].GetNativeID().wMainID );
		tbItem.set("wSID", m_PutOnItems[i].GetNativeID().wSubID );

		tbItemList.set( nDisplayCnt++, tbItem );

	}

	return tbItemList;
}

lua_tinker::table GLPetClient::GetPetSkillList( int nIndex )
{
	lua_tinker::table tbSkill( GLWidgetScript::GetInstance().GetLuaState() );

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( EMSKILL_PET, nIndex );
	if ( pSkill )
	{
		// 기본 정보 -----------------------------------------------------------
		tbSkill.set( "dwID", pSkill->GetId().dwID );			// ID
		tbSkill.set( "wMID", pSkill->GetId().wMainID );			// wMID
		tbSkill.set( "wSID", pSkill->GetId().wSubID );			// wSID
		tbSkill.set( "strName", pSkill->GetName() );			// 스킬 이름
		tbSkill.set( "dwGrade", pSkill->m_sBASIC.dwGRADE );		// 등급
		tbSkill.set( "bPassive", (pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ? 1:0) ); //패시브 스킬 유무 확인

		//	Note : 익힌 스킬인지 검사.
		PETSKILL_MAP_ITER learniter = m_ExpSkills.find ( pSkill->GetId().dwID );
		if ( learniter==m_ExpSkills.end() )
			tbSkill.set( "bLearn", FALSE );	
		else
			tbSkill.set( "bLearn", TRUE );		

		//스킬 활성화 확인
		//활성화 중이라면 사용중인 슬롯 번호를 반환
		int nActivatedIndex( 0 );
		for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
		{
			if ( m_sActiveSkillID[i] == pSkill->GetId() )
				nActivatedIndex = i + 1;	
		}
		tbSkill.set( "nActivated", nActivatedIndex );	
		
	}

	return tbSkill;
}

lua_tinker::table GLPetClient::GetPetSkillListAll()
{
	int nDisplayCnt = 1; //Lua
	lua_tinker::table tbResult( GLWidgetScript::GetInstance().GetLuaState() );
	
	for( int i = 0; i < MAX_PETSKILL; ++i )
	{
		lua_tinker::table tbSkill( GLWidgetScript::GetInstance().GetLuaState() );
		
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( EMSKILL_PET, i );
		if ( pSkill )
		{
			// 기본 정보 -----------------------------------------------------------
			tbSkill.set( "dwID", pSkill->GetId().dwID );			// ID
			tbSkill.set( "wMID", pSkill->GetId().wMainID );			// wMID
			tbSkill.set( "wSID", pSkill->GetId().wSubID );			// wSID
			tbSkill.set( "strName", pSkill->GetName() );			// 스킬 이름
			tbSkill.set( "dwGrade", pSkill->m_sBASIC.dwGRADE );		// 등급
			tbSkill.set( "bPassive", (pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ? 1:0) ); //패시브 스킬 유무 확인

			//	Note : 익힌 스킬인지 검사.
			PETSKILL_MAP_ITER learniter = m_ExpSkills.find ( pSkill->GetId().dwID );
			if ( learniter==m_ExpSkills.end() )
				tbSkill.set( "bLearn", FALSE );	
			else
				tbSkill.set( "bLearn", TRUE );		

			//스킬 활성화 확인
			//활성화 중이라면 사용중인 슬롯 번호를 반환
			int nActivatedIndex( 0 );
			for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
			{
				if ( m_sActiveSkillID[i] == pSkill->GetId() )
					nActivatedIndex = i + 1;	
			}
			tbSkill.set( "nActivated", nActivatedIndex );	
			
			//결과 입력
			tbResult.set( nDisplayCnt++, tbSkill );
		}
	}

	return tbResult;
}


void GLPetClient::SetActivatedSkill( lua_tinker::table tbActivatedSkill )
{
	for( int i = 1; i <= tbActivatedSkill.TableLen(); ++i )
	{
		if( i > m_wMaxSkillSlot ) continue; //듀얼스킬 카드를 사용하였으면 2 아니면 1

		lua_tinker::table tbSkill = tbActivatedSkill.get< lua_tinker::table >( i );

		SNATIVEID sID;

		if( tbSkill.get< DWORD >( "dwID" ) == -1 )
			sID = NATIVEID_NULL() ;
		else
			sID = tbSkill.get< DWORD >( "dwID" );
		
		if( m_sActiveSkillID[i - 1] != sID )
		{
			ReqChangeActiveSkill(sID, i - 1);
		}	
	}
}

WORD GLPetClient::ColorDWORDtoWORD( DWORD dwColor )
{
	WORD wR, wG, wB;
	wR = (WORD)((dwColor&0xff0000)>>16);
	wG = (WORD)((dwColor&0xff00)>>8);
	wB = (WORD)(dwColor&0xff);

	// 압축시
	wR = wR >> 3;
	wG = wG >> 3;
	wB = wB >> 3;
	return (wR<<10) + (wG<<5) + wB;
}

DWORD GLPetClient::GetColor()
{
	int nR = int( float( (m_wColor & 0x7c00) >> 10 ) * 255.0f / 31.0f );
	int nG = int( float( (m_wColor & 0x3e0) >> 5 ) * 255.0f / 31.0f );
	int nB = int( float( (m_wColor & 0x1f) ) * 255.0f / 31.0f );

	return ( 0xFF000000 | (nR << 16) | (nG << 8) | nB );
}

void GLPetClient::SetColor( DWORD dwColor )
{
	ColorChange( ColorDWORDtoWORD( dwColor ) );
}

void GLPetClient::CommitColor( DWORD dwColor )
{
	if( NULL == m_pOwner )
		return;

	ReqChangeColor( ColorDWORDtoWORD( dwColor ) );
}
