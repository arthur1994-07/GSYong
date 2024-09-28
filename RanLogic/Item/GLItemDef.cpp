#include "../pch.h"
#include "./GLItemDef.h"
#include "../../SigmaCore/gassert.h"

#include "./GLItemGrindingScript.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

EMGRINDING_RS_INDEX GrindRsClassToIndex ( const EMGRINDING_RS_FLAGS emClass )
{
	switch (emClass)
	{
	case EMGRINDING_RS_FIRE:	return EMGRINDING_RS_FIRE_INDEX;
	case EMGRINDING_RS_ICE:		return EMGRINDING_RS_ICE_INDEX;
	case EMGRINDING_RS_ELECTRIC:return EMGRINDING_RS_ELECTRIC_INDEX;
	case EMGRINDING_RS_POISON:	return EMGRINDING_RS_POISON_INDEX;
	case EMGRINDING_RS_SPIRIT:	return EMGRINDING_RS_SPIRIT_INDEX;
	};

	GASSERT(0&&"잘못된 저항아이템 클래스입니다.");
	return EMGRINDING_RS_FIRE_INDEX;
}

EMGRINDING_RS_FLAGS GrindIndexToRsClass ( const EMGRINDING_RS_INDEX emIndex )
{
	switch (emIndex)
	{
	case EMGRINDING_RS_FIRE_INDEX:		return EMGRINDING_RS_FIRE;
	case EMGRINDING_RS_ICE_INDEX:		return EMGRINDING_RS_ICE;
	case EMGRINDING_RS_ELECTRIC_INDEX:	return EMGRINDING_RS_ELECTRIC;
	case EMGRINDING_RS_POISON_INDEX:	return EMGRINDING_RS_POISON;
	case EMGRINDING_RS_SPIRIT_INDEX:	return EMGRINDING_RS_SPIRIT;
	};

	GASSERT(0&&"잘못된 저항아이템 클래스입니다.");
	return EMGRINDING_RS_FIRE;
}

const bool SGRINDING_OPT::ISENABLELOGIC()
{
	if ( spSCRIPT && spSCRIPT->IsExistEntry() )
	{
		return true;
	}

	return false;
}

const bool SGRINDING_OPT::ISENABLELOGIC_CONDITION()
{
	if ( spSCRIPT && spSCRIPT->IsExistCondition() )
	{
		return true;
	}

	return false;
}

const bool SGRINDING_OPT::ISENABLELOGIC_CONSUME()
{
	if ( spSCRIPT && spSCRIPT->IsExistConsume() )
	{
		return true;
	}

	return false;
}

CGrindingScript* SGRINDING_OPT::GETSCRIPT()
{
	return spSCRIPT.get();
}

namespace COMMENT
{
	std::string ITEMLEVEL[EMITEM_LEVEL_NSIZE] =
	{
		"일반",
		"진귀한",
		"유일한",
		"무한",
		"백열",
	};

	std::string ITEMTYPE[ITEM_NSIZE] =
	{
		"착용,무기,도구",
		"화살",
		"약품",
		"스킬서",
		"귀환서",
		"인증서",
		"연마제",
		"부적",
		"승차권",
		"스킬포인트 리셋", 

		"스텟포인트 리셋", // 10
		"스킬, 스텟 리셋",
		"선물상자",
		"망각의 세제",
		"확성기",
		"폭죽",
		"캐릭터 추가 카드",
		"인벤 확장 카드",
		"창고 확장 카드",
		"창고 연결 카드", 

		"프리미엄 세트", // 20
		"개인상점 허가권",
		"랜덤 아이탬",
		"코스툼 분리",
		"헤어스타일 변경",
		"얼굴 변경",
		"? 아이템",
		"CD",
		"친구에게",
		"클럽호출", 

		"헤어샾 이용권", // 30
		"이름변경 카드",
		"헤어스타일",
		"헤어컬러",
		"귀혼주",
		"펫 카드",	// PetData
		"펫 먹이",
		"펫 이름변경",
		"펫 컬러변경",
		"펫 스타일변경", 

        "팻 스킬", // 40
        "SMS문자 발송",
        "팻 부활카드",
        "소방주(소멸방지)",
        "개조 기능",
        "탈것",
        "탈것 기름",
        "베트남 탐닉 방지 아이템 획득",
        "베트남 탐닉 방지 경험치 획득",
        "성별 변경 카드", 

        "휴지통 카드", // 50
        "소환서	",
        "펫 스킨 팩",
        "얼굴스타일",
        "택시카드",
        "재료아이템",
        "NPC소환 카드",
        "총알",
        "도시락",
        "펫 스킬 중복사용 카드", 

        "강화주 (강화 확률 증가)", // 60
        "완화주 (리셋 단계 완화)",
        "오토바이 컬러 변경카드",
        "오토바이 부스트 기능카드",
        "스킬, 스텟 포인트 선택 리셋. (신 망각의 약)",
        "코스튬 컬러 변경 카드",
        "우편함 연결 카드",
        "환불가능 Point Card",
        "환불불가 Point Card",
		"조합서",

		"제작초기화", // 70
        "개인상점 검색카드",
        "수리도구 연결카드",
		"인장카드",
		"스테이지 패스",
		"인던 입장횟수 충전",
		"카드게임 연결카드",
        "란 게임머니",      
        "위탁판매 확장 슬롯 카드", 
		"잠금 상자",
		
        "잠금 열쇠",  // 80
        "포장지",
        "포장박스",
		"학원변경카드(성문으로)",
		"학원변경카드(현암으로)",
		"학원변경카드(봉황으로)",
		"국가변경카드",
		"코스튬 능력치 부여",
		"효과해제 약품",
		"코스튬 능력치 부여 해제",

		"경험치 카드", // 90
		"선택형 박스",	//91
		"경험치 압축", // 92
		"경험치 캡슐",	// 93

		"일시강화",

	};

    std::string ItemTypeName(EMITEM_TYPE Type)
    {
        if (Type < ITEM_NSIZE)
            return ITEMTYPE[Type];
        else
            return std::string("");
    }

	std::string ITEMSUIT[SUIT_NSIZE] =
	{
		"모자류",
		"상의",
		"하의",
		"장갑류",
		"신발류",

		"무기류",

		"목걸이",
		"팔찌",

		"반지",

		"펫 A",	// PetData
		"펫 B",
		"S.C",
		"S.C Skin",
		"S.C Parts_A",
		"S.C Parts_B",
		"S.C Parts_C",
		"S.C Parts_D",
		"S.C Parts_E",
		"S.C Parts_F",

        "허리띠",
        "귀걸이",
        "장신구",
		"장식",
    };

    std::string ItemSuitName(EMSUIT Type)
    {
        if (Type < SUIT_NSIZE)
            return ITEMSUIT[Type];
        else
            return std::string("");
    }

	std::string ITEMDRUG_ER[ITEM_DRUG_ER_SIZE] =
	{
		"스턴 해제",
		"환독 해제",
		"물약사용금지 해제"
	};

	std::string ITEMDRUG[ITEM_DRUG_SIZE] =
	{
		"NUNE",
		"체력회복",
		"기력회복",
		"근력회복",
		"체력+기력 회복",
		"체력+활력 회복",
		"체력+기력+활력 회복",
		"이상치료",

		"학교귀환",
		"시작귀환",
		"직전귀환", // 10

		"부활기능",
		"체력회복+이상치료",
		"체력+기력+활력회복+이상치료",

		"특정공간이동",
		"전투수치획득",

		"스테이지 패스",
		"인던 입장횟수 충전권",
		"성향회복",
		"코스튬 능력치 부여 설정",
		"효과해제",
		"코스튬 능력치 부여 해제",

		"경험치 압축(100억)",	//22
		"경험치 압축(1000억)",	//23
		"경험치 캡슐(100억)",	//24
		"경험치 캡슐(1000억)",	//25
		"일시강화",   // sckim , 누락 추가
		"매크로시간충전",
		"기간 연장",
	};

	std::string ITEMDRUG_VAR[ITEM_DRUG_SIZE] =
	{
		"용량",
		"용량",
		"용량",
		"용량",
		"용량",
		"용량",
		"용량",
		"용량",

		"용량",
		"용량",
		"용량", //10

		"용량",
		"용량",
		"용량",

		"용량",
		"용량",
		"Stage", // 16
		"Level", // 17
		"용량",
		"등급",
		"용량", // 20
		"용량",
		"배율(%)", //22
		"배율(%)", //23
		"배율(%)", 
		"배율(%)",
		"차수",
		"용량",
	};

	std::string ITEMPROHIBIT[PROHIBIT_ITEM_SIZE] =
	{
		"Taxi_Card_Block",
		"Teleport_Card_Block",
		"Recall_Block",
		"Cure_Block",
		"Special_Potion_Block",
	};

	//std::string ITEMSLOT[SLOT_NSIZE_S] =
	//{
	//	"모자류",
	//	"상체",
	//	"하체",
	//	"손",
	//	"발",

	//	"오른손 도구",
	//	"오른손 도구 보조",
	//	"왼손 도구",
	//	"왼손 도구 보조",

	//	"목걸이",
	//	"손목",

	//	"오른손 손가락",
	//	"왼손 손가락"
	//};

	std::string ITEMATTACK[ITEMATT_NSIZE+1] =
	{
		"'비'착용",			// 0
		"검",					// 1
		"도",					// 2
		"단검",				// 3
		"창",					// 4
		"몽둥이",			// 5
		"권",					// 6
		"활",					// 7
		"투척",				// 8
		"건",					// 9
		"레일건",			// 10
		"포탈건",			// 11
        "해머",				// 12
        "듀얼스피어",		// 13
        "사이드",			// 14
		"극강글러브",		// 15
		"마술지팡이",		// 16
		"마술상자",			// 17
		"채찍",				// 18
		"방패",				// 19
		"망치",				// 20
		"우산",				// 21
        "타입검사안함",		// 22
	};

	std::string ITEMADDON[EMADD_SIZE] =
	{
		"없음",
		"명중율",
		"회피율",
		"대미지",
		"방어력",
		"체력",
		"마나",
		"스테미나",
		"STATS 힘",
		"STATS 체력",
		"STATS 기력",
		"STATS 민첩",
		"STATS 지력",
		"STATS 근력",
		"격투치",
		"사격치",
		"기력치",

		"없음",	
		"HP 증가율",
		"MP 증가율",
		"SP 증가율",
		"HP+MP+SP 증가율",
		"이동속도 증가율",
		"공격속도 증가율",
		"크리티컬 발생확률",
		"강한타격 발생확률",

		"없음",
		"HP 증가량",
		"MP 증가량",
		"SP 증가량",
		"HP+MP+SP 증가량",
		"이동속도 증가량",
		"공격속도 증가량",
		"크리티컬 발생량",
		"없음",

		"보스몹 피격 데미지 감소율",
		"보스몹 고정피해량",
		"일반몹 위급상태 크리티컬 능력",
		"보스몹 위급상태 크리티컬 능력",
		"자신의 위급상태 피해 감소",
		"일반 몹 사살 추가 경험치",
		"몹 사살 게임머니 드랍율",
		"캐릭터 물리근접 피해 감소",
		"캐릭터 물리사격 피해 감소",
		"캐릭터 마법작용 피해 감소",
		"솔로 시 HP 증가",
		"파티 시 HP 증가",
		"솔로 시 공격속도 상승",
		"파티 시 공격속도 상승",
		"랜덤",
	};

	std::string ITEMVAR[EMVAR_SIZE] =
	{
		"없음",
		"HP 증가율",
		"MP 증가율",
		"SP 증가율",
		"HP+MP+SP 증가율"
	};

	std::string ITEMVOL[EMVAR_SIZE] = // by 경대
	{
		"없음",
		"HP 증가량",
		"MP 증가량",
		"SP 증가량",
		"HP+MP+SP 증가량"
	};

	std::string ITEM_QUE_TYPE[QUESTION_SIZE] =
	{
		"none",
		"Speed Up",
		"Crazy",
		"Attack Up",
		"Exp Up",
		"Exp Get",
		"Lucky",
		"Bomb!!",
		"Mob Gen",

		"Speed Up Max",
		"Madness",
		"Attack Up Max",
		"Heal",
	};

	std::string ITEM_QUE_VAR1[QUESTION_SIZE] =
	{
		"없음",
		"이동속도",
		"이동속도",
		"Damage",
		"경험치",
		"없음",
		"발생율",
		"Damage",
		"MID",

		"이동속도",
		"이동속도",
		"Damage",

		"hp"
	};

	std::string ITEM_QUE_VAR2[QUESTION_SIZE] =
	{
		"없음",
		"없음",
		"공격속도",
		"없음",
		"없음",
		"없음",
		"없음",
		"없음",
		"SID",

		"없음",
		"공격속도",
		"없음",
		"없음"
	};

	float ITEMVAR_SCALE[EMVAR_SIZE] =
	{
		1.0f,
		100.0f,
		100.0f,
		100.0f,
		100.0f
	};

	float ITEMADDON_VAR_SCALE[EMADD_SIZE] =
	{
		1.0f
	};

	bool IsITEMVAR_SCALE ( EMITEM_VAR emITEM_VAR )
	{
		return ITEMVAR_SCALE[emITEM_VAR]==100.0f;
	};

	std::string GRINDING_TYPE[EMGRINDING_NSIZE] =
	{
		"없음",

		"공격력",
		"방어력",

		"저항(화)",
		"저항(빙)",
		"저항(전)",
		"저항(독)",
		"저항(정)",
	};

	std::string GRINDING_LEVEL[EMGRINDER_SIZE] =
	{
		"보통",
		"상위",
		"최상위",
	};

	std::string GRINDING_RESIST[EMGRINDING_RS_NSIZE] =
	{
		"화염",
		"얼음",
		"전기",
		"독",
		"기",
	};

    //cwBack 2012-01-20 랜덤옵션이 저항 이후에 후가된 옵션이 보이지 않는 문제
    std::string ITEM_RANDOM_OPT[RandomOption::RANDOM_OPTION_NSIZE] =
    {
        "없음",
        "공격력",
        "방어력",

        "명중률",
        "회피율",

        "HP증가",
        "MP증가",
        "SP증가",

        "HP증가율",
        "MP증가율",
        "SP증가율",
        "HP+MP+SP+증가율",

        "공격력 연마",
        "방어력 연마",

        "공격 영역",
        "SP 소모량",
        "저항",

        "이동속도 증가",

        "공격력(정수)",
        "방어력(정수)",

        "명중률(정수)",
        "회피율(정수)",

        "힘(정수)",
        "체력(정수)",
        "정신(정수)",
        "민첩(정수)",
        "근력(정수)",

        "격투치(정수)",
        "사격치(정수)", 
        "마력치(정수)",

        "포션HP증가량(정수)",
        "포션MP증가량(정수)",
        "포션SP증가량(정수)", 
        "몬스터 킬시 CP 증가량(정수)",

		"1등급 스킬 지속시간",
		"2등급 스킬 지속시간",
		"3등급 스킬 지속시간",
		"4등급 스킬 지속시간",
		"5등급 스킬 지속시간",
		"6등급 스킬 지속시간",
		"7등급 스킬 지속시간",
		"8등급 스킬 지속시간",
		"9등급 스킬 지속시간",
		"10등급 스킬 지속시간",
		"11등급 스킬 지속시간",
		"12등급 스킬 지속시간",
		"13등급 스킬 지속시간",
		"14등급 스킬 지속시간",
		"15등급 스킬 지속시간",
		"16등급 스킬 지속시간",
		"17등급 스킬 지속시간",
		"18등급 스킬 지속시간",
		"19등급 스킬 지속시간",
		"20등급 스킬 지속시간",
		"21등급 스킬 지속시간",
		"22등급 스킬 지속시간",
		"23등급 스킬 지속시간",
		"24등급 스킬 지속시간",
		"25등급 스킬 지속시간",
		"1등급 스킬 데미지 증가율",
		"2등급 스킬 데미지 증가율",
		"3등급 스킬 데미지 증가율",
		"4등급 스킬 데미지 증가율",
		"5등급 스킬 데미지 증가율",
		"6등급 스킬 데미지 증가율",
		"7등급 스킬 데미지 증가율",
		"8등급 스킬 데미지 증가율",
		"9등급 스킬 데미지 증가율",
		"10등급 스킬 데미지 증가율",
		"11등급 스킬 데미지 증가율",
		"12등급 스킬 데미지 증가율",
		"13등급 스킬 데미지 증가율",
		"14등급 스킬 데미지 증가율",
		"15등급 스킬 데미지 증가율",
		"16등급 스킬 데미지 증가율",
		"17등급 스킬 데미지 증가율",
		"18등급 스킬 데미지 증가율",
		"19등급 스킬 데미지 증가율",
		"20등급 스킬 데미지 증가율",
		"21등급 스킬 데미지 증가율",
		"22등급 스킬 데미지 증가율",
		"23등급 스킬 데미지 증가율",
		"24등급 스킬 데미지 증가율",
		"25등급 스킬 데미지 증가율",
		"1등급 스킬 딜레이 감소",
		"2등급 스킬 딜레이 감소",
		"3등급 스킬 딜레이 감소",
		"4등급 스킬 딜레이 감소",
		"5등급 스킬 딜레이 감소",
		"6등급 스킬 딜레이 감소",
		"7등급 스킬 딜레이 감소",
		"8등급 스킬 딜레이 감소",
		"9등급 스킬 딜레이 감소",
		"10등급 스킬 딜레이 감소",
		"11등급 스킬 딜레이 감소",
		"12등급 스킬 딜레이 감소",
		"13등급 스킬 딜레이 감소",
		"14등급 스킬 딜레이 감소",
		"15등급 스킬 딜레이 감소",
		"16등급 스킬 딜레이 감소",
		"17등급 스킬 딜레이 감소",
		"18등급 스킬 딜레이 감소",
		"19등급 스킬 딜레이 감소",
		"20등급 스킬 딜레이 감소",
		"21등급 스킬 딜레이 감소",
		"22등급 스킬 딜레이 감소",
		"23등급 스킬 딜레이 감소",
		"24등급 스킬 딜레이 감소",
		"25등급 스킬 딜레이 감소",
		"크리티컬 데미지",
		"강한타격 데미지",
		"아이템 착용레벨 감소",
		"처치시 HP 회복률",
		"처치시 MP 회복률",
		"처치시 SP 회복률",
		"처치시 HP+MP+SP 회복률",
		"전체 MP 비율만큼 공격력증가",
		"전체 SP 비율만큼 체력증가",
		"전체 명중 비율만큼 격투치 증가",
		"전체 명중 비율만큼 사격치 증가",
		"전체 명중 비율만큼 기력치 증가",
    };

	float ITEM_RANDOM_OPT_SCALE[RandomOption::RANDOM_OPTION_NSIZE] =
	{
		1.0f,
		1.0f,	//	공격력.
		1.0f,	//	방어력.

		1.0f,	//	명중율.
		1.0f,	//	회피율.

		1.0f,	//	hp 증가.
		1.0f,	//	mp 증가.
		1.0f,	//	sp 증가.

		100.0f,	//	hp 증가율.
		100.0f,	//	mp 증가율.
		100.0f,	//	sp 증가율.
		100.0f,	//	hp, mp, sp 증가율.

		1.0f,	//	공격력 연마.
		1.0f,	//	방어력 연마.

		1.0f,	//	공격 영역.
		1.0f,	//	sp 소모량.
		1.0f,	//	저항.

		1.0f,	// 이동속도 증가

		1.0f,	//	공격력(정수).
		1.0f,	//	방어력(정수).

		1.0f,	//	명중율(정수).
		1.0f,	//	회피율(정수).

		1.0f,	//	힘(정수).
		1.0f,	//	체력(정수).
		1.0f,	//	정신(정수).
		1.0f,	//	민첩(정수).
		1.0f,	//	근력(정수).

		1.0f,	//	격투치(정수).
		1.0f,	//	사격치(정수).
		1.0f,	//	마력치(정수).

		1.0f,	//	포션 hp 증가량(정수).
		1.0f,	//	포션 mp 증가량(정수).
		1.0f,	//	포션 sp 증가량(정수).
		1.0f,	//	몬스터 킬시 CP 증가량(정수).
	};

	DWORD	ITEMCOLOR[EMITEM_LEVEL_NSIZE] = 
	{
		0xffff0000,
		0xff00ff00,
		0xff0000ff,
		0xffffff00,
		0xffff00ff
	};


	std::string COOLTYPE[EMCOOL_SIZE] =
	{
		"아이템타입",
		"아이템MID/SID"
	};

	std::string BIKETYPE[EMBIKE_NSIZE] =
	{
		"BT-5",
		"BT-7",
		"BT-9",
		"EV-1"
	};

	std::string BOARDTYPE[ EMBIKE_NSIZE ] =
	{
		"호버보드",
	};

	std::string CARTYPE[EMBIKE_NSIZE] = 
	{
		"Ferrari",
		"CAR_B",
		"CAR_C",
		"CAR_D",
	};

	std::string REARCARTYPE[EMBIKE_NSIZE] =
	{
		"REARCAR_A",
		"REARCAR_B",
		"REARCAR_C",
		"REARCAR_D",
	};

	std::string SKILL_LINK_TYPE[EMITEM_SKILL_SIZE] =
	{
		"전체",
		"자신캐릭터",
		"적캐릭터",
		"몹"
	};
};

namespace db
{
	State::State()
		: m_nState( DB_NONE )
	{
	}

	void State::setState( int emState, bool bForce /*= false*/ )
	{
		if ( true == bForce )
		{
			m_nState = emState;
			return;
		}

		switch ( m_nState )
		{
		case DB_NONE:
			if ( DB_INSERT == emState )
			{
				m_nState = DB_INSERT;
			}
			else if ( DB_UPDATE == emState )
			{
				m_nState = DB_UPDATE;
			}
			else if ( DB_NONE == emState )
			{
				m_nState = DB_NONE;
			}
			else
			{
				MIN_ASSERT( 0 && "STATE Change Fail" );
			}
			break;
		case DB_INSERT:
			if ( DB_DELETE == emState )
			{
				m_nState = DB_NONE;
			}
			else if ( DB_INSERT == emState )
			{
				m_nState = DB_INSERT;
			}
			else
			{
				MIN_ASSERT( 0 && "STATE Change Fail" );
			}
			break;
		case DB_UPDATE:
			if ( DB_DELETE == emState )
			{
				m_nState = DB_DELETE;
			}
			else if ( DB_UPDATE == emState )
			{
				m_nState = DB_UPDATE;
			}
			else
			{
				MIN_ASSERT( 0 && "STATE Change Fail" );
			}
			break;
		case DB_DELETE:
			{
				MIN_ASSERT( 0 && "STATE Change Fail" );
			}
			break;
		default:
			break;
		}
	}

};
