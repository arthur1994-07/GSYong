
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
			통합 클럽 UI를 생성한다.
			화면 중앙에 최상위 모달 박스로 생성 한다.
			
			\return 생성 성공 유무
		 */
virtual bool				CreateSubControl();

		/**
			버튼 클릭, 타임 종료 등의 메시지를 처리 한다.

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

	CBasicTextButton*	m_pOk;		//@ 입장 버튼

	/**
        외부 인터페이스

     */
    GLGaeaClient*       m_pGaeaClient;
	CBasicTextBox* m_pMatchingLabel;
	CBasicTextBox* m_pMatchingTimerLabel;
	CSwapImage* m_pTimerImage;

	DWORD m_timer;
	bool m_isrevers;
	DWORD m_SavedTime;

	/**
		시간 정보

	 */
    int                 m_nUnitsDigitMin;   //@ 분 1 자리
    int                 m_nTensDigitMin;    //@ 분 10자리
    int                 m_nUnitsDigitSec;   //@ 초 1 자리
    int                 m_nTensDigitSec;    //@ 초 10자리
	float m_fElapsedTime;
};