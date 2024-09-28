#pragma once

#include "../Widget/GLWidgetScript.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanGfxUI/GfxDef.h"

class GLCharacter;

struct STEMP_SUIT_PUTON
{
	EMSUIT emSuit;
	int nCountPutOn;

	STEMP_SUIT_PUTON ( void )
		: emSuit ( SUIT_HEADGEAR )
		, nCountPutOn ( 0 )
	{

	}
};

class GLItemTooltip
{
private :
	GLItemTooltip();
	GLItemTooltip( const GLItemTooltip& value );
	~GLItemTooltip();

public :
	static GLItemTooltip& GetInstance()
	{
		static GLItemTooltip Instance;
		return Instance;
	}

private :
	enum ITEM_TOOLTIP_STATE_BLOW
	{
		ITEM_TOOLTIP_VALUE				= 0,
		ITEM_TOOLTIP_VALUE_COUNT_1_VAR1 = 1,
		ITEM_TOOLTIP_VALUE_COUNT_1_VAR2 = 2,
		ITEM_TOOLTIP_VALUE_COUNT_2		= 3,
	};

private :
	GLCharacter* m_pCharacter;

	GLCHARLOGIC* m_pOtherCharData;

	SITEM* m_pItemData;
	SITEM* m_pItemDisguise;

	SITEMCUSTOM m_sItemCustom;

	float	m_fDurability;
	WORD	m_wInvenPosX;
	WORD	m_wInvenPosY;

	float m_fDurabilityValue[DURABILITY_APPLY_COUNT];

public:
	inline void SetOtherCharData ( GLCHARLOGIC* _pOtherCharData ) { m_pOtherCharData = _pOtherCharData; }
	inline const bool IsReferTooltip ( void ) const { if ( NULL == m_pOtherCharData ) { return false; } return true; }

private :
	void CutZero( CString& cstr, int ncount );
	void ResetDurabilityValue( float fResetValue = 1.0f );

public :
	bool GetItemDataHeader( const SITEMCUSTOM& sItemCustom, LuaTable& tb );
	bool GetItemDataInfo( const SITEMCUSTOM& sItemCustom, LuaTable& tb, int nType, bool bSimple );

	bool GetItemDurabilityHeader(const SITEMCUSTOM& sItemCustom, LuaTable& tb, int nType, bool bSimple);
	bool GetItemDurability(const SITEMCUSTOM& sItemCustom, LuaTable& tb, int nType, bool bSimple);

	void SetInvenPos( WORD wPosX, WORD wPosY );

	const SITEM* GetItemData() const { return m_pItemData; }

// Ŭ����, ����, �п� �������� ��� ������ ������ �����ϰ� ������������ ����;
// �ʿ�� GenSchooló�� �ܺο��� ���ڹ޾Ƽ� ����Ҽ� �ֵ�������;
public:
	// ������ ������ ���� ��� ����( ����(O), �ڽ�Ƭ(x) )
	void Durability_Header( LuaTable& tb , float fDurability );
	void Durability_MakeBasic( LuaTable& tb , float fDurability );
	void Durability_MakeRandom( LuaTable& tb, float fDurability  );
	void Durability_MakeAdd( LuaTable& tb   , float fDurability  );
	
	// ������ ��ų��ũ
	void GenItemSkillLink( LuaTable& tb );
	// �̸�
	void GenName( LuaTable& tb );
	// ���	
	void GenGrade( LuaTable& tb );
	// �б�
	void GenSchool( DWORD dwReqSchool, LuaTable& tb );
	// Ŭ����
	void GenClass( DWORD dwReqCharClass, LuaTable& tb );
	// ����
	void GenSex( DWORD dwReqCharClass, LuaTable& tb );
	// ����
	void GenLevel( LuaTable& tb );
	// ����
	void GenStat( LuaTable& tb );
	// Ÿ������(����, ���� ��)
	void GenTypeData( LuaTable& tb );
	// ������
	void GenDurability( LuaTable& tb );
	// ������
	void GenDamage( LuaTable& tb );
	// ����
	void GenDefence( LuaTable& tb );
	// ���߷�
	void GenHitRate( LuaTable& tb );
	// ȸ����
	void GenAvoidRate( LuaTable& tb );
	// �Ҹ�sp
	void GenCostSp( LuaTable& tb );
	// ������
	void GenPileNum(LuaTable& tb, int nType);
	// ��ǰ�� ���ȿ��
	void GenDrugUseEffect( LuaTable& tb );
	// ���� �ð�
	void GenCoolTime( LuaTable& tb );
	// ���� ��ġ
	void GenAddOpt( LuaTable& tb, bool bSimple );
	// ����ġ ����
	void GenExp( LuaTable& tb );
	// ����
	void GenResist( LuaTable& tb );
	// �����̻�
	void GenStateBlow( LuaTable& tb );
	// ������ ����
	void GenExplain( LuaTable& tb );
	// ������ �߰� ���� ( ����ġ ����� �� ĸ���� ���� �߰� ����)
	void GenExplainEx( LuaTable& tb );
	// �ŷ�
	void GenThrowTradeSale( LuaTable& tb );
	// �춧, �ȶ� ����
	void GenPrice( LuaTable& tb, int nType );
	// �䱸 ����Ʈ
	void GenActPoint( LuaTable& tb );
	// �䱸 �⿩��
	void GenContribution( LuaTable& tb );
	// �Ⱓ
	void GenTime( LuaTable& tb );
	// ���ø���Ʈ ��ó
	void GenWishlist(LuaTable& tb, int nType);
	// ��Ʈ������
	void GenSetItem( LuaTable& tb );
	// ��Ʈ������( �̸� )
	void GenSetItemName( LuaTable& tb,
		const SSETITEM* pSetItem, 
		const SSETITEMSEARCHINFO& sInfoPutOnSetItem );	
	// ��Ʈ������( ���� ) 	
	void GenSetItemParts( LuaTable& tb,
		const SSETITEM* pSetItem, 
		const SSETITEMSEARCHINFO& sInfoPutOnSetItem );		
	// ��Ʈ������( ȿ�� )
	void GenSetItemBonus( LuaTable& tb,
		const SSETITEM* pSetItem, 
		const SSETITEMSEARCHINFO& sInfoPutOnSetItem );	
	// ���� �ɼ�
	void GenRandomOpt( LuaTable& tb );
	// ������ �̸�����
	void GenPreview( LuaTable& tb );
	// ��ȯ ������
	void GenTradeItem( LuaTable& tb, int nType );

	void Gen_CostumeState( LuaTable& tb, bool bSimple );

	// ��ȯ ����
	void GenDrugCall( LuaTable& tb );

private :
	// Ÿ�Կ� ���� �Ʒ� �߿� �ϳ� ����
	void GenType( LuaTable& tb );

	// ��ų������
	void GetSkillItem( LuaTable& tb );
	// �� ��ų
	void GetPetSkillItem( LuaTable& tb );
	// Ż ��
	void GetVehicleItem( LuaTable& tb );
	// �� ī��
	void GetPetCardItem( LuaTable& tb );
	// ��ġ�ڽ�
	void GetLunchBoxItem( LuaTable& tb );
	// �� ���ų
	void GetPetDualSkillItem( LuaTable& tb );
	// ����Ʈ ī��
	void GetPointCard( LuaTable& tb );
	// ���� ����, ���� ����;
	void GetBox( LuaTable& tb );
};
