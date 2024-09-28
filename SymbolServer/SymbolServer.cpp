// SymbolServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "MiniDumpEx.h"

int _tmain(int argc, _TCHAR* argv[])
{
    MiniDumpInitializer dumper;

    // create a minidump;
    int* p = 0;
    *p = 1;

	return 0;
}

