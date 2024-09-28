#pragma once

#include "../Util/UIGroupHelper.h"

// 2차 비밀번호 - 랜덤 숫자 키패드
// 버튼 대신에 이미지를 버튼화함

// 0 ~ 9 출력
// 최소 숫자 0, 최대 숫자 9

class COuterInterface;
class CModalWindow; // 2차비번
class CSwapImage;
class CBasicLineBox;

class CRandPassKeyboard : public CUIGroupHelper
{
	enum
	{
		// 버튼 11개;
		KEY_BUTTON_0 = NO_ID+1,
		KEY_BUTTON_1,
		KEY_BUTTON_2,
		KEY_BUTTON_3,
		KEY_BUTTON_4,
		KEY_BUTTON_5,
		KEY_BUTTON_6,
		KEY_BUTTON_7,
		KEY_BUTTON_8,
		KEY_BUTTON_9, 
		KEY_BUTTON_10,

		// 백스페이스 키;
		KEY_BUTTON_BACKSPACE_KEY,
	};

	enum
	{
		KEY_BUTTON_SIZE = 11,
	};

public:
	CRandPassKeyboard(COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	CRandPassKeyboard(CModalWindow* pModalWindow, EngineDeviceMan* pEngineDevice); // 2차비번
	virtual	~CRandPassKeyboard();

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

	void CreateSubControl();

protected:
	void TranslateKeyMsg( UIGUID ControlID, DWORD dwMsg, BOOL bNumber = FALSE );
	void SetNumberImage();

protected:
	COuterInterface*	m_pInterface;
	CModalWindow*		m_pModalWindow;
	CBasicLineBox*		m_pLineBoxBackground;
	CSwapImage*			m_pNumImage[ KEY_BUTTON_SIZE ];

private :
	std::vector< unsigned int > m_vecNum;
};