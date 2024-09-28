#pragma once

#include "./GLSkillDef.h"
#include "../Character/GLCharDefine.h"

namespace SKILL
{
	struct SBASIC_100
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		glold::GLITEM_ATT_102	emUSE_LITEM;			//	��밡�� �޼� ���� �Ӽ�.
		glold::GLITEM_ATT_102	emUSE_RITEM;			//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_101
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		glold::GLITEM_ATT_102	emUSE_LITEM;			//	��밡�� �޼� ���� �Ӽ�.
		glold::GLITEM_ATT_102	emUSE_RITEM;			//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_102
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		glold_103::GLITEM_ATT_103	emUSE_LITEM;			//	��밡�� �޼� ���� �Ӽ�.
		glold_103::GLITEM_ATT_103	emUSE_RITEM;			//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_103
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_105
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.
		DWORD				dwMAXLEVEL;					//	�ִ� ����.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_106	//	������ ���� ���� ����
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.
		DWORD				dwGRADE;					//	���.
		DWORD				dwMAXLEVEL;					//	�ִ� ����.

		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.

		bool				bLearnView;					//  GM�� ���̴� ��ų
	};


	struct SBASIC_107	//	��ųŸ�� �ο�
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )
		bool				bLearnView;					//  GM�� ���̴� ��ų

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.

		DWORD				dwMAXLEVEL;					//	�ִ� ����.
		DWORD				dwGRADE;					//	���.

		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )
		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_108	//	����Ÿ�� ����
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )
		bool				bLearnView;					//  GM�� ���̴� ��ų
		bool				bNonEffectRemove;				//	ȿ������ �Ұ� �ɼ�

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.

		DWORD				dwMAXLEVEL;					//	�ִ� ����.
		DWORD				dwGRADE;					//	���.

		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )
		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMACTION_TYPE		emACTION_TYPE;				//	��ų �ൿŸ�� ( �Ϲ�,����,�����,����Ʈ)

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&

		glold_104::GLITEM_ATT_104			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		glold_104::GLITEM_ATT_104			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.
	};

	struct SBASIC_109
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )
		bool				bLearnView;					//  GM�� ���̴� ��ų
		bool				bNonEffectRemove;			//	ȿ������ �Ұ� �ɼ�

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.

		DWORD				dwMAXLEVEL;					//	�ִ� ����.
		DWORD				dwGRADE;					//	���.

		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )
		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMACTION_TYPE		emACTION_TYPE;				//	��ų �ൿŸ�� ( �Ϲ�,����,�����,����Ʈ)

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&

		glold_105::GLSKILL_ATT_105		emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		glold_105::GLSKILL_ATT_105		emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.

		SBASIC_109 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_105::SKILLATT_NOCARE),
			emUSE_RITEM(glold_105::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC_110
	{
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )
		bool				bLearnView;					//  GM�� ���̴� ��ų
		bool				bNonEffectRemove;			//	ȿ������ �Ұ� �ɼ�
		bool				bMobEffectRate;				//	�� ȿ�� ���� ���� �ɼ�. by luxes.

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.

		DWORD				dwMAXLEVEL;					//	�ִ� ����.
		DWORD				dwGRADE;					//	���.

		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )
		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMACTION_TYPE		emACTION_TYPE;				//	��ų �ൿŸ�� ( �Ϲ�,����,�����,����Ʈ)

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&

		glold_105::GLSKILL_ATT_105		emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		glold_105::GLSKILL_ATT_105		emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.

		SBASIC_110 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_105::SKILLATT_NOCARE),
			emUSE_RITEM(glold_105::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};


#define FLAG_SB_RUNNING_CAST                    0x00000001  // �̵��ϸ鼭 ��ų���� ���ɿ���;
#define FLAG_SB_EFFECT_FORCE_VISIBLE            0x00010000  // ���� ��ǥ�� �ɼǽÿ��� ������ ������;

	struct SBASIC_111   // �ظ�, ����Ǿ�, ���̵� ���� ���� �߰��� ���� ������ ��ȭ.
	{
		enum { VERSION = 0x0111 };
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )
		bool				bLearnView;					//  GM�� ���̴� ��ų
		bool				bNonEffectRemove;			//	ȿ������ �Ұ� �ɼ�
		bool				bMobEffectRate;				//	�� ȿ�� ���� ���� �ɼ�. by luxes.

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.

		DWORD				dwMAXLEVEL;					//	�ִ� ����.
		DWORD				dwGRADE;					//	���.

		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )
		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMACTION_TYPE		emACTION_TYPE;				//	��ų �ൿŸ�� ( �Ϲ�,����,�����,����Ʈ)

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&

		GLSKILL_ATT			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		GLSKILL_ATT			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.


		SBASIC_111 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(SKILLATT_NOCARE),
			emUSE_RITEM(SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}	
	};

	struct SBASIC_112   // �ظ�, ����Ǿ�, ���̵� ���� ���� �߰��� ���� ������ ��ȭ.
	{
		enum { VERSION = 0x0112 };
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )
		bool				bLearnView;					//  GM�� ���̴� ��ų
		bool				bNonEffectRemove;			//	ȿ������ �Ұ� �ɼ�
		bool				bMobEffectRate;				//	�� ȿ�� ���� ���� �ɼ�. by luxes.

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.

		DWORD				dwMAXLEVEL;					//	�ִ� ����.
		DWORD				dwGRADE;					//	���.

		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )
		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMACTION_TYPE		emACTION_TYPE;				//	��ų �ൿŸ�� ( �Ϲ�,����,�����,����Ʈ)

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&

		glold_106::GLSKILL_ATT_106			emUSE_LITEM;	//	��밡�� �޼� ���� �Ӽ�.
		glold_106::GLSKILL_ATT_106			emUSE_RITEM;	//	��밡�� ������ ���� �Ӽ�.

		DWORD				dwFlags;					//

		SBASIC_112 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_106::SKILLATT_NOCARE),
			emUSE_RITEM(glold_106::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false),
			dwFlags(0)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC_113  // �ذ��۷��� ���� �߰��� ���� ������ ��ȭ.
	{
		enum { VERSION = 0x0113 };
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )
		bool				bLearnView;					//  GM�� ���̴� ��ų
		bool				bNonEffectRemove;			//	ȿ������ �Ұ� �ɼ�
		bool				bMobEffectRate;				//	�� ȿ�� ���� ���� �ɼ�. by luxes.

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.

		DWORD				dwMAXLEVEL;					//	�ִ� ����.
		DWORD				dwGRADE;					//	���.

		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )
		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMACTION_TYPE		emACTION_TYPE;				//	��ų �ൿŸ�� ( �Ϲ�,����,�����,����Ʈ)

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&

		glold_107::GLSKILL_ATT_107		emUSE_LITEM;		//	��밡�� �޼� ���� �Ӽ�.
		glold_107::GLSKILL_ATT_107		emUSE_RITEM;	//	��밡�� ������ ���� �Ӽ�.		

		DWORD				dwFlags;					//

		SBASIC_113 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_107::SKILLATT_NOCARE),
			emUSE_RITEM(glold_107::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false),
			dwFlags(0)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC_114  // ������ ���� �߰��� ���� ������ ��ȭ;
	{
		enum { VERSION = 0x0114 };
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )
		bool				bLearnView;					//  GM�� ���̴� ��ų
		bool				bNonEffectRemove;			//	ȿ������ �Ұ� �ɼ�
		bool				bMobEffectRate;				//	�� ȿ�� ���� ���� �ɼ�. by luxes.

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.

		DWORD				dwMAXLEVEL;					//	�ִ� ����.
		DWORD				dwGRADE;					//	���.

		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )
		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMACTION_TYPE		emACTION_TYPE;				//	��ų �ൿŸ�� ( �Ϲ�,����,�����,����Ʈ)

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&

		glold_108::GLSKILL_ATT_108 	emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		glold_108::GLSKILL_ATT_108	emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.

		DWORD				dwFlags;					//

		SBASIC_114 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(glold_108::SKILLATT_NOCARE),
			emUSE_RITEM(glold_108::SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false),
			dwFlags(0)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC_115  // ������ ���� �߰��� ���� ������ ��ȭ;
	{
		enum { VERSION = 0x0115 };
		SNATIVEID			sNATIVEID;					//	���� ID. ( ���� �����ÿ� �ο��Ǵ� ���� ID )
		bool				bLearnView;					//  GM�� ���̴� ��ų
		bool				bNonEffectRemove;			//	ȿ������ �Ұ� �ɼ�
		bool				bMobEffectRate;				//	�� ȿ�� ���� ���� �ɼ�. by luxes.

		char				szNAME[MAX_SZNAME];			//	��ų�� �̸�.

		DWORD				dwMAXLEVEL;					//	�ִ� ����.
		DWORD				dwGRADE;					//	���.

		EMAPPLY				emAPPLY;					//	��ų ����. ( ��������, ��������, ���� ���� )
		EMROLE				emROLE;						//	��ų ���� Ư��. ( ���, ����, ��ú� )
		EMACTION_TYPE		emACTION_TYPE;				//	��ų �ൿŸ�� ( �Ϲ�,����,�����,����Ʈ)

		WORD				wTARRANGE;					//	��ǥ ���� �Ÿ�.

		//	Skill �� ���� �޴� ��ǥ.
		EMIMPACT_SIDE		emIMPACT_SIDE;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )
		EMIMPACT_TARGET		emIMPACT_TAR;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
		EMIMPACT_REALM		emIMPACT_REALM;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&

		GLSKILL_ATT			emUSE_LITEM;				//	��밡�� �޼� ���� �Ӽ�.
		GLSKILL_ATT			emUSE_RITEM;				//	��밡�� ������ ���� �Ӽ�.

		DWORD				dwFlags;					//

		SBASIC_115 (void) :
		dwGRADE(0),
			dwMAXLEVEL(9),

			emROLE(EMROLE_NORMAL),
			emAPPLY(EMAPPLY_MAGIC),
			emACTION_TYPE(EMACTION_TYPE_NORMAL),

			emIMPACT_TAR(TAR_SPEC),
			emIMPACT_REALM(REALM_SELF),
			emIMPACT_SIDE(EMIMPACT_SIDE_ENEMY),

			wTARRANGE(50),

			emUSE_LITEM(SKILLATT_NOCARE),
			emUSE_RITEM(SKILLATT_NOCARE),
			bLearnView(false),
			bNonEffectRemove(false),
			bMobEffectRate(false),
			dwFlags(0)
		{
			SecureZeroMemory ( szNAME, sizeof(char)*MAX_SZNAME );
		}
	};

	struct SBASIC : public SBASIC_115
	{
		// �̰����� ���� ���� ���� �� ��;
		// ���� ���� �� ������ ���� �ҷ����� �� ���� ����;
		SBASIC(){}

		void Assign( SBASIC_100& sOldData );
		void Assign( SBASIC_101& sOldData );
		void Assign( SBASIC_102& sOldData );
		void Assign( SBASIC_103& sOldData );
		void Assign( SBASIC_105& sOldData );
		void Assign( SBASIC_106& sOldData );
		void Assign( SBASIC_107& sOldData );
		void Assign( SBASIC_108& sOldData );
		void Assign( SBASIC_109& sOldData );
		void Assign( SBASIC_110& sOldData );
		void Assign( SBASIC_111& sOldData );
		void Assign( SBASIC_112& sOldData );
		void Assign( SBASIC_113& sOldData );
		void Assign( SBASIC_114& sOldData );

		//! ���� ID. (���� �����ÿ� �ο��Ǵ� ���� ID)
		inline SNATIVEID GetId() const { return sNATIVEID; }

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		//! "��" (�� ������ �ڱ���, �� ������ �����)
		inline EMIMPACT_SIDE ImpactSide() const { return emIMPACT_SIDE; }
	}; //struct SBASIC
};