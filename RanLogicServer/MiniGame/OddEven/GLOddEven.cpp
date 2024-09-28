#include "pch.h"

#include "../../../SigmaCore/Math/Random.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogic/MiniGame/OddEven/GLOddEvenSettings.h"
#include "../../../RanLogic/Msg/GLContrlMiniGameMsg.h"

#include "../../Character/GLChar.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "../../Land/GLLAndMan.h"
#include "../../Util/GLItemLimit.h"
#include "GLOddEven.h"

namespace MiniGame
{
	GLOddEvenField::GLOddEvenField(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID)
		: MiniGame::GLOddEven(dwGaeaID, dwMiniGameID, fBeginTime, NPCID, dwNPCGaeaID)
		, m_pGaeaServer(pGaeaServer)
	{		
	}

	GLOddEvenField::~GLOddEvenField(void)
	{
	}

	const bool GLOddEvenField::_setDetermineNumber(void)
	{ // �� �Լ��� ȣ��Ǵ� ������ Ȧ/¦(m_emSelected) ���� ��;
		// �ܰ�(m_nCurrentStep)�� ���� �� �־�� ��;

		if ( m_emSelected != MiniGame::EMODDEVEN_ODD &&
			m_emSelected != MiniGame::EMODDEVEN_EVEN )
			return false; // ��ȿ�� ���� ���� �� ���� �ʴٸ� ����;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT &&
			m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_ROLL )
			return false; // ��ȿ�� �������������� ��û�� �ƴ϶�� ����;

		std::vector<MiniGame::EMODDEVEN> shuffleBuffer;

		{ // Ȯ���� ���� ���� �迭 ����;
			shuffleBuffer.reserve(100);

			MiniGame::EMODDEVEN_EVEN; // 1;
			MiniGame::EMODDEVEN_ODD; // 2;		

			const MiniGame::EMODDEVEN emRight = m_emSelected;
			const MiniGame::EMODDEVEN emWrong = (m_emSelected == MiniGame::EMODDEVEN_EVEN) ? MiniGame::EMODDEVEN_ODD : MiniGame::EMODDEVEN_EVEN;
			
			const DWORD nSucessRate = MiniGame::OddEvenSettings::getInstance()->getSuccessRate(m_nCurrentStep); // ���� Ȯ��;
			for ( DWORD _i = 0; _i < nSucessRate; ++_i )
				shuffleBuffer.push_back(emRight);

			for ( DWORD _i = nSucessRate; _i < 100; ++_i )
				shuffleBuffer.push_back(emWrong);

			std::random_shuffle(shuffleBuffer.begin(), shuffleBuffer.end());
		}		

		DWORD nSum = 0;
		unsigned int nDetNumber[MiniGame::EMODDEVEN_MAX_DICE];
		for ( DWORD _i = MiniGame::EMODDEVEN_MAX_DICE; _i; --_i )
		{
			nDetNumber[_i - 1] = sc::Random::getInstance().RandomNumber(1, 6);
			nSum += nDetNumber[_i - 1];
		}

		const unsigned int nDiceNumber = sc::Random::getInstance().RandomNumber(0, 99); // 0~99���� ���ڸ� �����ϰ� �����Ѵ�.
		
		m_emResult = shuffleBuffer[nDiceNumber];
		const MiniGame::EMODDEVEN emDiceResult = (nSum & 0x1) ? MiniGame::EMODDEVEN_ODD : MiniGame::EMODDEVEN_EVEN;

		if ( m_emResult == emDiceResult )
		{ // �÷��̾ ������ �����, �迭�� ���� ���� ����� ���ٸ� �״�� ����;
			for ( DWORD _i = 0; _i < MiniGame::EMODDEVEN_MAX_DICE; ++_i )
				m_nDetermineNumber[_i] = nDetNumber[_i];
		}
		else
		{ // �׷��� ������ �ֻ��� �Ѱ��� ���� �ٲ�;
			this->m_nDetermineNumber[0] = (7 - nDetNumber[0]);
			for ( DWORD _i = 1; _i < MiniGame::EMODDEVEN_MAX_DICE; ++_i )
				m_nDetermineNumber[_i] = nDetNumber[_i];			
		}

		return true;
	}


	void GLOddEvenField::FrameMove(const float fTime, const float fElapsedTime)
	{
	}

	/*
	 * �Ʒ� �ۼ��� ��� �Լ����� �ߺ� �޼����� �� �� �ִٴ� �����Ͽ� �ۼ� �ؾ� ��;
	*/
	const bool GLOddEvenField::_doMessageRequestBegin(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_BEGIN* const pRequestBeginMessage = 
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_BEGIN* const)pMessage;

		if ( m_emCurrentStage == MiniGame::EMODDEVEN_STAGE_SELECT )
			return true; // �̹� ó�� �Ǿ��ٸ� ����;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_READY )
			return false; // �غ� �ܰ谡 �ƴϸ� ����;		

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // ���� Ƚ���� ���ǵ� ũ�⺸�� ũ�ٸ� ����;

		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
		{
			// ��� �� �α� ����
			// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
			sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on _doMessageRequestBegin ", dwGaeaID));
			return false; // ��ȿ���� ���� �ɸ��Ͷ�� ����;
		}

		MiniGame::OddEvenSettings* const pSettings = MiniGame::OddEvenSettings::getInstance();
		const LONGLONG& lnMoney = pPlayer->GetInvenMoney();
		const LONGLONG& lnRequireMoney = pSettings->getRequireMoney();		
		if ( lnRequireMoney > lnMoney )
			return false;
		
		const LONGLONG& lnCostMoney = pSettings->getCostMoney(m_nCurrentStep);
		if ( lnCostMoney > lnMoney )
			return false;

		pPlayer->CheckMoneyUpdate( lnMoney, lnCostMoney, TRUE, "MiniGame - OddEven Shuffle.");
		// ��� �� �α� ����
		pPlayer->LogMoneyExchange(
			gamelog::ID_CHAR, pPlayer->CharDbNum(), 
			gamelog::ID_NONE, m_nCurrentStep, 
			lnCostMoney,
			ITEM_ROUTE_ODD_EVEN,
			(pPlayer == NULL) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pPlayer->GetCurrentMap(), LOG_MONEY_ODD_EVEN_CHARGE);

		// ���� �ڵ忡 ������ �ִ� �κ��ε�, ���� �Ϸ��� �ٲ�� ��;
// 		GLMSG::NET_ODD_EVEN_LOG_FH MsgFH(
// 			dwGaeaID,
// 			pPlayer->CharDbNum(),
// 			m_nCurrentStep,
// 			WinLost,
// 			static_cast<__int64> (m_ui64BattingMoney),
// 			DividendMoney,
// 			m_lnMoney);
// 		m_pGaeaServer->SendToCache(&MsgFH);

		pPlayer->SubtractInvenMoneyServer(lnCostMoney, SUBTRACT_INVEN_MONEY_ODDEVEN_COST);
		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_SELECT;
		m_emSelected = MiniGame::EMODDEVEN_NOT_SELECTED;

		GLMSG::NET_MINIGAME_ODDEVEN_DO_BEGIN doBeginMessage(m_nCurrentStep, lnCostMoney);
		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), &doBeginMessage);
		
		return true;
	}

	const bool GLOddEvenField::_doMessageRequestSelectOddEven(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN* const pRequestSelectOddEvenMessage = 
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT )
			return true; // �̹� ���� �� �ִ�; Ȧ/¦ ���� �ܰ谡 �ƴϸ� ����;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // ���� Ƚ���� ���ǵ� ũ�⺸�� ũ�ٸ� ����;

		if ( pRequestSelectOddEvenMessage->emOddEven >= MiniGame::EMODDEVEN_NSIZE )
			return false; // ��ȿ���� ���� ���� ���̶�� ����;

		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
			return false; // ��ȿ���� ���� �ɸ��Ͷ�� ����;

		m_emSelected = pRequestSelectOddEvenMessage->emOddEven;
		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_ROLL;

		if ( GLOddEvenField::_setDetermineNumber() == false )
			return false; // ������ �߸����־� �ֻ��� ���� �������� ���ߴٸ� ����;

		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), &GLMSG::NET_MINIGAME_ODDEVEN_DO_SELECT_ODDEVEN(pRequestSelectOddEvenMessage->emOddEven));		
		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), &GLMSG::NET_MINIGAME_ODDEVEN_DO_ROLL(m_nDetermineNumber));

		return true;
	}

	const bool GLOddEvenField::_doMessageRequestRoll(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID)
	{ // ��ȹ�� ��û���� ������ ����;
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_ROLL* const pRequestRollMessage = 
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_ROLL* const)pMessage;

		if ( m_emCurrentStage == MiniGame::EMODDEVEN_STAGE_ROLL )
			return true; // �̹� ó�� �Ǿ��ٸ� ����;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT )			
			return false; // Ȧ/¦ ���� �ܰ谡 �ƴϸ� ����;		

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // ���� Ƚ���� ���ǵ� ũ�⺸�� ũ�ٸ� ����;

		if ( pRequestRollMessage->nCurrentStep != m_nCurrentStep )
			return false; // ���� Ƚ�� �� ���� ������ Ʋ���ٸ� ����;		

		if ( pRequestRollMessage->emOddEven >= MiniGame::EMODDEVEN_NSIZE )
			return false; // ��ȿ���� ���� ���� ���̶�� ����;

		if ( pRequestRollMessage->emOddEven != m_emSelected )
			return false; // ���� ���� ���� ������ Ʋ���ٸ� ����;

		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
			return false; // ��ȿ���� ���� �ɸ��Ͷ�� ����;

		if ( GLOddEvenField::_setDetermineNumber() == false )
			return false; // ������ �߸����־� �ֻ��� ���� �������� ���ߴٸ� ����;

		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_ROLL;
		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), &GLMSG::NET_MINIGAME_ODDEVEN_DO_ROLL(m_nDetermineNumber));

		return true;
	}

	const bool GLOddEvenField::_doMessageRequestResult(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RESULT* const pRequestResultMessage = 
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RESULT* const)pMessage;

		// �� �޼����� �ֻ��� �����Ⱑ ���� ��;
		// �� �ѹ��� �������� ������ ���� �������� �˻縸 ��;
		// �׷��� �ʴٴ°� ��Ŷ ���� Ȥ��, �ֻ��� ���� ����;
		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_ROLL )
			return false;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // ���� Ƚ���� ���ǵ� ũ�⺸�� ũ�ٸ� ����;

		if ( m_emSelected != MiniGame::EMODDEVEN_ODD &&
			m_emSelected != MiniGame::EMODDEVEN_EVEN )
			return false; // ��ȿ���� ���� ���� ���̶�� ����;

		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
			return false; // ��ȿ���� ���� �÷��̾��� ����;

		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_RESULT;
		
		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), &GLMSG::NET_MINIGAME_ODDEVEN_DO_RESULT(m_emSelected, m_emResult));

		return true;
	}

	const bool GLOddEvenField::_doMessageRequestReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_REWARD* const pRequestRewardMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_REWARD* const)pMessage;

		if ( m_emCurrentStage == MiniGame::EMODDEVEN_STAGE_SELECT_REWARD || 
			m_emCurrentStage == MiniGame::EMODDEVEN_STAGE_READY )
			return true; // �̹� ó�� �Ǿ��ٸ� ����;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_RESULT )
			return false; // ��� ���� �ܰ谡 �ƴϸ� ����;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // ���� Ƚ���� ���ǵ� ũ�⺸�� ũ�ٸ� ����;

		if ( m_emResult != pRequestRewardMessage->emResult )
			return false; // ��� ���� ������ �ٸ��ٸ� ����;

		if ( m_emSelected != MiniGame::EMODDEVEN_ODD &&
			m_emSelected != MiniGame::EMODDEVEN_EVEN )
			return false; // ��ȿ���� ���� ���� ���̶�� ����;

		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
			return false;

		const bool bWin = m_emResult == m_emSelected;		
		const bool bLoseReward = MiniGame::OddEvenSettings::getInstance()->getLoseReward();
		const MiniGame::EMODDEVEN_STAGE emNextStage = ((bWin || bLoseReward) ? MiniGame::EMODDEVEN_STAGE_SELECT_REWARD : MiniGame::EMODDEVEN_STAGE_READY);
		m_emCurrentStage = emNextStage;
		switch ( emNextStage )
		{
		case MiniGame::EMODDEVEN_STAGE_READY:
			GLOddEven::reset();
			break;
		}

		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), &GLMSG::NET_MINIGAME_ODDEVEN_DO_REWARD(emNextStage, bWin));
		
		return true;
	}

	const bool GLOddEvenField::_doMessageRequestSelectReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD* const pRequestSelectRewardMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD* const)pMessage;

		if ( m_emCurrentStage == MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD )
			return true; // �̹� ó�� �Ǿ��ٸ� ����;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT_REWARD )
			return false; // ���� ���� �ܰ谡 �ƴϸ� ����;

		MiniGame::OddEvenSettings* const pOddEven = MiniGame::OddEvenSettings::getInstance();
		const DWORD nMaxWin = pOddEven->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // ���� Ƚ���� ���ǵ� ũ�⺸�� ũ�ٸ� ����;

		const bool bWin = m_emResult == m_emSelected;
		const bool bLoseReward = pOddEven->getLoseReward();
		if ( bWin == false )
		{
			if ( bLoseReward == false )
				return false; // �¸� ���� �ʾҰ�, �й�� ������ ���ٸ� ����;			
		}		

		if ( pRequestSelectRewardMessage->nItemIndex >= MiniGame::EMODDEVEN_MAX_REWARD )
			return false;
		

		const DWORD nStage = ((bWin == true) ? m_nCurrentStep : MiniGame::EMODDEVEN_REWARD_FAIL);

		if ( pOddEven->getTakeItem(nStage, pRequestSelectRewardMessage->nItemIndex, m_DetermineItemID) == false )
			return false;

		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
			return false;

		m_nSelectItemIndex = pRequestSelectRewardMessage->nItemIndex;
		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD;
		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), &GLMSG::NET_MINIGAME_ODDEVEN_DO_SELECT_REWARD(nStage, pRequestSelectRewardMessage->nItemIndex, m_DetermineItemID));

		return true;
	}

	const bool GLOddEvenField::_doMessageRequestReceiveReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD* const pRequestReceiveRewardMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD* const)pMessage;

		if ( m_emCurrentStage == MiniGame::EMODDEVEN_STAGE_READY )
			return true; // �̹� ó�� �Ǿ��ٸ� ����;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD )
			return false; // ���� �ޱ� �ܰ谡 �ƴϸ� ����;

		MiniGame::OddEvenSettings* const pOddEven = MiniGame::OddEvenSettings::getInstance();
		const DWORD nMaxWin = pOddEven->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // ���� Ƚ���� ���ǵ� ũ�⺸�� ũ�ٸ� ����;

		const bool bWin = m_emResult == m_emSelected;
		const bool bLoseReward = pOddEven->getLoseReward();
		if ( bWin == false )
		{
			if ( bLoseReward == false )
				return false; // �¸� ���� �ʾҰ�, �й�� ������ ���ٸ� ����;

			if ( m_nSelectItemIndex != 0 )
				return false; // �й�� ���� ������ �ε����� 0�� �ƴϸ� ����;
			// �й�� ���� ������ �ڽ��� �ϳ�����;
		}		

		if ( m_nSelectItemIndex >= MiniGame::EMODDEVEN_MAX_REWARD )
			return false;
		
		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
			return false;

		const SNATIVEID receiveItemID = m_DetermineItemID[m_nSelectItemIndex];

		// ������ �߻�
		SITEMCUSTOM newItem;
		m_pGaeaServer->NewItemMake(newItem, receiveItemID, EMGEN_BILLING);
		if ( newItem.GetNativeID() != SNATIVEID(false) )
		{
			newItem.GenerateBasicStat( false );
			newItem.GenerateLinkSkill();
			newItem.GenerateAddOption();

			m_pGaeaServer->GetItemLimit()->LogItemBasicStatOption( newItem );
			m_pGaeaServer->GetItemLimit()->LogItemLinkSkillOption( newItem );
			m_pGaeaServer->GetItemLimit()->LogItemAddonOption( newItem );

			if ( newItem.GENERATE_RANDOM_OPT( false ) )
			{
				GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
				pItemLimit->LogItemRandomOption(newItem);
			}

			CItemDrop dropItem;
			dropItem.sItemCustom = newItem;
			if ( pPlayer->InsertToInven(&dropItem, true, false, false, false, true) == false )
			{
				// �κ��丮�� ����á�ٰų� �ϴ� ������ ������ ��� �������� ����;
				const bool bResult = m_pGaeaServer->PostSendSystemMail(
					pPlayer->GetCharID(),
					std::string(pPlayer->m_szName),
					&dropItem,
					(LONGLONG)0,
					ATTENDANCE_REWARD_POST_DESIGN,
					ATTENDANCE_REWARD_POST_SENDER,
					std::string(ID2SERVERTEXT("PARTY_TENDER_POST_SENDER_NAME")),
					std::string(ID2SERVERTEXT("PARTY_TENDER_POST_TITLE")),
					std::string(ID2SERVERTEXT("PARTY_TENDER_POST_CONTENT"))
					);

				if ( bResult == false )
					return false; // ���� ������ ����;			
			}
		}		
		
		const bool bResult = bWin ? nextStage() : reset();
		if ( bResult == false )
			return false;

		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), &GLMSG::NET_MINIGAME_ODDEVEN_DO_RECEIVE_REWARD());

		return true;
	}


	const bool GLOddEvenField::doBegin(void)
	{
		GLChar* const pPlayer = m_pGaeaServer->GetChar(m_dwOwnerGaeaID);
		if ( pPlayer == NULL )
			return false; // ��ȿ���� ���� �ɸ��Ͷ�� ����;

		MiniGame::OddEvenSettings* const pSettings = MiniGame::OddEvenSettings::getInstance();

		GLMSG::NET_MINIGAME_ODDEVEN_SETTINGS settingsMessage;
		settingsMessage.nMaxWin = pSettings->getMaxWin();
		settingsMessage.nRequireLevel = pSettings->getRequireLevel();
		settingsMessage.nRequireMoney = pSettings->getRequireMoney();
		settingsMessage.fTimeLimit = pSettings->getTimeLimit();
		settingsMessage.fAnimateTime = pSettings->getAnimateTime();
		settingsMessage.fAnimateIdleTime = pSettings->getAnimateIdleTime();
		settingsMessage.bLoseReward = pSettings->getLoseReward();
		for ( DWORD _i = settingsMessage.nMaxWin; _i--; )
			settingsMessage.nCostMoney[_i] = pSettings->getCostMoney(_i);

		m_pGaeaServer->SENDTOCLIENT(pPlayer->ClientSlot(), &settingsMessage);

		pPlayer->SetMovement(CHAR_MOVEMENT_STATE_MINIGAME_ODDEVEN, false);		

		return true;
	}

	const bool GLOddEvenField::doEnd(void)
	{
		GLChar* const pPlayer = m_pGaeaServer->GetChar(m_dwOwnerGaeaID);
		if ( pPlayer == NULL )
			return false; // ��ȿ���� ���� �ɸ��Ͷ�� ����;

		pPlayer->SetMovement(CHAR_MOVEMENT_STATE_MINIGAME_ODDEVEN, true);

		return true;
	}
	const bool GLOddEvenField::doMessage(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID)
	{
		GLChar* const pPlayer = m_pGaeaServer->GetChar(m_dwOwnerGaeaID);
		if ( pPlayer == NULL )
			return false;

		GLLandMan* const pLand = pPlayer->GetLandMan();
		if ( pLand == NULL )
			return false;

		if ( pPlayer->UserLevel() < USER_GM3 )
		{
			GLCrow* const pNPC = pLand->GetCrow(m_dwNPCGaeaID);
			if ( pNPC == NULL )
				return false;
			
            if ( !pNPC->IsNpcInteractionAble(pPlayer->GetPosition(), pPlayer->GETBODYRADIUS()))
            {
                return false;
            }
		}		

		switch ( pMessage->nSubType )
		{
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_BEGIN:
			return _doMessageRequestBegin(pMessage, dwGaeaID);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_SELECT_ODDEVEN:
			return _doMessageRequestSelectOddEven(pMessage, dwGaeaID);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_ROLL:
			return _doMessageRequestRoll(pMessage, dwGaeaID);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_RESULT:
			return _doMessageRequestResult(pMessage, dwGaeaID);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_REWARD:
			return _doMessageRequestReward(pMessage, dwGaeaID);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_SELECT_REWARD:
			return _doMessageRequestSelectReward(pMessage, dwGaeaID);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_RECEIVE_REWARD:
			return _doMessageRequestReceiveReward(pMessage, dwGaeaID);
		}

		return false;
	}
}
