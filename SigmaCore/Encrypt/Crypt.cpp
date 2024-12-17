#include "pch.h"
#include "stdafx.h"
#include "Crypt.h"
#include "../enginelib/Common/SUBPATH.h"
#include "../enginelib/G-Logic/GLOGIC.h"
#include "../enginelib/Common/IniLoader.h"
#include "../enginelib/Common/GLTexFile.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace CCrypt
{
	BOOL	bCryptRCC = TRUE;
	BOOL	bEngineDebug = TRUE;

	void  LoadSetting( const TCHAR *szRootPath )
	{
		CString strFileName = szRootPath;
		strFileName += "\\GameSet.ini";



		DWORD	dwKey = 0;
			
		if ( dwKey == 789456 )	bCryptRCC = FALSE;

	}

	void Encryption(BYTE *B ,int dwSize)
	{
		for( int i = 0 ; i < dwSize ; i ++ )
		{
			B[i] = B[i] ^ EN;
			B[i] += ( EN ^ EN2 );
		};
		return ;
	}
	void Decryption(BYTE *B,int dwSize)
	{
		for( int i = 0 ; i < dwSize ; i ++ )
		{
			B[i] -= ( EN ^ EN2 );
			B[i] = B[i] ^ EN;
		};
		return ;
	}
};
