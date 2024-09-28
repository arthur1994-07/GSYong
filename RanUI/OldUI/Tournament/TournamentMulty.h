
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
			���� Ŭ�� UI�� �����Ѵ�.
			ȭ�� �߾ӿ� �ֻ��� ��� �ڽ��� ���� �Ѵ�.
			
			\return ���� ���� ����
		 */
virtual bool				CreateSubControl();

		/**
			��ư Ŭ��, Ÿ�� ���� ���� �޽����� ó�� �Ѵ�.

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
        �ܺ� �������̽�

     */
    GLGaeaClient*       m_pGaeaClient;
};