#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/TriggerSystem/ClientMapInfoMessage.h"

#include "PVEUserInterface.h"
#include "PVEEntranceConfirm.h"
#include "PVERetry.h"
#include "PVEEntranceState.h"
#include "PVEBoard.h"
#include "PVEIndicator.h"
#include "PVEResult.h"
#include "PVEPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPVEUserInterface::CPVEUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
	CUIGroup(pEngineDevice)
,	m_pGaeaClient(pGaeaClient)
,	m_pInterface(pInterface)
,	m_pEntranceState(0)
,	m_pBoard(0)
,	m_pIndicator(0)
,	m_pEntranceConfirm(0)
,	m_pRetry(0)
,	m_pResult(0)
,   m_bVisibleEntranceState(false)
,   m_bVisibleStage(false)
,   m_bVisibleMessage(false)
,   m_bVisibleResult(false)
,   m_bVisibleStageTimeIndicator(false)
,   m_bVisibleStageIndicator(false)
,   m_bVisibleMonsterIndicator(false)
,   m_bVisiblePositioningIndicator(false)
,	m_bVisibleRetry(false)
{
	
}

CPVEUserInterface::~CPVEUserInterface()
{

}

bool CPVEUserInterface::CreateSubControl()
{
	if (m_pEntranceState == 0)
	{
		m_pEntranceState = new CPVEEntranceState(m_pEngineDevice);

		if (m_pEntranceState == 0)
		{
			return false;
		}
		
		m_pEntranceState->Create ( WAIL_ENTRANCE_STATE, "CONTROL_PVE_ENTRANCESTATE", UI_FLAG_XRESRATE | UI_FLAG_YRESRATE );
		m_pEntranceState->CreateSubControl();
		m_pInterface->UiRegisterControl ( m_pEntranceState, false );
		m_pInterface->UiShowGroupFocus ( WAIL_ENTRANCE_STATE );
		m_pInterface->UiHideGroup( WAIL_ENTRANCE_STATE );
		m_pInterface->AddNonCloseUI(WAIL_ENTRANCE_STATE);
	}

	if (m_pBoard == 0)
	{
		m_pBoard = new CPVEBoard(m_pEngineDevice);

		if (m_pBoard == 0)
		{
			return false;
		}
		m_pBoard->Create ( WAIL_BOARD, "CONTROL_PVE_BOARD", UI_FLAG_XRESRATE | UI_FLAG_YRESRATE );
		m_pBoard->CreateSubControl();
		m_pBoard->SetVisibleSingle(FALSE);
		m_pInterface->UiRegisterControl ( m_pBoard, false );
		m_pInterface->UiShowGroupFocus ( WAIL_BOARD );
		m_pInterface->UiHideGroup( WAIL_BOARD );
		m_pInterface->AddNonCloseUI(WAIL_BOARD);
	}

	if (m_pIndicator == 0)
	{
		m_pIndicator = new CPVEIndicator(m_pEngineDevice);

		if (m_pIndicator == 0)
		{
			return false;
		}
		m_pIndicator->Create ( WAIL_INDICATOR, "CONTROL_PVE_INDICATOR", UI_FLAG_XRESRATE | UI_FLAG_YRESRATE );
		m_pIndicator->CreateSubControl();
		m_pIndicator->SetAlignFlag( UI_FLAG_CENTER_X );
		m_pInterface->UiRegisterControl ( m_pIndicator, false );
		m_pInterface->UiShowGroupFocus ( WAIL_INDICATOR );
		m_pInterface->UiHideGroup( WAIL_INDICATOR );
		m_pInterface->AddNonCloseUI(WAIL_INDICATOR);
	}

	if (m_pEntranceConfirm == 0)
	{
		m_pEntranceConfirm = new CPVEEntranceConfirm(m_pGaeaClient, m_pInterface, m_pEngineDevice);

        if (m_pEntranceConfirm == 0)
		{
			return false;
		}

        m_pEntranceConfirm->Create(COMPETITION_PVEENTRANCECONFIRM, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        m_pEntranceConfirm->CreateBaseWindowBodyLightGray("COMPETITION_PVE_ENTRANCE_WINDOW");
        m_pEntranceConfirm->Create(COMPETITION_PVEENTRANCECONFIRM, "COMPETITION_PVE_ENTRANCE_WINDOW", UI_FLAG_XRESRATE | UI_FLAG_YRESRATE);
		m_pEntranceConfirm->CreateSubControl();
		//m_pEntranceConfirm->SetVisibleSingle(false);
		//m_pInterface->AddNonCloseUI(COMPETITION_PVEENTRANCECONFIRM);

        if (m_pInterface)
        {
            m_pInterface->UiRegisterControl(m_pEntranceConfirm);
        }
	}

	if (m_pRetry == 0)
	{
		m_pRetry = new CPVERetry(m_pGaeaClient, m_pInterface, m_pEngineDevice);

		if (m_pRetry == 0)
		{
			return false;
		}

        m_pRetry->Create(COMPETITION_PVERETRY, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        m_pRetry->CreateBaseWindowBodyLightGray("COMPETITION_PVE_RETRY_WINDOW");
        m_pRetry->Create(COMPETITION_PVERETRY, "COMPETITION_PVE_RETRY_WINDOW", UI_FLAG_XRESRATE | UI_FLAG_YRESRATE);
        m_pRetry->CreateSubControl();

        if (m_pInterface)
        {
            m_pInterface->UiRegisterControl(m_pRetry);
        }
	}

	if (m_pResult == 0)
	{
		m_pResult = new CPVEResult(m_pGaeaClient, m_pInterface, m_pEngineDevice);

		if (m_pResult == 0)
		{
			return false;
		}

        m_pResult->Create(COMPETITION_PVECLEAR, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE);
        m_pResult->CreateBaseWindowBodyLightGrayNoTitle("COMPETITION_PVE_CLEAR_WINDOW");
        m_pResult->Create(COMPETITION_PVECLEAR, "COMPETITION_PVE_CLEAR_WINDOW", UI_FLAG_XRESRATE | UI_FLAG_YRESRATE);
        m_pResult->CreateSubControl();
		m_pResult->SetVisibleSingle(FALSE);
		//m_pInterface->AddNonCloseUI(COMPETITION_PVECLEAR);

        if (m_pInterface)
        {
            m_pInterface->UiRegisterControl(m_pResult);
        }
	}

	return true;
}

void CPVEUserInterface::SetEntranceState(int nCur, int nAll, int nSec, bool bShow)
{
	if (m_pEntranceState)
	{
		m_pEntranceState->SetEntranceState(nCur, nAll, nSec);
        m_pEntranceState->SetVisibleSingle(bShow ? TRUE : FALSE);

        m_bVisibleEntranceState = bShow;
	}
}

void CPVEUserInterface::SetStage(int nStage, bool bShow)
{
	if (m_pBoard)
	{
        m_pBoard->SetOrdinalToUpBoard(nStage, bShow);
		
        m_bVisibleStage = bShow;
	}
}

void CPVEUserInterface::SetMessage(const TCHAR* pText, bool bShow)
{
	if (m_pBoard)
	{
        const TCHAR* pStr = "";

        if (pText && _tcsclen(pText) > 0)
        {
            pStr = ID2GAMEINTEXT(pText, 0);
        }

		m_pBoard->SetTextToDownBoard(pStr, NS_UITEXTCOLOR::AQUA, bShow);
		m_pBoard->SetVisibleSingle(bShow);

        m_bVisibleMessage = bShow;
	}
}

void CPVEUserInterface::SetStageTimeIndicator(int nSec, bool bBlank, bool bShow)
{
	if (m_pIndicator)
	{
		m_pIndicator->SetTime(nSec, bBlank, bShow);
		m_pIndicator->SetVisibleSingle(bShow);

        m_bVisibleStageTimeIndicator = bShow;
	}
}

void CPVEUserInterface::SetStageIndicator(int nStage, int nEnd, int nRange, bool bShow)
{
	if (m_pIndicator)
	{
		m_pIndicator->SetStage(nStage, nEnd, nRange, bShow);
		m_pIndicator->SetVisibleSingle(bShow);

        m_bVisibleStageIndicator = bShow;
	}
}

void CPVEUserInterface::SetMonsterIndicator(int nRem, int nAll, bool bShow)
{
	if (m_pIndicator)
	{
		m_pIndicator->SetMonster(nRem, nAll, bShow);

        m_bVisibleMonsterIndicator = bShow;
	}
}

void CPVEUserInterface::SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow)
{
    if (m_pIndicator)
    {
        m_pIndicator->SetPositioning(nX, nY, nNow, nNeed, bShow);

        m_bVisiblePositioningIndicator = bShow;
    }
}

void CPVEUserInterface::SetResult(int nResult, bool bShow)
{
    if (m_pBoard)
    {
        m_pBoard->SetResult(CPVEBoard::ERESULT_PVE(nResult), bShow);

        m_bVisibleResult = bShow;
    }
}

void CPVEUserInterface::SetRetry(LONGLONG nMoney, int nTime, bool bShow)
{
	if (m_pRetry)
	{
		m_pRetry->SetNewInstance( false, 0, SNATIVEID() );

		if (bShow)
		{
			m_pRetry->Open(nMoney, nTime);
		}
		else
		{
			m_pRetry->Close();
		}

		m_bVisibleRetry = bShow;
	}
}

void CPVEUserInterface::SetNewInstanceRetry (
	LONGLONG nMoney,
	int nTime,
	bool bShow,
	InstanceSystem::InstanceMapID nInstanceID,
	SNATIVEID sKeyMapID )
{
	if (m_pRetry)
	{
		m_pRetry->SetNewInstance( true, nInstanceID, sKeyMapID );

		if (bShow)
		{
			m_pRetry->Open(nMoney, nTime);
		}
		else
		{
			m_pRetry->Close();
		}

		m_bVisibleRetry = bShow;
	}
}

void CPVEUserInterface::SetEntrance(bool bShow)
{        
	if (m_pEntranceConfirm && m_pGaeaClient)
	{
        TriggerSystem::SClientTriggerMapInfo* pInfo = 0;

        if (m_pGaeaClient->GetPVEEntranceMapInfo(pInfo))
        {
            m_pEntranceConfirm->SetMapName(pInfo->mapname.c_str());
            m_pEntranceConfirm->SetTimer(pInfo->waitentrance);

            if (bShow)
            {
                m_pEntranceConfirm->Open();
            }
            else
            {
                m_pEntranceConfirm->Close();
            }
        }
	}
}

void CPVEUserInterface::SetStageResult()
{
	if (m_pResult)
	{
        m_pResult->SetStageResult();
	}
}

void CPVEUserInterface::SetVisibleSingle(BOOL bVisible)
{
    if (bVisible == false)
    {
        if (m_bVisibleEntranceState)
        {
            SetEntranceState(0, 0, 0, bVisible);
        }

        if (m_bVisibleStage)
        {
            SetStage(0, bVisible);
        }

        if (m_bVisibleMessage)
        {
            SetMessage(0, bVisible);
        }

        if (m_bVisibleResult)
        {
            SetResult(0, bVisible);
        }

        if (m_bVisibleStageTimeIndicator)
        {
            SetStageTimeIndicator(0, false, bVisible);
        }

        if (m_bVisibleStageIndicator)
        {
            SetStageIndicator(0, 0, 0, bVisible);
        }

        if (m_bVisibleMonsterIndicator)
        {
            SetMonsterIndicator(0, 0, bVisible);
        }

        if (m_bVisiblePositioningIndicator)
        {
            SetPositioningIndicator(0, 0, 0, 0, bVisible);
        }

		if (m_bVisibleRetry)
		{
			SetRetry(0, 0, bVisible);
			SetNewInstanceRetry(0, 0, bVisible, 0, SNATIVEID() );
		}
    }

    CUIGroup::SetVisibleSingle(bVisible);
}

MyPVEUserInterface::MyPVEUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
	CPVEUserInterface(pGaeaClient, pInterface, pEngineDevice)
{
    
}

void MyPVEUserInterface::CreateUIWindowAndRegisterOwnership()
{
	Create(COMPETITION_PVEUI, _T("COMPETITION_PVE_UI"), UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateSubControl();
	SetAlignFlag(UI_FLAG_CENTER_Y | UI_FLAG_RIGHT);
	m_pInterface->UiRegisterControl(this);
}

void MyPVEUserInterface::ToggleShow(bool bShow)
{
    if (m_pInterface)
    {
        if (bShow)
        {
            if (m_bVisibleEntranceState || m_bVisibleStage || m_bVisibleMessage || m_bVisibleResult || m_bVisibleRetry ||
                m_bVisibleStageTimeIndicator || m_bVisibleStageIndicator || m_bVisibleMonsterIndicator || m_bVisiblePositioningIndicator)
            {
                m_pInterface->UiShowGroupTop(COMPETITION_PVEUI);
            }
        }
        else
        {
            if (!m_bVisibleEntranceState && !m_bVisibleStage && !m_bVisibleMessage && !m_bVisibleResult && !m_bVisibleRetry && 
                !m_bVisibleStageTimeIndicator && !m_bVisibleStageIndicator && !m_bVisibleMonsterIndicator && !m_bVisiblePositioningIndicator)
            {
                m_pInterface->UiHideGroup(COMPETITION_PVEUI);
            }
        }
    }
}

void MyPVEUserInterface::SetEntranceConfirm(bool bShow)
{
    SetEntrance(bShow);
}

void MyPVEUserInterface::SetEntranceState(int nCur, int nAll, int nSec, bool bShow)
{
    CPVEUserInterface::SetEntranceState(nCur, nAll, nSec, bShow);
    ToggleShow(bShow);
}

void MyPVEUserInterface::SetStage(int nStage, bool bShow)
{
    CPVEUserInterface::SetStage(nStage, bShow);
    ToggleShow(bShow);
}

void MyPVEUserInterface::SetMessage(const TCHAR* pText, bool bShow)
{    
    CPVEUserInterface::SetMessage(pText, bShow);
    ToggleShow(bShow);
}

void MyPVEUserInterface::SetMonsterIndicator(int nCur, int nAll, bool bShow)
{
    CPVEUserInterface::SetMonsterIndicator(nCur, nAll, bShow);
    ToggleShow(bShow);
}

void MyPVEUserInterface::SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow)
{
    CPVEUserInterface::SetPositioningIndicator(nX, nY, nNow, nNeed, bShow);
    ToggleShow(bShow);
}

void MyPVEUserInterface::SetTimeIndicator(int nSec, bool bShow)
{
	CPVEUserInterface::SetStageTimeIndicator(nSec, false, bShow);
	ToggleShow(bShow);
}

void MyPVEUserInterface::SetStageIndicator(int nStage, int nEnd, int nRange, int nSec, bool bBlank, bool bShow)
{
    CPVEUserInterface::SetStageTimeIndicator(nSec, bBlank, bShow);
    CPVEUserInterface::SetStageIndicator(nStage, nEnd, nRange, bShow);
    ToggleShow(bShow);
}

void MyPVEUserInterface::SetStageResult()
{
	AllClose();
    CPVEUserInterface::SetStageResult();
}

void MyPVEUserInterface::SetStageState(int nState, bool bShow)
{
    switch (nState)
    {
    case GLMSG::STAGE_STATE_STAGECLEAR:
    case GLMSG::STAGE_STATE_GAMEROVER_COMPLETED:
		AllClose();
        CPVEUserInterface::SetResult(CPVEBoard::ERESULT_SUCCEED, bShow);
        break;

    case GLMSG::STAGE_STATE_DIED:
        CPVEUserInterface::SetResult(CPVEBoard::ERESULT_WAITAFTERDEATH, bShow);
        break;

    case GLMSG::STAGE_STATE_ONGOING:
	case GLMSG::STAGE_STATE_RETRY:		
        break;

    default:
		AllClose();
        CPVEUserInterface::SetResult(CPVEBoard::ERESULT_FAILED, bShow);
        break;
    }

    ToggleShow(bShow);
}

void MyPVEUserInterface::SetRetry(LONGLONG nMoney, int nTime, bool bShow)
{
    CPVEUserInterface::SetRetry(nMoney, nTime, bShow);
}

void MyPVEUserInterface::SetNewInstanceRetry(LONGLONG nMoney, int nTime, bool bShow, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID)
{
	CPVEUserInterface::SetNewInstanceRetry(nMoney, nTime, bShow, nInstanceID, sKeyMapID);
}

void MyPVEUserInterface::AllClose()
{
	CPVEUserInterface::SetVisibleSingle(FALSE);
	CPVEUserInterface::SetVisibleSingle(TRUE);
}
