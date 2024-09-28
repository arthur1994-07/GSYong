#pragma once

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Character/GLCharData.h"

#include "./GLBuffManager.h"

// ----------------------------------------------------------------------------

class GLGaeaServer;
class GLChar;

namespace triggermanager
{
    class GLTriggerManagerFD;
};

namespace buffmanager
{
    class GLBuffManagerFD : public GLBuffManager
    {
    public:
        GLBuffManagerFD( GLGaeaServer* pGaeaServer );
        virtual ~GLBuffManagerFD();

    public:
        virtual void ToDo_BindTrigger();

    public:
        void Trigger_Connect_To_Field( GLChar* pChar );
        void Trigger_CTF_Begin();
        void Trigger_CTF_End( ENUM_SCHOOL emVictorySchool, UINT nRewardBuffs );
        void Trigger_CTF_New_Character( GLChar* pChar );
        void Trigger_CTF_Quit_Character( GLChar* pChar );
        void Trigger_CTF_Changed_UserCount( UINT* pSchoolArray );
		void Trigger_CTF_Rebirth_Character(GLChar* pChar);

    public:
        const bool ReceiveBuff( GLChar* pChar,
            const SNATIVEID& skill_id,
            const WORD wlevel,
            const SSKILLFACT::STIME& sTime=SSKILLFACT::STIME(_SKILLFACT_DEFAULTTIME),
            EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NORMAL,
            const DWORD dwCount=_SKILLFACT_DEFAULTCUSTOMCOUNT );

        const bool ResetBuff( GLChar* pChar, const DWORD dwIndex );
        const bool ResetBuff( GLChar* pChar, const WORD wMid, EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NONE );
        const bool ResetBuff( GLChar* pChar, const SNATIVEID* arrskill_id, const DWORD dwSize, EMSKILLFACT_TYPE emSkillFactType=EMSKILLFACT_TYPE_NONE );

    private:
        UINT          m_nKey_Connect_To_Field;
        UINT          m_nKey_CTF_Begin;
        UINT          m_nKey_CTF_End;
        UINT          m_nKey_CTF_New_Character;
        UINT          m_nKey_CTF_Quit_Character;
        UINT          m_nKey_CTF_Changed_UserCount;
		UINT          m_nKey_CTF_Rebirth_Character;

    private:
        GLGaeaServer* m_pGaeaServer;

    };
};

