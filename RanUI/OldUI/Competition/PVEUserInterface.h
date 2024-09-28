#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

class EngineDeviceMan;
class CPVEResult;
class CPVEEntranceConfirm;
class CPVERetry;
class CPVEEntranceState;
class CPVEBoard;
class CPVEIndicator;

/**
	PVE�� ���¸� ��� �ϴ� UI �� ���� �Ѵ�.
	���� UI ���� ���������� �̷��� �ִ�.
	�ϳ��� ������ ������ ���� ������ ȿ���� ������.

 */
class CPVEUserInterface
	: public CUIGroup
{
public:
		CPVEUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual ~CPVEUserInterface();

		/**
			���� ���¸� ��� �Ѵ�.

			\param nCur ���� ���� ��Ƽ����
			\param nAll ��ü �����ؾ��� ��Ƽ����, 0 �� ������ ������ ��ü ��Ƽ���� ���
            \param nSec ���� �ð�
			\param bShow ȭ�鿡 Visible ����
		 */
		void				SetEntranceState(int nCur, int nAll, int nSec, bool bShow);

		/**
			�������� ���� ǥ��(��� ������ �ʰ� ��� ���̴� UI)
			�������� ǥ��, �������� �޽��� ó��

		 */
		void				SetStage(int nStage, bool bShow);
		void				SetMessage(const TCHAR* pText, bool bShow);

		/**
			�������� ���� ���� ǥ��(�������� ���� ���� ��� ǥ�� �Ǵ� UI)
			�������� ���� �ð�(�ʴ���), �������� �ѹ�, ���� ����, �̵� ����Ʈ ��

		 */
		void				SetStageTimeIndicator(int nSec, bool bBlank, bool bShow);
		void				SetStageIndicator(int nStage, int nEnd, int nRange, bool bShow);
		void				SetMonsterIndicator(int nRem, int nAll, bool bShow);
        void				SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow);

		/**
			��� ���� ǥ�� ó��
			����, ���� ���, ���� �� ���� �Ѵ�.

		 */
        void                SetResult(int nResult, bool bShow);

		/**
			Ȯ�� �޽���â ó��
			�Լ��� �� ȣ�� Ŭ���� �Լ��� ���� �Ѵ�.

		 */
		void				SetRetry(LONGLONG nMoney, int nTime, bool bShow);
		void				SetEntrance(bool bShow);
		void				SetNewInstanceRetry(LONGLONG nMoney, int nTime, bool bShow, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID);

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
			PVE ������ ���� UI

		 */
		CPVEEntranceState*	m_pEntranceState;	//@ ���� ���� UI ó����
		CPVEBoard*			m_pBoard;			//@ ���������� �޽��� ǥ�ñ�
		CPVEIndicator*		m_pIndicator;		//@ �������� ���� ǥ�ñ�
		CPVEEntranceConfirm*m_pEntranceConfirm;	//@ ���� Ȯ�� �޽��� �ڽ�
		CPVERetry*			m_pRetry;			//@ �絵�� Ȯ�� �޽��� �ڽ�
		CPVEResult*			m_pResult;			//@ ���(����) ������

		/**
			�ܺ� �ǵ��� Ŭ������

		 */
		GLGaeaClient*		m_pGaeaClient;
		CInnerInterface*	m_pInterface;

        /**
            Visible ����

         */
        bool             	m_bVisibleEntranceState;
        bool             	m_bVisibleStage;
        bool             	m_bVisibleMessage;
        bool             	m_bVisibleResult;
        bool             	m_bVisibleStageTimeIndicator;
        bool             	m_bVisibleStageIndicator;
        bool             	m_bVisibleMonsterIndicator;
        bool             	m_bVisiblePositioningIndicator;
		bool				m_bVisibleRetry;
};

class MyPVEUserInterface
	: public IPVEUserInterface
	, private CPVEUserInterface
{
public:
		MyPVEUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
virtual void CreateUIWindowAndRegisterOwnership();
virtual void ToggleShow(bool bShow);
virtual void SetEntranceConfirm(bool bShow);
virtual void SetEntranceState(int nCur, int nAll, int nSec, bool bShow);
virtual void SetStage(int nStage, bool bShow);
virtual void SetMessage(const TCHAR* pText, bool bShow);
virtual void SetMonsterIndicator(int nCur, int nAll, bool bShow);
virtual void SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow);
virtual void SetTimeIndicator( int nSec, bool bShow );
virtual void SetStageIndicator(int nStage, int nEnd, int nRange, int nSec, bool bBlank, bool bShow);
virtual void SetStageState(int nState, bool bShow);
virtual void SetRetry(LONGLONG nMoney, int nTime, bool bShow);
virtual void SetStageResult();
virtual void SetNewInstanceRetry(LONGLONG nMoney, int nTime, bool bShow, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID);
virtual void AllClose();
};
