
#pragma	once

#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

class	CStatsResetComboBoxRollOver : public CBasicComboBoxRollOver
{
public:
	CStatsResetComboBoxRollOver (EngineDeviceMan* pEngineDevice);
	virtual	~CStatsResetComboBoxRollOver ();

protected:
	void	CreateComboBoxRollOverImage ( bool bSelectInit );
	void	CreateScrollBar				();
};
