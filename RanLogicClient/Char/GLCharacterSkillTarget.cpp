#include "../pch.h"

#include "../../EngineLib/GUInterface/Cursor.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxMethods.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/EditMeshs.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"

#include "../../RanLogic/GLLevelFile.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/stl_Func.h"

#include "../Land/GLLandManClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "../Party/GLPartyClient.h"
#include "../Strike/GLStrikeM.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------
BOOL SKT_SELFZONEOUR( const GLSKILL&		sSKILL, 
					  const WORD			wLEVEL, 
					  GLCharacter*			pChar, 
					  GLGaeaClient*			pGaeaClient, 
					  const D3DXVECTOR3&	vPos,
					  SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_SELFZONEENEMY( const STARGETID&		sTargetID, 
					    const GLSKILL&			sSKILL, 
						const WORD				wLEVEL, 
						GLCharacter*			pChar, 
						GLGaeaClient*			pGaeaClient, 
						const D3DXVECTOR3&		vPos,
						SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_SELFZONANYBODY( const GLSKILL&			sSKILL, 
						 const WORD				wLEVEL,
						 GLCharacter*			pChar,
						 GLGaeaClient*			pGaeaClient, 
						 const D3DXVECTOR3&		vPos,
						 SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARSPECOUR_SELF( const STARGETID&		sTargetID, 
						 GLGaeaClient*			pGaeaClient, 
						 SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARSPECENEMY_SELF( const STARGETID&	sTargetID, 
						   GLGaeaClient*		pGaeaClient, 
						   SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARSPECANYBODY_SELF( const STARGETID&	sTargetID,
							 SKILLTARGETDATA&	sSkillTarget );

BOOL SKT_TARSPECOUR( const STARGETID&		sTargetID, 
					 const GLSKILL&			sSKILL, 
					 const WORD				wLEVEL, 
					 GLCharacter*			pChar, 
					 GLGaeaClient*			pGaeaClient, 
					 const D3DXVECTOR3&		vPos, 
					 SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARSPECENEMY( const STARGETID&		sTargetID, 
					  const GLSKILL&		sSKILL, 
					  const WORD			wLEVEL, 
					  GLCharacter*			pChar, 
					  GLGaeaClient*			pGaeaClient, 
					  const D3DXVECTOR3&	vPos, 
					  SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARSPECANYBODY( const STARGETID&		sTargetID,
						const GLSKILL&			sSKILL,
						const WORD				wLEVEL,
						GLCharacter*			pChar,
						GLGaeaClient*			pGaeaClient,
						const D3DXVECTOR3&		vPos,
						SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARSPECFANWIZE( const STARGETID&		sTargetID,
						const GLSKILL&			sSKILL,
						const WORD				wLEVEL,
						GLCharacter*			pChar,
						GLGaeaClient*			pGaeaClient,
						const D3DXVECTOR3&		vPos, 
						SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARSELFTOSPEC( const STARGETID&		sTargetID, 
					   const GLSKILL&			sSKILL, 
					   const WORD				wLEVEL,
					   GLCharacter*				pChar, 
					   GLGaeaClient*			pGaeaClient, 
					   const D3DXVECTOR3&		vPos, 
					   SKILLTARGETDATA&			sSkillTarget );

BOOL SKT_TARZONEOURZONE( const STARGETID&		sTargetID,
						 const GLSKILL&			sSKILL,
						 const WORD				wLEVEL,
						 GLCharacter*			pChar,
						 GLGaeaClient*			pGaeaClient,
						 SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARZONEENEMYZONE( const STARGETID&		sTargetID, 
						  const GLSKILL&		sSKILL, 
						  const WORD			LEVEL,
						  GLCharacter*			pChar, 
						  GLGaeaClient*			pGaeaClient, 
						  const D3DXVECTOR3&	vPos, 
						  SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARZONEANYBODYZONE( const STARGETID&	TargetID,
							const GLSKILL&		sSKILL,
							const WORD			LEVEL,
							GLCharacter*		pChar,
							GLGaeaClient*		pGaeaClient,
							const D3DXVECTOR3&	vPos,
							SKILLTARGETDATA&	sSkillTarget );

BOOL SKT_TARZONEOURFANWISE( const D3DXVECTOR3&	vTarPos,
						   const GLSKILL&		sSKILL,
						   const WORD			wLEVEL,
						   GLCharacter*			pChar,
						   GLGaeaClient*		pGaeaClient,
						   const D3DXVECTOR3&	vPos, 
						   SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARZONEENEMYFANWISE( const STARGETID&		sTargetID,
							  const GLSKILL&		sSKILL, 
							  const WORD			wLEVEL, 
							  GLCharacter*			pChar, 
							  GLGaeaClient*			pGaeaClient,
							  const D3DXVECTOR3&	vPos,
							  SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARZONEANYBODYFANWISE( const D3DXVECTOR3&	vTarPos,
							   const GLSKILL&		sSKILL,
							   const WORD			wLEVEL,
							   GLCharacter*			pChar,
							   GLGaeaClient*		pGaeaClient,
							   const D3DXVECTOR3&	vPos,
							   SKILLTARGETDATA&	sSkillTarget );

BOOL SKT_SELFRANDOMZONE( const GLSKILL&			sSKILL,
						const WORD				wLEVEL,
						GLCharacter*			pChar, 
						const D3DXVECTOR3&		vPos, 
						SKILLTARGETDATA&		sSkillTarget );

BOOL SKT_TARLANDOURZONE( const STARGETID&		sTargetID,
						const GLSKILL&			sSKILL,
						const WORD				wLEVEL,
						GLCharacter*			pChar,
						GLGaeaClient*			pGaeaClient,
						SKILLTARGETDATA&		sSkillTarget );

TARDISTMAP		GLCharacter::ms_vecTAR_TEMP;

void SelectFromNearRange( IN TARDISTMAP& vecTAR_TEMP, 
						  OUT WORD &nTarget, 
						  IN const WORD wTARNUM, 
						  IN const WORD wMaxTarget, 
						  OUT STARID *sTARIDS  )
{
	//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
	//
	std::sort( vecTAR_TEMP.begin(), vecTAR_TEMP.end() );
	TARDISTMAP_ITER iter = vecTAR_TEMP.begin();
	TARDISTMAP_ITER iter_end = vecTAR_TEMP.end();
	for( WORD i=nTarget; (iter!=iter_end) && (i<wMaxTarget); ++iter, ++i )
	{
		if( wTARNUM <= nTarget )
			break;

		sTARIDS[nTarget].SET ( (*iter).m_sTARGETID );
		++nTarget;
	}
}

void AddTarget( GLCharacter* pChar, SKILLTARGETDATA& sSkillTarget )
{
	sSkillTarget.TargetIDs[sSkillTarget.nTarget].emCrow	= pChar->GETCROW();
	sSkillTarget.TargetIDs[sSkillTarget.nTarget].dwID	= pChar->GetGaeaID();
	++sSkillTarget.nTarget;
}

void AddTarget( const STARGETID& sTargetID, SKILLTARGETDATA& sSkillTarget )
{
	sSkillTarget.TargetIDs[sSkillTarget.nTarget].emCrow = sTargetID.emCrow;
	sSkillTarget.TargetIDs[sSkillTarget.nTarget].dwID	 = sTargetID.GaeaId;
	++sSkillTarget.nTarget;
}

void EraseVector( TARDISTMAP &vecTar )
{
	if( !vecTar.empty() )
		vecTar.erase ( vecTar.begin(), vecTar.end() );
}

VEC_SK_ITER_RANGE Equal_Range( VEC_SK_TAR *pDetectMap, EMCROW emCrow )
{
	return std::equal_range ( pDetectMap->begin(), pDetectMap->end(), emCrow, STAR_ORDER() );
};

const bool GLCharacter::SkillActivationInfoFanwise(
    IN const SKILLID& sSkillID, 
    IN const D3DXVECTOR3& vFromPt,
    IN const D3DXVECTOR3& vTargetPt,
    OUT D3DXVECTOR3& vOutDirection,
    OUT float& fOutRadius,
    OUT float& fOutDegree)
{
	GLSKILL* pSkill = GLSkillMan::GetInstance().GetData(sSkillID.mid(), sSkillID.sid());
	if (!pSkill)
		return false;

	float fRadius = static_cast<float>(GETBODYRADIUS() + GETSKILLRANGE_APPLY(*pSkill, sSkillID.wLEVEL));
	float fDegree = static_cast<float>(pSkill->m_sAPPLY.sDATA_LVL[sSkillID.wLEVEL].wAPPLYANGLE);

	fOutRadius = fRadius;
	fOutDegree = fDegree;

	//! ����üũ
	D3DXVECTOR3 vOrgPos = GetPosition();
	D3DXVECTOR3 vPos	= vOrgPos;
	vPos.y += (GetBodyHeight()*0.5f);

	D3DXPLANE plCollision;
	D3DXVECTOR3 vCollision;
	D3DXPlaneFromPointNormal( &plCollision, &vOrgPos, &D3DXVECTOR3(0.0f, 1.0f, 0.0f) );

	if ( D3DXPlaneIntersectLine(&vCollision, &plCollision, &vFromPt, &vTargetPt) )
	{
		vOutDirection = vCollision - vOrgPos;
		D3DXVec3Normalize(&vOutDirection, &vOutDirection);
	}
	else
	{
		vOutDirection = vTargetPt - vFromPt;
		D3DXVec3Normalize(&vOutDirection, &vOutDirection);
	}

	return true;
}

const bool GLCharacter::SkillActivationInfoCircle( IN const SKILLID& sSkillID, 
										   IN const D3DXVECTOR3& vFromPt,
										   IN const D3DXVECTOR3& vTargetPt,
										   OUT bool& bOutEnable, OUT D3DXVECTOR3& vOutPoint, OUT float& fOutRadius )
{
	bOutEnable = false;
	vOutPoint  = D3DXVECTOR3(0.0f,0.0f,0.0f);

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.mid(), sSkillID.sid() );

	if ( !pSkill ) 
	{
		return false;
	}

	float fDistance = static_cast<float>(GETBODYRADIUS() + GETSKILLRANGE_TAR(*pSkill));
	float fRadius   = static_cast<float>(pSkill->m_sAPPLY.sDATA_LVL[sSkillID.wLEVEL].wAPPLYRANGE);
	fOutRadius = fRadius;

	BOOL bColl = FALSE;
	DWORD dwCollID;

	ClientActor::IsCollision(vFromPt, vTargetPt, vOutPoint, dwCollID, bColl);

	if ( !bColl )
	{
		bOutEnable = false;

		GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();

		if ( pLandMan && pLandMan->GetLandMan() )
		{
			LPDXFRAME pDxFrame;
			DxLandMan* pDxLand = pLandMan->GetLandMan();
			pDxLand->IsCollision( vFromPt,  D3DXVECTOR3(vTargetPt), vOutPoint, bColl, pDxFrame, EMCC_CULL_CCW );
		}
	}
	else
	{
		bOutEnable = true;
	}

	if ( !bColl )
	{
		return false;
	}

	//! �Ÿ�üũ
	D3DXVECTOR3 vDistance = GetPosition() - vOutPoint;
	if ( fDistance < D3DXVec3Length(&vDistance) )
	{
		bOutEnable = false;
	}

	return true;
}

/**
 * 
 */
BOOL SelectSkillTarget(
					   GLGaeaClient* const pGaeaClient,
					   const SNATIVEID&		sSkillID,
					   const WORD				wSkillLevel,
					   const STARGETID&		sTargetID,
					   GLCharacter*			pChar,
					   const D3DXVECTOR3&		vPos,
					   SKILLTARGETDATA&		sSkillTarget )
{
	sSkillTarget.nTarget	= 0;
	sSkillTarget.TargetID	= sTargetID;

	PGLSKILL	pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
	if ( !pSkill )							
		return FALSE;

	switch( pSkill->m_sBASIC.emIMPACT_TAR )
	{
	case TAR_SELF:
		switch ( pSkill->m_sBASIC.emIMPACT_REALM )
		{
		case REALM_KEEP_ZONE:
			return FALSE;
		case REALM_SELF:
			// �ڽ��� Ÿ���̰� ��ȯ�� ��� �ڽ� ������ �������� ��ȯ�Ѵ�
			if ( pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_SUMMONS )
				SKT_SELFRANDOMZONE( *pSkill, wSkillLevel, pChar, vPos, sSkillTarget );
			else
			{
				AddTarget( pChar, sSkillTarget );
			}
			return TRUE;
		case REALM_ZONE:
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
			case EMIMPACT_SIDE_PARTY:	
				return SKT_SELFZONEOUR( *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
			case EMIMPACT_SIDE_ENEMY:
				return SKT_SELFZONEENEMY( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
			case EMIMPACT_SIDE_ANYBODY:
				return SKT_SELFZONANYBODY( *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
			};
			break;
		};
		break;
	case TAR_SPEC:
		{
			if( sTargetID.GaeaId == EMTARGET_NULL )
				return FALSE;
			
			switch ( pSkill->m_sBASIC.emIMPACT_REALM )
			{
			case REALM_KEEP_ZONE:	return FALSE;

			case REALM_SELF:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_PARTY:	
					return SKT_TARSPECOUR_SELF( sTargetID, pGaeaClient, sSkillTarget );
				case EMIMPACT_SIDE_ENEMY:
					return SKT_TARSPECENEMY_SELF( sTargetID, pGaeaClient, sSkillTarget );
				case EMIMPACT_SIDE_ANYBODY:
					return SKT_TARSPECANYBODY_SELF( sTargetID, sSkillTarget );
				};
				break;
			case REALM_ZONE:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_PARTY:	
					return SKT_TARSPECOUR( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
				case EMIMPACT_SIDE_ENEMY:
					return SKT_TARSPECENEMY( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
				case EMIMPACT_SIDE_ANYBODY:
					return SKT_TARSPECANYBODY( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
				};
				break;

			case REALM_FANWIZE:
				switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
				{
				case EMIMPACT_SIDE_PARTY:	
				case EMIMPACT_SIDE_ANYBODY:
					return FALSE;
				case EMIMPACT_SIDE_ENEMY:
					return SKT_TARSPECFANWIZE( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
				};
				break;
			};	
		}
		break;

	case TAR_SELF_TOSPEC:
		switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
		{
		case EMIMPACT_SIDE_PARTY:	
		case EMIMPACT_SIDE_ANYBODY:
			return FALSE;
		case EMIMPACT_SIDE_ENEMY:
			return SKT_TARSELFTOSPEC( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
		};
		break;
	case TAR_ZONE:
		switch ( pSkill->m_sBASIC.emIMPACT_REALM )
		{
		case REALM_ZONE:
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
			case EMIMPACT_SIDE_PARTY:	
				return SKT_TARZONEOURZONE( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, sSkillTarget );
			case EMIMPACT_SIDE_ENEMY:
				return SKT_TARZONEENEMYZONE( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
			case EMIMPACT_SIDE_ANYBODY:
				return SKT_TARZONEANYBODYZONE( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
			};
			break;
		case REALM_FANWIZE:
			switch ( pSkill->m_sBASIC.emIMPACT_SIDE )
			{
			case EMIMPACT_SIDE_PARTY:
				return SKT_TARZONEOURFANWISE( sTargetID.vPos, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
			case EMIMPACT_SIDE_ENEMY:
				return SKT_TARZONEENEMYFANWISE( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
			case EMIMPACT_SIDE_ANYBODY:
				return SKT_TARZONEANYBODYFANWISE( sTargetID.vPos, *pSkill, wSkillLevel, pChar, pGaeaClient, vPos, sSkillTarget );
			};
			break;
		};

	case TAR_LAND:
		boost::shared_ptr<NavigationMesh> spNavi = pGaeaClient->GetActiveMap()->GetNaviMesh();
		if ( !spNavi )
			return FALSE;

		D3DXVECTOR3		vCollision;
		DWORD			dwCollID;
		BOOL			bColl;
		spNavi->IsCollision( sTargetID.vPos + D3DXVECTOR3( 0.0f, 10.0f, 0.0f ), sTargetID.vPos + D3DXVECTOR3( 0.0f, -10.0f, 0.0f ), vCollision, dwCollID, bColl );
		if ( bColl == FALSE )
			return FALSE;

		sSkillTarget.TargetID.vPos = vCollision;		
		return SKT_TARLANDOURZONE( sTargetID, *pSkill, wSkillLevel, pChar, pGaeaClient, sSkillTarget );
	};

	return FALSE;
} //SelectSkillTarget

/**
 * �ڱ����� ��ų üũ( �ڱ� ��ġ�� ���� )
 */
BOOL SKT_SELFZONEOUR( const GLSKILL&		sSKILL, 
					  const WORD			wLEVEL,
					  GLCharacter*			pChar,
					  GLGaeaClient*			pGaeaClient,
					  const D3DXVECTOR3&	vPos,					//GetPosition()
					  SKILLTARGETDATA&		sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	// �⺻ Ÿ�������� �߰��Ѵ� ( �ڱ��ڽ� );
	AddTarget( pChar, sSkillTarget );

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM <= sSkillTarget.nTarget )	
		return TRUE;

	// �߰� Ÿ������ �˻��Ѵ� ( �ڱ�� ���� �Ѽ� );
	VEC_SK_ITER_RANGE	pairRange;
	float				fAttackRange	= pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;
	//float				fAttackRange	= ::pow(pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2.f, 2.f);
	DWORD				emCrow			= ( CROW_EX_PC );

	BOOL	bApply_ValidTarget = !sSKILL.IsRebirth();

	VEC_SK_TAR *pDetectMap = bApply_ValidTarget == TRUE ? 
		pGaeaClient->DetectCrow ( emCrow, vPos, fAttackRange ) : pGaeaClient->DetectCrowAll ( emCrow, vPos, fAttackRange );

	if ( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER iter;

		// ���� ĳ���Ϳ��Ը� ��ų�� �� �� �ִ�;
		// ĳ���͸� �����ϰ�� ���� ���� �� �� ����;
		// ��ȯ���� ��쵵 ĳ���� �����μ� �Ǻ��Ѵ�;
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);

			if ( pChar->IsReActionable( sFindID, EMIMPACT_SIDE_PARTY ) == FALSE )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		// �����ͺ��� ������ ������ŭ �̾Ƴ���;
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;

			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if( sSkillTarget.nTarget == 0 )
		return FALSE;

	return TRUE;

/*
	//	Note : ��Ƽ���� �߿� Skill ������ ������ ��� �˻�.
	//
	PLANDMANCLIENT pLand = pGaeaClient->GetActiveMap();
	if( !pLand )
		return TRUE;
	
	const float fDetectRange = ::pow(pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2.0f, 2.0f);

	EraseVector( GLCharacter::ms_vecTAR_TEMP );

	GLPartyClient* const pPartyClient = pGaeaClient->GetMyPartyClient();	
	for ( DWORD dwIndex = 0; dwIndex < pPartyClient->GetNMember(); dwIndex++ )
	{
		const GLPartySlotClient* const pMember = pPartyClient->GetMember(dwIndex);
		if ( pMember == NULL )
			continue;

		std::tr1::shared_ptr<GLCharClient> pMemberChar = pLand->GetChar ( pMember->m_dwGaeaID );
		if( !pMember->isOneSelf(pGaeaClient) && pMemberChar )
		{
			bool bValidBody = !sSKILL.IsRebirth();							// ��Ȱ�϶��� ���� ��� üũ
			bool bIsValid	= (0 != pMemberChar->IsValidBody());			// ���������
			if ( bIsValid == bValidBody )
			{
				const D3DXVECTOR3& vMemPos = pMember->m_vPosition;

				const float fLength = D3DXVec3LengthSq(&D3DXVECTOR3( vPos - vMemPos));
				if ( fLength <= fDetectRange )
				{
					STARGETID sFindID ( pMemberChar->GETCROW(), pMemberChar->GetGaeaID(), vMemPos );
					GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
				}
			}
		} //if
	} //for

	SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs  );

	return TRUE;*/
} //SKT_SELFZONEOUR

/**
 * ��ų�� ���� ������ ������ ��(PC/Mob) check
 * ���鿡�� ��ų üũ ( �ڱ� ��ġ�� ���� )
 */
BOOL SKT_SELFZONEENEMY( const STARGETID&		sTargetID, 
						const GLSKILL&			sSKILL, 
						const WORD				wLEVEL,
					    GLCharacter*			pChar,
					    GLGaeaClient*			pGaeaClient,
						const D3DXVECTOR3&		vPos,			//GetPosition()
						OUT SKILLTARGETDATA&	sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL&		sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	WORD wRange = sSKILL_DATA.wAPPLYRANGE;

	/*if( RF_PROVOKE(pChar->m_EffSkillVarSet).IsOn() )
	{
		wRange = RF_PROVOKE(pChar->m_EffSkillVarSet).GetEffDistance();
	}*/
	//	Crow �˻�.
	VEC_SK_ITER_RANGE		pairRange;
	float				fAttackRange	= pChar->GETBODYRADIUS() + static_cast<float>(wRange) + 2;
	DWORD				emCrow			= ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	VEC_SK_TAR *pDetectMap = pGaeaClient->DetectCrow ( emCrow, vPos, fAttackRange );
	if ( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

        // �ڽ��ֺ� ���� ��������ϴ� ��ų�� Ÿ���� �־�� ��ų����� ����;
        // 
        // �ش�Ÿ���� ������ �ֺ� ����� �˻��Ѵ�.;
        // �ֺ�Ÿ�� �˻� �� �ش� Ÿ���� ���ݰ������� �˻� �� ��;
        // �ش�Ÿ���� ���� ������ ��� �ش�Ÿ���� �켱������ ����Ѵ�.;
        // (Ÿ������ ���� ���� �ڽ��ֺ����ݽ�ų�� �켱���� ���� ����� �ȴ�.);
        // ���� ������������ Ÿ���� �����ϸ� �ش� Ÿ�ٸ� ���� ������ ����� �ȴ�.;
		
        //	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);			
			
			ClientActor* pTARGET = pGaeaClient->GetCopyActor(sFindID);

            //��ȹ���䱸���� (�������ݽ� Ÿ���õ� ���� �������� �� �� �ְ� �ش޶�� ��);
            //Ÿ�ٿ� ���� ��ȿ���˻�� �������ݻ������� �ƴ����� �Ǻ��Ѵ�.;
            if ( sTargetID == sFindID )
            {
                if ( pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY) == FALSE )
                    continue;
            }
            //Ÿ�� ���� �ڽ� �ֺ� Ÿ�ٿ� ���� ��ȿ�� �˻�� �������� ���¸� �˻����� �ʴ´�.;
            else if ( pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) == FALSE )
            {
				continue;
            }

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	MOB.
		pairRange = Equal_Range( pDetectMap, CROW_MOB );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() ) 
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	SUMMON.
		pairRange = Equal_Range( pDetectMap, CROW_SUMMON );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() )
				continue;

			//��ȹ���䱸���� (�������ݽ� Ÿ���õ� ���� �������� �� �� �ְ� �ش޶�� ��);
			//Ÿ�ٿ� ���� ��ȿ���˻�� �������ݻ������� �ƴ����� �Ǻ��Ѵ�.;
			if ( sTargetID == sFindID )
			{
				if ( FALSE == pChar->IsReActionable( sFindID, EMIMPACT_SIDE_ENEMY ) )
					continue;
			}
			//Ÿ�� ���� �ڽ� �ֺ� Ÿ�ٿ� ���� ��ȿ�� �˻�� �������� ���¸� �˻����� �ʴ´�.;
			else if ( FALSE == pChar->IsReActionable( sFindID, EMIMPACT_SIDE_ENEMY, false ) )
			{
				continue;
			}

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if( sSkillTarget.nTarget == 0 )
		return FALSE;

	return TRUE;
} //SKT_SELFZONEENEMY

/**
 * ��ų�� ���� ������ ������ PC�� check
 * ��ο��� ��ų üũ ( �ڱ� ��ġ�� ���� )
 */
BOOL SKT_SELFZONANYBODY( const GLSKILL&			sSKILL, 
						 const WORD				wLEVEL,
					     GLCharacter*			pChar,
					     GLGaeaClient*			pGaeaClient,
						 const D3DXVECTOR3&		vPos,				//GetPosition()
						 SKILLTARGETDATA&		sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	//	Crow �˻�.
	float	fAttackRange = pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2.0f;

	AddTarget( pChar, sSkillTarget );

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if ( wTARNUM <= sSkillTarget.nTarget )
		return TRUE;

	VEC_SK_ITER_RANGE			pairRange;
	VEC_SK_TAR *pDetectMap = pGaeaClient->DetectCrow ( CROW_EX_PC, vPos, fAttackRange );
	if( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( !pChar->IsReActionable(sFindID,EMIMPACT_SIDE_ANYBODY) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3( vPos - sFindID.vPos ) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
	}

	if( sSkillTarget.nTarget == 0 )
		return FALSE;

	return TRUE;
} //SKT_SELFZONANYBODY

/**
 * ��ų�� ���� ������ ������ ��Ƽ��� check
 * �ڱ����� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
 */
BOOL SKT_TARSPECOUR_SELF( const STARGETID&			sTargetID, 
						  GLGaeaClient*				pGaeaClient,
						  SKILLTARGETDATA&			sSkillTarget )
{
	// Ÿ���� ���� �� ���� ( ���� ������ ��ų�� �ɾ����� ���Ѵ� );
	if( sTargetID.emCrow == CROW_MOB )
		return FALSE;

	//	ENFIND_TAR_OUR
	//	�ڽ��� �ƴ϶��....
	//	Ÿ�� ��� �����ҷ��� �̰͵� �����ؾߵȴ�.
//	if ( sTargetID.dwID != m_dwGaeaID )
//	{
		/*GLPartySlotClient* pParty = pGaeaClient->GetMyPartyClient()->GetMemberByGaeaID(sTargetID.GaeaId);
		if (!pParty)
            return FALSE;*/
//	}

	// �ڱ� �ڽ��� ���Ѵ�;
	GLCharacter* pChar = pGaeaClient->GetCharacter();
	if ( NULL == pChar )
		return FALSE;

	// Ÿ���� ���� �Ѽ����� üũ�Ѵ�;
	if ( !pChar->IsReActionable( sTargetID, EMIMPACT_SIDE_PARTY ) )
		return FALSE;

	// Ÿ���� �߰��Ѵ�;
	AddTarget( sTargetID, sSkillTarget );

	return TRUE;
} //SKT_TARSPECOUR_SELF

/**
 * Ÿ�ٸ� �߰�
 * ���鿡�� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
 */
BOOL SKT_TARSPECENEMY_SELF( const STARGETID&		sTargetID, 
						    GLGaeaClient*			pGaeaClient,
							SKILLTARGETDATA&		sSkillTarget )
{
	//	Note : ���� �������� �������� üũ�Ѵ�
	//
	ClientActor *pTARGET = pGaeaClient->GetCopyActor(sTargetID);
	if( !pTARGET || pTARGET->IsNonDamaged() )
		return FALSE;

	AddTarget( sTargetID, sSkillTarget );

	return TRUE;
} //SKT_TARSPECENEMY_SELF

/**
 * ��ų�� ���� ������ ������ PC check
 * ��ο��� ��ų üũ ( ��� ��ġ�� ���� )�̰� �ڽ� ����
 */
BOOL SKT_TARSPECANYBODY_SELF( const STARGETID &sTargetID, SKILLTARGETDATA& sSkillTarget )
{
	if( sTargetID.emCrow == CROW_MOB || sTargetID.emCrow == CROW_SUMMON )
		return FALSE;

	AddTarget( sTargetID, sSkillTarget );

	return TRUE;
} //SKT_TARSPECANYBODY_SELF

/**
 * ��ų�� ���� ������ ������ ��Ƽ����� check
 * �ڱ����� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
 */
BOOL SKT_TARSPECOUR( const STARGETID&		sTargetID, 
					 const GLSKILL&			sSKILL, 
					 const WORD				wLEVEL,
				     GLCharacter*			pChar,
				     GLGaeaClient*			pGaeaClient,
					 const D3DXVECTOR3&		vPos, 
					 SKILLTARGETDATA&		sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL&	 sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	//	Crow �˻�.
	float			fAttackRange = pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;
	D3DXVECTOR3		vFindPos;

	//	�ڱ� �ڽ��� Ÿ���̸� ������ش�. �ƴϸ� �ؿ��� üũ�Ѵ�.
	if( sTargetID.GaeaId == pChar->GetGaeaID() ) 
	{
		AddTarget( pChar, sSkillTarget );
		vFindPos = vPos;
	}
	else 
	{
		if ( !pGaeaClient->ValidCheckTarget( sTargetID ) )
			return FALSE;

		vFindPos = pGaeaClient->GetTargetPos( sTargetID );
	} //if..else

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if( wTARNUM <= sSkillTarget.nTarget )
		return TRUE;

	VEC_SK_ITER_RANGE		pairRange;
	VEC_SK_TAR*			pDetectMap = pGaeaClient->DetectCrow ( CROW_EX_PC, vFindPos, fAttackRange );
	if ( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( !pChar->IsReActionable( sFindID, EMIMPACT_SIDE_PARTY ) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	�ڱ��ڽ� ���
		float fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vPos));
		if ( fLength <= fAttackRange )
		{
			STARGETID sFindID ( pChar->GETCROW(), pChar->GetGaeaID(), vPos );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
		}


		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if ( sSkillTarget.nTarget==0 )
		return FALSE;

	return TRUE;

	/*const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	if( !pGaeaClient->ValidCheckTarget(sTargetID) )
		return FALSE;

	const WORD wTARNUM = sSKILL_DATA.wTARNUM;
	D3DXVECTOR3		vFindPos	= pGaeaClient->GetTargetPos(sTargetID);

	//	Note : ��Ƽ���� �߿� Skill ������ ������ �ɹ��� �˻�.(�ڱ��ڽ��� ����)
	//
	PLANDMANCLIENT	pLand		= pGaeaClient->GetActiveMap();
	if( !pLand )
		return TRUE;

	float			fDetectRange = pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	EraseVector( GLCharacter::ms_vecTAR_TEMP );
	
	GLPartyClient* const pPartyClient = pGaeaClient->GetMyPartyClient();	
	if ( pPartyClient->isValid() == true )
	{
		for ( DWORD dwIndex = 0; dwIndex < pPartyClient->GetNMember(); ++dwIndex )
		{			
			const GLPartySlotClient* const pMember = pPartyClient->GetMember(dwIndex);
			if ( pMember == NULL )
				continue;

			const DWORD dwGaeaID = pMember->m_dwGaeaID;
			ClientActor* pPartyChar = pGaeaClient->GetCopyActor(CROW_PC, dwGaeaID);			 

			if ( pPartyChar )
			{
				const D3DXVECTOR3& vPosition = pPartyChar->GetPosition();
				float fLength = D3DXVec3Length ( &D3DXVECTOR3( vFindPos - vPosition ) );
				if ( fLength <= fDetectRange )
				{
					STARGETID sFindID ( CROW_PC, dwGaeaID, vPosition );
					GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
				}
			}			
		}		
	}
	else
	{
		AddTarget( sTargetID, sSkillTarget );
		
		if( wTARNUM <= sSkillTarget.nTarget )
			return TRUE;
	}	
	
	SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );

	return TRUE;*/
} //SKT_TARSPECOUR

/**
 * ��ų�� ���� ������ ������ ���� Check (������ Ÿ���� �־�� �ߵ���)
 * ���鿡�� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
 */
BOOL SKT_TARSPECENEMY( const STARGETID&			sTargetID, 
					   const GLSKILL&			sSKILL, 
					   const WORD				wLEVEL,
					   GLCharacter*				pChar,
					   GLGaeaClient*			pGaeaClient,
					   const D3DXVECTOR3&		vPos,
					   SKILLTARGETDATA&			sSkillTarget )
{
	const SKILL::CDATA_LVL&		sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	// Ÿ���� ��ȿ�� �˻�
	if ( !pGaeaClient->ValidCheckTarget(sTargetID) )
		return FALSE;

	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	WORD wRange = sSKILL_DATA.wAPPLYRANGE;

	VEC_SK_ITER_RANGE		pairRange;
	D3DXVECTOR3			vFindPos	 = pGaeaClient->GetTargetPos( sTargetID );
	float				fAttackRange = pChar->GETBODYRADIUS() + static_cast<float>(wRange) + 2;
	DWORD				emCrow		 = ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	VEC_SK_TAR*			pDetectMap   = pGaeaClient->DetectCrow ( emCrow, vFindPos, fAttackRange );
	if( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			ClientActor* pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( (sTargetID != sFindID) && !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq( &D3DXVECTOR3( vFindPos - sFindID.vPos) );

			if( RF_PROVOKE( pChar->m_EffSkillVarSet ).IsOn() && sFindID == sTargetID )	fLengthSq = 0;

			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	MOB.
		pairRange = Equal_Range( pDetectMap, CROW_MOB);
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);
			
			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() || !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	SUMMON.
		pairRange = Equal_Range( pDetectMap, CROW_SUMMON );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);
			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() || !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if ( sSkillTarget.nTarget == 0 )
		return FALSE;

	return TRUE;
} //SKT_TARSPECENEMY

/**
 * ��ų�� ���� ������ ������ PC check (������ Ÿ���� �־�� �ߵ���)
 * ��ο��� ��ų üũ ( ��� ��ġ�� ���� )�̰� ��ǥ ����
 */
BOOL SKT_TARSPECANYBODY( const STARGETID&		sTargetID, 
						 const GLSKILL&			sSKILL, 
						 const WORD				wLEVEL,
					     GLCharacter*			pChar,
					     GLGaeaClient*			pGaeaClient,
						 const D3DXVECTOR3&		vPos,
						 SKILLTARGETDATA&		sSkillTarget )
{
	const SKILL::CDATA_LVL&		sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	//	Crow �˻�.
	float			fAttackRange = pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;
	D3DXVECTOR3		vFindPos;

	//	�ڱ� �ڽ��� Ÿ���̸� ������ش�. �ƴϸ� �ؿ��� üũ�Ѵ�.
	if( sTargetID.GaeaId == pChar->GetGaeaID() ) 
	{
		AddTarget( pChar, sSkillTarget );
		vFindPos = vPos;
	}
	else 
	{
		if ( !pGaeaClient->ValidCheckTarget( sTargetID) )
			return FALSE;

		vFindPos = pGaeaClient->GetTargetPos( sTargetID );
	} //if..else

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if( wTARNUM <= sSkillTarget.nTarget )
		return TRUE;

	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	VEC_SK_ITER_RANGE		pairRange;
	VEC_SK_TAR*			pDetectMap = pGaeaClient->DetectCrow ( CROW_EX_PC, vFindPos, fAttackRange );
	if( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( !pChar->IsReActionable(sFindID,EMIMPACT_SIDE_ANYBODY) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	�ڱ��ڽ� ���
		float fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vPos));
		if ( fLength <= fAttackRange )
		{
			STARGETID sFindID ( pChar->GETCROW(), pChar->GetGaeaID(), vPos );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
		}


		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if ( sSkillTarget.nTarget==0 )
		return FALSE;

	return TRUE;
} //SKT_TARSPECANYBODY

/**
 * ��ų�� ���� ������ ������ �� check (ù Ÿ���� �ݰ泻�� ����)
 */
BOOL SKT_TARSPECFANWIZE( const STARGETID&		sTargetID, 
						 const GLSKILL&			sSKILL, 
						 const WORD				wLEVEL,
					     GLCharacter*			pChar,
					     GLGaeaClient*			pGaeaClient,
						 const D3DXVECTOR3&		vPos,
						 SKILLTARGETDATA&		sSkillTarget )
{
	const SKILL::CDATA_LVL&		sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	if( !pGaeaClient->ValidCheckTarget(sTargetID) )
		return FALSE;

	D3DXVECTOR3		vTarPosition = pGaeaClient->GetTargetPos(sTargetID);
	D3DXVECTOR3		vTarOrgDir	 = vTarPosition - vPos;
	ClientActor*	pTarget		 = pGaeaClient->GetCopyActor ( sTargetID.emCrow, sTargetID.GaeaId );
	D3DXVECTOR3		vFindPos	 = vPos;
	if ( !pTarget )
		return FALSE;

	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	//	ù ǥ�� ����.
	sSkillTarget.nTarget = 0;
	sSkillTarget.TargetIDs[sSkillTarget.nTarget].SET( sTargetID );
	sSkillTarget.nTarget++;

	float fApplyAngle = D3DXToRadian( sSKILL_DATA.wAPPLYANGLE ) / 2.0f;

	//	Note : ��Ÿ��� ��ų�� ��� GETSUM_TARRANGE() ����.
	float fAttackRange = (float) ( pTarget->GetBodyRadius() + pChar->GETBODYRADIUS() + pChar->GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );

	VEC_SK_ITER_RANGE		pairRange;
	DWORD				emCrow		= ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	VEC_SK_TAR*			pDetectMap	= pGaeaClient->DetectCrow ( emCrow, vFindPos, fAttackRange );
	if ( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for( ; (iter != pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if( sFindID==sTargetID )
				continue;
			ClientActor* pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if( (sTargetID != sFindID) && !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) )
				continue;		

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3( vPos - sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		} //for

		//	MOB.
		pairRange = Equal_Range( pDetectMap, CROW_MOB );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);
			if ( sFindID==sTargetID )		continue;
			
			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() ) continue;

			D3DXVECTOR3 vTarDir = sFindID.vPos - vPos;

			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
			if ( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle )
			{
				// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
				if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
					continue;

				float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
				GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
			}
		}

		//	SUMMON.
		pairRange = Equal_Range( pDetectMap, CROW_SUMMON );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);
			if ( sFindID==sTargetID )		continue;

			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() || !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) ) continue;

			D3DXVECTOR3 vTarDir = sFindID.vPos - vPos;

			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
			if ( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle )
			{
				// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
				if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
					continue;

				float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
				GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
			}
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if ( sSkillTarget.nTarget==0 )
		return FALSE;

	return TRUE;
} //SKT_TARSPECFANWIZE

/**
 * ��ų�� ���� ������ ������ �� check (���뽺ų�� ����)
 */
BOOL SKT_TARSELFTOSPEC( const STARGETID&		sTargetID, 
						const GLSKILL&			sSKILL, 
						const WORD				wLEVEL,
					    GLCharacter*			pChar,
					    GLGaeaClient*			pGaeaClient,
						const D3DXVECTOR3&		vPos,
						SKILLTARGETDATA&		sSkillTarget )
{
	const SKILL::CDATA_LVL&			sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	//	�ڽſ��� ��ǥ���� ���� ������ ��� ���� ���.
	ClientActor*			pTarget = pGaeaClient->GetCopyActor( sTargetID.emCrow, sTargetID.GaeaId );
	if ( !pTarget )
		return FALSE;

	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	D3DXVECTOR3			vTarget		= sTargetID.vPos;	
	D3DXVECTOR3			vFrom		= vPos;

	vTarget.y	+= pTarget->GetBodyHeight() * 0.75f;	
	vFrom.y		+= pChar->GetBodyHeight() * 0.75f;

	D3DXVECTOR3			vDir		= vTarget - vFrom;
	D3DXVec3Normalize( &vDir, &vDir );

	//	Note : ��Ÿ��� ��ų�� ��� GETSUM_TARRANGE() ����.
	float				fAttackRange = (float)( pTarget->GetBodyRadius() + pChar->GETBODYRADIUS() + pChar->GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );
	vTarget = vFrom + vDir * fAttackRange;

	//	Crow �˻�.
	VEC_SK_ITER_RANGE		pairRange;
	DWORD				emCrow = ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	
	VEC_SK_TAR*			pDetectMap;
	if( sSKILL_DATA.wAPPLYRANGE > 0 )
		pDetectMap = pGaeaClient->DetectCrowWithCylinder( emCrow, vPos, sTargetID.vPos, sSKILL_DATA.wAPPLYRANGE * 0.5f );
	else
		pDetectMap = pGaeaClient->DetectCrow( emCrow, vFrom, vTarget );

	if( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		for( iter = pairRange.first; iter != pairRange.second; ++iter )
		{
			const STARGETID&	sFindID = (*iter);
			ClientActor*		pTARGET = pGaeaClient->GetCopyActor( sFindID );
			if( (sTargetID != sFindID) && !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
				continue;

			float				fLengthSq = D3DXVec3LengthSq( &D3DXVECTOR3( vPos - sFindID.vPos ) );
			GLCharacter::ms_vecTAR_TEMP.push_back( STARDIST(fLengthSq,sFindID) );
		} //for

		//	MOB.
		pairRange = Equal_Range( pDetectMap, CROW_MOB );
		for( iter = pairRange.first; iter != pairRange.second; ++iter )
		{
			const STARGETID&		sFindID = (*iter);
			ClientActor*	pTARGET = pGaeaClient->GetCopyActor( sFindID );
			if( !pTARGET || pTARGET->IsNonDamaged() )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFrom, sFindID.vPos, 5.f ) )
				continue;

			float	fLengthSq = D3DXVec3LengthSq( &D3DXVECTOR3( vFrom-sFindID.vPos ) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST( fLengthSq, sFindID ) );
		} //for

		//	SUMMON.
		pairRange = Equal_Range( pDetectMap, CROW_SUMMON );
		for( iter = pairRange.first; iter != pairRange.second; ++iter )
		{
			const STARGETID&		sFindID = (*iter);

			ClientActor*	pTARGET = pGaeaClient->GetCopyActor( sFindID );
			if( !pTARGET || pTARGET->IsNonDamaged() || !pChar->IsReActionable( sFindID, EMIMPACT_SIDE_ENEMY, false ) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFrom, sFindID.vPos, 5.f ) )
				continue;

			float	fLengthSq = D3DXVec3LengthSq( &D3DXVECTOR3( vFrom-sFindID.vPos ) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST( fLengthSq,sFindID ) );
		} //for

		WORD	wPIERCENUM = sSKILL_DATA.wPIERCENUM + pChar->GETSUM_PIERCE();
		SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wPIERCENUM, wPIERCENUM, sSkillTarget.TargetIDs  );
	} //if

	if( sSkillTarget.nTarget == 0 )
		return FALSE;

	return TRUE;
} //SKT_TARSELFTOSPEC

/**
 * ��ų�� ���� ������ ������ ��Ƽ�� check
 */
BOOL SKT_TARZONEOURZONE( const STARGETID&		sTargetID, 
						 const GLSKILL&			sSKILL, 
						 const WORD				wLEVEL,
						 GLCharacter*			pChar,
						 GLGaeaClient*			pGaeaClient,
						 SKILLTARGETDATA&		sSkillTarget )
{
	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	AddTarget( pChar, sSkillTarget );

	WORD		wTARNUM = sSKILL_DATA.wTARNUM;
	if( wTARNUM <= sSkillTarget.nTarget )	
		return TRUE;

	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const D3DXVECTOR3& vFindPos = sTargetID.vPos;

	//	Crow �˻�.
	float fAttackRange = pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	VEC_SK_ITER_RANGE pairRange;	
	VEC_SK_TAR *pDetectMap = pGaeaClient->DetectCrow( CROW_EX_PC, vFindPos, fAttackRange );
	if ( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for( ; iter != pairRange.second; ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( FALSE == pChar->IsReActionable( sFindID, EMIMPACT_SIDE_PARTY ) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if( sSkillTarget.nTarget == 0 )
		return FALSE;

	return TRUE;

	/*const SKILL::CDATA_LVL&		sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];
	AddTarget( pChar, sSkillTarget );

	WORD		wTARNUM = sSKILL_DATA.wTARNUM;
	if( wTARNUM <= sSkillTarget.nTarget )	
		return TRUE;

	const D3DXVECTOR3& vFindPos = sTargetID.vPos;

	WORD		TARNUM = sSKILL_DATA.wTARNUM;
	if( wTARNUM <= sSkillTarget.nTarget )
		return TRUE;

	// ��Ƽ���� �߿� Skill ������ ������ ��� �˻�.
	PLANDMANCLIENT pLand = pGaeaClient->GetActiveMap();
	if ( !pLand )
		return TRUE;

	EraseVector( GLCharacter::ms_vecTAR_TEMP );

	float					fDetectRange = pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	GLPartyClient* const pPartyClient = pGaeaClient->GetMyPartyClient();
	for( DWORD dwIndex = 0; dwIndex < pPartyClient->GetNMember(); dwIndex++ )
	{
		const GLPartySlotClient* const pMember = pPartyClient->GetMember(dwIndex);
		if ( pMember == NULL )
			continue;

		std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar ( pMember->m_dwGaeaID );

		if ( !pMember->isOneSelf(pGaeaClient) && pChar )
		{
			const D3DXVECTOR3& vMemPos = pMember->m_vPosition;

			float fLength = D3DXVec3Length ( &D3DXVECTOR3(vFindPos-vMemPos) );
			if ( fLength <= fDetectRange )
			{
				STARGETID sFindID ( pChar->GETCROW(), pChar->GetGaeaID(), vMemPos );
				GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
			}
		}
	}
	
	SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );

	return TRUE;*/
} //SKT_TARZONEOURZONE

/**
 * ��ų�� ���� ������ ������ ��(PC/MOB) check (Ÿ�� �˻� ����)
 */
BOOL SKT_TARZONEENEMYZONE( const STARGETID&			sTargetID, 
						   const GLSKILL&			sSKILL, 
						   const WORD				wLEVEL,
						   GLCharacter*				pChar,
						   GLGaeaClient*			pGaeaClient,
						   const D3DXVECTOR3&		vPos,
						   SKILLTARGETDATA&			sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	const D3DXVECTOR3& vFindPos = sTargetID.vPos;

	WORD wRange = sSKILL_DATA.wAPPLYRANGE;

	/*
	if( RF_PROVOKE( pChar->m_EffSkillVarSet ).IsOn() ) 
	{
		wRange = RF_PROVOKE( pChar->m_EffSkillVarSet ).GetEffDistance();
	}*/

	//	Crow �˻�.
	VEC_SK_ITER_RANGE		pairRange;
	DWORD				emCrow = ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	VEC_SK_TAR*			pDetectMap = pGaeaClient->DetectCrow ( emCrow, vFindPos, static_cast<float>(wRange) );
	if( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID&	sFindID = (*iter);
			ClientActor*		pTARGET = pGaeaClient->GetCopyActor(sFindID);

			// ������ Ÿ������ ������ ���� Ÿ���� ��˻��ϴ� ��찡 �ִ�;
			if( (sTargetID != sFindID) && !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3( vPos - sFindID.vPos ) );
			GLCharacter::ms_vecTAR_TEMP.push_back( STARDIST(fLengthSq,sFindID) );
		}

		//	MOB.
		pairRange = Equal_Range( pDetectMap, CROW_MOB );
		iter = pairRange.first;
		for( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	SUMMON.
		pairRange = Equal_Range( pDetectMap, CROW_SUMMON );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() || !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if ( sSkillTarget.nTarget==0 )
		return FALSE;

	return TRUE;
} //SKT_TARZONEENEMYZONE

/**
 * ��ų�� ���� ������ ������ PC check (Ÿ�� �˻� ����)
 */
BOOL SKT_TARZONEANYBODYZONE( const STARGETID&		sTargetID, 
							 const GLSKILL&			sSKILL, 
							 const WORD				wLEVEL,
							 GLCharacter*			pChar,
							 GLGaeaClient*			pGaeaClient,
							 const D3DXVECTOR3&		vPos,
							 OUT SKILLTARGETDATA&	sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	const D3DXVECTOR3& vFindPos = sTargetID.vPos;

	//	Crow �˻�.
	float fAttackRange = pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2;

	VEC_SK_ITER_RANGE pairRange;	
	VEC_SK_TAR *pDetectMap = pGaeaClient->DetectCrow( CROW_EX_PC, vFindPos, fAttackRange );
	if ( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for( ; iter != pairRange.second; ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( FALSE == pChar->IsReActionable(sFindID,EMIMPACT_SIDE_ANYBODY) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( vFindPos, sFindID.vPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vFindPos-sFindID.vPos) );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if( sSkillTarget.nTarget == 0 )
		return FALSE;

	return TRUE;
}

/**
 * ��ų�� ���� ������ ������ ��Ƽ�� check
 */
BOOL SKT_TARZONEOURFANWISE( const D3DXVECTOR3&		vTarPos, 
							const GLSKILL&			sSKILL, 
							const WORD				wLEVEL,
							GLCharacter*			pChar,
							GLGaeaClient*			pGaeaClient,
							const D3DXVECTOR3&		vPos,
							SKILLTARGETDATA&		sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	AddTarget( pChar, sSkillTarget );

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if( wTARNUM <= sSkillTarget.nTarget )	
		return TRUE;

	//	Crow �˻�.
	float fApplyAngle  = D3DXToRadian(sSKILL_DATA.wAPPLYANGLE) / 2.0f;
	float fAttackRange = (float)( pChar->GETBODYRADIUS() + pChar->GETSKILLRANGE_APPLY( sSKILL, wLEVEL ) + 2 );

	const D3DXVECTOR3 vTarOrgDir(vTarPos - vPos);

	EraseVector( GLCharacter::ms_vecTAR_TEMP );

	VEC_SK_ITER_RANGE			pairRange;	
	VEC_SK_TAR*				pDetectMap = pGaeaClient->DetectCrow( CROW_EX_PC, vPos, fAttackRange );
	if ( pDetectMap )
	{
		VEC_SK_ITER iterDetect;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iterDetect = pairRange.first;
		for ( ; (iterDetect!=pairRange.second); ++iterDetect )
		{
			const STARGETID &sFindID = (*iterDetect);
			if ( FALSE == pChar->IsReActionable( sFindID, EMIMPACT_SIDE_PARTY ) )
				continue;

			D3DXVECTOR3 vTarDir = sFindID.vPos - vPos;
			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
			if ( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle )
			{
				// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
				if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
					continue;

				float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3( vPos - sFindID.vPos ) );
				GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
			}
		}

		// �����ͺ��� ������ ������ŭ �̾Ƴ���.
		WORD wTARNUM = sSKILL_DATA.wTARNUM;
		SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
	} //if

	if ( sSkillTarget.nTarget==0 )
		return FALSE;

	return TRUE;

	/*const SKILL::CDATA_LVL&		sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	AddTarget( pChar, sSkillTarget );

	WORD wTARNUM = sSKILL_DATA.wTARNUM;
	if( wTARNUM <= sSkillTarget.nTarget )	
		return TRUE;
	
	//	Note : ��Ƽ���� �߿� Skill ������ ������ ��� �˻�.
	//
	PLANDMANCLIENT pLand = pGaeaClient->GetActiveMap();
	if ( !pLand )
		return TRUE;

	float fApplyAngle  = D3DXToRadian(sSKILL_DATA.wAPPLYANGLE) / 2.0f;
	float fDetectRange = (float) ( pChar->GETBODYRADIUS() + pChar->GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );

	EraseVector( GLCharacter::ms_vecTAR_TEMP );

	D3DXVECTOR3				vTarPosition	= vTarPos;
	D3DXVECTOR3				vTarOrgDir		= vTarPosition - vPos;	

	GLPartyClient* const pPartyClient = pGaeaClient->GetMyPartyClient();
	for( DWORD dwIndex = 0; dwIndex < pPartyClient->GetNMember(); dwIndex++ )
	{
		const GLPartySlotClient* const pMember = pPartyClient->GetMember(dwIndex);
		if ( pMember == NULL )
			continue;

		if( pMember->m_dwGaeaID == pChar->GetGaeaID() )
			continue;

		std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar ( pMember->m_dwGaeaID );
		if ( pChar == NULL )
			continue;
		
		const D3DXVECTOR3& vMemPos = pMember->m_vPosition;

		D3DXVECTOR3 vTarDir = vMemPos - vPos;
		float fDir    = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
		float fLength = D3DXVec3Length ( &D3DXVECTOR3(vPos - vMemPos) );
		if ( sSKILL_DATA.wAPPLYANGLE==0 || (fabs(fDir) <= fApplyAngle && fLength <= fDetectRange) )
		{
			STARGETID sFindID ( pChar->GETCROW(), pChar->GetGaeaID(), vMemPos );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
		}
	}
	
	SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );

	return TRUE;*/
} //SKT_TARZONEOURFANWISE

/**
 * ��ų�� ���� ������ ������ ��(PC/MOB) check (Ÿ�� �˻� ����)
 */
BOOL SKT_TARZONEENEMYFANWISE( const STARGETID&			sTargetID, 
							  const GLSKILL&			sSKILL, 
							  const WORD				wLEVEL,
							  GLCharacter*				pChar,
							  GLGaeaClient*				pGaeaClient,
							  const D3DXVECTOR3&		vPos,
							  SKILLTARGETDATA&			sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	float fApplyAngle  = D3DXToRadian(sSKILL_DATA.wAPPLYANGLE) / 2.0f;
	float fAttackRange = (float) ( pChar->GETBODYRADIUS() + pChar->GETSKILLRANGE_APPLY(sSKILL,wLEVEL) + 2 );

	const D3DXVECTOR3 vTarOrgDir(sTargetID.vPos - vPos);

	//	Crow �˻�.

	EraseVector( GLCharacter::ms_vecTAR_TEMP );

	VEC_SK_ITER_RANGE			pairRange;
	DWORD					emCrow = ( CROW_EX_MOB | CROW_EX_PC | CROW_EX_SUMMON );
	VEC_SK_TAR*				pDetectMap = pGaeaClient->DetectCrow ( emCrow, vPos, fAttackRange );
	if ( pDetectMap )
	{
		VEC_SK_ITER iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			const STARGETID &sFindID = (*iter);

			ClientActor* pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( (sTargetID != sFindID) && !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) )
				continue;

			D3DXVECTOR3 vTarDir = sFindID.vPos - vPos;
			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
			if ( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle )
			{
				// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
				if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
					continue;

				float fLengthSq = D3DXVec3LengthSq( &D3DXVECTOR3( vPos - sFindID.vPos) );
				GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
			}
		}

		//	MOB.
		pairRange = Equal_Range( pDetectMap, CROW_MOB );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() )
				continue;

			D3DXVECTOR3 vTarDir = sFindID.vPos - vPos;
			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
			if ( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle )
			{
				// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
				if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
					continue;

				float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vPos-sFindID.vPos) );
				GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
			}
		}

		//	SUMMON.
		pairRange = Equal_Range( pDetectMap, CROW_SUMMON );
		iter = pairRange.first;
		for ( ; (iter!=pairRange.second); ++iter )
		{
			STARGETID sFindID = (*iter);

			//	Note : ���� �������� �������� üũ�Ѵ�
			//
			ClientActor *pTARGET = pGaeaClient->GetCopyActor(sFindID);
			if ( !pTARGET || pTARGET->IsNonDamaged() || !pChar->IsReActionable(sFindID, EMIMPACT_SIDE_ENEMY, false) )
				continue;

			D3DXVECTOR3 vTarDir = sFindID.vPos - vPos;
			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
			if ( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle )
			{
				// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
				if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
					continue;

				float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(vPos-sFindID.vPos) );
				GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
			}
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if ( sSkillTarget.nTarget==0 )
		return FALSE;

	return TRUE;
}

/**
 * ��ų�� ���� ������ ������ PC check (Ÿ�� �˻� ����)
 */
BOOL SKT_TARZONEANYBODYFANWISE( const D3DXVECTOR3&		vTarPos, 
								const GLSKILL&			sSKILL, 
								const WORD				wLEVEL,
								GLCharacter*			pChar,
								GLGaeaClient*			pGaeaClient,
								const D3DXVECTOR3&		vPos,
								OUT SKILLTARGETDATA&	sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	//	Crow �˻�.
	float fApplyAngle  = D3DXToRadian(sSKILL_DATA.wAPPLYANGLE) / 2.0f;
	float fAttackRange = (float)( pChar->GETBODYRADIUS() + pChar->GETSKILLRANGE_APPLY( sSKILL, wLEVEL ) + 2 );

	const D3DXVECTOR3 vTarOrgDir(vTarPos - vPos);

	EraseVector( GLCharacter::ms_vecTAR_TEMP );

	VEC_SK_ITER_RANGE			pairRange;	
	VEC_SK_TAR*				pDetectMap = pGaeaClient->DetectCrow( CROW_EX_PC, vPos, fAttackRange );
	if ( pDetectMap )
	{
		VEC_SK_ITER iterDetect;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iterDetect = pairRange.first;
		for ( ; (iterDetect!=pairRange.second); ++iterDetect )
		{
			const STARGETID &sFindID = (*iterDetect);
			if ( FALSE == pChar->IsReActionable( sFindID, EMIMPACT_SIDE_ANYBODY ) )
				continue;

			D3DXVECTOR3 vTarDir = sFindID.vPos - vPos;
			float fDir = DXGetThetaYFromDirection ( vTarDir, vTarOrgDir );
			if ( sSKILL_DATA.wAPPLYANGLE==0 || fabs(fDir) <= fApplyAngle )
			{
				// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
				if ( pLandManClient->IsLayerSimpleCollision( vPos, sFindID.vPos, 5.f ) )
					continue;

				float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3( vPos - sFindID.vPos ) );
				GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
			}
		}

		// �����ͺ��� ������ ������ŭ �̾Ƴ���.
		WORD wTARNUM = sSKILL_DATA.wTARNUM;
		SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
	} //if

	if ( sSkillTarget.nTarget==0 )
		return FALSE;

	return TRUE;
} //SKT_TARZONEANYBODYFANWISE

/**
 * ��ų�� ���� ������ ������ ��Ƽ�� check
 */
BOOL SKT_TARLANDOURZONE( const STARGETID&		sTargetID, 
						 const GLSKILL&			sSKILL, 
						 const WORD				wLEVEL,
						 GLCharacter*			pChar,
						 GLGaeaClient*			pGaeaClient,
						 SKILLTARGETDATA&		sSkillTarget )
{
	GLLandManClient* pLandManClient = pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return FALSE;

	const SKILL::CDATA_LVL &sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	AddTarget( pChar, sSkillTarget );

	WORD		wTARNUM = sSKILL_DATA.wTARNUM;
	if( wTARNUM <= sSkillTarget.nTarget )	
		return TRUE;

	const D3DXVECTOR3& vFindPos = pChar->GetPosition();

	WORD wRange = sSKILL_DATA.wAPPLYRANGE;

	/*
	if( RF_PROVOKE( pChar->m_EffSkillVarSet ).IsOn() )
	{
		wRange = RF_PROVOKE( pChar->m_EffSkillVarSet ).GetEffDistance();
	}*/

	//	Crow �˻�.
	//const float fDetectRange = ::pow(float(pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2), 2.0f );
	const float fDetectRange = float(pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2);

	VEC_SK_ITER_RANGE pairRange;	
	VEC_SK_TAR *pDetectMap = pGaeaClient->DetectCrow( CROW_EX_PC, vFindPos, fDetectRange );
	if ( pDetectMap )
	{
		EraseVector( GLCharacter::ms_vecTAR_TEMP );

		VEC_SK_ITER			iter;

		//	PC.
		pairRange = Equal_Range( pDetectMap, CROW_PC );
		iter = pairRange.first;
		for( ; iter != pairRange.second; ++iter )
		{
			const STARGETID &sFindID = (*iter);
			if ( FALSE == pChar->IsReActionable( sFindID, EMIMPACT_SIDE_PARTY ) )
				continue;

			// Simple Collision �� �浹�Ǹ�, Target ���� ������ �Ѵ�.
			if ( pLandManClient->IsLayerSimpleCollision( sFindID.vPos, vFindPos, 5.f ) )
				continue;

			float fLengthSq = D3DXVec3LengthSq ( &D3DXVECTOR3(sFindID.vPos-vFindPos) );

			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLengthSq,sFindID) );
		}

		//	Note : �����ͺ��� ������ ������ŭ �̾Ƴ���.
		//
		{
			WORD wTARNUM = sSKILL_DATA.wTARNUM;
			SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );
		}
	}

	if( sSkillTarget.nTarget == 0 )
		return FALSE;

	return TRUE;


	/*const SKILL::CDATA_LVL&		sSKILL_DATA = sSKILL.m_sAPPLY.sDATA_LVL[wLEVEL];

	AddTarget( pChar, sSkillTarget );

	WORD		wTARNUM = sSKILL_DATA.wTARNUM;
	if( wTARNUM <= sSkillTarget.nTarget )	
		return TRUE;

	const D3DXVECTOR3& vFindPos = pChar->GetPosition();

	// ��Ƽ���� �߿� Skill ������ ������ ��� �˻�.
	PLANDMANCLIENT pLand = pGaeaClient->GetActiveMap();
	if ( !pLand )
		return TRUE;

	EraseVector( GLCharacter::ms_vecTAR_TEMP );

	const float fDetectRange = ::pow(float(pChar->GETBODYRADIUS() + static_cast<float>(sSKILL_DATA.wAPPLYRANGE) + 2), 2.0f);

	GLPartyClient* const pPartyClient = pGaeaClient->GetMyPartyClient();
	for( DWORD dwIndex = 0; dwIndex < MAXPARTY; dwIndex++ )
	{
		const GLPartySlotClient* const pMember = pPartyClient->GetMember(dwIndex);
		if ( pMember == NULL )
			continue;

		if( pMember->m_dwGaeaID == pChar->GetGaeaID() )
			continue;

		std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar ( pMember->m_dwGaeaID );

		if ( pChar == NULL )
			continue;

		const D3DXVECTOR3& vMemPos = pMember->m_vPosition;

		const float fLength = D3DXVec3LengthSq ( &D3DXVECTOR3(vMemPos - vFindPos) );
		if ( fLength <= fDetectRange )
		{
			STARGETID sFindID ( pChar->GETCROW(), pChar->GetGaeaID(), vMemPos );
			GLCharacter::ms_vecTAR_TEMP.push_back ( STARDIST(fLength,sFindID) );
		}
	}
	
	SelectFromNearRange( GLCharacter::ms_vecTAR_TEMP, sSkillTarget.nTarget, wTARNUM, static_cast<WORD>(EMTARGET_NET), sSkillTarget.TargetIDs );

	return TRUE;*/
} //SKT_TARZONEOURZONE

/**
 * �ڽ��� ��ġ�� ��ȯ���� ��ȯ�Ϸ� �Ҷ� ��ġ�� �������� ����ش�.
 */
BOOL SKT_SELFRANDOMZONE( const GLSKILL&			sSKILL, 
						 const WORD				wLEVEL, 
						 GLCharacter*			pChar,
						 const D3DXVECTOR3&		vPos,
						 SKILLTARGETDATA&		sSkillTarget )
{
	const SKILL::SCREATURE_LVL&		sCREATURE_LVL	= sSKILL.m_sSPECIAL_SKILL.sCREATURE_LVL[wLEVEL];
	PCROWDATA						pCrowData		= GLCrowDataMan::GetInstance().GetCrowData( sCREATURE_LVL.sID );
	if( !pCrowData )
	{
		pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pChar->m_sBasicSummonID );
		if ( !pCrowData )
			return FALSE;
	}

	PGLSUMMON			pSummonData = GLCONST_SUMMON::GetSummonData( pCrowData->m_emSummonType );
	if ( !pSummonData )
		return FALSE;

	sSkillTarget.TargetID.vPos = vPos + pSummonData->GetRandomPosition();

	AddTarget( pChar, sSkillTarget );

	return TRUE;
} //SKT_SELFRANDOMZONE