#include "pch.h"
#include "../SigmaCore/Math/Random.h"
#include "./GLogicEx.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLCHARLOGIC::RESETSKEFF(size_t i)
{
    if (i < SKILLFACT_SIZE)
	{
        m_sSKILLFACT[i].RESET();
	}
}

void GLCHARLOGIC::RESETAGESKEFF(size_t i)
{ 
    if (i < SKILLFACT_SIZE)
        m_sSKILLFACT[i].fAGE = 0.0f;
}

void GLCHARLOGIC::SetSkillFact(size_t i, const SSKILLFACT& Fact)
{
    if (i < SKILLFACT_SIZE)
        m_sSKILLFACT[i] = Fact;
}

void GLCHARLOGIC::SetSkillFact(size_t i, BOOL Enable)
{
    if (i < SKILLFACT_SIZE)
        m_sSKILLFACT[i].bEnable = Enable;
}

const SSKILLFACT& GLCHARLOGIC::GetSkillFact(size_t i) const
{
    if (i < SKILLFACT_SIZE)
    {
        return m_sSKILLFACT[i];
    }
    else
    {
        static SSKILLFACT Temp;
        return Temp;
    }
}

SSKILLFACT& GLCHARLOGIC::GetSkillFact(size_t i)
{
    if (i < SKILLFACT_SIZE)
    {
        return m_sSKILLFACT[i];
    }
    else
    {
        static SSKILLFACT Temp;
        return Temp;
    }
}

const SNATIVEID& GLCHARLOGIC::GetSkillFactId(size_t i) const
{
    if (i < SKILLFACT_SIZE)
    {
        return m_sSKILLFACT[i].sNATIVEID;
    }
    else
    {
        static SNATIVEID Temp(NATIVEID_NULL());
        return Temp;
    }
}

SNATIVEID& GLCHARLOGIC::GetSkillFactId(size_t i)
{
    if (i < SKILLFACT_SIZE)
    {
        return m_sSKILLFACT[i].sNATIVEID;
    }
    else
    {
        static SNATIVEID Temp(NATIVEID_NULL());
        return Temp;
    }
}

float GLCHARLOGIC::GetSkillFactAge(size_t i) const
{
    if (i < SKILLFACT_SIZE)
        return m_sSKILLFACT[i].fAGE;
    else
        return 0.0f;
}

void GLCHARLOGIC::RemoveSkillFactofSimpact(const SNATIVEID& skill_id, const EMIMPACT_ADDON* emSPEC_ADDON)
{
	for ( DWORD _i = CROW_SKILLFACT_SIZE; _i; --_i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - _i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if ( sSKEFF.bEnable == false )
			continue;		

		if ( sSKEFF.sNATIVEID != skill_id )
			continue;

		std::vector<SKILL::SIMPACT_ADDON_LEVEL>::iterator _iter;

		for( int i = 0; i < SKILL::MAX_ADDON; ++i )
		{
			_iter = sSKEFF.vecADDON.begin();

			while( _iter != sSKEFF.vecADDON.end() )
			{
				if( _iter->emADDON == emSPEC_ADDON[i] )
				{
					sSKEFF.vecADDON.erase(_iter);
					break;
				}

				++_iter;
			}
		}

		return;
	}
}

void GLCHARLOGIC::RemoveSkillFactofSpecial(const SNATIVEID& skill_id, const EMSPEC_ADDON* emSPEC_ADDON)
{
	for ( DWORD _i = CROW_SKILLFACT_SIZE; _i; --_i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - _i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if ( sSKEFF.bEnable == false )
			continue;		

		if ( sSKEFF.sNATIVEID != skill_id )
			continue;

		std::vector<SKILL::SSPEC_ADDON_LEVEL>::iterator _iter;

		for( int i = 0; i < SKILL::MAX_ADDON; ++i )
		{
			_iter = sSKEFF.vecSPEC.begin();

			while( _iter != sSKEFF.vecSPEC.end() )
			{
				if( _iter->emSPEC == emSPEC_ADDON[i] )
				{
					sSKEFF.vecSPEC.erase(_iter);
					break;
				}

				++_iter;
			}
		}

		return;
	}
}
void GLCHARLOGIC::RemoveSkillFactBySkillID(const SNATIVEID& skill_id)
{
    for ( DWORD _i = SKILLFACT_SIZE; _i; --_i)
    {
        SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - _i];
        if (sSKEFF.sNATIVEID == NATIVEID_NULL())
            continue;

        if ( sSKEFF.bEnable == false )
            continue;		

        if ( sSKEFF.sNATIVEID != skill_id )
            continue;

        UPDATE_DATA_END(SKILLFACT_SIZE - _i);
    }
}

void GLCHARLOGIC::RemoveSkillFactofNormalBuf()
{
	for ( DWORD _i = SKILLFACT_SIZE_NORMAL; _i; --_i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE_NORMAL - _i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;

		if ( sSKEFF.bEnable == false )
			continue;		

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSKEFF.sNATIVEID );
		
		if( pSkill == NULL )
			continue;

		if( pSkill->m_sBASIC.emACTION_TYPE != SKILL::EMACTION_TYPE_NORMAL && 
			pSkill->m_sBASIC.emACTION_TYPE != SKILL::EMACTION_TYPE_BUFF )
			continue;

		UPDATE_DATA_END(SKILLFACT_SIZE_NORMAL - _i);
	}
}

void GLCHARLOGIC::RemoveSkillFactofNormalAll()
{
	for ( DWORD i = SKILLFACT_SIZE_NORMAL; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[ SKILLFACT_SIZE_NORMAL - i ];
		if ( sSKEFF.sNATIVEID == NATIVEID_NULL() )
			continue;

		if ( sSKEFF.bEnable == false )
			continue;		

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSKEFF.sNATIVEID );

		if( pSkill == NULL )
			continue;

		UPDATE_DATA_END( SKILLFACT_SIZE_NORMAL - i );
	}
}

void GLCHARLOGIC::RemoveSkillFactBySpecialForce(const EMSPEC_ADDON emSPEC_ADDON )
{
	for ( DWORD _i = SKILLFACT_SIZE; _i; --_i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - _i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;

		if ( sSKEFF.bEnable == false )
			continue;		

		const DWORD nSpecSize = sSKEFF.vecSPEC.size();
		for ( DWORD _j = nSpecSize; _j ; --_j )
		{
			if (sSKEFF.vecSPEC[nSpecSize - _j].emSPEC == emSPEC_ADDON)
			{
				UPDATE_DATA_END(SKILLFACT_SIZE - _i, FALSE, TRUE);
				break;
			}
		}
	}	
}

void GLCHARLOGIC::RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON )
{
	for ( DWORD _i = SKILLFACT_SIZE; _i; --_i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[SKILLFACT_SIZE - _i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;

		if ( sSKEFF.bEnable == false )
			continue;		

		const DWORD nSpecSize = sSKEFF.vecSPEC.size();
		for ( DWORD _j = nSpecSize; _j ; --_j )
		{
			if (sSKEFF.vecSPEC[nSpecSize - _j].emSPEC == emSPEC_ADDON)
			{
				UPDATE_DATA_END(SKILLFACT_SIZE - _i );
				break;
			}
		}
	}	
}

void GLCHARLOGIC::RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON, int iSlotNum)
{
	if( iSlotNum >= SKILLFACT_SIZE || iSlotNum < 0 ) return;

	SSKILLFACT& sSKEFF = m_sSKILLFACT[iSlotNum];

	if (sSKEFF.sNATIVEID == NATIVEID_NULL()) return;
		
	if ( sSKEFF.bEnable == false )	return;

	const DWORD nSpecSize = sSKEFF.vecSPEC.size();

	for ( DWORD _j = nSpecSize; _j ; --_j )
	{
		if (sSKEFF.vecSPEC[nSpecSize - _j].emSPEC == emSPEC_ADDON)
		{			
			UPDATE_DATA_END(iSlotNum);
			break;
		}
	}
}

void GLCROWLOGIC::RemoveSkillFactBySkillID(const SNATIVEID& skill_id)
{
    for ( DWORD _i = CROW_SKILLFACT_SIZE; _i; --_i)
    {
        SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - _i];
        if (sSKEFF.sNATIVEID == NATIVEID_NULL())
            continue;
        if ( sSKEFF.bEnable == false )
            continue;		

        if ( sSKEFF.sNATIVEID != skill_id )
            continue;

        UPDATE_DATA_END(CROW_SKILLFACT_SIZE - _i);
    }
}

void GLCROWLOGIC::RemoveSkillFactBySpecial(const EMSPEC_ADDON emSPEC_ADDON)
{
	for (size_t i = CROW_SKILLFACT_SIZE; i; --i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if (!sSKEFF.bEnable)
			continue;		

		const size_t SpecSize = sSKEFF.vecSPEC.size();
		for (size_t j = SpecSize; j ; --j)
		{
			if (sSKEFF.vecSPEC[SpecSize - j].emSPEC == emSPEC_ADDON)
			{
				UPDATE_DATA_END(CROW_SKILLFACT_SIZE - i);
				break;
			}
		}
	}
}

void GLCROWLOGIC::RemoveSkillFactofSpecial(const SNATIVEID& skill_id, const EMSPEC_ADDON* emSPEC_ADDON)
{
	for ( DWORD _i = CROW_SKILLFACT_SIZE; _i; --_i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - _i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if ( sSKEFF.bEnable == false )
			continue;		

		if ( sSKEFF.sNATIVEID != skill_id )
			continue;

		std::vector<SKILL::SSPEC_ADDON_LEVEL>::iterator _iter;

		for( int i = 0; i < SKILL::MAX_ADDON; ++i )
		{
			_iter = sSKEFF.vecSPEC.begin();

			while( _iter != sSKEFF.vecSPEC.end() )
			{
				if( _iter->emSPEC == emSPEC_ADDON[i] )
				{
					sSKEFF.vecSPEC.erase(_iter);
				}

				++_iter;
			}
		}

		return;
	}
}

void GLCROWLOGIC::RemoveSkillFactofSimpact(const SNATIVEID& skill_id, const EMIMPACT_ADDON* emSPEC_ADDON)
{
	for ( DWORD _i = CROW_SKILLFACT_SIZE; _i; --_i)
	{
		SSKILLFACT& sSKEFF = m_sSKILLFACT[CROW_SKILLFACT_SIZE - _i];
		if (sSKEFF.sNATIVEID == NATIVEID_NULL())
			continue;
		if ( sSKEFF.bEnable == false )
			continue;		

		if ( sSKEFF.sNATIVEID != skill_id )
			continue;

		std::vector<SKILL::SIMPACT_ADDON_LEVEL>::iterator _iter;

		for( int i = 0; i < SKILL::MAX_ADDON; ++i )
		{
			_iter = sSKEFF.vecADDON.begin();

			while( _iter != sSKEFF.vecADDON.end() )
			{
				if( _iter->emADDON == emSPEC_ADDON[i] )
				{
					sSKEFF.vecADDON.erase(_iter);
					break;
				}

				++_iter;
			}
		}

		return;
	}
}



void GLCHARLOGIC::SetFactTime( const float fVAR, const DWORD dwFLAG )
{
    for(int i = 0; i < SKILLFACT_SIZE; ++i )
    {
		if ( m_sSKILLFACT[i].IsHaveExpiredTime() )
			continue;

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sSKILLFACT[i].sNATIVEID );
        if ( pSkill )
        {
            const float fSkillLIFE = pSkill->m_sAPPLY.sDATA_LVL[ m_sSKILLFACT[i].wLEVEL].fLIFE;
            // 버프, 디버, 리미트, 인가?
            if ( CHECK_FLAG_SKILL( dwFLAG, pSkill->m_sBASIC.emACTION_TYPE ) )  
            {
                //m_sSKILLFACT[i].fAGE  *= fVAR;
                //m_sSKILLFACT[i].fLIFE *= fVAR;
				m_sSKILLFACT[i].fAGE  += (1*fVAR) + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE);
				m_sSKILLFACT[i].fLIFE += (1*fVAR) + GetInc_SkillDurationTAdd(pSkill->m_sBASIC.dwGRADE);

                //LIMIT( m_sSKILLFACT[i].fAGE, m_sSKILLFACT[i].fLIFE * GLCONST_CHAR::fENDURE_VARY_MAX, 0.0f );
                //LIMIT( m_sSKILLFACT[i].fLIFE, m_sSKILLFACT[i].fLIFE * GLCONST_CHAR::fENDURE_VARY_MAX, 0.0f );

				LIMIT( m_sSKILLFACT[i].fAGE, m_sSKILLFACT[i].fLIFE + GLCONST_CHAR::fENDURE_VARY_MAX, 0.0f );
				LIMIT( m_sSKILLFACT[i].fLIFE, m_sSKILLFACT[i].fLIFE + GLCONST_CHAR::fENDURE_VARY_MAX, 0.0f );
            }
        }
    }
}


float GLCHARLOGIC::CheckEndureVaryInSPEC( const SKILL::EMACTION_TYPE emACTION_TYPE)
{
    float fSkillLifeInc = 1.0f;
    for(int i = 0; i < SKILLFACT_SIZE; ++i )
    {
        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sSKILLFACT[i].sNATIVEID );
        if ( pSkill )
        {
            const size_t SpecSize = m_sSKILLFACT[i].vecSPEC.size();
            for (size_t j = SpecSize; j ; --j)
            {
                EMSPEC_ADDON emSPEC = m_sSKILLFACT[i].vecSPEC[SpecSize - j].emSPEC;
                SKILL::SSPEC sSPEC  = m_sSKILLFACT[i].vecSPEC[SpecSize - j].sSPEC;
                if (emSPEC == EMSPECA_ENDURE_VARY)
                {
                    if ( CHECK_FLAG_SKILL( sSPEC.dwFLAG, emACTION_TYPE ) ) 
                        fSkillLifeInc *= sSPEC.fVAR2;
                }
            }
        }
    }
    LIMIT( fSkillLifeInc, GLCONST_CHAR::fENDURE_VARY_MAX, 0.0f );
    return fSkillLifeInc;
}

BOOL GLCHARLOGIC::DoItemSkill( const PGLSKILL pSkill, WORD wEMLinkType, std::vector<sItemSkill>& vinfo )
{
	if( pSkill == NULL ) return FALSE;

	WORD wWeapon[SLOT_NSIZE_S_3] = { SLOT_RHAND, SLOT_LHAND, SLOT_RHAND_S, SLOT_LHAND_S, SLOT_UPPER, SLOT_LOWER };

	ITEM::SISKILL sISkill;

	EMITEM_SKILL_LINK eType = (EMITEM_SKILL_LINK)wEMLinkType;

	for ( int i = 0; i < SLOT_NSIZE_S_3; ++i )
	{
		const SITEM* pITEM = GET_SLOT_ITEMDATA ( EMSLOT(wWeapon[i]) );
		const SITEMCUSTOM& sItemCustom = GET_SLOT_ITEM( EMSLOT( wWeapon[i] ) );

		if( pITEM == NULL ) continue;

		//if( pITEM->sItemSkill.sSkillID == NATIVEID_NULL() ) continue;
		const SNATIVEID& sLinkSkillID = sItemCustom.GetLinkSkillID();
		if ( false == sLinkSkillID.IsValidNativeID() )
			continue;

		if( eType != EMITEM_SKILL_SELF   && 
			eType != EMITEM_SKILL_ENERMY && 
			eType != EMITEM_SKILL_MOB ) {
			continue;
		}

		//if( ( pITEM->sItemSkill.emSkillTarget == eType ) ||
		//	( pITEM->sItemSkill.emSkillTarget != eType && pITEM->sItemSkill.emSkillTarget == EMITEM_SKILL_ALL) ) {
		if( ( sItemCustom.GetLinkSkillTarget() == eType ) ||
			( sItemCustom.GetLinkSkillTarget() != eType && sItemCustom.GetLinkSkillTarget() == EMITEM_SKILL_ALL ) )
		{

			sISkill = pITEM->sItemSkill;

			//if( sc::Random::getInstance().IsSucceed(sISkill.fOccuRate*100) && sISkill.wSkillLv > 0 )
			//{
			//	vinfo.push_back( sItemSkill(sISkill.sSkillID, sISkill.wSkillLv - 1 ));
			//}
			if ( sc::Random::getInstance().IsSucceed( sItemCustom.GetLinkSkillOccurRate() * 100 ) &&
				sItemCustom.GetLinkSkillLevel() >= 0 )
			{
				vinfo.push_back( sItemSkill( sLinkSkillID, sItemCustom.GetLinkSkillLevel() ) );
			}
		}
	}


	return ( vinfo.size() > 0 ) ? TRUE:FALSE;

}