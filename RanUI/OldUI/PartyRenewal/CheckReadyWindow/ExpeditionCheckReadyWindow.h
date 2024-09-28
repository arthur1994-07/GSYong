#pragma once

#include "../../../InnerInterface.h"
#include "../../Util/UIWindowEx.h"

class GLGaeaClient;
class RnButton;
class RnProgressBar;
class CBasicScrollBarEx;
class ExpeditionCheckReadyWindow : public CUIWindowEx
{
	enum
	{
		BUTTON_YES = ET_CONTROL_NEXT + 1, // 준비 완료 버튼;
		BUTTON_NO, // 준비 미완료 버튼;
	};

public:	
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;	
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void SetVisibleSingle(BOOL bVisible) override;

	void CreateSubControl(void);

private:
	void TranslateUIMessage_CheckReady(const bool bReady);

public:
	ExpeditionCheckReadyWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~ExpeditionCheckReadyWindow(void);

private:
	GLGaeaClient* m_pGaeaClient;
	RnProgressBar* m_pProgressBar;
	RnButton* m_pButtonYes;
	RnButton* m_pButtonNo;
	CBasicTextBox* m_pCheckReadyContent;

	DWORD m_dwElapsedTime;
};

class MyExpeditionCheckReadyWindow : public IExpeditionCheckReadyWindow, public ExpeditionCheckReadyWindow
{
public:
	virtual void CreateUIWindowAndRegisterOwnership(void) override;

public:
	MyExpeditionCheckReadyWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
};