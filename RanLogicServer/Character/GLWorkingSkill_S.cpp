
#include "../pch.h"

#define FLAG_HIT_SUBSKILL			0x00000001

class GLWorkingSkill_s;

#include "GLChar.h"
#include "GLWorkingSkill_s.h"
#include "../FieldServer/GLGaeaServer.h"

GLWorkingSkill_s::GLWorkingSkill_s(GLWorkingSkill_s* pParent) :
	m_pSkillData		( NULL ),
	m_pCharacter		( NULL ),
	m_pAniAttack		( NULL ),
	m_pNext				( NULL ),
	m_pParent			( pParent ),
	m_wTARNUM			( 0 ),
	m_fSkillAniTime		( 0.0f ),
	m_bDelLock			( false )
{
	m_vItemSkList.clear();
} //GLWorkingSkill_s::GLWorkingSkill_s

GLWorkingSkill_s::~GLWorkingSkill_s()
{
	SAFE_DELETE( m_pNext );

	Clear();
} //GLWorkingSkill_s::~GLWorkingSkill_s

/**
 * Ŭ����.
 */
void GLWorkingSkill_s::Clear()
{
	if( !m_bDelLock )
		SAFE_DELETE( m_pNext );

	m_bDelLock				= false;

	m_pSkillData			= NULL;
	m_sSkill.sNativeID		= NATIVEID_NULL();
	m_pCharacter			= NULL;
	m_pAniAttack			= NULL;
	m_nStartSubSkillStep	= -1;
	m_fSkillAniTime = 0.0f;
	
} //GLWorkingSkill_s::Clear

void GLWorkingSkill_s::SetSkill( const SCHARSKILL sSkill, GLChar* pOwnerChar )
{
	Clear();

	if( !pOwnerChar )
		sc::writeLogError( "Skill����ϴ� Char�� Null�� �� �����ϴ�. " );

	m_pCharacter	= pOwnerChar;
	m_sSkill		= sSkill;
	m_pSkillData	= GLSkillMan::GetInstance().GetData( m_sSkill.sNativeID.wMainID, m_sSkill.sNativeID.wSubID );
} //GLWorkingSkill_s::SetSkill

bool GLWorkingSkill_s::SetItemSkillTarget( sItemSkill& info, const STARGETID* pTargetID, GLChar* pChar )
{
	if( !info.sIDSkill.IsValidNativeID() || pChar == NULL ) return false;

	STARID				sTARIDS[EMTARGET_NET<<1];
	WORD				wTARCount = 0;

	GLLandMan* pLandMan = pChar->GetLandMan();

	if( pLandMan ) {

		ServerActor* pTarget = NULL;

		if( pTargetID ) pTarget = pChar->GetTarget( *pTargetID );

		pLandMan->m_SkillTargetSystem.SelectSkillTarget( pChar, pTarget, info.sIDSkill, info.wLVSkill, pChar->GetPositionActor(), sTARIDS, wTARCount );
	}
	
	if( pTargetID && wTARCount > 0 )
	{
		SAFE_DELETE( m_pNext );
		m_pNext = new GLWorkingSkill_s(this);

		for( WORD i=0; i<wTARCount; ++i )
		{
			m_pNext->m_sTARIDS[i] = sTARIDS[i];
			m_pNext->m_wTARNUM++;
		}		

		memcpy( &m_pNext->m_TargetID, pTargetID, sizeof( STARGETID ) );

		GASSERT( m_pNext );
		SCHARSKILL		sSubSkill( info.sIDSkill, info.wLVSkill );
		m_pNext->Start( sSubSkill, pChar, false, &m_TargetID, 0, NULL, true );
		//m_pNext->SetItemSkill(info);

		return true;
	}

	return false;
}

void GLWorkingSkill_s::SetSkillTarget( const STARGETID* pTargetID, const WORD wTARNUM, const STARID* pTARIDS )
{
	if( wTARNUM > 0 )
	{
		memcpy( &m_sTARIDS, pTARIDS, sizeof( STARID ) * wTARNUM );
		m_wTARNUM		= wTARNUM;
	} //if
	memcpy( &m_TargetID, pTargetID, sizeof( STARGETID ) );

	if( pTargetID    && pTARIDS &&
		m_pSkillData && m_pSkillData->m_sEXT_DATA.idMultiTargetSkill != NATIVEID_NULL() )
	{
		if( m_pNext ) {
			PGLSKILL pTemp = m_pNext->GetSkillData();

			if( pTemp && pTemp->m_sBASIC.sNATIVEID != m_pSkillData->m_sEXT_DATA.idMultiTargetSkill ){
				
			}
		}

		SAFE_DELETE( m_pNext );
		m_pNext = new GLWorkingSkill_s(this);

		for( WORD i=0; i<EMTARGET_NET; ++i )
		{
			if( pTARIDS[m_wTARNUM+i].dwID == 0xffffffff )
				break;

			m_pNext->m_sTARIDS[i] = pTARIDS[m_wTARNUM+i];
			m_pNext->m_wTARNUM++;
		}

		memcpy( &m_pNext->m_TargetID, pTargetID, sizeof( STARGETID ) );

		m_nStartSubSkillStep = m_pSkillData->m_sEXT_DATA.nMultiTatgetEffectFrame;

		if( m_nStartSubSkillStep == 0 )
		{
			GASSERT( m_pNext );
			SCHARSKILL		sSubSkill( m_pSkillData->m_sEXT_DATA.idMultiTargetSkill, m_sSkill.wLevel );

			m_pNext->Start( sSubSkill, m_pCharacter, false, &m_TargetID, 0, NULL, true );
		}
		else
		{
			if( m_nStartSubSkillStep > (int)m_sHITARRAY.size() )
				m_nStartSubSkillStep = m_sHITARRAY.size();

			m_nStartSubSkillStep--;
			if( m_nStartSubSkillStep < 0 )
				m_nStartSubSkillStep = 0;
		} //if..else
	}
	
} //GLWorkingSkill_s::SetSkillTarget


/**
 * 
 */
BOOL GLWorkingSkill_s::HitDamage( const DWORD dwThisKey, const SANIATTACK &sAniAttack )
{
	if( !m_pCharacter )
		return FALSE;

	if( m_wCurTrack < (WORD)m_sHITARRAY.size() )
	{
		if ( dwThisKey >= m_sHITARRAY[m_wCurTrack] )
		{
			m_pCharacter->HitDamage( m_strike );
			m_wCurTrack++;								//	ť ����Ÿ�� ó���� ��� �����Ѵ�.
		} //if
	} //if

	//	�̻����� ���� ����Ÿ�� ���� ó������ �������,
	//	�������� �Ѳ����� ó���ϰ�, FALSE�� �����Ѵ�.
	if( dwThisKey >= sAniAttack.m_dwETime )
	{
		while ( m_wCurTrack < (WORD)m_sHITARRAY.size() )
		{
			m_pCharacter->HitDamage( m_strike );
			m_wCurTrack++;								//	ť ����Ÿ�� ó���� ��� �����Ѵ�.
		} //while

		return FALSE;
	} //if

	// ���� ������ ���� ���� �ʾ����� hit�� �Ϸ�̰� ����Ű�� ���� ���� ������ ���
	//		������ ����Ȱɷ� ����. ( Ŭ���̾�Ʈ�� �������� ������ ����. )
	if( m_wCurTrack >= (WORD)m_sHITARRAY.size() )
	{
		//DWORD dwDISKEY = sAniAttack.m_dwETime - dwThisKey;
		//if ( dwDISKEY <= 5 )		return FALSE;

		return FALSE;
	} //if

	return TRUE;
} //GLWorkingSkill_s::HitDamage

BOOL GLWorkingSkill_s::ApplyAllSkill()
{
	WORD	 wAttackDiv = (WORD)m_sHITARRAY.size();
	while( m_wCurTrack < wAttackDiv )
	{
		m_pCharacter->ApplySkill( m_sSkill, m_strike, m_TargetID, m_wTARNUM, m_sTARIDS, wAttackDiv );
		m_wCurTrack++;
	} //while

	return TRUE;
} //GLWorkingSkill_s::ApplyAllSkill

BOOL GLWorkingSkill_s::IsApplyEnd()
{	
	if( (m_wCurTrack + 1 >= m_sHITARRAY.size()) )
	{
		if( m_pNext && m_pNext->IsApplyEnd() )
			return TRUE;
		else
		if( m_pNext == NULL )
			return TRUE;
	}

	return FALSE;
}

BOOL GLWorkingSkill_s::IsApplyEnd(SNATIVEID sSkillIdx)
{	
	if( sSkillIdx == NATIVEID_NULL() ) return TRUE;

	if( m_sSkill.sNativeID == sSkillIdx )
	{
		if( (m_wCurTrack + 1 >= m_sHITARRAY.size()) )
		{
			return TRUE;
		}
	}
	else
	{
		if( m_pNext && m_pNext->IsApplyEnd(sSkillIdx))
			return TRUE;
	}
	
	return FALSE;
}

BOOL GLWorkingSkill_s::ApplySkill()
{
	if( !m_pSkillData )
		return false;	

	DWORD dwThisKey = static_cast<DWORD>(m_fSkillAniTime);
	if( m_pAniAttack )
		dwThisKey += m_pAniAttack->m_dwSTime;

	BOOL bResult(FALSE);
	WORD		wAttackDiv = (WORD)m_sHITARRAY.size();
	
	while( m_wCurTrack < wAttackDiv )
	{ //���� �ð��� ������ ���ó��.
		if( (m_sHITARRAY[m_wCurTrack] > dwThisKey) && (m_pParent == NULL) )
			break;
				
		if ( m_pCharacter->ApplySkill( m_sSkill, m_strike, m_TargetID, m_wTARNUM, m_sTARIDS, wAttackDiv ) == FALSE )
		{
			++m_wCurTrack;
			continue;
		}

		bResult = TRUE;
		if ( m_pSkillData == NULL )
			return TRUE;

		if( m_pSkillData->m_sEXT_DATA.idMultiTargetSkill != NATIVEID_NULL() &&
			m_nStartSubSkillStep == m_wCurTrack  )
		{
			SCHARSKILL		sSubSkill( m_pSkillData->m_sEXT_DATA.idMultiTargetSkill, m_sSkill.wLevel ); 
			GASSERT( m_pNext );

			m_pNext->Start( sSubSkill, m_pCharacter, false, &m_TargetID, 0, NULL, true );

			//ApplyItemSkillRunII();
		}
				
		++m_wCurTrack;
	} //while

	if( m_pAniAttack && m_pAniAttack->IsLOOP() )
	{
		if ( !m_pCharacter )
		{
			sc::writeLogError( 0 && "BOOL GLWorkingSkill_s::ApplySkill() m_pCharacter is null." );
			return FALSE;
		}

		if( m_wCurTrack >= (WORD)m_sHITARRAY.size() &&
			m_pNext == NULL )
		{
			DWORD dwSkillCheckOption = 0;
			if( m_pCharacter->IsDefenseSkill( m_sSkill.sNativeID ) )
				dwSkillCheckOption |= EMSKILLCHECKOPTION_DEFENSESKILL;

			EMSKILLCHECK emCHECK = m_pCharacter->CHECHSKILL( m_sSkill.sNativeID, 1, m_pCharacter->VehicleState(), dwSkillCheckOption );

			Start( m_sSkill, m_pCharacter, emCHECK == EMSKILL_NOTSP, &m_TargetID, m_wTARNUM, m_sTARIDS, false );
		}
	}

	return bResult;
} //GLWorkingSkill_s::ApplySkill

DWORD GLWorkingSkill_s::GetNumTargetInSkill()
{
	GLLandMan* const pLand = m_pCharacter->GetLandMan();
	if ( !pLand )
		return 0;

	float		fSkillRange;
	if( !m_pCharacter->GetSkillRange( m_sSkill.sNativeID, fSkillRange ) )
		return FALSE;

	DWORD			dwTarget = 0;
	STARGETID		sTARID;
	if( m_wTARNUM < 1 )
		m_wTARNUM = 0;

	for( WORD i = 0; i < m_wTARNUM; ++i )
	{
		sTARID.emCrow = m_sTARIDS[i].GETCROW();
		sTARID.GaeaId = m_sTARIDS[i].GETID();
		ServerActor* pACTOR = m_pCharacter->GetTarget( sTARID );
		if( !pACTOR )
			continue;

		// ��� ������ ����Ÿ���̸� ���� ����
		GLChar* pChar = m_pCharacter->GetChar( sTARID.GaeaId );
		if( pChar )
		{
			if ( pChar->m_sCONFTING.IsPOWERFULTIME() )
				continue;
		}

		float			fReActionRange	= (float) ( pACTOR->GetBodyRadius() + fSkillRange );		// Ÿ�ٹݰ� + ��ų ��ȿ�ݰ�
		fReActionRange *= GLCONST_CHAR::fREACT_VALID_SCALE;

		if ( !pLand->IsAbleAttack_Range_LayerSimpleCollision( fReActionRange, m_pCharacter->GetPosition(), pACTOR->GetPosition(), 5.f ) )
		{
			m_sTARIDS[i].dwID = GAEAID_NULL;
		}
		else
		{
			dwTarget++;
		}
	} //for

	if( dwTarget < 1 )
		dwTarget = 0;

	return dwTarget;
} //GLWorkingSkill_s::GetNumTargetInSkill

BOOL GLWorkingSkill_s::SendToClientNoTarget()
{
	if( !m_pSkillData )			// it's empty.
		return TRUE;

	// ��ų�� ����Ǵ� Ÿ�� �˻�.
	//
	DWORD		dwNumTarget = GetNumTargetInSkill();
	// ��ų ��ȿ�ݰ� ���� Ÿ���� ���� ��� (�� ������ġ ��ų�� ����)
	if( m_pSkillData->m_sBASIC.emIMPACT_TAR != TAR_LAND && dwNumTarget == 0 )
	{
		// �ڽſ���.
		GLMSG::SNETPC_ATTACK_DAMAGE		NetMsg;
		NetMsg.emTarCrow		= m_TargetID.emCrow;
		NetMsg.dwTarID			= m_TargetID.GaeaId;
		NetMsg.nDamage			= 0;
		NetMsg.wSchool          = m_pCharacter->GetSchool();
		m_pCharacter->SendToClient( &NetMsg );

		return TRUE;
	} //if

	return FALSE;
} //GLWorkingSkill_s::SendToClientNoTarget


BOOL GLWorkingSkill_s::Start( const SCHARSKILL&		sSkill,
							  GLChar*				pOwnerChar,
							  BOOL					bLowSP, 
							  const STARGETID*		pTargetID, 
							  const WORD			wTARNUM, 
							  const STARID*			pTARIDS,
							  const bool			bSubSkill )
{
	if( !pOwnerChar )
	{
		sc::writeLogError( "Character �� �������� ����." );
		return FALSE;
	} //if

	if( bSubSkill ) SetDelLock( true );

	SetSkill( sSkill, pOwnerChar );

	STARGETID			sTargetID;
	float				fHitTime		= 0.0f;
	bool				bCheckHit		= FALSE;
	bool				bCreateOneTrack = false;

	DWORD dwApplyFlag;
	m_sHITARRAY.clear();			// ť �ʱ�ȭ
	if( sSkill.sNativeID.wMainID == SNATIVEID::ID_NULL ||
		sSkill.sNativeID.wSubID == SNATIVEID::ID_NULL )
	{
		EMANI_SUBTYPE		emANISUBTYPE	= m_pCharacter->GetAniSubType();
		m_pAniAttack = m_pCharacter->GetAniAttack( AN_ATTACK, emANISUBTYPE );
		if ( !m_pAniAttack )	
			return FALSE;

		dwApplyFlag = m_pCharacter->ISLONGRANGE_ARMS() ? SKILL::EMAPPLY_DWORD_RANGE : SKILL::EMAPPLY_DWORD_MELEE;
		sTargetID = m_pCharacter->GetTargetID();
		bCheckHit = TRUE;
	}
	else
	{
		if( !m_pSkillData )
		{
			sc::writeLogError( "SkillData �� �������� ����. SetSkill�Լ����� �⺻���� ���� �����ʿ�" );
			return FALSE;
		} //if

		m_pAniAttack  = m_pCharacter->GetAniAttack( m_pSkillData->m_sEXT_DATA.emANIMTYPE, m_pSkillData->m_sEXT_DATA.emANISTYPE );
		if( !m_pAniAttack )
		{
			if( bSubSkill )
				bCreateOneTrack = true;
			else
				return FALSE;
		} //if
	
		if( m_pSkillData->m_sBASIC.emAPPLY != SKILL::EMAPPLY_MAGIC && 
			m_pSkillData->m_sBASIC.emIMPACT_TAR != TAR_LAND )
		{
			// ��ų Ÿ���� �ϳ��� ��ȿ�ϰ� Ȯ���� �ϰ� ����.
			//	NEED : ���� Ÿ���� ��� �ٽ� Ȯ���ؾ���.
			sTargetID.emCrow = EMCROW(m_pCharacter->GetTarIds()[0].emCrow);
			sTargetID.GaeaId = m_pCharacter->GetTarIds()[0].dwID;

			bCheckHit = TRUE;
		}
		else
		{
			sTargetID.GaeaId = EMTARGET_NULL;
		} //if..else
		dwApplyFlag = (1 << m_pSkillData->m_sBASIC.emAPPLY);
	} //if..else

	m_strike.bLowSP = cast_bool( bLowSP );
	m_strike.bHit	= true;
	if( bCheckHit && 
		!m_pCharacter->CheckHit( sTargetID, bLowSP, dwApplyFlag ) )
	{
		m_strike.bHit = false;
	} //if
	
	m_wCurTrack		= 0;
	m_fAccumTime	= 0.0;
	m_fSkillAniTime = 0.0f;


	if( bCreateOneTrack )
	{
		m_sHITARRAY.resize( 1 );
		m_sHITARRAY[0] = static_cast<DWORD>(fHitTime);
	}
	else
	{
		m_sHITARRAY.resize( m_pAniAttack->m_wDivCount );
		for( int i = 0; i < m_pAniAttack->m_wDivCount; i++ )
		{
			m_sHITARRAY[i] = m_pAniAttack->m_wDivFrame[i];
		} //for
	} //if..else

	SetSkillTarget( pTargetID, wTARNUM, pTARIDS );

	// ��ų ���� �Ҹ𰪵� �Ҹ��Ŵ.
	pOwnerChar->ACCOUNTSKILL( m_sSkill, 1, true );

	///////////////////////////////////
	ApplyItemSkillRun( m_pCharacter );
	///////////////////////////////////

	return TRUE;
} //GLWorkingSkill_s::Start

/**
 * ��ų�� attack ani�� ����������� ����.
 *
 * @param dwThisKey				[in] current frame time.
 * @return if it's over time, return TRUE.
 */
BOOL GLWorkingSkill_s::MoveNext()
{
	if ( m_pCharacter == NULL )
		return TRUE;			// not start yet

	if ( (ApplySkill() == TRUE) && (m_pNext != NULL) )
		m_pNext->MoveNext();

	const BOOL bContinue(m_wCurTrack < (WORD)m_sHITARRAY.size());
	if ( bContinue == FALSE )
	{
		Clear();
	}

	return bContinue;
} //GLWorkingSkill_s::MoveNext

void GLWorkingSkill_s::FrameMove( float fElapsedTime )
{
	if( !m_pCharacter || !m_pSkillData )
		return;

	m_fAccumTime += fElapsedTime;

	const float fSkinAniElap = (m_pCharacter->GETATT_ITEM() + m_pCharacter->GETATTVELO()) * fElapsedTime * UNITANIKEY_PERSEC;
	m_fSkillAniTime += fSkinAniElap;

	if( m_pNext )
		m_pNext->FrameMove( fElapsedTime );

	if( m_pCharacter->isAction( GLAT_SKILL ) )			// ��ų������ ���� ���Ļ�� ���Ͻ��� ���ʿ�
		return;

	if( m_pSkillData->m_sBASIC.dwFlags & FLAG_SB_RUNNING_CAST &&
		m_wCurTrack == 0 )
	{ // this skill is a running skill.
		if( m_sHITARRAY.size() != 1 )
			m_sHITARRAY.resize( 1 );

		if (m_sHITARRAY.size())
			m_sHITARRAY[0] = static_cast<DWORD>(m_pSkillData->m_sAPPLY.fRunningEffTime);

		if( !MoveNext() )
			return;
	} //if
} //GLWorkingSkill_s::FrameMove

BOOL GLWorkingSkill_s::ApplyItemSkillRunII()
{
	if( m_pSkillData == NULL || m_pCharacter == NULL ) return FALSE;

	if( m_pParent == NULL && m_strike.bHit )
	{
		//ApplyItemSkill(m_vItemSkList);

		return TRUE;
	}

	return FALSE;
}

BOOL GLWorkingSkill_s::ApplyItemSkillRun( GLChar* pChar )
{
	if( m_pSkillData == NULL || m_pCharacter == NULL ) return FALSE;
	
	if( m_pParent == NULL && m_strike.bHit )
	{		
		m_vItemSkList.clear();

		std::vector<sItemSkill> info;

		if( m_pCharacter->ApplyItemSkillRun(m_sSkill, m_pSkillData, m_TargetID, m_wTARNUM, m_sTARIDS, info) )
		{
			m_vItemSkList = info;

			ApplyItemSkill(info, pChar);
		}

		return TRUE;
	}

	return FALSE;
}

void GLWorkingSkill_s::ApplyItemSkill( std::vector<sItemSkill>& info, GLChar* pChar )
{
	if( info.size() == 0 ) return;

	if( m_pNext ) {
		m_pNext->ApplyItemSkill(info, pChar);
		return;
	}

	sItemSkill sTemp = info.back();

	info.pop_back();
		
	if( SetItemSkillTarget( sTemp, &m_TargetID, pChar ) )
	{
		m_pNext->ApplyItemSkill(info, pChar);
	}
	else
	{
		ApplyItemSkill( info, pChar );
	}

}



//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// global functions for skill
//////////////////////////////////////////////////////////////////////////

//	�������, �о�� Ȯ�� ��� �� �Ÿ� Ȯ��
bool CheckPullIn( ServerActor* pActor, ServerActor* pTarget, float fRate, float& fDist )
{
	WORD	wLevel		=  pActor->GetLevel();
    WORD	wTarLevel	= pTarget->GetLevel();
    int		wTarAvoid	= pTarget->GetAvoid();

    float	fBasicRate	= ( (float)( wLevel - wTarLevel ) / (float) GLCONST_CHAR::wMAX_LEVEL * (float)wTarAvoid ) * 0.01f;
    fBasicRate += fRate;

	if (fBasicRate > 1.0f)
        fBasicRate = 1.0f;
    else if (fBasicRate < 0.0f)
        fBasicRate = 0.0f;

    if (fBasicRate > sc::Random::RANDOM_POS()) 
    {
        fDist = D3DXVec3Length(&D3DXVECTOR3(pActor->GetPosition() - pTarget->GetPosition()));
        fDist -= GLCONST_CHAR::wBODYRADIUS;
        fDist -= pTarget->GetBodyRadius();
        fDist = -fDist;

        return true;
    }

    return false;
} //CheckPullIn

//	�������, �о�� Ȯ�� ��� �� �Ÿ� Ȯ��
bool CheckKnockBack( ServerActor* pActor, ServerActor* pTarget, const float fRate )
{
	WORD	wLevel		=  pActor->GetLevel();
    WORD	wTarLevel	= pTarget->GetLevel();
    int		wTarAvoid	= pTarget->GetAvoid();

    float	fBasicRate	= ( (float)( wLevel - wTarLevel ) / (float) GLCONST_CHAR::wMAX_LEVEL * (float)wTarAvoid ) * 0.01f;
    fBasicRate += fRate;

    if( fBasicRate > 1.0f )
		fBasicRate = 1.0f;
    else if( fBasicRate < 0.0f ) 
		fBasicRate = 0.0f;

    if( fBasicRate > sc::Random::RANDOM_POS() )
		return true;

	return false;
}

bool GetPullSpeed( ServerActor* pActor, ServerActor* pTarget, const SKILL::SSPEC& sSKILL_SPEC, float& fSpeed, float& fDist )
{
    if( !pTarget )
        return false;

	if( pTarget->GetCrow() == CROW_MOB )
	{	// ���� ��� ��ų ȿ�� ���� ����. by luxes.
		if( !CheckPullIn( pActor, pTarget, sSKILL_SPEC.fVAR1 * sSKILL_SPEC.fRate, fDist ) )
			return false;

		fSpeed = sSKILL_SPEC.fVAR2 * sSKILL_SPEC.fRate2;
	}
	else
	{
		if( !CheckPullIn( pActor, pTarget, sSKILL_SPEC.fVAR1, fDist ) ) 
			return false;
		
		fSpeed = sSKILL_SPEC.fVAR2;
	} //if..else

	return true;
} //GetPullSpeed

bool ApplySkillTargetPullin( ServerActor* pActor, ServerActor* pTarget, const SKILL::SSPEC& sSKILL_SPEC, SSKILLACTEX& sSKILLACTEX )
{
	if( !pTarget->IsValidBody() )
		return false;

	if( pTarget->GetCrow() == CROW_MOB )
	{
		GLCrow*		pMob = dynamic_cast<GLCrow*>( pTarget );
		if( pMob->IsCheckedActFlags( EMCROWACT_BOSS | EMCROWACT_POSHOLD | EMCROWACT_DIRHOLD ) ||
			pMob->IsNoAllPenalty() )
		{
			return false;
		} //if
	} //if

	float		fDist = 0.0f;
	if( !GetPullSpeed( pActor, pTarget, sSKILL_SPEC, sSKILLACTEX.fPUSH_SPEED, fDist ) )
		return false;

	sSKILLACTEX.fPUSH_PULL	= fDist;
	sSKILLACTEX.dwAniType	= AN_SUB_PULLIN;

	return true;
} //ApplySkillTargetPullin

bool ApplySkillTargetKnockback( ServerActor* pActor, ServerActor* pTarget, const SKILL::SSPEC& sSKILL_SPEC, SSKILLACTEX& sSKILLACTEX )
{
	if( !pTarget->IsValidBody() )
		return false;

	float		fDist = 0.0f;
	if( pTarget->GetCrow() == CROW_MOB )
	{ // ���� ��� ��ų ȿ�� ���� ����. by luxes.
		GLCrow*		pMob = dynamic_cast<GLCrow*>( pTarget );
		if( pMob->IsCheckedActFlags( EMCROWACT_BOSS | EMCROWACT_POSHOLD | EMCROWACT_DIRHOLD ) ||
			pMob->IsNoAllPenalty() )
		{
			return false;
		} //if

		if( !CheckKnockBack( pActor, pTarget, sSKILL_SPEC.fVAR1 * sSKILL_SPEC.fRate ) ) 
			return false;

		sSKILLACTEX.fPUSH_PULL = sSKILL_SPEC.fVAR2 * sSKILL_SPEC.fRate2;				
	}
	else
	{
		if( !CheckKnockBack( pActor, pTarget, sSKILL_SPEC.fVAR1 ) )
			return false;
		
		sSKILLACTEX.fPUSH_PULL = sSKILL_SPEC.fVAR2;				
	} //if..else

	sSKILLACTEX.fPUSH_SPEED = GLCONST_CHAR::fPUSHPULL_VELO;
	sSKILLACTEX.dwAniType	= AN_SUB_49;
	
	return true;
} //ApplySkillTargetKnockback

bool ApplySkillTargetOnward( ServerActor* pActor, ServerActor* pTarget, const STARGETID& sSelectTARID, const SKILL::SSPEC& sSKILL_SPEC, SSKILLACTEX& sSKILLACTEX )
{
	if( !pTarget->IsValidBody() )
		return false;

	float		fDist = 0.0f, fSpeed = 0.0f;
	if( !GetPullSpeed( pActor, pTarget, sSKILL_SPEC, fSpeed, fDist ) )
		return false;

	if ( sSelectTARID.GaeaId == pTarget->GetGaeaID() )
	{
		sSKILLACTEX.fPUSH_SPEED = fSpeed;
		sSKILLACTEX.fPUSH_PULL	= fDist; 
		sSKILLACTEX.dwAniType	= sSKILL_SPEC.dwFLAG;
		sSKILLACTEX.bOnward		= true;
	}
	return true;
} //ApplySkillTargetOnward

void ApplySkillTargetStun( ServerActor* pACTOR )
{
	if( !pACTOR->IsValidBody() )
		return;

	pACTOR->DO_STUN_ACTION();
} //ApplySkillTargetStun

bool ApplySkillSelfTeleport( 
							GLGaeaServer* const pGaeaServer, 
							ServerActor*			pActor,
							PGLSKILL				pSkill, 
							const WORD				wSKILL_LVL, 
							const SKILL::SSPEC&	sSKILL_SPEC,
							const STARGETID&		TargetID, 
							const WORD&			wTARNUM, 
							const STARID*			sTARIDS )
{
	float		fRatio = sSKILL_SPEC.fVAR1;
	if( fRatio < 1.0f )
	{
		fRatio = min( 1.0f, fRatio );
		fRatio = max( 0.0f, fRatio );
		if( fRatio <= sc::Random::RANDOM_POS() ) 
			return false;
	} //if

	if( !pActor->CheckStateToGo() )
		return false;

	// where
	D3DXVECTOR3				vDestPos = TargetID.vPos;				// specific destination from a client. that needs checking about it

	D3DXVECTOR3		vDestPosF;
	D3DXVECTOR3		vDist = vDestPos - pActor->GetPosition();

	GLLandMan* pLandMan = pActor->GetLandMan();
	for ( DWORD _i = wTARNUM; _i; --_i )
	{		
		ServerActor* const pActor = pGaeaServer->GetTarget(pLandMan, sTARIDS[_i - 1]);
		if ( pActor == NULL )
			continue;

		if( pActor->GetNowHP() == 0 )
			continue;

		if( pActor->GetCrow() == CROW_MOB )
		{
			GLCrow* const pCrow = dynamic_cast<GLCrow*>( pActor );
			if( pCrow->IsCheckedActFlags( EMCROWACT_BOSS | EMCROWACT_POSHOLD | EMCROWACT_DIRHOLD ) ||
				pCrow->IsNoAllPenalty() )
			{
				continue;
			} //if
		} //if

		vDestPosF = pActor->GetPosition() + vDist;
		vDestPosF.y = vDestPos.y;
		pActor->MoveTo( &vDestPosF, true, MOVE_OPTION_NOT_TURN_IDLE );
	}	

	return true;
} //ApplySkillSelfTeleport

/**
 * 
 */
bool ApplySkillSelfSwapPos( ServerActor*			pActor,
							PGLSKILL				pSkill, 
							const WORD				wSKILL_LVL, 
							const SKILL::SSPEC&		sSKILL_SPEC,
							const WORD&				wTARNUM, 
							const STARID*			sTARIDS,
							GLLandMan*				pLandMan )
{
	if( wTARNUM == 0 )
		return false;

	STARGETID			sActId( pActor->GetCrow(), pActor->GetGaeaID() );
	STARGETID			sTarId( sTARIDS[0].GETCROW(), sTARIDS[0].GETID() );

	ServerActor*	pTarget = pActor->GetTarget( sTarId );
	if( !pTarget )
		return false;

	if( !pTarget->IsValidBody() )
		return false;

	if( !pActor->CheckStateToGo() )
		return false;

	SSKILLACTEX			sSKILLACTEX;
	sSKILLACTEX.idSKILL		= pSkill->GetId();
	sSKILLACTEX.wSKILL_LVL	= wSKILL_LVL;

	DWORD		dwSwapFlag = sSKILL_SPEC.dwFLAG;
	if( pTarget->GetCrow() == CROW_MOB )
	{
		GLCrow*		pMob = dynamic_cast<GLCrow*>( pTarget );
		if( pMob->IsCheckedActFlags( EMCROWACT_BOSS | EMCROWACT_POSHOLD | EMCROWACT_DIRHOLD ) ||
			pMob->IsNoAllPenalty() )
		{
			if( dwSwapFlag == 0 )
				dwSwapFlag = 1;
			else if( dwSwapFlag == 2 )
				return false;
		} //if
	} //if

	float		fDist = 0.0f;
	if( !GetPullSpeed( pActor, pTarget, sSKILL_SPEC, sSKILLACTEX.fPUSH_SPEED, fDist ) )
		return false;

	sSKILLACTEX.fPUSH_PULL	= fDist;
	sSKILLACTEX.dwAniType	= AN_SUB_PULLIN;

	pLandMan->SwapPosAct( sActId, sTarId, sSKILLACTEX.fPUSH_SPEED, sSKILLACTEX.dwAniType, dwSwapFlag );

	return true;
} //ApplySkillSelfSwapPos

bool ApplySkillSelfHallucinate( GLChar* pChar, PGLSKILL pSkill, const WORD wSKILL_LVL, const SKILL::SSPEC& sSKILL_SPEC)
{
	if ( sSKILL_SPEC.dwFLAG & EMSPEC_HALLUCINATE_DWORD_NONTARGETABLE )
	{
		pChar->ResetTargetID();
		GLMSG::SNETPC_BEGIN_SKILL_FC NetMsg;
		NetMsg.sSkillID = pSkill->GetId();
		NetMsg.wLevel = wSKILL_LVL;
		NetMsg.sTargetID = STARGETID(pChar->GetCrow(), pChar->GetGaeaID());
		pChar->SendToClient( &NetMsg );
	}
	return true;
} // ApplySkillSelfHallucinate