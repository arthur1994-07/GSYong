#include "pch.h"

#include "UIMutuallyExclusive.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

UIMutuallyExclusive::UIMutuallyExclusive()
{
    AddDefine( POSTBOX_WINDOW, STORAGE_WINDOW );
}

UIMutuallyExclusive& UIMutuallyExclusive::GetInstance()
{
    static UIMutuallyExclusive inst;
    return inst;
}

UIGUID UIMutuallyExclusive::GetMutually( UIGUID WindowID )
{
    std::map<UIGUID, UIGUID>::iterator it = m_MutuallyExclusiveWindow.find( WindowID );
    if( it != m_MutuallyExclusiveWindow.end() )
        return m_MutuallyExclusiveWindow.find( WindowID )->second;

    return 0;
}


void UIMutuallyExclusive::AddDefine( UIGUID WindowID, UIGUID Target_WindowID )
{
    m_MutuallyExclusiveWindow.insert( std::make_pair(WindowID, Target_WindowID) );
    m_MutuallyExclusiveWindow.insert( std::make_pair(Target_WindowID, WindowID) );
}
