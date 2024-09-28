#pragma once

#include "../../../../../RanLogic/Character/GLCharDefine.h"
#include "../../../Util/UIGroupHelper.h"

struct SITEM;
struct GLPartySlotClient;
class CUIControl;
class CBasicTextBox;
class DistributionMemberSlot : public CUIGroupHelper
{
public:
	void CreateSubControl(void);
	const bool SetSlot(GLPartySlotClient* pMemberSlot, const SITEM* const pItemData, const SNATIVEID& ItemMapID);
	void SetSelect(void);	

	void RESET(void);

public:
	DistributionMemberSlot(EngineDeviceMan* pEngineDevice);
	virtual ~DistributionMemberSlot(void){}

private:
	CUIControl* m_pSelectBox;
	CBasicTextBox* m_pClass;
	CBasicTextBox* m_pLevel;
	CBasicTextBox* m_pName;	

	GLPartySlotClient* m_pMember;
};