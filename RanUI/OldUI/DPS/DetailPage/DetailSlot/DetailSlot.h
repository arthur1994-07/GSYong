#pragma once

#include "../../../../InnerInterface.h"
#include "../../../Util/UIGroupHelper.h"

namespace DamageRecord
{
	class DetailElement;
}
class GLGaeaClient;
class CBasicTextBox;
class DetailSlot : public CUIGroupHelper
{
public:
	void CreateSubControl(void);
	void SetSlotInformation(const DamageRecord::DetailElement* const pElement, const DWORD nDamage);

public:
	DetailSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient, const DWORD nSlotIndex);
	virtual ~DetailSlot(void);

private:
	const DWORD m_nSlotIndex;
	GLGaeaClient* const m_pGaeaClient;

	CBasicTextBox* m_pName;
	CBasicTextBox* m_pDamage;
	CBasicTextBox* m_pUseCount;
	CBasicTextBox* m_pDPS;
	CBasicTextBox* m_pRate;
};
