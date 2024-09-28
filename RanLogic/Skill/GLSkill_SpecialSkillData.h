
#pragma once

namespace SKILL
{
	//	소환 값 래밸별 설정.
	struct SCREATURE_LVL
	{
		SNATIVEID			sID;                // 소환수 ID
		FLOAT				fSEC_RECALLDELAY;   // 소환 딜레이 초
		FLOAT				fSEC_ACTIVETIME;    // 소환수 활동 시간
		DWORD               dwMAX;              // 최대 소환 갯수
		WORD                wUSESKILLLEVEL;     // 소환수가 사용할 스킬 레벨
		DWORD				dwType;				// 소환 타입;
		bool				bEventSkill;		// 이벤트 스킬 반응 여부;

		SCREATURE_LVL (void);
	};
};