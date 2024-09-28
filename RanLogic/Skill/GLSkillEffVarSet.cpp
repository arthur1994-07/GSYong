#include "pch.h"

#include "../GLogicEx.h"

namespace NSSKILL_EFFSET
{
	// struct Begin ****************************************************************************************************************/

	_sScreenThrow::_sScreenThrow()
	{

	}

	BOOL _sScreenThrow::IsOn(WORD wEffID)
	{
		if( wEffID >= GLCONST_CHAR::SCREEN_EFFECT_NUM ) return FALSE;

		return sEffInfo[wEffID].bEffID;
	}

	void _sScreenThrow::Reset()
	{
		for( int i = 0; i < GLCONST_CHAR::SCREEN_EFFECT_NUM; i++ )
			sEffInfo[i].Reset();
	}

	void _sScreenThrow::ResetValue(WORD wEffID)
	{
		if( wEffID >= GLCONST_CHAR::SCREEN_EFFECT_NUM ) return;

		sEffInfo[wEffID].Reset();
	}

	void _sScreenThrow::TurnOn( const STARGETID& sID, WORD wEffID)
	{
		if( wEffID >= GLCONST_CHAR::SCREEN_EFFECT_NUM ) return;
		
		sEffInfo[wEffID].bEffID   = TRUE;
		sEffInfo[wEffID].sTagetID = sID;
	}

	STARGETID& _sScreenThrow::GetTargetID(WORD wEffID)
	{
		return sEffInfo[wEffID].sTagetID;
	}

	void _sFeignDeath::End( GLCHARLOGIC* const pCharLogic, BOOL bForceEnd )
	{
		if( pCharLogic == NULL ) return;

		//if( bOn )
		pCharLogic->End_FeignDeath( bForceEnd );
	}

	void _sSpecialImmune::TurnOn( BYTE iIndex )
	{
		bOn = TRUE;

		if( iIndex != 0 ) vImmuneList.push_back( iIndex );
	}

	BOOL _sSpecialImmune::IsImmune( BYTE iIndex ) const
	{
		for ( vList_const_iter iter = vImmuneList.begin();
			iter != vImmuneList.end(); ++iter )
		{
			const BYTE iTempIndex = *iter;
			if ( iIndex == iTempIndex )
				return TRUE;
		}

		return FALSE;
	}

	void _sActiveDamage::TurnOn( const DWORD& idSkill, const WORD& wLv, const int& iCnt, const int& iDamage )
	{
		bOn = TRUE;

		if( iCnt == 0 && iDamage == 0 ) return;

		if( IsSkill(idSkill) )
		{
			BYTE iValue = ( iCnt > 0 && iDamage > 0 ) ? VALUE_BOTH : VALUE_OR;

			vAccuList.push_back( sAccuInfo( idSkill, wLv, iCnt, iDamage, iValue ) );
		}
	}

	BOOL _sActiveDamage::UpdateSkill( const DWORD& idSkill, const WORD& wLv, const int& iCnt, const int& iDamage )
	{
		vList_iter _iter = vAccuList.begin();

		while( _iter != vAccuList.end() )
		{
			if( _iter->dwSkillId == idSkill )
			{
				_iter->iAccuCnt = iCnt;
				_iter->iAccuDmg = iDamage;
				_iter->wSkillLv = wLv;
				_iter->iCheck   = ( iCnt > 0 && iDamage > 0 ) ? VALUE_BOTH : VALUE_OR;

				return FALSE;
			}

			++_iter;
		}

		return TRUE;
	}

	BOOL _sActiveDamage::IsSkill( const DWORD& dwID )
	{
		vList_iter _iter = vAccuList.begin();

		while( _iter != vAccuList.end() )
		{
			if( _iter->dwSkillId == dwID ) 	return FALSE;

			++_iter;
		}

		return TRUE;
	}

	void _sActiveDamage::Accumulate( DWORD dwDamage, const EMCROW eCrow, const DWORD dwID )
	{
		if( !bOn ) {
			ResetValue( TRUE );
			return;
		}

		vList_iter _iter = vAccuList.begin();

		while( _iter != vAccuList.end() )
		{
			sAccuInfo* pData = &(*_iter);

			if( pData->iAccuCnt > 0 ) {
				pData->iAccuCnt -= 1;
				pData->sID = STARGETID( eCrow, dwID );
			}

			if( pData->iAccuDmg > 0 ) {
				pData->iAccuDmg -= dwDamage;
				pData->sID = STARGETID( eCrow, dwID );
			}

			++_iter;
		}
	}

	BOOL _sActiveDamage::Invoke( WORD& wLv, DWORD& dwSkill, STARGETID& sAID )
	{
		vList_iter _iter = vAccuList.begin();

		while( _iter != vAccuList.end() )
		{
			BOOL bInvoke = FALSE;

			switch( _iter->iCheck )
			{
			case VALUE_OR   :
				bInvoke = (_iter->iAccuDmg <= 0 && _iter->iAccuCnt <= 0) ? TRUE : FALSE;
				break;
			case VALUE_BOTH :
				bInvoke = (_iter->iAccuDmg <= 0 || _iter->iAccuCnt <= 0) ? TRUE : FALSE;
				break;
			}

			if( bInvoke )  {

				SNATIVEID sID = SNATIVEID( _iter->dwSkillId );

				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sID );

				for ( DWORD i = 0; pSkill && i < pSkill->m_sAPPLY.vecSPEC.size(); ++i )
				{
					const EMSPEC_ADDON		emAddon = pSkill->m_sAPPLY.vecSPEC[i].emSPEC;
					const SKILL::SSPEC&		sSPEC   = pSkill->m_sAPPLY.vecSPEC[i].sSPEC[ _iter->wSkillLv ];

					if( emAddon == EMSPECA_ACTIVATE_DAMAGE )
					{
						wLv     = _iter->wSkillLv;
						dwSkill = sSPEC.dwNativeID.dwID;
						sAID    = _iter->sID;
						
						vAccuList.erase( _iter );

						return TRUE;
					}
				}
			}

			++_iter;
		}

		return FALSE;
	}

	void _sActiveDamage::DeleteSkill( const DWORD& dwID )
	{
		vList_iter _iter = vAccuList.begin();

		while( _iter != vAccuList.end() )
		{
			sAccuInfo* pData = &(*_iter);

			if( pData->dwSkillId == dwID )
			{
				vAccuList.erase( _iter );
				return;
			}

			++_iter;
		}
	}

	// struct End ******************************************************************************************************************/

};