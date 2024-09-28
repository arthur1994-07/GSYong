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

// 클래스, 레벨, 학원 툴팁등의 경우 아이템 툴팁을 제외하고도 여러군데에서 쓴다;
// 필요시 GenSchool처럼 외부에서 인자받아서 사용할수 있도록하자;
public:
	// 아이템 내구도 툴팁 출력 별도( 무기(O), 코스튬(x) )
	void Durability_Header( LuaTable& tb , float fDurability );
	void Durability_MakeBasic( LuaTable& tb , float fDurability );
	void Durability_MakeRandom( LuaTable& tb, float fDurability  );
	void Durability_MakeAdd( LuaTable& tb   , float fDurability  );
	
	// 아이템 스킬링크
	void GenItemSkillLink( LuaTable& tb );
	// 이름
	void GenName( LuaTable& tb );
	// 등급	
	void GenGrade( LuaTable& tb );
	// 학교
	void GenSchool( DWORD dwReqSchool, LuaTable& tb );
	// 클래스
	void GenClass( DWORD dwReqCharClass, LuaTable& tb );
	// 성별
	void GenSex( DWORD dwReqCharClass, LuaTable& tb );
	// 레벨
	void GenLevel( LuaTable& tb );
	// 스탯
	void GenStat( LuaTable& tb );
	// 타입정보(무기, 복장 등)
	void GenTypeData( LuaTable& tb );
	// 내구도
	void GenDurability( LuaTable& tb );
	// 데미지
	void GenDamage( LuaTable& tb );
	// 방어력
	void GenDefence( LuaTable& tb );
	// 명중률
	void GenHitRate( LuaTable& tb );
	// 회피율
	void GenAvoidRate( LuaTable& tb );
	// 소모sp
	void GenCostSp( LuaTable& tb );
	// 소지량
	void GenPileNum(LuaTable& tb, int nType);
	// 약품류 사용효과
	void GenDrugUseEffect( LuaTable& tb );
	// 재사용 시간
	void GenCoolTime( LuaTable& tb );
	// 가산 수치
	void GenAddOpt( LuaTable& tb, bool bSimple );
	// 경험치 배율
	void GenExp( LuaTable& tb );
	// 저항
	void GenResist( LuaTable& tb );
	// 상태이상
	void GenStateBlow( LuaTable& tb );
	// 아이템 설명
	void GenExplain( LuaTable& tb );
	// 아이템 추가 설명 ( 경험치 압축기 및 캡슐을 위한 추가 설명)
	void GenExplainEx( LuaTable& tb );
	// 거래
	void GenThrowTradeSale( LuaTable& tb );
	// 살때, 팔때 가격
	void GenPrice( LuaTable& tb, int nType );
	// 요구 포인트
	void GenActPoint( LuaTable& tb );
	// 요구 기여도
	void GenContribution( LuaTable& tb );
	// 기간
	void GenTime( LuaTable& tb );
	// 위시리스트 출처
	void GenWishlist(LuaTable& tb, int nType);
	// 세트아이템
	void GenSetItem( LuaTable& tb );
	// 세트아이템( 이름 )
	void GenSetItemName( LuaTable& tb,
		const SSETITEM* pSetItem, 
		const SSETITEMSEARCHINFO& sInfoPutOnSetItem );	
	// 세트아이템( 부위 ) 	
	void GenSetItemParts( LuaTable& tb,
		const SSETITEM* pSetItem, 
		const SSETITEMSEARCHINFO& sInfoPutOnSetItem );		
	// 세트아이템( 효과 )
	void GenSetItemBonus( LuaTable& tb,
		const SSETITEM* pSetItem, 
		const SSETITEMSEARCHINFO& sInfoPutOnSetItem );	
	// 랜덤 옵션
	void GenRandomOpt( LuaTable& tb );
	// 아이템 미리보기
	void GenPreview( LuaTable& tb );
	// 교환 아이템
	void GenTradeItem( LuaTable& tb, int nType );

	void Gen_CostumeState( LuaTable& tb, bool bSimple );

	// 귀환 종류
	void GenDrugCall( LuaTable& tb );

private :
	// 타입에 따라서 아래 중에 하나 실행
	void GenType( LuaTable& tb );

	// 스킬아이템
	void GetSkillItem( LuaTable& tb );
	// 펫 스킬
	void GetPetSkillItem( LuaTable& tb );
	// 탈 것
	void GetVehicleItem( LuaTable& tb );
	// 펫 카드
	void GetPetCardItem( LuaTable& tb );
	// 런치박스
	void GetLunchBoxItem( LuaTable& tb );
	// 펫 듀얼스킬
	void GetPetDualSkillItem( LuaTable& tb );
	// 포인트 카드
	void GetPointCard( LuaTable& tb );
	// 선물 상자, 랜덤 상자;
	void GetBox( LuaTable& tb );
};
