#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../EngineLib/Common/ByteStream.h"

#include "./ItemLevel/GLItemLevel.h"
#include "./ItemBasicStat/GLItemBasicStatMan.h"
#include "./ItemSkill/GLItemLinkSkillMan.h"

#include "../GLogicData.h"
#include "../GLogicEx.h"
#include "../GLogicDataMan.h"
#include "./GLItem.h"
#include "./GLItemCustom_Addon.h"
#include "./GLItemMan.h"
#include "./GLItemCustom.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const bool RandomOpt::Option::operator ==(const RandomOpt::Option& _rhs) const
{
	if ( nOptValue == _rhs.nOptValue && cOptType == _rhs.cOptType )
		return true;
	return false;
}
const bool RandomOpt::Option::operator !=(const RandomOpt::Option& _rhs) const
{
	if ( nOptValue != _rhs.nOptValue || cOptType != _rhs.cOptType )
		return true;
	return false;
}
const RandomOpt::Option& RandomOpt::Option::operator =(const RandomOpt::Option& rhs)
{
	nOptValue = rhs.nOptValue;
	cOptType = rhs.cOptType;
	cOptCount = rhs.cOptCount;
	return *this;
}
RandomOpt::Option::Option(const BYTE _cOptType, const short _nOptValue, const BYTE _cOptCount)
: nOptValue(_nOptValue)
, cOptType(_cOptType)
, cOptCount(_cOptCount)
{
}
RandomOpt::Option::Option(const BYTE _cOptType, const short _nOptValue)
: nOptValue(_nOptValue)
, cOptType(_cOptType)
, cOptCount(0)
{
}
RandomOpt::Option::Option(void)
: nOptValue(0)
, cOptType(0)
, cOptCount(0)
{
}
void RandomOpt::Option::reset(void)
{
	cOptType = RandomOption::RANDOM_OPTION_NULL;
	cOptCount = 0;
	nOptValue = 0;
}

/*
const RandomOpt::FixOption& RandomOpt::FixOption::operator =(const FixOption& rhs)
{
	nSize = rhs.nSize;
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
		cOptType[_i] = rhs.cOptType[_i];

	return *this;
}
const bool RandomOpt::FixOption::operator ==(const BYTE& _cOptType) const
{
	if ( _cOptType == RandomOption::RANDOM_OPTION_NULL )
		return false;

	for ( DWORD _i(nSize); _i--; )
	{
		if ( _cOptType == cOptType[_i] )
			return true;
	}

	return false;
}
RandomOpt::FixOption::FixOption(void)
: nSize(0)
{
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
		cOptType[_i] = 0;
}
void RandomOpt::FixOption::push(const BYTE _cOptType, const DWORD nFixMaximum)
{
	if ( nSize >= nFixMaximum )
		return;

	cOptType[nSize++] = _cOptType;
}
void RandomOpt::FixOption::clear(void)
{
	nSize = 0;
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
		cOptType[_i] = 0;
}
const bool RandomOpt::FixOption::isValid(void) const
{
	if ( nSize == 0 )
		return false;

	return true;
}

const DWORD RandomOpt::FixOption::getNSize(void) const
{
	return nSize;
}
*/

const bool RandomOpt::operator ==(const RandomOpt& rhs)
{
	// ���� Ƚ���� ���� �񱳴� ���� �ʴ´�;
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
	{
		if ( option[_i] != rhs.option[_i] )
			return false;
	}

	return true;
}
const RandomOpt& RandomOpt::operator =(const RandomOpt& rhs)
{
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
		option[_i] = rhs.option[_i];

	return *this;
}

bool RandomOpt::Load_Ver100(const RandomOpt_0100& sRandomOpt)
{
	for ( DWORD _i(RandomOpt_0100::NSIZE); _i--; )
		option[_i] = sRandomOpt.option[_i];

	return true;
}

RandomOpt::RandomOpt(void)
{

}
RandomOpt::RandomOpt(const RandomOpt& rhs)
{
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
		option[_i] = rhs.option[_i];
}

void RandomOpt::remove(const DWORD nIndex)
{
	if ( nIndex >= RandomOpt::NSIZE )
		return;	

	for ( DWORD _i(nIndex); _i < RandomOpt::NSIZE - 1; ++_i )
		option[_i] = option[_i + 1];

	option[RandomOpt::NSIZE - 1].reset();
}


const bool RandomOpt::assign(const DWORD nIndex, const BYTE nType, const short nValue)
{
	if ( nIndex >= RandomOpt::NSIZE )
    {
        option[nIndex] = Option();
		return false;
    }

	if ( nType == RandomOption::RANDOM_OPTION_NULL )
    {
	    option[nIndex] = Option();
		return false;
    }

	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
	{
		if ( option[_i].cOptType == nType )
			return false;
	}

	option[nIndex] = Option(nType, nValue);
	return true;
}

const bool RandomOpt::assign(const DWORD nIndex, const RandomOpt::Option& _option)
{
	if ( nIndex >= RandomOpt::NSIZE )
    {
        option[nIndex] = Option();
        return false;
    }

	if ( _option.cOptType == RandomOption::RANDOM_OPTION_NULL )
    {
        option[nIndex] = Option();
        return false;
    }

	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
	{
		if ( option[_i].cOptType == _option.cOptType )
			return false;
	}

	option[nIndex] = _option;
	return true;
}

const bool RandomOpt::setOption(const BYTE nType, const float fValue)
{
	const DWORD nIndex(RandomOpt::_findEmptySlot());
	if ( nIndex == RandomOpt::EMNULL )
		return false;
	

	return RandomOpt::setOption(nIndex, nType, fValue);
}

// ������ ���� ���� ���� ��;
const bool RandomOpt::setOption(const DWORD nIndex, const BYTE nType, const float fValue)
{
	short nVALUE(0);
	switch ( nType )
	{
	default:
	case RandomOption::RANDOM_OPTION_NULL:
		return false;

	case RandomOption::RANDOM_OPTION_RATE_DAMAGE:
	case RandomOption::RANDOM_OPTION_RATE_DEFENCE:
	case RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE:
		//nVALUE = short(fValue*100.0f);
		nVALUE = short((float)(fValue*10000.0f));
		if ( 10000 < nVALUE )
			nVALUE = 10000;
		else if ( nVALUE < -10000 )
			nVALUE = -10000;
		break;

	case RandomOption::RANDOM_OPTION_RATE_HIT:
	case RandomOption::RANDOM_OPTION_RATE_AVOID:
		//nVALUE = short(fValue*100.0f);
		nVALUE = short((float)(fValue*10000.0f));
		if ( 10000 < nVALUE )
			nVALUE = 10000;
		else if ( nVALUE < -10000 )
			nVALUE = -10000;
		break;

	case RandomOption::RANDOM_OPTION_VALUE_HP:
	case RandomOption::RANDOM_OPTION_VALUE_MP:
	case RandomOption::RANDOM_OPTION_VALUE_SP:
		nVALUE = short(fValue);
		break;

	case RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP:
	case RandomOption::RANDOM_OPTION_RATE_RECOVERY_MP:
	case RandomOption::RANDOM_OPTION_RATE_RECOVERY_SP:
	case RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP:
		nVALUE = short((float)(fValue * 10000.0f));
		break;

	case RandomOption::RANDOM_OPTION_GRIND_DAMAGE:
	case RandomOption::RANDOM_OPTION_GRIND_DEFENCE:
		nVALUE = short(fValue);
		break;

	case RandomOption::RANDOM_OPTION_RATE_RANGE_ATTACK:
		nVALUE = short((float)(fValue * 100.0f));
		break;

	case RandomOption::RANDOM_OPTION_CONSUME_SP:
		nVALUE = short(fValue);
		break;

	case RandomOption::RANDOM_OPTION_VALUE_RESIST:
		nVALUE = short(fValue);
		break;

	case RandomOption::RANDOM_OPTION_VALUE_DAMAGE:
	case RandomOption::RANDOM_OPTION_VALUE_DEFENCE:
	case RandomOption::RANDOM_OPTION_VALUE_HIT:
	case RandomOption::RANDOM_OPTION_VALUE_AVOID:
		nVALUE = short(fValue);
		break;

	case RandomOption::RANDOM_OPTION_VALUE_POW:
	case RandomOption::RANDOM_OPTION_VALUE_STR:
	case RandomOption::RANDOM_OPTION_VALUE_SPI:
	case RandomOption::RANDOM_OPTION_VALUE_DEX:
	case RandomOption::RANDOM_OPTION_VALUE_STA:
		nVALUE = short(fValue);
		break;

	case RandomOption::RANDOM_OPTION_VALUE_MELEE:
	case RandomOption::RANDOM_OPTION_VALUE_RANGE:
	case RandomOption::RANDOM_OPTION_VALUE_MAGIC:
		nVALUE = short(fValue);
		break;

	case RandomOption::RANDOM_OPTION_VALUE_POTION_HP:
	case RandomOption::RANDOM_OPTION_VALUE_POTION_MP:
	case RandomOption::RANDOM_OPTION_VALUE_POTION_SP:
	case RandomOption::RANDOM_OPTION_VALUE_KILL_CP:
		nVALUE = short(fValue);
		break;
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_DURATIONTIME_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_2_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_3_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_4_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_5_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_6_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_7_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_8_DURATIONTIME_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_9_DURATIONTIME_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_10_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_11_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_12_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_13_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_14_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_15_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_16_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_17_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_18_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_19_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_20_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_21_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_22_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_23_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_24_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_25_DURATIONTIME_ADD:
		nVALUE = short((float)(fValue*100.0f));
		break;
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_DAMAGE_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_2_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_3_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_4_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_5_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_6_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_7_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_8_DAMAGE_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_9_DAMAGE_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_10_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_11_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_12_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_13_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_14_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_15_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_16_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_17_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_18_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_19_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_20_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_21_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_22_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_23_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_24_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_25_DAMAGE_ADD:
		nVALUE = short((float)(fValue*100.0f));
		break;
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_2_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_3_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_4_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_5_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_6_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_7_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_8_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_9_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_10_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_11_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_12_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_13_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_14_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_15_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_16_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_17_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_18_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_19_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_20_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_21_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_22_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_23_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_24_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_25_SKILLDILAY_DEC:
		nVALUE = short( (float)(fValue*100.0f));
		break;

	case RandomOption::RANDOM_OPTION_VALUE_CRITICALDAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_CRUSHINGBLOW_ADD:
		nVALUE = short( (float)( fValue*1000.0f ) );
		break;
	case RandomOption::RANDOM_OPTION_VALUE_WEARABLELEVEL_ADD:
		nVALUE = short(fValue);
		break;
	case RandomOption::RANDOM_OPTION_VALUE_LIFE_PER_KILL:
	case RandomOption::RANDOM_OPTION_VALUE_MP_PER_KILL:
	case RandomOption::RANDOM_OPTION_VALUE_SP_PER_KILL:
	case RandomOption::RANDOM_OPTION_VALUE_HPMPSP_PER_KILL:
		nVALUE = short( (float)( fValue*1000.0f ));
		break;
	case RandomOption::RANDOM_OPTION_VALUE_ALLMPRATIO_TO_ATTACK:
	case RandomOption::RANDOM_OPTION_VALUE_ALLSPRATIO_TO_HP:
	case RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MELEE:
	case RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_RANGE:
	case RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MAGIC:
		nVALUE = short( (float)(fValue*1000.0f));
		break;
	};

	return RandomOpt::assign(nIndex, nType, nVALUE);
}

const BYTE RandomOpt::getType(const DWORD nIndex) const
{
	return nIndex < RandomOpt::NSIZE ? option[nIndex].cOptType : RandomOption::RANDOM_OPTION_NULL;
}
const short RandomOpt::getValue(const DWORD nIndex) const
{
	return nIndex < RandomOpt::NSIZE ? option[nIndex].nOptValue : RandomOption::RANDOM_OPTION_NULL;
}
const BYTE RandomOpt::getCount(const DWORD nIndex) const
{
	return nIndex < RandomOpt::NSIZE ? option[nIndex].cOptCount : RandomOption::RANDOM_OPTION_NULL;
}

const int RandomOpt::reset(const BYTE cDAMAGE, const BYTE cDEFENSE, const ITEM_RANDOMOP_UTILL::FixOption& fixOption, const DWORD nFixMaximum)
{
	const BYTE cPassType(cDAMAGE == 0 ? (cDEFENSE == 0 ? RandomOption::RANDOM_OPTION_NULL : RandomOption::RANDOM_OPTION_GRIND_DEFENCE) : RandomOption::RANDOM_OPTION_GRIND_DAMAGE);
	// ���/���� ���� �ɼ��� �پ� �ִ� ��� ������ ����;

	const DWORD sizeFixed(fixOption.getNSize());
	const DWORD sizeFixable(nFixMaximum);

	DWORD nSize(RandomOpt::NSIZE);
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
	{
		const BYTE& cOptType = option[_i].cOptType;
		if ( cOptType == cPassType )
			continue;

		if ( (fixOption == cOptType) && (sizeFixed <= sizeFixable) )
		{ // ���� �ɼǿ� ���� �Ǿ� ������ ���� Ƚ�� ī������ �Ѵ�;
			++option[_i].cOptCount;
		}		
		else
		{ // ���� �ɼ��� �ƴϰų� ���� ������ ���� �� ��� ���� �Ѵ�;
			// ������ �ϰ������� �ϴ°� ������, ��ȭ �ɼ��� ���ܵ־� �ϱ⿡ �̿� ���� �Ѵ�;
			RandomOpt::remove(_i);
		}
	}

	return RandomOpt::NSIZE - RandomOpt::_size();
}

const DWORD RandomOpt::_findEmptySlot(void) const
{
	for ( DWORD _i(0); _i < RandomOpt::NSIZE; ++_i )
	{
		if ( option[_i].cOptType == RandomOption::RANDOM_OPTION_NULL )
			return _i;
	}

	return RandomOpt::EMNULL;
}

const DWORD RandomOpt::_size(void) const
{
	DWORD nSize(0);
	for ( DWORD _i(RandomOpt::NSIZE); _i--; )
		nSize += (option[_i].cOptType == RandomOption::RANDOM_OPTION_NULL ? 1 : 0);

	return nSize;
}

STLGrindInfo::STLGrindInfo() : cGradeValue(0), tFireTime(0)
{	
}

STLGrindInfo::STLGrindInfo( const STLGrindInfo& _rhs )
{
	cGradeValue = _rhs.cGradeValue;
	tFireTime   = _rhs.tFireTime;
}

void STLGrindInfo::resetInfo()
{
	cGradeValue = 0;
	tFireTime   = 0;
}

const STLGrindInfo& STLGrindInfo::operator =(const STLGrindInfo& _rhs)
{
	cGradeValue = _rhs.cGradeValue;
	tFireTime   = _rhs.tFireTime;

	return *this;
}

const bool STLGrindInfo::operator ==(const STLGrindInfo& _rhs)
{
	if( cGradeValue == _rhs.cGradeValue && tFireTime == _rhs.tFireTime )
		return true;

	return false;
}



void SITEMCUSTOM_BASIC::Initialize ( void )
{
	m_sBasicAttackDamage = GLPADATA();
	m_wBasicDefence = 0;

	m_emLinkSkillTarget = EMITEM_SKILL_ALL;
	m_sLinkSkillID = SNATIVEID( false );
	m_wLinkSkillLevel = 0;
	m_fLinkSkillOccurRate = 0.f;
}

void SITEMCUSTOM_BASIC::DefaultInitialize_BasicAttackDamage ( const SITEM* _pItem )
{
	if ( NULL == _pItem )
		return;

	m_sBasicAttackDamage = _pItem->GetAttackDamage();
}

void SITEMCUSTOM_BASIC::DefaultInitialize_BasicDefence ( const SITEM* _pItem )
{
	if ( NULL == _pItem )
		return;

	m_wBasicDefence = _pItem->GetDefence();
}

void SITEMCUSTOM_BASIC::DefaultInitialize_LinkSkill ( const SITEM* _pItem )
{
	if ( NULL == _pItem )
		return;

	m_emLinkSkillTarget = _pItem->GetLinkSkillTarget();
	m_sLinkSkillID = _pItem->GetLinkSkillID();
	m_wLinkSkillLevel = _pItem->GetLinkSkillLevel();
	m_fLinkSkillOccurRate = _pItem->GetLinkSkillOccurRate();
}

void SITEMCUSTOM_BASIC::DefaultInitialize ( const SITEM* _pItem )
{
	if ( NULL == _pItem )
		return;

	m_sBasicAttackDamage = _pItem->GetAttackDamage();
	m_wBasicDefence = _pItem->GetDefence();

	m_emLinkSkillTarget = _pItem->GetLinkSkillTarget();
	m_sLinkSkillID = _pItem->GetLinkSkillID();
	m_wLinkSkillLevel = _pItem->GetLinkSkillLevel();
	m_fLinkSkillOccurRate = _pItem->GetLinkSkillOccurRate();
}

void SITEMCUSTOM_BASIC::GenerateBasicAttackDamage_Make ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex )
{
	m_sBasicAttackDamage = GLItemBasicStatMan::Instance()->GetAttackDamage_MakeItem( _emItemLevel, _nType, _nIndex );
}

void SITEMCUSTOM_BASIC::GenerateBasicAttackDamage_Drop ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex )
{
	m_sBasicAttackDamage = GLItemBasicStatMan::Instance()->GetAttackDamage_DropItem( _emItemLevel, _nType, _nIndex );
}

void SITEMCUSTOM_BASIC::GenerateBasicDefence_Make ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex )
{
	m_wBasicDefence = GLItemBasicStatMan::Instance()->GetDefence_MakeItem( _emItemLevel, _nType, _nIndex );
}

void SITEMCUSTOM_BASIC::GenerateBasicDefence_Drop ( const EMITEMLEVEL _emItemLevel, const int _nType, const int _nIndex )
{
	m_wBasicDefence = GLItemBasicStatMan::Instance()->GetDefence_DropItem( _emItemLevel, _nType, _nIndex );
}

void SITEMCUSTOM_BASIC::GenerateLinkSkill ( const int _nLinkSkillIndex )
{
	GLItemLinkSkillMan::Instance()->GetLinkSkill(
		_nLinkSkillIndex,
		m_emLinkSkillTarget,
		m_sLinkSkillID,
		m_wLinkSkillLevel,
		m_fLinkSkillOccurRate );
}



SITEMCUSTOM::SITEMCUSTOM ()
	: sNativeID(false)
	, nidDISGUISE(false)
	, tBORNTIME(0)
	, tDISGUISE(0)
	, tPERIODEX_BASIC(0)
	, tPERIODEX_DISGUISE(0)
	//, lnGenNum(0)
	, wTurnNum(1)
	, cGenType(EMGEN_DEFAULT)
	, cPeriodExtensionCount(0)	
	, cCostumePeriodExtensionCount(0)
	
	, cDAMAGE(0)
	, cDEFENSE(0)

	, cRESIST_FIRE(0)	
	, cRESIST_ICE(0)
	, cRESIST_ELEC(0)
	, cRESIST_POISON(0)
	, cRESIST_SPIRIT(0)

	, m_PetDbNum(0)
	, m_VehicleDbNum(0)
	, bVietnamGainItem(false)

	, dwMainColor(0)
	, dwSubColor(0)
    , wDurability(0)
    , dwDurabilityCount(0)

    , wRanPointTradeCount(0)
{
}

SITEMCUSTOM::SITEMCUSTOM(const SNATIVEID& sNID)
	: sNativeID(sNID)
	, nidDISGUISE(false)
	, tBORNTIME(0)
	, tDISGUISE(0)
	, tPERIODEX_BASIC(0)
	, tPERIODEX_DISGUISE(0)
	//, lnGenNum(0)
	, wTurnNum(1)	
	, cGenType(EMGEN_DEFAULT)
	, cPeriodExtensionCount(0)	
	, cCostumePeriodExtensionCount(0)
	
	, cDAMAGE(0)
	, cDEFENSE(0)

	, cRESIST_FIRE(0)	
	, cRESIST_ICE(0)
	, cRESIST_ELEC(0)
	, cRESIST_POISON(0)
	, cRESIST_SPIRIT(0)	

	, m_PetDbNum(0)
	, m_VehicleDbNum(0)
	, bVietnamGainItem(false)

	, dwMainColor(0)
	, dwSubColor(0)
    , wDurability(0)
    , dwDurabilityCount(0)
    , wRanPointTradeCount(0)
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem(sNID);
    if( pItem )
        SetDefaultValue( pItem );
}

SITEMCUSTOM::SITEMCUSTOM( const SITEMCUSTOM& sItemcustom )
    : sNativeID( sItemcustom.sNativeID )
    , nidDISGUISE( sItemcustom.nidDISGUISE )
    , tBORNTIME( sItemcustom.tBORNTIME )
    , tDISGUISE( sItemcustom.tDISGUISE )
	, tPERIODEX_BASIC(sItemcustom.tPERIODEX_BASIC)
	, tPERIODEX_DISGUISE(sItemcustom.tPERIODEX_DISGUISE)
   // , lnGenNum( sItemcustom.lnGenNum )
    , wTurnNum( sItemcustom.wTurnNum )
    , cGenType( sItemcustom.cGenType )
    , cPeriodExtensionCount( sItemcustom.cPeriodExtensionCount )	
    , cCostumePeriodExtensionCount( sItemcustom.cCostumePeriodExtensionCount )

    , cDAMAGE( sItemcustom.cDAMAGE )
    , cDEFENSE( sItemcustom.cDEFENSE )

    , cRESIST_FIRE( sItemcustom.cRESIST_FIRE )	
    , cRESIST_ICE( sItemcustom.cRESIST_ICE )
    , cRESIST_ELEC( sItemcustom.cRESIST_ELEC )
    , cRESIST_POISON( sItemcustom.cRESIST_POISON )
    , cRESIST_SPIRIT( sItemcustom.cRESIST_SPIRIT )

    , randomOption( sItemcustom.randomOption )

    , m_PetDbNum( sItemcustom.m_PetDbNum )
    , m_VehicleDbNum( sItemcustom.m_VehicleDbNum )
    , bVietnamGainItem( sItemcustom.bVietnamGainItem )

    , dwMainColor( sItemcustom.dwMainColor )
    , dwSubColor( sItemcustom.dwSubColor )
    , wDurability( sItemcustom.wDurability )
    , dwDurabilityCount( sItemcustom.dwDurabilityCount )
    , wRanPointTradeCount( sItemcustom.wRanPointTradeCount )
	, costumeUser( sItemcustom.costumeUser )
	, sTLGrind( sItemcustom.sTLGrind )
	, guid( sItemcustom.guid )
	, itemState( sItemcustom.itemState )

	, sBasicInfo ( sItemcustom.sBasicInfo )
	, sAddonInfo ( sItemcustom.sAddonInfo )
{
}

void SITEMCUSTOM::Initialize( const SNATIVEID& sNID )
{
	sNativeID = sNID;
	nidDISGUISE = false;
	tBORNTIME = 0;
	tDISGUISE = 0;
	tPERIODEX_BASIC = 0;
	tPERIODEX_DISGUISE = 0;
	wTurnNum = 1;
	cGenType = EMGEN_DEFAULT;
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;
	cDAMAGE = 0;
	cDEFENSE = 0;
	cRESIST_FIRE = 0;
	cRESIST_ICE = 0;
	cRESIST_ELEC = 0;
	cRESIST_POISON = 0;
	cRESIST_SPIRIT = 0;
	m_PetDbNum = 0;
	m_VehicleDbNum = 0;
	bVietnamGainItem = false;
	dwMainColor = 0;
	dwSubColor = 0;
	wDurability = 0;
	dwDurabilityCount = 0;
	wRanPointTradeCount = 0;

	sBasicInfo.Initialize();

	const SITEM* pItem = GLogicData::GetInstance().GetItem( sNID );
	if ( pItem )
		SetDefaultValue( pItem );
	guid.Reset();
}

void SITEMCUSTOM::ResetGrind ( EMGRINDING_TYPE emTYPE )
{
	switch ( emTYPE )
	{
	case EMGRINDING_NUNE:
		cDAMAGE = (0);
		cDEFENSE = (0);

		cRESIST_FIRE = (0);
		cRESIST_ICE = (0);
		cRESIST_ELEC = (0);
		cRESIST_POISON = (0);
		cRESIST_SPIRIT = (0);
		break;

	case EMGRINDING_DAMAGE:
		cDAMAGE = (0);
		break;
	case EMGRINDING_DEFENSE:
		cDEFENSE = (0);
		break;

	case EMGRINDING_RESIST_FIRE:
		cRESIST_FIRE = (0);
		break;
	case EMGRINDING_RESIST_ICE:
		cRESIST_ICE = (0);
		break;
	case EMGRINDING_RESIST_ELEC:
		cRESIST_ELEC = (0);
		break;
	case EMGRINDING_RESIST_POISON:
		cRESIST_POISON = (0);
		break;
	case EMGRINDING_RESIST_SPIRIT:
		cRESIST_SPIRIT = (0);
		break;
	};
}

void SITEMCUSTOM::DecreaseGrind ( EMGRINDING_TYPE emTYPE, const INT nValue )
{
	INT nDec = abs(nValue);

	switch ( emTYPE )
	{
	case EMGRINDING_NUNE:
		cDAMAGE        = (cDAMAGE > nDec)        ? (cDAMAGE - nDec)        : 0;
		cDEFENSE       = (cDEFENSE > nDec)       ? (cDEFENSE - nDec)       : 0;
		cRESIST_FIRE   = (cRESIST_FIRE > nDec)   ? (cRESIST_FIRE - nDec)   : 0;
		cRESIST_ICE    = (cRESIST_ICE > nDec)    ? (cRESIST_ICE - nDec)    : 0;
		cRESIST_ELEC   = (cRESIST_ELEC > nDec)   ? (cRESIST_ELEC - nDec)   : 0;
		cRESIST_POISON = (cRESIST_POISON > nDec) ? (cRESIST_POISON - nDec) : 0;
		cRESIST_SPIRIT = (cRESIST_SPIRIT > nDec) ? (cRESIST_SPIRIT - nDec) : 0;
		break;

	case EMGRINDING_DAMAGE:
		cDAMAGE        = (cDAMAGE > nDec)        ? (cDAMAGE - nDec)        : 0;
		break;
	case EMGRINDING_DEFENSE:
		cDEFENSE       = (cDEFENSE > nDec)       ? (cDEFENSE - nDec)       : 0;
		break;
	case EMGRINDING_RESIST_FIRE:
		cRESIST_FIRE   = (cRESIST_FIRE > nDec)   ? (cRESIST_FIRE - nDec)   : 0;
		break;
	case EMGRINDING_RESIST_ICE:
		cRESIST_ICE    = (cRESIST_ICE > nDec)    ? (cRESIST_ICE - nDec)    : 0;
		break;
	case EMGRINDING_RESIST_ELEC:
		cRESIST_ELEC   = (cRESIST_ELEC > nDec)   ? (cRESIST_ELEC - nDec)   : 0;
		break;
	case EMGRINDING_RESIST_POISON:
		cRESIST_POISON = (cRESIST_POISON > nDec) ? (cRESIST_POISON - nDec) : 0;
		break;
	case EMGRINDING_RESIST_SPIRIT:
		cRESIST_SPIRIT = (cRESIST_SPIRIT > nDec) ? (cRESIST_SPIRIT - nDec) : 0;
		break;
	};
}

bool SITEMCUSTOM::operator == ( const SITEMCUSTOM &value )
{
	if ( 
		sNativeID == value.sNativeID && 
		nidDISGUISE == value.nidDISGUISE && 
		tBORNTIME == value.tBORNTIME && 
		tPERIODEX_BASIC == value.tPERIODEX_BASIC && 
		tPERIODEX_DISGUISE == value.tPERIODEX_DISGUISE &&
		tDISGUISE == value.tDISGUISE && 
		//lnGenNum == value.lnGenNum && 
		wTurnNum == value.wTurnNum && 
		cGenType == value.cGenType && 
		cPeriodExtensionCount == value.cPeriodExtensionCount && 
		cCostumePeriodExtensionCount == value.cCostumePeriodExtensionCount &&
		cDAMAGE == value.cDAMAGE && 
		cDEFENSE == value.cDEFENSE && 
		cRESIST_FIRE == value.cRESIST_FIRE && 
		cRESIST_ICE == value.cRESIST_ICE && 
		cRESIST_ELEC == value.cRESIST_ELEC && 
		cRESIST_POISON == value.cRESIST_POISON && 
		cRESIST_SPIRIT == value.cRESIST_SPIRIT && 
		randomOption == value.randomOption && 		
		m_PetDbNum == value.m_PetDbNum && 
		m_VehicleDbNum == value.m_VehicleDbNum && 
		bVietnamGainItem == value.bVietnamGainItem && 
		dwMainColor == value.dwMainColor && 
		dwSubColor == value.dwSubColor &&
        wDurability == value.wDurability &&
        dwDurabilityCount == value.dwDurabilityCount &&
        wRanPointTradeCount == value.wRanPointTradeCount &&
		costumeUser == value.costumeUser &&
		sTLGrind == value.sTLGrind &&
		guid == value.guid && 
		itemState.getState() == value.itemState.getState()
		// �⺻�ɷ�ġ�� üũ���� �ʴ´�;
		)
	{
		return true;
	}
	else
	{
		return false;
	}
	//return !memcmp(this,&value,sizeof(SITEMCUSTOM));
}

bool SITEMCUSTOM::operator != ( const SITEMCUSTOM &value )
{
	return !( *this == value );
	//return 0!=memcmp(this,&value,sizeof(SITEMCUSTOM));
}

namespace
{
    BYTE CAST2GRADE ( int nVALUE, int nSTEP )
    {
        BYTE cGRADE(0);
        if ( nVALUE > 0 )	cGRADE += 1;
        cGRADE += BYTE ( (nVALUE-1)/nSTEP );

        if ( cGRADE>9 )		cGRADE = 9;
        return cGRADE;
    }

    BYTE CAST2GRADE_EX ( int nVALUE, int nSTEP1, int nSTEP2 )
    {
        nVALUE = nVALUE * nSTEP1;

        BYTE cGRADE(0);
        if ( nVALUE > 0 )	cGRADE += 1;
        cGRADE += BYTE ( (nVALUE-1)/nSTEP2 );

        if ( cGRADE>9 )		cGRADE = 9;
        return cGRADE;
    }
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_100 &sOld )
{
	sNativeID = sOld.sNativeID;

	cGenType = EMGEN_OLD;

	wTurnNum = sOld.wTurnNum;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	cDAMAGE = CAST2GRADE ( sOld.nDamage, sInfoByItemLevel.wDamageByGrade/*GLCONST_CHAR::wDAMAGE_GRADE*/ );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, sInfoByItemLevel.wDefenceByGrade/*GLCONST_CHAR::wDEFENSE_GRADE*/ );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, sInfoByItemLevel.wFireResistByGrade/*GLCONST_CHAR::wRESIST_FIRE_GRADE*/ );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, sInfoByItemLevel.wIceResistByGrade/*GLCONST_CHAR::wRESIST_ICE_GRADE*/ );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, sInfoByItemLevel.wElectricResistByGrade/*GLCONST_CHAR::wRESIST_ELEC_GRADE*/ );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, sInfoByItemLevel.wPoisonResistByGrade/*GLCONST_CHAR::wRESIST_POISON_GRADE*/ );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		const SITEM *pITEM = GLogicData::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )
			bRESET_GRINDING = ( pITEM && ( pITEM->GetAttackDamage() != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_101 &sOld )
{
	sNativeID = sOld.sNativeID;

	cGenType = EMGEN_OLD;

	wTurnNum = sOld.wTurnNum;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	cDAMAGE = CAST2GRADE ( sOld.nDamage, sInfoByItemLevel.wDamageByGrade/*GLCONST_CHAR::wDAMAGE_GRADE*/ );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, sInfoByItemLevel.wDefenceByGrade/*GLCONST_CHAR::wDEFENSE_GRADE*/ );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, sInfoByItemLevel.wFireResistByGrade/*GLCONST_CHAR::wRESIST_FIRE_GRADE*/ );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, sInfoByItemLevel.wIceResistByGrade/*GLCONST_CHAR::wRESIST_ICE_GRADE*/ );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, sInfoByItemLevel.wElectricResistByGrade/*GLCONST_CHAR::wRESIST_ELEC_GRADE*/ );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, sInfoByItemLevel.wPoisonResistByGrade/*GLCONST_CHAR::wRESIST_POISON_GRADE*/ );
	cRESIST_SPIRIT = CAST2GRADE ( sOld.sResist.nSpirit, sInfoByItemLevel.wSpiritResistByGrade/*GLCONST_CHAR::wRESIST_SPIRIT_GRADE*/ );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		const SITEM *pITEM = GLogicData::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )
			bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_102 &sOld )
{
	sNativeID = sOld.sNativeID;

	cGenType = sOld.cGenType;
	//cFieldID = sOld.cFieldID;
	//lnGenNum = sOld.dwGenNum;

	wTurnNum = sOld.wTurnNum;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	cDAMAGE = CAST2GRADE ( sOld.nDamage, sInfoByItemLevel.wDamageByGrade/*GLCONST_CHAR::wDAMAGE_GRADE*/ );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, sInfoByItemLevel.wDefenceByGrade/*GLCONST_CHAR::wDEFENSE_GRADE*/ );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, sInfoByItemLevel.wFireResistByGrade/*GLCONST_CHAR::wRESIST_FIRE_GRADE*/ );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, sInfoByItemLevel.wIceResistByGrade/*GLCONST_CHAR::wRESIST_ICE_GRADE*/ );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, sInfoByItemLevel.wElectricResistByGrade/*GLCONST_CHAR::wRESIST_ELEC_GRADE*/ );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, sInfoByItemLevel.wPoisonResistByGrade/*GLCONST_CHAR::wRESIST_POISON_GRADE*/ );
	cRESIST_SPIRIT = CAST2GRADE ( sOld.sResist.nSpirit, sInfoByItemLevel.wSpiritResistByGrade/*GLCONST_CHAR::wRESIST_SPIRIT_GRADE*/ );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		const SITEM *pITEM = GLogicData::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )
			bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_103 &sOld )
{
	sNativeID = sOld.sNativeID;

	cGenType = sOld.cGenType;
	//cFieldID = sOld.cFieldID;
	//lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	cDAMAGE = CAST2GRADE ( sOld.nDamage, sInfoByItemLevel.wDamageByGrade/*GLCONST_CHAR::wDAMAGE_GRADE*/ );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, sInfoByItemLevel.wDefenceByGrade/*GLCONST_CHAR::wDEFENSE_GRADE*/ );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, sInfoByItemLevel.wFireResistByGrade/*GLCONST_CHAR::wRESIST_FIRE_GRADE*/ );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, sInfoByItemLevel.wIceResistByGrade/*GLCONST_CHAR::wRESIST_ICE_GRADE*/ );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, sInfoByItemLevel.wElectricResistByGrade/*GLCONST_CHAR::wRESIST_ELEC_GRADE*/ );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, sInfoByItemLevel.wPoisonResistByGrade/*GLCONST_CHAR::wRESIST_POISON_GRADE*/ );
	cRESIST_SPIRIT = CAST2GRADE ( sOld.sResist.nSpirit, sInfoByItemLevel.wSpiritResistByGrade/*GLCONST_CHAR::wRESIST_SPIRIT_GRADE*/ );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		const SITEM *pITEM = GLogicData::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )
			bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_104 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;

	cGenType = sOld.cGenType;
	//cFieldID = sOld.cFieldID;
	//lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	cDAMAGE = CAST2GRADE ( sOld.nDamage, sInfoByItemLevel.wDamageByGrade/*GLCONST_CHAR::wDAMAGE_GRADE*/ );
	cDEFENSE = CAST2GRADE ( sOld.nDefense, sInfoByItemLevel.wDefenceByGrade/*GLCONST_CHAR::wDEFENSE_GRADE*/ );

	cRESIST_FIRE = CAST2GRADE ( sOld.sResist.nFire, sInfoByItemLevel.wFireResistByGrade/*GLCONST_CHAR::wRESIST_FIRE_GRADE*/ );
	cRESIST_ICE = CAST2GRADE ( sOld.sResist.nIce, sInfoByItemLevel.wIceResistByGrade/*GLCONST_CHAR::wRESIST_ICE_GRADE*/ );
	cRESIST_ELEC = CAST2GRADE ( sOld.sResist.nElectric, sInfoByItemLevel.wElectricResistByGrade/*GLCONST_CHAR::wRESIST_ELEC_GRADE*/ );
	cRESIST_POISON = CAST2GRADE ( sOld.sResist.nPoison, sInfoByItemLevel.wPoisonResistByGrade/*GLCONST_CHAR::wRESIST_POISON_GRADE*/ );
	cRESIST_SPIRIT = CAST2GRADE ( sOld.sResist.nSpirit, sInfoByItemLevel.wSpiritResistByGrade/*GLCONST_CHAR::wRESIST_SPIRIT_GRADE*/ );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		const SITEM *pITEM = GLogicData::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )
			bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_105 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;

	cGenType = sOld.cGenType;
	//cFieldID = sOld.cFieldID;
	//lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	cDAMAGE = CAST2GRADE_EX ( sOld.cDAMAGE, 3, sInfoByItemLevel.wDamageByGrade/*GLCONST_CHAR::wDAMAGE_GRADE*/ );
	cDEFENSE = CAST2GRADE_EX ( sOld.cDEFENSE, 3, sInfoByItemLevel.wDefenceByGrade/*GLCONST_CHAR::wDEFENSE_GRADE*/ );

	cRESIST_FIRE = CAST2GRADE_EX ( sOld.cRESIST_FIRE, 3, sInfoByItemLevel.wFireResistByGrade/*GLCONST_CHAR::wRESIST_FIRE_GRADE*/ );
	cRESIST_ICE = CAST2GRADE_EX ( sOld.cRESIST_ICE, 3, sInfoByItemLevel.wIceResistByGrade/*GLCONST_CHAR::wRESIST_ICE_GRADE*/ );
	cRESIST_ELEC = CAST2GRADE_EX ( sOld.cRESIST_ELEC, 3, sInfoByItemLevel.wElectricResistByGrade/*GLCONST_CHAR::wRESIST_ELEC_GRADE*/ );
	cRESIST_POISON = CAST2GRADE_EX ( sOld.cRESIST_POISON, 3, sInfoByItemLevel.wPoisonResistByGrade/*GLCONST_CHAR::wRESIST_POISON_GRADE*/ );
	cRESIST_SPIRIT = CAST2GRADE_EX ( sOld.cRESIST_SPIRIT, 3, sInfoByItemLevel.wSpiritResistByGrade/*GLCONST_CHAR::wRESIST_SPIRIT_GRADE*/ );

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		const SITEM *pITEM = GLogicData::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )
			bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_106 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = 0;

	cGenType = sOld.cGenType;
	//cFieldID = sOld.cFieldID;
	//lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;

	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		const SITEM *pITEM = GLogicData::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )
			bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_107 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = sOld.tDISGUISE;

	cGenType = sOld.cGenType;
	cCostumePeriodExtensionCount = 0;
	cPeriodExtensionCount = 0;
	//cChnID = 0;
	//lnGenNum = sOld.lnGenNum;

	wTurnNum = sOld.wTurnNum;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;

	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;

	if ( cRESIST_FIRE==9 && cRESIST_ICE==9 && cRESIST_ELEC==9 && cRESIST_POISON==9 && cRESIST_SPIRIT==9 )
	{
		const SITEM *pITEM = GLogicData::GetInstance().GetItem ( sNativeID );

		bool bRESET_GRINDING = pITEM && pITEM->sSuitOp.wReModelNum==0;
		if ( !bRESET_GRINDING )
			bRESET_GRINDING = ( pITEM && ( pITEM->sSuitOp.gdDamage != GLPADATA(0,0) ) );
		if ( bRESET_GRINDING )
		{
			ResetGrind(EMGRINDING_RESIST_FIRE);
			ResetGrind(EMGRINDING_RESIST_ICE);
			ResetGrind(EMGRINDING_RESIST_ELEC);
			ResetGrind(EMGRINDING_RESIST_POISON);
			ResetGrind(EMGRINDING_RESIST_SPIRIT);
		}
	}

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_108 &sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = sOld.tDISGUISE;

	cGenType = sOld.cGenType;
	// cChnID = sOld.cChnID;
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;
	//lnGenNum = sOld.lnGenNum;
	wTurnNum = sOld.wTurnNum;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;

	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_109 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	//lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	// cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.assign(0, sOld.cOptTYPE1, sOld.nOptVALUE1);
	randomOption.assign(1, sOld.cOptTYPE2, sOld.nOptVALUE2);
	randomOption.assign(2, sOld.cOptTYPE3, sOld.nOptVALUE3);
	randomOption.assign(3, sOld.cOptTYPE4, sOld.nOptVALUE4);	

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_110 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	//lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	// cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.assign(0, sOld.cOptTYPE1, sOld.nOptVALUE1);
	randomOption.assign(1, sOld.cOptTYPE2, sOld.nOptVALUE2);
	randomOption.assign(2, sOld.cOptTYPE3, sOld.nOptVALUE3);
	randomOption.assign(3, sOld.cOptTYPE4, sOld.nOptVALUE4);	

	m_PetDbNum			= sOld.dwPetID;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_111 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	//lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	// cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.assign(0, sOld.cOptTYPE1, sOld.nOptVALUE1);
	randomOption.assign(1, sOld.cOptTYPE2, sOld.nOptVALUE2);
	randomOption.assign(2, sOld.cOptTYPE3, sOld.nOptVALUE3);
	randomOption.assign(3, sOld.cOptTYPE4, sOld.nOptVALUE4);	

	m_PetDbNum			= sOld.dwPetID;
	m_VehicleDbNum		= sOld.dwVehicleID;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_112 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	//lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	// cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.assign(0, sOld.cOptTYPE1, sOld.nOptVALUE1);
	randomOption.assign(1, sOld.cOptTYPE2, sOld.nOptVALUE2);
	randomOption.assign(2, sOld.cOptTYPE3, sOld.nOptVALUE3);
	randomOption.assign(3, sOld.cOptTYPE4, sOld.nOptVALUE4);	

	m_PetDbNum			= sOld.dwPetID;
	m_VehicleDbNum		= sOld.dwVehicleID;

	bVietnamGainItem	= sOld.bVietnamGainItem;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}


void SITEMCUSTOM::Assign( const SITEMCUSTOM_113 &sOld )
{
    sNativeID			= sOld.sNativeID;	
    nidDISGUISE			= sOld.nidDISGUISE;
    tBORNTIME			= sOld.tBORNTIME;
    tDISGUISE			= sOld.tDISGUISE;

    //lnGenNum			= sOld.lnGenNum;
    wTurnNum			= sOld.wTurnNum;
    cGenType			= sOld.cGenType;	
    // cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

    cDAMAGE				= sOld.cDAMAGE;	
    cDEFENSE			= sOld.cDEFENSE;	
    cRESIST_FIRE		= sOld.cRESIST_FIRE;
    cRESIST_ICE			= sOld.cRESIST_ICE;
    cRESIST_ELEC		= sOld.cRESIST_ELEC;
    cRESIST_POISON		= sOld.cRESIST_POISON;
    cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.assign(0, sOld.cOptTYPE1, sOld.nOptVALUE1);
	randomOption.assign(1, sOld.cOptTYPE2, sOld.nOptVALUE2);
	randomOption.assign(2, sOld.cOptTYPE3, sOld.nOptVALUE3);
	randomOption.assign(3, sOld.cOptTYPE4, sOld.nOptVALUE4);	

    m_PetDbNum			= sOld.dwPetID;
    m_VehicleDbNum		= sOld.dwVehicleID;

    bVietnamGainItem	= sOld.bVietnamGainItem;

    dwMainColor			= sOld.dwMainColor;
    dwSubColor			= sOld.dwSubColor;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign( const SITEMCUSTOM_114 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	//lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	//cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.assign(0, sOld.cOptTYPE1, sOld.nOptVALUE1);
	randomOption.assign(1, sOld.cOptTYPE2, sOld.nOptVALUE2);
	randomOption.assign(2, sOld.cOptTYPE3, sOld.nOptVALUE3);
	randomOption.assign(3, sOld.cOptTYPE4, sOld.nOptVALUE4);	

	m_PetDbNum			= sOld.dwPetID;
	m_VehicleDbNum		= sOld.dwVehicleID;

	bVietnamGainItem	= sOld.bVietnamGainItem;

	dwMainColor			= sOld.dwMainColor;
	dwSubColor			= sOld.dwSubColor;

    // �������� ���� MAX -> 0����
    //�Ҹ�Ǵ����� 0->MAX�� ��굵�Ƿ� ���� �Ǿ���.
    // ������ �������� �׳� �������� Ǯ�� ����� ������.
	wDurability = 0;
	dwDurabilityCount = 0;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign( const SITEMCUSTOM_115 &sOld )
{
    sNativeID			= sOld.sNativeID;	
    nidDISGUISE			= sOld.nidDISGUISE;
    tBORNTIME			= sOld.tBORNTIME;
    tDISGUISE			= sOld.tDISGUISE;

    //lnGenNum			= sOld.lnGenNum;
    wTurnNum			= sOld.wTurnNum;
    cGenType			= sOld.cGenType;	
    //cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

    cDAMAGE				= sOld.cDAMAGE;	
    cDEFENSE			= sOld.cDEFENSE;	
    cRESIST_FIRE		= sOld.cRESIST_FIRE;
    cRESIST_ICE			= sOld.cRESIST_ICE;
    cRESIST_ELEC		= sOld.cRESIST_ELEC;
    cRESIST_POISON		= sOld.cRESIST_POISON;
    cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

    randomOption.Load_Ver100(sOld.randomOption);	

    m_PetDbNum			= sOld.m_PetDbNum;
    m_VehicleDbNum		= sOld.m_VehicleDbNum;

    bVietnamGainItem	= sOld.bVietnamGainItem;

    dwMainColor			= sOld.dwMainColor;
    dwSubColor			= sOld.dwSubColor;

    // �������� ���� MAX -> 0����
    //�Ҹ�Ǵ����� 0->MAX�� ��굵�Ƿ� ���� �Ǿ���.
    // ������ �������� �׳� �������� Ǯ�� ����� ������.
    
    // ���� ���������� ������ ���� ���� �ؾ��Ѵ�.
    //wDurability = sOld.wDurability;
    //dwDurabilityCount = sOld.dwDurabilityCount;.
    wDurability = 0;
    dwDurabilityCount = 0;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign( const SITEMCUSTOM_116 &sOld )
{
    sNativeID			= sOld.sNativeID;	
    nidDISGUISE			= sOld.nidDISGUISE;
    tBORNTIME			= sOld.tBORNTIME;
    tDISGUISE			= sOld.tDISGUISE;

    //lnGenNum			= sOld.lnGenNum;
    wTurnNum			= sOld.wTurnNum;
    cGenType			= sOld.cGenType;	
    // cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

    cDAMAGE				= sOld.cDAMAGE;	
    cDEFENSE			= sOld.cDEFENSE;	
    cRESIST_FIRE		= sOld.cRESIST_FIRE;
    cRESIST_ICE			= sOld.cRESIST_ICE;
    cRESIST_ELEC		= sOld.cRESIST_ELEC;
    cRESIST_POISON		= sOld.cRESIST_POISON;
    cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

    randomOption.Load_Ver100(sOld.randomOption);	

    m_PetDbNum			= sOld.m_PetDbNum;
    m_VehicleDbNum		= sOld.m_VehicleDbNum;

    bVietnamGainItem	= sOld.bVietnamGainItem;

    dwMainColor			= sOld.dwMainColor;
    dwSubColor			= sOld.dwSubColor;

    wDurability         = sOld.wDurability;
    dwDurabilityCount   = sOld.dwDurabilityCount;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign( const SITEMCUSTOM_117 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	//lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	//cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.Load_Ver100(sOld.randomOption);	

	m_PetDbNum			= sOld.m_PetDbNum;
	m_VehicleDbNum		= sOld.m_VehicleDbNum;

	bVietnamGainItem	= sOld.bVietnamGainItem;

	dwMainColor			= sOld.dwMainColor;
	dwSubColor			= sOld.dwSubColor;

	wDurability         = sOld.wDurability;
	dwDurabilityCount   = sOld.dwDurabilityCount;

	wRanPointTradeCount = sOld.wRanPointTradeCount;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign( const SITEMCUSTOM_118 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	//lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	//cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.Load_Ver100(sOld.randomOption);	

	m_PetDbNum			= sOld.m_PetDbNum;
	m_VehicleDbNum		= sOld.m_VehicleDbNum;

	bVietnamGainItem	= sOld.bVietnamGainItem;

	dwMainColor			= sOld.dwMainColor;
	dwSubColor			= sOld.dwSubColor;

	wDurability         = sOld.wDurability;
	dwDurabilityCount   = sOld.dwDurabilityCount;

	wRanPointTradeCount = sOld.wRanPointTradeCount;
	costumeUser         = sOld.costumeUser;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign( const SITEMCUSTOM_119 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	//lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	//cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.Load_Ver100(sOld.randomOption);	

	m_PetDbNum			= sOld.m_PetDbNum;
	m_VehicleDbNum		= sOld.m_VehicleDbNum;

	bVietnamGainItem	= sOld.bVietnamGainItem;

	dwMainColor			= sOld.dwMainColor;
	dwSubColor			= sOld.dwSubColor;

	wDurability         = sOld.wDurability;
	dwDurabilityCount   = sOld.dwDurabilityCount;

	wRanPointTradeCount = sOld.wRanPointTradeCount;
	costumeUser         = sOld.costumeUser;
	sTLGrind			= sOld.sTLGrind;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign( const SITEMCUSTOM_120 &sOld )
{
	sNativeID			= sOld.sNativeID;	
	nidDISGUISE			= sOld.nidDISGUISE;
	tBORNTIME			= sOld.tBORNTIME;
	tDISGUISE			= sOld.tDISGUISE;

	//lnGenNum			= sOld.lnGenNum;
	wTurnNum			= sOld.wTurnNum;
	cGenType			= sOld.cGenType;	
	//cChnID				= sOld.cChnID;		
	cPeriodExtensionCount = 0;
	cCostumePeriodExtensionCount = 0;

	cDAMAGE				= sOld.cDAMAGE;	
	cDEFENSE			= sOld.cDEFENSE;	
	cRESIST_FIRE		= sOld.cRESIST_FIRE;
	cRESIST_ICE			= sOld.cRESIST_ICE;
	cRESIST_ELEC		= sOld.cRESIST_ELEC;
	cRESIST_POISON		= sOld.cRESIST_POISON;
	cRESIST_SPIRIT		= sOld.cRESIST_SPIRIT;

	randomOption.Load_Ver100(sOld.randomOption);	

	m_PetDbNum			= sOld.m_PetDbNum;
	m_VehicleDbNum		= sOld.m_VehicleDbNum;

	bVietnamGainItem	= sOld.bVietnamGainItem;

	dwMainColor			= sOld.dwMainColor;
	dwSubColor			= sOld.dwSubColor;

	wDurability         = sOld.wDurability;
	dwDurabilityCount   = sOld.dwDurabilityCount;

	wRanPointTradeCount = sOld.wRanPointTradeCount;
	costumeUser         = sOld.costumeUser;
	sTLGrind			= sOld.sTLGrind;

	guid		= sOld.guid;
	itemState	= sOld.itemState;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_121& sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = sOld.tDISGUISE;

	tPERIODEX_BASIC = sOld.tPERIODEX_BASIC;
	tPERIODEX_DISGUISE = sOld.tPERIODEX_DISGUISE;

	wTurnNum = sOld.wTurnNum;
	cGenType = sOld.cGenType;
	cPeriodExtensionCount = sOld.cPeriodExtensionCount;
	cCostumePeriodExtensionCount = sOld.cCostumePeriodExtensionCount;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;
	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;

	randomOption.Load_Ver100(sOld.randomOption);

	m_PetDbNum = sOld.m_PetDbNum;
	m_VehicleDbNum = sOld.m_VehicleDbNum;

	bVietnamGainItem = sOld.bVietnamGainItem;

	dwMainColor = sOld.dwMainColor;
	dwSubColor = sOld.dwSubColor;

	wDurability = sOld.wDurability;
	dwDurabilityCount = sOld.dwDurabilityCount;

	wRanPointTradeCount = sOld.wRanPointTradeCount;
	costumeUser = sOld.costumeUser;
	sTLGrind = sOld.sTLGrind;

	guid = sOld.guid;
	itemState = sOld.itemState;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_122& sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = sOld.tDISGUISE;

	tPERIODEX_BASIC = sOld.tPERIODEX_BASIC;
	tPERIODEX_DISGUISE = sOld.tPERIODEX_DISGUISE;

	wTurnNum = sOld.wTurnNum;
	cGenType = sOld.cGenType;
	cPeriodExtensionCount = sOld.cPeriodExtensionCount;
	cCostumePeriodExtensionCount = sOld.cCostumePeriodExtensionCount;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;
	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;

	randomOption.Load_Ver100(sOld.randomOption);

	m_PetDbNum = sOld.m_PetDbNum;
	m_VehicleDbNum = sOld.m_VehicleDbNum;

	bVietnamGainItem = sOld.bVietnamGainItem;

	dwMainColor = sOld.dwMainColor;
	dwSubColor = sOld.dwSubColor;

	wDurability = sOld.wDurability;
	dwDurabilityCount = sOld.dwDurabilityCount;

	wRanPointTradeCount = sOld.wRanPointTradeCount;
	costumeUser = sOld.costumeUser;
	sTLGrind = sOld.sTLGrind;

	guid = sOld.guid;
	itemState = sOld.itemState;

	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
	{
		sBasicInfo.DefaultInitialize( pITEM );
		sAddonInfo.DefaultInitialize( pITEM );
	}

	// ���� ������ �����Ƿ� �߰��� �����Ѵ�;
	sBasicInfo.SetBasicAttackDamage( sOld.sBasicAttackDamage );
	sBasicInfo.SetBasicDefence( sOld.wBasicDefence );
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_123& sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = sOld.tDISGUISE;

	tPERIODEX_BASIC = sOld.tPERIODEX_BASIC;
	tPERIODEX_DISGUISE = sOld.tPERIODEX_DISGUISE;

	wTurnNum = sOld.wTurnNum;
	cGenType = sOld.cGenType;
	cPeriodExtensionCount = sOld.cPeriodExtensionCount;
	cCostumePeriodExtensionCount = sOld.cCostumePeriodExtensionCount;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;
	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;

	randomOption.Load_Ver100(sOld.randomOption);

	m_PetDbNum = sOld.m_PetDbNum;
	m_VehicleDbNum = sOld.m_VehicleDbNum;

	bVietnamGainItem = sOld.bVietnamGainItem;

	dwMainColor = sOld.dwMainColor;
	dwSubColor = sOld.dwSubColor;

	wDurability = sOld.wDurability;
	dwDurabilityCount = sOld.dwDurabilityCount;

	wRanPointTradeCount = sOld.wRanPointTradeCount;
	costumeUser = sOld.costumeUser;
	sTLGrind = sOld.sTLGrind;

	guid = sOld.guid;
	itemState = sOld.itemState;

	sBasicInfo = sOld.sBasicInfo;
	// ������ �⺻ ������ Default �� �ʱ�ȭ�Ѵ�;
	const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( pITEM )
		sAddonInfo.DefaultInitialize( pITEM );
}

void SITEMCUSTOM::Assign ( const SITEMCUSTOM_124& sOld )
{
	sNativeID = sOld.sNativeID;
	nidDISGUISE = sOld.nidDISGUISE;
	tBORNTIME = sOld.tBORNTIME;
	tDISGUISE = sOld.tDISGUISE;

	tPERIODEX_BASIC = sOld.tPERIODEX_BASIC;
	tPERIODEX_DISGUISE = sOld.tPERIODEX_DISGUISE;

	wTurnNum = sOld.wTurnNum;
	cGenType = sOld.cGenType;
	cPeriodExtensionCount = sOld.cPeriodExtensionCount;
	cCostumePeriodExtensionCount = sOld.cCostumePeriodExtensionCount;

	cDAMAGE = sOld.cDAMAGE;
	cDEFENSE = sOld.cDEFENSE;
	cRESIST_FIRE = sOld.cRESIST_FIRE;
	cRESIST_ICE = sOld.cRESIST_ICE;
	cRESIST_ELEC = sOld.cRESIST_ELEC;
	cRESIST_POISON = sOld.cRESIST_POISON;
	cRESIST_SPIRIT = sOld.cRESIST_SPIRIT;

	randomOption.Load_Ver100(sOld.randomOption);

	m_PetDbNum = sOld.m_PetDbNum;
	m_VehicleDbNum = sOld.m_VehicleDbNum;

	bVietnamGainItem = sOld.bVietnamGainItem;

	dwMainColor = sOld.dwMainColor;
	dwSubColor = sOld.dwSubColor;

	wDurability = sOld.wDurability;
	dwDurabilityCount = sOld.dwDurabilityCount;

	wRanPointTradeCount = sOld.wRanPointTradeCount;
	costumeUser = sOld.costumeUser;
	sTLGrind = sOld.sTLGrind;

	guid = sOld.guid;
	itemState = sOld.itemState;

	sBasicInfo = sOld.sBasicInfo;
	sAddonInfo = sOld.sAddonInfo;
}

void SITEMCUSTOM::Assign(const SITEM_LOBY& sOldLoby)
{
    MIN_STATIC_ASSERT(SITEM_LOBY::VERSION == 0x0103);
	sNativeID			= sOldLoby.sNativeID;	
	nidDISGUISE			= sOldLoby.nidDISGUISE;	

	cDAMAGE				= sOldLoby.cDAMAGE;	
	cDEFENSE			= sOldLoby.cDEFENSE;	

	dwMainColor			= sOldLoby.dwMainColor;
	dwSubColor			= sOldLoby.dwSubColor;
}

void SITEMCUSTOM::Assign ( const SITEMCLIENT &sClient )
{
	nidDISGUISE = sClient.nidDISGUISE;
	sNativeID = sClient.sNativeID;
	wTurnNum = sClient.wTurnNum;

	if ( sClient.GETOptTYPE1() == RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE )
	{
		randomOption.assign(0, sClient.cOptTYPE1, sClient.nOptVALUE1);
	}
}

void SITEMCUSTOM::AutoAssign( se::ByteStream& ByteStream )
{
    DWORD dwVersion, dwSize, dwNum;

    ByteStream >> dwVersion;
    ByteStream >> dwSize;
    ByteStream >> dwNum;

    SITEMCUSTOM sItemCustom;

    if ( dwVersion==0x0100 )
    {
        SITEMCUSTOM_100 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0101 )
    {
        SITEMCUSTOM_101 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0102 )
    {
        SITEMCUSTOM_102 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0103 )
    {
        SITEMCUSTOM_103 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0104 )
    {
        SITEMCUSTOM_104 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0105 )
    {
        SITEMCUSTOM_105 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0106 )
    {
        SITEMCUSTOM_106 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0107 )
    {
        SITEMCUSTOM_107 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0108 )
    {
        SITEMCUSTOM_108 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0109 )
    {
        SITEMCUSTOM_109 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0110 )
    {
        SITEMCUSTOM_110 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0111 )
    {
        SITEMCUSTOM_111 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0112 )
    {
        SITEMCUSTOM_112 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0113 )
    {
        SITEMCUSTOM_113 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0114 )
    {
        SITEMCUSTOM_114 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0115 )
    {
        SITEMCUSTOM_115 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
    else if ( dwVersion==0x0116 )
    {
        SITEMCUSTOM_116 sItemCustomOld;
        GASSERT(dwSize==sizeof(sItemCustomOld));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

        this->Assign( sItemCustomOld );
    }
	else if ( dwVersion==0x0117 )
	{
		SITEMCUSTOM_117 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0118 )
	{
		SITEMCUSTOM_118 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0119 )
	{
		SITEMCUSTOM_119 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0120 )
	{
		SITEMCUSTOM_120 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0121 )
	{
		SITEMCUSTOM_121 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0122 )
	{
		SITEMCUSTOM_122 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0123 )
	{
		SITEMCUSTOM_123 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0124 )
	{
		SITEMCUSTOM_124 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		this->Assign( sItemCustomOld );
	}
    else if ( dwVersion==SITEMCUSTOM::VERSION )
    {
        GASSERT(dwSize==sizeof(SITEMCUSTOM));
        ByteStream.ReadBuffer( (LPBYTE)&sItemCustom, sizeof(SITEMCUSTOM) );

        *this = sItemCustom;
    }
    else
    {
    }
}

void SITEMCUSTOM::AutoAssignForMigration( se::ByteStream& ByteStream )
{
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	SITEMCUSTOM sItemCustom;

	if ( dwVersion==0x0100 )
	{
		SITEMCUSTOM_100 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0101 )
	{
		SITEMCUSTOM_101 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0102 )
	{
		SITEMCUSTOM_102 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0103 )
	{
		SITEMCUSTOM_103 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0104 )
	{
		SITEMCUSTOM_104 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0105 )
	{
		SITEMCUSTOM_105 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0106 )
	{
		SITEMCUSTOM_106 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0107 )
	{
		SITEMCUSTOM_107 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0108 )
	{
		SITEMCUSTOM_108 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0109 )
	{
		SITEMCUSTOM_109 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0110 )
	{
		SITEMCUSTOM_110 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0111 )
	{
		SITEMCUSTOM_111 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0112 )
	{
		SITEMCUSTOM_112 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0113 )
	{
		SITEMCUSTOM_113 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0114 )
	{
		SITEMCUSTOM_114 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0115 )
	{
		SITEMCUSTOM_115 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0116 )
	{
		SITEMCUSTOM_116 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0117 )
	{
		SITEMCUSTOM_117 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0118 )
	{
		SITEMCUSTOM_118 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0119 )
	{
		SITEMCUSTOM_119 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0120 )
	{
		SITEMCUSTOM_120 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0121 )
	{
		SITEMCUSTOM_121 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0122 )
	{
		SITEMCUSTOM_122 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==0x0123 )
	{
		SITEMCUSTOM_123 sItemCustomOld;
		GASSERT(dwSize==sizeof(sItemCustomOld));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

		sItemCustomOld.sNativeID.wMainID += 3000;
		if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
		{
			sItemCustomOld.nidDISGUISE.wMainID += 3000;
		}

		this->Assign( sItemCustomOld );
	}
	else if ( dwVersion==SITEMCUSTOM::VERSION )
	{
		// ������ ������ �ö󰡵� xxx_migration(MIGRATION) �Լ����� ������ �ʿ� ����
		GASSERT(dwSize==sizeof(SITEMCUSTOM));
		ByteStream.ReadBuffer( (LPBYTE)&sItemCustom, sizeof(SITEMCUSTOM) );

		SNATIVEID ItemID = sItemCustom.GetRealNativeID();
		ItemID.wMainID += 3000;
		sItemCustom.SetNativeID( ItemID );
		if ( NATIVEID_NULL() != sItemCustom.nidDISGUISE )
		{
			sItemCustom.nidDISGUISE.wMainID += 3000;
		}

		*this = sItemCustom;
	}
	else
	{
	}
}

bool SITEMCUSTOM::IS_GUID_NULL()
{
	sc::MGUID guidnull;
	if ( guidnull == guid )
	{
		return true;
	}
	else
	{
		return false;
	}
}

SNATIVEID SITEMCUSTOM::GetNativeID() const
{
	if ( IsBoxWrapped() )
		return GetBoxWrappedItemID();
	return sNativeID;
}

EMITEMLEVEL SITEMCUSTOM::GetItemLevel () const
{
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( NULL == pItem )
		return EMITEM_LEVEL_NORMAL;

	return pItem->sBasicOp.emLevel;
}

// *****************************************************
// Desc: ��þƮ ������
// *****************************************************
WORD SITEMCUSTOM::GETGRADE_DAMAGE ( float fDurability  ) const
{
	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	BYTE cGRADE = grade_damage();
	if ( (WORD)cGRADE > GRADE_LIMIT_MAX )	cGRADE = GRADE_LIMIT_MAX;

    if ( cGRADE > GRADE_HIGH )
	{
		WORD wDAMAGE = getdamage(fDurability).wHigh; // ���� ������
		
		WORD wMaDAMAGE = getmadamage(fDurability);   // ��� ������
		WORD wDAMAGE_RATE = max (wDAMAGE,wMaDAMAGE);
		
		BYTE cOVER = cGRADE - GRADE_HIGH - 1;
		BYTE cOVER_N = GRADE_HIGH - GRADE_NORMAL;
		
		//	"����"
		//		*, GRADE_NORMAL ��� �̻��� ������ ����.
		//wDAMAGE_RATE = (WORD) ( wDAMAGE_RATE * GLCONST_CHAR::fDAMAGE_GRADE * cOVER_N * 0.01f );
		wDAMAGE_RATE = (WORD) ( ( wDAMAGE_RATE * sInfoByItemLevel.fDamageRateByGrade * cOVER_N ) / 100 );
		//		*, GRADE_NORMAL ��� ���ϴ� ���������� ����.
		//wDAMAGE_RATE += GLCONST_CHAR::wDAMAGE_GRADE * GRADE_NORMAL;
		wDAMAGE_RATE += sInfoByItemLevel.wDamageByGrade * GRADE_NORMAL;
		//		*, GRADE_HIGH ��� �̻��� ��޺� ������ ����
		//wDAMAGE_RATE = (WORD)( wDAMAGE_RATE * GLCONST_CHAR::fDAMAGE_GRADE_TOP[cOVER] );
		wDAMAGE_RATE = (WORD)( wDAMAGE_RATE * sInfoByItemLevel.fArrDamageRateByHighGrade[cOVER] );

		//	"����"
		//DWORD dwDAMAGE_VALUE = GLCONST_CHAR::wDAMAGE_GRADE * GRADE_HIGH;
		DWORD dwDAMAGE_VALUE = sInfoByItemLevel.wDamageByGrade * GRADE_HIGH;
		//	"����"
		//dwDAMAGE_VALUE = (WORD)(dwDAMAGE_VALUE * GLCONST_CHAR::fDAMAGE_GRADE_TOP[cOVER]);
		dwDAMAGE_VALUE = (WORD)(dwDAMAGE_VALUE * sInfoByItemLevel.fArrDamageRateByHighGrade[cOVER]);
		dwDAMAGE_VALUE = dwDAMAGE_VALUE > USHRT_MAX ? USHRT_MAX : WORD(dwDAMAGE_VALUE);

		//	"����", "����" �� ū���� ����.
		return (WORD)max ( wDAMAGE_RATE, dwDAMAGE_VALUE );


	}
	else if ( cGRADE > GRADE_NORMAL )
	{
		//	"���� ���ݷ�" ����� ������������ ���ġ ������ �ΰ��� �� ū���� ����.
		WORD wDAMAGE = getdamage(fDurability).wHigh; // ���� ������
		WORD wMaDAMAGE = getmadamage(fDurability);   // ��� ������
		WORD wDAMAGE_RATE = max (wDAMAGE,wMaDAMAGE);

		BYTE cOVER = cGRADE-GRADE_NORMAL;

		//	"����"
		//		*, GRADE_NORMAL ��� �̻��� ������ ����.
		//wDAMAGE_RATE = (WORD) ( wDAMAGE_RATE * GLCONST_CHAR::fDAMAGE_GRADE * cOVER * 0.01f );
		wDAMAGE_RATE = (WORD) ( wDAMAGE_RATE * sInfoByItemLevel.fDamageRateByGrade * cOVER * 0.01f );
		//		*, GRADE_NORMAL ��� ���ϴ� ���������� ����.
		//wDAMAGE_RATE += GLCONST_CHAR::wDAMAGE_GRADE * GRADE_NORMAL;
		wDAMAGE_RATE += sInfoByItemLevel.wDamageByGrade * GRADE_NORMAL;

		//	"����"
		//DWORD dwDAMAGE_VALUE = GLCONST_CHAR::wDAMAGE_GRADE * cGRADE;
		DWORD dwDAMAGE_VALUE = sInfoByItemLevel.wDamageByGrade * cGRADE;
		dwDAMAGE_VALUE = dwDAMAGE_VALUE > USHRT_MAX ? USHRT_MAX : WORD(dwDAMAGE_VALUE);

		//	"����", "����" �� ū���� ����.
		return (WORD)max ( wDAMAGE_RATE, dwDAMAGE_VALUE );
	}

	//	"����"
	//DWORD dwDAMAGE_VALUE = GLCONST_CHAR::wDAMAGE_GRADE * cGRADE;
	DWORD dwDAMAGE_VALUE = sInfoByItemLevel.wDamageByGrade * cGRADE;
	dwDAMAGE_VALUE = dwDAMAGE_VALUE > USHRT_MAX ? USHRT_MAX : WORD(dwDAMAGE_VALUE);

	return (WORD)dwDAMAGE_VALUE;
}

WORD SITEMCUSTOM::GETGRADE_DEFENSE () const
{
	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	BYTE cGRADE = grade_defense();
	if ( (WORD)cGRADE > sInfoByItemLevel.wMaxGrade /*GLCONST_CHAR::wGRADE_MAX*/ )
		cGRADE = (BYTE)sInfoByItemLevel.wMaxGrade/*(BYTE)GLCONST_CHAR::wGRADE_MAX*/;

	//	���� ���� ��.
	if ( cGRADE > GRADE_HIGH )
	{
		//	"���� ����" ����.
		WORD wDEFENSE_RATE = getdefense();

		BYTE cOVER = cGRADE - GRADE_HIGH - 1;
		BYTE cOVER_N = GRADE_HIGH - GRADE_NORMAL;

		//wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE*GLCONST_CHAR::fDEFENSE_GRADE * 0.01f );
		//wDEFENSE_RATE = (WORD)(wDEFENSE_RATE*cOVER_N);

		//	"����"
		//		*, GRADE_NORMAL ��� �̻��� ������ ����.
		//wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE*GLCONST_CHAR::fDEFENSE_GRADE * cOVER_N * 0.01f );
		wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE*sInfoByItemLevel.fDefenceRateByGrade * cOVER_N * 0.01f );
		//		*, GRADE_NORMAL ��� ���ϴ� ���������� ����.
		//wDEFENSE_RATE += GLCONST_CHAR::wDEFENSE_GRADE*GRADE_NORMAL;
		wDEFENSE_RATE += sInfoByItemLevel.wDefenceByGrade*GRADE_NORMAL;
		//		*, GRADE_HIGH ��� �̻��� ��޺� ������ ����
		//wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE * GLCONST_CHAR::fDEFENSE_GRADE_TOP[cOVER] );
		wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE * sInfoByItemLevel.fArrDefenceRateByHighGrade[cOVER] );

		//	"����"
		//WORD wDEFENSE_VALUE = GLCONST_CHAR::wDEFENSE_GRADE * GRADE_HIGH;
		WORD wDEFENSE_VALUE = sInfoByItemLevel.wDefenceByGrade * GRADE_HIGH;
		
		//	"����"
		//wDEFENSE_VALUE = (WORD)(wDEFENSE_VALUE * GLCONST_CHAR::fDEFENSE_GRADE_TOP[cOVER]);	
		wDEFENSE_VALUE = (WORD)(wDEFENSE_VALUE * sInfoByItemLevel.fArrDefenceRateByHighGrade[cOVER]);	

		//	"����", "����" �� ū���� ����.
		return max ( wDEFENSE_RATE, wDEFENSE_VALUE );


	}
	else if ( cGRADE > GRADE_NORMAL )
	{
		//	"���� ����" ����.
		WORD wDEFENSE_RATE = getdefense();


		BYTE cOVER = cGRADE-GRADE_NORMAL;

		//wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE*GLCONST_CHAR::fDEFENSE_GRADE * 0.01f );

		//wDEFENSE_RATE = (WORD)(wDEFENSE_RATE*cOVER);

		//	"����"
		//		*, GRADE_NORMAL ��� �̻��� ������ ����.
		//wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE*GLCONST_CHAR::fDEFENSE_GRADE * cOVER * 0.01f );
		wDEFENSE_RATE = (WORD) ( wDEFENSE_RATE*sInfoByItemLevel.fDefenceRateByGrade * cOVER * 0.01f );
		//		*, GRADE_NORMAL ��� ���ϴ� ���������� ����.
		//wDEFENSE_RATE += GLCONST_CHAR::wDEFENSE_GRADE*GRADE_NORMAL;
		wDEFENSE_RATE += sInfoByItemLevel.wDefenceByGrade*GRADE_NORMAL;

		//	"����"
		//WORD wDEFENSE_VALUE = GLCONST_CHAR::wDEFENSE_GRADE * cGRADE;
		WORD wDEFENSE_VALUE = sInfoByItemLevel.wDefenceByGrade * cGRADE;

		//	"����", "����" �� ū���� ����.
		return max ( wDEFENSE_RATE, wDEFENSE_VALUE );
	}

	//	"����"
	//WORD wDEFENSE_VALUE = GLCONST_CHAR::wDEFENSE_GRADE * cGRADE;
	WORD wDEFENSE_VALUE = sInfoByItemLevel.wDefenceByGrade * cGRADE;
	return wDEFENSE_VALUE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_FIRE () const
{
	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	BYTE cGRADE = grade_fire();
	if ( (WORD)cGRADE > GRADE_LIMIT_MAX)	cGRADE = GRADE_LIMIT_MAX;

	//return GLCONST_CHAR::wRESIST_FIRE_GRADE * cGRADE;
	return sInfoByItemLevel.wFireResistByGrade * cGRADE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_ICE () const
{
	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	BYTE cGRADE = grade_ice();
	if ( (WORD)cGRADE > GRADE_LIMIT_MAX )	cGRADE = GRADE_LIMIT_MAX;

	//return GLCONST_CHAR::wRESIST_ICE_GRADE * cGRADE;
	return sInfoByItemLevel.wIceResistByGrade * cGRADE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_ELEC () const
{
	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	BYTE cGRADE = grade_elec();
	if ( (WORD)cGRADE > GRADE_LIMIT_MAX )	cGRADE = GRADE_LIMIT_MAX;

	//return GLCONST_CHAR::wRESIST_ELEC_GRADE * cGRADE;
	return sInfoByItemLevel.wElectricResistByGrade * cGRADE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_POISON () const
{
	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	BYTE cGRADE = grade_poison();
	if ( (WORD)cGRADE > GRADE_LIMIT_MAX )	cGRADE = GRADE_LIMIT_MAX;

	//return GLCONST_CHAR::wRESIST_POISON_GRADE * cGRADE;
	return sInfoByItemLevel.wPoisonResistByGrade * cGRADE;
}

WORD SITEMCUSTOM::GETGRADE_RESIST_SPIRIT () const
{
	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	BYTE cGRADE = grade_spirit();
	if ( (WORD)cGRADE > GRADE_LIMIT_MAX )	cGRADE = GRADE_LIMIT_MAX;

	//return GLCONST_CHAR::wRESIST_SPIRIT_GRADE * cGRADE;
	return sInfoByItemLevel.wSpiritResistByGrade * cGRADE;
}

BYTE SITEMCUSTOM::grade_damage () const
{
	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	const BYTE gradeDamage(BYTE(cDAMAGE) + BYTE(GETOptVALUE(RandomOption::RANDOM_OPTION_GRIND_DAMAGE)));	
	//return gradeDamage > BYTE(GLCONST_CHAR::wGRADE_MAX) ? BYTE(GLCONST_CHAR::wGRADE_MAX) : gradeDamage;

	return gradeDamage > BYTE(sInfoByItemLevel.wMaxGrade) ? BYTE(sInfoByItemLevel.wMaxGrade) : gradeDamage;
}

BYTE SITEMCUSTOM::grade_defense () const
{
	const SINFO_BYITEMLEVEL& sInfoByItemLevel =
		GLItemLevelMan::Instance()->GetInfoByItemLevel( GetItemLevel() );

	const BYTE gradeDefense(BYTE(cDEFENSE) + BYTE(GETOptVALUE(RandomOption::RANDOM_OPTION_GRIND_DEFENCE)));	
	//return gradeDefense > BYTE(GLCONST_CHAR::wGRADE_MAX) ? BYTE(GLCONST_CHAR::wGRADE_MAX) : gradeDefense;

	return gradeDefense > BYTE(sInfoByItemLevel.wMaxGrade) ? BYTE(sInfoByItemLevel.wMaxGrade) : gradeDefense;
}

BYTE SITEMCUSTOM::GETGRADE ( EMGRINDING_TYPE emTYPE ) const
{
	switch ( emTYPE )
	{
		//	Note : EMGRINDING_NUNE ���� ��� ���ǽÿ� ������ ���� ��� ǥ��. ( �� ���� �κ� ���� )
	case EMGRINDING_NUNE:
		{
			BYTE cDAM = grade_damage();
			BYTE cDEF = grade_defense();
			return cDAM > cDEF ? cDAM : cDEF;
		}

	case EMGRINDING_DAMAGE:
		return grade_damage();

	case EMGRINDING_DEFENSE:
		return grade_defense();

	case EMGRINDING_RESIST_FIRE:
		return grade_fire();

	case EMGRINDING_RESIST_ICE:
		return grade_ice();

	case EMGRINDING_RESIST_ELEC:
		return grade_elec();

	case EMGRINDING_RESIST_POISON:
		return grade_poison();

	case EMGRINDING_RESIST_SPIRIT:
		return grade_spirit();
	};

	return 0;
}

BYTE SITEMCUSTOM::GETGRADE_EFFECT () const
{
	BYTE cGRADE = GETGRADE(EMGRINDING_NUNE);

	if ( cGRADE==0 )	return 0;

	// +9 ��� ������ ��� 
	if( cGRADE <= 9 )
	{
		return cGRADE = 1 + (cGRADE-1)/2;
	}
	
	return cGRADE = cGRADE - 4;

}

inline float SITEMCUSTOM::GETOptVALUE ( const RandomOption::RANDOM_OPTION _typeRandomOption, short nVALUE ) const
{
	switch ( _typeRandomOption )
	{
	default:
	case RandomOption::RANDOM_OPTION_NULL:
		return 0.0f;

	case RandomOption::RANDOM_OPTION_RATE_DAMAGE:
	case RandomOption::RANDOM_OPTION_RATE_DEFENCE:
	case RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE:
		return nVALUE*0.01f;

	case RandomOption::RANDOM_OPTION_RATE_HIT:
	case RandomOption::RANDOM_OPTION_RATE_AVOID:
		return nVALUE*0.01f;

	case RandomOption::RANDOM_OPTION_VALUE_HP:
	case RandomOption::RANDOM_OPTION_VALUE_MP:
	case RandomOption::RANDOM_OPTION_VALUE_SP:
		return nVALUE;

	case RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP:
	case RandomOption::RANDOM_OPTION_RATE_RECOVERY_MP:
	case RandomOption::RANDOM_OPTION_RATE_RECOVERY_SP:
	case RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP:
		return nVALUE*0.0001f;

	case RandomOption::RANDOM_OPTION_GRIND_DAMAGE:
	case RandomOption::RANDOM_OPTION_GRIND_DEFENCE:
		return nVALUE;

	case RandomOption::RANDOM_OPTION_RATE_RANGE_ATTACK:
		return nVALUE*0.01f;

	case RandomOption::RANDOM_OPTION_CONSUME_SP:
		return nVALUE;

	case RandomOption::RANDOM_OPTION_VALUE_RESIST:
		return nVALUE;

	case RandomOption::RANDOM_OPTION_VALUE_DAMAGE:
	case RandomOption::RANDOM_OPTION_VALUE_DEFENCE:
	case RandomOption::RANDOM_OPTION_VALUE_HIT:
	case RandomOption::RANDOM_OPTION_VALUE_AVOID:
		return nVALUE;

	case RandomOption::RANDOM_OPTION_VALUE_POW:
	case RandomOption::RANDOM_OPTION_VALUE_STR:
	case RandomOption::RANDOM_OPTION_VALUE_SPI:
	case RandomOption::RANDOM_OPTION_VALUE_DEX:
	case RandomOption::RANDOM_OPTION_VALUE_STA:
		return nVALUE;

	case RandomOption::RANDOM_OPTION_VALUE_MELEE:
	case RandomOption::RANDOM_OPTION_VALUE_RANGE:
	case RandomOption::RANDOM_OPTION_VALUE_MAGIC:
		return nVALUE;

	case RandomOption::RANDOM_OPTION_VALUE_POTION_HP:
	case RandomOption::RANDOM_OPTION_VALUE_POTION_MP:
	case RandomOption::RANDOM_OPTION_VALUE_POTION_SP:
	case RandomOption::RANDOM_OPTION_VALUE_KILL_CP:
		return nVALUE;

	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_DURATIONTIME_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_2_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_3_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_4_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_5_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_6_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_7_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_8_DURATIONTIME_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_9_DURATIONTIME_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_10_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_11_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_12_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_13_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_14_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_15_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_16_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_17_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_18_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_19_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_20_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_21_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_22_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_23_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_24_DURATIONTIME_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_25_DURATIONTIME_ADD:
		return nVALUE*0.01f;
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_DAMAGE_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_2_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_3_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_4_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_5_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_6_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_7_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_8_DAMAGE_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_9_DAMAGE_ADD: 
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_10_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_11_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_12_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_13_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_14_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_15_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_16_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_17_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_18_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_19_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_20_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_21_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_22_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_23_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_24_DAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_25_DAMAGE_ADD:
		return nVALUE*0.01f;
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_1_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_2_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_3_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_4_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_5_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_6_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_7_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_8_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_9_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_10_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_11_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_12_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_13_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_14_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_15_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_16_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_17_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_18_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_19_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_20_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_21_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_22_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_23_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_24_SKILLDILAY_DEC:
	case RandomOption::RANDOM_OPTION_VALUE_SKILL_GRADE_25_SKILLDILAY_DEC:
		return nVALUE *0.01f;
	case RandomOption::RANDOM_OPTION_VALUE_CRITICALDAMAGE_ADD:
	case RandomOption::RANDOM_OPTION_VALUE_CRUSHINGBLOW_ADD:
		return nVALUE *0.001f;
	case RandomOption::RANDOM_OPTION_VALUE_WEARABLELEVEL_ADD:
		return nVALUE;
		break;
	case RandomOption::RANDOM_OPTION_VALUE_LIFE_PER_KILL:
	case RandomOption::RANDOM_OPTION_VALUE_MP_PER_KILL:
	case RandomOption::RANDOM_OPTION_VALUE_SP_PER_KILL:
	case RandomOption::RANDOM_OPTION_VALUE_HPMPSP_PER_KILL:
		return nVALUE *0.001f;
	case RandomOption::RANDOM_OPTION_VALUE_ALLMPRATIO_TO_ATTACK:
	case RandomOption::RANDOM_OPTION_VALUE_ALLSPRATIO_TO_HP:
	case RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MELEE:
	case RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_RANGE:
	case RandomOption::RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MAGIC:
		return nVALUE *0.001f;
	};

	return 0.0f;
}

const bool SITEMCUSTOM::SETOptVALUE ( int nIndex, const RandomOption::RANDOM_OPTION _typeRandomOption, float fVALUE )
{
	return randomOption.setOption(nIndex, _typeRandomOption, fVALUE);	
}

const bool SITEMCUSTOM::SETOptVALUEEmpty( const RandomOption::RANDOM_OPTION _typeRandomOption, float fVALUE )	// ITEMREBUILD_MARK
{
	return randomOption.setOption(_typeRandomOption, fVALUE);	
}

int SITEMCUSTOM::RESETOptVALUE(const ITEM_RANDOMOP_UTILL::FixOption& fixOption, const DWORD nFixMaximum)
{
	return randomOption.reset(cDAMAGE, cDEFENSE, fixOption, nFixMaximum);
}


float SITEMCUSTOM::GETOptVALUE1 () const
{
	const RandomOption::RANDOM_OPTION _typeRandomOption(GETOptTYPE1());
	short nVALUE = randomOption.getValue(0);

	return GETOptVALUE ( _typeRandomOption, nVALUE );
}

float SITEMCUSTOM::GETOptVALUE2 () const
{
	const RandomOption::RANDOM_OPTION _typeRandomOption(GETOptTYPE2());
	short nVALUE = randomOption.getValue(1);

	return GETOptVALUE ( _typeRandomOption, nVALUE );
}

float SITEMCUSTOM::GETOptVALUE3 () const
{
	const RandomOption::RANDOM_OPTION _typeRandomOption(GETOptTYPE3());
	short nVALUE = randomOption.getValue(2);

	return GETOptVALUE ( _typeRandomOption, nVALUE );
}

float SITEMCUSTOM::GETOptVALUE4 () const
{
	const RandomOption::RANDOM_OPTION _typeRandomOption(GETOptTYPE4());
	short nVALUE = randomOption.getValue(3);

	return GETOptVALUE ( _typeRandomOption, nVALUE );
}

float SITEMCUSTOM::GETOptVALUE5 () const
{
	const RandomOption::RANDOM_OPTION _typeRandomOption(GETOptTYPE5());
	short nVALUE = randomOption.getValue(4);

	return GETOptVALUE ( _typeRandomOption, nVALUE );
}

float SITEMCUSTOM::getOptValue(const DWORD nIndex) const
{
	const RandomOption::RANDOM_OPTION _typeRandomOption(RandomOption::RANDOM_OPTION(randomOption.getType(nIndex)));
	const short nValue(randomOption.getValue(nIndex));

	return GETOptVALUE ( _typeRandomOption, nValue );
}

const DWORD SITEMCUSTOM::getOptCount(const DWORD nIndex) const
{
	return randomOption.getCount(nIndex);
}

BOOL SITEMCUSTOM::IsPerRandOpt( INT nRandOPT ) const
{
	if ( nRandOPT >= RandomOption::RANDOM_OPTION_NSIZE )
		return FALSE;

	return RandomOption::bValueInteger[nRandOPT] == true ? false : true;	
}

const int SITEMCUSTOM::GetOptIndex ( const RandomOption::RANDOM_OPTION _typeRandomOption ) const
{
	if ( GETOptTYPE1() == _typeRandomOption )
		return 0;
	else if ( GETOptTYPE2() == _typeRandomOption )
		return 1;
	else if ( GETOptTYPE3() == _typeRandomOption )
		return 2;
	else if ( GETOptTYPE4() == _typeRandomOption )
		return 3;
	else if ( GETOptTYPE5() == _typeRandomOption )
		return 4;

	return -1;
}

float SITEMCUSTOM::GETOptVALUE ( const RandomOption::RANDOM_OPTION _typeRandomOption) const
{
	if ( GETOptTYPE1() == _typeRandomOption )
		return GETOptVALUE1();
	else if ( GETOptTYPE2() == _typeRandomOption )
		return GETOptVALUE2();
	else if ( GETOptTYPE3() == _typeRandomOption )
		return GETOptVALUE3();
	else if ( GETOptTYPE4() == _typeRandomOption )
		return GETOptVALUE4();
	else if ( GETOptTYPE5() == _typeRandomOption )
		return GETOptVALUE5();

	return 0;
}

short SITEMCUSTOM::GETOptVALUE_SHORT ( const RandomOption::RANDOM_OPTION _typeRandomOption ) const
{
	if ( GETOptTYPE1() == _typeRandomOption )
		return randomOption.getValue(0);
	else if ( GETOptTYPE2() == _typeRandomOption )
		return randomOption.getValue(1);
	else if ( GETOptTYPE3() == _typeRandomOption )
		return randomOption.getValue(2);
	else if ( GETOptTYPE4() == _typeRandomOption )
		return randomOption.getValue(3);
	else if ( GETOptTYPE5() == _typeRandomOption )
		return randomOption.getValue(4);

	return 0;
}

BOOL SITEMCUSTOM::IsSetRandOpt() const
{
	if( GETOptTYPE1() != RandomOption::RANDOM_OPTION_NULL )
		return TRUE;
	else if( GETOptTYPE2() != RandomOption::RANDOM_OPTION_NULL )
		return TRUE;
	else if( GETOptTYPE3() != RandomOption::RANDOM_OPTION_NULL )
		return TRUE;
	else if( GETOptTYPE4() != RandomOption::RANDOM_OPTION_NULL )
		return TRUE;
	else if( GETOptTYPE5() != RandomOption::RANDOM_OPTION_NULL )
		return TRUE;

	return FALSE;
}

#define USAGE_RANDOM_OPT_SE
#ifdef USAGE_RANDOM_OPT_SE
//#ifdef DAUMTESTPARAM	// �ѱ� �׼������� 2�� ���� �ɼ� ���� ���
	const bool SITEMCUSTOM::GENERATE_RANDOM_OPT(const bool bGenItem, const ITEM_RANDOMOP_UTILL::FixOption& fixOption, SITEM* pItemStamp)
	{
		RandomOption::Entity *pRANDOM_SET(GLItemMan::GetInstance().GetItemRandomOpt(GetNativeID()));
		if ( pRANDOM_SET == NULL )
			return false;		

		const RandomOption::Entity& _entityRandomOption(*pRANDOM_SET);

		//	���� �ɼ� �߻� Ȯ�� �˻�.
		const float _fRateRandomOptionGenerate(_entityRandomOption.getRateRandomOptionGenerate());
        if (bGenItem && !sc::Random::getInstance().IsSucceed(_fRateRandomOptionGenerate))
			return true;		

		unsigned int nGEN( 0 );
		bool bSuccess = false; // �� �ɼǸ��� ����Ȯ���� �����ϱ⶧���� �־��� ��� ��� �����ؼ� �ɼ��� �Ⱥٴ� ��찡 �߻��ϹǷ� �ּ� 1���� ������ �ֵ��� �����ϱ� ���ؼ� ���

		const RandomOption::GENERATE _typeGenerate(bGenItem == true ? RandomOption::GENERATE_DROP : RandomOption::GENERATE_REMODEL);
		const unsigned int _sizeValueBunch(_entityRandomOption.getSizeValueBunch(_typeGenerate));
		//  �����ɼ� ���� ���;
		//  * �ε��� ������ ����;
		//  * ���ʴ�� Ȯ�� �˻��� ����Ѵٸ� ����;
		//  * ���н� ���� �ε��� ����� �ּ�/�ִ� �ɼǰ����� �°� �ٽ� ������;
		//     ���ǿ� �°� �ݺ�����ӵ����°ͺ��� �� ����� �� ȿ������;

		//	������ �ɼ��� ���� ������ �ο� �ϱ� ���ؼ� ������ ����.
		std::vector< DWORD > vecORDER, vecFailedOrder;		
		vecORDER.reserve( _sizeValueBunch );
		for( DWORD _index(0); _index < _sizeValueBunch; ++_index )
			vecORDER.push_back(_index);

		std::random_shuffle(vecORDER.begin(), vecORDER.end());

		// ���� �� ������ ������ ������ �������� ������ �ʱ�ȭ ��Ű�� �����Ƿ�, ������ ���� �� �ִ� ������ �����Ѵ�
		const WORD wFixMaximum(pItemStamp == NULL ? 0 : pItemStamp->sSuitOp.wReModelNum);
		if ( bGenItem == false )
			nGEN = (unsigned int)RESETOptVALUE(fixOption, wFixMaximum);


		RandomOption::RANDOM_OPTION defaultOptOption = RandomOption::RANDOM_OPTION_NULL;
		RandomOption::ValueBunch defuultOptValueBunch ; 

		//	�����ɼ� �߻�.
		const unsigned int _sizeRandomOptionGenerate(_entityRandomOption.getSizeRandomOptionGenerate(_typeGenerate));
		for( unsigned int _index(0); (_index < _sizeValueBunch) && (nGEN < _sizeRandomOptionGenerate); ++_index )
		{
			//	���� ���� ���� ����.
			float fNowRate = sc::Random::getInstance().GetFloatPercent();
			const DWORD dwORDER = vecORDER[_index];

			// ���� �����Ǵ� �����۰� �����ϴ� �������� ���� �ɼ� ������ �����Ѵ�
			const RandomOption::ValueBunchVector& _vectorValueBunch(*_entityRandomOption.getVectorValueBunch(_typeGenerate));

			const RandomOption::ValueBunch& _valueBunch(_vectorValueBunch[dwORDER]);
			const RandomOption::RANDOM_OPTION _typeRandomOption(_valueBunch.getTypeRandomOption());

			if ( _typeRandomOption == RandomOption::RANDOM_OPTION_NULL )
				continue;

			// ���� �� ���� �ɼ� Ÿ���� ���ݷ� �����̰�, ������ ���ݷ� ������ �̹� ������ �ɼ� �߻��� ����Ѵ�.
			// ���� �� ���� �ɼ� Ÿ���� ���� �����̰�, ������ ���� ������ �̹� ������ �ɼ� �߻��� ����Ѵ�.
			if ( bGenItem == false )
			{
				switch ( _typeRandomOption )
				{
				case RandomOption::RANDOM_OPTION_GRIND_DAMAGE:
					if ( cDAMAGE != 0 )
						continue;
					break;
				case RandomOption::RANDOM_OPTION_GRIND_DEFENCE:
					if ( cDEFENSE != 0 )
						continue;
					break;
				}
			}

			if ( fixOption == (BYTE)_typeRandomOption )
				continue;

			// ��� ������ ��츦 ���ؼ� ���������� �ɼ� �ο� �õ��� �ߴ� ���� ����
			defaultOptOption = _typeRandomOption;
			defuultOptValueBunch = _valueBunch;

			//	�߻�Ȯ���� �����Ҷ� �߻�.
			if ( fNowRate < _valueBunch.getRateChoose() )
			{

				const RandomOption::Value& _value(_valueBunch.getValue());				
				const float _fValue(_value.getValue(RandomOption::bValueInteger[_valueBunch.getTypeRandomOption()]));
				

				// Ȯ���� �°� ������ ������ �ϳ� ���´�
				if ( SETOptVALUEEmpty(_typeRandomOption, _fValue) == true )
				{
					++nGEN;
					bSuccess = true;
				}
			}
			else // SETOptVALUEEmpty�� �����ϸ� ������ ��ü�� �Ұ�����;
			{
				vecFailedOrder.push_back(dwORDER);
			}
		}

		std::random_shuffle(vecFailedOrder.begin(), vecFailedOrder.end());
		//gbgim;
		// ���  �������� �ƴҰ��, �ּ� �����ɼ� üũ�� �������ش�;
		if ( bGenItem == false )
		{
			for ( int i = 0; (nGEN < _entityRandomOption.GetMinGain()) && (i < (int)vecFailedOrder.size()); ++i )
			{
				// ���� �����Ǵ� �����۰� �����ϴ� �������� ���� �ɼ� ������ �����Ѵ�;
				const DWORD dwORDER = vecFailedOrder[i];
				const RandomOption::ValueBunchVector& _vectorValueBunch(*_entityRandomOption.getVectorValueBunch(_typeGenerate));

				const RandomOption::ValueBunch& _valueBunch(_vectorValueBunch[dwORDER]);
				const RandomOption::RANDOM_OPTION _typeRandomOption(_valueBunch.getTypeRandomOption());

				if ( _typeRandomOption == RandomOption::RANDOM_OPTION_NULL )
					continue;

				const RandomOption::Value& _value(_valueBunch.getValue());				
				const float _fValue(_value.getValue(RandomOption::bValueInteger[_valueBunch.getTypeRandomOption()]));
				// Ȯ���� �°� ������ ������ �ϳ� ���´�;
				if ( SETOptVALUEEmpty(_typeRandomOption, _fValue) == true )
				{
					++nGEN;
					bSuccess = true;
				}
			}
		}
		//

		// SETOptVALUEEmpty() ���� �ߺ� ó���� �ϰ� �ִ�;
		// ���� �ɼ� �ߺ� ������ ���� ���� �ʴ´ٸ� �Ʒ� �ڵ带 ����� ���� �غ��� �Ѵ�;
		unsigned int _nIndex(-1);
		do
		{
			_nIndex = -1;
			// �ߺ� Ÿ���� �ִ��� �˻��ؼ� ���� ó�� ���ش�;
			for ( DWORD _i(_sizeRandomOptionGenerate); _i--; )
			{
				if ( randomOption.option[_i].cOptType == 0 )
					continue;

				for ( DWORD _j(0); _j < _i; ++_j )
				{
					if ( randomOption.option[_i].cOptType == randomOption.option[_j].cOptType )
					{
						_nIndex = _j;
					}
				}				
			}			
			randomOption.remove(_nIndex);			
		}
		while ( _nIndex != -1 );
		

		return true;
	}

	const bool SITEMCUSTOM::ConfirmValidRandomOpt ()
	{
		// �������� ���� �ɼ� ���� ������ ���´�;
		RandomOption::Entity* pRANDOM_SET =
			GLItemMan::GetInstance().GetItemRandomOpt( GetNativeID() );
		if ( NULL == pRANDOM_SET )
			return true;

		const RandomOption::Entity& sEntityRandomOption = *pRANDOM_SET;

		// ���� �������� �����ɼ��� ��ȿ���� üũ�Ѵ�;
		for ( int i=0; i<RandomOpt::NSIZE; ++i )
		{
			// ���� �ɼ� Ÿ���� ���´�;
			const RandomOption::RANDOM_OPTION randomOptType =
				static_cast< const RandomOption::RANDOM_OPTION >(
				randomOption.getType( i ) );

			// Ÿ���� ������ ���´�;
			RandomOption::Information sInformation;
			if ( sEntityRandomOption.getInformation(
				RandomOption::GENERATE_DROP,
				RandomOption::RANDOM_OPTION( randomOptType ),
				sInformation ) )
			{
				// ���� ���ͼ� �ִ밪 ��ȿ���� üũ�Ѵ�;
				// �ּҰ��� ���� �����ɼ��� ��� �� ���� ���� �ֱ� ������;
				// üũ���� �ʴ´�;
				const int nValue = static_cast< const int >(
					randomOption.getValue( i ) );
				if ( nValue > sInformation.valueIntegerHigh )
				{
					sc::writeLogInfo ( sc::string::format(
						"Invalid Random Option, %1%~%2% ( %3% )",
						sInformation.valueIntegerLow,
						sInformation.valueIntegerHigh,
						nValue ) );

					return false;
				}
			}
		}

		// ��� ��ȿ�� ��� ��ȿ�� �������̴�;
		return true;
	}

// 	int SITEMCUSTOM::GENERATE_RANDOM_OPT_SE( const SRANDOM_OPT_SET* pOptSet )
// 	{
// 		if( !pOptSet )
// 			return INT_MIN;
// 
// 		float fRatePer = sc::Random::getInstance().getFloatPercent();
// 		float fRateCur = 0.0f;
// 
// 		for( WORD i=0; i<pOptSet->wSECT; ++i )
// 		{
// 			float fRate = pOptSet->pSECT[i].fRATE;
// 
// 			if( ( fRateCur <= fRatePer ) && ( fRatePer < ( fRateCur + fRate ) ) )
// 				return i;
// 
// 			fRateCur += fRate;
// 		}
// 
// 		return INT_MIN;
// 	}
	#undef USAGE_RANDOM_OPT_SE
#else
	bool SITEMCUSTOM::GENERATE_RANDOM_OPT( bool bGenItem, const ITEM_RANDOMOP_UTILL::FixOption& fixOption )
	{
		SRANDOM_GEN *pRANDOM_SET = GLItemMan::GetInstance().GetItemRandomOpt( GetNativeID() );
		if( !pRANDOM_SET )
			return false;

		const SRANDOM_GEN &sRANDOM = *pRANDOM_SET;

		//	���� �ɼ� �߻� Ȯ�� �˻�.
		if( !RANDOM_GEN( sRANDOM.fRATE ) )
			return true;

		int nGEN( 0 );

		//	������ �ɼ��� ���� ������ �ο� �ϱ� ���ؼ� ������ ����.
		std::vector< DWORD > vecORDER;
		vecORDER.reserve( SRANDOM_GEN::MAX_SET );
		for( DWORD i=0; i<sRANDOM.dwSET_NUM; ++i )
			vecORDER.push_back( i );

		std::random_shuffle( vecORDER.begin(), vecORDER.end() );

		//	�����ɼ� �߻�.
		for( i=0; i<sRANDOM.dwSET_NUM && nGEN<SRANDOM_GEN::MAX_OPT; ++i )
		{
			float fNowRate = sc::Random::getInstance().GetFloatPercent();
			DWORD dwORDER = vecORDER[i];

			//	���� ���� ���� ����.
			const SRANDOM_OPT_SET &sSET = sRANDOM.sSET[dwORDER];
			if( sSET.wTYPE == RandomOption::RANDOM_OPTION_NULL )
				continue;

			//	�߻�Ȯ���� �����Ҷ� �߻�.
			if( fNowRate<sSET.fRATE )
			{
				float fPOS = sc::Random::getInstance().GetFloatPercent();
				float fVALUE = sSET.fLOW + (sSET.fHIGH-sSET.fLOW) * fPOS * 0.01f;
				SETOptVALUE( nGEN++, (EMRANDOM_OPT)sSET.wTYPE, fVALUE );
			}
		}

		return true;
	}

	int SITEMCUSTOM::GENERATE_RANDOM_OPT_SE( const SRANDOM_OPT_SET* pOptSet )
	{
		return INT_MIN;
	}
#endif

// ������ �⺻ �ɷ�ġ ���� �ο�;
const bool SITEMCUSTOM::GenerateBasicStat ( const bool _bDropItem, const WORD _wFixedIndex )
{
	// ������ �����͸� �޾ƿ´�;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( GetNativeID() );
	if ( NULL == pItem )
		return false;

	// �������� ����� ���Ѵ�;
	EMITEMLEVEL emItemLevel = pItem->GetItemLevel();

	// �������� �⺻ ���ݷ�, ������ �����ɷ�ġ Index �� ���Ѵ�;
	int nBasicAttackDamageType = pItem->GetBasicAttackDamageType();
	int nBasicAttackDamageIndex = pItem->GetBasicAttackDamageIndex();
	int nBasicDefenceType = pItem->GetBasicDefenceType();
	int nBasicDefenceIndex = pItem->GetBasicDefenceIndex();

	// ��� �� �⺻ ���ݷ�, ���� ���� ��Ʈ �Ŵ����� ����� ���Ѵ�;
	int nBasicAttackDamageIndex_MaxSize = GLItemBasicStatMan::Instance()->GetSizeAttackItemSetMan( emItemLevel, nBasicAttackDamageType );
	int nBasicDefenceIndex_MaxSize = GLItemBasicStatMan::Instance()->GetSizeDefenceItemSetMan( emItemLevel, nBasicDefenceType );

	// ����Ǵ� �������� ��� ó���Ѵ�;
	if ( _bDropItem )
	{
		if ( 0 <= nBasicAttackDamageIndex && nBasicAttackDamageIndex < nBasicAttackDamageIndex_MaxSize )
		{
			sBasicInfo.GenerateBasicAttackDamage_Drop( emItemLevel, nBasicAttackDamageType, nBasicAttackDamageIndex );
		}
		else
		{
			// �����ɷ�ġ Index�� �������� ���� �ƴ� ��� ( �Ϲ������� -1 �ƴ϶�� ���� �Ǽ� );
			// ���� �ɷ�ġ�� �����Ѵ�;
			sBasicInfo.DefaultInitialize_BasicAttackDamage( pItem );
		}

		if ( 0 <= nBasicDefenceIndex && nBasicDefenceIndex < nBasicDefenceIndex_MaxSize )
		{
			sBasicInfo.GenerateBasicDefence_Drop( emItemLevel, nBasicDefenceType, nBasicDefenceIndex );
		}
		else
		{
			// �����ɷ�ġ Index�� �������� ���� �ƴ� ��� ( �Ϲ������� -1 �ƴ϶�� ���� �Ǽ� );
			// ���� �ɷ�ġ�� �����Ѵ�;
			sBasicInfo.DefaultInitialize_BasicDefence( pItem );
		}
	}
	// ��������� �������� ��� ó���Ѵ�;
	else
	{
		if ( !( _wFixedIndex & EMREBUILD_FIXED_BASICSTATS_ATTACKDAMAGE ) )
		{
			if ( 0 <= nBasicAttackDamageIndex && nBasicAttackDamageIndex < nBasicAttackDamageIndex_MaxSize )
			{
				sBasicInfo.GenerateBasicAttackDamage_Make( emItemLevel, nBasicAttackDamageType, nBasicAttackDamageIndex );
			}
			else
			{
				// �����ɷ�ġ Index�� �������� ���� �ƴ� ��� ( �Ϲ������� -1 �ƴ϶�� ���� �Ǽ� );
				// ���� �ɷ�ġ�� �����Ѵ�;
				sBasicInfo.DefaultInitialize_BasicAttackDamage( pItem );
			}
		}

		if ( !( _wFixedIndex & EMREBUILD_FIXED_BASICSTATS_DEFENCE ) )
		{
			if ( 0 <= nBasicDefenceIndex && nBasicDefenceIndex < nBasicDefenceIndex_MaxSize )
			{
				sBasicInfo.GenerateBasicDefence_Make( emItemLevel, nBasicDefenceType, nBasicDefenceIndex );
			}
			else
			{
				// �����ɷ�ġ Index�� �������� ���� �ƴ� ��� ( �Ϲ������� -1 �ƴ϶�� ���� �Ǽ� );
				// ���� �ɷ�ġ�� �����Ѵ�;
				sBasicInfo.DefaultInitialize_BasicDefence( pItem );
			}
		}
	}

	return true;
}

// ��ũ ��ų ���� �ο�;
const bool SITEMCUSTOM::GenerateLinkSkill ( void )
{
	// ������ �����͸� �޾ƿ´�;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( GetNativeID() );
	if ( NULL == pItem )
		return false;

	// �������� ��ũ ��ų ��Ʈ Index �� ���Ѵ�;
	int nLinkSkillIndex = pItem->GetLinkSkillIndex();

	int nLinkSkillIndex_MaxSize = GLItemLinkSkillMan::Instance()->GetSizeLinkSkillSet();

	if ( 0 <= nLinkSkillIndex && nLinkSkillIndex < nLinkSkillIndex_MaxSize )
	{
		sBasicInfo.GenerateLinkSkill( nLinkSkillIndex );
	}
	else
	{
		// ��ũ ��ų ��Ʈ Index �� �������� ���� �ƴ� ��� ( �Ϲ������� -1 �ƴ϶�� ���� �Ǽ� );
		// ���� ��ũ ��ų�� �����Ѵ�;
		sBasicInfo.DefaultInitialize_LinkSkill( pItem );
	}

	return true;
}

const bool SITEMCUSTOM::GenerateAddOption ( const bool _bDropItem )
{
	// ������ �����͸� �޾ƿ´�;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( GetNativeID() );
	if ( NULL == pItem )
		return false;

	// �ʱ�ȭ�Ѵ�;
	const UINT nMax( min( SITEMCUSTOM_ADDON::ADDON_SIZE, ITEM::SSUIT::ADDON_SIZE ) );
	for ( UINT i=0; i<nMax; ++i )
	{
		sAddonInfo.m_sADDON[i] = pItem->sSuitOp.sADDON[i];

		if ( pItem->sSuitOp.sADDON[i].emTYPE == EMADD_RANDOM )
			sAddonInfo.m_sADDON[i].emTYPE = EMADD_NONE;
	}

	// ����ɼ��� �����;
	bool bReturn = false;
	if ( true == _bDropItem )
	{
		bReturn = sAddonInfo.Generate_RandomAddon(
			RandomAddon::GENERATE_GEN|RandomAddon::GENERATE_ONLYRANDOM,
			pItem );
	}
	else
	{
		bReturn = sAddonInfo.Generate_RandomAddon(
			RandomAddon::GENERATE_REBUILD|RandomAddon::GENERATE_ONLYRANDOM,
			pItem );
	}

	return bReturn;
}

// *****************************************************
// Desc: ������ ���� ������ ��ȯ
// *****************************************************
GLPADATA SITEMCUSTOM::getdamage (float fDurability, EMCHARINDEX charIdx, int nLevel) const
{
	/*const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return GLPADATA();
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	GLPADATA sDAMAGE = sSUIT.gdDamage;*/
	GLPADATA sDAMAGE = GetBasicAttackDamage();

	bool bCsApply = IsCSApplied();

	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( nidDISGUISE );
	if( NULL != pDisguise )
	{
		sDAMAGE += pDisguise->GetAttackDamage();
	}
	
	if( bCsApply )
	{
		if( pDisguise == NULL )
			sDAMAGE = GLPADATA(0, 0);
		else
			sDAMAGE = GetBasicAttackDamage();
	}

	// ���� ��ġ �ջ�
	
	const float fRATE(GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DAMAGE));
	const float fVALUE(GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DAMAGE));

	if( fDurability >= 1.0f )
	{
		sDAMAGE.wLow *= fDurability;
		sDAMAGE.wHigh *= fDurability;
	}

	// ������ ���� �ɼǰ��� ������ ��ġ�� �ȵ�. ( ��ȹ�ǵ�.. )
	// �׷��� ������ �ְ� �޴� ���� �ƴ� �� �Ѵ� �ʿ���.
	// ���������ؼ� �ڵ尡 �߸��Ȱ� ���� �̷��� ������ �߰��ϴ� �������� ������.

	if ( fRATE!=0.0f && (charIdx != GLCI_NUM_ACTOR && nLevel != -1) )
	{
		/*sDAMAGE.wLow = sSUIT.gdDamage.wLow + GLCONST_CHAR::cCONSTCLASS[charIdx].fLVLUP_AP * nLevel * (fRATE*0.01f);
		sDAMAGE.wHigh = sSUIT.gdDamage.wHigh + GLCONST_CHAR::cCONSTCLASS[charIdx].fLVLUP_AP * nLevel * (fRATE*0.01f);*/
		sDAMAGE.wLow = GetBasicAttackDamage().GetLow() + GLCONST_CHAR::cCONSTCLASS[charIdx].fLVLUP_AP * nLevel * (fRATE*0.01f);
		sDAMAGE.wHigh = GetBasicAttackDamage().GetHigh() + GLCONST_CHAR::cCONSTCLASS[charIdx].fLVLUP_AP * nLevel * (fRATE*0.01f);
	}

	if ( fVALUE != 0.0f )
		sDAMAGE.VAR_PARAM(int(fVALUE));	

	return sDAMAGE;
}

// *****************************************************
// Desc: ���(����) ������ ��ȯ
// *****************************************************
WORD SITEMCUSTOM::getmadamage ( float fDurability ) const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	WORD wMaDAMAGE(0);

	for ( int i=0; i<ITEM::SSUIT::ADDON_SIZE; ++i )
	{
		const ITEM::SADDON &sADDON = sSUIT.sADDON[i];
		if ( sADDON.emTYPE==EMADD_MA )
		{
			wMaDAMAGE += sADDON.nVALUE;
		}
	}

	// ��� ���������� ���� ��ġ�� �����Ѵ�
	float fRATE  = GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DAMAGE);
	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DAMAGE);


	if( fDurability > 1.0f )
		wMaDAMAGE *= fDurability;
		
	fVALUE += GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MAGIC);

	if ( fRATE!=0.0f || fVALUE!=0.0f )
	{
		wMaDAMAGE = WORD( (wMaDAMAGE*(100.0f+fRATE)) / 100 /**0.01f*/);
		GLOGICEX::VARIATION( wMaDAMAGE, USHRT_MAX, int(fVALUE) );
	}

	return wMaDAMAGE;
}

short SITEMCUSTOM::getdefense (EMCHARINDEX charIdx, int nLevel) const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	//short nDEFENSE = sSUIT.nDefense;
	short nDEFENSE = GetBasicDefence();
	short nCsDef   = 0;

	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( nidDISGUISE );

	// �ڽ�Ƭ �ɷ�ġ ���� ��/��
	bool bCsApply = IsCSApplied();

	if( bCsApply )
	{
		COSTUMEUSERVALUE::sCostumeUser::sInvest rTemp;

		if( costumeUser.getTypeInfo( (BYTE)E_DEFENCE, rTemp ) ) {
			const COSTUMESTATSDATA::sPsData* pData = COSSDATA.GetStatInfo(sSUIT.emSuit, E_DEFENCE);

			if( pData ) {
				nCsDef = (short)(rTemp.wInvestPt*pData->fInc);
			}			
		}

		if( pDisguise == NULL ) {
			nDEFENSE = nCsDef;
			nCsDef   = 0;
		}
	}
	
	if( NULL != pDisguise )	{
		if( !bCsApply ) {
			nDEFENSE += pDisguise->GetDefence();
		}
		else {
			nDEFENSE += nCsDef;
		}
	}
	else {
		nDEFENSE += nCsDef;
	}

	float fRATE  = GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DEFENCE);
	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DEFENCE);
	
	short nprevDEFENSE(nDEFENSE);

	if( (pDisguise == NULL) && (bCsApply == true) )
	{
		fRATE = 0.f; fVALUE = 0.f;
	}

	if ( fRATE!=0.0f || fVALUE!=0.0f )
	{
		// nDEFENSE = short(nDEFENSE*(100.0f+fRATE)*0.01f) + short(fVALUE);
		nDEFENSE = nDEFENSE + fVALUE + (GLCONST_CHAR::cCONSTCLASS[charIdx].fLVLUP_DP * nLevel * (fRATE*0.01f));
		
		//���� �ɼ�
		//�� fRATE�� 0 ���� ũ���� ��� �������� 1 ������ ��� �ּҰ��� 1�� ����
		if( (nprevDEFENSE == nDEFENSE) &&  (fRATE != 0.0f) ) nDEFENSE += 1;
	}
	
	return nDEFENSE;
}

// ���� �� Ÿ��üũ���ϰ� �۾Ƴ���?;
float SITEMCUSTOM::GETMOVESPEED() const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	// �Ʒ� �� �̻��ѵ�? Ÿ��üũ ���ϰ� �۾Ƴ־�ǳ�?;
	//float fSpeed = sSUIT.sVOLUME.fVolume;
	float fSpeed = 0.f;

	// ����ɼǵ� ���� �����ɼ��̱� ������ �ٷ� üũ�ϸ� �ȴ�;
	//fSpeed = sAddonInfo.GetValue_OptType<float>(EMADDEX_INC_MOVE_SPEED);
	/// ����ɼ��� ����ȭ�� �ȵǴ°� ����, ���� �� �����ؾ��Ѵ�
	sSUIT.GetHaveAddonValue_float(EMADDEX_INC_MOVE_SPEED, fSpeed);

	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( nidDISGUISE );
	if( NULL != pDisguise )
	{
		//��ȭ�� ȿ���� ó�� ��ȭ�� �ɼ��� GETMOVESPEEDR() �Լ����� ó��
		const ITEM::SSUIT& sDisguiseSuit = pDisguise->sSuitOp;
		float fDisguiseSpeed = 0.0f;

		// �ڽ����� ��쿡�� ���� �ɼ����μ� ����� �� �����Ƿ� ���� �ɼ��� ���� ��츸 �ҷ��´�;
		if ( sDisguiseSuit.GetHaveAddonValue_float(EMADDEX_INC_MOVE_SPEED, fDisguiseSpeed) == true )
			fSpeed += fDisguiseSpeed;
	}

	// ���� ��ġ �ջ�
	float fRATE = GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE);
	if ( fRATE!=0 )
	{
		fSpeed = ( fSpeed * ( 100.0f + fRATE )  * 0.01f );
	}

	return fSpeed;
}

float SITEMCUSTOM::GETMOVESPEED_EXCEPTDISGUISE() const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fSpeed = 0.0f;

	// ����ɼǵ� ���� �����ɼ��̱� ������ �ٷ� üũ�ϸ� �ȴ�;
	//fSpeed = sAddonInfo.GetValue_OptType<float>(EMADDEX_INC_MOVE_SPEED);
	/// ����ɼ��� ����ȭ�� �ȵǴ°� ����, ���� �� �����ؾ��Ѵ�
	sSUIT.GetHaveAddonValue_float(EMADDEX_INC_MOVE_SPEED, fSpeed);

	// ���� ��ġ �ջ�
	float fRATE = GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE);
	if ( fRATE!=0 )
	{
		fSpeed = ( fSpeed * ( 100.0f + fRATE )  * 0.01f );
	}

	return fSpeed;
}

float SITEMCUSTOM::GETMOVESPEEDR() const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fSpeed = 0.f;

	// ����ɼǵ� ���� �����ɼ��̱� ������ �ٷ� üũ�ϸ� �ȴ�;
	//fSpeed = sAddonInfo.GetValue_OptType<float>(EMADDEX_INCR_MOVE_SPEED);
	/// ����ɼ��� ����ȭ�� �ȵǴ°� ����, ���� �� �����ؾ��Ѵ�
	sSUIT.GetHaveAddonValue_float(EMADDEX_INCR_MOVE_SPEED, fSpeed);

	fSpeed += GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE);
// 	if ( sSUIT.sVARIATE.emTYPE == RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE )
// 	{
// 		fSpeed = sSUIT.sVARIATE.fVariate;
// 	}

/*
	// ���� ��ġ �ջ�
	float fRATE = GETOptVALUE(EMR_OPT_MOVE_SPEED);
	if ( fRATE!=0 )
	{
		fSpeed = ( fSpeed * ( 100.0f + fRATE )  * 0.01f );
	}
*/
	return fSpeed;
}

GLDWDATA SITEMCUSTOM::GETDAMAGE (EMCHARINDEX charIdx, int nLevel) const
{
	GLDWDATA sDAMAGE(getdamage( 0.0f, charIdx, nLevel));

	sDAMAGE.nNow += int(GETGRADE_DAMAGE());
	sDAMAGE.nMax += int(GETGRADE_DAMAGE());

	return sDAMAGE;
}

WORD SITEMCUSTOM::GETMaDAMAGE () const
{
	WORD wMaDAMAGE = getmadamage();

	wMaDAMAGE += GETGRADE_DAMAGE();

	return wMaDAMAGE;
}

short SITEMCUSTOM::GETDEFENSE (EMCHARINDEX charIdx, int nLevel) const
{
	short nDEFENSE = getdefense(charIdx, nLevel);

	nDEFENSE += GETGRADE_DEFENSE();

	return nDEFENSE;
}

short SITEMCUSTOM::GETHITRATE () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nHITRATE = sSUIT.nHitRate;

	bool bCsApply = IsCSApplied();

	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( nidDISGUISE );
	if( NULL != pDisguise )
	{
		const ITEM::SSUIT& sDisguiseSuit = pDisguise->sSuitOp;
		nHITRATE += sDisguiseSuit.nHitRate;
	}

	if( bCsApply )
	{
		if( NULL == pDisguise )	{
			nHITRATE = 0;
		}
		else {
			nHITRATE = sSUIT.nHitRate;
		}
	}

	float fRATE  = GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_HIT);
	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_HIT);

	if ( fRATE!=0.0f || fVALUE!=0.0f )
	{
		nHITRATE = short(nHITRATE+fRATE) + short(fVALUE);
	}

	return nHITRATE;
}

short SITEMCUSTOM::GETAVOIDRATE () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	bool bCsApply = IsCSApplied();

	short nAVOIDRATE = sSUIT.nAvoidRate;

	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( nidDISGUISE );
	if( NULL != pDisguise )
	{
		const ITEM::SSUIT& sDisguiseSuit = pDisguise->sSuitOp;
		nAVOIDRATE += sDisguiseSuit.nAvoidRate;
	}

	if( bCsApply )
	{
		if( NULL == pDisguise )	{
			nAVOIDRATE = 0;
		}
		else {
			nAVOIDRATE = sSUIT.nAvoidRate;
		}
	}

	float fRATE  = GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_AVOID);
	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_AVOID);

	if ( fRATE!=0.0f || fVALUE!=0.0f )
	{
		nAVOIDRATE = short(nAVOIDRATE+fRATE) + short(fVALUE);
	}

	return nAVOIDRATE;
}

WORD SITEMCUSTOM::GETATTRANGE () const
{
	/*const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	WORD wATTRANGE = sSUIT.wAttRange;

	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( nidDISGUISE );
	if( NULL != pDisguise )
	{
		const ITEM::SSUIT& sDisguiseSuit = pDisguise->sSuitOp;
		wATTRANGE += sDisguiseSuit.wAttRange;
	}

	float fRATE = GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RANGE_ATTACK);
	if ( fRATE!=0.0f )
	{
		wATTRANGE = WORD(wATTRANGE+fRATE);
	}

	return wATTRANGE;*/
	return 0;
}

WORD SITEMCUSTOM::GETREQ_SP () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	WORD wREQSP = sSUIT.wReqSP;

	const SITEM* pDisguise = GLogicData::GetInstance().GetItem( nidDISGUISE );
	if( NULL != pDisguise )
	{
		const ITEM::SSUIT& sDisguiseSuit = pDisguise->sSuitOp;
		wREQSP += sDisguiseSuit.wReqSP;
	}

	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_CONSUME_SP);
	if ( fVALUE!=0.0f )
	{
		if ( fVALUE+wREQSP>0.0f )	wREQSP = WORD(wREQSP+fVALUE);
		else						wREQSP = 0;
	}

	return wREQSP;
}

int SITEMCUSTOM::GETADDHP () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	int nHP(0);

	for ( DWORD addon=0; addon<ITEM::SSUIT::ADDON_SIZE; ++addon )
	{
		if ( sSUIT.sADDON[addon].emTYPE==EMADD_HP )
		{
			nHP += sSUIT.sADDON[addon].nVALUE;
		}
	}

	nHP += (int) GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_HP);

	return nHP;
}

int SITEMCUSTOM::GETADDMP () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	int nMP(0);

	for ( DWORD addon=0; addon<ITEM::SSUIT::ADDON_SIZE; ++addon )
	{
		if ( sSUIT.sADDON[addon].emTYPE==EMADD_MP )
		{
			nMP += sSUIT.sADDON[addon].nVALUE;
		}
	}

	nMP += (int) GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MP);

	return nMP;
}

int SITEMCUSTOM::GETADDSP () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	int nSP(0);

	for ( DWORD addon=0; addon<ITEM::SSUIT::ADDON_SIZE; ++addon )
	{
		if ( sSUIT.sADDON[addon].emTYPE==EMADD_SP )
		{
			nSP += sSUIT.sADDON[addon].nVALUE;
		}
	}

	nSP += (int) GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_SP);

	return nSP;
}

int SITEMCUSTOM::GETADDMA () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	INT wMaDAMAGE(0);

	for ( int i=0; i<ITEM::SSUIT::ADDON_SIZE; ++i )
	{
		const ITEM::SADDON &sADDON = sSUIT.sADDON[i];
		if ( sADDON.emTYPE==EMADD_MA )
		{
			wMaDAMAGE += sADDON.nVALUE;
		}
	}

	float fRATE  = GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_DAMAGE);
	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_DAMAGE);

	fVALUE += GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_MAGIC);

	if ( fRATE!=0 || fVALUE!=0)
	{
		wMaDAMAGE = INT(wMaDAMAGE*(100.0f+fRATE)*0.01f) + INT(fVALUE);
	}

	return wMaDAMAGE;
}

float SITEMCUSTOM::GETINCHP () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fINC(0);

	// ����ɼǵ� ���� �����ɼ��̱� ������ �ٷ� üũ�ϸ� �ȴ�;
	//fINC = sAddonInfo.GetValue_OptType<float>(EMADDEX_INCR_HP);
	/// ����ɼ��� ����ȭ�� �ȵǴ°� ����, ���� �� �����ؾ��Ѵ�
	sSUIT.GetHaveAddonValue_float(EMADDEX_INCR_HP, fINC);

	fINC += GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP);

	return fINC;
}

float SITEMCUSTOM::GETINCMP () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fINC(0);

	// ����ɼǵ� ���� �����ɼ��̱� ������ �ٷ� üũ�ϸ� �ȴ�;
	//fINC = sAddonInfo.GetValue_OptType<float>(EMADDEX_INCR_MP);
	/// ����ɼ��� ����ȭ�� �ȵǴ°� ����, ���� �� �����ؾ��Ѵ�
	sSUIT.GetHaveAddonValue_float(EMADDEX_INCR_MP, fINC);

	fINC += GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_MP);

	return fINC;
}

float SITEMCUSTOM::GETINCSP () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fINC(0);

	// ����ɼǵ� ���� �����ɼ��̱� ������ �ٷ� üũ�ϸ� �ȴ�;
	//fINC = sAddonInfo.GetValue_OptType<float>(EMADDEX_INCR_SP);
	/// ����ɼ��� ����ȭ�� �ȵǴ°� ����, ���� �� �����ؾ��Ѵ�
	sSUIT.GetHaveAddonValue_float(EMADDEX_INCR_SP, fINC);

	fINC += GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_SP);

	return fINC;
}

float SITEMCUSTOM::GETINCAP () const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	float fINC(0);

	// ����ɼǵ� ���� �����ɼ��̱� ������ �ٷ� üũ�ϸ� �ȴ�;
	//fINC = sAddonInfo.GetValue_OptType<float>(EMADDEX_INCR_AP);
	/// ����ɼ��� ����ȭ�� �ȵǴ°� ����, ���� �� �����ؾ��Ѵ�
	sSUIT.GetHaveAddonValue_float(EMADDEX_INCR_AP, fINC);

	fINC += GETOptVALUE(RandomOption::RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP);

	return fINC;
}

WORD SITEMCUSTOM::GETRESIST_FIRE ( float fDurability ) const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nFire;

	bool bCsApply = IsCSApplied();

	if( bCsApply )
	{
		short nCsResist = 0;
		COSTUMEUSERVALUE::sCostumeUser::sInvest rTemp;

		if( costumeUser.getTypeInfo( (BYTE)E_RESIST, rTemp ) ) {
			const COSTUMESTATSDATA::sPsData* pData = COSSDATA.GetStatInfo(sSUIT.emSuit, E_RESIST);

			if( pData ) {
				nCsResist = (short)(rTemp.wInvestPt*pData->fInc);
			}			
		}

		if( nidDISGUISE == NATIVEID_NULL() )
			nRESIST = nCsResist;
		else
			nRESIST += nCsResist;
	}

//	if( fDurability > 1.0f )
//		nRESIST * fDurability;

	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_FIRE();
}

WORD SITEMCUSTOM::GETRESIST_ICE ( float fDurability ) const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nIce;

	bool bCsApply = IsCSApplied();

	if( bCsApply )
	{
		short nCsResist = 0;
		COSTUMEUSERVALUE::sCostumeUser::sInvest rTemp;

		if( costumeUser.getTypeInfo( (BYTE)E_RESIST, rTemp ) ) {
			const COSTUMESTATSDATA::sPsData* pData = COSSDATA.GetStatInfo(sSUIT.emSuit, E_RESIST);

			if( pData ) {
				nCsResist = (short)(rTemp.wInvestPt*pData->fInc);
			}			
		}

		if( nidDISGUISE == NATIVEID_NULL() )
			nRESIST = nCsResist;
		else
			nRESIST += nCsResist;
	}

	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_ICE();
}

WORD SITEMCUSTOM::GETRESIST_ELEC ( float fDurability ) const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nElectric;

	bool bCsApply = IsCSApplied();

	if( bCsApply )
	{
		short nCsResist = 0;
		COSTUMEUSERVALUE::sCostumeUser::sInvest rTemp;

		if( costumeUser.getTypeInfo( (BYTE)E_RESIST, rTemp ) ) {
			const COSTUMESTATSDATA::sPsData* pData = COSSDATA.GetStatInfo(sSUIT.emSuit, E_RESIST);

			if( pData ) {
				nCsResist = (short)(rTemp.wInvestPt*pData->fInc);
			}			
		}

		if( nidDISGUISE == NATIVEID_NULL() )
			nRESIST = nCsResist;
		else
			nRESIST += nCsResist;
	}

	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_ELEC();
}

WORD SITEMCUSTOM::GETRESIST_POISON ( float fDurability ) const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nPoison;

	bool bCsApply = IsCSApplied();

	if( bCsApply )
	{
		short nCsResist = 0;
		COSTUMEUSERVALUE::sCostumeUser::sInvest rTemp;

		if( costumeUser.getTypeInfo( (BYTE)E_RESIST, rTemp ) ) {
			const COSTUMESTATSDATA::sPsData* pData = COSSDATA.GetStatInfo(sSUIT.emSuit, E_RESIST);

			if( pData ) {
				nCsResist = (short)(rTemp.wInvestPt*pData->fInc);
			}			
		}

		if( nidDISGUISE == NATIVEID_NULL() )
			nRESIST = nCsResist;
		else
			nRESIST += nCsResist;
	}

	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_POISON();
}

WORD SITEMCUSTOM::GETRESIST_SPIRIT ( float fDurability ) const
{
	const SITEM *pITEM = GLogicData::GetInstance().GetItem(GetNativeID());
	if ( !pITEM )	return 0;
	const ITEM::SSUIT &sSUIT = pITEM->sSuitOp;

	short nRESIST = sSUIT.sResist.nSpirit;

	bool bCsApply = IsCSApplied();

	if( bCsApply )
	{
		short nCsResist = 0;
		COSTUMEUSERVALUE::sCostumeUser::sInvest rTemp;

		if( costumeUser.getTypeInfo( (BYTE)E_RESIST, rTemp ) ) {
			const COSTUMESTATSDATA::sPsData* pData = COSSDATA.GetStatInfo(sSUIT.emSuit, E_RESIST);

			if( pData ) {
				nCsResist = (short)(rTemp.wInvestPt*pData->fInc);
			}			
		}

		if( nidDISGUISE == NATIVEID_NULL() )
			nRESIST = nCsResist;
		else
			nRESIST += nCsResist;
	}

	float fVALUE = GETOptVALUE(RandomOption::RANDOM_OPTION_VALUE_RESIST);
	if ( fVALUE!=0.0f )
	{
		nRESIST = int(nRESIST+fVALUE);
	}

	return nRESIST + GETGRADE_RESIST_SPIRIT();
}


BOOL SETPUTONITEMS_BYBUF ( SITEMCUSTOM *pPutOnItems, se::ByteStream &ByteStream )
{
    if ( ByteStream.IsEmpty() )	return TRUE;

    //	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + PUTONITEMS_ARRAY[SIZE]
    //
    DWORD dwVersion, dwSize, dwNum;

    ByteStream >> dwVersion;
    ByteStream >> dwSize;
    ByteStream >> dwNum;

    SITEMCUSTOM sItemCustom;
    for ( DWORD i=0; i<dwNum; i++ )
    {
        if ( i>=SLOT_TSIZE )	break;

        //	Note : ���� �������� ���� ������ ���� �������� ó�� �� �κ�.
        //
        if ( dwVersion==0x0100 )
        {
            SITEMCUSTOM_100 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0101 )
        {
            SITEMCUSTOM_101 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0102 )
        {
            SITEMCUSTOM_102 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0103 )
        {
            SITEMCUSTOM_103 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0104 )
        {
            SITEMCUSTOM_104 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0105 )
        {
            SITEMCUSTOM_105 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0106 )
        {
            SITEMCUSTOM_106 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0107 )
        {
            SITEMCUSTOM_107 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0108 )
        {
            SITEMCUSTOM_108 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0109 )
        {
            SITEMCUSTOM_109 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );
        }
        else if ( dwVersion==0x0110)
        {
            SITEMCUSTOM_110 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );

        }
        else if ( dwVersion==0x0111)
        {
            SITEMCUSTOM_111 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );

        }
        else if ( dwVersion==0x0112)
        {
            SITEMCUSTOM_112 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );

        }
        else if ( dwVersion==0x0113)
        {
            SITEMCUSTOM_113 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );

        }
		else if ( dwVersion==0x0114)
		{
			SITEMCUSTOM_114 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
        else if ( dwVersion==0x0115)
        {
            SITEMCUSTOM_115 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );

        }
        else if ( dwVersion==0x0116)
        {
            SITEMCUSTOM_116 sItemCustomOld;
            GASSERT(dwSize==sizeof(sItemCustomOld));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

            pPutOnItems[i].Assign ( sItemCustomOld );

        }
		else if ( dwVersion==0x0117)
		{
			SITEMCUSTOM_117 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion==0x0118)
		{
			SITEMCUSTOM_118 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion==0x0119)
		{
			SITEMCUSTOM_119 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion==0x0120)
		{
			SITEMCUSTOM_120 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion==0x0121)
		{
			SITEMCUSTOM_121 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion==0x0122)
		{
			SITEMCUSTOM_122 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion==0x0123 )
		{
			SITEMCUSTOM_123 sItemCustomOld;
			GASSERT(dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer ( (LPBYTE)&sItemCustomOld, sizeof(sItemCustomOld) );

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
        else if ( dwVersion==SITEMCUSTOM::VERSION )
        {
            GASSERT(dwSize==sizeof(SITEMCUSTOM));
            ByteStream.ReadBuffer ( (LPBYTE)&sItemCustom, sizeof(SITEMCUSTOM) );

            pPutOnItems[i] = sItemCustom;
        }
        else
        {
        }
    }

    return TRUE;
}

BOOL SETPUTONITEMS_BYBUF_FOR_MIGRATION( SITEMCUSTOM *pPutOnItems, se::ByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
		return TRUE;

	// DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + PUTONITEMS_ARRAY[SIZE]
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	SITEMCUSTOM sItemCustom;
	for ( DWORD i = 0; i < dwNum; i++ )
	{
		if ( i >= SLOT_TSIZE )
			break;

		if ( dwVersion == 0x0100 )
		{
			SITEMCUSTOM_100 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0101 )
		{
			SITEMCUSTOM_101 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0102 )
		{
			SITEMCUSTOM_102 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0103 )
		{
			SITEMCUSTOM_103 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0104 )
		{
			SITEMCUSTOM_104 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0105 )
		{
			SITEMCUSTOM_105 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0106 )
		{
			SITEMCUSTOM_106 sItemCustomOld;
			GASSERT( dwSize==sizeof(sItemCustomOld));
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0107 )
		{
			SITEMCUSTOM_107 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0108 )
		{
			SITEMCUSTOM_108 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0109 )
		{
			SITEMCUSTOM_109 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );
		}
		else if ( dwVersion == 0x0110)
		{
			SITEMCUSTOM_110 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0111)
		{
			SITEMCUSTOM_111 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0112)
		{
			SITEMCUSTOM_112 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0113)
		{
			SITEMCUSTOM_113 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0114)
		{
			SITEMCUSTOM_114 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0115)
		{
			SITEMCUSTOM_115 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0116)
		{
			SITEMCUSTOM_116 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0117)
		{
			SITEMCUSTOM_117 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0118)
		{
			SITEMCUSTOM_118 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0119)
		{
			SITEMCUSTOM_119 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0120)
		{
			SITEMCUSTOM_120 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0121)
		{
			SITEMCUSTOM_121 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0122)
		{
			SITEMCUSTOM_122 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == 0x0123)
		{
			SITEMCUSTOM_123 sItemCustomOld;
			GASSERT( dwSize == sizeof( sItemCustomOld ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustomOld, sizeof( sItemCustomOld ) );

			sItemCustomOld.sNativeID.wMainID += 3000;
			if ( NATIVEID_NULL() != sItemCustomOld.nidDISGUISE )
			{
				sItemCustomOld.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i].Assign ( sItemCustomOld );

		}
		else if ( dwVersion == SITEMCUSTOM::VERSION )
		{
			// ������ ������ �ö󰡵� xxx_migration(MIGRATION) �Լ����� ������ �ʿ� ����
			GASSERT( dwSize == sizeof( SITEMCUSTOM ) );
			ByteStream.ReadBuffer( ( LPBYTE )&sItemCustom, sizeof( SITEMCUSTOM ) );

			SNATIVEID ItemID = sItemCustom.GetRealNativeID();
			ItemID.wMainID += 3000;
			sItemCustom.SetNativeID( ItemID );
			if ( NATIVEID_NULL() != sItemCustom.nidDISGUISE )
			{
				sItemCustom.nidDISGUISE.wMainID += 3000;
			}

			pPutOnItems[i] = sItemCustom;
		}
		else
		{
		}
	}

	return TRUE;
}

// SITEM �� �⺻ ������ �����Ѵ�
void SITEMCUSTOM::SetDefaultValue ( const SITEM* pItem )
{
    sNativeID = pItem->sBasicOp.sNativeID;
    wTurnNum  = pItem->GETAPPLYNUM();
    cDAMAGE   = (BYTE)pItem->sBasicOp.wGradeAttack;
    cDEFENSE  = (BYTE)pItem->sBasicOp.wGradeDefense;
    wDurability = 0;
	if ( pItem->sBasicOp.Type() == ITEM_LOCK_BOX )
		bVietnamGainItem = true;

	{
		// Default Value �������� �������� �ʴ´�;
		// ������ �ʿ��� ��� �������� ������ �� ������ �ٽ� ���õȴ�;
		sBasicInfo.DefaultInitialize( pItem );
		sAddonInfo.DefaultInitialize( pItem );
	}
}

void SITEMCUSTOM::SetGM_GEN_RANDOMGENNUM( const bool bEnableTrade, const sc::MGUID& id )
{
	cGenType = bEnableTrade == true ? EMGEN_GMEDIT2 : EMGEN_GMEDIT;
	//cFieldID = 0xff;
	//sc::RandomInt randomInt;
	//lnGenNum = randomInt.Int64();
	guid = id;
}

bool SITEMCUSTOM::CheckItemFlag( EMITEM_FLAG emItemFlag, EMCHECKFLAG_OPTION emFlag /*=CHECKFLAGOP_ALL*/ ) const
{
	// 	if (GLUseFeatures::GetInstance().IsUsingOPItemCheck())
	// 	{
	// 		if ( emItemFlag & (TRADE_EXCHANGE|TRADE_THROW|ITEM_DISGUISE|ITEM_BOX_WRAPPABLE|TRADE_STORAGE) )
	// 		{
	// 			if ( IsOpItem() )
	// 				return false;
	// 		}
	// 	}

	bool bBase      = false;
	bool bCostume   = false;

	if ( emFlag & CHECKFLAGOP_NORMAL )
	{
		SNATIVEID sTempNativeID = GetNativeID();

		// �������� ��� ������ �װ��� ��쿡�� �������� ������ ���󰡵��� �Ѵ�;
		// ��ȹ���� ��û���� �� �����̸� ��ȹ���� �𸦰� ���� ����صд�;
		// �̹� TRADE_STORAGE �� ���� �� �κ��� Ȯ���� �ȵ� ��ʰ� �ִ�, ��������;
		if ( emItemFlag & ( TRADE_ALL|ITEM_BOX_WRAPPABLE|ITEM_SPLIT|LIMIT_LOCKER ) )
			sTempNativeID = GetRealNativeID();

		const SITEM* pItem = GLogicData::GetInstance().GetItem ( sTempNativeID );
		if ( pItem )
			bBase = pItem->sBasicOp.dwFlags & emItemFlag;

		if ( emFlag != CHECKFLAGOP_ALL )
			return bBase;
	}

	if( emFlag & CHECKFLAGOP_COSTUME )
	{
		const SITEM* pItem = GLogicData::GetInstance().GetItem ( nidDISGUISE );
		if ( pItem )
			bCostume = pItem->sBasicOp.dwFlags & emItemFlag;
		else
			bCostume = true;

		if ( emFlag != CHECKFLAGOP_ALL )
			return bCostume;
	}

	return bBase && bCostume;
}


// ������ �Ǹ� ����
bool SITEMCUSTOM::IsSale() const
{
	// http://172.16.2.87/redmine/issues/4281 ������ ������ �Ǹ��� ���� �ڽ�Ƭ���� �Բ� �˻��ϵ��� �� [8/31/2016 khkim]
    return CheckItemFlag(TRADE_SALE, /*CHECKFLAGOP_NORMAL*/CHECKFLAGOP_ALL);
}

//	���ΰ� ��ȯ ����.
bool SITEMCUSTOM::IsExchange() const
{
    return CheckItemFlag(TRADE_EXCHANGE, CHECKFLAGOP_ALL);
}

//	â�� ������ �ִ���
bool SITEMCUSTOM::IsNotStorage() const
{
	// TRADE_STORAGE �� â�� ���� �� �ִ��� ���ΰ� �ƴ϶�;
	// â�� ���� �� ������ ���θ� �Ǵ��Ѵ�;
	// �׷��Ƿ� ���� �ݴ�� �������� �ʾƵ� �ǹ̰� ��Ȯ�ϴ�;
	// http://172.16.2.87/redmine/issues/4281 ������Ŀ�� �������� ���� �� �ڽ�Ƭ���� �Բ� �˻��ϵ��� �� [8/31/2016 khkim]
	// �ŷ�, ������ʹ� �ٸ��� ���� ������, �ڽ�Ƭ �Ѵ� �˻��ؼ� OR ���� �����ؾ� �Ѵ�.
	bool bBase = CheckItemFlag( LIMIT_LOCKER, CHECKFLAGOP_NORMAL );
	bool bCostume = false;
	if ( nidDISGUISE != NATIVEID_NULL() )
	{
		bCostume = CheckItemFlag( LIMIT_LOCKER, CHECKFLAGOP_COSTUME );
	}
	return bBase || bCostume;
}


//	�ʵ忡 ������ ����.
bool SITEMCUSTOM::IsThrow() const
{
    return CheckItemFlag(TRADE_THROW, CHECKFLAGOP_ALL);
}

// ���� �̺�Ʈ �������� �Ѱ� �̻� �� �ݰ� �ϱ� ����.
bool SITEMCUSTOM::IsEvent() const
{
    return CheckItemFlag(TRADE_EVENT_SGL, CHECKFLAGOP_NORMAL);
}

//	�ļ� ����
bool SITEMCUSTOM::IsGarbage() const
{
    return CheckItemFlag(TRADE_GARBAGE, CHECKFLAGOP_NORMAL);
}

//	�ڽ���.
bool SITEMCUSTOM::IsDisguise() const
{
    return CheckItemFlag(ITEM_DISGUISE, CHECKFLAGOP_NORMAL);
}

//	��ȿ��. - // ���Ѻ� �������� ��� ��ȿ �Ⱓ.
bool SITEMCUSTOM::IsTimeLimit( EMCHECKFLAG_OPTION emFlag /*= CHECKFLAGOP_ALL*/ ) const
{
	return CheckItemFlag(ITEM_TIMELMT, emFlag);
}

//	��� ä��.
bool SITEMCUSTOM::IsChannel_All() const
{
    return CheckItemFlag(ITEM_CHANNEL_ALL, CHECKFLAGOP_NORMAL);
}


//	��� ����
bool SITEMCUSTOM::IsRestrict() const
{
    return CheckItemFlag(ITEM_RESTRICT, CHECKFLAGOP_NORMAL);
}

// ������ ���� ���ɿ���;
bool SITEMCUSTOM::IsBoxWrappable() const
{
	// http://172.16.2.87/redmine/issues/4281 ������ ���� �ڽ�Ƭ���� �Բ� �˻��ϵ��� �� [8/31/2016 khkim]
    return CheckItemFlag(ITEM_BOX_WRAPPABLE, /*CHECKFLAGOP_NORMAL*/CHECKFLAGOP_ALL);
}

bool SITEMCUSTOM::IsDecompose() const
{
	bool bEnable = CheckItemFlag(ITEM_SPLIT, CHECKFLAGOP_NORMAL);
	if ( true == bEnable )
	{
		// �ڽ����� �پ��ִٸ� ���ذ� �Ұ����ؾ� �Ѵ�;
		if ( nidDISGUISE != NATIVEID_NULL() )
		{
			bEnable = false;
		}
	}

	return bEnable;
}

bool SITEMCUSTOM::IsBoxWrapped() const     // ����Ǿ��ִ°�;
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem ( sNativeID );
    if ( (pItem) && (pItem->sBasicOp.emItemType == ITEM_WRAPPED_BOX) )
        return true;

    return false;
}

bool SITEMCUSTOM::IsCSApplied() const
{
	return costumeUser.IsCSApplied();
}

void SITEMCUSTOM::ResetInvest()
{
	costumeUser.ResetInvest();
}

void SITEMCUSTOM::TempEnchantRecovery()
{
	//const SITEM* pItem = GLogicData::GetInstance().GetItem ( GetNativeID() );

	//if ( pItem )
	//{
	//	if( pItem->sSuitOp.gdDamage == GLPADATA(0,0) )
	//	{
	//		cDEFENSE = sTLGrind.cGradeValue;
	//	}
	//	else
	//	{
	//		cDAMAGE = sTLGrind.cGradeValue;
	//	}
	//
	//	sTLGrind.resetInfo();
	//}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( GetNativeID() );
	if ( NULL == pItem )
	{
		// �̰��� ���� �� ����;
		sc::writeLogError( "[ Item Log ] [ Failed TempEnchantRecovery. ]" );
		return;
	}

	// 1. ���� ����� �ʱ�ȭ ���ش�;
	cDAMAGE = 0;
	cDEFENSE = 0;

	// 2. Ÿ�Կ� �´� �����ϰ� �ִ� ���� ������� �ǵ�����;
	if ( EMREMODEL_TYPE_DAMAGE == pItem->sSuitOp.wReModelNum )
	{
		cDAMAGE = sTLGrind.cGradeValue;
	}
	else if ( EMREMODEL_TYPE_DEFENCEANDRESIST == pItem->sSuitOp.wReModelNum )
	{
		cDEFENSE = sTLGrind.cGradeValue;
	}

	/*if( GetBasicAttackDamage() == GLPADATA(0,0) )
	{
		cDEFENSE = sTLGrind.cGradeValue;
	}
	else
	{
		cDAMAGE = sTLGrind.cGradeValue;
	}*/

	// 3. �Ͻð�ȭ�� �ʱ�ȭ�Ѵ�;
	sTLGrind.resetInfo();
}

bool SITEMCUSTOM::IsTempEnchant() const
{
	if( sTLGrind.cGradeValue > 0 || sTLGrind.tFireTime != 0 )
		return true;

	return false;
}

bool SITEMCUSTOM::IsOpItem(void) const
{
	for ( INT i(0); i < RandomOpt::NSIZE; ++i )
	{
		const int nType(getOptType(i));
		if ( nType == RandomOption::RANDOM_OPTION_NULL )
			continue;

		const DWORD nCountFix(getOptCount(i));
		float fValue = getOptValue(i);// * COMMENT::ITEM_RANDOM_OPT_SCALE[nType];
		// const bool bPerRandOpt(IsPerRandOpt(nType));

		const SITEM* const pItemData = GLogicData::GetInstance().GetItem(GetNativeID());
		if( (RandomOption::RANDOM_OPTION_NULL < nType) && (nType < RandomOption::RANDOM_OPTION_NSIZE) )
		{
			const RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
			const RandomOption::Entity* const _pEntity(_managerRandomOption.find(pItemData->sRandomOpt.szNAME));
			if ( _pEntity != NULL)
			{				
				RandomOption::Information _infoRemodel, _infoDrop;
				const RandomOption::RANDOM_OPTION _typeRandomOption = RandomOption::RANDOM_OPTION(nType);

				bool bExistInfo = _pEntity->getInformation(RandomOption::GENERATE_REMODEL, _typeRandomOption, _infoRemodel);
				bExistInfo |= _pEntity->getInformation(RandomOption::GENERATE_DROP, _typeRandomOption, _infoDrop);
//				if ( _pEntity->getInformation(RandomOption::GENERATE_REMODEL, _typeRandomOption, _infoRemodel) ||
//					_pEntity->getInformation(RandomOption::GENERATE_DROP, _typeRandomOption, _infoDrop) )
				if ( bExistInfo )
				{
					RandomOpt sRandomOptionValue;
					if ( IsPerRandOpt(nType) )
					{
						float fHighValue_Compare = ( _infoRemodel.valueFloatHigh < _infoDrop.valueFloatHigh ) ? _infoDrop.valueFloatHigh : _infoRemodel.valueFloatHigh;
						float fLowValue_Compare = ( _infoRemodel.valueFloatLow < _infoDrop.valueFloatLow ) ? _infoRemodel.valueFloatLow : _infoDrop.valueFloatLow;

						// Convert;
						sRandomOptionValue.setOption( 0, static_cast< RandomOption::RANDOM_OPTION >( nType ), fHighValue_Compare );
						fHighValue_Compare = GETOptVALUE( static_cast< RandomOption::RANDOM_OPTION >( nType ), sRandomOptionValue.getValue( 0 ) );
						sRandomOptionValue.remove( 0 );

						sRandomOptionValue.setOption( 0, static_cast< RandomOption::RANDOM_OPTION >( nType ), fLowValue_Compare );
						fLowValue_Compare = GETOptVALUE( static_cast< RandomOption::RANDOM_OPTION >( nType ), sRandomOptionValue.getValue( 0 ) );
						sRandomOptionValue.remove( 0 );

						// Check;
						if ( fHighValue_Compare < fValue )
						{
							sc::writeLogInfo( sc::string::format(
								"[ RandomOption Log ] [ A type %1% is over power. ( %2% ( High Value ) < %3% ( Value ) ) ]",
								nType,
								fHighValue_Compare,
								fValue ) );

							return true;
						}
						else if ( fValue < fLowValue_Compare )
						{
							sc::writeLogInfo( sc::string::format(
								"[ RandomOption Log ] [ A type %1% is over power. ( %2% ( Value ) < %3% ( Low Value ) ) ]",
								nType,
								fValue,
								fLowValue_Compare ) );

							return true;
						}
					}
					else
					{
						int nHighValue_Compare = ( _infoRemodel.valueIntegerHigh < _infoDrop.valueIntegerHigh ) ? _infoDrop.valueIntegerHigh : _infoRemodel.valueIntegerHigh;
						int nLowValue_Compare = ( _infoRemodel.valueIntegerLow < _infoDrop.valueIntegerLow ) ? _infoRemodel.valueIntegerLow : _infoDrop.valueIntegerLow;

						// Convert;
						sRandomOptionValue.setOption( 0, static_cast< RandomOption::RANDOM_OPTION >( nType ), static_cast< float >( nHighValue_Compare ) );
						nHighValue_Compare = static_cast< int >( GETOptVALUE( static_cast< RandomOption::RANDOM_OPTION >( nType ), sRandomOptionValue.getValue( 0 ) ) );
						sRandomOptionValue.remove( 0 );

						sRandomOptionValue.setOption( 0, static_cast< RandomOption::RANDOM_OPTION >( nType ), static_cast< float >( nLowValue_Compare ) );
						nLowValue_Compare = static_cast< int >( GETOptVALUE( static_cast< RandomOption::RANDOM_OPTION >( nType ), sRandomOptionValue.getValue( 0 ) ) );
						sRandomOptionValue.remove( 0 );

						// Check;
						if ( nHighValue_Compare < static_cast< int >( fValue ) )
						{
							sc::writeLogInfo( sc::string::format(
								"[ RandomOption Log ] [ A type %1% is over power. ( %2% ( High Value ) < %3% ( Value ) ) ]",
								nType,
								nHighValue_Compare,
								static_cast< int >( fValue ) ) );

							return true;
						}
						else if ( static_cast< int >( fValue ) < nLowValue_Compare )
						{
							sc::writeLogInfo( sc::string::format(
								"[ RandomOption Log ] [ A type %1% is over power. ( %2% ( Value ) < %3% ( Low Value ) ) ]",
								nType,
								static_cast< int >( fValue ),
								nLowValue_Compare ) );

							return true;
						}
					}
				}
				else
				{
					return true;
				}
			}
			// �����ɼ� �����Ͱ� ���, GMCharEdit���� ���������� ���������� �̰� ���� OP�� �����Ѵ�;
			else
			{
				return true;
			}
		}
	}
	return false;
}

bool SITEMCUSTOM::BoxWrapping(const SNATIVEID& _wrapperID)
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem ( sNativeID );
    if ( pItem == NULL )
        return false;

    // ���� PetDbNum�� ������������ Id����ҷ� ���� �ֱ⶧���� ��ī��� ������ �ȵȴ�;
    if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
        return false;

    const SITEM* pWrapper = GLogicData::GetInstance().GetItem ( _wrapperID );
    if ( (pWrapper == NULL) || (pWrapper->sBasicOp.emItemType != ITEM_BOX_WRAPPER) )
        return false;

    const SITEM* pBox = GLogicData::GetInstance().GetItem ( pWrapper->sBasicOp.sSubID );
    if ( (pBox == NULL) || (pBox->sBasicOp.emItemType != ITEM_WRAPPED_BOX) )
        return false;

    m_PetDbNum = sNativeID.dwID;
    sNativeID = pWrapper->sBasicOp.sSubID;
    return true;
}

bool SITEMCUSTOM::BoxUnwrapping() // �ڽ� ������ ����;
{
    if ( IsBoxWrapped() == false )
        return false;

    sNativeID = m_PetDbNum;
    m_PetDbNum = 0;

    return true;
}

SNATIVEID SITEMCUSTOM::GetBoxWrappedItemID() const
{
    if ( IsBoxWrapped() == false )
        return sNativeID;

    return SNATIVEID(m_PetDbNum);
}

std::string SITEMCUSTOM::GETNAME() const            //  ������ �̸�;
{
    std::string strItemName = "";
    const SITEM* pContentItemData = GLogicData::GetInstance().GetItem( GetBoxWrappedItemID() );
    if ( !pContentItemData )
        return strItemName;

    BYTE uGRADE = GETGRADE( EMGRINDING_NUNE );

    // TODO : �� ������ ���Ͽ��� �������� �����ؾ� �Ѵ�.
    if ( uGRADE )
    {
        int nGrade = static_cast<int>(uGRADE);
        strItemName = sc::string::format( "+%1% %2%", nGrade, pContentItemData->GetName() );
    }
    else
        strItemName = sc::string::format( "%s", pContentItemData->GetName() );

    if ( IsBoxWrapped() )
    {
        const SITEM* pWrappedBox = GLogicData::GetInstance().GetItem( sNativeID );
        if ( !pWrappedBox )
            return strItemName;

        strItemName = sc::string::format( pWrappedBox->GetName(), strItemName.c_str() );
    }

    return strItemName;
}


__time64_t SITEMCUSTOM::GetPeriodExBasic_ConvertDBData(void) const
{
	CTime cDbStdTime(1970, 2, 1, 0, 0, 0);
	return cDbStdTime.GetTime() + tPERIODEX_BASIC;
}

__time64_t SITEMCUSTOM::GetPeriodExDisguise_ConvertDBData(void) const
{
	CTime cDbStdTime(1970, 2, 1, 0, 0, 0);
	return cDbStdTime.GetTime() + tPERIODEX_DISGUISE;
}

void SITEMCUSTOM::SetPeriodExBasic_FromDB(__time64_t tBasic)
{
	CTime cDbStdTime(1970, 2, 1, 0, 0, 0), cReceiveTime(tBasic);
	tPERIODEX_BASIC = cReceiveTime.GetTime() - cDbStdTime.GetTime();
}

void SITEMCUSTOM::SetPeriodExDisguise_FromDB(__time64_t tDisguise)
{
	CTime cDbStdTime(1970, 2, 1, 0, 0, 0), cReceiveTime(tDisguise);
	tPERIODEX_DISGUISE = cReceiveTime.GetTime() - cDbStdTime.GetTime();
}
