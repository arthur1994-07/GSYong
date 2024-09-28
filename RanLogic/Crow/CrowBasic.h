#ifndef _CROW_BASIC_H_
#define _CROW_BASIC_H_

#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../Pet/GLPetDefine.h"
#include "../Summon/GLSummonDefine.h"
#include "../Crow/GLCrowDefine.h"
#include "../Character/GLCharDefine.h"

#define MOBNPCEDIT_VERSION 0x0122

struct SCROWBASIC_100
{
    SNATIVEID			sNativeID;					//	���� ID.
    char				m_szName[CHAR_SZNAME];		//	�̸�.
    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    WORD				m_wLevel;					//	����.

    WORD				m_wHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST_101			m_sResist;					//	���װ�.
};

struct SCROWBASIC_101
{
    SNATIVEID			sNativeID;					//	���� ID.
    char				m_szName[CHAR_SZNAME];		//	�̸�.
    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    WORD				m_wLevel;					//	����.

    WORD				m_wHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST				m_sResist;					//	���װ�.
};

struct SCROWBASIC_102
{
    SNATIVEID			sNativeID;					//	���� ID.
    char				m_szName[CHAR_SZNAME];		//	�̸�.
    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    WORD				m_wLevel;					//	����.

    WORD				m_wHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST				m_sResist;					//	���װ�.

    WORD				m_wBonusExp;				//	�߰� ����ġ.
};

struct SCROWBASIC_103
{
    SNATIVEID			sNativeID;					//	���� ID.

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST				m_sResist;					//	���װ�.

    WORD				m_wBonusExp;				//	�߰� ����ġ.
};

struct SCROWBASIC_104
{
    SNATIVEID			sNativeID;					//	���� ID.

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST				m_sResist;					//	���װ�.

    WORD				m_wBonusExp;				//	�߰� ����ġ.
};

struct SCROWBASIC_105
{
    SNATIVEID			sNativeID;					//	���� ID.

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST				m_sResist;					//	���װ�.

    WORD				m_wBonusExp;				//	�߰� ����ġ.

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��
};

struct SCROWBASIC_106
{
    SNATIVEID			sNativeID;					//	���� ID.

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST				m_sResist;					//	���װ�.

    WORD				m_wBonusExp;				//	�߰� ����ġ.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��
};

struct SCROWBASIC_108
{
    SNATIVEID			sNativeID;					//	���� ID.

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST				m_sResist;					//	���װ�.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��
};

struct SCROWBASIC_109
{
    SNATIVEID			sNativeID;					//	���� ID.

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST				m_sResist;					//	���װ�.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��

    DWORD				m_dwGenTime;				// ȣ�� �ð�
};

struct SCROWBASIC_110	//	������ ���� ���� ����
{
    SNATIVEID			sNativeID;					//	���� ID.

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    EMTRIBE				m_emTribe;					//	����.
    EMCROW				m_emCrow;					//	Npc, Mob.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wDefense;					//	����.

    SRESIST				m_sResist;					//	���װ�.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)

    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�
};

struct SCROWBASIC_111 // �Ҹ��� ȿ�� ���� �߰�
{
    SNATIVEID			sNativeID;					//	���� ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	����.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wDefense;					//	����.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.

    SRESIST				m_sResist;					//	���װ�.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)
    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��
};

struct SCROWBASIC_112	// �߰� CP �߰�
{

    SNATIVEID			sNativeID;					//	���� ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	����.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wDefense;					//	����.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.

    SRESIST				m_sResist;					//	���װ�.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)
    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�

    bool				m_bNoPenalty;				// �Ҹ��� ȿ�� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��

};

struct SCROWBASIC_113 // �߰� ���� �ȹ��� ���� �߰�
{

    SNATIVEID			sNativeID;					//	���� ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	����.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wDefense;					//	����.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.
    WORD				m_wBonusCP;					//	�߰� CP.

    SRESIST				m_sResist;					//	���װ�.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)
    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�

    bool				m_bNoPenalty;				// �Ҹ��� ȿ�� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��
};

struct SCROWBASIC_114 // ��ȯ�� Ÿ�� �߰�, Ÿ�Ժ� �Ҹ��� ȿ�� ����
{
    SNATIVEID			sNativeID;					//	���� ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	����.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wDefense;					//	����.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.
    WORD				m_wBonusCP;					//	�߰� CP.

    SRESIST				m_sResist;					//	���װ�.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    bool                m_bNonDamaged;              //  ���� �ȹ��� ����

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)
    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�

    bool				m_bNoPenalty;				// �Ҹ��� ȿ�� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��
};

struct SCROWBASIC_115 // NPC Ÿ�� �߰�
{
    SNATIVEID			sNativeID;					//	���� ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	����.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wDefense;					//	����.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.
    WORD				m_wBonusCP;					//	�߰� CP.

    SRESIST				m_sResist;					//	���װ�.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    bool                m_bNonDamaged;              //  ���� �ȹ��� ����

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)
    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�

    bool				m_bNoPenalty;				// �Ҹ��� ȿ�� ����
    DWORD               m_dwNoPenaltyFlag;			// Ÿ�Ժ� �Ҹ��� ȿ�� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��

    // ��ȯ�� Ÿ�� �߰�
    SUMMON_TYPE         m_emSummonType;             // ��ȯ�� Ÿ��
};

struct SCROWBASIC_116
{
    SNATIVEID			sNativeID;					//	���� ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	����.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wDefense;					//	����.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.
    WORD				m_wBonusCP;					//	�߰� CP.

    SRESIST				m_sResist;					//	���װ�.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    bool                m_bNonDamaged;              //  ���� �ȹ��� ����

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)
    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�

    bool				m_bNoAllPenalty;			// ��� �Ҹ��� ȿ�� ����
    DWORD               m_dwNoPenaltyFlag;			// Ÿ�Ժ� �Ҹ��� ȿ�� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��

    // ��ȯ�� Ÿ�� �߰�
    SUMMON_TYPE         m_emSummonType;             // ��ȯ�� Ÿ��

    // NPC Ÿ�� �߰�
    NPC_TYPE			m_emNPCType;                // NPC Ÿ��
};

struct SCROWBASIC_117
{
    SNATIVEID			sNativeID;					//	���� ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	����.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wDefense;					//	����.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.
    WORD				m_wBonusCP;					//	�߰� CP.

    SRESIST				m_sResist;					//	���װ�.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    bool                m_bNonDamaged;              //  ���� �ȹ��� ����

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)
    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�

    bool				m_bNoAllPenalty;			// ��� �Ҹ��� ȿ�� ����
    DWORD               m_dwNoPenaltyFlag;			// Ÿ�Ժ� �Ҹ��� ȿ�� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��

    // ��ȯ�� Ÿ�� �߰�
    SUMMON_TYPE         m_emSummonType;             // ��ȯ�� Ÿ��

    // NPC Ÿ�� �߰�
    NPC_TYPE			m_emNPCType;                // NPC Ÿ��

    bool                m_bHideNaming;              // �̸�/HP ǥ������ ����
    bool                m_bNonTargeting;            // Ÿ���� ���� ���ϰ� ��
};

struct SCROWBASIC_118
{
	SNATIVEID			sNativeID;					//	���� ID.

	EMCROW				m_emCrow;					//	Npc, Mob.
	EMTRIBE				m_emTribe;					//	����.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wDefense;					//	����.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	DWORD				m_wBonusExp;				//	�߰� ����ġ.
	WORD				m_wBonusCP;					//	�߰� CP.

	SRESIST				m_sResist;					//	���װ�.

	bool				m_bOverlapAttack;			//  �������� ���� ���� ����

	bool                m_bNonDamaged;              //  ���� �ȹ��� ����

	DWORD				m_dwGenTime;				// ȣ�� �ð�

	WORD				m_wGatherTimeLow;			// ä���ð�(Low)
	WORD				m_wGatherTimeHigh;			// ä���ð�(High)
	float				m_fGatherRate;				// ä��Ȯ��
	WORD				m_wGatherAnimation;			// �ִϸ��̼�

	bool				m_bNoAllPenalty;			// ��� �Ҹ��� ȿ�� ����
	DWORD               m_dwNoPenaltyFlag;			// Ÿ�Ժ� �Ҹ��� ȿ�� ����

	// ��Ÿ�� �߰�
	int				    m_emPetType;				// ��Ÿ��

	// ��ȯ�� Ÿ�� �߰�
	SUMMON_TYPE         m_emSummonType;             // ��ȯ�� Ÿ��

	// NPC Ÿ�� �߰�
	NPC_TYPE			m_emNPCType;                // NPC Ÿ��

	// 0x0117 �߰�
	bool                m_bHideNaming;              // �̸�/HP ǥ������ ����
	bool                m_bNonTargeting;            // Ÿ���� ���� ���ϰ� ��

	// 0x0118 �߰�
	float               m_fScale;                   // �����ϰ�
};

struct SCROWBASIC_119
{
	SNATIVEID			sNativeID;					//	���� ID.

	EMCROW				m_emCrow;					//	Npc, Mob.
	EMTRIBE				m_emTribe;					//	����.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wDefense;					//	����.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	DWORD				m_wBonusExp;				//	�߰� ����ġ.
	WORD				m_wBonusCP;					//	�߰� CP.

	SRESIST				m_sResist;					//	���װ�.

	bool				m_bOverlapAttack;			//  �������� ���� ���� ����

	bool                m_bNonDamaged;              //  ���� �ȹ��� ����

	DWORD				m_dwGenTime;				// ȣ�� �ð�

	WORD				m_wGatherTimeLow;			// ä���ð�(Low)
	WORD				m_wGatherTimeHigh;			// ä���ð�(High)
	float				m_fGatherRate;				// ä��Ȯ��
	WORD				m_wGatherAnimation;			// �ִϸ��̼�

	bool				m_bNoAllPenalty;			// ��� �Ҹ��� ȿ�� ����
	DWORD               m_dwNoPenaltyFlag;			// Ÿ�Ժ� �Ҹ��� ȿ�� ����

	// ��Ÿ�� �߰�
	int				    m_emPetType;				// ��Ÿ��

	// ��ȯ�� Ÿ�� �߰�
	SUMMON_TYPE         m_emSummonType;             // ��ȯ�� Ÿ��

	// NPC Ÿ�� �߰�
	NPC_TYPE			m_emNPCType;                // NPC Ÿ��

	// 0x0117 �߰�
	bool                m_bHideNaming;              // �̸�/HP ǥ������ ����
	bool                m_bNonTargeting;            // Ÿ���� ���� ���ϰ� ��

	// 0x0118 �߰�
	float               m_fScale;                   // �����ϰ�
	// 0x0119 �߰�
	bool				m_bHideTooltip;				// Mob/NPC ���� ����;
};

struct SCROWBASIC_120
{
	SNATIVEID			sNativeID;					//	���� ID.

	EMCROW				m_emCrow;					//	Npc, Mob.
	EMTRIBE				m_emTribe;					//	����.
	EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

	char				m_szName[CHAR_SZNAME];		//	�̸�.

	WORD				m_wLevel;					//	����.

	DWORD				m_dwHP;						//	HP
	WORD				m_wMP;						//	MP
	WORD				m_wSP;						//	SP

	//	ȸ����.
	float				m_fIncHP;					//	HP ȸ����.
	float				m_fIncMP;					//	MP ȸ����.
	float				m_fIncSP;					//	SP ȸ����.

	WORD				m_wDefense;					//	����.

	WORD				m_wViewRange;				//	�þ� �Ÿ�.
	WORD				m_wAvoidRate;				//	ȸ����.
	WORD				m_wHitRate;					//	������.

	DWORD				m_wBonusExp;				//	�߰� ����ġ.
	WORD				m_wBonusCP;					//	�߰� CP.

	SRESIST				m_sResist;					//	���װ�.

	bool				m_bOverlapAttack;			//  �������� ���� ���� ����

	bool                m_bNonDamaged;              //  ���� �ȹ��� ����

	DWORD				m_dwGenTime;				// ȣ�� �ð�

	WORD				m_wGatherTimeLow;			// ä���ð�(Low)
	WORD				m_wGatherTimeHigh;			// ä���ð�(High)
	float				m_fGatherRate;				// ä��Ȯ��
	WORD				m_wGatherAnimation;			// �ִϸ��̼�

	bool				m_bNoAllPenalty;			// ��� �Ҹ��� ȿ�� ����
	DWORD               m_dwNoPenaltyFlag;			// Ÿ�Ժ� �Ҹ��� ȿ�� ����

	// ��Ÿ�� �߰�
	int				    m_emPetType;				// ��Ÿ��

	// ��ȯ�� Ÿ�� �߰�
	SUMMON_TYPE         m_emSummonType;             // ��ȯ�� Ÿ��

	// NPC Ÿ�� �߰�
	NPC_TYPE			m_emNPCType;                // NPC Ÿ��

	// 0x0117 �߰�
	bool                m_bHideNaming;              // �̸�/HP ǥ������ ����
	bool                m_bNonTargeting;            // Ÿ���� ���� ���ϰ� ��

	// 0x0118 �߰�
	float               m_fScale;                   // �����ϰ�

	// 0x0119 �߰�
	bool				m_bHideTooltip;				// Mob/NPC ���� ����;
	// 0x0200 �߰�;
	WORD				m_wResist_Probability;		// ���� Ȯ��;

};

struct SCROWBASIC_121
{
    SNATIVEID			sNativeID;					//	���� ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	����.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wDefense;					//	����.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.
    WORD				m_wBonusCP;					//	�߰� CP.

    SRESIST				m_sResist;					//	���װ�.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    bool                m_bNonDamaged;              //  ���� �ȹ��� ����

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)
    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�

    bool				m_bNoAllPenalty;			// ��� �Ҹ��� ȿ�� ����
    DWORD               m_dwNoPenaltyFlag;			// Ÿ�Ժ� �Ҹ��� ȿ�� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��

    // ��ȯ�� Ÿ�� �߰�
    SUMMON_TYPE         m_emSummonType;             // ��ȯ�� Ÿ��

    // NPC Ÿ�� �߰�
    NPC_TYPE			m_emNPCType;                // NPC Ÿ��

    // 0x0117 �߰�
    bool                m_bHideNaming;              // �̸�/HP ǥ������ ����
    bool                m_bNonTargeting;            // Ÿ���� ���� ���ϰ� ��

    // 0x0118 �߰�
    float               m_fScale;                   // �����ϰ�

    // 0x0119 �߰�
    bool				m_bHideTooltip;				// Mob/NPC ���� ����;

    // 0x0200 �߰�;
    WORD				m_wResist_Probability;		// ���� Ȯ��;

    // 0x0121 �߰�;
	float				m_fBarrierRadius;			// ��ֹ� �ݰ�;
	float				m_fBarrierWidth;			// ��ֹ� �β�;
};
/**
    �ֽ� ������ �����͸� �ǹ� �Ѵ�.
    �ֽ� ������ 0x0117 �̶�� SCROWBASIC �� 0x0117 �� �ǹ� �Ѵ�.

    ���������� �ε��� ���� ���� �������� ����ü�� ���ǵǾ� �ִ�.

    �׷��� ���Ӱ� ������ �߰��Ǹ� ��������� ���������� ����� ��� �ϹǷ�
    �߰��� ������ ��Ȯ�� �˱� ���� ���� �߰��� �߰��� ������ ���� ������ ��� ���ֵ��� �Ѵ�.
    �ɼ� ������ ���� �Ʒ��κп� ������ �߰��ϵ��� �Ѵ�.
    0x0117 ������ ���� �Ѵ�.

 */
struct SCROWBASIC
{
    enum { VERSION = MOBNPCEDIT_VERSION, };

    SNATIVEID			sNativeID;					//	���� ID.

    EMCROW				m_emCrow;					//	Npc, Mob.
    EMTRIBE				m_emTribe;					//	����.
    EMBRIGHT			m_emBright;					//	�Ӽ� (��/��)

    char				m_szName[CHAR_SZNAME];		//	�̸�.

    WORD				m_wLevel;					//	����.

    DWORD				m_dwHP;						//	HP
    WORD				m_wMP;						//	MP
    WORD				m_wSP;						//	SP

    //	ȸ����.
    float				m_fIncHP;					//	HP ȸ����.
    float				m_fIncMP;					//	MP ȸ����.
    float				m_fIncSP;					//	SP ȸ����.

    WORD				m_wDefense;					//	����.

    WORD				m_wViewRange;				//	�þ� �Ÿ�.
    WORD				m_wAvoidRate;				//	ȸ����.
    WORD				m_wHitRate;					//	������.

    DWORD				m_wBonusExp;				//	�߰� ����ġ.
    WORD				m_wBonusCP;					//	�߰� CP.

    SRESIST				m_sResist;					//	���װ�.

    bool				m_bOverlapAttack;			//  �������� ���� ���� ����

    bool                m_bNonDamaged;              //  ���� �ȹ��� ����

    DWORD				m_dwGenTime;				// ȣ�� �ð�

    WORD				m_wGatherTimeLow;			// ä���ð�(Low)
    WORD				m_wGatherTimeHigh;			// ä���ð�(High)
    float				m_fGatherRate;				// ä��Ȯ��
    WORD				m_wGatherAnimation;			// �ִϸ��̼�

    bool				m_bNoAllPenalty;			// ��� �Ҹ��� ȿ�� ����
    DWORD               m_dwNoPenaltyFlag;			// Ÿ�Ժ� �Ҹ��� ȿ�� ����

    // ��Ÿ�� �߰�
    int				    m_emPetType;				// ��Ÿ��

    // ��ȯ�� Ÿ�� �߰�
    SUMMON_TYPE         m_emSummonType;             // ��ȯ�� Ÿ��

    // NPC Ÿ�� �߰�
    NPC_TYPE			m_emNPCType;                // NPC Ÿ��

    // 0x0117 �߰�
    bool                m_bHideNaming;              // �̸�/HP ǥ������ ����
    bool                m_bNonTargeting;            // Ÿ���� ���� ���ϰ� ��

    // 0x0118 �߰�
    float               m_fScale;                   // �����ϰ�

	// 0x0119 �߰�
	bool				m_bHideTooltip;				// Mob/NPC ���� ����;

	// 0x0120 �߰�;
	WORD				m_wResist_Probability;		// ���� Ȯ��;

	// 0x0121 �߰�;
	float				m_fBarrierRadius;			// ��ֹ� �ݰ�;
	float				m_fBarrierWidth;			// ��ֹ� �β�;

    // 0x0121 �߰�;
    float               m_fRefenceActionableRange;  // ��ȣ�ۿ� �ݰ�;

    SCROWBASIC();
    SCROWBASIC& operator = ( const SCROWBASIC_100 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_101 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_102 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_103 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_104 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_105 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_106 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_108 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_109 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_110 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_111 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_112 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_113 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_114 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_115 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_116 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_117 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_118 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_119 &sCrowBasic );
	SCROWBASIC& operator = ( const SCROWBASIC_120 &sCrowBasic );
    SCROWBASIC& operator = ( const SCROWBASIC_121 &sCrowBasic );

    static VOID SaveCsvHead ( std::fstream &SFile );
    VOID SaveCsv(std::fstream& SFile);
    VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);
};

#endif // _CROW_BASIC_H_