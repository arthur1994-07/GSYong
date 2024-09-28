#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Land/GLLandManClient.h"
#include "../GLGaeaClient.h"
#include "../GLogicExClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Strike/GLStrikeM.h"
#include "../Char/GLCharacter.h"

#include "./GLCharClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

HRESULT GLCharClient::MsgMoveState(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_MOVESTATE_BRD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_MOVESTATE_BRD*> ( nmg );

    BOOL bRun = IsActState ( EM_ACT_RUN );
    BOOL bToRun = pNetMsg->dwActState&EM_ACT_RUN;

    if ( bRun != bToRun )
    {
        //	Note : 이동 상태 변경.
		if ( bToRun )
			SetActState(EM_ACT_RUN);
		else 
			ReSetActState(EM_ACT_RUN);


		ClientActor::SetMaxSpeed ( GetMoveVelo () );
    }

	bool bInvisibleTarget = false;
    if ( pNetMsg->dwActState & EM_REQ_VISIBLENONE )
	{
		SetActState ( EM_REQ_VISIBLENONE );
		bInvisibleTarget = true;
	}
    else
		ReSetActState ( EM_REQ_VISIBLENONE );

    if ( pNetMsg->dwActState & EM_REQ_VISIBLEOFF )
	{
		SetActState ( EM_REQ_VISIBLEOFF );
		bInvisibleTarget = true;
	}
    else
		ReSetActState ( EM_REQ_VISIBLEOFF );

    if ( pNetMsg->dwActState & EM_ACT_PEACEMODE )
		SetActState ( EM_ACT_PEACEMODE );
    else
		ReSetActState ( EM_ACT_PEACEMODE );

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( bInvisibleTarget )
	{
		if ( (pCharacter->GetSelectID() == STARGETID(CROW_PC, m_dwGaeaID)) )
		{
			pCharacter->ResetSelectID(); // 선택 대상 ID 널;
			pCharacter->ResetCursorID();
			pCharacter->ResetActionID();
		}
	}	

    return S_OK;
}

HRESULT GLCharClient::MsgGoto(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_GOTO_BRD *pNetMsg = reinterpret_cast<GLMSG::SNETPC_GOTO_BRD*> ( nmg );

    if ( pNetMsg->dwActState&EM_ACT_RUN )	SetActState ( EM_ACT_RUN );
    else									ReSetActState ( EM_ACT_RUN );

    m_sTargetID.vPos = pNetMsg->vTarPos;
    TurnAction ( GLAT_MOVE );

    const BOOL bSucceed = ClientActor::GotoLocation(m_sTargetID.vPos, 10.0f);

    if ( bSucceed )
        ClientActor::SetMaxSpeed ( GetMoveVelo() );

    return S_OK;
}

void GLCharClient::StartAttackProc ()
{
    m_nattSTEP = 0;
    m_fattTIMER = 0.0f;

    if( m_pSkinChar )  m_pSkinChar->ResetStrikeAttachBone();
}

void GLCharClient::AttackEffect ( const SANIMSTRIKE &sStrikeEff )
{
    BOOL bOk = m_pGaeaClient->ValidCheckTarget ( m_sTargetID );
    if ( !bOk )	return;

    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( m_sTargetID );
    if ( pActor )	pActor->ReceiveSwing ();

    //	타겟의 위치.
    D3DXVECTOR3 vTARPOS = m_pGaeaClient->GetTargetPos ( m_sTargetID );
	const D3DXVECTOR3& _vPosition = GetPosition();

    D3DXVECTOR3 vDir = vTARPOS - _vPosition;
    vDir.y = 0.f;
    D3DXVec3Normalize ( &vDir, &vDir );

    vTARPOS.y += 15.0f;

    D3DXVECTOR3 vTarDir = vTARPOS - _vPosition;
    float fLength = D3DXVec3Length ( &vTarDir );

    D3DXVec3Normalize ( &vTarDir, &vTarDir );
    D3DXVECTOR3 vTARPAR = _vPosition + vTarDir*fLength * 10.0f;

    //	타격 위치를 알아냄.
    STRIKE::SSTRIKE sStrike;
    STRIKE::CSELECTOR Selector ( vTARPAR, m_pSkinChar->m_PartArray );
    bOk = Selector.SELECT ( sStrikeEff.m_emPiece, sStrike );
    if ( !bOk )	return;

    EMSLOT emSlot = PIECE_2_SLOT(sStrikeEff.m_emPiece);
    if( emSlot==SLOT_TSIZE )		return;

    //	오른손 무기만 적용되게 한다. 듀얼 CPS 때문이다. 
    //	LHAND 도구 설정했는데 무기는 오른손에만 있을경우 때문이다.
    //	if( emSlot==SLOT_RHAND )		emSlot = GetCurRHand();
    //	else if( emSlot==SLOT_LHAND )	emSlot = GetCurLHand();
    emSlot = GetCurRHand();

    D3DXMATRIX matEffect;
    D3DXMatrixTranslation ( &matEffect, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );

    STARGETID sTargetID = m_sTargetID;
    sTargetID.vPos = vTARPOS;

    //	Note : 무기의 속성 살펴봄.
    //
    EMELEMENT emELEMENT = GET_ITEM_ELMT();

    if ( VALID_SLOT_ITEM(emSlot) )
    {
        SITEM *pItem = GLogicData::GetInstance().GetItem(GET_SLOT_ITEM(emSlot).sNativeID);
        if ( pItem )
        {
            if ( !ISLONGRANGE_ARMS( pItem->sSuitOp.emAttack ) )
            {
                std::string strEFFECT = pItem->GetTargetEffect();
                if ( strEFFECT.empty() )	strEFFECT = GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT);

                DxEffGroupPlayer::GetInstance().NewEffGroup
                    (
                    strEFFECT.c_str(),
                    matEffect,
                    &sTargetID,
					TRUE, 
					FALSE
                    );

                DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
            }
            else
            {
				std::string strEFFECT = pItem->GetTargetEffect();
				if ( strEFFECT.empty() )
				{
					const GLITEM_ATT emItemAtt = CONVERT_ITEMATT( pItem->sSuitOp.emAttack );
					switch ( emItemAtt )
					{
					case ITEMATT_BOW:
						strEFFECT = GLCONST_CHAR::strSTRIKE_ARROW_EFFECT;
						break;

					case ITEMATT_PISTOL:
					case ITEMATT_RAILGUN:
					case ITEMATT_PORTALGUN:
						strEFFECT = GLCONST_CHAR::strSTRIKE_BULLET_EFFECT;
						break;
					}
				}

				if ( false == strEFFECT.empty() )
				{
					SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup(
						strEFFECT.c_str(),
						matEffect,
						&sTargetID,
						TRUE,
						FALSE );

					if ( spEffSingleG.get() )
					{
						spEffSingleG->AddEffAfter ( EFFASINGLE, GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT)  );
						spEffSingleG->AddEffAfter ( EFFABODY, std::string(pItem->GetTargBodyEffect()) );
						spEffSingleG->AddEffAfter ( EFFABODY, GLCONST_CHAR::strAMBIENT_EFFECT );
					}
					else
					{
						DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT), TRUE, TRUE );
						//spEffSingleG->AddEffAfter ( EFFABODY, std::string(pLHAND->GetTargBodyEffect()) );
						//spEffSingleG->AddEffAfter ( EFFABODY, GLCONST_CHAR::strAMBIENT_EFFECT );
					}
				}
            }
        }
    }
    else
    {
        DxEffGroupPlayer::GetInstance().NewEffGroup
            (
            GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT),
            matEffect,
            &sTargetID,
			TRUE,
			FALSE
            );

        DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
    }
}

BOOL GLCharClient::AttackProc ( float fElapsedTime )
{
    m_fattTIMER += fElapsedTime * (GETATTVELO() + GETATT_ITEM()) * UNITANIKEY_PERSEC;

	const DxSkinAnimation* pAnimCont = m_pSkinChar->GETCURANIM();
    //GASSERT(pAnimCont);
	if ( !pAnimCont )
		return FALSE;

    if ( pAnimCont->m_wStrikeCount == 0 )
		return FALSE;

    const DWORD dwThisKey = static_cast<DWORD>(m_fattTIMER);
	const DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;

    if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
    {
        m_pSkinChar->UpdateStrikeAttachBone( dwThisKey, m_nattSTEP );
        if ( dwThisKey>=dwStrikeKey )
        {
            if ( m_pGaeaClient->IsVisibleCV(m_sTargetID) )
            {
                //	Note : 공격시 이팩트 발휘..
                //
                AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
            }

            m_nattSTEP++;
        }
    }

    return TRUE;
}

BOOL GLCharClient::AttackVehicleProc ( float fElapsedTime )
{
    m_fattTIMER += fElapsedTime * GETATTVELO();
    m_fattTIMER += GETATT_ITEM();

	const DxSkinAnimation* pAnimCont = m_sVehicle.m_pSkinChar->GETCURANIM();
    //GASSERT(pAnimCont); 
	if ( !pAnimCont )
		return FALSE;

    if ( pAnimCont->m_wStrikeCount == 0 )
		return FALSE;

    DWORD dwThisKey = DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

    DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
    if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
    {
        m_sVehicle.m_pSkinChar->UpdateStrikeAttachBone( dwThisKey, m_nattSTEP );
        if ( dwThisKey>=dwStrikeKey )
        {
            if ( m_pGaeaClient->IsVisibleCV(m_sTargetID) )
            {
                //	Note : 공격시 이팩트 발휘..
                //
                AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
            }

            m_nattSTEP++;
        }
    }

    return TRUE;
}

void GLCharClient::StartSkillProc ()
{
    m_nattSTEP = 0;
    m_fattTIMER = 0.0f;

    if( m_pSkinChar )
		m_pSkinChar->ResetStrikeAttachBone();

	if ( RENDERPARAM::g_bForceLowHardwareMode )
		return;

    PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
    if ( !pSkill )
		return;

    //	Note : 공격 방향으로 회전.
    //
	const D3DXVECTOR3 vReDirect = UpdateSkillDirection(m_pGaeaClient, GetPosition(), GetDirect(), m_idACTIVESKILL, m_vTARPOS, m_sTARIDS);
	ClientActor::SetDirect(vReDirect);

    STARGETID sTARG(GETCROW(),m_dwGaeaID);

    //	Note : 스킬 시작과 동시에 나타나는 이팩트. 발동.
    //
    EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
    if ( pSkill->m_sEXT_DATA.emSELFBODY==SKILL::EMTIME_FIRST )
        DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &vReDirect);

    EMSLOT emRHand = GetCurRHand();

    SANIMSTRIKE sStrike;
    sStrike.m_emPiece = VALID_SLOT_ITEM(emRHand) ? PIECE_RHAND : PIECE_GLOVE;
    sStrike.m_emEffect = EMSF_TARGET;
    sStrike.m_dwFrame = 0;

    if ( pSkill->m_sEXT_DATA.emSELFZONE01==SKILL::EMTIME_FIRST )
        SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

    if ( pSkill->m_sEXT_DATA.emSELFZONE02==SKILL::EMTIME_FIRST )
        SK_EFF_SELFZONE ( sStrike, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );
}

void GLCharClient::SetHiddenWeaponPiece()
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if( !pSkill )
		return;

	const bool bHiddenWeapon = pSkill->m_sLEARN.bHiddenWeapon;
	if( !bHiddenWeapon )
		return;

	// 스킬에 사용되는 무기를 세팅한다;
	SNATIVEID sHiddenWeapon = pSkill->m_sLEARN.sHiddenWeapon;
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sHiddenWeapon );
	if( pItem )
	{
		// [shhan][2014.12.18] rm #571 - 극강부 사격스킬(건류/극강글러브류) 후 왼손 권총 잔상문제.
		//						망토로 인해 ResetHiddenWeaponPiece 에서 UpdateSuit 안 했더니 
		//						양손권총 무기 사용 후 한손무기 스킬을 사용하면 다른한손에 권총이 보이는 버그가 있었다.
		//						그래서 활성화 된 위치만 렌더링 되도록 수정작업함.
		EMPIECECHAR emType(PIECE_RHAND_HIDDEN);
		if ( m_pSkinChar->SetPieceHiddenWeapon ( pItem->GetWearingFile(m_CHARINDEX),	m_pd3dDevice, 0, EMSCD_ALL_THREAD, emType, pItem->GetSelfBodyEffect() ) == S_OK )
		{
			if ( emType == PIECE_RHAND_HIDDEN || emType == PIECE_LHAND_HIDDEN )
			{
				m_pSkinChar->SetPartRend( emType, TRUE );
			}
		}
		if ( m_pSkinChar->SetPieceHiddenWeapon ( pItem->GetWearingFileEx(m_CHARINDEX),	m_pd3dDevice, 0, EMSCD_ALL_THREAD,  emType, pItem->GetSelfBodyEffect() ) == S_OK )
		{
			if ( emType == PIECE_RHAND_HIDDEN || emType == PIECE_LHAND_HIDDEN )
			{
				m_pSkinChar->SetPartRend( emType, TRUE );
			}
		}
	}
}

void GLCharClient::ResetHiddenWeaponPiece()
{
	PDXCHARPART pRHAND_HIDDEN = m_pSkinChar->GetPiece( PIECE_RHAND_HIDDEN );
	PDXCHARPART pLHAND_HIDDEN = m_pSkinChar->GetPiece( PIECE_LHAND_HIDDEN );
	if( pRHAND_HIDDEN->IsActiveCharPart() || pLHAND_HIDDEN->IsActiveCharPart() )
	{
		m_pSkinChar->SetPartRend ( PIECE_RHAND_HIDDEN, FALSE );
		m_pSkinChar->SetPartRend ( PIECE_LHAND_HIDDEN, FALSE );

		// [shhan][2014.11.20] 마술부에서 UpdateSuit 를 하면 망토가 리셋되면서 흔들리는 현상이 있어서 막아봄.
		//						일단 주석해도 될 듯 한데, 문제가 되면 망토도 생각하면서 수정작업 필요함.
		//UpdateSuit();
	}
}

void GLCharClient::SKT_EFF_HOLDOUT ( STARGETID sTarget, DWORD dwDamageFlag )
{
    for ( int i=0; i<SKILLFACT_SIZE; ++i )
    {
        SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
        if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKEFF.sNATIVEID.wMainID, sSKEFF.sNATIVEID.wSubID );
        if ( !pSkill )							continue;		

        if ( !pSkill->m_sEXT_DATA.strHOLDOUT.empty() )
        {

            bool bEffect = false;

            for ( DWORD j = 0; j < pSkill->m_sAPPLY.vecSPEC.size(); ++j )
            {

                if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_PSY_DAMAGE_REDUCE && 
                    dwDamageFlag & DAMAGE_TYPE_PSY_REDUCE)
                {
                    bEffect = true;
                    break;
                }

                if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_MAGIC_DAMAGE_REDUCE && 
                    dwDamageFlag & DAMAGE_TYPE_MAGIC_REDUCE)
                {
                    bEffect = true;
                    break;
                }

                if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_PSY_DAMAGE_REFLECTION && 
                    dwDamageFlag & DAMAGE_TYPE_PSY_REFLECTION)
                {
                    bEffect = true;
                    break;
                }

                if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_MAGIC_DAMAGE_REFLECTION && 
                    dwDamageFlag & DAMAGE_TYPE_MAGIC_REFLECTION)
                {
                    bEffect = true;
                    break;
                }

                if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_CURSE && 
                    dwDamageFlag & DAMAGE_TYPE_CURSE)
                {
                    bEffect = true;
                    break;
                }

				if (pSkill->m_sAPPLY.vecSPEC[j].emSPEC == EMSPECA_ILLUSION && 
					dwDamageFlag & DAMAGE_TYPE_ILLUSION)
				{
					bEffect = true;
					break;
				}
            }

            if ( !bEffect )
				continue;

            const char*			szZONE_EFF	= pSkill->m_sEXT_DATA.strHOLDOUT.c_str();
			const D3DXVECTOR3&	_vPosition	= GetPosition();
            D3DXVECTOR3			vDIR		= sTarget.vPos - _vPosition;

            D3DXVECTOR3 vDIR_ORG(1,0,0);
            float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

            D3DXMATRIX matTrans;
            D3DXMatrixRotationY ( &matTrans, fdir_y );
            matTrans._41 = _vPosition.x;
            matTrans._42 = _vPosition.y;
            matTrans._43 = _vPosition.z;

            //	Note : 자기 위치 이펙트 발생시킴.
			DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &sTarget, TRUE, FALSE );
        }
    }
}

void GLCharClient::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
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

    //	Note : 자기 위치 이펙트 발생시킴.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, pTarget, TRUE, FALSE );
}

void GLCharClient::SK_EFF_TARZONE ( const STARGETID &_sTarget, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF )
{
    D3DXMATRIX matTrans;
    STARGETID sTarget = _sTarget;

	const D3DXVECTOR3& _vPosition = GetPosition();

    //	방향 회전	
    D3DXVECTOR3 vDIR = _vPosition - sTarget.vPos;

    D3DXVECTOR3 vDIR_ORG(1,0,0);
    float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

    D3DXMatrixRotationY ( &matTrans, fdir_y );

    //	스트라이크 지점에서.
    BOOL bTRANS(FALSE);
    if ( emPOS == SKILL::EMPOS_STRIKE )
    {
        DxSkinChar* pSkin = m_pGaeaClient->GetSkinChar ( sTarget );

        //	타겟의 위치.
        D3DXVECTOR3 vTARPOS ( _vPosition.x, _vPosition.y + 15.0f, _vPosition.z );;

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

    //	Note : 목표 위치 이펙트 발생시킴.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, &sTarget, TRUE, FALSE );
}

void GLCharClient::SK_EFF_TARG ( const PGLSKILL pSkill, const SANIMSTRIKE &sStrikeEff, const STARGETID &_sTarget )
{
    D3DXMATRIX matTrans;
    STARGETID sTarget = _sTarget;

	const D3DXVECTOR3 _vPosition = GetPosition();

    //	스트라이크 지점에서.
    BOOL bTRANS(FALSE);
    if ( pSkill->m_sEXT_DATA.emTARG_POSA == SKILL::EMPOS_STRIKE )
    {
        //	타겟의 위치.
        D3DXVECTOR3 vSTRPOS ( sTarget.vPos.x, sTarget.vPos.y+15, sTarget.vPos.z );		

        D3DXVECTOR3 vDir = vSTRPOS - _vPosition;
        const float fLength = D3DXVec3Length ( &vDir ) * 10.0f;

        D3DXVec3Normalize ( &vDir, &vDir );
        D3DXVECTOR3 vSTRPAR = _vPosition + vDir * fLength;

        //	스트라이크 위치를 알아냄.
        STRIKE::SSTRIKE sStrike;


		bool bAttBone = false;
		if ( m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
		{
			bAttBone = !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->m_strAttBoneName.empty();
		}

		bool bExtreme = false;
		const SITEM* pRightItem = GET_SLOT_ITEMDATA( GetCurRHand() );
		if( pRightItem )
		{
			bExtreme = ( pRightItem->sSuitOp.emAttack == ITEMATT_EXTREME_GLOVE && (m_emClass & GLCC_EXTREME) );
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
		else if( bExtreme )
		{
			STRIKE::CSELECTOR Selector ( vSTRPAR, m_pSkinChar->m_PartArray );
			if( sStrikeEff.m_emPiece == PIECE_RHAND )
				bOk = Selector.SELECT ( PIECE_RHAND_HIDDEN, sStrike );
			else if( sStrikeEff.m_emPiece == PIECE_LHAND  )
				bOk = Selector.SELECT ( PIECE_LHAND_HIDDEN, sStrike );
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
        D3DXVECTOR3 vTARPOS ( _vPosition.x, _vPosition.y+15, _vPosition.z );;

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

    // V607 Ownerless expression 'sTarget.vPos'. glcharclientex.cpp 605
    //if ( !bIMPACT )
    //{
    //    sTarget.vPos;
    //}

    EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
	SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARG(emELMT), matTrans, &sTarget, TRUE, FALSE );
    if ( spEffSingleG.get() )
	{
		//	Note : 목표 위치 이펙트.
		if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFASINGLE, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

		//	Note : 목표 몸 이펙트.
		if ( pSkill->m_sEXT_DATA.emTARGBODY01==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT) );

		if ( pSkill->m_sEXT_DATA.emTARGBODY02==SKILL::EMTIME_TARG_OVR )
			spEffSingleG->AddEffAfter ( EFFABODY, pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT) );
	}
	else
	{
		//	Note : 목표 위치 이펙트.
		if ( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_TARG_OVR )
			DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, FALSE );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_TARG_OVR )
			DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, FALSE );		
	}
}

void GLCharClient::SkillEffectZone( STARGETID& sTarget, const SANIMSTRIKE &sStrikeEff )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )	return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	const D3DXVECTOR3& _vPosition = GetPosition();

	EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);

	if ( pSkill->m_sEXT_DATA.emTARG == EMTIME_IMPACT )
	{
		//D3DXMatrixTranslation ( &matTrans, m_vTARPOS.x, m_vTARPOS.y, m_vTARPOS.z );
		D3DXMatrixTranslation ( &matTrans, sTarget.vPos.x, sTarget.vPos.y, sTarget.vPos.z );
		SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE );
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
		DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE  );
}

void GLCharClient::SkillEffectSpecific( STARGETID& sTarget, const SANIMSTRIKE &sStrikeEff )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )	return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	const D3DXVECTOR3& _vPosition = GetPosition();

	EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);

	D3DXVECTOR3 vDir = sTarget.vPos - _vPosition;
	vDir.y = 0.f;
	D3DXVec3Normalize ( &vDir, &vDir );

	//	Note : 목표 지향 이펙트에 실어 보내는 이펙트.
	//		( 실어 보낼때는 목표 위치 이펙트의 위치는 자동으로 지향 이팩트의 종료 지점으로 고정. )
	if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
	{
		SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
	}
	//	단독으로 목표 지점에 발생.
	else
	{
		//	Note : 목표 위치 이펙트.
		if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
			SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

		if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
			SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

		//	Note : 목표 몸 이펙트.
		if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
		if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
	}
}

void GLCharClient::SkillEffectUnit( const SANIMSTRIKE &sStrikeEff )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )	return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	const D3DXVECTOR3& _vPosition = GetPosition();

	EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);

	if ( m_wTARNUM )
	{
		STARGETID sCId   = STARGETID(CROW_PC, GetGaeaID());
		CSkillCAIdx* pCharCa   = GetSkillCA();
		
		for ( WORD i=0; i<m_wTARNUM; ++i )
		{
			bool bTargetEffect = false;

			CSkillCAInterface cSkillCA;
			
			STARGETID sCTaId = STARGETID(m_sTARIDS[i].emCrow, m_sTARIDS[i].dwID);

			CSkillCAIdx* pClientCa = &m_sTARCA[i];

			if( pClientCa->GetCaData() != NULL )
			{
				cSkillCA.Cal_LastApplyValueII( pSkill->m_sBASIC.sNATIVEID, m_wACTIVESKILL_LEVEL, pCharCa, pClientCa, sCId, sCTaId, true );

				STARGETID sTemp = cSkillCA.Cal_LastDamage( pSkill->m_sBASIC.sNATIVEID, m_wACTIVESKILL_LEVEL, pCharCa, pClientCa, sCId, sCTaId, true );

				if( (sTemp != sCTaId) || pCharCa->IsSimPacAddon() || pCharCa->IsSpecddon() )
				{
					STARGETID	sTarget = sCId;

					sTarget.vPos = m_pGaeaClient->GetTargetPos( sTarget );

					D3DXVECTOR3 vDir = sTarget.vPos - _vPosition;
					vDir.y = 0.f;
					D3DXVec3Normalize ( &vDir, &vDir );

					if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
					{
						SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
					}
					//	단독으로 목표 지점에 발생.
					else
					{
						//	Note : 목표 위치 이펙트.
						if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
							SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

						if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
							SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

						//	Note : 목표 몸 이펙트.
						if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
							DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
						if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
							DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
					}
				}

				if( (sTemp == sCTaId) || pClientCa->IsSpecddon() || pClientCa->IsSimPacAddon() )
				{
					bTargetEffect = true;
				}
			}
			else
			{
				bTargetEffect = true;
			}

			if( !bTargetEffect ) continue;

			STARGETID sTarget(m_sTARIDS[i].emCrow, m_sTARIDS[i].dwID);
			if ( !m_pGaeaClient->IsVisibleCV(sTarget) )	continue;
			sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

			D3DXVECTOR3 vDir = sTarget.vPos - _vPosition;
			vDir.y = 0.f;
			D3DXVec3Normalize ( &vDir, &vDir );

			//	Note : 목표 지향 이펙트에 실어 보내는 이펙트.
			//		( 실어 보낼때는 목표 위치 이펙트의 위치는 자동으로 지향 이팩트의 종료 지점으로 고정. )
			if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
			{
				SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
			}
			//	단독으로 목표 지점에 발생.
			else
			{
				//	Note : 목표 위치 이펙트.
				if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
					SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

				if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
					SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

				//	Note : 목표 몸 이펙트.
				if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
					DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
				if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
					DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
			}

			if ( pSkill->m_sEXT_DATA.bTARG_ONE )	break;
		}
	}
}

void GLCharClient::SkillEffectTarget( STARGETID& sTarget, const SANIMSTRIKE &sStrikeEff )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
	if ( !pSkill )	return;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	CSkillCAInterface cSkillCA;

	CSkillCAIdx* pCharCa   = GetSkillCA();
	CSkillCAIdx* pClientCa = &m_sTARCA[m_wTARNUM-1];
		
	STARGETID sCId   = STARGETID(CROW_PC, GetGaeaID());
	STARGETID sCTaId = STARGETID(m_sTARIDS[m_wTARNUM-1].emCrow, m_sTARIDS[m_wTARNUM-1].dwID);;

	if( pClientCa->GetCaData() ) 
	{
		cSkillCA.Cal_LastApplyValueII( pSkill->m_sBASIC.sNATIVEID, m_wACTIVESKILL_LEVEL, pCharCa, pClientCa, sCId, sCTaId, true );

		STARGETID sTemp = cSkillCA.Cal_LastDamage( pSkill->m_sBASIC.sNATIVEID, m_wACTIVESKILL_LEVEL, pCharCa, pClientCa, sCId, sCTaId, true );

		if( (sTemp != sCTaId) || pCharCa->IsSimPacAddon() || pCharCa->IsSpecddon() )
		{
			STARGETID sTemp = sCId;

			sTemp.vPos = GetPosition();
			
			switch( pSkill->m_sBASIC.emIMPACT_TAR )
			{
			case TAR_ZONE		 : SkillEffectZone( sTemp, sStrikeEff );     break;
			case TAR_SELF_TOSPEC : SkillEffectSpecific( sTemp, sStrikeEff ); break;
			}
		}

		if( (sTemp == sCTaId) || pClientCa->IsSimPacAddon() || pClientCa->IsSpecddon() )
		{
			switch( pSkill->m_sBASIC.emIMPACT_TAR )
			{
			case TAR_ZONE		 : SkillEffectZone( sTarget, sStrikeEff );     break;
			case TAR_SELF_TOSPEC : SkillEffectSpecific( sTarget, sStrikeEff ); break;
			}
		}
	}
	else
	{
		switch( pSkill->m_sBASIC.emIMPACT_TAR )
		{
		case TAR_ZONE		 : SkillEffectZone( sTarget, sStrikeEff );     break;
		case TAR_SELF_TOSPEC : SkillEffectSpecific( sTarget, sStrikeEff ); break;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void GLCharClient::SkillEffect ( const SANIMSTRIKE &sStrikeEff )
{
	if ( RENDERPARAM::g_bForceLowHardwareMode )
		return;

    PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
    if ( !pSkill )							return;

    D3DXMATRIX matTrans;
    using namespace SKILL;

	const D3DXVECTOR3& _vPosition = GetPosition();
	
    //	Note : 타겟 이펙트가 지면을 향해 발사되는 경우.
    //
    EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);

    if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_ZONE )
    {
		STARGETID sTarget = STARGETID(m_sTARIDS[m_wTARNUM-1].emCrow, m_sTARIDS[m_wTARNUM-1].dwID);

		sTarget.vPos = m_vTARPOS;

		//SkillEffectZone( sTarget, sStrikeEff );
		SkillEffectTarget( sTarget, sStrikeEff );
    }
    //	Note : 타겟 이펙트가 지면을 향해 발사되는 경우.
    //		자기 위치에서 대상 위치까지. ( 화살 관통 )
    else if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
    {
        if ( m_wTARNUM == 0 )	return;

        STARGETID sTarget(m_sTARIDS[m_wTARNUM-1].emCrow, m_sTARIDS[m_wTARNUM-1].dwID);
        if ( !m_pGaeaClient->IsVisibleCV(sTarget) )		return;
        sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

        //SkillEffectSpecific( sTarget, sStrikeEff );
		SkillEffectTarget( sTarget, sStrikeEff );
    }
    //	Note : 타겟 이펙트가 유닛을 향해 발사되는 경우.
    //
    else
    {
        SkillEffectUnit(sStrikeEff);        
    }

    STARGETID sTARG(GETCROW(),m_dwGaeaID);

    if ( pSkill->m_sEXT_DATA.emSELFZONE01 == EMTIME_IMPACT )
        SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

    if ( pSkill->m_sEXT_DATA.emSELFZONE02 == EMTIME_IMPACT )
        SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

    if ( pSkill->m_sEXT_DATA.emSELFBODY == EMTIME_IMPACT )
        DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &GetDirect() );
}

void GLCharClient::SkillEffectDealyAction( const SANIMSTRIKE &sStrikeEff, SNATIVEID sSkillId, SKILLTARGETDATA *sSkillTargetData )
{
	if ( RENDERPARAM::g_bForceLowHardwareMode )
		return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillId.wMainID, sSkillId.wSubID );
	if ( !pSkill )
		return;

	D3DXMATRIX matTrans;
	using namespace SKILL;

	const D3DXVECTOR3& _vPosition = GetPosition();

	//	Note : 타겟 이펙트가 지면을 향해 발사되는 경우.
	//
	EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
	if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_ZONE )
	{
		if ( pSkill->m_sEXT_DATA.emTARG == EMTIME_IMPACT )
		{
			const D3DXVECTOR3& vPosition = sSkillTargetData->TargetID.vPos;
			D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
			SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE );
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
			DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), matTrans, &TARGETID_NULL, TRUE, FALSE  );
	}
	//	Note : 타겟 이펙트가 지면을 향해 발사되는 경우.
	//		자기 위치에서 대상 위치까지. ( 화살 관통 )
	else if ( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SELF_TOSPEC )
	{
		if ( sSkillTargetData->nTarget == 0 )	return;

		STARGETID sTarget(
			sSkillTargetData->TargetIDs[sSkillTargetData->nTarget-1].emCrow,
			sSkillTargetData->TargetIDs[sSkillTargetData->nTarget-1].dwID);
		if ( !m_pGaeaClient->IsVisibleCV(sTarget) )		return;
		sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

		D3DXVECTOR3 vDir = sTarget.vPos - _vPosition;
		vDir.y = 0.f;
		D3DXVec3Normalize ( &vDir, &vDir );

		//	Note : 목표 지향 이펙트에 실어 보내는 이펙트.
		//		( 실어 보낼때는 목표 위치 이펙트의 위치는 자동으로 지향 이팩트의 종료 지점으로 고정. )
		if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
		{
			SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
		}
		//	단독으로 목표 지점에 발생.
		else
		{
			//	Note : 목표 위치 이펙트.
			if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
				SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

			//	Note : 목표 몸 이펙트.
			if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
			if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
		}
	}
	//	Note : 타겟 이펙트가 유닛을 향해 발사되는 경우.
	//
	else
	{
		//	Note : 목표 유닛이 있는 경우.
		//
		if ( sSkillTargetData->nTarget )
		{
			for ( WORD i=0; i<sSkillTargetData->nTarget; ++i )
			{
				STARGETID sTarget(
					sSkillTargetData->TargetIDs[i].emCrow,
					sSkillTargetData->TargetIDs[i].dwID);
				if ( !m_pGaeaClient->IsVisibleCV(sTarget) )	continue;
				sTarget.vPos = m_pGaeaClient->GetTargetPos(sTarget);

				D3DXVECTOR3 vDir = sTarget.vPos - _vPosition;
				vDir.y = 0.f;
				D3DXVec3Normalize ( &vDir, &vDir );

				//	Note : 목표 지향 이펙트에 실어 보내는 이펙트.
				//		( 실어 보낼때는 목표 위치 이펙트의 위치는 자동으로 지향 이팩트의 종료 지점으로 고정. )
				if ( pSkill->m_sEXT_DATA.VALIDTARG(emELMT) )
				{
					SK_EFF_TARG ( pSkill, sStrikeEff, sTarget );
				}
				//	단독으로 목표 지점에 발생.
				else
				{
					//	Note : 목표 위치 이펙트.
					if ( pSkill->m_sEXT_DATA.emTARGZONE01==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

					if ( pSkill->m_sEXT_DATA.emTARGZONE02==EMTIME_IMPACT )
						SK_EFF_TARZONE ( sTarget, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

					//	Note : 목표 몸 이펙트.
					if ( pSkill->m_sEXT_DATA.emTARGBODY01==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
					if ( pSkill->m_sEXT_DATA.emTARGBODY02==EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
				}

				if ( pSkill->m_sEXT_DATA.bTARG_ONE )	break;
			}
		}
	}


	STARGETID sTARG(GETCROW(),m_dwGaeaID);

	if ( pSkill->m_sEXT_DATA.emSELFZONE01 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFZONE02 == EMTIME_IMPACT )
		SK_EFF_SELFZONE ( sStrikeEff, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if ( pSkill->m_sEXT_DATA.emSELFBODY == EMTIME_IMPACT )
		DxEffGroupPlayer::GetInstance().NewEffBody ( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &GetDirect() );
}


BOOL GLCharClient::SkillProc ( float fElapsedTime )
{
    m_fattTIMER += fElapsedTime * (GETATTVELO() + GETATT_ITEM()) * UNITANIKEY_PERSEC;
    //GASSERT(m_pSkinChar->GETCURANIMNODE());

    //PANIMCONTNODE pAnicontNode = m_pSkinChar->GETCURANIMNODE();
    //GASSERT(pAnicontNode);
    //DxSkinAnimation* pAnimCont = pAnicontNode->m_pDxSkinAni;

	const DxSkinAnimation* pAnimCont = m_pSkinChar->GETCURANIM();
	if ( !pAnimCont )
		return FALSE;

    if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

    const DWORD dwThisKey = pAnimCont->m_dwSTime + static_cast<DWORD>(m_fattTIMER);

    const DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
    if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
    {
        if ( dwThisKey>=dwStrikeKey )
        {
            //	Note : 스킬 이펙트 발동.
            //
            SkillEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );

            m_nattSTEP++;
        }
    }

    return TRUE;
}

void GLCharClient::UPDATE_ITEM ( )
{
    m_fITEM_MOVE_R = 0.0f;
    m_fITEMATTVELO_R = 0.0f;
    m_fITEM_MOVE = 0.0f;
    m_fITEMATTVELO = 0.0f;

    for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
    {
        EMSLOT emSLOT = static_cast<EMSLOT>(i);
        if (!VALID_SLOT_ITEM(emSLOT))
            continue;

        SITEMCUSTOM sItemCustom;
        const SITEMCLIENT& sItemClient = m_CharData.m_PutOnItems[i];
        sItemCustom.Assign( sItemClient );

        SITEM* sItem = GET_SLOT_ITEMDATA( emSLOT );
        if ( !sItem ) continue;

        //	Note : 변화율 효과.
		for ( INT k = 0; k < ITEM::SSUIT::ADDON_SIZE; ++k )
		{
			switch ( sItem->sSuitOp.sADDON[k].emTYPE )
			{
			case EMADDEX_INCR_HP:
			case EMADDEX_INCR_MP:
			case EMADDEX_INCR_SP:
			case EMADDEX_INCR_AP:
				break;
			case EMADDEX_INCR_MOVE_SPEED:
				if (emSLOT != SLOT_VEHICLE)
					m_fITEM_MOVE_R += sItemCustom.GETMOVESPEEDR();
				else if (m_emVehicle)
					m_fITEM_MOVE_R += m_sVehicle.GetSpeedRate() + sItemCustom.GETMOVESPEEDR();
				break;
			case EMADDEX_INCR_ATTACK_SPEED:
				m_fITEMATTVELO_R	+= sItem->sSuitOp.sADDON[k].fVALUE;
				break;
			case EMADDEX_INCR_CRITICAL_RATE:
				break;
			case EMADDEX_INCR_CRUSHING_BLOW:
				break;
			case EMADDEX_INC_HP:
			case EMADDEX_INC_MP:
			case EMADDEX_INC_SP:
			case EMADDEX_INC_AP:
				break;
			case EMADDEX_INC_MOVE_SPEED:
				if ( emSLOT != SLOT_VEHICLE )
					m_fITEM_MOVE += sItemCustom.GETMOVESPEED();
				else if ( m_emVehicle )
					m_fITEM_MOVE += m_sVehicle.GetSpeedVol() + sItemCustom.GETMOVESPEED();
				break;
			case EMADDEX_INC_ATTACK_SPEED:
				m_fITEMATTVELO += sItem->sSuitOp.sADDON[k].fVALUE;
				break;
			case EMADDEX_INC_CRITICAL_RATE:
				break;
			case EMADDEX_INC_CRUSHING_BLOW:
				break;
			}
		}
        // 이동속도 랜덤옵션 적용( 파츠는 위에서 적용 m_sVehicle.GetSpeedRate(); ) 
        //		if ( emSLOT != SLOT_VEHICLE )	m_fITEM_MOVE += sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE );
        //		else if ( m_bVehicle ) m_fITEM_MOVE += sItemCustom.GETOptVALUE( RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE );

    }
}

void GLCharClient::UpdateMapEffect()
{
    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
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

void GLCharClient::UpdateLandEffect()
{
    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
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
            if( m_dwTransformSkill != SNATIVEID::ID_NULL )
            {
                m_dwTransformSkill = SNATIVEID::ID_NULL;
                UpdateSuit();
            }
            for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
            {
                FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,vPosition), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );

				m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, m_sSKILLFACT[ i ].sNATIVEID.dwID, i );
                RESETSKEFF(i);
            }			
            return;
        }
    }
}

void GLCharClient::UPDATE_DATA ( float fTime, float fElapsedTime, BOOL bClient )
{
	ZeroMemory(m_bSkillFactsSpecial, sizeof(m_bSkillFactsSpecial));
    m_fSTATE_MOVE_RATE		= 1.0f;
	m_fSTATE_MOVE_VALUE = 0;
    m_fSTATE_DELAY		= 1.0f;

    m_fSKILL_MOVE		= 0.0f;
    m_fATTVELO			= 0.0f;

    m_bSTATE_PANT 		= false;
    m_bSTATE_STUN 		= false;

	m_sDamageSpec.RESET();
	m_sDamageSpec = m_sSUM_PASSIVE.m_sDamageSpec;
	m_sDefenseSkill.RESET();
	m_sINVISIBLE.RESET();	
	m_sRELEASE_ENDURE.RESET();
	m_sHALLUCINATE.RESET();
	m_sDOMINATE.RESET();
	m_sIncreaseEff.RESET();
    m_bSafeZone			= false;
    
    m_sLINKHP.fTake= 1.0f;
    m_sLINKHP.fRef= 0.0f;

    m_fMoveVeloLimit_Min = -1.0f;
    m_fMoveVeloLimit_Max = 1000.0f;

	RF_SCREEN_THROW( m_EffSkillVarSet ).Reset();
	RF_ENTRANCE_EXIT( m_EffSkillVarSet ).ResetValue();
	RF_FEIGN_DEATH( m_EffSkillVarSet ).ResetValue();
	RF_DISGUISE( m_EffSkillVarSet ).ResetValue();
	RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
	RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
	RF_ACTIVE( m_EffSkillVarSet ).ResetValue();
	
    memset ( m_bActionType, false, sizeof( bool ) * SKILL::EMACTION_TYPE_NSIZE );

    for ( int i=0; i<SKILLFACT_SIZE; ++i )
    {
        SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
        if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;
        if ( !sSKEFF.bEnable )						continue;

        if ( !sSKEFF.IsAgeInfinite() )
        {
            sSKEFF.fAGE -= fElapsedTime;
        }

        if ( !bClient )
        {
            if ( !sSKEFF.IsAlive() )
            {
				m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, sSKEFF.sNATIVEID.dwID, i );
                RESETSKEFF(i);
            }
        }

        if ( bClient ) 
        {
            PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSKEFF.sNATIVEID );
            if ( pSkill ) 
            {
                m_bActionType[pSkill->m_sBASIC.emACTION_TYPE] = true;
            }
        }

        for ( DWORD j = 0; j < sSKEFF.vecSPEC.size(); ++j )
        {
            // Note : 탈 것 타고 효과 유지.
            bool bKeepSpecAddonSkill = true;

            if ( m_emVehicle != EMVEHICLE_OFF )
            {
                for ( int emSpec = 0; emSpec < GLCONST_CHAR::wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM; emSpec++ )
                {
                    if ( sSKEFF.vecSPEC[j].emSPEC == GLCONST_CHAR::wNON_APPLY_SKILL_SPEC_ON_VEHICLE[ emSpec ] )
                    {
                        bKeepSpecAddonSkill = false;
                        break;
                    }
                }
            }

            if ( bKeepSpecAddonSkill )
            {
				m_bSkillFactsSpecial[sSKEFF.vecSPEC[j].emSPEC] = true;
                SKILL::SSPEC& _sSpec = sSKEFF.vecSPEC[j].sSPEC;

                switch ( sSKEFF.vecSPEC[j].emSPEC )
                {
				case EMSPECA_ACTIVATE_DAMAGE :
					RF_ACTIVE( m_EffSkillVarSet ).TurnOn();
					break;
				case EMSPECA_SPECIAL_IMMUNE :
					//RF_IMMUNE( m_EffSkillVarSet ).TurnOn( (BYTE)_sSpec.nVAR1 );
					RF_IMMUNE( m_EffSkillVarSet ).TurnOn( 0 );
					break;
				case EMSPECA_PROVOKE    :
					{
						WORD wValue = (WORD)_sSpec.fVAR1;

						if( wValue == NULL_DISTANCE ) wValue = 0;

						RF_PROVOKE( m_EffSkillVarSet ).TurnOn( sSKEFF.sID, wValue );
					}					
					break;
				case EMSPECA_DISGUISE :
					RF_DISGUISE( m_EffSkillVarSet ).TurnOn( (int)_sSpec.fVAR2, (int)_sSpec.fVAR1 );
					break;
				case EMSPECA_SCREEN_THROW :
					RF_SCREEN_THROW( m_EffSkillVarSet ).TurnOn( sSKEFF.sID, _sSpec.fVAR1 );
					break;
				case EMSPECA_CAMERA_HIT :
					RF_CAMERA_HIT( m_EffSkillVarSet ).TurnOn( (WORD)_sSpec.fVAR1 );
					break;
				case EMSPECA_FEIGN_DEATH :
					RF_FEIGN_DEATH( m_EffSkillVarSet ).TurnOn( _sSpec.dwFLAG&FEIGN_DEATH_TARGETING_DISABLE, _sSpec.dwFLAG&FEIGN_DEATH_BUF_DEL, i );
					break;
				case EMSPECA_ENTRANCE_EXIT :
					RF_ENTRANCE_EXIT( m_EffSkillVarSet ).TurnOn();
					break;
				case EMSPECA_COUNTATTACK :
					m_SkillCAIndex.SetData( SNATIVEID(_sSpec.fVAR1, _sSpec.fVAR2 ),  &sSKEFF.pcCAData, i, sSKEFF.sNATIVEID, (int)sSKEFF.wLEVEL );
					break;
                case EMSPECA_MOVEVELO:
                    m_fSKILL_MOVE += _sSpec.fVAR1;
                    break;

				case EMSPECA_INVISIBLE:
					m_sINVISIBLE.SET(_sSpec);
					break;
				case EMSPECA_RECVISIBLE:		
					m_sRECVISIBLE.SET(_sSpec);
					break;

                case EMSPECA_VEHICLE_GETOFF: //봉주                    
					m_bSkillFactsSpecial[EMSPECA_VEHICLE_GETOFF] = DWORD(_sSpec.fVAR2) ? true : false;          
                    break;
                case EMSPECA_LINK_HP:
                    if ( static_cast<int>(_sSpec.fVAR2) == 1 )
                    {
                        if ( _sSpec.fVAR1 < m_sLINKHP.fTake )
                            m_sLINKHP.fTake = _sSpec.fVAR1;
                    }
                    break;

                case EMSPECA_ATTACKVELO:
                    //	공격 속도 계념이므로 ( 공격 시간을 -0.1 (-10%) 감소 시키기 위해서는 부호 반전을 하여야함. )
                    m_fATTVELO -= _sSpec.fVAR1;
                    break;

                case EMSPECA_SKILLDELAY:
                    break;

				case EMSPECA_TAUNT:					
					break;

				case EMSPECA_TURN_STATE_BATTLE:
					m_bSkillFactsSpecial[EMSPECA_TURN_STATE_BATTLE] = m_bSkillFactsSpecial[EMSPECA_TURN_STATE_PEACE] ? false : true;
					break;

				case EMSPECA_TURN_STATE_PEACE:
					m_bSkillFactsSpecial[EMSPECA_TURN_STATE_BATTLE] = false;
					break;

                case EMSPECA_STUN:
                    {
                        //	다른 항목은 상태이상 기절 효과로 대체
                        m_fSTATE_MOVE_RATE = 0.0f;								//	기절.
                        m_bSTATE_STUN = true;
                    }
                    break;
				case EMSPECA_RELEASE_ENDURE:		
					m_sRELEASE_ENDURE.SET(_sSpec, i);
					break;
				case EMSPECA_RELEASE_PET:
					m_bSkillFactsSpecial[EMSPECA_RELEASE_PET] = DWORD(_sSpec.fVAR1) ? true : false;		
					break;
				case EMSPECA_IMMUNE:
					m_sDamageSpec.m_emImmuneApplyType |= DWORD(_sSpec.fVAR1);
					m_sDamageSpec.m_emImmuneActionType |= DWORD(_sSpec.fVAR2);
					break;
				case EMSPECA_STIGMA:
					{
						STARGETID sTargetID(CROW_PC, m_dwUserID);
						m_sSTIGMA_TAKER.PushStigmaPack(sSKEFF.sID, sTargetID, sSKEFF.sNATIVEID, _sSpec);
					}		
					break;
				case EMSPECA_HALLUCINATE:
					if ( m_CharData.m_Base.sHP.nNow )
						m_sHALLUCINATE.SET(_sSpec);
					break;
				case EMSPECA_DOMINATION:
					m_sDOMINATE.SET(_sSpec);
					break;
				case EMSPECA_VIEW_RANGE_RATE:
					if ( _sSpec.fVAR1 < m_sRateViewRange.fRate )
						m_sRateViewRange = _sSpec.fVAR1;
					break;
				case EMSPECA_CLONE:
					m_sClone.SetClonNum(static_cast<int>(_sSpec.fVAR1));
					break;

                case EMSPECA_MOVEVELO_LIMIT:
                    {
                        if ( _sSpec.fVAR1 >= 0.0f )
                            m_fMoveVeloLimit_Min = max(m_fMoveVeloLimit_Min, _sSpec.fVAR1); // fVar1 : 최소 이속제한 (1.0 == 100%) 더 큰 값으로 갱신된다.;
                        if ( _sSpec.fVAR2 >= 0.0f )
                            m_fMoveVeloLimit_Max = min(m_fMoveVeloLimit_Max, _sSpec.fVAR2); // fVar2 : 최대 이속제한 (1.0 == 100%) 더 작은 값으로 갱신된다.;
                    }
                    break;
                };
            }
        }
    }

    for ( int i=0; i<EMBLOW_MULTI; ++i )
    {
        SSTATEBLOW &sSTATEBLOW = m_sSTATEBLOWS[i];
        if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )		continue;

        sSTATEBLOW.fAGE -= fElapsedTime;

        switch ( sSTATEBLOW.emBLOW )
        {
        case EMBLOW_NUMB:
            m_fSTATE_MOVE_RATE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
            m_fSTATE_DELAY = 1.0f + sSTATEBLOW.fSTATE_VAR2;		//	딜래이 증가.
            break;

        case EMBLOW_STUN:
            m_fSTATE_MOVE_RATE = 0.0f;								//	기절.
            m_bSTATE_PANT = true;
            m_bSTATE_STUN = true;
            break;

        case EMBLOW_STONE:
            m_fSTATE_MOVE_RATE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
            break;

        case EMBLOW_BURN:
            break;

        case EMBLOW_FROZEN:
            m_fSTATE_MOVE_RATE += sSTATEBLOW.fSTATE_VAR1;			//	이동속도 감소.
            break;

        case EMBLOW_MAD:
            m_bSTATE_PANT = true;
            break;

        case EMBLOW_POISON:
            break;

        case EMBLOW_CURSE:
            break;
        };
    }

    // Note : 탈 것 타고 효과 유지.
    bool bKeepQBox = true;

    if ( m_emVehicle != EMVEHICLE_OFF )
    {
        for ( int i = 0; i < GLCONST_CHAR::wNON_APPLY_QBOX_ON_VEHICLE_NUM; i++ )
        {
            // Note : 탈 것 타고 있을 때 적용안할 QBOX 효과들.
            if ( m_CharData.m_Base.sQITEMFACT.emType == GLCONST_CHAR::wNON_APPLY_QBOX_ON_VEHICLE[ i ] )
            {
                bKeepQBox = false;
                break;
            }
        }
    }

    if ( bKeepQBox )
    {
        switch ( m_CharData.m_Base.sQITEMFACT.emType )
        {
        case QUESTION_SPEED_UP:
            m_fSKILL_MOVE += (m_CharData.m_Base.sQITEMFACT.wParam1/100.0f);
            break;

        case QUESTION_CRAZY:
            m_fSKILL_MOVE += (m_CharData.m_Base.sQITEMFACT.wParam1/100.0f);
            m_fATTVELO += (m_CharData.m_Base.sQITEMFACT.wParam2/100.0f);
            break;

        case QUESTION_ATTACK_UP:
            break;

        case QUESTION_EXP_UP:
            break;

        case QUESTION_LUCKY:
            break;

        case QUESTION_SPEED_UP_M:
            m_fSKILL_MOVE += (m_CharData.m_Base.sQITEMFACT.wParam1/100.0f);
            break;

        case QUESTION_MADNESS:
            m_fSKILL_MOVE += (m_CharData.m_Base.sQITEMFACT.wParam1/100.0f);
            m_fATTVELO += (m_CharData.m_Base.sQITEMFACT.wParam2/100.0f);
            break;

        case QUESTION_ATTACK_UP_M:
            break;
        };
    }


    // 지형 효과 처리
    for ( size_t i = 0; i < m_vecLandEffect.size(); ++i )	
    {
        SLANDEFFECT_VALUE landEffect = m_vecLandEffect[i];
        if( !landEffect.IsUse() ) continue;

        CDebugSet::ToView( 4, (int)i, "%d %2.2f", landEffect.emLandEffectType, landEffect.fValue );

        switch( landEffect.emLandEffectType ) 
        {
        case EMLANDEFFECT_ATK_SPEED: // 공격속도
            m_fATTVELO	  += landEffect.fValue;
            break;
        case EMLANDEFFECT_MOVE_SPEED: // 이동속도
            m_fSKILL_MOVE += landEffect.fValue;
            break;
        case EMLANDEFFECT_HP_RATE:	  // HP 증폭율
        case EMLANDEFFECT_MP_RATE:    // MP 증폭율
        case EMLANDEFFECT_RECOVER_RATE: // 회복율
        case EMLANDEFFECT_DAMAGE_RATE:	// 공격치 변화율
        case EMLANDEFFECT_DEFENSE_RATE:	// 방어치 변화율
        case EMLANDEFFECT_RESIST_RATE:	// 저항 수치			
        case EMLANDEFFECT_CANCEL_ALLBUFF: // 모든 버프 취소
            break;
        case EMLANDEFFECT_SAFE_ZONE:
            m_bSafeZone = true;
            break;
        }
    }

    //	맵 효과들
    for ( size_t i = 0; i < m_vecMapEffect.size(); ++i ) 
    {
        SMAP_EFFECT mapEffect = m_vecMapEffect[i];

        CDebugSet::ToView( 6, (int)i, "%d %2.2f", mapEffect.emType, mapEffect.fRate );

        if ( mapEffect.emType == EMMAP_EFFECT_NONE ) continue;

        switch( mapEffect.emType ) 
        {
        case EMMAP_EFFECT_ATTACK_SPEED: // 공격속도
            m_fATTVELO	  += mapEffect.fRate;
            break;
        case EMMAP_EFFECT_MOVE_SPEED: // 이동속도
            m_fSKILL_MOVE += mapEffect.fRate;
            break;
        case EMMAP_EFFECT_HP_RATE:	  // HP 증폭률
        case EMMAP_EFFECT_MP_RATE:    // MP 증폭률
        case EMMAP_EFFECT_ALL_RATE: // 회복율
        case EMMAP_EFFECT_DAMAGE_RATE:	// 공격치 변화율
        case EMMAP_EFFECT_DEFENSE_RATE:	// 방어치 변화율
            break;
        }
    }


    //	Memo :	? 아이템 이벤트 적용
    if( m_CharData.m_Base.sEVENTFACT.IsACTIVE( EMGM_EVENT_SPEED ) )
        m_fSKILL_MOVE += (m_CharData.m_Base.sEVENTFACT.wSpeed/100.0f);
    if( m_CharData.m_Base.sEVENTFACT.IsACTIVE( EMGM_EVENT_ASPEED) )
        m_fATTVELO += (m_CharData.m_Base.sEVENTFACT.wASpeed/100.0f);

    // IP Event 적용.
    if ( m_CharData.m_Base.sEVENTFACT.IsIPEvent() )
    {
        DWORD AttackSpeed_IPEvent = ran_event::GLIPEvent::Instance()->GetAttackSpeed();
        DWORD MoveSpeed_IPEvent = ran_event::GLIPEvent::Instance()->GetMoveSpeed();

        if ( AttackSpeed_IPEvent != 0 )
            m_fATTVELO += (AttackSpeed_IPEvent*0.01f);

        if ( MoveSpeed_IPEvent != 0 )
            m_fSKILL_MOVE += (MoveSpeed_IPEvent*0.01f);
    }
    //

    //	공격/이동 속도. ( 지속 skill 속성 가산된 부분에 state, passive skill 속성 가산. )
    m_fATTVELO += m_fSTATE_MOVE_RATE + m_CharData.m_Base.sPASSIVE_SKILL.m_fATTVELO;
    m_fSKILL_MOVE += m_CharData.m_Base.sPASSIVE_SKILL.m_fMOVEVELO;	
}

void GLCharClient::RestoreActStateByInvisible()
{
	if ( !IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) )
		return;

	if ( m_sINVISIBLE.bPrevStateRun )
		SetActState(EM_ACT_RUN);
	else
		ReSetActState(EM_ACT_RUN);
}

void GLCharClient::AddSkillFact_SpecialAddon_Client(const EMSPEC_ADDON& emAddon, const SKILL::SSPEC& sSPEC, SSKILLFACT& sSKILLEF)
{
	SKILL::SSPEC_ADDON_LEVEL sAddon;

	sAddon.emSPEC			= emAddon;
	sAddon.sSPEC.fVAR1		= sSPEC.fVAR1;
	sAddon.sSPEC.fVAR2		= sSPEC.fVAR2;
	sAddon.sSPEC.fRate		= sSPEC.fRate;	// 효과 비율 적용. by luxes.
	sAddon.sSPEC.fRate2 	= sSPEC.fRate2;	// 효과 비율 적용. by luxes.
	sAddon.sSPEC.dwFLAG 	= sSPEC.dwFLAG;
	sAddon.sSPEC.dwNativeID = sSPEC.dwNativeID;

	sSKILLEF.vecSPEC.push_back( sAddon );
}

void GLCharClient::RECEIVE_SKILLFACT_Invisible(const float fVAR2)
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
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter->GetSelectID() == STARGETID(CROW_PC, m_dwGaeaID) )
	{
		pCharacter->ResetSelectID(); // 선택 대상 ID 널;
		pCharacter->ResetCursorID();
		pCharacter->ResetActionID();
		pCharacter->ClearReservationAction();
		pCharacter->TurnAction(GLAT_IDLE);
	}		
} //GLCharClient::RECEIVE_SKILLFACT_Invisible

const BOOL GLCharClient::RECEIVE_SKILLFACT_AIRBORNE(const SKILL::SSPEC& sSPEC, const SKILL::CDATA_LVL& sSKILL_DATA)
{
	if ( (this->m_bSkillFactsSpecial[EMSPECA_AIRBORNE] == true) && (sSPEC.dwFLAG == 0) )
		return FALSE;

	const float _fTimeLife(sSKILL_DATA.fLIFE);
	const float _fTimeLevitate(sSPEC.fVAR1);
	const float _fHeightMax(sSPEC.fVAR2);	
	this->m_sAirborne.push_back(_fTimeLife, _fTimeLevitate, _fHeightMax);

	return TRUE;
}

const bool GLCharClient::RECEIVE_SKILLFACT_Special(PGLSKILL pSkill, const WORD wlevel, const DWORD dwCount, const STARGETID& sID, SSKILLFACT& sSKILLEF )
{
	bool bHold = false;

	for ( DWORD i = 0; i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
	{
		const EMSPEC_ADDON&		emAddon = pSkill->m_sAPPLY.vecSPEC[i].emSPEC;
		const SKILL::SSPEC&		sSPEC = pSkill->m_sAPPLY.vecSPEC[i].sSPEC[wlevel];

		switch ( pSkill->m_sAPPLY.vecSPEC[i].emSPEC )
		{
		case EMSPECA_COUNTATTACK :
			{
				bHold = TRUE;
				sSKILLEF.pcCAData = pSkill->m_sAPPLY.sCAData.sSPEC_CA[wlevel];
				AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_INVISIBLE:
			bHold = true;
			RECEIVE_SKILLFACT_Invisible(sSPEC.fVAR2);			
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			break;
		case EMSPECA_AIRBORNE:			
			if ( RECEIVE_SKILLFACT_AIRBORNE(sSPEC, pSkill->m_sAPPLY.sDATA_LVL[wlevel]) == TRUE )
			{
				bHold = true;
				AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_ENTRANCE_EXIT :
			{
				RECEIVE_SKILLFACT_EntranceExit(pSkill->GetId(), sSPEC, wlevel, sID );
				bHold = true;
				AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_SCREEN_THROW :
			{
				sSKILLEF.sID = sID;
				bHold = true;
				AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_PROVOKE    :
			{
				sSKILLEF.sID = sID;
				bHold = true;
				AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			{
				bHold = true;
				AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			}
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
        case EMSPECA_TAUNT:
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
		case EMSPECA_ITEMDROP_RATE:
		case EMSPECA_MONEYDROP_RATE:
		case EMSPECA_EXP_RATE:
		case EMSPECA_NPC_TALK:
		case EMSPECA_ITEMDROP_SPECIFIC:
		case EMSPECA_VEHICLE_GETOFF: //봉주
		case EMSPECA_TURN_STATE_BATTLE:
		case EMSPECA_TURN_STATE_PEACE:		
		case EMSPECA_ENDURE_VARY: //봉주
		case EMSPECA_LINK_HP: //봉주
		case EMSPECA_RELEASE_ENDURE:
		case EMSPECA_HALLUCINATE:
		case EMSPECA_DOMINATION:
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
		case EMSPECA_DELAY_ACTION_SKILL:
		case EMSPECA_CLONE:
        case EMSPECA_MOVEVELO_LIMIT:
		case EMSPECA_FEIGN_DEATH :
		case EMSPECA_CAMERA_HIT :
		case EMSPECA_DISGUISE   :
			AddSkillFact_SpecialAddon_Client( emAddon, sSPEC, sSKILLEF );
			bHold = true;

			break;
		};
	} // for
	
	return bHold;
} // GLCharClient::RECEIVE_SKILLFACT_Special

const bool GLCharClient::RECEIVE_SKILLFACT_Addon( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF )
{
	const size_t nADDON = pSkill->m_sAPPLY.vecADDON.size();
	for ( DWORD i = 0; i < nADDON; ++i )
	{		
		SKILL::SIMPACT_ADDON_LEVEL sAddon;

		sAddon.emADDON = pSkill->m_sAPPLY.vecADDON[i].emADDON;
		sAddon.fADDON_VAR = pSkill->m_sAPPLY.vecADDON[i].fADDON_VAR[wlevel];
		sAddon.fRate = pSkill->m_sAPPLY.vecADDON[i].fRate[wlevel]; // 효과 비율 적용. by luxes.
		sSKILLEF.vecADDON.push_back( sAddon );
	}

	return nADDON ? true : false;
} // GLCharClient::RECEIVE_SKILLFACT_Addon

const bool GLCharClient::RECEIVE_SKILLFACT_Basic( PGLSKILL pSkill, const WORD wlevel, SSKILLFACT& sSKILLEF)
{
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
		sSKILLEF.emTYPE = pSkill->m_sAPPLY.emBASIC_TYPE;
		sSKILLEF.fMVAR = pSkill->m_sAPPLY.sDATA_LVL[wlevel].fBASIC_VAR;
		return true;
	};
	return false;
} // GLCharClient::RECEIVE_SKILLFACT_Basic

const BOOL GLCharClient::RECEIVE_SKILLCA( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg )
{	
	if( pMsg == NULL ) return FALSE;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pMsg->skill_id );
	if ( !pSkill )
		return FALSE;

	if ( pMsg->wSELSLOT >= SKILLFACT_SIZE )
		return FALSE;

	SSKILLFACT sSKILLEF;

	bool bHold = false;

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
	bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, pMsg->wLEVEL, _SKILLFACT_DEFAULTCUSTOMCOUNT, pMsg->sID, sSKILLEF );

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[pMsg->wLEVEL];

	if ( bHold )
	{
		sSKILLEF.sNATIVEID	= pMsg->skill_id;
		sSKILLEF.wLEVEL		= pMsg->wLEVEL;
		sSKILLEF.bEnable    = TRUE;

		if ( pMsg->sTIME.IsEXPIREDTIME )
		{
			sSKILLEF.SetAgeInfinite();
			sSKILLEF.nEXPIREDTIME = pMsg->sTIME.IsEXPIREDTIME;
		}
		else
		{
			if ( pMsg->sTIME.fAGE == _SKILLFACT_DEFAULTTIME )
				sSKILLEF.fAGE = sSKILL_DATA.fLIFE;
			else
				sSKILLEF.fAGE = pMsg->sTIME.fAGE;
		}

		if (pMsg->wSELSLOT >= SKILLFACT_SIZE)
			return FALSE;

		m_sSKILLFACT[pMsg->wSELSLOT] = sSKILLEF;

		if( pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
		{
			m_dwTransformSkill = pMsg->wSELSLOT;
			m_IdSkillTransform = pMsg->skill_id;
			m_sSKILLFACT[pMsg->wSELSLOT].SetSpecialSkill( SKILL::EMSSTYPE_TRANSFORM );
			m_sSKILLFACT[pMsg->wSELSLOT].bRanderSpecialEffect = FALSE;

			if( m_pSkinChar )
			{
				for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
				{
					if( m_wSex == 1 )
					{
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].empty() )
							continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
					else
					{
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].empty() )
							continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
				}
			}
		}

		return TRUE;
	}

	return FALSE;

}

const BOOL GLCharClient::RECEIVE_SKILLFACT( const GLMSG::SNETPC_SKILLHOLD_BRD* pMsg )
{
	//	스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pMsg->skill_id );
	if ( !pSkill )
		return FALSE;

	if ( pMsg->wSELSLOT >= SKILLFACT_SIZE )
		return FALSE;

	SSKILLFACT sSKILLEF;

	bool bHold = false;

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
	bHold |= RECEIVE_SKILLFACT_Special( &tempSkill, pMsg->wLEVEL, _SKILLFACT_DEFAULTCUSTOMCOUNT, pMsg->sID, sSKILLEF );

	//	Note : 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
	//

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[pMsg->wLEVEL];

	if ( bHold )
	{
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
			if ( pMsg->sTIME.fAGE == _SKILLFACT_DEFAULTTIME )
				sSKILLEF.fAGE = sSKILL_DATA.fLIFE;
			else
				sSKILLEF.fAGE = pMsg->sTIME.fAGE;
		}

		if (pMsg->wSELSLOT >= SKILLFACT_SIZE)
			return FALSE;

		m_sSKILLFACT[pMsg->wSELSLOT] = sSKILLEF;

		if( pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
		{
			m_dwTransformSkill = pMsg->wSELSLOT;
			m_IdSkillTransform = pMsg->skill_id;
			m_sSKILLFACT[pMsg->wSELSLOT].SetSpecialSkill( SKILL::EMSSTYPE_TRANSFORM );
			m_sSKILLFACT[pMsg->wSELSLOT].bRanderSpecialEffect = FALSE;

			if( m_pSkinChar )
			{
				for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
				{
					if( m_wSex == 1 )
					{
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].empty() )
							continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
					else
					{
						if( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].empty() )
							continue;
						m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
					}
				}
			}			
		}

		m_pGaeaClient->PartyMemberAddBuff( m_dwGaeaID, sSKILLEF.sNATIVEID.dwID, pMsg->wSELSLOT );

		return TRUE;
	}

	//GASSERT ( 0 && "지속 스킬정보가 유효하지 않음." );

	return FALSE;
}

const BOOL GLCharClient::RECEIVE_SKILLFACT ( const SNATIVEID& skill_id, const WORD wlevel, const DWORD dwSELECT, const STARGETID& sID, const SSKILLFACT::STIME sTime, const DWORD dwCount )
{
    //	스킬 정보 가져옴.
    PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( skill_id.wMainID, skill_id.wSubID );
    if ( !pSkill )
		return FALSE;

	if ( dwSELECT >= SKILLFACT_SIZE )
		return FALSE;

    SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[wlevel];
    
    SSKILLFACT sSKILLEF;

	bool bHold = false;

	bHold |= RECEIVE_SKILLFACT_Basic( pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Addon( pSkill, wlevel, sSKILLEF );
	bHold |= RECEIVE_SKILLFACT_Special( pSkill, wlevel, dwCount, sID, sSKILLEF );

    //	Note : 지속형 스킬일 경우 최적 슬롯을 찾아서 스킬 효과를 넣어줌.
    //
    if ( bHold )
    {
        sSKILLEF.sNATIVEID	   = skill_id;
        sSKILLEF.wLEVEL		   = wlevel;
        sSKILLEF.bEnable       = TRUE;

        if ( sTime.IsEXPIREDTIME )
        {
            sSKILLEF.SetAgeInfinite();
            sSKILLEF.nEXPIREDTIME = sTime.nEXPIREDTIME;
        }
        else
        {
            if ( sTime.fAGE == _SKILLFACT_DEFAULTTIME )
				sSKILLEF.fAGE = sSKILL_DATA.fLIFE;
            else
				sSKILLEF.fAGE = sTime.fAGE;
        }

        m_sSKILLFACT[dwSELECT] = sSKILLEF;

        if( pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
        {
            m_dwTransformSkill = dwSELECT;
            m_sSKILLFACT[dwSELECT].SetSpecialSkill( SKILL::EMSSTYPE_TRANSFORM );
            m_sSKILLFACT[dwSELECT].bRanderSpecialEffect = FALSE;

            if( m_pSkinChar )
            {
                for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
                {
                    if( m_CharData.m_Base.wSex == 1 )
					{
                        if( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].empty() ) 
							continue;

                        m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
                    }
					else
					{
                        if( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].empty() )
							continue;

                        m_pSkinChar->SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
                    }
                }
            }
        }

		m_pGaeaClient->PartyMemberAddBuff( m_dwGaeaID, sSKILLEF.sNATIVEID.dwID, dwSELECT );

        return TRUE;
    }

    //GASSERT ( 0 && "지속 스킬정보가 유효하지 않음." );

    return FALSE;
}

EMSLOT GLCharClient::GetCurRHand()
{
    if( IsUseArmSub() ) return SLOT_RHAND_S;
    else				return SLOT_RHAND;
}

EMSLOT GLCharClient::GetCurLHand()
{
    if( IsUseArmSub() ) return SLOT_LHAND_S;
    else				return SLOT_LHAND;
}

BOOL GLCharClient::VALID_SLOT_ITEM ( EMSLOT _slot )							
{ 
    if( m_CharData.m_PutOnItems[_slot].sNativeID==NATIVEID_NULL() )
		return FALSE;

    if( IsUseArmSub() )
    {
        if( _slot == SLOT_RHAND || _slot == SLOT_LHAND )
			return FALSE;
    }
    else
    {
        if( _slot == SLOT_RHAND_S || _slot == SLOT_LHAND_S )
			return FALSE;
    }

    return TRUE;
}

BOOL GLCharClient::IsCurUseArm( EMSLOT emSlot )
{
    if( IsUseArmSub() )
    {
        if( emSlot == SLOT_RHAND || emSlot == SLOT_LHAND )
			return FALSE;
    }
    else
    {
        if( emSlot == SLOT_RHAND_S || emSlot == SLOT_LHAND_S )
			return FALSE;
    }

    return TRUE;
}

D3DXVECTOR3 GLCharClient::GetPosBodyHeight ()
{
	if( NULL == m_pSkinChar )
		return GetPosition();

	const D3DXVECTOR3& vPosition = GetPosition();
    if ( m_emVehicle != EMVEHICLE_OFF && m_sVehicle.m_emTYPE == VEHICLE_TYPE_BOARD ) 
		return vPosition + D3DXVECTOR3(0.0f, m_pSkinChar->m_fHeight + m_sAirborne.getHeight() + 10.0f, 0.0f);        
    else 
        return vPosition + D3DXVECTOR3(0.0f, m_pSkinChar->m_fHeight + m_sAirborne.getHeight(), 0.0f);
}


void GLCharClient::DISABLEALLLANDEFF()
{
    if ( !m_vecLandEffect.empty() )
		m_vecLandEffect.clear();
}

void GLCharClient::SETENABLEALLLUNCHBOX( const BOOL bEnable )
{
    for ( int i = SKILLFACT_INDEX_LUNCHBOX_START; i < SKILLFACT_INDEX_EXP_LUNCHBOX_END; ++i )
    {
        m_sSKILLFACT[i].bEnable = bEnable;
    }
}

void GLCharClient::ADDLANDEFF( SLANDEFFECT_VALUE& landEffect )
{
    if ( EMLANDEFFECT_MULTI > m_vecLandEffect.size() ) 
        m_vecLandEffect.push_back( landEffect );
}

void GLCharClient::DELLANDEFF( SLANDEFFECT_VALUE& landEffect )
{
    VEC_LANDEFF_VALUE_ITER pos = std::find( m_vecLandEffect.begin(), m_vecLandEffect.end(), landEffect );
    if ( pos != m_vecLandEffect.end() )
		m_vecLandEffect.erase( pos );
}

void GLCharClient::DISABLEALLMAPEFF()
{
    if ( !m_vecMapEffect.empty() )
		m_vecMapEffect.clear();
}

void GLCharClient::ADDMAPEFF( SMAP_EFFECT& mapEffect )
{
    if ( EMLANDEFFECT_MULTI > m_vecMapEffect.size() ) 
        m_vecMapEffect.push_back( mapEffect );
}

void GLCharClient::DELMAPEFF( SMAP_EFFECT& mapEffect )
{
    VEC_MAP_EFFECT_ITER pos = std::find( m_vecMapEffect.begin(), m_vecMapEffect.end(), mapEffect );
    if ( pos != m_vecMapEffect.end() )
		m_vecMapEffect.erase( pos );
}


//  대련 종료시 처리 작업
void GLCharClient::EndConfting()
{
    DisableDebuffSkillFact();	
}

void GLCharClient::MsgJumpPosBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_JUMP_POS_BRD* pNetMsg = (GLMSG::SNETPC_JUMP_POS_BRD*) pMsg;        
	ClientActor::InitializeActorMove(pNetMsg->vPOS);
}

void GLCharClient::MsgAnimoveStartBrd(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_ANIMOVE_START_BRD *pNetMsg = (GLMSG::SNETPC_ANIMOVE_START_BRD *) pMsg;
	m_fAnimationMoveRotationY = pNetMsg->m_fAngleY;
}

void GLCharClient::MsgAttackBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ATTACK_BRD *pNetMsg = (GLMSG::SNETPC_ATTACK_BRD *) pMsg;

    //	Note : 공격 수행.
    m_sTargetID.emCrow = pNetMsg->emTarCrow;
    m_sTargetID.GaeaId = pNetMsg->dwTarID;
    m_dwANISUBSELECT = pNetMsg->dwAniSel;
    TurnAction ( GLAT_ATTACK );	
}

void GLCharClient::MsgAttackCancelBrd(NET_MSG_GENERIC* pMsg)
{
    if (isAction(GLAT_ATTACK))
        TurnAction(GLAT_IDLE);
}

void GLCharClient::MsgAttackBlockingBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ATTACK_BLOCKING_BRD *pNetMsg = (GLMSG::SNETPC_ATTACK_BLOCKING_BRD *) pMsg;
    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
    if ( pActor == NULL )
		return;

	STARGETID ActorID(m_emCrow, m_dwGaeaID, m_vPosition);
	pActor->ReceiveBlocking(ActorID);
	m_pGaeaClient->logDamageRecord(ActorID, 0, pNetMsg->SkillID, pNetMsg->wSkillLevel);
}

void GLCharClient::MsgAttackAvoidBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ATTACK_AVOID_BRD *pNetMsg = (GLMSG::SNETPC_ATTACK_AVOID_BRD *) pMsg;
    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
	if ( pActor == NULL )
		return;

	STARGETID ActorID(m_emCrow, m_dwGaeaID, m_vPosition);
	pActor->ReceiveAVoid( ActorID );
	m_pGaeaClient->logDamageRecord(ActorID, 0, pNetMsg->SkillID, pNetMsg->wSkillLevel);
}

void GLCharClient::MsgAttackDamageBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ATTACK_DAMAGE_BRD *pNetMsg = (GLMSG::SNETPC_ATTACK_DAMAGE_BRD *) pMsg;

    ClientActor* pActor = m_pGaeaClient->GetCopyActor ( pNetMsg->emTarCrow, pNetMsg->dwTarID );
    if ( pActor == NULL )
		return;

	STARGETID sACTOR(GetCrow(),GetGaeaID(),GetPosition());
	pActor->ReceiveDamage ( pNetMsg->nDamage, pNetMsg->dwDamageFlag, SCHOOL_NONE, sACTOR );
	m_pGaeaClient->logDamageRecord(sACTOR, pNetMsg->nDamage, NATIVEID_NULL(), 0, pNetMsg->dwDamageFlag);
}

void GLCharClient::MsgActionBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ACTION_BRD *pNetMsg = (GLMSG::SNET_ACTION_BRD *)pMsg;

	// 살아나는 경우 부활 이펙트;
	const EMACTIONTYPE eActionType = GetAction();
	if ( eActionType == GLAT_DIE && pNetMsg->emAction == GLAT_IDLE )
	{
		D3DXMATRIX matEffect;
		D3DXMatrixTranslation( &matEffect, GetPosition().x, GetPosition().y, GetPosition().z );

		STARGETID sTargetID( CROW_PC, GetGaeaID(), GetPosition() );
		DxEffGroupPlayer::GetInstance().NewEffGroup(
			GLCONST_CHAR::strREBIRTH_EFFECT.c_str(),
			matEffect,
			&sTargetID,
			FALSE, 
			FALSE );
	}

    TurnAction ( pNetMsg->emAction );
}

void GLCharClient::MsgPutonReleaseBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PUTON_RELEASE_BRD *pNetMsg = (GLMSG::SNETPC_PUTON_RELEASE_BRD *)pMsg;

    //	제거되는 아이탬 반영.
    m_CharData.m_PutOnItems[pNetMsg->emSlot] = SITEMCLIENT ( SNATIVEID(false) );
    UpdateSuit ();
    UPDATE_ITEM();

    ReSelectAnimation ();
}

void GLCharClient::MsgPutonUpdateBrd(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::SNETPC_PUTON_UPDATE_BRD Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return;

    // 제거되는 아이탬이 있을 경우.
    if (Data.m_SlotRelease < SLOT_NSIZE_S_2)
        m_CharData.m_PutOnItems[Data.m_SlotRelease] = SITEMCLIENT ( SNATIVEID(false) );

    //	변경되는 아이탬 반영.
    m_CharData.m_PutOnItems[Data.m_Slot] = Data.m_Item;
    UpdateSuit ();
    UPDATE_ITEM();

    ReSelectAnimation ();
}

void GLCharClient::MsgPutonChangeBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PUTON_CHANGE_BRD *pNetMsg = (GLMSG::SNETPC_PUTON_CHANGE_BRD *)pMsg;

    SetUseArmSub( pNetMsg->bUseArmSub );

    UpdateSuit ();
    UPDATE_ITEM();

    ReSelectAnimation ();
}

void GLCharClient::MsgLevelUpBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_LEVELUP_BRD* pNetMsg = (GLMSG::SNETPC_REQ_LEVELUP_BRD*)pMsg;

    m_CharData.m_Base.wLevel = pNetMsg->wLevel;

	const D3DXVECTOR3& vPosition = GetPosition();
    D3DXMATRIX		matEffect;
    D3DXMatrixTranslation( &matEffect, vPosition.x, vPosition.y, vPosition.z );

    STARGETID		sTargetID(CROW_PC, m_dwGaeaID, vPosition);
    DxEffGroupPlayer::GetInstance().NewEffGroup( GLCONST_CHAR::strLEVELUP_EFFECT.c_str(),
												 matEffect,
												 &sTargetID,
												 TRUE, 
												 FALSE );
}

void GLCharClient::MsgReqSkillBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_SKILL_BRD *pNetMsg = (GLMSG::SNETPC_REQ_SKILL_BRD *)pMsg;

    m_idACTIVESKILL = pNetMsg->skill_id;
    m_wACTIVESKILL_LEVEL = pNetMsg->wLEVEL;
    m_vTARPOS = pNetMsg->vTARPOS;

    m_wTARNUM = pNetMsg->wTARNUM;
    if ( m_wTARNUM > EMTARGET_NET )
    {
        return;
    }
    else
    {
        memcpy ( m_sTARIDS, pNetMsg->sTARIDS, sizeof(STARID)*m_wTARNUM );
				
		for( int i = 0 ; i < m_wTARNUM; ++i )
		{
			if( m_pGaeaClient ) {

				STARGETID sCTaId = STARGETID(m_sTARIDS[i].emCrow, m_sTARIDS[i].dwID);

				CSkillCAIdx* pTemp = m_pGaeaClient->GetTargetCA( sCTaId );

				if( pTemp != NULL && pTemp->m_nSkillLv >= 0 )
				{
					PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( pTemp->GetOriginSkillID() );

					if( pSkill )
					{
						m_sTARCA[i].AllReset();
						m_sTARCA[i].SetData( NATIVEID_NULL(), &pSkill->m_sAPPLY.sCAData.sSPEC_CA[pTemp->m_nSkillLv], 0, pTemp->GetOriginSkillID(), 0 );
					}					
				}
				else
				{
					m_sTARCA[i].AllReset();
				}

			}
		}

		//////////////////////////////////////////////////////////////////////////
		// GLAT_SKILL 에서 GLAT_SKILL_WAIT 으로 변경되었다.
		// 스킬 시작시 Effect 와 타격시 Effect 의 시점이 약간은 딜레이가 될 가능성이 보여서 작업되었다.
		//
        TurnAction ( GLAT_SKILL_WAIT );		// 스킬이 발동됨.
    }
}


void GLCharClient::MsgDealyActionSkillBrd( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION_BRD *NetMsgDealy = (GLMSG::SNETPC_REQ_SKILL_DEALY_ACTION_BRD *)pMsg;
	NetMsgDealy->skill_id;
	NetMsgDealy->wLEVEL;

	std::tr1::shared_ptr<GLCharClient> pCharacter = m_pGaeaClient->GetChar(NetMsgDealy->dwID);
	if (pCharacter)
	{
		SCHARSKILL sSkill( NetMsgDealy->skill_id, NetMsgDealy->wLEVEL);
		SKILLTARGETDATA sTargetData;

		for(int i = 0; i < NetMsgDealy->wTARNUM; i ++ )
		{
			sTargetData.TargetIDs[i] = NetMsgDealy->sTARIDS[i];
			sTargetData.nTarget++;
		}
		sTargetData.TargetID = NetMsgDealy->sTARIDS[0];

		pCharacter->StartDealyAction( sSkill, pCharacter, &sTargetData, false );

		EMSLOT				emRHand = GetCurRHand();
		SANIMSTRIKE			sStrike;
		sStrike.m_emPiece	= VALID_SLOT_ITEM( emRHand ) ? PIECE_RHAND : PIECE_GLOVE;
		sStrike.m_emEffect	= EMSF_TARGET;
		sStrike.m_dwFrame	= 0;

		pCharacter->SkillEffectDealyAction( sStrike, NetMsgDealy->skill_id, &sTargetData );
	}
}

void GLCharClient::MsgEndSkillBrd(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNETPC_END_SKILL_BRD* pNetMsg = (GLMSG::SNETPC_END_SKILL_BRD*)pMsg;
	UPDATE_DATA_END(pNetMsg->dwSkillfactIndex, FALSE, pNetMsg->bForceEnd);
}

void GLCharClient::MsgSkillCancelBrd(NET_MSG_GENERIC* pMsg)
{
    if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
        TurnAction(GLAT_IDLE);
}

void GLCharClient::MsgSkillFactBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SKILLFACT_BRD *pNetMsg = (GLMSG::SNETPC_SKILLFACT_BRD *)pMsg;

    m_CharData.m_Base.sHP.VARIATION ( pNetMsg->nVAR_HP );

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
			if ( pCharacter && (pNetMsg->emCrow == CROW_PC) && (pNetMsg->dwID == pCharacter->GetGaeaID()) )
			{
				bRenderEffect = TRUE;
			}
		}

		//	Note : 방어스킬의 이펙트가 있을때 발동시킴.
		STARGETID sACTOR(pNetMsg->sACTOR.GETCROW(),pNetMsg->sACTOR.GETID());
		sACTOR.vPos = m_pGaeaClient->GetTargetPos ( sACTOR );

		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay(
			vPos, -pNetMsg->nVAR_HP, pNetMsg->dwDamageFlag, UI_UNDERATTACK, sACTOR, m_sTargetID );

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

				//	Note : 자기 위치 이펙트 발생시킴.
				DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR, TRUE, FALSE );
			}

			if ( pNetMsg->dwDamageFlag & DAMAGE_TYPE_CA )
			{	
				// 강한타격 이펙트
				const D3DXVECTOR3& vPosition = GetPosition();
				STARGETID sMyID(CROW_PC,m_dwGaeaID, vPosition);

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

void GLCharClient::MsgSkillHoldBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SKILLHOLD_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_BRD *)pMsg;

    //	지속형 스킬의 경우 스킬 팩터 추가됨.
    if ( pNetMsg->skill_id != SNATIVEID(false) )
    {
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
					, pNetMsg->sTIME
					);
			}
			break;
		}
	}
        
	FACTEFF::NewSkillFactEffect(
		STARGETID(CROW_PC,m_dwGaeaID, GetPosition()),
		m_pSkinChar, pNetMsg->skill_id, GetTransMatrix(), GetDirect() );

	STARGETID sMyTID(CROW_PC, m_dwGaeaID, GetPositionActor());
	FACTEFF::NewSkillEffect_AppliPoint( m_pGaeaClient
		, pNetMsg->sID, sMyTID, pNetMsg->skill_id );
}

void GLCharClient::MsgSaveSkillFactBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SAVESKILLFACT_BRD *pNetMsg = (GLMSG::SNETPC_SAVESKILLFACT_BRD *)pMsg;
	const DWORD nSkillFactSize(pNetMsg->nSkillFactSize);
	for( DWORD i = 0; i < nSkillFactSize; ++i )
    {
        SCHARSAVESKILLFACT &sSAVESKILLFACT = pNetMsg->sSAVESKILLFACT[i];

        if( sSAVESKILLFACT.sNATIVEID == NATIVEID_NULL() )
            continue;
        RECEIVE_SKILLFACT( sSAVESKILLFACT.sNATIVEID, static_cast<WORD>(sSAVESKILLFACT.cLEVEL), sSAVESKILLFACT.wSLOT, sSAVESKILLFACT.sID,
            SSKILLFACT::STIME(sSAVESKILLFACT), sSAVESKILLFACT.dwCount );
    }
}

void GLCharClient::MsgSkillHOldRsBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_SKILLHOLD_RS_BRD *pNetMsg = (GLMSG::SNETPC_SKILLHOLD_RS_BRD *)pMsg;

	const D3DXVECTOR3& vPosition = GetPosition();
    // 스킬 fact 들을 종료.
    // 바로 리샛 하지 않고 여기서 시간 조종하여 정상 종료되게 함. ( 이팩트 종료 때문. )
    for ( int i=0; i<SKILLFACT_SIZE; ++i )
    {
        if ( pNetMsg->bRESET[i] && m_sSKILLFACT[i].sNATIVEID != NATIVEID_NULL() )
        {
			//BRD
			// 현재 구조 상 깔끔하게 지속형 스킬에 대한 종료 처리를 할 수 없음,
			// 해서 아래와 같은 식으로 처리하고는 있으나 반드시 바꿔야 함;
			// 기능(스킬) 별로 모듈화 하여 인터페이스로 진입, 진행, 종료, 갱신, 검사등을 구현하는 식으로;			

			GLCharClient::UPDATE_DATA_END(i);
			FACTEFF::DeleteSkillFactEffect( STARGETID(CROW_PC,m_dwGaeaID, GetPosition()), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );			
        }
    }
}

void GLCharClient::MsgTateBlowBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_STATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_STATEBLOW_BRD *)pMsg;

	const D3DXVECTOR3& vPosition = GetPosition();
    if( pNetMsg->emBLOW <= EMBLOW_SINGLE )
    {		
        FACTEFF::DeleteBlowSingleEffect( STARGETID(CROW_PC, m_dwGaeaID, vPosition), m_pSkinChar, m_sSTATEBLOWS );

        for( int i=0; i<EMBLOW_MULTI; ++i )
			m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
    }

    SSTATEBLOW *pSTATEBLOW = NULL;
    if ( pNetMsg->emBLOW <= EMBLOW_SINGLE )
		pSTATEBLOW = &m_sSTATEBLOWS[0];
    else
		pSTATEBLOW = &m_sSTATEBLOWS[pNetMsg->emBLOW-EMBLOW_SINGLE];

    pSTATEBLOW->emBLOW		= pNetMsg->emBLOW;
    pSTATEBLOW->fAGE				= pNetMsg->fAGE;
    pSTATEBLOW->fSTATE_VAR1 = pNetMsg->fSTATE_VAR1;
    pSTATEBLOW->fSTATE_VAR2 = pNetMsg->fSTATE_VAR2;
    pSTATEBLOW->wSchool		= pNetMsg->wSchool;

    FACTEFF::NewBlowEffect (
		STARGETID(CROW_PC, m_dwGaeaID, vPosition),
		m_pSkinChar, pSTATEBLOW->emBLOW, GetTransMatrix(), GetDirect() );	// 효과 생성.
}

void GLCharClient::MsgCureStateBlowBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_CURESTATEBLOW_BRD *pNetMsg = (GLMSG::SNETPC_CURESTATEBLOW_BRD *)pMsg;

	const D3DXVECTOR3& vPosition = GetPosition();
    for ( int i=0; i<EMBLOW_MULTI; ++i )
    {
        EMSTATE_BLOW emBLOW = m_sSTATEBLOWS[i].emBLOW;

        if ( emBLOW==EMBLOW_NONE )						continue;

        EMDISORDER emDIS = STATE_TO_DISORDER(emBLOW);
        if ( !(pNetMsg->dwCUREFLAG&emDIS) )				continue;

        DISABLEBLOW(i);
        FACTEFF::DeleteBlowEffect ( STARGETID(CROW_PC, m_dwGaeaID, vPosition), m_pSkinChar, emBLOW );
    }
}

void GLCharClient::MsgPushPullBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_PUSHPULL_BRD *pNetMsg = (GLMSG::SNET_PUSHPULL_BRD *)pMsg;
    const D3DXVECTOR3 &vMovePos = pNetMsg->vMovePos;

    //	Note : 밀려날 위치로 이동 시도.
    //
	const BOOL bSucceed = ClientActor::GotoLocation(vMovePos, 5.0f);

    if ( bSucceed )
    {
        //	Note : 밀려나는 엑션 시작.
        //
        m_sTargetID.vPos = vMovePos;
        m_dwANISUBSELECT = pNetMsg->dwAniType;

		TurnAction ( GLAT_PUSHPULL );

        //	Note : 밀리는 속도 설정.
        //
        ClientActor::SetMaxSpeed ( pNetMsg->fPUSH_SPEED );
    }
}

void GLCharClient::MsgLandEffectUpdateBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_LANDEFFECT_UPDATE_BRD* pNetMsg = ( GLMSG::SNETPC_LANDEFFECT_UPDATE_BRD* ) pMsg;
    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
    if ( !pLand )
        return;
    const SLEVEL_ETC_FUNC *pLevelEtcFunc = pLand->GetLevelEtcFunc();
    if( !pLevelEtcFunc )
        return;

    if ( pNetMsg->bADD ) 
    {
        ADDLANDEFF( pNetMsg->landEffect );
    }
    else
    {
        DELLANDEFF( pNetMsg->landEffect );
    }
}

void GLCharClient::MsgMapEffectUpdateBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_MAPEFFECT_UPDATE_BRD* pNetMsg = ( GLMSG::SNETPC_MAPEFFECT_UPDATE_BRD* ) pMsg;

    if ( pNetMsg->bADD ) 
    {
        ADDMAPEFF( pNetMsg->sMapEffect );
    }
    else
    {
        DELMAPEFF( pNetMsg->sMapEffect );
    }
}

void GLCharClient::MsgConfrontEndToCltBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_CONFRONT_END2_CLT_BRD *pNetMsg = (GLMSG::SNETPC_CONFRONT_END2_CLT_BRD *)pMsg;

    switch ( pNetMsg->emEND )
    {
    case EMCONFRONT_END_WIN:
        SetActState(EM_ACT_CONFT_WIN);
        TurnAction ( GLAT_CONFT_END );
        break;

    case EMCONFRONT_END_LOSS:
        ReSetActState(EM_ACT_CONFT_WIN);
        TurnAction ( GLAT_CONFT_END );
        break;

    case EMCONFRONT_END_NOTWIN:
        ReSetActState(EM_ACT_CONFT_WIN);
        TurnAction ( GLAT_CONFT_END );
        break;

    case EMCONFRONT_END_TIME:
        ReSetActState(EM_ACT_CONFT_WIN);
        TurnAction ( GLAT_CONFT_END );
        break;

    case EMCONFRONT_END_DISWIN:
        SetActState(EM_ACT_CONFT_WIN);
        TurnAction ( GLAT_CONFT_END );
        break;

    case EMCONFRONT_END_DISLOSS:
        ReSetActState(EM_ACT_CONFT_WIN);
        TurnAction ( GLAT_CONFT_END );
        break;

    case EMCONFRONT_END_PWIN:
        SetActState(EM_ACT_CONFT_WIN);
        TurnAction ( GLAT_CONFT_END );
        break;

    case EMCONFRONT_END_PLOSS:
        ReSetActState(EM_ACT_CONFT_WIN);
        TurnAction ( GLAT_CONFT_END );
        break;
    };

    //  대련 종료 처리
    EndConfting();
}

void GLCharClient::MsgUpdateBrightBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_BRIGHT_BRD *pNetMsg = (GLMSG::SNETPC_UPDATE_BRIGHT_BRD *)pMsg;
    m_CharData.m_Base.nBright = pNetMsg->nBright;
}

void GLCharClient::MsgUpdateStateBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_STATE_BRD *pNetMsg = (GLMSG::SNETPC_UPDATE_STATE_BRD *)pMsg;
    m_CharData.m_Base.sHP = pNetMsg->sHP;

    //CDebugSet::ToListView ( "NET_MSG_GCTRL_UPDATE_STATE_BRD %s,  %d/%d", m_CharData.szName, m_CharData.sHP.wNow, m_CharData.sHP.wMax );
}

void GLCharClient::MsgTitleNotifyChange(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_TITLE_NOTIFY_CHANGE* pNetMsg = ( GLMSG::SNET_TITLE_NOTIFY_CHANGE* ) pMsg;

    if ( !strcmp( pNetMsg->szNewTitle, "" ) )
    {
        m_CharData.m_Base.byActivityClass = ACTIVITY_CLASS_SIZE;
        StringCchCopy( m_CharData.m_Base.szTitle, EM_TITLEID_MAXLENGTH, "" );
    }
    else
    {
        m_CharData.m_Base.byActivityClass = pNetMsg->nClass;
        StringCchCopy( m_CharData.m_Base.szTitle, EM_TITLEID_MAXLENGTH, pNetMsg->szNewTitle );
    }
}

void GLCharClient::MsgActivityPopupComplate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_ACTIVITY_POPUP_COMPLETE* pNetMsg = ( GLMSG::SNET_ACTIVITY_POPUP_COMPLETE* ) pMsg;

	const D3DXVECTOR3& vPosition = GetPosition();
    D3DXMATRIX matEffect;
    D3DXMatrixTranslation ( &matEffect, vPosition.x, vPosition.y, vPosition.z );

    STARGETID sTargetID(CROW_PC, m_dwGaeaID, vPosition);
    DxEffGroupPlayer::GetInstance().NewEffGroup
        (
        GLCONST_CHAR::strCOMPLATE_ACTIVITY_EFFECT.c_str(),
        matEffect,
        &sTargetID,
		TRUE, 
		FALSE
        );

    // 완료 메세지 출력여부
    Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity( pNetMsg->dwActivityID );
    if ( pActivity )
    { 
        if ( pActivity->m_nNotifyLevel == ACTIVITY_NOTIFY_AROUND || pActivity->m_nNotifyLevel == ACTIVITY_NOTIFY_CLUBANDAROUND ) 
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT( "ACTIVITY_NOTIFY_COMPLATE" ), m_strName, pActivity->GetTitle() );
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::ITEMMONEY, strText.c_str() );
        }
    }
}

void GLCharClient::MsgUpdateCdmSafetimeBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_CDM_SAFETIME_BRD* pNetMsg = ( GLMSG::SNETPC_UPDATE_CDM_SAFETIME_BRD* ) pMsg;
    m_CharData.m_Base.m_fCDMSafeTime = pNetMsg->fCDMSafeTime;
}

void GLCharClient::MsgGatheringResultBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_GATHERING_RESULT_BRD* pNetMsg = ( GLMSG::SNETPC_REQ_GATHERING_RESULT_BRD* ) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMREQ_GATHER_RESULT_SUCCESS:
    case EMREQ_GATHER_RESULT_SUCCESS_EX:
        {
            m_dwANISUBGESTURE = 0;
        }
        break;
    case EMREQ_GATHER_RESULT_FAIL:
    case EMREQ_GATHER_RESULT_ITEMFAIL:
        {
            m_dwANISUBGESTURE = 1;
        }
        break;
    }
}

void GLCharClient::MsgReqGatheringCancelBrd(NET_MSG_GENERIC* pMsg)
{
    if (isAction(GLAT_GATHERING))
        TurnAction(GLAT_IDLE);
}

void GLCharClient::MsgReqGatheringBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_GATHERING_BRD *pNetMsg = (GLMSG::SNETPC_REQ_GATHERING_BRD *) pMsg;

    //	Note : 공격 수행.
    m_dwANISUBGESTURE = pNetMsg->dwAniSel;
    TurnAction ( GLAT_GATHERING );
}

void GLCharClient::MsgItemShopOpenBrd(NET_MSG_GENERIC* pMsg)
{	
//#if defined ( JP_PARAM ) || defined(_RELEASED)	// JAPAN Item Shop
	EMSERVICE_PROVIDER sp = m_pGaeaClient->GetServiceProvider();
	if (sp == SP_JAPAN || sp == SP_OFFICE_TEST)
	{
		GLMSG::SNETPC_OPEN_ITEMSHOP_BRD* pNetMsg = ( GLMSG::SNETPC_OPEN_ITEMSHOP_BRD* ) pMsg;
		m_bItemShopOpen = pNetMsg->bOpen;

		//!! SF_TODO
		//if ( m_bItemShopOpen )	m_pInterface->ItemShopAddShopIcon( m_dwGaeaID );
		//else	m_pInterface->ItemShopDelShopIcon( m_dwGaeaID );
	}
//#endif
}

void GLCharClient::MsgVehicleBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_VEHICLE_BRD *pNetMsg = (GLMSG::SNETPC_VEHICLE_BRD *) pMsg;

    m_emVehicle = pNetMsg->emVehicle;
    m_sVehicle.m_DbNum = pNetMsg->sVehicle.m_DbNum;
    m_sVehicle.m_emTYPE = pNetMsg->sVehicle.m_emTYPE;
    m_sVehicle.m_sVehicleID = pNetMsg->sVehicle.m_sVehicleID;

    for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
    {
        m_sVehicle.m_PutOnItems[i].Assign( pNetMsg->sVehicle.m_PutOnItems[i] );
    }

    for ( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
    {
        m_sVehicle.m_sColor[ i ].dwMainColor = pNetMsg->sVehicle.m_sColor[ i ].dwMainColor;
        m_sVehicle.m_sColor[ i ].dwSubColor = pNetMsg->sVehicle.m_sColor[ i ].dwSubColor;
    }
    m_sVehicle.ITEM_UPDATE();		

    UpdateSuit ();
}

void GLCharClient::MsgVehicleRemoveSlotItemBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_BRD* pNetMsg = ( GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM_BRD* ) pMsg;

    // 슬롯아이템 제거
    m_sVehicle.ReSetSlotItem ( pNetMsg->emSuit );

    UpdateSuit();
    UPDATE_ITEM();
}

void GLCharClient::MsgVehicleReqSlotExHoldBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD* pNetMsg = (GLMSG::SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD*) pMsg;

    // 손에든 아이템을
    m_sVehicle.SetSlotItem ( pNetMsg->emSuit, pNetMsg->sItemCustom );

    UpdateSuit();
    UPDATE_ITEM();
}

void GLCharClient::MsgVehicleAccessoryDeleteBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_VEHICLE_ACCESSORY_DELETE_BRD* pNetMsg = (GLMSG::SNET_VEHICLE_ACCESSORY_DELETE_BRD*) pMsg;

    if ( m_sVehicle.m_DbNum == pNetMsg->m_VehicleDbNum )
    {
        m_sVehicle.m_PutOnItems[pNetMsg->accetype] = SITEMCUSTOM ();			
        m_sVehicle.ITEM_UPDATE();
        UpdateSuit();
        UPDATE_ITEM();
    }
}

void GLCharClient::MsgUngetVehicleBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UNGET_VEHICLE_BRD* pNetMsg = (GLMSG::SNETPC_UNGET_VEHICLE_BRD*) pMsg;			
    m_sVehicle.SetActiveValue( false );	
    m_sVehicle.RESET();
}

void GLCharClient::MsgGetVehicleBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_GET_VEHICLE_BRD* pPacket = (GLMSG::SNETPC_GET_VEHICLE_BRD*) pMsg;

	switch( pPacket->eInfoState )
	{
	case EMVEHICLE_BROAD_ITEM_INFO :
		{
			BYTE iRef = pPacket->iIndex;
			if( iRef < ACCE_TYPE_SIZE )
			{
				m_sVehicle.m_PutOnItems[iRef] = pPacket->PutOnItems;
				m_sVehicle.m_sColor[iRef]     = pPacket->sVehicleColor;
			}
		}
		break;
	case EMVEHICLE_BROAD_OK :
		{
			m_sVehicle.m_DbNum = pPacket->basisInfo.m_VehicleDbNum;
			m_sVehicle.m_emTYPE = pPacket->basisInfo.emTYPE;
			m_sVehicle.m_sVehicleID = pPacket->basisInfo.sVehicleID;

			m_sVehicle.SetActiveValue(true);
			m_sVehicle.ITEM_UPDATE();

			UpdateSuit();				
			UPDATE_ITEM();
		}
		break;
	}
/*
    m_sVehicle.m_DbNum = pPacket->m_VehicleDbNum;
    m_sVehicle.m_emTYPE = pPacket->emTYPE;
    m_sVehicle.m_sVehicleID = pPacket->sVehicleID;

    for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
    {
        m_sVehicle.m_PutOnItems[i] = pPacket->PutOnItems[i];
    }

    for ( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
    {
        m_sVehicle.m_sColor[ i ] = pPacket->sVehicleColor[ i ];
    }

    m_sVehicle.SetActiveValue(true);
    m_sVehicle.ITEM_UPDATE();

    UpdateSuit();				
    UPDATE_ITEM();
*/
}

void GLCharClient::MsgActiveWithVehicleBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ACTIVE_WITH_VEHICLE_BRD* pNetMsg = (GLMSG::SNETPC_ACTIVE_WITH_VEHICLE_BRD*) pMsg;

    if ( pNetMsg->bActive )
    {
        m_emVehicle = pNetMsg->emVehicle;
        int emType = m_sVehicle.m_emTYPE ;
        m_emANISUBTYPE = (EMANI_SUBTYPE) (AN_SUB_HOVERBOARD + emType) ;
        SetActState(EM_ACT_PEACEMODE);

        for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
        {
            m_sVehicle.PassengerIdSet(i, pNetMsg->dwPassengerID[i]);
        }

        GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        std::tr1::shared_ptr<GLCharClient> pChar;
        for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
        {
            if (m_sVehicle.PassengerId(i) == pCharacter->GetGaeaID())
            {
                for (INT j=0; j<MAX_PASSENGER_COUNT; ++j)
                {
                    pCharacter->VehiclePassengerSet(j, m_sVehicle.PassengerId(j));
                }
            }
            else
            {
                pChar = pLand->GetChar ( m_sVehicle.PassengerId(i) );
                if ( !pChar )	
                    continue;

                for( INT j = 0; j < MAX_PASSENGER_COUNT; j++ )
                {
                    pChar->m_sVehicle.PassengerIdSet(j, m_sVehicle.PassengerId(j));
                }
            }
        }		

        UpdateSuit();				
        ReSelectAnimation();				
        UPDATE_ITEM();
    }
    else
    {
        if ( !m_emVehicle )	return;
        EMSLOT emRHand = GetCurRHand();
        EMSLOT emLHand = GetCurLHand();

        SITEM* pRHAND = NULL;
        SITEM* pLHAND = NULL;

        if ( m_CharData.m_PutOnItems[emRHand].sNativeID!=NATIVEID_NULL() )
            pRHAND = GLogicData::GetInstance().GetItem ( m_CharData.m_PutOnItems[emRHand].sNativeID );

        if ( m_CharData.m_PutOnItems[emLHand].sNativeID!=NATIVEID_NULL() )
            pLHAND = GLogicData::GetInstance().GetItem ( m_CharData.m_PutOnItems[emLHand].sNativeID );

        m_emANISUBTYPE = CHECK_ANISUB ( pRHAND, pLHAND, CharClassToIndex( GetClass() ) );


        GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
        GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
        std::tr1::shared_ptr<GLCharClient> pChar;
        for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
        {
            if (m_sVehicle.PassengerId(i) == pCharacter->GetGaeaID())
            {
                for (INT j=0; j<MAX_PASSENGER_COUNT; ++j)
                {
                    if (pCharacter->VehiclePassenger(j) == m_dwGaeaID)
                    {
                        pCharacter->VehiclePassengerSet(j, GAEAID_NULL);
                    }
                }
            }
            else 
            {
                pChar = pLand->GetChar(m_sVehicle.PassengerId(i));
                if (pChar)
                {
                    for (INT j=0; j<MAX_PASSENGER_COUNT; ++j)
                    {
                        if (pChar->VehiclePassengerId(j) == m_dwGaeaID)
                        {
                            pChar->VehiclePassengerIdSet(j, GAEAID_NULL);
                        }
                    }
                }
            }

        }


        for (INT i=0; i<MAX_PASSENGER_COUNT; ++i)
        {
            m_sVehicle.PassengerIdSet(i, GAEAID_NULL);
        }

        m_emVehicle = EMVEHICLE_OFF;		

        UpdateSuit();				
        ReSelectAnimation();				
        UPDATE_ITEM();

    }
}

void GLCharClient::MsgActiveVehicleBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_ACTIVE_VEHICLE_BRD* pNetMsg = (GLMSG::SNETPC_ACTIVE_VEHICLE_BRD*) pMsg;

    if ( pNetMsg->bActive )
    {
        if ( m_emVehicle )
            return;
        m_emVehicle = EMVEHICLE_DRIVER;
        int emType = m_sVehicle.m_emTYPE ;
        m_emANISUBTYPE = (EMANI_SUBTYPE) (AN_SUB_HOVERBOARD + emType) ;
        SetActState(EM_ACT_PEACEMODE);

        for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
        {
            m_sVehicle.PassengerIdSet(i, pNetMsg->dwPassengerID[i]);
        }

        m_sVehicle.BoostReset();

        // 탑승 이팩트 추가
        STARGETID sTargetID(CROW_PC, m_dwGaeaID, GetPosition());
        DxEffGroupPlayer::GetInstance().NewEffGroup
            (
            GLCONST_CHAR::strVEHICLE_GEN_EFFECT.c_str(),
            GetTransMatrix(),
            &sTargetID,
			TRUE, 
			FALSE
            );			

        UpdateSuit( );				
        ReSelectAnimation();				
        UPDATE_ITEM ();

    }
    else
    {
        if ( !m_emVehicle )
            return;
        m_emVehicle = EMVEHICLE_OFF;		
        EMSLOT emRHand = GetCurRHand();
        EMSLOT emLHand = GetCurLHand();

        SITEM* pRHAND = NULL;
        SITEM* pLHAND = NULL;

        if ( m_CharData.m_PutOnItems[emRHand].sNativeID!=NATIVEID_NULL() )
            pRHAND = GLogicData::GetInstance().GetItem ( m_CharData.m_PutOnItems[emRHand].sNativeID );

        if ( m_CharData.m_PutOnItems[emLHand].sNativeID!=NATIVEID_NULL() )
            pLHAND = GLogicData::GetInstance().GetItem ( m_CharData.m_PutOnItems[emLHand].sNativeID );

        m_emANISUBTYPE = CHECK_ANISUB ( pRHAND, pLHAND, CharClassToIndex( GetClass() ) );

        for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
        {
            m_sVehicle.PassengerIdSet(i, GAEAID_NULL);
        }

        m_sVehicle.BoostReset();		

        UpdateSuit();				
        ReSelectAnimation();				
        UPDATE_ITEM();
    }
}

void GLCharClient::MsgEventFactEndBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_EVENTFACT_END_BRD *pNetMsg = (GLMSG::SNETPC_EVENTFACT_END_BRD *)pMsg;
    m_CharData.m_Base.sEVENTFACT.ResetEVENT( pNetMsg->emType );
}

void GLCharClient::MsgEventFactBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_EVENTFACT_BRD *pNetMsg = (GLMSG::SNETPC_EVENTFACT_BRD *)pMsg;

    m_CharData.m_Base.sEVENTFACT.SetEVENT( pNetMsg->emType, pNetMsg->wValue );
}

void GLCharClient::MsgQuestionItemFactEndBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_QITEMFACT_END_BRD *pNetMsg = (GLMSG::SNETPC_QITEMFACT_END_BRD *)pMsg;
    m_CharData.m_Base.sQITEMFACT.RESET();
}

void GLCharClient::MsgIPEventBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_IPEVENT_BRD* pNetMsg = (GLMSG::SNETPC_IPEVENT_BRD*)pMsg;

    ran_event::GLIPEvent::Instance()->SetValues( pNetMsg->m_AttackValue, pNetMsg->m_AttackSpeed, pNetMsg->m_MoveSpeed, pNetMsg->m_ItemDrop
        , pNetMsg->m_MoneyDrop, pNetMsg->m_Exp, pNetMsg->m_HPRecovery, pNetMsg->m_MPRecovery, pNetMsg->m_SPRecovery, pNetMsg->m_AllRecovery, pNetMsg->m_CriticalRate );

    m_CharData.m_Base.sEVENTFACT.bIPEvent = pNetMsg->bEnable;
}

void GLCharClient::MsgQuestionItemFactBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_QITEMFACT_BRD *pNetMsg = (GLMSG::SNETPC_QITEMFACT_BRD *)pMsg;

    m_CharData.m_Base.sQITEMFACT.emType = pNetMsg->sFACT.emType;
    m_CharData.m_Base.sQITEMFACT.fTime = pNetMsg->sFACT.fTime;
    m_CharData.m_Base.sQITEMFACT.wParam1 = pNetMsg->sFACT.wParam1;
    m_CharData.m_Base.sQITEMFACT.wParam2 = pNetMsg->sFACT.wParam2;
}

void GLCharClient::MsgReqGestureBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_GESTURE_BRD *pNetMsg = (GLMSG::SNETPC_REQ_GESTURE_BRD *)pMsg;

    //	Note : 모션 종류가 평화 모드일 경우만 제스쳐를 할 수 있다.
    GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
    BOOL bPeaceZone = pLand ? pLand->IsPeaceZone() : FALSE;
    if ( !bPeaceZone && IsActState(EM_ACT_PEACEMODE) )		bPeaceZone = TRUE;

    if ( !bPeaceZone )		return;

    //	Note : 해당 제스쳐 에니메이션이 존제시 구동.
    const PANIMCONTNODE pNODE = m_pSkinChar->GETANI ( AN_GESTURE, EMANI_SUBTYPE(pNetMsg->dwID) );
    if ( !pNODE )			return;

    //	Note : 신체가 정상적일때 구동.
    if ( !IsValidBody() )	return;

    //	Note : 제스쳐를 행함.
    m_dwANISUBGESTURE = (DWORD) pNetMsg->dwID;
    TurnAction(GLAT_TALK);
}

void GLCharClient::MsgInvenRenameBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_RENAME_BRD *pNetMsg = (GLMSG::SNETPC_INVEN_RENAME_BRD *)pMsg;

    StringCchCopy(m_CharData.m_Base.szName, CHAR_SZNAME, pNetMsg->szName);
}

void GLCharClient::MsgInvenFaceChangeBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_FACE_CHANGE_BRD *pNetMsg = (GLMSG::SNETPC_INVEN_FACE_CHANGE_BRD *)pMsg;
    m_CharData.m_Base.wFace = (WORD) pNetMsg->dwID;
    UpdateSuit();
}

void GLCharClient::MsgUseBikeBoostCancelBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_USE_BIKE_BOOST_CANCEL_BRD *pNetMsg = (GLMSG::SNETPC_REQ_USE_BIKE_BOOST_CANCEL_BRD*)pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMINVEN_USE_BIKE_BOOST_CANCEL:
        {
            m_sVehicle.BoostReset();
        }
        break;

    case EMINVEN_USE_BIKE_BOOST_DELAY_CANCEL:
        {
            m_sVehicle.m_fBoostDelay = 0.0f;
            m_sVehicle.m_bUseBoostDelay = FALSE;
        }
        break;
    }
}

void GLCharClient::MsgUseBikeBoostBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_USE_BIKE_BOOST_BRD *pNetMsg = (GLMSG::SNETPC_REQ_USE_BIKE_BOOST_BRD*)pMsg;

    m_sVehicle.m_bOnceBoost = TRUE;
    m_sVehicle.m_bUseBoost = TRUE;
    m_sVehicle.m_fBoostTime = 0.0f;
}

void GLCharClient::MsgLearnBikeBoostBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_REQ_BIKE_BOOST_BRD *pNetMsg = (GLMSG::SNETPC_REQ_BIKE_BOOST_BRD*)pMsg;

    m_sVehicle.m_dwBoosterID = pNetMsg->dwBoosterID;
}

void GLCharClient::MsgVehicleColorChangeBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE_BRD *pNetMsg = (GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE_BRD*)pMsg;

    for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
    {
        m_sVehicle.m_sColor[ i ].dwMainColor = pNetMsg->dwMainColor[ i ];
        m_sVehicle.m_sColor[ i ].dwSubColor = pNetMsg->dwSubColor[ i ];
    }
    UpdateSuit();
}

void GLCharClient::MsgCostumeColorChangeBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE_BRD *pNetMsg = (GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE_BRD*)pMsg;

    EMSLOT	emSlot = pNetMsg->emSlot;

    if ( m_CharData.m_PutOnItems[ emSlot ].sNativeID != NATIVEID_NULL() )
    {
        m_CharData.m_PutOnItems[ emSlot ].dwMainColor = pNetMsg->dwMainColor;
        m_CharData.m_PutOnItems[ emSlot ].dwSubColor = pNetMsg->dwSubColor;
    }

    UpdateSuit();
}

void GLCharClient::MsgInvenHairColorChangeBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_BRD *pNetMsg = (GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE_BRD*)pMsg;

    m_CharData.m_Base.wHairColor = pNetMsg->wHairColor;
    UpdateSuit();
}

void GLCharClient::MsgInvenHairChangeBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_INVEN_HAIR_CHANGE_BRD *pNetMsg = (GLMSG::SNETPC_INVEN_HAIR_CHANGE_BRD *)pMsg;
    m_CharData.m_Base.wHair = (WORD) pNetMsg->dwID;
    UpdateSuit();
}

void GLCharClient::MsgClubAuthorityBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_AUTHORITY_BRD *pNetMsg = (GLMSG::SNET_CLUB_AUTHORITY_BRD *)pMsg;
    m_CharData.m_Base.dwGuildMaster = pNetMsg->dwCharID;
}

void GLCharClient::MsgClubDelBrd(NET_MSG_GENERIC* pMsg)
{
    m_CharData.m_Base.dwGuild = CLUB_NULL;
    m_CharData.m_Base.dwGuildMarkVer = 0;
    StringCchCopy ( m_CharData.m_Base.szNick, CHAR_SZNAME, "" );
}

void GLCharClient::MsgClubInfoNickBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_INFO_NICK_BRD *pNetMsg = (GLMSG::SNET_CLUB_INFO_NICK_BRD *)pMsg;
    StringCchCopy ( m_CharData.m_Base.szNick, CHAR_SZNAME, pNetMsg->szNickName );
}

void GLCharClient::MsgClubInfoMarkBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_INFO_MARK_BRD *pNetMsg = (GLMSG::SNET_CLUB_INFO_MARK_BRD *)pMsg;

    m_CharData.m_Base.dwGuildMarkVer = pNetMsg->dwMarkVer;
    //	Note : 클럽마크 버전 확인.
    //
    m_pGaeaClient->ReqClubMarkInfo(m_CharData.m_Base.dwGuild, m_CharData.m_Base.dwGuildMarkVer);
}

void GLCharClient::MsgClubInfoBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CLUB_INFO_BRD *pNetMsg = (GLMSG::SNET_CLUB_INFO_BRD *)pMsg;

    m_CharData.m_Base.dwGuild = pNetMsg->dwClubID;
    m_CharData.m_Base.dwGuildMarkVer = pNetMsg->dwMarkVer;

    StringCchCopy ( m_CharData.m_Base.szNick, CHAR_SZNAME, pNetMsg->szNickName );

    //	Note : 클럽마크 버전 확인.
    //
    m_pGaeaClient->ReqClubMarkInfo(m_CharData.m_Base.dwGuild, m_CharData.m_Base.dwGuildMarkVer);
}

void GLCharClient::MsgPrivateMarketItemInfoBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_ITEM_INFO_BRD *pNetNsg = (GLMSG::SNETPC_PMARKET_ITEM_INFO_BRD *)pMsg;

    SINVENITEM sINVENITEM;
    sINVENITEM.sItemCustom = pNetNsg->sITEMCUSTOM;
    sINVENITEM.wPosX = pNetNsg->sINVENPOS.wMainID;
    sINVENITEM.wPosY = pNetNsg->sINVENPOS.wSubID;
    m_sPMarket.RegItem ( sINVENITEM, pNetNsg->llPRICE, pNetNsg->dwNUMBER, pNetNsg->sSALEPOS );
    m_sPMarket.SetSaleState ( pNetNsg->sSALEPOS, pNetNsg->dwNUMBER, pNetNsg->bSOLD );

	m_pGaeaClient->GetPrivateMarketClient()->UpdatePrivateMarketItemList( m_sPMarket, CharDbNum(), false );
}

void GLCharClient::MsgPrivateMarketItemUpdateBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_ITEM_UPDATE_BRD *pNetMsg = (GLMSG::SNETPC_PMARKET_ITEM_UPDATE_BRD *) pMsg;

    SSALEITEM *pSALEITEM = m_sPMarket.GetItem ( pNetMsg->sSALEPOS );
    if ( pSALEITEM )
    {
        pSALEITEM->bSOLD = pNetMsg->bSOLD;
        pSALEITEM->dwNUMBER = pNetMsg->dwNUMBER;
        pSALEITEM->sITEMCUSTOM.wTurnNum = (WORD) pNetMsg->dwNUMBER;

        SINVENITEM *pINVENITEM = m_sPMarket.GetInven().GetItem(pNetMsg->sSALEPOS.wMainID,pNetMsg->sSALEPOS.wSubID);
        if ( pINVENITEM )
        {
            pINVENITEM->sItemCustom.wTurnNum = (WORD) pNetMsg->dwNUMBER;
        }
    }

	m_pGaeaClient->GetPrivateMarketClient()->UpdatePrivateMarketItemList( m_sPMarket, CharDbNum(), false );
}

void GLCharClient::MsgPrivateMarketCloseBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_CLOSE_BRD *pNetNsg = (GLMSG::SNETPC_PMARKET_CLOSE_BRD *)pMsg;

    m_sPMarket.DoMarketClose();

	// 개인상점 디스플레이 삭제
	GfxInterfaceBase::GetInstance()->DeletePrivateMarketDisplay( m_dwGaeaID );

	// 현재 보고 있는 개인상점이 닫힌거면 UI도 끄기
	if( m_dwGaeaID == m_pGaeaClient->GetPrivateMarketClient()->GetOpenerID() )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_ReqPMClose );
	}
}

void GLCharClient::MsgPrivateMarketOpenBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PMARKET_OPEN_BRD *pNetNsg = (GLMSG::SNETPC_PMARKET_OPEN_BRD *)pMsg;

    m_sPMarket.SetTITLE ( pNetNsg->m_PrivateMarketTitle );
    m_sPMarket.DoMarketOpen();
}

void GLCharClient::MsgUpdatePassiveBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_PASSIVE_BRD *pNetMsg = (GLMSG::SNETPC_UPDATE_PASSIVE_BRD *)pMsg;
    m_CharData.m_Base.sPASSIVE_SKILL = pNetMsg->sSKILL_DATA;
}

void GLCharClient::MsgPartyBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_PARTY_BRD *pNetMsg = (GLMSG::SNET_PARTY_BRD *)pMsg;
    m_CharData.m_Base.PartyID = pNetMsg->PartyID;
    m_CharData.m_Base.dwPMasterID = pNetMsg->dwPMasterID;
}

void GLCharClient::MsgUpdateFlags(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_FLAGS_BRD *pNetMsg = (GLMSG::SNETPC_UPDATE_FLAGS_BRD *)pMsg;
    m_CharData.m_Base.dwFLAGS = pNetMsg->dwFLAGS;
}

// 스킬 효과 적용;
void GLCharClient::MsgApplySkillEffect ( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNET_APPLY_SKILLEFFECT_FC* pNetMsg =
		static_cast< GLMSG::SNET_APPLY_SKILLEFFECT_FC* >( pMsg );
	if ( !pNetMsg )
		return;

	//m_bSkillFactsSpecial[ pNetMsg->emSpecAddon ] = pNetMsg->bState;
}

void GLCharClient::CalculateMatrix( const D3DXVECTOR3& vPosition )
{
	std::tr1::shared_ptr<GLCharClient> pCharClient;
	D3DXMATRIX matTrans, matYRot, matPassenger;
	D3DXVECTOR3 vPos = GetPosition();
	const DxVehicle* pSkinChar = NULL;

	if( m_emVehicle )
	{
		for( WORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			if( m_sVehicle.PassengerId(i) == m_dwGaeaID )
			{
				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
				if( i == 0 )
				{
					pSkinChar = m_sVehicle.m_pSkinChar;
					if ( pSkinChar )
					{
						for( WORD z = 1; z < MAX_PASSENGER_COUNT; z++ )
						{
							if( pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(z) )
							{
								// 위치를 업데이트 해서 떨림 현상을 없애도록 한다.
								pCharacter->SetPosition( vPos );
							}
						}
					}
				}
				else
				{
					pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(0) );
					if( pCharClient )
					{
						vPos = pCharClient->GetPosition();
						SetPosition( vPos );
						SetPositionActor(vPos);
					}
					else if( pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(0) )
					{
						vPos = pCharacter->GetPosition();
						SetPosition( vPos );
						SetPositionActor(vPos);
					}
				}
				break;
			}
		}

		const D3DXVECTOR3& vDirect = GetDirect();
		const D3DXVECTOR3& vDirectOrig = GetDirectOrig();
		if( pSkinChar )
		{
			D3DXVECTOR3 vFrontHighPos, vFrontLowPos, vBackHighPos, vBackLowPos;
			D3DXVECTOR3 vAnswerFrontPos, vAnswerBackPos;
			DWORD dwCollID;
			BOOL bFrontColl, bBackColl;
			D3DXVECTOR3 vAngle;

			vFrontHighPos = vFrontLowPos = vPosition + ( vDirect * pSkinChar->m_vMax.x );
			vFrontHighPos.y += 10.0f;
			vFrontLowPos.y -= 10.0f;

			vBackHighPos = vBackLowPos = vPosition + ( - vDirect * pSkinChar->m_vMax.x );
			vBackHighPos.y += 10.0f;
			vBackLowPos.y -= 10.0f;

			ClientActor::IsCollision(vFrontHighPos, vFrontLowPos, vAnswerFrontPos, dwCollID, bFrontColl);
			ClientActor::IsCollision(vBackHighPos, vBackLowPos, vAnswerBackPos, dwCollID, bBackColl);

			if( bFrontColl && bBackColl )
			{
				vAngle = vAnswerFrontPos - vAnswerBackPos;
				D3DXVec3Normalize( &vAngle, &vAngle );

				float fTheta = (vAngle.y * D3DX_PI / 3.0f );
				D3DXMatrixRotationX( &m_matBikeTrans, fTheta );

				D3DXMATRIX matRot;
				fTheta = DXGetThetaYFromDirection( vDirect, vDirectOrig );
				D3DXMatrixRotationY( &matRot, fTheta );
				D3DXMatrixMultiply( &m_matBikeTrans, &m_matBikeTrans, &matRot );

				m_matBikeTrans._41 = vPosition.x;
				m_matBikeTrans._42 = vPosition.y;
				m_matBikeTrans._43 = vPosition.z;
			}
			else
			{
				float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
				D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
				fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
				D3DXMatrixRotationY ( &matYRot, fThetaY );

				m_matBikeTrans = matYRot * matTrans;
			}
		}
		else
		{
			D3DXMATRIX matTrans, matYRot;
			D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
			float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
			D3DXMatrixRotationY ( &matYRot, fThetaY );

			SetTransMatrix(D3DXMATRIX(matYRot * matTrans));
		}
	}
	else
	{
		const D3DXVECTOR3& vDirect = GetDirect();
		const D3DXVECTOR3& vDirectOrig = GetDirectOrig();
		D3DXMATRIX matTrans, matYRot;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));
	}

	//std::tr1::shared_ptr<GLCharClient> pCharClient;
	//const SPassengerData* pPassengerData = NULL;
	//D3DXMATRIX matTrans, matYRot;
	//const D3DXVECTOR3& vPosition = GetPosition();
	//SITEM* pItem = GET_SLOT_ITEMDATA ( SLOT_VEHICLE );

	//if( pItem )
	//{
	//	if( m_emVehicle == EMVEHICLE_DRIVER && pItem->VehicleType() != VEHICLE_TYPE_BOARD )
	//	{
	//		// Vehicle의 운전자가 현재 자신인지 확인한다.
	//		if ( m_dwGaeaID != m_sVehicle.PassengerId(0) )
	//			return;

	//		// Skin 이 만들어졌는지 확인한다.
	//		DxVehicle* pVehicle = m_sVehicle.m_pSkinChar;
	//		if ( !pVehicle )
	//			return;

	//		//pVehicle->UpdatePassengerPos();

	//		// PassengerMatrix Data 가 정상인지 아닌지 확인한다.
	//		if ( !pVehicle->IsValidPassengerMatrixData() )
	//			return;

	//		for( DWORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
	//		{
	//			// PassengerData 가 정상인지 확인한다.
	//			pPassengerData = pVehicle->GetPassengerData(i);
	//			if( !pPassengerData )
	//				continue;

	//			if( i == 0 )	// 운전자, 자기 자신
	//			{
	//				SetVehiclePassengerID( i );
	//				SetTransMatrix(pPassengerData->matWorld);
	//			}
	//			else
	//			{
	//				pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(i) );
	//				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	//				if( pCharClient )
	//				{
	//					pCharClient->SetVehiclePassengerID( i );
	//					pCharClient->SetTransMatrix( pPassengerData->matWorld );
	//				}
	//				else if( pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(i) )
	//				{
	//					pCharacter->SetVehiclePassengerID( i );
	//					pCharacter->SetTransMatrix( pPassengerData->matWorld );
	//				}
	//			}
	//		}
	//	}
	//	else if( m_emVehicle == EMVEHICLE_PASSENGER )
	//	{
	//		// 탑승자 입장이라면, 위치좌표는 그 Vehicle 의 운전자쪽에서 계산이 되도록 한다.
	//		// 여기에서는 Vehicle 정보가 없기 때문이다.
	//		return;
	//	}
	//	else
	//	{
	//		D3DXMATRIX matTrans = GetTransMatrix();
	//		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
	//		float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
	//		D3DXMatrixRotationY ( &matYRot, fThetaY );

	//		const D3DXMATRIX matRotTrans = matYRot * matTrans;
	//		SetTransMatrix(matRotTrans);
	//	}
	//}
	//else
	//{
	//	if( m_emVehicle )
	//		return;

	//	D3DXMATRIX matTrans = GetTransMatrix();
	//	D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
	//	float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
	//	D3DXMatrixRotationY ( &matYRot, fThetaY );

	//	const D3DXMATRIX matRotTrans = matYRot * matTrans;
	//	SetTransMatrix(matRotTrans);
	//}	
}

// Vehicle. 내가 어느 위치에 앉아 있는가~?
void GLCharClient::SetVehiclePassengerID( DWORD dwID )
{
	m_dwVehiclePassengerID = dwID;
}

DWORD GLCharClient::GetVehiclePassengerID() const
{
	return m_dwVehiclePassengerID;
}

void GLCharClient::MsgDrugFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_DRUG_FB *pNetMsg = (GLMSG::SNET_DRUG_FB *)pMsg;

	SITEM* pItem = NULL;

	switch( pNetMsg->emFB )
	{
	case EMREQ_DRUG_CONSUME_SUCCESS :
		{
			pItem = GLogicData::GetInstance().GetItem(pNetMsg->sItemID);
		}
		break;
	}

	if( pItem == NULL ) return;

	switch( pItem->sDrugOp.emDrug )
	{
	case ITEM_DRUG_EFFECT_RELEASE :
		{
			ReleaseCureEffect( pItem->sDrugOp.dwERList );
		}
		break;
	}
}

const bool GLCharClient::RECEIVE_SKILLFACT_EntranceExit(const SNATIVEID& sSkillID, const SKILL::SSPEC& sSPEC, WORD wLevel, const STARGETID sID)
{
	TurnAction(GLAT_IDLE);

	return false;
}

const BOOL GLCharClient::IsNonTargetable( ) const
{ 
	if( m_sHALLUCINATE.bNonTargetable || !m_bVisible || RF_FEIGN_DEATH(m_EffSkillVarSet).IsSelectDisable()  )
		return TRUE;

	return FALSE;//(m_sHALLUCINATE.bNonTargetable == true) || (m_bVisible == FALSE);
}

void GLCharClient::MsgClubRenameBrd( NET_MSG_GENERIC* pMsg )
{
	GLMSG::SNET_CLUB_RENAME_BRD* pNetMsg = ( GLMSG::SNET_CLUB_RENAME_BRD* ) pMsg;
	if ( pNetMsg->szClubName )
	{
		StringCchCopy( m_ClubName, CHAR_SZNAME, pNetMsg->szClubName );
		/*m_CharData.m_Base.SetNickName( pNetMsg->szClubName );*/
	}	
}
