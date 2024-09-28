#ifndef _CROW_BASIC_H_
#define _CROW_BASIC_H_

#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../Pet/GLPetDefine.h"
#include "../Summon/GLSummonDefine.h"
#include "../Crow/GLCrowDefine.h"
#include "../Character/GLCharDefine.h"

#define MOBNPCEDIT_VERSION 0x0122

struct SCROWBASIC_100
{
    SNATIVEID			sNativeID;					//	고유 ID.
    char				m_szName[CHAR_SZNAME];		//	이름.
    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    WORD				m_wLevel;					//	레벨.

    WORD				m_wHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST_101			m_sResist;					//	저항값.
};

struct SCROWBASIC_101
{
    SNATIVEID			sNativeID;					//	고유 ID.
    char				m_szName[CHAR_SZNAME];		//	이름.
    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    WORD				m_wLevel;					//	레벨.

    WORD				m_wHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST				m_sResist;					//	저항값.
};

struct SCROWBASIC_102
{
    SNATIVEID			sNativeID;					//	고유 ID.
    char				m_szName[CHAR_SZNAME];		//	이름.
    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    WORD				m_wLevel;					//	레벨.

    WORD				m_wHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST				m_sResist;					//	저항값.

    WORD				m_wBonusExp;				//	추가 경험치.
};

struct SCROWBASIC_103
{
    SNATIVEID			sNativeID;					//	고유 ID.

    char				m_szName[CHAR_SZNAME];		//	이름.

    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST				m_sResist;					//	저항값.

    WORD				m_wBonusExp;				//	추가 경험치.
};

struct SCROWBASIC_104
{
    SNATIVEID			sNativeID;					//	고유 ID.

    char				m_szName[CHAR_SZNAME];		//	이름.

    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST				m_sResist;					//	저항값.

    WORD				m_wBonusExp;				//	추가 경험치.
};

struct SCROWBASIC_105
{
    SNATIVEID			sNativeID;					//	고유 ID.

    char				m_szName[CHAR_SZNAME];		//	이름.

    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST				m_sResist;					//	저항값.

    WORD				m_wBonusExp;				//	추가 경험치.

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입
};

struct SCROWBASIC_106
{
    SNATIVEID			sNativeID;					//	고유 ID.

    char				m_szName[CHAR_SZNAME];		//	이름.

    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST				m_sResist;					//	저항값.

    WORD				m_wBonusExp;				//	추가 경험치.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입
};

struct SCROWBASIC_108
{
    SNATIVEID			sNativeID;					//	고유 ID.

    char				m_szName[CHAR_SZNAME];		//	이름.

    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST				m_sResist;					//	저항값.

    DWORD				m_wBonusExp;				//	추가 경험치.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입
};

struct SCROWBASIC_109
{
    SNATIVEID			sNativeID;					//	고유 ID.

    char				m_szName[CHAR_SZNAME];		//	이름.

    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST				m_sResist;					//	저항값.

    DWORD				m_wBonusExp;				//	추가 경험치.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입

    DWORD				m_dwGenTime;				// 호출 시간
};

struct SCROWBASIC_110	//	데이터 포맷 변경 진행
{
    SNATIVEID			sNativeID;					//	고유 ID.

    char				m_szName[CHAR_SZNAME];		//	이름.

    EMTRIBE				m_emTribe;					//	종족.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wDefense;					//	방어력.

    SRESIST				m_sResist;					//	저항값.

    DWORD				m_wBonusExp;				//	추가 경험치.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)

    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션
};

struct SCROWBASIC_111 // 불리한 효과 방지 추가
{
    SNATIVEID			sNativeID;					//	고유 ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	종족.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    char				m_szName[CHAR_SZNAME];		//	이름.

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wDefense;					//	방어력.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    DWORD				m_wBonusExp;				//	추가 경험치.

    SRESIST				m_sResist;					//	저항값.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)
    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입
};

struct SCROWBASIC_112	// 추가 CP 추가
{

    SNATIVEID			sNativeID;					//	고유 ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	종족.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    char				m_szName[CHAR_SZNAME];		//	이름.

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wDefense;					//	방어력.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    DWORD				m_wBonusExp;				//	추가 경험치.

    SRESIST				m_sResist;					//	저항값.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)
    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션

    bool				m_bNoPenalty;				// 불리한 효과 방지

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입

};

struct SCROWBASIC_113 // 추가 공격 안받음 여부 추가
{

    SNATIVEID			sNativeID;					//	고유 ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	종족.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    char				m_szName[CHAR_SZNAME];		//	이름.

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wDefense;					//	방어력.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    DWORD				m_wBonusExp;				//	추가 경험치.
    WORD				m_wBonusCP;					//	추가 CP.

    SRESIST				m_sResist;					//	저항값.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)
    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션

    bool				m_bNoPenalty;				// 불리한 효과 방지

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입
};

struct SCROWBASIC_114 // 소환수 타입 추가, 타입별 불리한 효과 방지
{
    SNATIVEID			sNativeID;					//	고유 ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	종족.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    char				m_szName[CHAR_SZNAME];		//	이름.

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wDefense;					//	방어력.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    DWORD				m_wBonusExp;				//	추가 경험치.
    WORD				m_wBonusCP;					//	추가 CP.

    SRESIST				m_sResist;					//	저항값.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    bool                m_bNonDamaged;              //  공격 안받음 여부

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)
    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션

    bool				m_bNoPenalty;				// 불리한 효과 방지

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입
};

struct SCROWBASIC_115 // NPC 타입 추가
{
    SNATIVEID			sNativeID;					//	고유 ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	종족.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    char				m_szName[CHAR_SZNAME];		//	이름.

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wDefense;					//	방어력.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    DWORD				m_wBonusExp;				//	추가 경험치.
    WORD				m_wBonusCP;					//	추가 CP.

    SRESIST				m_sResist;					//	저항값.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    bool                m_bNonDamaged;              //  공격 안받음 여부

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)
    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션

    bool				m_bNoPenalty;				// 불리한 효과 방지
    DWORD               m_dwNoPenaltyFlag;			// 타입별 불리한 효과 방지

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입

    // 소환수 타입 추가
    SUMMON_TYPE         m_emSummonType;             // 소환수 타입
};

struct SCROWBASIC_116
{
    SNATIVEID			sNativeID;					//	고유 ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	종족.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    char				m_szName[CHAR_SZNAME];		//	이름.

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wDefense;					//	방어력.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    DWORD				m_wBonusExp;				//	추가 경험치.
    WORD				m_wBonusCP;					//	추가 CP.

    SRESIST				m_sResist;					//	저항값.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    bool                m_bNonDamaged;              //  공격 안받음 여부

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)
    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션

    bool				m_bNoAllPenalty;			// 모든 불리한 효과 방지
    DWORD               m_dwNoPenaltyFlag;			// 타입별 불리한 효과 방지

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입

    // 소환수 타입 추가
    SUMMON_TYPE         m_emSummonType;             // 소환수 타입

    // NPC 타입 추가
    NPC_TYPE			m_emNPCType;                // NPC 타입
};

struct SCROWBASIC_117
{
    SNATIVEID			sNativeID;					//	고유 ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	종족.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    char				m_szName[CHAR_SZNAME];		//	이름.

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wDefense;					//	방어력.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    DWORD				m_wBonusExp;				//	추가 경험치.
    WORD				m_wBonusCP;					//	추가 CP.

    SRESIST				m_sResist;					//	저항값.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    bool                m_bNonDamaged;              //  공격 안받음 여부

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)
    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션

    bool				m_bNoAllPenalty;			// 모든 불리한 효과 방지
    DWORD               m_dwNoPenaltyFlag;			// 타입별 불리한 효과 방지

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입

    // 소환수 타입 추가
    SUMMON_TYPE         m_emSummonType;             // 소환수 타입

    // NPC 타입 추가
    NPC_TYPE			m_emNPCType;                // NPC 타입

    bool                m_bHideNaming;              // 이름/HP 표시하지 않음
    bool                m_bNonTargeting;            // 타켓팅 하지 못하게 함
};

struct SCROWBASIC_118
{
	SNATIVEID			sNativeID;					//	고유 ID.

	EMCROW				m_emCrow;					//	Npc, Mob.
	EMTRIBE				m_emTribe;					//	종족.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	char				m_szName[CHAR_SZNAME];		//	이름.

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wDefense;					//	방어력.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	DWORD				m_wBonusExp;				//	추가 경험치.
	WORD				m_wBonusCP;					//	추가 CP.

	SRESIST				m_sResist;					//	저항값.

	bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

	bool                m_bNonDamaged;              //  공격 안받음 여부

	DWORD				m_dwGenTime;				// 호출 시간

	WORD				m_wGatherTimeLow;			// 채집시간(Low)
	WORD				m_wGatherTimeHigh;			// 채집시간(High)
	float				m_fGatherRate;				// 채집확률
	WORD				m_wGatherAnimation;			// 애니메이션

	bool				m_bNoAllPenalty;			// 모든 불리한 효과 방지
	DWORD               m_dwNoPenaltyFlag;			// 타입별 불리한 효과 방지

	// 팻타입 추가
	int				    m_emPetType;				// 팻타입

	// 소환수 타입 추가
	SUMMON_TYPE         m_emSummonType;             // 소환수 타입

	// NPC 타입 추가
	NPC_TYPE			m_emNPCType;                // NPC 타입

	// 0x0117 추가
	bool                m_bHideNaming;              // 이름/HP 표시하지 않음
	bool                m_bNonTargeting;            // 타켓팅 하지 못하게 함

	// 0x0118 추가
	float               m_fScale;                   // 스케일값
};

struct SCROWBASIC_119
{
	SNATIVEID			sNativeID;					//	고유 ID.

	EMCROW				m_emCrow;					//	Npc, Mob.
	EMTRIBE				m_emTribe;					//	종족.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	char				m_szName[CHAR_SZNAME];		//	이름.

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wDefense;					//	방어력.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	DWORD				m_wBonusExp;				//	추가 경험치.
	WORD				m_wBonusCP;					//	추가 CP.

	SRESIST				m_sResist;					//	저항값.

	bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

	bool                m_bNonDamaged;              //  공격 안받음 여부

	DWORD				m_dwGenTime;				// 호출 시간

	WORD				m_wGatherTimeLow;			// 채집시간(Low)
	WORD				m_wGatherTimeHigh;			// 채집시간(High)
	float				m_fGatherRate;				// 채집확률
	WORD				m_wGatherAnimation;			// 애니메이션

	bool				m_bNoAllPenalty;			// 모든 불리한 효과 방지
	DWORD               m_dwNoPenaltyFlag;			// 타입별 불리한 효과 방지

	// 팻타입 추가
	int				    m_emPetType;				// 팻타입

	// 소환수 타입 추가
	SUMMON_TYPE         m_emSummonType;             // 소환수 타입

	// NPC 타입 추가
	NPC_TYPE			m_emNPCType;                // NPC 타입

	// 0x0117 추가
	bool                m_bHideNaming;              // 이름/HP 표시하지 않음
	bool                m_bNonTargeting;            // 타켓팅 하지 못하게 함

	// 0x0118 추가
	float               m_fScale;                   // 스케일값
	// 0x0119 추가
	bool				m_bHideTooltip;				// Mob/NPC 툴팁 숨김;
};

struct SCROWBASIC_120
{
	SNATIVEID			sNativeID;					//	고유 ID.

	EMCROW				m_emCrow;					//	Npc, Mob.
	EMTRIBE				m_emTribe;					//	종족.
	EMBRIGHT			m_emBright;					//	속성 (광/암)

	char				m_szName[CHAR_SZNAME];		//	이름.

	WORD				m_wLevel;					//	레벨.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	회복율.
	float				m_fIncHP;					//	HP 회복율.
	float				m_fIncMP;					//	MP 회복율.
	float				m_fIncSP;					//	SP 회복율.

	WORD				m_wDefense;					//	방어력.

	WORD				m_wViewRange;				//	시야 거리.
	WORD				m_wAvoidRate;				//	회피율.
	WORD				m_wHitRate;					//	명중율.

	DWORD				m_wBonusExp;				//	추가 경험치.
	WORD				m_wBonusCP;					//	추가 CP.

	SRESIST				m_sResist;					//	저항값.

	bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

	bool                m_bNonDamaged;              //  공격 안받음 여부

	DWORD				m_dwGenTime;				// 호출 시간

	WORD				m_wGatherTimeLow;			// 채집시간(Low)
	WORD				m_wGatherTimeHigh;			// 채집시간(High)
	float				m_fGatherRate;				// 채집확률
	WORD				m_wGatherAnimation;			// 애니메이션

	bool				m_bNoAllPenalty;			// 모든 불리한 효과 방지
	DWORD               m_dwNoPenaltyFlag;			// 타입별 불리한 효과 방지

	// 팻타입 추가
	int				    m_emPetType;				// 팻타입

	// 소환수 타입 추가
	SUMMON_TYPE         m_emSummonType;             // 소환수 타입

	// NPC 타입 추가
	NPC_TYPE			m_emNPCType;                // NPC 타입

	// 0x0117 추가
	bool                m_bHideNaming;              // 이름/HP 표시하지 않음
	bool                m_bNonTargeting;            // 타켓팅 하지 못하게 함

	// 0x0118 추가
	float               m_fScale;                   // 스케일값

	// 0x0119 추가
	bool				m_bHideTooltip;				// Mob/NPC 툴팁 숨김;
	// 0x0200 추가;
	WORD				m_wResist_Probability;		// 저항 확률;

};

struct SCROWBASIC_121
{
    SNATIVEID			sNativeID;					//	고유 ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	종족.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    char				m_szName[CHAR_SZNAME];		//	이름.

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wDefense;					//	방어력.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    DWORD				m_wBonusExp;				//	추가 경험치.
    WORD				m_wBonusCP;					//	추가 CP.

    SRESIST				m_sResist;					//	저항값.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    bool                m_bNonDamaged;              //  공격 안받음 여부

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)
    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션

    bool				m_bNoAllPenalty;			// 모든 불리한 효과 방지
    DWORD               m_dwNoPenaltyFlag;			// 타입별 불리한 효과 방지

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입

    // 소환수 타입 추가
    SUMMON_TYPE         m_emSummonType;             // 소환수 타입

    // NPC 타입 추가
    NPC_TYPE			m_emNPCType;                // NPC 타입

    // 0x0117 추가
    bool                m_bHideNaming;              // 이름/HP 표시하지 않음
    bool                m_bNonTargeting;            // 타켓팅 하지 못하게 함

    // 0x0118 추가
    float               m_fScale;                   // 스케일값

    // 0x0119 추가
    bool				m_bHideTooltip;				// Mob/NPC 툴팁 숨김;

    // 0x0200 추가;
    WORD				m_wResist_Probability;		// 저항 확률;

    // 0x0121 추가;
	float				m_fBarrierRadius;			// 장애물 반경;
	float				m_fBarrierWidth;			// 장애물 두께;
};
/**
    최신 버전의 데이터를 의미 한다.
    최신 버전이 0x0117 이라면 SCROWBASIC 는 0x0117 를 의미 한다.

    구버전들의 로딩을 위해 위에 버전별로 구조체가 정의되어 있다.

    그런데 새롭게 변수가 추가되면 현재버전을 구버전으로 만들어 줘야 하므로
    추가한 사항을 정확히 알기 위해 버전 추가시 추가한 변수들 위에 버전을 명시 해주도록 한다.
    될수 있으면 가장 아랫부분에 변수를 추가하도록 한다.
    0x0117 버전을 참조 한다.

 */
struct SCROWBASIC
{
    enum { VERSION = MOBNPCEDIT_VERSION, };

    SNATIVEID			sNativeID;					//	고유 ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	종족.
    EMBRIGHT			m_emBright;					//	속성 (광/암)

    char				m_szName[CHAR_SZNAME];		//	이름.

    WORD				m_wLevel;					//	레벨.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	회복율.
    float				m_fIncHP;					//	HP 회복율.
    float				m_fIncMP;					//	MP 회복율.
    float				m_fIncSP;					//	SP 회복율.

    WORD				m_wDefense;					//	방어력.

    WORD				m_wViewRange;				//	시야 거리.
    WORD				m_wAvoidRate;				//	회피율.
    WORD				m_wHitRate;					//	명중율.

    DWORD				m_wBonusExp;				//	추가 경험치.
    WORD				m_wBonusCP;					//	추가 CP.

    SRESIST				m_sResist;					//	저항값.

    bool				m_bOverlapAttack;			//  겹쳤을때 공격 가능 여부

    bool                m_bNonDamaged;              //  공격 안받음 여부

    DWORD				m_dwGenTime;				// 호출 시간

    WORD				m_wGatherTimeLow;			// 채집시간(Low)
    WORD				m_wGatherTimeHigh;			// 채집시간(High)
    float				m_fGatherRate;				// 채집확률
    WORD				m_wGatherAnimation;			// 애니메이션

    bool				m_bNoAllPenalty;			// 모든 불리한 효과 방지
    DWORD               m_dwNoPenaltyFlag;			// 타입별 불리한 효과 방지

    // 팻타입 추가
    int				    m_emPetType;				// 팻타입

    // 소환수 타입 추가
    SUMMON_TYPE         m_emSummonType;             // 소환수 타입

    // NPC 타입 추가
    NPC_TYPE			m_emNPCType;                // NPC 타입

    // 0x0117 추가
    bool                m_bHideNaming;              // 이름/HP 표시하지 않음
    bool                m_bNonTargeting;            // 타켓팅 하지 못하게 함

    // 0x0118 추가
    float               m_fScale;                   // 스케일값

	// 0x0119 추가
	bool				m_bHideTooltip;				// Mob/NPC 툴팁 숨김;

	// 0x0120 추가;
	WORD				m_wResist_Probability;		// 저항 확률;

	// 0x0121 추가;
	float				m_fBarrierRadius;			// 장애물 반경;
	float				m_fBarrierWidth;			// 장애물 두께;

    // 0x0121 추가;
    float               m_fRefenceActionableRange;  // 상호작용 반경;

    SCROWBASIC();
    SCROWBASIC& operator = ( const SCROWBASIC_100 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_101 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_102 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_103 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_104 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_105 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_106 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_108 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_109 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_110 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_111 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_112 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_113 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_114 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_115 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_116 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_117 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_118 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_119 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_120 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_121 &sCrowBasic );

    static VOID SaveCsvHead ( std::fstream &SFile );
    VOID SaveCsv(std::fstream& SFile);
    VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);
};

#endif // _CROW_BASIC_H_