
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


class CTournamentMulty
	: public CUIGroupHelperNonClose
{

private:
	enum
	{
		CONTROL_ID_NONE = NO_ID + 1,
		CONTROL_ID_BACK,
	};

public:
		CTournamentMulty(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual	~CTournamentMulty();

		/**
			통합 클럽 UI를 생성한다.
			화면 중앙에 최상위 모달 박스로 생성 한다.
			
			\return 생성 성공 유무
		 */
virtual bool				CreateSubControl();

		/**
			버튼 클릭, 타임 종료 등의 메시지를 처리 한다.

		 */
//virtual	void				TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);

CBasicTextBox*		CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID );
CBasicLineBoxSmart* CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID = NO_ID  );

protected:

	CBasicLineBoxSmart*			TOURNAMENT_RIGHT_MULTY_TEAM_BG;				//@ 
	CBasicLineBoxSmart*			TOURNAMENT_RIGHT_MULTY_TEAM_BG_INNER;		//@
//	CBasicLineBoxSmart*			TOURNAMENT_RIGHT_MULTY_TEAM_TOP;			//@
//	CBasicLineBoxSmart*			TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM;			//@
	CBasicLineBoxSmart*			TOURNAMENT_RIGHT_MULTY_TEAM_RESUALT;		//@


	CBasicTextBox*				TOURNAMENT_RIGHT_MULTY_TEAM_TOP_RANK;				//@
	CBasicTextBox*				TOURNAMENT_RIGHT_MULTY_TEAM_TOP_NAME;				//@
	CBasicTextBox*				TOURNAMENT_RIGHT_MULTY_TEAM_TOP_SCORE;				//@

	CBasicTextBox*				TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_RANK;		//@
	CBasicTextBox*				TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_NAME;		//@
	CBasicTextBox*				TOURNAMENT_RIGHT_MULTY_TEAM_MYTEAM_TOP_SCORE;		//@
	CBasicTextBox*				TOURNAMENT_RIGHT_MULTY_TEAM_RESUALT_TB;		//@

	/**
        외부 인터페이스

     */
    GLGaeaClient*       m_pGaeaClient;
};