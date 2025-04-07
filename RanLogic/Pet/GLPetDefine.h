#ifndef _GLPET_DEFINE_H_
#define _GLPET_DEFINE_H_

#pragma once

enum PETDEFINE
{
    PETNAMESIZE = 12,
    MAX_PETSKILL = 11,
    MAXPETREVINFO = 20,
    MAXRIGHTOFITEM = 30,
    MAX_PETSKILLSLOT = 2,
    MAX_PET_SATIATION = 1000,
};

enum EMPETACTIONTYPE
{
    PETAT_IDLE		= 0,
    PETAT_FUNNY1	= 1,
    PETAT_FUNNY2	= 2,
    PETAT_FUNNY3	= 3,
    PETAT_WALK		= 4,
    PETAT_RUN		= 5,
    PETAT_SKILL		= 6,
    PETAT_ATTACK	= 7,
    PETAT_SAD		= 8,

    PETAT_NONE		= 9,
};

enum EMPETACTSTATE
{
    EM_PETACT_MOVE			  = 0x00000001,		// 움직임
    EM_PETACT_WAIT			  = 0x00000002,		// 잠시대기
    EM_PETACT_STOP			  = 0x00000004,		// 완전정지
    EM_PETACT_RUN			  = 0x00000010,		// 뛰기모드
    EM_PETACT_RUN_CLOSELY	  = 0x00000020,		// 근접거리까지 뛰기모드
    EM_PETACT_RUN_PICKUP_ITEM = 0x00000040,		// 아이템을 줍기위해 뛰기모드
    EM_PETACT_FUNNY			  = 0x00000100,		// Funny Action모드
    EM_PETACT_ATTACK		  = 0x00000200,		// Attack모드
    EM_PETACT_SAD			  = 0x00000400,		// Sad모드
	EM_PETACT_QBOXPICK_WAIT	  = 0x00000800,		// QBox 줍고, 다시 줍기 위해 기다리는 상태
};

enum EMPETGETITEM_SKILL
{
    EMPETSKILL_GETALL		= 0x0001,
    EMPETSKILL_GETRARE		= 0x0002,
    EMPETSKILL_GETPOTIONS	= 0x0004,
    EMPETSKILL_GETMONEY		= 0x0008,
    EMPETSKILL_GETSTONE		= 0x0010,
	EMPETSKILL_GETQBOX		= 0x0020,
	EMPETSKILL_GETMATERIALS	= 0x0040,

    EMPETSKILL_NONE			= 0,
};

enum EMPETEQUALITY_SKILL
{
    EMPETEQUALITY_SKILL_NONE			 = 0,  // 없다
    EMPETEQUALITY_SKILL_DUPLICATED		 = 1,  // 스킬이 중복
    EMPETEQUALITY_SKILL_DUPLICATIONSTATS = 2,  // 스탯 증가가 중복
};

namespace pet
{

enum DB_PET_INVEN_TYPE
{
    TYPEA = 1,
    TYPEB = 2,
    SKILL = 3,
};

} // namespace pet

typedef std::vector<int> VEC_PET_DEFINE;
namespace COMMENT
{
    extern VEC_PET_DEFINE PET_DEFINE;

    int GetPetDefineIndex( const unsigned int nIndex );
};

enum PET_TYPE
{
    PETTYPE_NONE = -1,
};

#endif // _GLPET_DEFINE_H_