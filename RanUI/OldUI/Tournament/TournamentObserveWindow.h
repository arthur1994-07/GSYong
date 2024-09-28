
#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Competition/PVEIndicator.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../InnerInterface.h"
#include "../Util/UIGroupHelperNonClose.h"

class CUINumberTimer;
class CUIStageIndicator;
class EngineDeviceMan;


class TournamentObserveWindow
	: public CUIGroupHelperNonClose
{

private:
	enum
	{
		CONTROL_ID_NONE = NO_ID + 1,
		CONTROL_ID_BACK,
	};

public:
		TournamentObserveWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual	~TournamentObserveWindow();

		/**
			���� Ŭ�� UI�� �����Ѵ�.
			ȭ�� �߾ӿ� �ֻ��� ��� �ڽ��� ���� �Ѵ�.
			
			\return ���� ���� ����
		 */
virtual bool				CreateSubControl();

		/**
			��ư Ŭ��, Ÿ�� ���� ���� �޽����� ó�� �Ѵ�.

		 */

virtual	void				TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);
CBasicTextBox*				CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID );
CBasicLineBoxSmart*			CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID = NO_ID  );
CBasicTextButton*			CreateButton(const TCHAR* name, const TCHAR* text, UIGUID id);

void						SetText( const char* strText );
void						SetTimer( int _timer, bool revers );
void						SetGameTime(int nTime);
void						Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
protected:

	CBasicTextButton*	m_pOk;		//@ ���� ��ư

	/**
        �ܺ� �������̽�

     */
    GLGaeaClient*       m_pGaeaClient;
	CBasicTextBox* m_pMatchingLabel;
	CBasicTextBox* m_pMatchingTimerLabel;
	CSwapImage* m_pTimerImage;

	DWORD m_timer;
	bool m_isrevers;
	DWORD m_SavedTime;

	/**
		�ð� ����

	 */
    int                 m_nUnitsDigitMin;   //@ �� 1 �ڸ�
    int                 m_nTensDigitMin;    //@ �� 10�ڸ�
    int                 m_nUnitsDigitSec;   //@ �� 1 �ڸ�
    int                 m_nTensDigitSec;    //@ �� 10�ڸ�
	float m_fElapsedTime;
};