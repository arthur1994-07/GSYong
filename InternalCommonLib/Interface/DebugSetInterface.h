#pragma once

namespace ic
{

class DebugSetInterface
{
public:
    static DebugSetInterface* g_pInstance;

public:
    static void SetInstance( DebugSetInterface* pInstance );
    static DebugSetInterface* GetInstance();

public:
    virtual void ToLogFile( const char *szFormat, ... ) = 0;

public:
    DebugSetInterface();
    virtual ~DebugSetInterface();
};

};