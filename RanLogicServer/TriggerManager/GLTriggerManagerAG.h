#pragma once

#include "../../RanLogic/TriggerManager/GLTriggerManager.h"
#include "../../RanLogic/Character/GLCharDefine.h"

#include <vector>

//--------------------------------------------------------------------------------------------//

class GLCharAG;

namespace triggermanager
{
    class GLTriggerManagerAG : public GLTriggerManager
    {
    private:
        friend GLTriggerManager;

    public:
        GLTriggerManagerAG();
        ~GLTriggerManagerAG();

    private:
        typedef CTriggerList<EMTRIGGER_TYPE_CONNECT_TO_FIELD, GLCharAG*> TRIGGER_CONNECT_TO_FIELD;
        typedef CTriggerList<EMTRIGGER_TYPE_CTF_END, ENUM_SCHOOL> TRIGGER_CTF_END;

        TRIGGER_CONNECT_TO_FIELD m_Trigger_Connect_To_Field;
        TRIGGER_CTF_END m_Trigger_CTF_End;

    public:
        TRIGGER_CONNECT_TO_FIELD& Get_Trigger_Connect_To_Field() { return m_Trigger_Connect_To_Field; }
        TRIGGER_CTF_END& Get_Trigger_CTF_End() { return m_Trigger_CTF_End; };

    public:
        static GLTriggerManagerAG& GetInstance();

    };
};
