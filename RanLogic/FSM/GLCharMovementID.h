#ifndef GLCharMovementID_H
#define GLCharMovementID_H

/**
    �δ����� ���� ��� Ʈ���ſ� ���� �������� ���´�.
    ���� ��� Ʈ���ſ� ���� ���� ��� �������� �������� ����
    Ʈ���Ű� �Ҹ� �� �� ���� �ȴ�.

 */
static const CharStateID CHAR_MOVEMENT_STATE_INSTANTMAP_READY = 0;


/**
	�̴ϰ��� - Ȧ¦ ����
	Ȧ¦ ������ ����(doBegin())�Ǹ� �������� ����;
	Ȧ¦ ���� ����(doBegin())�Ǹ� �������� ǰ;
 */
static const CharStateID CHAR_MOVEMENT_STATE_MINIGAME_ODDEVEN = 1;


// �δ����� ��ũ��Ʈ�� �������� ����;
static const CharStateID CHAR_MOVEMENT_INSTANCE_SCRIPT = 2;

/**
	�̴ϰ��� - �ػ罺Ȧ��
	�ػ罺Ȧ�� ������ ����(doBegin())�Ǹ� �������� ����;
	�ػ罺Ȧ�� ���� ����(doBegin())�Ǹ� �������� ǰ;
 */
static const CharStateID CHAR_MOVEMENT_STATE_MINIGAME_TEXASHOLDEM = 2;

/**
	���λ���
 */
static const CharStateID CHAR_MOVEMENT_STATE_PRIVATE_MARKET = 3;

#endif
