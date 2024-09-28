#pragma once

#include "fdi.h"

namespace mincab
{

/*
 * Function prototypes 
 */
//	szPathName���� Ÿ�� ���丮�� ����ϰ�, szFileName���� FullPathFileName�� ����Ѵ�.

BOOL Extract(const char* szPathName, const char* szFileName);
const char*	GetErrorMsg();

void SetCabForceTerminate(BOOL* pForceTerminate);
void SetCabProgressValue(ULONGLONG* pCur, ULONGLONG* pEnd);

} // namespace mincab