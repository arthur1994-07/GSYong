#ifndef GLCharStateDefine_H
#define GLCharStateDefine_H

#include "TMiniFSM.h"

/**
    상태값은 서버에서 관리의 주체가 되어야 한다.

    만약 클라이언트에서 상태값을 변경하고 싶다면 서버에 상태값 변경을 요청해
    서버와 동기화를 맞춘 후 변경 메시지를 받아서 클라이언트는 변경 해주도록 한다.
    
 */
typedef bool    CharState;
typedef int     CharStateID;
typedef BYTE    CharStateIndex;

/**
    캐릭터 상태 타입을 정의 한다.

 */
typedef TMiniFSM<CharStateID, CharState> GLStateMiniFSM;

/**
    캐릭터 상태의 기본값을 정의 한다.

 */
static const CharState      CHAR_STATE_DEFAULT              = true;

/**
    캐릭터 메인 상태 구분자 나열

    CharStateIndex 는 메인 상태 구분자
    CharStateID 는 서브 상태 구분자로 이해 하자.

    메인 상태 구분자로 어떤 상태(부활, 움직임등) 인지를 파악할 수 있고
    서브 상태 구분자로 설정된 상태 값의 의도, 사용 목적을 파악 할 수 있다.

 */
static const CharStateIndex CHAR_STATE_INDEX_NONE           = 0;
static const CharStateIndex CHAR_STATE_INDEX_MOVEMENT       = 1;

/**
    부활 서브 상태 구분자들을 정의 한다.
    부활이 가능한 상태 유무를 가진다.

 */
#include "GLCharRebirthID.h"

/**
    움직임 서브 상태 구분자들을 정의 한다.
    움직임 가능한 상태 유무를 가진다.
    움직임에 스킬도 포함된다.

 */
#include "GLCharMovementID.h"

#endif