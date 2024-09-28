#include "stdafx.h"
#include "./charset.h"


#include "../DebugInclude.h"


namespace CHARSET
{
	int g_nCodePage = 0;

	void SetCodePage ( int nCodePage )
	{
		g_nCodePage = nCodePage;
	}

	bool IsLeadChar ( char cC )
	{
		if( IsDBCSLeadByteEx( g_nCodePage, (BYTE)cC ) )
			return true;
		else
			return false;
	}
};