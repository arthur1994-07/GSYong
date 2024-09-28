#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/GLogicData.h"

#include "./GLItemBasicSet.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������ �⺻ �������� ������;
class GLItemBasicStatMan : public CSingletone< GLItemBasicStatMan >
{
protected:
	friend CSingletone;


public:
	// �⺻ �ɷ�ġ ���� �����͸� �ҷ��´�;
	// ���� �ѹ��� ȣ��ȴ�;
	const bool LoadData ( void );


public:
	// �⺻ �ɷ�ġ : ���ݷ�, Index �� �˸��� ���ݷ��� �������� �����´�;
	const GLPADATA GetAttackDamage_MakeItem ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex ) const;
	const GLPADATA GetAttackDamage_DropItem ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex ) const;

	// �⺻ �ɷ�ġ : ����, Index �� �˸��� ������ �������� �����´�;
	const WORD GetDefence_MakeItem ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex ) const;
	const WORD GetDefence_DropItem ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex ) const;

	// �⺻ �ɷ�ġ ��� �� ���ݷ� ��Ʈ�� ������ ���Ѵ�;
	const int GetSizeAttackItemSetMan ( const EMITEMLEVEL _emItemLevel, const int _nType ) const;

	// �⺻ �ɷ�ġ ��� �� ���� ��Ʈ�� ������ ���Ѵ�;
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