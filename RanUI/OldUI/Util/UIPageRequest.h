
#pragma once

struct SPAGEREQUEST
{
    DWORD dwID;
    DWORD dwARG1;

    SPAGEREQUEST ()                                  : dwID(0),         dwARG1(0)      {}
    SPAGEREQUEST ( DWORD dwRequest, DWORD dwArg1=0 ) : dwID(dwRequest), dwARG1(dwArg1) {}
};
