#pragma	once

#include "./BasicComboBox.h"
#include "UITextControl.h"

class CSmartComboBoxBasic : public CBasicComboBox
{

public:
	CSmartComboBoxBasic(EngineDeviceMan* pEngineDevice);
	virtual	~CSmartComboBoxBasic();

public:
	void CreateSmartComboBox(const char* szComboBoxControl, const char* szTextureInfo);

public:
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);

protected:
	void CreateSmartComboImage(const char* szTextureInfo);
	void CreateSmartTextBox();

};
