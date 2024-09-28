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
	PVE�� ���¸� ��� �ϴ� UI �� ���� �Ѵ�.
	���� UI ���� ���������� �̷��� �ִ�.
	�ϳ��� ������ ������ ���� ������ ȿ���� ������.

 */
class CTournamentUserInterface
	: public CUIGroup
{
public:
		CTournamentUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CTournamentUserInterface();


		/**
			������Ʈ ����.
		 */
		virtual				void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );



		/**
			�������� ���� ���� ǥ��(�������� ���� ���� ��� ǥ�� �Ǵ� UI)
			�������� ���� �ð�(�ʴ���), �������� �ѹ�, ���� ����, �̵� ����Ʈ ��

		 */
		void				SetStageTimeIndicator(int nSec, bool bBlank, bool bShow);
		void				SetStageIndicator(int nStage, int nEnd, int nRange, bool bShow);
		void				SetMonsterIndicator(int nRem, int nAll, bool bShow);
        void				SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow);


		/**
			�������� ����� ����Ѵ�.

		 */
		void				SetStageResult();

		/**
			������ ������ ���� �Ѵ�.
			�츮�� �������� ���� �� ���� ��� UI �� ������ �ʰ� ó�� �� �ش�.

		 */
        void                SetVisibleSingle(BOOL bVisible);

protected:
		/**
			����� ��Ʈ�ѵ��� ���� �Ѵ�.
			
		 */
		bool				CreateSubControl();

		/**
			Tournament ������ ���� UI

		 */
		

		float						m_fElapsedTime;		// �ð� ����!

		

		/**
			�ܺ� �ǵ��� Ŭ������

		 */
		GLGaeaClient*		m_pGaeaClient;
		CInnerInterface*	m_pInterface;

        /**
            Visible ����

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
