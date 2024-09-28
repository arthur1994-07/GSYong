#pragma once

#include "./GLSkillDef.h"
#include "./GLSkill_LearnData.h"
#include "../Character/GLCharDefine.h"

namespace SKILL
{
	struct SLEARN_100
	{
		EMBRIGHT			emBRIGHT;					//	�Ӽ�.
		DWORD				dwCLASS;					//	����.
		SNATIVEID			sSKILL;						//	�䱸 ������ų.

		SLEARN_LVL			sLVL_STEP[MAX_LEVEL_BEFORE];		//	���� �� �䱸 ��ġ.
	};

	struct SLEARN_101	//	������ ���� ���� ����
	{
		EMBRIGHT			emBRIGHT;					//	�Ӽ�.
		DWORD				dwCLASS;					//	����.
		SNATIVEID			sSKILL;						//	�䱸 ������ų.

		SLEARN_LVL_101		sLVL_STEP[MAX_LEVEL_BEFORE];		//	���� �� �䱸 ��ġ.
	};

	struct SLEARN_102
	{
		DWORD				dwCLASS;					//	����.
		EMBRIGHT			emBRIGHT;					//	�Ӽ�.
		SNATIVEID			sSKILL;						//	�䱸 ������ų.

		SLEARN_LVL_102		sLVL_STEP[MAX_LEVEL_BEFORE];		//	���� �� �䱸 ��ġ.
	};

	struct SLEARN_103
	{
		enum { VERSION = 0x0103, };

		DWORD				dwCLASS;					//	����.
		EMBRIGHT			emBRIGHT;					//	�Ӽ�.
		SNATIVEID			sSKILL;						//	�䱸 ������ų.

		SLEARN_LVL_103		sLVL_STEP[MAX_LEVEL_BEFORE];		//	���� �� �䱸 ��ġ.

		BOOL				bOnlyOneStats;				//	���� �Ѱ��� ���� �� ���� ����
		SNATIVEID			sHiddenWeapon;				//	�۷��� �� ��� ��ų ��� �� ���������
	};

	struct SLEARN_104
	{
		enum { VERSION = 0x0104, };

		DWORD				dwCLASS;					//	����.
		EMBRIGHT			emBRIGHT;					//	�Ӽ�.
		SNATIVEID			sSKILL;						//	�䱸 ������ų.

		SLEARN_LVL_103		sLVL_STEP[MAX_LEVEL_BEFORE];		//	���� �� �䱸 ��ġ.

		BOOL				bOnlyOneStats;				//	���� �Ѱ��� ���� �� ���� ����
		SNATIVEID			sHiddenWeapon;				//	��ų ��� �� 2������ ��� ������
		BOOL				bHiddenWeapon;				//	��ų ��� 2������ ��� ����
	};

	struct SLEARN
	{
		enum { VERSION = 0x0105, };

		DWORD				dwCLASS;					//	����.
		EMBRIGHT			emBRIGHT;					//	�Ӽ�.
		SNATIVEID			sSKILL;						//	�䱸 ������ų.

		SLEARN_LVL			sLVL_STEP[MAX_LEVEL];		//	���� �� �䱸 ��ġ.

		BOOL				bOnlyOneStats;				//	���� �Ѱ��� ���� �� ���� ����
		SNATIVEID			sHiddenWeapon;				//	��ų ��� �� 2������ ��� ������
		BOOL				bHiddenWeapon;				//	��ų ��� 2������ ��� ����

		SLEARN (void);

		void Assign ( SLEARN_100 &OldData );
		void Assign ( SLEARN_101 &OldData );
		void Assign ( SLEARN_102 &OldData );
		void Assign ( SLEARN_103 &OldData );
		void Assign ( SLEARN_104 &OldData );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );
	};
};