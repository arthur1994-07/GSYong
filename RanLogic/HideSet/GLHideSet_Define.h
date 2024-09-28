#ifndef _EMHIDESET_DEFINE_H_
#define _EMHIDESET_DEFINE_H_

#pragma once

enum EMHIDESET
{
	// �Ϲ�;
	EMHIDE_PICKUP_ITEM = 0, // ������ �ݱ�
	EMHIDE_GETON_VEHICLE, // Ż�� Ÿ��
	EMHIDE_GETOFF_VEHICLE, // Ż�� ������
	EMHIDE_DEATH, // ���

	EMHIDE_UPDATE_EXP, // ����ġ ȹ��
	EMHIDE_LEVELUP, // ������
	EMHIDE_LEVELDOWN, // ���� �ٿ�

	// �ʵ� �̵�;
	EMHIDE_TURN_WALK, // �ȱ� ��ȯ
	EMHIDE_TURN_RUN, // �ٱ� ��ȯ
	EMHIDE_ON_VEHICLE, // Ż�� ž�� ����
	EMHIDE_BUS, // ����

	EMHIDE_TAXI, // �ý�

	// ����;
	EMHIDE_START_ATTACK, // �Ϲ� ���� ����
	EMHIDE_USE_SKILL, // ��ų ���
	EMHIDE_TAKE_BUFF, // �ڽſ��� ���� ����
	EMHIDE_TAKE_LIMIT, // �ڽſ��� ����Ʈ ��ų

	EMHIDE_TAKE_DEBUFF, // �ڽſ��� ����� ��ų ����
	EMHIDE_ON_HIT, // ���� ���ݿ� ���ߵǾ� HP�� ������
	EMHIDE_TAKE_DAMAGE_BY_DEBUFF, // ���� ���ӵ����� ��ų�� ���ߵǾ� HP�� ������
	EMHIDE_AVOID, //���� ȸ��

	// Q�ڽ� ȿ�� ���� ����;
	EMHIDE_SPEEDUP,
	EMHIDE_SPEEDUP_MAX,
	EMHIDE_CRAZY,
	EMHIDE_MADNESS,

	EMHIDE_EXP_TIME,
	EMHIDE_EXP_GET,
	EMHIDE_POWERUP,
	EMHIDE_POWERUP_MAX,

	EMHIDE_BOMB,
	EMHIDE_HEAL,
	EMHIDE_LUCKY,
	EMHIDE_SUMMON,

	// ������ ��� ����;
	EMHIDE_USE_SKILLBOOK, // ��ų�� ���
	EMHIDE_USE_ACCREDIT, // ������ ���(���� ����)
	EMHIDE_RESET_SKILL_STAT, // ��ų/���� ���� ���, ��� �Ϸ�
	EMHIDE_USE_GRINDER, // ������ ���

	EMHIDE_USE_INVEN_EXTENSION, // �κ� Ȯ�� ���
	EMHIDE_USE_STORAGE_EXTENSION, // â�� Ȯ�� ���
	EMHIDE_USE_STORAGE_LINK, // â�� ���� ���
	EMHIDE_USE_MARKET_GRANT, // ���λ��� �㰡�� ��� ����

	EMHIDE_USE_DISGUISE_SPLIT, // �ڽ�Ƭ �и� ���
	EMHIDE_USE_DISGUISE_REMOVE, // ����(�ڽ�Ƭ ����) ���
	EMHIDE_USE_LOOK_CHANGE, // ��/��Ÿ��/����÷� ����
	EMHIDE_USE_PET_CHANGE, // �� �̸�/�÷� ���� �� ���� ���

	EMHIDE_USE_LUNCH, // ���ö� ���
	EMHIDE_USE_POINT_CHARGE, // ����Ʈ ī��� �ΰ��� ����Ʈ ����	

	// Ż�� ����;
	EMHIDE_REMODEL_VEHICLE, // Ż �� ������ Ż��

	// ���;	
	EMHIDE_BEING_SELF, // ���� ��� ����/��� ��
	EMHIDE_BEING_PARTY, // ��Ƽ ��� ����/��� ��
	EMHIDE_BEING_CLUB, // Ŭ�� ��� ����/��� ��	

	// ���� ����;
	EMHIDE_STORE_OPEN, // ���� ���� ��ġ
	EMHIDE_STORE_CLOSE, // ���� ���� ����

	// NPC;
	EMHIDE_TALK_START, // ���� ��ȭ ����(���ɱ�)
	EMHIDE_TALK_END, // ���� NPC���� �ŷ� �Ϸ�(Ȥ�� ��ȭ �Ϸ�, â ����)
	EMHIDE_QUEST_ACCEPT, // NPC ��ȭ�� ����Ʈ ����
	EMHIDE_QUEST_COMPLETE, // NPC ��ȭ�� ����Ʈ �Ϸ�

	// ���ΰŷ�;
	EMHIDE_DEAL_ACCEPT_SELF, // ���� �ŷ� ��û ������
	EMHIDE_DEAL_ACCEPT_TARGET, // ���� �ŷ� ��û ���� ����
	EMHIDE_DEAL_REQUET_SEND, // ���� �ŷ� ��û ��
	EMHIDE_DEAL_REQUEST_RECEIVE, // ���� �ŷ� ��û ����	
	

	// ��; // ���� ���϶��� �굵 ���ŵǰ�;
	EMHIDE_PET_SUMMON, // �� ��ȯ
	EMHIDE_PET_SUMMONNING, // �� ��ȯ ��
	EMHIDE_PET_SUMMON_RELEASE, // �� ��ȯ ����
	EMHIDE_PET_SKILL_LEARN, // �� ��ų ����

	// ������;
	EMHIDE_GUIDANCE_CERTIFY_HIDECANCLE,	// �������̸� ���̰��Ұ��ΰ�;

	EMHIDE_SIZE
};

#endif