#pragma once

#include "../Util/UIGroupHelper.h"

// 2�� ��й�ȣ - ���� ���� Ű�е�
// ��ư ��ſ� �̹����� ��ưȭ��

// 0 ~ 9 ���
// �ּ� ���� 0, �ִ� ���� 9

class COuterInterface;
class CModalWindow; // 2�����
class CSwapImage;
class CBasicLineBox;

class CRandPassKeyboard : public CUIGroupHelper
{
	enum
	{
		// ��ư 11��;
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

		// �齺���̽� Ű;
		KEY_BUTTON_BACKSPACE_KEY,
	};

	enum
	{
		KEY_BUTTON_SIZE = 11,
	};

public:
	CRandPassKeyboard(COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	CRandPassKeyboard(CModalWindow* pModalWindow, EngineDeviceMan* pEngineDevice); // 2�����
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