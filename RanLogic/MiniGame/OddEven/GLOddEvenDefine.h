#pragma once

#include "../IMiniGame.h"

namespace MiniGame
{
	enum EMODDEVEN
	{
		EMODDEVEN_MAX_DICE = 2,
		EMODDEVEN_MAX_REWARD = 4,
		EMODDEVEN_REWARD_FAIL = -1, // 꽝인 경우 보상 아이템 인덱스;
		EMODDEVEN_REWARD_PRECISION = 100000, // 아이템 테이블 정밀도;

		EMODDEVEN_NOT_SELECTED = 0, // 선택 되지 않음;		
		EMODDEVEN_EVEN = 1, // 짝수;
		EMODDEVEN_ODD = 2, // 홀수;
		EMODDEVEN_NSIZE = 3, // 유효성 검사용;
	};

	enum EMODDEVEN_STAGE
	{
		EMODDEVEN_STAGE_READY, // "진행한다/연승도전" 누르기 전 단계;
		EMODDEVEN_STAGE_SELECT, // 홀수/짝수 선택 후 "굴리기" 전 단계;
		EMODDEVEN_STAGE_ROLL, // 굴리는 중;
		EMODDEVEN_STAGE_RESULT, // 결과 보기;
		EMODDEVEN_STAGE_SELECT_REWARD, // 보상 선택, 버튼은 비활성화;
		EMODDEVEN_STAGE_RECEIVE_REWARD, // 보상 받기;
		EMODDEVEN_STAGE_NSIZE,
	};	
}