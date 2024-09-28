#include "GLChar.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../Party/GLPartyField.h"
// ::sqrt ������ ���ϱ� ���� ���������� ���;
// SKILL_REVISION_RANGE : Ŭ���̾�Ʈ�� ��ġ ����ȭ�� ��Ȯ�� ���� �ʱ� ������ �ణ�� �������� �־���;
// Ŭ���̾�Ʈ�� ��� 2�� ���� �Ǿ� ����;
#define SKILL_REVISION_RANGE 20


const bool isMember(GLChar* const pCaster, const DWORD dwTargetGaeaID, const EMCROW emCrow)
{
	const GLPartyField* const pMyParty = pCaster->GetMyParty();
	if ( pMyParty )
		return emCrow == CROW_PC ? pMyParty->isMemberByGaeaID(dwTargetGaeaID) : false;
	
	return emCrow == CROW_PC ? pCaster->GetGaeaID() == dwTargetGaeaID : false;
}

void SendToMsg_SkillTargetFailed(GLChar* const pChar, const STARGETID& TargetID)
{
	//	Note : �ڽſ���;
	GLMSG::SNETPC_ATTACK_AVOID NetMsg;
	NetMsg.emTarCrow	= TargetID.emCrow;
	NetMsg.dwTarID		= TargetID.GaeaId;

	pChar->SendToClient( &NetMsg );

	// �ֺ� Ŭ���̾�Ʈ�鿡�� �޼��� ����;
	GLMSG::SNETPC_ATTACK_AVOID_BRD NetMsgBrd;
	NetMsgBrd.dwGaeaID	= pChar->GetGaeaID();
	NetMsgBrd.emTarCrow	= TargetID.emCrow;
	NetMsgBrd.dwTarID	= TargetID.GaeaId;

	pChar->SendMsgViewAround(&NetMsgBrd);	
}

const bool SkillTarget_Self_Self(GLSkillTargetArgv& SkillTargetArgv)
{
	const SKILL::SSPECIAL_SKILL& SkillSpecialSkill = SkillTargetArgv.pSkill->m_sSPECIAL_SKILL;	
	if ( SkillSpecialSkill.emSSTYPE == SKILL::EMSSTYPE_SUMMONS )
	{
		/*const SKILL::SCREATURE_LVL& sCREATURE_LVL = SkillSpecialSkill.sCREATURE_LVL[SkillTargetArgv.wSkillLevel];
		const SCROWDATA* const  pCrowData = GLCrowDataMan::GetInstance().GetCrowData(sCREATURE_LVL.sID);
		if( pCrowData == NULL )
			return false;

		const GLSUMMON* const pSummonData = GLCONST_SUMMON::GetSummonData(pCrowData->m_emSummonType);
		if ( pSummonData == NULL )
			return false;*/
	}

	return true;
}

const bool setTarget_Self_Zone(GLSkillTargetArgv& SkillTargetArgv, const EMIMPACT_SIDE emSide)
{	
	GLChar* const pCaster = SkillTargetArgv.pChar;
	GLLandMan* const pLand(pCaster->GetLandMan());
	if ( !pLand )
	{
		return false;
	}

	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];

	WORD wRange = SkillLevelData.wAPPLYRANGE;

	/*
	if( RF_PROVOKE( pCaster->m_EffSkillVarSet ).IsOn() )
	{
		wRange = RF_PROVOKE( pCaster->m_EffSkillVarSet ).GetEffDistance();
	}
	*/

	const float fLimitApplyRange = ::pow(float(pCaster->GETBODYRADIUS() + pCaster->GETSUM_APPLYRANGE_ALL() + wRange + SKILL_REVISION_RANGE), 2.0f); 
		
	BYTE btIndex = 0;
	STARID* const sTARIDS = SkillTargetArgv.pNetMsg->sTARIDS;
	
	const D3DXVECTOR3& vMyPosition = pCaster->GetPosition();	
	const DWORD nTarget = SkillTargetArgv.pNetMsg->btTARNUM;	
	for (DWORD _i = 0; _i < nTarget; ++_i )
	{
		const EMCROW emCrow = EMCROW(sTARIDS[_i].emCrow);
		const DWORD dwTargetGaeaID = sTARIDS[_i].dwID;
		ServerActor* pTargetActor = pCaster->GetTarget(STARGETID(emCrow, dwTargetGaeaID));
		if ( pTargetActor == NULL )
			continue;

		if ( !pCaster->IsReActionable( pTargetActor, emSide ) )
			continue;

		/*const bool bMember = isMember(pCaster, dwTargetGaeaID, emCrow);

		if ( (emSide == EMIMPACT_SIDE_PARTY) && (bMember == false) )
			continue; // �Ʊ� ��� ��Ƽ���� �ƴϸ� �н�;
		else if ( (emSide == EMIMPACT_SIDE_ENEMY) && (bMember == true) )
			continue; // ���� ��� ��Ƽ���̸� �н�;*/
		
		const float fDistance = ::D3DXVec3LengthSq(&D3DXVECTOR3(vMyPosition - pTargetActor->GetPosition()));
		if ( fDistance > fLimitApplyRange )
			continue;

		// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
		if ( pLand->IsLayerSimpleCollision( vMyPosition, pTargetActor->GetPosition(), 5.f ) )
		{
			continue;
		}

		sTARIDS[btIndex++] = sTARIDS[_i];
	}

	// MultiSkill�� �����Ѵٸ� ��ȿ���˻�� �پ�� Ÿ�ټ���ŭ ����;
	if ( SkillTargetArgv.pNetMsg->btSECONDTARNUM > 0 )
	{
		memcpy( sTARIDS + btIndex, sTARIDS + SkillTargetArgv.pNetMsg->btTARNUM, sizeof( STARID ) * SkillTargetArgv.pNetMsg->btSECONDTARNUM );
		sTARIDS[ btIndex + SkillTargetArgv.pNetMsg->btSECONDTARNUM ].dwID = 0xFFFFFFFF;			//end mask
	}

	SkillTargetArgv.pNetMsg->btTARNUM = btIndex;
	return true;
}

const bool SkillTarget_Spec_Self(GLSkillTargetArgv& SkillTargetArgv, const EMIMPACT_SIDE emSide)
{
	GLChar* const pCaster = SkillTargetArgv.pChar;	
	GLLandMan* const pLand(pCaster->GetLandMan());
	if ( !pLand )
		return false;
	
	STARID* const sTARIDS = SkillTargetArgv.pNetMsg->sTARIDS;
	const EMCROW emCrow = EMCROW(sTARIDS[0].emCrow);
	const DWORD dwTargetGaeaID = sTARIDS[0].dwID;

	ServerActor* pTargetActor = pCaster->GetTarget(STARGETID(emCrow, dwTargetGaeaID));
	if ( pTargetActor == NULL )
		return false;

	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;

	WORD wRange = SkillBasic.wTARRANGE;

	/*
	if( RF_PROVOKE( pCaster->m_EffSkillVarSet ).IsOn() )
	{
		wRange = RF_PROVOKE( pCaster->m_EffSkillVarSet ).GetEffDistance();
	}
	*/

	const D3DXVECTOR3& vTargetPosition = pTargetActor->GetPosition();
	const float fDistanceToTarget = ::D3DXVec3LengthSq(&D3DXVECTOR3(pCaster->GetPosition() - vTargetPosition));
	const float fLimitTargetRange = ::pow(float(pCaster->GETBODYRADIUS() + pCaster->GETSUM_TARRANGE_ALL() + wRange + SKILL_REVISION_RANGE), 2.0f); 
	if ( fDistanceToTarget > fLimitTargetRange )
		return false;

	if ( !pCaster->IsReActionable( pTargetActor, emSide ) )
		return false;

	// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
	if ( pLand->IsLayerSimpleCollision( pCaster->GetPosition(), vTargetPosition, 5.f ) )
	{
		return false;
	}

	/*const bool bMember = isMember(pCaster, dwTargetGaeaID, emCrow);

	if ( (emSide == EMIMPACT_SIDE_PARTY) && (bMember == false) )
		return false; // �Ʊ� ��� ��Ƽ���� �ƴϸ� �н�;
	else if ( (emSide == EMIMPACT_SIDE_ENEMY) && (bMember == true) )
		return false; // ���� ��� ��Ƽ���̸� �н�;*/

	return true;
}

const bool SkillTarget_Spec_Zone(GLSkillTargetArgv& SkillTargetArgv, const EMIMPACT_SIDE emSide)
{
	GLChar* const pCaster = SkillTargetArgv.pChar;	
	GLLandMan* const pLand(pCaster->GetLandMan());
	if ( !pLand )
		return false;

	const ServerActor* const pTargetActor = pCaster->GetTarget(SkillTargetArgv.TargetID);
	if ( pTargetActor == NULL )
		return false;	

	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;

	WORD wRange = SkillBasic.wTARRANGE;

	/*
	if( RF_PROVOKE( pCaster->m_EffSkillVarSet ).IsOn() )
	{
		wRange = RF_PROVOKE( pCaster->m_EffSkillVarSet ).GetEffDistance();
	}
	*/
	
	const D3DXVECTOR3& vTargetPosition = pTargetActor->GetPosition();
	const float fDistanceToTarget = ::D3DXVec3LengthSq(&D3DXVECTOR3(pCaster->GetPosition() - vTargetPosition));
	const float fLimitTargetRange = ::pow(float(pCaster->GETBODYRADIUS() + pCaster->GETSUM_TARRANGE_ALL() + wRange + SKILL_REVISION_RANGE), 2.0f); 
	if ( fDistanceToTarget > fLimitTargetRange )
		return false;
	
	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	const float fLimitRange = ::pow(float(pTargetActor->GetBodyRadius() + pCaster->GETSUM_APPLYRANGE_ALL() + SkillLevelData.wAPPLYRANGE + SKILL_REVISION_RANGE), 2.0f);
	const DWORD nTarget = SkillTargetArgv.pNetMsg->btTARNUM;
	
	BYTE btIndex = 0;	
	STARID* const sTARIDS = SkillTargetArgv.pNetMsg->sTARIDS;	
	for (DWORD _i = 0; _i < nTarget; ++_i )
	{
		const EMCROW emCrow = EMCROW(sTARIDS[_i].emCrow);
		const DWORD dwTargetGaeaID = sTARIDS[_i].dwID;
		ServerActor* pTargetActor = pCaster->GetTarget(STARGETID(emCrow, dwTargetGaeaID));
		if ( pTargetActor == NULL )
			continue;

		if ( !pCaster->IsReActionable( pTargetActor, emSide ) )
			continue;

		/*const bool bMember = isMember(pCaster, dwTargetGaeaID, emCrow);

		if ( (emSide == EMIMPACT_SIDE_PARTY) && (bMember == false) )
			continue; // �Ʊ� ��� ��Ƽ���� �ƴϸ� �н�;
		else if ( (emSide == EMIMPACT_SIDE_ENEMY) && (bMember == true) )
			continue; // ���� ��� ��Ƽ���̸� �н�;*/

		const float fDistance = ::D3DXVec3LengthSq(&D3DXVECTOR3(vTargetPosition - pTargetActor->GetPosition()));
		if ( fDistance > fLimitRange )
			continue;

		// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
		if ( pLand->IsLayerSimpleCollision( vTargetPosition, pTargetActor->GetPosition(), 5.f ) )
		{
			continue;
		}
		
		sTARIDS[btIndex++] = sTARIDS[_i];
	}

	// MultiSkill�� �����Ѵٸ� ��ȿ���˻�� �پ�� Ÿ�ټ���ŭ ����;
	if ( SkillTargetArgv.pNetMsg->btSECONDTARNUM > 0 )
	{
		memcpy( sTARIDS + btIndex, sTARIDS + SkillTargetArgv.pNetMsg->btTARNUM, sizeof( STARID ) * SkillTargetArgv.pNetMsg->btSECONDTARNUM );
		sTARIDS[ btIndex + SkillTargetArgv.pNetMsg->btSECONDTARNUM ].dwID = 0xFFFFFFFF;			//end mask
	}

	// Ÿ�ټ��� �����Ѵ�;
	SkillTargetArgv.pNetMsg->btTARNUM = btIndex;

	return true;
}

const bool SkillTarget_Spec_FanWize(GLSkillTargetArgv& SkillTargetArgv, const EMIMPACT_SIDE emSide)
{
	GLChar* const pCaster = SkillTargetArgv.pChar;
	GLLandMan* const pLand(pCaster->GetLandMan());
	if ( !pLand )
		return false;

	const ServerActor* const pTargetActor = pCaster->GetTarget(SkillTargetArgv.TargetID);
	if ( pTargetActor == NULL )
		return false;

	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;

	WORD wRange = SkillBasic.wTARRANGE;

	/*
	if( RF_PROVOKE( pCaster->m_EffSkillVarSet ).IsOn() )
	{
		wRange = RF_PROVOKE( pCaster->m_EffSkillVarSet ).GetEffDistance();
	}
	*/

	const D3DXVECTOR3& vMyPosition = pCaster->GetPosition();
	const D3DXVECTOR3& vTargetDirect = pTargetActor->GetPosition() - vMyPosition;

	const float fDistanceToTarget = ::D3DXVec3LengthSq(&vTargetDirect);
	const float fLimitTargetRange = ::pow(float(pTargetActor->GetBodyRadius() + pCaster->GETBODYRADIUS() + pCaster->GETSUM_TARRANGE_ALL() + wRange + SKILL_REVISION_RANGE), 2.0f); 
	if ( fDistanceToTarget > fLimitTargetRange )
		return false;

	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	const float fLimitApplyAngle = D3DXToRadian(SkillLevelData.wAPPLYANGLE) * 0.5f;
	const float fLimitApplyRange = ::pow(float(pTargetActor->GetBodyRadius() + pCaster->GETBODYRADIUS() + pCaster->GETSUM_APPLYRANGE_ALL() + SkillLevelData.wAPPLYRANGE + SKILL_REVISION_RANGE), 2.0f);
	const DWORD nTarget = SkillTargetArgv.pNetMsg->btTARNUM;

	BYTE btIndex = 0;	
	STARID* sTARIDS = SkillTargetArgv.pNetMsg->sTARIDS;
	for (DWORD _i = 0; _i < nTarget; ++_i )
	{
		const EMCROW emCrow = EMCROW(sTARIDS[_i].emCrow);
		const DWORD dwTargetGaeaID = sTARIDS[_i].dwID;
		ServerActor* pTargetActor = pCaster->GetTarget(STARGETID(emCrow, dwTargetGaeaID));
		if ( pTargetActor == NULL )
			continue;

		if ( !pCaster->IsReActionable( pTargetActor, emSide ) )
			continue;

		/*const bool bMember = isMember(pCaster, dwTargetGaeaID, emCrow);

		if ( (emSide == EMIMPACT_SIDE_PARTY) && (bMember == false) )
			continue; // �Ʊ� ��� ��Ƽ���� �ƴϸ� �н�;
		else if ( (emSide == EMIMPACT_SIDE_ENEMY) && (bMember == true) )
			continue; // ���� ��� ��Ƽ���̸� �н�;*/

		const D3DXVECTOR3 vDirectToTarget = pTargetActor->GetPosition() - vMyPosition;
		const float fDistance = ::D3DXVec3LengthSq(&vDirectToTarget);
		if ( fDistance > fLimitApplyRange )
			continue;

		const float fAngle = ::abs(::DXGetThetaYFromDirection(vDirectToTarget, vTargetDirect));
		if ( (SkillLevelData.wAPPLYANGLE != 0) && (fAngle > fLimitApplyAngle) )
			continue;

		// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
		if ( pLand->IsLayerSimpleCollision( pTargetActor->GetPosition(), vMyPosition, 5.f ) )
		{
			continue;
		}

		sTARIDS[btIndex++] = sTARIDS[_i];
	}

	// MultiSkill�� �����Ѵٸ� ��ȿ���˻�� �پ�� Ÿ�ټ���ŭ ����;
	if ( SkillTargetArgv.pNetMsg->btSECONDTARNUM > 0 )
	{
		memcpy( sTARIDS + btIndex, sTARIDS + SkillTargetArgv.pNetMsg->btTARNUM, sizeof( STARID ) * SkillTargetArgv.pNetMsg->btSECONDTARNUM );
		sTARIDS[ btIndex + SkillTargetArgv.pNetMsg->btSECONDTARNUM ].dwID = 0xFFFFFFFF;			//end mask
	}

	SkillTargetArgv.pNetMsg->btTARNUM = btIndex;

	return true;
}

const bool SetTarget_SelfToSpec_Piercing(GLSkillTargetArgv& SkillTargetArgv, const EMIMPACT_SIDE emSide)
{
	GLChar* const pCaster = SkillTargetArgv.pChar;	
	GLLandMan* const pLand(pCaster->GetLandMan());
	if ( !pLand )
		return false;

	const ServerActor* const pTargetActor = pCaster->GetTarget(SkillTargetArgv.TargetID);
	if ( pTargetActor == NULL )
		return false;
	
	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;

	WORD wRange = SkillBasic.wTARRANGE;

	/*
	if( RF_PROVOKE( pCaster->m_EffSkillVarSet ).IsOn() )
	{
		wRange = RF_PROVOKE( pCaster->m_EffSkillVarSet ).GetEffDistance();
	}
	*/

	const D3DXVECTOR3& vTargetPosition = pTargetActor->GetPosition();
	const D3DXVECTOR3& vTargetDirect = vTargetPosition - pCaster->GetPosition();
	const float fDistanceToTarget = ::D3DXVec3LengthSq(&vTargetDirect);
	const float fLimitTargetRange = ::pow(float(pTargetActor->GetBodyRadius() + pCaster->GETBODYRADIUS() + pCaster->GETSUM_TARRANGE_ALL() + wRange + SKILL_REVISION_RANGE), 2.0f); 
	if ( fDistanceToTarget > fLimitTargetRange )
		return false;

	D3DXVECTOR3 vRightNormal;
	::D3DXVec3Normalize(&vRightNormal, &D3DXVECTOR3(vTargetDirect.z, vTargetDirect.y, -vTargetDirect.x));

	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	const WORD wApplyRange = SkillLevelData.wAPPLYRANGE + WORD(pCaster->GETSUM_APPLYRANGE_ALL());
	const DWORD nTarget = SkillTargetArgv.pNetMsg->btTARNUM;		

	BYTE btIndex = 0;	
	STARID* sTARIDS = SkillTargetArgv.pNetMsg->sTARIDS;
	for (DWORD _i = 0; _i < nTarget; ++_i )
	{
		const EMCROW emCrow = EMCROW(sTARIDS[_i].emCrow);
		const DWORD dwTargetGaeaID = sTARIDS[_i].dwID;
		ServerActor* pTargetActor = pCaster->GetTarget(STARGETID(emCrow, dwTargetGaeaID));
		if ( pTargetActor == NULL )
			continue;

		if ( !pCaster->IsReActionable( pTargetActor, emSide ) )
			continue;

		/*const bool bMember = isMember(pCaster, dwTargetGaeaID, emCrow);

		if ( (emSide == EMIMPACT_SIDE_PARTY) && (bMember == false) )
			continue; // �Ʊ� ��� ��Ƽ���� �ƴϸ� �н�;
		else if ( (emSide == EMIMPACT_SIDE_ENEMY) && (bMember == true) )
			continue; // ���� ��� ��Ƽ���̸� �н�;*/

		const D3DXVECTOR3 vDirectToTarget = vTargetPosition - pTargetActor->GetPosition();
		const float fDistance = ::D3DXVec3LengthSq(&vDirectToTarget);
		if ( fDistance > fLimitTargetRange )
			continue;

		const float fDistanceFromLine = ::D3DXVec3Dot(&vDirectToTarget, &vRightNormal);
		if ( fDistanceFromLine > float(pTargetActor->GetBodyRadius() + wApplyRange) )
			continue;

		// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
		if ( pLand->IsLayerSimpleCollision( vTargetPosition, pTargetActor->GetPosition(), 5.f ) )
		{
			continue;
		}

		sTARIDS[btIndex++] = sTARIDS[_i];
	}

	// MultiSkill�� �����Ѵٸ� ��ȿ���˻�� �پ�� Ÿ�ټ���ŭ ����;
	if ( SkillTargetArgv.pNetMsg->btSECONDTARNUM > 0 )
	{
		memcpy( sTARIDS + btIndex, sTARIDS + SkillTargetArgv.pNetMsg->btTARNUM, sizeof( STARID ) * SkillTargetArgv.pNetMsg->btSECONDTARNUM );
		sTARIDS[ btIndex + SkillTargetArgv.pNetMsg->btSECONDTARNUM ].dwID = 0xFFFFFFFF;			//end mask
	}

	SkillTargetArgv.pNetMsg->btTARNUM = btIndex;
	return true;
}

inline const bool SetTarget_Zone_Zone(GLSkillTargetArgv& SkillTargetArgv, const EMIMPACT_SIDE emSide)
{
	GLChar* const pCaster = SkillTargetArgv.pChar;
	GLLandMan* const pLand(pCaster->GetLandMan());
	if ( !pLand )
		return false;

	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;

	WORD wRange = SkillBasic.wTARRANGE;

	/*
	if( RF_PROVOKE( pCaster->m_EffSkillVarSet ).IsOn() )
	{
		wRange = RF_PROVOKE( pCaster->m_EffSkillVarSet ).GetEffDistance();
	}
	*/

	const D3DXVECTOR3& vTargetPosition = SkillTargetArgv.TargetID.vPos;
	const float fDistanceToTarget = ::D3DXVec3LengthSq(&D3DXVECTOR3(pCaster->GetPosition() - vTargetPosition));
	const float fLimitTargetRange = ::pow(float(pCaster->GETBODYRADIUS() + pCaster->GETSUM_TARRANGE_ALL() + wRange + SKILL_REVISION_RANGE), 2.0f); 
	if ( fDistanceToTarget > fLimitTargetRange )
		return false;

	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	const float fLimitApplyRange = ::pow(float(pCaster->GETBODYRADIUS() + pCaster->GETSUM_APPLYRANGE_ALL() + SkillLevelData.wAPPLYRANGE + SKILL_REVISION_RANGE), 2.0f);
	const DWORD nTarget = SkillTargetArgv.pNetMsg->btTARNUM;

	BYTE btIndex = 0;	
	STARID* sTARIDS = SkillTargetArgv.pNetMsg->sTARIDS;
	for (DWORD _i = 0; _i < nTarget; ++_i )
	{
		const EMCROW emCrow = EMCROW(sTARIDS[_i].emCrow);
		const DWORD dwTargetGaeaID = sTARIDS[_i].dwID;
		ServerActor* pTargetActor = pCaster->GetTarget(STARGETID(emCrow, dwTargetGaeaID));
		if ( pTargetActor == NULL )
			continue;

		if ( !pCaster->IsReActionable( pTargetActor, emSide ) )
			continue;

		/*const bool bMember = isMember(pCaster, dwTargetGaeaID, emCrow);

		if ( (emSide == EMIMPACT_SIDE_PARTY) && (bMember == false) )
			continue; // �Ʊ� ��� ��Ƽ���� �ƴϸ� �н�;
		else if ( (emSide == EMIMPACT_SIDE_ENEMY) && (bMember == true) )
			continue; // ���� ��� ��Ƽ���̸� �н�;*/

		const float fDistance = ::D3DXVec3LengthSq(&D3DXVECTOR3(vTargetPosition - pTargetActor->GetPosition()));
		if ( fDistance > fLimitApplyRange )
			continue;

		// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
		if ( pLand->IsLayerSimpleCollision( vTargetPosition, pTargetActor->GetPosition(), 5.f ) )
		{
			continue;
		}

		sTARIDS[btIndex++] = sTARIDS[_i];
	}

	// MultiSkill�� �����Ѵٸ� ��ȿ���˻�� �پ�� Ÿ�ټ���ŭ ����;
	if ( SkillTargetArgv.pNetMsg->btSECONDTARNUM > 0 )
	{
		memcpy( sTARIDS + btIndex, sTARIDS + SkillTargetArgv.pNetMsg->btTARNUM, sizeof( STARID ) * SkillTargetArgv.pNetMsg->btSECONDTARNUM );
		sTARIDS[ btIndex + SkillTargetArgv.pNetMsg->btSECONDTARNUM ].dwID = 0xFFFFFFFF;			//end mask
	}

	SkillTargetArgv.pNetMsg->btTARNUM = btIndex;
	return true;
}

inline const bool SetTarget_Zone_FanWize(GLSkillTargetArgv& SkillTargetArgv, const EMIMPACT_SIDE emSide)
{
	GLChar* const pCaster = SkillTargetArgv.pChar;
	GLLandMan* const pLand(pCaster->GetLandMan());
	if ( !pLand )
		return false;

	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;

	WORD wRange = SkillBasic.wTARRANGE;

	/*
	if( RF_PROVOKE( pCaster->m_EffSkillVarSet ).IsOn() )
	{
		wRange = RF_PROVOKE( pCaster->m_EffSkillVarSet ).GetEffDistance();
	}
	*/

	const D3DXVECTOR3& vTargetPosition = SkillTargetArgv.TargetID.vPos;	
	const D3DXVECTOR3& vTargetDirect = vTargetPosition - pCaster->GetPosition();
	const float fDistanceToTarget = ::D3DXVec3LengthSq(&vTargetDirect);
	const float fLimitTargetRange = ::pow(float(pCaster->GETBODYRADIUS() + pCaster->GETSUM_TARRANGE_ALL() + wRange + SKILL_REVISION_RANGE), 2.0f); 
	if ( fDistanceToTarget > fLimitTargetRange )
	{
		SkillTargetArgv.pNetMsg->btTARNUM = 0;
		return false;
	}
	
	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	const float fLimitApplyAngle = D3DXToRadian(SkillLevelData.wAPPLYANGLE) * 0.5f;
	const float fLimitApplyRange = ::pow(float(pCaster->GETBODYRADIUS() + pCaster->GETSUM_APPLYRANGE_ALL() + SkillLevelData.wAPPLYRANGE + SKILL_REVISION_RANGE), 2.0f);
	const DWORD nTarget = SkillTargetArgv.pNetMsg->btTARNUM;

	BYTE btIndex = 0;	
	STARID* sTARIDS = SkillTargetArgv.pNetMsg->sTARIDS;
	for (DWORD _i = 0; _i < nTarget; ++_i )
	{
		const EMCROW emCrow = EMCROW(sTARIDS[_i].emCrow);
		const DWORD dwTargetGaeaID = sTARIDS[_i].dwID;
		ServerActor* pTargetActor = pCaster->GetTarget(STARGETID(emCrow, dwTargetGaeaID));
		if ( pTargetActor == NULL )
			continue;

		if ( !pCaster->IsReActionable( pTargetActor, emSide ) )
			continue;

		/*const bool bMember = isMember(pCaster, dwTargetGaeaID, emCrow);

		if ( (emSide == EMIMPACT_SIDE_PARTY) && (bMember == false) )
			continue; // �Ʊ� ��� ��Ƽ���� �ƴϸ� �н�;
		else if ( emSide == EMIMPACT_SIDE_ENEMY )
		{
			if ( bMember == true )
				continue; // ���� ��� ��Ƽ���̸� �н�;
		}*/

		const D3DXVECTOR3 vDirectToTarget = pTargetActor->GetPosition() - pCaster->GetPosition();
		const float fDistance = ::D3DXVec3LengthSq(&vDirectToTarget);
		if ( fDistance > fLimitApplyRange )
			continue;

		const float fAngle = ::abs(::DXGetThetaYFromDirection(vDirectToTarget, vTargetDirect));
		if ( (SkillLevelData.wAPPLYANGLE != 0) && (fAngle > fLimitApplyAngle) )
			continue;

		// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
		if ( pLand->IsLayerSimpleCollision( pTargetActor->GetPosition(), pCaster->GetPosition(), 5.f ) )
		{
			continue;
		}

		sTARIDS[btIndex++] = sTARIDS[_i];
	}

	// MultiSkill�� �����Ѵٸ� ��ȿ���˻�� �پ�� Ÿ�ټ���ŭ ����;
	if ( SkillTargetArgv.pNetMsg->btSECONDTARNUM > 0 )
	{
		memcpy( sTARIDS + btIndex, sTARIDS + SkillTargetArgv.pNetMsg->btTARNUM, sizeof( STARID ) * SkillTargetArgv.pNetMsg->btSECONDTARNUM );
		sTARIDS[ btIndex + SkillTargetArgv.pNetMsg->btSECONDTARNUM ].dwID = 0xFFFFFFFF;			//end mask
	}

	SkillTargetArgv.pNetMsg->btTARNUM = btIndex;
	return true;
}






const bool setTarget_Self(GLSkillTargetArgv& SkillTargetArgv)
{
    static std::set<DWORD> setHackingUser;

	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	if ( SkillTargetArgv.pNetMsg->btTARNUM > SkillLevelData.wTARNUM )
		return false;

    const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;
	switch ( SkillBasic.emIMPACT_REALM )
	{
	case REALM_SELF:
        {
            // �ڱ��ڽſ��� ����ؾ��ϴ� ��ų�� ��Ŷ�������� ������ ����ϴ� ���� ���� [2/27/2014 hsshin]
            GLChar* pChar = SkillTargetArgv.pChar;
            if ( pChar->GetGaeaID() != SkillTargetArgv.TargetID.GaeaId )
            {
                const DWORD charDbNum = pChar->CharDbNum();
                const DWORD charUserID = pChar->GetUserID();

                // ��Ŷ ������ �� ������ �����Ͽ� �α� ���� (�αװ� ���ߵǴ°��� �������� �ߺ��˻�);
                if ( setHackingUser.count(charDbNum) == 0 )
                {
                    setHackingUser.insert(charDbNum);
                    sc::writeLogError(sc::string::format("ReqSkill Packet Hacking User!!! CharID: %1%, UserID: %2%", charDbNum, charUserID));
                }

                /* // [5/14/2014 hsshin] ��Ŷ �������� �ʾ������� Ư����ų�� ���Ͽ� �����ϴ� ������ �������� �ڵ�� ����;
                GLGaeaServer* pGaeaServer = pChar->GetGaeaServer();
                if ( pGaeaServer )
                {
                    NET_CLOSE_CLIENT netMsg(charDbNum);
                    pGaeaServer->SENDTOAGENT(&netMsg);
                }*/
                return false;
            }
            return SkillTarget_Self_Self(SkillTargetArgv);
        }
	case REALM_ZONE:
		return setTarget_Self_Zone(SkillTargetArgv, SkillBasic.emIMPACT_SIDE);
	}

	return false;
}

const bool setTarget_Spec(GLSkillTargetArgv& SkillTargetArgv)
{
    //���߿� �ɷ��ִٸ� Ÿ���� ������ ���ߴ���̾���Ѵ�.
    if ( SkillTargetArgv.pChar->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
    {
        if ( SkillTargetArgv.TargetID.GaeaId != SkillTargetArgv.pChar->m_sTaunt.sTauntTargetID.GaeaId )
            return false;
    }

	if ( SkillTargetArgv.pChar->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) )
	{		
		if ( SkillTargetArgv.TargetID.GaeaId != RF_PROVOKE ( SkillTargetArgv.pChar->m_EffSkillVarSet ).GetHostID().GaeaId )
			return false;
	}

	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	if ( SkillTargetArgv.pNetMsg->btTARNUM > SkillLevelData.wTARNUM )
		return false;

	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;
	switch ( SkillBasic.emIMPACT_REALM )
	{
	case REALM_SELF:
		return SkillTarget_Spec_Self(SkillTargetArgv, SkillBasic.emIMPACT_SIDE);
	case REALM_ZONE:
		return SkillTarget_Spec_Zone(SkillTargetArgv, SkillBasic.emIMPACT_SIDE);
	case REALM_FANWIZE:
		return SkillTarget_Spec_FanWize(SkillTargetArgv, SkillBasic.emIMPACT_SIDE);
	}

	return false;
}

const bool setTarget_SelfToSpec(GLSkillTargetArgv& SkillTargetArgv)
{
    //���߿� �ɷ��ִٸ� Ÿ���� ������ ���ߴ���̾���Ѵ�.
    if ( SkillTargetArgv.pChar->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
    {
        if ( SkillTargetArgv.TargetID.GaeaId != SkillTargetArgv.pChar->m_sTaunt.sTauntTargetID.GaeaId )
            return false;
    }

	if ( SkillTargetArgv.pChar->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) )
	{		
		if ( SkillTargetArgv.TargetID.GaeaId != RF_PROVOKE ( SkillTargetArgv.pChar->m_EffSkillVarSet ).GetHostID().GaeaId )
			return false;
	}

	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	const DWORD nTarget = SkillTargetArgv.pChar->GETSUM_PIERCE() + SkillLevelData.wPIERCENUM;
	if ( SkillTargetArgv.pNetMsg->btTARNUM > nTarget )
		return false;	

	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;
	return SetTarget_SelfToSpec_Piercing(SkillTargetArgv, SkillBasic.emIMPACT_SIDE);
}

const bool setTarget_Zone(GLSkillTargetArgv& SkillTargetArgv)
{	
    //���߿� �ɷ��ִٸ� Ÿ���� ������ ���ߴ���̾���Ѵ�.
    if ( SkillTargetArgv.pChar->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
    {
        if ( SkillTargetArgv.TargetID.GaeaId != SkillTargetArgv.pChar->m_sTaunt.sTauntTargetID.GaeaId )
            return false;
    }

	if ( SkillTargetArgv.pChar->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) )
	{		
		if ( SkillTargetArgv.TargetID.GaeaId != RF_PROVOKE ( SkillTargetArgv.pChar->m_EffSkillVarSet ).GetHostID().GaeaId )
			return false;
	}

	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	if ( SkillTargetArgv.pNetMsg->btTARNUM > SkillLevelData.wTARNUM )
		return false;

	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;
	switch ( SkillBasic.emIMPACT_REALM )
	{
	case REALM_ZONE:
		return SetTarget_Zone_Zone(SkillTargetArgv, SkillBasic.emIMPACT_SIDE);
	case REALM_FANWIZE:
		return SetTarget_Zone_FanWize(SkillTargetArgv, SkillBasic.emIMPACT_SIDE);
	}

	return false;
}

const bool setTarget_Land(GLSkillTargetArgv& SkillTargetArgv)
{
	GLChar* const pCaster = SkillTargetArgv.pChar;
	GLLandMan* const pLand(pCaster->GetLandMan());
	if ( !pLand )
		return false;

	boost::shared_ptr<NavigationMesh> spNavi = pCaster->GetLandMan()->GetNavi();
	if ( spNavi == NULL )
		return false;

	const D3DXVECTOR3& vTargetPosition = SkillTargetArgv.pNetMsg->vTargetPosition;
	D3DXVECTOR3		vCollision;
	DWORD			dwCollID;
	BOOL			bColl;
	spNavi->IsCollision(vTargetPosition + D3DXVECTOR3( 0.0f, 10.0f, 0.0f ), vTargetPosition + D3DXVECTOR3( 0.0f, -10.0f, 0.0f ), vCollision, dwCollID, bColl );
	if ( bColl == FALSE )
		return false;	
	
	const SKILL::SBASIC& SkillBasic = SkillTargetArgv.pSkill->m_sBASIC;
	const D3DXVECTOR3& vMyPosition = pCaster->GetPosition();
	const float fDistanceToTarget = ::D3DXVec3LengthSq(&D3DXVECTOR3(vMyPosition - vTargetPosition));
	const float fLimitTargetRange = ::pow(float(pCaster->GETBODYRADIUS() + pCaster->GETSUM_TARRANGE_ALL() + SkillBasic.wTARRANGE + SKILL_REVISION_RANGE), 2.0f); 
	if ( fDistanceToTarget > fLimitTargetRange )
		return false;

	const SKILL::CDATA_LVL& SkillLevelData = SkillTargetArgv.pSkill->m_sAPPLY.sDATA_LVL[SkillTargetArgv.wSkillLevel];
	const float fLimitApplyRange = ::pow(float(pCaster->GETBODYRADIUS() + pCaster->GETSUM_APPLYRANGE_ALL() + SkillLevelData.wAPPLYRANGE + SKILL_REVISION_RANGE), 2.0f);
	const DWORD nTarget = SkillTargetArgv.pNetMsg->btTARNUM;

	BYTE btIndex = 0;	
	STARID* sTARIDS = SkillTargetArgv.pNetMsg->sTARIDS;
	for (DWORD _i = 0; _i < nTarget; ++_i )
	{
		const EMCROW emCrow = EMCROW(sTARIDS[_i].emCrow);
		const DWORD dwTargetGaeaID = sTARIDS[_i].dwID;
		ServerActor* pTargetActor = pCaster->GetTarget(STARGETID(emCrow, dwTargetGaeaID));
		if ( pTargetActor == NULL )
			continue;

		if ( !pCaster->IsReActionable( pTargetActor, SkillBasic.emIMPACT_SIDE ) )
			continue;

		/*const bool bMember = isMember(pCaster, dwTargetGaeaID, emCrow);

		if ( (SkillBasic.emIMPACT_SIDE == EMIMPACT_SIDE_PARTY) && (bMember == false) )
			continue; // �Ʊ� ��� ��Ƽ���� �ƴϸ� �н�;
		else if ( (SkillBasic.emIMPACT_SIDE == EMIMPACT_SIDE_ENEMY) && (bMember == true) )
			continue; // ���� ��� ��Ƽ���̸� �н�;*/

		const float fDistance = ::D3DXVec3LengthSq(&D3DXVECTOR3(vMyPosition - pTargetActor->GetPosition()));
		if ( fDistance > fLimitApplyRange )
			continue;

		// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
		if ( pLand->IsLayerSimpleCollision( vMyPosition, pTargetActor->GetPosition(), 5.f ) )
		{
			continue;
		}

		sTARIDS[btIndex++] = sTARIDS[_i];
	}

	// MultiSkill�� �����Ѵٸ� ��ȿ���˻�� �پ�� Ÿ�ټ���ŭ ����;
	if ( SkillTargetArgv.pNetMsg->btSECONDTARNUM > 0 )
	{
		memcpy( sTARIDS + btIndex, sTARIDS + SkillTargetArgv.pNetMsg->btTARNUM, sizeof( STARID ) * SkillTargetArgv.pNetMsg->btSECONDTARNUM );
		sTARIDS[ btIndex + SkillTargetArgv.pNetMsg->btSECONDTARNUM ].dwID = 0xFFFFFFFF;			//end mask
	}

	SkillTargetArgv.pNetMsg->btTARNUM = btIndex;

	return true;
}
