#include "StdAfx.h"
#include "EHStackWalker.h"
#include "MinBugTrap.h"
#include <stdio.h>
#include <fstream>

using namespace std;

CEHStackWalker::CEHStackWalker( void )
{
}

void CEHStackWalker::SetLogFilePath( void )
{
	GetOutofMemoryLogPath( m_szDumpPath );
}

CEHStackWalker::~CEHStackWalker( void )
{
}

void CEHStackWalker::OnOutput( LPCSTR szText )
{
	wofstream outFile( m_szDumpPath,ios::app );
	if ( !outFile.good() )
	{
		return;
	}

	outFile << szText << endl;
}
