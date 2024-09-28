#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"
#include "../../../RanLogic/MatchSystem/MatchModel.h"

class EngineDeviceMan;
class CPVEResult;
class CPVEEntranceConfirm;
class CPVERetry;
class CPVEEntranceState;
class CPVEBoard;
class CPVEIndicator;
class CTournamentIndicator;
class CTournamentEntranceConfirm;
class CTournamentMulty;
class CTournamentMiniExpeditionInfoGroup;
class TournamentObserveWindow;


#include "../Competition/PVEUserInterface.h"

/**
	PVE의 상태를 출력 하는 UI 를 정의 한다.
	여러 UI 들이 복합적으로 이뤄져 있다.
	하나의 컨텐츠 단위로 묶어 관리의 효율을 높였다.

 */
class CTournamentUserInterface
	: public CUIGroup
{
public:
		CTournamentUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CTournamentUserInterface();


		/**
			업데이트 관리.
		 */
		virtual				void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );



		/**
			스테이지 진행 상태 표시(스테이지 유지 동안 계속 표시 되는 UI)
			스테이지 진행 시간(초단위), 스테이지 넘버, 잡은 몬스터, 이동 포인트 순

		 */
		void				SetStageTimeIndicator(int nSec, bool bBlank, bool bShow);
		void				SetStageIndicator(int nStage, int nEnd, int nRange, bool bShow);
		void				SetMonsterIndicator(int nRem, int nAll, bool bShow);
        void				SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow);


		/**
			스테이지 결과를 출력한다.

		 */
		void				SetStageResult();

		/**
			보일지 유무를 설정 한다.
			우리는 보여지지 않을 때 관련 모든 UI 를 보이지 않게 처리 해 준다.

		 */
        void                SetVisibleSingle(BOOL bVisible);

protected:
		/**
			사용할 컨트롤들을 생성 한다.
			
		 */
		bool				CreateSubControl();

		/**
			Tournament 컨텐츠 관련 UI

		 */
		

		float						m_fElapsedTime;		// 시간 저장!

		

		/**
			외부 피드백용 클래스들

		 */
		GLGaeaClient*		m_pGaeaClient;
		CInnerInterface*	m_pInterface;

        /**
            Visible 변수

         */
        bool             	m_bVisibleStageTimeIndicator;
        bool             	m_bVisibleStageIndicator;
        bool             	m_bVisibleMonsterIndicator;
		DWORD m_TeamFactionSize;
};

class MyTournamentUserInterface
	: public IPTournamentUserInterface
	, private CTournamentUserInterface
{
public:
		MyTournamentUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual void CreateUIWindowAndRegisterOwnership();
virtual void ToggleShow(bool bShow);
virtual void SetEntranceConfirm(bool bShow);
virtual void SetEntranceConfirm(bool bShow,int waitentrance);
virtual void SetMonsterIndicator(int nCur, int nAll, bool bShow);
virtual void SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow);
virtual void SetStageIndicator(int nSec, bool bBlank, bool bShow);
virtual void SetStageState(int nState, bool bShow);
virtual void AllClose();
virtual void SetCloseAll();

};
