#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

#include "./ILottoSystemUI.h"

class CInnerInterface;
class EngieDeviceMan;

class CSwapImage;
class CBasicTextBox;

// 로또 시스템의 Notify Button;
// 로또 시스템의 UI를 키는 역할과 상태를 알리는 역할을 함;
class CLottoNotifyButton : public CUIGroup
{
private:
	enum
	{
		ENotifyButton		= NO_ID +1
	};

	enum ELOTTO_NOTIFY_TYPE
	{
		ENOTIFY_TYPE_NONE,
		ENOTIFY_TYPE_WAIT_STATE,
		ENOTIFY_TYPE_DRAWING_STATE
	};

public:
	CLottoNotifyButton( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoNotifyButton();

	void CreateSubControl();

public:
	void SetNotifyType( ELOTTO_NOTIFY_TYPE notifyType );

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	CSwapImage* m_pLottoImage;
	CSwapImage* m_pStateImage;
	CSwapImage* m_pCountTextBoxBackgroundImage;
	CBasicTextBox* m_pCountTextBox;

	ELOTTO_NOTIFY_TYPE m_type;
	bool m_bRead;

	std::string m_strTooltip;

	unsigned int m_curLottoSystemId;

	static const float fDESC_ANIMATION_SPEED_DELAY;

protected:
	CInnerInterface* m_pInterface;
};

class MyLottoNotifyButton : public ILottoNotifyButton, public CLottoNotifyButton
{
public:
	MyLottoNotifyButton( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyLottoNotifyButton() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual bool IsVisible();
};