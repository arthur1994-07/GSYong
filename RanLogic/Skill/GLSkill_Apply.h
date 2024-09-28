
#pragma once

#include "../Item/GLItemDef.h"
#include "../Character/GLCharDefine.h"

#include "./GLSkill_ApplyData.h"
#include "./GLSkillDef.h"

namespace SKILL
{
	struct SAPPLY_100
	{
		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		DWORD				dwCUREFLAG;					//	해독 종류.
		SKILL::CDATA_LVL_100 sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.

		//	부가 효과 설정.
		EMIMPACT_ADDON		emADDON;					//	부가효과 종류.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	변화 수치.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.

		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC_100	sSPEC[MAX_LEVEL_BEFORE];			//	특수 기능 설정값.
	};

	struct SAPPLY_101
	{
		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		DWORD				dwCUREFLAG;					//	해독 종류.
		SKILL::CDATA_LVL_101 sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.

		//	부가 효과 설정.
		EMIMPACT_ADDON		emADDON;					//	부가효과 종류.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	변화 수치.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.

		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC_100	sSPEC[MAX_LEVEL_BEFORE];			//	특수 기능 설정값.
	};

	struct SAPPLY_102
	{
		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		DWORD				dwCUREFLAG;					//	해독 종류.
		SKILL::CDATA_LVL_101 sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.

		//	부가 효과 설정.
		EMIMPACT_ADDON		emADDON;					//	부가효과 종류.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	변화 수치.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.

		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC_100	sSPEC[MAX_LEVEL_BEFORE];			//	특수 기능 설정값.
	};

	struct SAPPLY_103	//	데이터 포맷 변경 진행
	{
		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		DWORD				dwCUREFLAG;					//	해독 종류.
		SKILL::CDATA_LVL_101 sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.

		//	부가 효과 설정.
		EMIMPACT_ADDON		emADDON;					//	부가효과 종류.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	변화 수치.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.

		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC_101		sSPEC[MAX_LEVEL_BEFORE];			//	특수 기능 설정값.
	};

	struct SAPPLY_104	// 사용시 조건 CP 추가
	{
		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		SKILL::CDATA_LVL_101 sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.
		DWORD				dwCUREFLAG;					//	해독 종류.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.

		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC_101		sSPEC[MAX_LEVEL_BEFORE];			//	특수 기능 설정값.

		//	부가 효과 설정.
		EMIMPACT_ADDON		emADDON;					//	부가효과 종류.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	변화 수치.	

	};

	struct SAPPLY_105	//	부가효과 및 특수효과 vector로 변경
	{
		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		SKILL::CDATA_LVL_102 sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.
		DWORD				dwCUREFLAG;					//	해독 종류.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.

		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC_101		sSPEC[MAX_LEVEL_BEFORE];			//	특수 기능 설정값.

		//	부가 효과 설정.
		EMIMPACT_ADDON		emADDON;					//	부가효과 종류.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	변화 수치.	
	};

	struct SAPPLY_106	// 레벨별 설정값 변경
	{
		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		SKILL::CDATA_LVL_102 sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.
		DWORD				dwCUREFLAG;					//	해독 종류.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.


		std::vector<SIMPACT_ADDON_100>	vecADDON;			//	부가효과
		std::vector<SSPEC_ADDON_100>	vecSPEC;			//	특수효과
	};

	struct SAPPLY_107	// 레벨별 설정값 변경
	{
		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		SKILL::CDATA_LVL_103 sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.
		DWORD				dwCUREFLAG;					//	해독 종류.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.


		std::vector<SIMPACT_ADDON_100>	vecADDON;			//	부가효과
		std::vector<SSPEC_ADDON_100>	vecSPEC;			//	특수효과
	};

	struct SAPPLY_108
	{
		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		SKILL::CDATA_LVL	sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.
		DWORD				dwCUREFLAG;					//	해독 종류.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.

		std::vector<SIMPACT_ADDON_100>	vecADDON;			//	부가효과
		std::vector<SSPEC_ADDON_100>	vecSPEC;			//	특수효과
	};

	struct SAPPLY_109	// 스킬효과 몬스터 적용비율 적용. by luxes.
	{
		// 이곳에다 변수 선언 하지 말 것;
		// 파일 저장 및 구버전 파일 불러오기 등 문제 생김;
		// 아래와 같이 되어있으면 다음 버전 추가시 힘듦!!;

		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		SKILL::CDATA_LVL	sDATA_LVL[MAX_LEVEL_BEFORE];		//	레벨별 설정 값.
		DWORD				dwCUREFLAG;					//	해독 종류.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	상태 이상 수치들.

		union 
		{
			int					nEffectFrame;			//  이팩트적용 타격점
			float				fRunningEffTime;		// 	런닝케스팅 시작시간.
		}; //uinon


		std::vector<SIMPACT_ADDON>	vecADDON;			//	부가효과
		std::vector<SSPEC_ADDON>	vecSPEC;			//	특수효과
		SSPEC_CA_ADDON_100			sCAData;			//  반격리스트 @@ vecSPEC설정에 반격이 있어야만 하며, vecSPEC설정에 반격은 최대 5개 까지 입력할수 있다.
	};
	// 109이후로는 스킬 최대 레벨이 9에서 24로 증가함;

	struct SAPPLY	// 스킬효과 몬스터 적용비율 적용. by luxes.
	{
		enum { VERSION = 0x0113 };

		// 이곳에다 변수 선언 하지 말 것;
		// 파일 저장 및 구버전 파일 불러오기 등 문제 생김;
		// 아래와 같이 되어있으면 다음 버전 추가시 힘듦!!;

		//	기본 적용값 설정.
		SKILL::EMTYPES		emBASIC_TYPE;				//	적용값 타입.
		EMELEMENT			emELEMENT;					//	( 수, 광, 화, 독, 없음 ) 속성.
		SKILL::CDATA_LVL	sDATA_LVL[MAX_LEVEL];		//	레벨별 설정 값.
		DWORD				dwCUREFLAG;					//	해독 종류.

		//	상태이상 설정.
		EMSTATE_BLOW		emSTATE_BLOW;				//	상태 이상 적용 타입.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL];		//	상태 이상 수치들.

		union 
		{
			int					nEffectFrame;			//  이팩트적용 타격점
			float				fRunningEffTime;		// 	런닝케스팅 시작시간.
		}; //uinon


		std::vector<SIMPACT_ADDON>	vecADDON;			//	부가효과
		std::vector<SSPEC_ADDON>	vecSPEC;			//	특수효과
		SSPEC_CA_ADDON				sCAData;			//  반격리스트 @@ vecSPEC설정에 반격이 있어야만 하며, vecSPEC설정에 반격은 최대 5개 까지 입력할수 있다.

		void Assign ( SAPPLY_100 &sOldData );
		void Assign ( SAPPLY_101 &sOldData );
		void Assign ( SAPPLY_102 &sOldData );
		void Assign ( SAPPLY_103 &sOldData );
		void Assign ( SAPPLY_104 &sOldData );
		void Assign ( SAPPLY_105 &sOldData );

		void LOAD_106 ( sc::BaseStream &SFile );
		void LOAD_107 ( sc::BaseStream &SFile );
		void LOAD_108 ( sc::BaseStream &SFile );
		void LOAD_109 ( sc::BaseStream &SFile );
		void LOAD_110 ( sc::BaseStream &SFile );
		void LOAD_111 ( sc::BaseStream &SFile );
		void LOAD_112 ( sc::BaseStream &SFile );
		void LOAD_113 ( sc::BaseStream &SFile );
		void SAVE ( sc::SerialFile &SFile );

		SAPPLY();
		static VOID SaveCsvHead(std::fstream& SFile);
		VOID SaveCsv(std::fstream& SFile);
		VOID LoadCsv(CStringArray& StrArray);
		inline SKILL::EMTYPES BasicType() const { return emBASIC_TYPE; }	//! 적용값 타입
	};
};