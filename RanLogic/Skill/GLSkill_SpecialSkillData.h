
#pragma once

namespace SKILL
{
	//	��ȯ �� ���뺰 ����.
	struct SCREATURE_LVL
	{
		SNATIVEID			sID;                // ��ȯ�� ID
		FLOAT				fSEC_RECALLDELAY;   // ��ȯ ������ ��
		FLOAT				fSEC_ACTIVETIME;    // ��ȯ�� Ȱ�� �ð�
		DWORD               dwMAX;              // �ִ� ��ȯ ����
		WORD                wUSESKILLLEVEL;     // ��ȯ���� ����� ��ų ����
		DWORD				dwType;				// ��ȯ Ÿ��;
		bool				bEventSkill;		// �̺�Ʈ ��ų ���� ����;

		SCREATURE_LVL (void);
	};
};