//-----------------------------------------------------------------------------
// File: Pch.h
//
// Desc: Header file to precompile
//
// Copyright (C) 1998-2000 Microsoft Corporation. All Rights Reserved.
//
//
//-----------------------------------------------------------------------------
#ifndef __PCH__H
#define __PCH__H

#ifdef _MAX_2012
    #include "../Max12_sdk/include/Max.h"
	#include "../Max12_sdk/include/Max.h"
    #include "../Max12_sdk/include/istdplug.h"
    #include "../Max12_sdk/include/iparamb2.h"
    #include "../Max12_sdk/include/iparamm2.h"
    #include "../Max12_sdk/include/cs/phyexp.h"
    #include "../Max12_sdk/include/stdmat.h"
    #include "../Max12_sdk/include/iskin.h"
#else
    #include "../Max11_sdk/include/Max.h"
    #include "../Max11_sdk/include/istdplug.h"
    #include "../Max11_sdk/include/iparamb2.h"
    #include "../Max11_sdk/include/iparamm2.h"
    #include "../Max11_sdk/include/cs/phyexp.h"
    #include "../Max11_sdk/include/stdmat.h"
    #include "../Max11_sdk/include/iskin.h"

//	#include "../../RanThirdParty/Max11_sdk/include/Max.h"
#endif

#include "resource.h"
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <direct.h>
#include <commdlg.h>

#include "d3dx9.h"

inline BOOL _streql ( char *str1, char *str2, int nComp )
{
	while ( (*str1==*str2) && (*str1) && (nComp>=0) )
	{
		++str1;
		++str2;
		--nComp;
	}

	return (nComp==0);
}

#endif