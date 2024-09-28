#include "pch.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Util/GLItemLimit.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Database/DbAction/DbActionGameItem.h"
#include "../Database/DbAction/DbActionGamePet.h"
#include "./GLPetField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLPetField::GLPetField(GLGaeaServer* pServer)
    : m_pGLGaeaServer(pServer)
    , m_vDir(D3DXVECTOR3(0,0,-1))
    , m_vDirOrig(D3DXVECTOR3(0,0,-1))
    , m_vPos(D3DXVECTOR3(0,0,0))
    , m_pLandMan(NULL)
    , m_pLandNode(NULL)
    , m_vTarPos(D3DXVECTOR3(0,0,0))
    , m_pQuadNode(NULL)
    , m_pCellNode(NULL)
    , m_dwActionFlag(0)
    , m_fTIMER(0.0f)
    , m_bValid(false)
    , m_wAniSub(0)
    , m_OwnerDbNum(0)
	, m_OwnerSvrGroupNum(MAXDWORD)
{
}

GLPetField::~GLPetField(void)
{
}

HRESULT GLPetField::Create(GLLandMan* pLandMan, GLChar* pOwner, GLPET* pPetData)
{
	if (!pPetData || !pLandMan || !pOwner)
        return E_FAIL;

	ASSIGN(*pPetData);

	//m_pOwner   = pOwner;
	m_pLandMan = pLandMan;

	SetValid ();

	// 스킬 슬롯 개수
	if ( m_wMaxSkillSlot == 0 )
	{
		m_wMaxSkillSlot = 1;
	}

	// 주인 주변
	D3DXVECTOR3 vRandPos, vOwnerPos; 
	vOwnerPos = pOwner->GetPosition();
	vRandPos = GLPETDEFINE::GetRandomPosition ();
	m_vPos = vRandPos + vOwnerPos;

	// 방향
	m_vDir = m_vPos - vOwnerPos;
	D3DXVec3Normalize ( &m_vDir, &m_vDir );

	m_dwOwner = pOwner->GetGaeaID();
	m_sMapID  = pOwner->GetCurrentMap();

	m_OwnerDbNum		= pOwner->CharDbNum();
	m_OwnerSvrGroupNum	= pOwner->SvrGroupNum();

	m_actorMove.Create ( m_pLandMan->GetNavi (), m_vPos );
	m_actorMove.Stop ();

	return S_OK;
}

HRESULT GLPetField::DeleteDeviceObject ()
{
	CleanUp ();
	return S_OK;
}

void GLPetField::CleanUp ()
{
	m_vPos	   = D3DXVECTOR3(0,0,0);
	m_vDir	   = D3DXVECTOR3(0,0,-1);
	m_vTarPos  = D3DXVECTOR3(0,0,0);
	m_vDirOrig = D3DXVECTOR3(0,0,-1);

	m_pLandMan  = NULL;
	m_pLandNode = NULL;
	m_pQuadNode = NULL;
	m_pCellNode = NULL;

	m_wAniSub		= 0;
	m_fTIMER		= 0.0f;
	m_bValid		= false;
	m_dwActionFlag  = 0;
	m_OwnerDbNum = 0;
	m_OwnerSvrGroupNum = MAXDWORD;

	m_actorMove.Stop();
	m_actorMove.Release();

	RESET ();
}

HRESULT GLPetField::SetPosition(GLLandMan* pLandMan)
{
	if (!pLandMan)
        return E_FAIL;

    GLChar* pOwner = m_pGLGaeaServer->GetCharByDbNum(m_OwnerDbNum);
    if (!pOwner)
        return E_FAIL;

	m_pLandMan = pLandMan;
	SetValid();

	// 주인 주변
	D3DXVECTOR3 vRandPos, vOwnerPos; 
	vOwnerPos = pOwner->GetPosition();
	vRandPos = GLPETDEFINE::GetRandomPosition ();
	m_vPos = vRandPos + vOwnerPos;

	// 방향
	m_vDir = m_vPos - vOwnerPos;
	D3DXVec3Normalize(&m_vDir, &m_vDir);

	m_sMapID = pOwner->GetCurrentMap();

	m_actorMove.Create(m_pLandMan->GetNavi (), m_vPos );
	m_actorMove.Stop();

	return S_OK;
}

void GLPetField::ResetActorMove()
{
	m_actorMove.Release();
}

HRESULT GLPetField::FrameMove( float fTime, float fElapsedTime )
{
	HRESULT hr(S_OK);
	//static float fTimer(0.0f);

	// PETSKINPACK 적용시
	if (IsUsePetSkinPack())
	{
		CTime currentTime = CTime::GetCurrentTime();
		CTimeSpan timeSpan = currentTime - m_sPetSkinPackData.startTime;
		if (timeSpan.GetTotalSeconds() >= m_sPetSkinPackData.dwPetSkinTime)
		{
			m_sPetSkinPackData.Init();

            GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
            if (pOwner)
            {
			    GLMSG::SNETPET_SKINPACKOPEN_FB NetMsgFB(EMPET_PETSKINPACKOPEN_FB_END);
			    NetMsgFB.sPetSkinPackData.Init();
			    m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);

			    GLMSG::SNETPET_SKINPACKOPEN_BRD NetMsgBRD;
			    NetMsgBRD.dwGUID = m_dwGUID;
			    NetMsgBRD.sPetSkinPackData.Init();
			    pOwner->SendMsgViewAround(&NetMsgBRD);
            }
		}
	}

	// 움직임 상태 제어
	if ( IsSTATE ( EM_PETACT_MOVE ) )	
	{
		// ============== Update about Actor (Begin) ============== /

		hr = m_actorMove.Update ( m_pLandMan->GetvecLayerCollision(), fElapsedTime );
		if ( FAILED(hr) )	return E_FAIL;

		if ( !m_actorMove.PathIsActive() ) m_actorMove.Stop ();

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

		// ============== Update about Actor (end)   ============== /
	}

	// 클라이언트 상태 업데이트
	UpdateClientState ( fElapsedTime );

	//	스킬 상태 프로세서
	SkillProcess ( fElapsedTime ) ;

	// 스킬딜레이 업데이트
	// UpdateSkillDelay ( fElapsedTime ); 기획팀 김병욱씨 요청으로 제거 [06.10.09]

	// 스킬 업데이트
	// UpdateSkillState ( fElapsedTime ); 기획팀 김병욱씨 요청으로 제거 [06.10.09]

	// 현재 위치 업데이트
	m_vPos = m_actorMove.Position();

	UpdatePetState();

	return S_OK;
}

HRESULT GLPetField::SkillProcess( float fElapsedTime ) 
{
	//	스킬 정보 가져옴.
	for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
	{
		if ( m_sActiveSkillID[i] == NATIVEID_NULL() ) continue;

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID[i] );
		if ( !pSkill )	return E_FAIL;

		PETSKILL_MAP_ITER iter = m_ExpSkills.find ( m_sActiveSkillID[i].dwID );
		if ( iter == m_ExpSkills.end() ) return E_FAIL;

		PETSKILL sPetSkill = (*iter).second;
		SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sPetSkill.wLevel];

		// 스킬들이 케릭터 능력 보조스킬이라면...
		switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
		{
		// 큇슬롯에 포션을 먹어준다. (한프레임에 한번만 약을 먹어준다!)
		case SKILL::EMFOR_PET_AUTOMEDISON:
			{
				PROFILE_BEGIN("AutoPotionDrug()");
				GLChar* pChar = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
				if (pChar)
					pChar->AutoPotionDrug();
				//m_pOwner->AutoPotionDrug();					
				PROFILE_END("AutoPotionDrug()");
			}
			break;

			// 아이템을 줍는 스킬
		case SKILL::EMFOR_PET_GETALLITEMS:
		case SKILL::EMFOR_PET_GETRAREITEMS:
		case SKILL::EMFOR_PET_GETPOTIONS:
		case SKILL::EMFOR_PET_GETSTONE:
		case SKILL::EMFOR_PET_GETMONEY:
		// 각각의 스킬별로 주인의 상태를 갱신해준다.
		case SKILL::EMFOR_PET_HEAL:
		case SKILL::EMFOR_PET_SUPPROT:
		case SKILL::EMFOR_PET_BACKUP_ATK:
		case SKILL::EMFOR_PET_BACKUP_DEF:
		//	아이템 보호
		case SKILL::EMFOR_PET_PROTECTITEMS:
			break;

		default: break;
		};
	}

	return S_OK;
}

HRESULT GLPetField::UpdateClientState ( float fElapsedTime )
{
	m_fTIMER += fElapsedTime;
	// 팻의 포만도 갱신
	if ( m_fTIMER > 3600.0f/GLPeriod::REALTIME_TO_VBR )
	{
		// 활성화 스킬 슬롯 갯수 체크
		int nActiveCount = 0;
		for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
		{
			if ( m_sActiveSkillID[i] == NATIVEID_NULL() )
			{
				continue;
			}

			++nActiveCount;
		}

		// 갯수가 0개가 아니라면 하나씩 감소시킨다. (인덱스 때문)
		if ( 0 < nActiveCount )
		{
			nActiveCount -= 1;
		}

		if ( MAX_PETSKILLSLOT <= nActiveCount )
		{
			nActiveCount = MAX_PETSKILLSLOT-1;
		}

		m_fTIMER = 0;
        m_nFull -= GLCONST_PET::pGLPET[m_emTYPE]->nFullDecrement[nActiveCount];
		if (m_nFull < 0)
            m_nFull = 0;

		// 클라이언트 포만도 갱신
        GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
        if (pOwner)
        {
		    GLMSG::SNETPET_UPDATE_CLIENT_PETFULL NetMsg(m_nFull);
		    m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsg);

			//m_pGLGaeaServer->AddGameAdoJob(
			//	db::DbActionPtr(
			//	new db::PetFullSet( (int)m_OwnerDbNum, (int)m_DbNum, m_nFull )));	
        }
	}
	
	return S_OK;
}

void GLPetField::UpdatePetState(void)
{
	GLChar* pOwner = m_pGLGaeaServer->GetCharByDbNum(m_OwnerDbNum);
	if ( !pOwner )
		return;

	// Pet Sad 액션;
	// 주인의 죽음시 한번 콜되도록 한다;
	if ( !pOwner->IsValidBody() && !IsSTATE(EM_PETACT_SAD) )
	{
		ReSetAllSTATE();
		SetSTATE ( EM_PETACT_SAD );
		// 클라이언트에 알림;
		GLMSG::SNETPET_SAD PetNetMsg;
		pOwner->SendToClient(&PetNetMsg);

		GLMSG::SNETPET_SAD_BRD PetNetMsgBrd(pOwner->GetGaeaID());
		PetNetMsgBrd.dwGUID = m_dwGUID;
		pOwner->SendMsgViewAround(&PetNetMsgBrd);
		return;
	}

	// 주인 부활시 한번 콜되도록 한다;
	if ( pOwner->IsValidBody() && IsSTATE(EM_PETACT_SAD) )
	{
		ReSetAllSTATE();
		// 클라이언트에 알림;
		GLMSG::SNETPET_SAD PetNetMsg;
		PetNetMsg.bReset = true;
		pOwner->SendToClient(&PetNetMsg);

		GLMSG::SNETPET_SAD_BRD PetNetMsgBrd(pOwner->GetGaeaID());
		PetNetMsgBrd.dwGUID = m_dwGUID;
		PetNetMsgBrd.bReset = true;
		pOwner->SendMsgViewAround(&PetNetMsgBrd);
	}
}

void GLPetField::ReqNetMsg_Drop(GLMSG::SNETPET_DROP_PET& Msg)
{
	//GLMSG::SNETPET_DROP_PET NetMsg;
	//NetMsg = GLMSG::SNETPET_DROP_PET();

	// 클라이언트에 뿌려줘야할 팻정보 설정
	
	Msg.Data.m_emTYPE			= m_emTYPE;
	Msg.Data.m_dwGUID			= m_dwGUID;
	Msg.Data.m_sPetID			= m_sPetID;
	Msg.Data.m_dwOwner			= m_dwOwner;
	Msg.Data.m_wStyle			= m_wStyle;
	Msg.Data.m_wColor			= m_wColor;
	Msg.Data.m_nFull			= m_nFull;
	Msg.Data.m_vPos				= m_vPos;
	Msg.Data.m_vDir				= m_vDir;
	Msg.Data.m_sMapID			= m_sMapID;
	Msg.Data.m_dwCellID			= m_dwCellID;
	Msg.Data.m_fRunSpeed		= m_fRunSpeed;
	Msg.Data.m_fWalkSpeed		= m_fWalkSpeed;
	Msg.Data.m_dwActionFlag		= m_dwActionFlag;
	Msg.Data.m_vTarPos			= m_vTarPos;
	Msg.Data.m_wAniSub			= m_wAniSub;
	Msg.Data.m_petSkinPackData	= m_sPetSkinPackData;
	Msg.Data.m_dwMaxSkillSlot	= m_wMaxSkillSlot;

	for (WORD i=0; i<ACCETYPESIZE; ++i)
	{
		Msg.Data.m_PutOnItems[i] = m_PutOnItems[i];
	}

	for (WORD i = 0; i < MAX_PETSKILLSLOT; ++i)
	{
		Msg.Data.m_sActiveSkillID[i] = m_sActiveSkillID[i];
	}

	Msg.Data.SetName(m_szName);
	//return NetMsg;
}

bool GLPetField::CheckSkill ( SNATIVEID sSkillID, WORD wSlot )
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

void GLPetField::AccountSkill ( SNATIVEID sSkillID )
{
	PETSKILL_MAP_ITER iter = m_ExpSkills.find ( sSkillID.dwID );
	if ( iter == m_ExpSkills.end() ) return;
	PETSKILL sPetSkill = (*iter).second;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
	if ( !pSkill )					 return;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sPetSkill.wLevel];

	// 메시지 딜레이만큼 더해준다.
	float fServerDelay = sSKILL_DATA.fDELAYTIME + NET_MSGDELAY; 
	m_SKILLDELAY.insert ( std::make_pair(sSkillID.dwID, fServerDelay) );
}

void GLPetField::UpdateSkillDelay ( float fElapsedTime )
{
	if ( m_SKILLDELAY.size() < 1 ) return;
	/*DELAY_MAP_ITER iter_del;*/
    PETDELAY_MAP_ITER iter_del;

	PETDELAY_MAP_ITER iter = m_SKILLDELAY.begin ();
	PETDELAY_MAP_ITER iter_end = m_SKILLDELAY.end ();

	for ( ; iter!=iter_end; )
	{
		float &fDelay = (*iter).second;
		iter_del = iter++;

		fDelay -= fElapsedTime;
		if ( fDelay <= 0.0f )	m_SKILLDELAY.erase ( iter_del );
	}
}

void GLPetField::UpdateSkillState(float fElapsedTime)
{
    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return;

	for (WORD i=0; i<m_wMaxSkillSlot; ++i)
	{
		if (m_sActiveSkillID[i] == NATIVEID_NULL())
            continue;
		
		if (pOwner->m_sPETSKILLFACT[i].fAGE < 0.0f)
		{
			pOwner->m_sPETSKILLFACT[i].RESET();
			m_sActiveSkillID[i] = NATIVEID_NULL();

            GLMSG::SNETPET_REMOVE_SKILLFACT NetMsg(i);
			m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsg);
		}
	}

	// 주변에 알림
	GLMSG::NET_PET_REQ_SKILL_CHANGE_FC NetMsgBRD;
	NetMsgBRD.dwGUID = m_dwGUID;

	for (WORD i=0; i<m_wMaxSkillSlot; ++i)
	{
		NetMsgBRD.sSKILL[i].dwTarID  = pOwner->GetGaeaID();
		NetMsgBRD.sSKILL[i].sSkillID = m_sActiveSkillID[i];
	}

	pOwner->SendMsgViewAround(&NetMsgBRD);
}

bool GLPetField::IsItemProtect()
{
	for (WORD i=0; i<m_wMaxSkillSlot; ++i)
	{
		if (m_sActiveSkillID[i] == NATIVEID_NULL())
            continue;

		const GLSKILL* pSkillData = GLSkillMan::GetInstance().GetData(m_sActiveSkillID[i]);
		if (pSkillData && pSkillData->ApplyType() == SKILL::EMFOR_PET_PROTECTITEMS)
            return true;
	}
	return false;
}

void GLPetField::ActiveSkill()
{
    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
	for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
	{
		if ( m_sActiveSkillID[i] != NATIVEID_NULL() ) 
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID[i] );
			if ( !pSkill )	return;

			PETSKILL_MAP_ITER iter = m_ExpSkills.find ( m_sActiveSkillID[i].dwID );
			if ( iter == m_ExpSkills.end() ) return;

			PETSKILL sPetSkill = (*iter).second;
			SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sPetSkill.wLevel];

			// 스킬들이 케릭터 능력 보조스킬이라면...
			switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
			{
			case SKILL::EMFOR_PET_HEAL:
			case SKILL::EMFOR_PET_SUPPROT:
			case SKILL::EMFOR_PET_BACKUP_ATK:
			case SKILL::EMFOR_PET_BACKUP_DEF:
			case SKILL::EMFOR_PET_AUTOMEDISON:
			case SKILL::EMFOR_PET_PROTECTITEMS:
			case SKILL::EMFOR_PET_GETALLITEMS:
			case SKILL::EMFOR_PET_GETRAREITEMS:
			case SKILL::EMFOR_PET_GETPOTIONS:
			case SKILL::EMFOR_PET_GETMONEY:
			case SKILL::EMFOR_PET_GETSTONE:
				break;
			};

			// 보조스킬 팩터에 추가
            if (pOwner)
            {
			    pOwner->m_sPETSKILLFACT[i].emTYPE    = pSkill->m_sAPPLY.emBASIC_TYPE;
			    pOwner->m_sPETSKILLFACT[i].fMVAR     = sSKILL_DATA.fBASIC_VAR;
			    pOwner->m_sPETSKILLFACT[i].fAGE      = sSKILL_DATA.fLIFE;
			    pOwner->m_sPETSKILLFACT[i].sNATIVEID = m_sActiveSkillID[i];

			    // 클라이언트에 팩터 추가 알림
			    GLMSG::SNETPET_ADD_SKILLFACT NetMsgAddSkill;
			    NetMsgAddSkill.sPETSKILLFACT[i].sNATIVEID = m_sActiveSkillID[i];
			    NetMsgAddSkill.sPETSKILLFACT[i].emTYPE	  = pSkill->m_sAPPLY.emBASIC_TYPE;
			    NetMsgAddSkill.sPETSKILLFACT[i].fMVAR	  = sSKILL_DATA.fBASIC_VAR;
			    NetMsgAddSkill.sPETSKILLFACT[i].fAge	  = sSKILL_DATA.fLIFE;
			    m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgAddSkill);
            }
		}
	}
}

DWORD GLPetField::GetActiveSkillSlotFlag(const SNATIVEID& sSkillID)
{
	DWORD dwFlag = 0;
	for (WORD i=0; i<m_wMaxSkillSlot; ++i)
	{
		if (m_sActiveSkillID[i] == sSkillID) 
		{
			dwFlag |= (1<<i);
		}
	}
	return dwFlag;
}

void GLPetField::UpdateTimeLmtItem( GLChar* pChar )
{
	if ( !pChar )
	{
		// 통계 및 로그 수정
		// 캐릭터가 존재하지 않음. 원래는 return 해야하지만 로그를 위해서 추가한 것이므로 일단 로그만 남기고 차후 발견시 처리
		sc::writeLogError( std::string( "Can not find GetCharByDbNum on UpdateTimeLmtItem" ) );
	}

    const CTime cTIME_CUR = CTime::GetCurrentTime();

    GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
    for ( WORD i = 0; i < ACCETYPESIZE; ++i )
    {
		SNATIVEID sPetPutonItemID = m_PutOnItems[i].GetNativeID();
        if ( sPetPutonItemID == NATIVEID_NULL () )
			continue;

        const SITEM* pITEM = GLogicData::GetInstance().GetItem( sPetPutonItemID );
        if ( !pITEM )
			continue;

        // 시한부 아이템
		if ( m_PutOnItems[i].IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
        {
            CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
            CTime cTIME_LMT( m_PutOnItems[i].tBORNTIME );
            cTIME_LMT += cSPAN + m_PutOnItems[i].GetPeriodExBasic();

            if ( cTIME_CUR > cTIME_LMT )
            {
                // 시간 제한으로 아이템 삭제 로그 남김.
				// 통계 및 로그 수정
				pItemLimit->LogItemExchange ( m_PutOnItems[i], gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, ITEM_ROUTE_DELETE, 0, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT, false );

                //	아이템 삭제.
				{
					m_PutOnItems[i].SetDbState( db::DB_DELETE );

					switch ( m_PutOnItems[i].GetDbState() )
					{
					case db::DB_DELETE:
						{
							m_pGLGaeaServer->AddGameAdoJob(
								db::DbActionPtr( 
								new db::ItemDelete( m_PutOnItems[i].GetGuid(), m_PutOnItems[i].GetDbState(), DELLTE_PET_INVEN_TIMELIMIT_ITEM ) ) );
						}
						break;
					default:
						{
							sc::writeLogError( std::string( "UpdateTimeLmtItem ItemDelete db state error" ) );
						}
						break;
					}

					m_PutOnItems[i] = SITEMCUSTOM();
				}

				ACCESSORYTYPE AccessoryType = static_cast< ACCESSORYTYPE >( i );

                //	[자신에게] 해당 아이탬을 인밴에서 삭제.
                GLMSG::SNETPET_ACCESSORY_DELETE NetMsg( AccessoryType );
                m_pGLGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &NetMsg );

                // 주변에 알림.
                GLMSG::SNETPET_ACCESSORY_DELETE_BRD NetMsgBrd( AccessoryType );
                NetMsgBrd.dwGUID = m_dwGUID;
                pChar->SendMsgViewAround( &NetMsgBrd );

                //	시간 제한으로 아이템 삭제 알림.
                GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
                NetMsgInvenDelTimeLmt.nidITEM = sPetPutonItemID;
                m_pGLGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &NetMsgInvenDelTimeLmt );
            }
        }
    }
}

void GLPetField::PetPutOnItemReset( ACCESSORYTYPE Type, bool DbUpdate /*= false*/ )
{
	if ( Type < ACCETYPEA || Type >= ACCETYPESIZE )
	{
		return;
	}

	SITEMCUSTOM& sItemCustom = m_PutOnItems[Type];
	if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
	{
		return;
	}

	sc::MGUID guid = sItemCustom.GetGuid();
	int DbState = sItemCustom.GetDbState();

	m_PutOnItems[Type] = SITEMCUSTOM( NATIVEID_NULL() );

	if ( DbUpdate )
	{
		AddGameDbAdoJob(
			db::DbActionPtr( 
			new db::ItemDelete( guid, DbState, PET_PUTON_ITEM_RESET ) ) );
	}
}

void GLPetField::PetPutOnItemReset( EMSUIT emSuit, bool DbUpdate /*= false*/ )
{
	ACCESSORYTYPE Type = ACCETYPEA;
	if ( GetAccessoryType( emSuit, Type ) )
	{
		PetPutOnItemReset( Type, DbUpdate );
	}
}

void GLPetField::PetPutOnItemUpdate( ACCESSORYTYPE Type, bool JustInvenTypeUpdate /*= false*/ )
{
	if ( Type < ACCETYPEA || Type >= ACCETYPESIZE )
	{
		return;
	}

	SITEMCUSTOM& sItemCustom = m_PutOnItems[Type];
	if ( sItemCustom.GetNativeID() == NATIVEID_NULL() )
	{
		return;
	}

	SINVENITEM_SAVE sItem( sItemCustom, static_cast< WORD >( Type ), 0 );

	int DbState = sItemCustom.GetDbState();
	if ( db::DB_INSERT == DbState || db::DB_NONE == DbState )
	{
		AddGameDbAdoJob(
			db::DbActionPtr( 
			new db::ItemInsert( m_DbNum, INVEN_PET, sItem, 94598 ) ) );

		sItemCustom.SetDbState( db::DB_UPDATE, true );
	}
	else
	{
		if ( JustInvenTypeUpdate )
		{
			// 저장 위치 및 좌표만 변경된다.
			m_pGLGaeaServer->UpdateItemInvenType( m_DbNum, sItemCustom, INVEN_PET, sItem.wPosX, sItem.wPosY );
		}
		else
		{
			AddGameDbAdoJob(
				db::DbActionPtr( 
				new db::ItemUpdate( m_DbNum, INVEN_PET, sItem, 94598 ) ) );
		}
	}
}

void GLPetField::PetPutOnItemUpdate( EMSUIT emSuit, bool JustInvenTypeUpdate /*= false*/ )
{
	ACCESSORYTYPE Type = ACCETYPEA;
	if ( GetAccessoryType( emSuit, Type ) )
	{
		PetPutOnItemUpdate( Type, JustInvenTypeUpdate );
	}
}

bool GLPetField::GetAccessoryType( EMSUIT emSuit, ACCESSORYTYPE& Type )
{
	WORD i = ( WORD ) emSuit - ( WORD ) SUIT_PET_A;
	if ( i >= ACCETYPESIZE )
	{
		return false;
	}
	else
	{
		Type = static_cast< ACCESSORYTYPE >( i );
		return true;
	}
}

bool GLPetField::IsValidSlot( EMSUIT emSuit )
{
	WORD i = ( WORD ) emSuit - ( WORD ) SUIT_PET_A;
	if ( i >= ACCETYPESIZE )
	{
		return false;
	}
	else
	{
		return true;
	}
}

HRESULT GLPetField::SetJumpPos( const D3DXVECTOR3& vPos )
{
	if (!m_pLandMan)
		return E_FAIL;

	GLChar* pOwner = m_pGLGaeaServer->GetCharByDbNum(m_OwnerDbNum);
	if (!pOwner)
		return E_FAIL;

	SetValid();

	// 주인 주변
	D3DXVECTOR3 vRandPos, vOwnerPos; 
	vOwnerPos = vPos;
	vRandPos = GLPETDEFINE::GetRandomPosition ();
	m_vPos = vRandPos + vOwnerPos;

	m_vTarPos = m_vPos;

	// 방향
	m_vDir = m_vPos - vOwnerPos;
	D3DXVec3Normalize(&m_vDir, &m_vDir);

	m_sMapID = pOwner->GetCurrentMap();

	m_actorMove.Create(m_pLandMan->GetNavi (), m_vPos );
	m_actorMove.Stop();

	ReSetSTATE ( EM_PETACT_MOVE );
	SetSTATE ( EM_PETACT_STOP );

	GLMSG::SNETPET_REQ_JUMP	NetMsg( m_dwGUID, m_vPos );

	m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsg );

	GLMSG::SNETPET_REQ_JUMP_FB	NetMsgFb( m_vPos );

	NetMsgFb.dwGUID = m_dwGUID;

	pOwner->SendMsgViewAround( &NetMsgFb );

	return S_OK;
}