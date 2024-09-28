#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Character/GLCharData.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 캐릭터 슬롯 데이터;
struct SCHAR_SLOT_DATA
{
public:
	BOOL						SETEXPSKILLS_BYBUF ( se::ByteStream& sByteStream );
	BOOL						SETSKILL_QUICKSLOT ( se::ByteStream& sByteStream );

	BOOL						GETEXPSKILLS_BYBUF ( se::ByteStream &ByteStream ) const;
	BOOL						GETSKILL_QUICKSLOT ( se::ByteStream &ByteStream ) const;

	DWORD						GetExpSkillsDataSize ( void ) const;
	BOOL						GetExpSkillsData ( BYTE* pDest, const DWORD dwDestSize ) const;

	DWORD						GetSkillQuickSlotDataSize ( void ) const;
	BOOL						GetSkillQuickSlotData ( BYTE* pDest, const DWORD dwDestSize ) const;


public:
	inline const bool			IsActiveSlot ( void ) const { return !( m_sStats.IsZERO() && m_wStatsPoint == 0 && m_ExpSkills.empty() && m_dwSkillPoint == 0 ); }


public:
	const SCHAR_SLOT_DATA&	operator = ( const SCHAR_SLOT_DATA& _sSlotData )
	{
		m_sStats = _sSlotData.m_sStats;
		m_ExpSkills = _sSlotData.m_ExpSkills;
		m_wSKILLQUICK_ACT = _sSlotData.m_wSKILLQUICK_ACT;

		memcpy ( m_sSKILLQUICK, _sSlotData.m_sSKILLQUICK, sizeof( SNATIVEID )*EMSKILLQUICK_SIZE );

		m_wStatsPoint = _sSlotData.m_wStatsPoint;
		m_dwSkillPoint = _sSlotData.m_dwSkillPoint;

		return *this;
	}


public:
	SCHARSTATS					m_sStats;								// Stat;
	SKILL_MAP					m_ExpSkills;							// Skill;
	WORD						m_wSKILLQUICK_ACT;						// Skill Quick Slot Active Skill;
	SNATIVEID					m_sSKILLQUICK[ EMSKILLQUICK_SIZE ];		// Skill Quick Slot;

	WORD						m_wStatsPoint;							// 가용 Stat Point;
	DWORD						m_dwSkillPoint;							// 가용 Skill Point;


public:
	SCHAR_SLOT_DATA ()
		: m_wSKILLQUICK_ACT ( 0 )
		, m_wStatsPoint ( 0 )
		, m_dwSkillPoint ( 0 )
	{
		for ( int i = 0; i<EMSKILLQUICK_SIZE; ++i )
			m_sSKILLQUICK[ i ] = NATIVEID_NULL();

		m_ExpSkills.clear();
	}
};


class GLCharSlot
{
public:
	const bool					SaveCharSlot ( const WORD _wChaSlot, const SCHAR_SLOT_DATA& _sSlotData );

	const SCHAR_SLOT_DATA&		GetSlotData ( const WORD _wChaSlot ) const;

	void						SkillPointUp ( const WORD _wSkillPoint );
	void						StatPointUp ( const WORD _wStatPoint );


private:
	DWORD						m_dwChaDBNum;

	SCHAR_SLOT_DATA				m_arrSlotData[ EMCHAR_SLOT_DATA_SIZE ];


public:
	GLCharSlot ( const DWORD _dwChaDBNum )
		: m_dwChaDBNum ( _dwChaDBNum )
	{
		for ( int i = 0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
			m_arrSlotData[ i ] = SCHAR_SLOT_DATA();
	}

	~GLCharSlot () { }
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 캐릭터 슬롯 관리자;
class GLCharSlotMan : public CSingletone< GLCharSlotMan >
{
protected:
	friend CSingletone;

	typedef std::map< DWORD, GLCharSlot >			MAP_CHARSLOT;
	typedef MAP_CHARSLOT::iterator					MAP_CHARSLOT_ITER;
	typedef MAP_CHARSLOT::const_iterator			MAP_CHARSLOT_CITER;
	typedef MAP_CHARSLOT::value_type				MAP_CHARSLOT_VALUE;


public:
	const bool					LoadData ( void );


public:
	const bool					SaveCharSlot ( const DWORD _dwChaDBNum, const WORD _wChaSlot, const SCHAR_SLOT_DATA& _sSlotData );

	const SCHAR_SLOT_DATA&		GetSlotData ( const DWORD _dwChaDBNum, const WORD _wChaSlot ) const;

	void						SkillPointUp ( const DWORD _dwChaDBNum, const WORD _wSkillPoint );
	void						StatPointUp ( const DWORD _dwChaDBNum, const WORD _wStatPoint );


public:
	inline const int			GetOpenSlotCount ( void ) const { return m_nOpenSlotCount; }
	inline const int			GetDefaultActiveSlotCount ( void ) const { return m_nDefaultActiveSlotCount; }
	inline const int			GetCoolTime_ChangeSlot ( void ) const { return m_nCoolTime_ChangeSlot; }


private:
	MAP_CHARSLOT				m_mapCharSlot;

	int							m_nOpenSlotCount;
	int							m_nDefaultActiveSlotCount;
	int							m_nCoolTime_ChangeSlot;


private:
	GLCharSlotMan ()
		: m_nOpenSlotCount ( 0 )
		, m_nDefaultActiveSlotCount ( 0 )
		, m_nCoolTime_ChangeSlot ( 0 )
	{

	}


public:
	~GLCharSlotMan () { }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////