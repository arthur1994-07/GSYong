
#include "pch.h"
#include "./GLItem_RandomGenOp_Utility.h"

namespace ITEM_RANDOMOP_UTILL{

	const FixOption& FixOption::operator =(const FixOption& rhs)
	{
		nSize = rhs.nSize;
		for ( DWORD _i(FixOption::FIX_MAXSIZE); _i--; )
			cOptType[_i] = rhs.cOptType[_i];

		return *this;
	}
	const bool FixOption::operator ==(const BYTE& _cOptType) const
	{
		for ( DWORD _i(nSize); _i--; )
		{
			if ( _cOptType == cOptType[_i] )
				return true;
		}
		return false;
	}

	FixOption::FixOption(void) : nSize(0)
	{
		for ( DWORD _i(FixOption::FIX_MAXSIZE); _i--; )
			cOptType[_i] = 0;
	}

	void FixOption::push(const BYTE _cOptType, const DWORD nFixMaximum)
	{
		if ( nSize >= nFixMaximum )
			return;

		cOptType[nSize++] = _cOptType;
	}

	void FixOption::clear(void)
	{
		nSize = 0;
		for ( DWORD _i(FixOption::FIX_MAXSIZE); _i--; )
			cOptType[_i] = 0;
	}

	const bool FixOption::isValid(void) const
	{
		return nSize == 0 ? false : true;
	}

	const DWORD FixOption::getNSize(void) const
	{
		return nSize;
	}

	const BYTE FixOption::GetOptType ( const UINT _nIndex ) const
	{
		if ( nSize <= _nIndex )
			return 0;

		return cOptType[_nIndex];
	}

};