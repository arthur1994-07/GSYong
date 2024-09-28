#ifndef _GL_SUMMON_DEFINE_H_
#define _GL_SUMMON_DEFINE_H_

#pragma once

enum SUMMON_TYPE
{	
    SUMMON_TYPE_NONE       = -1,	// 타입이 없음;
    SUMMON_TYPE_NORMAL	   = 0,		// 스크립트로 관리되는 타입;
    SUMMON_TYPE_ATTACK     = 1,		// 공격 타입;
    SUMMON_TYPE_DISPOSABLE = 2,		// 자폭 타입;
    SUMMON_TYPE_BUFF       = 3,		// 버프 타입;

	SUMMON_TYPE_WAY_1	   = 4,
	SUMMON_TYPE_WAY_2	   = 5,
	SUMMON_TYPE_NPC		   = 6,
    SUMMON_TYPE_SIZE       = 7,
};

#endif // _GL_SUMMON_DEFINE_H_