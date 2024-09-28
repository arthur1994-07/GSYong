#include "stdafx.h"

#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../../EngineLib/GUInterface/UIRenderQueue.h"
#include "../../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../../EngineLib/DxTools/d3dfont.h"
#include "../../../../EngineLib/DeviceMan.h"

#include "../../../../EngineLib/DxSound/DxSoundLib.h"
#include "../../../../EngineLib/DxMeshs/DxSimpleMeshMan.h"
#include "../../../../EngineLib/DxTools/DxBackUpRendTarget.h"

#include "../../../../RanLogic/GLogicDataMan.h"
#include "../../../../RanLogic/Msg/GLContrlMiniGameMsg.h"
#include "../../../../RanLogic/MiniGame/OddEven/GLOddEvenSettings.h"

#include "../../../../RanLogicClient/MiniGame/OddEven/GLOddEven.h"
#include "../../../../RanLogicClient/MiniGame/GLMiniGameManager.h"
#include "../../../../RanLogicClient/MiniGame/GLMiniGameFactory.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"
#include "../../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../InnerInterface.h"
#include "../../Util/RnButton.h"
#include "../../Util/RnProgressBar.h"
#include "../../Item/ItemImage.h"
#include "../ColorProperty/ColorProperty.h"

#include "OddEvenWindow.h"

OddEvenWindow::OddEvenWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: CUIWindowEx( pInterface, pEngineDevice )
, m_pInterface(pInterface)
, m_pGaeaClient(pGaeaClient)
, m_pMiniGameManager(pGaeaClient->GetMiniGameManager())
, m_pTargetSurface(NULL)
, m_pTargetZBuffer(NULL)
, m_pTargetTexture(NULL)
, m_pDice(NULL)
, m_nRequestRewardCount(0)
, m_fCurrentTime(0.0f)
, m_fLastRequestTime(0.0f)
, m_fResultElapsedTime(0.0f)
, m_fSelectRewardAnimateElapsedTime(0.0f)
, m_bLock(false)
{		
}

OddEvenWindow::~OddEvenWindow(void)
{
	MiniGame::destroyDice(m_pDice);
	SAFE_RELEASE(m_pTargetSurface);
	SAFE_RELEASE(m_pTargetZBuffer);
	SAFE_RELEASE(m_pTargetTexture);
}

void OddEvenWindow::_updateButtons(const MiniGame::GLOddEvenClient* const pGame)
{
	const MiniGame::EMODDEVEN_STAGE emCurrentStage = pGame->getCurrentStage();	

	m_pBeginButton->SetVisibleSingle(FALSE);
//	m_pRollButton->SetVisibleSingle(FALSE);
	m_pExitButton->SetVisibleSingle(FALSE);
	m_pRebeginButton->SetVisibleSingle(FALSE);
	m_pReceiveButton->SetVisibleSingle(FALSE);

	m_pOddButton->SetVisibleSingle(FALSE);	
	m_pEvenButton->SetVisibleSingle(FALSE);

	for ( DWORD _i = 0; _i < ODDEVEN_REWARD_NSIZE; ++_i )
	{
		m_pRewardCloseBox[_i]->SetVisibleSingle(FALSE);
		m_pRewardOpenBox[_i][0]->SetVisibleSingle(FALSE);
		m_pRewardOpenBox[_i][1]->SetVisibleSingle(FALSE);
		m_pRewardOpenSelectBox[_i]->SetVisibleSingle(FALSE);
	}	

	MiniGame::OddEvenSettings* const pOddEven = MiniGame::OddEvenSettings::getInstance();
	switch ( emCurrentStage )
	{
	case MiniGame::EMODDEVEN_STAGE_READY:
		{
			const DWORD nMaxWin = pOddEven->getMaxWin();
			const DWORD nCurrentStep = pGame->getCurrentStep();
			if ( nCurrentStep == 0 )
				m_pBeginButton->SetVisibleSingle(TRUE);
			else if ( nCurrentStep == nMaxWin )
				m_pExitButton->SetVisibleSingle(TRUE);
			else
				m_pRebeginButton->SetVisibleSingle(TRUE);

			m_pEvenButton->SetFlip(FALSE);			
			m_pOddButton->SetFlip(FALSE);
		}
		break;
	case MiniGame::EMODDEVEN_STAGE_SELECT:
		{			
//			m_pRollButton->SetVisibleSingle(FALSE);
			m_pOddButton->SetVisibleSingle(TRUE);
			m_pEvenButton->SetVisibleSingle(TRUE);
		}
		break;	
	case MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD:
		{
			const float fAnimateTime(1.0f / pOddEven->getAnimateTime());
			const float fIdleTime(pOddEven->getAnimateIdleTime() * fAnimateTime);
			if ( pGame->getSelected() == pGame->getResult() )
			{
				if ( m_fSelectRewardAnimateElapsedTime > (fAnimateTime * 2.0f + fIdleTime) )
					m_pReceiveButton->SetVisibleSingle(TRUE);

				for ( DWORD _i = 1; _i < ODDEVEN_REWARD_NSIZE; ++_i )
				{
					if ( (pGame->getSelectItemIndex() + 1) == _i )
						m_pRewardOpenSelectBox[_i]->SetVisibleSingle(TRUE);
					else
						m_pRewardOpenBox[_i][1]->SetVisibleSingle(TRUE);
				}
			}
			else
			{
				if ( m_fSelectRewardAnimateElapsedTime > fAnimateTime )
					m_pReceiveButton->SetVisibleSingle(TRUE);

				m_pRewardOpenBox[0][1]->SetVisibleSingle(TRUE);
			}
		}
		
		break;
	case MiniGame::EMODDEVEN_STAGE_SELECT_REWARD:
		if ( pGame->getSelected() == pGame->getResult() )
		{
			for ( DWORD _i = 1; _i < ODDEVEN_REWARD_NSIZE; ++_i )
				m_pRewardCloseBox[_i]->SetVisibleSingle(TRUE);
		}
		else
			m_pRewardCloseBox[0]->SetVisibleSingle(TRUE);		
		break;
	case MiniGame::EMODDEVEN_STAGE_ROLL:
	case MiniGame::EMODDEVEN_STAGE_RESULT:		
		break;
	}	
}

void OddEvenWindow::_updateLabel(const MiniGame::GLOddEvenClient* const pGame)
{
	const MiniGame::EMODDEVEN_STAGE emCurrentStage = pGame->getCurrentStage();
	for ( DWORD _i = 0; _i < ODDEVEN_LABEL_NSIZE; ++_i )
		m_pLabel[_i]->SetVisibleSingle(FALSE);	

	switch ( emCurrentStage )
	{
		case MiniGame::EMODDEVEN_STAGE_READY:			
			m_pLabel[ODDEVEN_YESNO_LABEL]->SetVisibleSingle(TRUE);			
			break;
		case MiniGame::EMODDEVEN_STAGE_SELECT:
			{
				const BOOL bSelected = pGame->getSelected() != MiniGame::EMODDEVEN_NOT_SELECTED;
				m_pLabel[ODDEVEN_SELECT_LABEL]->SetVisibleSingle(!bSelected);
				m_pLabel[ODDEVEN_READY_LABEL]->SetVisibleSingle(bSelected);
			}			
			break;
		case MiniGame::EMODDEVEN_STAGE_RESULT:
			if ( pGame->getResult() == pGame->getSelected() )
				m_pLabel[ODDEVEN_WIN_LABEL]->SetVisibleSingle(TRUE);
			else
				m_pLabel[ODDEVEN_LOSE_LABEL]->SetVisibleSingle(TRUE);			
			break;
		case MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD:
		case MiniGame::EMODDEVEN_STAGE_SELECT_REWARD:
			m_pLabel[ODDEVEN_REWARD_LABEL]->SetVisibleSingle(TRUE);
			break;
	}
}

void OddEvenWindow::_updateGuideText(const MiniGame::GLOddEvenClient* const pGame)
{
	const LONGLONG& lnMoney = m_pGaeaClient->GetCharacter()->GetInvenMoney();
	CString strMoney = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(lnMoney, 3, "," );
	m_pMoneyText->SetOneLineText( strMoney );

	MiniGame::OddEvenSettings* const pOddEven = MiniGame::OddEvenSettings::getInstance();
	const MiniGame::EMODDEVEN_STAGE emCurrentStage = pGame->getCurrentStage();		
	const DWORD nStep = pGame->getCurrentStep();	
	const DWORD nMaxWin = pOddEven->getMaxWin();

	m_pProcessText[0]->ClearText();
	m_pProcessText[1]->ClearText();
	m_pProcessText[2]->ClearText();	
	m_pProcessText[3]->ClearText();
	m_pGuideText->ClearText();
	m_pInformationText->ClearText();

	CString strWin;	
	CString strMaxWin;
	strWin.Format((char*)ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 10), nStep);	
	strMaxWin.Format((char*)ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 9), nMaxWin);	
	m_pInformationText->AddText(strWin, NS_UITEXTCOLOR::GOLD);
	m_pInformationText->AddString(0, strMaxWin);	
	m_pProgressText->SetVisibleSingle(FALSE);

	DWORD dwColor[4] = 
	{
		NS_UITEXTCOLOR::WHITE,
		NS_UITEXTCOLOR::WHITE,
		NS_UITEXTCOLOR::WHITE,
		NS_UITEXTCOLOR::WHITE
	};
	CString strGuide;
	switch ( emCurrentStage )
	{
	case MiniGame::EMODDEVEN_STAGE_READY:
		{	
			if ( nStep == 0 )
				strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_BEGIN"));
			else if ( nStep == nMaxWin )
			{
				strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_END"));
				m_pGuideText->AddText(strGuide);
				break;
			}
			else
				strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_REBEGIN"));

			m_pGuideText->AddText(strGuide);

			const LONGLONG lnCostMoney(pOddEven->getCostMoney(nStep));
			strGuide = NS_UITEXTCONTROL::MAKE_MONEY_FORMAT(lnCostMoney, 3, "," );
			m_pGuideText->AddString(0, strGuide, NS_UITEXTCOLOR::TOMATO);

			if ( lnCostMoney > lnMoney )
			{
				m_pMoneyText->ClearText();
				m_pMoneyText->SetOneLineText( strMoney, NS_UITEXTCOLOR::DISABLE );
			}			

			strGuide.Format((char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_BEGIN_END"));
			m_pGuideText->AddString(0, strGuide);
			dwColor[0] = NS_UITEXTCOLOR::GOLD;
		}			
		break;
	case MiniGame::EMODDEVEN_STAGE_SELECT:
		if ( pGame->getSelected() == MiniGame::EMODDEVEN_NOT_SELECTED )
			strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_SELECT_ODDEVEN"));
		else
			strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_ROLL"));
		m_pGuideText->AddText(strGuide);
		dwColor[1] = NS_UITEXTCOLOR::GOLD;
		break;
	case MiniGame::EMODDEVEN_STAGE_ROLL:		
		strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_ROLLING"));
		m_pGuideText->AddText(strGuide);
		dwColor[2] = NS_UITEXTCOLOR::GOLD;
		break;
	case MiniGame::EMODDEVEN_STAGE_RESULT:
		if ( pGame->getSelected() == pGame->getResult() )
			strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_RESULT_WIN"));
		else
			strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_RESULT_LOSE"));
		m_pGuideText->AddText(strGuide);
		dwColor[2] = NS_UITEXTCOLOR::GOLD;
		break;
	case MiniGame::EMODDEVEN_STAGE_SELECT_REWARD:
	case MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD:
		if ( pGame->getSelected() == pGame->getResult() )
			strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_REWARD_WIN"), pGame->getCurrentStep() + 1);
		else
			strGuide.Format( (char*)ID2GAMEINTEXT("MINIGAME_ODDEVEN_PROCESS_REWARD_LOSE"));
		m_pGuideText->AddText(strGuide);
		dwColor[3] = NS_UITEXTCOLOR::GOLD;
		break;
	}	

	m_pProcessText[0]->AddText(ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 11), dwColor[0]);
	m_pProcessText[1]->AddText(ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 12), dwColor[1]);
	m_pProcessText[2]->AddText(ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 3), dwColor[2]);
	m_pProcessText[3]->AddText(ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 13), dwColor[3]);
	
	switch ( pGame->getSelected() )
	{
	case MiniGame::EMODDEVEN_ODD:
		
		m_pProcessText[1]->AddString(0, ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 7), dwColor[1]);
		break;
	case MiniGame::EMODDEVEN_EVEN:		
		m_pProcessText[1]->AddString(0, ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 8), dwColor[1]);
		break;
	default:
		break;
	}		
}

void OddEvenWindow::_updateStage(const MiniGame::GLOddEvenClient* const pGame, const float fElapsedTime)
{
	MiniGame::OddEvenSettings* const pOddEven = MiniGame::OddEvenSettings::getInstance();
	const MiniGame::EMODDEVEN_STAGE emCurrentStage = pGame->getCurrentStage();

	for ( DWORD _i = 0; _i < ODDEVEN_REWARD_NSIZE; ++_i )
		m_pItemImage[_i]->SetVisibleSingle(FALSE);
	m_pProgressBar->SetVisibleSingle(FALSE);

	m_bLock = true;
	switch ( emCurrentStage )
	{
	case MiniGame::EMODDEVEN_STAGE_READY:
		m_bLock = false;
		m_pDice->reset();
		break;
	case MiniGame::EMODDEVEN_STAGE_SELECT:
		m_fResultElapsedTime = 0.0f;
		m_fSelectRewardAnimateElapsedTime = 0.0f;
		m_fSelectRewardElapsedTime = 0.0f;
		m_nRequestRewardCount = 0;
		m_bSelectReward = false;
		break;
	case MiniGame::EMODDEVEN_STAGE_ROLL:
		if ( m_pDice->update(0.0f, fElapsedTime) != MiniGame::EMDICE_GAME_STATE_FINISH )
			break;

		m_pDice->lock();
		m_pMiniGameManager->doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RESULT());
		break;
	case MiniGame::EMODDEVEN_STAGE_RESULT:		
		// 3초간 1번씩 3번 요청해도 응답이 없으면 문제가 생긴것으로 하고 종료 시킴;
		m_fResultElapsedTime += fElapsedTime;		
		if ( m_fResultElapsedTime > 3.0f && m_nRequestRewardCount < 4 )
		{
			m_fResultElapsedTime = 0.0f;
			m_pMiniGameManager->doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_REWARD());
			++m_nRequestRewardCount;
			for ( DWORD _i = 0; _i < ODDEVEN_REWARD_NSIZE; ++_i )
				m_pItemImage[_i]->SetAnimatePosition(10.0f, 30.0f, 0.1f, 0.1f);
		}
		if ( m_nRequestRewardCount == 4 )
			m_pMiniGameManager->RequestClose();
		break;
	case MiniGame::EMODDEVEN_STAGE_SELECT_REWARD:
		{
			m_pProgressBar->SetVisibleSingle(TRUE);
			const int nMaxTime = int(pOddEven->getTimeLimit() * 10.0f);
			const int nElapsedTime = nMaxTime - int(m_fSelectRewardElapsedTime * 10.0f);
			if ( nElapsedTime < 0 )
			{
				_translateUIMessageRewardButton(0);
				m_pProgressBar->SetMax(100);
				m_pProgressBar->SetNow(0);
			}
			else
			{
				m_pProgressBar->SetMax(nMaxTime);
				m_pProgressBar->SetNow(nElapsedTime);
				m_fSelectRewardElapsedTime += fElapsedTime;		
			}		
		}
		break;
	case MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD:
		m_fSelectRewardAnimateElapsedTime += fElapsedTime;

		const DWORD nSelectItemIndex = pGame->getSelectItemIndex();
		if ( nSelectItemIndex >= MiniGame::EMODDEVEN_MAX_REWARD )
		{
			m_pMiniGameManager->RequestClose();
			break;
		}

		{			
			const float fT(m_fSelectRewardAnimateElapsedTime * pOddEven->getAnimateTime());
			const bool bWin(pGame->getSelected() == pGame->getResult() ? true : false);
			const DWORD nImageIndex = bWin ? nSelectItemIndex + 1 : nSelectItemIndex;
			m_pItemImage[nImageIndex]->SetVisibleSingle(TRUE);
			m_pItemImage[nImageIndex]->SetItem(*pGame->getDetermineItemID(nSelectItemIndex));
			m_pItemImage[nImageIndex]->doAnimate(fT);
			
			const float fDelayTime(pOddEven->getAnimateIdleTime() + 1.0f);
			if ( bWin && fT > fDelayTime )
			{
				for ( DWORD _i = 1; _i < ODDEVEN_REWARD_NSIZE; ++_i )
				{
					if ( nImageIndex == _i )
						continue;
					m_pItemImage[_i]->SetVisibleSingle(TRUE);
					m_pItemImage[_i]->SetItem(*pGame->getDetermineItemID(_i - 1));
					m_pItemImage[_i]->doAnimate(fT - fDelayTime);
				}
			}
		}
		break;	
	}
}

void OddEvenWindow::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{	
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
	m_fCurrentTime += fElapsedTime;

	const MiniGame::GLOddEvenClient* const pGame = (const MiniGame::GLOddEvenClient* const)m_pMiniGameManager->GetMiniGame(MiniGame::EMMINIGAME_ODDEVEN);
	if ( pGame == NULL || m_pDice == NULL )
	{
		m_pMiniGameManager->RequestClose();
		return; // 닫기 요청 해야 함;
	}
	
	OddEvenWindow::_updateStage(pGame, fElapsedTime);
	OddEvenWindow::_updateButtons(pGame);
	OddEvenWindow::_updateLabel(pGame);
	OddEvenWindow::_updateGuideText(pGame);	
}


void OddEvenWindow::_translateUIMessageBeginButton(void)
{
	m_pMiniGameManager->doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_BEGIN());
}

void OddEvenWindow::_translateUIMessageRollButton(void)
{
	m_pMiniGameManager->doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_ROLL());
}

void OddEvenWindow::_translateUIMessageExitButton(void)
{
	m_pMiniGameManager->RequestClose();
}
void OddEvenWindow::_translateUIMessageOddButton(void)
{
	m_pMiniGameManager->doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN(MiniGame::EMODDEVEN_ODD));
}
void OddEvenWindow::_translateUIMessageEvenButton(void)
{	
	m_pMiniGameManager->doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN(MiniGame::EMODDEVEN_EVEN));
}

void OddEvenWindow::_translateUIMessageRewardButton(const DWORD nRewardIndex)
{
	if ( m_bSelectReward == true )
		return;
	m_bSelectReward = true;
	m_pMiniGameManager->doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD(nRewardIndex));
}

void OddEvenWindow::_translateUIMessageReceiveButton(void)
{
	m_pMiniGameManager->doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD());
}

void OddEvenWindow::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIWindowEx::TranslateUIMessage(ControlID, dwMsg);
	const MiniGame::GLOddEvenClient* const pGame = (const MiniGame::GLOddEvenClient* const)m_pMiniGameManager->GetMiniGame(MiniGame::EMMINIGAME_ODDEVEN);
	if ( pGame == NULL || m_pDice == NULL )
	{
		m_pMiniGameManager->RequestClose();
		return;
	}	

	switch ( ControlID )
	{
	case ODDEVEN_BEGIN_BUTTON:
	case ODDEVEN_REBEGIN_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			_translateUIMessageBeginButton();
		break;
// 	case ODDEVEN_ROLL_BUTTON:
// 		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
// 			_translateUIMessageRollButton();
// 		break;
	case ODDEVEN_RECEIVE_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			_translateUIMessageReceiveButton();
		break;
	case ODDEVEN_EXIT_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			_translateUIMessageExitButton();
		break;
	case ODDEVEN_ODD_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			_translateUIMessageOddButton();	
		break;
	case ODDEVEN_EVEN_BUTTON:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			_translateUIMessageEvenButton();
		break;
	case ODDEVEN_REWARD_LOSE:
		if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
			_translateUIMessageRewardButton(0);
		break;
	case ODDEVEN_REWARD_1:
	case ODDEVEN_REWARD_2:
	case ODDEVEN_REWARD_3:
	case ODDEVEN_REWARD_4:
		{
			const DWORD nRewardIndex = ControlID - ODDEVEN_REWARD_1;
			if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
				_translateUIMessageRewardButton(nRewardIndex);
		}
		break;
	case ODDEVEN_REWARD_ITEM_LOSE:
		if ( CHECK_MOUSE_IN(dwMsg) )
		{			
			const SNATIVEID& ItemID = *pGame->getDetermineItemID(0);
			const SITEM* pItemData = GLogicData::GetInstance().GetItem(ItemID);	
			if ( pItemData == NULL )
				break;

			m_pInterface->SHOW_ITEM_INFO(SITEMCUSTOM(pItemData->BasicId()), 
				FALSE, FALSE, FALSE, FALSE, FALSE, 
				USHRT_MAX, USHRT_MAX);
		}
		break;
	case ODDEVEN_REWARD_ITEM_1:
	case ODDEVEN_REWARD_ITEM_2:
	case ODDEVEN_REWARD_ITEM_3:
	case ODDEVEN_REWARD_ITEM_4:
		if ( CHECK_MOUSE_IN(dwMsg) )
		{
			const DWORD nIndex = ControlID - ODDEVEN_REWARD_ITEM_1;
			const SNATIVEID& ItemID = *pGame->getDetermineItemID(nIndex);
			const SITEM* pItemData = GLogicData::GetInstance().GetItem(ItemID);
			if ( pItemData == NULL )
				break;

			m_pInterface->SHOW_ITEM_INFO(SITEMCUSTOM(pItemData->BasicId()), 
				FALSE, FALSE, FALSE, FALSE, FALSE, 
				USHRT_MAX, USHRT_MAX);
		}
		break;
	}

	const MiniGame::EMODDEVEN_STAGE emCurrentStage = pGame->getCurrentStage();
	if ( emCurrentStage == MiniGame::EMODDEVEN_STAGE_ROLL)
	{
		const float _fPosition[2] = {float(m_PosX) - this->m_rcGlobalPos.left, float(m_PosY) - this->m_rcGlobalPos.top};	
		const float _fRePosition[2] = {_fPosition[0] - this->m_fHalfSize[0], this->m_fHalfSize[1] - _fPosition[1]};	
		const float _fDetPosition[2] = {_fRePosition[0] * this->m_fUHalfSize[0], _fRePosition[1] * this->m_fUHalfSize[1]};

		const DWORD* const nDetermineNumber = pGame->getDetermineNumber();
		m_pDice->grab(_fPosition[0], _fPosition[1]);
		m_pDice->holdOut((const unsigned int* const)nDetermineNumber, _fDetPosition[0], _fDetPosition[1]);	

// 		if ( CHECK_LB_UP_LIKE(dwMsg) )
// 			m_pDice->holdOut((const unsigned int* const)nDetermineNumber, _fDetPosition[0], _fDetPosition[1]);		
// 		else if ( CHECK_LB_DOWN_LIKE(dwMsg) || CHECK_LB_DRAG_LIKE(dwMsg) )
// 		{			
// 			m_pDice->grab(_fDetPosition[0], _fDetPosition[1]);
// 		}	
	}	
}


/*
 * MiniGameManager에서 호출 됨; 실제로 켜고 끔;*/
void OddEvenWindow::SetVisible(const bool bVisible)
{
	IColorProperty* const pColorProperty(m_pInterface->GetColorPropertyWindow());
	if ( bVisible == TRUE )
	{
		if ( IsVisible() == TRUE )
			return;

		if ( m_pMiniGameManager->GetMiniGame() == NULL )
			return; // 게임 로직이 활성화 되지 않았다면 NULL임;

		DxSoundLib::GetInstance()->CreateSound("DICE_ROLL", GLCONST_CHAR::strDICE_ROLL.c_str(), SFX_SOUND);			
		m_pDice = MiniGame::createDice(MiniGame::EMODDEVEN_MAX_DICE, 8, 8);
// RM# 1827 [9/10/2015 gbgim];
// 따로 처리하지않고 바로 주석처리한다;
// 		GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();		
// 		if ( pCharacter->UserLevel() >= USER_GM3 )
// 		{
// 			m_pInterface->ToggleWindowColorProperty(true);
// 			pColorProperty->bindLightProperty(&this->m_Light[0], 0);
// 			pColorProperty->bindLightProperty(&this->m_Light[1], 1);
// 			pColorProperty->bindLightProperty(&this->m_Light[2], 2);
// 			pColorProperty->bindMaterialProperty(&this->m_Material);
// 		}
		CUIWindowEx::SetVisibleSingle( TRUE );
	}
	else
	{
		MiniGame::destroyDice(m_pDice);
		DxSoundLib::GetInstance()->ReleaseSound("DICE_ROLL");
		CUIWindowEx::SetVisibleSingle( FALSE );
		m_pInterface->ToggleWindowColorProperty(false);
		pColorProperty->resetProperty();
	}
}


/*
 * UI 기본 로직에 의해 호출 됨; 서버에 요청 함;*/
void OddEvenWindow::SetVisibleSingle(BOOL bVisible)
{
	if ( m_bLock == true )
		return;

	if ( m_fCurrentTime - m_fLastRequestTime < 1.0f )
		return;
	m_fLastRequestTime = m_fCurrentTime;

	if ( bVisible == TRUE )
		m_pMiniGameManager->RequestOpen(MiniGame::EMMINIGAME_ODDEVEN);
	else
		m_pMiniGameManager->RequestClose();
	
	// Server측에 요청을 한 뒤, 서버의 응답 여부에 따라 켬/끔이 결정 됨;
	// 게임중에는 꺼지면 안됨;
}


HRESULT OddEvenWindow::Render(LPDIRECT3DDEVICEQ pd3dDevice)
{
	GASSERT( pd3dDevice );

	if ( !IsVisible() )
		return S_OK;

//	CUIGroup::Render(pd3dDevice);	
	HRESULT hr = S_OK;

	const MiniGame::GLOddEvenClient* const pGame = (const MiniGame::GLOddEvenClient* const)m_pMiniGameManager->GetMiniGame(MiniGame::EMMINIGAME_ODDEVEN);
	if ( pGame == NULL || m_pDice == NULL )
	{
		m_pMiniGameManager->RequestClose();
		return S_OK;
	}	

	RenderState renderState;

	DxBackupTarget sBackupTarget;
	sBackupTarget.Backup( pd3dDevice );

	_renderSetRenderState(pd3dDevice, renderState);	
	_renderRenderItem(pd3dDevice);
	_renderRecoverRenderState(pd3dDevice, renderState);
	sBackupTarget.Restore();


	CUIControl::ChangeSizeWindow();
	hr = CUIControl::Render ( pd3dDevice );
	if( FAILED( hr ) )
		return hr;

	

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl )
			continue;

		if ( !pControl->IsVisible () )
			continue;

		if ( pControl->GetWndID() == ODDEVEN_MONEY_BUTTON )
		{
			switch ( pGame->getCurrentStage() )
			{
			case MiniGame::EMODDEVEN_STAGE_SELECT_REWARD:
			case MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD:
				break;
			default:
				if ( FAILED(_renderRenderDiceTable(pd3dDevice)) )
					return E_FAIL;	
				break;
			}		
		}

		pControl->ChangeSizeWindow();
		hr = pControl->Render( pd3dDevice );
		if ( FAILED ( hr ) )
			return hr;
	}

	

	return S_OK;
}

void OddEvenWindow::_renderSetRenderState(LPDIRECT3DDEVICEQ pd3dDevice, RenderState& renderState)
{	
	pd3dDevice->GetMaterial(&renderState.Material);
	pd3dDevice->GetLight( 0, &renderState.Light[0] );
	pd3dDevice->GetLight( 1, &renderState.Light[1] );
	pd3dDevice->GetLight( 2, &renderState.Light[2] );
	pd3dDevice->GetRenderState( D3DRS_LIGHTING, &renderState.dwLight );
	pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &renderState.dwNormalize );
	pd3dDevice->GetRenderState( D3DRS_SPECULARENABLE, &renderState.dwSpecular );
	pd3dDevice->GetRenderState( D3DRS_ANTIALIASEDLINEENABLE, &renderState.dwAntialiased);
	pd3dDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &renderState.dwMagFilter);	
	pd3dDevice->GetSamplerState(0, D3DSAMP_MINFILTER, &renderState.dwMinFilter);	

	pd3dDevice->GetTransform( D3DTS_VIEW, &renderState.mViewPREV );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &renderState.mProjPREV);

	pd3dDevice->SetRenderTarget ( 0, this->m_pTargetSurface );
	pd3dDevice->SetDepthStencilSurface ( this->m_pTargetZBuffer );

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000L, 1.0f, 0x0L);	

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, true );
	pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, true );
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	
	pd3dDevice->SetMaterial(&m_Material);
	pd3dDevice->SetLight(0, &m_Light[0]);
	pd3dDevice->SetLight(1, &m_Light[1]);
	pd3dDevice->SetLight(2, &m_Light[2]);
	pd3dDevice->LightEnable(0, true);		
	pd3dDevice->LightEnable(1, true);	
	pd3dDevice->LightEnable(2, true);	

	pd3dDevice->SetTransform( D3DTS_VIEW, &this->m_mView );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &this->m_mProjection);
}

void OddEvenWindow::_renderRenderItem(LPDIRECT3DDEVICEQ pd3dDevice)
{
	if ( m_pDice == NULL )
		return;

	for ( unsigned int _i = m_pDice->getNDice(); _i; --_i )
	{			
		DxSimMesh* const _pSimMeshDice = m_DimMeshDice[_i - 1].get();		
		if ( !_pSimMeshDice )
			break;		
		const D3DXMATRIX& _mWorld = D3DXMATRIX(*m_pDice->getDiceWorldMatrix(_i - 1));

		_pSimMeshDice->UseMeshMaterials(TRUE);
		_pSimMeshDice->RenderItem(pd3dDevice, _mWorld, &m_Material);
	}
}

void OddEvenWindow::_renderRecoverRenderState(LPDIRECT3DDEVICEQ pd3dDevice, const RenderState& renderState)
{
	pd3dDevice->SetMaterial(&renderState.Material);
	pd3dDevice->SetLight(0, &renderState.Light[0]);
	pd3dDevice->SetLight(1, &renderState.Light[1]);
	pd3dDevice->SetLight(2, &renderState.Light[2]);
	pd3dDevice->LightEnable(0, true);
	pd3dDevice->LightEnable(1, false);
	pd3dDevice->LightEnable(2, false);

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, renderState.dwLight );
	pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, renderState.dwNormalize );
	pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, renderState.dwSpecular );
	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, renderState.dwMagFilter);	
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, renderState.dwMinFilter);		

	pd3dDevice->SetTransform( D3DTS_VIEW, &renderState.mViewPREV );
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &renderState.mProjPREV);
}

const HRESULT OddEvenWindow::_renderRenderDiceTable(LPDIRECT3DDEVICEQ pd3dDevice)
{
	UIVERTEX* pUIVertex = m_pDiceTable->GetUIVertex();

	::memcpy(m_TargetUIVertex, pUIVertex, sizeof(m_UIVertex));

	const D3DXVECTOR2 vRePos[4] = {
		m_UIVertex[0].GetPos() - D3DXVECTOR2(0.0f, -20.0f),
		m_UIVertex[1].GetPos() - D3DXVECTOR2(0.0f, -20.0f),
	};
	
	m_TargetUIVertex[0].SetPos(vRePos[0]);
	m_TargetUIVertex[1].SetPos(vRePos[1]);
	m_TargetUIVertex[0].SetTexturePos(0.0f, 0.0f);
	m_TargetUIVertex[1].SetTexturePos(1.0f, 0.0f);

	m_TargetUIVertex[2].SetTexturePos(1.0f, 1.0f);
	m_TargetUIVertex[3].SetTexturePos(0.0f, 1.0f);

	if( CUIRenderQueue::Get()->Render( m_pTargetTexture, m_TargetUIVertex ) )
		return S_OK;

	return E_FAIL;
}

HRESULT OddEvenWindow::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	HRESULT hr(S_OK);

	V_RETURN( CUIGroup::InitDeviceObjects( pd3dDevice ) );

	m_DimMeshDice[0] = DxSimpleMeshMan::GetInstance().Load ( 
		"dice_red.x", 
		FALSE, FALSE, TEXTURE_RESIZE_NONE, TEXTURE_EFFECT );	
	m_DimMeshDice[1] = DxSimpleMeshMan::GetInstance().Load ( 
		"dice_black.x", 
		FALSE, FALSE, TEXTURE_RESIZE_NONE, TEXTURE_EFFECT );	


	ZeroMemory(&m_Light[0], sizeof(D3DLIGHT9));
	ZeroMemory(&m_Light[1], sizeof(D3DLIGHT9));
	ZeroMemory(&m_Light[2], sizeof(D3DLIGHT9));
	D3DXVECTOR3 _vLight0 = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	D3DXVECTOR3 _vLight1 = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	D3DXVECTOR3 _vLight2 = D3DXVECTOR3(-0.5f, -1.0f, 0.0f);
	D3DXVec3Normalize(&_vLight0, &_vLight0);
	D3DXVec3Normalize(&_vLight1, &_vLight1);
	D3DXVec3Normalize(&_vLight2, &_vLight2);

	const float _fFactor(1.0f / 255.0f);
	m_Light[0].Type = D3DLIGHT_DIRECTIONAL;	
	m_Light[0].Direction = _vLight0; 
	m_Light[0].Diffuse	= D3DXCOLOR(255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f);	
	m_Light[0].Ambient	= D3DXCOLOR(211.0f * _fFactor, 211.0f * _fFactor, 211.0f * _fFactor, 1.0f);
	m_Light[0].Specular	= D3DXCOLOR(255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f);	

	m_Light[1].Type = D3DLIGHT_DIRECTIONAL;	
	m_Light[1].Direction = _vLight1; 
	m_Light[1].Diffuse	= D3DXCOLOR(139.0f * _fFactor, 106.0f * _fFactor, 0.0f * _fFactor, 1.0f);	
	m_Light[1].Ambient	= D3DXCOLOR(11.0f * _fFactor, 11.0f * _fFactor, 11.0f * _fFactor, 1.0f);	
	m_Light[1].Specular	= D3DXCOLOR(139.0f * _fFactor, 91.0f * _fFactor, 26.0f * _fFactor, 1.0f);	

	m_Light[2].Type = D3DLIGHT_DIRECTIONAL;	
	m_Light[2].Direction = _vLight2; 
	m_Light[2].Diffuse	= D3DXCOLOR(69.0f * _fFactor, 96.0f * _fFactor, 255.0f * _fFactor, 1.0f);	
	m_Light[2].Ambient	= D3DXCOLOR(7.0f * _fFactor, 7.0f * _fFactor, 7.0f * _fFactor, 1.0f);	
	m_Light[2].Specular	= D3DXCOLOR(151.0f * _fFactor, 171.0f * _fFactor, 255.0f * _fFactor, 1.0f);	

	ZeroMemory(&m_Material, sizeof(m_Material));
	m_Material.Diffuse	= D3DXCOLOR(255.0f * _fFactor, 255.0f * _fFactor, 255.0f * _fFactor, 1.0f);	
	m_Material.Ambient	= D3DXCOLOR(163.0f * _fFactor, 163.0f * _fFactor, 163.0f * _fFactor, 1.0f);	
	m_Material.Specular	= D3DXCOLOR(237.0f * _fFactor, 237.0f * _fFactor, 237.0f * _fFactor, 1.0f);	
	m_Material.Emissive	= D3DXCOLOR(0.0f * _fFactor, 0.0f * _fFactor, 0.0f * _fFactor, 1.0f);	
	m_Material.Power = 8.8f;

	::D3DXMatrixLookAtLH(&this->m_mView,  &D3DXVECTOR3(0.0f, 0.0f, -17.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	::D3DXMatrixPerspectiveFovLH(&this->m_mProjection, 0.2f * D3DX_PI, 1.0f, 1.0f, 128.0f);	

	return hr;
}
HRESULT OddEvenWindow::RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	GASSERT( pd3dDevice );

	HRESULT hr(S_OK);
	V_RETURN(CUIGroup::RestoreDeviceObjects( pd3dDevice )); 

	SAFE_RELEASE(m_pTargetTexture);
	SAFE_RELEASE(m_pTargetSurface);
	SAFE_RELEASE(m_pTargetZBuffer);	

	LPDIRECT3DSURFACEQ pBackBuffer;
	if ( pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) != D3D_OK )
		return E_FAIL;

	pBackBuffer->GetDesc( &this->m_d3dBackBufferDesc );
	pBackBuffer->Release();

	const UIRECT& uiRect = m_pDiceTable->GetGlobalPos();
	this->m_d3dBackBufferDesc.Width  = (UINT)uiRect.sizeX;
	this->m_d3dBackBufferDesc.Height = (UINT)uiRect.sizeY;
	this->m_fHalfSize[0] = uiRect.sizeX * 0.5f;
	this->m_fHalfSize[1] = uiRect.sizeY * 0.5f;
	this->m_fUHalfSize[0] = 1.0f / (uiRect.sizeX * 0.5f);
	this->m_fUHalfSize[1] = 1.0f / (uiRect.sizeY * 0.5f);

	V_RETURN ( pd3dDevice->CreateDepthStencilSurface(
		m_d3dBackBufferDesc.Width, m_d3dBackBufferDesc.Height, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE,
		0, TRUE, &m_pTargetZBuffer, NULL ) );
	V_RETURN ( pd3dDevice->CreateTexture(
		m_d3dBackBufferDesc.Width, m_d3dBackBufferDesc.Height, 1, 
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTargetTexture, NULL ) );

	V_RETURN(m_pTargetTexture->GetSurfaceLevel(0, &this->m_pTargetSurface));

	return S_OK;
}
HRESULT OddEvenWindow::InvalidateDeviceObjects(void)
{
	HRESULT hr(S_OK);

	V_RETURN(CUIControl::InvalidateDeviceObjects());

	SAFE_RELEASE(m_pTargetSurface);
	SAFE_RELEASE(m_pTargetZBuffer);
	SAFE_RELEASE(m_pTargetTexture);

	return S_OK;
}


void OddEvenWindow::CreateUIWindowAndRegisterOwnership(void)
{
	Create( MINIGAME_ODD_EVEN_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CreateBaseWindowLightGray("MINIGAME_ODDEVEN_WINDOW_BASE", (char*)ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 0 ));
	SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_CENTER_X );

	{
		m_pDiceTable = CreateLineBox( "MINIGAME_ODDEVEN_WINDOW_TABLE", "PARTY_INFO_LINEBOX_TEXTUREINFO" );
		m_pDiceTable->SetVisibleSingle(FALSE);
		CreateLineBox( "MINIGAME_ODDEVEN_WINDOW_PANEL", "PARTY_INFO_LINEBOX_TEXTUREINFO" );
		CreateLineBox( "MINIGAME_ODDEVEN_WINDOW_PANEL_INFO_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO" );
		CreateLineBox( "MINIGAME_ODDEVEN_WINDOW_PANEL_SELECT_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO" );
		CreateLineBox( "MINIGAME_ODDEVEN_WINDOW_PANEL_MONEY_BOX", "PARTY_INFO_LINEBOX_TEXTUREINFO" );

		CBasicButton* pButton = NULL;
		pButton = CreateFlipButton( "MINIGAME_ODDEVEN_MONEY_BUTTON", "MINIGAME_ODDEVEN_MONEY_BUTTON_FLIP", ODDEVEN_MONEY_BUTTON, CBasicButton::CLICK_FLIP);
		pButton->SetAlignFlag(UI_FLAG_BOTTOM);
		pButton->SetUseDynamic(FALSE);

		CD3DFontPar* pFont10 = m_pEngineDevice->LoadFont( _DEFAULT_FONT, 10, D3DFONT_SHADOW | D3DFONT_ASCII );
		m_pMoneyText = CreateStaticControl("MINIGAME_ODDEVEN_WINDOW_PANEL_MONEY_BOX_TEXT", pFont10, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT);
		m_pMoneyText->SetAlignFlag(UI_FLAG_BOTTOM);

		m_pGuideText = CreateStaticControl("MINIGAME_ODDEVEN_WINDOW_PANEL_GUIDE_TEXT", pFont10, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT);
		m_pGuideText->SetAlignFlag(UI_FLAG_BOTTOM);

		m_pProcessText[0] = CreateStaticControl("MINIGAME_ODDEVEN_WINDOW_PANEL_PROCESS_YESNO_TEXT", pFont10, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT);
		m_pProcessText[1] = CreateStaticControl("MINIGAME_ODDEVEN_WINDOW_PANEL_PROCESS_SELECT_TEXT", pFont10, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT);
		m_pProcessText[2] = CreateStaticControl("MINIGAME_ODDEVEN_WINDOW_PANEL_PROCESS_ROLL_TEXT", pFont10, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT);
		m_pProcessText[3] = CreateStaticControl("MINIGAME_ODDEVEN_WINDOW_PANEL_PROCESS_REWARD_TEXT", pFont10, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT);
		m_pProcessText[0]->SetText(ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 11));	
		m_pProcessText[1]->SetText(ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 12));	
		m_pProcessText[2]->SetText(ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 3));	
		m_pProcessText[3]->SetText(ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 13));

		m_pInformationText = CreateStaticControl("MINIGAME_ODDEVEN_WINDOW_PANEL_INFORMATION_TEXT", pFont10, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_LEFT);
		m_pInformationText->SetAlignFlag(UI_FLAG_BOTTOM);

		m_pProgressText = CreateStaticControl("MINIGAME_ODDEVEN_WINDOW_PANEL_PROGRESS_TEXT", pFont10, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X);
		m_pProgressText->SetText((char*)ID2GAMEWORD("MINIGAME_ODDEVEN_TEXT", 14));
		m_pProgressText->SetAlignFlag(UI_FLAG_BOTTOM);

		const std::string szLabelName[ODDEVEN_LABEL_NSIZE] = 
		{
			"MINIGAME_TEXT_YESNO",
			"MINIGAME_TEXT_READY",
			"MINIGAME_TEXT_SELECT",			
			"MINIGAME_TEXT_WIN",
			"MINIGAME_TEXT_LOSE",
			"MINIGAME_TEXT_REWARD"
		};

		for ( DWORD _i = 0; _i < ODDEVEN_LABEL_NSIZE; ++_i )
		{
			m_pLabel[_i] = CUIWindowEx::CreateControl(szLabelName[_i]);
			m_pLabel[_i]->SetVisibleSingle(FALSE);
		}		

		RnButton::CreateArg Arg;
		Arg.defaultTextureKey = "PARTY_INFO_BUTTON_NORMAL";
		Arg.mouseOverTextureKey = "PARTY_INFO_BUTTON_OVER";
		Arg.mouseClickTextureKey = "PARTY_INFO_BUTTON_CLICK";
		{ // [진행] 버튼;
			Arg.text = ID2GAMEWORD( "MINIGAME_ODDEVEN_TEXT", 1);

			m_pBeginButton = new RnButton(m_pEngineDevice);
			m_pBeginButton->CreateSub(this,"MINIGAME_ODDEVEN_WINDOW_PANEL_BUTTON", UI_FLAG_DEFAULT, ODDEVEN_BEGIN_BUTTON);
			m_pBeginButton->CreateSubControl(Arg);
			RegisterControl(m_pBeginButton);
		}
		{ // [연승 도전] 버튼;
			Arg.text = ID2GAMEWORD( "MINIGAME_ODDEVEN_TEXT", 2);

			m_pRebeginButton = new RnButton(m_pEngineDevice);
			m_pRebeginButton->CreateSub(this,"MINIGAME_ODDEVEN_WINDOW_PANEL_BUTTON", UI_FLAG_DEFAULT, ODDEVEN_REBEGIN_BUTTON);
			m_pRebeginButton->CreateSubControl(Arg);
			RegisterControl(m_pRebeginButton);
		}
		{
			Arg.text = ID2GAMEWORD( "MINIGAME_ODDEVEN_TEXT", 5);

			m_pReceiveButton = new RnButton(m_pEngineDevice);
			m_pReceiveButton->CreateSub(this,"MINIGAME_ODDEVEN_WINDOW_PANEL_BUTTON", UI_FLAG_DEFAULT, ODDEVEN_RECEIVE_BUTTON);
			m_pReceiveButton->CreateSubControl(Arg);
			RegisterControl(m_pReceiveButton);			
		}
		{ // [나가기] 버튼;
			Arg.text = ID2GAMEWORD( "MINIGAME_ODDEVEN_TEXT", 4);

			m_pExitButton = new RnButton(m_pEngineDevice);
			m_pExitButton->CreateSub(this,"MINIGAME_ODDEVEN_WINDOW_PANEL_BUTTON", UI_FLAG_DEFAULT, ODDEVEN_EXIT_BUTTON);
			m_pExitButton->CreateSubControl(Arg);
			RegisterControl(m_pExitButton);
		}
		{ // 홀/짝 선택 버튼;
			m_pOddButton = new CBasicButton(m_pEngineDevice);
			m_pOddButton->CreateSub( this, "MINIGAME_ODDEVEN_ODD_BUTTON", UI_FLAG_DEFAULT, ODDEVEN_ODD_BUTTON );
			m_pOddButton->CreateMouseOver( "MINIGAME_ODDEVEN_ODD_BUTTON_OVER");
			m_pOddButton->CreateFlip( "MINIGAME_ODDEVEN_ODD_BUTTON_FLIP", CBasicButton::RADIO_FLIP );
			RegisterControl(m_pOddButton);

			m_pEvenButton = new CBasicButton(m_pEngineDevice);
			m_pEvenButton->CreateSub( this, "MINIGAME_ODDEVEN_EVEN_BUTTON", UI_FLAG_DEFAULT, ODDEVEN_EVEN_BUTTON );
			m_pEvenButton->CreateMouseOver( "MINIGAME_ODDEVEN_EVEN_BUTTON_OVER");
			m_pEvenButton->CreateFlip( "MINIGAME_ODDEVEN_EVEN_BUTTON_FLIP", CBasicButton::RADIO_FLIP );
			RegisterControl(m_pEvenButton);
		}

		{ // 보상 박스 버튼;
			const std::string szRewardName[ODDEVEN_REWARD_NSIZE][5] = 
			{
				{"MINIGAME_BOX_BRONZE_CLOSE", "MINIGAME_BOX_BRONZE_CLOSE_OVER", "MINIGAME_BOX_BRONZE_HALF_OPEN", "MINIGAME_BOX_BRONZE_OPEN", "MINIGAME_BOX_BRONZE_OPEN_OVER"},
				{"MINIGAME_BOX_GOLD_CLOSE_0", "MINIGAME_BOX_GOLD_CLOSE_OVER_0", "MINIGAME_BOX_GOLD_HALF_OPEN_0", "MINIGAME_BOX_GOLD_OPEN_0", "MINIGAME_BOX_GOLD_OPEN_OVER_0"},
				{"MINIGAME_BOX_GOLD_CLOSE_1", "MINIGAME_BOX_GOLD_CLOSE_OVER_1", "MINIGAME_BOX_GOLD_HALF_OPEN_1", "MINIGAME_BOX_GOLD_OPEN_1", "MINIGAME_BOX_GOLD_OPEN_OVER_1"},
				{"MINIGAME_BOX_GOLD_CLOSE_2", "MINIGAME_BOX_GOLD_CLOSE_OVER_2", "MINIGAME_BOX_GOLD_HALF_OPEN_2", "MINIGAME_BOX_GOLD_OPEN_2", "MINIGAME_BOX_GOLD_OPEN_OVER_2"},
				{"MINIGAME_BOX_GOLD_CLOSE_3", "MINIGAME_BOX_GOLD_CLOSE_OVER_3", "MINIGAME_BOX_GOLD_HALF_OPEN_3", "MINIGAME_BOX_GOLD_OPEN_3", "MINIGAME_BOX_GOLD_OPEN_OVER_3"}				
			};
			for ( DWORD _i = 0; _i < ODDEVEN_REWARD_NSIZE; ++_i )
			{
				m_pRewardCloseBox[_i] = new CBasicButton(m_pEngineDevice);
				m_pRewardCloseBox[_i]->CreateSub(this, szRewardName[_i][0].c_str(), UI_FLAG_DEFAULT, ODDEVEN_REWARD_LOSE + _i);
				m_pRewardCloseBox[_i]->CreateFlip(szRewardName[_i][1].c_str(), CBasicButton::MOUSEIN_FLIP);
				RegisterControl(m_pRewardCloseBox[_i]);

				m_pRewardOpenBox[_i][0] = new CUIControl(m_pEngineDevice);
				m_pRewardOpenBox[_i][0]->CreateSub(this, szRewardName[_i][2].c_str(), UI_FLAG_DEFAULT);
				RegisterControl(m_pRewardOpenBox[_i][0]);

				m_pRewardOpenBox[_i][1] = new CUIControl(m_pEngineDevice);
				m_pRewardOpenBox[_i][1]->CreateSub(this, szRewardName[_i][3].c_str(), UI_FLAG_DEFAULT);				
				RegisterControl(m_pRewardOpenBox[_i][1]);

				m_pRewardOpenSelectBox[_i] = new CUIControl(m_pEngineDevice);
				m_pRewardOpenSelectBox[_i]->CreateSub(this, szRewardName[_i][4].c_str(), UI_FLAG_DEFAULT);				
				RegisterControl(m_pRewardOpenSelectBox[_i]);
			}

			const std::string szRewardItem[ODDEVEN_REWARD_NSIZE] =
			{
				"MINIGAME_ODDEVEN_REWARD_ITEM_0",
				"MINIGAME_ODDEVEN_REWARD_ITEM_1",
				"MINIGAME_ODDEVEN_REWARD_ITEM_2",
				"MINIGAME_ODDEVEN_REWARD_ITEM_3",
				"MINIGAME_ODDEVEN_REWARD_ITEM_4",
			};
			for ( DWORD _i = 0; _i < ODDEVEN_REWARD_NSIZE; ++_i )
			{
				m_pItemImage[_i] = new CItemImage(m_pGaeaClient, m_pEngineDevice);
				m_pItemImage[_i]->CreateSub(this, szRewardItem[_i].c_str(), UI_FLAG_DEFAULT, ODDEVEN_REWARD_ITEM_LOSE + _i);
				m_pItemImage[_i]->CreateSubControl();
				RegisterControl(m_pItemImage[_i]);
			}							
		}

		{
			RnProgressBar::CreateArg arg;
			arg.controlName = "MINIGAME_ODDEVEN_WINDOW_PANEL_PROGRESS_BAR";
			arg.backgroundTextureName = "PARTY_TENDER_ITEM_PROGRESS_BG_TEXTURE";
			arg.overTextureName = "PARTY_TENDER_ITEM_PROGRESS_OVER_TEXTURE";

			m_pProgressBar = new RnProgressBar( m_pEngineDevice );
			m_pProgressBar->CreateSub( this, "MINIGAME_ODDEVEN_WINDOW_BASE", UI_FLAG_DEFAULT);
			m_pProgressBar->CreateSubControl(arg);
			RegisterControl(m_pProgressBar);
		}
	}

	m_pInterface->UiRegisterControl( this, true );
	m_pInterface->UiShowGroupFocus( GetWndID() );
	CUIWindowEx::SetVisibleSingle(FALSE);
}