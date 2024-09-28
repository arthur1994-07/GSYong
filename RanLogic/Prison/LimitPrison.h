#pragma	once

#include "../Land/GLMapList.h"

#define PRISONLIMIT	CPrisionLimitChecker::GetInstance()

//struct SMAPNODE;

class CPrisionLimitChecker
{
public :
	CPrisionLimitChecker();
	~CPrisionLimitChecker();

public :
	bool IsLimitStatus( const SMAPNODE *pMapNode, DWORD dwUserLv, EPML eLimit = PML_NONE );
	bool IsPrisionMap( const SMAPNODE *pMapNode, DWORD dwUserLv );
public :
	static CPrisionLimitChecker& GetInstance();
};