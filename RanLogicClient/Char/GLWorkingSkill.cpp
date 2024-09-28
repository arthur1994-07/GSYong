
class GLWorkingSkill;

#include "pch.h"

#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"

#include "GLCharacter.h"
#include "GLWorkingSkill.h"

#include "../GLGaeaClient.h"

#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/Skill/GLCountAttackSkill.h"

/**
 * ������.
 */
GLWorkingSkill::GLWorkingSkill(GLGaeaClient* const pGaeaClient)
: m_pGaeaClient(pGaeaClient)
, m_pSkillData( NULL )
, m_pNext( NULL )
, m_pCharacter( NULL )
, m_fAccumTime( 0.0f )
, m_fSkillAniTime( 0.0f)
, m_nStep( 0 )
{
} //GLWorkingSkill::GLWorkingSkill

/**
 * �Ҹ���.
 */
GLWorkingSkill::~GLWorkingSkill()
{
	Clear();
} //GLWorkingSkill::~GLWorkingSkill

/**
 * Ŭ����.
 */
void GLWorkingSkill::Clear()
{
	SAFE_DELETE( m_pNext );
	m_pSkillData = NULL;
	m_sSkill.sNativeID = NATIVEID_NULL();
	m_pCharacter = NULL;
	m_fAccumTime = 0.0f;
    m_fSkillAniTime = 0.0f;
	m_nStep		 = 0;
	m_pAnimCont	 = NULL;    
} //GLWorkingSkill::Clear

/**
 * ��ų�������� ����.
 *
 * @param id					[in] ��ų���̵�.
 * @param pOwnerChar			[in] ����� ĳ����.
 * @return ������ TRUE.
 */
BOOL GLWorkingSkill::SetSkill( const SCHARSKILL& sSkill, GLCharacter* pOwnerChar )
{
	Clear();

	m_sSkill		= sSkill;
	if( !m_sSkill.sNativeID.IsValidNativeID() )
		return FALSE;

	m_pSkillData = GLSkillMan::GetInstance().GetData( m_sSkill.sNativeID.wMainID, m_sSkill.sNativeID.wSubID );
	if( !m_pSkillData )
		return FALSE;

	m_pCharacter	= pOwnerChar;
	m_idSubSkill	= m_pSkillData->m_sEXT_DATA.idMultiTargetSkill;
	DxSkinChar*			pSkinChar	= m_pCharacter->GetSkinChar();

	const PANIMCONTNODE		pAnicont	= pSkinChar->GETANI( m_pSkillData->m_sEXT_DATA.emANIMTYPE, m_pSkillData->m_sEXT_DATA.emANISTYPE );
	if( !pAnicont || !pAnicont->m_pDxSkinAni )
		return FALSE;

	m_pAnimCont		= pAnicont->m_pDxSkinAni;
	if( m_idSubSkill != NATIVEID_NULL() )
	{
		m_nStartSubSkillStep = m_pSkillData->m_sEXT_DATA.nMultiTatgetEffectFrame;
		if( m_nStartSubSkillStep > m_pAnimCont->m_wStrikeCount )
			m_nStartSubSkillStep = m_pAnimCont->m_wStrikeCount;

		m_nStartSubSkillStep--;
		if( m_nStartSubSkillStep < 0 )
			m_nStartSubSkillStep = 0;
	} //if

	return TRUE;
} //GLWorkingSkill::SetSkill

/**
 * �ڽ��� �̵��� �� �ִ� ��ų�� �ִ� ��� ��ų ���� ������ ��ġ�� ��´�.
 *
 * @param vPos					[out] over there
 * @return if you are going to somewhere, return true
 */
bool GLWorkingSkill::GetArrivalPos( D3DXVECTOR3& vPos, BOOL& bOriglnPos )
{
	if( !m_pSkillData || !m_pCharacter )
		return false;

	bOriglnPos = FALSE;

	for( unsigned int i = 0; i < m_pSkillData->m_sAPPLY.vecSPEC.size(); i++ )
	{
		if( m_pSkillData->m_sAPPLY.vecSPEC[i].emSPEC == EMSPECA_TELEPORT )
		{
			switch( m_pSkillData->m_sBASIC.emIMPACT_TAR )
			{
			case TAR_LAND:
				vPos = m_sTargetData.TargetID.vPos;
				bOriglnPos = TRUE;
				return true;
			case TAR_ZONE:
				vPos = m_sTargetData.TargetID.vPos;
				return true;

			case TAR_SELF_TOSPEC:
			case TAR_SPEC:
				if( m_sTargetData.nTarget > 0 )
				{
					ClientActor*	pTarget = m_pCharacter->GetCrowOnLand( m_sTargetData.TargetIDs[0].GETCROW(), 
																		   m_sTargetData.TargetIDs[0].GETID() );
					if( pTarget &&
						pTarget->IsValidBody() )
					{
						vPos = pTarget->GetPosition();
						return true;
					} //if
				} //if
				break;

			case TAR_SELF:
				vPos = m_pCharacter->GetPosition();
				vPos.x += (float)(sc::Random::getInstance().GetValue() % m_pSkillData->m_sAPPLY.sDATA_LVL[m_sSkill.wLevel].wAPPLYRANGE);
				vPos.z += (float)(sc::Random::getInstance().GetValue() % m_pSkillData->m_sAPPLY.sDATA_LVL[m_sSkill.wLevel].wAPPLYRANGE);
				return true;
			} //switch
			return true;
		}
		else if( m_pSkillData->m_sAPPLY.vecSPEC[i].emSPEC == EMSPECA_SWAPPOS )
		{
			if( m_pSkillData->m_sAPPLY.vecSPEC[i].sSPEC[m_sSkill.wLevel].dwFLAG < 2 )		// that mean including actor that is a character
			{
				if( m_sTargetData.nTarget > 0 )
				{
					ClientActor*	pTarget = m_pCharacter->GetCrowOnLand( m_sTargetData.TargetIDs[0].GETCROW(), 
																		   m_sTargetData.TargetIDs[0].GETID() );
					if( pTarget &&
						pTarget->IsValidBody() )
					{
						vPos = pTarget->GetPosition();
						return true;
					} //if
				} //if
			} //if
		} //if..else
	} //for
	
	return false;
} //GLWorkingSkill::GetArrivalPos

void GLWorkingSkill::SkillEffectZone( const SKILLTARGETDATA& sTargetData )
{
	GASSERT( m_pCharacter );

	if( !m_pSkillData )
		return;

	int					nEffFrame = m_nStep + 1;
	D3DXMATRIX			matTrans;
	D3DXMatrixIdentity( &matTrans );
	EMELEMENT			emELMT = ( m_pSkillData->m_sAPPLY.emELEMENT == EMELEMENT_ARM ) ? m_pCharacter->GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
	if( m_pSkillData->m_sEXT_DATA.emTARG == SKILL::EMTIME_IMPACT )
	{
		const D3DXVECTOR3& vPosition = sTargetData.TargetID.vPos;
		D3DXMatrixTranslation( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup( m_pSkillData->m_sEXT_DATA.GETTARG( emELMT ), matTrans, &TARGETID_NULL, TRUE, TRUE );
		if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_TARG_OVR ) ||
			( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
		{
			if( spEffSingleG.get() )
				spEffSingleG->AddEffAfter( EFFASINGLE, m_pSkillData->m_sEXT_DATA.GETTARGZONE01(emELMT) );
			else
				DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_pSkillData->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, TRUE );
		} //if

		if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_TARG_OVR ) ||
			( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
		{
			if( spEffSingleG.get() )
				spEffSingleG->AddEffAfter( EFFASINGLE, m_pSkillData->m_sEXT_DATA.GETTARGZONE02(emELMT) );
			else
				DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_pSkillData->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, TRUE );
		} //if

		if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_TARG_OVR ) ||
			( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
		{
			if( spEffSingleG.get() )
				spEffSingleG->AddEffAfter( EFFASINGLE, m_pSkillData->m_sEXT_DATA.GETTARGZONE03(emELMT) );
			else
				DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_pSkillData->m_sEXT_DATA.GETTARGZONE03(emELMT), TRUE, TRUE );
		} //if
	} //if

	if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_IMPACT ) ||
		( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
	{
		DxEffGroupPlayer::GetInstance().NewEffGroup( m_pSkillData->m_sEXT_DATA.GETTARGZONE01(emELMT), matTrans, &TARGETID_NULL, TRUE, TRUE );
	} //if

	if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_IMPACT ) ||
		( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
	{
		DxEffGroupPlayer::GetInstance().NewEffGroup( m_pSkillData->m_sEXT_DATA.GETTARGZONE02(emELMT), matTrans, &TARGETID_NULL, TRUE, TRUE );
	} //if

	if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_IMPACT ) ||
		( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
	{
		DxEffGroupPlayer::GetInstance().NewEffGroup( m_pSkillData->m_sEXT_DATA.GETTARGZONE03(emELMT), matTrans, &TARGETID_NULL, TRUE, TRUE );
	} //if
} //GLWorkingSkill::SkillEffectZone

void GLWorkingSkill::SkillEffectSpecific( const SKILLTARGETDATA& sTargetData, const SANIMSTRIKE &sStrikeEff )
{
	GASSERT( m_pCharacter );

	if( !m_pSkillData )
		return;

	if( sTargetData.nTarget == 0 )
		return;

	WORD			wNumTarIds	= sTargetData.nTarget;
	STARID*			pTarIds		= (STARID*)sTargetData.TargetIDs;
	STARGETID		sTarget( pTarIds[wNumTarIds-1].emCrow, pTarIds[wNumTarIds-1].dwID);

	if( !m_pGaeaClient->IsVisibleCV(sTarget) )
		return;

	sTarget.vPos = m_pGaeaClient->GetTargetPos( sTarget );

	D3DXVECTOR3 vDir = sTarget.vPos - m_pCharacter->GetPosition();
	vDir.y = 0.f;
	D3DXVec3Normalize( &vDir, &vDir );

	EMELEMENT		emELMT = ( m_pSkillData->m_sAPPLY.emELEMENT == EMELEMENT_ARM ) ? m_pCharacter->GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
	if( m_pSkillData->m_sEXT_DATA.VALIDTARG(emELMT) )			//��ǥ ���� ����Ʈ�� �Ǿ� ������ ����Ʈ.( �Ǿ� �������� ��ǥ ��ġ ����Ʈ�� ��ġ�� �ڵ����� ���� ����Ʈ�� ���� �������� ����. )
	{
		m_pCharacter->SK_EFF_TARG( m_pSkillData, sStrikeEff, sTarget );
	}
	else												// �ܵ����� ��ǥ ������ �߻�.
	{
		int					nEffFrame = m_nStep + 1;
		//	Note : ��ǥ ��ġ ����Ʈ.
		if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_IMPACT ) ||
			( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
		{
			m_pCharacter->SK_EFF_TARZONE( sTarget, m_pSkillData->m_sEXT_DATA.emTARGZONE01_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE01(emELMT) );
		} //if

		if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_IMPACT ) ||
			( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
		{
			m_pCharacter->SK_EFF_TARZONE( sTarget, m_pSkillData->m_sEXT_DATA.emTARGZONE02_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE02(emELMT) );
		} //if

		if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_IMPACT ) ||
			( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
		{
			m_pCharacter->SK_EFF_TARZONE( sTarget, m_pSkillData->m_sEXT_DATA.emTARGZONE03_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE03(emELMT) );
		} //if

		//	Note : ��ǥ �� ����Ʈ.
		if( m_pSkillData->m_sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffBody ( m_pSkillData->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );

		if( m_pSkillData->m_sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_IMPACT )
			DxEffGroupPlayer::GetInstance().NewEffBody ( m_pSkillData->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
	} //if..else
} //GLWorkingSkill::SkillEffectSpecific

void GLWorkingSkill::SkillEffectUnit( const SKILLTARGETDATA& sTargetData, const SANIMSTRIKE &sStrikeEff )
{
	GASSERT( m_pCharacter );

	if( !m_pSkillData )
		return;

	int				nEffFrame = m_nStep + 1;
	WORD			wNumTarIds	= sTargetData.nTarget;
	STARID*			pTarIds		= (STARID*)sTargetData.TargetIDs;

	CSkillCAIdx* pCharCa   = m_pCharacter->GetSkillCA();

	for( WORD i =0 ; i < wNumTarIds; ++i )
	{
		bool bTargetEffect = false;

		CSkillCAInterface cSkillCA;
		
		STARGETID sCId   = STARGETID(CROW_PC, m_pCharacter->GetGaeaID());
		STARGETID sCTaId = STARGETID(pTarIds[i].emCrow, pTarIds[i].dwID);

		CSkillCAIdx* pClientCa = m_pGaeaClient->GetTargetCA(sCTaId);

		if( pClientCa ) {		
						
			cSkillCA.Cal_LastApplyValueII( m_sSkill.sNativeID, m_sSkill.wLevel, pCharCa, pClientCa, sCId, sCTaId, true );

			STARGETID sTemp = cSkillCA.Cal_LastDamage( m_sSkill.sNativeID, m_sSkill.wLevel, pCharCa, pClientCa, sCId, sCTaId, true );

			if( (sTemp != sCTaId) || pCharCa->IsSimPacAddon() || pCharCa->IsSpecddon() )
			{
				STARGETID	sTarget = sCId;

				sTarget.vPos = m_pGaeaClient->GetTargetPos( sTarget );

				D3DXVECTOR3 vDir = sTarget.vPos - m_pCharacter->GetPosition();
				vDir.y = 0.f;
				D3DXVec3Normalize ( &vDir, &vDir );

				EMELEMENT		emELMT = ( m_pSkillData->m_sAPPLY.emELEMENT == EMELEMENT_ARM ) ? m_pCharacter->GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
				if ( m_pSkillData->m_sEXT_DATA.VALIDTARG( emELMT ) )		// ��ǥ ���� ����Ʈ�� �Ǿ� ������ ����Ʈ. ( �Ǿ� �������� ��ǥ ��ġ ����Ʈ�� ��ġ�� �ڵ����� ���� ����Ʈ�� ���� �������� ����. )
				{
					sCTaId.vPos = m_pGaeaClient->GetTargetPos( sCTaId );
					m_pCharacter->SK_EFF_TARG( m_pSkillData, sStrikeEff, sCTaId, &sCId );
				}
				else												//	�ܵ����� ��ǥ ������ �߻�.
				{
					//	Note : ��ǥ ��ġ ����Ʈ.
					if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_IMPACT ) ||
						( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
					{
						m_pCharacter->SK_EFF_TARZONE( sTarget, m_pSkillData->m_sEXT_DATA.emTARGZONE01_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE01(emELMT) );
					} //if

					if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_IMPACT ) ||
						( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
					{
						m_pCharacter->SK_EFF_TARZONE( sTarget, m_pSkillData->m_sEXT_DATA.emTARGZONE02_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE02(emELMT) );
					} //if

					if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_IMPACT ) ||
						( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
					{
						m_pCharacter->SK_EFF_TARZONE( sTarget, m_pSkillData->m_sEXT_DATA.emTARGZONE03_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE03(emELMT) );
					} //if

					//	Note : ��ǥ �� ����Ʈ.
					if( m_pSkillData->m_sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( m_pSkillData->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );

					if( m_pSkillData->m_sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_IMPACT )
						DxEffGroupPlayer::GetInstance().NewEffBody ( m_pSkillData->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
				} //if..else
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

		STARGETID	sTarget(pTarIds[i].emCrow, pTarIds[i].dwID);
		if( !m_pGaeaClient->IsVisibleCV(sTarget) || !bTargetEffect)
			continue;
		
		sTarget.vPos = m_pGaeaClient->GetTargetPos( sTarget );

		D3DXVECTOR3 vDir = sTarget.vPos - m_pCharacter->GetPosition();
		vDir.y = 0.f;
		D3DXVec3Normalize ( &vDir, &vDir );

		const bool bBuff_Debuf(m_pSkillData->m_sBASIC.emACTION_TYPE == SKILL::EMACTION_TYPE_BUFF || m_pSkillData->m_sBASIC.emACTION_TYPE == SKILL::EMACTION_TYPE_DEBUFF);

		EMELEMENT		emELMT = ( m_pSkillData->m_sAPPLY.emELEMENT == EMELEMENT_ARM ) ? m_pCharacter->GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
		if ( m_pSkillData->m_sEXT_DATA.VALIDTARG( emELMT ) )		// ��ǥ ���� ����Ʈ�� �Ǿ� ������ ����Ʈ. ( �Ǿ� �������� ��ǥ ��ġ ����Ʈ�� ��ġ�� �ڵ����� ���� ����Ʈ�� ���� �������� ����. )
		{
			m_pCharacter->SK_EFF_TARG( m_pSkillData, sStrikeEff, sTarget );
		}
		else												//	�ܵ����� ��ǥ ������ �߻�.
		{
			// ����&������� ����Ʈ�� Ŭ���̾�Ʈ�� �ش� ��ų ȿ���� �޾��� ��� ����Ѵ�;
			// NewSkillEffect_AppliPoint ����;
			if ( !bBuff_Debuf )
			{
				//	Note : ��ǥ ��ġ ����Ʈ.
				if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_IMPACT ) ||
					( m_pSkillData->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame) )
				{
					m_pCharacter->SK_EFF_TARZONE( sTarget, m_pSkillData->m_sEXT_DATA.emTARGZONE01_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE01(emELMT) );
				} //if

				if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_IMPACT ) ||
					( m_pSkillData->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame) )
				{
					m_pCharacter->SK_EFF_TARZONE( sTarget, m_pSkillData->m_sEXT_DATA.emTARGZONE02_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE02(emELMT) );
				} //if

				if( ( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_IMPACT ) ||
					( m_pSkillData->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame) )
				{
					m_pCharacter->SK_EFF_TARZONE( sTarget, m_pSkillData->m_sEXT_DATA.emTARGZONE03_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE03(emELMT) );
				} //if
			}

			//	Note : ��ǥ �� ����Ʈ.
			if( m_pSkillData->m_sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( m_pSkillData->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );

			if( m_pSkillData->m_sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_IMPACT )
				DxEffGroupPlayer::GetInstance().NewEffBody ( m_pSkillData->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
		} //if..else

		if ( m_pSkillData->m_sEXT_DATA.bTARG_ONE )
			break;
	} //for
} //GLCharacter::SkillEffectUnit

void GLWorkingSkill::SkillEffectTargetChange( const SANIMSTRIKE &sStrikeEff )
{
	CSkillCAInterface cSkillCA;

	STARGETID sCId   = STARGETID(CROW_PC, m_pCharacter->GetGaeaID());
	//STARGETID sCTaId = m_sTargetData.TargetID;
	WORD wNum = m_sTargetData.nTarget;
	
	STARGETID sCTaId;

	if(wNum > 0) {
		sCTaId = STARGETID(m_sTargetData.TargetIDs[wNum-1].emCrow, m_sTargetData.TargetIDs[wNum-1].dwID);
	}

	CSkillCAIdx* pCharCa   = m_pCharacter->GetSkillCA();
	CSkillCAIdx* pClientCa = m_pGaeaClient->GetTargetCA(sCTaId);

	if( pClientCa ) {		
		
		cSkillCA.Cal_LastApplyValueII( m_sSkill.sNativeID, m_sSkill.wLevel, pCharCa, pClientCa, sCId, sCTaId, true );

		STARGETID sTemp = cSkillCA.Cal_LastDamage( m_sSkill.sNativeID, m_sSkill.wLevel, pCharCa, pClientCa, sCId, sCTaId, true );

		SKILLTARGETDATA sTempData = m_sTargetData;

		if( (sTemp != sCTaId) || pCharCa->IsSimPacAddon() || pCharCa->IsSpecddon() )
		{			
			sTempData.TargetID = sCId;
			sTempData.TargetID.vPos = m_pCharacter->GetPosition();

			switch( m_pSkillData->m_sBASIC.emIMPACT_TAR )
			{
			case TAR_ZONE		 : SkillEffectZone( sTempData );					break;
			case TAR_SELF_TOSPEC : SkillEffectSpecific( sTempData, sStrikeEff );	break;
			}
		}

		if( (sTemp == sCTaId) || pClientCa->IsSimPacAddon() || pClientCa->IsSpecddon() )
		{
			switch( m_pSkillData->m_sBASIC.emIMPACT_TAR )
			{
			case TAR_ZONE		 : SkillEffectZone( m_sTargetData );					break;
			case TAR_SELF_TOSPEC : SkillEffectSpecific( m_sTargetData, sStrikeEff );	break;
			}	
		}		
	}	
	else
	{
		switch( m_pSkillData->m_sBASIC.emIMPACT_TAR )
		{
		case TAR_ZONE		 : SkillEffectZone( m_sTargetData );					break;
		case TAR_SELF_TOSPEC : SkillEffectSpecific( m_sTargetData, sStrikeEff );	break;
		}	
	}
}

void GLWorkingSkill::SkillEffectTarget( const SKILLTARGETDATA& sTargetData, const SANIMSTRIKE &sStrikeEff )
{
	if( !m_pSkillData )
		return;
	
	switch( m_pSkillData->m_sBASIC.emIMPACT_TAR )
	{
	case TAR_ZONE:				SkillEffectTargetChange(sStrikeEff);/*SkillEffectZone( sTargetData );*/						break;		// Ÿ�� ����Ʈ�� ������ ���� �߻�Ǵ� ���.
	case TAR_SELF_TOSPEC:		SkillEffectTargetChange(sStrikeEff);/*SkillEffectSpecific( sTargetData, sStrikeEff );*/		break;		// Ÿ�� ����Ʈ�� ������ ���� �߻�Ǵ� ���. �ڱ� ��ġ���� ��� ��ġ����. ( ȭ�� ���� )
	default:					SkillEffectUnit( sTargetData, sStrikeEff );			break;		// Ÿ�� ����Ʈ�� ������ ���� �߻�Ǵ� ���. ��ǥ ������ �ִ� ���.
	} //switch
} //GLWorkingSkill::SkillEffectTarget

void GLWorkingSkill::SkillEffectSelf( const SANIMSTRIKE &sStrikeEff )
{
	GASSERT( m_pCharacter );

	if( !m_pSkillData )
		return;

	int				nEffFrame = m_nStep + 1;
	STARGETID		sTARG( m_pCharacter->GETCROW(), m_pCharacter->GetGaeaID() );
	EMELEMENT		emELMT = ( m_pSkillData->m_sAPPLY.emELEMENT == EMELEMENT_ARM ) ? m_pCharacter->GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);
	if( ( m_pSkillData->m_sEXT_DATA.emSELFZONE01 == SKILL::EMTIME_IMPACT ) ||
		( m_pSkillData->m_sEXT_DATA.emSELFZONE01 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
	{
		m_pCharacter->SK_EFF_SELFZONE( sStrikeEff, m_pSkillData->m_sEXT_DATA.emSELFZONE01_POS, m_pSkillData->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );
	} //if

	if( ( m_pSkillData->m_sEXT_DATA.emSELFZONE02 == SKILL::EMTIME_IMPACT ) ||
		( m_pSkillData->m_sEXT_DATA.emSELFZONE02 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
	{
		m_pCharacter->SK_EFF_SELFZONE( sStrikeEff, m_pSkillData->m_sEXT_DATA.emSELFZONE02_POS, m_pSkillData->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );
	} //if

	if( ( m_pSkillData->m_sEXT_DATA.emSELFZONE03 == SKILL::EMTIME_IMPACT ) ||
		( m_pSkillData->m_sEXT_DATA.emSELFZONE03 == SKILL::EMTIME_EFFECT && m_pSkillData->m_sAPPLY.nEffectFrame == nEffFrame ) )
	{
		m_pCharacter->SK_EFF_SELFZONE( sStrikeEff, m_pSkillData->m_sEXT_DATA.emSELFZONE03_POS, m_pSkillData->m_sEXT_DATA.GETSELFZONE03(emELMT), &sTARG );
	} //if

	if( m_pSkillData->m_sEXT_DATA.emSELFBODY == SKILL::EMTIME_IMPACT )
		DxEffGroupPlayer::GetInstance().NewEffBody( m_pSkillData->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &m_pCharacter->GetDirect() );
} //GLWorkingSkill::SkillEffectSelf

void GLWorkingSkill::Start( const SCHARSKILL& sSkill, GLCharacter* pOwnerChar, const SKILLTARGETDATA* pTargetData, bool bSubSkill )
{
	GASSERT( pOwnerChar );

	SetSkill( sSkill, pOwnerChar );
	SetSkillTarget( pTargetData );

	if( sSkill.sNativeID.wMainID == EMSKILL_BIKE )
		pOwnerChar->ACCOUNTBIKESKILL( m_sSkill, 1 );
	else
		pOwnerChar->ACCOUNTSKILL( m_sSkill, 1 );

	m_nStep			= 0;
	m_fAccumTime	= 0.0f;
    m_fSkillAniTime = 0.0f;

	EMSLOT				emRHand = m_pCharacter->GetCurRHand();
	STARGETID			sTARG( m_pCharacter->GETCROW(), m_pCharacter->GetGaeaID() );
	EMELEMENT			emELMT = ( m_pSkillData->m_sAPPLY.emELEMENT == EMELEMENT_ARM ) ? m_pCharacter->GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);	// ��ų ���۰� ���ÿ� ��Ÿ���� ����Ʈ. �ߵ�.
	if( m_pSkillData->m_sEXT_DATA.emSELFBODY == SKILL::EMTIME_FIRST )
		DxEffGroupPlayer::GetInstance().NewEffBody( m_pSkillData->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &m_pCharacter->GetDirect() );

	SANIMSTRIKE			sStrike;
	sStrike.m_emPiece	= m_pCharacter->VALID_SLOT_ITEM( emRHand ) ? PIECE_RHAND : PIECE_GLOVE;
	sStrike.m_emEffect	= EMSF_TARGET;
	sStrike.m_dwFrame	= 0;

	if( m_pSkillData->m_sEXT_DATA.emSELFZONE01 == SKILL::EMTIME_FIRST )
		m_pCharacter->SK_EFF_SELFZONE( sStrike, m_pSkillData->m_sEXT_DATA.emSELFZONE01_POS, m_pSkillData->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if( m_pSkillData->m_sEXT_DATA.emSELFZONE02 == SKILL::EMTIME_FIRST )
		m_pCharacter->SK_EFF_SELFZONE( sStrike, m_pSkillData->m_sEXT_DATA.emSELFZONE02_POS, m_pSkillData->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if( m_pSkillData->m_sEXT_DATA.emSELFZONE03 == SKILL::EMTIME_FIRST )
		m_pCharacter->SK_EFF_SELFZONE( sStrike, m_pSkillData->m_sEXT_DATA.emSELFZONE03_POS, m_pSkillData->m_sEXT_DATA.GETSELFZONE03(emELMT), &sTARG );

	WORD				wNumTarIds	= m_sTargetData.nTarget;
	STARID*				pTarIds		= (STARID*)m_sTargetData.TargetIDs;
	if( m_pSkillData->m_sBASIC.emIMPACT_TAR == TAR_SPEC )
	{
		for( WORD i = 0; i < wNumTarIds; ++i )
		{
			STARGETID		sTARGET;
			sTARGET.emCrow = pTarIds[i].GETCROW();
			sTARGET.GaeaId = pTarIds[i].GETID();

			if( !m_pGaeaClient->IsVisibleCV(sTARGET) )
				continue;

			sTARGET.vPos = m_pGaeaClient->GetTargetPos( sTARGET );
			if( m_pSkillData->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_FIRST )
				m_pCharacter->SK_EFF_TARZONE( sTARGET, m_pSkillData->m_sEXT_DATA.emTARGZONE01_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if( m_pSkillData->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_FIRST )
				m_pCharacter->SK_EFF_TARZONE( sTARGET, m_pSkillData->m_sEXT_DATA.emTARGZONE02_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE02(emELMT) );

			if( m_pSkillData->m_sEXT_DATA.emTARGZONE03==SKILL::EMTIME_FIRST )
				m_pCharacter->SK_EFF_TARZONE( sTARGET, m_pSkillData->m_sEXT_DATA.emTARGZONE03_POS, m_pSkillData->m_sEXT_DATA.GETTARGZONE03(emELMT) );
		} //for
	} //if

	if( !bSubSkill )
		SendSkillMsg();

	if( m_idSubSkill != NATIVEID_NULL() &&
		m_pSkillData->m_sEXT_DATA.nMultiTatgetEffectFrame == 0 )			//immediately
	{
		GASSERT( m_pNext );

		m_nStartSubSkillStep = -1;
		SCHARSKILL		sSubSkill( m_idSubSkill, m_sSkill.wLevel );
		m_pNext->Start( sSubSkill, m_pCharacter, NULL, true );
	} //if
} //GLWorkingSkill::Start

void GLWorkingSkill::StartDealyAction( const SCHARSKILL& sSkill, GLCharacter* pOwnerChar, const SKILLTARGETDATA* pTargetData, bool bSubSkill )
{
	GASSERT( pOwnerChar );

	m_nStep			= 0;
	m_fAccumTime	= 0.0f;
	m_fSkillAniTime = 0.0f;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkill.sNativeID.wMainID, sSkill.sNativeID.wSubID );
	if ( !pSkill )
		return;

	EMSLOT				emRHand = pOwnerChar->GetCurRHand();
	STARGETID			sTARG( pOwnerChar->GETCROW(), pOwnerChar->GetGaeaID() );
	EMELEMENT			emELMT = ( pSkill->m_sAPPLY.emELEMENT == EMELEMENT_ARM ) ? pOwnerChar->GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);	// ��ų ���۰� ���ÿ� ��Ÿ���� ����Ʈ. �ߵ�.
	if( pSkill->m_sEXT_DATA.emSELFBODY == SKILL::EMTIME_FIRST )
		DxEffGroupPlayer::GetInstance().NewEffBody( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &pOwnerChar->GetDirect() );

	SANIMSTRIKE			sStrike;
	sStrike.m_emPiece	= pOwnerChar->VALID_SLOT_ITEM( emRHand ) ? PIECE_RHAND : PIECE_GLOVE;
	sStrike.m_emEffect	= EMSF_TARGET;
	sStrike.m_dwFrame	= 0;

	if( pSkill->m_sEXT_DATA.emSELFZONE01 == SKILL::EMTIME_FIRST )
		pOwnerChar->SK_EFF_SELFZONE( sStrike, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if( pSkill->m_sEXT_DATA.emSELFZONE02 == SKILL::EMTIME_FIRST )
		pOwnerChar->SK_EFF_SELFZONE( sStrike, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if( pSkill->m_sEXT_DATA.emSELFZONE03 == SKILL::EMTIME_FIRST )
		pOwnerChar->SK_EFF_SELFZONE( sStrike, pSkill->m_sEXT_DATA.emSELFZONE03_POS, pSkill->m_sEXT_DATA.GETSELFZONE03(emELMT), &sTARG );

	WORD				wNumTarIds	= pTargetData->nTarget;
	STARID*				pTarIds		= (STARID*)pTargetData->TargetIDs;
	if( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SPEC )
	{
		for( WORD i = 0; i < wNumTarIds; ++i )
		{
			STARGETID		sTARGET;
			sTARGET.emCrow = pTarIds[i].GETCROW();
			sTARGET.GaeaId = pTarIds[i].GETID();

			if( !m_pGaeaClient->IsVisibleCV(sTARGET) )
				continue;

			sTARGET.vPos = m_pGaeaClient->GetTargetPos( sTARGET );
			if( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_FIRST )
				pOwnerChar->SK_EFF_TARZONE( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_FIRST )
				pOwnerChar->SK_EFF_TARZONE( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

			if( pSkill->m_sEXT_DATA.emTARGZONE03==SKILL::EMTIME_FIRST )
				pOwnerChar->SK_EFF_TARZONE( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE03_POS, pSkill->m_sEXT_DATA.GETTARGZONE03(emELMT) );
		} //for
	} //if

	//if( !bSubSkill )
	//	SendSkillMsg();

	//if( m_idSubSkill != NATIVEID_NULL() &&
	//	m_pSkillData->m_sEXT_DATA.nMultiTatgetEffectFrame == 0 )			//immediately
	//{
	//	GASSERT( m_pNext );

	//	m_nStartSubSkillStep = -1;
	//	SCHARSKILL		sSubSkill( m_idSubSkill, m_sSkill.wLevel );
	//	m_pNext->Start( sSubSkill, m_pCharacter, NULL, true );
	//} //if
	//} 
}

void GLWorkingSkill::SendSkillMsg()
{
	STARGETID sSelectID(m_pGaeaClient->GetCharacter()->GetSelectID());

	GLMSG::SNETPC_REQ_SKILL		NetMsg;
	NetMsg.skill_id	= m_sSkill.sNativeID;
	NetMsg.vTargetPosition	= m_sTargetData.TargetID.vPos;
	NetMsg.emTarCrow = sSelectID.emCrow;
	NetMsg.dwTarID = sSelectID.GaeaId;
	NetMsg.vSelectTargetPosition = sSelectID.vPos;

	if( m_pCharacter->IsDefenseSkill() )
		NetMsg.bDefenseSkill = true;
	
	for( WORD i = 0; i < m_sTargetData.nTarget; ++i )
		NetMsg.ADDTARGET( m_sTargetData.TargetIDs[i] );

	if( m_pNext )			// there is second skill 
	{
		for( WORD i = 0; i < m_pNext->m_sTargetData.nTarget; ++i )
			NetMsg.ADDSECONDTARGET( m_pNext->m_sTargetData.TargetIDs[i] );
	} //if

	m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
} //GLWorkingSkill::SendSkillMsg

/**
 * ��ų ����Ÿ�� ����.
 *
 * @param targetData				[in] struct SKILLTARGETDATA.
 */
void GLWorkingSkill::SetSkillTarget( const SKILLTARGETDATA* pTargetData )
{
	if( pTargetData )
		m_sTargetData = *pTargetData;

	if( m_idSubSkill != NATIVEID_NULL() )
	{
		SAFE_DELETE( m_pNext );
		m_pNext = new GLWorkingSkill(m_pGaeaClient);

		D3DXVECTOR3		vPos;

		BOOL bOriginPos = FALSE;

		if( !GetArrivalPos( vPos, bOriginPos ) )
			vPos = m_pCharacter->GetPosition();
		else
			m_sTargetData.TargetID.vPos = vPos;

		SCHARSKILL		sSubSkill( m_idSubSkill, m_sSkill.wLevel );
		STARGETID		sTargetID;
		sTargetID.emCrow = m_sTargetData.TargetID.emCrow;
		sTargetID.GaeaId = m_sTargetData.TargetID.GaeaId;
		sTargetID.vPos	 = vPos;

		if( bOriginPos )
			vPos = m_pCharacter->GetPosition();

		::SelectSkillTarget(m_pGaeaClient, sSubSkill.sNativeID, sSubSkill.wLevel, sTargetID, m_pCharacter, vPos, m_pNext->m_sTargetData );

	} //if
} //GLWorkingSkill::SetSkillTarget

/**
 * ���罺�ܿ� ������ ����Ʈ.
 *
 * @param sStrikeEff				[in] struct SANIMSTRIKE.
 * @param nStep						[in] ���罺��.
 */
BOOL GLWorkingSkill::MoveNext()
{
	if( !m_pCharacter )				// that mean including started skill
		return TRUE;

	if( !m_pAnimCont || m_pAnimCont->m_wStrikeCount == 0 )
		return FALSE;

	GASSERT( m_pSkillData );

    if (m_pAnimCont->IsValidateANI() == false)
    {
        // �ִϸ��̼��� �ε� ���� ���� 
        // �ð� ����� ó������ ���ϰ� �Ѵ�.
        m_fSkillAniTime = 0.0f;
        return TRUE;
    }

    const DWORD dwThisKey = (DWORD)m_fSkillAniTime + m_pAnimCont->m_dwSTime;

	while( m_nStep < m_pAnimCont->m_wStrikeCount )
	{
		DWORD		dwStrikeKey	= m_pAnimCont->m_sStrikeEff[m_nStep].m_dwFrame;
		m_pCharacter->GetSkinChar()->UpdateStrikeAttachBone( dwThisKey, m_nStep );
		if( dwThisKey >= dwStrikeKey )
		{
			if( m_pNext && m_nStep == m_nStartSubSkillStep )
			{
				SCHARSKILL		sSubSkill( m_idSubSkill, m_sSkill.wLevel );
				m_pNext->Start( sSubSkill, m_pCharacter, NULL, true );
			} //if

			SkillEffectSelf( m_pAnimCont->m_sStrikeEff[m_nStep] );
			SkillEffectTarget( m_sTargetData, m_pAnimCont->m_sStrikeEff[m_nStep] );
			m_nStep++;
		}
		else
		{
			break;
		} //if..else
	} //if

	if( m_pNext )
	{
		if( !m_pNext->MoveNext() )
			SAFE_DELETE( m_pNext );
	} //if

	if( dwThisKey >= m_pAnimCont->m_dwETime )
	{
		if( m_pAnimCont->m_dwFlag & ACF_LOOP )
		{
			m_fAccumTime = 0;
            m_fSkillAniTime = 0;
			m_nStep = 0;
		}
		else
		{
			Clear();
			return FALSE;
		} //if..else
	} //if

	return TRUE;
} //GLWorkingSkill::MoveNext

BOOL GLWorkingSkill::CheckBasicFlag( DWORD dwCheckFlag )
{
	return ( m_pSkillData && (m_pSkillData->m_sBASIC.dwFlags & dwCheckFlag) );
} //GLWorkingSkill::CheckBasicFlag

/**
 * ����ĳ���� ��ų ����ó��. (���� ����ĳ���ÿ� ������ ����� �����ϰ�����)
 *
 * @param fElapsedTime						[in] ����ð�.
 * @return ����ĳ���� ��ų�̸� ���� TRUE.
 */
void GLWorkingSkill::FrameMove( float fElapsedTime )
{
	if( !m_pCharacter )
		return;

	m_fAccumTime += fElapsedTime;
	const float fSkinAniElap = (m_pCharacter->GETATT_ITEM() + m_pCharacter->GETATTVELO()) * fElapsedTime * UNITANIKEY_PERSEC;
	m_fSkillAniTime += fSkinAniElap;

	if( m_pNext )
		m_pNext->FrameMove( fElapsedTime );

	if( !m_pCharacter->isAction( GLAT_SKILL ) )
	{
		if( CheckBasicFlag( FLAG_SB_RUNNING_CAST ) &&
			( m_pSkillData->m_sAPPLY.fRunningEffTime < m_fAccumTime ) &&
			m_nStep == 0 )
		{
			EMSKILLCHECK emSkillCheck = m_pCharacter->SkillCheck( m_sSkill.sNativeID, STARGETID(), 1, true, true, false );
			if( emSkillCheck == EMSKILL_OK || emSkillCheck == EMSKILL_NOTSP ) 
			{
				SkillEffectSelf( m_pAnimCont->m_sStrikeEff[0] );
				SkillEffectTarget( m_sTargetData, m_pAnimCont->m_sStrikeEff[0] );
			} //if

			Clear();				// just one time.
			return;
		} //if
	} //if

	if( m_pCharacter->isAction( GLAT_PUSHPULL ) &&				// ���� �ȵ��!
		m_idSubSkill != NATIVEID_NULL() )
	{
		MoveNext();
	} //if
} //GLWorkingSkill::FrameMove
