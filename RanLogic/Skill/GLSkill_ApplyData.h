
#pragma once

#include "./GLSkillDef.h"
#include "./GLSkill_SubData.h"
#include "./GLSkill.h"

#include "../Character/GLCharDefine.h"

namespace SKILL
{
	struct SSTATE_BLOW
	{
		float			fRATE;		//	발생 확율.
		float			fVAR1;		//	적용 값 1.
		float			fVAR2;		//	적용 값 2.

		SSTATE_BLOW ();
	};

	struct SIMPACT_ADDON_100
	{
		//	부가 효과 설정.
		EMIMPACT_ADDON		emADDON;					//	부가효과 종류.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	변화 수치.	

		SIMPACT_ADDON_100();
	};

	struct SIMPACT_ADDON_101
	{
		//	부가 효과 설정.
		EMIMPACT_ADDON		emADDON;					//	부가효과 종류.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	변화 수치.	
		float				fRate[MAX_LEVEL_BEFORE]; // 몬스터 효과 비율. by luxes.

		SIMPACT_ADDON_101();
	};


	struct SIMPACT_ADDON
	{
		//	부가 효과 설정.
		EMIMPACT_ADDON		emADDON;					//	부가효과 종류.
		float				fADDON_VAR[MAX_LEVEL];		//	변화 수치.	
		float				fRate[MAX_LEVEL]; // 몬스터 효과 비율. by luxes.

		SIMPACT_ADDON(const EMIMPACT_ADDON eAddon);
		SIMPACT_ADDON();
		void Assign( SIMPACT_ADDON_100& sOldData );
		void Assign( SIMPACT_ADDON_101& sOldData );
		bool operator()(SIMPACT_ADDON& key)		const;
	};

	struct SSPEC_ADDON_100
	{
		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC_101	sSPEC[MAX_LEVEL_BEFORE];			//	특수 기능 설정값.

		SSPEC_ADDON_100()
			: emSPEC ( EMSPECA_NULL )
		{
		}
	};

	struct SSPEC_ADDON_101
	{
		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC_102	sSPEC[MAX_LEVEL_BEFORE];			//	특수 기능 설정값.

		SSPEC_ADDON_101()
			: emSPEC ( EMSPECA_NULL )
		{
		}
	};

	struct SSPEC_ADDON_102
	{
		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC		sSPEC[MAX_LEVEL_BEFORE];			//	특수 기능 설정값.
		
		SSPEC_ADDON_102()
			: emSPEC ( EMSPECA_NULL )
		{
		}
	};

	struct SSPEC_ADDON
	{
		//	특수기능 설정.
		EMSPEC_ADDON		emSPEC;						//	특수.
		SKILL::SSPEC		sSPEC[MAX_LEVEL];			//	특수 기능 설정값.

		SSPEC_ADDON();
		SSPEC_ADDON(const EMSPEC_ADDON eAddon);
		void Assign( SSPEC_ADDON_100& sOldData );
		void Assign( SSPEC_ADDON_101& sOldData );
		void Assign( SSPEC_ADDON_102& sOldData );
		bool operator()(SSPEC_ADDON& key) const;
	};

	struct CDATA_LVL_100
	{
		float			fDELAYTIME;			//	딜래이 타임.

		float			fLIFE;				//	지속시간.
		WORD			wAPPLYRANGE;		//	적용 영역.
		WORD			wAPPLYNUM;			//	적용 갯수.
		WORD			wAPPLYANGLE;		//	적용 각도.
		WORD			wPIERCENUM;			//	관통 정도.

		float			fBASIC_VAR;			//	변화값.

		//	사용시 소모-값들. ( 사용조건 )
		WORD			wUSE_ARROWNUM;		//	소모 화살 갯수.

		WORD			wUSE_HP;			//	사용시 HP 소진량.
		WORD			wUSE_MP;			//	사용시 MP 소진량.
		WORD			wUSE_SP;			//	사용시 SP 소진량.

		WORD			wUSE_HP_PTY;		//	사용시 HP 파티원 소진량.
		WORD			wUSE_MP_PTY;		//	사용시 MP 파티원 소진량.
		WORD			wUSE_SP_PTY;		//	사용시 SP 파티원 소진량.
	};

	struct CDATA_LVL_101
	{
		float			fDELAYTIME;			//	딜래이 타임.

		float			fLIFE;				//	지속시간.
		WORD			wAPPLYRANGE;		//	적용 영역.
		WORD			wAPPLYNUM;			//	적용 갯수.
		WORD			wAPPLYANGLE;		//	적용 각도.
		WORD			wPIERCENUM;			//	관통 정도.
		WORD			wTARNUM;			//	타겟 갯수.

		float			fBASIC_VAR;			//	변화값.

		//	사용시 소모-값들. ( 사용조건 )
		WORD			wUSE_ARROWNUM;		//	소모 화살 갯수.
		WORD			wUSE_CHARMNUM;		//	소모 부적 갯수.

		WORD			wUSE_EXP;			//	사용시 EXP 소진량.
		WORD			wUSE_HP;			//	사용시 HP 소진량.
		WORD			wUSE_MP;			//	사용시 MP 소진량.
		WORD			wUSE_SP;			//	사용시 SP 소진량.

		WORD			wUSE_HP_PTY;		//	사용시 HP 파티원 소진량.
		WORD			wUSE_MP_PTY;		//	사용시 MP 파티원 소진량.
		WORD			wUSE_SP_PTY;		//	사용시 SP 파티원 소진량.
	};

	struct CDATA_LVL_102	// CP 추가
	{
		float			fDELAYTIME;			//	딜래이 타임.

		float			fLIFE;				//	지속시간.
		WORD			wAPPLYRANGE;		//	적용 영역.
		WORD			wAPPLYNUM;			//	적용 갯수.
		WORD			wAPPLYANGLE;		//	적용 각도.
		WORD			wPIERCENUM;			//	관통 정도.
		WORD			wTARNUM;			//	타겟 갯수.

		float			fBASIC_VAR;			//	변화값.

		//	사용시 소모-값들. ( 사용조건 )
		WORD			wUSE_ARROWNUM;		//	소모 화살 갯수.
		WORD			wUSE_CHARMNUM;		//	소모 부적 갯수.

		WORD			wUSE_EXP;			//	사용시 EXP 소진량.
		WORD			wUSE_HP;			//	사용시 HP 소진량.
		WORD			wUSE_MP;			//	사용시 MP 소진량.
		WORD			wUSE_SP;			//	사용시 SP 소진량.
		WORD			wUSE_CP;			//	사용시 CP 소진량.

		WORD			wUSE_HP_PTY;		//	사용시 HP 파티원 소진량.
		WORD			wUSE_MP_PTY;		//	사용시 MP 파티원 소진량.
		WORD			wUSE_SP_PTY;		//	사용시 SP 파티원 소진량.
	};

	struct CDATA_LVL_103	//	중성탄 추가
	{
		float			fDELAYTIME;			//	딜래이 타임.

		float			fLIFE;				//	지속시간.
		WORD			wAPPLYRANGE;		//	적용 영역.
		WORD			wAPPLYNUM;			//	적용 갯수.
		WORD			wAPPLYANGLE;		//	적용 각도.
		WORD			wPIERCENUM;			//	관통 정도.
		WORD			wTARNUM;			//	타겟 갯수.

		float			fBASIC_VAR;			//	변화값.

		//	사용시 소모-값들. ( 사용조건 )
		WORD			wUSE_ARROWNUM;		//	소모 화살 갯수.
		WORD			wUSE_CHARMNUM;		//	소모 부적 갯수.
		WORD			wUSE_BULLETNUM;		//	소모 부적 갯수.

		WORD			wUSE_EXP;			//	사용시 EXP 소진량.
		WORD			wUSE_HP;			//	사용시 HP 소진량.
		WORD			wUSE_MP;			//	사용시 MP 소진량.
		WORD			wUSE_SP;			//	사용시 SP 소진량.
		WORD			wUSE_CP;			//	사용시 CP 소진량.

		WORD			wUSE_HP_PTY;		//	사용시 HP 파티원 소진량.
		WORD			wUSE_MP_PTY;		//	사용시 MP 파티원 소진량.
		WORD			wUSE_SP_PTY;		//	사용시 SP 파티원 소진량.
	};

	//	적용 값 래밸별 설정.
	struct CDATA_LVL	//	배터리 추가
	{
		float			fDELAYTIME;			//	딜래이 타임.

		float			fLIFE;				//	지속시간.
		WORD			wAPPLYRANGE;		//	적용 영역.
		WORD			wAPPLYNUM;			//	적용 갯수. // 미사용;
		WORD			wAPPLYANGLE;		//	적용 각도.
		WORD			wPIERCENUM;			//	관통 정도.
		WORD			wTARNUM;			//	타겟 갯수.

		float			fBASIC_VAR;			//	변화값.

		//	사용시 소모-값들. ( 사용조건 )
		WORD			wUSE_ARROWNUM;		//	소모 화살 갯수.
		WORD			wUSE_CHARMNUM;		//	소모 부적 갯수.
		WORD			wUSE_BULLETNUM;		//	소모 부적 갯수.

		WORD			wUSE_EXP;			//	사용시 EXP 소진량.
		WORD			wUSE_HP;			//	사용시 HP 소진량.
		WORD			wUSE_MP;			//	사용시 MP 소진량.
		WORD			wUSE_SP;			//	사용시 SP 소진량.
		WORD			wUSE_CP;			//	사용시 CP 소진량.
		INT				nUSE_BATTERY;		//	사용시 배터리 소진량.

		WORD			wUSE_HP_PTY;		//	사용시 HP 파티원 소진량.
		WORD			wUSE_MP_PTY;		//	사용시 MP 파티원 소진량.
		WORD			wUSE_SP_PTY;		//	사용시 SP 파티원 소진량.

		void Assign ( CDATA_LVL_100 &sOldData );
		void Assign ( CDATA_LVL_101 &sOldData );
		void Assign ( CDATA_LVL_102 &sOldData );
		void Assign ( CDATA_LVL_103 &sOldData );

		CDATA_LVL (void);
	};

	struct SSPEC_CA
	{
		// SSPEC내부로 들어가던가 데미지 반격, 스킬 효과 반격, 이렇게 둘이 스킬 효과로 빠지는게 맞는것같다;
		// 아래 둘은 플래그로 포함되고 배열만 따로 존재하던가 NID와 겹치던가로;
		bool bDamageApply;
		bool bAllApply;

		EMSPEC_ADDON		emSPEC[MAX_ADDON];

		SSPEC_CA();

		void resetemSpec();
		void AllReset();
	};

	struct SSPEC_CA_ADDON_100
	{
		SKILL::SSPEC_CA   sSPEC_CA[MAX_LEVEL_BEFORE];
	};

	struct SSPEC_CA_ADDON
	{
		SKILL::SSPEC_CA   sSPEC_CA[MAX_LEVEL];

		void Assign ( SSPEC_CA_ADDON_100 &sOldData );

		void resetemSpec();
		void AllReset();
	};
};