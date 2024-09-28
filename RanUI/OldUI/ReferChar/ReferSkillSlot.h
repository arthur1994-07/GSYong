#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"

#include "../../InnerInterface.h"

class CSkillImage;
class CBasicTextBox;

class CReferSkillSlot : public CUIGroup
{
private:
    enum
	{
		SKILL_SLOT_IMAGE = NO_ID + 1,
	};

public:
    CReferSkillSlot( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CReferSkillSlot(){};

	void	            CreateSubControl();

	void		        SetNativeSkillID( SNATIVEID sNativeID, WORD nLevel, BOOL bMaster );
    SNATIVEID	        GetNativeSkillID();

protected:
	GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

private:
    CSkillImage*	    m_pSkillImage;
    CBasicTextBox*	    m_pTextBox;

    SNATIVEID           m_sNativeID;
};