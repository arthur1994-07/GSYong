#pragma once

#include "../Widget/GLWidgetScript.h"

struct SCHARSKILL;

class GLSkillTooltip
{	
private :
	GLSkillTooltip();
	GLSkillTooltip( const GLSkillTooltip& value );
	~GLSkillTooltip();

public :
	static GLSkillTooltip& GetInstance();

private :
	GLSKILL*		m_pSkill;
	SCHARSKILL*		m_pLearnedSkill;

public :
	bool GetHeader( const SNATIVEID& sID, LuaTable& tbHeader, WORD wSlotType, int nLevel = -1 );
	bool GetTooltip( const SNATIVEID& sID, LuaTable& tbTooltip, WORD wSlotType, int nLevel = -1, int iDisguise = DISGUISE_NONE );

private :
	bool SaveSkill( const SNATIVEID& sID ); 

private :
	void GenName( LuaTable& tb );
	void GenLevel( LuaTable& tb, int nLevel );
	void GenGrade( LuaTable& tb );
	void GenAttribute( LuaTable& tb );
	void GenType( LuaTable& tb );
	void GenActionType( LuaTable& tb );
	void GenApply( LuaTable& tb );
	void GenUseItem( LuaTable& tb );
	void GenUseCondition( LuaTable& tb, int nLevel );
	void GenBasicAttribute( LuaTable& tb, int nLevel );
	void GenRequestSkill( LuaTable& tb );
	void GenDemandMoney( LuaTable& tb, INT nLevel );
	void GenTarget( LuaTable& tb );
	void GenArea( LuaTable& tb );
	void GenPart( LuaTable& tb );
	void GenRange( LuaTable& tb );
	void GenApplyType( LuaTable& tb, int nLevel );
	void GenSideEffect( LuaTable& tb, int nLevel );
	void GenSpecialEffect( LuaTable& tb, int nLevel );
	void GenSpecialEffectCA( LuaTable& tb, int nLevel );
	void GenSpecialAbility( LuaTable& tb );
	void GenSubSkill( LuaTable& tb, int nLevel );
	void GenDesc( LuaTable& tb, WORD wSlotType, int nLevel );
	void GenBlow( LuaTable& tb, int nLevel );
	void GenDisguise( LuaTable& tb, int iDisguise );
	// 스킬 요구치
	void GenRequire( LuaTable& tb, int nLevel );

public :
	LuaTable CalcDamage( bool bMyChar, WORD wLevel );
};
