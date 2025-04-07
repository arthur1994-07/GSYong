#include "pch.h"

#include <process.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <algorithm>

#include "../../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/File/StringFile.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/Util/Util.h"
#include "../../../RanLogic/GLUseFeatures.h"

#include "../../DxTools/NsLoadingLagTester.h"
#include "../../DxTools/DxMethods.h"
#include "../../GUInterface/GameTextControl.h"
#include "./DxSkinAnimation.h"
#include "./DxSkinAnimationManager.h"
#include "./DxAnimationListManager.h"

#include "./SAnimationDefine.h"
#include "./DxAnimationList.h"

#include "DxSkinAnimationManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace COMMENT
{
	/*
		AN_GUARD_N			= 0,	//	경계.
	AN_PLACID			= 1,	//	휴식.
	AN_WALK				= 2,	//	걷기.
	AN_RUN				= 3,	//	뛰기.
	AN_ATTACK			= 4,	//	공격.
	AN_ATTACK_MIX1		= 5,	//	공격.	// 추가 < 뛰기, 걷기용 >
	AN_ATTACK_MIX2		= 6,	//	공격.	// 추가	< 게걸음용 >
	AN_SHOCK			= 7,	//	충격.
	AN_DIE				= 8,	//	죽음.

	AN_CREATE			= 9,	//	생성.
	AN_TALK				= 10,	//	대화.

	AN_SKILL			= 11,	//	스킬.
	AN_GESTURE			= 12,	//	몸짓.
	AN_SHOCK_MIX		= 13,	//	충격,mix용.

	AN_GUARD_L			= 14,	//	경계,SP부족.
	AN_CONFT_WIN		= 15,	//	대련,승리.
	AN_CONFT_LOSS		= 16,	//	대련,패배.

	AN_SPEC				= 17,	//	특수.

	//AN_NONE				= 16,
	//AN_TYPE_SIZE		= 16,

	AN_SKILL_A			= 18,	//	스킬.
	AN_SKILL_B			= 19,	//	스킬.
	AN_SKILL_C			= 20,	//	스킬.
	AN_SKILL_D			= 21,	//	스킬.
	AN_SKILL_E			= 22,	//	스킬.
	AN_SKILL_F			= 23,	//	스킬.
	*/
	TSTRING ANI_MAINTYPE[AN_TYPE_SIZE] =
	{
		_T("경계 -Lookout"),
		_T("휴식 -Rest"),
		_T("걷기 -Walk"),
		_T("뛰기 -Run"),
		_T("공격 -Attack"),
		_T("충격 -Shock"),
		_T("쓰러지기 -Down"),

		_T("생성 -Create"),
		_T("대화 -Conversation"),

		_T("스킬 -Skill-A"),
		_T("몸짓 -Gesture"),
		_T("충격[MIX] -Shock[MIX]"),

		_T("경계[SP부족] -Lookout[SP_lack]"),

		_T("대련[승리] -Fight[win]"),
		_T("대련[패배] -Fight[lose]"),
		_T("특수 -Speciality"),

		_T("스킬 -Skill-B"),
		_T("스킬 -Skill-C"),
		_T("스킬 -Skill-D"),
		_T("스킬 -Skill-E"),

		_T("채집"),

		_T("BT - 5"),
		_T("BT - 7"),
		_T("BT - 9"),
		_T("EV - 1"),

		_T("비행"), // 주병진
		_T("스킬 -Skill-F"), // 과학부

		_T("스킬 -Skill-G"),
		_T("스킬 -Skill-H"),
		_T("Car_Ferrari"),			// Vehicle Car;
		_T("Car_B"),
		_T("Car_C"),
		_T("Car_D"),
		_T("Rearcar_A"),
		_T("Rearcar_B"),
		_T("Rearcar_C"),
		_T("Rearcar_D"),
	};

    TSTRING ANI_VEHICLE_MAINTYPE[AN_VEHICLE_SIZE] =
	{
		_T("Stay"),
		_T("Walk"),
		_T("Booster On"),
		_T("Run"),
		_T("Down"),
		_T("Shock"),
		_T("Attack"),

		_T("Skill_A"),
		_T("Skill_B"),
		_T("Skill_C"),
		_T("Skill_D"),

		_T("Anim_001"),
		_T("Anim_002"),
		_T("Anim_003"),
		_T("Anim_004"),
		_T("Anim_005"),
	};

	TSTRING ANI_ATTBONE_MAINTYPE[AN_ATTBONE_SIZE] =
	{
		_T("Stay"),
		_T("Attack_001"),
		_T("Attack_002"),
		_T("Attack_003"),
		_T("Attack_004"),
		_T("Attack_005"),

		_T("SAnim_001"),
		_T("SAnim_002"),
		_T("SAnim_003"),
		_T("SAnim_004"),
		_T("SAnim_005"),
		_T("SAnim_006"),
		_T("SAnim_007"),
		_T("SAnim_008"),
		_T("SAnim_009"),
		_T("SAnim_010"),
		_T("SAnim_011"),
		_T("SAnim_012"),
	};
	
	TSTRING ANI_SUBTYPE[AN_SUB_00_SIZE] =
	{
		_T("00 맨손 -Empty Hands"),
		_T("01 한손검 -One Hand Sword"),
		_T("02 양손검 -Two Hand Sword"),
		_T("03 쌍검 -Two Sword"),
		_T("04 단검 -Dagger"),
		_T("05 창 -Spear"),
		_T("06 활 -Bow"),
		_T("07 투척 -Throw"),
		_T("08 마법 -Magic"),
		_T("09 양손검 대형 -Two Hand Big Sword"),

		_T("10 몽둥이 -Club"),

		_T("11 한손도 -One Hand KATANA"),
		_T("12 양손도 -Two Hand KATANA"),
		_T("13 양손도 대형 -Two Hand Big KATANA"),
		_T("14 쌍도 -Two KATANA"),

		_T("15 권법 -kung fu"),

		_T("16 빗자루 -Broom"),

		_T("17 호버"),

		_T("18 권총"),
		_T("19 소총"),

		_T("20 해머 - Hammer"),
		_T("21 듀얼 스피어 - Dual spear"),
		_T("22 사이드 - Side"),

		_T("23 극강글러브 - Extreme Glove"),
		_T("24 마술지팡이- TRICK_STICK"),
		_T("25 마술상자- TRICK_BOX"),
		_T("26 채찍 - TRICK_WHIP"),

		_T("27 변술부 해머 Hammer"),
		_T("28 변술부 방패 Shield"),
		_T("29 변술부 우산 Umbrella"),

		_T("SUB_30"),
		_T("SUB_31"),
		_T("SUB_32"),
		_T("SUB_33"),
		_T("SUB_34"),
		_T("SUB_35"),
		_T("SUB_36"),
		_T("SUB_37"),
		_T("SUB_38"),
		_T("SUB_39"),

		_T("SUB_40"),
		_T("SUB_41"),
		_T("SUB_42"),
		_T("SUB_43"),
		_T("SUB_44"),
		_T("SUB_45"),
		_T("SUB_46"),
		_T("SUB_47"),
		_T("SUB_48"),
		_T("SUB_49"),

		_T("SUB_50"),
		_T("SUB_51"),
		_T("SUB_52"),
		_T("SUB_53"),
		_T("SUB_54"),
		_T("SUB_55"),
		_T("SUB_56"),
		_T("SUB_57"),
		_T("SUB_58"),
		_T("SUB_59"),

		_T("SUB_60"),
		_T("SUB_61"),
		_T("SUB_62"),
		_T("SUB_63"),
		_T("SUB_64"),
		_T("SUB_65"),
		_T("SUB_66"),
		_T("SUB_67"),
		_T("SUB_68"),
		_T("SUB_69"),

		_T("SUB_70"),
		_T("SUB_71"),
		_T("SUB_72"),
		_T("SUB_73"),
		_T("SUB_74"),
		_T("SUB_75"),
		_T("SUB_76"),
		_T("SUB_77"),
		_T("SUB_78"),
		_T("SUB_79"),

		_T("SUB_80"),
		_T("SUB_81"),
		_T("SUB_82"),
		_T("SUB_83"),
		_T("SUB_84"),
		_T("SUB_85"),
		_T("SUB_86"),
		_T("SUB_87"),
		_T("SUB_88"),
		_T("SUB_89"),

		_T("SUB_90"),
		_T("SUB_91"),
		_T("SUB_92"),
		_T("SUB_93"),
		_T("SUB_94"),
		_T("SUB_95"),
		_T("SUB_96"),
		_T("SUB_97"),
		_T("SUB_98"),
		_T("SUB_99"),
	};

	TSTRING ANI_SUBTYPE_00[AN_SUB_00_SIZE] =
	{
		_T("SUB_00"),
		_T("SUB_01"),
		_T("SUB_02"),
		_T("SUB_03"),
		_T("SUB_04"),
		_T("SUB_05"),
		_T("SUB_06"),
		_T("SUB_07"),
		_T("SUB_08"),
		_T("SUB_09"),
		_T("SUB_10"),
		_T("SUB_11"),
		_T("SUB_12"),
		_T("SUB_13"),
		_T("SUB_14"),
		_T("SUB_15"),
		_T("SUB_16"),
		_T("SUB_17"),
		_T("SUB_18"),
		_T("SUB_19"),

		_T("SUB_20"),
		_T("SUB_21"),
		_T("SUB_22"),
		_T("SUB_23"),
		_T("SUB_24"),
		_T("SUB_25"),
		_T("SUB_26"),
		_T("SUB_27"),
		_T("SUB_28"),
		_T("SUB_29"),
		_T("SUB_30"),
		_T("SUB_31"),
		_T("SUB_32"),
		_T("SUB_33"),
		_T("SUB_34"),
		_T("SUB_35"),
		_T("SUB_36"),
		_T("SUB_37"),
		_T("SUB_38"),
		_T("SUB_39"),

		_T("SUB_40"),
		_T("SUB_41"),
		_T("SUB_42"),
		_T("SUB_43"),
		_T("SUB_44"),
		_T("SUB_45"),
		_T("SUB_46"),
		_T("SUB_47"),
		_T("SUB_48"),
		_T("SUB_49"),

		_T("SUB_50"),
		_T("SUB_51"),
		_T("SUB_52"),
		_T("SUB_53"),
		_T("SUB_54"),
		_T("SUB_55"),
		_T("SUB_56"),
		_T("SUB_57"),
		_T("SUB_58"),
		_T("SUB_59"),

		_T("SUB_60"),
		_T("SUB_61"),
		_T("SUB_62"),
		_T("SUB_63"),
		_T("SUB_64"),
		_T("SUB_65"),
		_T("SUB_66"),
		_T("SUB_67"),
		_T("SUB_68"),
		_T("SUB_69"),

		_T("SUB_70"),
		_T("SUB_71"),
		_T("SUB_72"),
		_T("SUB_73"),
		_T("SUB_74"),
		_T("SUB_75"),
		_T("SUB_76"),
		_T("SUB_77"),
		_T("SUB_78"),
		_T("SUB_79"),

		_T("SUB_80"),
		_T("SUB_81"),
		_T("SUB_82"),
		_T("SUB_83"),
		_T("SUB_84"),
		_T("SUB_85"),
		_T("SUB_86"),
		_T("SUB_87"),
		_T("SUB_88"),
		_T("SUB_89"),

		_T("SUB_90"),
		_T("SUB_91"),
		_T("SUB_92"),
		_T("SUB_93"),
		_T("SUB_94"),
		_T("SUB_95"),
		_T("SUB_96"),
		_T("SUB_97"),
		_T("SUB_98"),
		_T("SUB_99"),
	};

	TSTRING ANI_SUBTYPE_01[AN_SUB_00_SIZE] =
	{
		_T("성공"),
		_T("실패"),
		_T("채집_01"),
		_T("SUB_03"),
		_T("SUB_04"),
		_T("SUB_05"),
		_T("SUB_06"),
		_T("SUB_07"),
		_T("SUB_08"),
		_T("SUB_09"),
		_T("SUB_10"),
		_T("SUB_11"),
		_T("SUB_12"),
		_T("SUB_13"),
		_T("SUB_14"),
		_T("SUB_15"),
		_T("SUB_16"),
		_T("SUB_17"),
		_T("SUB_18"),
		_T("SUB_19"),

		_T("SUB_20"),
		_T("SUB_21"),
		_T("SUB_22"),
		_T("SUB_23"),
		_T("SUB_24"),
		_T("SUB_25"),
		_T("SUB_26"),
		_T("SUB_27"),
		_T("SUB_28"),
		_T("SUB_29"),
		_T("SUB_30"),
		_T("SUB_31"),
		_T("SUB_32"),
		_T("SUB_33"),
		_T("SUB_34"),
		_T("SUB_35"),
		_T("SUB_36"),
		_T("SUB_37"),
		_T("SUB_38"),
		_T("SUB_39"),

		_T("SUB_40"),
		_T("SUB_41"),
		_T("SUB_42"),
		_T("SUB_43"),
		_T("SUB_44"),
		_T("SUB_45"),
		_T("SUB_46"),
		_T("SUB_47"),
		_T("SUB_48"),
		_T("SUB_49"),

		_T("SUB_50"),
		_T("SUB_51"),
		_T("SUB_52"),
		_T("SUB_53"),
		_T("SUB_54"),
		_T("SUB_55"),
		_T("SUB_56"),
		_T("SUB_57"),
		_T("SUB_58"),
		_T("SUB_59"),

		_T("SUB_60"),
		_T("SUB_61"),
		_T("SUB_62"),
		_T("SUB_63"),
		_T("SUB_64"),
		_T("SUB_65"),
		_T("SUB_66"),
		_T("SUB_67"),
		_T("SUB_68"),
		_T("SUB_69"),

		_T("SUB_70"),
		_T("SUB_71"),
		_T("SUB_72"),
		_T("SUB_73"),
		_T("SUB_74"),
		_T("SUB_75"),
		_T("SUB_76"),
		_T("SUB_77"),
		_T("SUB_78"),
		_T("SUB_79"),

		_T("SUB_80"),
		_T("SUB_81"),
		_T("SUB_82"),
		_T("SUB_83"),
		_T("SUB_84"),
		_T("SUB_85"),
		_T("SUB_86"),
		_T("SUB_87"),
		_T("SUB_88"),
		_T("SUB_89"),

		_T("SUB_90"),
		_T("SUB_91"),
		_T("SUB_92"),
		_T("SUB_93"),
		_T("SUB_94"),
		_T("SUB_95"),
		_T("SUB_96"),
		_T("SUB_97"),
		_T("SUB_98"),
		_T("SUB_99"),
	};

	TSTRING ANI_SUBTYPE_02[AN_SUB_00_SIZE] =
	{
		_T("끌어오기"),
		_T("밀어내기"),
		_T("진격_01"),
		_T("SUB_03"),
		_T("SUB_04"),
		_T("SUB_05"),
		_T("SUB_06"),
		_T("SUB_07"),
		_T("SUB_08"),
		_T("SUB_09"),
		_T("SUB_10"),
		_T("SUB_11"),
		_T("SUB_12"),
		_T("SUB_13"),
		_T("SUB_14"),
		_T("SUB_15"),
		_T("SUB_16"),
		_T("SUB_17"),
		_T("SUB_18"),
		_T("SUB_19"),

		_T("SUB_20"),
		_T("SUB_21"),
		_T("SUB_22"),
		_T("SUB_23"),
		_T("SUB_24"),
		_T("SUB_25"),
		_T("SUB_26"),
		_T("SUB_27"),
		_T("SUB_28"),
		_T("SUB_29"),
		_T("SUB_30"),
		_T("SUB_31"),
		_T("SUB_32"),
		_T("SUB_33"),
		_T("SUB_34"),
		_T("SUB_35"),
		_T("SUB_36"),
		_T("SUB_37"),
		_T("SUB_38"),
		_T("SUB_39"),

		_T("SUB_40"),
		_T("SUB_41"),
		_T("SUB_42"),
		_T("SUB_43"),
		_T("SUB_44"),
		_T("SUB_45"),
		_T("SUB_46"),
		_T("SUB_47"),
		_T("SUB_48"),
		_T("SUB_49"),

		_T("SUB_50"),
		_T("SUB_51"),
		_T("SUB_52"),
		_T("SUB_53"),
		_T("SUB_54"),
		_T("SUB_55"),
		_T("SUB_56"),
		_T("SUB_57"),
		_T("SUB_58"),
		_T("SUB_59"),

		_T("SUB_60"),
		_T("SUB_61"),
		_T("SUB_62"),
		_T("SUB_63"),
		_T("SUB_64"),
		_T("SUB_65"),
		_T("SUB_66"),
		_T("SUB_67"),
		_T("SUB_68"),
		_T("SUB_69"),

		_T("SUB_70"),
		_T("SUB_71"),
		_T("SUB_72"),
		_T("SUB_73"),
		_T("SUB_74"),
		_T("SUB_75"),
		_T("SUB_76"),
		_T("SUB_77"),
		_T("SUB_78"),
		_T("SUB_79"),

		_T("SUB_80"),
		_T("SUB_81"),
		_T("SUB_82"),
		_T("SUB_83"),
		_T("SUB_84"),
		_T("SUB_85"),
		_T("SUB_86"),
		_T("SUB_87"),
		_T("SUB_88"),
		_T("SUB_89"),

		_T("SUB_90"),
		_T("SUB_91"),
		_T("SUB_92"),
		_T("SUB_93"),
		_T("SUB_94"),
		_T("SUB_95"),
		_T("SUB_96"),
		_T("SUB_97"),
		_T("SUB_98"),
		_T("SUB_99"),
	};

	TSTRING ANI_SUBTYPE_03[AN_SUB_00_SIZE] =
	{
		_T("운전자_탑승"),			// 0
		_T("운전자_서기"),			// 1
		_T("운전자_이동(걷기)"),	// 2
		_T("운전자_이동(뛰기)"),	// 3
		_T("운전자_부스터 사용"),	// 4
		_T("운전자_죽음"),			// 5
		_T("보조자_탑승"),			// 6
		_T("보조자_서기"),			// 7
		_T("보조자_이동(걷기)"),	// 8
		_T("보조자_이동(뛰기)"),	// 9
		_T("보조자_부스터 사용"),	// 10
		_T("보조자_죽음"),			// 11

		_T("SUB_12"),
		_T("SUB_13"),
		_T("SUB_14"),
		_T("SUB_15"),
		_T("SUB_16"),
		_T("SUB_17"),
		_T("SUB_18"),
		_T("SUB_19"),

		_T("SUB_20"),
		_T("SUB_21"),
		_T("SUB_22"),
		_T("SUB_23"),
		_T("SUB_24"),
		_T("SUB_25"),
		_T("SUB_26"),
		_T("SUB_27"),
		_T("SUB_28"),
		_T("SUB_29"),
		_T("SUB_30"),
		_T("SUB_31"),
		_T("SUB_32"),
		_T("SUB_33"),
		_T("SUB_34"),
		_T("SUB_35"),
		_T("SUB_36"),
		_T("SUB_37"),
		_T("SUB_38"),
		_T("SUB_39"),

		_T("SUB_40"),
		_T("SUB_41"),
		_T("SUB_42"),
		_T("SUB_43"),
		_T("SUB_44"),
		_T("SUB_45"),
		_T("SUB_46"),
		_T("SUB_47"),
		_T("SUB_48"),
		_T("SUB_49"),

		_T("SUB_50"),
		_T("SUB_51"),
		_T("SUB_52"),
		_T("SUB_53"),
		_T("SUB_54"),
		_T("SUB_55"),
		_T("SUB_56"),
		_T("SUB_57"),
		_T("SUB_58"),
		_T("SUB_59"),

		_T("SUB_60"),
		_T("SUB_61"),
		_T("SUB_62"),
		_T("SUB_63"),
		_T("SUB_64"),
		_T("SUB_65"),
		_T("SUB_66"),
		_T("SUB_67"),
		_T("SUB_68"),
		_T("SUB_69"),

		_T("SUB_70"),
		_T("SUB_71"),
		_T("SUB_72"),
		_T("SUB_73"),
		_T("SUB_74"),
		_T("SUB_75"),
		_T("SUB_76"),
		_T("SUB_77"),
		_T("SUB_78"),
		_T("SUB_79"),

		_T("SUB_80"),
		_T("SUB_81"),
		_T("SUB_82"),
		_T("SUB_83"),
		_T("SUB_84"),
		_T("SUB_85"),
		_T("SUB_86"),
		_T("SUB_87"),
		_T("SUB_88"),
		_T("SUB_89"),

		_T("SUB_90"),
		_T("SUB_91"),
		_T("SUB_92"),
		_T("SUB_93"),
		_T("SUB_94"),
		_T("SUB_95"),
		_T("SUB_96"),
		_T("SUB_97"),
		_T("SUB_98"),
		_T("SUB_99"),
	};

    //주병진 Start
	TSTRING ANI_SUBTYPE_FLY[AN_SUB_FLY_SIZE] =
	{
		_T("비행준비"),
		_T("비행-저속"),
		_T("비행-고속"),
		_T("비행완료"),
		_T("SUB_04"),
		_T("SUB_05"),
		_T("SUB_06"),
		_T("SUB_07"),
		_T("SUB_08"),
		_T("SUB_09"),
	};
	//주병진 End

	TSTRING STRIKE_EFF[EMSF_SIZE] =
	{
		_T("NULL"),
		_T("SELFBODY"),
		_T("TARGET")
	};

	CString GetAnimMainType(int nType)
	{
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			return CGameTextMan::GetInstance().GetCommentText("ANI_MAINTYPE",nType).GetString();
		else
			return COMMENT::ANI_MAINTYPE[nType].c_str();
	}
	CString GetAnimSubType(int nMType, int nSType)
	{
		switch ( nMType )
		{
		case AN_SKILL:
		case AN_SKILL_B:
		case AN_SKILL_C:
		case AN_SKILL_D:
		case AN_SKILL_E:
		case AN_SKILL_F:
		case AN_SKILL_G:
		case AN_SKILL_H:
		case AN_GESTURE:
		case AN_SPEC:
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					return CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_00", nSType).GetString();
				else
					return COMMENT::ANI_SUBTYPE_00[nSType].c_str();

			}
			break;
		case AN_GATHERING:
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					return CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_01", nSType).GetString();
				else
					return COMMENT::ANI_SUBTYPE_01[nSType].c_str();
			}
			break;
		case AN_FLY:
			{
				// 아래의 COMMENT는 데이터에서 읽어오지 않는다;
				return COMMENT::ANI_SUBTYPE_FLY[nSType].c_str();
			}
			break;
		case AN_BIKE_A:
		case AN_BIKE_B:
		case AN_BIKE_C:
		case AN_BIKE_D:
		case AN_CAR_A:
		case AN_CAR_B:
		case AN_CAR_C:
		case AN_CAR_D:
		case AN_REARCAR_A:
		case AN_REARCAR_B:
		case AN_REARCAR_C:
		case AN_REARCAR_D:
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					return CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE_03", nSType).GetString();
				else
					return COMMENT::ANI_SUBTYPE_03[nSType].c_str();
			}
			break;
		default:
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					return CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE", nSType).GetString();
				else
					return COMMENT::ANI_SUBTYPE[nSType].c_str();
			}
			break;
		};
	}
}


DxSkinAnimationManager::DxSkinAnimationManager(void)
{
	m_pd3dDevice = NULL;
}

DxSkinAnimationManager::~DxSkinAnimationManager(void)
{
}

DxSkinAnimationManager& DxSkinAnimationManager::GetInstance()
{
	static DxSkinAnimationManager sam; return sam;
}


DxSkinAnimationManager::Resource 
DxSkinAnimationManager::LoadAnimationCFG( const TSTRING& nameCFG )
{
	//Resource res = LoadData(GetPath()+nameCFG,false,EMPTY_ADD_DATA(),false);
	Resource res = LoadData(nameCFG,false,EMPTY_ADD_DATA(),false);
	BackUpCache(res);
	return res;
}

// 빡치는 함수 -_-
DxSkinAnimationManager::Resource DxSkinAnimationManager::LoadAnimContainerFromTool(
	const TSTRING& nameCFG,
	const TSTRING& nameXFile,
	const TSTRING& nameSkeleton,
	BOOL bRefesh)
{
	HRESULT hr = S_OK;

	if(bRefesh)
	{
		DeleteData(nameCFG);
	}

	BOOL bSaveCfg(FALSE);
	Resource cfgRes = LoadAnimationCFG( nameCFG );
	if ( cfgRes.IsValid() )
	{
		CString strINFO_Skeleton = cfgRes.get()->m_strSkeletion.c_str();
		if ( strINFO_Skeleton.MakeLower() != CString(nameSkeleton.c_str()).MakeLower() )
		{
            CDebugSet::ToLogFile( sc::string::format( _T("본이름 불일치:%s, %s"),
				                nameCFG.c_str(),
				                strINFO_Skeleton.GetString() ) );
        }
	}
	else
	{
		SANIMCONINFO animInfo;
		animInfo.m_strCfgFile   = nameCFG;
		animInfo.m_strXFile     = nameXFile;
		animInfo.m_strBinFile   = sc::util::ChangeExtName( nameCFG, _T("bin") );
		animInfo.m_strSkeletion = nameSkeleton;

        CDebugSet::ToLogFile( sc::string::format( _T("Animation cfg Load failed : %s"),
				            nameCFG.c_str() ) );

		bSaveCfg = TRUE;
		animInfo.SaveFile( nameCFG.c_str(), nameXFile.c_str() );

		cfgRes = LoadAnimationCFG( nameCFG );
		if ( cfgRes.get() )
		{
			cfgRes.get()->m_dwFlag |= ACF_NEWINFO;
		}

		MIN_ASSERT(cfgRes.IsValid());
	}

	hr = cfgRes.get()->CreateAnimationDataFromTool( bRefesh );
	if ( FAILED(hr) )
	{
        CDebugSet::ToLogFile( sc::string::format( _T("Animation X-File Load failed : %s"),
				            cfgRes.get()->m_strCfgFile ) );
	}

	// 새로운 cfg 파일일 경우는
	// 정상적인 데이터를 위해서 재 Save를 한다.
	// 또한 m_dwETime 값이 0 이라서 m_dwETimeOrig을 대입해준다.
	if ( bSaveCfg )
	{
		cfgRes.get()->m_dwETime = cfgRes.get()->m_dwETimeOrig;
		SaveAnimConInfo( nameXFile, nameCFG );
	}

	return cfgRes;
}

BOOL DxSkinAnimationManager::SaveAnimConInfo( const TSTRING& nameXFile, const TSTRING& nameCFG )
{
	DxSkinAnimation* pSkinAniRes = GetCompletedData(nameCFG);
	if ( !pSkinAniRes )
		return FALSE;

	pSkinAniRes->SaveFile( nameCFG.c_str(), nameXFile.c_str() );
	return TRUE;
}

//// [shhan][2014.12.08] 이전에는 팔벌리는 문제로 List 를 등록 시켰는데 지금은 애니메이션이 로딩 안되면 렌더 조차 안하기 때문에
////						Preload 를 할 필요가 없다.
//BOOL DxSkinAnimationManager::BackUpList(const TCHAR* szAniContList )
//{
//	CString strPath;
//	strPath = GetPath().c_str();
//	strPath += szAniContList;
//
//	sc::CStringFile StrFile;
//	if ( !StrFile.Open(strPath.GetString()) )
//	{
//		return FALSE;
//	}
//
//	std::vector<std::string> SplitVec;
//	SplitVec.reserve(2);
//
//	CString strLine;
//	while ( StrFile.GetNextLine(strLine) )
//	{
//		std::string OrigianlStr(strLine.GetString());
//		if (OrigianlStr.empty())
//			continue;
//
//		SplitVec.clear();
//		boost::split(SplitVec, OrigianlStr, boost::algorithm::is_any_of("\t ,"), boost::algorithm::token_compress_on);        
//
//		if ( SplitVec[0].empty() )
//			continue;
//
//		sc::string::lowerString(SplitVec[0]);
//
//		//CStringArray stringArray;
//		//STRUTIL::StringSeparate( strLine, stringArray );
//		//
//		//CString cfgName = stringArray.GetAt(0);
//		//TSTRING strName = cfgName.MakeLower().GetString();
//
//		SplitVec[0] = sc::util::ChangeExtName( SplitVec[0], _T("cfg") );
//		DxSkinAnimationRes cfgRes = LoadAnimationCFG( SplitVec[0].c_str() );
//	
//		if ( !cfgRes.IsValid() )
//		{
//			TSTRING ErrorMsg = sc::string::format( _T(" ( %1%: Line:%2% ) =>%3%"), __FUNCTION__, __LINE__, TSTRING(_T("not exist cfg file : ")) + SplitVec[0].c_str() );
//			CDebugSet::ToLogFile(ErrorMsg);
//
//			continue;
//		}
//
//		//cfgRes.get()->SetAnimationList();
//
//		// Note : 현재 Bone도 Preload 쪽으로 등록 시킨다.
//		DxSkeletonManager::GetInstance().LoadSkeletonAndBackUp( cfgRes.get()->m_strSkeletion );
//
//		// Note : 현재 Bin도 Preload 쪽으로 등록 시킨다.
//		DxAnimationListResource aniListRes = 
//			DxAnimationListManager::GetInstance().LoadAnimList_BIN( 
//			cfgRes.get()->m_strBinFile.c_str(),
//			cfgRes.get()->m_skeletonRes.GetFileName(), false );
//
//		DxAnimationListManager::GetInstance().BackUpCache(aniListRes);
//
//		//---------------------------------------------------//
//		NSLoadingDirect::FrameMoveRender( FALSE );
//		//---------------------------------------------------//
//	}
//
//	m_backupListFileName = szAniContList;
//	return TRUE;
//}
//
//void DxSkinAnimationManager::ReloadBackUpList()
//{
//	BackUpList(m_backupListFileName.c_str());
//}

void DxSkinAnimationManager::ProcessCompleteData(
    SP_QUEUE_DATA_VECTOR& dataVec,
    NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
	DWORD dwPrevTick, 
	DWORD nWorkTime )
{
	SP_QUEUE_DATA_VECTOR::iterator it = dataVec.begin();
    while( it != dataVec.end() )
    {
		DWORD dwTick( timeGetTime() );

        const SP_QUEUE_DATA& spResData = *it;
        ++it;

		MIN_ASSERT(!spResData->m_fileName.empty());
		MIN_ASSERT(spResData->m_spRawData != NULL);
		MIN_ASSERT(m_pd3dDevice);

		SP_COMPLETED_DATA spSkinAnimData(new DxSkinAnimation);
		SANIMCONINFO_MSG sMsg = spSkinAnimData->LoadFile( spResData->m_spRawData.get(), spResData->m_rawDataSize,m_pd3dDevice );
		switch ( sMsg )
		{
		case SANIMCONINFO_MSG_OK:
			break;

		case SANIMCONINFO_MSG_FALSE:
			{
				TSTRING strName(_T("SANIMCONINFO::LoadFile() 문제가 생겼습니다. "));
				strName += spResData->m_fileName;
				strName += " 가 잘못됬습니다.";

				CDebugSet::ToLogFile( strName );

				GASSERT( 0 && _T("SANIMCONINFO::LoadFile() 문제가 생겼습니다. log를 참조해주세요.") );
			}
			break;

		case SANIMCONINFO_MSG_UNKNOWN_FILE_VERSION:
			{
				TSTRING strName(_T("SANIMCONINFO::LoadFile() unknown file version. "));
				strName += spResData->m_fileName;

				CDebugSet::ToLogFile( strName );

				GASSERT( 0 && _T("SANIMCONINFO::LoadFile() unknown file version. log를 참조해주세요.") );
			}
			break;

		default:
			{
				TSTRING strName(_T("SANIMCONINFO::LoadFile() 정상적인 return 값이 아닙니다. "));
				strName += spResData->m_fileName;
				strName += " 가 잘못됬습니다.";

				CDebugSet::ToLogFile( strName );

				GASSERT( 0 && _T("SANIMCONINFO::LoadFile() 정상적인 return 값이 아닙니다. log를 참조해주세요.") );
			}
			break;
		};

		// 에니메이션 로드.
		spSkinAnimData->LoadSkeleton( false );

		pCompletedDataVec->push_back( std::make_pair(spResData->m_fileName,spSkinAnimData) );

#ifdef _RELEASED
		if ( NSLoadingLagTester::g_DxSkinAnimationManager_ProcessCompleteData < timeGetTime() - dwTick )
		{
			NSLoadingLagTester::g_DxSkinAnimationManager_ProcessCompleteData = timeGetTime() - dwTick;
			NSLoadingLagTester::g_strDxSkinAnimationManager_ProcessCompleteData = spResData->m_fileName;
		}
#endif

		//// Log에 남겨본다.
		//if ( dwTick > 10 )
		//	sc::writeLogInfo( sc::string::format("DxSkinAnimationManager cfg Load %1% - %2%", spResData->m_fileName, dwTick ) );

		if ( timeGetTime() - dwPrevTick > nWorkTime )
		{
			break;
		}
    }

    if( !dataVec.empty() )
        dataVec.erase( dataVec.begin(), it );
}

void DxSkinAnimationManager::DeleteCacheDataAll()
{
	// 맵이 바뀔때는 백업된  이펙트들을 초기화 해야한다.
	// 안그러면 지워진이펙트를 참조하게 됨
	COMPLTED_DATA_MAP::iterator it = m_backupCacheCompletedDataMap.begin();
	for( ; it != m_backupCacheCompletedDataMap.end(); ++it)
		it->second.m_spData->InitEffAniList( m_pd3dDevice );


	ResourceManager::DeleteCacheDataAll();
}

void DxSkinAnimationManager::Archive( boost::function<void (const TCHAR*)> fun )
{
    BOOST_FOREACH(COMPLTED_DATA_MAP::value_type& it, m_completedDataMap)
    {
        fun( (m_path + it.first).c_str() );
    }

    BOOST_FOREACH(COMPLTED_DATA_MAP::value_type& it, m_backupCacheCompletedDataMap)
    {
        fun( (m_path + it.first).c_str() );
    }
}

