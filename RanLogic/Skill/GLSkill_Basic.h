#pragma once

#include "./GLSkillDef.h"
#include "../Character/GLCharDefine.h"

namespace SKILL
{
	struct SBASIC_100
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.
		DWORD				dwGRADE;					//	등급.

		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )

		//	Skill 의 영향 받는 목표.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )

		WORD				wTARRANGE;					//	목표 가능 거리.

		glold::GLITEM_ATT_102	emUSE_LITEM;			//	사용가능 왼손 도구 속성.
		glold::GLITEM_ATT_102	emUSE_RITEM;			//	사용가능 오른손 도구 속성.
	};

	struct SBASIC_101
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.
		DWORD				dwGRADE;					//	등급.

		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )

		//	Skill 의 영향 받는 목표.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )

		WORD				wTARRANGE;					//	목표 가능 거리.

		glold::GLITEM_ATT_102	emUSE_LITEM;			//	사용가능 왼손 도구 속성.
		glold::GLITEM_ATT_102	emUSE_RITEM;			//	사용가능 오른손 도구 속성.
	};

	struct SBASIC_102
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.
		DWORD				dwGRADE;					//	등급.

		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )

		//	Skill 의 영향 받는 목표.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )

		WORD				wTARRANGE;					//	목표 가능 거리.

		glold_103::GLITEM_ATT_103	emUSE_LITEM;			//	사용가능 왼손 도구 속성.
		glold_103::GLITEM_ATT_103	emUSE_RITEM;			//	사용가능 오른손 도구 속성.
	};

	struct SBASIC_103
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.
		DWORD				dwGRADE;					//	등급.

		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )

		//	Skill 의 영향 받는 목표.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )

		WORD				wTARRANGE;					//	목표 가능 거리.

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	사용가능 오른손 도구 속성.
	};

	struct SBASIC_105
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.
		DWORD				dwGRADE;					//	등급.
		DWORD				dwMAXLEVEL;					//	최대 레벨.

		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )

		//	Skill 의 영향 받는 목표.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )

		WORD				wTARRANGE;					//	목표 가능 거리.

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	사용가능 오른손 도구 속성.
	};

	struct SBASIC_106	//	데이터 포맷 변경 진행
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.
		DWORD				dwGRADE;					//	등급.
		DWORD				dwMAXLEVEL;					//	최대 레벨.

		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )

		//	Skill 의 영향 받는 목표.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )

		WORD				wTARRANGE;					//	목표 가능 거리.

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	사용가능 오른손 도구 속성.

		bool				bLearnView;					//  GM만 보이는 스킬
	};


	struct SBASIC_107	//	스킬타입 부여
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )
		bool				bLearnView;					//  GM만 보이는 스킬

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.

		DWORD				dwMAXLEVEL;					//	최대 레벨.
		DWORD				dwGRADE;					//	등급.

		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )
		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )

		WORD				wTARRANGE;					//	목표 가능 거리.

		//	Skill 의 영향 받는 목표.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	사용가능 오른손 도구 속성.
	};

	struct SBASIC_108	//	무기타입 변경
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )
		bool				bLearnView;					//  GM만 보이는 스킬
		bool				bNonEffectRemove;				//	효과제거 불가 옵션

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.

		DWORD				dwMAXLEVEL;					//	최대 레벨.
		DWORD				dwGRADE;					//	등급.

		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )
		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMACTION_TYPE		emACTION_TYPE;				//	스킬 행동타입 ( 일반,버프,디버프,리미트)

		WORD				wTARRANGE;					//	목표 가능 거리.

		//	Skill 의 영향 받는 목표.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	사용가능 오른손 도구 속성.
	};

	struct SBASIC_109
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )
		bool				bLearnView;					//  GM만 보이는 스킬
		bool				bNonEffectRemove;			//	효과제거 불가 옵션

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.

		DWORD				dwMAXLEVEL;					//	최대 레벨.
		DWORD				dwGRADE;					//	등급.

		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )
		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMACTION_TYPE		emACTION_TYPE;				//	스킬 행동타입 ( 일반,버프,디버프,리미트)

		WORD				wTARRANGE;					//	목표 가능 거리.

		//	Skill 의 영향 받는 목표.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&

		glold_105::GLSKILL_ATT_105		emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		glold_105::GLSKILL_ATT_105		emUSE_RITEM;				//	사용가능 오른손 도구 속성.

		SBASIC_109 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_105::SKILLATT_NOCARE),
			emUSE_RITEM(glold_105::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC_110
	{
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )
		bool				bLearnView;					//  GM만 보이는 스킬
		bool				bNonEffectRemove;			//	효과제거 불가 옵션
		bool				bMobEffectRate;				//	몹 효과 비율 적용 옵션. by luxes.

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.

		DWORD				dwMAXLEVEL;					//	최대 레벨.
		DWORD				dwGRADE;					//	등급.

		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )
		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMACTION_TYPE		emACTION_TYPE;				//	스킬 행동타입 ( 일반,버프,디버프,리미트)

		WORD				wTARRANGE;					//	목표 가능 거리.

		//	Skill 의 영향 받는 목표.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&

		glold_105::GLSKILL_ATT_105		emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		glold_105::GLSKILL_ATT_105		emUSE_RITEM;				//	사용가능 오른손 도구 속성.

		SBASIC_110 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_105::SKILLATT_NOCARE),
			emUSE_RITEM(glold_105::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};


#define FLAG_SB_RUNNING_CAST                    0x00000001  // 이동하면서 스킬시전 가능여부;
#define FLAG_SB_EFFECT_FORCE_VISIBLE            0x00010000  // 버프 미표시 옵션시에도 강제로 렌더링;

	struct SBASIC_111   // 해머, 듀얼스피어, 사이드 무기 종류 추가에 따른 데이터 변화.
	{
		enum { VERSION = 0x0111 };
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )
		bool				bLearnView;					//  GM만 보이는 스킬
		bool				bNonEffectRemove;			//	효과제거 불가 옵션
		bool				bMobEffectRate;				//	몹 효과 비율 적용 옵션. by luxes.

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.

		DWORD				dwMAXLEVEL;					//	최대 레벨.
		DWORD				dwGRADE;					//	등급.

		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )
		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMACTION_TYPE		emACTION_TYPE;				//	스킬 행동타입 ( 일반,버프,디버프,리미트)

		WORD				wTARRANGE;					//	목표 가능 거리.

		//	Skill 의 영향 받는 목표.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&

		GLSKILL_ATT			emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		GLSKILL_ATT			emUSE_RITEM;				//	사용가능 오른손 도구 속성.


		SBASIC_111 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(SKILLATT_NOCARE),
			emUSE_RITEM(SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}	
	};

	struct SBASIC_112   // 해머, 듀얼스피어, 사이드 무기 종류 추가에 따른 데이터 변화.
	{
		enum { VERSION = 0x0112 };
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )
		bool				bLearnView;					//  GM만 보이는 스킬
		bool				bNonEffectRemove;			//	효과제거 불가 옵션
		bool				bMobEffectRate;				//	몹 효과 비율 적용 옵션. by luxes.

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.

		DWORD				dwMAXLEVEL;					//	최대 레벨.
		DWORD				dwGRADE;					//	등급.

		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )
		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMACTION_TYPE		emACTION_TYPE;				//	스킬 행동타입 ( 일반,버프,디버프,리미트)

		WORD				wTARRANGE;					//	목표 가능 거리.

		//	Skill 의 영향 받는 목표.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&

		glold_106::GLSKILL_ATT_106			emUSE_LITEM;	//	사용가능 왼손 도구 속성.
		glold_106::GLSKILL_ATT_106			emUSE_RITEM;	//	사용가능 오른손 도구 속성.

		DWORD				dwFlags;					//

		SBASIC_112 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_106::SKILLATT_NOCARE),
			emUSE_RITEM(glold_106::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false),
			dwFlags(0)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC_113  // 극강글러브 종류 추가에 따른 데이터 변화.
	{
		enum { VERSION = 0x0113 };
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )
		bool				bLearnView;					//  GM만 보이는 스킬
		bool				bNonEffectRemove;			//	효과제거 불가 옵션
		bool				bMobEffectRate;				//	몹 효과 비율 적용 옵션. by luxes.

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.

		DWORD				dwMAXLEVEL;					//	최대 레벨.
		DWORD				dwGRADE;					//	등급.

		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )
		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMACTION_TYPE		emACTION_TYPE;				//	스킬 행동타입 ( 일반,버프,디버프,리미트)

		WORD				wTARRANGE;					//	목표 가능 거리.

		//	Skill 의 영향 받는 목표.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&

		glold_107::GLSKILL_ATT_107		emUSE_LITEM;		//	사용가능 왼손 도구 속성.
		glold_107::GLSKILL_ATT_107		emUSE_RITEM;	//	사용가능 오른손 도구 속성.		

		DWORD				dwFlags;					//

		SBASIC_113 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_107::SKILLATT_NOCARE),
			emUSE_RITEM(glold_107::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false),
			dwFlags(0)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC_114  // 마술부 무기 추가에 따른 데이터 변화;
	{
		enum { VERSION = 0x0114 };
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )
		bool				bLearnView;					//  GM만 보이는 스킬
		bool				bNonEffectRemove;			//	효과제거 불가 옵션
		bool				bMobEffectRate;				//	몹 효과 비율 적용 옵션. by luxes.

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.

		DWORD				dwMAXLEVEL;					//	최대 레벨.
		DWORD				dwGRADE;					//	등급.

		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )
		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMACTION_TYPE		emACTION_TYPE;				//	스킬 행동타입 ( 일반,버프,디버프,리미트)

		WORD				wTARRANGE;					//	목표 가능 거리.

		//	Skill 의 영향 받는 목표.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&

		glold_108::GLSKILL_ATT_108 	emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		glold_108::GLSKILL_ATT_108	emUSE_RITEM;				//	사용가능 오른손 도구 속성.

		DWORD				dwFlags;					//

		SBASIC_114 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_108::SKILLATT_NOCARE),
			emUSE_RITEM(glold_108::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false),
			dwFlags(0)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC_115  // 마술부 무기 추가에 따른 데이터 변화;
	{
		enum { VERSION = 0x0115 };
		SNATIVEID			sNATIVEID;					//	고유 ID. ( 최초 생성시에 부여되는 고유 ID )
		bool				bLearnView;					//  GM만 보이는 스킬
		bool				bNonEffectRemove;			//	효과제거 불가 옵션
		bool				bMobEffectRate;				//	몹 효과 비율 적용 옵션. by luxes.

		char				szNAME[MAX_SZNAME];			//	스킬의 이름.

		DWORD				dwMAXLEVEL;					//	최대 레벨.
		DWORD				dwGRADE;					//	등급.

		EMAPPLY				emAPPLY;					//	스킬 유형. ( 물리공격, 마법공격, 마법 보조 )
		EMROLE				emROLE;						//	스킬 동작 특성. ( 즉시, 지속, 페시브 )
		EMACTION_TYPE		emACTION_TYPE;				//	스킬 행동타입 ( 일반,버프,디버프,리미트)

		WORD				wTARRANGE;					//	목표 가능 거리.

		//	Skill 의 영향 받는 목표.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	타겟. ( 자신, 대상, 위치 ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&

		GLSKILL_ATT			emUSE_LITEM;				//	사용가능 왼손 도구 속성.
		GLSKILL_ATT			emUSE_RITEM;				//	사용가능 오른손 도구 속성.

		DWORD				dwFlags;					//

		SBASIC_115 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(SKILLATT_NOCARE),
			emUSE_RITEM(SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false),
			dwFlags(0)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC : public SBASIC_115
	{
		// 이곳에다 변수 선언 하지 말 것;
		// 파일 저장 및 구버전 파일 불러오기 등 문제 생김;
		SBASIC(){}

		void Assign( SBASIC_100& sOldData );
		void Assign( SBASIC_101& sOldData );
		void Assign( SBASIC_102& sOldData );
		void Assign( SBASIC_103& sOldData );
		void Assign( SBASIC_105& sOldData );
		void Assign( SBASIC_106& sOldData );
		void Assign( SBASIC_107& sOldData );
		void Assign( SBASIC_108& sOldData );
		void Assign( SBASIC_109& sOldData );
		void Assign( SBASIC_110& sOldData );
		void Assign( SBASIC_111& sOldData );
		void Assign( SBASIC_112& sOldData );
		void Assign( SBASIC_113& sOldData );
		void Assign( SBASIC_114& sOldData );

		//! 고유 ID. (최초 생성시에 부여되는 고유 ID)
		inline SNATIVEID GetId() const { return sNATIVEID; }

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		//! "편" (그 주위의 자기편, 그 주위의 상대편)
		inline EMIMPACT_SIDE ImpactSide() const { return emIMPACT_SIDE; }
	}; //struct SBASIC
};