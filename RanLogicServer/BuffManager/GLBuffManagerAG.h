#pragma once

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Character/GLCharData.h"

#include "./GLBuffManager.h"

// ----------------------------------------------------------------------------

class GLAgentServer;

namespace triggermanager
{
    class GLTriggerManagerAG;
};

namespace buffmanager
{
    class GLBuffManagerAG : public GLBuffManager
    {
    public:
        GLBuffManagerAG( GLAgentServer* pAgentServer );
        virtual ~GLBuffManagerAG();

    public:
        virtual void ToDo_BindTrigger();

    public:
        void Trigger_CTF_End( ENUM_SCHOOL emVictorySchool );

    private:
        UINT m_nKey_CTF_End;

    private:
        GLAgentServer* m_pAgentServer;

    };
};

