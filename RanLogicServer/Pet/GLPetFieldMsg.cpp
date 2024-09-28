#include "pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../MfcExLib/RanFilter.h"

#include "../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"

#include "../Database/DbManager.h"
#include "../Database/DBAction/DbActionGamePet.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Util/GLItemLimit.h"
#include "./GLPetField.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLPetField::AddGameDbJob(db::DbActionPtr spJob)
{
    m_pGLGaeaServer->AddGameDbJob(spJob);
}

void GLPetField::AddGameDbAdoJob(db::DbActionPtr spJob)
{
    m_pGLGaeaServer->AddGameAdoJob(spJob);
}

HRESULT	GLPetField::MsgPetGoto(NET_MSG_GENERIC* nmg)
{
	// 지형이 사라진 상태이며, 아직 소환이 안된 상태일 수 있다. ( 인던 진입 해서 소환 후 소환 해제 및 인던 해제 상태. 주인은 다른 맵에 존재 )
	if ( !m_pLandMan )
		return E_FAIL;

	GLMSG::SNETPET_REQ_GOTO* pPacket = ( GLMSG::SNETPET_REQ_GOTO* ) nmg;

	// 서버와 클라이언트의 팻위치가 서로 많이 어긋나면 위치를 강제한다
	D3DXVECTOR3 vDist = m_vPos - pPacket->vCurPos;
	float fDist = D3DXVec3Length(&vDist);
	if ( fDist > 60.0f )
	{
		// 자신에게 위치를 강제함

		// 주변에 알림
	}

	m_vTarPos = pPacket->vTarPos;

	// 이동처리
	BOOL bGoto = m_actorMove.GotoLocation ( m_vTarPos+D3DXVECTOR3(0,+10,0),
							                m_vTarPos+D3DXVECTOR3(0,-10,0) );

	if ( !bGoto )
        return E_FAIL;

	m_dwActionFlag = pPacket->dwFlag;

	if ( IsSTATE ( EM_PETACT_RUN ) )
        m_actorMove.SetMaxSpeed ( m_fRunSpeed );
	else
        m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
	
	// 주변에 알림
    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (pOwner)
    {
	    GLMSG::SNETPET_GOTO_BRD NetMsgBrd(pPacket->vCurPos,m_vTarPos,m_dwActionFlag);
	    NetMsgBrd.dwGUID = m_dwGUID;
	    pOwner->SendMsgViewAround(&NetMsgBrd);
    }

	return S_OK;
}

HRESULT	GLPetField::MsgPetStop(NET_MSG_GENERIC* nmg)
{
	// 지형이 사라진 상태이며, 아직 소환이 안된 상태일 수 있다. ( 인던 진입 해서 소환 후 소환 해제 및 인던 해제 상태. 주인은 다른 맵에 존재 )
	if ( !m_pLandMan )
		return E_FAIL;

	GLMSG::SNETPET_REQ_STOP* pPacket = ( GLMSG::SNETPET_REQ_STOP* ) nmg;

	// 클라이언트의 멈춤 메시지에 따라 서버의 위치를 항상 클라이언트와 똑같이 강제한다.
	m_dwActionFlag = pPacket->dwFlag;
	m_vPos		   = pPacket->vPos;
	m_actorMove.Stop ();
	m_actorMove.SetPosition ( m_vPos, -1 );

	// 주변에 알림
    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (pOwner)
    {
	    GLMSG::SNETPET_STOP_BRD NetMsgBrd(pPacket->vPos,pPacket->dwFlag,pPacket->bStopAttack);
	    NetMsgBrd.dwGUID = m_dwGUID;
	    pOwner->SendMsgViewAround(&NetMsgBrd);
    }

	return S_OK;
}

HRESULT	GLPetField::MsgPetUpdateMoveState(NET_MSG_GENERIC* nmg)
{
	// 지형이 사라진 상태이며, 아직 소환이 안된 상태일 수 있다. ( 인던 진입 해서 소환 후 소환 해제 및 인던 해제 상태. 주인은 다른 맵에 존재 )
	if ( !m_pLandMan )
		return E_FAIL;

	GLMSG::SNETPET_REQ_UPDATE_MOVESTATE* pPacket = (GLMSG::SNETPET_REQ_UPDATE_MOVESTATE*) nmg;

	m_dwActionFlag = pPacket->dwFlag;

	if (IsSTATE(EM_PETACT_RUN))
        m_actorMove.SetMaxSpeed(m_fRunSpeed);
	else
        m_actorMove.SetMaxSpeed(m_fWalkSpeed);
	
	// 주변에 알림
    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (pOwner)
    {
	    GLMSG::SNETPET_REQ_UPDATE_MOVESTATE_BRD NetMsgBrd(pPacket->dwFlag);
	    NetMsgBrd.dwGUID = m_dwGUID;
	    pOwner->SendMsgViewAround(&NetMsgBrd);
    }

	return S_OK;
}

HRESULT GLPetField::MsgPetRename(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_RENAME *pPacket = (GLMSG::SNETPET_REQ_RENAME*) nmg;

	GLMSG::SNETPET_REQ_RENAME_FB NetMsgFB(EMPET_REQ_RENAME_FB_FB_FAIL);

    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return E_FAIL;

	SINVENITEM* pINVENITEM = pOwner->InvenGetItem( pPacket->wPosX, pPacket->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_NOITEM;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	if ( pOwner->CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
        return S_FALSE;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_PET_RENAME )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_BADITEM;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}
	
	// Note : 레벨 조건 확인
	//
	if ( !pOwner->SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	CString strChaName(pPacket->szName);
	strChaName.Trim(_T(" ")); // 앞뒤 공백제거

	// 스페이스 찾기, 캐릭터이름 4글자 이하 에러, 캐릭터 만들 수 없음.
	if ((strChaName.FindOneOf(" ") != -1) || (strChaName.GetLength() < 4)) 
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_LENGTH;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	BOOL bFILTER0 = STRUTIL::CheckString( strChaName );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strChaName );
	if ( bFILTER0 || bFILTER1 )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_BAD_NAME;
		m_pGLGaeaServer->SENDTOCLIENT ( pOwner->ClientSlot(), &NetMsgFB );
		return S_FALSE;			
	}

	if ( m_pGLGaeaServer->GetServiceProvider() == SP_GS )//
		strChaName = sc::string::utf8_to_ansi( std::string(strChaName.GetString()) ).c_str();

	// 태국어 문자 조합 체크
    if ( RENDERPARAM::emLangSet == language::THAILAND ) 
    {
	    if ( !STRUTIL::CheckThaiString(strChaName) )
	    {
		    NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_THAICHAR_ERROR;
		    m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		    return E_FAIL;
	    }
    }

	// 베트남 문자 조합 체크 
    if ( RENDERPARAM::emLangSet == language::VIETNAM ) 
    {
	    if( STRUTIL::CheckVietnamString( strChaName ) )
	    {
		    NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_VNCHAR_ERROR;
		    m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		    return E_FAIL;
	    }
    }


	if ( m_pGLGaeaServer->IsEmulatorMode() )
	{
		// 클라이언트에 처리 결과 전송.
		StringCchCopy ( NetMsgFB.szName, PETNAMESIZE+1, pPacket->szName );
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_OK;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);

		//	[자신에게] 인밴에 아이탬 제거.
		pOwner->ConsumeInvenItem( pPacket->wPosX, pPacket->wPosY );
	}

	// 이름변경 적용
	//SetName(strChaName.GetString());

	if ( GLHIDESET::CheckHideSet(EMHIDE_USE_PET_CHANGE) )
	{
		pOwner->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		pOwner->RestoreActStateByInvisible();
	}

	// 펫 액션 로그
    GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
	pItemLimit->LogPetAction(m_DbNum, pINVENITEM->sItemCustom.GetNativeID(), pet::EM_ACTION_RENAME, m_nFull);

	// DB에 저장요청	
	AddGameDbAdoJob(
        db::DbActionPtr(
            new db::PetRename(
                pOwner->ClientSlot(),
                pOwner->m_CharDbNum,
                m_DbNum, strChaName.GetString(),
                pPacket->wPosX,
                pPacket->wPosY)));
	return S_OK;
}

HRESULT GLPetField::PetRenameFeedBack(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_RENAME_FROMDB_FB* pPacket = (GLMSG::SNETPET_REQ_RENAME_FROMDB_FB*) nmg;
	
    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return E_FAIL;

	// 클라이언트에 처리 결과 전송.
	GLMSG::SNETPET_REQ_RENAME_FB NetMsgFB( EMPET_REQ_RENAME_FB_FB_FAIL );

	if ( pPacket->emFB == EMPET_REQ_RENAME_FB_FROM_DB_FAIL )
	{
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FROM_DB_FAIL;
		StringCchCopy ( NetMsgFB.szName, PETNAMESIZE+1, pPacket->szName );
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}
	else if ( pPacket->emFB == EMPET_REQ_RENAME_FB_FROM_DB_OK )
	{
		SetName(pPacket->szName);
		StringCchCopy ( NetMsgFB.szName, PETNAMESIZE+1, pPacket->szName );
		NetMsgFB.emFB = EMPET_REQ_RENAME_FB_FB_OK;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);

		// 주변에 이름변경을 알림.
		GLMSG::SNETPET_REQ_RENAME_BRD NetMsgBrd(pPacket->szName);
		NetMsgBrd.dwGUID = m_dwGUID;		
		pOwner->SendMsgViewAround(&NetMsgBrd);

		//	[자신에게] 인밴에 아이탬 제거.
		pOwner->ConsumeInvenItem( pPacket->wPosX, pPacket->wPosY );		
	}	

	return S_OK;
}

HRESULT	GLPetField::MsgPetChangeColor(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_CHANGE_COLOR *pPacket = (GLMSG::SNETPET_REQ_CHANGE_COLOR*) nmg;

    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return E_FAIL;

	GLMSG::SNETPET_REQ_CHANGE_COLOR_FB NetMsgFB(EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL,-1);

	if ( !GLCONST_PET::IsChangeablePetColor ( m_emTYPE ) )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_UNCHANGEABLE;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	SINVENITEM* pINVENITEM = pOwner->InvenGetItem( pPacket->wPosX, pPacket->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	if ( pOwner->CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_COOLTIME;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_PET_COLOR )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	if ( !pOwner->SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	if ( IsUsePetSkinPack() )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	m_wColor = pPacket->wColor;

	AddGameDbAdoJob(
        db::DbActionPtr(
            new db::PetColorSet(pOwner->m_CharDbNum, m_DbNum, m_wColor)));	

	pOwner->ConsumeInvenItem( pPacket->wPosX, pPacket->wPosY );

	// 펫 액션 로그
    GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
	pItemLimit->LogPetAction(m_DbNum, pINVENITEM->sItemCustom.GetNativeID(), pet::EM_ACTION_COLOR, m_nFull);

	NetMsgFB.emFB   = EMPET_REQ_COLOR_CHANGE_FB_OK;
	NetMsgFB.wColor = pPacket->wColor;
	m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);

	GLMSG::SNETPET_REQ_CHANGE_COLOR_BRD NetMsgBRD(pPacket->wColor);
	NetMsgBRD.dwGUID = m_dwGUID;
	pOwner->SendMsgViewAround(&NetMsgBRD);

	if ( GLHIDESET::CheckHideSet(EMHIDE_USE_PET_CHANGE) )
	{
		pOwner->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		pOwner->RestoreActStateByInvisible();
	}

	return S_OK;
}
HRESULT	GLPetField::MsgPetChangeStyle(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_CHANGE_STYLE *pPacket = (GLMSG::SNETPET_REQ_CHANGE_STYLE*) nmg;

    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return E_FAIL;

	GLMSG::SNETPET_REQ_CHANGE_STYLE_FB NetMsgFB(EMPET_REQ_STYLECOLOR_CHANGE_FB_FAIL,-1,0);	

	SINVENITEM* pINVENITEM = pOwner->InvenGetItem( pPacket->wPosX, pPacket->wPosY );
	if ( !pINVENITEM )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_NOITEM;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	if ( pOwner->CheckCoolTime( pINVENITEM->sItemCustom.GetNativeID() ) )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_COOLTIME;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
	if ( !pITEM || pITEM->sBasicOp.emItemType != ITEM_PET_STYLE )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_BADITEM;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	if ( !pOwner->SIMPLE_CHECK_ITEM( pINVENITEM->sItemCustom ) )
	{
		return E_FAIL;
	}

	if ( IsUsePetSkinPack() )
	{
		NetMsgFB.emFB = EMPET_REQ_STYLECOLOR_CHANGE_FB_SKINPACK;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	if ( m_wColor == GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[m_wStyle] )
	{
		m_wColor = GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[pPacket->wStyle];

		AddGameDbAdoJob(
            db::DbActionPtr(
                new db::PetColorSet(pOwner->m_CharDbNum, m_DbNum, m_wColor)));        
	}

	m_wStyle = pPacket->wStyle;

    AddGameDbAdoJob(
        db::DbActionPtr(
            new db::PetStyleSet(pOwner->m_CharDbNum, m_DbNum, m_wStyle)));

	pOwner->ConsumeInvenItem( pPacket->wPosX, pPacket->wPosY );

	// 펫 액션 로그
    GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
	pItemLimit->LogPetAction(m_DbNum, pINVENITEM->sItemCustom.GetNativeID(), pet::EM_ACTION_STYLE, m_nFull);
	
	NetMsgFB.emFB   = EMPET_REQ_STYLE_CHANGE_FB_OK;
	NetMsgFB.wStyle = pPacket->wStyle;
	NetMsgFB.wColor = m_wColor;
	m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);

	GLMSG::SNETPET_REQ_CHANGE_STYLE_BRD NetMsgBRD(pPacket->wStyle,m_wColor);
	NetMsgBRD.dwGUID = m_dwGUID;
	pOwner->SendMsgViewAround(&NetMsgBRD);

	if ( GLHIDESET::CheckHideSet(EMHIDE_USE_PET_CHANGE) )
	{
		pOwner->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		pOwner->RestoreActStateByInvisible();
	}
	
	return S_OK;
}

HRESULT GLPetField::MsgPetChangeActiveSkill(NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_PET_REQ_SKILL_CHANGE_CF* pPacket = (GLMSG::NET_PET_REQ_SKILL_CHANGE_CF*) nmg;

    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return E_FAIL;

    if ( !IsValid() )
        return E_FAIL;

	WORD wSLOT = pPacket->wSlot;

	// 잘못된 스킬, 스킬딜레이, 슬롯문제가 있다면 취소
	if ( !CheckSkill( pPacket->sSkillID, wSLOT ) && pPacket->sSkillID != NATIVEID_NULL() )
        return E_FAIL;

	if ( m_sActiveSkillID[wSLOT] == pPacket->sSkillID )
        return E_FAIL;

	// 스킬을 중복으로 사용하려 했을 경우
	if ( IsSkillDuplicated( pPacket->sSkillID, wSLOT ) )
        return E_FAIL;

	//	현재 스킬이 구동중이고 다른 스킬을 사용할려고 한다면
	if (m_sActiveSkillID[wSLOT] != NATIVEID_NULL())
	{
		if (m_sActiveSkillID[wSLOT].Mid() != PET_SKILL_MAIN)
		{
            sc::writeLogError(
                sc::string::format(
                    "GLPetField::MsgChangeActiveSkill_03, MID %1% SID %2%",
                    m_sActiveSkillID[wSLOT].Mid(),
                    m_sActiveSkillID[wSLOT].Sid()));
		}

		// DB에 스킬 취소 저장
		DWORD dwActiveSkillSlotFlag = GetActiveSkillSlotFlag(m_sActiveSkillID[wSLOT]) & (~(1<<wSLOT));

		PETSKILL sPetSkill( m_sActiveSkillID[wSLOT], 0 );
		AddGameDbAdoJob(
            db::DbActionPtr(
                new db::PetSkillSet(pOwner->m_CharDbNum, m_DbNum, sPetSkill, dwActiveSkillSlotFlag)));		
	}   

	if (pPacket->sSkillID == NATIVEID_NULL())
	{
		if (m_sActiveSkillID[wSLOT] == NATIVEID_NULL())
            return S_OK;
			
		m_sActiveSkillID[wSLOT] = pPacket->sSkillID;

		// 보조스킬 팩터에 추가
		pOwner->m_sPETSKILLFACT[wSLOT].RESET();

		// 스킬제거
		GLMSG::SNETPET_REMOVE_SKILLFACT NetMsgRemoveSkill(wSLOT);
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgRemoveSkill);

		// 주변에 알림
		GLMSG::NET_PET_REQ_SKILL_CHANGE_FC NetMsgBRD;
		NetMsgBRD.dwGUID  = m_dwGUID;
		NetMsgBRD.sSKILL[wSLOT].dwTarID  = pOwner->GetGaeaID();
		NetMsgBRD.sSKILL[wSLOT].sSkillID = pPacket->sSkillID;
		pOwner->SendMsgViewAround(&NetMsgBRD);
	}
	else
	{
		GLSKILL* pSkill = GLSkillMan::GetInstance().GetData(pPacket->sSkillID);
		if (!pSkill)
            return E_FAIL;

		PETSKILL_MAP_ITER iter = m_ExpSkills.find(pPacket->sSkillID.dwID);
		if (iter == m_ExpSkills.end())
            return E_FAIL;

		m_sActiveSkillID[wSLOT] = pPacket->sSkillID;
		
		const PETSKILL& sPetSkill = iter->second;
		const SKILL::CDATA_LVL& sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sPetSkill.wLevel];

		//AccountSkill ( m_sActiveSkillID );

		// 스킬들이 케릭터 능력 보조스킬이라면...
		switch (pSkill->ApplyType())
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

		// 보조 스킬 펙터에 추가
		pOwner->m_sPETSKILLFACT[wSLOT].emTYPE    = pSkill->m_sAPPLY.emBASIC_TYPE;
		pOwner->m_sPETSKILLFACT[wSLOT].fMVAR     = sSKILL_DATA.fBASIC_VAR;
		pOwner->m_sPETSKILLFACT[wSLOT].fAGE      = sSKILL_DATA.fLIFE;
		pOwner->m_sPETSKILLFACT[wSLOT].sNATIVEID = m_sActiveSkillID[wSLOT];

		// 클라이언트에 펙터 추가 알림
		GLMSG::SNETPET_ADD_SKILLFACT NetMsgAddSkill;
		NetMsgAddSkill.sPETSKILLFACT[wSLOT].sNATIVEID = m_sActiveSkillID[wSLOT];
		NetMsgAddSkill.sPETSKILLFACT[wSLOT].emTYPE	  = pSkill->m_sAPPLY.emBASIC_TYPE;
		NetMsgAddSkill.sPETSKILLFACT[wSLOT].fMVAR	  = sSKILL_DATA.fBASIC_VAR;
		NetMsgAddSkill.sPETSKILLFACT[wSLOT].fAge	  = sSKILL_DATA.fLIFE;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgAddSkill);

		// 주변에 알림
		GLMSG::NET_PET_REQ_SKILL_CHANGE_FC NetMsgBRD;
		NetMsgBRD.dwGUID   = m_dwGUID;
		NetMsgBRD.sSKILL[wSLOT].dwTarID  = pOwner->GetGaeaID();
		NetMsgBRD.sSKILL[wSLOT].sSkillID = pPacket->sSkillID;
		pOwner->SendMsgViewAround(&NetMsgBRD);

		if (m_sActiveSkillID[wSLOT].Mid() != PET_SKILL_MAIN)
		{
            sc::writeLogError(
                sc::string::format(
                    "GLPetField::MsgChangeActiveSkill_03, MID %1% SID %2%",  
                    m_sActiveSkillID[wSLOT].wMainID,
                    m_sActiveSkillID[wSLOT].wSubID));
		}

		// DB에 스킬 변경 저장
		AddGameDbAdoJob(
            db::DbActionPtr(
                new db::PetSkillSet(pOwner->m_CharDbNum, m_DbNum, sPetSkill, GetActiveSkillSlotFlag(m_sActiveSkillID[wSLOT]))));
	}

    LogPetSkillChange();
    
	return S_OK;
}

void GLPetField::LogPetSkillChange()
{
    if (MAX_PETSKILLSLOT < 2)
        return;

    GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();

    // 대만 요청, 펫 스킬이 변경되면 모든 슬롯의 스킬정보를 Log 에 남겨달라!
    // 작업항목 : 2162
    // Wiki : http://goo.gl/Vo8b7    

    // A Slot
    if (m_sActiveSkillID[0] != NATIVEID_NULL())
        pItemLimit->LogPetAction(m_DbNum, m_sActiveSkillID[0], pet::EM_ACTION_SKILL_CHANGE_A, m_nFull);
    else
        pItemLimit->LogPetAction(m_DbNum, SNATIVEID(0,0), pet::EM_ACTION_SKILL_CHANGE_A, m_nFull);

    // B Slot
    if (m_sActiveSkillID[1] != NATIVEID_NULL())
        pItemLimit->LogPetAction(m_DbNum, m_sActiveSkillID[1], pet::EM_ACTION_SKILL_CHANGE_B, m_nFull);
    else
        pItemLimit->LogPetAction(m_DbNum, SNATIVEID(0,0), pet::EM_ACTION_SKILL_CHANGE_B, m_nFull);
}

// #item
// Hold <-> Slot
HRESULT	GLPetField::MsgPetAccInvenExSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_INVEN_EX_SLOT* pPacket = ( GLMSG::SNETPET_REQ_INVEN_EX_SLOT* ) nmg;

    GLChar* pOwner = m_pGLGaeaServer->GetChar64( m_OwnerDbNum, m_OwnerSvrGroupNum );
    if ( !pOwner )
        return E_FAIL;

	GLLandMan* const pLand( pOwner->GetLandMan() );
	if ( !pLand )
		return E_FAIL;
	
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_FB NetMsgFB( EMPET_REQ_SLOT_EX_HOLD_FB_FAIL, SUIT_NSIZE );

	// 공격중이나 스킬 시전 중에 슬롯 변경을 수행 할 수 없다고 봄.
	if ( pOwner->isAction( GLAT_ATTACK ) || pOwner->isAction( GLAT_SKILL ) )
        return E_FAIL;

	if ( !IsValidSlot( pPacket->emSuit ) )
	{
		return E_FAIL;
	}

    SINVENITEM* pInvenItem = pOwner->InvenGetItem( pPacket->wHoldPosX, pPacket->wHoldPosY );
    if ( !pInvenItem )
        return E_FAIL;

	SITEMCUSTOM sHoldItemCustom = pInvenItem->CustomItem();
	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype( pPacket->emSuit );

	const SITEM* pHoldItem = GLogicData::GetInstance().GetItem( sHoldItemCustom.GetNativeID() );
	const SITEM* pSlotItem = GLogicData::GetInstance().GetItem( sSlotItemCustom.GetNativeID() );

	if ( !pHoldItem || !pSlotItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_FAIL;
		m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )
		return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit != SUIT_PET_A && pHoldItem->sSuitOp.emSuit != SUIT_PET_B )
	{
		// 팻용 아이템이 아닐때
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );
		return E_FAIL;
	}

	// 서로 다른 타입일때
	if ( pHoldItem->sSuitOp.emSuit != pSlotItem->sSuitOp.emSuit )
	{
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH;
		m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );
		return E_FAIL; 
	}

	if ( !CheckSlotItem( pHoldItem->sBasicOp.sNativeID, pPacket->emSuit ) )
        return E_FAIL;

    SINVENITEM sHold_BackUp = *pInvenItem;		//	Inventory에 있던 아이템 백업.

    WORD wHoldPosX = sHold_BackUp.wPosX;		//	아이템 위치.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	SINVENITEM* pInsertedInvenItem = NULL;

	// #item
	// pet puton(Inven) <-> inven(Hold)
	// pet puton 아이템(Inven) pPacket->emSuit, sSlotItemCustom
	// inven 아이템(Hold) wHoldPosX, wHoldPosY, sHold_BackUp

    {
		if ( !ReSetSlotItem( pPacket->emSuit ) )
		{
			return S_FALSE;
		}

		if ( !pOwner->InvenDeleteItem( wHoldPosX, wHoldPosY, false, FALSE, 1, false ) )
		{
			if ( !SetSlotItem( pPacket->emSuit, sSlotItemCustom ) )
			{
				GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB NetRemoveMsg( pPacket->emSuit );
				m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetRemoveMsg );

				GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetRemoveBRD( SITEMCUSTOM( NATIVEID_NULL() ), pPacket->emSuit );
				NetRemoveBRD.dwGUID = m_dwGUID;
				pOwner->SendMsgViewAround( &NetRemoveBRD );

				pOwner->DropItem( sSlotItemCustom, pOwner->GetPosition() );
			}

			return S_FALSE;
		}
    }

	{
		if ( !SetSlotItem( pPacket->emSuit, sHoldItemCustom ) )
		{
			GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB NetRemoveMsg( pPacket->emSuit );
			m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetRemoveMsg );

			GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetRemoveBRD( SITEMCUSTOM( NATIVEID_NULL() ), pPacket->emSuit );
			NetRemoveBRD.dwGUID = m_dwGUID;
			pOwner->SendMsgViewAround( &NetRemoveBRD );

			pOwner->InvenDeleteSend( wHoldPosX, wHoldPosY, FALSE );

			pOwner->DropItem( sSlotItemCustom, pOwner->GetPosition() );

			pOwner->DropItem( sHoldItemCustom, pOwner->GetPosition() );

			return S_FALSE;
		}

		if ( !pOwner->InvenInsertItem( sSlotItemCustom, wHoldPosX, wHoldPosY, false, 1, false ) )
		{
			GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB NetRemoveMsg( pPacket->emSuit );
			m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetRemoveMsg );

			GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetRemoveBRD( SITEMCUSTOM( NATIVEID_NULL() ), pPacket->emSuit );
			NetRemoveBRD.dwGUID = m_dwGUID;
			pOwner->SendMsgViewAround( &NetRemoveBRD );

			pOwner->InvenDeleteSend( wHoldPosX, wHoldPosY, FALSE );

			SITEMCUSTOM	sTempSlotItemCustom = GetSlotitembySuittype( pPacket->emSuit );
			if ( NATIVEID_NULL() == sTempSlotItemCustom.GetNativeID() )
			{
				NET_DISCONNECT_CHARACTER NetMsg;
				NetMsg.ChaDbNum = pOwner->CharDbNum();
				m_pGLGaeaServer->SENDTOAGENT( &NetMsg );
			}
			else
			{
				NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_OK;
				NetMsgFB.sHoldtoSlotItemCustom = sTempSlotItemCustom;
				NetMsgFB.emSuit = pPacket->emSuit;
				m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );

				GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetMsgBRD( sTempSlotItemCustom, pPacket->emSuit );
				NetMsgBRD.dwGUID = m_dwGUID;
				pOwner->SendMsgViewAround( &NetMsgBRD );
			}

			pOwner->DropItem( sSlotItemCustom, pOwner->GetPosition(), NATIVEID_NULL() == sTempSlotItemCustom.GetNativeID() ? false : true );

			return S_FALSE;
		}

		pInsertedInvenItem = pOwner->InvenGetItem( wHoldPosX, wHoldPosY );
		if ( !pInsertedInvenItem )
		{
			NET_DISCONNECT_CHARACTER NetMsg;
			NetMsg.ChaDbNum = pOwner->CharDbNum();
			m_pGLGaeaServer->SENDTOAGENT( &NetMsg );

			return S_FALSE;
		}
	}

	// 펫 장착창 디비 업데이트
	PetPutOnItemUpdate( pPacket->emSuit, true );

	// 인벤토리 아이템 디비 업데이트
	pOwner->InvenUpdateItem( pInsertedInvenItem, true );

	GLMSG::SNETPC_INVEN_DEL_AND_INSERT NetMsg_Del_Insert;
	NetMsg_Del_Insert.wDelX = sHold_BackUp.wPosX;
	NetMsg_Del_Insert.wDelY = sHold_BackUp.wPosY;
	NetMsg_Del_Insert.sInsert = *pInsertedInvenItem;

	msgpack::sbuffer SendBuffer;
	msgpack::pack( SendBuffer, NetMsg_Del_Insert );
	pOwner->SendToClient( NET_MSG_GCTRL_INVEN_DEL_INSERT, SendBuffer );

	// 자신에게 알림
	NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.sHoldtoSlotItemCustom = sHoldItemCustom;
	NetMsgFB.emSuit = pPacket->emSuit;
	m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetMsgBRD( sHoldItemCustom, pPacket->emSuit );
	NetMsgBRD.dwGUID = m_dwGUID;
	pOwner->SendMsgViewAround( &NetMsgBRD );
	
	return S_OK;
}

// #item
// Hold -> Slot
HRESULT	GLPetField::MsgPetAccInvenToSlot( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_INVEN_TO_SLOT* pPacket = ( GLMSG::SNETPET_REQ_INVEN_TO_SLOT* ) nmg;
    GLChar* pOwner = m_pGLGaeaServer->GetChar64( m_OwnerDbNum, m_OwnerSvrGroupNum );
    if ( !pOwner )
        return E_FAIL;

	GLMSG::SNETPET_REQ_HOLD_TO_SLOT_FB NetMsgFB( EMPET_REQ_SLOT_EX_HOLD_FB_FAIL,SUIT_NSIZE );

	//	Note : 공격중이나 스킬 시전 중에 슬롯 변경을 수행 할 수 없다고 봄.
	//
	if ( pOwner->isAction( GLAT_ATTACK ) || pOwner->isAction( GLAT_SKILL ) )
        return E_FAIL;

	if ( !IsValidSlot( pPacket->emSuit ) )
	{
		return E_FAIL;
	}

	// 펫 장착창에 아이템이 없어야 한다.
	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype( pPacket->emSuit );
	if ( sSlotItemCustom.GetNativeID() != NATIVEID_NULL() )
	{
		return E_FAIL;
	}

    SINVENITEM* pHoldInvenItem = pOwner->InvenGetItem( pPacket->wHoldPosX, pPacket->wHoldPosY );
    if ( !pHoldInvenItem )
        return E_FAIL;

	const SITEM* pHoldItem = GLogicData::GetInstance().GetItem( pHoldInvenItem->CustomItem().GetNativeID() );
	if ( !pHoldItem ) 
	{
		// 일반오류
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_FAIL;
		m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );
		return E_FAIL;
	}

	if ( pHoldItem->sBasicOp.emItemType != ITEM_SUIT )
        return E_FAIL;

	if ( pHoldItem->sSuitOp.emSuit != SUIT_PET_A && pHoldItem->sSuitOp.emSuit != SUIT_PET_B )
	{
		// 팻용 아이템이 아닐때
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_INVALIDITEM;
		m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );

		return E_FAIL;
	}

	// 서로 다른 타입일때
	if ( pHoldItem->sSuitOp.emSuit != pPacket->emSuit )
	{
		NetMsgFB.emFB = EMPET_REQ_SLOT_EX_HOLD_FB_NOMATCH;
		m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );

		return E_FAIL; 
	}

	if ( !CheckSlotItem( pHoldItem->sBasicOp.sNativeID, pPacket->emSuit ) )
        return E_FAIL;

	SITEMCUSTOM sHoldItemCustom = pHoldInvenItem->CustomItem();

    // 손에든 아이템을 제거하고
    if ( !pOwner->InvenDeleteItem( pPacket->wHoldPosX, pPacket->wHoldPosY, false, sHoldItemCustom.bVietnamGainItem, 1, false ) )
        return E_FAIL;

	// 손에든 아이템을 팻에게 장착시키고
	if ( !SetSlotItem( pPacket->emSuit, sHoldItemCustom ) )
	{
		// 문제 발생 삭제된 인벤토리 아이템 복구
		if ( !pOwner->InvenInsertItem( sHoldItemCustom, pPacket->wHoldPosX, pPacket->wHoldPosY, false, 1, false ) )
		{
			pOwner->DropItem( sHoldItemCustom, pOwner->GetPosition() );
		}
		return E_FAIL;
	}

	// 인벤토리 아이템 삭제 알림
	pOwner->InvenDeleteSend( pPacket->wHoldPosX, pPacket->wHoldPosY, sHoldItemCustom.bVietnamGainItem );

	// 펫 장착창에 들어간 아이템 간단한 디비 업데이트(위치 변경만 업데이트함)
	PetPutOnItemUpdate( pPacket->emSuit, true );

	// 자신에게 알림
	NetMsgFB.emFB		 = EMPET_REQ_SLOT_EX_HOLD_FB_OK;
	NetMsgFB.emSuit		 = pPacket->emSuit;
	NetMsgFB.sItemCustom = GetSlotitembySuittype( pPacket->emSuit );
	m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetMsgBRD( GetSlotitembySuittype( pPacket->emSuit ), pPacket->emSuit );
	NetMsgBRD.dwGUID = m_dwGUID;
	pOwner->SendMsgViewAround( &NetMsgBRD );
	
	return S_OK;
}

// Slot -> Hold
HRESULT	GLPetField::MsgPetAccSlotToInven( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_SLOT_TO_INVEN* pPacket = ( GLMSG::SNETPET_REQ_SLOT_TO_INVEN* ) nmg;
	
	GLChar* pOwner = m_pGLGaeaServer->GetChar64( m_OwnerDbNum, m_OwnerSvrGroupNum );
    if ( !pOwner )
        return E_FAIL;

	//	Note : 공격중이나 스킬 시전 중에 슬롯 변경을 수행 할 수 없다고 봄.
	//
	if ( pOwner->isAction( GLAT_ATTACK ) || pOwner->isAction( GLAT_SKILL ) )
        return E_FAIL;

	if ( !IsValidSlot( pPacket->emSuit ) )
	{
		return E_FAIL;
	}

	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype( pPacket->emSuit );
	const SITEM* pSlotItem = GLogicData::GetInstance().GetItem( sSlotItemCustom.GetNativeID() );
	if ( !pSlotItem ) 
	{
		// 일반오류
        GLMSG::SNETPET_REQ_SLOT_TO_HOLD_FB NetMsgFB( EMPET_REQ_SLOT_EX_HOLD_FB_FAIL, SUIT_NSIZE );
		m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );
		return E_FAIL;
	}

	if ( !pOwner->InvenIsInsertable( pPacket->wPosX, pPacket->wPosY ) )
	{
		return E_FAIL;
	}

	// 장착된 아이템을 제거한다. IsValidSlot 확인했기 때문에 무조건 성공한다. 리턴값 처리 하지 않는다.
	ReSetSlotItem( pPacket->emSuit );

	// 장착된 아이템을 인벤토리로.
    if ( !pOwner->InvenInsertItem( sSlotItemCustom, pPacket->wPosX, pPacket->wPosY, true, 1, false ) )
        return E_FAIL;

	// 저장 위치 및 좌표만 디비 업데이트
	pOwner->InvenUpdateItem( pOwner->InvenGetItem( pPacket->wPosX, pPacket->wPosY ), true );

    // 자신에게 알림
    GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB NetMsgFB( pPacket->emSuit );
    m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );

	// 다른 클라이언트 화면에 아이템이 바뀐 팻이 출력되어야 한다
	GLMSG::SNETPET_REQ_SLOT_EX_HOLD_BRD NetMsgBRD( SITEMCUSTOM( NATIVEID_NULL() ), pPacket->emSuit );
	NetMsgBRD.dwGUID = m_dwGUID;
	pOwner->SendMsgViewAround( &NetMsgBRD );
	
	return S_OK;
}

HRESULT	GLPetField::MsgPetLearnSkill(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_LEARNSKILL* pPacket = ( GLMSG::SNETPET_REQ_LEARNSKILL* ) nmg;

    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return E_FAIL;

	GLMSG::SNETPET_REQ_LEARNSKILL_FB NetMsgFB;
	SINVENITEM* pInvenItem(NULL);

	// 창고에 있는 아이템 여부
	if ( pPacket->bStorage )
	{
		if ( !pOwner->IsKEEP_STORAGE (pPacket->dwChannel) )
		{
			NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_INVALIDSTORAGE;
			m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
			return E_FAIL;
		}

		pInvenItem = pOwner->UserStorageFindPosItem( pPacket->dwChannel, pPacket->wPosX, pPacket->wPosY );
		if ( !pInvenItem )
		{
			NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_NOITEM;
			m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
			return E_FAIL;
		}
	}
	else
	{
		pInvenItem = pOwner->InvenFindPosItem( pPacket->wPosX, pPacket->wPosY );
		if ( !pInvenItem )
		{
			NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_NOITEM;
			m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
			return E_FAIL;
		}		
	}

	if ( pOwner->CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
        return E_FAIL;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );

	// 팻용 스킬인지 유무????
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_PET_SKILL )
	{
		NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_BADITEM;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	if ( !pOwner->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	SNATIVEID sSKILL_ID(pItem->sSkillBookOp.sSkill_ID.dwID);

	if ( ISLEARNED_SKILL( sSKILL_ID ) )
	{
		//	이미 습득한 스킬.
		NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_ALREADY;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	// 스킬습득
	LEARN_SKILL ( sSKILL_ID );

	NetMsgFB.emFB = EMPET_REQ_LEARNSKILL_FB_OK;
	NetMsgFB.wPosX = pPacket->wPosX;
	NetMsgFB.wPosY = pPacket->wPosY;
	m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);

	if ( GLHIDESET::CheckHideSet(EMHIDE_PET_SUMMON_RELEASE) )
	{
		pOwner->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
		pOwner->RestoreActStateByInvisible();
	}

	// 스킬 배움을 알림 (이펙트)
	GLMSG::SNETPET_REQ_LEARNSKILL_BRD NetMsgBRD;
	NetMsgBRD.dwGUID = m_dwGUID;
	pOwner->SendMsgViewAround(&NetMsgBRD);

	// 아이템 소모시킴(인벤토리/창고)
	if (pPacket->bStorage)
        pOwner->ConsumeStorageItem( pPacket->dwChannel, pPacket->wPosX, pPacket->wPosY );
	else
        pOwner->ConsumeInvenItem( pPacket->wPosX, pPacket->wPosY );

	// 펫 액션 로그
    GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
	pItemLimit->LogPetAction(m_DbNum, pInvenItem->GetNativeID(), pet::EM_ACTION_SKILL_LEARN, m_nFull);

	if (sSKILL_ID.Mid() != PET_SKILL_MAIN)
	{
        sc::writeLogError(
            sc::string::format(
                "GLPetField::MsgPetLearnSkill %1% %2%",  
                sSKILL_ID.wMainID,
                sSKILL_ID.wSubID));
	}

	// DB 저장
	PETSKILL sPetSkill( sSKILL_ID, 0 );
	AddGameDbAdoJob(
        db::DbActionPtr(
            new db::PetSkillSet(pOwner->m_CharDbNum, m_DbNum, sPetSkill, 0)));
	return S_OK;
}

HRESULT	GLPetField::MsgPetSkillSlotExpansion(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_REQ_SKILLSLOT_EXPANSION* pPacket = ( GLMSG::SNETPET_REQ_SKILLSLOT_EXPANSION* ) nmg;
    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return E_FAIL;

	GLMSG::SNETPET_REQ_SKILLSLOT_EXPANSION_FB NetMsgFB;
	SINVENITEM* pInvenItem(NULL);

	// 타입 복사
	NetMsgFB.emType = pPacket->emType;

	// 창고에 있는 아이템 여부
	if ( pPacket->bStorage )
	{
		if ( !pOwner->IsKEEP_STORAGE (pPacket->dwChannel) )
		{
			NetMsgFB.emFB = EMPET_REQ_SKILLSLOT_EXPANSION_FB_INVALIDSTORAGE;
			m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
			return E_FAIL;
		}

		pInvenItem = pOwner->UserStorageFindPosItem( pPacket->dwChannel, pPacket->wPosX, pPacket->wPosY );
		if ( !pInvenItem )
		{
			NetMsgFB.emFB = EMPET_REQ_SKILLSLOT_EXPANSION_FB_NOITEM;
			m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
			return E_FAIL;
		}
	}
	else
	{
		pInvenItem = pOwner->InvenFindPosItem( pPacket->wPosX, pPacket->wPosY );
		if ( !pInvenItem )
		{
			NetMsgFB.emFB = EMPET_REQ_SKILLSLOT_EXPANSION_FB_NOITEM;
			m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
			return E_FAIL;
		}		
	}

	if ( pOwner->CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
        return E_FAIL;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );

	if ( !pItem )
	{
		NetMsgFB.emFB = EMPET_REQ_SKILLSLOT_EXPANSION_FB_BADITEM;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	// Note : 레벨 조건 확인
	//
	if ( !pOwner->SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return E_FAIL;
	}

	//  Note : 확장 스킬 타입에 따라처리
	switch ( pItem->sBasicOp.emItemType )
	{
	case ITEM_PET_DUALSKILL:
		{
			// Note : 요청 타입이 다름
			if ( pPacket->emType != EMPET_SKILLSLOT_EXPANSION_TYPE_DUAL )
			{
				NetMsgFB.emFB = EMPET_REQ_SKILLSLOT_EXPANSION_FB_BADITEM;
				m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
				return E_FAIL;
			}
			// Note : 이미배움
			if ( m_wMaxSkillSlot >= 2 )
			{
				NetMsgFB.emFB = EMPET_REQ_SKILLSLOT_EXPANSION_FB_ALREADY;
				m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
				return E_FAIL;
			}
			// Note : 최대치에 도달
			else if ( m_wMaxSkillSlot >= MAX_PETSKILLSLOT )
			{
				NetMsgFB.emFB = EMPET_REQ_SKILLSLOT_EXPANSION_FB_ALREADY;
				m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(),&NetMsgFB);
				return E_FAIL;
			}

			m_wMaxSkillSlot = 2;

			// 펫 액션 로그
            GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
			pItemLimit->LogPetAction(m_DbNum, pInvenItem->sItemCustom.GetNativeID(), pet::EM_ACTION_PET_DUALSKILL, m_nFull);
		}
		break;

	default:
		{
			// Note : 잘못된 아이템
			NetMsgFB.emFB = EMPET_REQ_SKILLSLOT_EXPANSION_FB_BADITEM;
			m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
			return E_FAIL;
		}
	}

	NetMsgFB.emFB	  = EMPET_REQ_SKILLSLOT_EXPANSION_FB_OK;
	NetMsgFB.wPosX	  = pPacket->wPosX;
	NetMsgFB.wPosY	  = pPacket->wPosY;
	NetMsgFB.wMaxSlot = m_wMaxSkillSlot;
	m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);

	// 스킬 확장을 알림 (이펙트)
	GLMSG::SNETPET_REQ_SKILLSLOT_EXPANSION_BRD NetMsgBRD(m_wMaxSkillSlot);
	NetMsgBRD.dwGUID   = m_dwGUID;
	pOwner->SendMsgViewAround(&NetMsgBRD);

	// 아이템 소모시킴(인벤토리/창고)
	if ( pPacket->bStorage )
        pOwner->ConsumeStorageItem( pPacket->dwChannel, pPacket->wPosX, pPacket->wPosY );
	else
        pOwner->ConsumeInvenItem( pPacket->wPosX, pPacket->wPosY );

	// DB 저장
	AddGameDbAdoJob(
        db::DbActionPtr(
            new db::PetSkillSlotNumSet(pOwner->m_CharDbNum, m_DbNum, m_wMaxSkillSlot)));
	return S_OK;
}

HRESULT	GLPetField::MsgPetRemoveSlotItem( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM* pPacket = ( GLMSG::SNETPET_REQ_REMOVE_SLOTITEM* ) nmg;

    GLChar* pOwner = m_pGLGaeaServer->GetChar64( m_OwnerDbNum, m_OwnerSvrGroupNum );
    if ( !pOwner )
        return E_FAIL;

	if ( !IsValidSlot( pPacket->emSuit ) )
	{
		return E_FAIL;
	}

	SITEMCUSTOM	sSlotItemCustom = GetSlotitembySuittype( pPacket->emSuit );
	const SITEM* pSlotItem = GLogicData::GetInstance().GetItem( sSlotItemCustom.GetNativeID() );
	if ( !pSlotItem ) 
	{
		return E_FAIL;
	}

	WORD wPosX( 0 ), wPosY( 0 );
	BOOL bOk = pOwner->InvenFindInsertable( wPosX, wPosY );
	if ( !bOk )
	{
		return E_FAIL;
	}

	// 인벤에 넣기
    if ( !pOwner->InvenInsertItem( sSlotItemCustom, wPosX, wPosY, true, 1, false ) )
    {
		return E_FAIL;
    }

	// 슬롯아이템 제거, 위에서 IsValidSlot 확인했기 때문에 리턴값 확인하지 않는다.
	ReSetSlotItem( pPacket->emSuit );

	// 디비 업데이트
	pOwner->InvenUpdateItem( pOwner->InvenGetItem( wPosX, wPosY ), true );

	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_FB NetMsgFB( pPacket->emSuit );
	m_pGLGaeaServer->SENDTOCLIENT( pOwner->ClientSlot(), &NetMsgFB );

	// 슬롯아이템 제거를 알림
	GLMSG::SNETPET_REQ_REMOVE_SLOTITEM_BRD NetMsgBRD( pPacket->emSuit );
	NetMsgBRD.dwGUID = m_dwGUID;
	pOwner->SendMsgViewAround( &NetMsgBRD );

	return S_OK;
}

HRESULT GLPetField::MsgPetSkinPackItem(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPET_SKINPACKOPEN *pPacket = (GLMSG::SNETPET_SKINPACKOPEN *)nmg;

    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return E_FAIL;

	GLMSG::SNETPET_SKINPACKOPEN_FB NetMsgFB;

    SINVENITEM* pHoldInvenItem = pOwner->InvenGetItem( pPacket->wHoldPosX, pPacket->wHoldPosY );

	if (!pHoldInvenItem)
        return S_FALSE;

	const SITEM* pHold = GLogicData::GetInstance().GetItem( pHoldInvenItem->Id() );

	SINVENITEM* pInvenItem = pOwner->InvenGetItem( pPacket->wPosX, pPacket->wPosY );
	if (!pInvenItem)
        return E_FAIL;
	if (pInvenItem->sItemCustom.PetDbNum() != m_DbNum)
        return E_FAIL;

	if ( !pHold || pHold->sBasicOp.emItemType != ITEM_PET_SKIN_PACK || pHold->sBasicOp.sNativeID != pPacket->sItemId )										
	{
		NetMsgFB.emFB = EMPET_PETSKINPACKOPEN_FB_FAIL;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}	

	if ( pOwner->CheckCoolTime( pHold->sBasicOp.sNativeID ) )
        return E_FAIL;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_PET_CARD  )
	{
		NetMsgFB.emFB = EMPET_PETSKINPACKOPEN_FB_PETCARD_FAIL;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	if ( !pHold->sPetSkinPack.VALID() )
	{
		NetMsgFB.emFB = EMPET_PETSKINPACKOPEN_FB_FAIL;
		m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);
		return E_FAIL;
	}

	float fNowRate = sc::Random::getInstance().GetFloatPercent();
	float fCurRate = 0.0f;
	size_t i = 0;
	for( i = 0; i < pHold->sPetSkinPack.vecPetSkinData.size(); i++ )
	{
		float fPreRate = fCurRate;
		fCurRate += pHold->sPetSkinPack.vecPetSkinData[i].fRate;

		if( ( fPreRate < fNowRate ) && ( fNowRate < fCurRate ) )
			break;
		
	}

	if ( i >= pHold->sPetSkinPack.vecPetSkinData.size() )
		i = pHold->sPetSkinPack.vecPetSkinData.size() - 1;

	m_sPetSkinPackData.Init();
	m_sPetSkinPackData.bUsePetSkinPack = TRUE;
	m_sPetSkinPackData.sMobID		   = pHold->sPetSkinPack.vecPetSkinData[i].sMobID;
	m_sPetSkinPackData.fScale		   = pHold->sPetSkinPack.vecPetSkinData[i].fScale;	
	m_sPetSkinPackData.dwPetSkinTime   = pHold->sPetSkinPack.dwPetSkinTime;
	m_sPetSkinPackData.startTime	   = CTime::GetCurrentTime();

	pInvenItem->sItemCustom.tBORNTIME = m_sPetSkinPackData.startTime.GetTime();
	pInvenItem->sItemCustom.tDISGUISE = pHold->sPetSkinPack.dwPetSkinTime;

	int nScale = (int)(m_sPetSkinPackData.fScale * 100.0f);

	AddGameDbAdoJob(
        db::DbActionPtr(
            new db::PetSkinPackSet(
                pOwner->m_CharDbNum,
                m_DbNum,
                m_sPetSkinPackData.sMobID.wMainID,
                m_sPetSkinPackData.sMobID.wSubID,
                nScale,
                m_sPetSkinPackData.dwPetSkinTime)));	

	// 펫 액션 로그
    GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
	pItemLimit->LogPetAction(m_DbNum, pInvenItem->sItemCustom.GetNativeID(), pet::EM_ACTION_PETSKINPACK, m_nFull);

    SINVENITEM sHold_BackUp = *pHoldInvenItem;	//	손에 든 아이템 백업.
    WORD wHoldPosX = sHold_BackUp.wPosX;		//	손에 든 아이템 위치.
    WORD wHoldPosY = sHold_BackUp.wPosY;

	//	성공시 아이탬 제거.
    pOwner->ConsumeInvenItem( wHoldPosX, wHoldPosY );

	NetMsgFB.emFB			  = EMPET_PETSKINPACKOPEN_FB_OK;
	NetMsgFB.sPetSkinPackData = m_sPetSkinPackData;
	m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetMsgFB);

	//	[자신에게] 손 있었던 아이탬 제거.
//	GLMSG::SNETPC_PUTON_RELEASE NetMsg_ReleaseHold(SLOT_HOLD);
//	m_pGLGaeaServer->SENDTOCLIENT(m_pOwner->m_dwClientID,&NetMsg_ReleaseHold);	

	// 코스튬 처럼 sItemCustom 구조체를 사용한다.
	GLMSG::SNET_INVEN_ITEM_UPDATE NetItemUpdate;
	NetItemUpdate.wPosX = pPacket->wPosX;
	NetItemUpdate.wPosY = pPacket->wPosY;
	NetItemUpdate.sItemCustom = pInvenItem->sItemCustom;
	m_pGLGaeaServer->SENDTOCLIENT(pOwner->ClientSlot(), &NetItemUpdate);

	GLMSG::SNETPET_SKINPACKOPEN_BRD NetMsgBRD;
	NetMsgBRD.dwGUID		   = m_dwGUID;
	NetMsgBRD.sPetSkinPackData = m_sPetSkinPackData;
	pOwner->SendMsgViewAround(&NetMsgBRD);


	return S_OK;
}

/*void GLPetField::MsgProcess(NET_MSG_GENERIC* nmg)
{
	switch ( nmg->nType )
	{
	case NET_MSG_PET_REQ_GOTO:											
		MsgPetGoto ( nmg );
		break;

	case NET_MSG_PET_REQ_STOP:
		MsgPetStop ( nmg );
		break;

	case NET_MSG_PET_REQ_UPDATE_MOVE_STATE:
		MsgPetUpdateMoveState ( nmg );
		break;

	case NET_MSG_PET_REQ_SKILLCHANGE:
		MsgPetChangeActiveSkill ( nmg );
		break;

	case NET_MSG_PET_REQ_RENAME:
		MsgPetRename ( nmg );
		break;

	case NET_MSG_PET_REQ_RENAME_FROMDB_FB:
		PetRenameFeedBack ( nmg );
		break;

	case NET_MSG_PET_REQ_CHANGE_COLOR:
		MsgPetChangeColor ( nmg );
		break;
	case NET_MSG_PET_REQ_CHANGE_STYLE:
		MsgPetChangeStyle ( nmg );
		break;

	case NET_MSG_PET_REQ_SLOT_EX_HOLD:
		MsgPetAccHoldExSlot ( nmg );
		break;

	case NET_MSG_PET_REQ_HOLD_TO_SLOT:
		MsgPetAccHoldToSlot ( nmg );
		break;

	case NET_MSG_PET_REQ_SLOT_TO_HOLD:
		MsgPetAccSlotToHold ( nmg );
		break;

	case NET_MSG_PET_REQ_LEARNSKILL:
		MsgPetLearnSkill ( nmg );
		break;

	case NET_MSG_PET_REQ_SKILLSLOT_EXPANSION:
		MsgPetSkillSlotExpansion ( nmg );
		break;

	case NET_MSG_PET_REQ_FUNNY:
		{
			MsgPetFunnyReq(nmg);
		}
		break;

	case NET_MSG_PET_REMOVE_SLOTITEM:
		MsgPetRemoveSlotItem ( nmg );
		break;

	case NET_MSG_PET_PETSKINPACKOPEN:
		MsgPetSkinPackItem ( nmg );
		break;

	default:

		sc::writeLogWarn(sc::string::format("[WARNING]GLPetField::MsgProcess Illigal Message(%d)", nmg->nType));

		break;
	};
}*/

HRESULT GLPetField::MsgPetFunnyReq( NET_MSG_GENERIC* nmg)
{
    GLChar* pOwner = m_pGLGaeaServer->GetChar64(m_OwnerDbNum, m_OwnerSvrGroupNum);
    if (!pOwner)
        return E_FAIL;

	GLMSG::SNETPET_REQ_FUNNY* pPacket = ( GLMSG::SNETPET_REQ_FUNNY* ) nmg;
	GLMSG::SNETPET_REQ_FUNNY_BRD NetMsgBRD(pPacket->wFunny);
	NetMsgBRD.dwGUID = m_dwGUID;
	pOwner->SendMsgViewAround(&NetMsgBRD);

	m_wAniSub = pPacket->wFunny;
	SetSTATE ( EM_PETACT_FUNNY );
	return S_OK;
}
