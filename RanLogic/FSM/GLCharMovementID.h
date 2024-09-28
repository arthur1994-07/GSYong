#ifndef GLCharMovementID_H
#define GLCharMovementID_H

/**
    인던에서 입장 대기 트리거에 의해 움직임을 막는다.
    입장 대기 트리거에 의해 맵의 모든 유저들의 움직임을 막고
    트리거가 소멸 될 시 해제 된다.

 */
static const CharStateID CHAR_MOVEMENT_STATE_INSTANTMAP_READY = 0;


/**
	미니게임 - 홀짝 게임
	홀짝 게임이 시작(doBegin())되면 움직임을 막음;
	홀짝 게임 종료(doBegin())되면 움직임을 품;
 */
static const CharStateID CHAR_MOVEMENT_STATE_MINIGAME_ODDEVEN = 1;


// 인던에서 스크립트로 움직임을 제어;
static const CharStateID CHAR_MOVEMENT_INSTANCE_SCRIPT = 2;

/**
	미니게임 - 텍사스홀덤
	텍사스홀덤 게임이 시작(doBegin())되면 움직임을 막음;
	텍사스홀덤 게임 종료(doBegin())되면 움직임을 품;
 */
static const CharStateID CHAR_MOVEMENT_STATE_MINIGAME_TEXASHOLDEM = 2;

/**
	개인상점
 */
static const CharStateID CHAR_MOVEMENT_STATE_PRIVATE_MARKET = 3;

#endif
