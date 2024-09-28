#pragma once

#include "../MiniGame/OddEven/GLOddEvenDefine.h"
#include "GLContrlBaseMsg.h"

namespace MiniGame
{
	enum EMMESSAGE_TYPE
	{
		// �̴ϰ��� �ý���;
		EMMESSAGE_REQUEST_OPEN, // �̴ϰ��� ���� ��û -> ����;
		EMMESSAGE_DO_OPEN, // �̴� ���� ���� ����/���� -> Ŭ���̾�Ʈ;
		EMMESSAGE_REQUEST_CLOSE, // �̴ϰ��� �ݱ� ��û -> ����;
		EMMESSAGE_DO_CLOSE, // �̴ϰ��� �ݱ� ����/���� -> Ŭ���̾�Ʈ;

		// Ȧ¦ ����;
		EMMESSAGE_ODDEVEN_SETTINGS, // Ȧ¦ ���� ���� -> Ŭ���̾�Ʈ;
		EMMESSAGE_ODDEVEN_REQUEST_BEGIN, // �̴� ���� ���� ��û -> ����;
		EMMESSAGE_ODDEVEN_DO_BEGIN, // �̴� ���� ���� ����/���� -> Ŭ���̾�Ʈ;
		EMMESSAGE_ODDEVEN_REQUEST_SELECT_ODDEVEN, // Ȧ¦ ���� ��û -> ����;
		EMMESSAGE_ODDEVEN_DO_SELECT_ODDEVEN, // Ȧ¦ ���� ����/���� -> Ŭ���̾�Ʈ;
		EMMESSAGE_ODDEVEN_REQUEST_ROLL, // ������ ��û -> ����;
		EMMESSAGE_ODDEVEN_DO_ROLL, // ������ ����/���� -> Ŭ���̾�Ʈ;
		EMMESSAGE_ODDEVEN_REQUEST_RESULT, // ��� ��û -> ����;
		EMMESSAGE_ODDEVEN_DO_RESULT, // ��� ��û �˸� -> Ŭ���̾�Ʈ;
		EMMESSAGE_ODDEVEN_REQUEST_REWARD, // ���� ȭ�� ��ȯ ��û -> ����;
		EMMESSAGE_ODDEVEN_DO_REWARD, // ���� ȭ�� ��Ȳ ��û�� ���� ��� �˸� -> Ŭ���̾�Ʈ;
		EMMESSAGE_ODDEVEN_REQUEST_SELECT_REWARD, // ���� ���� ��û -> ����;
		EMMESSAGE_ODDEVEN_DO_SELECT_REWARD, // ���� ���ÿ� ���� ��� �˸� -> Ŭ���̾�Ʈ;
		EMMESSAGE_ODDEVEN_REQUEST_RECEIVE_REWARD, // ���� �ޱ� -> ����;
		EMMESSAGE_ODDEVEN_DO_RECEIVE_REWARD, // ���� �ޱ� -> Ŭ���̾�Ʈ;

		// �Ʒ��� �߰�;

		EMMESSAGE_NSIZE,
	};

	/*
	 * NO_ERROR�� �ƴ� ��� ��쿡 ���� �̴ϰ����� ���� �ϹǷ�;
	 * ��� Ȧ¦ ���Ӹ��� �ƴ� ���� ���� �޼����� ��� ��;
	 */
	enum EMERROR_TYPE
	{
		EMERROR_END, // ���� �ڵ�(���� ��ġ �ٲ��� ����);
		EMERROR_NO_ERROR,		
		EMERROR_NO_LAND, // ��ȿ���� ���� ����;
		EMERROR_NO_NPC, // ��ȿ���� ���� NPC;
		EMERROR_NO_ACTIONABLE, // ���� Ȥ�� ��ȿ���� ���� NPC ����Ÿ;
		EMERROR_ALREADY_BEGUN, // ���� ��;
		EMERROR_CREATE_FAIL, // �̴� ���� ���� ����;
		EMERROR_NO_CREATED,  // ���� �� ���� ����;
		EMERROR_DIFFERENT_ID, // ��ȿ���� ���� ID;
		EMERROR_PROCESS_ERROR, // doMessage() error;
		EMERROR_NOT_ENOUGH_MONEY, // ���� �ݾ� ����;
		EMERROR_NSIZE,
	};
}

namespace GLMSG
{
	struct NET_MINIGAME_MESSAGE : public NET_MSG_GENERIC
	{
		const DWORD nSubType;
		const DWORD dwMiniGameID;

		NET_MINIGAME_MESSAGE(const DWORD nSize, const DWORD _nSubType, const DWORD _dwMiniGameID)
			: NET_MSG_GENERIC(NET_MSG_GCTRL_MINIGAME, nSize)
			, nSubType(_nSubType)
			, dwMiniGameID(_dwMiniGameID)
		{
		}
	};


	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////


	struct NET_MINIGAME_REQUEST_OPEN : public NET_MINIGAME_MESSAGE
	{
		const SNATIVEID NPCID;
		const DWORD dwNPCGaeaID;

		NET_MINIGAME_REQUEST_OPEN(const DWORD dwMiniGameID, const SNATIVEID& _NPCID, const DWORD _dwNPCGaeaID)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_REQUEST_OPEN), MiniGame::EMMESSAGE_REQUEST_OPEN, dwMiniGameID)
			, NPCID(_NPCID)
			, dwNPCGaeaID(_dwNPCGaeaID)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_REQUEST_OPEN) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_REQUEST_CLOSE: public NET_MINIGAME_MESSAGE
	{
		NET_MINIGAME_REQUEST_CLOSE(void)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_REQUEST_CLOSE), MiniGame::EMMESSAGE_REQUEST_CLOSE, MiniGame::EMMINIGAME_NSIZE)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_REQUEST_CLOSE) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_DO_CLOSE : public NET_MINIGAME_MESSAGE
	{
		const MiniGame::EMERROR_TYPE emError;
		NET_MINIGAME_DO_CLOSE(const DWORD dwMiniGameID, const MiniGame::EMERROR_TYPE _emError)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_DO_CLOSE), MiniGame::EMMESSAGE_DO_CLOSE, dwMiniGameID)
			, emError(_emError)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_DO_CLOSE) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_DO_OPEN : public NET_MINIGAME_MESSAGE
	{
		const SNATIVEID NPCID;
		const DWORD dwNPCGaeaID;		
		const float fBeginTime;
		const MiniGame::EMERROR_TYPE emError;

		NET_MINIGAME_DO_OPEN(const DWORD dwMiniGameID, const SNATIVEID _NPCID, const DWORD _dwNPCGaeaID, const float _fBeginTime, const MiniGame::EMERROR_TYPE _emError)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_DO_OPEN), MiniGame::EMMESSAGE_DO_OPEN, dwMiniGameID)
			, NPCID(_NPCID)
			, dwNPCGaeaID(_dwNPCGaeaID)
			, fBeginTime(_fBeginTime)
			, emError(_emError)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_DO_OPEN) <= NET_DATA_BUFSIZE);
		}
		NET_MINIGAME_DO_OPEN(const DWORD dwMiniGameID, const MiniGame::EMERROR_TYPE _emError)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_DO_OPEN), MiniGame::EMMESSAGE_DO_OPEN, dwMiniGameID)
			, NPCID(SNATIVEID(false))
			, dwNPCGaeaID(GAEAID_NULL)
			, fBeginTime(0.0f)
			, emError(_emError)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_DO_OPEN) <= NET_DATA_BUFSIZE);
		}
	};



	// Ȧ¦ ����;
	struct NET_MINIGAME_ODDEVEN_REQUEST_BEGIN : public NET_MINIGAME_MESSAGE
	{
		NET_MINIGAME_ODDEVEN_REQUEST_BEGIN(void)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_BEGIN), MiniGame::EMMESSAGE_ODDEVEN_REQUEST_BEGIN, MiniGame::EMMINIGAME_ODDEVEN)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_BEGIN) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_ODDEVEN_SETTINGS : public NET_MINIGAME_MESSAGE
	{
		DWORD nRequireLevel; // �÷��� �ּ� �䱸 ����;
		DWORD nRequireMoney; // �÷��� �ּ� �䱸 ���ӸӴ�;
		DWORD nMaxWin; // �ִ� ���� �ܰ�;
		float fTimeLimit; // ���� ���� ���� �ð�;
		float fAnimateTime; // ���� �ִϸ��̼� ���� �ð�;
		float fAnimateIdleTime; // ���� �ִϸ��̼� �� ��� �ð�;
		bool bLoseReward; // �й�� ���� ���� ��/��;
		DWORD nCostMoney[64]; // �������� �� �Ҹ� �ݾ�;

		NET_MINIGAME_ODDEVEN_SETTINGS(void)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_SETTINGS), MiniGame::EMMESSAGE_ODDEVEN_SETTINGS, MiniGame::EMMINIGAME_ODDEVEN)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_SETTINGS) <= NET_DATA_BUFSIZE);
		}
	};
	struct NET_MINIGAME_ODDEVEN_DO_BEGIN : public NET_MINIGAME_MESSAGE
	{
		const DWORD nCurrentStep;		
		const LONGLONG nCostMoney;

		NET_MINIGAME_ODDEVEN_DO_BEGIN(const DWORD _nCurrentStep, const LONGLONG _nCostMoney)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_DO_BEGIN), MiniGame::EMMESSAGE_ODDEVEN_DO_BEGIN, MiniGame::EMMINIGAME_ODDEVEN)
			, nCurrentStep(_nCurrentStep)
			, nCostMoney(_nCostMoney)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_DO_BEGIN) <= NET_DATA_BUFSIZE);
		}
	};


	struct NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN : public NET_MINIGAME_MESSAGE
	{
		const MiniGame::EMODDEVEN emOddEven;

		NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN(const MiniGame::EMODDEVEN _emOddEven)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN), MiniGame::EMMESSAGE_ODDEVEN_REQUEST_SELECT_ODDEVEN, MiniGame::EMMINIGAME_ODDEVEN)
			, emOddEven(_emOddEven)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_ODDEVEN_DO_SELECT_ODDEVEN : public NET_MINIGAME_MESSAGE
	{
		const MiniGame::EMODDEVEN emOddEven;

		NET_MINIGAME_ODDEVEN_DO_SELECT_ODDEVEN(const MiniGame::EMODDEVEN _emOddEven)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_DO_SELECT_ODDEVEN), MiniGame::EMMESSAGE_ODDEVEN_DO_SELECT_ODDEVEN, MiniGame::EMMINIGAME_ODDEVEN)
			, emOddEven(_emOddEven)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_DO_SELECT_ODDEVEN) <= NET_DATA_BUFSIZE);
		}
	};

	

	struct NET_MINIGAME_ODDEVEN_REQUEST_ROLL : public NET_MINIGAME_MESSAGE
	{
		const MiniGame::EMODDEVEN emOddEven;
		const DWORD nCurrentStep;

		NET_MINIGAME_ODDEVEN_REQUEST_ROLL(const MiniGame::EMODDEVEN _emOddEven = MiniGame::EMODDEVEN_NOT_SELECTED, const DWORD _nCurrentStep = 0)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_ROLL), MiniGame::EMMESSAGE_ODDEVEN_REQUEST_ROLL, MiniGame::EMMINIGAME_ODDEVEN)
			, emOddEven(_emOddEven)
			, nCurrentStep(_nCurrentStep)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_ROLL) <= NET_DATA_BUFSIZE);
		}		
	};

	struct NET_MINIGAME_ODDEVEN_DO_ROLL : public NET_MINIGAME_MESSAGE
	{
		DWORD nDetermineNumber[MiniGame::EMODDEVEN_MAX_DICE];

		NET_MINIGAME_ODDEVEN_DO_ROLL(const DWORD _nDetermineNumber[MiniGame::EMODDEVEN_MAX_DICE])
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_DO_ROLL), MiniGame::EMMESSAGE_ODDEVEN_DO_ROLL, MiniGame::EMMINIGAME_ODDEVEN)
		{
			for ( DWORD _i = MiniGame::EMODDEVEN_MAX_DICE; _i; --_i )
				nDetermineNumber[_i - 1] = _nDetermineNumber[_i - 1];

			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_DO_ROLL) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_ODDEVEN_REQUEST_RESULT : public NET_MINIGAME_MESSAGE
	{
		NET_MINIGAME_ODDEVEN_REQUEST_RESULT(void)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_RESULT), MiniGame::EMMESSAGE_ODDEVEN_REQUEST_RESULT, MiniGame::EMMINIGAME_ODDEVEN)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_RESULT) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_ODDEVEN_DO_RESULT : public NET_MINIGAME_MESSAGE
	{
		const MiniGame::EMODDEVEN emSelected;
		const MiniGame::EMODDEVEN emResult;

		NET_MINIGAME_ODDEVEN_DO_RESULT(const MiniGame::EMODDEVEN _emSelected, const MiniGame::EMODDEVEN _emResult)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_DO_RESULT), MiniGame::EMMESSAGE_ODDEVEN_DO_RESULT, MiniGame::EMMINIGAME_ODDEVEN)
			, emSelected(_emSelected)
			, emResult(_emResult)			
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_DO_RESULT) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_ODDEVEN_REQUEST_REWARD : public NET_MINIGAME_MESSAGE
	{
		const MiniGame::EMODDEVEN emResult;

		NET_MINIGAME_ODDEVEN_REQUEST_REWARD(const MiniGame::EMODDEVEN _emResult = MiniGame::EMODDEVEN_NOT_SELECTED)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_REWARD), MiniGame::EMMESSAGE_ODDEVEN_REQUEST_REWARD, MiniGame::EMMINIGAME_ODDEVEN)
			, emResult(_emResult)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_REWARD) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_ODDEVEN_DO_REWARD : public NET_MINIGAME_MESSAGE
	{
		const MiniGame::EMODDEVEN_STAGE emNextStage;
		const bool bWin; // bLoseReward ���� ���� ��� ���� �ޱ� �ܰ�ΰ� �� ����;

		NET_MINIGAME_ODDEVEN_DO_REWARD(const MiniGame::EMODDEVEN_STAGE _emNextStage, const bool _bWin)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_DO_REWARD), MiniGame::EMMESSAGE_ODDEVEN_DO_REWARD, MiniGame::EMMINIGAME_ODDEVEN)
			, emNextStage(_emNextStage)
			, bWin(_bWin)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_DO_REWARD) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD : public NET_MINIGAME_MESSAGE
	{
		const DWORD nItemIndex;

		NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD(const DWORD _nItemIndex = 0)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD), MiniGame::EMMESSAGE_ODDEVEN_REQUEST_SELECT_REWARD, MiniGame::EMMINIGAME_ODDEVEN)
			, nItemIndex(_nItemIndex)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_ODDEVEN_DO_SELECT_REWARD : public NET_MINIGAME_MESSAGE
	{
		SNATIVEID ItemID[MiniGame::EMODDEVEN_MAX_REWARD];
		const DWORD nStage;
		const DWORD nItemIndex;

		NET_MINIGAME_ODDEVEN_DO_SELECT_REWARD(const DWORD _nStage, const DWORD _nItemIndex, const SNATIVEID _ItemID[MiniGame::EMODDEVEN_MAX_REWARD])
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_DO_SELECT_REWARD), MiniGame::EMMESSAGE_ODDEVEN_DO_SELECT_REWARD, MiniGame::EMMINIGAME_ODDEVEN)
			, nStage(_nStage)
			, nItemIndex(_nItemIndex)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_DO_SELECT_REWARD) <= NET_DATA_BUFSIZE);
			for ( DWORD _i = MiniGame::EMODDEVEN_MAX_REWARD; _i; --_i )
			{
				ItemID[_i - 1] = _ItemID[_i - 1];
			}
		}
	};

	struct NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD : public NET_MINIGAME_MESSAGE
	{
		NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD(void)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD), MiniGame::EMMESSAGE_ODDEVEN_REQUEST_RECEIVE_REWARD, MiniGame::EMMINIGAME_ODDEVEN)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD) <= NET_DATA_BUFSIZE);
		}
	};

	struct NET_MINIGAME_ODDEVEN_DO_RECEIVE_REWARD : public NET_MINIGAME_MESSAGE
	{
		NET_MINIGAME_ODDEVEN_DO_RECEIVE_REWARD(void)
			: NET_MINIGAME_MESSAGE(sizeof(NET_MINIGAME_ODDEVEN_DO_RECEIVE_REWARD), MiniGame::EMMESSAGE_ODDEVEN_DO_RECEIVE_REWARD, MiniGame::EMMINIGAME_ODDEVEN)
		{
			MIN_STATIC_ASSERT(sizeof(NET_MINIGAME_ODDEVEN_DO_RECEIVE_REWARD) <= NET_DATA_BUFSIZE);
		}
	};

}
