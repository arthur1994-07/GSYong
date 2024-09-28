#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxFootColl.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/Occlusion/NSOcclusionCulling.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/GlobalParam.h"
#include "../../EngineLib/DxTools/EditMeshs.h"

#include "../../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/TransformSet/GLTransformSet.h"
#include "../../RanLogic/RANPARAM.h"

#include "../Char/GLCharacter.h"
#include "../Land/GLLandManClient.h"
#include "../Effect/GLFactEffect.h"
#include "../GLGaeaClient.h"
#include "../GLogicExClient.h"
#include "../Strike/GLStrikeM.h"
#include "../Party/GLPartyClient.h"
#include "../Char/SkinCharMulti.h"
#include "./GLCrowClient.h"
#include "../GfxInterfaceBase.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLCrowClient::GLCrowClient(GLGaeaClient* pGaeaClient) 
	: ClientActor(pGaeaClient, CROW_MOB)
	, m_pd3dDevice(NULL)
	, m_pSkinChar(NULL)
	, m_pSkinCharMulti(NULL)
	, m_pTransformSet(NULL)

	, m_bVisible(TRUE)
	, m_fBillboardAlpha(1.f)

	, m_fScale( 1.0f )
	, m_vMaxOrg(8,20,8)
	, m_vMinOrg(-8,0,-8)

	, m_vMax(8,20,8)
	, m_vMin(-8,0,-8)
	, m_fHeight(20.f)

	, m_pLandManClient(NULL)
	, m_fAge(0.0f)

	, m_wTARNUM(0)
	, m_vTARPOS(0,0,0)

	, m_pAttackProp(NULL)
	, m_dwANISUBSELECT(0)

	, m_fattTIMER(0.0f)
	, m_nattSTEP(0)

	, m_fIdleTime(0.0f)
	, m_fStayTimer(0.0f)
	, m_fAttackTimer(0.0f)
	, m_fTargetTimer(0.0f)
	, m_fMoveDelay(0.0f)

	, m_spWorldLight(new ActorWorldLight)
	, m_spNameCollision(new ActorNameCollision(EMCC_CULL_NONE))
{
	m_sTargetID.GaeaId = EMTARGET_NULL;
}

GLCrowClient::~GLCrowClient(void)
{
	FactEffectProcessing( EMFACTEFFECTPROCESSING_DESTRUCTOR );

	m_pd3dDevice = NULL;

	ClientActor::Reset();

	m_pLandManClient = NULL;

	m_sTargetID.RESET();	
	
	SAFE_DELETE(m_pTransformSet);
	SAFE_DELETE(m_pSkinChar);

	if ( m_pSkinCharMulti )
	{
		m_pSkinCharMulti->ResetSkinCharMulti();
		SAFE_DELETE(m_pSkinCharMulti);
	}
}

HRESULT GLCrowClient::CreateCrow ( GLLandManClient *pLandManClient, SDROP_CROW *pCrowDrop, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);
	GASSERT(pLandManClient);

	m_pLandManClient = pLandManClient;
	m_pd3dDevice = pd3dDevice;

	// 몹의 설정 정보를 가져옴.
	m_pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pCrowDrop->sNativeID );
	if( !m_pCrowData )
	{
		SNATIVEID sMobID = pCrowDrop->sNativeID;
		sc::writeLogError(
            sc::string::format(
                "GLCrowClient::CreateCrow()-GLCrowDataMan::GetCrowData() [%1%/%2%] 가져오기 실패.",
                sMobID.wMainID, sMobID.wSubID));
		return E_FAIL;
	}

	GLCROWLOGIC::INIT_DATA ();

	// 이동 제어 초기화.	
	ClientActor::InitializeActorMove(pCrowDrop->vPos, m_pLandManClient->GetNaviMesh(), pCrowDrop->vDir);
	ClientActor::SetMaxSpeed(m_pCrowData->m_sAction.m_fWalkVelo);
	m_fScale = pCrowDrop->fScale;		

	DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pCrowData->GetSkinObjFile(), m_pd3dDevice );
	if ( !pCharData )
	{
		if( strlen( m_pCrowData->GetSkinObjFile() ) )
		{
			sc::writeLogError(
                sc::string::format(
                    "GLCrowClient::CreateCrow()-DxSkinCharDataContainer::LoadData() [%1%] 가져오기 실패.",
                    m_pCrowData->GetSkinObjFile()));
		}
		return E_FAIL;
	}

	SAFE_DELETE(m_pSkinChar);
	SAFE_DELETE(m_pTransformSet);
	m_pTransformSet = new GLTransformSet(pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE);
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData ( pCharData, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );	
    m_pSkinChar->SetInteractiveScale(m_pCrowData->m_sBasic.m_fScale * m_fScale);

	if ( m_pSkinCharMulti )
	{
		m_pSkinCharMulti->ResetSkinCharMulti();
		SAFE_DELETE(m_pSkinCharMulti);
	}
	m_pSkinCharMulti = new SkinCharMulti( pCharData, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	// 현정보 적용.
	SetGaeaID(pCrowDrop->dwGlobalID);

	m_dwNowHP = pCrowDrop->dwNowHP;
	m_wNowMP = pCrowDrop->wNowMP;

    GLCROWLOGIC::SetMultipleHP(pCrowDrop->fMultipleHP);

	GLCROWLOGIC::SetHPRestorationRatePlus(pCrowDrop->fRestorationHP_Plus);
	GLCROWLOGIC::SetMPRestorationRatePlus(pCrowDrop->fRestorationMP_Plus);
	GLCROWLOGIC::SetSPRestorationRatePlus(pCrowDrop->fRestorationSP_Plus);

    for( int i=0; i<SCHOOL_NUM; ++i )
    {
        m_arrHP4School[i] = pCrowDrop->arrHP4School[i];
    }

    m_wSchoolAuthed = pCrowDrop->wSchoolAuthed;

	if ( pCrowDrop->dwActState & EM_REQ_VISIBLENONE )	SetActState ( EM_REQ_VISIBLENONE );
	if ( pCrowDrop->dwActState & EM_REQ_VISIBLEOFF )	SetActState ( EM_REQ_VISIBLEOFF );

	switch ( pCrowDrop->emAction )
	{
	case GLAT_MOVE:
		{
			// 이동 시키기 위해 내부 전달 메시지 발생.
			//
			/*GLMSG::SNETCROW_MOVETO NetMsgGoto;
			NetMsgGoto.dwGlobID = pCrowDrop->dwGlobID;
			NetMsgGoto.dwActState = pCrowDrop->dwActState;
			NetMsgGoto.vCurPos = pCrowDrop->vPos;
			NetMsgGoto.vTarPos = pCrowDrop->TargetID.vPos;

			MsgProcess ( (NET_MSG_GENERIC*) &NetMsgGoto );*/

			// 생성시 이동중인 위치에서 서버와 클라이언트가 서로 다른
			// 루트를 통해 이동하는 문제가 생기므로 수정함.
			if ( pCrowDrop->dwActState&EM_ACT_RUN )
				SetActState ( EM_ACT_RUN );
			else
				ReSetActState ( EM_ACT_RUN );

			m_sTargetID.vPos = pCrowDrop->TargetID.vPos;
			TurnAction ( GLAT_MOVE );
			
			ClientActor::SetPositionActor(pCrowDrop->vStartPos); // 시작위치를 지정해주고			
			ClientActor::GotoLocation(m_sTargetID.vPos, 10.0f); // 웨이포인트를 설정하고			
			ClientActor::SetPositionActor(pCrowDrop->vPos); // 현재의 위치를 지정해주고
			ClientActor::SetNextWayPoint(pCrowDrop->sNextWaypoint); // 다음 웨이포인트를 설정해주고

			// 방향을 업데이트 한다.
			if ( !m_pCrowData->IsDirHold() )
			{				
				const D3DXVECTOR3& vMovement = ClientActor::GetNextPosition();
				if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
				{
					D3DXVECTOR3 vDirection = vMovement - pCrowDrop->vPos;
					if ( !DxIsMinVector(vDirection,0.2f) )
					{
						D3DXVec3Normalize ( &vDirection, &vDirection );
						ClientActor::SetDirect(vDirection);
					}
				}
			}
		}
		break;

    case GLAT_AUTHED:
        TurnAction ( GLAT_AUTHED );
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
		SDROP_SKILLFACT& sDropFact = pCrowDrop->sSKILLFACT[i];
		if (sDropFact.sNATIVEID != NATIVEID_NULL())
		    RECEIVE_SKILLFACT (sDropFact.sNATIVEID, static_cast<WORD>(sDropFact.cLEVEL), sDropFact.wSLOT, sDropFact.sTargetID, SSKILLFACT::STIME(sDropFact) );
	}
	
	// 현재 상태이상 업데이트.
	//
	for (int i=0; i<EMBLOW_MULTI; ++i)
	{
		SSTATEBLOW& sDropBlow = pCrowDrop->sSTATEBLOWS[i];

        // 상태이상 없음.
		if (sDropBlow.emBLOW==EMBLOW_NONE)
            continue;

        // Error
        if ( static_cast<DWORD>(sDropBlow.emBLOW)>=EMBLOW_SIZE)
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

        // Assign inself
		//sDropBlow.emBLOW      = sDropBlow.emBLOW;
		//sDropBlow.fAGE        = sDropBlow.fAGE;
		//sDropBlow.fSTATE_VAR1 = sDropBlow.fSTATE_VAR1;
		//sDropBlow.fSTATE_VAR2 = sDropBlow.fSTATE_VAR2;
        if (pSTATEBLOW)
            pSTATEBLOW->SetData(sDropBlow);
	}

	//	스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect ( 
		STARGETID(GETCROW(), m_dwGaeaID, pCrowDrop->vPos), 
		m_pSkinChar, m_sSKILLFACT, CROW_SKILLFACT_SIZE, m_sSTATEBLOWS, GetTransMatrix(), GetDirect() );

	// 생성 효과 발생
	//
	if ( pCrowDrop->dwFLAGS&SDROP_CROW::CROW_GEM )
	{
		D3DXMATRIX matEffect;
		D3DXMatrixTranslation ( &matEffect, pCrowDrop->vPos.x, pCrowDrop->vPos.y, pCrowDrop->vPos.z );

		std::string strGEM_EFFECT = GLCONST_CHAR::strMOB_GEM_EFFECT.c_str();
		if ( !m_pCrowData->m_sAction.m_strBirthEffect.empty() )
            strGEM_EFFECT = m_pCrowData->m_sAction.m_strBirthEffect;

		STARGETID sTargetID(GETCROW(), m_dwGaeaID, pCrowDrop->vPos);
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
		DEFAULTANI( m_pSkinChar );
 	}

	// 로딩중일수도 있다.
	//if ( !m_pSkinChar->GETCURANIMNODE() )
	//{
	//	sc::writeLogError(
	//		sc::string::format(
	//		"current animation node null point error [%1%] [M %2% S %3%]",
	//		m_pSkinChar->GetFileName(),
	//		m_pSkinChar->GETCURMTYPE(),
	//		m_pSkinChar->GETCURSTYPE()));
	//}

	m_sCQState.Init();

	m_fInfluenceY = pCrowDrop->fInfluenceY;

	return S_OK;
}


void GLCrowClient::UpdateQuestNpcState()
{

	if( !m_sCQState.bQuestNPC ) return;
	

	BOOL bOldViewEffect = m_sCQState.bViewEffect;

	m_sCQState.bViewEffect = FALSE;

	GLCharacter &cMYCHAR = *m_pGaeaClient->GetCharacter();
	CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();

	VEC_CQSTATE_ITER iter = m_sCQState.vecCQState.begin();	
	for ( ; iter != m_sCQState.vecCQState.end(); ++iter)
	{
		if (iter->sNPCComCondition.DoTEST(&cMYCHAR, m_pGaeaClient->GetMyPartyClient()->GetNMember(), cCurSvrTime.GetHour()))
		{		
			m_sCQState.bViewEffect = TRUE;
			break;
		}
	}

	const D3DXVECTOR3& vPosition = GetPosition();
	STARGETID sTargetID(GETCROW(), m_dwGaeaID, vPosition);

	D3DXMATRIX matEffect;
	D3DXMatrixTranslation(&matEffect, vPosition.x, vPosition.y+m_fHeight, vPosition.z);


	if( bOldViewEffect != m_sCQState.bViewEffect )
	{
		if( m_sCQState.bViewEffect )
		{
			m_sCQState.matEffect = matEffect;
			DxEffGroupPlayer::GetInstance().PassiveEffect( GLCONST_CHAR::strCOMPLETE_NPC_EFFECT.c_str(), matEffect, sTargetID );
		}else{
			DxEffGroupPlayer::GetInstance().DeletePassiveEffect( GLCONST_CHAR::strCOMPLETE_NPC_EFFECT.c_str(), sTargetID );
		}
	}
	else
	{
		if( m_sCQState.bViewEffect && m_sCQState.matEffect != matEffect )
		{
			DxEffGroupPlayer::GetInstance().DeletePassiveEffect( GLCONST_CHAR::strCOMPLETE_NPC_EFFECT.c_str(), sTargetID );
			DxEffGroupPlayer::GetInstance().PassiveEffect( GLCONST_CHAR::strCOMPLETE_NPC_EFFECT.c_str(), matEffect, sTargetID );
			m_sCQState.matEffect = matEffect;
		}
	}
}

void GLCrowClient::SetQuestCompleteNPC( const DWORD dwQuestID /* = UINT_MAX */ )
{

	for( size_t i = 0; i < m_sCQState.vecCQState.size(); i++ )
	{
		if( m_sCQState.vecCQState[i].dwQuestID == dwQuestID )
            return;
	}

	m_sCQState.bQuestNPC	= TRUE;
	m_sCQState.bViewEffect	= FALSE;

	CNpcDialogueSet dlgSet;

	if( strlen( m_pCrowData->GetTalkFile() ) == 0 )
        return;
	if( !dlgSet.Load( m_pCrowData->GetTalkFile() ) )
    {
        std::tr1::shared_ptr<NpcTalkMan> spNpcTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
        if ( !spNpcTalkMan )
            return;

        std::tr1::shared_ptr<CNpcDialogueSet> spDlgSet = spNpcTalkMan->Find( m_pCrowData->GetTalkFile() );

        if ( !spDlgSet)
            return;

        dlgSet = *spDlgSet.get();
    }

	GLCharacter &cMYCHAR = *m_pGaeaClient->GetCharacter();
	GLQUESTPROG* pPROG = cMYCHAR.m_cQuestPlay.FindProc ( dwQuestID );
	if( !pPROG ) return;


	DWORD dwTalkSize = dlgSet.GetTalkSize();
	for( DWORD i = 0; i < dwTalkSize; i++ )
	{
        std::tr1::shared_ptr<SNpcTalk> spNpcTalk = dlgSet.GetTalk(i);
		if (!spNpcTalk)
            continue;
		if (spNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_STEP &&
			spNpcTalk->m_dwACTION_PARAM1 == dwQuestID &&
			spNpcTalk->m_dwACTION_PARAM2 == pPROG->m_dwSTEP)
		{
			if (spNpcTalk->GetCondition())
			{
				m_sCQState.AddCQState(dwQuestID, *spNpcTalk->GetCondition());
			}
			break;
		}
	}
}

void GLCrowClient::SetQuestCompleteNPC(const std::vector<DWORD>& vecQuestID)
{
	
	m_sCQState.Init();

	if( vecQuestID.size() == 0 )
	{
		const D3DXVECTOR3& vPosition = GetPosition();
		STARGETID sTargetID(GETCROW(), m_dwGaeaID, vPosition);

		D3DXMATRIX matEffect;
		D3DXMatrixTranslation ( &matEffect, vPosition.x, vPosition.y, vPosition.z );

		DxEffGroupPlayer::GetInstance().DeletePassiveEffect( GLCONST_CHAR::strCOMPLETE_NPC_EFFECT.c_str(), sTargetID );
		return;
	}

	for( size_t i = 0; i < vecQuestID.size(); i++ )
	{
		SetQuestCompleteNPC( vecQuestID[i] );
	}	
}

 

WORD GLCrowClient::GetBodyRadius ()
{
	return GETBODYRADIUS();
}

// void GLCrowClient::SetPosition ( D3DXVECTOR3 &vPos )
// { // QQQ
// 	GASSERT(m_pCrowData);
// 	
// 	ClientActor::SetPosition(vPos);
// 	m_actorMove.SetPosition ( vPos, -1 );
// 	if ( m_actorMove.PathIsActive() )
//         m_actorMove.Stop();
// }

float GLCrowClient::GetMoveVelo ()
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
	//float fVELO = IsSTATE(EM_ACT_RUN) ? m_pCrowData->m_sAction.m_fRunVelo : m_pCrowData->m_sAction.m_fWalkVelo;
	//return fVELO * GLCROWLOGIC::GETMOVEVELO();
}

// BOOL GLCrowClient::IsValidBody () const
// {
// 	return ( m_Action < GLAT_FALLING );
// }

// BOOL GLCrowClient::IsDie ()
// {
// 	if ( isAction(GLAT_DIE) )
// 		return TRUE;
// 
// 	return FALSE;
// }

void GLCrowClient::UpdateActState(void)
{
	if ( IsActState( EM_ACT_EMERGENCY ) )
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GETMAXHP()))
			, fCheckRate(m_pCrowData->IsBoss() ? GLCONST_CHAR::fEmergency_BossHPR : GLCONST_CHAR::fEmergency_MobHPR);
		if ( fCheckRate < fHPRate )
		{
			ReSetActState( EM_ACT_EMERGENCY );
		}
	}
	else
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GETMAXHP()))
			, fCheckRate(m_pCrowData->IsBoss() ? GLCONST_CHAR::fEmergency_BossHPR : GLCONST_CHAR::fEmergency_MobHPR);
		if ( fHPRate <= fCheckRate )
		{
			SetActState( EM_ACT_EMERGENCY );
		}
	}
}

BOOL GLCrowClient::IsHaveVisibleBody ()
{
	return ( m_pCrowData->GetSkinObjFile() && (strlen(m_pCrowData->GetSkinObjFile())!=0) );
}

const BOOL GLCrowClient::CalcVisibleDetect(const float fTime)
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

const BOOL	GLCrowClient::CalcVisibleDetectAuto(const float fTime, const float fDistance, const D3DXVECTOR3& vDistance)
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

BOOL GLCrowClient::IsNonDamaged () const
{
	if( m_pCrowData )
		return m_pCrowData->m_bNonDamaged;
    else
	    return FALSE;
}

void GLCrowClient::TurnAction ( EMACTIONTYPE toAction )
{
	// 이전 액션 취소(쓰러질때와 죽을때는 제외)
	//
	const EMACTIONTYPE& emActionType = GetAction();
	switch ( emActionType )
	{
	case GLAT_IDLE:
		break;

	case GLAT_FALLING:
		if ( toAction==GLAT_FALLING )
			return;
		break;
		
	case GLAT_DIE:
		if ( toAction==GLAT_FALLING || toAction==GLAT_DIE )
			return;
		break;
	case GLAT_SKILL_WAIT:
	case GLAT_SKILL:
		if ( toAction==GLAT_MOVE )
			return;
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
		m_fIdleTime = 0.0f;
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:

		// 같은 애니메이션으로 공격하면 애니메이션이 중간 중간 안나오는 경우가 생겨서, 리셋을 해준다.
		if ( m_pSkinChar )
		{
			SELECTANI( m_pSkinChar, AN_GUARD_N );
		}

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
		StartSkillProc ();
		break;

	case GLAT_FALLING:
		FactEffectProcessing(  EMFACTEFFECTPROCESSING_DEAD );
		break;

    case GLAT_AUTHED:
        FactEffectProcessing( EMFACTEFFECTPROCESSING_AUTH );
        break;

	case GLAT_DIE:
		{
			FactEffectProcessing( EMFACTEFFECTPROCESSING_DEAD );

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

			STARGETID sTargetID(GETCROW(), m_dwGaeaID, vPosition);
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
	
	if ( ClientActor::PathIsActive() )
	{
		if ( !isAction(GLAT_MOVE, GLAT_PUSHPULL) )
			ClientActor::StopActorMove();
	}
}

void GLCrowClient::StartAttackProc ()
{
	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;
}

void GLCrowClient::AttackEffect ( const SANIMSTRIKE &sStrikeEff )
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

	EMELEMENT emELEMENT = STATE_TO_ELEMENT ( m_pAttackProp->emBLOW_TYPE );

	if ( m_pAttackProp->emAttRgType == EMATT_LONG )
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
				spEffSingleG->AddEffAfter ( EFFASINGLE, GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT) );

				if ( !m_pAttackProp->strTargBodyEffect.empty() )
					spEffSingleG->AddEffAfter ( EFFABODY, std::string(m_pAttackProp->strTargBodyEffect.c_str()) );

				spEffSingleG->AddEffAfter ( EFFABODY, GLCONST_CHAR::strAMBIENT_EFFECT );
			}
			else
			{
				DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT), TRUE, FALSE );

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

BOOL GLCrowClient::AttackProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime * GETATTVELO();

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

void GLCrowClient::StartSkillProc ()
{
	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;

	if ( RENDERPARAM::g_bForceLowHardwareMode )
		return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )							return;

    // Mob의 현재 방향 업데이트.
	if ( !m_pCrowData->IsDirHold() )
	{
        // 목표물에 따라 방향을 미리 회전한다.
		const D3DXVECTOR3 vReDirect = 
			UpdateSkillDirection(m_pGaeaClient, GetPosition(), GetDirect(), m_idACTIVESKILL, m_vTARPOS, m_sTARIDS);
		ClientActor::SetDirect(vReDirect);
    }

	const SCROWATTACK &sATTACK = m_pCrowData->m_vecCrowAttack[m_dwAType];
	const ANIMCONTNODE* pAniContNode = m_pSkinChar->findanicontnode ( sATTACK.strAniFile.c_str() );
	if ( !pAniContNode )					
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

			// 캐릭터 visible off 상태라면 제외
			if( CROW_PC == m_sTARIDS[ i ].emCrow )
			{
				if( m_pGaeaClient->GetCharacter()->GetGaeaID() == m_sTARIDS[ i ].dwID )
				{
					if( true == m_pGaeaClient->GetCharacter()->IsActState( EM_REQ_VISIBLEOFF ) )
						continue;
				}
				else
				{
					GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
					std::tr1::shared_ptr< GLCharClient > pChar = pLandMan->GetChar( m_sTARIDS[ i ].dwID );
					if( NULL == pChar )
						continue;

					if( true == pChar->IsActState( EM_REQ_VISIBLEOFF ) )
						continue;
				}
			}

			if ( !m_pGaeaClient->IsVisibleCV(sTARGET) )	continue;
			sTARGET.vPos = m_pGaeaClient->GetTargetPos(sTARGET);

			if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_FIRST )
				SK_EFF_TARZONE ( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_FIRST )
				SK_EFF_TARZONE ( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );
		}
	}

}


void GLCrowClient::StartSkillDealyAction(const SCHARSKILL& sSkill, SCROWDATA* pCrowData, const SKILLTARGETDATA* pTargetData, bool bSubSkill )
{
	m_nattSTEP = 0;
	m_fattTIMER = 0.0f;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkill.sNativeID.wMainID, sSkill.sNativeID.wSubID );
	if ( !pSkill )
		return;

	STARGETID sTARG(GETCROW(), m_dwGaeaID);

	EMELEMENT emELMT(EMELEMENT_SPIRIT);

	// 스킬 시작과 동시에 나타나는 이팩트. 발동.
	//
	if ( pSkill->m_sEXT_DATA.emSELFBODY==SKILL::EMTIME_FIRST )
		DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &GetDirect() );

	SANIMSTRIKE sStrike;
	sStrike.m_emPiece = PIECE_RHAND;
	sStrike.m_emEffect = EMSF_TARGET;
	sStrike.m_dwFrame = 0;

	if ( pSkill->m_sEXT_DATA.emSELFZONE01==SKILL::EMTIME_FIRST )
		SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE02==SKILL::EMTIME_FIRST )
		SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	WORD				wNumTarIds	= pTargetData->nTarget;
	STARID*				pTarIds		= (STARID*)pTargetData->TargetIDs;
	if( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SPEC )
	{
		EMELEMENT emELMT(EMELEMENT_SPIRIT);

		for ( WORD i=0; i<wNumTarIds; ++i )
		{
			STARGETID sTARGET;
			sTARGET.emCrow = pTarIds[i].GETCROW();
			sTARGET.GaeaId = pTarIds[i].GETID();

			// 캐릭터 visible off 상태라면 제외
			if( CROW_PC == pTarIds[ i ].emCrow )
			{
				if( m_pGaeaClient->GetCharacter()->GetGaeaID() == pTarIds[ i ].dwID )
				{
					if( true == m_pGaeaClient->GetCharacter()->IsActState( EM_REQ_VISIBLEOFF ) )
						continue;
				}
				else
				{
					GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
					std::tr1::shared_ptr< GLCharClient > pChar = pLandMan->GetChar( pTarIds[ i ].dwID );
					if( NULL == pChar )
						continue;

					if( true == pChar->IsActState( EM_REQ_VISIBLEOFF ) )
						continue;
				}
			}

			if ( !m_pGaeaClient->IsVisibleCV(sTARGET) )
				continue;
			sTARGET.vPos = m_pGaeaClient->GetTargetPos(sTARGET);

			if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_FIRST )
				SK_EFF_TARZONE ( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_FIRST )
				SK_EFF_TARZONE ( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );
		}
	}
}

void GLCrowClient::SKT_EFF_HOLDOUT (const STARGETID& sTarget , DWORD dwDamageFlag )
{
	for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
		if (sSKEFF.sNATIVEID==NATIVEID_NULL())
            continue;

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

				if ( (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_CURSE && 
					dwDamageFlag & DAMAGE_TYPE_CURSE ) )
				{
					bEffect = true;
					break;
				}

				if ( (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_ILLUSION && 
					 dwDamageFlag & DAMAGE_TYPE_ILLUSION ) )
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

void GLCrowClient::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
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

void GLCrowClient::SK_EFF_TARZONE ( const STARGETID &_sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF )
{
	D3DXMATRIX matTrans;
	STARGETID sTarget = _sTarget;

	const D3DXVECTOR3 vPosition = GetPosition();

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

void GLCrowClient::SK_EFF_TARG ( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &_sTarget )
{
	D3DXMATRIX matTrans;
	STARGETID sTarget = _sTarget;

	const D3DXVECTOR3& vPosition = GetPosition();

	//	스트라이크 지점에서.
	BOOL bTRANS(FALSE);
	if ( pSkill->m_sEXT_DATA.emTARG_POSA == SKILL::EMPOS_STRIKE )
	{
		//	타겟의 위치.
		D3DXVECTOR3 vSTRPOS ( sTarget.vPos.x, sTarget.vPos.y+15, sTarget.vPos.z );		

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
		D3DXVECTOR3 vTARPOS ( vPosition.x, vPosition.y + 15.0f, vPosition.z );;

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

    // V607 Ownerless expression 'sTarget.vPos'. glcrowclient.cpp 1149
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
			DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, FALSE );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_TARG_OVR )
			DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, FALSE );

		//// 목표 몸 이펙트.
		//if ( pSkill->m_sEXT_DATA.emTARGBODY01==SKILL::EMTIME_TARG_OVR )
		//	spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT) );

		//if ( pSkill->m_sEXT_DATA.emTARGBODY02==SKILL::EMTIME_TARG_OVR )
		//	spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT) );
	}
}

void GLCrowClient::SkillEffect ( const SANIMSTRIKE &sStrikeEff )
{
	if ( RENDERPARAM::g_bForceLowHardwareMode )
		return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )							return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	const D3DXVECTOR3& vPosition = GetPosition();
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
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, FALSE );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_TARG_OVR )
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, FALSE );
			}
		}

		if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE  );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE );
	}
	// 타겟 이펙트가 지면을 향해 발사되는 경우.
	//		자기 위치에서 대상 위치까지. ( 화살 관통 )
	else if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
	{
		if ( m_wTARNUM == 0 )	return;

		STARGETID sTarget(m_sTARIDS[m_wTARNUM-1].emCrow, m_sTARIDS[m_wTARNUM-1].dwID);
		if ( !m_pGaeaClient->IsVisibleCV(sTarget) )
			return;
		sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

		D3DXVECTOR3 vDir = sTarget.vPos - vPosition;
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
				if ( !m_pGaeaClient->IsVisibleCV(sTarget) )
					continue;

				// 캐릭터 visible off 상태라면 제외
				if( CROW_PC == m_sTARIDS[ i ].emCrow )
				{
					if( m_pGaeaClient->GetCharacter()->GetGaeaID() == m_sTARIDS[ i ].dwID )
					{
						if( true == m_pGaeaClient->GetCharacter()->IsActState( EM_REQ_VISIBLEOFF ) )
							continue;
					}
					else
					{
						GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
						std::tr1::shared_ptr< GLCharClient > pChar = pLandMan->GetChar( m_sTARIDS[ i ].dwID );
						if( NULL == pChar )
							continue;

						if( true == pChar->IsActState( EM_REQ_VISIBLEOFF ) )
							continue;
					}
				}

				sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

				D3DXVECTOR3 vDir = sTarget.vPos - vPosition;
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

void GLCrowClient::SkillEffectDealyAction( const SANIMSTRIKE &sStrikeEff, SNATIVEID sSkillId, SKILLTARGETDATA *sSkillTargetData )
{
	if ( RENDERPARAM::g_bForceLowHardwareMode )
		return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillId.wMainID, sSkillId.wSubID );
	if ( !pSkill )
		return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	const D3DXVECTOR3& vPosition = GetPosition();
	// 타겟 이펙트가 지면을 향해 발사되는 경우.
	//
	EMELEMENT emELMT(EMELEMENT_SPIRIT);
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_ZONE )
	{
		if ( pSkill->m_sEXT_DATA.emTARG == EMTIME_IMPACT )
		{
			const D3DXVECTOR3& vPosition = sSkillTargetData->TargetID.vPos;
			D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
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
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, FALSE );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_TARG_OVR )
					DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, FALSE );
			}
		}

		if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE  );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE );
	}
	// 타겟 이펙트가 지면을 향해 발사되는 경우.
	//		자기 위치에서 대상 위치까지. ( 화살 관통 )
	else if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
	{
		if ( sSkillTargetData->nTarget == 0 )	return;

		STARGETID sTarget(sSkillTargetData->TargetIDs[sSkillTargetData->nTarget-1].emCrow, sSkillTargetData->TargetIDs[sSkillTargetData->nTarget-1].dwID);
		if ( !m_pGaeaClient->IsVisibleCV(sTarget) )
			return;
		sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

		D3DXVECTOR3 vDir = sTarget.vPos - vPosition;
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
		if ( sSkillTargetData->nTarget )
		{
			for ( WORD i=0; i<sSkillTargetData->nTarget; ++i )
			{
				STARGETID sTarget(sSkillTargetData->TargetIDs[i].emCrow, sSkillTargetData->TargetIDs[i].dwID);
				if ( !m_pGaeaClient->IsVisibleCV(sTarget) )
					continue;

				// 캐릭터 visible off 상태라면 제외
				if( CROW_PC == sSkillTargetData->TargetIDs[ i ].emCrow )
				{
					if( m_pGaeaClient->GetCharacter()->GetGaeaID() == sSkillTargetData->TargetIDs[ i ].dwID )
					{
						if( true == m_pGaeaClient->GetCharacter()->IsActState( EM_REQ_VISIBLEOFF ) )
							continue;
					}
					else
					{
						GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
						std::tr1::shared_ptr< GLCharClient > pChar = pLandMan->GetChar( sSkillTargetData->TargetIDs[ i ].dwID );
						if( NULL == pChar )
							continue;

						if( true == pChar->IsActState( EM_REQ_VISIBLEOFF ) )
							continue;
					}
				}

				sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

				D3DXVECTOR3 vDir = sTarget.vPos - vPosition;
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

				if ( pSkill->m_sEXT_DATA.bTARG_ONE )
					break;
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

BOOL GLCrowClient::SkillProc ( float fElapsedTime )
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

HRESULT GLCrowClient::UpdateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	BOOL bLowSP(false);
	BOOL bFreeze = FALSE;

	DxSkinChar* pRenderChar = NULL;
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	if ( pRenderChar == NULL )
		return E_FAIL;

	switch ( GetAction() )
	{
	case GLAT_IDLE:
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
			if ( m_sHALLUCINATE.bEmoticon )
			{
				if ( !IsANI_SELECTANI( pRenderChar,AN_GESTURE, m_sHALLUCINATE.emAni_SubType) )
					SELECTANI( pRenderChar, emMType, emSType );
			}
			else
				SELECTANI( pRenderChar, emMType, emSType );
		}
		break;

	case GLAT_TALK:
		SELECTANI( pRenderChar, AN_TALK, AN_SUB_NONE );
		if ( pRenderChar->IsANI_ISENDANIM( AN_TALK, AN_SUB_NONE ) )
			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_MOVE:
		if ( IsActState(EM_ACT_RUN) )
			SELECTANI( pRenderChar, AN_RUN, AN_SUB_NONE );
		else
			SELECTANI( pRenderChar, AN_WALK, AN_SUB_NONE );
		break;

	case GLAT_ATTACK:
		if ( IsANI_SELECTANI ( pRenderChar, m_pAttackProp->strAniFile.c_str() ) )
		{
			if ( pRenderChar->IsANI_ISENDANIM( m_pAttackProp->strAniFile.c_str() ) )
				TurnAction ( GLAT_IDLE );
		}
		else
		{
			SELECTANI(pRenderChar, AN_ATTACK, AN_SUB_NONE);

			if ( pRenderChar->IsANI_ISENDANIM( AN_ATTACK, AN_SUB_NONE ) )
				TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_SKILL:
		if ( IsANI_SELECTANI ( pRenderChar, m_pAttackProp->strAniFile.c_str() ) )
		{
			if ( pRenderChar->IsANI_ISENDANIM( m_pAttackProp->strAniFile.c_str() ) )
				TurnAction ( GLAT_IDLE );
		}
		else
		{
			SELECTANI(pRenderChar, AN_ATTACK, AN_SUB_NONE);

			if ( pRenderChar->IsANI_ISENDANIM( AN_ATTACK, AN_SUB_NONE ) )
				TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_SHOCK:
		SELECTANI( pRenderChar, AN_SHOCK, AN_SUB_NONE );
//		if ( m_pSkinChar->IsANI_ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:
		if ( !pRenderChar->IsANI_SELECTANI ( AN_SPEC, (EMANI_SUBTYPE)m_dwANISUBSELECT ) )
		{
			SELECTANI( pRenderChar, AN_GUARD_N, AN_SUB_NONE );
		}
		break;

	case GLAT_FALLING:
		SELECTANI( pRenderChar, AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		if ( pRenderChar->IsANI_ISENDANIM( AN_DIE, AN_SUB_NONE ) )
			TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		bFreeze = TRUE;
		SELECTANI( pRenderChar, AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		TOENDTIME( pRenderChar );
		break;
	case GLAT_GATHERING:
		break;
	};

	// Mob 스킨 업데이트.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( GetAction() )
	{
	case GLAT_MOVE:
		fSkinAniElap *= GLCROWLOGIC::GETMOVEVELO();
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		fSkinAniElap *= GETATTVELO();
		break;

	case GLAT_FALLING:
	case GLAT_DIE:
		m_fBillboardAlpha -= fElapsedTime;
		if ( m_fBillboardAlpha < 0.f )
			m_fBillboardAlpha = 0.f;
		break;
	};

	SetPositionSkin ( pRenderChar, GetPosition() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight() + m_fInfluenceY, 0.0f) );

	if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME )
	{
		// [shhan][2014.12.19] 이걸 하지 않으면 m_vecReserveFunction 에 값이 계속 누적되어서 메모리 부족현상이 나타난다.
		//
		// 렌더하지 않는다.
		//
		pRenderChar->ClearReserveAction();
	}
	else if ( RANPARAM::emAroundDetail == EM_AROUND_BOARD || RENDERPARAM::g_bForceLowHardwareMode )
	{
		if ( pRenderChar->IsExistBillboard() )
		{
			// [shhan][2014.12.19] 이걸 하지 않으면 m_vecReserveFunction 에 값이 계속 누적되어서 메모리 부족현상이 나타난다.
			//
			// 렌더하지 않는다.
			//
			pRenderChar->ClearReserveAction();
		}
		else
		{
			FrameMoveSkin ( pRenderChar, fTime, fSkinAniElap, FALSE, m_matTrans, TRUE, bFreeze );
		}
	}
	else
	{
		FrameMoveSkin ( pRenderChar, fTime, fSkinAniElap, FALSE, m_matTrans, TRUE, bFreeze );
	}

	return S_OK;
}

BOOL GLCrowClient::IsCollisionVolume ()
{
	const CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume();
	if ( COLLISION::IsCollisionVolume ( CV, m_vMax, m_vMin ) )	return TRUE;

	return FALSE;
}

HRESULT GLCrowClient::FrameMove ( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;
	GASSERT(m_pCrowData);
	GASSERT(m_pLandManClient);
	m_pAttackProp = &(m_pCrowData->m_vecCrowAttack[m_dwAType]);

	// 공격 목표의 유효성 검사.
	//		(주의) 타갯정보 사용전 반드시 호출하여 유효성 검사 필요.
	//
	ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( m_sTargetID );
	if ( !pTarget )
	{
		m_sTargetID.GaeaId = EMTARGET_NULL;
		if ( isAction(GLAT_ATTACK) )
			TurnAction ( GLAT_IDLE );
	}
	else
	{
		if( pTarget->GetCrow() == CROW_PC && pTarget->IsNonTargetable() )
		{
			if ( !isAction(GLAT_FALLING, GLAT_DIE) )
				TurnAction(GLAT_IDLE);
		}
	}

	m_sAirborne.update(fElapsedTime);
	if ( IsValidBody() )
	{
		GLCROWLOGIC::UPDATE_DATA ( fTime, fElapsedTime, TRUE );
		UpdateActState();
	}

	// 에니메이션, 스킨 업데이트.
	//
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

	switch ( m_sDOMINATION.emController )
	{	
	case EMSPEC_DOMINATE_TYPE_FIX:
		if ( !isAction(GLAT_FALLING, GLAT_DIE) )
			TurnAction(GLAT_IDLE);
		break;
	}

	UpdateAnimation( fTime, fElapsedTime );

	switch ( GetAction() )
	{
	case GLAT_MOVE:
		{
			// 케릭의 이동 업데이트.
			//
			ClientActor::SetMaxSpeed(GetMoveVelo ());
			ClientActor::Update(m_pGaeaClient->GetLandManClient()->GetvecLayerCollision(), fElapsedTime);		

			if (m_pGaeaClient->GetLandManClient()->IsObstacle( m_actorMove.Position(), m_actorMove.GetPreviousPosition() ))
			{
				//m_actorMove.Stop ();
				ClientActor::SetPositionActor(m_actorMove.GetPreviousPosition());
				//ClientActor::SetPosition(m_actorMove.GetPreviousPosition());
				m_actorMove.Stop ();
			}

			if ( !ClientActor::PathIsActive() )
			{
				ClientActor::StopActorMove();
				TurnAction ( GLAT_IDLE );
			}

			// 케릭의 현제 위치 업데이트.
			//
			//const D3DXVECTOR3& vPosition = ClientActor::RefreshPosition(0.0f);

			// Mob의 현재 방향 업데이트.
			if ( !m_pCrowData->IsDirHold() )
			{
				D3DXVECTOR3 vMovement = ClientActor::GetNextPosition();
				if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
				{
					D3DXVECTOR3 vDirection = vMovement - ClientActor::GetPositionActor();
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

            // Mob의 현재 방향 업데이트.
			if ( !m_pCrowData->IsDirHold() )
			{
			    // 공격 방향으로 회전.
			    //
			    ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( m_sTargetID );
			    if ( pTarget )
			    {
				    D3DXVECTOR3 vDirection = pTarget->GetPositionActor() - GetPositionActor();
				    D3DXVec3Normalize ( &vDirection, &vDirection );
					ClientActor::SetDirect(vDirection);
			    }
            }
		}
		break;

	case GLAT_SKILL:
		{
			SkillProc ( fElapsedTime );

            // Mob의 현재 방향 업데이트.
	        if ( !m_pCrowData->IsDirHold() )
	        {
                // 목표물에 따라 방향을 미리 회전한다.
				const D3DXVECTOR3 vReDirect = 
					UpdateSkillDirection(m_pGaeaClient, GetPositionActor(), GetDirect(), m_idACTIVESKILL, m_vTARPOS, m_sTARIDS);
				ClientActor::SetDirect(vReDirect);
            }
		}
		break;
	case GLAT_SHOCK:
		TurnAction( GLAT_IDLE );
		break;
	case GLAT_PUSHPULL:
		{
			//m_actorMove.SetMaxSpeed ( GLCONST_CHAR::fPUSHPULL_VELO );
			ClientActor::Update(m_pGaeaClient->GetLandManClient()->GetvecLayerCollision(), fElapsedTime);
			
			if ( !ClientActor::PathIsActive() )
			{				
				if ( m_pSkinChar->IsCurAniConInfo(ACF_LOOP) || pRenderChar->ISENDANIM () )
				{
					ClientActor::StopActorMove();
					TurnAction ( GLAT_IDLE );
				}
			}
		}
		break;
	};

	// [AnimationMove]
	SkinAniControl_AnimationMove();

	// [shhan][2015.01.27] RefreshPosition 를 안해주니 PUSHPULL 될 시 캐릭터 형상과 이름, 타켓팅 되는 위치가 따로 분리되는 문제가 생겼다.
	//						m_vPosition 것을 없앨 경우는 PUSHPULL Test 도 같이 해봐야 할것 같음.
	ClientActor::RefreshPosition(0.f);

	{
		const BOOL bPrevState = m_bVisible;
		m_bVisible = CalcVisibleDetect(fTime);	
		if ( m_bVisible == FALSE )
			DxEffGroupPlayer::GetInstance().RenderEnable(STARGETID(GETCROW(), m_dwGaeaID), false);
		else if ( m_bVisible != bPrevState )
			DxEffGroupPlayer::GetInstance().RenderEnable(STARGETID(GETCROW(), m_dwGaeaID), true);
	}

	// 스킬 이펙트 업데이트.
	//
	FACTEFF::UpdateSkillEffect(
		m_pGaeaClient,
		STARGETID(GETCROW(), m_dwGaeaID, ClientActor::GetPositionActor()),
		pRenderChar,
		m_sSKILLFACT,
		CROW_SKILLFACT_SIZE,
		m_sSTATEBLOWS);



	//////////////////////////////////////////////////////////////////////////
	// Skin 관련 위치를 사용한다. ( vPositionOfSkin ) ( airbone 적용된 값 )
	//////////////////////////////////////////////////////////////////////////

	//const D3DXVECTOR3& vPosition = ClientActor::RefreshPosition(0.0f);
	//const D3DXVECTOR3& vPosition = GetPosition();
	//const D3DXVECTOR3& vPosition = ClientActor::RefreshPosition(m_pSkinChar->m_fHeightYPos) + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f);
	const D3DXVECTOR3& vPositionOfSkin = ClientActor::GetPositionActor() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight() + m_fInfluenceY, 0.0f);

	// Mob의 현재 위치 업데이트.
	//	
	//ClientActor::RefreshPosition(pRenderChar->m_fHeightYPos);


	DxLandMan* pLandMan(NULL);
	if ( m_pLandManClient )
	{
		pLandMan = m_pLandManClient->GetLandMan();
	}

	// chf에 셋팅되어 있는 것으로 설정된다.
	// 4발 짐승일 경우 캐릭터 툴에서 셋팅된 Matrix 변환 코드에 검사가 필요함.
	D3DXMATRIX matTrans, matYRot;
	if ( pLandMan && m_pSkinChar->GetFootColl()->GetMatrix( matTrans, pLandMan, vPositionOfSkin, GetDirect(), GetDirectOrig() ) )
	{
		SetTransMatrix( matTrans );
	}
	else
	{
		// 현재 위치와 방향으로 Transform 메트릭스 계산.
		//
		D3DXMatrixTranslation ( &matTrans, vPositionOfSkin.x, vPositionOfSkin.y, vPositionOfSkin.z );
		float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
		D3DXMatrixRotationY ( &matYRot, fThetaY );

		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));
	}

	// AABB 계산.
	//
	m_vMax = vPositionOfSkin + m_vMaxOrg;
	m_vMin = vPositionOfSkin + m_vMinOrg;


	if ( IsHideNaming() )
	{
		// 초기화
		m_spNameCollision->SetHideNaming();
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
		// Thread 상으로 넘긴다.
		m_spNameCollision->InitData( spLandMan );
		NSSkinAniThread::AddNameCollision( m_spNameCollision, DxViewPort::GetInstance().GetFromPt(), vPositionOfSkin + D3DXVECTOR3(0.0f, 8.0f, 0.0f) );
	}


	//////////////////////////////////////////////////////////////////////////
	//			ActorWorldLight 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spWorldLight->InitData( spLandMan );
	NSSkinAniThread::AddActorWorldLight( m_spWorldLight, vPositionOfSkin );

	return S_OK;
}

float GLCrowClient::GetDirection ()
{
	return DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
}

HRESULT GLCrowClient::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
	const D3DXVECTOR3& vPosition( GetPosition() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight() + m_fInfluenceY, 0.0f) );
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

	UpdateQuestNpcState();


	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE);

	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
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
		// 평상시 모습이거나 혹은 보드를 타고 있을 경우는 RANPARAM::emAroundDetail 에 따른다.
		if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME )
		{
			// 렌더하지 않는다.
		}
		else if ( RANPARAM::emAroundDetail == EM_AROUND_BOARD || RENDERPARAM::g_bForceLowHardwareMode )
		{
			if ( !pRenderChar->RenderBillboard( matTrans, m_fBillboardAlpha ) )
			{
				RenderMyChar( pd3dDevice, pRenderChar, TRUE, bHALF_VISIBLE );
			}
		}
		else
		{
			RenderMyChar( pd3dDevice, pRenderChar, TRUE, bHALF_VISIBLE );
		}
	}

	if(m_pCrowData->IsBarrier())
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

HRESULT GLCrowClient::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
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

HRESULT GLCrowClient::RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
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

void GLCrowClient::RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pRenderChar, BOOL bOpaqueState, BOOL bHALF_VISIBLE )
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
		m_pSkinCharMulti->RenderMyChar( pd3dDevice, TRUE, bHALF_VISIBLE, m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult, FALSE );
}

HRESULT GLCrowClient::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD )
{
	HRESULT hr;
	if ( !IsVisibleDetect() )
        return S_FALSE;

    if ( IsDie() )
        return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
        return S_OK;
	
	// 그림자 랜더링.
	//
	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )	
		hr = DxShadowMap::GetInstance().RenderShadowCharMob ( pRenderChar, GetTransMatrix(), pd3dDevice, bWorldSystemOLD );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT GLCrowClient::RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	HRESULT hr;
	if ( !IsVisibleDetect() )
		return S_FALSE;

	if ( IsDie() )
		return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
		return S_OK;

	// 그림자 랜더링.
	//
	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )	
		hr = DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight ( pRenderChar, GetTransMatrix(), pd3dDevice, bUsedMaterialSystem, dwSlot );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT GLCrowClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	m_pTransformSet->RestoreDeviceObjects ( pd3dDevice );
	
	return S_OK;
}

HRESULT GLCrowClient::InvalidateDeviceObjects ()
{
	m_pSkinChar->InvalidateDeviceObjects ();
	m_pTransformSet->InvalidateDeviceObjects ();
	return S_OK;
}

void GLCrowClient::ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR)
{
    //! 인증기일때 학원이 없다면 데미지를 주지않는다.
    //  MsgCrowReceiveDamage 에서 따로 처리한다
    if (m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR)
    {
        if ( wSchool >= SCHOOL_NONE )
        {
            return;
        }
    }

	if (dwDamageFlag & DAMAGE_TYPE_SHOCK)
        TurnAction(GLAT_SHOCK);
	
	if (IsCollisionVolume())
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, nDamage, dwDamageFlag, UI_ATTACK, sACTOR, m_sTargetID );
		
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

    if (m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR)
    {
        RECEIVE_DAMAGE_BY_SCHOOL(nDamage, wSchool);
    }
    else
    {
        RECEIVE_DAMAGE(nDamage);
    }	

	SKT_EFF_HOLDOUT ( sACTOR, dwDamageFlag );
}
void GLCrowClient::ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR)
{
	//! 인증기일때 학원이 없다면 데미지를 주지않는다.
	//  MsgCrowReceiveDamage 에서 따로 처리한다
	if (m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR)
	{
		if ( wSchool >= SCHOOL_NONE )
		{
			return;
		}
	}

	if (IsCollisionVolume())
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, nHeal, dwHealFlag, UI_UNDERATTACK, sACTOR, m_sTargetID );
	}

	if (m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR)
	{
		RECEIVE_HEAL_BY_SCHOOL(nHeal, wSchool);
	}
	else
	{
		RECEIVE_HEAL(nHeal);
	}	
}

void GLCrowClient::ReceiveBlocking(const STARGETID& sACTOR)
{
	BOOL bCollision = FALSE;

	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, DAMAGE_TYPE_NONE, UI_ATTACK, sACTOR, m_sTargetID );
		bCollision = TRUE;
	}
}

void GLCrowClient::ReceiveAVoid ( const STARGETID& sACTOR )
{
	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, DAMAGE_TYPE_NONE, UI_ATTACK, sACTOR, m_sTargetID );
	}
}

void GLCrowClient::ReceiveSwing ()
{
	m_pSkinChar->DOSHOCKMIX();
}

void GLCrowClient::RestoreActStateByInvisible()
{
	if ( !IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) )
		return;

	if ( m_sINVISIBLE.bPrevStateRun )
		SetActState(EM_ACT_RUN);
	else
		ReSetActState(EM_ACT_RUN);
}

const BOOL GLCrowClient::RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF )
{
	const size_t nADDON = pSkill->m_sAPPLY.vecADDON.size();
	for( DWORD i = 0; i < nADDON; ++i )
	{		
		SKILL::SIMPACT_ADDON_LEVEL		sAddon;

		switch ( pSkill->m_sAPPLY.vecADDON[i].emADDON )
		{	// 몬스터에게 스킬 효과 비율 적용. by luxes.
		case EMIMPACTA_HITRATE:
		case EMIMPACTA_AVOIDRATE:
		case EMIMPACTA_DAMAGE:
		case EMIMPACTA_DEFENSE:
		case EMIMPACTA_PA:
		case EMIMPACTA_SA:
		case EMIMPACTA_MA:
		case EMIMPACTA_RESIST:
		case EMIMPACTA_STATS:
			sAddon.emADDON		= pSkill->m_sAPPLY.vecADDON[i].emADDON;
			sAddon.fADDON_VAR	= pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel] * pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel];
			sAddon.fRate		= pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel];
			sSKILLEF.vecADDON.push_back( sAddon );
			break;

		case EMIMPACTA_VARHP:
		case EMIMPACTA_VARMP:
		case EMIMPACTA_VARSP:
		case EMIMPACTA_VARAP:
		case EMIMPACTA_DAMAGE_RATE:
		case EMIMPACTA_DEFENSE_RATE:
		case EMIMPACTA_HP_RATE:
		case EMIMPACTA_MP_RATE:
		case EMIMPACTA_SP_RATE:
			sAddon.emADDON		= pSkill->m_sAPPLY.vecADDON[i].emADDON;
			sAddon.fADDON_VAR	= pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel] * pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel];
			sAddon.fRate		= pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel];
			sSKILLEF.vecADDON.push_back( sAddon );
			break;

		default:
			sAddon.emADDON		= pSkill->m_sAPPLY.vecADDON[i].emADDON;
			sAddon.fADDON_VAR	= pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel];
			sAddon.fRate		= pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel];
			sSKILLEF.vecADDON.push_back( sAddon );
			break;
		}; //switch
	} //for

	return nADDON ? TRUE : FALSE;
} //GLCrowClient::RECEIVE_SKILLFACT_Addon

const BOOL GLCrowClient::RECEIVE_SKILLFACT_Basic ( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF )
{
	switch( pSkill->m_sAPPLY.emBASIC_TYPE )
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
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR	= pSkill->m_sAPPLY.sDATA_LVL[wlevel].fBASIC_VAR;
		return TRUE;
		break;
	}; //switch

	return FALSE;
} //GLCrowClient::RECEIVE_SKILLFACT_Basic

void GLCrowClient::AddSkillFact_SpecialAddon_Client(const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF, const bool bUseMobRate)
{
	SKILL::SSPEC_ADDON_LEVEL sAddon;

	sAddon.emSPEC			= emAddon;
	sAddon.sSPEC.fVAR1 	= bUseMobRate == true ? sSPEC.fVAR1 * sSPEC.fRate : sSPEC.fVAR1;
	sAddon.sSPEC.fVAR2 	= bUseMobRate == true ? sSPEC.fVAR2 * sSPEC.fRate2 : sSPEC.fVAR2;
	sAddon.sSPEC.fRate 		= sSPEC.fRate;
	sAddon.sSPEC.fRate2 	= sSPEC.fRate2;
	sAddon.sSPEC.dwFLAG 	= sSPEC.dwFLAG;
	sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

	sSKILLEF.vecSPEC.push_back( sAddon );
}

const BOOL GLCrowClient::RECEIVE_SKILLFACT_AIRBORNE(const SKILL::SSPEC& sSPEC, const SKILL::CDATA_LVL& sSKILL_DATA)
{
	if ( (this->m_bSkillFactsSpecial[EMSPECA_AIRBORNE] == true) && (sSPEC.dwFLAG == 0) )
		return FALSE;

	const float _fTimeLife(sSKILL_DATA.fLIFE);
	const float _fTimeLevitate(sSPEC.fVAR1);
	const float _fHeightMax(sSPEC.fVAR2);	
	this->m_sAirborne.push_back(_fTimeLife, _fTimeLevitate, _fHeightMax);

	return TRUE;
}

const BOOL GLCrowClient::RECEIVE_SKILLFACT_Special( PGLSKILL pSkill, const WORD wlevel, const DWORD dwCount, SSKILLFACT& sSKILLEF, const STARGETID& sID )
{
	BOOL bHold(FALSE);
	for( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
	{
		const EMSPEC_ADDON emAddon(pSkill->m_sAPPLY.vecSPEC[i].emSPEC);
		const SKILL::SSPEC& sSPEC(pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wlevel]);
		switch( emAddon )
		{
		case EMSPECA_LINK_HP:
			{
				bHold = TRUE;
				const DWORD dwLinkHP(static_cast<DWORD>(GETMAXHP() * m_sLINKHP.fRef));
				if ( m_dwNowHP > dwLinkHP )
					m_dwNowHP = dwLinkHP;

				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_AIRBORNE:
			if ( RECEIVE_SKILLFACT_AIRBORNE(sSPEC, pSkill->m_sAPPLY.sDATA_LVL[wlevel]) == TRUE )
			{
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			}
			break;
		case EMSPECA_IGNORED_DAMAGE:
		case EMSPECA_REFDAMAGE:
		case EMSPECA_RANGE_PSY_LONG:
		case EMSPECA_MOVEVELO:
		case EMSPECA_SKILLDELAY:
		case EMSPECA_TARRANGE_ALL:
		case EMSPECA_APPLYRANGE_ALL:
		case EMSPECA_PSY_DAMAGE_REDUCE:
		case EMSPECA_MAGIC_DAMAGE_REDUCE:
		case EMSPECA_PSY_DAMAGE_REFLECTION:
		case EMSPECA_MAGIC_DAMAGE_REFLECTION:
		case EMSPECA_DEFENSE_SKILL_ACTIVE:
		case EMSPECA_CURSE:
			bHold = TRUE;
			AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF, true);
			break;
		case EMSPECA_ENTRANCE_EXIT :
			{
				RECEIVE_SKILLFACT_EntranceExit(pSkill->GetId(), sSPEC, wlevel, sID );
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF, true);
			}
			break;
		case EMSPECA_PROVOKE :
			{	
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF, true);
			}
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			{
				bHold = TRUE;
				AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF, true);
			}
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
		case EMSPECA_NONBLOW:
		case EMSPECA_PIERCE:
		case EMSPECA_ATTACKVELO:
		case EMSPECA_DURATION_DAMAGE:
		case EMSPECA_STUN:
		case EMSPECA_NON_DRUG:
		case EMSPECA_NON_SKILL:
		case EMSPECA_INCREASE_EFF:
		case EMSPECA_TURN_STATE_BATTLE:
		case EMSPECA_TURN_STATE_PEACE:
		case EMSPECA_ILLUSION:
		case EMSPECA_IMMUNE:
		case EMSPECA_STIGMA:
		case EMSPECA_RELEASE_ENDURE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_DOMINATION:
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
		case EMSPECA_MOVEVELO_VALUE:
		case EMSPECA_DAMAGE_RATE_ALL:
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
        case EMSPECA_MOVEVELO_LIMIT:
		case EMSPECA_FEIGN_DEATH :
		case EMSPECA_SCREEN_THROW :
		case EMSPECA_CAMERA_HIT :
			bHold = TRUE;
			AddSkillFact_SpecialAddon_Client(emAddon, sSPEC, sSKILLEF);
			break;        
		}; //switch
	} //for

	return bHold;
} //GLCrowClient::RECEIVE_SKILLFACT_Special

const BOOL GLCrowClient::RECEIVE_SKILLFACT_BasicAction( PGLSKILL pSkill, const WORD wlevel, const SSKILLFACT::STIME sTime, SSKILLFACT& sSKILLEF )
{
	sSKILLEF.sNATIVEID	= pSkill->GetId();
	sSKILLEF.wLEVEL		= wlevel;
	sSKILLEF.bEnable    = TRUE;

    if ( sTime.IsEXPIREDTIME )
    {
        sSKILLEF.SetAgeInfinite();
        sSKILLEF.nEXPIREDTIME = sTime.nEXPIREDTIME;
    }
    else
    {
		SKILL::CDATA_LVL&		sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wlevel];
        if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )	
			sSKILLEF.fAGE = sSKILL_DATA.fLIFE;
        else									   
			sSKILLEF.fAGE = sTime.fAGE;
    } //if..else

	return TRUE;
} //GLCrowClient::RECEIVE_SKILLFACT_BasicAction

const BOOL GLCrowClient::RECEIVE_SKILLCA( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg )
{
	// Note : Normal 타입이 아니면 튕긴다.
	if( EMSKILLFACTTYPE_BY_INDEX(pMsg->wSELSLOT) != EMSKILLFACT_TYPE_NORMAL )
		return FALSE;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pMsg->skill_id );
	if ( !pSkill )
		return FALSE;

	BOOL			bHold = FALSE;
	SSKILLFACT		sSKILLEF;

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

	bHold |= RECEIVE_SKILLFACT_Addon( &tempSkill, pMsg->wLEVEL, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, pMsg->wLEVEL, 0, sSKILLEF, pMsg->sID );

	if ( bHold )
	{
		if( !RECEIVE_SKILLFACT_BasicAction( pSkill, pMsg->wLEVEL, pMsg->sTIME, sSKILLEF ) )
			return FALSE;

		if( pMsg->wSELSLOT < CROW_SKILLFACT_SIZE )
			m_sSKILLFACT[pMsg->wSELSLOT] = sSKILLEF;
	}

	return TRUE;
}

const BOOL GLCrowClient::RECEIVE_SKILLFACT( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg )
{
	// Note : Normal 타입이 아니면 튕긴다.
	if( EMSKILLFACTTYPE_BY_INDEX(pMsg->wSELSLOT) != EMSKILLFACT_TYPE_NORMAL )
		return FALSE;

	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pMsg->skill_id );
	if ( !pSkill )
		return FALSE;

	BOOL			bHold = FALSE;
	SSKILLFACT		sSKILLEF;

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

	bHold |= RECEIVE_SKILLFACT_Basic( &tempSkill, pMsg->wLEVEL, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Addon( &tempSkill, pMsg->wLEVEL, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, pMsg->wLEVEL, 0, sSKILLEF, pMsg->sID );

	if ( bHold )
	{
		if( !RECEIVE_SKILLFACT_BasicAction( pSkill, pMsg->wLEVEL, pMsg->sTIME, sSKILLEF ) )
			return FALSE;

		if( pMsg->wSELSLOT < CROW_SKILLFACT_SIZE )
			m_sSKILLFACT[pMsg->wSELSLOT] = sSKILLEF;
	}

	return TRUE;
}

const BOOL GLCrowClient::RECEIVE_SKILLFACT( const SNATIVEID& skill_id, const WORD wlevel, const DWORD dwSELECT, const STARGETID& sID, const SSKILLFACT::STIME sTime, const DWORD dwCount )
{
	// Note : Normal 타입이 아니면 튕긴다.
	if( EMSKILLFACTTYPE_BY_INDEX(dwSELECT) != EMSKILLFACT_TYPE_NORMAL )
		return FALSE;

	//	스킬 정보 가져옴.
	PGLSKILL		pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
	if( !pSkill )
		return FALSE;

	BOOL					bHold = FALSE;
	SSKILLFACT		sSKILLEF;

	bHold |= RECEIVE_SKILLFACT_Basic( pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Addon( pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( pSkill, wlevel, dwCount, sSKILLEF, sID );

	// 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//		   현재 불리한 효과를 적용불가 기능이 체크되어 있으면 효과가 적용되지 않는다.
	//  Note : bSkillEffectApply - MobNPCEdit에서 설정해놓은 '저항확률'값에 따른 적용 여부;
	// const WORD dwPercentage = sc::Random::getInstance().getPercent();
	// const BOOL bSkillEffectApply = dwPercentage <= m_pCrowData->m_wResist_Probability;
	if ( bHold )//&& !m_pCrowData->m_bNoAllPenalty )
	{
		if( !RECEIVE_SKILLFACT_BasicAction( pSkill, wlevel, sTime, sSKILLEF ) )
			return FALSE;

		if( dwSELECT < CROW_SKILLFACT_SIZE )
			m_sSKILLFACT[dwSELECT] = sSKILLEF;
	} //if

	//GASSERT ( 0 && "지속 스킬정보가 유효하지 않음." );

    return TRUE;
} //GLCrowClient::RECEIVE_SKILLFACT

void GLCrowClient::UPDATE_DATA_END(const DWORD dwSkillfactIndex){
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
		case EMSPECA_HALLUCINATE:
			{
				DxSkinChar* pRenderChar = NULL;
				if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) != NULL )
				{
					DEFAULTANI( pRenderChar );
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

	FACTEFF::DeleteSkillFactEffect(STARGETID(GETCROW(), m_dwGaeaID, GetPosition()), m_pSkinChar, sSKILLFACT.sNATIVEID);
	sSKILLFACT.RESET();
}

// Note : bjju. 상황에 따라 Effect를 처리하는 함수
void GLCrowClient::FactEffectProcessing ( EMFACTEFFPROCESSING emFactEffCase )
{
    // 초기화가 안되었으므로 작업 불필요.
    if ( !m_pCrowData )
        return;

    switch ( emFactEffCase )
    {
    case EMFACTEFFECTPROCESSING_DESTRUCTOR:
    //case EMFACTEFFECTPROCESSING_DEAD:
    //case EMFACTEFFECTPROCESSING_REBIRTH:
    //case EMFACTEFFECTPROCESSING_MOVEMAP:
        {
            FACTEFF::DeleteAllEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), m_pSkinChar, m_sSKILLFACT, CROW_SKILLFACT_SIZE, m_sSTATEBLOWS );

            for ( int i=0; i<EMBLOW_MULTI;   ++i )												
                m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;

            for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )												
                m_sSKILLFACT [i].RESET();
        }
        break;

    case EMFACTEFFECTPROCESSING_DEAD:
    case EMFACTEFFECTPROCESSING_REBIRTH:
    case EMFACTEFFECTPROCESSING_MOVEMAP:
        {
            FACTEFF::DeleteEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), m_pSkinChar, m_sSKILLFACT, CROW_SKILLFACT_SIZE, m_sSTATEBLOWS );

            for ( int i=0; i<EMBLOW_MULTI;   ++i )												
                m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;

            for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )												
                m_sSKILLFACT[i].RESET();
        }
        break;

    case EMFACTEFFECTPROCESSING_AUTH:
        {
			const D3DXVECTOR3& vPosition = GetPosition();

            //! 학원 전용 인증 이펙트를 생성한다.
            STARGETID sTargetID(GETCROW(), m_dwGaeaID, vPosition);

			// 현재 위치와 방향으로 Transform 메트릭스 계산.
			//
			D3DXMATRIX matTrans, matYRot;
			D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
			const float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
			D3DXMatrixRotationY ( &matYRot, fThetaY );

			const D3DXMATRIX matRotTrans = matYRot * matTrans;
			SetTransMatrix(matRotTrans);

            switch( m_wSchoolAuthed )
            {
            case SCHOOL_SM: DxEffGroupPlayer::GetInstance().NewEffGroup( GLCONST_CHAR::strAUTH_SM_EFFECT.c_str(), matRotTrans, &sTargetID, TRUE, FALSE ); break;
            case SCHOOL_HA: DxEffGroupPlayer::GetInstance().NewEffGroup( GLCONST_CHAR::strAUTH_HA_EFFECT.c_str(), matRotTrans, &sTargetID, TRUE, FALSE ); break;
            case SCHOOL_BH: DxEffGroupPlayer::GetInstance().NewEffGroup( GLCONST_CHAR::strAUTH_BH_EFFECT.c_str(), matRotTrans, &sTargetID, TRUE, FALSE ); break;
            };
        }
        break;
    }
}

void GLCrowClient::MsgPushPullBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_PUSHPULL_BRD *pNetMsg = (GLMSG::SNET_PUSHPULL_BRD *)pMsg;
    const D3DXVECTOR3 &vMovePos = pNetMsg->vMovePos;

    // 밀려날 위치로 이동 시도.
    //
	
    const BOOL bSucceed = ClientActor::GotoLocation(vMovePos, 5.0f);

    if ( bSucceed )
    {
        // 밀려나는 엑션 시작.
        //
        m_sTargetID.vPos = vMovePos;
        m_dwANISUBSELECT = pNetMsg->dwAniType;

		TurnAction ( GLAT_PUSHPULL );

        // 밀리는 속도 설정.
        //
        ClientActor::SetMaxSpeed ( pNetMsg->fPUSH_SPEED );
    }
}

void GLCrowClient::MsgCureStateBlowBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_CURESTATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_CURESTATEBLOW_BRD *)pMsg;

	const D3DXVECTOR3& vPosition = GetPosition();

    for ( int i=0; i<EMBLOW_MULTI; ++i )
    {
        EMSTATE_BLOW emBLOW = m_sSTATEBLOWS[i].emBLOW;
        if ( emBLOW==EMBLOW_NONE )		continue;

        EMDISORDER emDIS = STATE_TO_DISORDER(emBLOW);
        if ( !(pNetMsg->dwCUREFLAG&emDIS) )				continue;

        DISABLEBLOW(i);
        FACTEFF::DeleteBlowEffect ( STARGETID(GETCROW(), m_dwGaeaID, vPosition), m_pSkinChar, emBLOW );
    }
}

void GLCrowClient::MsgStateBlowBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_STATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_STATEBLOW_BRD *)pMsg;

	const D3DXVECTOR3& vPosition = GetPosition();

    // "단독상태이상"일 경우 단독 효과 모두 끄기.
    if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )
    {
        FACTEFF::DeleteBlowSingleEffect ( STARGETID(GETCROW(), m_dwGaeaID, vPosition), m_pSkinChar, m_sSTATEBLOWS );

        for ( int i=0; i<EMBLOW_MULTI; ++i ) m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
    }

    // 상태 이상 추가.
    SSTATEBLOW *pSTATEBLOW = NULL;
    if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )
		pSTATEBLOW = &m_sSTATEBLOWS[0];
    else
		pSTATEBLOW = &m_sSTATEBLOWS[pNetMsg->emBLOW-EMBLOW_SINGLE];

    pSTATEBLOW->emBLOW = pNetMsg->emBLOW;
    pSTATEBLOW->fAGE = pNetMsg->fAGE;
    pSTATEBLOW->fSTATE_VAR1 = pNetMsg->fSTATE_VAR1;
    pSTATEBLOW->fSTATE_VAR2 = pNetMsg->fSTATE_VAR2;
    pSTATEBLOW->wSchool = pNetMsg->wSchool;


    // 효과 생성.
    FACTEFF::NewBlowEffect ( STARGETID(GETCROW(), m_dwGaeaID, vPosition), m_pSkinChar, pSTATEBLOW->emBLOW, GetTransMatrix(), GetDirect() );
}

void GLCrowClient::MsgSkillHoldRsBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SKILLHOLD_RS_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_RS_BRD *)pMsg;

	const D3DXVECTOR3& vPosition = GetPosition();

    // 스킬 fact 들을 종료.
    //		바로 리샛 하지 않고 여기서 시간 조종하여 정상 종료되게 함. ( 이팩트 종료 때문. )
    for ( int i=0; i<CROW_SKILLFACT_SIZE; ++i )
    {
        if ( pNetMsg->bRESET[i] && m_sSKILLFACT[i].sNATIVEID != NATIVEID_NULL() )
        {
            FACTEFF::DeleteSkillFactEffect ( STARGETID(GETCROW(), m_dwGaeaID, vPosition), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );

            RESETSKEFF(i);
        }
    }
}

void GLCrowClient::MsgSkillHoldBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SKILLHOLD_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_BRD *)pMsg;

    //	지속형 스킬의 경우 스킬 팩터 추가됨.
    if ( pNetMsg->skill_id != SNATIVEID(false) )
    {
        if ( pNetMsg->fPARAM )
            m_sLINKHP.fRef = pNetMsg->fPARAM; //MaxHP 비율을 서버로 부터 받음

		switch( pNetMsg->sEffInfo.eType )
		{
		case GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_ATTACK :
			{
				RECEIVE_SKILLCA( pNetMsg );
			}
			break;
		case GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_TARGET :
			{
				RECEIVE_SKILLFACT ( pNetMsg );
			}
			break;
		default :
			{
				RECEIVE_SKILLFACT ( pNetMsg->skill_id
					, pNetMsg->wLEVEL
					, pNetMsg->wSELSLOT
					, pNetMsg->sID
					, pNetMsg->sTIME );
				FACTEFF::NewSkillFactEffect (
					STARGETID(GETCROW(), m_dwGaeaID, GetPosition()),
					m_pSkinChar, pNetMsg->skill_id, GetTransMatrix(), GetDirect() );
			}
			break;
		}

        
    }
}

void GLCrowClient::MsgSkillFactBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SKILLFACT_BRD *pNetMsg = (GLMSG::SNETPC_SKILLFACT_BRD *)pMsg;

    //! 인증기라면
    if (m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR)
    {
        EMCROW emCrow   = pNetMsg->sACTOR.GETCROW();
        DWORD  dwGaeaID = pNetMsg->sACTOR.GETID();
        WORD   wSchool  = pNetMsg->wSchool;

        GASSERT( wSchool < SCHOOL_NUM );
        GLOGICEX::VARIATION ( m_arrHP4School[wSchool], GETMAXHP(), pNetMsg->nVAR_HP );
        GLOGICEX::VARIATION ( m_wNowMP, GETMAXMP(), pNetMsg->nVAR_MP );
        GLOGICEX::VARIATION ( m_wNowSP, GETMAXSP(), pNetMsg->nVAR_SP );
    }
    else
    {
        GLOGICEX::VARIATION ( m_dwNowHP, GETMAXHP(), pNetMsg->nVAR_HP );
        GLOGICEX::VARIATION ( m_wNowMP, GETMAXMP(), pNetMsg->nVAR_MP );
        GLOGICEX::VARIATION ( m_wNowSP, GETMAXSP(), pNetMsg->nVAR_SP );
    }

    //	데미지 메시지.
    if ( pNetMsg->nVAR_HP < 0 )
    {
        if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_SHOCK )
			TurnAction ( GLAT_SHOCK );
        else
			ReceiveSwing ();

		BOOL bRenderEffect(TRUE);
		if ( RENDERPARAM::g_bForceLowHardwareMode )
		{
			// 저사양 모드면, Effect 를 나오지 않도록 한다.
			bRenderEffect = FALSE;

			// 하지만 저사양 모드라도, GLCharacter 가 공격한 거면 Effect 가 보이도록 한다.
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
			if ( pCharacter && (pNetMsg->emCrow == CROW_PC ) && (pNetMsg->dwID == pCharacter->GetGaeaID()) )
			{
				bRenderEffect = TRUE;
			}
		}

		// 방어스킬의 이펙트가 있을때 발동시킴.
		STARGETID sACTOR(pNetMsg->sACTOR.GETCROW(),pNetMsg->sACTOR.GETID());
		sACTOR.vPos = m_pGaeaClient->GetTargetPos ( sACTOR );

		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, -pNetMsg->nVAR_HP, 
			pNetMsg->dwDamageFlag, UI_ATTACK, sACTOR, m_sTargetID );

		// Effect 렌더한다.
		if ( bRenderEffect )
		{
			SKT_EFF_HOLDOUT ( sACTOR, pNetMsg->dwDamageFlag );

			if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
			{
				// 강한타격 이펙트

				const D3DXVECTOR3& vPosition = GetPosition();

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

			if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CA )
			{	
				// 강한타격 이펙트
				const D3DXVECTOR3& vPosition = GetPosition();
				STARGETID sMyID(CROW_MOB,m_dwGaeaID, vPosition);

				D3DXMATRIX matTrans;
				D3DXMatrixTranslation(&matTrans, vPosition.x, vPosition.y, vPosition.z );

				matTrans._41 = vPosition.x;
				matTrans._42 = vPosition.y + 10.0f;
				matTrans._43 = vPosition.z;

				//	Note : 자기 위치 이펙트 발생시킴.
				DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCA_EFFECT.c_str(), matTrans, &sMyID, TRUE, FALSE );
			}
		}
		
		RELEASE_ENDURE_PROC();

		m_pGaeaClient->logDamageRecord(sACTOR, -pNetMsg->nVAR_HP, pNetMsg->sSkillID, pNetMsg->wSkillLevel, pNetMsg->dwDamageFlag);
    }
}

void GLCrowClient::MsgActionBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ACTION_BRD *pNetMsg = (GLMSG::SNET_ACTION_BRD *)pMsg;
    TurnAction ( pNetMsg->emAction );
}

void GLCrowClient::MsgCrowDamage(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETCROW_DAMAGE *pNetMsg = (GLMSG::SNETCROW_DAMAGE *) pMsg;

	// dwTarID == '나'일 경우 은신 해제; QQQ
    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
    if ( pActor )
    {
        STARGETID sACTOR(GetCrow(),GetGaeaID(),GetPosition());
        pActor->ReceiveDamage ( pNetMsg->nDamage, pNetMsg->dwDamageFlag, SCHOOL_NONE, sACTOR );

        GLCharacter* pChar = m_pGaeaClient->GetCharacter();
        if( pChar && pChar->GetGaeaID() == pNetMsg->dwTarID )
        {
            if( pNetMsg->emHitCrow == CROW_PC )
                pChar->DurabilityDropHit( pNetMsg->nDamage, TRUE );
            else
                pChar->DurabilityDropHit( pNetMsg->nDamage, FALSE );
                
        }
        
    }
}

void GLCrowClient::MsgCrowAvoid(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETCROW_AVOID *pNetMsg = (GLMSG::SNETCROW_AVOID *) pMsg;
    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
    if ( pActor == NULL )
		return;

	const STARGETID ActorID(m_emCrow, m_dwGaeaID, m_vPosition);
	pActor->ReceiveAVoid( ActorID );
	m_pGaeaClient->logDamageRecord(ActorID, 0, pNetMsg->SkillID, pNetMsg->wSkillLevel);
}

void GLCrowClient::MsgCrowAttackBlocking(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETCROW_BLOCKING *pNetMsg = (GLMSG::SNETCROW_BLOCKING *) pMsg;
    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
    if ( pActor == NULL )
		return;

	const STARGETID ActorID(m_emCrow, m_dwGaeaID, m_vPosition);
	pActor->ReceiveBlocking(ActorID);
	m_pGaeaClient->logDamageRecord(ActorID, 0, pNetMsg->SkillID, pNetMsg->wSkillLevel);
}

void GLCrowClient::MsgCrowSkill(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETCROW_SKILL *pNetMsg = (GLMSG::SNETCROW_SKILL *)pMsg;

    m_dwAType = pNetMsg->dwAType;
    m_idACTIVESKILL = pNetMsg->skill_id;
    m_wACTIVESKILL_LVL = pNetMsg->skill_lev;
    m_vTARPOS = pNetMsg->vTARPOS;

    D3DXVECTOR3 vDist =  pNetMsg->vPos - GetPosition();
    float fDist = D3DXVec3Length(&vDist);
    if ( fDist > 100.0f ) 
    {
        CDebugSet::ToView( 3, 8, "GLAT_SKILL fDist %f", fDist );
		ClientActor::SetPositionActor(pNetMsg->vPos);
    }

    m_wTARNUM = pNetMsg->wTARNUM;
    if( m_wTARNUM > EMTARGET_NET )	
    {
        return;
    }
    else
    {
        // 스킬을 쓴 대상의 첫번째의 타겟을 가져온다
        m_sTargetID.emCrow = pNetMsg->sTARIDS[0].GETCROW();
        m_sTargetID.GaeaId = pNetMsg->sTARIDS[0].GETID();
        m_sTargetID.vPos = pNetMsg->vTARPOS;

        memcpy ( m_sTARIDS, pNetMsg->sTARIDS, sizeof(STARID)*m_wTARNUM );

		//////////////////////////////////////////////////////////////////////////
		// GLAT_SKILL_WAIT 를 사용을 안 했다. 시작과 동시에 나오는 Effect 의 경우 아주 미세한 오차가 생길 수 있다.
		// 
        TurnAction ( GLAT_SKILL );
    }
}

void GLCrowClient::MsgCrowSkillDealyAction( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETCROW_SKILL_DEALY_ACTION *pNetMsg = (GLMSG::SNETCROW_SKILL_DEALY_ACTION *)pMsg;
	pNetMsg->skill_id;
	pNetMsg->skill_lev;
	SCHARSKILL sSkill( pNetMsg->skill_id, pNetMsg->skill_lev);
	SKILLTARGETDATA sTargetData;

	for(int i = 0; i < pNetMsg->wTARNUM; i ++ )
	{
		sTargetData.TargetIDs[i] = pNetMsg->sTARIDS[i];
		sTargetData.nTarget++;
	}
	sTargetData.TargetID = pNetMsg->sTARIDS[0];

	SANIMSTRIKE			sStrike;
	sStrike.m_emPiece	= PIECE_RHAND;
	sStrike.m_emEffect	= EMSF_TARGET;
	sStrike.m_dwFrame	= 0;

	StartSkillDealyAction(sSkill, m_pCrowData, &sTargetData, false );
	SkillEffectDealyAction( sStrike, pNetMsg->skill_id, &sTargetData );
}

void GLCrowClient::MsgCrowEndSkill(NET_MSG_GENERIC* pMsg){
	GLMSG::SNETCROW_END_SKILL* pPacket = (GLMSG::SNETCROW_END_SKILL*)pMsg;
	UPDATE_DATA_END(pPacket->dwSkillfactIndex);
}

void GLCrowClient::MsgCrowUpdateState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETCROW_UPDATE_STATE* _pPacket = (GLMSG::SNETCROW_UPDATE_STATE*) pMsg;
    if ( _pPacket->emAction != GLAT_SIZE )
        TurnAction(_pPacket->emAction);

    m_dwNowHP = _pPacket->dwNowHP;

	switch ( _pPacket->emStatusType )
	{
	case GLMSG::SNETCROW_UPDATE_STATE::EMCHANGESTATUS_TYPE_HPRESTORATION_RATE:
		SetHPRestorationRatePlus( _pPacket->fStatusValue );
		break;
			
	case GLMSG::SNETCROW_UPDATE_STATE::EMCHANGESTATUS_TYPE_MPRESTORATION_RATE:
		SetMPRestorationRatePlus( _pPacket->fStatusValue );
		break;
			
	case GLMSG::SNETCROW_UPDATE_STATE::EMCHANGESTATUS_TYPE_SPRESTORATION_RATE:
		SetSPRestorationRatePlus( _pPacket->fStatusValue );
		break;
			
	case GLMSG::SNETCROW_UPDATE_STATE::EMCHANGESTATUS_TYPE_HP_MULTIPLE:
		SetMultipleHP( _pPacket->fStatusValue );
		break;
	}
}

void GLCrowClient::MsgCrowAttack(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETCROW_ATTACK *pNetMsg = (GLMSG::SNETCROW_ATTACK *) pMsg;


    D3DXVECTOR3 vDist =  pNetMsg->vPos - GetPosition();
    float fDist = D3DXVec3Length(&vDist);
    if ( fDist > 100.0f ) 
    {
        CDebugSet::ToView( 3, 7, "GLAT_ATTACK fDist %f", fDist );
		ClientActor::SetPositionActor(pNetMsg->vPos);
    }

    m_sTargetID.emCrow = pNetMsg->emTarCrow;
    m_sTargetID.GaeaId = pNetMsg->dwTarID;
    m_dwAType = pNetMsg->dwAType;
    TurnAction ( GLAT_ATTACK );

    //CDebugSet::ToListView ( "NET_MSG_GCTRL_CROW_ATTACK" );
}

void GLCrowClient::MoveTo( const D3DXVECTOR3& vPos, const DWORD dwCell, const DWORD dwFlags )
{
	if( dwFlags & EMJUMPTO )
	{
		ClientActor::InitializeActorMove(vPos);
		return;
	} //if

	if(  ( ClientActor::GetCurrentCellID() != dwCell ) &&				// 서버/클라이언트간의 셀이 서로 다르면 정정해주어야 정확한 웨이포인트로 이동하게 된다.
		!( dwFlags & EMTRACING ) )									// 하지만 추격중이면 지정해주지 않는다.	
		ClientActor::SetPositionActor(vPos);
	else
	{
		// 서버와 클라이언트 포지션 동기화 합니다.
		D3DXVECTOR3		vDist =  vPos - GetPosition();
		float			fDist = D3DXVec3LengthSq( &vDist );
		if( fDist > 10000.0f ) 
		{
			CDebugSet::ToView( 3, 6, "GLAT_MOVE fDist(square) %f", fDist );
			ClientActor::SetPositionActor(vPos);
		} //if
	} //if..else
	
    BOOL				bSucceed = ClientActor::GotoLocation( m_sTargetID.vPos, 10.0f );
	//CDebugSet::ToListView ( "NET_MSG_GCTRL_CROW_MOVETO" );
} //GLCrowClient::MoveTo

/**
 * 이동관련 수신 패킷 모두처리.
 */
void GLCrowClient::MsgCrowMoveTo(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETCROW_MOVETO*		pNetMsg = (GLMSG::SNETCROW_MOVETO*) pMsg;

    if( pNetMsg->dwActState&EM_ACT_RUN )
		SetActState( EM_ACT_RUN );
    else
		ReSetActState( EM_ACT_RUN );

    m_sTargetID.emCrow	= pNetMsg->emTarCrow;
    m_sTargetID.GaeaId	= pNetMsg->dwTarID;
    m_sTargetID.vPos	= pNetMsg->vTarPos;
    TurnAction( GLAT_MOVE );

	MoveTo( pNetMsg->vCurPos, pNetMsg->dwCellID, pNetMsg->dwFlags );
}

void GLCrowClient::MsgCrowAuthed(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CTF_CROW_AUTHED *pPacket = (GLMSG::SNET_CTF_CROW_AUTHED *)pMsg;

    if ( m_pCrowData->m_emNPCType != NPC_TYPE_AUTHENTICATOR )
    {
        GASSERT( !"GLCrowClient::MsgCrowAuthed() - crow is not authenticator type" );
    }

    for(int i=0; i<SCHOOL_NUM; i++)
    {
        m_arrHP4School[i] = pPacket->arrHP4School[i];
    }

	bool bEffect = true;

	bEffect =  m_wSchoolAuthed != pPacket->wSchoolAuthed ? true : false;

    m_wSchoolAuthed	= pPacket->wSchoolAuthed;
    
	// #RM 957
	if( m_wSchoolAuthed != SCHOOL_NONE && bEffect )
	{
		//! 이전의 이펙트를 삭제한다.
		STARGETID sTargetID(GETCROW(), m_dwGaeaID, GetPosition());
		DxEffGroupPlayer::GetInstance().DeletePassiveEffect( sTargetID );
		//! 인증 이펙트를 생성한다. bThread = false, 첫 이펙트가 원활하게 보이지 않는다 굿이 스레드를 사용할 필요가 없을듯 하다.
		DxEffGroupPlayer::GetInstance().NewEffGroup( GLCONST_CHAR::strAUTH_EFFECT.c_str(), GetTransMatrix(), &sTargetID, FALSE, FALSE );

		TurnAction( GLAT_AUTHED );
	}    
}

// 스킬 효과 적용;
void GLCrowClient::MsgApplySkillEffect ( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNET_APPLY_SKILLEFFECT_FC* pNetMsg =
		static_cast< GLMSG::SNET_APPLY_SKILLEFFECT_FC* >( pMsg );
	if ( !pNetMsg )
		return;

	//m_bSkillFactsSpecial[ pNetMsg->emSpecAddon ] = pNetMsg->bState;
}

const BOOL GLCrowClient::IsNonTargetable( void ) const 
{
    return  m_bVisible == FALSE ||
            m_sHALLUCINATE.bNonTargetable || 
            (m_pCrowData && m_pCrowData->m_bNonTargeting);
}

const bool GLCrowClient::IsHideNaming() const
{
    return  (m_pCrowData && m_pCrowData->m_bHideNaming);
}

const bool GLCrowClient::IsHideTooltip() const
{
	return ( m_pCrowData && m_pCrowData->m_sBasic.m_bHideTooltip );
}

BOOL GLCrowClient::IsVisibleDisplayName() const
{
	if ( m_spNameCollision->GetCollsion() )
		return FALSE;

	return TRUE;
}

void GLCrowClient::SELECTANI( DxSkinChar* pRenderChar, const TCHAR* szAnim, DWORD dwFlags/*=NULL*/ )
{
	if ( pRenderChar )
		pRenderChar->SELECTANI ( szAnim, dwFlags );

	if ( m_pSkinCharMulti ) 
		m_pSkinCharMulti->SELECTANI ( szAnim, dwFlags );

}

void GLCrowClient::SELECTANI( DxSkinChar* pRenderChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0*/ )
{
	if ( pRenderChar )
		pRenderChar->SELECTANI ( MType, SType, dwFlags, dwSelect );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SELECTANI ( MType, SType, dwFlags, dwSelect );
}

void GLCrowClient::DEFAULTANI( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->DEFAULTANI();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->DEFAULTANI();
}

void GLCrowClient::TOSTARTTIME( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->TOSTARTTIME();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->TOSTARTTIME();
}

void GLCrowClient::TOENDTIME( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->TOENDTIME();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->TOENDTIME();
}

BOOL GLCrowClient::IsANI_SELECTANI( DxSkinChar *pSkinChar, const TCHAR* szAnim, DWORD dwFlags/*=NULL */ )
{
	BOOL bOK = FALSE;
	bOK = pSkinChar->IsANI_SELECTANI ( szAnim, dwFlags );

	//if ( m_pSkinCharMulti )
	//	bOK = m_pSkinCharMulti->IsANI_SELECTANI ( szAnim, dwFlags );

	return bOK;
}

BOOL GLCrowClient::IsANI_SELECTANI( DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0 */ )
{
	BOOL bOK = FALSE;
	bOK = pSkinChar->IsANI_SELECTANI ( MType, SType, dwFlags, dwSelect );

	//if ( m_pSkinCharMulti )
	//	bOK = m_pSkinCharMulti->IsANI_SELECTANI (  MType, SType, dwFlags, dwSelect  );

	return bOK;
}

void GLCrowClient::SetPositionSkin( DxSkinChar *pSkinChar, const D3DXVECTOR3& vPos )
{
	pSkinChar->SetPosition ( vPos );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SetPosition( vPos );
}

HRESULT GLCrowClient::FrameMoveSkin( DxSkinChar *pSkinChar, const float fTime, const float fElapsedTime, BOOL bNoneOverFrame, const D3DXMATRIX &matRot, const BOOL bContinue/*=TRUE*/, BOOL const bFreeze/*=FALSE*/, const BOOL bAttackMode/*=TRUE*/, const BOOL bBoardRender /*= FALSE */ )
{
	HRESULT hr = S_FALSE;
	hr = pSkinChar->FrameMove ( fTime, fElapsedTime, bNoneOverFrame, matRot, bContinue, bFreeze, bAttackMode, bBoardRender );
	if ( E_FAIL == hr )
		return hr;

	if ( m_pSkinCharMulti )
		hr = m_pSkinCharMulti->FrameMove( fTime, fElapsedTime, bNoneOverFrame, matRot , bContinue, bFreeze, bAttackMode, bBoardRender );

	return hr;
}

const bool GLCrowClient::RECEIVE_SKILLFACT_EntranceExit( const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID )
{
	TurnAction(GLAT_IDLE); 
	
	return false;
}

void GLCrowClient::SkinAniControl_AnimationMove()
{
	// 애니메이션에서 캐릭터를 이동시켜줬을 수도 있으니 확인하자.
	if( m_pSkinChar->IsAnimationMove() )
	{
		D3DXVECTOR3 vPos;
		D3DXMATRIX matRotate( GetTransMatrix() );
		matRotate._41 = 0.f;
		matRotate._42 = 0.f;
		matRotate._43 = 0.f;
		D3DXVec3TransformCoord( &vPos, &m_pSkinChar->GetAnimationMovePos(), &matRotate );

		vPos += GetPositionActor();
		SetPositionActor( vPos );
	}
}