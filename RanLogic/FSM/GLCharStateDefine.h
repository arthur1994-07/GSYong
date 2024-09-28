#ifndef GLCharStateDefine_H
#define GLCharStateDefine_H

#include "TMiniFSM.h"

/**
    ���°��� �������� ������ ��ü�� �Ǿ�� �Ѵ�.

    ���� Ŭ���̾�Ʈ���� ���°��� �����ϰ� �ʹٸ� ������ ���°� ������ ��û��
    ������ ����ȭ�� ���� �� ���� �޽����� �޾Ƽ� Ŭ���̾�Ʈ�� ���� ���ֵ��� �Ѵ�.
    
 */
typedef bool    CharState;
typedef int     CharStateID;
typedef BYTE    CharStateIndex;

/**
    ĳ���� ���� Ÿ���� ���� �Ѵ�.

 */
typedef TMiniFSM<CharStateID, CharState> GLStateMiniFSM;

/**
    ĳ���� ������ �⺻���� ���� �Ѵ�.

 */
static const CharState      CHAR_STATE_DEFAULT              = true;

/**
    ĳ���� ���� ���� ������ ����

    CharStateIndex �� ���� ���� ������
    CharStateID �� ���� ���� �����ڷ� ���� ����.

    ���� ���� �����ڷ� � ����(��Ȱ, �����ӵ�) ������ �ľ��� �� �ְ�
    ���� ���� �����ڷ� ������ ���� ���� �ǵ�, ��� ������ �ľ� �� �� �ִ�.

 */
static const CharStateIndex CHAR_STATE_INDEX_NONE           = 0;
static const CharStateIndex CHAR_STATE_INDEX_MOVEMENT       = 1;

/**
    ��Ȱ ���� ���� �����ڵ��� ���� �Ѵ�.
    ��Ȱ�� ������ ���� ������ ������.

 */
#include "GLCharRebirthID.h"

/**
    ������ ���� ���� �����ڵ��� ���� �Ѵ�.
    ������ ������ ���� ������ ������.
    �����ӿ� ��ų�� ���Եȴ�.

 */
#include "GLCharMovementID.h"

#endif