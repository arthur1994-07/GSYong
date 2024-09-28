
#pragma once

#include "./GLSkillDef.h"
#include "../Character/GLCharDefine.h"
#include "./GLSkill_SpecialSkillData.h"

namespace SKILL
{
	struct SSPECIAL_SKILL_102
	{
		EMSPECIALSKILLTYPE	emSSTYPE;					//  특수 스킬 타입
		std::string			strEffectName;				//  변신시 이펙트, 소환시 게이트 이펙트

		// 변신 관련 데이터
		DWORD				dwRemainSecond;				//  남은 초
		std::string			strTransform_Man[EMTRANSFORM_NSIZE];	//	변신시 사용하는 파일이름
		std::string			strTransform_Woman[EMTRANSFORM_NSIZE];	//	변신시 사용하는 파일이름

		// 소환 관련 데이터
		SNATIVEID			sCreatureID;			// 소환수 ID
		DWORD				dwRecallDelaySec;		// 소환 딜레이 초
		DWORD				dwCreatureActTimeSec;	// 소환수 활동 시간

		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SSPECIAL_SKILL_104
	{
		EMSPECIALSKILLTYPE	emSSTYPE;					//  특수 스킬 타입
		std::string			strEffectName;				//  변신시 이펙트, 소환시 게이트 이펙트


		// 변신 관련 데이터
		DWORD				dwRemainSecond;				//  남은 초
		std::string			strTransform_Man[EMTRANSFORM_NSIZE];	//	변신시 사용하는 파일이름
		std::string			strTransform_Woman[EMTRANSFORM_NSIZE];	//	변신시 사용하는 파일이름

		// 소환 관련 데이터
		SCREATURE_LVL       sCREATURE_LVL[MAX_LEVEL_BEFORE];
		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SSPECIAL_SKILL
	{
		enum { VERSION = 0x0105, };

		EMSPECIALSKILLTYPE	emSSTYPE;					//  특수 스킬 타입
		std::string			strEffectName;				//  변신시 이펙트, 소환시 게이트 이펙트


		// 변신 관련 데이터
		DWORD				dwRemainSecond;				//  남은 초
		std::string			strTransform_Man[EMTRANSFORM_NSIZE];	//	변신시 사용하는 파일이름
		std::string			strTransform_Woman[EMTRANSFORM_NSIZE];	//	변신시 사용하는 파일이름

		// 소환 관련 데이터
		SCREATURE_LVL       sCREATURE_LVL[MAX_LEVEL];

		SSPECIAL_SKILL();
		void ResetAll();

		BOOL SAVE ( sc::SerialFile &SFile );
		BOOL LOAD ( sc::BaseStream &SFile );
		BOOL LOAD_100 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
		BOOL LOAD_101 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
		BOOL LOAD_102 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
		BOOL LOAD_103 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
		BOOL LOAD_104 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		SSPECIAL_SKILL & operator= ( const SSPECIAL_SKILL  &value );
	};
};