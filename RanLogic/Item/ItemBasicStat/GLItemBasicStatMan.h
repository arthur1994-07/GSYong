#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/GLogicData.h"

#include "./GLItemBasicSet.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 아이템 기본 랜덤스탯 관리자;
class GLItemBasicStatMan : public CSingletone< GLItemBasicStatMan >
{
protected:
	friend CSingletone;


public:
	// 기본 능력치 랜덤 데이터를 불러온다;
	// 최초 한번만 호출된다;
	const bool LoadData ( void );


public:
	// 기본 능력치 : 공격력, Index 에 알맞은 공격력을 랜덤으로 가져온다;
	const GLPADATA GetAttackDamage_MakeItem ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex ) const;
	const GLPADATA GetAttackDamage_DropItem ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex ) const;

	// 기본 능력치 : 방어력, Index 에 알맞은 방어력을 랜덤으로 가져온다;
	const WORD GetDefence_MakeItem ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex ) const;
	const WORD GetDefence_DropItem ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex ) const;

	// 기본 능력치 등급 별 공격력 세트의 개수를 구한다;
	const int GetSizeAttackItemSetMan ( const EMITEMLEVEL _emItemLevel, const int _nType ) const;

	// 기본 능력치 등급 별 방어력 세트의 개수를 구한다;
	const int GetSizeDefenceItemSetMan ( const EMITEMLEVEL _emItemLevel, const int _nType ) const;


private:
	CBasicItemAttackTypeSetMan m_sAttackItemSetTypeManList[ EMITEM_LEVEL_NSIZE ];
	CBasicItemDefenceTypeSetMan m_sDefenceItemSetTypeManList[ EMITEM_LEVEL_NSIZE ];


private:
	GLItemBasicStatMan () { }


public:
	~GLItemBasicStatMan () { }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////