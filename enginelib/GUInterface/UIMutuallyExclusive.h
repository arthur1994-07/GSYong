#pragma once
#include "InterfaceBaseDefine.h"

class UIMutuallyExclusive// : public boost::noncopyable
{
public:
    static UIMutuallyExclusive& GetInstance();

    UIMutuallyExclusive();
    UIGUID GetMutually( UIGUID WindowID );;
    
protected:
    void AddDefine( UIGUID WindowID, UIGUID Target_WindowID );
    std::map<UIGUID, UIGUID> m_MutuallyExclusiveWindow;
};