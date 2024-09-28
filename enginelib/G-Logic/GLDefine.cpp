#include "pch.h"

#include "GLDefine.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

STARGETID::STARGETID(void)
: emCrow(CROW_MOB)
, GaeaId(EMTARGET_NULL)
, vPos(0.0f, 0.0f, 0.0f)
{
}
STARGETID::STARGETID(const STARID& rhs)
: emCrow(EMCROW(rhs.emCrow))
, GaeaId(rhs.dwID)
, vPos(0.0f, 0.0f, 0.0f)
{
}
STARGETID::STARGETID(const EMCROW _emCrow, const DWORD _dwID)
: emCrow(_emCrow)
, GaeaId(_dwID)
, vPos(0.0f, 0.0f, 0.0f)
{
}

STARGETID::STARGETID(const DWORD _emCrow, const DWORD _dwID)
: emCrow(EMCROW(_emCrow))
, GaeaId(_dwID)
, vPos(0.0f, 0.0f, 0.0f)
{
}

STARGETID::STARGETID(const EMCROW _emCrow, const DWORD _dwID, const D3DXVECTOR3& _vPos)
: emCrow(_emCrow)
, GaeaId(_dwID)
, vPos(_vPos)
{
}

STARGETID::STARGETID(const DWORD _emCrow, const DWORD _dwID, const D3DXVECTOR3& _vPos)
: emCrow(EMCROW(_emCrow))
, GaeaId(_dwID)
, vPos(_vPos)
{
}

void STARGETID::RESET(void)
{
	emCrow = CROW_MOB;
	GaeaId = EMTARGET_NULL;
	vPos = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
}

const bool STARGETID::ISNULL(void) const
{
	return (GaeaId == EMTARGET_NULL);
}

const STARGETID& STARGETID::operator =(const STARID& rhs)
{		
	emCrow = EMCROW(rhs.emCrow);
	GaeaId = rhs.dwID;
	vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return *this;
}

const bool STARGETID::operator < (const STARGETID& TargetID) const
{
	if ( std::make_pair(emCrow, GaeaId) < std::make_pair(TargetID.emCrow, TargetID.GaeaId) )
		return true;
	return false;
}

const bool STARGETID::operator == (const STARGETID& TargetID) const
{
	return ( TargetID.GaeaId == GaeaId && TargetID.emCrow == emCrow );
}

const bool STARGETID::operator != (const STARGETID& TargetID) const
{
	return ( TargetID.GaeaId != GaeaId || TargetID.emCrow != emCrow );
}

STARID::STARID(void)
: emCrow(CROW_MOB)
, dwID(EMTARGET_NULL)
{
}

STARID::STARID(const EMCROW _emCrow, const DWORD _dwID) 
: emCrow(DWORD(_emCrow))
, dwID(_dwID)
{
}
STARID::STARID(const DWORD _emCrow, const DWORD _dwID) 
: emCrow(_emCrow)
, dwID(_dwID)
{
}

void STARID::RESET(void)
{
	emCrow = CROW_MOB;
	dwID = EMTARGET_NULL;
}

void STARID::SET(const STARGETID &sTARID)
{
	emCrow = DWORD(sTARID.emCrow);
	dwID = sTARID.GaeaId;
}

const EMCROW STARID::GETCROW(void) const
{
	return EMCROW(emCrow);
}
const DWORD STARID::GETID(void) const
{		
	return dwID;
}


namespace COMMENT
{	
	std::string CROW[CROW_NUM] = 
	{
		"CROW_PC",
		"CROW_NPC",
		"CROW_MOB",
		"CROW_ITEM",
		"CROW_MONEY",
		"CROW_PET"	// PetData
		"CROW_MATERIAL"
	};

	std::string ATTACK_RGTYPE[EMATT_SIZE] =
	{
		"단거리",
		"장거리",
	};
}


void GLDWDATA::LIMIT(void)
{
	if ( nNow > nMax )
		nNow = nMax;
}