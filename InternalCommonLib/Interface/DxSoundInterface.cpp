#include "stdafx.h"

#include "./DxSoundInterface.h"

#include "../../InternalCommonLib/DebugInclude.h"

namespace ic
{
    static IDxSound* s_pDxSoundInstance = NULL;

    VOID IDxSound::SetInstance( IDxSound* pInstance )
    {
        s_pDxSoundInstance = pInstance;
    }

    IDxSound* IDxSound::GetInstance()
    {
        return s_pDxSoundInstance;
    }
}