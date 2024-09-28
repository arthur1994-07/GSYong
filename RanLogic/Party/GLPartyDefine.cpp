#include "pch.h"
#include "../Item/GLItem.h"
#include "GLPartyDefine.h"
#include "../RANPARAM.h"

const GLPartyID& GLPartyID::operator =(const DWORD& dwPartyID)
{
	wPartyID = WORD(dwPartyID & 0xffff);	
	wExpeditionID = WORD(dwPartyID >> 16);
	return *this;
}

const bool GLPartyID::operator !=(const GLPartyID& PartyID) const
{
	if ( PartyID.isValidExpedition() == true )
		return (wExpeditionID != PartyID.wExpeditionID);

	if ( PartyID.isValidParty() == true )
		return (wPartyID != PartyID.wPartyID);

	return true;
}

const bool GLPartyID::operator ==(const GLPartyID& PartyID) const
{
	if ( PartyID.isValidExpedition() == true )
		return (wExpeditionID == PartyID.wExpeditionID);

	if ( PartyID.isValidParty() == true )
		return (wPartyID == PartyID.wPartyID);

	return false;
}

const bool GLPartyID::operator !=(const DWORD& dwPartyID) const
{
	return GLPartyID(dwPartyID) != *this;
}
const bool GLPartyID::operator ==(const DWORD& dwPartyID) const
{
	return GLPartyID(dwPartyID) == *this;
}
const bool GLPartyID::operator <(const GLPartyID& PartyID) const
{
	if ( PartyID.isValidExpedition() == true )
		return wExpeditionID < PartyID.wExpeditionID;
	else
		return wPartyID < PartyID.wPartyID;
}
const bool GLPartyID::operator >(const GLPartyID& PartyID) const
{
	if ( PartyID.isValidExpedition() == true )
		return wExpeditionID > PartyID.wExpeditionID;
	else
		return wPartyID > PartyID.wPartyID;
}

GLPartyID::GLPartyID(const GLPartyID& PartyID)
{
	wPartyID = PartyID.wPartyID;
	wExpeditionID = PartyID.wExpeditionID;	
}
GLPartyID::GLPartyID(const DWORD& dwPartyID, const DWORD& dwExpeditionID)
{
	wPartyID = WORD(dwPartyID);
	wExpeditionID = WORD(dwExpeditionID);	
}
GLPartyID::GLPartyID(const DWORD& dwPartyID)
{
	wPartyID = WORD(dwPartyID & 0xffff);	
	wExpeditionID = WORD(dwPartyID >> 16);	
}

GLPartyID::GLPartyID(void) : 
wPartyID(INVALID),
wExpeditionID(INVALID)
{
}

void GLPartyID::Reset(void)
{
	wPartyID = INVALID;
	wExpeditionID = INVALID;
}
const bool GLPartyID::isValidExpedition(void) const
{ 
	return ((wExpeditionID != INVALID) && (wPartyID != INVALID));	
}
const bool GLPartyID::isValidParty(void) const
{	
	return (wPartyID == INVALID ? false : true);
}
const DWORD GLPartyID::GetID(void) const
{
	const DWORD dwLow = DWORD(wPartyID);
	const DWORD dwHigh = DWORD(wExpeditionID) << 16;
	return (dwHigh + dwLow);
}
const DWORD GLPartyID::GetPartyID(void) const
{
	return wPartyID;
}
const DWORD GLPartyID::GetExpeditionID(void) const
{
	return wExpeditionID;
}
void GLPartyID::SetPartyID(const WORD _wPartyID)
{
	wPartyID = _wPartyID;
}
void GLPartyID::SetExpeditionID(const WORD _wExpeditionID)
{
	wExpeditionID = _wExpeditionID;
}
void GLPartyID::SetPartyID(const DWORD dwPartyID)
{
	wPartyID = WORD(dwPartyID);
}
void GLPartyID::SetExpeditionID(const DWORD dwExpeditionID)
{
	wExpeditionID = WORD(dwExpeditionID);
}

SPartyOption::SPartyOption(void)
: emItemOption(EMPARTY_ITEM_FREE)
, emMoneyOption(EMPARTY_MONEY_FREE)
, emItemFilterLevel(EMPARTY_ITEM_FILTER_MIDTOHIGH)
{
}
SPartyOption::SPartyOption(const EMPARTY_ITEM_OPTION _emItemOption, const EMPARTY_MONEY_OPTION _emMoneyOption)
: emItemOption(_emItemOption)
, emMoneyOption(_emMoneyOption)
, emItemFilterLevel(EMPARTY_ITEM_FILTER_MIDTOHIGH)
{
}
SPartyOption::SPartyOption(const DWORD dwItemOption, const DWORD dwMoneyOption)
: emItemOption(EMPARTY_ITEM_OPTION(dwItemOption))
, emMoneyOption(EMPARTY_MONEY_OPTION(dwMoneyOption))
, emItemFilterLevel(EMPARTY_ITEM_FILTER_MIDTOHIGH)
{
}
void SPartyOption::SetItemOption(const EMPARTY_ITEM_OPTION _emItemOption)
{
	if ( DWORD(_emItemOption) > (EMPARTY_ITEM_NSIZE - 1) )
		return;

	emItemOption = _emItemOption;
}
void SPartyOption::SetMoneyOption(const EMPARTY_MONEY_OPTION _emMoneyOption)
{
	if ( DWORD(_emMoneyOption) > (EMPARTY_MONEY_NSIZE - 1) )
		return;

	emMoneyOption = _emMoneyOption;
}

void SPartyOption::SetItemFilter(const EMPARTY_ITEM_FILTER_OPTION _emItemFilterLevel)
{
	if ( DWORD(_emItemFilterLevel) > (EMPARTY_ITEM_FILTER_NSIZE - 1))
		return;

	emItemFilterLevel = _emItemFilterLevel;
}


const EMPARTY_ITEM_OPTION SPartyOption::GetItemOption(void) const
{
	return emItemOption;
}
const EMPARTY_MONEY_OPTION SPartyOption::GetMoneyOption(void) const
{
	return emMoneyOption;
}

const EMPARTY_ITEM_FILTER_OPTION SPartyOption::GetItemFilter(void) const
{
	return emItemFilterLevel;
}

void SPartyOption::Reset(void)
{
	emItemOption = EMPARTY_ITEM_FREE;
	emMoneyOption = EMPARTY_MONEY_FREE;
	emItemFilterLevel = EMPARTY_ITEM_FILTER_MIDTOHIGH;

	// Ranparam ÃÊ±âÈ­ 
	RANPARAM::dwPARTY_GET_ITEM = (DWORD)emItemOption;
	RANPARAM::dwPARTY_GET_ITEM_Filter = (DWORD)EMPARTY_ITEM_FILTER_MIDTOHIGH;
	RANPARAM::dwPARTY_GET_MONEY = (DWORD)EMPARTY_MONEY_FREE;
}

const bool SPartyOption::isValidOption(void) const
{
	if ( unsigned int(emItemOption) > (EMPARTY_ITEM_NSIZE - 1) )
		return false;

	if ( unsigned int(emMoneyOption) > (EMPARTY_MONEY_NSIZE - 1) )
		return false;

	return true;
}

const bool SPartyOption::operator != ( const SPartyOption& PartyOption ) const
{	
	if ( (emItemOption != PartyOption.emItemOption) ||
		(emMoneyOption != PartyOption.emMoneyOption) )
		return true;
	return false;
}

const bool SPartyOption::operator == ( const SPartyOption& PartyOption ) const
{
	if ( (emItemOption != PartyOption.emItemOption) ||
		(emMoneyOption != PartyOption.emMoneyOption) )
		return false;
	return true;
}


GLExpedition::GLExpedition(void)
{	
	Reset();
}
GLExpedition::~GLExpedition(void)
{
}

void GLExpedition::Reset(void)
{
	m_dwExpeditionID = DEFPARTY_NULL;
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		m_dwMasterGaeaIDs[_i - 1] = GAEAID_NULL;
		m_PartyIDs[_i - 1].Reset();
	}
}

const bool GLExpedition::isMaster(const DWORD dwGaeaID) const
{
	return (m_dwMasterGaeaIDs[0] == dwGaeaID);
}
const bool GLExpedition::isSubMaster(const DWORD dwGaeaID) const
{
	for ( DWORD _i = 1; _i < MAX_PARTYGROUP; ++_i )
	{
		if ( m_dwMasterGaeaIDs[_i] == dwGaeaID )
			return true;
	}

	return false;
}
void GLExpedition::SetMasterGaeaID(const DWORD dwGaeaID, const DWORD dwPartySlotIndex)
{
	if ( dwPartySlotIndex >= MAX_PARTYGROUP )
		return;

	const DWORD dwCurrentGaeaID = m_dwMasterGaeaIDs[dwPartySlotIndex];

	DWORD nPrevPartyIndex = DEFSLOT_NULL;
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		if ( m_dwMasterGaeaIDs[_i - 1] != dwGaeaID )
			continue;

		nPrevPartyIndex= _i - 1;
		break;
	}

	if ( nPrevPartyIndex != DEFSLOT_NULL )
		m_dwMasterGaeaIDs[nPrevPartyIndex] = GAEAID_NULL;

	m_dwMasterGaeaIDs[dwPartySlotIndex] = dwGaeaID;	
}

void GLExpedition::SetExpeditionID(const DWORD dwExpeditionID)
{
	m_dwExpeditionID = dwExpeditionID;
}
void GLExpedition::SetPartyID(const GLPartyID PartyIDs[MAX_PARTYGROUP])
{
	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		m_PartyIDs[_i - 1] = PartyIDs[_i - 1];
}

const DWORD GLExpedition::GetMasterGaeaID(void) const
{
	return m_dwMasterGaeaIDs[0];
}

const DWORD GLExpedition::GetMasterGaeaID(const DWORD dwPartyIndex) const
{
	return m_dwMasterGaeaIDs[dwPartyIndex];
}

const DWORD GLExpedition::GetExpeditionID(void) const
{
	return m_dwExpeditionID;
}

const DWORD GLExpedition::GetPartySlotIndex(const GLPartyID& PartyID) const
{
	if ( PartyID.isValidExpedition() == false )
		return DEFPARTY_NULL;

	if ( PartyID.isValidParty() == false )
		return DEFPARTY_NULL;	

	for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
	{
		if ( m_PartyIDs[_i - 1].GetPartyID() == PartyID.GetPartyID() )
			return _i - 1;
	}

	return DEFPARTY_NULL;	
}

const GLPartyID& GLExpedition::GetPartyID(const DWORD dwIndex) const
{	
	return m_PartyIDs[dwIndex];
}

GLTenderItemCustom::GLTenderItemCustom(const MapID& mapID, const float fInsertTime, const D3DXVECTOR3& vGeneratePosition)
: m_mapID(mapID)
, m_fInsertTime(fInsertTime)
, m_vGeneratePosition(vGeneratePosition)
{
} 

