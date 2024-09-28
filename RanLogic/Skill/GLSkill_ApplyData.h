
#pragma once

#include "./GLSkillDef.h"
#include "./GLSkill_SubData.h"
#include "./GLSkill.h"

#include "../Character/GLCharDefine.h"

namespace SKILL
{
	struct SSTATE_BLOW
	{
		float			fRATE;		//	�߻� Ȯ��.
		float			fVAR1;		//	���� �� 1.
		float			fVAR2;		//	���� �� 2.

		SSTATE_BLOW ();
	};

	struct SIMPACT_ADDON_100
	{
		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	��ȭ ��ġ.	

		SIMPACT_ADDON_100();
	};

	struct SIMPACT_ADDON_101
	{
		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL_BEFORE];		//	��ȭ ��ġ.	
		float				fRate[MAX_LEVEL_BEFORE]; // ���� ȿ�� ����. by luxes.

		SIMPACT_ADDON_101();
	};


	struct SIMPACT_ADDON
	{
		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR[MAX_LEVEL];		//	��ȭ ��ġ.	
		float				fRate[MAX_LEVEL]; // ���� ȿ�� ����. by luxes.

		SIMPACT_ADDON(const EMIMPACT_ADDON eAddon);
		SIMPACT_ADDON();
		void Assign( SIMPACT_ADDON_100& sOldData );
		void Assign( SIMPACT_ADDON_101& sOldData );
		bool operator()(SIMPACT_ADDON& key)		const;
	};

	struct SSPEC_ADDON_100
	{
		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_101	sSPEC[MAX_LEVEL_BEFORE];			//	Ư�� ��� ������.

		SSPEC_ADDON_100()
			: emSPEC ( EMSPECA_NULL )
		{
		}
	};

	struct SSPEC_ADDON_101
	{
		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC_102	sSPEC[MAX_LEVEL_BEFORE];			//	Ư�� ��� ������.

		SSPEC_ADDON_101()
			: emSPEC ( EMSPECA_NULL )
		{
		}
	};

	struct SSPEC_ADDON_102
	{
		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC		sSPEC[MAX_LEVEL_BEFORE];			//	Ư�� ��� ������.
		
		SSPEC_ADDON_102()
			: emSPEC ( EMSPECA_NULL )
		{
		}
	};

	struct SSPEC_ADDON
	{
		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC		sSPEC[MAX_LEVEL];			//	Ư�� ��� ������.

		SSPEC_ADDON();
		SSPEC_ADDON(const EMSPEC_ADDON eAddon);
		void Assign( SSPEC_ADDON_100& sOldData );
		void Assign( SSPEC_ADDON_101& sOldData );
		void Assign( SSPEC_ADDON_102& sOldData );
		bool operator()(SSPEC_ADDON& key) const;
	};

	struct CDATA_LVL_100
	{
		float			fDELAYTIME;			//	������ Ÿ��.

		float			fLIFE;				//	���ӽð�.
		WORD			wAPPLYRANGE;		//	���� ����.
		WORD			wAPPLYNUM;			//	���� ����.
		WORD			wAPPLYANGLE;		//	���� ����.
		WORD			wPIERCENUM;			//	���� ����.

		float			fBASIC_VAR;			//	��ȭ��.

		//	���� �Ҹ�-����. ( ������� )
		WORD			wUSE_ARROWNUM;		//	�Ҹ� ȭ�� ����.

		WORD			wUSE_HP;			//	���� HP ������.
		WORD			wUSE_MP;			//	���� MP ������.
		WORD			wUSE_SP;			//	���� SP ������.

		WORD			wUSE_HP_PTY;		//	���� HP ��Ƽ�� ������.
		WORD			wUSE_MP_PTY;		//	���� MP ��Ƽ�� ������.
		WORD			wUSE_SP_PTY;		//	���� SP ��Ƽ�� ������.
	};

	struct CDATA_LVL_101
	{
		float			fDELAYTIME;			//	������ Ÿ��.

		float			fLIFE;				//	���ӽð�.
		WORD			wAPPLYRANGE;		//	���� ����.
		WORD			wAPPLYNUM;			//	���� ����.
		WORD			wAPPLYANGLE;		//	���� ����.
		WORD			wPIERCENUM;			//	���� ����.
		WORD			wTARNUM;			//	Ÿ�� ����.

		float			fBASIC_VAR;			//	��ȭ��.

		//	���� �Ҹ�-����. ( ������� )
		WORD			wUSE_ARROWNUM;		//	�Ҹ� ȭ�� ����.
		WORD			wUSE_CHARMNUM;		//	�Ҹ� ���� ����.

		WORD			wUSE_EXP;			//	���� EXP ������.
		WORD			wUSE_HP;			//	���� HP ������.
		WORD			wUSE_MP;			//	���� MP ������.
		WORD			wUSE_SP;			//	���� SP ������.

		WORD			wUSE_HP_PTY;		//	���� HP ��Ƽ�� ������.
		WORD			wUSE_MP_PTY;		//	���� MP ��Ƽ�� ������.
		WORD			wUSE_SP_PTY;		//	���� SP ��Ƽ�� ������.
	};

	struct CDATA_LVL_102	// CP �߰�
	{
		float			fDELAYTIME;			//	������ Ÿ��.

		float			fLIFE;				//	���ӽð�.
		WORD			wAPPLYRANGE;		//	���� ����.
		WORD			wAPPLYNUM;			//	���� ����.
		WORD			wAPPLYANGLE;		//	���� ����.
		WORD			wPIERCENUM;			//	���� ����.
		WORD			wTARNUM;			//	Ÿ�� ����.

		float			fBASIC_VAR;			//	��ȭ��.

		//	���� �Ҹ�-����. ( ������� )
		WORD			wUSE_ARROWNUM;		//	�Ҹ� ȭ�� ����.
		WORD			wUSE_CHARMNUM;		//	�Ҹ� ���� ����.

		WORD			wUSE_EXP;			//	���� EXP ������.
		WORD			wUSE_HP;			//	���� HP ������.
		WORD			wUSE_MP;			//	���� MP ������.
		WORD			wUSE_SP;			//	���� SP ������.
		WORD			wUSE_CP;			//	���� CP ������.

		WORD			wUSE_HP_PTY;		//	���� HP ��Ƽ�� ������.
		WORD			wUSE_MP_PTY;		//	���� MP ��Ƽ�� ������.
		WORD			wUSE_SP_PTY;		//	���� SP ��Ƽ�� ������.
	};

	struct CDATA_LVL_103	//	�߼�ź �߰�
	{
		float			fDELAYTIME;			//	������ Ÿ��.

		float			fLIFE;				//	���ӽð�.
		WORD			wAPPLYRANGE;		//	���� ����.
		WORD			wAPPLYNUM;			//	���� ����.
		WORD			wAPPLYANGLE;		//	���� ����.
		WORD			wPIERCENUM;			//	���� ����.
		WORD			wTARNUM;			//	Ÿ�� ����.

		float			fBASIC_VAR;			//	��ȭ��.

		//	���� �Ҹ�-����. ( ������� )
		WORD			wUSE_ARROWNUM;		//	�Ҹ� ȭ�� ����.
		WORD			wUSE_CHARMNUM;		//	�Ҹ� ���� ����.
		WORD			wUSE_BULLETNUM;		//	�Ҹ� ���� ����.

		WORD			wUSE_EXP;			//	���� EXP ������.
		WORD			wUSE_HP;			//	���� HP ������.
		WORD			wUSE_MP;			//	���� MP ������.
		WORD			wUSE_SP;			//	���� SP ������.
		WORD			wUSE_CP;			//	���� CP ������.

		WORD			wUSE_HP_PTY;		//	���� HP ��Ƽ�� ������.
		WORD			wUSE_MP_PTY;		//	���� MP ��Ƽ�� ������.
		WORD			wUSE_SP_PTY;		//	���� SP ��Ƽ�� ������.
	};

	//	���� �� ���뺰 ����.
	struct CDATA_LVL	//	���͸� �߰�
	{
		float			fDELAYTIME;			//	������ Ÿ��.

		float			fLIFE;				//	���ӽð�.
		WORD			wAPPLYRANGE;		//	���� ����.
		WORD			wAPPLYNUM;			//	���� ����. // �̻��;
		WORD			wAPPLYANGLE;		//	���� ����.
		WORD			wPIERCENUM;			//	���� ����.
		WORD			wTARNUM;			//	Ÿ�� ����.

		float			fBASIC_VAR;			//	��ȭ��.

		//	���� �Ҹ�-����. ( ������� )
		WORD			wUSE_ARROWNUM;		//	�Ҹ� ȭ�� ����.
		WORD			wUSE_CHARMNUM;		//	�Ҹ� ���� ����.
		WORD			wUSE_BULLETNUM;		//	�Ҹ� ���� ����.

		WORD			wUSE_EXP;			//	���� EXP ������.
		WORD			wUSE_HP;			//	���� HP ������.
		WORD			wUSE_MP;			//	���� MP ������.
		WORD			wUSE_SP;			//	���� SP ������.
		WORD			wUSE_CP;			//	���� CP ������.
		INT				nUSE_BATTERY;		//	���� ���͸� ������.

		WORD			wUSE_HP_PTY;		//	���� HP ��Ƽ�� ������.
		WORD			wUSE_MP_PTY;		//	���� MP ��Ƽ�� ������.
		WORD			wUSE_SP_PTY;		//	���� SP ��Ƽ�� ������.

		void Assign ( CDATA_LVL_100 &sOldData );
		void Assign ( CDATA_LVL_101 &sOldData );
		void Assign ( CDATA_LVL_102 &sOldData );
		void Assign ( CDATA_LVL_103 &sOldData );

		CDATA_LVL (void);
	};

	struct SSPEC_CA
	{
		// SSPEC���η� ������ ������ �ݰ�, ��ų ȿ�� �ݰ�, �̷��� ���� ��ų ȿ���� �����°� �´°Ͱ���;
		// �Ʒ� ���� �÷��׷� ���Եǰ� �迭�� ���� �����ϴ��� NID�� ��ġ������;
		bool bDamageApply;
		bool bAllApply;

		EMSPEC_ADDON		emSPEC[MAX_ADDON];

		SSPEC_CA();

		void resetemSpec();
		void AllReset();
	};

	struct SSPEC_CA_ADDON_100
	{
		SKILL::SSPEC_CA   sSPEC_CA[MAX_LEVEL_BEFORE];
	};

	struct SSPEC_CA_ADDON
	{
		SKILL::SSPEC_CA   sSPEC_CA[MAX_LEVEL];

		void Assign ( SSPEC_CA_ADDON_100 &sOldData );

		void resetemSpec();
		void AllReset();
	};
};