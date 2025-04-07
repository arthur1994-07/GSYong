#ifndef _GL_ITEM_SUIT_H_
#define _GL_ITEM_SUIT_H_

#pragma once

#include "./GLItemDef.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}

namespace ITEM
{
    GLITEM_ATT assign_att ( glold::GLITEM_ATT_102 emOld );
    GLITEM_ATT assign_att ( glold_103::GLITEM_ATT_103 emOld );
    GLITEM_ATT assign_att ( glold_104::GLITEM_ATT_104 emOld );
	GLITEM_ATT assign_att ( glold_105::GLITEM_ATT_105 emOld );
	GLITEM_ATT assign_att ( glold_108::GLITEM_ATT_108 emOld );

    struct SSATE_BLOW_100
    {
        EMSTATE_BLOW	emTYPE;		//	���� Ÿ��.
        float			fRATE;		//	�߻� Ȯ��.
        float			fLIFE;		//	���� �ð�.
        float			fVALUE;		//	���� ��.
    };

    struct SSATE_BLOW
    {
        EMSTATE_BLOW	emTYPE;		//	���� Ÿ��.
        float			fRATE;		//	�߻� Ȯ��.
        float			fLIFE;		//	���� �ð�.
        float			fVAR1;		//	���� �� 1.
        float			fVAR2;		//	���� �� 2.

        SSATE_BLOW ()
            : emTYPE(EMBLOW_NONE)
            , fRATE(0.0f)
            , fLIFE(0.0f)
            , fVAR1(0.0f)
            , fVAR2(0.0f)
        {
        }
    };

    enum	EMHAND
    {
        EMHAND_BOTHHAND		= 0x0001,
        EMHAND_BIG			= 0x0002,
        EMHAND_BROOM		= 0x0004,
    };

    struct SSUIT_100
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        BOOL		bBothHand;					//	��� ��� ��������.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        glold::GLITEM_ATT_102	emAttack;					//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        SRESIST		sResist;					//	���װ�.

        WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

        //	Ư�� �ΰ� ȿ����.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SSATE_BLOW_100	sBLOW;					//	���� �̻� ȿ��.
    };

    struct SSUIT_101
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        BOOL		bBothHand;					//	��� ��� ��������.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        glold::GLITEM_ATT_102	emAttack;					//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        SRESIST		sResist;					//	���װ�.

        WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

        //	Ư�� �ΰ� ȿ����.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
    };

    struct SSUIT_102
    {
        enum { ADDON_SIZE = 4 };
        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        bool		bBothHand;					//	��� ��� ��������.
        bool		bBig;						//	����.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        glold::GLITEM_ATT_102	emAttack;		//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.
        WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        SRESIST		sResist;					//	���װ�.

        WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

        //	Ư�� �ΰ� ȿ����.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
    };

    struct SSUIT_103
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        bool		bBothHand;					//	��� ��� ��������.
        bool		bBig;						//	����.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        glold_103::GLITEM_ATT_103	emAttack;	//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.
        WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        SRESIST		sResist;					//	���װ�.

        WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

        //	Ư�� �ΰ� ȿ����.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
    };

    struct SSUIT_104
    {
        enum { ADDON_SIZE = 4 };
        EMSUIT		emSuit;
        bool		bBothHand;
        bool		bBig;
        EMITEM_HAND	emHand;

        glold_104::GLITEM_ATT_104	emAttack;
        WORD		wAttRange;
        WORD		wReqSP;

        short		nHitRate;
        short		nAvoidRate;

        GLPADATA	gdDamage;
        short		nDefense;

        SRESIST_101	sResist;

        WORD		wReModelNum;

        SADDON		sADDON[ADDON_SIZE];
        SVAR		sVARIATE;
        SSATE_BLOW	sBLOW;
    };

    struct SSUIT_105
    {
        enum { ADDON_SIZE = 4 };
        EMSUIT		emSuit;
        bool		bBothHand;
        bool		bBig;
        EMITEM_HAND	emHand;

        glold_104::GLITEM_ATT_104	emAttack;
        WORD		wAttRange;
        WORD		wReqSP;

        short		nHitRate;
        short		nAvoidRate;

        GLPADATA	gdDamage;
        short		nDefense;

        SRESIST		sResist;

        WORD		wReModelNum;				//	���� ���� Ƚ��.		( '0' ���� �Ұ� )

        //	Ư�� �ΰ� ȿ����.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
    };

    struct SSUIT_106
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        DWORD		dwHAND;						//	�� ���� �Ӽ�.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        glold_104::GLITEM_ATT_104	emAttack;	//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.
        WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        SRESIST		sResist;					//	���װ�.

        //	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
        //	���, �� �����ۿ����� ������ ��ȣ.
        WORD		wReModelNum;

        //	Ư�� �ΰ� ȿ����.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
    };

    struct SSUIT_107	//	������ ���� ���� ����
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        DWORD		dwHAND;						//	�� ���� �Ӽ�.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        glold_104::GLITEM_ATT_104	emAttack;	//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.
        WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        SRESIST		sResist;					//	���װ�.

        //	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
        //	���, �� �����ۿ����� ������ ��ȣ.
        WORD		wReModelNum;

        //	Ư�� �ΰ� ȿ����.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SVOL		sVOLUME;					//  ��ȭ�� ȿ��. By ���
        SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
    };

    struct SSUIT_108	//	����Ÿ�� ��������
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        DWORD		dwHAND;						//	�� ���� �Ӽ�.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        SRESIST		sResist;					//	���װ�.

        glold_104::GLITEM_ATT_104	emAttack;	//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.
        WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

        //	Ư�� �ΰ� ȿ����.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SVOL		sVOLUME;					//  ��ȭ�� ȿ��. By ���
        SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.

        //	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
        //	���, �� �����ۿ����� ������ ��ȣ.
        WORD		wReModelNum;
    };

    //	���� ������, ���� ������.
    struct SSUIT_109 // By ���
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        DWORD		dwHAND;						//	�� ���� �Ӽ�.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        SRESIST		sResist;					//	���װ�.

        GLITEM_ATT	emAttack;					//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.
        WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

        //	Ư�� �ΰ� ȿ����.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SVOL		sVOLUME;					//  ��ȭ�� ȿ��. By ���
        SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.

        //	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
        //	���, �� �����ۿ����� ������ ��ȣ.
        WORD		wReModelNum;
    };

    struct SSUIT_110 // ���� ���� ��� �߰�. by luxes.
    {
        enum { ADDON_SIZE = 4 };

        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        DWORD		dwHAND;						//	�� ���� �Ӽ�.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        SRESIST		sResist;					//	���װ�.

        GLITEM_ATT	emAttack;	//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.
        WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

        //	Ư�� �ΰ� ȿ����.
        SVAR		sVARIATE;					//	��ȭ�� ȿ��.
        SVOL		sVOLUME;					//  ��ȭ�� ȿ��. By ���
        SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
        SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.

        //	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
        //	���, �� �����ۿ����� ������ ��ȣ.
        WORD		wReModelNum;

        bool		bChangeColor;				// �ڽ�Ƭ ���� ����. by luxes.

    };

	struct SSUIT_111 // ������ �߰�
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		DWORD		dwHAND;						//	�� ���� �Ӽ�.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		SRESIST		sResist;					//	���װ�.

		GLITEM_ATT	emAttack;	//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.
		WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

		//	Ư�� �ΰ� ȿ����.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SVOL		sVOLUME;					//  ��ȭ�� ȿ��. By ���
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.

		//	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
		//	���, �� �����ۿ����� ������ ��ȣ.
		WORD		wReModelNum;

		bool		bChangeColor;				// �ڽ�Ƭ ���� ����. by luxes.

		// ������
		WORD        wDurabilityMax;             //! �ִ� ������ (max) [ 0 == ������ ������ ]
		DWORD       dwRepairPrice;              //! �������� (1p)
	};

	struct SSUIT_112 // ������ �߰�
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		DWORD		dwHAND;						//	�� ���� �Ӽ�.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		SRESIST		sResist;					//	���װ�.

		glold_108::GLITEM_ATT_108	emAttack;	//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.
		WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

		//	Ư�� �ΰ� ȿ����.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SVOL		sVOLUME;					//  ��ȭ�� ȿ��. By ���
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.

		//	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
		// ���� ���� ����;
		//	���, �� �����ۿ����� ������ ��ȣ.
		WORD		wReModelNum;

		unsignedState nItemSuitState; // ������ ����(GLItemDef.h/EMITEM_SUIT_STATE);

		// ������
		WORD        wDurabilityMax;             //! �ִ� ������ (max) [ 0 == ������ ������ ]
		DWORD       dwRepairPrice;              //! �������� (1p)
	};

	struct SSUIT_113
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		DWORD		dwHAND;						//	�� ���� �Ӽ�.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		SRESIST		sResist;					//	���װ�.

		GLITEM_ATT	emAttack;	//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.
		WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

		//	Ư�� �ΰ� ȿ����.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SVOL		sVOLUME;					//  ��ȭ�� ȿ��. By ���
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.

		//	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
		// ���� ���� ����;
		//	���, �� �����ۿ����� ������ ��ȣ.
		WORD		wReModelNum;

		unsignedState nItemSuitState; // ������ ����(GLItemDef.h/EMITEM_SUIT_STATE);

		// ������
		WORD        wDurabilityMax;             //! �ִ� ������ (max) [ 0 == ������ ������ ]
		DWORD       dwRepairPrice;              //! �������� (1p)
	};

	struct SSUIT_114
	{
		enum { ADDON_SIZE = 4 };

		EMSUIT		emSuit;						//	������ ���� Ÿ��.
		DWORD		dwHAND;						//	�� ���� �Ӽ�.
		EMITEM_HAND	emHand;						//	���� �� ��ġ.

		GLPADATA	gdDamage;					//	�����. ( +/- )
		short		nDefense;					//	���. ( +/- )

		short		nHitRate;					//	������. ( +/- ) (%)
		short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

		SRESIST		sResist;					//	���װ�.

		GLITEM_ATT	emAttack;	//	���ݼӼ�.
		WORD		wAttRange;					//	���� ���� �Ÿ�.
		WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

		//	Ư�� �ΰ� ȿ����.
		SVAR		sVARIATE;					//	��ȭ�� ȿ��.
		SVOL		sVOLUME;					//  ��ȭ�� ȿ��. By ���
		SSATE_BLOW	sBLOW;						//	���� �̻� ȿ��.
		SADDON		sADDON[ADDON_SIZE];			//	���� ȿ��.

		//	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
		// ���� ���� ����;
		//	���, �� �����ۿ����� ������ ��ȣ.
		WORD		wReModelNum;

		unsignedState nItemSuitState; // ������ ����(GLItemDef.h/EMITEM_SUIT_STATE);

		// ������
		WORD        wDurabilityMax;             //! �ִ� ������ (max) [ 0 == ������ ������ ]
		DWORD       dwRepairPrice;              //! �������� (1p)

		// �⺻ �ɷ�ġ ���� ��Ʈ �Ŵ��� Index;
		int			nBasicAttackDamageIndex;
		int			nBasicDefenceIndex;

		// �⺻ �ɷ�ġ �������� ����;
		bool		bEditableBasicAttackDamage;
		bool		bEditableBasicDefence;
		SSUIT_114 ()
			: emSuit(SUIT_HEADGEAR)
			, dwHAND(NULL)
			, emHand(HAND_RIGHT)

			, emAttack(ITEMATT_NOTHING)
			, wAttRange(4)
			, wReqSP(0)

			, nHitRate(0)
			, nAvoidRate(0)

			, gdDamage(0,0)
			, nDefense(0)

			, wReModelNum(0)

			, wDurabilityMax(0)
			, dwRepairPrice(0)

			, nBasicAttackDamageIndex(-1)
			, nBasicDefenceIndex(-1)

			, bEditableBasicAttackDamage(false)
			, bEditableBasicDefence(false)
		{
		}

		// 115 ���� ���ķδ� SADDON, sVARIATE, sVOLUME�� SADDON�ϳ��� ��ģ��;
		// ��ǥ�������� sVARIATE, sVOLUME�� ��������̱⿡ 114������ ��������;
		// ���� �������� �����ý�Ų�� �ٽ� �ֽŹ������� ������ ��Ų��;
		void Assign ( SSUIT_100 &Suit100 );
		void Assign ( SSUIT_101 &Suit101 );
		void Assign ( SSUIT_102 &Suit102 );
		void Assign ( SSUIT_103 &Suit103 );
		void Assign ( SSUIT_104 &Suit104 );
		void Assign ( SSUIT_105 &Suit105 );
		void Assign ( SSUIT_106 &Suit106 );
		void Assign ( SSUIT_107 &Suit107 );
		void Assign ( SSUIT_108 &Suit108 );
		void Assign ( SSUIT_109 &Suit109 );
		void Assign ( SSUIT_110 &sSuitOld );
		void Assign ( SSUIT_111 &sSuitOld );
		void Assign ( SSUIT_112 &sSuitOld );
		void Assign ( SSUIT_113& sSuitOld );

		void SETBOTHHAND ( bool bBOTH )		{ (bBOTH) ? (dwHAND|=EMHAND_BOTHHAND) : (dwHAND&=~EMHAND_BOTHHAND); }
		void SETBIG ( bool bBIG )			{ (bBIG) ? (dwHAND|=EMHAND_BIG) : (dwHAND&=~EMHAND_BIG); }
	};

	struct SSUIT_115
	{
		enum { ADDON_SIZE = 6, STRING_LENGTH = 64, };

		EMSUIT			emSuit;
		DWORD			dwHAND;
		EMITEM_HAND		emHand;

		GLPADATA		gdDamage;
		short			nDefense;

		short			nHitRate;
		short			nAvoidRate;

		SRESIST			sResist;

		GLITEM_ATT		emAttack;
		WORD			wAttRange;
		WORD			wReqSP;

		SSATE_BLOW		sBLOW;
		SADDON			sADDON[ ADDON_SIZE ];
		char			szRandomAddonScript[ STRING_LENGTH ];

		WORD			wReModelNum;

		unsignedState	nItemSuitState;

		WORD			wDurabilityMax;
		DWORD			dwRepairPrice;

		int				nBasicAttackDamageIndex;
		int				nBasicDefenceIndex;

		bool			bEditableBasicAttackDamage;
		bool			bEditableBasicDefence;
	};

    struct SSUIT // ������ �߰�
    {
        enum { ADDON_SIZE = 6, VERSION = 0x0116, STRING_LENGTH = 64, };

        EMSUIT		emSuit;						//	������ ���� Ÿ��.
        DWORD		dwHAND;						//	�� ���� �Ӽ�.
        EMITEM_HAND	emHand;						//	���� �� ��ġ.

        GLPADATA	gdDamage;					//	�����. ( +/- )
        short		nDefense;					//	���. ( +/- )

        short		nHitRate;					//	������. ( +/- ) (%)
        short		nAvoidRate;					//	ȸ����. ( +/- ) (%)

        SRESIST		sResist;					//	���װ�.

        GLITEM_ATT	emAttack;	//	���ݼӼ�.
        WORD		wAttRange;					//	���� ���� �Ÿ�.
        WORD		wReqSP;						//	���ݽ� �ʿ��� SP.

        //	Ư�� �ΰ� ȿ����.
//		SVAR		sVARIATE;						//	��ȭ�� ȿ��.
//		SVOL		sVOLUME;						//  ��ȭ�� ȿ��. By ���
        SSATE_BLOW	sBLOW;							//	���� �̻� ȿ��.
        SADDON		sADDON[ADDON_SIZE];				//	���� ȿ��.
		char		szRandomAddonScript[STRING_LENGTH];	//	���� ����ȿ�� ��������;

        //	���� ���� Ƚ��.		( '0' ���� �Ұ� ),
		// ���� ���� ����;
        //	���, �� �����ۿ����� ������ ��ȣ.
        WORD		wReModelNum;

		unsignedState nItemSuitState; // ������ ����(GLItemDef.h/EMITEM_SUIT_STATE);

        // ������
        WORD        wDurabilityMax;             //! �ִ� ������ (max) [ 0 == ������ ������ ]
        DWORD       dwRepairPrice;              //! �������� (1p)

		// �⺻ �ɷ�ġ ���� ��Ʈ �Ŵ��� Index;
		int			nBasicAttackDamageType;
		int			nBasicAttackDamageIndex;

		int			nBasicDefenceType;
		int			nBasicDefenceIndex;

		// �⺻ �ɷ�ġ �������� ����;
		bool		bEditableBasicAttackDamage;
		bool		bEditableBasicDefence;

    SSUIT ()
            : emSuit(SUIT_HEADGEAR)
            , dwHAND(NULL)
            , emHand(HAND_RIGHT)

            , emAttack(ITEMATT_NOTHING)
            , wAttRange(4)
            , wReqSP(0)

            , nHitRate(0)
            , nAvoidRate(0)

            , gdDamage(0,0)
            , nDefense(0)

            , wReModelNum(0)
            
            , wDurabilityMax(0)
            , dwRepairPrice(0)

			, nBasicAttackDamageType(-1)
			, nBasicAttackDamageIndex(-1)

			, nBasicDefenceType(-1)
			, nBasicDefenceIndex(-1)

			, bEditableBasicAttackDamage(false)
			, bEditableBasicDefence(false)
        {
			memset(szRandomAddonScript, 0, sizeof(char) * STRING_LENGTH);
        }

		void Assign ( SSUIT_100 &Suit100 );
		void Assign ( SSUIT_101 &Suit101 );
		void Assign ( SSUIT_102 &Suit102 );
		void Assign ( SSUIT_103 &Suit103 );
		void Assign ( SSUIT_104 &Suit104 );
		void Assign ( SSUIT_105 &Suit105 );
		void Assign ( SSUIT_106 &Suit106 );
		void Assign ( SSUIT_107 &Suit107 );
		void Assign ( SSUIT_108 &Suit108 );
		void Assign ( SSUIT_109 &Suit109 );
		void Assign ( SSUIT_110 &sSuitOld );
		void Assign ( SSUIT_111 &sSuitOld );
		void Assign ( SSUIT_112 &sSuitOld );
		void Assign ( SSUIT_113& sSuitOld );
		void Assign ( SSUIT_114& sSuitOld );
		// 0x115���� ���� �����͵��� SADDON, SVAR, SVOL�� SADDON �ϳ��� ��ģ��;
		// �Ϲ��� �ε�(���� Assign) ���Ŀ� �̷�������Ѵ�;
		void Assign ( SSUIT_115& sSuitOld );



        static VOID SaveCsvHead ( std::fstream &SFile );
        VOID SaveCsv ( std::fstream &SFile );
        VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );

        void SETBOTHHAND ( bool bBOTH )		{ (bBOTH) ? (dwHAND|=EMHAND_BOTHHAND) : (dwHAND&=~EMHAND_BOTHHAND); }
        void SETBIG ( bool bBIG )			{ (bBIG) ? (dwHAND|=EMHAND_BIG) : (dwHAND&=~EMHAND_BIG); }
        void SETBROOM ( bool bBROOM )		{ (bBROOM) ? (dwHAND|=EMHAND_BROOM) : (dwHAND&=~EMHAND_BROOM); }
		void SETSTATE(const DWORD nState, const bool bOn) { bOn ? nItemSuitState += nState : nItemSuitState -= nState; }		

        const bool IsBOTHHAND () const		{ return NULL!=(dwHAND&EMHAND_BOTHHAND); }
        const bool IsBIG () const				{ return NULL!=(dwHAND&EMHAND_BIG); }
        const bool IsBROOM () const			{ return NULL!=(dwHAND&EMHAND_BROOM); }
		const bool IsSTATE(const DWORD nState)	const { return nItemSuitState[nState]; }
		const bool IsStatEnable() const;
		const bool IsNone() const;

        //! ������ ���� Ÿ��
        inline EMSUIT Suit() const { return emSuit; }

		// �⺻ ���ݷ�, ����;
		inline const GLPADATA GetAttackDamage ( void ) const { return gdDamage; }
		inline const WORD GetDefence ( void ) const { return nDefense; }

		// �⺻ ���ݷ�, ���� ���� ��Ʈ �Ŵ��� Index;
		inline const int GetBasicAttackDamageType ( void ) const { return nBasicAttackDamageType; }
		inline const int GetBasicAttackDamageIndex ( void ) const { return nBasicAttackDamageIndex; }

		inline const int GetBasicDefenceType ( void ) const { return nBasicDefenceType; }
		inline const int GetBasicDefenceIndex ( void ) const { return nBasicDefenceIndex; }

		// �⺻ ���ݷ�, ���� �������� ����;
		inline const bool IsEditableBasicAttackDamage ( void ) const { return bEditableBasicAttackDamage && ( -1 != GetBasicAttackDamageIndex() ) && ( -1 != GetBasicAttackDamageType() ); }
		inline const bool IsEditableBasicDefence ( void ) const { return bEditableBasicDefence && ( -1 != GetBasicDefenceIndex() ) && ( -1 != GetBasicDefenceType() ); }

		// �ش� Addon�� �������ִ°�?;
		const bool IsHaveAddon( IN const EMITEM_ADDON emAddon, OUT int& nIdx ) const;
		// res:���翩��, �ش� Addon �� ��� ����;
		const bool GetHaveAddonValue_float( IN const EMITEM_ADDON emAddon, OUT float& fResult ) const;
		const bool GetHaveAddonValue_int( IN const EMITEM_ADDON emAddon, OUT INT& nResult ) const;

		/// Datatyoe �� �ٲ� ��츦 ����� �������ش�;
		/// ������ �ʿ������ ���� ������ �ȴ°��� �� ���� ����̴�;
		/// �̹� �̷��� �����Ǿ����� Datatype �� ��ȯ�� ���� ���׼������Ŀ��� ���� �����Ƿ�;
		/// �״�� �������;

		/// �Ʒ� �Լ����� GLItemCustom_Addon �� �ִ� GetValue Function �� ���� �����Ǿ�� �Ѵ�;
		/// �ߺ��ڵ尡 ������ ������ ���� ���� ���� �κ��̶� �׳� �ߺ����״�
		template < typename T >
		const T GetAddonValue_CompareExchange ( const int _nIndex, const bool _bInteger )
		{
			if ( _nIndex < 0 || ADDON_SIZE <= _nIndex )
				return static_cast< const T >( 0 );

			int nValue = 0;
			float fValue = 0.f;
			if ( _bInteger )
			{
				nValue = sADDON[ _nIndex ].nVALUE;
				if ( 100000 <= nValue || nValue <= -100000 )
					nValue = static_cast< int >( sADDON[ _nIndex ].fVALUE );

				if ( 100000 <= nValue || nValue <= -100000 )
					nValue = 0;

				if ( sADDON[ _nIndex ].nVALUE != nValue )
					sADDON[ _nIndex ].nVALUE = nValue;
			}
			else
			{
				fValue = sADDON[ _nIndex ].fVALUE;
				if ( 100000.f <= fValue || fValue <= -100000.f )
					fValue = static_cast< float >( sADDON[ _nIndex ].nVALUE );

				if ( 100000.f <= fValue || fValue <= -100000.f )
					fValue = 0.f;

				if ( sADDON[ _nIndex ].fVALUE != fValue )
					sADDON[ _nIndex ].fVALUE = fValue;
			}

			const T tReturnValue = static_cast< const T >( ( _bInteger ) ? nValue : fValue );

			return tReturnValue;
		}

		// const version;
		template < typename T >
		const T GetAddonValue ( const int _nIndex, const bool _bInteger ) const
		{
			if ( _nIndex < 0 || ADDON_SIZE <= _nIndex )
				return static_cast< const T >( 0 );

			int nValue = 0;
			float fValue = 0.f;
			if ( _bInteger )
			{
				nValue = sADDON[ _nIndex ].nVALUE;
				if ( 100000 <= nValue || nValue <= -100000 )
					nValue = static_cast< int >( sADDON[ _nIndex ].fVALUE );

				if ( 100000 <= nValue || nValue <= -100000 )
					nValue = 0;
			}
			else
			{
				fValue = sADDON[ _nIndex ].fVALUE;
				if ( 100000.f <= fValue || fValue <= -100000.f )
					fValue = static_cast< float >( sADDON[ _nIndex ].nVALUE );

				if ( 100000.f <= fValue || fValue <= -100000.f )
					fValue = 0.f;
			}

			const T tReturnValue = static_cast< const T >( ( _bInteger ) ? nValue : fValue );

			return tReturnValue;
		}
    };

} // namespace ITEM
#endif // _GL_ITEM_SUIT_H_
