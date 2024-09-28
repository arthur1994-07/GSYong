#include "pch.h"
#include "./UITextControl.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace	NS_UITEXTCONTROL
{
	DWORD	GetEvaluateColor ( BOOL bValue )
	{
		if ( bValue )	return ENABLE;
		else			return DISABLE;
	}

    DWORD   GetActivityColor ( int nClass )
    {
        if ( nClass < 0 || nClass >= NS_UITEXTCOLOR::ACTIVITY_TITLE_COLOR_SIZE ) 
        {
            return ENABLE;
        }

        return NS_UITEXTCOLOR::ACTIVITY_TITLE_COLOR[nClass];
    }
}