#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/GLogicData.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������ ��ũ ��ų;
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
	EMITEM_SKILL_LINK m_emTarget;		// Ÿ��;

	SNATIVEID m_sSkillID;				// ��ų ID;

	int m_nMinLevel;					// �ּ� ��ų ����;
	int m_nMaxLevel;					// �ִ� ��ų ����;

	float m_fOccurRate;					// ��ų �ߵ� Ȯ��;


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
// ������ ��ũ ��ų ��Ʈ;
class CItemLinkSkillSet
{
public:
	const bool LoadData ( LuaPlus::LuaObject& _sMember );


public:
	// ������ ��ũ ��ų�� �������� ȹ���Ѵ�;
	void GetLinkSkill_Random (
		EMITEM_SKILL_LINK& _emRefSkillTarget,
		SNATIVEID& _sRefSkillID,
		int& _nRefSkillLevel,
		float& _fRefSkillOccurRate ) const;

	// ��ũ ��ų ������ ���Ѵ�;
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
// ������ ��ũ ��ų ������;
class GLItemLinkSkillMan : public CSingletone< GLItemLinkSkillMan >
{
protected:
	friend CSingletone;


public:
	// ��ũ ��ų ���� �����͸� �ҷ��´�;
	// ���� �ѹ��� ȣ��ȴ�;
	const bool LoadData ( void );


public:
	// ������ ��ũ ��ų�� �������� ȹ���Ѵ�;
	void GetLinkSkill (
		const int _nIndex,
		EMITEM_SKILL_LINK& _emRefSkillTarget,
		SNATIVEID& _sRefSkillID,
		int& _nRefSkillLevel,
		float& _fRefSkillOccurRate ) const;

	// ��ũ ��ų ��Ʈ�� ������ ���Ѵ�;
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