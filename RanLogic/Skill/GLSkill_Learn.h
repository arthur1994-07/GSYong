#pragma once

#include "./GLSkillDef.h"
#include "./GLSkill_LearnData.h"
#include "../Character/GLCharDefine.h"

namespace SKILL
{
	struct SLEARN_100
	{
		EMBRIGHT			emBRIGHT;					//	속성.
		DWORD				dwCLASS;					//	직업.
		SNATIVEID			sSKILL;						//	요구 보유스킬.

		SLEARN_LVL			sLVL_STEP[MAX_LEVEL_BEFORE];		//	레벨 별 요구 수치.
	};

	struct SLEARN_101	//	데이터 포맷 변경 진행
	{
		EMBRIGHT			emBRIGHT;					//	속성.
		DWORD				dwCLASS;					//	직업.
		SNATIVEID			sSKILL;						//	요구 보유스킬.

		SLEARN_LVL_101		sLVL_STEP[MAX_LEVEL_BEFORE];		//	레벨 별 요구 수치.
	};

	struct SLEARN_102
	{
		DWORD				dwCLASS;					//	직업.
		EMBRIGHT			emBRIGHT;					//	속성.
		SNATIVEID			sSKILL;						//	요구 보유스킬.

		SLEARN_LVL_102		sLVL_STEP[MAX_LEVEL_BEFORE];		//	레벨 별 요구 수치.
	};

	struct SLEARN_103
	{
		enum { VERSION = 0x0103, };

		DWORD				dwCLASS;					//	직업.
		EMBRIGHT			emBRIGHT;					//	속성.
		SNATIVEID			sSKILL;						//	요구 보유스킬.

		SLEARN_LVL_103		sLVL_STEP[MAX_LEVEL_BEFORE];		//	레벨 별 요구 수치.

		BOOL				bOnlyOneStats;				//	스탯 한가지 만족 시 가능 유무
		SNATIVEID			sHiddenWeapon;				//	글러브 일 경우 스킬 사용 시 변경아이템
	};

	struct SLEARN_104
	{
		enum { VERSION = 0x0104, };

		DWORD				dwCLASS;					//	직업.
		EMBRIGHT			emBRIGHT;					//	속성.
		SNATIVEID			sSKILL;						//	요구 보유스킬.

		SLEARN_LVL_103		sLVL_STEP[MAX_LEVEL_BEFORE];		//	레벨 별 요구 수치.

		BOOL				bOnlyOneStats;				//	스탯 한가지 만족 시 가능 유무
		SNATIVEID			sHiddenWeapon;				//	스킬 사용 시 2차무기 출력 아이템
		BOOL				bHiddenWeapon;				//	스킬 사용 2차무기 출력 유무
	};

	struct SLEARN
	{
		enum { VERSION = 0x0105, };

		DWORD				dwCLASS;					//	직업.
		EMBRIGHT			emBRIGHT;					//	속성.
		SNATIVEID			sSKILL;						//	요구 보유스킬.

		SLEARN_LVL			sLVL_STEP[MAX_LEVEL];		//	레벨 별 요구 수치.

		BOOL				bOnlyOneStats;				//	스탯 한가지 만족 시 가능 유무
		SNATIVEID			sHiddenWeapon;				//	스킬 사용 시 2차무기 출력 아이템
		BOOL				bHiddenWeapon;				//	스킬 사용 2차무기 출력 유무

		SLEARN (void);

		void Assign ( SLEARN_100 &OldData );
		void Assign ( SLEARN_101 &OldData );
		void Assign ( SLEARN_102 &OldData );
		void Assign ( SLEARN_103 &OldData );
		void Assign ( SLEARN_104 &OldData );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};
};