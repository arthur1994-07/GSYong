
#include "../pch.h"
#include "./GLCrowDefine.h"

namespace COMMENT
{
	std::string SALETYPE[CROWSALE_SIZE] = 
	{
		"ETC",
		"Armor",
		"Weapon",
		"S.R.C"
	};

	int GetSaleType( const std::string& strType )
	{
		for ( int i = 0; i < CROWSALE_SIZE; ++i )
		{
			if ( strcmp(SALETYPE[i].c_str(), strType.c_str()) == 0 )
				return i;
		}
		return CROWSALE_SIZE;
	}
};