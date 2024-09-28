#ifndef MiniDumpEx_H
#define MiniDumpEx_H

#include "windows.h"

class MiniDumpInitializer
{
public:
	MiniDumpInitializer();
	~MiniDumpInitializer();

private:
	LPTOP_LEVEL_EXCEPTION_FILTER m_pOldFilter;
};

#endif