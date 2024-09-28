#pragma once

#include "../../InternalCommonLib/Interface/DebugSetInterface.h"

class IDebugSet : public ic::DebugSetInterface
{
public:
    virtual void ToLogFile( const char *szFormat, ... );

public:
    IDebugSet();
    virtual ~IDebugSet();
};
