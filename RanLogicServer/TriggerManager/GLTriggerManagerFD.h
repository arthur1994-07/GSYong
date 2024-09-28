#pragma once

#include "../../RanLogic/TriggerManager/GLTriggerManager.h"
#include "../../RanLogic/Character/GLCharDefine.h"

#include <vector>

//--------------------------------------------------------------------------------------------//

class GLGaeaServer;
class GLChar;

namespace triggermanager
{
    class GLTriggerManagerFD : public GLTriggerManager
    {
    public:
        friend GLTriggerManager;

    private:
        GLTriggerManagerFD();
        ~GLTriggerManagerFD();

    private:
        typedef CTriggerList<EMTRIGGER_TYPE_CONNECT_TO_FIELD, GLChar*> TRIGGER_CONNECT_TO_FIELD;
        typedef CTriggerList<EMTRIGGER_TYPE_CTF_BEGIN> TRIGGER_CTF_BEGIN;
        typedef CTriggerList<EMTRIGGER_TYPE_CTF_END, ENUM_SCHOOL, UINT> TRIGGER_CTF_END;
        typedef CTriggerList<EMTRIGGER_TYPE_CTF_NEW_CHARACTER, GLChar*> TRIGGER_CTF_NEW_CHARACTER;
        typedef CTriggerList<EMTRIGGER_TYPE_CTF_QUIT_CHARACTER, GLChar*> TRIGGER_CTF_QUIT_CHARACTER;
        typedef CTriggerList<EMTRIGGER_TYPE_CTF_CHANGED_USERCOUNT, UINT*> TRIGGER_CTF_CHANGED_USERCOUNT;
		typedef CTriggerList<EMTRIGGER_TYPE_CTF_REBIRTH_CHARACTER, GLChar*> TRIGGER_CTF_REBIRTH_CHARACTER;

        TRIGGER_CONNECT_TO_FIELD m_Trigger_Connect_To_Field;
        TRIGGER_CTF_BEGIN m_Trigger_CTF_Begin;
        TRIGGER_CTF_END m_Trigger_CTF_End;
        TRIGGER_CTF_NEW_CHARACTER m_Trigger_CTF_New_Character;
        TRIGGER_CTF_QUIT_CHARACTER m_Trigger_CTF_Quit_Character;
        TRIGGER_CTF_CHANGED_USERCOUNT m_Trigger_CTF_Changed_UserCount;
		TRIGGER_CTF_REBIRTH_CHARACTER m_Trigger_CTF_Rebirth_Character;

    public:
        TRIGGER_CONNECT_TO_FIELD& Get_Trigger_Connect_To_Field() { return m_Trigger_Connect_To_Field; }
        TRIGGER_CTF_BEGIN& Get_Trigger_CTF_Begin() { return m_Trigger_CTF_Begin; };
        TRIGGER_CTF_END& Get_Trigger_CTF_End() { return m_Trigger_CTF_End; };
        TRIGGER_CTF_NEW_CHARACTER& Get_Trigger_CTF_New_Character() { return m_Trigger_CTF_New_Character; };
        TRIGGER_CTF_QUIT_CHARACTER& Get_Trigger_CTF_Quit_Character() { return m_Trigger_CTF_Quit_Character; };
        TRIGGER_CTF_CHANGED_USERCOUNT& Get_Trigger_CTF_Changed_UserCount() { return m_Trigger_CTF_Changed_UserCount; };
		TRIGGER_CTF_REBIRTH_CHARACTER& Get_Trigger_CTF_Rebirth_Character(){ return m_Trigger_CTF_Rebirth_Character;};

    public:
        static GLTriggerManagerFD& GetInstance();

    };

}
