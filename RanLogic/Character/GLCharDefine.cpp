#include "../pch.h"

#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"

#include "./GLCharDefine.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

WORD school2index ( const EMSCHOOLFLAG emSchool )
{
	switch ( emSchool )
	{
	case GLSCHOOL_00:	return 0;
	case GLSCHOOL_01:	return 1;
	case GLSCHOOL_02:	return 2;
	};

	return 0;
}

EMSCHOOLFLAG index2school ( const WORD wSchool )
{
	switch ( wSchool )
	{
	case 0:	return GLSCHOOL_00;
	case 1:	return GLSCHOOL_01;
	case 2:	return GLSCHOOL_02;
	};

	return GLSCHOOL_00;
}

EMCLASSINDEX CharClassToClassIndex ( const EMCHARCLASS emClass )
{
	switch (emClass)
	{
	case GLCC_FIGHTER_M:
	case GLCC_FIGHTER_W:
	case GLCC_FIGHTER:
		return GLCL_FIGHTER;

	case GLCC_ARMS_M:
	case GLCC_ARMS_W:
	case GLCC_ARMS:
		return GLCL_ARMS;

	case GLCC_ARCHER_M:
	case GLCC_ARCHER_W:
	case GLCC_ARCHER:
		return GLCL_ARCHER;	

	case GLCC_SPIRIT_M:
	case GLCC_SPIRIT_W:
	case GLCC_SPIRIT:
		return GLCL_SPIRIT;

	case GLCC_EXTREME_M:
	case GLCC_EXTREME_W:
	case GLCC_EXTREME:
		return GLCL_EXTREME;

	case GLCC_SCIENTIST_M:
	case GLCC_SCIENTIST_W:
	case GLCC_SCIENTIST:
		return GLCL_SCIENTIST;

    case GLCC_ASSASSIN_M:
    case GLCC_ASSASSIN_W:
	case GLCC_ASSASSIN:
		return GLCL_ASSASSIN;

	case GLCC_TRICKER_M:
	case GLCC_TRICKER_W:
	case GLCC_TRICKER:
		return GLCL_TRICKER;

    case GLCC_ETC_M:
    case GLCC_ETC_W:
	case GLCC_ETC:
		return GLCL_ETC;

	case GLCC_ACTOR_M:
	case GLCC_ACTOR_W:
	case GLCC_ACTOR:
		return GLCL_ACTOR;
	};

	return GLCL_FIGHTER;
}

EMCHARINDEX CharClassToIndex ( const EMCHARCLASS emClass )
{
	switch (emClass)
	{
	case GLCC_FIGHTER_M:		return GLCI_FIGHTER_M;
	case GLCC_ARMS_M:		return GLCI_ARMS_M;
	case GLCC_ARCHER_W:		return GLCI_ARCHER_W;
	case GLCC_SPIRIT_W:		return GLCI_SPIRIT_W;
	case GLCC_EXTREME_M:	return GLCI_EXTREME_M;
	case GLCC_EXTREME_W:	return GLCI_EXTREME_W;
	
	case GLCC_FIGHTER_W:	return GLCI_FIGHTER_W;
	case GLCC_ARMS_W:		return GLCI_ARMS_W;
	case GLCC_ARCHER_M:		return GLCI_ARCHER_M;
	case GLCC_SPIRIT_M:		return GLCI_SPIRIT_M;
	case GLCC_SCIENTIST_M:	return GLCI_SCIENTIST_M;
	case GLCC_SCIENTIST_W:	return GLCI_SCIENTIST_W;

    case GLCC_ASSASSIN_M:	return GLCI_ASSASSIN_M;
    case GLCC_ASSASSIN_W:	return GLCI_ASSASSIN_W;
	case GLCC_TRICKER_M:		return GLCI_TRICKER_M;
	case GLCC_TRICKER_W:	return GLCI_TRICKER_W;
    case GLCC_ETC_M:			return GLCI_ETC_M;
    case GLCC_ETC_W:			return GLCI_ETC_W;
	case GLCC_ACTOR_M:		return GLCI_ACTOR_M;
	case GLCC_ACTOR_W:		return GLCI_ACTOR_W;
	};


	sc::writeLogError(sc::string::format("CharClassToIndex() - 잘못된 캐릭터 클래스입니다.(%1%)",emClass));	
	GASSERT(0&&"잘못된 케릭터 클래스입니다.");

	return GLCI_FIGHTER_M;
}

EMCHARCLASS CharIndexToClass ( const EMCHARINDEX emIndex )
{
	switch (emIndex)
	{
	case GLCI_FIGHTER_M:		return GLCC_FIGHTER_M;
	case GLCI_ARMS_M:			return GLCC_ARMS_M;
	case GLCI_ARCHER_W:		return GLCC_ARCHER_W;
	case GLCI_SPIRIT_W:		return GLCC_SPIRIT_W;

	case GLCI_EXTREME_M:	return GLCC_EXTREME_M;
	case GLCI_EXTREME_W:	return GLCC_EXTREME_W;

	case GLCI_FIGHTER_W:		return GLCC_FIGHTER_W;
	case GLCI_ARMS_W:			return GLCC_ARMS_W;
	case GLCI_ARCHER_M:		return GLCC_ARCHER_M;
	case GLCI_SPIRIT_M:		return GLCC_SPIRIT_M;
	case GLCI_SCIENTIST_M:	return GLCC_SCIENTIST_M;
	case GLCI_SCIENTIST_W:	return GLCC_SCIENTIST_W;

    case GLCI_ASSASSIN_M:	return GLCC_ASSASSIN_M;
    case GLCI_ASSASSIN_W:	return GLCC_ASSASSIN_W;
	case GLCI_TRICKER_M:		return GLCC_TRICKER_M;
	case GLCI_TRICKER_W:		return GLCC_TRICKER_W;

    case GLCI_ETC_M:			return GLCC_ETC_M;
    case GLCI_ETC_W:			return GLCC_ETC_W;

	case GLCI_ACTOR_M:		return GLCC_ACTOR_M;
	case GLCI_ACTOR_W:		return GLCC_ACTOR_W;
	};

	sc::writeLogError(sc::string::format("CharIndexToClass() - 잘못된 캐릭터 인덱스입니다.(%1%)",emIndex));
	GASSERT(0&&"잘못된 케릭터 인덱스입니다.");

	return GLCC_FIGHTER_M;
}


EMCHARCLASS CharClassToSex ( const EMCHARCLASS emClass )
{
	switch (emClass)
	{
	case GLCC_FIGHTER_M:		return GLCC_FIGHTER_W;
	case GLCC_ARMS_M:		return GLCC_ARMS_W;
	case GLCC_ARCHER_W:		return GLCC_ARCHER_M;
	case GLCC_SPIRIT_W:		return GLCC_SPIRIT_M;
	case GLCC_EXTREME_M:	return GLCC_EXTREME_W;
	case GLCC_EXTREME_W:	return GLCC_EXTREME_M;
	
	case GLCC_FIGHTER_W:	return GLCC_FIGHTER_M;
	case GLCC_ARMS_W:		return GLCC_ARMS_M;
	case GLCC_ARCHER_M:		return GLCC_ARCHER_W;
	case GLCC_SPIRIT_M:		return GLCC_SPIRIT_W;
	case GLCC_SCIENTIST_M:	return GLCC_SCIENTIST_W;
	case GLCC_SCIENTIST_W:	return GLCC_SCIENTIST_M;

    case GLCC_ASSASSIN_M:	return GLCC_ASSASSIN_W;
    case GLCC_ASSASSIN_W:	return GLCC_ASSASSIN_M;
	case GLCC_TRICKER_M:		return GLCC_TRICKER_W;
	case GLCC_TRICKER_W:	return GLCC_TRICKER_M;
    case GLCC_ETC_M:			return GLCC_ETC_W;
    case GLCC_ETC_W:			return GLCC_ETC_M;
	case GLCC_ACTOR_M:		return GLCC_ACTOR_W;
	case GLCC_ACTOR_W:		return GLCC_ACTOR_M;
	};

	sc::writeLogError(sc::string::format("CharClassToSex() - 잘못된 캐릭터 클래스입니다.(%1%)",emClass));	
	GASSERT(0&&"잘못된 케릭터 클래스입니다.");

	return GLCC_FIGHTER_M;
}

EMCHARINDEX CharIndexToSex ( const EMCHARINDEX emIndex )
{
	switch (emIndex)
	{
	case GLCI_FIGHTER_M:		return GLCI_FIGHTER_W;
	case GLCI_ARMS_M:			return GLCI_ARMS_W;
	case GLCI_ARCHER_W:		return GLCI_ARCHER_M;
	case GLCI_SPIRIT_W:		return GLCI_SPIRIT_M;

	case GLCI_EXTREME_M:	return GLCI_EXTREME_W;
	case GLCI_EXTREME_W:	return GLCI_EXTREME_M;

	case GLCI_FIGHTER_W:		return GLCI_FIGHTER_M;
	case GLCI_ARMS_W:			return GLCI_ARMS_M;
	case GLCI_ARCHER_M:		return GLCI_ARCHER_W;
	case GLCI_SPIRIT_M:		return GLCI_SPIRIT_W;
	case GLCI_SCIENTIST_M:	return GLCI_SCIENTIST_W;
	case GLCI_SCIENTIST_W:	return GLCI_SCIENTIST_M;

    case GLCI_ASSASSIN_M:	return GLCI_ASSASSIN_W;
    case GLCI_ASSASSIN_W:	return GLCI_ASSASSIN_M;
	case GLCI_TRICKER_M:		return GLCI_TRICKER_W;
	case GLCI_TRICKER_W:		return GLCI_TRICKER_M;
    case GLCI_ETC_M:			return GLCI_ETC_W;
    case GLCI_ETC_W:			return GLCI_ETC_M;
	case GLCI_ACTOR_M:		return GLCI_ACTOR_W;
	case GLCI_ACTOR_W:		return GLCI_ACTOR_M;
	};
	
	sc::writeLogError(sc::string::format("CharIndexToSex() - 잘못된 캐릭터 인덱스입니다.(%1%)",emIndex));
	GASSERT(0&&"잘못된 케릭터 인덱스입니다.");

	return GLCI_FIGHTER_M;
}

EMCHARCLASS CharSexandClassIndexToCharClass( const int nSex, const EMCLASSINDEX emclassIndex )
{
	switch (emclassIndex)
	{
	case GLCL_FIGHTER: // 격투부
		{
			if ( nSex == 1 )
				return GLCC_FIGHTER_M;
			else if (nSex == 2)
				return GLCC_FIGHTER_W;
			else
				return GLCC_FIGHTER;
		}
		break;
	case GLCL_ARMS: // 검도부
		{
			if ( nSex == 1 )
				return GLCC_ARMS_M;
			else if (nSex == 2)
				return GLCC_ARMS_W;
			else
				return GLCC_ARMS;
		}
		break;
	case GLCL_ARCHER: // 양궁부
		{
			if ( nSex == 1 )
				return GLCC_ARCHER_M;
			else if (nSex == 2)
				return GLCC_ARCHER_W;
			else
				return GLCC_ARCHER;
		}
		break;
	case GLCL_SPIRIT: // 기예부
		{
			if ( nSex == 1 )
				return GLCC_SPIRIT_M;
			else if (nSex == 2)
				return GLCC_SPIRIT_W;
			else
				return GLCC_SPIRIT;
		}
		break;
	case GLCL_EXTREME: // 극강부
		{
			if ( nSex == 1 )
				return GLCC_EXTREME_M;
			else if (nSex == 2)
				return GLCC_EXTREME_W;
			else
				return GLCC_EXTREME;
		}
		break;
	case GLCL_SCIENTIST: // 과학부
		{
			if ( nSex == 1 )
				return GLCC_SCIENTIST_M;
			else if (nSex == 2)
				return GLCC_SCIENTIST_W;
			else
				return GLCC_SCIENTIST;
		}
		break;
	case GLCL_ASSASSIN: // 인술부
		{
			if ( nSex == 1 )
				return GLCC_ASSASSIN_M;
			else if (nSex == 2)
				return GLCC_ASSASSIN_W;
			else
				return GLCC_ASSASSIN;
		}
		break;
	case GLCL_TRICKER: // 마술부;
		{
			if ( nSex == 1 )
				return GLCC_TRICKER_M;
			else if (nSex == 2)
				return GLCC_TRICKER_W;
			else
				return GLCC_TRICKER;
		}
		break;
	case GLCL_ACTOR:
		{
			if ( nSex == 1 )
				return GLCC_ACTOR_M;
			else if (nSex == 2)
				return GLCC_ACTOR_W;
			else
				return GLCC_ACTOR;
		}
		break;
	default:
		{
			if (nSex == 1)
				return GLCC_MAN;
			else if (nSex == 2)
				return GLCC_WOMAN;
			else
				return GLCC_ALL_NEWSEX;
		}
		break;
	};

		return GLCC_ALL_NEWSEX;
}

VOID GetCharSkillClassIndex( const EMCHARCLASS emClass, WORD & wBeginIndex, WORD & wEndIndex )
{
	switch ( emClass )
	{
	case GLCC_FIGHTER_M:
	case GLCC_FIGHTER_W:
		{
			wBeginIndex = EMSKILL_FIGHTER_01;
			wEndIndex = EMSKILL_FIGHTER_04;
		}
		break;

	case GLCC_ARMS_M:
	case GLCC_ARMS_W:
		{
			wBeginIndex = EMSKILL_ARMS_01;
			wEndIndex = EMSKILL_ARMS_04;
		}
		break;

	case GLCC_ARCHER_M:
	case GLCC_ARCHER_W:
		{
			wBeginIndex = EMSKILL_ARCHER_01;
			wEndIndex = EMSKILL_ARCHER_04;
		}
		break;

	case GLCC_SPIRIT_M:
	case GLCC_SPIRIT_W:
		{
			wBeginIndex = EMSKILL_SPIRIT_01;
			wEndIndex = EMSKILL_SPIRIT_04;
		}
		break;	

	case GLCC_EXTREME_M:
	case GLCC_EXTREME_W:
		{
			wBeginIndex = EMSKILL_EXTREME_01;
			wEndIndex = EMSKILL_EXTREME_04;
		}
		break;

	case GLCC_SCIENTIST_M:
	case GLCC_SCIENTIST_W:
		{
			wBeginIndex = EMSKILL_SCIENTIST_01;
			wEndIndex = EMSKILL_SCIENTIST_04;			
		}
		break;

    case GLCC_ASSASSIN_M:
    case GLCC_ASSASSIN_W:
        {
            wBeginIndex = EMSKILL_ASSASSIN_01;
            wEndIndex = EMSKILL_ASSASSIN_04;
        }
        break;

    case GLCC_TRICKER_M:
    case GLCC_TRICKER_W:
        {
            wBeginIndex = EMSKILL_TRICKER_01;
            wEndIndex = EMSKILL_TRICKER_04;
        }
        break;
	case GLCC_ACTOR_M:
	case GLCC_ACTOR_W:
		{
			// TODO : 임시테스트
			//wBeginIndex = EMSKILL_ACTOR_01;
			//wEndIndex = EMSKILL_ACTOR_04;
			wBeginIndex = EMSKILL_EXTREME_01;
			wEndIndex = EMSKILL_EXTREME_04;
		}
		break;
	default:
		
		sc::writeLogError(sc::string::format("GetCharSkillClassIndex() - 잘못된 캐릭터 클래스입니다.(%1%)",emClass));		
		GASSERT(0&&"잘못된 케릭터 클래스입니다.");

		break;
	};
}

namespace COMMENT
{
	std::string ACTIONTYPE[GLAT_SIZE] =
	{
		"GLAT_IDLE",
		"GLAT_MOVE",
		"GLAT_ATTACK",
		"GLAT_SKILL",
		"GLAT_SHOCK",
		"GLAT_PUSHPULL",
		"GLAT_GATHERING",
		"GLAT_TALK",
		"GLAT_CONFT_END",
		"GLAT_MOTION",

		"GLAT_AUTHED",
		"GLAT_FALLING",
		"GLAT_DIE",
	};

	std::string TRIBE[TRIBE_NSIZE] = 
	{
		"인간",
		"괴물",
		"천사",
		"악마",
		"정령",
		"요정",
		"드래곤"
	};

	std::string MOVETYPE[MOVETYPE_NSIZE] = 
	{
		"지면 이동",
		"공중 이동",
		"수면 이동"
	};

	std::string ELEMENT[EMELEMENT_MAXNUM2] = 
	{
		"정기",
		"불",
		"얼음",
		"전기",
		"독",
		"석화",
		"착란",
		"기절",
		"저주",
		"도구에 종속"
	};

	std::string BRIGHT[BRIGHT_SIZE] =
	{
		"{빛}",
		"{어둠}",
		"[빛+어둠]"
	};

	std::string CHARCLASS[GLCI_NUM_ACTOR] =
	{
		"격투부남",
		"검도부남",
		"양궁부여",
		"기예부여",
		"극강부남",
		"극강부여",
		"격투부여",
		"검도부여",
		"양궁부남",
		"기예부남",
		"과학부남",
		"과학부여",
        "인술부남",
        "인술부여",
		"마술부남",
		"마술부여",
        "기타남",
        "기타여",
		"연극부남",
		"연극부여",
	};	
	
	std::string EMSCHOOL[GLSCHOOL_NUM] =
	{
		"school 1",
		"school 2",
		"school 3",
	};

	std::string CHARCOMMENT[GLCI_NUM_ACTOR] = 
	{
		"", // 격투부 남;
		"", // 검도부 남;
		"", // 양궁부 여;
		"", // 기예부 여;
		"", // 극강부 남;
		"", // 극강부 여;
		"", // 격투부 여;
		"", // 검도부 여;
		"", // 양궁부 남;
		"", // 기예부 남;
		"", // 과학부 남;
		"", // 과학부 여;
        "", // 인술부 남;
        "", // 인술부 여;
        "", // 마술부 남;
        "", // 마술부 여;
		"", // 기타 남;
		"", // 기타 여;
		"", // 연극부 남
		"", // 연극부 여
	};

	std::string TRANSFORM_SET[EMTRANSFORM_SET_MAXSIZE] =
	{
		"",
	};

	std::string TFCHAR_SET[EMTRANSFORM_CHAR_SET_MAXSIZE] =
	{
		"",
	};

	std::string BLOW[EMBLOW_SIZE] =
	{
		"없음",	// 0
		"마비",	// 1
		"기절",	// 2
		"석화",	// 3
		"화염",	// 4
		"냉동",	// 5

		"착란",	// 6
		"중독",	// 7
		"저주",	// 8
	};

	std::string BLOW_VAR1[EMBLOW_SIZE] =
	{
		"없음",
		"이속변화율",	//"마비"
		"없음",			//"기절"
		"이속변화율",	//"석화"
		"없음",			//"화염"
		"이속변화율",	//"냉동"

		"없음",			//"착란"
		"없음",			//"중독"
		"없음",			//"저주"
	};

	float BLOW_VAR1_SCALE[EMBLOW_SIZE] =
	{
		1.0f,
		100.0f,			//"마비"
		1.0f,			//"기절"
		100.0f,			//"석화"
		1.0f,			//"화염"
		100.0f,			//"냉동"

		1.0f,			//"착란"
		1.0f,			//"중독"
		1.0f,			//"저주"
	};

	std::string BLOW_VAR2[EMBLOW_SIZE] =
	{
		"없음",
		"딜래이변화율",		//"마비"
		"없음",				//"기절"
		"없음",				//"석화"
		"지속타격치",		//"화염"
		"타격가산율",		//"냉동"

		"없음",				//"착란"
		"지속타격치",		//"중독"
		"지속타격치",		//"저주"
	};

	float BLOW_VAR2_SCALE[EMBLOW_SIZE] =
	{
		1.0f,
		100.0f,				//"마비"
		1.0f,				//"기절"
		1.0f,				//"석화"
		1.0f,				//"화염"
		100.0f,				//"냉동"

		1.0f,				//"착란"
		1.0f,				//"중독"
		1.0f,				//"저주"
	};

	std::string IMPACT_ADDON[EIMPACTA_SIZE] =
	{
		"없음",
		"명중율 변화량",
		"회피율 변화량",
	
		"공격치 변화량",
		"방어치 변화량",

		"HP 변화율",
		"MP 변화율",
		"SP 변화율",
		"HP+MP+SP 변화율",

		"공격치 변화율",
		"방어치 변화율",

		"격투치",
		"사격치",
		"기력치",

		"HP 증폭율",
		"MP 증폭율",
		"SP 증폭율",
		"저항치 변화",
		"능력치 변화",

        "HP 회복력 증감",
        "MP 회복력 증감",
        "SP 회복력 증감",
        "CP 회복력 증감",
	};

    // 1.0f     이면 실수 증가 ex) 1,2,3,4
    // 100.0f  이면 %증가      ex) 1%, 2%, 3%
	float IMPACT_ADDON_SCALE[EIMPACTA_SIZE] =
	{
		1.0f,
		1.0f,
		1.0f,
	
		1.0f,
		1.0f,

		100.0f,
		100.0f,
		100.0f,
		100.0f,

		100.0f,
		100.0f,

		1.0f,
		1.0f,
		1.0f,

		1.0f,
		1.0f,
		1.0f,

		1.0f,
		1.0f,

        100.0f,
        100.0f,
        100.0f,
        100.0f,
        1.0f,
	};

	std::string IMPACT_TAR[EMIMPACT_TARGET_NSIZE] =
	{
		"자신",
		"타겟",
		"자신부터 타겟까지",
		"지면위치의 주변 타겟",
		"지면위치"
	};

	std::string IMPACT_REALM[EMIMPACT_REALM_NSIZE] =
	{
		"'목표' 자신",
		"'목표' 주위",
		"'목표' 주위 지속",
		"'목표'를 향한 부채꼴"
	};

	std::string IMPACT_SIDE[EMIMPACT_SIDE_NSIZE] =
	{
		"자기편에게",
		"적에게",
		"모두에게"
	};

	std::string SPEC_ADDON[EMSPECA_NSIZE] =
	{
		"기능 없음",						// 0.기능없음.
		"당기기/밀기",					// 1.당기기/밀기.
		"대미지 반사",				   // 2.대미지반사.
		"부활",							   // 3.부활.
		"HP 빼앗음",						// 4.HP 빼앗음.
		"MP 빼앗음",					   // 5.MP 빼앗음.
		"SP 빼앗음",						// 6.SP 빼앗음.
		"HP 나눔",                          // 7.HP 나눔.
		"MP 나눔",							// 8.MP 나눔.
		"SP 나눔",                          // 9.SP 나눔.
		"상태이상 방지",                // 10.상태이상 방지.
		"상태이상 회복",                // 11.상태이상 회복.
		"관통정도",                        // 12.관통정도.
		"사정거리",                        // 13.사정거리.
		"이동속도(실수)",                        // 14.이동속도.
		"진격",							   // 15.진격
		"은신",							   // 16.은신
		"은신 감지",					   // 17.은신 감지
		"공격속도",                        // 18.공속율.
		"스킬 딜래이",				   // 19.스킬 딜래이 변화율
		"강한타격",                        // 20.강타		
		"물리 데미지 흡수율",        // 21.물리 데미지 흡수율
		"마법 데미지 흡수율",        // 22.마법 데미지 흡수율
		"물리 데미지 반사율",        // 23.물리 데미지 반사율
		"마법 데미지 반사율",        // 24.마법 데미지 반사율
		"스킬 효과 제거",			   // 25.스킬효과 제거
		"스턴",								// 26.스턴
		"발동형 스킬",				   // 27.발동형 스킬
		"끌어오기",                        // 28.끌어오기
		"밀어내기",                        // 29.밀어내기
		"지속데미지",						// 30.지속데미지
		"저주",							   // 31.저주
		"스킬사정거리 변화",			// 32.사정거리
		"스킬적용영역 변화",			// 33.적용 영역
		"물약사용금지",					// 34.물약 사용 금지
		"스킬사용금지",					// 35.스킬 사용 금지
		"데미지 흡수",				   // 36.데미지 흡수
		"아이템드롭율",					// 37.아이템 드랖율
		"금액드롭율",						// 38.금액 드랖율
		"경험치증가율",					// 39.경험치 증가율
		"특정 NPC 대화",				   // 40.특정 NPC 대화
		"특정 아이템 드롭",		   // 41.특정 아이템 드랖
		"순간이동",                        // 42.순간 이동
		"위치교대",                        // 43.위치 교대
		"증강",							   // 44.증강
		"허상",							   // 45.허상
        "하차",							   // 46.하차
		"지속시간증감",					// 47.지속시간 증감
		"낙인",								// 48.낙인
		"환독",							   // 49.환독
		"피격시 효과 해제",		   // 50. 피격시 스킬효과 해제
		"고리",							   // 51. 고리
		"수독",							   // 52. 수독
		"면역",							   // 53. 면역
		"전투상태",                        // 54. 전투상태
		"전투상태 해제"                 // 55. 전투상태 해제
		"도발",							   // 56. 도발		
		"지배",							   // 57. 지배
		"랜덤경험치증가율",			// 58. 랜덤 경험치 증가율;
		"스킬 링크",					// 59. 스킬 링크;
		"공격력(정수)",					//60. 공격력 가산 
		"공격력(실수, 스킬 데미지)",	//61. 공격력 % (근거리, 원거리 보정전)
		"방어력(정수)",					//62. 방어력 가산
		"방어력(실수)",					//63. 방어력 %
		"명중율(정수)",					//64. 명중율 가산
		"명중율(실수)",					//65. 명중율 %
		"회피율(정수)",					//66. 회피율 가산
		"회피율(실수)",					//67. 회피율 %
		"HP 증가",						//68. HP증가
		"MP 증가",						//69. MP증가
		"SP 증가",						//70. SP증가
		"CP 증가(미사용)",				//71. CP증가
		"HP 회복율",					//72. HP회복율
		"MP 회복율",					//73. MP회복율
		"SP 회복율",					//74. SP회복율
		"HP,MP,SP 회복율",				//75. HP,MP,SP 회복율
		"공격 영역",					//76. 공격 영역
		"SP 소모량",					//77. SP 소모량
		"저항",							//78. 저항
		"힘",							//79. 힘
		"체력",							//80. 체력
		"정신",							//81. 정신
		"민첩",							//82. 민첩
		"근력",							//83. 근력
		"격투치",						//84. 격투치
		"사격치",						//85. 사격치
		"마력치",						//86. 마력치
		"포션 HP 증가량",				//87. 포션 HP 증가량
		"포션 MP 증가량",				//88. 포션 MP 증가량
		"포션 SP 증가량",				//89. 포션 SP 증가량
		"몬스터 킬시 CP 증가량",		//90. 몬스터 킬시 CP 증가량
		"이동속도(정수)"				//91. 이동속도 정수 증가량(탈것과 동일한 수치)
		"데미지(실수, 전체데미지)",		//92. 데미지 (근거리, 사격 보정치 적용 받은 최종 수치에서 곱연산)
 
	};

	std::string SPEC_ADDON_VAR1[EMSPECA_NSIZE] =
	{
		"없음",				// 0.기능없음.
		"거리",				// 1.당기기/밀기.
		"없음",				// 2.대미지반사.
		"회복율",			// 3.부활.
		"흡수율",			// 4.HP 빼앗음.
		"흡수율",			// 5.MP 빼앗음.
		"흡수율",			// 6.SP 빼앗음.
		"없음",				// 7.HP 나눔.
		"없음",				// 8.MP 나눔.
		"없음",				// 9.SP 나눔.
		"없음",				// 10.상태이상 방지.
		"없음",				// 11.상태이상 회복.
		"관통증가",			// 12.관통정도.
		"공격거리",			// 13.사정거리.
		"이속율",			// 14.이동속도.
		"거리",				// 15.진격
		"은신 레벨",		// 16.은신
		"감지 범위",		// 17.은신 감지
		"공속율",			// 18.공속율.
		"변화율",			// 19.스킬 딜래이 변화율
		"거리",				// 20.강타		
		"흡수율",			// 21.물리 데미지 흡수율
		"흡수율",			// 22.마법 데미지 흡수율
		"반사율",			// 23.물리 데미지 반사율
		"반사율",			// 24.마법 데미지 반사율
		"없음",				// 25.스킬효과 제거
		"없음",				// 26.스턴
		"확률",				// 27.발동형 스킬
		"확률",				// 28.끌어오기
		"확률",				// 29.밀어내기
		"시간",				// 30.지속데미지
        "반사율",			// 31.저주
        "거리",				// 32.사정거리
        "거리",				// 33.적용 영역
        "없음",				// 34.물약 사용 금지
        "없음",				// 35.스킬 사용 금지
        "횟수",				// 36.데미지 흡수
        "증가율",			// 37.아이템 드랖율
        "증가율",			// 38.금액 드랖율
        "증가율",			// 39.경험치 증가율
        "없음",				// 40.특정 NPC 대화
        "없음",				// 41.특정 아이템 드랖
        "없음",				// 42.순간 이동
        "확률",				// 43.위치 교대
		"강타확률",			// 44.강화
        "없음",				// 45.허상
        "확률",				// 46.하차
		"증감비율1",		// 47.지속시간 증감
		"유지거리",			// 48.낙인
		"변신ID",			// 49.환독
		"확률",				// 50. 피격시 스킬효과 해제
		"확률",				// 51. 고리
		"확률",				// 52. 수독
		"적용타입",			// 53. 면역
		"확률",				// 54. 전투상태 효과
		"없음",				// 55. 전투상태 해제
		"확률",				// 56. 도발		
		"확률",				// 57. 지배
		"색인",				// 58. 랜덤 경험치 증가율;
		"확률",				// 59. 스킬 링크;
		"값",					// 60. 공격력(정수);
		"값",					// 61. 공격력(비율);
		"값",					// 62. 방어력(정수);
		"값",					// 63. 방어력(비율);
		"값",					// 64. 명중률(정수);
		"값",					// 65. 명중률(비율);
		"값",					// 66. 회피율(정수);
		"값",					// 67. 회피율(비율);
		"값",					// 68. hp증가;
		"값",					// 69. mp증가;
		"값",					// 70. sp증가;
		"값",					// 71. cp증가;
		"값",					// 72. hp회복율;
		"값",					// 73. mp회복율;
		"값",					// 74. sp회복율;
		"값",					// 75. hp, mp, sp 회복율;
		"값",					// 76. 공격 영역;
		"값",					// 77. sp 소모량;
		"값",					// 78. 저항;
		"값",					// 79. 힘(정수);
		"값",					// 80. 체력(정수);
		"값",					// 81. 정신(정수);
		"값",					// 82. 민첩(정수);
		"값",					// 83. 근력(정수);
		"값",					// 84. 격투치(정수);
		"값",					// 85. 사격치(정수);
		"값",					// 86. 마력치(정수);
		"값",					// 87. 포션 hp증가(정수);
		"값",					// 88. 포션 mp증가(정수);
		"값",					// 89. 포션 sp증가(정수);
		"값",					// 90. cp증가(정수);
		"값",					// 91. 이동속도;
		"값",					// 92. 데미지(비율);
		"값",					// 93. 시야 제한(비율);
	};

    // 1.0f     이면 실수 증가 ex) 1,2,3,4
    // 100.0f  이면 %증가      ex) 1%, 2%, 3%
	float SPEC_ADDON_VAR1_SCALE[EMSPECA_NSIZE] =
	{
		1.0f,			// 0.기능없음.
		1.0f,			// 1.당기기/밀기.
		1.0f,			// 2.대미지반사.
		100.0f,		// 3.부활.
		100.0f,		// 4.HP 빼앗음.
		100.0f,		// 5.MP 빼앗음.
		100.0f,		// 6.SP 빼앗음.
		1.0f,			// 7.HP 나눔.
		1.0f,			// 8.MP 나눔.
		1.0f,			// 9.SP 나눔.
		1.0f,			// 10.상태이상 방지.
		1.0f,			// 11.상태이상 회복.
		1.0f,			// 12.관통정도.
		1.0f,			// 13.사정거리.
		100.0f,		// 14.이동속도.
		100.0f,		// 15.진격
		1.0f,			// 16.은신
		80.0f,			// 17.은신 감지
		100.0f,		// 18.공속율.
		1.0f,			// 19.스킬 딜래이 변화율
		100.0f,		// 20.강타		
		100.0f,		// 21.물리 데미지 흡수율
		100.0f,		// 22.마법 데미지 흡수율
		100.0f,		// 23.물리 데미지 반사율
		100.0f,		// 24.마법 데미지 반사율
		1.0f,			// 25.이로운 지속효과 제거
		1.0f,			// 26.스턴
		100.0f,		// 27.발동형 스킬
		100.0f,		// 28.끌어오기
		100.0f,		// 29.밀어내기
		1.0f,			// 30.지속데미지        
        1.0f,			// 31.저주
        1.0f,			// 32.사정거리
        1.0f,			// 33.적용 영역
        1.0f,			// 34.물약 사용 금지
        1.0f,			// 35.스킬 사용 금지
        1.0f,			// 36.데미지 흡수
        1.0f,			// 37.아이템 드랖율
        1.0f,			// 38.금액 드랖율
        1.0f,			// 39.경험치 증가율
        1.0f,			// 40.특정 NPC 대화
        1.0f,			// 41.특정 아이템 드랖
        1.0f,			// 42.순간 이동
        1.0f,			// 43.위치 교대
		100.0f,			// 44.강화
        1.0f,			// 45.허상
		1.0f,			// 46.하차
		1.0f,			// 47.지속시간 증감
		0.0f,			// 48.낙인
		0.0f,			// 49.환독
		0.0f,			// 50. 피격시 스킬효과 해제
		0.0f,			// 51. 고리
		0.0f,			// 52. 수독
		0.0f,			// 53. 면역
		0.0f,			// 54. 전투상태 효과
		0.0f,			// 55. 전투상태 해제
		1.0f,			// 56. 도발		
		1.0f,			// 57. 지배	
		1.0f,			// 58. 랜덤 경험치 증가율;
		0.0f,			// 59. 스킬 링크;
		0.0f,			// 60. 공격력(정수);
		1.0f,			// 61. 공격력(비율);
		0.0f,			// 62. 방어력(정수);
		1.0f,			// 63. 방어력(비율);
		0.0f,			// 64. 명중률(정수);
		1.0f,			// 65. 명중률(비율);
		0.0f,			// 66. 회피율(정수);
		1.0f,			// 67. 회피율(비율);
		0.0f,			// 68. hp증가;
		0.0f,			// 69. mp증가;
		0.0f,			// 70. sp증가;
		0.0f,			// 71. cp증가;
		1.0f,			// 72. hp회복율;
		1.0f,			// 73. mp회복율;
		1.0f,			// 74. sp회복율;
		1.0f,			// 75. hp, mp, sp 회복율;
		0.0f,			// 76. 공격 영역;
		0.0f,			// 77. sp 소모량;
		0.0f,			// 78. 저항;
		0.0f,			// 79. 힘(정수);
		0.0f,			// 80. 체력(정수);
		0.0f,			// 81. 정신(정수);
		0.0f,			// 82. 민첩(정수);
		0.0f,			// 83. 근력(정수);
		0.0f,			// 84. 격투치(정수);
		0.0f,			// 85. 사격치(정수);
		0.0f,			// 86. 마력치(정수);
		0.0f,			// 87. 포션 hp증가(정수);
		0.0f,			// 88. 포션 mp증가(정수);
		0.0f,			// 89. 포션 sp증가(정수);
		0.0f,			// 90. cp증가(정수);
		0.0f,			// 91. 이동속도;
		1.0f,			// 92. 데미지(비율);
		1.0f,			// 93. 시야 제한(비율);
	};

	std::string SPEC_ADDON_VAR2[EMSPECA_NSIZE] =
	{
		"없음",				// 0.기능없음.
		"확율",				// 1.당기기/밀기.
		"없음",				// 2.대미지반사.
		"없음",				// 3.부활.
		"없음",				// 4.HP 빼앗음.
		"없음",				// 5.MP 빼앗음.
		"없음",				// 6.SP 빼앗음.
		"없음",				// 7.HP 나눔.
		"없음",				// 8.MP 나눔.
		"없음",				// 9.SP 나눔.
		"없음",				// 10.상태이상 방지.
		"없음",				// 11.상태이상 회복.
		"없음",				// 12.관통정도.
		"없음",				// 13.사정거리.
		"없음",				// 14.이동속도.
		"속도",				// 15.진격
		"애니메이션",		// 16.은신
		"감지 레벨",		// 17.은신 감지
		"없음",				// 18.공속율.
		"없음",				// 19.스킬 딜래이 변화율
		"없음",				// 20.강타		
		"없음",				// 21.물리 데미지 흡수율
		"확률",				// 22.마법 데미지 흡수율
		"확률",				// 23.물리 데미지 반사율
		"없음",				// 24.마법 데미지 반사율
		"없음",				// 25.이로운 지속효과 제거
		"없음",				// 26.스턴
		"없음",				// 27.발동형 스킬
		"이동속도",			// 28.끌어오기
		"거리",				// 29.밀어내기
        "데미지",		   // 30.지속데미지        
        "없음",			   // 31.저주
        "없음",			   // 32.사정거리
        "없음",			   // 33.적용 영역
        "없음",			   // 34.물약 사용 금지
        "없음",			   // 35.스킬 사용 금지
        "흡수량",			// 36.데미지 흡수
        "없음",			   // 37.아이템 드랖율
        "없음",			   // 38.금액 드랖율
        "없음",			   // 39.경험치 증가율
        "없음",			   // 40.특정 NPC 대화
        "없음",			   // 41.특정 아이템 드랖
        "없음",			   // 42.순간 이동
        "이동 속도",		// 43.위치 교대
        "피해증감",			// 44.강화
		"없음",				// 45.허상
		"지속여부",			// 46.하차
		"증감비율2",		// 47.지속시간 증감
		"적용비율",			// 48.낙인
		"이모티콘ID",		// 49.환독
		"행동타입",			// 50. 피격시 스킬효과 해제
		"없음",				// 51. 고리
		"없음",				// 52. 수독
		"행동타입",			// 53. 면역
		"지속시간",			// 54. 전투상태 효과
		"없음",				// 55. 전투상태 해제
		"유효거리",			// 56. 도발		
		"적용범위",			// 57. 지배
		"중첩회수",			// 58. 랜덤 경험치 증가율;
		"없음",				// 59. 스킬 링크;
		"없음",				// 60. 공격력(정수);
		"없음",				// 61. 공격력(비율);
		"없음",				// 62. 방어력(정수);
		"없음",				// 63. 방어력(비율);
		"없음",				// 64. 명중률(정수);
		"없음",				// 65. 명중률(비율);
		"없음",				// 66. 회피율(정수);
		"없음",				// 67. 회피율(비율);
		"없음",				// 68. hp증가;
		"없음",				// 69. mp증가;
		"없음",				// 70. sp증가;
		"없음",				// 71. cp증가;
		"없음",				// 72. hp회복율;
		"없음",				// 73. mp회복율;
		"없음",				// 74. sp회복율;
		"없음",				// 75. hp, mp, sp 회복율;
		"없음",				// 76. 공격 영역;
		"없음",				// 77. sp 소모량;
		"없음",				// 78. 저항;
		"없음",				// 79. 힘(정수);
		"없음",				// 80. 체력(정수);
		"없음",				// 81. 정신(정수);
		"없음",				// 82. 민첩(정수);
		"없음",				// 83. 근력(정수);
		"없음",				// 84. 격투치(정수);
		"없음",				// 85. 사격치(정수);
		"없음",				// 86. 마력치(정수);
		"없음",				// 87. 포션 hp증가(정수);
		"없음",				// 88. 포션 mp증가(정수);
		"없음",				// 89. 포션 sp증가(정수);
		"없음",				// 90. cp증가(정수);
		"없음",				// 91. 이동속도;
		"없음",				// 92. 데미지(비율);
		"없음",				// 93. 시야 제한(비율);
	};

    // 1.0f     이면 실수 증가 ex) 1,2,3,4
    // 100.0f  이면 %증가      ex) 1%, 2%, 3%
	float SPEC_ADDON_VAR2_SCALE[EMSPECA_NSIZE] =
	{
		1.0f,			// 0.기능없음.
		100.0f,		// 1.당기기/밀기.
		1.0f,			// 2.대미지반사.
		1.0f,			// 3.부활.
		1.0f,			// 4.HP 빼앗음.
		1.0f,			// 5.MP 빼앗음.
		1.0f,			// 6.SP 빼앗음.
		1.0f,			// 7.HP 나눔.
		1.0f,			// 8.MP 나눔.
		1.0f,			// 9.SP 나눔.
		1.0f,			// 10. 상태이상 방지.
		1.0f,			// 11. 상태이상 회복.
		1.0f,			// 12. 관통정도.
		1.0f,			// 13. 사정거리.
		1.0f,			// 14. 이동속도.
		1.0f,			// 15. 진격
		0.0f,			// 16. 은신
		1.0f,			// 17. 은신 감지
		1.0f,			// 18. 공속율.
		1.0f,			// 19. 스킬 딜래이 변화율
		1.0f,			// 20. 강타		
		1.0f,			// 21. 물리 데미지 흡수율
		100.0f,		// 22. 마법 데미지 흡수율
		100.0f,		// 23. 물리 데미지 반사율
		100.0f,		// 24. 마법 데미지 반사율
		1.0f,			// 25. 이로운 지속효과 제거
		1.0f,			// 26. 스턴
		1.0f,			// 27. 발동형 스킬
		1.0f,			// 28. 끌어오기
		1.0f,			// 29. 밀어내기
        1.0f,			// 30. 지속데미지        
        1.0f,			// 31. 저주
        1.0f,			// 32. 사정거리
        1.0f,			// 33. 적용 영역
        1.0f,			// 34. 물약 사용 금지
        1.0f,			// 35. 스킬 사용 금지
        1.0f,			// 36. 데미지 흡수
        1.0f,			// 37. 아이템 드랖율
        1.0f,			// 38. 금액 드랖율
        1.0f,			// 39. 경험치 증가율
        1.0f,			// 40. 특정 NPC 대화
        1.0f,			// 41. 특정 아이템 드랖
        1.0f,			// 42. 순간 이동
        1.0f,			// 43. 위치 교대
        100.0f,		// 44. 강화
        1.0f,			// 45. 허상
		1.0f,			// 46. 하차
		1.0f,			// 47. 지속시간 증감
		1.0f,			// 48. 낙인
		1.0f,			// 49. 환독
		1.0f,			// 50. 피격시 스킬효과 해제
		1.0f,			// 51. 고리
		1.0f,			// 52. 수독
		1.0f,			// 53. 면역
		1.0f,			// 54. 전투상태 효과
		1.0f,			// 55. 전투상태 해제
		1.0f,			// 56. 도발		
		10.0f,			// 57. 지배
		1.0f,			// 58. 랜덤 경험치 증가율;
		0.0f,			// 59. 스킬 링크;
		0.0f,			// 60. 공격력(정수);
		0.0f,			// 61. 공격력(비율);
		0.0f,			// 62. 방어력(정수);
		0.0f,			// 63. 방어력(비율);
		0.0f,			// 64. 명중률(정수);
		0.0f,			// 65. 명중률(비율);
		0.0f,			// 66. 회피율(정수);
		0.0f,			// 67. 회피율(비율);
		0.0f,			// 68. hp증가;
		0.0f,			// 69. mp증가;
		0.0f,			// 70. sp증가;
		0.0f,			// 71. cp증가;
		0.0f,			// 72. hp회복율;
		0.0f,			// 73. mp회복율;
		0.0f,			// 74. sp회복율;
		0.0f,			// 75. hp, mp, sp 회복율;
		0.0f,			// 76. 공격 영역;
		0.0f,			// 77. sp 소모량;
		0.0f,			// 78. 저항;
		0.0f,			// 79. 힘(정수);
		0.0f,			// 80. 체력(정수);
		0.0f,			// 81. 정신(정수);
		0.0f,			// 82. 민첩(정수);
		0.0f,			// 83. 근력(정수);
		0.0f,			// 84. 격투치(정수);
		0.0f,			// 85. 사격치(정수);
		0.0f,			// 86. 마력치(정수);
		0.0f,			// 87. 포션 hp증가(정수);
		0.0f,			// 88. 포션 mp증가(정수);
		0.0f,			// 89. 포션 sp증가(정수);
		0.0f,			// 90. cp증가(정수);
		0.0f,			// 91. 이동속도;
		0.0f,			// 92. 데미지(비율);
		0.0f,			// 93. 시야 제한(비율);

	};

	std::string SPEC_STIGMA_TYPE[EMSPEC_STIGMA_TYPE_NSIZE] =
	{
		"일반공격 피해",
		"스킬공격 피해",
		"버프",
		"디버프",
		"리미트",
	};

	std::string SPEC_HALLUCINATE_TYPE[EMSPEC_HALLUCINATE_TYPE_NSIZE] =
	{
		"변신",
		"강제이모티콘",
		"타겟팅 불가"		
	};

	std::string SPEC_DOMINATE_TYPE[EMSPEC_DOMINATE_TYPE_NSIZE] =
	{
		"자신",
		"시전자",
		"무작위",
	};

    std::string SPEC_ASIGN_TYPE[EMSPEC_SKILL_ASING_NSIZE] =
    {
        "적용 전",
        "적용 후"        
    };

    std::string SPEC_INC_TYPE[EMSPEC_INC_TYPE_NSIZE] =
    {
        "증가",
        "감소"
    };

	std::string SPEC_INVISIBLE_USE_TYPE = "전투상태중 사용";

    std::string SPEC_SKILL_CONTINUE = "지속";

	std::string SPEC_INVISIBLE_ANI_TYPE[EMSPEC_INVISIBLE_ANI_TYPE_NSIZE] =
	{
		"없음",
		"걷기",
		"뛰기",
	};

	std::string SPEC_SWAPPOS_DIR_TYPE[EMSPEC_SWAPPOS_DIR_NSIZE] = 
	{
		"양쪽",
		"자신",
		"타겟",
	};

	std::string SKILLCLASS[EMSKILLCLASS_NSIZE] =
	{
		"격투부-class1",
		"격투부-class2",
		"격투부-class3",
		"격투부-class4",

		"검도부-class1",
		"검도부-class2",
		"검도부-class3",
		"검도부-class4",

		"양궁부-class1",
		"양궁부-class2",
		"양궁부-class3",
		"양궁부-class4",

		"기예부-class1",
		"기예부-class2",
		"기예부-class3",
		"기예부-class4",

		"NPC_01",
		"NPC_02",
		"NPC_03",
		"NPC_04",
		"NPC_05",
		"NPC_06",
		"NPC_07",
		"NPC_08",
		"NPC_09",
		"NPC_10",
		"NPC_11",
		"NPC_12",
		"NPC_13",
		"NPC_14",
		"극강부-class1",
		"극강부-class2",
		"극강부-class3",
		"극강부-class4",
		"NPC_19",
		"NPC_20"

		"과학부-class1",
		"과학부-class2",
		"과학부-class3",
		"과학부-class4",

		"음식(도시락)",

		"소환수",
        "BIKE",

        "인술부-class1",
        "인술부-class2",
        "인술부-class3",
        "인술부-class4",

        "기타-class1",
        "기타-class2",
        "기타-class3",
        "기타-class4",

        "점령전",
		"이벤트",
	};

	std::string LANDEFFECT_TYPE[EMLANDEFFECT_SIZE] =
	{
		"공격속도",
		"이동속도",
		"HP 증폭율",
		"MP 증폭율",
		"회복율",
		"공격치 변화율",
		"방어치 변화율",
		"저항수치",
		"모든 버프취소"
	};

	int SPEC_CA[EMSPECA_CAK_MAX] = 
	{
		0,
	};
};

void SaveEmCharClassExcel(sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{
    Xls.RenameWorksheet(SheetNum, SheetName);

    int Row = 0;
    int Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "Type");               Xls.SetCell(SheetNum, Row, Col++, "Value"); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_NONE");          Xls.SetCell(SheetNum, Row, Col++, GLCC_NONE); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_FIGHTER_M");     Xls.SetCell(SheetNum, Row, Col++, GLCC_FIGHTER_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARMS_M");        Xls.SetCell(SheetNum, Row, Col++, GLCC_ARMS_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARCHER_W");      Xls.SetCell(SheetNum, Row, Col++, GLCC_ARCHER_W); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SPIRIT_W");      Xls.SetCell(SheetNum, Row, Col++, GLCC_SPIRIT_W); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_EXTREME_M");     Xls.SetCell(SheetNum, Row, Col++, GLCC_EXTREME_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_EXTREME_W");     Xls.SetCell(SheetNum, Row, Col++, GLCC_EXTREME_W); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_FIGHTER_W");     Xls.SetCell(SheetNum, Row, Col++, GLCC_FIGHTER_W); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARMS_W");        Xls.SetCell(SheetNum, Row, Col++, GLCC_ARMS_W); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARCHER_M");      Xls.SetCell(SheetNum, Row, Col++, GLCC_ARCHER_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SPIRIT_M");      Xls.SetCell(SheetNum, Row, Col++, GLCC_SPIRIT_M); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SCIENTIST_M");   Xls.SetCell(SheetNum, Row, Col++, GLCC_SCIENTIST_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SCIENTIST_W");   Xls.SetCell(SheetNum, Row, Col++, GLCC_SCIENTIST_W); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ASSASSIN_M");    Xls.SetCell(SheetNum, Row, Col++, GLCC_ASSASSIN_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ASSASSIN_W");    Xls.SetCell(SheetNum, Row, Col++, GLCC_ASSASSIN_W); Row++; Col = 0;

	Xls.SetCell(SheetNum, Row, Col++, "GLCC_TRICKER_M");    Xls.SetCell(SheetNum, Row, Col++, GLCC_TRICKER_M); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_TRICKER_W");    Xls.SetCell(SheetNum, Row, Col++, GLCC_TRICKER_W); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ETC_M");         Xls.SetCell(SheetNum, Row, Col++, GLCC_ETC_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ETC_W");         Xls.SetCell(SheetNum, Row, Col++, GLCC_ETC_W); Row++; Col = 0;

	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ACTOR_M");         Xls.SetCell(SheetNum, Row, Col++, GLCC_ACTOR_M); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ACTOR_W");         Xls.SetCell(SheetNum, Row, Col++, GLCC_ACTOR_W); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_FIGHTER");       Xls.SetCell(SheetNum, Row, Col++, GLCC_FIGHTER); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARMS");          Xls.SetCell(SheetNum, Row, Col++, GLCC_ARMS); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARCHER");        Xls.SetCell(SheetNum, Row, Col++, GLCC_ARCHER); Row++; Col = 0;	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SPIRIT");        Xls.SetCell(SheetNum, Row, Col++, GLCC_SPIRIT); Row++; Col = 0;	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_EXTREME");       Xls.SetCell(SheetNum, Row, Col++, GLCC_EXTREME); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SCIENTIST");     Xls.SetCell(SheetNum, Row, Col++, GLCC_SCIENTIST); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ASSASSIN");      Xls.SetCell(SheetNum, Row, Col++, GLCC_ASSASSIN); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_TRICKER");      Xls.SetCell(SheetNum, Row, Col++, GLCC_TRICKER); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ETC");           Xls.SetCell(SheetNum, Row, Col++, GLCC_ETC); Row++; Col = 0;      

	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ACTOR");           Xls.SetCell(SheetNum, Row, Col++, GLCC_ACTOR); Row++; Col = 0;
	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL");           Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL); Row++; Col = 0;		
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_OLD_EX");    Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_OLD_EX); Row++; Col = 0;	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_NEWSEX");    Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_NEWSEX); Row++; Col = 0;	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_SCIENTIST"); Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_SCIENTIST); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_ASSASSIN");  Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_ASSASSIN); Row++; Col = 0; 
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_TRICKER");   Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_TRICKER); Row++; Col = 0; 
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_ACTOR");     Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_ACTOR); Row++; Col = 0; 

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_MAN");           Xls.SetCell(SheetNum, Row, Col++, GLCC_MAN); Row++; Col = 0;		
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_WOMAN");         Xls.SetCell(SheetNum, Row, Col++, GLCC_WOMAN); Row++; Col = 0;		
}

void SaveEmSchoolFlagExcel(sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{
    Xls.RenameWorksheet(SheetNum, SheetName);
    int Row = 0;
    int Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "School");             Xls.SetCell(SheetNum, Row, Col++, "Value"); Row++; Col = 0;
	// 아래의 COMMENT는 데이터에서 읽어오지않는다;
	Xls.SetCell(SheetNum, Row, Col++, COMMENT::EMSCHOOL[0]); Xls.SetCell(SheetNum, Row, Col++, GLSCHOOL_00); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, COMMENT::EMSCHOOL[1]); Xls.SetCell(SheetNum, Row, Col++, GLSCHOOL_01); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, COMMENT::EMSCHOOL[2]); Xls.SetCell(SheetNum, Row, Col++, GLSCHOOL_02); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "All");                Xls.SetCell(SheetNum, Row, Col++, GLSCHOOL_ALL); Row++; Col = 0;
}

SSEARCHITEMESULT::SSEARCHITEMESULT()
    : nSearchItemID(false)
    , llPRICE(0)
    , vShopPos(0.0f, 0.0f, 0.0f)
{
    memset(szShopName, 0, sizeof(char) * (MAP_NAME_LENGTH));
}

void SSEARCHITEMESULT::SetShopName(const std::string& Name)
{
    StringCchCopy(szShopName, MAP_NAME_LENGTH, Name.c_str());
}

