#include "pch.h"

#include "GLSkill.h"
#include "../Character/GLCharDefine.h"

#include "GLCountAttackSkill.h"


CSkillCAIdx::CSkillCAIdx() : m_SkillID(NATIVEID_NULL()), m_pcCAData(NULL), m_nFactSlot(-1), m_nSkillLv(-1), m_OrignSkill(NATIVEID_NULL())
{
	m_Apply.reserve(MAX_MEM_SIZE);
	m_Pact.reserve(MAX_MEM_SIZE);
}

CSkillCAIdx::~CSkillCAIdx()
{

}

void CSkillCAIdx::SetData( SNATIVEID sSkill, const SKILL::SSPEC_CA* pData, int iFactSlot, SNATIVEID sOrginSkill, int iSkillLv )
{
	if( pData == NULL ) return;

	m_SkillID  = sSkill;
	m_pcCAData = pData;

	m_nFactSlot = iFactSlot;

	m_nSkillLv = iSkillLv;
	m_OrignSkill = sOrginSkill;
}

void CSkillCAIdx::AllReset()
{
	ResetData();

	m_bUse      = false;
	m_bSuccess	= false;
	m_nFactSlot = -1;
	m_nSkillLv  = -1;
}

void CSkillCAIdx::ResetData()
{
	m_SkillID  = NATIVEID_NULL();
	m_OrignSkill  = NATIVEID_NULL();
	m_pcCAData = NULL;

	m_Apply.clear();
	m_Pact.clear();

	//m_bUse = false;
	//m_nFactSlot = -1;
}

void CSkillCAIdx::ResetEffList()
{
	m_Apply.clear();
	m_Pact.clear();
}

bool CSkillCAIdx::IsAlive( WORD wSlot )
{
	if( m_nFactSlot == wSlot && m_bUse )
	{
		return false;
	}

	return true;
}

/**************************************************************************************************************************************************/


CSkillCAInterface::CSkillCAInterface()
{

}

CSkillCAInterface::~CSkillCAInterface()
{

}

void CSkillCAInterface::GetCAEffList( CSkillCAIdx* psCA, std::vector<int>& rfList )
{
	const SKILL::SSPEC_CA* pData = psCA->GetCaData();

	if( pData == NULL ) return;

	if( pData->bAllApply ) 
	{
		for( int i = 0; i < EMSPECA_CAK_MAX; ++i )
		{
			EMSPEC_ADDON eTemp = static_cast<EMSPEC_ADDON>(COMMENT::SPEC_CA[i]);

			if( eTemp != EMSPECA_NULL )
			{
				rfList.push_back((int)eTemp);
			}
		}
	}
	else
	{
		for( int i = 0; i < SKILL::MAX_ADDON; ++i )
		{
			if( pData->emSPEC[i] != EMSPECA_NULL )
			{
				rfList.push_back((int)pData->emSPEC[i]);
			}
		}
	}
}

bool CSkillCAInterface::IsCAEnable( const PGLSKILL pSkill )
{
	if( pSkill == NULL ) return false;

	// 기본형에서만 작동
	if( pSkill->m_sBASIC.emROLE != SKILL::EMROLE_NORMAL		  ) return false;
	if( pSkill->m_sBASIC.emIMPACT_SIDE != EMIMPACT_SIDE_ENEMY ) return false;

	return true;
}

bool CSkillCAInterface::Cal_LastApplyValueII( SNATIVEID sAttackID, WORD wAttLv, CSkillCAIdx* pAtt, CSkillCAIdx* pTar, const STARGETID& sAt, const STARGETID& sTa, bool bSkillEnd )
{
	pAtt->ResetEffList();
	pTar->ResetEffList();
	
	const PGLSKILL	pSkill	= GLSkillMan::GetInstance().GetData( sAttackID );	

	if( pSkill == NULL ) return false;

	Cal_SideApply( pSkill, wAttLv, pAtt, pTar, sAt, sTa, bSkillEnd );

	/* -----------------------
	   공격자 반격집합 "A"
	   타겟의 반격집합 "B"
	   공격자 효과집합 "C" 
   ---------------------------*/	
		
	std::map< std::string, std::vector<int> > mapList;

	GetCAEffList( pAtt, mapList["A"] );
	GetCAEffList( pTar, mapList["B"] );

	// 예외
	if( sAt.emCrow == CROW_MOB )
		mapList["B"].clear();

	bool bEqual = false;

	if( (pAtt == pTar) || !IsCAEnable(pSkill) ) {
		mapList["A"].clear();
		mapList["B"].clear();

		pAtt->SetUseCA( false );	

		bEqual = true;
	}
	
	std::vector<SKILL::SSPEC_ADDON>::iterator _iter = pSkill->m_sAPPLY.vecSPEC.begin();

	while( _iter != pSkill->m_sAPPLY.vecSPEC.end() )
	{
		mapList["C"].push_back( (int)_iter->emSPEC );

		++_iter;
	}

	std::sort(mapList["C"].begin(), mapList["C"].end());
	std::sort(mapList["B"].begin(), mapList["B"].end());
	std::sort(mapList["A"].begin(), mapList["A"].end());
	// C와B 교집합
	std::set_intersection( mapList["C"].begin(), mapList["C"].end(), mapList["B"].begin(), mapList["B"].end(), std::back_inserter( mapList["CnB"] ));

	std::sort(mapList["CnB"].begin(), mapList["CnB"].end());
	// A, B, C 교집합
	std::set_intersection( mapList["CnB"].begin(), mapList["CnB"].end(), mapList["A"].begin(), mapList["A"].end(), std::back_inserter(mapList["CnBnA"]));
	std::sort(mapList["CnBnA"].begin(), mapList["CnBnA"].end());
	// (C&B) - (C&B&A) 공격자가 최종 받는 효과.
	std::set_difference( mapList["CnB"].begin(), mapList["CnB"].end(), mapList["CnBnA"].begin(), mapList["CnBnA"].end(), std::back_inserter(mapList["(CnB)-(CnBnA)"] ));
	std::sort(mapList["(CnB)-(CnBnA)"].begin(), mapList["(CnB)-(CnBnA)"].end());
	// (C-(C&B)) 
	std::set_difference( mapList["C"].begin(), mapList["C"].end(), mapList["CnB"].begin(), mapList["CnB"].end(), std::back_inserter(mapList["C-(CnB)"] ));
	std::sort(mapList["C-(CnB)"].begin(), mapList["C-(CnB)"].end());
	// (C&B&A) + (C-(C&B)) 타켓이 최종 받는 효과
	std::set_union( mapList["CnBnA"].begin(), mapList["CnBnA"].end(), mapList["C-(CnB)"].begin(), mapList["C-(CnB)"].end(), std::back_inserter( mapList["(CnBnA)+(C-(CnB))"] ) );
	std::sort(mapList["(CnBnA)+(C-(CnB))"].begin(), mapList["(CnBnA)+(C-(CnB))"].end());

	// 공격자가 받을 효과를 넣는다.
	std::vector<int>::iterator _vIterAtt = mapList["(CnB)-(CnBnA)"].begin();

	std::vector<SKILL::SSPEC_ADDON>::iterator   _viter;
	std::vector<SKILL::SSPEC_ADDON*>::iterator   _viterII;

	std::vector<SKILL::SSPEC_ADDON*> vAddon;

	_viter = pSkill->m_sAPPLY.vecSPEC.begin();

	while( _viter != pSkill->m_sAPPLY.vecSPEC.end() )
	{
		vAddon.push_back(&(*_viter));

		++_viter;
	}

	while( _vIterAtt != mapList["(CnB)-(CnBnA)"].end() && !bEqual)
	{
		_viterII = vAddon.begin();

		while( _viterII != vAddon.end() )
		{			
			if( (*_viterII)->emSPEC == (EMSPEC_ADDON)*_vIterAtt )
			{
				pAtt->m_Apply.push_back( *_viterII );

				vAddon.erase(_viterII);
				break;
			}

			++_viterII;
		}

		++_vIterAtt;
	}

	vAddon.clear();

	_viter = pSkill->m_sAPPLY.vecSPEC.begin();

	while( _viter != pSkill->m_sAPPLY.vecSPEC.end() )
	{
		vAddon.push_back(&(*_viter));

		++_viter;
	}

	// 타켓이 받을 효과를 넣는다.
	std::vector<int>::iterator _vIterTar = mapList["(CnBnA)+(C-(CnB))"].begin();

	while( _vIterTar != mapList["(CnBnA)+(C-(CnB))"].end() )
	{
		_viterII = vAddon.begin();

		while( _viterII != vAddon.end() )
		{
			if( (*_viterII)->emSPEC == (EMSPEC_ADDON)*_vIterTar )
			{
				pTar->m_Apply.push_back( *_viterII );

				vAddon.erase(_viterII);
				break;
			}

			++_viterII;
		}
		
		++_vIterTar;
	}

	if( mapList["CnB"].size() > 0   ) pTar->SetUseCA( bSkillEnd ? true : false );
	if( mapList["CnBnA"].size() > 0 ) pAtt->SetUseCA( bSkillEnd ? true : false );

	if( pAtt->m_Apply.size() > 0 || pTar->m_Apply.size() ) return true;

	return false;
} 

void CSkillCAInterface::Cal_SideApply( const PGLSKILL pSkill, WORD wAttLv, CSkillCAIdx* pAtt, CSkillCAIdx* pTar, const STARGETID& sAt, const STARGETID& sTa, bool bSkillEnd )
{	
	if( pSkill == NULL ) return;

	bool bAttAll = false;
	bool bTarAll = false;

	if( pAtt->GetCaData() )	bAttAll = pAtt->GetCaData()->bAllApply;
	if( pTar->GetCaData() ) bTarAll = pTar->GetCaData()->bAllApply;

	// 예외
	if( sAt.emCrow == CROW_MOB )
		bTarAll = false;

	std::vector<SKILL::SIMPACT_ADDON>::iterator _iter = pSkill->m_sAPPLY.vecADDON.begin();

	std::map< std::string, std::vector<int> > mapList;

	while( _iter != pSkill->m_sAPPLY.vecADDON.end() )
	{
		if( _iter->fADDON_VAR[wAttLv] < 0 ) {
			mapList["B"].push_back( (int)_iter->emADDON);
		}

		mapList["A"].push_back( (int)_iter->emADDON);

		++_iter;
	}

	std::vector<int>::iterator _iter1;
	std::vector<SKILL::SIMPACT_ADDON>::iterator _iters;

	std::sort(mapList["A"].begin(), mapList["A"].end());
	std::sort(mapList["B"].begin(), mapList["B"].end());

	// 공격자
	std::set_intersection( mapList["A"].begin(), mapList["A"].end(), mapList["B"].begin(), mapList["B"].end(), std::back_inserter( mapList["AnB"] ));
	std::sort(mapList["AnB"].begin(), mapList["AnB"].end());
	// 타켓
	std::set_difference( mapList["A"].begin(), mapList["A"].end(), mapList["AnB"].begin(), mapList["AnB"].end(), std::back_inserter(mapList["A-(AnB)"] ));
	std::sort(mapList["A-(AnB)"].begin(), mapList["A-(AnB)"].end());
	std::set_union( mapList["A-(AnB)"].begin(), mapList["A-(AnB)"].end(), mapList["AnB"].begin(), mapList["AnB"].end(), std::back_inserter( mapList["A-(AnB)+AnB"] ) );

	if( bAttAll && bTarAll) 
	{		
		_iter1 = mapList["A"].begin();

		while( _iter1 != mapList["A"].end())
		{
			_iters = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( (EMIMPACT_ADDON)*_iter1 ) );
			
			if( _iters != pSkill->m_sAPPLY.vecADDON.end() )
			{
				pTar->m_Pact.push_back( &(*_iters) );
			}

			++_iter1;
		}

		if(mapList["A"].size() > 0 )
		{
			pAtt->SetUseCA( bSkillEnd ? true : false );
			pTar->SetUseCA( bSkillEnd ? true : false );
		}
	}
	else
	if( !bAttAll && bTarAll ) 
	{		
		_iter1 = mapList["AnB"].begin();

		while( _iter1 != mapList["AnB"].end())
		{
			_iters = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( (EMIMPACT_ADDON)*_iter1 ) );

			if( _iters != pSkill->m_sAPPLY.vecADDON.end() )
			{
				pAtt->m_Pact.push_back( &(*_iters) );
			}

			++_iter1;
		}

		_iter1 = mapList["A-(AnB)"].begin();

		while( _iter1 != mapList["A-(AnB)"].end())
		{
			_iters = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( (EMIMPACT_ADDON)*_iter1 ) );

			if( _iters != pSkill->m_sAPPLY.vecADDON.end() )
			{
				pTar->m_Pact.push_back( &(*_iters) );
			}

			++_iter1;
		}

		if( mapList["AnB"].size() > 0 ) pTar->SetUseCA( bSkillEnd ? true : false );
	}
	else
	{
		_iter1 = mapList["A"].begin();

		while( _iter1 != mapList["A"].end())
		{
			_iters = std::find_if( pSkill->m_sAPPLY.vecADDON.begin(), pSkill->m_sAPPLY.vecADDON.end(), SKILL::SIMPACT_ADDON( (EMIMPACT_ADDON)*_iter1 ) );

			if( _iters != pSkill->m_sAPPLY.vecADDON.end() )
			{
				pTar->m_Pact.push_back( &(*_iters) );
			}

			++_iter1;
		}
	}
}

STARGETID CSkillCAInterface::Cal_LastDamage( SNATIVEID sAttackID, WORD wAttLv, CSkillCAIdx* pAtt, CSkillCAIdx* pTar, const STARGETID& sAt, const STARGETID& sTa, bool bSkillEnd )
{
	STARGETID sAtID = sAt;
	STARGETID sTaID = sTa;

	const PGLSKILL	pSkill	= GLSkillMan::GetInstance().GetData( sAttackID );	

	if( pSkill == NULL ) return sTaID;

	if( (sAt == sTa) || (pSkill->m_sAPPLY.sDATA_LVL[wAttLv].fBASIC_VAR >= 0) || sAt.emCrow == CROW_MOB )
	{
		pAtt->SetUseCA( false );
		return sTaID;
	}
	
	bool bAttDamage = false;
	bool bTarDamage = false;

	if( pAtt->GetCaData() )
	    bAttDamage = pAtt->GetCaData()->bDamageApply;
	
	if( pTar->GetCaData() )
		bTarDamage = pTar->GetCaData()->bDamageApply;

	if( bAttDamage  && bTarDamage )  
	{
		pAtt->SetUseCA( bSkillEnd ? true : false );
		pTar->SetUseCA( bSkillEnd ? true : false );
		return sTaID;
	}

	if( !bAttDamage && bTarDamage )   
	{
		pTar->SetUseCA( bSkillEnd ? true : false );
		return sAtID;
	}
	
	return sTaID;
}