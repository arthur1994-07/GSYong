#pragma once

#include "../Competition/CompetitionPage.h"
#include "../Util/UIPage.h"
#include "../../../RanLogic/GLCompetition.h"
#include "../../../RanLogic/Character/GLCharDefine.h"
#include "../../../RanLogic/PVE/MapTable.h"
#include "../../../RanLogicClient/TriggerSystem/ClientMapInfoMessage.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"

class CUIListBox;
class CSwapImage;
class GLGaeaClient;
class CBasicTextBox;
class CBasicTextButton;
class CBasicTextBoxEx;
class CBasicScrollBarEx;
class CInnerInterface;
class EngineDeviceMan;
class GLPVEClient;

typedef TriggerSystem::SClientTriggerMapInfo TriggerMapInfo;
typedef TriggerSystem::TriggerMapInfoList TriggerMapInfoList;

/**
	전장 UI 의 PVE 페이지를 처리 한다.
	전장 UI는 게임 접속 후 'U' 키를 누르면 뜬다.
	
 */

using namespace MatchingSystem;

class CTournamentPage
	: public CCompetitionPage
{
public:
		CTournamentPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CTournamentPage();

		enum
		{
			CONTROL_ID_NONE = PAGE_NEXT,
			CONTROL_ID_BUTTON_ENTRANCE,
			CONTROL_ID_BUTTON_MOREINFO,
            CONTROL_ID_LISTBOX_INSTANCEMAP,
		};

        void                        Init();
virtual void	                    CreateSubControl();
virtual void				        Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);
virtual	void				        TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);

		void				        Update();

		void				        OnEntrance();
		void				        OnClearOfBelonging();


virtual void						OnPageVisible(bool _isOpen);

        /**
        	페이지가 활성화 될때 호출 된다.
            우리는 활성화 될 때 최신 정보로 갱신 한다.

         */
virtual void				        BeginSubControl() { Update(); }

protected:
        /**
        	PVE 인던 리스트 박스의 인던을 선택 한다.

         */
        void                        SetCurSel(int nIndex);

        /**
            페이지 정보를 설정 한다.

         */
        void				        SetTitle(const TCHAR* pTitle);
        void				        SetMapInfo(const TCHAR* pMapInfo);
		void						AddMapInfo(const TCHAR* pMapInfo);
		void				        SetMapImage(SNATIVEID mid);
        void				        SetReward(const TCHAR* pReward);

        /**
            PVE 인던 리스트 박스의 아이템을 추가 한다.

         */
        void				        SetTournamentMap(const TCHAR* pTitle, DWORD TornamentType, DWORD NowStatus,int nIndex = -1);

        /**
            현재 리스트박스 에서 선택된 맵 정보를 추출 한다.

         */
        bool                        GetSelCurInfo(GLMatchingInfo*& info);

        /**
        	내부 자료 구조를 외부와 동기화 한다.

         */
        void                        UpdateSelCur();
        void                        UpdateListBox();

        /**
            외부 인터페이스

         */
		CInnerInterface*	        m_pInterface;
		GLGaeaClient*               m_pGaeaClient;

        /**
            UI 

         */
		CBasicTextButton*	        m_pEntranceButton;
		CBasicTextButton*	        m_pClearOfBelongingButton;
		CBasicTextBox*		        m_pText_Title;
		CBasicTextBox*		        m_pText_Info;
		CBasicTextBox*		        m_pListbox_Title;
		CUIListBox*			        m_pListbox;
		CSwapImage*			        m_pMap;

		/**
			토너먼트 로직 처리기
		 */
		GLTouranmentClient*				m_pLogic;
};
