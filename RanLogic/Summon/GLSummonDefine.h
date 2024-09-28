#ifndef _GL_SUMMON_DEFINE_H_
#define _GL_SUMMON_DEFINE_H_

#pragma once

enum SUMMON_TYPE
{	
    SUMMON_TYPE_NONE       = -1,	// Ÿ���� ����;
    SUMMON_TYPE_NORMAL	   = 0,		// ��ũ��Ʈ�� �����Ǵ� Ÿ��;
    SUMMON_TYPE_ATTACK     = 1,		// ���� Ÿ��;
    SUMMON_TYPE_DISPOSABLE = 2,		// ���� Ÿ��;
    SUMMON_TYPE_BUFF       = 3,		// ���� Ÿ��;

	SUMMON_TYPE_WAY_1	   = 4,
	SUMMON_TYPE_WAY_2	   = 5,
	SUMMON_TYPE_NPC		   = 6,
    SUMMON_TYPE_SIZE       = 7,
};

#endif // _GL_SUMMON_DEFINE_H_