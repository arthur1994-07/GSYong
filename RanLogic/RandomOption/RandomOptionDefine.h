#pragma once

#include <map>
#include <vector>
#include <list>
#include <StrSafe.h>
#include "../../SigmaCore/Math/Random.h"

class gltexfile;
class GLogicFile;
class GLogicData;
namespace sc
{
	class SerialFile;
	class BaseStream;
}
namespace RandomOption
{
	enum
	{
		LENGTH_NAME = 64,
	};
	enum LOAD_TYPE
	{
		LOAD_CLIENT,
		LOAD_SERVER,
		LOAD_TOOL,
	};

	enum GENERATE
	{
		GENERATE_DROP,
		GENERATE_REMODEL,
		GENERATE_NSIZE,
	};
	const char cNameGenerate[RandomOption::GENERATE_NSIZE][8] =
	{
		"���",
		"����",
	};
	enum RANDOM_OPTION
	{
		RANDOM_OPTION_NULL = 0,
		RANDOM_OPTION_RATE_DAMAGE = 1, // ���ݷ�(����);
		RANDOM_OPTION_RATE_DEFENCE = 2,	//	����(����);
		RANDOM_OPTION_RATE_HIT = 3, // ����(����);
		RANDOM_OPTION_RATE_AVOID = 4, // ȸ��(����);

		RANDOM_OPTION_VALUE_HP = 5, // hp ũ�� ��ȭ(��);
		RANDOM_OPTION_VALUE_MP = 6, // mp ũ�� ��ȭ(��);
		RANDOM_OPTION_VALUE_SP = 7, // sp ũ�� ��ȭ(��);

		RANDOM_OPTION_RATE_RECOVERY_HP = 8, // hp ȸ��(����);
		RANDOM_OPTION_RATE_RECOVERY_MP = 9, // mp ȸ��(����);
		RANDOM_OPTION_RATE_RECOVERY_SP = 10, // sp ȸ��(����);
		RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP = 11, // hp, mp, sp ȸ��(����);

		RANDOM_OPTION_GRIND_DAMAGE = 12, // ���ݷ� ����;
		RANDOM_OPTION_GRIND_DEFENCE = 13, // ���� ����;

		RANDOM_OPTION_RATE_RANGE_ATTACK = 14, // ���� ����(����);
		RANDOM_OPTION_CONSUME_SP = 15, // sp �Ҹ�(��);
		RANDOM_OPTION_VALUE_RESIST = 16, // ����(��);

		RANDOM_OPTION_RATE_SPEED_MOVE = 17, // �̵� �ӵ�(����);

		RANDOM_OPTION_VALUE_DAMAGE = 18, // ���ݷ�(��);
		RANDOM_OPTION_VALUE_DEFENCE = 19, // ����(��);

		RANDOM_OPTION_VALUE_HIT = 20, // ������(��);
		RANDOM_OPTION_VALUE_AVOID = 21, // ȸ����(��);

		RANDOM_OPTION_VALUE_POW = 22, // ��(��);
		RANDOM_OPTION_VALUE_STR = 23, // ü��(��);
		RANDOM_OPTION_VALUE_SPI = 24, // ����(��);
		RANDOM_OPTION_VALUE_DEX = 25,	 // ��ø(��);
		RANDOM_OPTION_VALUE_STA = 26, // �ٷ�(��);

		RANDOM_OPTION_VALUE_MELEE = 27, // ����ġ(��);
		RANDOM_OPTION_VALUE_RANGE = 28, // ���ġ(��);
		RANDOM_OPTION_VALUE_MAGIC = 29, // ����ġ(��);

		RANDOM_OPTION_VALUE_POTION_HP = 30, // ���࿡ ���� hp ȸ��(��);
		RANDOM_OPTION_VALUE_POTION_MP = 31, // ���࿡ ���� mp ȸ��(��);
		RANDOM_OPTION_VALUE_POTION_SP = 32, // ���࿡ ���� shp ȸ��(��);
		RANDOM_OPTION_VALUE_KILL_CP = 33, // ���� ų�� CP ��ȭ(��);

		RANDOM_OPTION_VALUE_SKILL_GRADE_1_DURATIONTIME_ADD  = 34,
		RANDOM_OPTION_VALUE_SKILL_GRADE_2_DURATIONTIME_ADD  = 35,
		RANDOM_OPTION_VALUE_SKILL_GRADE_3_DURATIONTIME_ADD  = 36,
		RANDOM_OPTION_VALUE_SKILL_GRADE_4_DURATIONTIME_ADD  = 37,
		RANDOM_OPTION_VALUE_SKILL_GRADE_5_DURATIONTIME_ADD  = 38,
		RANDOM_OPTION_VALUE_SKILL_GRADE_6_DURATIONTIME_ADD  = 39,
		RANDOM_OPTION_VALUE_SKILL_GRADE_7_DURATIONTIME_ADD  = 40,
		RANDOM_OPTION_VALUE_SKILL_GRADE_8_DURATIONTIME_ADD  = 41,
		RANDOM_OPTION_VALUE_SKILL_GRADE_9_DURATIONTIME_ADD  = 42,
		RANDOM_OPTION_VALUE_SKILL_GRADE_10_DURATIONTIME_ADD = 43,
		RANDOM_OPTION_VALUE_SKILL_GRADE_11_DURATIONTIME_ADD = 44,
		RANDOM_OPTION_VALUE_SKILL_GRADE_12_DURATIONTIME_ADD = 45,
		RANDOM_OPTION_VALUE_SKILL_GRADE_13_DURATIONTIME_ADD = 46,
		RANDOM_OPTION_VALUE_SKILL_GRADE_14_DURATIONTIME_ADD = 47,
		RANDOM_OPTION_VALUE_SKILL_GRADE_15_DURATIONTIME_ADD = 48,
		RANDOM_OPTION_VALUE_SKILL_GRADE_16_DURATIONTIME_ADD = 49,
		RANDOM_OPTION_VALUE_SKILL_GRADE_17_DURATIONTIME_ADD = 50,
		RANDOM_OPTION_VALUE_SKILL_GRADE_18_DURATIONTIME_ADD = 51,
		RANDOM_OPTION_VALUE_SKILL_GRADE_19_DURATIONTIME_ADD = 52,
		RANDOM_OPTION_VALUE_SKILL_GRADE_20_DURATIONTIME_ADD = 53,
		RANDOM_OPTION_VALUE_SKILL_GRADE_21_DURATIONTIME_ADD = 54,
		RANDOM_OPTION_VALUE_SKILL_GRADE_22_DURATIONTIME_ADD = 55,
		RANDOM_OPTION_VALUE_SKILL_GRADE_23_DURATIONTIME_ADD = 56,
		RANDOM_OPTION_VALUE_SKILL_GRADE_24_DURATIONTIME_ADD = 57,
		RANDOM_OPTION_VALUE_SKILL_GRADE_25_DURATIONTIME_ADD = 58,

		RANDOM_OPTION_VALUE_SKILL_GRADE_1_DAMAGE_ADD  = 59,
		RANDOM_OPTION_VALUE_SKILL_GRADE_2_DAMAGE_ADD  = 60,
		RANDOM_OPTION_VALUE_SKILL_GRADE_3_DAMAGE_ADD  = 61,
		RANDOM_OPTION_VALUE_SKILL_GRADE_4_DAMAGE_ADD  = 62,
		RANDOM_OPTION_VALUE_SKILL_GRADE_5_DAMAGE_ADD  = 63,
		RANDOM_OPTION_VALUE_SKILL_GRADE_6_DAMAGE_ADD  = 64,
		RANDOM_OPTION_VALUE_SKILL_GRADE_7_DAMAGE_ADD  = 65,
		RANDOM_OPTION_VALUE_SKILL_GRADE_8_DAMAGE_ADD  = 66,
		RANDOM_OPTION_VALUE_SKILL_GRADE_9_DAMAGE_ADD  = 67,
		RANDOM_OPTION_VALUE_SKILL_GRADE_10_DAMAGE_ADD = 68,
		RANDOM_OPTION_VALUE_SKILL_GRADE_11_DAMAGE_ADD = 69,
		RANDOM_OPTION_VALUE_SKILL_GRADE_12_DAMAGE_ADD = 70,
		RANDOM_OPTION_VALUE_SKILL_GRADE_13_DAMAGE_ADD = 71,
		RANDOM_OPTION_VALUE_SKILL_GRADE_14_DAMAGE_ADD = 72,
		RANDOM_OPTION_VALUE_SKILL_GRADE_15_DAMAGE_ADD = 73,
		RANDOM_OPTION_VALUE_SKILL_GRADE_16_DAMAGE_ADD = 74,
		RANDOM_OPTION_VALUE_SKILL_GRADE_17_DAMAGE_ADD = 75,
		RANDOM_OPTION_VALUE_SKILL_GRADE_18_DAMAGE_ADD = 76,
		RANDOM_OPTION_VALUE_SKILL_GRADE_19_DAMAGE_ADD = 77,
		RANDOM_OPTION_VALUE_SKILL_GRADE_20_DAMAGE_ADD = 78,
		RANDOM_OPTION_VALUE_SKILL_GRADE_21_DAMAGE_ADD = 79,
		RANDOM_OPTION_VALUE_SKILL_GRADE_22_DAMAGE_ADD = 80,
		RANDOM_OPTION_VALUE_SKILL_GRADE_23_DAMAGE_ADD = 81,
		RANDOM_OPTION_VALUE_SKILL_GRADE_24_DAMAGE_ADD = 82,
		RANDOM_OPTION_VALUE_SKILL_GRADE_25_DAMAGE_ADD = 83,

		RANDOM_OPTION_VALUE_SKILL_GRADE_1_SKILLDILAY_DEC  = 84,
		RANDOM_OPTION_VALUE_SKILL_GRADE_2_SKILLDILAY_DEC  = 85,
		RANDOM_OPTION_VALUE_SKILL_GRADE_3_SKILLDILAY_DEC  = 86,
		RANDOM_OPTION_VALUE_SKILL_GRADE_4_SKILLDILAY_DEC  = 87,
		RANDOM_OPTION_VALUE_SKILL_GRADE_5_SKILLDILAY_DEC  = 88,
		RANDOM_OPTION_VALUE_SKILL_GRADE_6_SKILLDILAY_DEC  = 89,
		RANDOM_OPTION_VALUE_SKILL_GRADE_7_SKILLDILAY_DEC  = 90,
		RANDOM_OPTION_VALUE_SKILL_GRADE_8_SKILLDILAY_DEC  = 91,
		RANDOM_OPTION_VALUE_SKILL_GRADE_9_SKILLDILAY_DEC  = 92,
		RANDOM_OPTION_VALUE_SKILL_GRADE_10_SKILLDILAY_DEC = 93,
		RANDOM_OPTION_VALUE_SKILL_GRADE_11_SKILLDILAY_DEC = 94,
		RANDOM_OPTION_VALUE_SKILL_GRADE_12_SKILLDILAY_DEC = 95,
		RANDOM_OPTION_VALUE_SKILL_GRADE_13_SKILLDILAY_DEC = 96,
		RANDOM_OPTION_VALUE_SKILL_GRADE_14_SKILLDILAY_DEC = 97,
		RANDOM_OPTION_VALUE_SKILL_GRADE_15_SKILLDILAY_DEC = 98,
		RANDOM_OPTION_VALUE_SKILL_GRADE_16_SKILLDILAY_DEC = 99,
		RANDOM_OPTION_VALUE_SKILL_GRADE_17_SKILLDILAY_DEC = 100,
		RANDOM_OPTION_VALUE_SKILL_GRADE_18_SKILLDILAY_DEC = 101,
		RANDOM_OPTION_VALUE_SKILL_GRADE_19_SKILLDILAY_DEC = 102,
		RANDOM_OPTION_VALUE_SKILL_GRADE_20_SKILLDILAY_DEC = 103,
		RANDOM_OPTION_VALUE_SKILL_GRADE_21_SKILLDILAY_DEC = 104,
		RANDOM_OPTION_VALUE_SKILL_GRADE_22_SKILLDILAY_DEC = 105,
		RANDOM_OPTION_VALUE_SKILL_GRADE_23_SKILLDILAY_DEC = 106,
		RANDOM_OPTION_VALUE_SKILL_GRADE_24_SKILLDILAY_DEC = 107,
		RANDOM_OPTION_VALUE_SKILL_GRADE_25_SKILLDILAY_DEC = 108,
		RANDOM_OPTION_VALUE_CRITICALDAMAGE_ADD		= 109,
		RANDOM_OPTION_VALUE_CRUSHINGBLOW_ADD		= 110,
		RANDOM_OPTION_VALUE_WEARABLELEVEL_ADD		= 111,
		RANDOM_OPTION_VALUE_LIFE_PER_KILL			= 112,			// óġ�� ����� ȸ����;
		RANDOM_OPTION_VALUE_MP_PER_KILL				= 113,				// óġ�� MP ȸ����;
		RANDOM_OPTION_VALUE_SP_PER_KILL				= 114,				// óġ�� SP ȸ����;
		RANDOM_OPTION_VALUE_HPMPSP_PER_KILL			= 115,			// óġ�� HP+MP+SP ȸ����;
		RANDOM_OPTION_VALUE_ALLMPRATIO_TO_ATTACK	= 116,		// ��ü MP ������ŭ ���ݷ�����;
		RANDOM_OPTION_VALUE_ALLSPRATIO_TO_HP		= 117,			// ��ü SP ������ŭ ü������;
		RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MELEE	= 118,			// ��ü ���� ������ŭ ����ġ ����;
		RANDOM_OPTION_VALUE_ALLHITRATIO_TO_RANGE	= 119,			// ��ü ���� ������ŭ ���ġ ����;
		RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MAGIC	= 120,			// ��ü ���� ������ŭ ���ġ ����;
		RANDOM_OPTION_NSIZE = 121,
	};

	const bool bValueInteger[RandomOption::RANDOM_OPTION_NSIZE] =
	{
		true, // RANDOM_OPTION_NULL = 0,
		false, // RANDOM_OPTION_RATE_DAMAGE = 1, // ���ݷ�(����);
		false, // RANDOM_OPTION_RATE_DEFENCE = 2,	//	����(����);
		false, // RANDOM_OPTION_RATE_HIT = 3, // ����(����);
		false, // RANDOM_OPTION_RATE_AVOID = 4, // ȸ��(����);
		true, // RANDOM_OPTION_VALUE_HP = 5, // hp ũ�� ��ȭ(��);
		true, // RANDOM_OPTION_VALUE_MP = 6, // mp ũ�� ��ȭ(��);
		true, // RANDOM_OPTION_VALUE_SP = 7, // sp ũ�� ��ȭ(��);
		false, // RANDOM_OPTION_RATE_RECOVERY_HP = 8, // hp ȸ��(����);
		false, // RANDOM_OPTION_RATE_RECOVERY_MP = 9, // mp ȸ��(����);
		false, // RANDOM_OPTION_RATE_RECOVERY_SP = 10, // sp ȸ��(����);
		false, // RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP = 11, // hp, mp, sp ȸ��(����);
		true, // RANDOM_OPTION_GRIND_DAMAGE = 12, // ���ݷ� ����;
		true, // RANDOM_OPTION_GRIND_DEFENCE = 13, // ���� ����;
		false, // RANDOM_OPTION_RATE_RANGE_ATTACK = 14, // ���� ����(����);
		true, // RANDOM_OPTION_CONSUME_SP = 15, // sp �Ҹ�(��);
		true, // RANDOM_OPTION_VALUE_RESIST = 16, // ����(��);
		false, // RANDOM_OPTION_RATE_SPEED_MOVE = 17, // �̵� �ӵ�(����);
		true, // RANDOM_OPTION_VALUE_DAMAGE = 18, // ���ݷ�(��);
		true, // RANDOM_OPTION_VALUE_DEFENCE = 19, // ����(��);
		true, // RANDOM_OPTION_VALUE_HIT = 20, // ������(��);
		true, // RANDOM_OPTION_VALUE_AVOID = 21, // ȸ����(��);
		true, // RANDOM_OPTION_VALUE_POW = 22, // ��(��);
		true, // RANDOM_OPTION_VALUE_STR = 23, // ü��(��);
		true, // RANDOM_OPTION_VALUE_SPI = 24, // ����(��);
		true, // RANDOM_OPTION_VALUE_DEX = 25,	 // ��ø(��);
		true, // RANDOM_OPTION_VALUE_STA = 26, // �ٷ�(��);
		true, // RANDOM_OPTION_MELEE = 27, // ����ġ(��);
		true, // RANDOM_OPTION_RANGE = 28, // ���ġ(��);
		true, // RANDOM_OPTION_MAGIC = 29, // ����ġ(��);
		true, // RANDOM_OPTION_VALUE_POTION_HP = 30, // ���࿡ ���� hp ȸ��(��);
		true, // RANDOM_OPTION_VALUE_POTION_MP = 31, // ���࿡ ���� mp ȸ��(��);
		true, // RANDOM_OPTION_VALUE_POTION_SP = 32, // ���࿡ ���� shp ȸ��(��);
		true, // RANDOM_OPTION_VALUE_KILL_CP = 33, // ���� ų�� CP ��ȭ(��);
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_1_DURATIONTIME_ADD ��ų��޺� ���ӽð� ����;
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_2_DURATIONTIME_ADD  
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_3_DURATIONTIME_ADD  
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_4_DURATIONTIME_ADD  
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_5_DURATIONTIME_ADD  
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_6_DURATIONTIME_ADD  
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_7_DURATIONTIME_ADD  
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_8_DURATIONTIME_ADD  
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_9_DURATIONTIME_ADD  
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_10_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_11_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_12_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_13_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_14_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_15_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_16_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_17_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_18_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_19_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_20_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_21_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_22_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_23_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_24_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_25_DURATIONTIME_ADD 
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_1_DAMAGE_ADD  = 59, ��ų ��޺� ������ ����%;
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_2_DAMAGE_ADD  = 60,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_3_DAMAGE_ADD  = 61,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_4_DAMAGE_ADD  = 62,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_5_DAMAGE_ADD  = 63,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_6_DAMAGE_ADD  = 64,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_7_DAMAGE_ADD  = 65,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_8_DAMAGE_ADD  = 66,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_9_DAMAGE_ADD  = 67,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_10_DAMAGE_ADD = 68,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_11_DAMAGE_ADD = 69,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_12_DAMAGE_ADD = 70,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_13_DAMAGE_ADD = 71,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_14_DAMAGE_ADD = 72,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_15_DAMAGE_ADD = 73,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_16_DAMAGE_ADD = 74,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_17_DAMAGE_ADD = 75,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_18_DAMAGE_ADD = 76,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_19_DAMAGE_ADD = 77,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_20_DAMAGE_ADD = 78,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_21_DAMAGE_ADD = 79,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_22_DAMAGE_ADD = 80,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_23_DAMAGE_ADD = 81,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_24_DAMAGE_ADD = 82,
		false, // RANDOM_OPTION_VALUE_SKILL_GRADE_25_DAMAGE_ADD = 83,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_1_SKILLDILAY_DEC  = 84, ��޺� ������ ����;
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_2_SKILLDILAY_DEC  = 85,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_3_SKILLDILAY_DEC  = 86,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_4_SKILLDILAY_DEC  = 87,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_5_SKILLDILAY_DEC  = 88,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_6_SKILLDILAY_DEC  = 89,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_7_SKILLDILAY_DEC  = 90,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_8_SKILLDILAY_DEC  = 91,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_9_SKILLDILAY_DEC  = 92,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_10_SKILLDILAY_DEC = 93,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_11_SKILLDILAY_DEC = 94,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_12_SKILLDILAY_DEC = 95,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_13_SKILLDILAY_DEC = 96,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_14_SKILLDILAY_DEC = 97,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_15_SKILLDILAY_DEC = 98,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_16_SKILLDILAY_DEC = 99,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_17_SKILLDILAY_DEC = 100,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_18_SKILLDILAY_DEC = 101,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_19_SKILLDILAY_DEC = 102,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_20_SKILLDILAY_DEC = 103,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_21_SKILLDILAY_DEC = 104,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_22_SKILLDILAY_DEC = 105,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_23_SKILLDILAY_DEC = 106,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_24_SKILLDILAY_DEC = 107,
		false,// RANDOM_OPTION_VALUE_SKILL_GRADE_25_SKILLDILAY_DEC = 108,
		false,// RANDOM_OPTION_VALUE_CRITICALDAMAGE_ADD = 109, ũ��Ƽ�� ������ ����
		false,// RANDOM_OPTION_VALUE_CRUSHINGBLOW_ADD = 110, ����Ÿ�� ������ ������
		true, // RANDOM_OPTION_VALUE_WEARABLELEVEL_DEC = 111, ���뷹�� ����;
		false,// RANDOM_OPTION_VALUE_LIFE_PER_KILL = 112, // óġ�� ����� ȸ����;
		false,// RANDOM_OPTION_VALUE_MP_PER_KILL = 112, // óġ�� MP ȸ����;
		false,// RANDOM_OPTION_VALUE_SP_PER_KILL = 114, // óġ�� SP ȸ����;
		false,// RANDOM_OPTION_VALUE_HPMPSP_PER_KILL = 115, // óġ�� HP+MP+SP ȸ����;
		false,// RANDOM_OPTION_VALUE_ALLMPRATIO_TO_ATTACK = 116,		// ��ü MP ������ŭ ���ݷ�����;
		false,// RANDOM_OPTION_VALUE_ALLSPRATIO_TO_HP = 117,			// ��ü SP ������ŭ ü������;
		false,// RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MELEE = 118,			// ��ü ���� ������ŭ ����ġ ����;
		false,// RANDOM_OPTION_VALUE_ALLHITRATIO_TO_RANGE = 119,			// ��ü ���� ������ŭ ���ġ ����;
		false,// RANDOM_OPTION_VALUE_ALLHITRATIO_TO_MAGIC = 120,			// ��ü ���� ������ŭ ���ġ ����;
	};

	const char cNameValue[RandomOption::RANDOM_OPTION_NSIZE][32] =
	{
		"null", // RANDOM_OPTION_NULL = 0,
		"����(����)", // RANDOM_OPTION_RATE_DAMAGE = 1, // ���ݷ�(����);
		"���(����)", // RANDOM_OPTION_RATE_DEFENCE = 2,	//	����(����);
		"����(����)", // RANDOM_OPTION_RATE_HIT = 3, // ����(����);
		"ȸ��(����)", // RANDOM_OPTION_RATE_AVOID = 4, // ȸ��(����);
		"hp ũ��(��)", // RANDOM_OPTION_VALUE_HP = 5, // hp ũ�� ��ȭ(��);
		"mp ũ��(��)", // RANDOM_OPTION_VALUE_MP = 6, // mp ũ�� ��ȭ(��);
		"sp ũ��(��)", // RANDOM_OPTION_VALUE_SP = 7, // sp ũ�� ��ȭ(��);
		"hp ȸ��(����)", // RANDOM_OPTION_RATE_RECOVERY_HP = 8, // hp ȸ��(����);
		"mp ȸ��(����)", // RANDOM_OPTION_RATE_RECOVERY_MP = 9, // mp ȸ��(����);
		"sp ȸ��(����)", // RANDOM_OPTION_RATE_RECOVERY_SP = 10, // sp ȸ��(����);
		"hp/mp/sp ȸ��(����)", // RANDOM_OPTION_RATE_RECOVERY_HP_MP_SP = 11, // hp, mp, sp ȸ��(����);
		"���� ����", // RANDOM_OPTION_GRIND_DAMAGE = 12, // ���ݷ� ����;
		"��� ����", // RANDOM_OPTION_GRIND_DEFENCE = 13, // ���� ����;
		"���� ����(����)", // RANDOM_OPTION_RATE_RANGE_ATTACK = 14, // ���� ����(����);
		"sp �Ҹ�(��)", // RANDOM_OPTION_CONSUME_SP = 15, // sp �Ҹ�(��);
		"����(��)", // RANDOM_OPTION_VALUE_RESIST = 16, // ����(��);
		"�̵� �ӵ�(����)", // RANDOM_OPTION_RATE_SPEED_MOVE = 17, // �̵� �ӵ�(����);
		"����(��)", // RANDOM_OPTION_VALUE_DAMAGE = 18, // ���ݷ�(��);
		"���(��)", // RANDOM_OPTION_VALUE_DEFENCE = 19, // ����(��);
		"����(��)", // RANDOM_OPTION_VALUE_HIT = 20, // ������(��);
		"ȸ��(��)", // RANDOM_OPTION_VALUE_AVOID = 21, // ȸ����(��);
		"��(��)", // RANDOM_OPTION_VALUE_POW = 22, // ��(��);
		"ü��(��)", // RANDOM_OPTION_VALUE_STR = 23, // ü��(��);
		"����(��)", // RANDOM_OPTION_VALUE_SPI = 24, // ����(��);
		"��ø(��)", // RANDOM_OPTION_VALUE_DEX = 25,	 // ��ø(��);
		"�ٷ�(��)", // RANDOM_OPTION_VALUE_STA = 26, // �ٷ�(��);
		"����(��)", // RANDOM_OPTION_MELEE = 27, // ����ġ(��);
		"���(��)", // RANDOM_OPTION_RANGE = 28, // ���ġ(��);
		"����(��)", // RANDOM_OPTION_MAGIC = 29, // ����ġ(��);
		"hpȸ��[����](��)", // RANDOM_OPTION_VALUE_POTION_HP = 30, // ���࿡ ���� hp ȸ��(��);
		"mpȸ��[����](��)", // RANDOM_OPTION_VALUE_POTION_MP = 31, // ���࿡ ���� mp ȸ��(��);
		"spȸ��[����](��)", // RANDOM_OPTION_VALUE_POTION_SP = 32, // ���࿡ ���� sp ȸ��(��);
		"����ų��cp��ȭ(��)", // RANDOM_OPTION_VALUE_KILL_CP = 33, // ���� ų�� CP ��ȭ(��);
		"1��� ��ų ���ӽð�",
		"2��� ��ų ���ӽð�",
		"3��� ��ų ���ӽð�",
		"4��� ��ų ���ӽð�",
		"5��� ��ų ���ӽð�",
		"6��� ��ų ���ӽð�",
		"7��� ��ų ���ӽð�",
		"8��� ��ų ���ӽð�",
		"9��� ��ų ���ӽð�",
		"10��� ��ų ���ӽð�",
		"11��� ��ų ���ӽð�",
		"12��� ��ų ���ӽð�",
		"13��� ��ų ���ӽð�",
		"14��� ��ų ���ӽð�",
		"15��� ��ų ���ӽð�",
		"16��� ��ų ���ӽð�",
		"17��� ��ų ���ӽð�",
		"18��� ��ų ���ӽð�",
		"19��� ��ų ���ӽð�",
		"20��� ��ų ���ӽð�",
		"21��� ��ų ���ӽð�",
		"22��� ��ų ���ӽð�",
		"23��� ��ų ���ӽð�",
		"24��� ��ų ���ӽð�",
		"25��� ��ų ���ӽð�",
		"1��� ��ų ������ ������",
		"2��� ��ų ������ ������",
		"3��� ��ų ������ ������",
		"4��� ��ų ������ ������",
		"5��� ��ų ������ ������",
		"6��� ��ų ������ ������",
		"7��� ��ų ������ ������",
		"8��� ��ų ������ ������",
		"9��� ��ų ������ ������",
		"10��� ��ų ������ ������",
		"11��� ��ų ������ ������",
		"12��� ��ų ������ ������",
		"13��� ��ų ������ ������",
		"14��� ��ų ������ ������",
		"15��� ��ų ������ ������",
		"16��� ��ų ������ ������",
		"17��� ��ų ������ ������",
		"18��� ��ų ������ ������",
		"19��� ��ų ������ ������",
		"20��� ��ų ������ ������",
		"21��� ��ų ������ ������",
		"22��� ��ų ������ ������",
		"23��� ��ų ������ ������",
		"24��� ��ų ������ ������",
		"25��� ��ų ������ ������",
		"1��� ��ų ������ ����",
		"2��� ��ų ������ ����",
		"3��� ��ų ������ ����",
		"4��� ��ų ������ ����",
		"5��� ��ų ������ ����",
		"6��� ��ų ������ ����",
		"7��� ��ų ������ ����",
		"8��� ��ų ������ ����",
		"9��� ��ų ������ ����",
		"10��� ��ų ������ ����",
		"11��� ��ų ������ ����",
		"12��� ��ų ������ ����",
		"13��� ��ų ������ ����",
		"14��� ��ų ������ ����",
		"15��� ��ų ������ ����",
		"16��� ��ų ������ ����",
		"17��� ��ų ������ ����",
		"18��� ��ų ������ ����",
		"19��� ��ų ������ ����",
		"20��� ��ų ������ ����",
		"21��� ��ų ������ ����",
		"22��� ��ų ������ ����",
		"23��� ��ų ������ ����",
		"24��� ��ų ������ ����",
		"25��� ��ų ������ ����",
		"ũ��Ƽ�� ������",
		"����Ÿ�� ������",
		"������ ���뷹�� ����",
		"óġ�� HP ȸ����",
		"óġ�� MP ȸ����",
		"óġ�� SP ȸ����",
		"óġ�� HP+MP+SP ȸ����",
		"��ü MP ������ŭ ���ݷ�����",
		"��ü SP ������ŭ ü������",
		"��ü ���� ������ŭ ����ġ ����",
		"��ü ���� ������ŭ ���ġ ����",
		"��ü ���� ������ŭ ���ġ ����"
	};

	class Information
	{
	public:
		bool bValueInteger;
		float fRateChoose;
		union
		{
			struct 
			{
				int valueIntegerHigh;
				int valueIntegerLow;
			};
			struct
			{
				float valueFloatHigh;
				float valueFloatLow;
			};
		};
	public:
		Information():valueIntegerHigh(0), valueIntegerLow(0)
		{
		}
	};

	class ValueTemporary
	{
	public:
		ValueTemporary(void)
			: valueIntegerHighCompare(-2100000000)
			, valueIntegerLowCompare(2100000000)
			, valueFloatHighCompare(-210000000.0f)
			, valueFloatLowCompare(210000000.0f)
		{
		}
		int valueIntegerHighCompare;
		int valueIntegerLowCompare;
		float valueFloatHighCompare;
		float valueFloatLowCompare;
	};
}


