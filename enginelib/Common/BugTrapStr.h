#ifndef _BUG_TRAP_STR_H_
#define _BUG_TRAP_STR_H_

#pragma once

#include "../ServiceProviderDefine.h"

namespace BUG_TRAP
{    
// 	extern TCHAR szCountry[MAX_PATH];
// 	extern TCHAR szEMail[MAX_PATH];
// 	extern TCHAR szSupportURL[MAX_PATH];
// 	extern TCHAR szServerAddr[MAX_PATH];
// 	extern SHORT nPort;

    std::string Country(EMSERVICE_PROVIDER Type);
    std::string Email(EMSERVICE_PROVIDER Type);
    std::string SupportUrl(EMSERVICE_PROVIDER Type);
    std::string ServerAddress(EMSERVICE_PROVIDER Type);
    SHORT Port(EMSERVICE_PROVIDER Type);
}

#endif // _BUG_TRAP_STR_H_