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
	ACF_MOVE		= 0x0020,	// Max���� ĳ���͸� �̵����� ������ ���� ���ӿ��� ����ȭ �����ش�.
	ACF_UPBODY		= 0x0100,	// UPBODY ����.		// UP DOWN �� ���� �ȵ� ���� ����Ʈ�� ��ü��.
	ACF_DOWNBODY	= 0x0200,	// UP�� ���� DOWN�� ���� �� ��� UP�� DOWN�� ������ �޾ƾ� �Ѵ�.
	ACF_SZNAME		= 128,
	ACF_DIV			= 8,
	ACF_STRIKE		= 9,
};

typedef std::list<SAnimation*> SANILIST;
typedef SANILIST::iterator	   SANILIST_ITER;

#endif // _SANIMATION_DEFINE_H_