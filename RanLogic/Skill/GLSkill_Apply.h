
#pragma once

#include "../Item/GLItemDef.h"
#include "../Character/GLCharDefine.h"

#include "./GLSkill_ApplyData.h"
#include "./GLSkillDef.h"

namespace SKILL
{
	struct SAPPLY_100
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		DWORD				dwCUREFLAG;					//	�ص� ����.
		SKILL::CDATA_LVL_100 sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	��ȭ ��ġ.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_100	sSPEC[MAX_LEVEL_BEFORE];			//	Ư�� ��� ������.
	};

	struct SAPPLY_101
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		DWORD				dwCUREFLAG;					//	�ص� ����.
		SKILL::CDATA_LVL_101 sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	��ȭ ��ġ.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_100	sSPEC[MAX_LEVEL_BEFORE];			//	Ư�� ��� ������.
	};

	struct SAPPLY_102
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		DWORD				dwCUREFLAG;					//	�ص� ����.
		SKILL::CDATA_LVL_101 sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	��ȭ ��ġ.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_100	sSPEC[MAX_LEVEL_BEFORE];			//	Ư�� ��� ������.
	};

	struct SAPPLY_103	//	������ ���� ���� ����
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		DWORD				dwCUREFLAG;					//	�ص� ����.
		SKILL::CDATA_LVL_101 sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	��ȭ ��ġ.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_101		sSPEC[MAX_LEVEL_BEFORE];			//	Ư�� ��� ������.
	};

	struct SAPPLY_104	// ���� ���� CP �߰�
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		SKILL::CDATA_LVL_101 sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.
		DWORD				dwCUREFLAG;					//	�ص� ����.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_101		sSPEC[MAX_LEVEL_BEFORE];			//	Ư�� ��� ������.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	��ȭ ��ġ.	

	};

	struct SAPPLY_105	//	�ΰ�ȿ�� �� Ư��ȿ�� vector�� ����
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		SKILL::CDATA_LVL_102 sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.
		DWORD				dwCUREFLAG;					//	�ص� ����.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.

		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_101		sSPEC[MAX_LEVEL_BEFORE];			//	Ư�� ��� ������.

		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	��ȭ ��ġ.	
	};

	struct SAPPLY_106	// ������ ������ ����
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		SKILL::CDATA_LVL_102 sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.
		DWORD				dwCUREFLAG;					//	�ص� ����.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.


		std::vector<SIMPACT_ADDON_100>	vecADDON;			//	�ΰ�ȿ��
		std::vector<SSPEC_ADDON_100>	vecSPEC;			//	Ư��ȿ��
	};

	struct SAPPLY_107	// ������ ������ ����
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		SKILL::CDATA_LVL_103 sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.
		DWORD				dwCUREFLAG;					//	�ص� ����.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.


		std::vector<SIMPACT_ADDON_100>	vecADDON;			//	�ΰ�ȿ��
		std::vector<SSPEC_ADDON_100>	vecSPEC;			//	Ư��ȿ��
	};

	struct SAPPLY_108
	{
		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		SKILL::CDATA_LVL	sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.
		DWORD				dwCUREFLAG;					//	�ص� ����.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.

		std::vector<SIMPACT_ADDON_100>	vecADDON;			//	�ΰ�ȿ��
		std::vector<SSPEC_ADDON_100>	vecSPEC;			//	Ư��ȿ��
	};

	struct SAPPLY_109	// ��ųȿ�� ���� ������� ����. by luxes.
	{
		// �̰����� ���� ���� ���� �� ��;
		// ���� ���� �� ������ ���� �ҷ����� �� ���� ����;
		// �Ʒ��� ���� �Ǿ������� ���� ���� �߰��� ����!!;

		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		SKILL::CDATA_LVL	sDATA_LVL[MAX_LEVEL_BEFORE];		//	������ ���� ��.
		DWORD				dwCUREFLAG;					//	�ص� ����.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL_BEFORE];		//	���� �̻� ��ġ��.

		union 
		{
			int					nEffectFrame;			//  ����Ʈ���� Ÿ����
			float				fRunningEffTime;		// 	�����ɽ��� ���۽ð�.
		}; //uinon


		std::vector<SIMPACT_ADDON>	vecADDON;			//	�ΰ�ȿ��
		std::vector<SSPEC_ADDON>	vecSPEC;			//	Ư��ȿ��
		SSPEC_CA_ADDON_100			sCAData;			//  �ݰݸ���Ʈ @@ vecSPEC������ �ݰ��� �־�߸� �ϸ�, vecSPEC������ �ݰ��� �ִ� 5�� ���� �Է��Ҽ� �ִ�.
	};
	// 109���ķδ� ��ų �ִ� ������ 9���� 24�� ������;

	struct SAPPLY	// ��ųȿ�� ���� ������� ����. by luxes.
	{
		enum { VERSION = 0x0113 };

		// �̰����� ���� ���� ���� �� ��;
		// ���� ���� �� ������ ���� �ҷ����� �� ���� ����;
		// �Ʒ��� ���� �Ǿ������� ���� ���� �߰��� ����!!;

		//	�⺻ ���밪 ����.
		SKILL::EMTYPES		emBASIC_TYPE;				//	���밪 Ÿ��.
		EMELEMENT			emELEMENT;					//	( ��, ��, ȭ, ��, ���� ) �Ӽ�.
		SKILL::CDATA_LVL	sDATA_LVL[MAX_LEVEL];		//	������ ���� ��.
		DWORD				dwCUREFLAG;					//	�ص� ����.

		//	�����̻� ����.
		EMSTATE_BLOW		emSTATE_BLOW;				//	���� �̻� ���� Ÿ��.
		SKILL::SSTATE_BLOW	sSTATE_BLOW[MAX_LEVEL];		//	���� �̻� ��ġ��.

		union 
		{
			int					nEffectFrame;			//  ����Ʈ���� Ÿ����
			float				fRunningEffTime;		// 	�����ɽ��� ���۽ð�.
		}; //uinon


		std::vector<SIMPACT_ADDON>	vecADDON;			//	�ΰ�ȿ��
		std::vector<SSPEC_ADDON>	vecSPEC;			//	Ư��ȿ��
		SSPEC_CA_ADDON				sCAData;			//  �ݰݸ���Ʈ @@ vecSPEC������ �ݰ��� �־�߸� �ϸ�, vecSPEC������ �ݰ��� �ִ� 5�� ���� �Է��Ҽ� �ִ�.

		void Assign ( SAPPLY_100 &sOldData );
		void Assign ( SAPPLY_101 &sOldData );
		void Assign ( SAPPLY_102 &sOldData );
		void Assign ( SAPPLY_103 &sOldData );
		void Assign ( SAPPLY_104 &sOldData );
		void Assign ( SAPPLY_105 &sOldData );

		void LOAD_106 ( sc::BaseStream &SFile );
		void LOAD_107 ( sc::BaseStream &SFile );
		void LOAD_108 ( sc::BaseStream &SFile );
		void LOAD_109 ( sc::BaseStream &SFile );
		void LOAD_110 ( sc::BaseStream &SFile );
		void LOAD_111 ( sc::BaseStream &SFile );
		void LOAD_112 ( sc::BaseStream &SFile );
		void LOAD_113 ( sc::BaseStream &SFile );
		void SAVE ( sc::SerialFile &SFile );

		SAPPLY();
		static VOID SaveCsvHead(std::fstream& SFile);
		VOID SaveCsv(std::fstream& SFile);
		VOID LoadCsv(CStringArray& StrArray);
		inline SKILL::EMTYPES BasicType() const { return emBASIC_TYPE; }	//! ���밪 Ÿ��
	};
};