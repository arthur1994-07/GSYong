#include "pch.h"
#include "GLInstanceSystemDefine.h"

namespace InstanceSystem
{
	extern __forceinline const unsigned int __fastcall generateHashKey(const void* const _pKey, const unsigned int _nSize)
	{
		unsigned int _nHashKey(0x1505);
		// 0001 0101 0000 0101 // 0x1505(5381);

		const TCHAR* const _pStream((const TCHAR* const)_pKey);

		for ( unsigned int _i(_nSize); _i--; )
			_nHashKey += (_nHashKey << 5) + unsigned int(_pStream[_i]);
		// hash * 33 + ch;

		return _nHashKey;
	}
}
