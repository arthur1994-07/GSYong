#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/GLogicData.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 아이템 링크 스킬;
struct SLINK_SKILL_INFO
{
public:
	inline const EMITEM_SKILL_LINK GetTarget ( void ) const { return m_emTarget; }
	inline const SNATIVEID& GetSkillID ( void ) const { return m_sSkillID; }
	inline const int GetMinLevel ( void ) const { return m_nMinLevel; }
	inline const int GetMaxLevel ( void ) const { return m_nMaxLevel; }
	inline const float GetOccurRate ( void ) const { return m_fOccurRate; }

	inline void SetTarget ( const EMITEM_SKILL_LINK _emTarget ) { m_emTarget = _emTarget; }
	inline void SetSkillID ( const SNATIVEID& _sSkillID ) { m_sSkillID = _sSkillID; }
	inline void SetMinLevel ( const int _nLevel )
	{
		if ( _nLevel < 0 )
			m_nMinLevel = 0;
		else if ( SKILL::MAX_LEVEL <= _nLevel )
			m_nMinLevel = SKILL::MAX_LEVEL-1;
		else
			m_nMinLevel = _nLevel;
	}
	inline void SetMaxLevel ( const int _nLevel )
	{
		if ( _nLevel < 0 )
			m_nMaxLevel = 0;
		else if ( SKILL::MAX_LEVEL <= _nLevel )
			m_nMaxLevel = SKILL::MAX_LEVEL-1;
		else
			m_nMaxLevel = _nLevel;
	}
	inline void SetOccurRate ( const float _fOccurRate ) { m_fOccurRate = _fOccurRate; }


private:
	EMITEM_SKILL_LINK m_emTarget;		// 타겟;

	SNATIVEID m_sSkillID;				// 스킬 ID;

	int m_nMinLevel;					// 최소 스킬 레벨;
	int m_nMaxLevel;					// 최대 스킬 레벨;

	float m_fOccurRate;					// 스킬 발동 확률;


public:
	SLINK_SKILL_INFO ()
		: m_emTarget ( EMITEM_SKILL_ALL )
		, m_sSkillID ( false )
		, m_nMaxLevel ( 0 )
		, m_nMinLevel ( 0 )
		, m_fOccurRate ( 0.f )
	{
	}
	~SLINK_SKILL_INFO () { }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 아이템 링크 스킬 세트;
class CItemLinkSkillSet
{
public:
	const bool LoadData ( LuaPlus::LuaObject& _sMember );


public:
	// 아이템 링크 스킬을 랜덤으로 획득한다;
	void GetLinkSkill_Random (
		EMITEM_SKILL_LINK& _emRefSkillTarget,
		SNATIVEID& _sRefSkillID,
		int& _nRefSkillLevel,
		float& _fRefSkillOccurRate ) const;

	// 링크 스킬 개수를 구한다;
	const int GetSizeLinkSkill ( void ) const;


private:
	std::vector< SLINK_SKILL_INFO > m_vecItemLinkSkill;


public:
	CItemLinkSkillSet () { }
	~CItemLinkSkillSet () { }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 아이템 링크 스킬 관리자;
class GLItemLinkSkillMan : public CSingletone< GLItemLinkSkillMan >
{
protected:
	friend CSingletone;


public:
	// 링크 스킬 랜덤 데이터를 불러온다;
	// 최초 한번만 호출된다;
	const bool LoadData ( void );


public:
	// 아이템 링크 스킬을 랜덤으로 획득한다;
	void GetLinkSkill (
		const int _nIndex,
		EMITEM_SKILL_LINK& _emRefSkillTarget,
		SNATIVEID& _sRefSkillID,
		int& _nRefSkillLevel,
		float& _fRefSkillOccurRate ) const;

	// 링크 스킬 세트의 개수를 구한다;
	const int GetSizeLinkSkillSet ( void ) const;


private:
	std::vector< CItemLinkSkillSet > m_vecItemLinkSkillSet;


private:
	GLItemLinkSkillMan () { }


public:
	~GLItemLinkSkillMan () { }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////