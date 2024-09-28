#pragma once

#include "../MiniGame/OddEven/GLOddEvenDefine.h"
#include "GLContrlBaseMsg.h"

namespace MiniGame
{
	enum EMMESSAGE_TYPE
	{
		// 미니게임 시스템;
		EMMESSAGE_REQUEST_OPEN, // 미니게임 생성 요청 -> 서버;
		EMMESSAGE_DO_OPEN, // 미니 게임 생성 성공/실패 -> 클라이언트;
		EMMESSAGE_REQUEST_CLOSE, // 미니게임 닫기 요청 -> 서버;
		EMMESSAGE_DO_CLOSE, // 미니게임 닫기 성공/실패 -> 클라이언트;

		// 홀짝 게임;
		EMMESSAGE_ODDEVEN_SETTINGS, // 홀짝 게임 설정 -> 클라이언트;
		EMMESSAGE_ODDEVEN_REQUEST_BEGIN, // 미니 게임 시작 요청 -> 서버;
		EMMESSAGE_ODDEVEN_DO_BEGIN, // 미니 게임 시작 성공/실패 -> 클라이언트;
		EMMESSAGE_ODDEVEN_REQUEST_SELECT_ODDEVEN, // 홀짝 선택 요청 -> 서버;
		EMMESSAGE_ODDEVEN_DO_SELECT_ODDEVEN, // 홀짝 선택 성공/실패 -> 클라이언트;
		EMMESSAGE_ODDEVEN_REQUEST_ROLL, // 굴리기 요청 -> 서버;
		EMMESSAGE_ODDEVEN_DO_ROLL, // 굴리기 성공/실패 -> 클라이언트;
		EMMESSAGE_ODDEVEN_REQUEST_RESULT, // 결과 요청 -> 서버;
		EMMESSAGE_ODDEVEN_DO_RESULT, // 결과 요청 알림 -> 클라이언트;
		EMMESSAGE_ODDEVEN_REQUEST_REWARD, // 보상 화면 전환 요청 -> 서버;
		EMMESSAGE_ODDEVEN_DO_REWARD, // 보상 화면 전황 요청에 대한 결과 알림 -> 클라이언트;
		EMMESSAGE_ODDEVEN_REQUEST_SELECT_REWARD, // 보상 선택 요청 -> 서버;
		EMMESSAGE_ODDEVEN_DO_SELECT_REWARD, // 보상 성택에 대한 결과 알림 -> 클라이언트;
		EMMESSAGE_ODDEVEN_REQUEST_RECEIVE_REWARD, // 보상 받기 -> 서버;
		EMMESSAGE_ODDEVEN_DO_RECEIVE_REWARD, // 보상 받기 -> 클라이언트;

		// 아래로 추가;

		EMMESSAGE_NSIZE,
	};

	/*
	 * NO_ERROR가 아닌 모든 경우에 대해 미니게임을 종료 하므로;
	 * 비단 홀짝 게임만이 아닌 공용 에러 메세지로 사용 함;
	 */
	enum EMERROR_TYPE
	{
		EMERROR_END, // 종료 코드(절대 위치 바꾸지 말것);
		EMERROR_NO_ERROR,		
		EMERROR_NO_LAND, // 유효하지 않은 지역;
		EMERROR_NO_NPC, // 유효하지 않은 NPC;
		EMERROR_NO_ACTIONABLE, // 범위 혹은 유효하지 않은 NPC 데이타;
		EMERROR_ALREADY_BEGUN, // 실행 중;
		EMERROR_CREATE_FAIL, // 미니 게임 생성 실패;
		EMERROR_NO_CREATED,  // 생성 되 있지 않음;
		EMERROR_DIFFERENT_ID, // 유효하지 않은 ID;
		EMERROR_PROCESS_ERROR, // doMessage() error;
		EMERROR_NOT_ENOUGH_MONEY, // 소지 금액 부족;
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



	// 홀짝 게임;
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
		DWORD nRequireLevel; // 플레이 최소 요구 레벨;
		DWORD nRequireMoney; // 플레이 최소 요구 게임머니;
		DWORD nMaxWin; // 최대 연승 단계;
		float fTimeLimit; // 보상 선택 제한 시간;
		float fAnimateTime; // 보상 애니메이션 진행 시간;
		float fAnimateIdleTime; // 보상 애니메이션 간 대기 시간;
		bool bLoseReward; // 패배시 보상 지급 유/무;
		DWORD nCostMoney[64]; // 스테이지 당 소모 금액;

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
		const bool bWin; // bLoseReward 값에 따라 졌어도 보상 받기 단계로갈 수 있음;

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
