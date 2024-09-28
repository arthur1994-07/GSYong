#pragma once

#include "GLContrlBaseMsg.h"
#include "../Character/GLCharDefine.h"
#include "../Item/GLItemDef.h"
#include "../PVE/PVEInDunID.h"

enum EMREQ_BOXOPEN_FB
{
	EMREQ_BOXOPEN_FB_FAIL		= 0,	//	�Ϲݿ���.
	EMREQ_BOXOPEN_FB_OK			= 1,	//	����.
	EMREQ_BOXOPEN_FB_NOITEM		= 2,	//	���� �������� ����.
	EMREQ_BOXOPEN_FB_NOBOX		= 3,	//	���ڰ� �ƴ�.
	EMREQ_BOXOPEN_FB_EMPTY		= 4,	//	���� �ȿ� ���빰�� �����ϴ�.
	EMREQ_BOXOPEN_FB_NOTINVEN	= 5,	//	�κ� ������ ����.
	EMREQ_BOXOPEN_FB_INVALIDITEM= 6,	//	�߸��� �������� ��� ����.
	EMREQ_BOXOPEN_FB_ERR_ACTIVITY		= 7,   // Ư��Ȱ�� ����Ʈ ����
	EMREQ_BOXOPEN_FB_ERR_CONTRIBUTION	= 8,	// �⿩�� ����Ʈ ����
};

enum EMREQ_DISGUISE_FB
{
	EMREQ_DISGUISE_FB_FAIL		= 0,	//	�Ϲݿ���.
	EMREQ_DISGUISE_FB_OK		= 1,	//	����.
	EMREQ_DISGUISE_FB_NOITEM	= 2,	//	�������� ����.
	EMREQ_DISGUISE_FB_NODISGUISE= 3,	//	�ڽ��� �������� ����.
	EMREQ_DISGUISE_FB_NOTSUIT	= 4,	//	������ �ƴϿ��� �Ұ���.
	EMREQ_DISGUISE_FB_DEFSUIT	= 5,	//	�ٸ� ������ ����.
	EMREQ_DISGUISE_FB_ALREADY	= 6,	//	�̹� �ڽ����� �����Ǿ� ����.
	EMREQ_DISGUISE_FB_PILED		= 7,	//	������ �����ۿ��� �ڽ�Ƭ �Ұ�. ( ����īƮ�� �������� �����ѵ� �ش� �����ۿ��� �ڽ�Ƭ ���� �� �����Ƿ� )
	EMREQ_DISGUISE_FB_ERR_ACTIVITY		= 8, // Ư��Ȱ�� ����Ʈ ����
	EMREQ_DISGUISE_FB_ERR_CONTRIBUTION	= 9,	// �⿩�� ����Ʈ ����
};	

enum EMREQ_PERIODEXTENSION_FB
{
	EMREQ_PERIODEX_FB_FAIL = 0,				// �Ϲ� ����;
	EMREQ_PERIODEX_FB_FAIL_DUPLICATION,		// �ڽ�Ƭ, ���������� ��� ���� ������ ��� ����;
	EMREQ_PERIODEX_FB_FAIL_LIMITCOUNT,		// ���� Ƚ�� ����;
	EMREQ_PERIODEX_FB_CHECK_EXPIRATIONDATE,	// ��� �ڽ�Ƭ���� �������� ª�� ��� Ȯ��;
	EMREQ_PERIODEX_FB_SUCCESS,				// ����!;
	EMREQ_PERIODEX_FB_SUCCESS_BASE,
	EMREQ_PERIODEX_FB_SUCCESS_COSTUME,
};

enum EMREQ_CLEANSER_FB
{
	EMREQ_CLEANSER_FB_FAIL		= 0,	//	��Ź�� ����.
	EMREQ_CLEANSER_FB_OK		= 1,	//	��Ź�� ����.
	EMREQ_CLEANSER_FB_NOITEM	= 2,	//	�������� ����.
	EMREQ_CLEANSER_FB_NOCLEANSER= 3,	//	������ �ƴմϴ�.
	EMREQ_CLEANSER_FB_NONEED	= 4,	//	��Ź�� �ʿ� ����.
};

enum EMREQ_RESET_SKST_FB
{
	EMREQ_RESET_SKST_FB_FAIL		= 0,	//	�ʱ�ȭ ����.
	EMREQ_RESET_SKST_FB_OK			= 1,	//	�ʱ�ȭ ����.
	EMREQ_RESET_SKST_FB_NOITEM		= 2,	//	�������� ����.
	EMREQ_RESET_SKST_FB_NOINVEN		= 3,	//	�κ����� ����.
	EMREQ_RESET_SKST_FB_OK2			= 4,	//	��ųâ���� �ʱ�ȭ ����.
};

enum EMREQ_CHARCARD_FB
{
	EMREQ_CHARCARD_FB_FAIL		= 0,	//	�ɸ��� ī�� ��뿡 ����.
	EMREQ_CHARCARD_FB_OK		= 1,	//	�ɸ��� ī�� ��뿡 ����.
	EMREQ_CHARCARD_FB_NOITEM	= 2,	//	�ɸ��� ī�� �������� ����.
};

enum EMREQ_STORAGECARD_FB
{
	EMREQ_STORAGECARD_FB_FAIL		= 0,	//	â�� ī�� ��뿡 ����.
	EMREQ_STORAGECARD_FB_OK			= 1,	//	â�� ī�� ��뿡 ����.
	EMREQ_STORAGECARD_FB_NOITEM		= 2,	//	â�� ī�� �������� ����.
	EMREQ_STORAGECARD_FB_INVNUM		= 3,	//	�߸��� â�� ��ȣ�� ����.
};

enum EMREQ_INVENLINE_FB
{
	EMREQ_INVENLINE_FB_FAIL			= 0,	//	�κ� ī�� ��뿡 ����.
	EMREQ_INVENLINE_FB_OK			= 1,	//	�κ� ī�� ��뿡 ����.
	EMREQ_INVENLINE_FB_NOITEM		= 2,	//	�κ� ī�� �������� ����.
	EMREQ_INVENLINE_FB_MAXLINE		= 3,	//	�� �̻� �߰� ������ �κ�Ȯ�� �Ұ���.
};

enum EMREQ_STORAGEOPEN_FB
{
	EMREQ_STORAGEOPEN_FB_FAIL		= 0,	//	â���� ī�� ��뿡 ����.
	EMREQ_STORAGEOPEN_FB_OK			= 1,	//	â���� ī�� ��뿡 ����.
	EMREQ_STORAGEOPEN_FB_NOITEM		= 2,	//	â���� ī�� �������� ����.
};

enum EMREQ_REMODELOPEN_FB
{
	EMREQ_REMODELOPEN_FB_FAIL		= 0,	//	����ī�� ��뿡 ����.
	EMREQ_REMODELOPEN_FB_OK			= 1,	//	����ī�� ��뿡 ����.
	EMREQ_REMODELOPEN_FB_NOITEM		= 2,	//	����ī�� �������� ����.
	EMREQ_REMODELOPEN_FB_STAMP_ERROR = 3,	//	������ �߸��� ����ī�� ���
};

enum EMREQ_GARBAGEOPEN_FB
{
	EMREQ_GARBAGEOPEN_FB_FAIL		= 0,	//	������ī�� ��뿡 ����.
	EMREQ_GARBAGEOPEN_FB_OK			= 1,	//	������ī�� ��뿡 ����.
	EMREQ_GARBAGEOPEN_FB_NOITEM		= 2,	//	������ī�� �������� ����.
};

enum EMREQ_PMARKET_SEARCH_CARD_FB
{
    EMREQ_PMARKET_SEARCH_OPEN_FB_FAIL     = 0,  //  ���λ��� �˻� ī�� ��뿡 ����;
    EMREQ_PMARKET_SEARCH_OPEN_FB_OK       = 1,  //  ���λ��� �˻� ī�� ��뿡 ����;
    EMREQ_PMARKET_SEARCH_OPEN_FB_NOITEM   = 2,  //  ���λ��� ���� ī�尡 �����ϴ�.;
    EMREQ_PMARKET_SEARCH_OPEN_FB_ALREADY  = 3,  //  ���λ��� �˻� ī�带 �̹� ������Դϴ�.;
    EMREQ_PMARKET_SEARCH_OPEN_FB_BLOCK    = 4,  //  ���λ��� �˻��� �Ұ����� �����Դϴ�.;
};

enum EMGARBAGE_RESULT_FB
{
	EMGARBAGE_RESULT_FB_FAIL		= 0,	//	������ ������ ����.
	EMGARBAGE_RESULT_FB_OK			= 1,	//	������ ��뿡 ����.
	EMGARBAGE_RESULT_FB_NOITEM		= 2,	//	�������� ����.
	EMGARBAGE_RESULT_FB_ITEMTYPE	= 3,	//	�����Ҽ� ���� �������Դϴ�.
	EMGARBAGE_RESULT_FB_MONEY		= 4,	//	�������� �����մϴ�.
};

enum EMREQ_PREMIUMSET_FB
{
	EMREQ_PREMIUMSET_FB_FAIL		= 0,	//	�����̾��� ��뿡 ����.
	EMREQ_PREMIUMSET_FB_OK			= 1,	//	�����̾��� ��뿡 ����.
	EMREQ_PREMIUMSET_FB_NOITEM		= 2,	//	�����̾��� �������� ����.

	EMREQ_PREMIUMSET_FB_NOTINVEN	= 3,	//	�����̾��� �ڽ��� �������� ������� �κ� ���� ����.
};

enum EMREQ_CHARGEDITEM_FROMDB_FB
{
	EMREQ_CHARGEDITEM_FROMDB_FB_END		       = 0,	//! ������������
	EMREQ_CHARGEDITEM_FROMDB_FB_OK		       = 1,	//! ����
    EMREQ_CHARGEDITEM_FROMDB_FB_END_FIRST_JOIN = 2, //! ������������, ���� ���� UI ���� ����
};

enum EMCHARGED_ITEM_GET_FB
{
	EMCHARGED_ITEM_GET_FB_FAIL		= 0,	//	�Ϲݿ���.
	EMCHARGED_ITEM_GET_FB_OK		= 1,	//	����.
	EMCHARGED_ITEM_GET_FB_NOITEM	= 2,	//	�������� ����.
	EMCHARGED_ITEM_GET_FB_NOINVEN	= 3,	//	�κ����� ����.
};

enum EMINVEN_RANDOMBOXOPEN_FB
{
	EMINVEN_RANDOMBOXOPEN_FB_FAIL	= 0,	//	�Ϲݿ���.
	EMINVEN_RANDOMBOXOPEN_FB_OK		= 1,	//	����.
	EMINVEN_RANDOMBOXOPEN_FB_EMPTY	= 2,	//	�ڽ��� �������� ����.
	EMINVEN_RANDOMBOXOPEN_FB_BADITEM= 3,	//	�߸��� ������.
	EMINVEN_RANDOMBOXOPEN_FB_NOINVEN= 4,	//	�κ��� �������� ����.
	EMINVEN_RANDOMBOXOPEN_FB_MISS	= 5,	//	��.
};

enum EMINVEN_LOCKBOX_FB
{
	EMINVEN_LOCKBOX_FB_UNLOCK_FAIL	= 0,	//	�Ϲݿ���;
	EMINVEN_LOCKBOX_FB_UNLOCK_OK,			//	����;
	EMINVEN_LOCKBOX_FB_UNLOCK_ALREADY,		//	�̹� ��������;
	EMINVEN_LOCKBOX_FB_UNLOCK_INCORRECT,	//	�ش� ����� �� �� ����;
	EMINVEN_LOCKBOX_FB_UNLOCK_KEYINSTANCE,	//	Ű�� �Ҹ�ǰ�� �ƴϾ ����;
	EMINVEN_LOCKBOX_FB_OPEN_FAIL,			//	�Ϲݿ���;
	EMINVEN_LOCKBOX_FB_OPEN_OK,				//	����;
	EMINVEN_LOCKBOX_FB_OPEN_NOINVEN,		//	�κ��� �������� ����;
	EMINVEN_LOCKBOX_FB_OPEN_LOCK,			//  �������;
	EMINVEN_LOCKBOX_FB_SHORT_KEY,			//	Ű������ ���ڶ�;
	EMINVEN_LOCKBOX_FB_FAIL,
};

enum EMINVEN_SELFORMBOX_SELOPEN_FB
{
	EMINVEN_SELFORMBOX_FB_FAIL = 0,
	EMINVEN_SELFORMBOX_FB_OK,
	EMINVEN_SELFORMBOX_FB_INVENNOSPACE,	// �κ��ڸ�����
};

enum EMINVEN_DISJUNCTION_FB
{
	EMINVEN_DISJUNCTION_FB_FAIL		= 0,	//	�Ϲ� ����.
	EMINVEN_DISJUNCTION_FB_OK		= 1,	//	����.
	EMINVEN_DISJUNCTION_FB_BADITEM	= 2,	//	�߸��� ������.
	EMINVEN_DISJUNCTION_FB_NOINVEN	= 3,	//	�κ� ���� ����.
	EMINVEN_DISJUNCTION_FB_NONEED	= 4,	//	�Ϲ� ����.
};

enum EMREBUILD_RESULT	// ITEMREBUILD_MARK
{
	EMREBUILD_RESULT_FAIL				= 0,	// �Ϲ� ����;
	EMREBUILD_RESULT_OPEN				= 1,	// ���� �������̽� ����;
	EMREBUILD_RESULT_SUCCESS			= 2,	// ���� ����;
	EMREBUILD_RESULT_DESTROY			= 3,	// ���� ����, ������ �Ҹ�;
	EMREBUILD_RESULT_MONEY				= 4,	// �ݾ��� ���� �ʾƼ� ���� ����;
	EMREBUILD_RESULT_CLOSE				= 5,	// ���� �������̽� �ݱ�;
	EMREBUILD_RESULT_TRY				= 6,	// ���� �õ�;
	EMREBUILD_RESULT_CANCEL				= 7,	// ���� ���;
	EMREBUILD_RESULT_DIRECTSUCCESS		= 8,	// �ٷ� ���� ����;
	
	EMREBUILD_CHANGE_REBUILDTYPE_KEEPORIGIN_CACHERESULT			= 11,
	EMREBUILD_CHANGE_REBUILDTYPE_APPLYRESULT_CACHEORIGINAL		= 12,
	EMREBUILD_RESULT_KEEPORIGIN_CACHERESULT						= 13,
	EMREBUILD_RESULT_APPLYRESULT_CACHEORIGINAL					= 14,
	EMREBUILD_TRY_SELECT_INTO_CACHE								= 15, // ĳ�̾��������� ���ư�;
	EMREBUILD_RESULT_SELECT_INTO_CACHE_OK						= 16,
	EMREBUILD_RESULT_SELECT_INTO_CACHE_FAIL						= 17,
	EMREBUILD_UPDATE											= 18,
};

enum EMREQ_NPC_RECALL_FB
{
	EMREQ_NPC_RECALL_FB_FAIL		= 0,	//	NPC ȣ�� ����
	EMREQ_NPC_RECALL_FB_OK			= 1,	//	NPC ȣ�� ����
	EMREQ_NPC_RECALL_FB_NOITEM		= 2,	//	NPC ȣ�� ī�� ������ ����
	EMREQ_NPC_RECALL_FB_NPC			= 3,	//	NPC DATA ERROR
	EMREQ_NPC_RECALL_FB_POS			= 4,	//	NPC DROP ERROR
};

enum EMREQ_NPC_COMMISSION_FB
{
	EMREQ_NPC_COMMISSION_FAIL		= 0,	//	NPC ���� ����
	EMREQ_NPC_COMMISSION_CHAR		= 1,	//	NPC CHAR�� �ٷ� ������Ʈ
	EMREQ_NPC_COMMISSION_DB			= 2,	//	NPC DB�� ����
};

enum EMITEM_MIX_FB
{
	EMITEM_MIX_FB_FAIL				= 0,	//	����
	EMITEM_MIX_FB_OK				= 1,	//	����
	EMITEM_MIX_FB_ERROR				= 2,	//	�Ϲ����� ����
	EMITEM_MIX_FB_NOMIX				= 3,	//	���� ����
	EMITEM_MIX_FB_NOMIXNUM			= 4,	//	���տ� ������ ���� �ʽ��ϴ�.
	EMITEM_MIX_FB_NOITEM			= 5,	//	������ ����	
	EMITEM_MIX_FB_NOMONEY			= 6,	//	������
	EMITEM_MIX_FB_NOINVEN			= 7,	//	�κ� ��ġ ����
	EMITEM_MIX_FB_NONPC				= 8,	//	NPC����
};

enum EMPUTONRELEASE_TYPE
{
	EMPUTONRELEASE_TYPE_NORMAL		= 0, // �⺻
	EMPUTONRELEASE_TYPE_REFRESH     = 1, // ������
	EMPUTONRELEASE_TYPE_TURN	    = 2, // ���� ����
};

enum EMTEMPINVENINSERT_FB
{
	EMTEMPINVENINSERT_FB_FAILED				  = 0, // �Ϲ� ����
	EMTEMPINVENINSERT_FB_SUCCESS			  = 1, // ����
	EMTEMPINVENINSERT_FB_NONSPACE_RETURNINVEN = 2, // �ǵ��ư� �κ��� ���� ����
	EMTEMPINVENINSERT_FB_NONSPACE_TEMPINVEN	  = 3, // �ӽ� �κ��� ���� ����
	EMTEMPINVENINSERT_FB_NON_ALLOWED_ITEM	  = 4, // ������� �ʴ� ������
};

enum EMREQ_BOX_WRAPPING_FB
{
    EMREQ_BOX_WRAPPING_FB_FAILED                = 0, // �Ϲ� ����;
    EMREQ_BOX_WRAPPING_FB_SUCCESS               = 1, // ����;
    EMREQ_BOX_WRAPPING_FB_NON_ALLOWED_ITEM      = 2, // ������ ������� �ʴ� ������;
};

enum EMREQ_BOX_UNWRAPPING_FB
{
    EMREQ_BOX_UNWRAPPING_FB_FAILED                = 0, // �Ϲ� ����;
    EMREQ_BOX_UNWRAPPING_FB_SUCCESS               = 1, // ����;
};

enum EMREQ_ITEMDECOMPOSE_FB
{
	EMREQ_ITEMDECOMPOSE_FB_FAILED	= 0,		// �Ϲ� ����;
	EMREQ_ITEMDECOMPOSE_FB_SUCCESS,				// ����;
	EMREQ_ITEMDECOMPOSE_FB_NOT_SPLIT,			// ���� �Ұ���;
	EMREQ_ITEMDECOMPOSE_FB_NOT_ENOUGH_INVEN,	// �κ� ���� ����;
	EMREQ_ITEMDECOMPOSE_FB_NOT_ENOUGH_MONEY,	// �� ����;
};

enum EMREQ_NPC_EXCHANGE_ITEM_FB
{
	EMREQ_NEI_EX_FAILED			= 0,
	EMREQ_NEI_EX_SUCCESS		= 1,		// ���� ��ȯ	
	EMREQ_NEI_EX_NITEM_FB		= 2,		// ��ȯ �������� ����
	EMREQ_NEI_EX_NRPT_FB		= 3,		// ������Ʈ ����
	EMREQ_NEI_EX_NLVPT_FB		= 4,		// ��Ȱ ����Ʈ ����
	EMREQ_NEI_EX_NSPACE_FB		= 5,		// ��������	
};

enum EMREQ_COSTUME_STATS_FB
{
	EMREQ_CS_FB_SUCCESS		= 1,	// ����
	EMREQ_CS_FB_FAIL1		= 2,	// �������� ����
	EMREQ_CS_FB_FAIL2		= 3,	
	EMREQ_CS_FB_FAIL3		= 4,
	EMREQ_CS_FB_FAIL4		= 5,
	EMREQ_CS_FB_FAIL5		= 6,
	EMREQ_CS_FB_FAIL6       = 7,
	ENREQ_CS_FB_NOTICE1     = 8,    // �ɷ�ġ ���� �˸�
	EMREQ_CS_FB_FAIL7	    = 9,
};	

enum EMREQ_COSTUME_STAT_RELEASE_FB
{
	EMREQ_CS_RELEASE_FB_SUCCESS = 1,
	EMREQ_CS_RELEASE_FB_FAIL    = 2,
	EMREQ_CS_RELEASE_FB_FAIL2	= 3,
};

enum EMREQ_DRUG_CONSUME_FB
{
	EMREQ_DRUG_CONSUME_SUCCESS = 0,
	EMREQ_DRUG_CONSUME_FAIL = 1,
};

enum EMREQ_TEMP_ENCHANT_RESULT
{
	EMREQ_TEMP_ENCHANT_NONE,
	EMREQ_TEMP_ENCHANT_SUCCESS,		// �Ͻð�ȭ ����
	EMREQ_TEMP_ENCHANT_FAIL,		// �Ͻð�ȭ ����
	EMREQ_TEMP_ENCHANT_NOT_ITEM,	// �Ͻð�ȭ�� �ο��� ���¿��� ��������� ���������� ��ȭ�� �Ұ�.
	EMREQ_TEMP_ENCHANT_FIRETIME,	// �ð�����
	EMREQ_TEMP_ENCHANT_NOT,			// ��ȭ�� �Ҽ� ���� �����ۿ� �Ͻð�ȭ�� �ο��ҽ� �Ұ�.
	EMREQ_TEMP_ENCHANT_EXIST,		// �Ͻð�ȭ�� �̹� �ο��Ǿ� ������ �Ͻð�ȭ�� ��ο��Ҷ� Y/N
	EMREQ_TEMP_ENCHANT_GRADE_NOT,   // ��ȭ ����� ��ȭ������ �Ͻð�ȭ ��޺��� ������ �Ұ�.
};

enum EMCHANGE_ITEMSKILL_FB
{
	EMCHANGE_ITEMSKILL_FB_FAIL				= 0,	// �����۽�ų����ī�� ��뿡 ����;
	EMCHANGE_ITEMSKILL_FB_OK				= 1,	// �����۽�ų����ī�� ��뿡 ����;
	EMCHANGE_ITEMSKILL_FB_NOITEM			= 2,	// �����۽�ų����ī�� �������� ����;
	EMCHANGE_ITEMSKILL_FB_COOLTIME			= 3,	// �����۽�ų����ī�� ��Ÿ��;
	EMCHANGE_ITEMSKILL_FB_NOTITEM			= 4,	// �����۽�ų����ī�� Ÿ�� �������� ����;
	EMCHANGE_ITEMSKILL_FB_DONTCHANGE		= 5,	// �����۽�ų������ �Ұ����� ������;
};

enum EMCHANGE_BASICSTATS_FB
{
	EMCHANGE_BASICSTATS_FB_FAIL				= 0,	// �⺻�ɷ�ġ����ī�� ��뿡 ����;
	EMCHANGE_BASICSTATS_FB_OK				= 1,	// �⺻�ɷ�ġ����ī�� ��뿡 ����;
	EMCHANGE_BASICSTATS_FB_NOITEM			= 2,	// �⺻�ɷ�ġ����ī�� �������� ����;
	EMCHANGE_BASICSTATS_FB_STAMP_ERROR		= 3,	// �⺻�ɷ�ġ����� �߸��� ����ī�� ���;
	EMCHANGE_BASICSTATS_FB_COOLTIME			= 4,	// �⺻�ɷ�ġ����ī�� ��Ÿ��;
	EMCHANGE_BASICSTATS_FB_NOTITEM			= 5,	// �⺻�ɷ�ġ����ī�� Ÿ�� �������� ����;
	EMCHANGE_BASICSTATS_FB_DONTCHANGE		= 6,	// �⺻�ɷ�ġ������ �Ұ����� ������;
};

enum EMCHANGE_ADDSTATS_FB
{
	EMCHANGE_ADDSTATS_FB_FAIL				= 0,	// ����ɷ�ġ����ī�� ��뿡 ����;
	EMCHANGE_ADDSTATS_FB_OK					= 1,	// ����ɷ�ġ����ī�� ��뿡 ����;
	EMCHANGE_ADDSTATS_FB_NOITEM				= 2,	// ����ɷ�ġ����ī�� �������� ����;
	EMCHANGE_ADDSTATS_FB_STAMP_ERROR		= 3,	// ����ɷ�ġ����� �߸��� ����ī�� ���;
	EMCHANGE_ADDSTATS_FB_COOLTIME			= 4,	// ����ɷ�ġ����ī�� ��Ÿ��;
	EMCHANGE_ADDSTATS_FB_NOTITEM			= 5,	// ����ɷ�ġ����ī�� Ÿ�� �������� ����;
	EMCHANGE_ADDSTATS_FB_DONTCHANGE			= 6,	// ����ɷ�ġ������ �Ұ����� ������;
};

#define MAX_COSTUME_INVEST_STATS	12

namespace GLMSG
{
	#pragma pack(1)

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_FIELD_TO_INVEN : public NET_MSG_GENERIC
	{
		EMCROW				emCrow;
		DWORD				dwID;
		bool				bPet;

		SNETPC_REQ_FIELD_TO_INVEN ()
            :	emCrow( CROW_PC )
			,	dwID ( 0 )
			,	bPet ( false )
		{
			dwSize = sizeof(SNETPC_REQ_FIELD_TO_INVEN);
			nType = NET_MSG_GCTRL_REQ_FIELD_TO_INVEN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FIELD_TO_INVEN)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_TAKE_FB : public NET_MSG_GENERIC
	{	
		EMCROW				emCrow;
		EMTAKE_FB			emTakeFB;

		SNETPC_REQ_TAKE_FB () 
			: emCrow(CROW_ITEM)
			, emTakeFB(EMTAKE_FB_OFF)
		{
			dwSize = sizeof(SNETPC_REQ_TAKE_FB);
			nType = NET_MSG_GCTRL_REQ_TAKE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_TAKE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_PICKUP_MONEY : public NET_MSG_GENERIC
	{
		
		LONGLONG			lnMoney;
		LONGLONG			lnPickUp;

		SNETPC_PICKUP_MONEY () 
			: lnMoney(0)
			, lnPickUp(0)
		{
			dwSize = sizeof(SNETPC_PICKUP_MONEY);
			nType = NET_MSG_GCTRL_PICKUP_MONEY;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PICKUP_MONEY)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_PICKUP_ITEM : public NET_MSG_GENERIC
	{
		
		SNATIVEID			sNID_ITEM;

		SNETPC_PICKUP_ITEM ()
		{
			dwSize = sizeof(SNETPC_PICKUP_ITEM);
			nType = NET_MSG_GCTRL_PICKUP_ITEM;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PICKUP_ITEM)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_FIELD_TO_HOLD : public NET_MSG_GENERIC
	{
		
		DWORD				dwGlobID;

		SNETPC_REQ_FIELD_TO_HOLD () 
			: dwGlobID(0)
		{
			dwSize = sizeof(SNETPC_REQ_FIELD_TO_HOLD);
			nType = NET_MSG_GCTRL_REQ_FIELD_TO_HOLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FIELD_TO_HOLD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_INVEN_TO_FIELD : public NET_MSG_GENERIC
	{	
		D3DXVECTOR3			vPos;
		SITEMCUSTOM			sItem;
		BOOL				bVietnamItem;
        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPC_REQ_INVEN_TO_FIELD () 
			: vPos(0,0,0)
			, bVietnamItem(FALSE)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_TO_FIELD);
			nType = NET_MSG_GCTRL_REQ_INVEN_TO_FIELD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_TO_FIELD)<=NET_DATA_BUFSIZE);
			sItem = SITEMCUSTOM();
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_INVEN_TO_HOLD : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_INVEN_TO_HOLD () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_TO_HOLD);
			nType = NET_MSG_GCTRL_REQ_INVEN_TO_HOLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_TO_HOLD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_INVEN_EX_INVEN : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPC_REQ_INVEN_EX_INVEN () 
			: wPosX(0)
			, wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_EX_INVEN);
			nType = NET_MSG_GCTRL_REQ_INVEN_EX_INVEN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_EX_INVEN)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNETPC_REQ_STORAGE_EX_INVEN : public NET_MSG_GENERIC
    {
        DWORD				dwChannel;
        DWORD				dwNpcGlobID;
        WORD				wPosX;
        WORD				wPosY;
        WORD                wHoldPosX;
        WORD                wHoldPosY;

        SNETPC_REQ_STORAGE_EX_INVEN () 
            : dwChannel(0)
            , dwNpcGlobID(0)
            , wPosX(0)
            , wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
        {
            dwSize = sizeof(SNETPC_REQ_STORAGE_EX_INVEN);
            nType = NET_MSG_GCTRL_REQ_STORAGE_EX_INVEN;
            MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STORAGE_EX_INVEN)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_INVEN_TO_SLOT : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;

		EMSLOT				emToSlot;

		SNETPC_REQ_INVEN_TO_SLOT () 
			: wPosX(0)
			, wPosY(0)
			, emToSlot(SLOT_LHAND)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_TO_SLOT);
			nType = NET_MSG_GCTRL_REQ_INVEN_TO_SLOT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_TO_SLOT)<=NET_DATA_BUFSIZE);
		}
	};

    //---------------------------------------------------------------------------NET
    struct SNETPC_REQ_SLOT_TO_INVEN : public NET_MSG_GENERIC
    {
        WORD				wPosX;
        WORD				wPosY;

        EMSLOT				emFromSlot;

        SNETPC_REQ_SLOT_TO_INVEN () 
            : wPosX(0)
            , wPosY(0)
            , emFromSlot(SLOT_LHAND)
        {
            dwSize = sizeof(SNETPC_REQ_SLOT_TO_INVEN);
            nType = NET_MSG_GCTRL_REQ_SLOT_TO_INVEN;
            MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SLOT_TO_INVEN)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_MONEY_TO_FIELD : public NET_MSG_GENERIC
	{
        enum { MAGIC_NUM=72301, };

		int         m_MagicNum;
        D3DXVECTOR3 vPos;
		LONGLONG    lnMoney;        

		SNETPC_REQ_MONEY_TO_FIELD()
			: vPos(0,0,0)
			, lnMoney(0)
            , m_MagicNum(MAGIC_NUM)
		{
			dwSize = sizeof(SNETPC_REQ_MONEY_TO_FIELD);
			nType = NET_MSG_GCTRL_REQ_MONEY_TO_FIELD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_MONEY_TO_FIELD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_INVEN_SPLIT : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;
		WORD				wSplit;

		SNETPC_REQ_INVEN_SPLIT () 
			: wPosX(0)
			, wPosY(0)
			, wSplit(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_SPLIT);
			nType = NET_MSG_GCTRL_REQ_INVEN_SPLIT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_SPLIT)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_SLOT_TO_HOLD : public NET_MSG_GENERIC
	{
		
		EMSLOT				emSlot;

		SNETPC_REQ_SLOT_TO_HOLD () 
			: emSlot(SLOT_HEADGEAR)
		{
			dwSize = sizeof(SNETPC_REQ_SLOT_TO_HOLD);
			nType = NET_MSG_GCTRL_REQ_SLOT_TO_HOLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SLOT_TO_HOLD)<=NET_DATA_BUFSIZE);
		}
	};

    //---------------------------------------------------------------------------NET
    struct SNETPC_REQ_HOLD_TO_INVEN : public NET_MSG_GENERIC
    {


        WORD				wPosX;
        WORD				wPosY;
        BOOL				bUseVietnamInven;

        SNETPC_REQ_HOLD_TO_INVEN () 
            : wPosX(0)
            , wPosY(0)
        {
            dwSize = sizeof(SNETPC_REQ_HOLD_TO_INVEN);
            nType = NET_MSG_GCTRL_REQ_HOLD_TO_INVEN;
            MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_HOLD_TO_INVEN)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_INVEN_TO_INVEN : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;
		BOOL				bUseVietnamInven;

        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPC_REQ_INVEN_TO_INVEN () 
			: wPosX(0)
			, wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_TO_INVEN);
			nType = NET_MSG_GCTRL_REQ_INVEN_TO_INVEN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_TO_INVEN)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNETPC_REQ_STORAGE_TO_INVEN : public NET_MSG_GENERIC
    {
        DWORD				dwChannel;
        DWORD				dwNpcGlobID;
        WORD				wPosX;
        WORD				wPosY;
        BOOL				bUseVietnamInven;

        WORD                wHoldPosX;
        WORD                wHoldPosY;

        SNETPC_REQ_STORAGE_TO_INVEN () 
            : dwChannel(0)
            , dwNpcGlobID(0)
            , wPosX(0)
            , wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
        {
            dwSize = sizeof(SNETPC_REQ_STORAGE_TO_INVEN);
            nType = NET_MSG_GCTRL_REQ_STORAGE_TO_INVEN;
            MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STORAGE_TO_INVEN)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_HOLD_TO_SLOT : public NET_MSG_GENERIC
	{
		

		EMSLOT				emSlot;

		SNETPC_REQ_HOLD_TO_SLOT () 
			: emSlot(SLOT_HEADGEAR)
		{
			dwSize = sizeof(SNETPC_REQ_HOLD_TO_SLOT);
			nType = NET_MSG_GCTRL_REQ_HOLD_TO_SLOT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_HOLD_TO_SLOT)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_SLOT_CHANGE : public NET_MSG_GENERIC
	{
		

		SNETPC_REQ_SLOT_CHANGE () 
		{
			dwSize = sizeof(SNETPC_REQ_SLOT_CHANGE);
			nType = NET_MSG_GCTRL_REQ_SLOT_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SLOT_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_HOLD_FB : public NET_MSG_GENERIC
	{
		

		EMNET_MSG			emHoldMsg;
		EMHOLD_FB			emHoldFB;

		SNETPC_REQ_HOLD_FB () 
			: emHoldMsg(NET_MSG_GCTRL_REQ_INVEN_TO_HOLD)
			, emHoldFB(EMHOLD_FB_OFF)
		{
			dwSize = sizeof(SNETPC_REQ_HOLD_FB);
			nType = NET_MSG_GCTRL_REQ_HOLD_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_HOLD_FB)<=NET_DATA_BUFSIZE);
		}

		SNETPC_REQ_HOLD_FB ( EMNET_MSG _msg, EMHOLD_FB _fb ) 
			: emHoldMsg(_msg)
			, emHoldFB(_fb)
		{
			dwSize = sizeof(SNETPC_REQ_HOLD_FB);
			nType = NET_MSG_GCTRL_REQ_HOLD_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_HOLD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_INVEN_INSERT //: public NET_MSG_GENERIC
	{	
		SINVENITEM			Data;
		BOOL				bVietnamInven;
		bool				bAllLine;
		bool				bNew;

        MSGPACK_DEFINE(Data, bVietnamInven, bAllLine, bNew);

		SNETPC_INVEN_INSERT ()
			: bVietnamInven(FALSE)
			, bAllLine ( false )
			, bNew( false )
		{
			//dwSize = sizeof(SNETPC_INVEN_INSERT);
			//nType = NET_MSG_GCTRL_INVEN_INSERT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_INSERT)<=NET_DATA_BUFSIZE);
		}

        SNETPC_INVEN_INSERT(
            const SINVENITEM& _Data,
            BOOL _bVietnamInven,
            bool _bAllLine,
			bool _bNew )
            : Data(_Data)
            , bVietnamInven(_bVietnamInven)
            , bAllLine(_bAllLine)
			, bNew( _bNew )
        {
            //dwSize = sizeof(SNETPC_INVEN_INSERT);
            //nType = NET_MSG_GCTRL_INVEN_INSERT;
            MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_INSERT)<=NET_DATA_BUFSIZE);
        }
	};

	//! Inventory item delete
	struct SNETPC_INVEN_DELETE : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{		
		WORD wPosX;
		WORD wPosY;
		BOOL bVietnamInven;		
        
        SNETPC_INVEN_DELETE(WORD _PosX, WORD _PosY, BOOL _bVietnamInven)
            : wPosX(_PosX)
            , wPosY(_PosY)
            , bVietnamInven(_bVietnamInven)
        {
            dwSize = sizeof(SNETPC_INVEN_DELETE);
            nType = NET_MSG_GCTRL_INVEN_DELETE;
            MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_DELETE)<=NET_DATA_BUFSIZE);
        }
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_INVEN_DEL_AND_INSERT //: public NET_MSG_GENERIC
	{		
		WORD				wDelX;
		WORD				wDelY;
		BOOL				bVietnamInven;        
		SINVENITEM			sInsert;

        MSGPACK_DEFINE(wDelX, wDelY, bVietnamInven, sInsert);

		SNETPC_INVEN_DEL_AND_INSERT ()
			: wDelX(0)
			, wDelY(0)
			, bVietnamInven(FALSE)
		{
			//dwSize = sizeof(SNETPC_INVEN_DEL_AND_INSERT);
			//nType = NET_MSG_GCTRL_INVEN_DEL_INSERT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_DEL_AND_INSERT)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_INVEN_DRUG_UPDATE : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		
		WORD				wPosX;
		WORD				wPosY;
		BOOL				bVietnamInven;

		WORD				wTurnNum;
		
		SNETPC_INVEN_DRUG_UPDATE () 
			: wPosX(0)
			, wPosY(0)
			, wTurnNum(0)
			, bVietnamInven(FALSE)
		{
			dwSize = sizeof(SNETPC_INVEN_DRUG_UPDATE);
			nType = NET_MSG_GCTRL_INVEN_DRUG_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_DRUG_UPDATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_PUTON_DRUG_UPDATE : public NET_MSG_GENERIC
	{
		
		EMSLOT				emSlot;
		WORD				wTurnNum;
		
		SNETPC_PUTON_DRUG_UPDATE () 
			: emSlot(SLOT_HEADGEAR)
			, wTurnNum(0)
		{
			dwSize = sizeof(SNETPC_PUTON_DRUG_UPDATE);
			nType = NET_MSG_GCTRL_PUTON_DRUG_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_DRUG_UPDATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_INVEN_ITEM_UPDATE : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;
		SITEMCUSTOM			sItemCustom;
		BOOL				bVietnamInven;

		SNET_INVEN_ITEM_UPDATE () 
			: wPosX(0)
			, wPosY(0)
			, bVietnamInven(FALSE)
		{
			dwSize = sizeof(SNET_INVEN_ITEM_UPDATE);
			nType = NET_MSG_GCTRL_INVEN_ITEM_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_ITEM_UPDATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_PUTON_RELEASE : public NET_MSG_GENERIC
	{
		
		EMSLOT				emSlot;
		EMPUTONRELEASE_TYPE emType;

		SNETPC_PUTON_RELEASE () 
			: emSlot(SLOT_HEADGEAR)
			, emType(EMPUTONRELEASE_TYPE_NORMAL)
		{
			dwSize = sizeof(SNETPC_PUTON_RELEASE);
			nType = NET_MSG_GCTRL_PUTON_RELEASE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_RELEASE)<=NET_DATA_BUFSIZE);
		}

		SNETPC_PUTON_RELEASE ( EMSLOT _slot ) 
			: emSlot(_slot)
			, emType(EMPUTONRELEASE_TYPE_NORMAL)
		{
			dwSize = sizeof(SNETPC_PUTON_RELEASE);
			nType = NET_MSG_GCTRL_PUTON_RELEASE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_RELEASE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_PUTON_RELEASE_BRD : public SNETPC_BROAD
	{
		EMSLOT				emSlot;

		SNETPC_PUTON_RELEASE_BRD () 
			: emSlot(SLOT_HEADGEAR)
		{
			dwSize = sizeof(SNETPC_PUTON_RELEASE_BRD);
			nType = NET_MSG_GCTRL_PUTON_RELEASE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_RELEASE_BRD)<=NET_DATA_BUFSIZE);
		}

        SNETPC_PUTON_RELEASE_BRD ( EMSLOT _slot ) 
            : emSlot(_slot)
        {
            dwSize = sizeof(SNETPC_PUTON_RELEASE_BRD);
            nType = NET_MSG_GCTRL_PUTON_RELEASE_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_RELEASE_BRD)<=NET_DATA_BUFSIZE);
        }
	};

	//! ��������� ��ȭ
	struct SNETPC_PUTON_UPDATE //: public NET_MSG_GENERIC
	{
		BYTE        m_SlotRelease; // EMSLOT
		BYTE        m_Slot; // EMSLOT
		SITEMCUSTOM m_Item;

        MSGPACK_DEFINE(m_SlotRelease, m_Slot, m_Item);

		SNETPC_PUTON_UPDATE()
			: m_SlotRelease(SLOT_TSIZE)
			, m_Slot(SLOT_HEADGEAR)
		{
            MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_UPDATE)<=NET_DATA_BUFSIZE);
			//dwSize = sizeof(SNETPC_PUTON_UPDATE);
			//nType = NET_MSG_GCTRL_PUTON_UPDATE;
		}
	};

	//! ��������� ��ȭ
	struct SNETPC_PUTON_UPDATE_BRD //: public SNETPC_BROAD
	{
        DWORD       m_GaeaID;
		BYTE		m_SlotRelease; // EMSLOT
		BYTE		m_Slot; // EMSLOT
		SITEMCLIENT	m_Item;

        MSGPACK_DEFINE(m_GaeaID, m_SlotRelease, m_Slot, m_Item);

		SNETPC_PUTON_UPDATE_BRD()
			: m_SlotRelease(SLOT_NSIZE_S_2)
			, m_Slot(SLOT_HEADGEAR)
		{
            MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_UPDATE_BRD)<=NET_DATA_BUFSIZE);
			//dwSize = sizeof(SNETPC_PUTON_UPDATE_BRD);
			//nType = NET_MSG_GCTRL_PUTON_UPDATE_BRD;			
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_PUTON_CHANGE : public NET_MSG_GENERIC
	{
		

		BOOL				bUseArmSub;
		float				fCONFT_HP_RATE;

		SNETPC_PUTON_CHANGE () 
			: bUseArmSub(FALSE)
			, fCONFT_HP_RATE(1.0f)
		{
			dwSize = sizeof(SNETPC_PUTON_CHANGE);
			nType = NET_MSG_GCTRL_PUTON_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_PUTON_CHANGE_BRD : public SNETPC_BROAD
	{
		BOOL				bUseArmSub;

		SNETPC_PUTON_CHANGE_BRD () 
			: bUseArmSub(FALSE)
		{
			dwSize = sizeof(SNETPC_PUTON_CHANGE_BRD);
			nType = NET_MSG_GCTRL_PUTON_CHANGE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_CHANGE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_PUTON_CHANGE_AG : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		BOOL				bUseArmSub;

		SNETPC_PUTON_CHANGE_AG () 
			: bUseArmSub(FALSE)
		{
			dwSize = sizeof(SNETPC_PUTON_CHANGE_AG);
			nType = NET_MSG_GCTRL_PUTON_CHANGE_AG;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PUTON_CHANGE_AG)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_BUY_FROM_NPC : public NET_MSG_GENERIC
	{
		SNATIVEID sNID;
		DWORD	  dwChannel;
		DWORD	  dwNpcGlobID;
		WORD	  wPosX;
		WORD	  wPosY;
		WORD	  wBuyNum;
		EMCROW	  emCrow;
        unsigned int Crc32;

		SNETPC_REQ_BUY_FROM_NPC()
			: dwChannel(0)
			, dwNpcGlobID(0)
			, wPosX(0)
			, wPosY(0)
			, wBuyNum(1)
			, emCrow(CROW_NPC)
		{
			dwSize = sizeof(SNETPC_REQ_BUY_FROM_NPC);
			nType = NET_MSG_GCTRL_REQ_BUY_FROM_NPC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_BUY_FROM_NPC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_EXCHANGE_BUY_FROM_NPC : public NET_MSG_GENERIC
	{
		EMREQ_NPC_EXCHANGE_ITEM_FB	ReValue;

		enum { MAX_ITEM_LIST = 10 };

		struct sExItemInfo
		{
			short	  wPosX;
			short	  wPosY;

			WORD	  wBuyNum;

			DWORD		dwChannel;

			sExItemInfo() : wPosX(-1), wPosY(-1), wBuyNum(1), dwChannel(0) {}
		};

		SNATIVEID	sNID;
		//DWORD		dwChannel;
		DWORD		dwNpcGlobID;
		EMCROW		emCrow;
		sExItemInfo itemlist[MAX_ITEM_LIST];

		unsigned int Crc32;

		SNETPC_REQ_EXCHANGE_BUY_FROM_NPC()
			: dwNpcGlobID(0), ReValue(EMREQ_NEI_EX_SUCCESS), emCrow( CROW_NPC )
		{
			dwSize = sizeof(SNETPC_REQ_EXCHANGE_BUY_FROM_NPC);
			nType = NET_MSG_GCTRL_REQ_EXCHANGE_BUY_FROM_NPC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_EXCHANGE_BUY_FROM_NPC)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_SALE_TO_NPC : public NET_MSG_GENERIC
	{		
		SNATIVEID	 sNID;
        WORD         wPosX;
        WORD         wPosY;
        WORD         wHoldPosX;
        WORD         wHoldPosY;
		DWORD		 dwNpcGlobID;
		EMCROW		 emCrow;
        unsigned int Crc32;

		SNETPC_REQ_SALE_TO_NPC()
			: dwNpcGlobID(0)
            , wPosX(USHRT_MAX)
            , wPosY(USHRT_MAX)
            , wHoldPosX(0)
            , wHoldPosY(0)
            , Crc32(0)
			, emCrow( CROW_NPC )
		{
			dwSize = sizeof(SNETPC_REQ_SALE_TO_NPC);
			nType = NET_MSG_GCTRL_REQ_SALE_TO_NPC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_SALE_TO_NPC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_INVENDRUG : public NET_MSG_GENERIC
	{		
		WORD wPosX;
		WORD wPosY;
		bool bPetSkill;
        unsigned int Crc32;

        SNETPC_REQ_INVENDRUG(WORD PosX=0, WORD PosY=0, bool PetSkill=false)
			: wPosX(PosX)
			, wPosY(PosY)
			, bPetSkill(PetSkill)
		{
			dwSize = sizeof(SNETPC_REQ_INVENDRUG);
			nType = NET_MSG_GCTRL_REQ_INVENDRUG;
		}
	};

	struct SNETPC_REQ_INVEN_RECALL : public NET_MSG_GENERIC
	{
		
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_INVEN_RECALL () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_RECALL);
			nType = NET_MSG_GCTRL_REQ_RECALL;
		}
	};

	struct SNETPC_REQ_INVEN_TELEPORT : public NET_MSG_GENERIC
	{
		
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_INVEN_TELEPORT () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_TELEPORT);
			nType = NET_MSG_GCTRL_REQ_TELEPORT;
		}
	};

	struct SNETPC_REQ_INVEN_LUNCHBOX : public NET_MSG_GENERIC
	{
		
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_INVEN_LUNCHBOX () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_LUNCHBOX);
			nType = NET_MSG_GCTRL_REQ_INVEN_LUNCHBOX;
		}
	};

	struct SNETPC_REQ_INVEN_LUNCHBOX_FB : public NET_MSG_GENERIC
	{		
		SNATIVEID		 sSKILLID;
		WORD             wLEVEL;
		WORD			 wSLOT;
		EMLUNCHBOX_FB	 emFB;

		SNETPC_REQ_INVEN_LUNCHBOX_FB () 
			: sSKILLID(NATIVEID_NULL())
			, wLEVEL  (0)
			, wSLOT   (0)
			, emFB    (EMLUNCHBOX_FB_OK)

		{
			dwSize = sizeof(SNETPC_REQ_INVEN_LUNCHBOX_FB);
			nType = NET_MSG_GCTRL_REQ_INVEN_LUNCHBOX_FB;
		}
	};

	struct SNETPC_REQ_INVEN_ITEMDECOMPOSE_CF : public NET_MSG_GENERIC
	{
		WORD wPosX, wPosY;
		SNETPC_REQ_INVEN_ITEMDECOMPOSE_CF()
			: wPosX(0), wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_ITEMDECOMPOSE_CF);
			nType = NET_MSG_GCTRL_REQ_INVEN_DECOMPOSE_CF;
		}
	};

	struct SNETPC_REQ_INVEN_ITEMDECOMPOSE_FB : public NET_MSG_GENERIC
	{
		enum { MAX_ITEM_COUNT = 10, };
		struct sExItemInfo{
			SNATIVEID sNID;
			WORD wTurnNum;
			sExItemInfo(void) : sNID(NATIVEID_NULL()), wTurnNum(-1) {}
		};
		SNATIVEID sDecomposeItem;					// ������ ������;
		sExItemInfo sGenIteminfo[MAX_ITEM_COUNT];	// �����Ͽ� ������ ������;
		EMREQ_ITEMDECOMPOSE_FB	emFeedback;			// ��� �޼���;
		SNETPC_REQ_INVEN_ITEMDECOMPOSE_FB()
			: emFeedback(EMREQ_ITEMDECOMPOSE_FB_FAILED)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_ITEMDECOMPOSE_FB);
			nType = NET_MSG_GCTRL_REQ_INVEN_DECOMPOSE_FB;
		}

		void push_back(SNATIVEID sNID, WORD wTurnNum)
		{
			for ( int i = 0; i < MAX_ITEM_COUNT; ++i )
			{
				if ( sGenIteminfo[i].sNID == sNID )
				{
					sGenIteminfo[i].wTurnNum += wTurnNum;
					break;
				}
				else if ( sGenIteminfo[i].sNID == NATIVEID_NULL() )
				{				
					sGenIteminfo[i].sNID = sNID;
					sGenIteminfo[i].wTurnNum = wTurnNum;
					break;
				}
			}
		}
	};

	struct SNETPC_REQ_LEARNSKILL : public NET_MSG_GENERIC
	{
		
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_LEARNSKILL () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_LEARNSKILL);
			nType = NET_MSG_GCTRL_REQ_LEARNSKILL;
		}
	};

	struct SNETPC_REQ_LEARNSKILL_FB : public NET_MSG_GENERIC
	{
		
		SNATIVEID			skill_id;
		EMSKILL_LEARNCHECK	emCHECK;

		SNETPC_REQ_LEARNSKILL_FB () 
			: skill_id(NATIVEID_NULL())
			, emCHECK(EMSKILL_LEARN_OK)
		{
			dwSize = sizeof(SNETPC_REQ_LEARNSKILL_FB);
			nType = NET_MSG_GCTRL_REQ_LEARNSKILL_FB;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM : public NET_MSG_GENERIC
	{
		DWORD				dwItemType;

		SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM() 
			: dwItemType(-1)
		{
			dwSize = sizeof(SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM);
			nType = NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB : public NET_MSG_GENERIC
	{
		DWORD				dwItemCount;

		SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB() 
			: dwItemCount(0)
		{
			dwSize = sizeof(SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB);
			nType = NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_FB : public NET_MSG_GENERIC
	{
		DWORD				Data;

		SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_FB() 
		{
			dwSize = sizeof(SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM_FB);
			nType = NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_FB;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	struct SNETPC_REQ_GETSTORAGE : public NET_MSG_GENERIC
	{
		
		DWORD				dwChannel;
		DWORD				dwNpcGlobID;

		SNETPC_REQ_GETSTORAGE () 
			: dwChannel(0)
			, dwNpcGlobID(0)
		{
			dwSize = sizeof(SNETPC_REQ_GETSTORAGE);
			nType = NET_MSG_GCTRL_REQ_GETSTORAGE;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_GETSTORAGE_FB : public NET_MSG_GENERIC
	{
		
		LONGLONG			lnMoney;
		DWORD				dwChannel;
		DWORD				dwNumStorageItem;

		SNETPC_REQ_GETSTORAGE_FB () 
			: lnMoney(0)
			, dwChannel(0)
			, dwNumStorageItem(0)
		{
			dwSize = sizeof(SNETPC_REQ_GETSTORAGE_FB);
			nType = NET_MSG_GCTRL_REQ_GETSTORAGE_FB;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_GETSTORAGE_ITEM //: public NET_MSG_GENERIC
	{		
		DWORD		dwChannel;
		SINVENITEM	Data;

        MSGPACK_DEFINE(dwChannel, Data);

		SNETPC_REQ_GETSTORAGE_ITEM()
			: dwChannel(0)
		{
			//dwSize = sizeof(SNETPC_REQ_GETSTORAGE_ITEM);
			//nType = NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_STORAGEDRUG : public NET_MSG_GENERIC
	{
		
		DWORD				dwChannel;
		DWORD				dwNpcGlobID;
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_STORAGEDRUG () 
			: dwChannel(0)
			, dwNpcGlobID(0)
			, wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_STORAGEDRUG);
			nType = NET_MSG_GCTRL_REQ_STORAGEDRUG;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_LEARNSKILL_STORAGE : public NET_MSG_GENERIC
	{
		

		DWORD				dwChannel;
		DWORD				dwNpcGlobID;
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_LEARNSKILL_STORAGE () 
			: dwChannel(0)
			, dwNpcGlobID(0)
			, wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_LEARNSKILL_STORAGE);
			nType = NET_MSG_GCTRL_REQ_STORAGESKILL;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_STORAGE_TO_HOLD : public NET_MSG_GENERIC
	{
		

		DWORD				dwChannel;
		DWORD				dwNpcGlobID;
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_STORAGE_TO_HOLD () 
			: dwChannel(0)
			, dwNpcGlobID(0)
			, wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_STORAGE_TO_HOLD);
			nType = NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STORAGE_TO_HOLD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_INVEN_EX_STORAGE : public NET_MSG_GENERIC
	{
		

		DWORD				dwChannel;
		DWORD				dwNpcGlobID;
		WORD				wPosX;
		WORD				wPosY;
        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPC_REQ_INVEN_EX_STORAGE () 
			: dwChannel(0)
			, dwNpcGlobID(0)
			, wPosX(0)
			, wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_EX_STORAGE);
			nType = NET_MSG_GCTRL_REQ_INVEN_EX_STORAGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_EX_STORAGE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_INVEN_TO_STORAGE : public NET_MSG_GENERIC
	{
		

		DWORD				dwChannel;
		DWORD				dwNpcGlobID;
		WORD				wPosX;
		WORD				wPosY;
        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPC_REQ_INVEN_TO_STORAGE () 
			: dwChannel(0)
			, dwNpcGlobID(0)
			, wPosX(0)
			, wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_TO_STORAGE);
			nType = NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_TO_STORAGE)<=NET_DATA_BUFSIZE);
		}
	};

    //---------------------------------------------------------------------------NET
    struct SNETPC_REQ_STORAGE_TO_STORAGE : public NET_MSG_GENERIC
    {
        DWORD				dwChannel;
        DWORD				dwNpcGlobID;
        WORD				wPosX;
        WORD				wPosY;

        DWORD				dwHoldChannel;
        DWORD				dwHoldNpcGlobID;
        WORD                wHoldPosX;
        WORD                wHoldPosY;

        SNETPC_REQ_STORAGE_TO_STORAGE () 
            : dwChannel(0)
            , dwNpcGlobID(0)
            , wPosX(0)
            , wPosY(0)
            , dwHoldChannel(0)
            , dwHoldNpcGlobID(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
        {
            dwSize = sizeof(SNETPC_REQ_STORAGE_TO_STORAGE);
            nType = NET_MSG_GCTRL_REQ_STORAGE_TO_STORAGE;
            MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STORAGE_TO_STORAGE)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------NET
	struct SNETPC_STORAGE_INSERT : public NET_MSG_GENERIC
	{
		DWORD				dwChannel;
		SINVENITEM			Data;

		SNETPC_STORAGE_INSERT () 
			: dwChannel(0)
		{
			dwSize = sizeof(SNETPC_STORAGE_INSERT);
			nType = NET_MSG_GCTRL_STORAGE_INSERT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_STORAGE_INSERT)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_STORAGE_DELETE : public NET_MSG_GENERIC
	{
		

		DWORD				dwChannel;
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_STORAGE_DELETE () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_STORAGE_DELETE);
			nType = NET_MSG_GCTRL_STORAGE_DELETE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_STORAGE_DELETE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_STORAGE_ITEM_UPDATE : public NET_MSG_GENERIC
	{
		

        DWORD				dwChannel;
		WORD				wPosX;
		WORD				wPosY;
		SITEMCUSTOM			sItemCustom;

		SNETPC_STORAGE_ITEM_UPDATE () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_STORAGE_ITEM_UPDATE);
			nType = NET_MSG_GCTRL_STORAGE_ITEM_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_STORAGE_ITEM_UPDATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_STORAGE_DEL_AND_INSERT : public NET_MSG_GENERIC
	{
		
		DWORD				dwChannel;

		WORD				wDelX;
		WORD				wDelY;
        
		SINVENITEM			sInsert;

		SNETPC_STORAGE_DEL_AND_INSERT () 
			: dwChannel(0)
			, wDelX(0)
			, wDelY(0)
		{
			dwSize = sizeof(SNETPC_STORAGE_DEL_AND_INSERT);
			nType = NET_MSG_GCTRL_STORAGE_DEL_INSERT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_STORAGE_DEL_AND_INSERT)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_STORAGE_DRUG_UPDATE : public NET_MSG_GENERIC
	{
		
		DWORD				dwChannel;

		WORD				wPosX;
		WORD				wPosY;

		WORD				wTurnNum;
		
		SNETPC_STORAGE_DRUG_UPDATE () 
			: dwChannel(0)
			, wPosX(0)
			, wPosY(0)
			, wTurnNum(0)
		{
			dwSize = sizeof(SNETPC_STORAGE_DRUG_UPDATE);
			nType = NET_MSG_GCTRL_STORAGE_DRUG_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_STORAGE_DRUG_UPDATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_STORAGE_SAVE_MONEY : public NET_MSG_GENERIC
	{		
        enum { MAGIC_NUM=17232, };

        int          m_MagicNum;
        LONGLONG	 lnMoney;
		DWORD		 dwNpcGlobID;
		unsigned int Crc32;
        
		SNETPC_REQ_STORAGE_SAVE_MONEY() 
			: lnMoney(0)
			, dwNpcGlobID(0)
            , m_MagicNum(MAGIC_NUM)
		{
			dwSize = sizeof(SNETPC_REQ_STORAGE_SAVE_MONEY);
			nType = NET_MSG_GCTRL_STORAGE_SAVE_MONEY;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STORAGE_SAVE_MONEY)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_STORAGE_DRAW_MONEY : public NET_MSG_GENERIC
	{
        enum { MAGIC_NUM=41343, };

		int          m_MagicNum;
        DWORD		 dwNpcGlobID;
        LONGLONG	 lnMoney;
		unsigned int Crc32;

		SNETPC_REQ_STORAGE_DRAW_MONEY()
			: lnMoney(0)
			, dwNpcGlobID(0)
            , m_MagicNum(MAGIC_NUM)
		{
			dwSize = sizeof(SNETPC_REQ_STORAGE_DRAW_MONEY);
			nType = NET_MSG_GCTRL_STORAGE_DRAW_MONEY;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STORAGE_DRAW_MONEY)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_STORAGE_UPDATE_MONEY : public NET_MSG_GENERIC
	{
		
		LONGLONG			lnMoney;

		SNETPC_REQ_STORAGE_UPDATE_MONEY ()
			: lnMoney(0)
		{
			dwSize = sizeof(SNETPC_REQ_STORAGE_UPDATE_MONEY);
			nType = NET_MSG_GCTRL_STORAGE_UPDATE_MONEY;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STORAGE_UPDATE_MONEY)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_STORAGE_SPLIT : public NET_MSG_GENERIC
	{
		

		DWORD				dwChannel;
		DWORD				dwNpcGlobID;
		WORD				wPosX;
		WORD				wPosY;
		WORD				wSplit;

		SNETPC_REQ_STORAGE_SPLIT () 
			: dwChannel(0)
			, dwNpcGlobID(0)
			, wPosX(0)
			, wPosY(0)
			, wSplit(0)
		{
			dwSize = sizeof(SNETPC_REQ_STORAGE_SPLIT);
			nType = NET_MSG_GCTRL_REQ_STORAGE_SPLIT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STORAGE_SPLIT)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_INVEN_GRINDING : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNET_INVEN_GRINDING () 
			: wPosX(0)
			, wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_GRINDING);
			nType = NET_MSG_GCTRL_INVEN_GRINDING;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_GRINDING)<=NET_DATA_BUFSIZE);
		}
	};

	// �Ͻð�ȭ
	struct SNET_INVEN_TL_GRINDING : public NET_MSG_GENERIC
	{
		EMREQ_TEMP_ENCHANT_RESULT eResult;

		WORD				wPosX;
		WORD				wPosY;
		WORD                wHoldPosX;
		WORD                wHoldPosY;

		SNATIVEID			fireItemID;

		SNET_INVEN_TL_GRINDING () 
			: wPosX(0)
			, wPosY(0)
			, wHoldPosX(0)
			, wHoldPosY(0)
			, eResult( EMREQ_TEMP_ENCHANT_NONE )
			, fireItemID(NATIVEID_NULL())
		{
			dwSize = sizeof(SNET_INVEN_TL_GRINDING);
			nType = NET_MSG_GCTRL_INVEN_TL_GRINDING;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_TL_GRINDING)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_LOCKBOX_UNLOCK : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
		WORD                wHoldPosX;
		WORD                wHoldPosY;
		SNET_INVEN_LOCKBOX_UNLOCK () 
			: wPosX(0)
			, wPosY(0)
			, wHoldPosX(0)
			, wHoldPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_LOCKBOX_UNLOCK);
			nType = NET_MSG_GCTRL_INVEN_LOCKBOX_UNLOCK;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_LOCKBOX_UNLOCK)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_INVEN_GRINDING_FB : public NET_MSG_GENERIC
	{
		

		bool				bRESET;
		bool				bTERMINATE;
		DWORD				dwGrindResult;
		EMGRINDING_FB		emGrindFB;

		BYTE				cGradePrev;
		BYTE				cGradeCurrent;

		SNET_INVEN_GRINDING_FB () 
			: bRESET(false)
			, bTERMINATE(false)
			, dwGrindResult(0)
			, emGrindFB(EMGRINDING_FAIL)
			, cGradePrev	(0)
			, cGradeCurrent (0)
		{
			dwSize = sizeof(SNET_INVEN_GRINDING_FB);
			nType = NET_MSG_GCTRL_INVEN_GRINDING_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_GRINDING_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_PERIODEXTEND : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
		WORD                wHoldPosX;
		WORD                wHoldPosY;
		bool				bReconfirm;

		SNET_INVEN_PERIODEXTEND () 
			: wPosX(0)
			, wPosY(0)
			, wHoldPosX(0)
			, wHoldPosY(0)
			, bReconfirm(false)
		{
			dwSize = sizeof(SNET_INVEN_PERIODEXTEND);
			nType = NET_MSG_GCTRL_INVEN_PERIODEXTEND;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_PERIODEXTEND)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_PERIODEXTEND_FB : public NET_MSG_GENERIC
	{
		EMREQ_PERIODEXTENSION_FB emResult;
		WORD				wPosX;
		WORD				wPosY;
		WORD                wHoldPosX;
		WORD                wHoldPosY;
		WORD				wRemainCount;

		SNET_INVEN_PERIODEXTEND_FB (WORD posX
			, WORD posY
			, WORD holdPosX
			, WORD holdPosY) 
			: emResult(EMREQ_PERIODEX_FB_FAIL)
			, wPosX(posX)
			, wPosY(posY)
			, wHoldPosX(holdPosX)
			, wHoldPosY(holdPosY)
		{
			dwSize = sizeof(SNET_INVEN_PERIODEXTEND_FB);
			nType = NET_MSG_GCTRL_INVEN_PERIODEXTEND_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_PERIODEXTEND_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_BOXOPEN : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		SNET_INVEN_BOXOPEN () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_BOXOPEN);
			nType = NET_MSG_GCTRL_INVEN_BOXOPEN;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_BOXOPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_BOXOPEN_FB : public NET_MSG_GENERIC
	{
		

		EMREQ_BOXOPEN_FB	emFB;

		SNET_INVEN_BOXOPEN_FB () 
			: emFB(EMREQ_BOXOPEN_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_BOXOPEN_FB);
			nType = NET_MSG_GCTRL_INVEN_BOXOPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_BOXOPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_DISGUISE : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNET_INVEN_DISGUISE () 
			: wPosX(0)
			, wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_DISGUISE);
			nType = NET_MSG_GCTRL_INVEN_DISGUISE;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_DISGUISE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_DISGUISE_FB : public NET_MSG_GENERIC
	{
		

		EMREQ_DISGUISE_FB	emFB;

		SNET_INVEN_DISGUISE_FB () 
			: emFB(EMREQ_DISGUISE_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_DISGUISE_FB);
			nType = NET_MSG_GCTRL_INVEN_DISGUISE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_DISGUISE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_CLEANSER : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;
        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNET_INVEN_CLEANSER () 
			: wPosX(0)
			, wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_CLEANSER);
			nType = NET_MSG_GCTRL_INVEN_CLEANSER;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_CLEANSER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_CLEANSER_FB : public NET_MSG_GENERIC
	{
		

		EMREQ_CLEANSER_FB	emFB;

		SNET_INVEN_CLEANSER_FB () 
			: emFB(EMREQ_CLEANSER_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_CLEANSER_FB);
			nType = NET_MSG_GCTRL_INVEN_CLEANSER_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_CLEANSER_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_RESET_SKST : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;
		WORD				wAmount;
		EMITEM_TYPE			emItemType;

		WORD                wResetStats[EMSIZE];

		SNET_INVEN_RESET_SKST () 
			: wPosX      (0)
			, wPosY      (0)
			, wAmount    (0)
			, emItemType (ITEM_SUIT)
		{
			dwSize = sizeof(SNET_INVEN_RESET_SKST);
			nType = NET_MSG_GCTRL_INVEN_RESET_SKST;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_RESET_SKST)<=NET_DATA_BUFSIZE);
			memset(wResetStats, 0, sizeof(WORD)*EMSIZE);
		}
	};

	struct SNET_INVEN_RESET_SKST_FB : public NET_MSG_GENERIC
	{
		

		LONGLONG			llGameMoney;
		WORD				wSTATS_NUM;
		WORD				wSKILL_NUM;
		WORD                wResetStats[EMSIZE];
		BOOL				bRESETALL;

		EMREQ_RESET_SKST_FB	emFB;

		SNET_INVEN_RESET_SKST_FB () 
			: llGameMoney(0)
			, wSTATS_NUM(0)
			, wSKILL_NUM(0)
			, bRESETALL(FALSE)
			, emFB(EMREQ_RESET_SKST_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_RESET_SKST_FB);
			nType = NET_MSG_GCTRL_INVEN_RESET_SKST_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_RESET_SKST_FB)<=NET_DATA_BUFSIZE);
			memset(wResetStats, 0, sizeof(WORD)*EMSIZE);
		}
	};

	struct SNET_INVEN_DEL_ITEM_TIMELMT : public NET_MSG_GENERIC
	{
		

		SNATIVEID			nidITEM;

		SNET_INVEN_DEL_ITEM_TIMELMT () 
			: nidITEM(false)
		{
			dwSize = sizeof(SNET_INVEN_DEL_ITEM_TIMELMT);
			nType = NET_MSG_GCTRL_INVEN_DEL_ITEM_TIMELMT;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_DEL_ITEM_TIMELMT)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_INVEN_CHARCARD : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		SNET_INVEN_CHARCARD () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_CHARCARD);
			nType = NET_MSG_GCTRL_INVEN_CHARCARD;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_CHARCARD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_CHARCARD_FB : public NET_MSG_GENERIC
	{
		EMREQ_CHARCARD_FB		emFB;

		SNET_INVEN_CHARCARD_FB () 
			: emFB(EMREQ_CHARCARD_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_CHARCARD_FB);
			nType = NET_MSG_GCTRL_INVEN_CHARCARD_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_CHARCARD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_STORAGECARD : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		WORD				wSTORAGE;

		SNET_INVEN_STORAGECARD () 
			: wPosX(0)
			, wPosY(0)
			, wSTORAGE(0)
		{
			dwSize = sizeof(SNET_INVEN_STORAGECARD);
			nType = NET_MSG_GCTRL_INVEN_STORAGECARD;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_STORAGECARD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_STORAGECARD_FB : public NET_MSG_GENERIC
	{
		EMREQ_STORAGECARD_FB	emFB;

		WORD					wSTORAGE;
		__time64_t				tSTORAGE_LIMIT;
		__time64_t				tSPAN;

		SNET_INVEN_STORAGECARD_FB () 
			: emFB(EMREQ_STORAGECARD_FB_FAIL)
			, wSTORAGE(0)
			, tSTORAGE_LIMIT(0)
			, tSPAN(0)
		{
			dwSize = sizeof(SNET_INVEN_STORAGECARD_FB);
			nType = NET_MSG_GCTRL_INVEN_STORAGECARD_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_STORAGECARD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_INVENLINE : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		SNET_INVEN_INVENLINE () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_INVENLINE);
			nType = NET_MSG_GCTRL_INVEN_INVENLINE;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_INVENLINE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_INVENLINE_FB : public NET_MSG_GENERIC
	{
		

		WORD				wINVENLINE;
		EMREQ_INVENLINE_FB	emFB;

		SNET_INVEN_INVENLINE_FB () 
			: emFB(EMREQ_INVENLINE_FB_FAIL)
			, wINVENLINE(0)
		{
			dwSize = sizeof(SNET_INVEN_INVENLINE_FB);
			nType = NET_MSG_GCTRL_INVEN_INVENLINE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_INVENLINE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_REMODELOPEN : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
		WORD				wStampCardPosX;
		WORD				wStampCardPosY;

		SNET_INVEN_REMODELOPEN(void)
			: wStampCardPosX(USHRT_MAX)
			, wStampCardPosY(USHRT_MAX)
		{
			dwSize = sizeof(SNET_INVEN_REMODELOPEN);
			nType = NET_MSG_GCTRL_INVEN_REMODELOPEN;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_REMODELOPEN)<=NET_DATA_BUFSIZE);
		}
	};	

	struct SNET_INVEN_REMODELOPEN_FB : public NET_MSG_GENERIC
	{
		EMREQ_REMODELOPEN_FB	emFB;

		SNET_INVEN_REMODELOPEN_FB () 
			: emFB(EMREQ_REMODELOPEN_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_REMODELOPEN_FB);
			nType = NET_MSG_GCTRL_INVEN_REMODELOPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_REMODELOPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_GARBAGEOPEN : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		SNET_INVEN_GARBAGEOPEN () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_GARBAGEOPEN);
			nType = NET_MSG_GCTRL_INVEN_GARBAGEOPEN;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_GARBAGEOPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_GARBAGEOPEN_FB : public NET_MSG_GENERIC
	{
		EMREQ_GARBAGEOPEN_FB	emFB;

		SNET_INVEN_GARBAGEOPEN_FB () 
			: emFB(EMREQ_GARBAGEOPEN_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_GARBAGEOPEN_FB);
			nType = NET_MSG_GCTRL_INVEN_GARBAGEOPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_GARBAGEOPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_STORAGEOPEN : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		SNET_INVEN_STORAGEOPEN () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_STORAGEOPEN);
			nType = NET_MSG_GCTRL_INVEN_STORAGEOPEN;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_STORAGEOPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_STORAGEOPEN_FB : public NET_MSG_GENERIC
	{
		EMREQ_STORAGEOPEN_FB	emFB;
		WORD				wPosX;
		WORD				wPosY;
		
		SNET_INVEN_STORAGEOPEN_FB () 
			: emFB(EMREQ_STORAGEOPEN_FB_FAIL)
			, wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_STORAGEOPEN_FB);
			nType = NET_MSG_GCTRL_INVEN_STORAGEOPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_STORAGEOPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNET_INVEN_STORAGECLOSE : public NET_MSG_GENERIC
    {


        SNET_INVEN_STORAGECLOSE () 
        {
            dwSize = sizeof(SNET_INVEN_STORAGECLOSE);
            nType = NET_MSG_GCTRL_INVEN_STORAGECLOSE;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_STORAGECLOSE)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_INVEN_PMARKET_SEARCH_OPEN_CF : public NET_MSG_GENERIC
    {
        WORD				wPosX;
        WORD				wPosY;

        SNET_INVEN_PMARKET_SEARCH_OPEN_CF () 
            : wPosX(0)
            , wPosY(0)
        {
            dwSize = sizeof(SNET_INVEN_PMARKET_SEARCH_OPEN_CF);
            nType = NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_CF;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_PMARKET_SEARCH_OPEN_CF)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_INVEN_PMARKET_SEARCH_OPEN_FA : public NET_MSG_GENERIC
    {
        DWORD               dwGaeaID;
        WORD				wPosX;
        WORD				wPosY;

        SNET_INVEN_PMARKET_SEARCH_OPEN_FA ()
            : dwGaeaID(GAEAID_NULL)
            , wPosX(0)
            , wPosY(0)
        {
            dwSize = sizeof(SNET_INVEN_PMARKET_SEARCH_OPEN_FA);
            nType = NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_FA;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_PMARKET_SEARCH_OPEN_FA)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_INVEN_PMARKET_SEARCH_OPEN_AF : public NET_MSG_GENERIC
    {
        DWORD               dwGaeaID;
        WORD				wPosX;
        WORD				wPosY;

        SNET_INVEN_PMARKET_SEARCH_OPEN_AF () 
            : dwGaeaID(GAEAID_NULL)
            , wPosX(0)
            , wPosY(0)
        {
            dwSize = sizeof(SNET_INVEN_PMARKET_SEARCH_OPEN_AF);
            nType = NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_AF;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_PMARKET_SEARCH_OPEN_AF)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_INVEN_PMARKET_SEARCH_OPEN_FB : public NET_MSG_GENERIC
    {
        EMREQ_PMARKET_SEARCH_CARD_FB	emFB;
        WORD				wPosX;
        WORD				wPosY;

        SNET_INVEN_PMARKET_SEARCH_OPEN_FB () 
            : emFB(EMREQ_PMARKET_SEARCH_OPEN_FB_FAIL)
            , wPosX(0)
            , wPosY(0)
        {
            dwSize = sizeof(SNET_INVEN_PMARKET_SEARCH_OPEN_FB);
            nType = NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_PMARKET_SEARCH_OPEN_FB)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_INVEN_PMARKET_SEARCH_CLOSE : public NET_MSG_GENERIC
    {
        SNET_INVEN_PMARKET_SEARCH_CLOSE () 
        {
            dwSize = sizeof(SNET_INVEN_PMARKET_SEARCH_CLOSE);
            nType = NET_MSG_GCTRL_PMARKET_SEARCH_CLOSE;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_PMARKET_SEARCH_CLOSE)<=NET_DATA_BUFSIZE);
        }
    };

    //! Premium set
	struct SNET_INVEN_PREMIUMSET : public NET_MSG_GENERIC
	{
		WORD m_PosX;
		WORD m_PosY;

        SNET_INVEN_PREMIUMSET(WORD PosX, WORD PosY)
			: m_PosX(PosX)
			, m_PosY(PosY)
		{
			dwSize = sizeof(SNET_INVEN_PREMIUMSET);
			nType = NET_MSG_GCTRL_INVEN_PREMIUMSET;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_PREMIUMSET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_PREMIUMSET_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		__time64_t				tLMT;
		__time64_t				tSPAN;

		EMREQ_PREMIUMSET_FB		emFB;

		SNET_INVEN_PREMIUMSET_FB () 
			: emFB(EMREQ_PREMIUMSET_FB_FAIL)
			, tLMT(0)
			, tSPAN(0)
		{
			dwSize = sizeof(SNET_INVEN_PREMIUMSET_FB);
			nType = NET_MSG_GCTRL_INVEN_PREMIUMSET_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_PREMIUMSET_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_GET_CHARGED_ITEM_CF : public NET_MSG_GENERIC
	{
		//DWORD					dwCharID;
		//char					szUID[USR_ID_LENGTH+1];
        bool m_bFirstJoin;

		NET_GET_CHARGED_ITEM_CF(bool bFirstJoin)
            : m_bFirstJoin(bFirstJoin)
			//: dwCharID(0)
		{
			dwSize = sizeof(NET_GET_CHARGED_ITEM_CF);
			nType  = NET_MSG_GET_CHARGED_ITEM_CF;
			MIN_STATIC_ASSERT(sizeof(NET_GET_CHARGED_ITEM_CF)<=NET_DATA_BUFSIZE);
			//memset ( szUID, 0, sizeof(char)*(USR_ID_LENGTH+1) );
		}

        //void SetUserID(const char* szUserId)
        //{
        //    StringCchCopy(szUID, USR_ID_LENGTH+1, szUserId);
        //}
	};

	struct NET_GET_CHARGED_ITEM_FC : public NET_MSG_GENERIC
	{        
		EMREQ_CHARGEDITEM_FROMDB_FB emFB;
        SNATIVEID nidITEM;
		char	  szPurKey[PURKEY_LENGTH+1];

		NET_GET_CHARGED_ITEM_FC()
			: emFB(EMREQ_CHARGEDITEM_FROMDB_FB_END)
			, nidITEM(false)
		{
			dwSize = sizeof(NET_GET_CHARGED_ITEM_FC);
			nType  = NET_MSG_GET_CHARGED_ITEM_FC;
			MIN_STATIC_ASSERT(sizeof(NET_GET_CHARGED_ITEM_FC)<=NET_DATA_BUFSIZE);
			memset(szPurKey, 0, sizeof(char)*(PURKEY_LENGTH+1));
		}

        void SetType(EMREQ_CHARGEDITEM_FROMDB_FB Type)
        {
            emFB = Type;
        }

        void SetData(const std::string& PurKey, SNATIVEID ItemId)
        {
            StringCchCopy(szPurKey, PURKEY_LENGTH+1, PurKey.c_str());
            nidITEM = ItemId;
        }
	};

	struct SNET_CHARGED_ITEM_GET : public NET_MSG_GENERIC
	{
		DWORD					dwID;
		char					szPurKey[PURKEY_LENGTH+1];

		SNET_CHARGED_ITEM_GET () 
			: dwID(UINT_MAX)
		{
			dwSize = sizeof(SNET_CHARGED_ITEM_GET);
			nType = NET_MSG_GCTRL_CHARGED_ITEM_GET;
			MIN_STATIC_ASSERT(sizeof(SNET_CHARGED_ITEM_GET)<=NET_DATA_BUFSIZE);

			memset(szPurKey, 0, sizeof(char) * (PURKEY_LENGTH+1));
		}
	};

	struct SNET_CHARGED_ITEM_GET_FB : public NET_MSG_GENERIC
	{
		EMCHARGED_ITEM_GET_FB	emFB;
		SNATIVEID				nidITEM;

		SNET_CHARGED_ITEM_GET_FB () 
			: nidITEM(false)
			, emFB(EMCHARGED_ITEM_GET_FB_FAIL)
		{
			dwSize = sizeof(SNET_CHARGED_ITEM_GET_FB);
			nType = NET_MSG_GCTRL_CHARGED_ITEM_GET_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CHARGED_ITEM_GET_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CHARGED_ITEM_DEL : public NET_MSG_GENERIC
	{
		DWORD					dwID;

		SNET_CHARGED_ITEM_DEL () 
			: dwID(UINT_MAX)
		{
			dwSize = sizeof(SNET_CHARGED_ITEM_DEL);
			nType = NET_MSG_GCTRL_CHARGED_ITEM_DEL;
			MIN_STATIC_ASSERT(sizeof(SNET_CHARGED_ITEM_DEL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CHARGED_ITEM2_INVEN : public NET_MSG_GENERIC
	{
		DWORD					dwUserID;
		SNATIVEID				nidITEM;
		char					szPurKey[PURKEY_LENGTH+1];
		DWORD					dwInvenPos;


		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(SNATIVEID)+sizeof(char)*(PURKEY_LENGTH+1)+sizeof(DWORD) };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_CHARGED_ITEM2_INVEN () 
			: dwUserID(0)
			, nidITEM(false)
			, dwInvenPos(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_CHARGED_ITEM2_INVEN;
			MIN_STATIC_ASSERT(sizeof(SNET_CHARGED_ITEM2_INVEN)<=NET_DATA_BUFSIZE);
			memset(szPurKey, 0, sizeof(char) * (PURKEY_LENGTH+1));
		}
	};

	struct SNET_INVEN_LOCKBOX_OPEN : public NET_MSG_GENERIC
	{
		WORD					wPosX;
		WORD					wPosY;

		SNET_INVEN_LOCKBOX_OPEN () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_LOCKBOX_OPEN);
			nType = NET_MSG_GCTRL_INVEN_LOCKBOXOPEN;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_LOCKBOX_OPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_RANDOMBOXOPEN : public NET_MSG_GENERIC
	{
		WORD					wPosX;
		WORD					wPosY;

		SNET_INVEN_RANDOMBOXOPEN () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_RANDOMBOXOPEN);
			nType = NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_RANDOMBOXOPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_SELFORMBOX_SELECTOPEN : public NET_MSG_GENERIC
	{
		WORD nSelPos;
		WORD nBoxPosX;
		WORD nBoxPosY;
		SNET_INVEN_SELFORMBOX_SELECTOPEN()
			: nSelPos(-1)
			, nBoxPosX(-1)
			, nBoxPosY(-1)
		{
			dwSize = sizeof(SNET_INVEN_SELFORMBOX_SELECTOPEN);
			nType = NET_MSG_GCTRL_INVEN_SELFORMBOX_SELECTOPEN;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_SELFORMBOX_SELECTOPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_RANDOMBOXOPEN_FB : public NET_MSG_GENERIC
	{
		EMINVEN_RANDOMBOXOPEN_FB	emFB;

		SNET_INVEN_RANDOMBOXOPEN_FB () 
			: emFB(EMINVEN_RANDOMBOXOPEN_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_RANDOMBOXOPEN_FB);
			nType = NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_RANDOMBOXOPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_LOCKBOX_FB : public NET_MSG_GENERIC
	{
		EMINVEN_LOCKBOX_FB	emFB;
		SNATIVEID			nidITEM;

		SNET_INVEN_LOCKBOX_FB () 
			: emFB(EMINVEN_LOCKBOX_FB_FAIL)
			, nidITEM(SNATIVEID(false))
		{
			dwSize = sizeof(SNET_INVEN_LOCKBOX_FB);
			nType = NET_MSG_GCTRL_INVEN_LOCKBOX_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_LOCKBOX_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_SELFORMBOX_SELECTOPEN_FB : public NET_MSG_GENERIC
	{
		EMINVEN_SELFORMBOX_SELOPEN_FB	emFB;

		SNET_INVEN_SELFORMBOX_SELECTOPEN_FB () 
			: emFB(EMINVEN_SELFORMBOX_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_SELFORMBOX_SELECTOPEN_FB);
			nType = NET_MSG_GCTRL_INVEN_SELFORMBOX_SELECTOPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_SELFORMBOX_SELECTOPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_INVEN_DISJUNCTION : public NET_MSG_GENERIC
	{
		WORD					wPosX;
		WORD					wPosY;
        WORD                    wHoldPosX;
        WORD                    wHoldPosY;

		SNET_INVEN_DISJUNCTION () 
			: wPosX(0)
			, wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_DISJUNCTION);
			nType = NET_MSG_GCTRL_INVEN_DISJUNCTION;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_DISJUNCTION)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_DISJUNCTION_FB : public NET_MSG_GENERIC
	{
		EMINVEN_DISJUNCTION_FB		emFB;

		SNET_INVEN_DISJUNCTION_FB () 
			: emFB(EMINVEN_DISJUNCTION_FB_FAIL)
		{
			dwSize = sizeof(SNET_INVEN_DISJUNCTION_FB);
			nType = NET_MSG_GCTRL_INVEN_DISJUNCTION_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_DISJUNCTION_FB)<=NET_DATA_BUFSIZE);
		}
	};

	// ITEMREBUILD_MARK
	struct SNET_REBUILD_RESULT : public NET_MSG_GENERIC
	{
		ITEM_RANDOMOP_UTILL::FixOption fixOption;
		WORD				wFixBasicStatsIndex;
		DWORD				dwNpcGlobID;
		EMREBUILD_RESULT	emResult;
		SITEMCUSTOM sItemCustom;
		bool bUseStampCard;

		EMTYPE_ITEM_REMODEL emTypeRemodel;

		SNET_REBUILD_RESULT()			
			: emResult( EMREBUILD_RESULT_FAIL )
			, wFixBasicStatsIndex(0)
			, dwNpcGlobID(0)
			, bUseStampCard(false)
			, emTypeRemodel( EMTYPE_ITEM_REMODEL_NONE )
		{
			dwSize = sizeof( SNET_REBUILD_RESULT );
			nType = NET_MSG_REBUILD_RESULT;
			MIN_STATIC_ASSERT( sizeof(SNET_REBUILD_RESULT)<= NET_DATA_BUFSIZE );
		}
	};

	// ������
	struct SNET_GARBAGE_RESULT : public NET_MSG_GENERIC
	{
		
		WORD				wPosX;	// ������ ������ ��ǥ
		WORD				wPosY;	// ������ ������ ��ǥ

		SNET_GARBAGE_RESULT()
			: wPosX(EInvenX)
			, wPosY(EInvenTotalLine)
		{
			dwSize = sizeof( SNET_GARBAGE_RESULT );
			nType = NET_MSG_GCTRL_GARBAGE_RESULT;
			MIN_STATIC_ASSERT( sizeof(SNET_GARBAGE_RESULT)<= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_GARBAGE_RESULT_FB : public NET_MSG_GENERIC
	{
		
		EMGARBAGE_RESULT_FB	emResult;		

		SNET_GARBAGE_RESULT_FB()
			: emResult ( EMGARBAGE_RESULT_FB_FAIL )
		{
			dwSize = sizeof( SNET_GARBAGE_RESULT_FB );
			nType = NET_MSG_GCTRL_GARBAGE_RESULT_FB;
			MIN_STATIC_ASSERT( sizeof(SNET_GARBAGE_RESULT_FB)<= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_REBUILD_MOVE_ITEM : public NET_MSG_GENERIC
	{
		enum EMITEM_TYPE
		{
			EMITEM_EQUIPMENT,
			EMITEM_STAMP,
		};
		WORD			wPosX;
		WORD			wPosY;
		EMITEM_TYPE	emItemType;

		SNET_REBUILD_MOVE_ITEM(const EMITEM_TYPE _emItemType = EMITEM_EQUIPMENT)
			: wPosX( USHRT_MAX )
			, wPosY( USHRT_MAX )
			, emItemType(_emItemType)
		{
			dwSize = sizeof( SNET_REBUILD_MOVE_ITEM );
			nType = NET_MSG_REBUILD_MOVE_ITEM;
			MIN_STATIC_ASSERT( sizeof(SNET_REBUILD_MOVE_ITEM)<= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_REBUILD_COST_MONEY : public NET_MSG_GENERIC
	{
		LONGLONG		i64CostMoney;

		SNET_REBUILD_COST_MONEY()
			: i64CostMoney( 0 )
		{
			dwSize = sizeof( SNET_REBUILD_COST_MONEY );
			nType = NET_MSG_REBUILD_COST_MONEY;
			MIN_STATIC_ASSERT( sizeof(SNET_REBUILD_COST_MONEY)<= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_REBUILD_INPUT_MONEY : public NET_MSG_GENERIC
	{
		LONGLONG		i64InputMoney;

		SNET_REBUILD_INPUT_MONEY()
			: i64InputMoney( 0 )
		{
			dwSize = sizeof( SNET_REBUILD_INPUT_MONEY );
			nType = NET_MSG_REBUILD_INPUT_MONEY;
			MIN_STATIC_ASSERT( sizeof(SNET_REBUILD_INPUT_MONEY)<= NET_DATA_BUFSIZE );
		}
	};

    //---------------------------------------------------------------------------NET
	// !! �ش� �޼��� ����ü�� ���� ������� ����
	// �κ� Ȯ�忡 ���� �ش� ��Ŷ ����ü�� �ܼ� TEMP������ ����. ���� ��Ʈ��ũ �����ϸ� ����
    struct SNETPC_REQ_ITEM_SORT : public NET_MSG_GENERIC
    {
		WORD nMoveCount;
        bool bHasPile;
        SINVEN_MOVE_ITEM_SORT sMoveItem[ EInvenTotalSlot ];

        SNETPC_REQ_ITEM_SORT ()
            : nMoveCount( 0 )
            , bHasPile(false)
        {
            dwSize = sizeof( SNETPC_REQ_ITEM_SORT );
            nType = NET_MSG_GCTRL_REQ_ITEM_SORT;
			
			for( int i = 0; i < EInvenTotalSlot; i++ )
			{
				sMoveItem[i].nOrigin = -1;
				sMoveItem[i].nTarget = -1;
			}
        }

        bool ADD_MOVE_ITEM( WORD nOrigin, WORD nTarget )
        {
            if( nMoveCount >= EInvenTotalSlot )
                return false;

            sMoveItem[ nMoveCount ].nOrigin = nOrigin;
            sMoveItem[ nMoveCount ].nTarget = nTarget;

			++nMoveCount;

            CalcSize();

            return true;
        }

        void RESET()
        {
            nMoveCount = 0;

            CalcSize();
        }

		void CalcSize()
		{
			dwSize = sizeof( NET_MSG_GENERIC );
			dwSize += sizeof( WORD );
            dwSize += sizeof( bool );
			dwSize += sizeof( SINVEN_MOVE_ITEM_SORT ) * nMoveCount;
		}
    };


	struct SNETPC_REQ_ITEM_SORT_START : public NET_MSG_GENERIC
    {
        SNETPC_REQ_ITEM_SORT_START ()
        {
            dwSize = sizeof( SNETPC_REQ_ITEM_SORT_START );
            nType = NET_MSG_GCTRL_REQ_ITEM_SORT_START;
            MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_ITEM_SORT_START ) <= NET_DATA_BUFSIZE );
        }
    };

	struct SNETPC_REQ_ITEM_SORT_DATA : public NET_MSG_GENERIC
    {
		WORD nCurCount;
		WORD nStartIdx;
        SINVEN_MOVE_ITEM_SORT sMoveItem[ 100 ];

        SNETPC_REQ_ITEM_SORT_DATA ()
			: nCurCount( 0 )
			, nStartIdx( 0 )
        {
            dwSize = sizeof( SNETPC_REQ_ITEM_SORT_DATA );
            nType = NET_MSG_GCTRL_REQ_ITEM_SORT_DATA;

			RESET();

            MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_ITEM_SORT_DATA ) <= NET_DATA_BUFSIZE );
        }

        bool ADD_MOVE_ITEM( WORD nOrigin, WORD nTarget, WORD nItemIdx )
        {
            if( nCurCount >= 100 )
                return false;

            sMoveItem[ nItemIdx ].nOrigin = nOrigin;
            sMoveItem[ nItemIdx ].nTarget = nTarget;

			nCurCount++;

            return true;
        }

        void RESET()
        {
            nCurCount = 0;
			nStartIdx = 0;

			for( int i = 0; i < 100; i++ )
			{
				sMoveItem[i].nOrigin = -1;
				sMoveItem[i].nTarget = -1;
			}
        }
    };

	struct SNETPC_REQ_ITEM_SORT_END : public NET_MSG_GENERIC
    {
		WORD nTotalCount;
        bool bHasPile;

        SNETPC_REQ_ITEM_SORT_END ( WORD nCount, bool bPile )
        {
            dwSize = sizeof( SNETPC_REQ_ITEM_SORT_END );
            nType = NET_MSG_GCTRL_REQ_ITEM_SORT_END;

			nTotalCount = nCount;
			bHasPile = bPile;

            MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_ITEM_SORT_END ) <= NET_DATA_BUFSIZE );
        }
    };

    //---------------------------------------------------------------------------NET
    struct SNET_ITEM_SORT_FB : public NET_MSG_GENERIC
    {
		int nRetCode;

        SNET_ITEM_SORT_FB (int nCode)
        {
            dwSize = sizeof( NET_MSG_GENERIC );
            nType = NET_MSG_GCTRL_ITEM_SORT_FAIL;
			nRetCode = nCode;
            MIN_STATIC_ASSERT( sizeof( SNET_ITEM_SORT_FB ) <= NET_DATA_BUFSIZE );
        }
    };

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_VNGAIN_TO_HOLD : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_VNGAIN_TO_HOLD () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_VNGAIN_TO_HOLD);
			nType = NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_VNGAIN_TO_HOLD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_VNGAIN_EX_HOLD : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_VNGAIN_EX_HOLD () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_VNGAIN_EX_HOLD);
			nType = NET_MSG_GCTRL_REQ_VNGAIN_EX_HOLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_VNGAIN_EX_HOLD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_HOLD_TO_VNGAIN : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_HOLD_TO_VNGAIN () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_HOLD_TO_VNGAIN);
			nType = NET_MSG_GCTRL_REQ_HOLD_TO_VNGAIN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_HOLD_TO_VNGAIN)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_VNINVEN_TO_INVEN : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_VNINVEN_TO_INVEN () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_VNINVEN_TO_INVEN);
			nType = NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_VNINVEN_TO_INVEN)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_VNINVEN_TO_INVEN_FB : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;
		WORD				wNewPosX;
		WORD				wNewPosY;
		SINVENITEM			Data;

		SNETPC_REQ_VNINVEN_TO_INVEN_FB () 
			: wPosX(0)
			, wPosY(0)
			, wNewPosX(0)
			, wNewPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_VNINVEN_TO_INVEN_FB);
			nType = NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_VNINVEN_TO_INVEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_VNGAIN_INVEN_RESET : public NET_MSG_GENERIC
	{
		

	
		SNETPC_REQ_VNGAIN_INVEN_RESET () 
		{
			dwSize = sizeof(SNETPC_REQ_VNGAIN_INVEN_RESET);
			nType = NET_MSG_GCTRL_REQ_VNGAIN_INVEN_RESET;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_VNGAIN_INVEN_RESET)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_ITEM_COOLTIME_UPDATE : public NET_MSG_GENERIC
	{
		
		
		DWORD				dwID;
		DWORD				dwCoolID;
		__time64_t			tUseTime;
		__time64_t			tCoolTime;
		EMCOOL_TYPE			emCoolType;
		
		SNETPC_ITEM_COOLTIME_UPDATE () 
			: dwID(0)
			, dwCoolID( 0 )
			, tUseTime(0)
			, tCoolTime(0)
			, emCoolType(EMCOOL_ITEMID)
		{
			dwSize = sizeof(SNETPC_ITEM_COOLTIME_UPDATE);
			nType = NET_MSG_GCTRL_ITEM_COOLTIME_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ITEM_COOLTIME_UPDATE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ITEM_COOLTIME_ERROR : public NET_MSG_GENERIC
	{
		
		SNATIVEID			sNativeID;

		SNET_ITEM_COOLTIME_ERROR () 
			: sNativeID(false)
		{
			dwSize = sizeof(SNET_ITEM_COOLTIME_ERROR);
			nType = NET_MSG_GCTRL_ITEM_COOLTIME_ERROR;
			MIN_STATIC_ASSERT(sizeof(SNET_ITEM_COOLTIME_ERROR)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_INVEN_NPC_RECALL : public NET_MSG_GENERIC
	{
		

		WORD				wPosX;
		WORD				wPosY;


		SNET_INVEN_NPC_RECALL () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_NPC_RECALL);
			nType = NET_MSG_GCTRL_NPC_RECALL;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_NPC_RECALL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_NPC_RECALL_FB : public NET_MSG_GENERIC
	{
		EMREQ_NPC_RECALL_FB	emFB;

		SNET_INVEN_NPC_RECALL_FB () 
			: emFB(EMREQ_NPC_RECALL_FB_FAIL)		
		{
			dwSize = sizeof(SNET_INVEN_NPC_RECALL_FB);
			nType = NET_MSG_GCTRL_NPC_RECALL_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_NPC_RECALL_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_NPC_COMMISSION : public NET_MSG_GENERIC
	{
        enum { MAGIC_NUM=9734678, };

        int         m_MagicNum;
        int			nCHANNEL;
        DWORD		dwUserID;
        LONGLONG	lnCommission;
		DWORD		dwFieldID;
		DWORD		dwCharID;
        
		SNET_INVEN_NPC_COMMISSION () 
			: nCHANNEL(0)
			, dwFieldID(0)
			, dwCharID(0)
			, dwUserID(0)
			, lnCommission(0)
            , m_MagicNum(MAGIC_NUM)
		{
			dwSize = sizeof(SNET_INVEN_NPC_COMMISSION);
			nType = NET_MSG_GCTRL_NPC_COMMISSION;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_NPC_COMMISSION)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_NPC_COMMISSION_FB : public NET_MSG_GENERIC
	{
        enum { MAGIC_NUM=9329987, };

		int m_MagicNum;
        EMREQ_NPC_COMMISSION_FB	emFB;
		DWORD		dwCharID;
		DWORD		dwUserID;
		LONGLONG	lnCommission;
        
		SNET_INVEN_NPC_COMMISSION_FB() 
			: emFB(EMREQ_NPC_COMMISSION_FAIL)
			, dwCharID(0)
			, dwUserID(0)
			, lnCommission(0)
            , m_MagicNum(MAGIC_NUM)
		{
			dwSize = sizeof(SNET_INVEN_NPC_COMMISSION_FB);
			nType = NET_MSG_GCTRL_NPC_COMMISSION_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_NPC_COMMISSION_FB)<=NET_DATA_BUFSIZE);
		}
	};

	// ������ ����
	struct SNET_INVEN_ITEM_MIX : public NET_MSG_GENERIC
	{
		DWORD				dwKey;
		SINVEN_POS			sInvenPos[ITEMMIX_ITEMNUM];
		DWORD				dwNpcGlobID;

		SNET_INVEN_ITEM_MIX()
			: dwKey( UINT_MAX )
			, dwNpcGlobID( UINT_MAX )
		{
			dwSize = sizeof( SNET_INVEN_ITEM_MIX );
			nType = NET_MSG_GCTRL_INVEN_ITEM_MIX;
			MIN_STATIC_ASSERT( sizeof(SNET_INVEN_ITEM_MIX)<= NET_DATA_BUFSIZE );
		}
	};

	struct NET_INVEN_ITEM_MIX_FC : public NET_MSG_GENERIC
	{
		EMITEM_MIX_FB emFB;
		SNATIVEID	  sNativeID;
		WORD		  wTurnNum;

		NET_INVEN_ITEM_MIX_FC()
			: emFB(EMITEM_MIX_FB_FAIL)
			, sNativeID(false)
			, wTurnNum(0)
		{
			dwSize = sizeof(NET_INVEN_ITEM_MIX_FC);
			nType = NET_MSG_INVEN_ITEM_MIX_FC;
            MIN_STATIC_ASSERT(sizeof(NET_INVEN_ITEM_MIX_FC)<= NET_DATA_BUFSIZE);
		}

        inline void SetResult(EMITEM_MIX_FB FB)
        {
            emFB = FB;
        }

        inline void SetData(EMITEM_MIX_FB FB, const SNATIVEID& NativeID, WORD TurnNum)
        {
            emFB = FB;
            sNativeID = NativeID;
            wTurnNum = TurnNum;
        }
	};

	struct SNETPC_REQ_INVEN_STAGE_PASS_CF: public NET_MSG_GENERIC
	{

		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_INVEN_STAGE_PASS_CF () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_STAGE_PASS_CF);
			nType = NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_CF;
		}
	};

	struct SNETPC_REQ_INVEN_STAGE_PASS_AF//: public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
		DWORD				dwCharDbNum;

		MSGPACK_DEFINE(wPosX, wPosY, dwCharDbNum);
		SNETPC_REQ_INVEN_STAGE_PASS_AF() : wPosX(0), wPosY(0), dwCharDbNum(0)
		{
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_STAGE_PASS_AF)<=NET_DATA_BUFSIZE);
			//nType = NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_AF;
		}

	};

	struct SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_CF: public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;

		SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_CF () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_CF);
			nType = NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_CF;
		}
	
	};

	struct SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FA
	{
		WORD				wPosX;
		WORD				wPosY;
		DWORD				dwCharDbNum;
		DWORD				dwMapID;
		int					nLevel;
		MSGPACK_DEFINE(dwCharDbNum, dwMapID, nLevel, wPosX, wPosY);
		SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FA() : wPosX(0), wPosY(0), dwCharDbNum(0), dwMapID(0), nLevel(0)
		{
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FA)<=NET_DATA_BUFSIZE);
			//nType = NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FA
		}
	};

	struct SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_AF//: public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
		DWORD				dwCharDbNum;
		bool				bOk;

		MSGPACK_DEFINE(wPosX, wPosY, dwCharDbNum, bOk);
		SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_AF() : wPosX(0), wPosY(0), dwCharDbNum(0), bOk(false)
		{
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_AF)<=NET_DATA_BUFSIZE);
			//nType = NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_AF;
		}

	};

	struct SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FC: public NET_MSG_GENERIC
	{
		DWORD				dwMapID;
		bool				bOk;

		SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FC () 
			: dwMapID(0)
			, bOk(false)
		{
			dwSize = sizeof(SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FC);
			nType = NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FC;
		}

	};


    struct SNETPC_REQ_CONSIGNEMENT_SLOT_EXPIRATION : public NET_MSG_GENERIC
    {
        WORD				wPosX;
        WORD				wPosY;

        SNETPC_REQ_CONSIGNEMENT_SLOT_EXPIRATION () 
            : wPosX(0)
            , wPosY(0)
        {
            dwSize = sizeof(SNETPC_REQ_CONSIGNEMENT_SLOT_EXPIRATION);
            nType = NET_MSG_GCTRL_INVEN_CONSIGNEMENT_SLOT_EXPIRATION_REQ;
        }

    };

    struct SNET_INVEN_BOX_WRAPPING : public NET_MSG_GENERIC
    {
        WORD				wPosX;      // ������ ����;
        WORD				wPosY;
        WORD                wHoldPosX;  // ������;
        WORD                wHoldPosY;

        SNET_INVEN_BOX_WRAPPING () 
            : wPosX(0)
            , wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
        {
            dwSize = sizeof(SNET_INVEN_BOX_WRAPPING);
            nType = NET_MSG_GCTRL_INVEN_BOX_WRAPPING;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_BOX_WRAPPING)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_INVEN_BOX_WRAPPING_FB : public NET_MSG_GENERIC
    {
        EMREQ_BOX_WRAPPING_FB	emFB;

        SNET_INVEN_BOX_WRAPPING_FB () 
            : emFB(EMREQ_BOX_WRAPPING_FB_FAILED)
        {
            dwSize = sizeof(SNET_INVEN_BOX_WRAPPING_FB);
            nType = NET_MSG_GCTRL_INVEN_BOX_WRAPPING_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_BOX_WRAPPING_FB)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_INVEN_BOX_UNWRAPPING : public NET_MSG_GENERIC
    {
        WORD				wPosX;      // ������ Ǯ ����;
        WORD				wPosY;

        SNET_INVEN_BOX_UNWRAPPING () 
            : wPosX(0)
            , wPosY(0)
        {
            dwSize = sizeof(SNET_INVEN_BOX_UNWRAPPING);
            nType = NET_MSG_GCTRL_INVEN_BOX_UNWRAPPING;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_BOX_UNWRAPPING)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNET_INVEN_BOX_UNWRAPPING_FB : public NET_MSG_GENERIC
    {
        EMREQ_BOX_UNWRAPPING_FB	emFB;

        SNET_INVEN_BOX_UNWRAPPING_FB () 
            : emFB(EMREQ_BOX_UNWRAPPING_FB_FAILED)
        {
            dwSize = sizeof(SNET_INVEN_BOX_UNWRAPPING_FB);
            nType = NET_MSG_GCTRL_INVEN_BOX_UNWRAPPING_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_INVEN_BOX_UNWRAPPING_FB)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNET_INVEN_COSTUME_STATS : public NET_MSG_GENERIC
	{
		struct SINVESTINFO
		{
			COSTUME_STATS	eStatsNum;
			WORD			wInvestPt;

			SINVESTINFO() : eStatsNum( E_NONE ), wInvestPt( 0 ) { }
		};

		WORD				wPosX;
		WORD				wPosY;
		WORD                wHoldPosX;
		WORD                wHoldPosY;

		SINVESTINFO			sInvestInfo[COSTUMEUSERVALUE::sCostumeUser::MAX_COS_SIZE];

		SNET_INVEN_COSTUME_STATS () 
			: wPosX(0)
			, wPosY(0)
			, wHoldPosX(0)
			, wHoldPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_COSTUME_STATS);
			nType = NET_MSG_GCTRL_INVEN_COSTUME_STATS;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_COSTUME_STATS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_COSTUME_STATS_FB : public NET_MSG_GENERIC
	{
		EMREQ_COSTUME_STATS_FB	emFB;

		SNATIVEID sItemID;

		SNET_INVEN_COSTUME_STATS_FB () 
			: emFB( EMREQ_CS_FB_SUCCESS ), sItemID(SNATIVEID(0,0))
		{
			dwSize = sizeof(SNET_INVEN_COSTUME_STATS_FB);
			nType = NET_MSG_GCTRL_INVEN_COSTUME_STATS_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_COSTUME_STATS_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_DRUG_FB : public SNETCROW_BROAD
	{
		EMREQ_DRUG_CONSUME_FB	emFB;

		SNATIVEID sItemID;

		SNET_DRUG_FB () 
			: emFB( EMREQ_DRUG_CONSUME_SUCCESS ), sItemID(SNATIVEID(0,0))
		{
			dwSize = sizeof(SNET_DRUG_FB);
			nType = NET_MSG_DRUG_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_DRUG_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_COSTUME_RELEASE : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
		WORD                wHoldPosX;
		WORD                wHoldPosY;

		unsigned int Crc32;


		SNET_INVEN_COSTUME_RELEASE () 
			: wPosX(0)
			, wPosY(0)
			, wHoldPosX(0)
			, wHoldPosY(0)
		{
			dwSize = sizeof(SNET_INVEN_COSTUME_RELEASE);
			nType = NET_MSG_GCTRL_INVEN_COSTUEM_RELEASE;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_COSTUME_RELEASE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INVEN_COSTUME_RELEASE_FB : public NET_MSG_GENERIC
	{
		EMREQ_COSTUME_STAT_RELEASE_FB	emFB;

		SNATIVEID sItemID;

		SNET_INVEN_COSTUME_RELEASE_FB () 
			: emFB( EMREQ_CS_RELEASE_FB_SUCCESS ), sItemID(SNATIVEID(0,0))
		{
			dwSize = sizeof(SNET_INVEN_COSTUME_RELEASE_FB);
			nType = NET_MSG_GCTRL_INVEN_COSTUEM_RELEASE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_INVEN_COSTUME_RELEASE_FB)<=NET_DATA_BUFSIZE);
		}
	};


	// ------------------------------------------------------------------------------ //
	// ������ ��ų ���� ī�� UI OPEN;
	struct SNET_INVEN_CHANGE_ITEMSKILL_UIOPEN : public NET_MSG_GENERIC
	{
		SNET_INVEN_CHANGE_ITEMSKILL_UIOPEN ( void )
		{
			dwSize = sizeof( SNET_INVEN_CHANGE_ITEMSKILL_UIOPEN );
			nType = NET_MSG_INVEN_CHANGE_ITEMSKILL_UIOPEN_CF;
			MIN_STATIC_ASSERT( sizeof( SNET_INVEN_CHANGE_ITEMSKILL_UIOPEN ) <= NET_DATA_BUFSIZE );
		}
	};	

	// ������ ��ų ���� ī�� ���� ó�� ���� �޽���;
	struct SNET_INVEN_CHANGE_ITEMSKILL_FB : public NET_MSG_GENERIC
	{
		EMCHANGE_ITEMSKILL_FB emFlag;

		SNET_INVEN_CHANGE_ITEMSKILL_FB () 
			: emFlag ( EMCHANGE_ITEMSKILL_FB_FAIL )
		{
			dwSize = sizeof( SNET_INVEN_CHANGE_ITEMSKILL_FB );
			nType = NET_MSG_INVEN_CHANGE_ITEMSKILL_UIOPEN_FB;
			MIN_STATIC_ASSERT( sizeof( SNET_INVEN_CHANGE_ITEMSKILL_FB ) <= NET_DATA_BUFSIZE );
		}
	};


	// ------------------------------------------------------------------------------ //
	// ������ �⺻ �ɷ�ġ ���� ī�� UI OPEN;
	struct SNET_INVEN_CHANGE_BASICSTATS_UIOPEN : public NET_MSG_GENERIC
	{
		SNET_INVEN_CHANGE_BASICSTATS_UIOPEN ( void )
		{
			dwSize = sizeof( SNET_INVEN_CHANGE_BASICSTATS_UIOPEN );
			nType = NET_MSG_INVEN_CHANGE_BASICSTATS_UIOPEN_CF;
			MIN_STATIC_ASSERT( sizeof( SNET_INVEN_CHANGE_BASICSTATS_UIOPEN ) <= NET_DATA_BUFSIZE );
		}
	};	

	// ������ �⺻ �ɷ�ġ ���� ī�� ���� ó�� ���� �޽���;
	struct SNET_INVEN_CHANGE_BASICSTATS_FB : public NET_MSG_GENERIC
	{
		EMCHANGE_BASICSTATS_FB emFlag;

		SNET_INVEN_CHANGE_BASICSTATS_FB () 
			: emFlag ( EMCHANGE_BASICSTATS_FB_FAIL )
		{
			dwSize = sizeof( SNET_INVEN_CHANGE_BASICSTATS_FB );
			nType = NET_MSG_INVEN_CHANGE_BASICSTATS_UIOPEN_FB;
			MIN_STATIC_ASSERT( sizeof( SNET_INVEN_CHANGE_BASICSTATS_FB ) <= NET_DATA_BUFSIZE );
		}
	};


	// ------------------------------------------------------------------------------ //
	// ������ ���� �ɷ�ġ ���� ī�� UI OPEN;
	struct SNET_INVEN_CHANGE_ADDSTATS_UIOPEN : public NET_MSG_GENERIC
	{
		SNET_INVEN_CHANGE_ADDSTATS_UIOPEN ( void )
		{
			dwSize = sizeof( SNET_INVEN_CHANGE_ADDSTATS_UIOPEN );
			nType = NET_MSG_INVEN_CHANGE_ADDSTATS_UIOPEN_CF;
			MIN_STATIC_ASSERT( sizeof( SNET_INVEN_CHANGE_ADDSTATS_UIOPEN ) <= NET_DATA_BUFSIZE );
		}
	};	

	// ������ ���� �ɷ�ġ ���� ī�� ���� ó�� ���� �޽���;
	struct SNET_INVEN_CHANGE_ADDSTATS_FB : public NET_MSG_GENERIC
	{
		EMCHANGE_ADDSTATS_FB emFlag;

		SNET_INVEN_CHANGE_ADDSTATS_FB () 
			: emFlag ( EMCHANGE_ADDSTATS_FB_FAIL )
		{
			dwSize = sizeof( SNET_INVEN_CHANGE_ADDSTATS_FB );
			nType = NET_MSG_INVEN_CHANGE_ADDSTATS_UIOPEN_FB;
			MIN_STATIC_ASSERT( sizeof( SNET_INVEN_CHANGE_ADDSTATS_FB ) <= NET_DATA_BUFSIZE );
		}
	};


	// ------------------------------------------------------------------------------ //

	// Revert to default structure packing
	#pragma pack()
};
