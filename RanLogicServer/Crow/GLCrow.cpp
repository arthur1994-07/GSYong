#include "../pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/TransformSet/GLTransformSet.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

#include "../TriggerSystem/ServerDieCrowMessage.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"

#include "../Summon/CSummonCompField.h"
#include "../Mob/CMobCompField.h"

#include "../FieldServer/GLGaeaServer.h"
#include "./GLCrow.h"

#include "../AI/CAICompField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define BOX_R	(4)

GLCrow::GLCrow(GLGaeaServer* pGaeaServer)
: ServerActor(pGaeaServer, CROW_MOB)
, m_nSkillReActionError( 0 )
, m_pd3dDevice(NULL)
, m_pSkinChar(NULL)

, m_vMaxOrg(BOX_R,20,BOX_R)
, m_vMinOrg(-BOX_R,0,-BOX_R)

, m_vMax(BOX_R,20,BOX_R)
, m_vMin(-BOX_R,0,-BOX_R)

, m_wTARNUM(0)
, m_vTARPOS(0,0,0)

, m_pQuadNode(NULL)
, m_pCellNode(NULL)

, m_fAge(0.0f)
, m_dwCFlag(NULL)
, m_vGenPos(0,0,0)
, m_vOrgTarPos(0,0,0)

, m_fScale( 1.0f )

, m_vStartPos(0,0,0)

, m_dwAttPosSlot(GLARoundSlot::SLOT_SIZE)

, m_emReAction(EMREACT_NONE)
, m_pAttackProp(0)

, m_fattTIMER(0.0f)

, m_sAssault(CROW_PC,GAEAID_NULL)

, m_fIdleTime(0.0f)
, m_fStayTimer(0.0f)

, m_fTargetTimer(0.0f)

, m_pCurAct(NULL)
, m_pMobSchedule(NULL)

, m_dwPC_AMOUNT(0)
, m_dwSUMMON_AMOUNT(0)
, m_bNpcRecall(false)
, m_dwCallCharID(0)
, m_dwCallUserID(0)
, m_tGenTime(0)
, m_lnCommission(0)

, m_fMultipleAttack(1.f)
, m_fMultipleDefense(1.f)
, m_fMultipleExp(1.f)
, m_fMultipleDrop(1.f)
, m_dwKillerDbNum(0)

, m_bUseReward(false)

, m_bReturnPatrolPos(false)
, m_bIsPatrolStop(false)
, m_vPatrolStopPos(0,0,0)

, m_nOffence ( 0 )
, m_nDefence ( 0 )
, m_nFireRegist ( 0 )
, m_nIceRegist ( 0 )
, m_nElectricRegist ( 0 )
, m_nPoisonRegist ( 0 )
, m_nSpiritRegist ( 0 )
, m_nAccuracyRate ( 0 )
, m_nAvoidanceRate ( 0 )
{	
	// AI 생성;
	/*AISystem::CAICompField* pAI = new AISystem::CAICompField( this );
	if ( pAI )
		InsertComponent( pAI );*/

}

GLCrow::~GLCrow(void)
{
	//SAFE_DELETE(m_pTransformSet);
	SAFE_DELETE(m_pSkinChar);
}

void GLCrow::RESET_DATA ()
{
	int i=0;

	GLCROWLOGIC::RESET_DATA();
	ServerActor::Reset();

	m_pd3dDevice = NULL;	

	SAFE_DELETE(m_pSkinChar);
	
	D3DXMatrixIdentity ( &m_matTrans );

	m_vMaxOrg = D3DXVECTOR3(0,0,0);
	m_vMinOrg = D3DXVECTOR3(0,0,0);

	m_vMax = D3DXVECTOR3(0,0,0);
	m_vMin = D3DXVECTOR3(0,0,0);

	m_dwGlobalID = GAEAID_NULL;

	m_mapID.reset();
	m_dwCeID = 0;
	
	m_pQuadNode = NULL;
	m_pCellNode = NULL;

	m_fAge = 0.0f;
	m_dwCFlag = NULL;
	m_vGenPos = D3DXVECTOR3(0,0,0);
	m_vOrgTarPos = D3DXVECTOR3(0,0,0);
	ServerActor::Reset();

	m_fScale = 1.0f;

	m_vStartPos = D3DXVECTOR3(0,0,0);

	m_dwAttPosSlot = GLARoundSlot::SLOT_SIZE;
	m_TargetID = STARGETID(CROW_PC,GAEAID_NULL);

	m_emReAction = EMREACT_NONE;
	m_pAttackProp = NULL;
	m_fattTIMER = 0.0f;
	m_sHITARRAY.clear();

	m_wTARNUM = 0;
	
	for ( i=0; i<EMTARGET_NET; ++i )
		m_sTARIDS[i] = STARID(CROW_PC,GAEAID_NULL);
	m_vTARPOS = D3DXVECTOR3(0,0,0);

	m_sAssault = STARGETID(CROW_PC,GAEAID_NULL);

	m_cDamageLog.clear();
	m_cDamageLogParty.clear();

	m_fIdleTime = 0.0f;
	m_fStayTimer = 0.0f;

	m_fTargetTimer = 0.0f;

	m_pCurAct = NULL;
	m_pMobSchedule = NULL;

	m_nSkillReActionError = 0;

	m_bNpcRecall = false;
	m_dwCallCharID = 0;
	m_dwCallUserID = 0;
	m_tGenTime = 0;
	m_lnCommission = 0;
    
    m_fMultipleAttack = 1.f;
    m_fMultipleDefense = 1.f;
    m_fMultipleExp = 1.f;
	m_fMultipleDrop = 1.f;
    m_dwKillerDbNum = 0;
	m_bReturnPatrolPos = false;
	m_bIsPatrolStop = false;
	m_vPatrolStopPos = D3DXVECTOR3(0,0,0);

	ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );
	if ( !pCrowComp )
	{
		sc::writeLogError( "GLCrow::RESET_DATA - pCrowComp is NULL." );
		return;
	}

	pCrowComp->Reset();
}

HRESULT GLCrow::CreateMob(
    GLMobSchedule* pMobSchedule,
    GLLandMan* pLandMan,
    SNATIVEID sCROWID,
    LPDIRECT3DDEVICEQ pd3dDevice/* =NULL */, 
    LPD3DXVECTOR3 pPos/* =NULL */,    
    bool bFirstCreateForInstMap /* = FALSE */,
    FLOAT fScale /* = 1.0f */)
{
	GASSERT(pLandMan);
	m_pd3dDevice = pd3dDevice;

	// 몹의 설정 정보를 가져옴.
	m_pCrowData = GLCrowDataMan::GetInstance().GetCrowData(sCROWID);
	if (!m_pCrowData)
	{
        sc::writeLogError(
            sc::string::format(
                "CreateCrow GetCrowData. CrowID %1%/%2%",
                sCROWID.wMainID,
                sCROWID.wSubID));
		return E_FAIL;
	}

	// Summon Data 세팅;
	MobSystem::CMobCompField* pMobComp =
		new MobSystem::CMobCompField( this );
	if ( !pMobComp )
	{
		sc::writeLogError( "GLCrow::CreateCrow - pMobComp is NULL." );
		return E_FAIL;
	}

	InsertComponent( pMobComp );

	pMobComp->SetCrowData( *m_pCrowData );
	pMobComp->Initialize();
	
	// 몹 로직 데이터 초기화.
	GLCROWLOGIC::INIT_DATA();    

	// 스케쥴러 초기화.
	m_pMobSchedule = pMobSchedule;
	
	// 위치 초기화.
	if (pPos)
		m_vPosition = *pPos;
	else if (m_pMobSchedule)
		m_vPosition = m_pMobSchedule->m_pAffineParts->vTrans;

	m_vGenPos = m_vPosition;
	m_vOrgTarPos = m_vPosition;

    m_fScale = fScale * (m_pMobSchedule != 0 ? m_pMobSchedule->m_fScale : 1.f);

	//	방향 초기화.
	D3DXMATRIX matY, matScale;
	D3DXMatrixIdentity ( &matY );
	D3DXMatrixIdentity ( &matScale );

	if ( m_pMobSchedule )
		D3DXMatrixRotationY ( &matY, m_pMobSchedule->m_pAffineParts->vRotate.x );

	D3DXVec3TransformCoord ( &m_vDirect, &vDIRORIG, &matY );

	if ( m_pMobSchedule )
		m_pCurAct = m_pMobSchedule->m_sMobActList.m_pHead;
	
	if( bFirstCreateForInstMap )
	{
		CreateActorMove(pLandMan->GetNavi(), m_vPosition, true);
		
		if ( GetPositionActor() == D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
		{
			PGLCROW    pCrow = pLandMan->GetCrow(m_dwGaeaID);

			if( pCrow )
				CreateActorMove(pLandMan->GetNavi(),  pCrow->GetPositionActor(), true,  pCrow->GetCurrentCellID());
            else
				return UINT_MAX;
		}
	}
    else
		CreateActorMove( pLandMan->GetNavi(), m_vPosition);

	// 주변 케릭터 인식.
	//
	UpdateViewAround ();

	if ( m_pd3dDevice )
	{
		DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pCrowData->GetSkinObjFile(), m_pd3dDevice );
		if ( !pCharData )
            return E_FAIL;

		SAFE_DELETE(m_pSkinChar);

		m_pSkinChar = new DxSkinChar;
		m_pSkinChar->SetCharData(pCharData, pd3dDevice, TRUE, EMSCD_ALL_THREAD);
        m_pSkinChar->SetInteractiveScale(m_pCrowData->m_sBasic.m_fScale * m_fScale);
        m_pSkinChar->GetAABBBox(m_vMaxOrg, m_vMinOrg);
	}

	return S_OK;
}

/*HRESULT GLCrow::CreateSummon ( LPDIRECT3DDEVICEQ pd3dDevice
							 , GLLandMan* pLandMan
							 , PGLSUMMON pSummonData
							 , ServerActor* pOwner
							 , D3DXVECTOR3& vPosition
							 , const STARGETID& sTarget )
{
	if (!pSummonData || !pLandMan || !pOwner)
		return E_FAIL;

	// 디바이스 세팅;
	m_pd3dDevice = pd3dDevice;

	m_pCrowData = GLCrowDataMan::GetInstance().GetCrowData(
		pSummonData->m_sSummonID );
	if( !m_pCrowData )
	{
		sc::writeLogError(
			sc::string::format(
			"GLCrow::CreateSummon. CrowID %1%/%2%",
			pSummonData->m_sSummonID.wMainID,
			pSummonData->m_sSummonID.wSubID));
		return E_FAIL;
	}

	// Summon Data 세팅;
	SummonSystem::CSummonCompField* pSummonComp =
		new SummonSystem::CSummonCompField( this );
	if ( !pSummonComp )
	{
		sc::writeLogError( "GLCrow::CreateSummon - pSummonComp is NULL." );
		return E_FAIL;
	}

	InsertComponent( pSummonComp );

	pSummonComp->SetCrowData( *m_pCrowData );
	pSummonComp->SetSummonData( *pSummonData );
	pSummonComp->SetSummoner( pOwner->GetActorID() );
	pSummonComp->Initialize();

	// CROW 로직 데이터 초기화;
	GLCROWLOGIC::INIT_DATA();

	// 위치;
	m_vPosition = vPosition;

	// 방향;
	D3DXVECTOR3 vOwnerPos = pOwner->GetPosition();
	m_vDirect = m_vPosition - vOwnerPos;
	D3DXVec3Normalize ( &m_vDirect, &m_vDirect );

	// 타겟ID
	m_TargetID.RESET();
	m_TargetID = sTarget;

	// 맵;
	m_mapID = pOwner->GetCurrentMap();

	// ActorMove;
	CreateActorMove( pLandMan->GetNavi(), m_vPosition );
	StopActorMove();

	// Hit Array 초기화;
	m_sHITARRAY.clear();

	m_fattTIMER = 0.0f;
	m_fIdleTime = 0.0f;

	// 주변 케릭터 인식;
	UpdateViewAround();
	if ( m_pd3dDevice )
	{
		DxSkinCharData* pCharData =
			DxSkinCharDataContainer::GetInstance().LoadData(
			m_pCrowData->GetSkinObjFile(), m_pd3dDevice );
		if ( !pCharData )
			return E_FAIL;

		SAFE_DELETE( m_pSkinChar );

		m_pSkinChar = new DxSkinChar;
		m_pSkinChar->SetCharData(
			pCharData, pd3dDevice, TRUE, EMSCD_ALL_THREAD );
		m_pSkinChar->SetInteractiveScale(
			m_pCrowData->m_sBasic.m_fScale * m_fScale );
		m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	}

	// 상태를 생성중으로 바꾼다;
	m_fIdleTime = 0.f;
	VECANIATTACK& cVecCreateAnimation =
		m_pCrowData->GetAnimation( AN_CREATE, AN_SUB_00 );
	if ( !cVecCreateAnimation.empty() )
	{
		SetActState( EM_SUMMONACT_CREATE );
	}

	return S_OK;
}*/

void GLCrow::SetPosition(const D3DXVECTOR3& vPosition)
{
	ToWalk ();

	if ( SetPositionActor(vPosition) == true )
		StopActorMove();

	return;
}

bool GLCrow::MoveTo( const D3DXVECTOR3* pTarPos, bool bJump, DWORD dwFlags )
{
	if ( m_pCrowData->IsPosHold() )
		return false;

	if( bJump )
		SetFLAG( EMJUMPTO );

	m_TargetID.vPos = *pTarPos;
	TurnAction( GLAT_MOVE );

	return true;
}

HRESULT GLCrow::AttackTo(STARGETID& sTARID, float fTime)
{
	// 현재 몹은 몹을 때릴 수 없음;
	// GLARoundSlot		m_ARoundSlot; 널 포인트 오류 발생;	
	if ( sTARID.emCrow != CROW_PC )
		return S_OK;
	
	m_TargetID = sTARID;
	AttackReAction(0.0f, fTime);	
	return S_OK;	
}

bool GLCrow::IsCheckedSkillFlagSpecial ( DWORD dwFlag )	const
{
	return GLCROWLOGIC::IsCheckedSkillFlagSpecial( dwFlag );
}

HRESULT GLCrow::ESCAPE ( STARGETID &sTARID, const DWORD dwDamage )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	if ( m_pCrowData->IsPosHold() )
		return S_FALSE;

	EMCROWACT_UP emCROWACT = GetActPattern();
	BOOL bESCAPE = ( EMCROWACT_UP_ESCAPE == emCROWACT ); // && ( dwDamage > DWORD ( GetMaxHP() * 0.10f ) ); // Note : damage에 따른 도망 로직은 요청으로 삭제.
    if ( !bESCAPE )
        return S_FALSE;

	ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, sTARID );
	if ( pACTOR && pACTOR->IsValidBody() )
	{
		const D3DXVECTOR3 &vACTOR_POS = pACTOR->GetPosition();

		D3DXVECTOR3 vDir = ( GetPosition() - vACTOR_POS );		//	Attack -> Receive 방향의 백터.
		vDir.y = 0.0f;
		D3DXVec3Normalize (  &vDir, &vDir );

		enum { EMROTSIZE = 5, EMESCAPE_RANGE = 60 };
		D3DXMATRIX _matROT000, _matROT045, _matROT315, _matROT090, _matROT270;
		D3DXMATRIX matROT000 = *D3DXMatrixRotationY ( &_matROT000, 0 );
		D3DXMATRIX matROT045 = *D3DXMatrixRotationY ( &_matROT045, D3DX_PI/4.0f );
		D3DXMATRIX matROT315 = *D3DXMatrixRotationY ( &_matROT315, -D3DX_PI/4.0f );
		D3DXMATRIX matROT090 = *D3DXMatrixRotationY ( &_matROT045, D3DX_PI/2.0f );
		D3DXMATRIX matROT270 = *D3DXMatrixRotationY ( &_matROT045, -D3DX_PI/2.0f );
		D3DXMATRIX *pmatROT[EMROTSIZE] = { &matROT000, &matROT045, &matROT315, &matROT090, &matROT270 };

		BOOL bCoB(FALSE);
		DWORD dwCoB(0);
		D3DXVECTOR3 vCoB(0,0,0);

		boost::shared_ptr<NavigationMesh> spNavi = pLand->GetNavi();
		if ( spNavi )
		{
			for ( int i=0; i<EMROTSIZE; ++i )
			{
				D3DXVECTOR3 vDirRot;
				D3DXVec3TransformCoord ( &vDirRot, &vDir, pmatROT[i] );

				D3DXVECTOR3 vPosB(GetPosition() + vDirRot * EMESCAPE_RANGE);
				spNavi->IsCollision(D3DXVECTOR3(vPosB.x,vPosB.y+20.0f,vPosB.z), D3DXVECTOR3(vPosB.x,vPosB.y-30.0f,vPosB.z), vCoB, dwCoB, bCoB );
				if(bCoB)
				{
					if( m_pCrowData->m_sAction.m_bRun )
						ToRun();
					MoveTo( &vCoB, false, 0 );
					return S_OK;
				}
			}
		}
	}

	return S_FALSE;
}

void GLCrow::ToRun ()
{
	GASSERT(m_pCrowData);

	SetActState(EM_ACT_RUN);
	m_actorMove.SetMaxSpeed ( m_pCrowData->m_sAction.m_fRunVelo );
}

void GLCrow::ToWalk ()
{
	GASSERT(m_pCrowData);

	ReSetActState(EM_ACT_RUN);
	m_actorMove.SetMaxSpeed ( m_pCrowData->m_sAction.m_fWalkVelo );
}

INT GLCrow::IsLowerHP ()							
{ 
	float fRate = 0.0f;
	for( INT i = 0; i < (INT)m_pCrowData->m_sAction.m_vecPatternList.size(); i++ )
	{
		fRate = m_pCrowData->m_sAction.m_vecPatternList[ i ].m_fPatternDNRate;
		if( GETHP() >= (int)( (float)GetMaxHP() * fRate ) / 100.0f )
		{
			return i;
		}
	}
	return -1;
}

EMCROWACT_UP GLCrow::GetActPattern ()
{
	INT nIndex = IsLowerHP();

	EMCROWACT_UP emACT_UP;

	if( nIndex >= 0 )
	{
		emACT_UP = m_pCrowData->m_sAction.m_vecPatternList[ nIndex ].m_emActPattern;
	}
	else if( nIndex < 0 )
	{
		emACT_UP = EMCROWACT_UP_IDLE;
	}

	return emACT_UP;
}

bool GLCrow::TurnActionIdle( const bool bMsg )
{
	m_fIdleTime = 0.0f;

	if( !bMsg )
		return false;

	GLMSG::SNET_ACTION_BRD NetMsg;
	NetMsg.emCrow	= m_pCrowData->m_emCrow;
	NetMsg.dwID		= m_dwGaeaID;
	NetMsg.emAction	= GLAT_IDLE;

	SendMsgViewAround(&NetMsg);

	return true;
} //GLCrow::TurnActionIdle

bool GLCrow::TurnActionMove(const bool bMsg, EMACTIONTYPE emOldAction)
{
	D3DXVECTOR3 vDist(m_vPosition - m_TargetID.vPos);
	if (D3DXVec3LengthSq(&vDist) < 1.0f)
	{
		TurnAction(GLAT_IDLE, (emOldAction != GLAT_IDLE));
		return false;
	}

	if (!bMsg)
		return false;

	D3DXVECTOR3 vP0(m_TargetID.vPos.x, m_TargetID.vPos.y + 10.0f, m_TargetID.vPos.z);
	D3DXVECTOR3	vP1(m_TargetID.vPos.x, m_TargetID.vPos.y - 10.0f, m_TargetID.vPos.z);
	BOOL bSucceed = m_actorMove.GotoLocation(vP0, vP1);
	if (bSucceed)
	{
		m_vStartPos = m_vPosition;
		if (m_dwCFlag & EMJUMPTO)
			SetPosition(m_TargetID.vPos);

		// 이동 메세지 발생.
		if ( m_emCrow != CROW_SUMMON )
		{
			GLMSG::SNETCROW_MOVETO		NetMsg(m_dwGaeaID,
												GetActState(),
												m_actorMove.Position(), // m_vPos가 변경될 수 있기때문에 변경함.
												m_TargetID.vPos,
												m_dwCeID,
												m_TargetID.GaeaId,
												m_TargetID.emCrow,
												m_dwCFlag );
			SendMsgViewAround( &NetMsg );
		}
		else
		{
			// 이동 메세지 발생.
			//
			GLMSG::SNET_SUMMON_GOTO NetMsg(
				m_dwGaeaID,
				m_TargetID,
				m_actorMove.Position(),
				m_TargetID.vPos,
				GetMoveVelo(),
				GetActState() );
			SendMsgViewAround ( &NetMsg );
		}
		

		m_dwCFlag &= ~EMJUMPTO;
	}

	return true;
}

bool GLCrow::PreTurnActionAttack()
{
	m_emReAction = EMREACT_NONE;

	// 공격수행이 끝날때 딜레이 등록.
	if( m_pAttackProp )
		m_fACTIONDELAY[m_dwAType] = m_pAttackProp->fDelay + 0.3f;

	return true;
} //GLCrow::PreTurnActionAttack

bool GLCrow::PreTurnActionSkill()
{
	m_idACTIVESKILL = NATIVEID_NULL();
	m_wTARNUM		= 0;
	m_emReAction	= EMREACT_NONE;

	if( m_pAttackProp )
	{
		SNATIVEID	sRunSkill	= m_pAttackProp->skill_id;
		WORD		wLevel		= m_pAttackProp->skill_lev;
		if( wLevel >= SKILL::MAX_LEVEL )
            return false;

		// 스킬 정보 가져옴.
		GLSKILL*	pSkill		= GLSkillMan::GetInstance().GetData( sRunSkill );
		if( !pSkill )
            return false;

		float		fSkillDelay = pSkill->m_sAPPLY.sDATA_LVL[wLevel].fDELAYTIME;
		m_fACTIONDELAY[m_dwAType] = fSkillDelay + 0.1f;
	} //if

	return true;
} //GLCrow::PreTurnActionSkill

bool GLCrow::TurnActionAttack( const bool bMsg )
{
	m_fattTIMER			= 0.0f;
	PreStrikeProc( FALSE, FALSE );

	if( !bMsg )
        return false;

	// 공격 메세지 발생
	GLMSG::SNETCROW_ATTACK		NetMsg( m_dwGaeaID,
										m_TargetID.emCrow,
										m_TargetID.GaeaId,
										m_dwAType,
										m_vPosition );
	SendMsgViewAround( &NetMsg );

	STARGETID		sTargetID = STARGETID( CROW_MOB, m_dwGaeaID, m_vPosition );
	if( m_TargetID.emCrow == CROW_PC )
	{
		PGLCHAR		pCHAR = m_pGaeaServer->GetChar ( m_TargetID.GaeaId );
		if( pCHAR )
		{ // PET Attack 처리
			PGLPETFIELD		pEnemyPet = m_pGaeaServer->GetPET ( pCHAR->m_dwPetGUID );
			if( pEnemyPet && pEnemyPet->IsValid() && !pEnemyPet->IsSTATE ( EM_PETACT_ATTACK ) )
			{
				float			fDist;
				D3DXVECTOR3		vOwnerPos	= pCHAR->GetPosition ();
				D3DXVECTOR3		vDist		= pEnemyPet->m_vPos - vOwnerPos;
				fDist = D3DXVec3Length( &vDist );

				if( fDist <= GLCONST_PET::fWalkArea )
				{ // in the area
					pEnemyPet->ReSetAllSTATE ();
					pEnemyPet->SetSTATE ( EM_PETACT_ATTACK );

					GLMSG::SNETPET_ATTACK NetMsg(sTargetID);
					m_pGaeaServer->SENDTOCLIENT ( pCHAR->ClientSlot(), &NetMsg );
					
					GLMSG::SNETPET_ATTACK_BRD NetMsgBRD(sTargetID);
					NetMsgBRD.dwGUID = pEnemyPet->m_dwGUID;
					pCHAR->SendMsgViewAround(&NetMsgBRD);
				} //if
			} //if
		} //if
	} //if

	return true;
} //GLCrow::TurnActionAttack

bool GLCrow::TurnActionSkill()
{
	m_fattTIMER = 0.0f;
	PreStrikeProc( TRUE, FALSE );

	StartSkillProc();

	return true;
} //GLCrow::TurnActionSkill

bool GLCrow::TurnActionAuthed()
{
	m_fIdleTime = 0.0f;

	GLMSG::SNET_CTF_CROW_AUTHED		msg;
    msg.emCrow		= m_pCrowData->m_emCrow;
    msg.sNativeID	= m_pCrowData->sNativeID;
    msg.dwID		= m_dwGaeaID;
	msg.dxPos		= GetPosition();	//미니맵에 인증마크를 출력하기 위해 좌표가 필요하다.
	
	for( int i=0; i < SCHOOL_NUM; i++ )
	{
		msg.arrHP4School[i] = m_arrHP4School[i];
	} //for

	msg.wSchoolAuthed = m_wSchoolAuthed;

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand != NULL )
        pLand->SendMsgPC( &msg );

	return true;
} //GLCrow::TurnActionAuthed

bool GLCrow::TurnActionFalling( const bool bMsg )
{
	m_fIdleTime = 0.0f;

	for (size_t i=0; i<CROW_SKILLFACT_SIZE; ++i)
		m_sSKILLFACT[i].RESET();	
	
	if( !bMsg )
		return false;

	// 주변 Char에게 쓰러짐을 알림.
	GLMSG::SNET_ACTION_BRD NetMsg;
	NetMsg.emCrow		= m_pCrowData->m_emCrow;
	NetMsg.dwID			= m_dwGaeaID;
	NetMsg.emAction		= GLAT_FALLING;

	SendMsgViewAround( &NetMsg );	

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand != NULL )
	{
		TriggerSystem::SServerDieCrowMessage msg;
		msg.dwCrowGaeaID = GetGaeaID();
		msg.eCromType = GETCROW();
		msg.dwKillerDbBum = GetKillerID();
		pLand->SendTriggerMessage(&msg);
	}

	return true;
} //GLCrow::TurnActionFalling

void GLCrow::TurnAction(const EMACTIONTYPE toAction, const bool bMsg)
{
	const EMACTIONTYPE& emAction = GetAction();

	// 이전 액션 취소.
	//
	switch( emAction )
	{
	case GLAT_IDLE:
        break;
	case GLAT_ATTACK:
        PreTurnActionAttack();
		SkinAniControl_AnimationMove();
        break;
	case GLAT_SKILL:
        PreTurnActionSkill();
		SkinAniControl_AnimationMove();
        break;

    // 몹이 두번이상 보상 드랍 하는 문제 [10/28/2013 hsshin]
    case GLAT_FALLING:
        // FALLING에서는 DIE외에는 상태를 변경할 수 없다;
        if ( toAction != GLAT_DIE )
            return;

        break;
    case GLAT_DIE:
        // 죽은 상태에서는 다른 액션으로 전환 불가;
        return;

	default:
        break;
	}
    
	// 액션 초기화.
	EMACTIONTYPE emOldAction = emAction;
	SetAction(toAction);

	switch (toAction)
	{
	case GLAT_IDLE:
        TurnActionIdle(bMsg);
        break;
	case GLAT_MOVE:
        TurnActionMove(bMsg, emOldAction);
        break;
	case GLAT_ATTACK:
        TurnActionAttack(bMsg);

		SendToChildCrowAttack();
        break;
	case GLAT_SKILL:
        TurnActionSkill();

		SendToChildCrowAttack();
        break;
	case GLAT_AUTHED:
        TurnActionAuthed();
        break;
	case GLAT_FALLING:
        TurnActionFalling(bMsg);
        break;
	case GLAT_SHOCK:
		if ( m_pCrowData->IsIgnoreShock() == FALSE )
			m_fIdleTime = 0.0f;
		break;
	case GLAT_PUSHPULL:
        m_fIdleTime = 0.0f;
        break;
	default:
        break;
	}

	if (m_actorMove.PathIsActive())
	{
		if (!isAction(GLAT_MOVE) && !isAction(GLAT_PUSHPULL))
			m_actorMove.Stop();
	}
}

void GLCrow::BeAuthedBy(WORD wSchool)
{
	GASSERT(wSchool < SCHOOL_NUM);

	m_wSchoolAuthed = wSchool;

	for (WORD i=0; i<SCHOOL_NUM; i++)
	{
		if (i == wSchool)
		{
			m_arrHP4School[i] = 0;
		}
		else
		{
			m_arrHP4School[i] = GetMaxHP() / 2;
		}
	}
}


WORD GLCrow::IsAuthedBy()
{
	return m_wSchoolAuthed;
}

DWORD GLCrow::ToDamage(const STARGETID &sTargetID, const int nDAMAGE, const DWORD dwDamageFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

	ServerActor* pACTOR_TAR = m_pGaeaServer->GetTarget(pLand, sTargetID);
	if (!pACTOR_TAR)
        return 0;

	// 타겟이 정상 상태일 경우에 damage 줄수 있음.
	//
	if ( (CROW_PC==sTargetID.emCrow || CROW_SUMMON==sTargetID.emCrow) && !pACTOR_TAR->IsValidBody() )
		return 0;
    else
	return pACTOR_TAR->ReceiveDamage ( GETCROW(), m_dwGaeaID, nDAMAGE, dwDamageFlag );
}

void GLCrow::DoFalling()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    // 현재 GLCrow의 경우 MOB,NPC의 경우만 죽음처리를 하도록 함. 나머지 경우 대해서 처리해야 하면 추후에 이부분을 변경해주면 됨 (CROW_ZONENAME 같은 의도치 않은 이벤트가 계속 발생해서);
    if ( ( GetCrow() == CROW_MOB ) || ( GetCrow() == CROW_NPC ) )
    {
        // 인던 리뉴얼/ HP가 0일때 죽일지 안죽일지에 대해서는 lua에서 제어를 할 수 있도록 함;
        InstanceSystem::InstanceField* const _pInstance = pLand->GetScriptInstance();
        if ( _pInstance != NULL )  // 인던이 아닐 경우 NULL;
        {
            DWORD dwID(m_sAssault.GaeaId);

			// Mob일 경우는 그대로 넣으면 된다;
			// PC일 경우 DBNum을 쓰기 때문에 따로 넣어야 한다;
            if ( m_sAssault.emCrow == CROW_PC )
            {
                GLChar* pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
                if ( pChar )
                    dwID = pChar->CharDbNum();
            }
			else if ( m_sAssault.emCrow == CROW_SUMMON )
			{
				GLSummonField* pSummon = pLand->GetSummon( m_sAssault.GaeaId );
				if ( pSummon )
				{
					ServerActor* pOwner = pSummon->GetOwner();

					// 현재는 소환수의 주인이 SUMMON일 경우는;
					// 생각하지 않는다;
					if ( pOwner && pOwner->GetCrow() == CROW_PC )
					{
						GLChar* pChar = static_cast< GLChar* >( pOwner );
						if ( pChar )
							dwID = pChar->CharDbNum();
					}
				}
			}

            // 만약 Event Died의 리턴값이 false일 경우 죽이는 처리를 하지 않는다는 뜻;
            if ( _pInstance->EventDie(GetCrow(), GetGaeaID(), m_sAssault.emCrow, dwID) == false )
                return;
        }
		// 죽은 몹 카운팅;
		pLand->AddDropOutCrowCount(GetNativeId());
    }

	if( GetCrow() != CROW_ZONENAME && GetCrow() != CROW_GATENAME && GetCrow() != CROW_PET )
	{
		//실제 죽음 처리;
		DoFalling_Proc();
	}
}

void GLCrow::DoFalling_Proc()
{
    // 쓰러질때 보상 발생. ( 경험치 + 아이템 + 금액 )
    GenerateReward ();

    //	행위 변화.
    TurnAction(GLAT_FALLING);

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    // 그룹 리더인 경우에 그룹 타겟을 초기화한다.
    if( IsGroupLeader() )
    {
		pLand->RESET_GROUPTARGET( GetGroupName() );
    }
}

void GLCrow::BlockProc(const STARGETID& sTargetID, const SNATIVEID& SkillID /* = NATIVEID_NULL() */, const WORD wSkillLevel /* = 0*/)
{
	// 주변 클라이언트들에게 메세지 전송. 
	GLMSG::SNETCROW_BLOCKING NetMsg(
        m_dwGaeaID,
        sTargetID.emCrow, sTargetID.GaeaId,
		SkillID, wSkillLevel);
	SendMsgViewAround(&NetMsg);	
}

void GLCrow::AvoidProc(const SNATIVEID& SkillID /* = NATIVEID_NULL() */, const WORD wSkillLevel /* = 0*/)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	// 주변 클라이언트들에게 메세지 전송.
	GLMSG::SNETCROW_AVOID NetMsg(
		m_dwGaeaID,
		m_TargetID.emCrow, m_TargetID.GaeaId,
		SkillID, wSkillLevel);
	SendMsgViewAround(&NetMsg);

	ServerActor* pACTOR_TAR = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
	if (!pACTOR_TAR)
        return;

	// 타겟이 정상 상태일 경우에 damage 줄수 있음.
	if (CROW_PC==m_TargetID.emCrow && pACTOR_TAR->IsValidBody())
	{
		pACTOR_TAR->ReceiveDamage(GETCROW(), m_dwGaeaID, 0, 0 );
	}
}

void GLCrow::DamageProc ( const int nDAMAGE, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	// 대상자에게 대미지값 적용.
	const DWORD dwActualDamage = ToDamage ( m_TargetID, nDAMAGE, dwDamageFlag );


	// 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETCROW_DAMAGE NetMsg(
        m_dwGaeaID,
        m_TargetID.emCrow,
        m_TargetID.GaeaId,
        dwActualDamage,
        m_dwNowHP,
        dwDamageFlag);
	SendMsgViewAround(&NetMsg);

    // 공격한 대상
    NetMsg.emHitCrow = m_emCrow;

	ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
    if (!pACTOR)
        return;

	pACTOR->ReactDamage( this, dwDamageFlag );

	// 상태 이상 유발.
	//
	if ( pACTOR && m_pAttackProp->emBLOW_TYPE!=EMBLOW_NONE )
	{
		// 발생 확율 계산.
		//
		short nBLOWRESIST = pACTOR->GETRESIST().GetElement ( STATE_TO_ELEMENT(m_pAttackProp->emBLOW_TYPE) );
		if ( nBLOWRESIST>99 )	nBLOWRESIST = 99;

//		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( m_pAttackProp->emBLOW_TYPE, GLPeriod::GetInstance().GetWeather(), pLand->IsWeatherActive() );
		DWORD dwWeather = GLPeriod::GetInstance().GetMapWeather( pLand->GetBaseMapID().wMainID, pLand->GetBaseMapID().wSubID );
		float fPOWER = GLOGICEX::WEATHER_BLOW_POW ( m_pAttackProp->emBLOW_TYPE, dwWeather, pLand->IsWeatherActive() );

		BOOL bBLOW = FALSE;
		if ( !(pACTOR->GETHOLDBLOW()&STATE_TO_DISORDER(m_pAttackProp->emBLOW_TYPE)) )
		{
			bBLOW = GLOGICEX::CHECKSTATEBLOW ( m_pAttackProp->fBLOW_RATE*fPOWER, GETLEVEL(), pACTOR->GetLevel(), nBLOWRESIST );
		}

		if ( bBLOW )
		{
			// 상태이상 발생.
			//	
			SSTATEBLOW sSTATEBLOW;
			float fLIFE = m_pAttackProp->fBLOW_LIFE * fPOWER;
			fLIFE = ( fLIFE - (fLIFE*nBLOWRESIST*0.01f*GLCONST_CHAR::fRESIST_G ) );

			sSTATEBLOW.emBLOW = m_pAttackProp->emBLOW_TYPE;
			sSTATEBLOW.fAGE = fLIFE;
			sSTATEBLOW.fSTATE_VAR1 = m_pAttackProp->fBLOW_VAR1;
			sSTATEBLOW.fSTATE_VAR2 = m_pAttackProp->fBLOW_VAR2;

			if ( pACTOR->STATEBLOW ( sSTATEBLOW ) )
			{
				// 상태이상 발생 Msg.
				//
				GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
				NetMsgState.emCrow = m_TargetID.emCrow;
				NetMsgState.dwID = m_TargetID.GaeaId;
				NetMsgState.emBLOW = sSTATEBLOW.emBLOW;
				NetMsgState.fAGE = fLIFE;
				NetMsgState.fSTATE_VAR1 = sSTATEBLOW.fSTATE_VAR1;
				NetMsgState.fSTATE_VAR2 = sSTATEBLOW.fSTATE_VAR2;

				// '스킬대상'의 주변 클라이언트들에게 메세지 전송.
				//
				pACTOR->SendMsgViewAround(&NetMsgState);

				// 스킬 대상에게 메세지 전송.
				if ( m_TargetID.emCrow==CROW_PC )	
					m_pGaeaServer->SENDTOCLIENT ( pACTOR->GETCLIENTID (), &NetMsgState );
			}
		}
	}
}

void GLCrow::DamageCurse(int nDAMAGE, const STARGETID& sACTOR, EMCROW emCrow)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, sACTOR );

    if ( !pACTOR )
    {
        return;
    }

    int nActualDamageHP = (int)-nDAMAGE;
    int nActualDamageMP(0);
    int nActualDamageSP(0);
    //	체력 깍는다.(꼭 데미지만 가능한것은 아니다 힐도 가능);
    VAR_BODY_POINT( sACTOR.emCrow, sACTOR.GaeaId, DAMAGE_TYPE_CURSE, nActualDamageHP, nActualDamageMP, nActualDamageSP );

	// 주변 클라이언트들에게 메세지 전송.
	//
	GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
	NetMsgBRD.emCrow = GetCrow();
	NetMsgBRD.dwID = m_dwGaeaID;

	//	스킬을 사용한 행위자 마킹.
	NetMsgBRD.sACTOR.emCrow = sACTOR.emCrow;
	NetMsgBRD.sACTOR.dwID = sACTOR.GaeaId;

	NetMsgBRD.dwDamageFlag = DAMAGE_TYPE_CURSE;
	NetMsgBRD.nVAR_HP = nActualDamageHP;
	NetMsgBRD.nVAR_MP = nActualDamageMP;
	NetMsgBRD.nVAR_SP = nActualDamageSP;


    NetMsgBRD.wSchool = pACTOR->GetSchool();

	SendMsgViewAround(&NetMsgBRD);
}

const bool GLCrow::IsImmuneToSkill ( BYTE iIndex ) const
{
	if ( RF_IMMUNE( m_EffSkillVarSet ).IsOn() )
	{
		if ( RF_IMMUNE( m_EffSkillVarSet ).IsImmune( iIndex ) )
		{
			return true;
		}
	}

	return false;
}

void GLCrow::DamageReflectionProc(int nDAMAGE, const STARGETID& sACTOR)
{
	// 대상자에게 대미지값 적용.
	const DWORD dwActualDamage = ToDamage ( sACTOR, nDAMAGE, 0 );

	// 주변 클라이언트들에게 메세지 전송.
	GLMSG::SNETCROW_DAMAGE NetMsg(
        m_dwGaeaID,
        sACTOR.emCrow,
        sACTOR.GaeaId,
        dwActualDamage,
        m_dwNowHP,
        DAMAGE_TYPE_NONE);
	SendMsgViewAround(&NetMsg);
}

void GLCrow::PreAttackProc ()
{
	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;

	m_sHITARRAY.clear ();

	for ( int i = 0; i < sAniAttack.m_wDivCount; i++ )
	{
		bool bhit = CheckHit(m_TargetID) ? true : false;
		m_sHITARRAY.push_back ( SSTRIKE(sAniAttack.m_wDivFrame[i],bhit) );
	}
}

void GLCrow::PreStrikeProc ( BOOL bSkill, BOOL bLowSP )
{
	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;

	//m_sHITARRAY.clear ();

	STARGETID sTargetID;
	BOOL bCheckHit = FALSE;

	if ( !bSkill )
	{
		sTargetID = m_TargetID;
		bCheckHit = TRUE;
	}
	else
	{
		// 스킬 정보 가져옴.
		//
		GLSKILL* pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
		if ( pSkill->m_sBASIC.emAPPLY != SKILL::EMAPPLY_MAGIC )
		{
			// 스킬 타겟을 하나만 유효하게 확인을 하고 있음.
			//	NEED : 다중 타겟일 경우 다시 확인해야함.
			sTargetID = STARGETID(m_sTARIDS[0].emCrow, m_sTARIDS[0].dwID);			
			bCheckHit = TRUE;
		}
		else
		{
			sTargetID.GaeaId = EMTARGET_NULL;
		}
	}

	//	큐 초기화
	m_sHITARRAY.clear ();

	for ( int i = 0; i < sAniAttack.m_wDivCount; i++ )
	{
		bool bhit = true;
		if (bCheckHit && (sTargetID.GaeaId != EMTARGET_NULL)) //GAEAID_NULL
		{
			if (!CheckHit(sTargetID))
                bhit = false;
		}

		m_sHITARRAY.push_back ( SSTRIKE(sAniAttack.m_wDivFrame[i],bhit) );
	}
}

BOOL GLCrow::AttackProcess ( float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	GASSERT(m_pAttackProp);
	m_fattTIMER += fElapsedTime * GETATTVELO();

	SANIATTACK &sAniAttack = m_pAttackProp->sAniAttack;
	int wTotalKeys = int(sAniAttack.m_dwETime) - int(sAniAttack.m_dwSTime);
	int wThisKey = int(m_fattTIMER*UNITANIKEY_PERSEC);

	ServerActor* const pTargetActor = m_pGaeaServer->GetTarget(pLand, m_TargetID);
	if ( pTargetActor == NULL )
	{
		TurnAction ( GLAT_IDLE, false );
		return FALSE;
	}

	if ( GetAttackRangeType()==EMATT_SHORT )
	{
		D3DXVECTOR3 vDist = m_vPosition - pTargetActor->GetPosition();
		float fTarLength = D3DXVec3Length(&vDist);

		WORD wAttackRange = pTargetActor->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 20;
		if ( wAttackRange*GLCONST_CHAR::fATTACK_RANGE_ADJUST_RATE < (WORD)(fTarLength) )
		{
			TurnAction ( GLAT_IDLE, false );
			return FALSE;
		}
	}

	if ( !m_sHITARRAY.empty() )
	{
		SSTRIKE sStrike = *m_sHITARRAY.begin();

		if ( wThisKey >= sStrike.wDivKey )
		{
			if ( sStrike.bHit )
			{
				int nDamage = 0;				
				const DWORD dwDamageFlag = CALCDAMAGE ( nDamage, m_dwGaeaID, m_TargetID, pLand );
				
				if ( CalcBlocking(pTargetActor, nDamage) == true )
					BlockProc(m_TargetID);
				else 
				{
					DamageProc(nDamage, dwDamageFlag);
				}
			}
			else
				AvoidProc ();

			//	큐 데이타가 처리된 경우 삭제한다.
			m_sHITARRAY.pop_front ();
		}	
	}

	//	이상증상에 의해 데이타가 전혀 처리되지 않은경우,
	//	마지막에 한꺼번에 처리하고, FALSE를 리턴한다.
	if ( wThisKey >= wTotalKeys )
	{
		if ( !m_sHITARRAY.empty() )
		{
			while ( !m_sHITARRAY.empty() )
			{
				SSTRIKE sStrike = *m_sHITARRAY.begin();
				
				if ( sStrike.bHit )
				{
					int nDamage = 0;
					DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
					dwDamageFlag = CALCDAMAGE ( nDamage, m_dwGaeaID, m_TargetID, pLand );

					if ( CalcBlocking(pTargetActor, nDamage) == true )
						BlockProc ( m_TargetID );
					else 
						DamageProc ( nDamage, dwDamageFlag );
				}
				else
					AvoidProc ();

				//	큐 데이타가 처리된 경우 삭제한다.
				m_sHITARRAY.pop_front ();
			}
		}

		return FALSE;
	}

	return TRUE;
}

BOOL GLCrow::ShockProcess ( float fElapsedTime )
{
	const VECANIATTACK &vecShock = m_pCrowData->GetAnimation(AN_SHOCK,AN_SUB_NONE);
	if ( vecShock.empty() )
        return FALSE;

	m_fIdleTime += fElapsedTime * GETATTVELO();
			
	const SANIATTACK &sAniShock = vecShock[0];

	int wTotalKeys = int(sAniShock.m_dwETime) - int(sAniShock.m_dwSTime);
	int wThisKey = int(m_fIdleTime*UNITANIKEY_PERSEC);
	
	if ( wThisKey >= wTotalKeys )
        return FALSE;

	return TRUE;
}

void GLCrow::ReqNetMsg_Drop(GLMSG::SNETDROP_CROW& Msg)
{
	//GLMSG::SNETDROP_CROW NetMsg;
	//NetMsg = GLMSG::SNETDROP_CROW();
	
	SDROP_CROW& dropCrow	= Msg.Data;
	dropCrow.sNativeID		= m_sNativeID;
	dropCrow.sMapID			= GetCurrentMap().getBaseMapID();
	dropCrow.dwCeID			= m_dwCeID;
	dropCrow.dwGlobalID		= m_dwGaeaID;
	dropCrow.vPos			= m_vPosition;
	dropCrow.vDir			= m_vDirect;
	dropCrow.dwNowHP		= m_dwNowHP;
	dropCrow.wNowMP			= m_wNowMP;
    dropCrow.fMultipleHP    = GetMultipleHP();
	dropCrow.fRestorationHP_Plus = GetHPRestorationRatePlus();
	dropCrow.fRestorationMP_Plus = GetMPRestorationRatePlus();
	dropCrow.fRestorationSP_Plus = GetSPRestorationRatePlus();
	dropCrow.fInfluenceY = m_pMobSchedule->m_fInfluenceY;

	//
	//mjeon.CaptureTheField
	//
	for (int i=0; i<SCHOOL_NUM; ++i)
	{
		dropCrow.arrHP4School[i] = m_arrHP4School[i];
	}

	dropCrow.wSchoolAuthed = m_wSchoolAuthed;
	dropCrow.fScale		   = m_fScale;

	if (m_fAge < 0.6f)
		dropCrow.dwFLAGS	|= SDROP_CROW::CROW_GEM;

	dropCrow.emAction		= GetAction();
	dropCrow.dwActState	= GetActState();
	dropCrow.TargetID		= m_TargetID;
	
	if (dropCrow.emAction==GLAT_MOVE)
	{
		if ( m_actorMove.PathIsActive() )
		{
			dropCrow.TargetID.vPos = m_actorMove.GetTargetPosition();

			// 서버 클라이언트간 몹의 위치 동기화를 위해
			dropCrow.vStartPos		= m_vStartPos;
			dropCrow.sNextWaypoint  = m_actorMove.GetNextWayPoint ();
		}
		else
		{
			dropCrow.emAction = GLAT_IDLE;
		}
	}
	
	for (int i=0; i<CROW_SKILLFACT_SIZE; ++i)
        dropCrow.sSKILLFACT[i].Assign ( m_sSKILLFACT[i], i );
	for (int i=0; i<EMBLOW_MULTI; ++i)
        dropCrow.sSTATEBLOWS[i] = m_sSTATEBLOWS[i];

	//return NetMsg;
}

inline void GLCrow::INSERT_PC_VIEW ( DWORD dwGAEAID )
{
	if ( m_vecPC_VIEW.size() < m_dwPC_AMOUNT+16 )
		m_vecPC_VIEW.resize(m_dwPC_AMOUNT+16);

	m_vecPC_VIEW[m_dwPC_AMOUNT++] = dwGAEAID;
}

inline void GLCrow::INSERT_SUMMON_VIEW ( DWORD dwGAEAID )
{
	if ( m_vecSUMMON_VIEW.size() < m_dwSUMMON_AMOUNT+16 )
		m_vecSUMMON_VIEW.resize(m_dwSUMMON_AMOUNT+16);

	m_vecSUMMON_VIEW[m_dwSUMMON_AMOUNT++] = dwGAEAID;
}

inline void GLCrow::INSERT_MOB_VIEW ( DWORD dwGAEAID )
{
	if ( m_vecMob_VIEW.size() < m_dwMOB_AMOUNT+16 )
		m_vecMob_VIEW.resize(m_dwMOB_AMOUNT+16);

	m_vecMob_VIEW[m_dwMOB_AMOUNT++] = dwGAEAID;
}

void GLCrow::UpdateViewAroundPc( GLLandNode *pLandNode, BOUDRECT& bound )
{
	if( pLandNode->m_PCList.m_dwAmount !=0 )
	{
		GLCHARNODE *pCharNode = pLandNode->m_PCList.m_pHead;
		for( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar *pChar = pCharNode->Data;
			// 캐릭터의 삭제 시점과 UpdateViewAround ()과의 불일치로 캐릭터가 존재하지 않을 수 있음.
			if ( pChar == NULL )													
				continue;

			if (!m_pGaeaServer->GetChar(pChar->GetGaeaID()))
				continue;

			const D3DXVECTOR3& vPosition = pChar->GetViewPosition();
			const int nPosition[2] = 
			{
				int(vPosition.x),
				int(vPosition.z)
			};

			BOUDRECT _boundRect(bound);
			
 			if ( pChar->m_bFlyCameraUse )
 			{
 				int nRange = pChar->GetViewAroundValue();
 				_boundRect.nMaxX += nRange;
 				_boundRect.nMaxZ += nRange;
 				_boundRect.nMinX -= nRange;
 				_boundRect.nMinZ -= nRange;
 			}

			if( _boundRect.IsWithIn(nPosition[0], nPosition[1]) )
				INSERT_PC_VIEW( pChar->GetGaeaID() );
		} //for
	} //if
} //GLCrow::UpdateViewAroundPc

void GLCrow::UpdateViewAroundSummon( GLLandNode *pLandNode, BOUDRECT& bound )
{
	if( pLandNode->m_SummonList.m_dwAmount !=0 )
	{
		GLSUMMONNODE *pSummonNode = pLandNode->m_SummonList.m_pHead;
		for ( ; pSummonNode; pSummonNode = pSummonNode->pNext )
		{
			GLSummonField* pSummon = pSummonNode->Data;
			if ( pSummon == NULL )
				continue;

			const D3DXVECTOR3& vPosition = pSummon->GetPosition();
			const int nPosition[2] = 
			{
				int(vPosition.x),
				int(vPosition.z)
			};

			if ( bound.IsWithIn(nPosition[0], nPosition[1]) )
				INSERT_SUMMON_VIEW ( pSummon->GetGaeaID() );
		} //for
	} //if
} //GLCrow::UpdateViewAroundSummon

void GLCrow::UpdateViewAroundMob( GLLandNode *pLandNode, BOUDRECT& bound )
{
	if( pLandNode->m_CROWList.m_dwAmount !=0 )
	{
		GLCROWNODE*		pCrowNode = pLandNode->m_CROWList.m_pHead;
		for ( ; pCrowNode; pCrowNode = pCrowNode->pNext )
		{
            if ( pCrowNode->Data == this ) // 자신은 제외;
                continue;

			GLCrow* pCrow = pCrowNode->Data;
			if ( pCrow ==NULL )
				continue;

			const D3DXVECTOR3& vPosition = pCrow->GetPosition();


			if ( bound.IsWithIn(static_cast<int>(vPosition.x), static_cast<int>(vPosition.y)) )
				INSERT_MOB_VIEW( pCrow->GetGaeaID() );
		} //for
	} //if
} //GLCrow::UpdateViewAroundMob

HRESULT GLCrow::UpdateViewAround ()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	m_dwPC_AMOUNT     = 0;
	m_dwSUMMON_AMOUNT = 0;
	m_dwMOB_AMOUNT	  = 0;

	// 현재 시야에 들어오는 CELL 모두 탐색.
	//
	const int nPosition[2] = 
	{
		int(m_vPosition.x),
		int(m_vPosition.z)
	};

	LANDQUADNODE* pQuadHead = NULL;
	const int _maxViewRange( MAX_VIEWRANGE );
	BOUDRECT bRect(nPosition[0]+_maxViewRange, nPosition[1]+_maxViewRange, nPosition[0]-_maxViewRange, nPosition[1]-_maxViewRange);
	pLand->GetLandTree()->FindNodes ( bRect, pLand->GetLandTree()->GetRootNode(), &pQuadHead );
	
	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GLLandNode *pLandNode = pQuadNode->pData;
		if( !pLandNode ) 
			continue;

		UpdateViewAroundPc( pLandNode, bRect );
		UpdateViewAroundSummon( pLandNode, bRect );
		UpdateViewAroundMob( pLandNode, bRect );
	} //for

	return S_OK;
}

// 클라 객체와 동일해야한다;
void GLCrow::UpdateActState(void)
{
	if ( IsActState( EM_ACT_EMERGENCY ) )
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GetMaxHP()))
			, fCheckRate(m_pCrowData->IsBoss() ? GLCONST_CHAR::fEmergency_BossHPR : GLCONST_CHAR::fEmergency_MobHPR);
		if ( fCheckRate < fHPRate )
		{
			ReSetActState( EM_ACT_EMERGENCY );
		}
	}
	else
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GetMaxHP()))
			, fCheckRate(m_pCrowData->IsBoss() ? GLCONST_CHAR::fEmergency_BossHPR : GLCONST_CHAR::fEmergency_MobHPR);
		if ( fHPRate <= fCheckRate )
		{
			SetActState( EM_ACT_EMERGENCY );
		}
	}
}

HRESULT GLCrow::SendMsgViewAround(NET_MSG_GENERIC* pMsg)
{
	DWORD dwGAEAID = 0;
	GLChar* pPChar = NULL;

	DWORD wSize = (DWORD)m_vecPC_VIEW.size();

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<wSize; ++i )
	{
		//	Memo :	m_vecPC_VIEW[i] is GaeaID
		pPChar = m_pGaeaServer->GetChar(m_vecPC_VIEW[i]);
		if (pPChar)
			m_pGaeaServer->SENDTOCLIENT(pPChar->ClientSlot(), pMsg);
	}

	return S_OK;
}
void GLCrow::SendToClient(NET_MSG_GENERIC* pMsg){
	GLCrow::SendMsgViewAround (pMsg);
}

void GLCrow::SendToClient(DWORD dwClientSlot, NET_MSG_GENERIC* pMsg)
{
	if (m_pGaeaServer)
		m_pGaeaServer->SENDTOCLIENT(dwClientSlot, pMsg);
}

BOOL GLCrow::IsCtrlBlockBody () const
{
	if ( isAction(GLAT_SHOCK) )		return TRUE;
	if ( isAction(GLAT_PUSHPULL) )	return TRUE;

	return FALSE;
}

BOOL GLCrow::IsNonDamaged() const
{
	if ( !m_pCrowData ) return FALSE;

	if ( m_pCrowData->m_bNonDamaged ) return TRUE;

	return FALSE;
}

HRESULT GLCrow::UpdateDurDamage ( float fTime, float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	//	지속 데미지 처리( 데미지 준다. ) 
	for ( int i = 0; i < EMELEMENT_MAXNUM; ++i ) 
	{
		if ( !m_sDurDamage[i].VAILD() ) continue;

		m_sDurDamage[i].m_fTime -= fElapsedTime;			
	
		if ( m_sDurDamage[i].m_fTime < 0.0f ) 
		{
			//	데미지 준다.
			m_sDurDamage[i].m_fTime = m_sDurDamage[i].m_fTickTime;

			//	데미지 계산

			// 전체저항값 = 상대의 저항값 / 100 * 속성저항치 적용비율
			int nRESIST = GETRESIST().GetElement( (EMELEMENT)i );
			float fResistTotal = (float) ((float) nRESIST * 0.01f * GLCONST_CHAR::fRESIST_G);
			fResistTotal = fResistTotal > GLCONST_CHAR::fRESIST_MAX_CROW ? GLCONST_CHAR::fRESIST_MAX_CROW : fResistTotal;

			// 최소/최대 데미지 = 데미지-(데미지*전체저항값)
			int nDamage = int(m_sDurDamage[i].m_fDamage);
			nDamage -= int(m_sDurDamage[i].m_fDamage  * fResistTotal);

            ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, m_sDurDamage[i].m_sID );

            if ( !pACTOR )
            {
                continue;
            }

            int nActualDamageHP = -nDamage;
            int nActualDamageMP(0);
            int nActualDamageSP(0);
            //	체력 깍는다.(꼭 데미지만 가능한것은 아니다 힐도 가능);
            VAR_BODY_POINT( m_sDurDamage[i].m_sID.emCrow, m_sDurDamage[i].m_sID.GaeaId, DAMAGE_TYPE_DOT, nActualDamageHP, nActualDamageMP, nActualDamageSP );

			GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
			NetMsgBRD.emCrow = GetCrow();
			NetMsgBRD.dwID = m_dwGaeaID;

			//	스킬을 사용한 행위자 마킹.
			NetMsgBRD.sACTOR.emCrow = m_sDurDamage[i].m_sID.emCrow;
			NetMsgBRD.sACTOR.dwID = m_sDurDamage[i].m_sID.GaeaId;

			NetMsgBRD.dwDamageFlag |= DAMAGE_TYPE_DOT;
			NetMsgBRD.nVAR_HP = nActualDamageHP;
			NetMsgBRD.nVAR_MP = nActualDamageMP;
			NetMsgBRD.nVAR_SP = nActualDamageSP;


            NetMsgBRD.wSchool = pACTOR->GetSchool();

			SendMsgViewAround(&NetMsgBRD);
		}
	}  

	return S_OK;
}

HRESULT GLCrow::UpateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	float fAnimation = fElapsedTime;

	EMANI_MAINTYPE MType = m_pSkinChar->GETCURMTYPE();
	EMANI_SUBTYPE SType = m_pSkinChar->GETCURSTYPE();
	float fLIFETIME = m_pSkinChar->GETCURANITIME();

	const EMACTIONTYPE& emAction = GetAction();
	switch ( emAction )
	{
	case GLAT_IDLE:
		if ( m_sHALLUCINATE.bEmoticon )
		{
			if ( !m_pSkinChar->IsANI_SELECTANI(AN_GESTURE, m_sHALLUCINATE.emAni_SubType) )
				m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		}
		else
			m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );		
		break;

	case GLAT_TALK:
		m_pSkinChar->SELECTANI ( AN_TALK, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_MOVE:
		if ( IsActState(EM_ACT_RUN) )
			m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
		else
			m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		break;

	case GLAT_ATTACK:
		m_pSkinChar->SELECTANI ( AN_ATTACK, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_SHOCK:
//		if ( !ShockProcess ( fElapsedTime ) )
//			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_FALLING:
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		if ( m_pSkinChar->ISENDANIM() )
			TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		fAnimation = 0.0f;
		m_pSkinChar->TOENDTIME();
		break;
	case GLAT_GATHERING:
		m_pSkinChar->SELECTANI ( AN_GATHERING, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )
			TurnAction ( GLAT_IDLE );
		break;
	};

	// Mob 스킨 업데이트.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( emAction )
	{
	case GLAT_MOVE:
		fSkinAniElap *= GLCROWLOGIC::GETMOVEVELO();
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		fSkinAniElap *= GETATTVELO();
		break;
	};

	m_pSkinChar->SetPosition ( m_vPosition );
	m_pSkinChar->FrameMove ( fTime, fSkinAniElap, FALSE, m_matTrans );

	return S_OK;
}

float GLCrow::GetMoveVelo()
{
    return GLOGICEX::GetMoveVelocity(
        IsActState(EM_ACT_RUN),
        m_pCrowData->m_sAction.m_fRunVelo,
        m_pCrowData->m_sAction.m_fWalkVelo,
        false,
        GLCROWLOGIC::GETMOVEVELO(),
        0.0f,
        m_fMoveVeloLimit_Min,
        m_fMoveVeloLimit_Max);
	//float fDefaultVelo = IsSTATE(EM_ACT_RUN) ? m_pCrowData->m_sAction.m_fRunVelo : m_pCrowData->m_sAction.m_fWalkVelo;
	//return fDefaultVelo * GLCROWLOGIC::GETMOVEVELO();
}

HRESULT GLCrow::FrameMove ( float fTime, float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );
	//HRESULT hr;
	GASSERT(m_pCrowData);
	GASSERT(pLand);

	// 랜드에서 등록된 셀을 업데이트 하기 위해서 호출함.
	//
	pLand->MoveCrow ( m_dwGaeaID, m_vPosition );
	UpdateViewAround ();

	m_fAge += fElapsedTime;	

	// Comp 획득;
	ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );

	// HP가 100%가 되면 DamageLog 초기화;
	if( GetMaxHP() <= GetNowHP() )
	{
		m_cDamageLog.clear();
		m_cSummonDamageLog.clear();
		m_cDamageLogParty.clear();
	}
	
	// 아래 조건을 모두 만족할 경우에는 움직일 필요가 없다고 판단, server의 과부하 방지를 위해 FrameMove를 하지 않는다;
	if ( pLand->GET_GROUPTARGET( GetGroupName() ).GaeaId == EMTARGET_NULL &&  // 현재 그룹(스위치)의 타겟이 없을 때;
		m_dwPC_AMOUNT==0 &&                 // 주변의 플레이어가 없을때;
		m_dwSUMMON_AMOUNT==0 &&             // 주변의 소환수가 없을때;
		!m_pSkinChar &&                     // skinchar가 무효일때(에뮬레이터,level edit)가 아닐때;
		!m_pCrowData->IsBoss() &&           // 보스몹이 아닐때;
        !IsGroupLeader() &&                 // 그룹(스위치) 몹 리더가 아닐때;
		!pLand->IsInstantMap() &&      // 구버전 인던이 아닐때 (본사 51층);
		!m_pGaeaServer->IsEmulatorMode() && // 에뮬레이터가 아닐때;
		!pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) && // 해당 랜드가 몬스터 무한 추적 옵션이 없을때;
		!m_bIsPatrolStop )  
		return S_OK;

	GASSERT(m_dwAType<m_pCrowData->m_vecCrowAttack.size());
	m_pAttackProp = &(m_pCrowData->m_vecCrowAttack[m_dwAType]);

	DWORD dwSize = (DWORD)m_pCrowData->m_vecCrowAttack.size();

	for ( DWORD i=0; i<dwSize; ++i )
	{
		if ( m_fACTIONDELAY[i] > 0.0f )
			m_fACTIONDELAY[i] -= fElapsedTime;
	}

	// AABB 계산.
	//
	m_vMax = m_vPosition + m_vMaxOrg;
	m_vMin = m_vPosition + m_vMinOrg;

	// 공격 목표의 유효성 검사.
	//		(주의) 타갯정보 사용전 반드시 호출하여 유효성 검사 필요.
	//
	bool bVALID_TAR(true);

	BOOL bCHECK_TAR = m_pGaeaServer->ValidCheckTarget(pLand,m_TargetID);
	
	if ( !bCHECK_TAR )
		bCHECK_TAR = false;



	// 공격으로 인하여 패트롤이 중단되었다.
	if( m_bIsPatrolStop == true )
	{
		GLMobSchedule* pMobSch = GetMobSchedule();
		float fActivityArea = 0;
		if( pMobSch )
			fActivityArea = pMobSch->m_fActivityArea;

		// 타겟이 있는가?
		if( bCHECK_TAR )
		{
			// 행동반경이 설정되어 있다면.
			if( 0 < fActivityArea )
			{
				D3DXVECTOR3 vDist = m_vPosition - m_vPatrolStopPos;
				float fDist = D3DXVec3LengthSq(&vDist);

				// 행동반경을 넘었을 때 원래 위치로 돌아가라.
				if( fActivityArea*fActivityArea < fDist )
				{
					m_bReturnPatrolPos = true; //복귀하라!
				}
			}
		}
		else
		{
			D3DXVECTOR3 vGenToPos = m_vGenPos - m_vPatrolStopPos;
			float fGenToPos = D3DXVec3Length( &vGenToPos );

			if( fActivityArea + 15 < fGenToPos )
			{
				m_vPatrolStopPos = m_vGenPos;
			}

			// 타겟이 없다면 복귀하라!
			MoveTo( &m_vPatrolStopPos, false, 0 );

			D3DXVECTOR3 vDist = m_vPosition - m_vPatrolStopPos;
			float fDist = D3DXVec3LengthSq(&vDist);
			
			// 원래 위치와 가까이 왔다면 리셋
			if( fDist < 15 )
			{
				m_bReturnPatrolPos = false;
				m_bIsPatrolStop = false;				// 패트롤 정지 해제
				m_vPatrolStopPos = D3DXVECTOR3(0,0,0);	// 패트롤 정지 위치 재설정
			}
		}

	}

	// //복귀 명령이 들어오면 무조껀 위치로 이동하라.
	// 대신 어뷰징을 막기위해서 HP를 채운다.
	if( m_bReturnPatrolPos == true )
	{
		ResetTarget();

		if( m_dwNowHP != GetMaxHP() )
		{
			STARGETID me;
			me.emCrow = m_emCrow;
			me.vPos = m_vPosition;
			me.GaeaId = m_dwGaeaID;

			DamageCurse( -GetMaxHP(), me );
		}


		// 뛸 수 있다면 뛰어라
		if ( m_pCrowData->m_sAction.m_bRun )
			ToRun();

		GLMobSchedule* pMobSch = GetMobSchedule();
		float fActivityArea = 0;
		if( pMobSch )
			fActivityArea = pMobSch->m_fActivityArea;

		D3DXVECTOR3 vGenToPos = m_vGenPos - m_vPatrolStopPos;
		float fGenToPos = D3DXVec3Length( &vGenToPos );
		
		if( fActivityArea + 15 < fGenToPos )
		{
			m_vPatrolStopPos = m_vGenPos;
		}

		// 생성위치로 돌아가라.
		MoveTo( &m_vPatrolStopPos, false, 0 );

		D3DXVECTOR3 vDist = m_vPosition - m_vPatrolStopPos;
		float fDist = D3DXVec3LengthSq(&vDist);

		// 원래 위치와 가까이 왔다면 리셋
		if( fDist < 15 )
		{
			m_bReturnPatrolPos = false;				// 귀환 해제
			m_bIsPatrolStop = false;				// 패트롤 정지 해제
			m_vPatrolStopPos = D3DXVECTOR3(0,0,0);	// 패트롤 정지 위치 재설정
		}
	}

	const bool bDominated = m_sDOMINATION.emController != EMSPEC_DOMINATE_TYPE_SELF;
	if ( bCHECK_TAR && m_TargetID.emCrow==CROW_PC )
	{
		PGLCHAR pCHAR = m_pGaeaServer->GetChar ( m_TargetID.GaeaId );
		if ( pCHAR )
		{
			// 대련중인가?
			if ( pCHAR->GETCONFTING()->IsCONFRONTING() )
				bVALID_TAR = false;
			
            // 캐릭터가 숨김상태이고 지배 당한 상태가 아닌가? GM 은 visible off 또는 none 이 가능하다.
			// 지배를 당한 상태에서는 지배자의 IsVisibleDetect()에 영향;
			if ( !pCHAR->IsVisibleDetect(this) && !bDominated )
				bVALID_TAR = false;

			// SafeZone 일경우 공격 불가
			if ( pCHAR->IsSafeZone() ) 
				bVALID_TAR = false;

			if( pCHAR->IsNonTargetable() )
				bVALID_TAR = false;

			// 몬스터가 끝까지 추적 및 그룹타겟인 경우 몹몰이를 방지하지 않는다.
			const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();
			BOOL bGroupTarget = IsGroupTarget( m_TargetID );
			if( !pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) && !bGroupTarget && !bDominated )
			{
				// 과도한 몹몰이 방지.
				//
				// 뛰기가능한 몹인가?
				if ( bVALID_TAR && m_pCrowData->m_sAction.m_bRun )
				{
					// 현재 캐릭터 주변에 몰려있는 몹의 숫자
					DWORD dwAmount = pCHAR->GetARoundSlot().GetAmount();
					// 현재 캐릭터 주변에 몰려있는 몹의 숫자가 설정된 갯수보다 크다면...
					if ( dwAmount > GLCONST_CHAR::wMAXMOB_TRACKING )
					{
						// 몹과 타겟과의 거리를 계산한다.
						D3DXVECTOR3 vDist = m_vPosition - m_vOrgTarPos;
						float fDist = D3DXVec3Length(&vDist);
						
						// 계산된 거리가 몹 트래킹 거리보다 크다면...
						if ( fDist > GLCONST_CHAR::fMOB_TRACING )
						{
							bVALID_TAR = false;

							// 뛸수 있는 몹이라면?
							if ( m_pCrowData->m_sAction.m_bRun )
							{
								// 뛰어라
								ToRun();
							}
							
							// 생성위치로 돌아가라.
							MoveTo ( &m_vGenPos, false, 0 );
						}
					}
				}
			}
		}
	}

	if ( bCHECK_TAR && m_TargetID.emCrow==CROW_SUMMON )
	{
		PGLSUMMONFIELD pSummon = pLand->GetSummon ( m_TargetID.GaeaId );		
		PGLCHAR		   pCHAR   = NULL;

		if ( pSummon && pSummon->m_sOwnerID.actorType == CROW_PC )
		{
			pCHAR = static_cast< PGLCHAR >( m_pGaeaServer->GetActor( pLand, pSummon->m_sOwnerID ) );
		}
		//m_pGaeaServer->GetChar ( pSummon->m_dwOwnerGaeaID );

		if ( pCHAR && pSummon )
		{
			// 대련중인가?
			if ( pCHAR->GETCONFTING()->IsCONFRONTING() )	bVALID_TAR = false;

			// 소환수가 숨김상태인가?
			if ( !pSummon->IsVisibleDetect( this ) )		bVALID_TAR = false;

			// 몬스터가 끝까지 추적 및 그룹타겟인 경우 몹몰이를 방지하지 않는다.
			const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();
			BOOL bGroupTarget = IsGroupTarget( m_TargetID );
			if( !pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) && !bGroupTarget )
			{
				// 과도한 몹몰이 방지.
				//
				// 뛰기가능한 몹인가?
				if ( bVALID_TAR && m_pCrowData->m_sAction.m_bRun )
				{
					// 현재 소환수 주변에 몰려있는 몹의 숫자
					DWORD dwAmount = pSummon->GetARoundSlot().GetAmount();
					// 현재 소환수 주변에 몰려있는 몹의 숫자가 설정된 갯수보다 크다면...

					if ( dwAmount > GLCONST_SUMMON::wMaxMobTracking[ pSummon->m_emTYPE ] )
					{
						// 몹과 타겟과의 거리를 계산한다.
						D3DXVECTOR3 vDist = m_vPosition - m_vOrgTarPos;
						float fDist = D3DXVec3Length(&vDist);

						// 계산된 거리가 몹 트래킹 거리보다 크다면...
						if ( fDist > GLCONST_CHAR::fMOB_TRACING )
						{
							bVALID_TAR = false;

							// 뛸수 있는 몹이라면?
							if ( m_pCrowData->m_sAction.m_bRun )
							{
								// 뛰어라
								ToRun();
							}

							// 생성위치로 돌아가라.
							MoveTo ( &m_vGenPos, false, 0 );
						}
					}
				}
			}
		}
	}
	
	// 타겟이 서버상에서 유효하지 않거나 더이상 추적할 필요가 없는 PC 일 경우
	if ( !bCHECK_TAR || !bVALID_TAR )
	{
		m_dwAttPosSlot = GLARoundSlot::SLOT_SIZE;
		
		// 타게팅을 무효화한다.
		ResetTarget();	
		m_TargetID = STARGETID(CROW_PC,GAEAID_NULL);

		// 그룹 리더이면 전체 그룹 타겟을 초기화
		if( IsGroupLeader() && pLand->GET_GROUPTARGET( GetGroupName() ).GaeaId != EMTARGET_NULL )
		{
			const SETGROUPMEM& setGroupMember	= pLand->GET_GROUPMEMBER(GetGroupName());
			SETGROUPMEM_CITER setMember_citer	= setGroupMember.begin();
			for( ; setMember_citer != setGroupMember.end(); ++setMember_citer )
			{
				// 멤버가 아니면 넘어간다.
				if( !pLand->IS_GROUPMEMBER(GetGroupName(), *setMember_citer ) )
					continue;

				// 자신이면 넘어간다.
				if( m_dwGaeaID == *setMember_citer )
					continue;

				PGLCROW pGLCrow = pLand->GetCrow(*setMember_citer);
				if( !pGLCrow )
					continue;

				if( pGLCrow->m_TargetID == pLand->GET_GROUPTARGET( GetGroupName() ) )
					pGLCrow->ResetTarget();
			}

			pLand->RESET_GROUPTARGET( GetGroupName() );
		}
	}

	//
	//mjeon.CaptureTheField
	//
	if(m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR)
	{
		if (m_pGaeaServer->IsCTFPlaying())
		{
			for (int i=0; i<SCHOOL_NUM; i++)
			{
				if (i != m_wSchoolAuthed && m_arrHP4School[i] == 0)
				{	
					//
					//인증학원 setup 및 인증기 HP 재설정
					//
					BeAuthedBy(i);

					TurnAction(GLAT_AUTHED);

					break;
				}
			}
		}

		//
		//인증 여부에 관계없이 아래의 사항들은 항상 update되어야 한다.
		//		
		GLCROWLOGIC::UPDATE_DATA ( fTime, fElapsedTime );
		UpdateDurDamage ( fTime, fElapsedTime );

		//goto crow_framemove_end;	//인증기의 경우 더 이상의 액션 처리는 필요없다.
	}


	// 눕지 않았으면
	if ( IsValidBody() )
	{
        if ( m_dwNowHP <= 0 )
		{
			DoFalling();	
		}
		// 타겟이 무효화 되었는데 공격액션 중이면
		else
        {
            if ( !isAction(GLAT_SHOCK) && !isAction(GLAT_PUSHPULL) )
            {
                if ( !IsFLAG(EMTARGET) && (isAction(GLAT_ATTACK) || isAction(GLAT_SKILL)) )
                {
                    // 유휴상태로 변경
                    TurnAction ( GLAT_IDLE, false );
                }
                if ( !m_bSTATE_STUN && !bDominated )				
                    RunSchedule ( fTime, fElapsedTime);	// 스케쥴 실행.;
            }
            // Crow 의 현재 상태값들을 Update 한다;
            GLCROWLOGIC::UPDATE_DATA ( fTime, fElapsedTime );
            UpdateDurDamage ( fTime, fElapsedTime );	
        }
	}


	if (m_pCrowData->m_emNPCType != NPC_TYPE_AUTHENTICATOR)
	{
		UpdateActState();
		FrameMoveAction( fElapsedTime );

		UpdateSkillEff( fTime, fElapsedTime );
	}
	//goto crow_framemove_end;

	// Mob의 현제 위치 업데이트.
	//
	m_vPosition = m_actorMove.Position();

	
	// m_pd3dDevice 가 유효할 경우만 아랫 코드가 실행된다. ( Level Editor 에서 태스트용. )
	//
	if ( !m_pd3dDevice )
		return S_OK;

	
	// 에니메이션, 스킨 업데이트.
	//
	UpateAnimation( fTime, fElapsedTime );


	// 현제 위치와 방향으로 Transform 메트릭스 계산.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPosition.x, m_vPosition.y + m_pMobSchedule->m_fInfluenceY, m_vPosition.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDirect, vDIRORIG );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	m_matTrans = matYRot * matTrans;

	return S_OK;
}

HRESULT GLCrow::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	
		return S_OK;

	if ( bRendAABB )
	{
		GLMobSchedule* pMobSch = GetMobSchedule();
		float fActivityArea = 0;
		if( pMobSch )
			fActivityArea = pMobSch->m_fActivityArea;

		D3DXMATRIX mat;
		D3DXMatrixRotationX( &mat, D3DX_PI*0.5 );

		EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );	//	테스트용.

		if( 0 < fActivityArea || true )
		{
			EDITMESHS::RENDERCYLINDER( GetPositionActor(), 
				static_cast<float>( fActivityArea ),
				static_cast<float>( 5 ),
				0xFFFF0000, 
				&mat );
		}

	}
	
	DxSkinChar* pRenderChar = m_pSkinChar;
	//if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
	//	pRenderChar = m_pSkinChar;

	if ( pRenderChar )
	{
		//if ( FAILED(m_pSkinChar->Render ( pd3dDevice, TRUE, 1.f, m_matTrans )) )
		//	return E_FAIL;

		//////////////////////////////////////////////////////////////////////////
		//
		//	이곳에 온다는건 Emulater를 돌린다는 거고 그건 개발팀에서 작업한다는거임.
		//	저사양은 생각 안해도 됨.
		//
		//if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		//{
		//	m_pSkinChar->Render( pd3dDevice, TRUE, 1.f, matTrans );
		//}
		//else
		{
			m_pSkinChar->SetPhysX_Cloth( TRUE );
			m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
			m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
			m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
			m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 1.f );
		}
	}

	return S_OK;
}

void GLCrow::SendPushPull( const D3DXVECTOR3 &vMovePos, const float fPUSH_SPEED, const DWORD dwAniType, const bool bReActor /*= true*/ )
{
	if ( !IsValidBody () )
		return;
	if ( m_pCrowData->IsPosHold() )	
		return;

	// 밀려날 위치로 이동 시도.
	//
	const BOOL bSucceed = m_actorMove.GotoLocation
	(
		D3DXVECTOR3(vMovePos.x,vMovePos.y+5,vMovePos.z),
		D3DXVECTOR3(vMovePos.x,vMovePos.y-5,vMovePos.z)
	);

	if ( bSucceed )
	{
		// 밀려나는 엑션 시작.
		//
		m_TargetID.vPos = vMovePos;
		TurnAction ( GLAT_PUSHPULL );

		// 밀리는 속도 설정.
		//
		m_actorMove.SetMaxSpeed ( fPUSH_SPEED );

		// 메시지 내용 설정.
		//
		GLMSG::SNET_PUSHPULL_BRD NetMsgBRD;
		NetMsgBRD.emCrow	= GETCROW();
		NetMsgBRD.dwID		= m_dwGaeaID;
		NetMsgBRD.vMovePos		= vMovePos;
		NetMsgBRD.fPUSH_SPEED	= fPUSH_SPEED;
		NetMsgBRD.dwAniType		= dwAniType;

		// [자신의 주변 Char] 에게 Msg를 전달.
		//
		SendMsgViewAround(&NetMsgBRD);
	}
}

// 피격 데미지 함수 통합 [9/26/2013 hsshin];
const DWORD GLCrow::ReceiveDamage ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    return ReceiveDamage_Procedure(emACrow, dwAID, dwDamage, dwDamageFlag);
}
const DWORD GLCrow::ReceiveDamage_Procedure ( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    PGLCHAR	pChar			= NULL;
    PGLSUMMONFIELD pSummon	= NULL;
    WORD wSchool			= 0;

	m_sAssault.emCrow	= emACrow;
	m_sAssault.GaeaId   = dwAID;

	// 인던 Script 에서의 Receive Damage 처리;
	DWORD dwActualDamage = dwDamage;
	InstanceSystem::InstanceField* const pInstance =
		pLand->GetScriptInstance();
	if ( NULL != pInstance )  // 인던이 아닐 경우 NULL;
	{
		// dwID는 플레이어일경우 DBNUM, 몹,Summon등은 Land내에서 유효한 GloabalID를 사용;
		DWORD dwID = m_sAssault.GaeaId;
		if ( m_sAssault.emCrow == CROW_PC )
		{
			GLChar* pChar = m_pGaeaServer->GetChar( m_sAssault.GaeaId );
			if ( pChar )
				dwID = pChar->CharDbNum();
		}

		if ( CROW_SUMMON == m_sAssault.emCrow )
		{
			GLSummonField* pSummon =
				pLand->GetSummon( m_sAssault.GaeaId );
			if ( pSummon )
			{
				ServerActor* pOwner = pSummon->GetOwner();
				if ( pOwner )
				{
					GLChar* pChar = static_cast< GLChar* >( pOwner );
					if ( pChar )
						dwID = pChar->CharDbNum();
				}
			}
		}

		// 주의! : dwDamage는 reference로 인자를 넘기기때문에 값이 변경될수 있다. ( 스크립트에서 데미지값 변경을 할 수 있게 하기 위함 );
		if ( false == pInstance->EventReceiveDamage(
			CROW_MOB,
			GetGaeaID(),
			emACrow,
			dwID,
			dwActualDamage,
			dwDamageFlag ) )  // return값이 false일 경우 기존의 데미지 처리를 하지 않겠다는 뜻;
		{
			return dwActualDamage;
		}
	}

    //
    //mjeon.CaptureTheField
    //
    if ( m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR )
        return ReceiveDamage_Procedure_For_CTF(emACrow, dwAID, dwDamage, dwDamageFlag);

    // Damage 적용.
    const DWORD dwDxHp = RECEIVE_DAMAGE ( dwDamage );

	// Damage 값이 있을때. 0 일때는 miss 발생 ( 타겟 지정을 위해 호출됨. )
	if ( dwDxHp > 0 )
	{
		//	Damage 기록.
		if ( emACrow == CROW_PC )
		{
			pChar = m_pGaeaServer->GetChar ( dwAID );

			if (pChar)
			{
				AddDamageLog      ( m_cDamageLog, dwAID, pChar->GetUserID(), dwDxHp );
				AddDamageLogParty ( m_cDamageLogParty, pChar->GetPartyID(), dwDxHp );
                pChar->ToAllDamage_PVE(dwDxHp);
			}
		}
		// Note : 소환수 공격일때 m_cSummonDamageLog에 기록한다
		else if ( emACrow == CROW_SUMMON )
		{
			pSummon = pLand->GetSummon(dwAID);			
			if (pSummon)
			{
				if ( pSummon->m_sOwnerID.actorType == CROW_PC )
				{
					GLChar* pSummonOwner = static_cast<GLChar*>(
						m_pGaeaServer->GetActor( pLand, pSummon->m_sOwnerID ) );
					if (pSummonOwner)
					{
						AddDamageLog(
							m_cSummonDamageLog,
							pSummonOwner->GetGaeaID(),
							pSummonOwner->GetUserID(),
							dwDxHp);
						AddDamageLogParty(
							m_cDamageLogParty,
							pSummonOwner->GetPartyID(),
							dwDxHp);

						pSummonOwner->ToAllDamage_PVE(dwDxHp);
					}
				}
			}
		}

		// 충격 받음.
		if( !m_pCrowData->IsIgnoreShock() )
		{
			if ( dwDamageFlag & DAMAGE_TYPE_SHOCK )	
			{
				TurnAction ( GLAT_SHOCK );
			}
		}
	}

	if ( m_dwNowHP > 0 )
	{
		Update_AccumulateHit( dwDxHp, emACrow, dwAID );

		Faction::GLActorID sAttackerID( emACrow, dwAID );
		ServerActor* pAttacker =
			m_pGaeaServer->GetActor( pLand, sAttackerID );
		if ( pAttacker )
			SendToChildCrowAttacked( pAttacker );
	}

	if( !RF_PROVOKE( m_EffSkillVarSet ).IsOn() )
	{
		// AI : 피격에 의한 타겟 변경 AI호출;
		SetTargeting(STARGETID(emACrow,dwAID));
	}

	return dwDxHp;
}

const DWORD GLCrow::ReceiveDamage_Procedure_For_CTF( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    //  기존 코드를 가급적 건드리지 않기 위해, 별도의 블럭으로 CTF 데미지 코드를 작성한다.
    //
    STARGETID sTargetID;
    PGLCHAR	pChar			= NULL;
    PGLSUMMONFIELD pSummon	= NULL;
    WORD wSchool			= 0;
    DWORD dwDxHp			= dwDamage;
    //
    //getting Attacker first
    //
    if ( emACrow == CROW_PC )
    {
        pChar     = m_pGaeaServer->GetChar ( dwAID );
        sTargetID = STARGETID(pChar->GETCROW(), pChar->GetGaeaID(), pChar->GetPosition());
    }			
    else if ( emACrow == CROW_SUMMON )
    {
        pSummon   = pLand->GetSummon ( dwAID );
        if (pSummon)
        {
            sTargetID = STARGETID(pSummon->GETCROW(), pSummon->GetGaeaID(), pSummon->GetPosition());
			if ( pSummon->m_sOwnerID.actorType == CROW_PC )
			{
				pChar = static_cast< GLChar* >( m_pGaeaServer->GetActor(
					pLand, pSummon->m_sOwnerID ) );
			}
        }
    }

    if (pChar)
    {	
        wSchool = pChar->m_wSchool;
        dwDxHp = RECEIVE_DAMAGE_BY_SCHOOL(dwDamage, wSchool);

        if ( emACrow == CROW_PC )
        {				
            if (pChar)
            {
                AddDamageLog      ( m_cDamageLog, dwAID, pChar->GetUserID(), dwDxHp );
                AddDamageLogParty ( m_cDamageLogParty, pChar->GetPartyID(), dwDxHp );
            }
            else
            {
                sc::writeLogError(std::string("GLCrow::ReceiveDamage() - Character does not exist."));
            }
        }
        // Note : 소환수 공격일때 m_cSummonDamageLog에 기록한다
        else if ( emACrow == CROW_SUMMON )
        {
            if (pSummon && pChar)
            {
				if ( pSummon->m_sOwnerID.actorType == CROW_PC )
				{
					GLChar* pSummonOwner = static_cast< GLChar* >(
						m_pGaeaServer->GetActor(
						pLand,
						pSummon->m_sOwnerID ) );
					if (pSummonOwner)
					{
						AddDamageLog(
							m_cSummonDamageLog,
							pSummonOwner->GetGaeaID(),
							pSummonOwner->GetUserID(),
							dwDxHp);
						AddDamageLogParty(
							m_cDamageLogParty,
							pSummonOwner->GetPartyID(),
							dwDxHp);
					}
				}   
            }
            else
            {
                sc::writeLogError(std::string("GLCrow::ReceiveDamage() - CROW_SUMMON does not exist."));
            }
        }

        if ( dwDamageFlag != DAMAGE_TYPE_GATHER )
        {
            pChar->CTFDamageTo(dwDxHp);
        }  
    }
    else
    {
        sc::writeLogError(sc::string::format("GLCrow::ReceiveDamage() - Character(%1%) does not exist.", dwAID));
    }	

	// 상태 전환의 목적이아니라, 제어기 정보가 변경될 시 다른 클라이언트와 동기화를 마추어주기위해 콜함;
	TurnActionAuthed();

	// 기존 패킷 전송부분은 삭제;
	// 호출하는 부분에서 보내주는 구간이 있다;

    return dwDxHp;	//CTF 인증기의 경우 무조건 이 블럭내의 처리만 수행한 후 탈출하게 된다.
    // 기존 코드를 가급적 건드리지 않기 위해, 별도의 블럭으로 CTF 데미지 코드를 작성한다.
}

// 회복 함수 통합 [9/26/2013 hsshin];
const DWORD GLCrow::ReceiveHeal(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    DWORD dwActualHeal(dwHeal);
    InstanceSystem::InstanceField* const _pInstance(pLand->GetScriptInstance());
    if ( _pInstance != NULL )  // 인던이 아닐 경우 NULL;
    {
        // dwID는 플레이어일경우 DBNUM, 몹,Summon등은 Land내에서 유효한 GloabalID를 사용;
        DWORD dwID(m_sAssault.GaeaId);
        if ( m_sAssault.emCrow == CROW_PC )
        {
            GLChar* pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
            if ( pChar )
                dwID = pChar->CharDbNum();
        }

        // 주의! : dwHeal는 reference로 인자를 넘기기때문에 값이 변경될수 있다. ( 스크립트에서 회복값 변경을 할 수 있게 하기 위함 );
        if ( _pInstance->EventReceiveHeal(emACrow, dwID, CROW_MOB, GetGaeaID(), dwActualHeal, dwHealFlag) == false )  // return값이 false일 경우 기존의 회복 처리를 하지 않겠다는 뜻;
            return dwActualHeal;
    }

    return ReceiveHeal_Procedure(emACrow, dwAID, dwActualHeal, dwHealFlag);
}
const DWORD GLCrow::ReceiveHeal_Procedure(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

	m_sHeal.emCrow = emACrow;
	m_sHeal.GaeaId = dwAID;

    const DWORD wDxHP = GLCROWLOGIC::RECEIVE_HEAL( (WORD) dwHeal );	

    if ( emACrow == CROW_PC )
    {
        GLChar* pChar = m_pGaeaServer->GetChar(dwAID);
        //
        //mjeon.CaptureTheField
        //
        if ( dwHealFlag != HEAL_TYPE_GATHER && pChar->IsCTFPlayer() )
        {
            pChar->CTFHealTo(wDxHP);
        }
        pChar->ToAllHeal_PVE(wDxHP);
    }
    else if ( emACrow == CROW_SUMMON )
    {        
        // Note : 주인이 친걸로 등록
        //
        PGLSUMMONFIELD pSummon = pLand->GetSummon(dwAID);
        if (pSummon)
        {
			if ( pSummon->m_sOwnerID.actorType == CROW_PC )
			{
				GLChar* pSummonOwner = static_cast< GLChar* >(
					m_pGaeaServer->GetActor(
					pLand,
					pSummon->m_sOwnerID ) );
				if (pSummonOwner)
				{
					//
					//mjeon.CaptureTheField
					//
					if ( dwHealFlag != HEAL_TYPE_GATHER && pSummonOwner->IsCTFPlayer() )
					{
						pSummonOwner->CTFHealTo(wDxHP);
					}
					pSummonOwner->ToAllHeal_PVE(wDxHP);
				}
			}
        }
    }

	return wDxHP;
}


const bool GLCrow::CheckHit(const STARGETID &cTargetID)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return false;

	ServerActor* const pTargetActor = GetTarget(cTargetID);
	if ( pTargetActor == NULL )
		return false;
    
    const EMBRIGHT emBright = pTargetActor->GetBright();
    const EM_BRIGHT_FB bFB = GLOGICEX::GLSPACEGAP(GETBRIGHT(), emBright, pLand->GETBRIGHT());
	const int nAVOID = pTargetActor->GetAvoid();

    const int nHitRate = GLOGICEX::GLHITRATE(GetHitRate(), nAVOID, bFB);

    return (nHitRate > (sc::Random::RANDOM_POS()*100));
}

DWORD GLCrow::CALCDAMAGE(
    int& rResultDAMAGE, const DWORD dwGaeaID, const STARGETID &cTargetID, const GLLandMan* pLandMan,
    const GLSKILL* pSkill, DWORD dwskill_lev, DWORD dwWeatherFlag, DWORD dwDivCount)
{
    ServerActor* pActor = m_pGaeaServer->GetTarget(pLandMan, cTargetID);
    if (!pActor)
        return FALSE;

    DAMAGE_SPEC*	pDamageSpec = pActor->GetDamageSpecPtr();
	if( pDamageSpec->m_dwFlags & DAMAGE_SPEC::FLAG_ILLUSION )
	{
		if( pDamageSpec->m_wIllusionRemain > 0 )
		{
			float		fElapsedTime = GetCurrentFrameTime() - pDamageSpec->m_fIllusionLastTime;
			if( fElapsedTime > GLCONST_CHAR::fIllusionInterval )
			{
				pDamageSpec->m_fIllusionLastTime = GetCurrentFrameTime();
				pDamageSpec->m_wIllusionRemain--;
				return DAMAGE_TYPE_ILLUSION;
			} //if
		} //if
	} //if

    int nDEFENSE = pActor->GetDefense ();
    int nDEFAULT_DEFENSE = pActor->GetBodyDefense ();
    int nITEM_DEFENSE = pActor->GetItemDefense ();

    int nLEVEL = pActor->GetLevel ();
    float fSTATE_DAMAGE = pActor->GETSTATE_DAMAGE ();
    const SRESIST &sRESIST = pActor->GETRESIST ();

	const WORD wGRADE(GetGradeAttack());

    int nDamageLow(GetOffenceLow());
	int nDamageHigh(GetOffenceHigh());

    DWORD dwDamageFlag = DAMAGE_TYPE_NONE;
    bool bShock = false;
    bool bCrushingBlow = false;
    bool bCritical = false; 
    bool bPsyDamage = true;

    float fDamageReduce = pDamageSpec->m_fPsyDamageReduce; // 효과 비율 적용. by luxes.
    float fDamageReflection = pDamageSpec->m_fPsyDamageReflection * pDamageSpec->m_fPsyDamageReflectionEffRate; // 효과 비율 적용. by luxes.
    float fDamageReflectionRate = pDamageSpec->m_fPsyDamageReflectionRate * pDamageSpec->m_fPsyDamageReflectionRateEffRate; // 효과 비율 적용. by luxes.
    float fDamageCurse = m_sDamageSpec.m_fDamageCurse * m_sDamageSpec.m_fDamageCurseEffRate; // 효과 비율 적용. by luxes.

    DEFENSE_SKILL sDefenseSkill = pActor->GetDefenseSkill();

	DWORD dwApplyFlag = 0;
    if ( pSkill )
    {
        const SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[dwskill_lev];
		if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneActionType, pSkill->m_sBASIC.emACTION_TYPE) ||
			CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, pSkill->m_sBASIC.emAPPLY))
		{
			rResultDAMAGE = 0;
			return DAMAGE_TYPE_IMMUNE;
		}

		dwApplyFlag = pSkill->m_sBASIC.emAPPLY;
        float _fRESIST_G = 0;
        switch ( pSkill->m_sBASIC.emAPPLY )
        {
            //	근접 공격.
        case SKILL::EMAPPLY_MELEE:
            _fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
            break;

            //	장거리 공격.
        case SKILL::EMAPPLY_RANGE:
            _fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;

            fDamageReflection = 0.0f;
            fDamageReflectionRate = 0.0f;
            break;

            //	마법 공격.
        case SKILL::EMAPPLY_MAGIC:
            nDEFENSE = 0;						//	마법 공격일때는 일반 방어력 무시됨.
            nDEFAULT_DEFENSE = 0;				//	마법 공격일때는 일반 방어력 무시됨.
            nITEM_DEFENSE = 0;					//	마법 공격일때는 일반 방어력 무시됨.
            _fRESIST_G = GLCONST_CHAR::fRESIST_G;

            fDamageReduce = pDamageSpec->m_fMagicDamageReduce * pDamageSpec->m_fMagicDamageReduceEffRate; // 효과 비율 적용. by luxes.
            fDamageReflection = pDamageSpec->m_fMagicDamageReflection * pDamageSpec->m_fMagicDamageReflectionEffRate; // 효과 비율 적용. by luxes.
            fDamageReflectionRate = pDamageSpec->m_fMagicDamageReflectionRate * pDamageSpec->m_fMagicDamageReflectionRateEffRate; // 효과 비율 적용. by luxes.
            bPsyDamage = false;
            break;
        };

		EMELEMENT emELMT(pSkill->m_sAPPLY.emELEMENT);

        short nRESIST = sRESIST.GetElement ( emELMT );        

		int nVAR(::abs(int(sSKILL_DATA.fBASIC_VAR)));

		{ // 몹 등급에 대한 반영율;
			float fGrade(float(wGRADE) / GLCONST_CHAR::fDAMAGE_GRADE_K);
			// 최소/최대 데미지 =  스킬데미지 + (최소/최대 데미지) * 반영율
			nDamageLow  += int(float(nDamageLow) * fGrade + nVAR);
			nDamageHigh += int(float(nDamageHigh) * fGrade + nVAR);
		}

		float fResistTotal(float(nRESIST) * 0.01f * _fRESIST_G);
		fResistTotal = fResistTotal > GLCONST_CHAR::fRESIST_MAX_CROW ? GLCONST_CHAR::fRESIST_MAX_CROW : fResistTotal;		

		// 최소/최대 데미지 = 데미지-(데미지*전체저항값)
		nDamageLow  -= int(float(nDamageLow)  * fResistTotal);
		nDamageHigh -= int(float(nDamageHigh) * fResistTotal);

		// 데미지가 0 보다 작으면 0 으로 고정한다.
		if ( nDamageLow < 0 )
			nDamageLow  = 0;
		if ( nDamageHigh < 0 )
			nDamageHigh = 0;
    }
	else
	{
		if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, SKILL::EMAPPLY_MELEE) )
		{				
			rResultDAMAGE = 0;
			return DAMAGE_TYPE_IMMUNE;
		}
		dwApplyFlag = SKILL::EMAPPLY_DWORD_MELEE;
	}

	SINCREASEEFF* const pIncreaseEff = pActor->GetIncreaseEff();

	// Critical 발생 확율	
	float _fDamageRate = m_fDamageRate;

	if ( pIncreaseEff )
	{
		if ( pIncreaseEff->dwApplyFlag & dwApplyFlag ) // 강화 효과
		{	
//			const int nIncreaseCrushingBlow = pIncreaseEff->GetIncreaseCrushingBlowRate(dwApplyFlag); // 몹은 강타가 없는듯;
			const float fIncreaseRate = float(pIncreaseEff->GetIncreaseDamageRate(dwApplyFlag)) * 0.01f;
			_fDamageRate += fIncreaseRate;
		}
	}

    // 공격력. 변화율 반영
    m_pGaeaServer->AdjuctDamage(CROW_MOB, nDamageLow, nDamageHigh, _fDamageRate, 0.0f);

    // 추가 보정치 산출.
    int nExtFORCE = 0;
    int ndxLvl = 0;    
    m_pGaeaServer->AdjustLevel(ndxLvl, nExtFORCE, nLEVEL, GETLEVEL());

    // Critical 발생 확율.
    bCritical = m_pGaeaServer->CalcCritical(CROW_MOB, GETHP(), GetMaxHP(), ndxLvl);
    
    // Damage 최대 최소값 사이의 랜덤 Damage 결정
    int nDAMAGE_OLD = m_pGaeaServer->CalcRandomDamage(nDamageLow, nDamageHigh, nExtFORCE);

	SIGNORED_DAMAGE* const pIgnoredDamage = pActor->GetIgnoredDamage();
	if ( (pIgnoredDamage != NULL) && (pIgnoredDamage->isAvailable() == true) )
	{
		if ( pIgnoredDamage->AbsorbDamage(nDAMAGE_OLD) == false )
			pActor->SKILL_EFFECT_REMOVE(EMSPECA_IGNORED_DAMAGE);

		if ( nDAMAGE_OLD == 0 )
			return DAMAGE_TYPE_NONE;
	}

    // 최저 수용 Damage 산출
    rResultDAMAGE = m_pGaeaServer->CalcLowDamage(CROW_MOB, nDAMAGE_OLD, nDEFENSE, fSTATE_DAMAGE);
	

    // 방어구 Damage 흡수율 반영
    rResultDAMAGE = m_pGaeaServer->CalcDefenseRate(CROW_MOB, nDEFAULT_DEFENSE, nITEM_DEFENSE, rResultDAMAGE, GetLevel());

    rResultDAMAGE = m_pGaeaServer->CalcCriticalDamage(bCritical, false, rResultDAMAGE);

    if (rResultDAMAGE <= 1)
    {
        bCritical = false;
        rResultDAMAGE = 1;
    }

    if (dwDivCount > 1)
        rResultDAMAGE /= dwDivCount;

    // 발동 스킬
    if (sDefenseSkill.m_dwSkillID != NATIVEID_NULL())
    {
        if (sDefenseSkill.m_fRate * sDefenseSkill.m_fRateEffRate > (sc::Random::RANDOM_POS()*1))
        {
            STARGETID sActor(GETCROW(),dwGaeaID);
            pActor->DefenseSkill(sDefenseSkill.m_dwSkillID, sDefenseSkill.m_wLevel, sActor);
        }
    }

    //	데미지 흡수
    if (fDamageReduce > 0.0f) 
    {
        int nDamageReduce = (int) (((rResultDAMAGE * fDamageReduce) * nLEVEL) / GLCONST_CHAR::wMAX_LEVEL);
        rResultDAMAGE -= nDamageReduce;

        if (bPsyDamage)
            dwDamageFlag += DAMAGE_TYPE_PSY_REDUCE;
        else
            dwDamageFlag += DAMAGE_TYPE_MAGIC_REDUCE;
    }

    //	데미지 반사 계산
    if (fDamageReflectionRate > 0.0f)
    {
        if (fDamageReflectionRate > (sc::Random::RANDOM_POS()*1))
        {
            int nDamageReflection = (int) (((rResultDAMAGE * fDamageReflection) * nLEVEL) / GLCONST_CHAR::wMAX_LEVEL);

            if (nDamageReflection > 0) 
            {
                if (bPsyDamage)
                    dwDamageFlag += DAMAGE_TYPE_PSY_REFLECTION;
                else
                    dwDamageFlag += DAMAGE_TYPE_MAGIC_REFLECTION;

                // 피격자 입장에서 데미지를 다시 보낸다.
                STARGETID sActor(GETCROW(),dwGaeaID);
                pActor->DamageReflectionProc(nDamageReflection, sActor);
            }
        }
    }

    //	저주 계산
    if (fDamageCurse > 0.0f) 
    {
        int nDamageCurse = (int) (rResultDAMAGE * fDamageCurse);
        if (nDamageCurse > 0) 
        {
            STARGETID sActor(GETCROW(),dwGaeaID);
            ServerActor *pMyActor = m_pGaeaServer->GetTarget(pLandMan, sActor);
            if (!pMyActor)
                return FALSE;

            STARGETID sCurseTargetID = m_sDamageSpec.m_sCurseTargetID;			
            pMyActor->DamageCurse(nDamageCurse, sCurseTargetID, cTargetID.emCrow);
        }
    }

    bShock = (0!=GLOGICEX::CHECKSHOCK(GETLEVEL(), nLEVEL, rResultDAMAGE, bCritical));

    if (bShock)
        dwDamageFlag += DAMAGE_TYPE_SHOCK;
    if (bCritical)
        dwDamageFlag += DAMAGE_TYPE_CRITICAL;

    rResultDAMAGE = int(rResultDAMAGE * GetMultipleAttack());
	
	if ( m_pCrowData->IsBoss() )
	{
		// EMITEM_VAR::EMVAR_IGNORE_BOSSDAMAGE
		// 마지막으로 연산 되어야한다;
		const float fIgnore_BossDamage(pActor->GetIgnoreBossDamageR());
		if ( fIgnore_BossDamage != 0.0f )
			rResultDAMAGE = rResultDAMAGE - (rResultDAMAGE * fIgnore_BossDamage);
	}

	// 캐릭터가 위급상태일 경우 피해감소 효과 체크후 감소시킨다;
	if ( pActor->IsActState(EM_ACT_EMERGENCY) )
	{
		const float fDecRDamage(pActor->GetDecRDamage_EmergencyState());
		rResultDAMAGE -= ((float)rResultDAMAGE * fDecRDamage);
	}

	if ( rResultDAMAGE < 0 )
		rResultDAMAGE = 0;

    return dwDamageFlag;
}

bool GLCrow::IsBasicTalk(int nAction) const
{
    if (m_pCrowData)
        return m_pCrowData->IsBasicTalk(nAction);
    else
        return false;
}

ServerActor* GLCrow::GetTarget(const STARGETID& sTargetID)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return NULL;

	return m_pGaeaServer->GetTarget(pLand, sTargetID); 
}

ServerActor* GLCrow::GetTarget(const GLLandMan* pLandMan, const STARGETID& sTargetID)
{
	return m_pGaeaServer->GetTarget(pLandMan, sTargetID); 
}

std::tr1::shared_ptr<SNpcTalk> GLCrow::GetTalk(DWORD dwTalkID) const
{
    if (m_pCrowData)
    {
        return m_pCrowData->GetTalk(dwTalkID);
    }
    else
    {
        static std::tr1::shared_ptr<SNpcTalk> NullPtr;
        return NullPtr;
    }
}

void GLCrow::Reset4CTF()
{
	//지속데미지 초기화
	for ( int i = 0; i < EMELEMENT_MAXNUM; ++i ) 
		m_sDurDamage[i].RESET();

	INIT_DATA();
}

void GLCrow::SetMultipleHP(float fMultiple)
{    
    GLCROWLOGIC::SetMultipleHP(fMultiple);

	GLMSG::SNETCROW_UPDATE_STATE NetMsgBrd( GetGaeaID(), m_dwNowHP );
	NetMsgBrd.emStatusType = GLMSG::SNETCROW_UPDATE_STATE::EMCHANGESTATUS_TYPE_HP_MULTIPLE;
	NetMsgBrd.fStatusValue = GLCROWLOGIC::GetMultipleHP();
	SendMsgViewAround( &NetMsgBrd );
}

const float GLCrow::GetMultipleHP() const
{
    return GLCROWLOGIC::GetMultipleHP();
}

void GLCrow::SetMultipleAttack(float fMultiple)
{
    m_fMultipleAttack = fMultiple;
}

const float GLCrow::GetMultipleAttack() const
{
    return m_fMultipleAttack;
}

void GLCrow::SetMultipleDefense(float fMultiple)
{
    m_fMultipleDefense = fMultiple;
}

const float GLCrow::GetMultipleDefense() const
{
    return m_fMultipleDefense;
}

void GLCrow::SetMultipleExp(float fMultiple)
{
    m_fMultipleExp = fMultiple;
}

const float GLCrow::GetMultipleExp() const
{
    return m_fMultipleExp;
}

void GLCrow::SetMultipleDrop(float fMultiple)
{
	m_fMultipleDrop = fMultiple;
}

const float GLCrow::GetMultipleDrop() const
{
	return m_fMultipleDrop;
}

void GLCrow::SetOffence ( int _nOffence )
{
	if ( GetBasicOffenceHigh() + _nOffence >= MAXOFFENCE )
		_nOffence = MAXOFFENCE - m_nOffence;
	else if ( GetBasicOffenceHigh() + _nOffence <= 0)
		_nOffence = -1*m_nOffence;

	m_nOffence = _nOffence;
}

void GLCrow::SetDefence ( int _nDefence )
{
	if ( GetBasicDefence() + _nDefence >= MAXDEFENCE )
		_nDefence = MAXDEFENCE - m_nDefence;
	else if ( GetBasicDefence() + _nDefence <= 0)
		_nDefence = -1*m_nDefence;

	m_nDefence = _nDefence;
}

void GLCrow::SetFireRegist ( int _nFireRegist )
{
	short nRegist = GetBasicRegist().GetElement( EMELEMENT_FIRE );
	if ( nRegist + _nFireRegist >= MAXREGIST )
		_nFireRegist = MAXREGIST - m_nFireRegist;
	else if ( nRegist + _nFireRegist <= 0)
		_nFireRegist = -1*m_nFireRegist;

	m_nFireRegist = _nFireRegist;
}

void GLCrow::SetIceRegist ( int _nIceRegist )
{
	short nRegist = GetBasicRegist().GetElement( EMELEMENT_ICE );
	if ( nRegist + _nIceRegist >= MAXREGIST )
		_nIceRegist = MAXREGIST - m_nIceRegist;
	else if ( nRegist + _nIceRegist <= 0)
		_nIceRegist = -1*m_nIceRegist;

	m_nIceRegist = _nIceRegist;
}

void GLCrow::SetElectricRegist ( int _nElectricRegist )
{
	short nRegist = GetBasicRegist().GetElement( EMELEMENT_ELECTRIC );
	if ( nRegist + _nElectricRegist >= MAXREGIST )
		_nElectricRegist = MAXREGIST - m_nElectricRegist;
	else if ( nRegist + _nElectricRegist <= 0)
		_nElectricRegist = -1*m_nElectricRegist;

	m_nElectricRegist = _nElectricRegist;
}

void GLCrow::SetPoisonRegist ( int _nPoisonRegist )
{
	short nRegist = GetBasicRegist().GetElement( EMELEMENT_POISON );
	if ( nRegist + _nPoisonRegist >= MAXREGIST )
		_nPoisonRegist = MAXREGIST - m_nPoisonRegist;
	else if ( nRegist + _nPoisonRegist <= 0)
		_nPoisonRegist = -1*m_nPoisonRegist;

	m_nPoisonRegist = _nPoisonRegist;
}

void GLCrow::SetSpiritRegist ( int _nSpiritRegist )
{
	short nRegist = GetBasicRegist().GetElement( EMELEMENT_SPIRIT );
	if ( nRegist + _nSpiritRegist >= MAXREGIST )
		_nSpiritRegist = MAXREGIST - m_nSpiritRegist;
	else if ( nRegist + _nSpiritRegist <= 0)
		_nSpiritRegist = -1*m_nSpiritRegist;

	m_nSpiritRegist = _nSpiritRegist;
}

void GLCrow::SetAccuracyRate ( int _nAccuracyRate )
{
	if ( GetBasicHitRate() + _nAccuracyRate >= MAXACCURACY )
		_nAccuracyRate = MAXACCURACY - m_nAccuracyRate;
	else if ( GetBasicHitRate() + _nAccuracyRate <= 0)
		_nAccuracyRate = -1*m_nAccuracyRate;

	m_nAccuracyRate = _nAccuracyRate;
}

void GLCrow::SetAvoidanceRate ( int _nAvoidanceRate )
{
	if ( GetBasicAvoid() + _nAvoidanceRate >= MAXACCURACY )
		_nAvoidanceRate = MAXAVOIDANCE - m_nAvoidanceRate;
	else if ( GetBasicAvoid() + _nAvoidanceRate <= 0)
		_nAvoidanceRate = -1*m_nAvoidanceRate;

	m_nAvoidanceRate = _nAvoidanceRate;
}

void GLCrow::SetHPRestorationRate ( float _fHPRestorationRate )
{
	SetHPRestorationRatePlus( _fHPRestorationRate );

	GLMSG::SNETCROW_UPDATE_STATE NetMsgBrd( GetGaeaID(), m_dwNowHP );
	NetMsgBrd.emStatusType = GLMSG::SNETCROW_UPDATE_STATE::EMCHANGESTATUS_TYPE_HPRESTORATION_RATE;
	NetMsgBrd.fStatusValue = GetHPRestorationRatePlus();
	SendMsgViewAround( &NetMsgBrd );
}

void GLCrow::SetMPRestorationRate ( float _fMPRestorationRate )
{
	SetMPRestorationRatePlus( _fMPRestorationRate );

	GLMSG::SNETCROW_UPDATE_STATE NetMsgBrd( GetGaeaID(), m_dwNowHP );
	NetMsgBrd.emStatusType = GLMSG::SNETCROW_UPDATE_STATE::EMCHANGESTATUS_TYPE_MPRESTORATION_RATE;
	NetMsgBrd.fStatusValue = GetMPRestorationRatePlus();
	SendMsgViewAround( &NetMsgBrd );
}

void GLCrow::SetSPRestorationRate ( float _fSPRestorationRate )
{
	SetSPRestorationRatePlus( _fSPRestorationRate );

	GLMSG::SNETCROW_UPDATE_STATE NetMsgBrd( GetGaeaID(), m_dwNowHP );
	NetMsgBrd.emStatusType = GLMSG::SNETCROW_UPDATE_STATE::EMCHANGESTATUS_TYPE_SPRESTORATION_RATE;
	NetMsgBrd.fStatusValue = GetSPRestorationRatePlus();
	SendMsgViewAround( &NetMsgBrd );
}

DWORD GLCrow::GetMaxHP() const
{
    return GETMAXHP();
}

DWORD GLCrow::GetBonusExp() const
{
    return DWORD(m_pCrowData->m_wBonusExp * GLCONST_CHAR::GET_EXP_SCALE() * GetMultipleExp());
}

const BOOL GLCrow::IsNoAllPenalty( const unsigned int uiPenaltyType ) const
{
	if( !m_pCrowData )
		return FALSE;

	if ( !m_pCrowData->m_wResist_Probability || 
		 m_pCrowData->m_wResist_Probability < sc::Random::getInstance().GetPercent() )
		return FALSE;

	// 모든 저항 옵션 체크;
	if ( m_pCrowData->m_bNoAllPenalty == TRUE )
		return TRUE;

	switch ( uiPenaltyType )
	{
	case SKILL::EMACTION_TYPE_NORMAL:
		if ( m_pCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_NORMAL ) 
			return TRUE; 
		break;
	case SKILL::EMACTION_TYPE_DEBUFF:
		if ( m_pCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_DEBUFF ) 
			return TRUE; 
		break;
	case SKILL::EMACTION_TYPE_LIMIT:  
		if ( m_pCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_LIMIT  ) 
			return TRUE; 
		break;
		// 체크하는 패널티 검사가 ;
		// EMACTION_TYPE, EMCROWNOPENALTY 두가지가 존재한다;
		// 이후 스킬로 빠질것임으로 이렇게 처리함;
	case EMCROWNOPENALTY_STATE:
		return TRUE;
		break;
	}

	return FALSE;
}

const BOOL GLCrow::IsNoAllPenalty() const 
{
	if( !m_pCrowData )
		return FALSE;

	if ( m_pCrowData->m_bNoAllPenalty == TRUE )
	{
		return sc::Random::getInstance().IsSucceed(m_pCrowData->m_wResist_Probability);
	}
	return FALSE;
} //IsNoAllPenalty

const BOOL GLCrow::IsNoPenalty( const unsigned int uiPenaltyType ) const
{
	switch ( uiPenaltyType )
	{
	case SKILL::EMACTION_TYPE_NORMAL:
		if ( (m_pCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_NORMAL) != EMCROWNOPENALTY_NORMAL ) 
			return FALSE;
		break;
	case SKILL::EMACTION_TYPE_DEBUFF:
		if ( (m_pCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_DEBUFF) != EMCROWNOPENALTY_DEBUFF ) 
			return FALSE;
		break;
	case SKILL::EMACTION_TYPE_LIMIT:  
		if ( (m_pCrowData->m_dwNoPenaltyFlag&EMCROWNOPENALTY_LIMIT)  != EMCROWNOPENALTY_LIMIT ) 
			return FALSE;
		break;
	default:
		{
			if ( uiPenaltyType != EMCROWNOPENALTY_STATE )
				return FALSE;
		}
		break;
	}

	return sc::Random::getInstance().IsSucceed( m_pCrowData->m_wResist_Probability );

	return FALSE;
}

DWORD GLCrow::GETCLIENTID()
{
	// Comp 획득;
	ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );
	if ( !pCrowComp )
		return ServerActor::GETCLIENTID();

	return pCrowComp->GETCLIENTID();
}

WORD GLCrow::GetSchool() const
{
	// Comp 획득;
	const ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< const ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );
	if ( !pCrowComp )
		return ServerActor::GetSchool();

	return pCrowComp->GetSchool();
}

SCONFTING* GLCrow::GETCONFTING()
{
	// Comp 획득;
	ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );
	if ( !pCrowComp )
		return ServerActor::GETCONFTING();

	return pCrowComp->GETCONFTING();
}

HRESULT GLCrow::FrameMoveAction( float fElapsedTime )
{
	GLLandMan* const pLand( GetLandMan() );

	if ( pLand == NULL )
		return FALSE;
	HRESULT hr(S_OK);

	// Comp 획득;
	ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );

	// 액션별 처리작업.
	//	
	switch ( m_sDOMINATION.emController )
	{
	case EMSPEC_DOMINATE_TYPE_RANDOM:		
		if ( isAction(GLAT_FALLING, GLAT_DIE) == false )
		{
			const DWORD dwCurTimer = ::clock();
			if ( dwCurTimer - m_sDOMINATION.dwTimer > 1000 )
			{
				m_sDOMINATION.dwTimer = dwCurTimer;
				const float fDirectAngle = sc::Random::getInstance().RANDOM_NUM() * 6.28f;
				const float fDistance = sc::Random::getInstance().RANDOM_POS() * m_sDOMINATION.fRadiusMoveable;
				const D3DXVECTOR3 vRandomPosition(m_vPosition.x + cos(fDirectAngle) * fDistance, m_vPosition.y, m_vPosition.z + sin(fDirectAngle) * fDistance);		
				MoveTo(&vRandomPosition, false, 0);
			}			
			TurnAction(GLAT_MOVE);
		}
		break;
	case EMSPEC_DOMINATE_TYPE_FIX:
		if ( isAction(GLAT_FALLING, GLAT_DIE) == false )
			TurnAction(GLAT_IDLE);
		break;
	}	

	const EMACTIONTYPE& emAction = GetAction();
	switch ( emAction )
	{
	case GLAT_IDLE:
		m_fIdleTime += fElapsedTime;

		if ( pCrowComp )
			pCrowComp->IdleProcess( fElapsedTime );
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		{
			if ( pCrowComp )
				pCrowComp->IdleProcess( fElapsedTime );

			// 케릭의 이동 업데이트.
			//
			m_actorMove.SetMaxSpeed ( GetMoveVelo() );
			m_actorMove.Update ( pLand->GetvecLayerCollision(), fElapsedTime );

            // 장애물판정 로직은 부하가 크고 현 로직이 비효율적으로 되어 있어서 지금 현재 딱히 사용되는 곳이 없어서 주석 처리함;
            // 만약 사용하고 싶다면 새로 만드는것을 추천;
// 			if (GetLandMan()->IsObstacle( m_actorMove.Position(), m_actorMove.GetPreviousPosition() ))
// 			{
// 				m_actorMove.Stop ();
// 				SetPositionActor(m_actorMove.GetPreviousPosition());
// 				//SetPosition(m_actorMove.GetPreviousPosition());
// 				//SendGoingMsg(m_actorMove.GetPreviousPosition());
// 
// 				GLMSG::SNETCROW_MOVETO		NetMsg(m_dwGaeaID,
// 					GetActState(),
// 					m_actorMove.GetPreviousPosition(), // m_vPos가 변경될 수 있기때문에 변경함.
// 					m_TargetID.vPos,
// 					m_dwCeID,
// 					m_TargetID.GaeaId,
// 					m_TargetID.emCrow,
// 					m_dwCFlag );
// 				SendMsgViewAround( &NetMsg );
// 			}

			//hr = m_actorMove.Update ( fElapsedTime );
			//if ( FAILED(hr) )
			//	return E_FAIL;

			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction ( GLAT_IDLE, false );
			}

			// Mob의 현재 위치 업데이트.
			//
			m_vPosition = m_actorMove.Position();

			// Mob의 현재 방향 업데이트.
			//
			D3DXVECTOR3 vMovement(m_actorMove.NextPosition());
			if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
			{
				D3DXVECTOR3 vDirection(vMovement - m_vPosition);
				if ( !DxIsMinVector(vDirection,0.2f) )
				{
					D3DXVec3Normalize ( &vDirection, &vDirection );
					m_vDirect = vDirection;
				}
			}
		}
		break;

	case GLAT_ATTACK:
		{
			if ( !AttackProcess ( fElapsedTime ) )
			{
				TurnAction ( GLAT_IDLE, false );
			}

			// 공격 방향으로 회전.
			//
			ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
			if ( pACTOR )
			{
				D3DXVECTOR3 vDirection = pACTOR->GetPosition() - m_vPosition;
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDirect = vDirection;
			}
		}
		break;

	case GLAT_SKILL:
		{
			if ( !SkillProcess ( fElapsedTime ) )
			{
				TurnAction ( GLAT_IDLE, false );
			}
		}
		break;

	case GLAT_SHOCK:
		//	한프레임만 적용되게 수정
		//		if ( !ShockProcess ( fElapsedTime ) )
		TurnAction ( GLAT_IDLE, false );
		break;

	case GLAT_FALLING:
		{
			m_fIdleTime += fElapsedTime;		

			if( m_pCrowData->m_sAction.m_bMobLink )
			{
				const float fDelayTime = m_pCrowData->m_sAction.m_fMobLinkDelay < 4.0f ? m_pCrowData->m_sAction.m_fMobLinkDelay : 4.0f;
				if ( m_fIdleTime > fDelayTime )	
				{				
					TurnAction ( GLAT_DIE );
					pLand->ReserveDropCrow( 
						m_pMobSchedule, 
						m_pCrowData->m_sAction.m_sMobLinkID, 
						m_vPosition.x, m_vPosition.z,
						m_pCrowData->m_sAction.m_fMobLinkScale, 
						m_pCrowData->m_sAction.m_fMobLinkDelay - 4.0f);
				}
			}
			else
			{
				if ( m_fIdleTime > 4.0f )	
					TurnAction ( GLAT_DIE );
			}

		}
		break;		

	case GLAT_PUSHPULL:
		{
			//m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );
			m_actorMove.Update ( pLand->GetvecLayerCollision(), fElapsedTime );
			if ( !m_actorMove.PathIsActive() )
			{
				m_actorMove.Stop ();
				TurnAction ( GLAT_IDLE, false );
			}

			// Mob의 현제 위치 업데이트.
			//
			//			m_vPos = m_actorMove.Position();
		}
		break;

	case GLAT_DIE:
		{			
		}
		break;
	case GLAT_GATHERING:
		break;
	};

	return hr;
}

BOOL GLCrow::IsSkillLoopEnd()
{
	int iSize = m_sHITARRAY.size();

	if( iSize - 1 <= 0 )
		return TRUE;

	return FALSE;
}

BOOL GLCrow::IsSkillEnd( SNATIVEID sSkill )
{
	if( sSkill == NATIVEID_NULL() )
		return FALSE;

	int iSize = m_sHITARRAY.size();

	if( iSize - 1 <= 0 )
		return TRUE;

	return FALSE;
}

void GLCrow::SelfSkillStart( SNATIVEID sSkillID, WORD wLevel )
{	
	if( sSkillID == NATIVEID_NULL() )
		return;

	STARID				sTARIDS[EMTARGET_NET<<1];
	WORD				wTARNUM = 1;	

	STARGETID sTargetID;
	sTargetID.emCrow = GetCrow();
	sTargetID.GaeaId = GetGaeaID();
	sTargetID.vPos = GetPosition();


	sTARIDS[0] = STARID( CROW_PC, GetGaeaID());
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );

		if( pSkill == NULL ) return;

		GLLandMan* const pLand( GetLandMan() );
		if ( pLand == NULL || (pLand && pLand->IsPeaceZone()))
			return;		

		SkillProc( FALSE, sSkillID, wLevel, wTARNUM, sTARIDS );
	}
}

void GLCrow::DelayActionSkillStart( STARGETID sCasterID, SNATIVEID sSkillID, WORD wLevel )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	ServerActor* pActorCaster = pLand->GetActor(sCasterID);

	if( pActorCaster == NULL || sSkillID == NATIVEID_NULL() ) return;

	STARID				sTARIDS[EMTARGET_NET<<1];
	WORD				wTARNUM = 0;

	pLand->m_SkillTargetSystem.SelectSkillTarget( pActorCaster, this, sSkillID, wLevel, GetPositionActor(), sTARIDS, wTARNUM);


	if ( sCasterID.emCrow == CROW_PC )
	{
		GLChar* pChar = pLand->GetChar(sCasterID.GaeaId);
		if ( pChar == NULL )
			return;

		STARGETID sTargetID;
		sTargetID.emCrow = GetCrow();
		sTargetID.GaeaId = GetGaeaID();
		sTargetID.vPos = GetPosition();

		pChar->ApplySkillDealy( SCHARSKILL( sSkillID, wLevel), sTargetID, wTARNUM, sTARIDS, 1 );
	}

	if ( sCasterID.emCrow == CROW_MOB )
	{
		GLCrow* pCrow = pLand->GetCrow(sCasterID.GaeaId);
		if ( pCrow == NULL )
			return;
		pCrow->ApplySkill(TRUE, sSkillID, wLevel, wTARNUM, sTARIDS);
	}

	if ( sCasterID.emCrow == CROW_SUMMON )
	{
		GLSummonField* pSummon = pLand->GetSummon(sCasterID.GaeaId);
		if ( pSummon == NULL )
			return;
		pSummon->ApplySkill(TRUE, sSkillID, wLevel, wTARNUM, sTARIDS);
	}

	if ( sSkillID.IsValidNativeID() )
	{
		GLMSG::SNETCROW_SKILL_DEALY_ACTION NetMsg(m_dwGaeaID);

		NetMsg.dwAType = m_dwAType;
		NetMsg.skill_id = sSkillID;
		NetMsg.skill_lev = wLevel;
		NetMsg.vTARPOS = m_vTARPOS;
		NetMsg.vPos = m_vPosition;

		for (WORD i=0; i<wTARNUM; ++i)
			NetMsg.ADDTARGET(sTARIDS[i]);
		SendMsgViewAround(&NetMsg);
	}
}

// End 시 위치를 옮겨주도록 하자.
void GLCrow::SkinAniControl_AnimationMove()
{
	if ( !m_pAttackProp )
		return;

	if ( !m_pAttackProp->m_bAnimationMove )
		return;

	GLLandMan* const pLand(GetLandMan());

	if ( pLand == NULL )
		return;

	// 1. 실제위치로 갈 수 있는지 확인하고 갈 수 있다면 이동한다.
	//		갈 수 없다면 아무 움직임이 없도록 한다. ( Character 랑 Crow 랑 다름 )
	// 2. 그리고 갈 수 있다면, 현재 위치를 전 유저에서 이동하라고 보내준다.
	//
	// 위의 작업이 MsgGoto 에서 다 작업된다.

	D3DXVECTOR3 vPos;
	D3DXMATRIX matRotate;
	float fThetaY = DXGetThetaYFromDirection ( m_vDirect, vDIRORIG );
	D3DXMatrixRotationY ( &matRotate, fThetaY );
	D3DXVec3TransformCoord( &vPos, &m_pAttackProp->m_vAnimationMovePos, &matRotate );

	vPos += GetPositionActor();

	if ( pLand->IsLayerSimpleCollision( GetPositionActor(), vPos, 5.f ) )
	{
		// 이동 불가
		return;
	}

	SetPositionActor( vPos );
}

void GLCrow::UpdateSkillEff( float fTime, float fElapsedTime )
{
	/// 도발 ////////////////////////////////////////////
	if( RF_PROVOKE( m_EffSkillVarSet ).IsOn() ) {
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetHostID();

		if( !sID.ISNULL() && sID != m_TargetID ) NewTarget ( sID, TRUE );
	}

	/// 피해발동 ////////////////////////////////////////////////
	if( RF_ACTIVE( m_EffSkillVarSet ).IsOn() )
	{
		WORD  wLv		= 0;
		DWORD dwSkillId = 0;
		STARGETID	sID;

		if( RF_ACTIVE( m_EffSkillVarSet ).Invoke( wLv, dwSkillId, sID ) )
		{
			InvokeActionSkillStart( sID, SNATIVEID(dwSkillId), wLv );
		}
	}
}

void GLCrow::InvokeActionSkillStart( STARGETID sTargetID, SNATIVEID sSkillID, WORD wLevel  )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

	ServerActor* pTarget = pLand->GetActor(sTargetID);

	if( pTarget == NULL || !sSkillID.IsValidNativeID() ) return;

	STARID				sTARIDS[EMTARGET_NET<<1];
	WORD				wTARNUM = 0;

	pLand->m_SkillTargetSystem.SelectSkillTarget( this, pTarget, sSkillID, wLevel, pTarget->GetPositionActor(), sTARIDS, wTARNUM);

	STARGETID sTarID = STARGETID( sTargetID.emCrow, sTargetID.GaeaId, pTarget->GetPosition() );

	ApplySkill( TRUE, sSkillID, wLevel, wTARNUM, sTARIDS );

	switch( sTarID.emCrow )
	{
	case CROW_PC :
		{
			GLChar* pChar = dynamic_cast<GLChar*>( pTarget );

			if( pChar ) {
				GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION NetMsgDealy;
				NetMsgDealy.emCrow		= sTarID.emCrow;
				NetMsgDealy.dwID		= sTarID.GaeaId;
				NetMsgDealy.skill_id	= sSkillID;
				NetMsgDealy.wLEVEL		= wLevel;
				NetMsgDealy.vTARPOS		= sTarID.vPos;

				for( WORD i=0; i<wTARNUM; ++i )
					NetMsgDealy.ADDTARGET ( sTARIDS[i] );

				pChar->SendToClient(&NetMsgDealy );

				GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION_BRD NetMsgBRD;
				NetMsgBRD.emCrow	= sTarID.emCrow;
				NetMsgBRD.dwID		= sTarID.GaeaId;
				NetMsgBRD.skill_id	= sSkillID;
				NetMsgBRD.wLEVEL	= wLevel;
				NetMsgBRD.vTARPOS	= sTarID.vPos;

				for( WORD i=0; i<wTARNUM; ++i )
					NetMsgBRD.ADDTARGET ( sTARIDS[i] );

				pChar->SendMsgViewAround(&NetMsgBRD);
			}
		}
		break;
	case CROW_MOB :
		{
			GLCrow* pMob = dynamic_cast<GLCrow*>( pTarget );

			if( pMob ) {

				GLMSG::SNETCROW_SKILL_DEALY_ACTION NetMsg(sTarID.GaeaId);

				NetMsg.dwAType   = pMob->m_dwAType;
				NetMsg.skill_id  = sSkillID;
				NetMsg.skill_lev = wLevel;
				NetMsg.vTARPOS   = pMob->GetTarPos();
				NetMsg.vPos      = sTarID.vPos;

				for (WORD i=0; i<wTARNUM; ++i)
					NetMsg.ADDTARGET(sTARIDS[i]);
				pMob->SendMsgViewAround(&NetMsg);
			}
		}
		break;
	case CROW_SUMMON :
		{
			GLSummonField* pSummon = dynamic_cast<GLSummonField*>( pTarget );

			if( pSummon ) {

				GLMSG::SNET_SUMMON_SKILL NetMsg( sTarID.GaeaId
					, pSummon->GetTargetID(), pSummon->m_dwAType
					, pSummon->m_bUsingEventSkill
					, sSkillID
					, wLevel
					, pSummon->GetTarPos() );

				for ( WORD i=0; i<wTARNUM; ++i )	
					NetMsg.ADDTARGET ( sTARIDS[i] );

				pSummon->SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
			}
		}
		break;
	}
}