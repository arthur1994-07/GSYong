#ifndef _SANIMATION_DEFINE_H_
#define _SANIMATION_DEFINE_H_

#pragma once

#include <list>

class SAnimation;

enum EMSTRIKE_EFF
{
	EMSF_NULL		= 0,
	EMSF_SELFBODY	= 1,
	EMSF_TARGET		= 2,

	EMSF_SIZE		= 3
};

enum EMANICONINFO
{
	ACF_NEWINFO		= 0x0001,
	ACF_LOOP		= 0x0010,
	ACF_MOVE		= 0x0020,	// Max에서 캐릭터를 이동시켜 놓으면 실제 게임에서 동기화 시켜준다.
	ACF_UPBODY		= 0x0100,	// UPBODY 셋팅.		// UP DOWN 이 셋팅 안될 경우는 디폴트로 전체다.
	ACF_DOWNBODY	= 0x0200,	// UP이 없고 DOWN만 셋팅 될 경우 UP은 DOWN에 영향을 받아야 한다.
	ACF_SZNAME		= 128,
	ACF_DIV			= 8,
	ACF_STRIKE		= 9,
};

typedef std::list<SAnimation*> SANILIST;
typedef SANILIST::iterator	   SANILIST_ITER;

#endif // _SANIMATION_DEFINE_H_