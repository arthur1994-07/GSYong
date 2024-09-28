#pragma once

#include "../../../../../../../EngineLib/G-Logic/TypeDefine.h"
#include "../../../../../Util/UIGroupHelper.h"

class CSkillImage;
class SkillEffectSlot : public CUIGroupHelper
{
	enum
	{
		SKILLEFFECT_IMAGE = NO_ID + 1,
	};
public:
	void CreateSubControl(const bool bSmall);
	const bool SetSkillImage(const SNATIVEID& SkillID);

public:
	SkillEffectSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~SkillEffectSlot(void){}

private:
	GLGaeaClient* m_pGaeaClient;
	CSkillImage* m_pSkillImage;
	SNATIVEID m_SkillID;
};
