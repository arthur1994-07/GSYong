
#pragma once

#include "./GLSkillDef.h"
#include "../Character/GLCharDefine.h"
#include "./GLSkill_SpecialSkillData.h"

namespace SKILL
{
	struct SSPECIAL_SKILL_102
	{
		EMSPECIALSKILLTYPE	emSSTYPE;					//  Ư�� ��ų Ÿ��
		std::string			strEffectName;				//  ���Ž� ����Ʈ, ��ȯ�� ����Ʈ ����Ʈ

		// ���� ���� ������
		DWORD				dwRemainSecond;				//  ���� ��
		std::string			strTransform_Man[EMTRANSFORM_NSIZE];	//	���Ž� ����ϴ� �����̸�
		std::string			strTransform_Woman[EMTRANSFORM_NSIZE];	//	���Ž� ����ϴ� �����̸�

		// ��ȯ ���� ������
		SNATIVEID			sCreatureID;			// ��ȯ�� ID
		DWORD				dwRecallDelaySec;		// ��ȯ ������ ��
		DWORD				dwCreatureActTimeSec;	// ��ȯ�� Ȱ�� �ð�

		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SSPECIAL_SKILL_104
	{
		EMSPECIALSKILLTYPE	emSSTYPE;					//  Ư�� ��ų Ÿ��
		std::string			strEffectName;				//  ���Ž� ����Ʈ, ��ȯ�� ����Ʈ ����Ʈ


		// ���� ���� ������
		DWORD				dwRemainSecond;				//  ���� ��
		std::string			strTransform_Man[EMTRANSFORM_NSIZE];	//	���Ž� ����ϴ� �����̸�
		std::string			strTransform_Woman[EMTRANSFORM_NSIZE];	//	���Ž� ����ϴ� �����̸�

		// ��ȯ ���� ������
		SCREATURE_LVL       sCREATURE_LVL[MAX_LEVEL_BEFORE];
		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SSPECIAL_SKILL
	{
		enum { VERSION = 0x0105, };

		EMSPECIALSKILLTYPE	emSSTYPE;					//  Ư�� ��ų Ÿ��
		std::string			strEffectName;				//  ���Ž� ����Ʈ, ��ȯ�� ����Ʈ ����Ʈ


		// ���� ���� ������
		DWORD				dwRemainSecond;				//  ���� ��
		std::string			strTransform_Man[EMTRANSFORM_NSIZE];	//	���Ž� ����ϴ� �����̸�
		std::string			strTransform_Woman[EMTRANSFORM_NSIZE];	//	���Ž� ����ϴ� �����̸�

		// ��ȯ ���� ������
		SCREATURE_LVL       sCREATURE_LVL[MAX_LEVEL];

		SSPECIAL_SKILL();
		void ResetAll();

		BOOL SAVE ( sc::SerialFile &SFile );
		BOOL LOAD ( sc::BaseStream &SFile );
		BOOL LOAD_100 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
		BOOL LOAD_101 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
		BOOL LOAD_102 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
		BOOL LOAD_103 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
		BOOL LOAD_104 ( sc::BaseStream &SFile );	//	������ ���� ���� ����

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		SSPECIAL_SKILL & operator= ( const SSPECIAL_SKILL  &value );
	};
};