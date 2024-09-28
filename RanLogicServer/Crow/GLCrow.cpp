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
	// AI ����;
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

	// ���� ���� ������ ������.
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

	// Summon Data ����;
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
	
	// �� ���� ������ �ʱ�ȭ.
	GLCROWLOGIC::INIT_DATA();    

	// �����췯 �ʱ�ȭ.
	m_pMobSchedule = pMobSchedule;
	
	// ��ġ �ʱ�ȭ.
	if (pPos)
		m_vPosition = *pPos;
	else if (m_pMobSchedule)
		m_vPosition = m_pMobSchedule->m_pAffineParts->vTrans;

	m_vGenPos = m_vPosition;
	m_vOrgTarPos = m_vPosition;

    m_fScale = fScale * (m_pMobSchedule != 0 ? m_pMobSchedule->m_fScale : 1.f);

	//	���� �ʱ�ȭ.
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

	// �ֺ� �ɸ��� �ν�.
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

	// ����̽� ����;
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

	// Summon Data ����;
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

	// CROW ���� ������ �ʱ�ȭ;
	GLCROWLOGIC::INIT_DATA();

	// ��ġ;
	m_vPosition = vPosition;

	// ����;
	D3DXVECTOR3 vOwnerPos = pOwner->GetPosition();
	m_vDirect = m_vPosition - vOwnerPos;
	D3DXVec3Normalize ( &m_vDirect, &m_vDirect );

	// Ÿ��ID
	m_TargetID.RESET();
	m_TargetID = sTarget;

	// ��;
	m_mapID = pOwner->GetCurrentMap();

	// ActorMove;
	CreateActorMove( pLandMan->GetNavi(), m_vPosition );
	StopActorMove();

	// Hit Array �ʱ�ȭ;
	m_sHITARRAY.clear();

	m_fattTIMER = 0.0f;
	m_fIdleTime = 0.0f;

	// �ֺ� �ɸ��� �ν�;
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

	// ���¸� ���������� �ٲ۴�;
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
	// ���� ���� ���� ���� �� ����;
	// GLARoundSlot		m_ARoundSlot; �� ����Ʈ ���� �߻�;	
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
	BOOL bESCAPE = ( EMCROWACT_UP_ESCAPE == emCROWACT ); // && ( dwDamage > DWORD ( GetMaxHP() * 0.10f ) ); // Note : damage�� ���� ���� ������ ��û���� ����.
    if ( !bESCAPE )
        return S_FALSE;

	ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, sTARID );
	if ( pACTOR && pACTOR->IsValidBody() )
	{
		const D3DXVECTOR3 &vACTOR_POS = pACTOR->GetPosition();

		D3DXVECTOR3 vDir = ( GetPosition() - vACTOR_POS );		//	Attack -> Receive ������ ����.
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

		// �̵� �޼��� �߻�.
		if ( m_emCrow != CROW_SUMMON )
		{
			GLMSG::SNETCROW_MOVETO		NetMsg(m_dwGaeaID,
												GetActState(),
												m_actorMove.Position(), // m_vPos�� ����� �� �ֱ⶧���� ������.
												m_TargetID.vPos,
												m_dwCeID,
												m_TargetID.GaeaId,
												m_TargetID.emCrow,
												m_dwCFlag );
			SendMsgViewAround( &NetMsg );
		}
		else
		{
			// �̵� �޼��� �߻�.
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

	// ���ݼ����� ������ ������ ���.
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

		// ��ų ���� ������.
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

	// ���� �޼��� �߻�
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
		{ // PET Attack ó��
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
	msg.dxPos		= GetPosition();	//�̴ϸʿ� ������ũ�� ����ϱ� ���� ��ǥ�� �ʿ��ϴ�.
	
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

	// �ֺ� Char���� �������� �˸�.
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

	// ���� �׼� ���.
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

    // ���� �ι��̻� ���� ��� �ϴ� ���� [10/28/2013 hsshin]
    case GLAT_FALLING:
        // FALLING������ DIE�ܿ��� ���¸� ������ �� ����;
        if ( toAction != GLAT_DIE )
            return;

        break;
    case GLAT_DIE:
        // ���� ���¿����� �ٸ� �׼����� ��ȯ �Ұ�;
        return;

	default:
        break;
	}
    
	// �׼� �ʱ�ȭ.
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

	// Ÿ���� ���� ������ ��쿡 damage �ټ� ����.
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

    // ���� GLCrow�� ��� MOB,NPC�� ��츸 ����ó���� �ϵ��� ��. ������ ��� ���ؼ� ó���ؾ� �ϸ� ���Ŀ� �̺κ��� �������ָ� �� (CROW_ZONENAME ���� �ǵ�ġ ���� �̺�Ʈ�� ��� �߻��ؼ�);
    if ( ( GetCrow() == CROW_MOB ) || ( GetCrow() == CROW_NPC ) )
    {
        // �δ� ������/ HP�� 0�϶� ������ ���������� ���ؼ��� lua���� ��� �� �� �ֵ��� ��;
        InstanceSystem::InstanceField* const _pInstance = pLand->GetScriptInstance();
        if ( _pInstance != NULL )  // �δ��� �ƴ� ��� NULL;
        {
            DWORD dwID(m_sAssault.GaeaId);

			// Mob�� ���� �״�� ������ �ȴ�;
			// PC�� ��� DBNum�� ���� ������ ���� �־�� �Ѵ�;
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

					// ����� ��ȯ���� ������ SUMMON�� ����;
					// �������� �ʴ´�;
					if ( pOwner && pOwner->GetCrow() == CROW_PC )
					{
						GLChar* pChar = static_cast< GLChar* >( pOwner );
						if ( pChar )
							dwID = pChar->CharDbNum();
					}
				}
			}

            // ���� Event Died�� ���ϰ��� false�� ��� ���̴� ó���� ���� �ʴ´ٴ� ��;
            if ( _pInstance->EventDie(GetCrow(), GetGaeaID(), m_sAssault.emCrow, dwID) == false )
                return;
        }
		// ���� �� ī����;
		pLand->AddDropOutCrowCount(GetNativeId());
    }

	if( GetCrow() != CROW_ZONENAME && GetCrow() != CROW_GATENAME && GetCrow() != CROW_PET )
	{
		//���� ���� ó��;
		DoFalling_Proc();
	}
}

void GLCrow::DoFalling_Proc()
{
    // �������� ���� �߻�. ( ����ġ + ������ + �ݾ� )
    GenerateReward ();

    //	���� ��ȭ.
    TurnAction(GLAT_FALLING);

	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return;

    // �׷� ������ ��쿡 �׷� Ÿ���� �ʱ�ȭ�Ѵ�.
    if( IsGroupLeader() )
    {
		pLand->RESET_GROUPTARGET( GetGroupName() );
    }
}

void GLCrow::BlockProc(const STARGETID& sTargetID, const SNATIVEID& SkillID /* = NATIVEID_NULL() */, const WORD wSkillLevel /* = 0*/)
{
	// �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����. 
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

	// �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	GLMSG::SNETCROW_AVOID NetMsg(
		m_dwGaeaID,
		m_TargetID.emCrow, m_TargetID.GaeaId,
		SkillID, wSkillLevel);
	SendMsgViewAround(&NetMsg);

	ServerActor* pACTOR_TAR = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
	if (!pACTOR_TAR)
        return;

	// Ÿ���� ���� ������ ��쿡 damage �ټ� ����.
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

	// ����ڿ��� ������� ����.
	const DWORD dwActualDamage = ToDamage ( m_TargetID, nDAMAGE, dwDamageFlag );


	// �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	//
	GLMSG::SNETCROW_DAMAGE NetMsg(
        m_dwGaeaID,
        m_TargetID.emCrow,
        m_TargetID.GaeaId,
        dwActualDamage,
        m_dwNowHP,
        dwDamageFlag);
	SendMsgViewAround(&NetMsg);

    // ������ ���
    NetMsg.emHitCrow = m_emCrow;

	ServerActor* pACTOR = m_pGaeaServer->GetTarget ( pLand, m_TargetID );
    if (!pACTOR)
        return;

	pACTOR->ReactDamage( this, dwDamageFlag );

	// ���� �̻� ����.
	//
	if ( pACTOR && m_pAttackProp->emBLOW_TYPE!=EMBLOW_NONE )
	{
		// �߻� Ȯ�� ���.
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
			// �����̻� �߻�.
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
				// �����̻� �߻� Msg.
				//
				GLMSG::SNETPC_STATEBLOW_BRD NetMsgState;
				NetMsgState.emCrow = m_TargetID.emCrow;
				NetMsgState.dwID = m_TargetID.GaeaId;
				NetMsgState.emBLOW = sSTATEBLOW.emBLOW;
				NetMsgState.fAGE = fLIFE;
				NetMsgState.fSTATE_VAR1 = sSTATEBLOW.fSTATE_VAR1;
				NetMsgState.fSTATE_VAR2 = sSTATEBLOW.fSTATE_VAR2;

				// '��ų���'�� �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
				//
				pACTOR->SendMsgViewAround(&NetMsgState);

				// ��ų ��󿡰� �޼��� ����.
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
    //	ü�� ��´�.(�� �������� �����Ѱ��� �ƴϴ� ���� ����);
    VAR_BODY_POINT( sACTOR.emCrow, sACTOR.GaeaId, DAMAGE_TYPE_CURSE, nActualDamageHP, nActualDamageMP, nActualDamageSP );

	// �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
	//
	GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
	NetMsgBRD.emCrow = GetCrow();
	NetMsgBRD.dwID = m_dwGaeaID;

	//	��ų�� ����� ������ ��ŷ.
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
	// ����ڿ��� ������� ����.
	const DWORD dwActualDamage = ToDamage ( sACTOR, nDAMAGE, 0 );

	// �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����.
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
		// ��ų ���� ������.
		//
		GLSKILL* pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
		if ( pSkill->m_sBASIC.emAPPLY != SKILL::EMAPPLY_MAGIC )
		{
			// ��ų Ÿ���� �ϳ��� ��ȿ�ϰ� Ȯ���� �ϰ� ����.
			//	NEED : ���� Ÿ���� ��� �ٽ� Ȯ���ؾ���.
			sTargetID = STARGETID(m_sTARIDS[0].emCrow, m_sTARIDS[0].dwID);			
			bCheckHit = TRUE;
		}
		else
		{
			sTargetID.GaeaId = EMTARGET_NULL;
		}
	}

	//	ť �ʱ�ȭ
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

			//	ť ����Ÿ�� ó���� ��� �����Ѵ�.
			m_sHITARRAY.pop_front ();
		}	
	}

	//	�̻����� ���� ����Ÿ�� ���� ó������ �������,
	//	�������� �Ѳ����� ó���ϰ�, FALSE�� �����Ѵ�.
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

				//	ť ����Ÿ�� ó���� ��� �����Ѵ�.
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

			// ���� Ŭ���̾�Ʈ�� ���� ��ġ ����ȭ�� ����
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
			// ĳ������ ���� ������ UpdateViewAround ()���� ����ġ�� ĳ���Ͱ� �������� ���� �� ����.
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
            if ( pCrowNode->Data == this ) // �ڽ��� ����;
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

	// ���� �þ߿� ������ CELL ��� Ž��.
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

// Ŭ�� ��ü�� �����ؾ��Ѵ�;
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

	//	���� ������ ó��( ������ �ش�. ) 
	for ( int i = 0; i < EMELEMENT_MAXNUM; ++i ) 
	{
		if ( !m_sDurDamage[i].VAILD() ) continue;

		m_sDurDamage[i].m_fTime -= fElapsedTime;			
	
		if ( m_sDurDamage[i].m_fTime < 0.0f ) 
		{
			//	������ �ش�.
			m_sDurDamage[i].m_fTime = m_sDurDamage[i].m_fTickTime;

			//	������ ���

			// ��ü���װ� = ����� ���װ� / 100 * �Ӽ�����ġ �������
			int nRESIST = GETRESIST().GetElement( (EMELEMENT)i );
			float fResistTotal = (float) ((float) nRESIST * 0.01f * GLCONST_CHAR::fRESIST_G);
			fResistTotal = fResistTotal > GLCONST_CHAR::fRESIST_MAX_CROW ? GLCONST_CHAR::fRESIST_MAX_CROW : fResistTotal;

			// �ּ�/�ִ� ������ = ������-(������*��ü���װ�)
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
            //	ü�� ��´�.(�� �������� �����Ѱ��� �ƴϴ� ���� ����);
            VAR_BODY_POINT( m_sDurDamage[i].m_sID.emCrow, m_sDurDamage[i].m_sID.GaeaId, DAMAGE_TYPE_DOT, nActualDamageHP, nActualDamageMP, nActualDamageSP );

			GLMSG::SNETPC_SKILLFACT_BRD NetMsgBRD;
			NetMsgBRD.emCrow = GetCrow();
			NetMsgBRD.dwID = m_dwGaeaID;

			//	��ų�� ����� ������ ��ŷ.
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

	// Mob ��Ų ������Ʈ.
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

	// ���忡�� ��ϵ� ���� ������Ʈ �ϱ� ���ؼ� ȣ����.
	//
	pLand->MoveCrow ( m_dwGaeaID, m_vPosition );
	UpdateViewAround ();

	m_fAge += fElapsedTime;	

	// Comp ȹ��;
	ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );

	// HP�� 100%�� �Ǹ� DamageLog �ʱ�ȭ;
	if( GetMaxHP() <= GetNowHP() )
	{
		m_cDamageLog.clear();
		m_cSummonDamageLog.clear();
		m_cDamageLogParty.clear();
	}
	
	// �Ʒ� ������ ��� ������ ��쿡�� ������ �ʿ䰡 ���ٰ� �Ǵ�, server�� ������ ������ ���� FrameMove�� ���� �ʴ´�;
	if ( pLand->GET_GROUPTARGET( GetGroupName() ).GaeaId == EMTARGET_NULL &&  // ���� �׷�(����ġ)�� Ÿ���� ���� ��;
		m_dwPC_AMOUNT==0 &&                 // �ֺ��� �÷��̾ ������;
		m_dwSUMMON_AMOUNT==0 &&             // �ֺ��� ��ȯ���� ������;
		!m_pSkinChar &&                     // skinchar�� ��ȿ�϶�(���ķ�����,level edit)�� �ƴҶ�;
		!m_pCrowData->IsBoss() &&           // �������� �ƴҶ�;
        !IsGroupLeader() &&                 // �׷�(����ġ) �� ������ �ƴҶ�;
		!pLand->IsInstantMap() &&      // ������ �δ��� �ƴҶ� (���� 51��);
		!m_pGaeaServer->IsEmulatorMode() && // ���ķ����Ͱ� �ƴҶ�;
		!pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) && // �ش� ���尡 ���� ���� ���� �ɼ��� ������;
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

	// AABB ���.
	//
	m_vMax = m_vPosition + m_vMaxOrg;
	m_vMin = m_vPosition + m_vMinOrg;

	// ���� ��ǥ�� ��ȿ�� �˻�.
	//		(����) Ÿ������ ����� �ݵ�� ȣ���Ͽ� ��ȿ�� �˻� �ʿ�.
	//
	bool bVALID_TAR(true);

	BOOL bCHECK_TAR = m_pGaeaServer->ValidCheckTarget(pLand,m_TargetID);
	
	if ( !bCHECK_TAR )
		bCHECK_TAR = false;



	// �������� ���Ͽ� ��Ʈ���� �ߴܵǾ���.
	if( m_bIsPatrolStop == true )
	{
		GLMobSchedule* pMobSch = GetMobSchedule();
		float fActivityArea = 0;
		if( pMobSch )
			fActivityArea = pMobSch->m_fActivityArea;

		// Ÿ���� �ִ°�?
		if( bCHECK_TAR )
		{
			// �ൿ�ݰ��� �����Ǿ� �ִٸ�.
			if( 0 < fActivityArea )
			{
				D3DXVECTOR3 vDist = m_vPosition - m_vPatrolStopPos;
				float fDist = D3DXVec3LengthSq(&vDist);

				// �ൿ�ݰ��� �Ѿ��� �� ���� ��ġ�� ���ư���.
				if( fActivityArea*fActivityArea < fDist )
				{
					m_bReturnPatrolPos = true; //�����϶�!
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

			// Ÿ���� ���ٸ� �����϶�!
			MoveTo( &m_vPatrolStopPos, false, 0 );

			D3DXVECTOR3 vDist = m_vPosition - m_vPatrolStopPos;
			float fDist = D3DXVec3LengthSq(&vDist);
			
			// ���� ��ġ�� ������ �Դٸ� ����
			if( fDist < 15 )
			{
				m_bReturnPatrolPos = false;
				m_bIsPatrolStop = false;				// ��Ʈ�� ���� ����
				m_vPatrolStopPos = D3DXVECTOR3(0,0,0);	// ��Ʈ�� ���� ��ġ �缳��
			}
		}

	}

	// //���� ����� ������ ������ ��ġ�� �̵��϶�.
	// ��� ���¡�� �������ؼ� HP�� ä���.
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


		// �� �� �ִٸ� �پ��
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

		// ������ġ�� ���ư���.
		MoveTo( &m_vPatrolStopPos, false, 0 );

		D3DXVECTOR3 vDist = m_vPosition - m_vPatrolStopPos;
		float fDist = D3DXVec3LengthSq(&vDist);

		// ���� ��ġ�� ������ �Դٸ� ����
		if( fDist < 15 )
		{
			m_bReturnPatrolPos = false;				// ��ȯ ����
			m_bIsPatrolStop = false;				// ��Ʈ�� ���� ����
			m_vPatrolStopPos = D3DXVECTOR3(0,0,0);	// ��Ʈ�� ���� ��ġ �缳��
		}
	}

	const bool bDominated = m_sDOMINATION.emController != EMSPEC_DOMINATE_TYPE_SELF;
	if ( bCHECK_TAR && m_TargetID.emCrow==CROW_PC )
	{
		PGLCHAR pCHAR = m_pGaeaServer->GetChar ( m_TargetID.GaeaId );
		if ( pCHAR )
		{
			// ������ΰ�?
			if ( pCHAR->GETCONFTING()->IsCONFRONTING() )
				bVALID_TAR = false;
			
            // ĳ���Ͱ� ��������̰� ���� ���� ���°� �ƴѰ�? GM �� visible off �Ǵ� none �� �����ϴ�.
			// ���踦 ���� ���¿����� �������� IsVisibleDetect()�� ����;
			if ( !pCHAR->IsVisibleDetect(this) && !bDominated )
				bVALID_TAR = false;

			// SafeZone �ϰ�� ���� �Ұ�
			if ( pCHAR->IsSafeZone() ) 
				bVALID_TAR = false;

			if( pCHAR->IsNonTargetable() )
				bVALID_TAR = false;

			// ���Ͱ� ������ ���� �� �׷�Ÿ���� ��� �����̸� �������� �ʴ´�.
			const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();
			BOOL bGroupTarget = IsGroupTarget( m_TargetID );
			if( !pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) && !bGroupTarget && !bDominated )
			{
				// ������ ������ ����.
				//
				// �ٱⰡ���� ���ΰ�?
				if ( bVALID_TAR && m_pCrowData->m_sAction.m_bRun )
				{
					// ���� ĳ���� �ֺ��� �����ִ� ���� ����
					DWORD dwAmount = pCHAR->GetARoundSlot().GetAmount();
					// ���� ĳ���� �ֺ��� �����ִ� ���� ���ڰ� ������ �������� ũ�ٸ�...
					if ( dwAmount > GLCONST_CHAR::wMAXMOB_TRACKING )
					{
						// ���� Ÿ�ٰ��� �Ÿ��� ����Ѵ�.
						D3DXVECTOR3 vDist = m_vPosition - m_vOrgTarPos;
						float fDist = D3DXVec3Length(&vDist);
						
						// ���� �Ÿ��� �� Ʈ��ŷ �Ÿ����� ũ�ٸ�...
						if ( fDist > GLCONST_CHAR::fMOB_TRACING )
						{
							bVALID_TAR = false;

							// �ۼ� �ִ� ���̶��?
							if ( m_pCrowData->m_sAction.m_bRun )
							{
								// �پ��
								ToRun();
							}
							
							// ������ġ�� ���ư���.
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
			// ������ΰ�?
			if ( pCHAR->GETCONFTING()->IsCONFRONTING() )	bVALID_TAR = false;

			// ��ȯ���� ��������ΰ�?
			if ( !pSummon->IsVisibleDetect( this ) )		bVALID_TAR = false;

			// ���Ͱ� ������ ���� �� �׷�Ÿ���� ��� �����̸� �������� �ʴ´�.
			const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();
			BOOL bGroupTarget = IsGroupTarget( m_TargetID );
			if( !pLand->GetUseLevelEtcFunc(EMETCFUNC_MOBCHASE) && !bGroupTarget )
			{
				// ������ ������ ����.
				//
				// �ٱⰡ���� ���ΰ�?
				if ( bVALID_TAR && m_pCrowData->m_sAction.m_bRun )
				{
					// ���� ��ȯ�� �ֺ��� �����ִ� ���� ����
					DWORD dwAmount = pSummon->GetARoundSlot().GetAmount();
					// ���� ��ȯ�� �ֺ��� �����ִ� ���� ���ڰ� ������ �������� ũ�ٸ�...

					if ( dwAmount > GLCONST_SUMMON::wMaxMobTracking[ pSummon->m_emTYPE ] )
					{
						// ���� Ÿ�ٰ��� �Ÿ��� ����Ѵ�.
						D3DXVECTOR3 vDist = m_vPosition - m_vOrgTarPos;
						float fDist = D3DXVec3Length(&vDist);

						// ���� �Ÿ��� �� Ʈ��ŷ �Ÿ����� ũ�ٸ�...
						if ( fDist > GLCONST_CHAR::fMOB_TRACING )
						{
							bVALID_TAR = false;

							// �ۼ� �ִ� ���̶��?
							if ( m_pCrowData->m_sAction.m_bRun )
							{
								// �پ��
								ToRun();
							}

							// ������ġ�� ���ư���.
							MoveTo ( &m_vGenPos, false, 0 );
						}
					}
				}
			}
		}
	}
	
	// Ÿ���� �����󿡼� ��ȿ���� �ʰų� ���̻� ������ �ʿ䰡 ���� PC �� ���
	if ( !bCHECK_TAR || !bVALID_TAR )
	{
		m_dwAttPosSlot = GLARoundSlot::SLOT_SIZE;
		
		// Ÿ������ ��ȿȭ�Ѵ�.
		ResetTarget();	
		m_TargetID = STARGETID(CROW_PC,GAEAID_NULL);

		// �׷� �����̸� ��ü �׷� Ÿ���� �ʱ�ȭ
		if( IsGroupLeader() && pLand->GET_GROUPTARGET( GetGroupName() ).GaeaId != EMTARGET_NULL )
		{
			const SETGROUPMEM& setGroupMember	= pLand->GET_GROUPMEMBER(GetGroupName());
			SETGROUPMEM_CITER setMember_citer	= setGroupMember.begin();
			for( ; setMember_citer != setGroupMember.end(); ++setMember_citer )
			{
				// ����� �ƴϸ� �Ѿ��.
				if( !pLand->IS_GROUPMEMBER(GetGroupName(), *setMember_citer ) )
					continue;

				// �ڽ��̸� �Ѿ��.
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
					//�����п� setup �� ������ HP �缳��
					//
					BeAuthedBy(i);

					TurnAction(GLAT_AUTHED);

					break;
				}
			}
		}

		//
		//���� ���ο� ������� �Ʒ��� ���׵��� �׻� update�Ǿ�� �Ѵ�.
		//		
		GLCROWLOGIC::UPDATE_DATA ( fTime, fElapsedTime );
		UpdateDurDamage ( fTime, fElapsedTime );

		//goto crow_framemove_end;	//�������� ��� �� �̻��� �׼� ó���� �ʿ����.
	}


	// ���� �ʾ�����
	if ( IsValidBody() )
	{
        if ( m_dwNowHP <= 0 )
		{
			DoFalling();	
		}
		// Ÿ���� ��ȿȭ �Ǿ��µ� ���ݾ׼� ���̸�
		else
        {
            if ( !isAction(GLAT_SHOCK) && !isAction(GLAT_PUSHPULL) )
            {
                if ( !IsFLAG(EMTARGET) && (isAction(GLAT_ATTACK) || isAction(GLAT_SKILL)) )
                {
                    // ���޻��·� ����
                    TurnAction ( GLAT_IDLE, false );
                }
                if ( !m_bSTATE_STUN && !bDominated )				
                    RunSchedule ( fTime, fElapsedTime);	// ������ ����.;
            }
            // Crow �� ���� ���°����� Update �Ѵ�;
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

	// Mob�� ���� ��ġ ������Ʈ.
	//
	m_vPosition = m_actorMove.Position();

	
	// m_pd3dDevice �� ��ȿ�� ��츸 �Ʒ� �ڵ尡 ����ȴ�. ( Level Editor ���� �½�Ʈ��. )
	//
	if ( !m_pd3dDevice )
		return S_OK;

	
	// ���ϸ��̼�, ��Ų ������Ʈ.
	//
	UpateAnimation( fTime, fElapsedTime );


	// ���� ��ġ�� �������� Transform ��Ʈ���� ���.
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

		EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );	//	�׽�Ʈ��.

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
		//	�̰��� �´ٴ°� Emulater�� �����ٴ� �Ű� �װ� ���������� �۾��Ѵٴ°���.
		//	������� ���� ���ص� ��.
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

	// �з��� ��ġ�� �̵� �õ�.
	//
	const BOOL bSucceed = m_actorMove.GotoLocation
	(
		D3DXVECTOR3(vMovePos.x,vMovePos.y+5,vMovePos.z),
		D3DXVECTOR3(vMovePos.x,vMovePos.y-5,vMovePos.z)
	);

	if ( bSucceed )
	{
		// �з����� ���� ����.
		//
		m_TargetID.vPos = vMovePos;
		TurnAction ( GLAT_PUSHPULL );

		// �и��� �ӵ� ����.
		//
		m_actorMove.SetMaxSpeed ( fPUSH_SPEED );

		// �޽��� ���� ����.
		//
		GLMSG::SNET_PUSHPULL_BRD NetMsgBRD;
		NetMsgBRD.emCrow	= GETCROW();
		NetMsgBRD.dwID		= m_dwGaeaID;
		NetMsgBRD.vMovePos		= vMovePos;
		NetMsgBRD.fPUSH_SPEED	= fPUSH_SPEED;
		NetMsgBRD.dwAniType		= dwAniType;

		// [�ڽ��� �ֺ� Char] ���� Msg�� ����.
		//
		SendMsgViewAround(&NetMsgBRD);
	}
}

// �ǰ� ������ �Լ� ���� [9/26/2013 hsshin];
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

	// �δ� Script ������ Receive Damage ó��;
	DWORD dwActualDamage = dwDamage;
	InstanceSystem::InstanceField* const pInstance =
		pLand->GetScriptInstance();
	if ( NULL != pInstance )  // �δ��� �ƴ� ��� NULL;
	{
		// dwID�� �÷��̾��ϰ�� DBNUM, ��,Summon���� Land������ ��ȿ�� GloabalID�� ���;
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

		// ����! : dwDamage�� reference�� ���ڸ� �ѱ�⶧���� ���� ����ɼ� �ִ�. ( ��ũ��Ʈ���� �������� ������ �� �� �ְ� �ϱ� ���� );
		if ( false == pInstance->EventReceiveDamage(
			CROW_MOB,
			GetGaeaID(),
			emACrow,
			dwID,
			dwActualDamage,
			dwDamageFlag ) )  // return���� false�� ��� ������ ������ ó���� ���� �ʰڴٴ� ��;
		{
			return dwActualDamage;
		}
	}

    //
    //mjeon.CaptureTheField
    //
    if ( m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR )
        return ReceiveDamage_Procedure_For_CTF(emACrow, dwAID, dwDamage, dwDamageFlag);

    // Damage ����.
    const DWORD dwDxHp = RECEIVE_DAMAGE ( dwDamage );

	// Damage ���� ������. 0 �϶��� miss �߻� ( Ÿ�� ������ ���� ȣ���. )
	if ( dwDxHp > 0 )
	{
		//	Damage ���.
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
		// Note : ��ȯ�� �����϶� m_cSummonDamageLog�� ����Ѵ�
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

		// ��� ����.
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
		// AI : �ǰݿ� ���� Ÿ�� ���� AIȣ��;
		SetTargeting(STARGETID(emACrow,dwAID));
	}

	return dwDxHp;
}

const DWORD GLCrow::ReceiveDamage_Procedure_For_CTF( const EMCROW emACrow, const DWORD dwAID, const DWORD dwDamage, const DWORD dwDamageFlag )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    //  ���� �ڵ带 ������ �ǵ帮�� �ʱ� ����, ������ ������ CTF ������ �ڵ带 �ۼ��Ѵ�.
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
        // Note : ��ȯ�� �����϶� m_cSummonDamageLog�� ����Ѵ�
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

	// ���� ��ȯ�� �����̾ƴ϶�, ����� ������ ����� �� �ٸ� Ŭ���̾�Ʈ�� ����ȭ�� ���߾��ֱ����� ����;
	TurnActionAuthed();

	// ���� ��Ŷ ���ۺκ��� ����;
	// ȣ���ϴ� �κп��� �����ִ� ������ �ִ�;

    return dwDxHp;	//CTF �������� ��� ������ �� ������ ó���� ������ �� Ż���ϰ� �ȴ�.
    // ���� �ڵ带 ������ �ǵ帮�� �ʱ� ����, ������ ������ CTF ������ �ڵ带 �ۼ��Ѵ�.
}

// ȸ�� �Լ� ���� [9/26/2013 hsshin];
const DWORD GLCrow::ReceiveHeal(const EMCROW emACrow, const DWORD dwAID, const DWORD dwHeal, const DWORD dwHealFlag)
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return 0;

    DWORD dwActualHeal(dwHeal);
    InstanceSystem::InstanceField* const _pInstance(pLand->GetScriptInstance());
    if ( _pInstance != NULL )  // �δ��� �ƴ� ��� NULL;
    {
        // dwID�� �÷��̾��ϰ�� DBNUM, ��,Summon���� Land������ ��ȿ�� GloabalID�� ���;
        DWORD dwID(m_sAssault.GaeaId);
        if ( m_sAssault.emCrow == CROW_PC )
        {
            GLChar* pChar = m_pGaeaServer->GetChar(m_sAssault.GaeaId);
            if ( pChar )
                dwID = pChar->CharDbNum();
        }

        // ����! : dwHeal�� reference�� ���ڸ� �ѱ�⶧���� ���� ����ɼ� �ִ�. ( ��ũ��Ʈ���� ȸ���� ������ �� �� �ְ� �ϱ� ���� );
        if ( _pInstance->EventReceiveHeal(emACrow, dwID, CROW_MOB, GetGaeaID(), dwActualHeal, dwHealFlag) == false )  // return���� false�� ��� ������ ȸ�� ó���� ���� �ʰڴٴ� ��;
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
        // Note : ������ ģ�ɷ� ���
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

    float fDamageReduce = pDamageSpec->m_fPsyDamageReduce; // ȿ�� ���� ����. by luxes.
    float fDamageReflection = pDamageSpec->m_fPsyDamageReflection * pDamageSpec->m_fPsyDamageReflectionEffRate; // ȿ�� ���� ����. by luxes.
    float fDamageReflectionRate = pDamageSpec->m_fPsyDamageReflectionRate * pDamageSpec->m_fPsyDamageReflectionRateEffRate; // ȿ�� ���� ����. by luxes.
    float fDamageCurse = m_sDamageSpec.m_fDamageCurse * m_sDamageSpec.m_fDamageCurseEffRate; // ȿ�� ���� ����. by luxes.

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
            //	���� ����.
        case SKILL::EMAPPLY_MELEE:
            _fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;
            break;

            //	��Ÿ� ����.
        case SKILL::EMAPPLY_RANGE:
            _fRESIST_G = GLCONST_CHAR::fRESIST_PHYSIC_G;

            fDamageReflection = 0.0f;
            fDamageReflectionRate = 0.0f;
            break;

            //	���� ����.
        case SKILL::EMAPPLY_MAGIC:
            nDEFENSE = 0;						//	���� �����϶��� �Ϲ� ���� ���õ�.
            nDEFAULT_DEFENSE = 0;				//	���� �����϶��� �Ϲ� ���� ���õ�.
            nITEM_DEFENSE = 0;					//	���� �����϶��� �Ϲ� ���� ���õ�.
            _fRESIST_G = GLCONST_CHAR::fRESIST_G;

            fDamageReduce = pDamageSpec->m_fMagicDamageReduce * pDamageSpec->m_fMagicDamageReduceEffRate; // ȿ�� ���� ����. by luxes.
            fDamageReflection = pDamageSpec->m_fMagicDamageReflection * pDamageSpec->m_fMagicDamageReflectionEffRate; // ȿ�� ���� ����. by luxes.
            fDamageReflectionRate = pDamageSpec->m_fMagicDamageReflectionRate * pDamageSpec->m_fMagicDamageReflectionRateEffRate; // ȿ�� ���� ����. by luxes.
            bPsyDamage = false;
            break;
        };

		EMELEMENT emELMT(pSkill->m_sAPPLY.emELEMENT);

        short nRESIST = sRESIST.GetElement ( emELMT );        

		int nVAR(::abs(int(sSKILL_DATA.fBASIC_VAR)));

		{ // �� ��޿� ���� �ݿ���;
			float fGrade(float(wGRADE) / GLCONST_CHAR::fDAMAGE_GRADE_K);
			// �ּ�/�ִ� ������ =  ��ų������ + (�ּ�/�ִ� ������) * �ݿ���
			nDamageLow  += int(float(nDamageLow) * fGrade + nVAR);
			nDamageHigh += int(float(nDamageHigh) * fGrade + nVAR);
		}

		float fResistTotal(float(nRESIST) * 0.01f * _fRESIST_G);
		fResistTotal = fResistTotal > GLCONST_CHAR::fRESIST_MAX_CROW ? GLCONST_CHAR::fRESIST_MAX_CROW : fResistTotal;		

		// �ּ�/�ִ� ������ = ������-(������*��ü���װ�)
		nDamageLow  -= int(float(nDamageLow)  * fResistTotal);
		nDamageHigh -= int(float(nDamageHigh) * fResistTotal);

		// �������� 0 ���� ������ 0 ���� �����Ѵ�.
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

	// Critical �߻� Ȯ��	
	float _fDamageRate = m_fDamageRate;

	if ( pIncreaseEff )
	{
		if ( pIncreaseEff->dwApplyFlag & dwApplyFlag ) // ��ȭ ȿ��
		{	
//			const int nIncreaseCrushingBlow = pIncreaseEff->GetIncreaseCrushingBlowRate(dwApplyFlag); // ���� ��Ÿ�� ���µ�;
			const float fIncreaseRate = float(pIncreaseEff->GetIncreaseDamageRate(dwApplyFlag)) * 0.01f;
			_fDamageRate += fIncreaseRate;
		}
	}

    // ���ݷ�. ��ȭ�� �ݿ�
    m_pGaeaServer->AdjuctDamage(CROW_MOB, nDamageLow, nDamageHigh, _fDamageRate, 0.0f);

    // �߰� ����ġ ����.
    int nExtFORCE = 0;
    int ndxLvl = 0;    
    m_pGaeaServer->AdjustLevel(ndxLvl, nExtFORCE, nLEVEL, GETLEVEL());

    // Critical �߻� Ȯ��.
    bCritical = m_pGaeaServer->CalcCritical(CROW_MOB, GETHP(), GetMaxHP(), ndxLvl);
    
    // Damage �ִ� �ּҰ� ������ ���� Damage ����
    int nDAMAGE_OLD = m_pGaeaServer->CalcRandomDamage(nDamageLow, nDamageHigh, nExtFORCE);

	SIGNORED_DAMAGE* const pIgnoredDamage = pActor->GetIgnoredDamage();
	if ( (pIgnoredDamage != NULL) && (pIgnoredDamage->isAvailable() == true) )
	{
		if ( pIgnoredDamage->AbsorbDamage(nDAMAGE_OLD) == false )
			pActor->SKILL_EFFECT_REMOVE(EMSPECA_IGNORED_DAMAGE);

		if ( nDAMAGE_OLD == 0 )
			return DAMAGE_TYPE_NONE;
	}

    // ���� ���� Damage ����
    rResultDAMAGE = m_pGaeaServer->CalcLowDamage(CROW_MOB, nDAMAGE_OLD, nDEFENSE, fSTATE_DAMAGE);
	

    // �� Damage ����� �ݿ�
    rResultDAMAGE = m_pGaeaServer->CalcDefenseRate(CROW_MOB, nDEFAULT_DEFENSE, nITEM_DEFENSE, rResultDAMAGE, GetLevel());

    rResultDAMAGE = m_pGaeaServer->CalcCriticalDamage(bCritical, false, rResultDAMAGE);

    if (rResultDAMAGE <= 1)
    {
        bCritical = false;
        rResultDAMAGE = 1;
    }

    if (dwDivCount > 1)
        rResultDAMAGE /= dwDivCount;

    // �ߵ� ��ų
    if (sDefenseSkill.m_dwSkillID != NATIVEID_NULL())
    {
        if (sDefenseSkill.m_fRate * sDefenseSkill.m_fRateEffRate > (sc::Random::RANDOM_POS()*1))
        {
            STARGETID sActor(GETCROW(),dwGaeaID);
            pActor->DefenseSkill(sDefenseSkill.m_dwSkillID, sDefenseSkill.m_wLevel, sActor);
        }
    }

    //	������ ���
    if (fDamageReduce > 0.0f) 
    {
        int nDamageReduce = (int) (((rResultDAMAGE * fDamageReduce) * nLEVEL) / GLCONST_CHAR::wMAX_LEVEL);
        rResultDAMAGE -= nDamageReduce;

        if (bPsyDamage)
            dwDamageFlag += DAMAGE_TYPE_PSY_REDUCE;
        else
            dwDamageFlag += DAMAGE_TYPE_MAGIC_REDUCE;
    }

    //	������ �ݻ� ���
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

                // �ǰ��� ���忡�� �������� �ٽ� ������.
                STARGETID sActor(GETCROW(),dwGaeaID);
                pActor->DamageReflectionProc(nDamageReflection, sActor);
            }
        }
    }

    //	���� ���
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
		// ���������� ���� �Ǿ���Ѵ�;
		const float fIgnore_BossDamage(pActor->GetIgnoreBossDamageR());
		if ( fIgnore_BossDamage != 0.0f )
			rResultDAMAGE = rResultDAMAGE - (rResultDAMAGE * fIgnore_BossDamage);
	}

	// ĳ���Ͱ� ���޻����� ��� ���ذ��� ȿ�� üũ�� ���ҽ�Ų��;
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
	//���ӵ����� �ʱ�ȭ
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

	// ��� ���� �ɼ� üũ;
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
		// üũ�ϴ� �г�Ƽ �˻簡 ;
		// EMACTION_TYPE, EMCROWNOPENALTY �ΰ����� �����Ѵ�;
		// ���� ��ų�� ������������ �̷��� ó����;
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
	// Comp ȹ��;
	ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );
	if ( !pCrowComp )
		return ServerActor::GETCLIENTID();

	return pCrowComp->GETCLIENTID();
}

WORD GLCrow::GetSchool() const
{
	// Comp ȹ��;
	const ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< const ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );
	if ( !pCrowComp )
		return ServerActor::GetSchool();

	return pCrowComp->GetSchool();
}

SCONFTING* GLCrow::GETCONFTING()
{
	// Comp ȹ��;
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

	// Comp ȹ��;
	ComponentSystem::ICrowCompField* pCrowComp =
		static_cast< ComponentSystem::ICrowCompField* >(
		GetComponent( ComponentSystem::EMFAMILY_CROW ) );

	// �׼Ǻ� ó���۾�.
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

			// �ɸ��� �̵� ������Ʈ.
			//
			m_actorMove.SetMaxSpeed ( GetMoveVelo() );
			m_actorMove.Update ( pLand->GetvecLayerCollision(), fElapsedTime );

            // ��ֹ����� ������ ���ϰ� ũ�� �� ������ ��ȿ�������� �Ǿ� �־ ���� ���� ���� ���Ǵ� ���� ��� �ּ� ó����;
            // ���� ����ϰ� �ʹٸ� ���� ����°��� ��õ;
// 			if (GetLandMan()->IsObstacle( m_actorMove.Position(), m_actorMove.GetPreviousPosition() ))
// 			{
// 				m_actorMove.Stop ();
// 				SetPositionActor(m_actorMove.GetPreviousPosition());
// 				//SetPosition(m_actorMove.GetPreviousPosition());
// 				//SendGoingMsg(m_actorMove.GetPreviousPosition());
// 
// 				GLMSG::SNETCROW_MOVETO		NetMsg(m_dwGaeaID,
// 					GetActState(),
// 					m_actorMove.GetPreviousPosition(), // m_vPos�� ����� �� �ֱ⶧���� ������.
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

			// Mob�� ���� ��ġ ������Ʈ.
			//
			m_vPosition = m_actorMove.Position();

			// Mob�� ���� ���� ������Ʈ.
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

			// ���� �������� ȸ��.
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
		//	�������Ӹ� ����ǰ� ����
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

			// Mob�� ���� ��ġ ������Ʈ.
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

// End �� ��ġ�� �Ű��ֵ��� ����.
void GLCrow::SkinAniControl_AnimationMove()
{
	if ( !m_pAttackProp )
		return;

	if ( !m_pAttackProp->m_bAnimationMove )
		return;

	GLLandMan* const pLand(GetLandMan());

	if ( pLand == NULL )
		return;

	// 1. ������ġ�� �� �� �ִ��� Ȯ���ϰ� �� �� �ִٸ� �̵��Ѵ�.
	//		�� �� ���ٸ� �ƹ� �������� ������ �Ѵ�. ( Character �� Crow �� �ٸ� )
	// 2. �׸��� �� �� �ִٸ�, ���� ��ġ�� �� �������� �̵��϶�� �����ش�.
	//
	// ���� �۾��� MsgGoto ���� �� �۾��ȴ�.

	D3DXVECTOR3 vPos;
	D3DXMATRIX matRotate;
	float fThetaY = DXGetThetaYFromDirection ( m_vDirect, vDIRORIG );
	D3DXMatrixRotationY ( &matRotate, fThetaY );
	D3DXVec3TransformCoord( &vPos, &m_pAttackProp->m_vAnimationMovePos, &matRotate );

	vPos += GetPositionActor();

	if ( pLand->IsLayerSimpleCollision( GetPositionActor(), vPos, 5.f ) )
	{
		// �̵� �Ұ�
		return;
	}

	SetPositionActor( vPos );
}

void GLCrow::UpdateSkillEff( float fTime, float fElapsedTime )
{
	/// ���� ////////////////////////////////////////////
	if( RF_PROVOKE( m_EffSkillVarSet ).IsOn() ) {
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetHostID();

		if( !sID.ISNULL() && sID != m_TargetID ) NewTarget ( sID, TRUE );
	}

	/// ���عߵ� ////////////////////////////////////////////////
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