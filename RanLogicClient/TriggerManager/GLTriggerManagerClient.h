#pragma once

#include "../../RanLogic/TriggerManager/GLTriggerManager.h"
#include "../../RanLogic/Character/GLCharDefine.h"

#include <vector>

//--------------------------------------------------------------------------------------------//

class GLCharacter;

namespace triggermanager
{
    class GLTriggerManagerClient : public GLTriggerManager
    {
    public:
        friend GLTriggerManager;

    private:
        GLTriggerManagerClient();
        ~GLTriggerManagerClient();

    private:
        typedef CTriggerList<EMTRIGGER_TYPE_CONNECT_TO_FIELD, GLCharacter*> TRIGGER_CONNECT_TO_FIELD;

        TRIGGER_CONNECT_TO_FIELD m_Trigger_Connect_To_Field;

    public:
        TRIGGER_CONNECT_TO_FIELD& Get_Trigger_Connect_To_Field() { return m_Trigger_Connect_To_Field; }

    public:
        static GLTriggerManagerClient& GetInstance();

    };
};
