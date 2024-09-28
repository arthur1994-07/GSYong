#pragma once

#include "../Core/CLIKDataBinding.h"
#include "../../RanLogic/Item/GLItemCustom.h"

#include "../GfxDef.h"

namespace NSGFX_SLOT
{
	void CreateSlot_Item( GFx::Movie* pMovie, GFx::Value* pValue, const SITEMCUSTOM& sItem, bool bNew = false, EMGFX_SLOT_TYPE emSlotType = EMGFX_SLOT_BASE, const bool _bReferChar = false );
	void CreateSlot_Skill( GFx::Movie* pMovie, GFx::Value* pValue, const SNATIVEID& sID );
	void CreateSlot_SummonState( GFx::Movie* pMovie, GFx::Value* pValue, std::string strIconPath, INT nIconIdxX, INT nIconIdxY);
}

//-----------------------------------------------------------------------------------------
// 슬롯 베이스 클래스
// 타입별로 상속받아 정의하자.
class GfxSlotBase : public CLIKDataWrapper
{
public :
	GfxSlotBase() {}
	virtual ~GfxSlotBase() {}

protected :
	std::string m_strBindingID;

public :
	virtual EMGFX_SLOT_TYPE GetBaseType() { return EMGFX_SLOT_BASE; }
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_BASE; }

public :
	virtual void OnAddHandler( const char* binding, const CLIKDataBindingHandlerDesc& desc );

public :
	// 슬롯 데이터 갱신
	virtual void GetValue( GFx::Movie* pMovie, GFx::Value* pValue ) = 0;
	// 슬롯 데이터 리셋
	virtual void Reset() = 0;
	// 툴팁(HTML), 루아에서 텍스트를 만들도록 루아 함수 호출
	virtual bool GetTooltip( GFx::Value* pTooltip ) = 0;
	// 서브 툴팁
	virtual bool GetSubTooltip(GFx::Value* pTooltipInfo) = 0;
	// 툴팁 추가 슬롯 개수
	virtual bool GetAddSlotCount( int& nCount );
	// 툴팁 추가 슬롯 바인딩 ID
	virtual bool GetAddSlotBinding( std::string& strBinding );
	// 툴팁 교환 슬롯 개수
	virtual bool GetTradeSlotCount( int& nCount );
	// 툴팁 교환 슬롯 바인딩 ID
	virtual bool GetTradeSlotBinding( std::string& strBinding );

public :
	// 홀드 시작
	virtual void HoldStart( bool bLock ) = 0;
	// 드랍 끝나고 전처리, 드래그로 이동당할 슬롯(타겟 슬롯)이 날아옴. (false 반환시 PostDropSlotProc은 처리하지 않는다.)
	virtual bool PreDropSlotProc( GfxSlotBase* pTargetSlot, bool bHoldLock, bool bTargetLock ) = 0;
	// 드랍 끝나고 후처리, 드래그로 이동시킨 슬롯(홀드 슬롯)이 날아옴.
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock ) = 0;
	// 드래그 처리
	virtual void DragSlotProc( GfxSlotBase* pTargetSlot ) = 0;
	// 왼쪽 마우스 클릭
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) = 0;
	// 오른쪽 마우스 클릭
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) = 0;
};

//-----------------------------------------------------------------------------------------
// 툴팁 슬롯
class GfxTooltipSlot : public GfxSlotBase
{
public :
	GfxTooltipSlot();
	virtual ~GfxTooltipSlot();

protected :
	GfxSlotBase* m_pSlot;
	// 슬롯에서 출력된 툴팁이 아니라면 아래 아이디를 참조함;
	// 출처가 애매모호하다;
	// 최상위 부모 슬롯 타입으로 분별하기도 애매함;
	// 아이템 슬롯에서 스킬 정보 출력해야하는 경우도 있음;
	// 제일 깔끔한건 툴팁타입으로 확인하는게 좋음!;
	EMGFX_TOOLTIP_TYPE	m_emParentTooltipType;
	SNATIVEID			m_sNativeID;

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_TOOLTIP; }

public :
	virtual void GetValue( GFx::Movie* pMovie, GFx::Value* pValue );
	virtual void Reset() {}
	virtual bool GetTooltip( GFx::Value* pTooltip ) { return false; }
	//  [12/8/2015 gbgim];
	virtual bool GetSubTooltip(GFx::Value* pTooltipInfo) {return false;}
	virtual bool GetTrade( std::string& strTrade ) { return false; }
	virtual bool GetAddSlotCount( int& nCount ) { return false; }
	virtual bool GetAddSlotBinding( std::string& strBinding ) { return false; }
	virtual void HoldStart( bool bLock ) {}
	virtual bool PreDropSlotProc( GfxSlotBase* pTargetSlot, bool bHoldLock, bool bTargetLock ) { return true; }
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock ) { return true; }
	virtual void DragSlotProc( GfxSlotBase* pTargetSlot ) {}
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) {}
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) {}

public :
	void SetSlot( GfxSlotBase* pSlot ) { m_pSlot = pSlot; }
	void SetTooltipType(EMGFX_TOOLTIP_TYPE emTooltipType) { m_emParentTooltipType = emTooltipType; }
	void SetNID( SNATIVEID sNid ) { m_sNativeID = sNid; }
};

//-----------------------------------------------------------------------------------------
// 아이템 베이스 슬롯
class GfxItemBaseSlot : public GfxSlotBase
{
public :
	GfxItemBaseSlot();
	virtual ~GfxItemBaseSlot();

protected :
	SITEMCUSTOM m_sItemCustom;

	bool m_bNew;
	bool m_bUseCompare;

public :
	virtual EMGFX_SLOT_TYPE GetBaseType() { return EMGFX_SLOT_ITEMBASE; }
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_ITEMBASE; }

public :
	virtual void GetValue( GFx::Movie* pMovie, GFx::Value* pValue );
	virtual void Reset();
	virtual bool GetTooltip( GFx::Value* pTooltip );
	//  [12/8/2015 gbgim];
	virtual bool GetSubTooltip( GFx::Value* pTooltipInfo );
	virtual void HoldStart( bool bLock ) {}
	virtual bool PreDropSlotProc( GfxSlotBase* pTargetSlot, bool bHoldLock, bool bTargetLock ) { return true; }
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock ) { return false; }
	virtual void DragSlotProc( GfxSlotBase* pTargetSlot ) {}
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetItemCustom( const SITEMCUSTOM& sItemCustom )	{ m_sItemCustom = sItemCustom; }
	const SITEMCUSTOM& GetItemCustom() const				{ return m_sItemCustom; }
	SITEMCUSTOM& GetItemCustom_NoneConst()					{ return m_sItemCustom; }

	void SetNew( bool bNew ) { m_bNew = bNew; }
	bool GetNew() { return m_bNew; }

private:
	void GetItemTooltipSubInfo(GFx::Value* pSubInfo);
};

//-----------------------------------------------------------------------------------------
// 스킬 베이스 슬롯
class GfxSkillBaseSlot : public GfxSlotBase
{
	
public :
	GfxSkillBaseSlot();
	virtual ~GfxSkillBaseSlot();

protected :
	SNATIVEID	m_sNativeID;
	int			m_nLevel;

public :
	virtual EMGFX_SLOT_TYPE GetBaseType() { return EMGFX_SLOT_SKILLBASE; }
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_SKILLBASE; }

public :
	virtual void GetValue( GFx::Movie* pMovie, GFx::Value* pValue );
	virtual void Reset();
	virtual bool GetTooltip( GFx::Value* pTooltip );
	virtual bool GetSubTooltip( GFx::Value* pSubTooltip );
	virtual bool GetTrade( std::string& strTrade ) { return false; }
	virtual void HoldStart( bool bLock ) {}
	virtual bool PreDropSlotProc( GfxSlotBase* pTargetSlot, bool bHoldLock, bool bTargetLock ) { return true; }
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock ) { return true; }
	virtual void DragSlotProc( GfxSlotBase* pTargetSlot ) {}
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) {}
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) {}

public :
	void SetNativeID( const SNATIVEID& sID ) { m_sNativeID = sID; }
	SNATIVEID GetNativeID() { return m_sNativeID; }

	void SetLevel( int nLevel ) { m_nLevel = nLevel; }
	int GetLevel() { return m_nLevel; }

private :
	void GetSkillTooltipSubInfo( const SNATIVEID& sID, WORD wLevel, GFx::Value* pSubInfo );
};

//-----------------------------------------------------------------------------------------
// 인벤토리 슬롯
class GfxInventorySlot : public GfxItemBaseSlot
{
public :
	GfxInventorySlot();
	virtual ~GfxInventorySlot();

protected :
	int m_nPosX;
	int m_nPosY;

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_INVENTORY; }

public :
	virtual bool GetTooltip( GFx::Value* pTooltip );
	virtual void HoldStart( bool bLock );
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void DragSlotProc( GfxSlotBase* pTargetSlot );
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

protected :
	bool IsPrivateMarketOpener();

public :
	void SetPosition( int nPosX, int nPosY );
	void GetPosition( int& nPosX, int& nPosY );
};

//-----------------------------------------------------------------------------------------
// 캐릭터 정보 슬롯
class GfxCharInfoSlot : public GfxItemBaseSlot
{
public :
	GfxCharInfoSlot();
	virtual ~GfxCharInfoSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_CHARINFO; }

protected :
	int m_nSlot;

public :
	virtual void HoldStart( bool bLock );
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
	virtual bool GetTooltip( GFx::Value* pTooltip );

private :
	void RegSlotTo();

public :
	void SetSlot( int nSlot ) { m_nSlot = nSlot; }
	int GetSlot() { return m_nSlot; }
};

//-----------------------------------------------------------------------------------------
// 인벤토리 은행 슬롯
class GfxInvenBankSlot : public GfxItemBaseSlot
{
public :
	GfxInvenBankSlot();
	virtual ~GfxInvenBankSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_INVEN_BANK; }

protected :
	int m_nPosX;
	int m_nPosY;

public :
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetPosition( int nPosX, int nPosY );
	void GetPosition( int& nPosX, int& nPosY );
};

//-----------------------------------------------------------------------------------------
// 인벤토리 위시리스트 슬롯
class GfxInvenWishListSlot : public GfxItemBaseSlot
{
public :
	GfxInvenWishListSlot();
	virtual ~GfxInvenWishListSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_INVEN_WISHLIST; }

protected :
	int m_nPosX;
	int m_nPosY;

public :
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetPosition( int nPosX, int nPosY );
	void GetPosition( int& nPosX, int& nPosY );
};

//-----------------------------------------------------------------------------------------
// 인벤토리 휴지통 슬롯
class GfxInvenTrashSlot : public GfxItemBaseSlot
{
public :
	GfxInvenTrashSlot();
	virtual ~GfxInvenTrashSlot();

protected :
	int m_nPosX;
	int m_nPosY;

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_INVEN_TRASH; }

public :
	virtual void Reset();
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
};

//-----------------------------------------------------------------------------------------
// 스킬 퀵슬롯
class GfxSkillQuickSlot : public GfxSkillBaseSlot
{
public :
	GfxSkillQuickSlot();
	virtual ~GfxSkillQuickSlot();

protected :
	// 현재 등록된 퀵슬롯 인덱스
	int m_nIndex;

	// 이전 스킬 ID
	SNATIVEID m_sPrevNativeID;
	// 이전 등록된 퀵슬롯 인덱스
	int m_nPrevIndex;

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_SQS; }

public :
	virtual bool GetTooltip( GFx::Value* pTooltip );
	virtual void HoldStart( bool bLock );
	virtual bool PreDropSlotProc( GfxSlotBase* pTargetSlot, bool bHoldLock, bool bTargetLock );
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetIndex( int nIndex ) { m_nIndex = nIndex; }
	int GetIndex() { return m_nIndex; }

	SNATIVEID GetPrevNativeID() { return m_sPrevNativeID; }
	int GetPrevIndex() { return m_nPrevIndex; }
};

//-----------------------------------------------------------------------------------------
// 아이템 퀵슬롯
class GfxItemQuickSlot : public GfxItemBaseSlot
{
public :
	GfxItemQuickSlot();
	virtual ~GfxItemQuickSlot();

protected :
	int m_nIndex;

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_IQS; }

public :
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetIndex( int nIndex ) { m_nIndex = nIndex; }
	int GetIndex() { return m_nIndex; }
};

//-----------------------------------------------------------------------------------------
// 미니게임 : 주사위 보상 아이템 슬롯;
class GfxMiniGameDiceRewardSlot : public GfxItemBaseSlot
{
public:
	GfxMiniGameDiceRewardSlot();
	virtual ~GfxMiniGameDiceRewardSlot();

protected:
	int m_nIndex;

public:
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_MINIGAME_DICE_REWARD; }

public:
	void SetIndex( int nIndex ) { m_nIndex = nIndex; }
	int GetIndex() { return m_nIndex; }
};
//-----------------------------------------------------------------------------------------
// 스킬창 슬롯
class GfxSkillSlot : public GfxSkillBaseSlot
{
public :
	GfxSkillSlot();
	virtual ~GfxSkillSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_SKILL; }

public :
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
};

//-----------------------------------------------------------------------------------------
// 탈것정보창 슬롯
class GfxVehicleSlot : public GfxItemBaseSlot
{
public :
	GfxVehicleSlot();
	virtual ~GfxVehicleSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_VEHICLE; }

public :
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

private :
	EMSUIT Binding2Suit();
};

//-----------------------------------------------------------------------------------------
// NPC 판매 아이템 슬롯
class GfxNpcMarketSlot : public GfxItemBaseSlot
{
public :
	GfxNpcMarketSlot();
	virtual ~GfxNpcMarketSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_NPCMARKET; }

private :
	int m_nPosX;
	int m_nPosY;

public :
	virtual bool GetTooltip( GFx::Value* pTooltip );
	//  [12/8/2015 gbgim];
	virtual bool GetSubTooltip(GFx::Value* pTooltipInfo);
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetPosition( int nPosX, int nPosY );
	void GetPosition( int& nPosX, int& nPosY );
};

//-----------------------------------------------------------------------------------------
// NPC 카트 아이템 슬롯
class GfxNpcCartSlot : public GfxItemBaseSlot
{
public :
	GfxNpcCartSlot();
	virtual ~GfxNpcCartSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_NPCCART; }

private :
	WORD m_wIndex;

public :
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetIndex( WORD wIndex ) { m_wIndex = wIndex; }
};

//-----------------------------------------------------------------------------------------
// 제작 슬롯;
class GfxProductSlot : public GfxItemBaseSlot
{
public :
	GfxProductSlot();
	virtual ~GfxProductSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_PRODUCT; }
};

//-----------------------------------------------------------------------------------------
// 포인트샾 슬롯;
class GfxPointShopSlot : public GfxItemBaseSlot
{
public :
	GfxPointShopSlot();
	virtual ~GfxPointShopSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_POINTSHOP; }

private :
	int m_nPosX;
	int m_nPosY;

public :
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetPosition( int nPosX, int nPosY );
	void GetPosition( int& nPosX, int& nPosY );
};

//-----------------------------------------------------------------------------------------
// 포인트샾 카트 슬롯;
class GfxPointShopCartSlot : public GfxItemBaseSlot
{
public :
	GfxPointShopCartSlot();
	virtual ~GfxPointShopCartSlot();

protected :
	int m_nIndex;

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_POINTSHOP_CART; }
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public:
	void SetIndex( int nIndex ) { m_nIndex = nIndex; }
	int GetIndex() { return m_nIndex; }
};

//---------------------------------------------------------------------------------------
// 거래 슬롯
class GfxTradeSlot : public GfxItemBaseSlot
{
public :
	GfxTradeSlot();
	virtual ~GfxTradeSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_TRADE; }

private :
	int m_PosX;
	int m_PosY;
	bool m_bMySlot;

public :
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetPosition( int nPosX, int nPosY );
	void GetPosition( int& nPosX, int& nPosY );
	void SetMySlot( bool bMySlot ) { m_bMySlot = bMySlot; }
};

//---------------------------------------------------------------------------------------
// 개인상점검색 슬롯
class GfxPMSPointTradeSlot : public GfxItemBaseSlot
{
public :
	GfxPMSPointTradeSlot();
	virtual ~GfxPMSPointTradeSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_PMSPOINTTRADE; }

public :
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
};

//-----------------------------------------------------------------------------------------
// 우편 슬롯;
class GfxPostBoxSlot : public GfxItemBaseSlot
{
public :
	GfxPostBoxSlot();
	virtual ~GfxPostBoxSlot();

protected :
	int m_nPosX;
	int m_nPosY;

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_POSTBOX; }

public :
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
};

class GfxPostBoxSubSlot : public GfxItemBaseSlot
{
public :
	GfxPostBoxSubSlot();
	virtual ~GfxPostBoxSubSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_POSTBOX_SUB; }
};

//-----------------------------------------------------------------------------------------
// 정보보기 슬롯
class GfxReferCharSlot : public GfxItemBaseSlot
{
public :
	GfxReferCharSlot();
	virtual ~GfxReferCharSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_REFERCHAR; }

public:
	virtual void GetValue( GFx::Movie* pMovie, GFx::Value* pValue );
	virtual bool GetTooltip( GFx::Value* pTooltip );

protected :
	int m_nSlot;

public :
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetSlot( int nSlot ) { m_nSlot = nSlot; }
	int GetSlot() { return m_nSlot; }
};

//-----------------------------------------------------------------------------------------
// 정보보기 스킬 슬롯
class GfxReferCharSkillSlot : public GfxSkillBaseSlot
{
public :
	GfxReferCharSkillSlot() {}
	virtual ~GfxReferCharSkillSlot() {}

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_REFERCHAR_SKILL; }
};

//---------------------------------------------------------------------------------------
//펫 장비 슬롯
class GfxPetSlot : public GfxItemBaseSlot
{

public:
	GfxPetSlot();
	virtual ~GfxPetSlot();

protected :
	int m_nSlot;

public:
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_PET; }
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetSlot( int nSlot ) { m_nSlot = nSlot; }
	int GetSlot() { return m_nSlot; }
};

//-----------------------------------------------------------------------------------------
// 클럽창고 슬롯
class GfxClubStorageSlot : public GfxItemBaseSlot
{
public :
	GfxClubStorageSlot();
	virtual ~GfxClubStorageSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_CLUBSTORAGE; }

protected :
	int m_nSlot;
	int m_PosX;
	int m_PosY;
	DWORD m_Channel;	

public :
	//virtual void GetValue( GFx::Movie* pMovie, GFx::Value* pValue );
	void SetSlot( int nSlot ) { m_nSlot = nSlot; }
	void SetItem(const SITEMCUSTOM &_sItemCustom)
	{
		m_sItemCustom = _sItemCustom;
	}
	int GetSlot() { return m_nSlot; }
	virtual void HoldStart( bool bLock );
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );

	void GetPosition(int &_PosX, int &_PosY)
	{
		_PosX = m_PosX;
		_PosY = m_PosY;
	}

	void SetPosition(int _PosX, int _PosY)
	{
		m_PosX = _PosX;
		m_PosY = _PosY;
	}

	void SetChannel(int _Channel)
	{
		m_Channel = _Channel;
	}
};

//-----------------------------------------------------------------------------------------
// 출석부 보상슬롯;
class GfxAttendanceAwardSlot : public GfxItemBaseSlot
{
public :
	GfxAttendanceAwardSlot();
	virtual ~GfxAttendanceAwardSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_ATTENDANCE_AWARD; }
};

//-----------------------------------------------------------------------------------------
// 특별활동 판매 아이템 슬롯;
class GfxActivitySalesSlot : public GfxItemBaseSlot
{
public :
	GfxActivitySalesSlot();
	virtual ~GfxActivitySalesSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_ACTIVITY_SALES; }
};

//-----------------------------------------------------------------------------------------
// 개조카드 아이템 슬롯
class GfxRebuildCardItemSlot : public GfxItemBaseSlot
{
public :
	GfxRebuildCardItemSlot();
	virtual ~GfxRebuildCardItemSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_REBUILD_CARD_ITEM; }

public :
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
};

//-----------------------------------------------------------------------------------------
// // 개조카드 스템프 슬롯
class GfxRebuildCardStempSlot : public GfxItemBaseSlot
{
public :
	GfxRebuildCardStempSlot();
	virtual ~GfxRebuildCardStempSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_REBUILD_CARD_STEMP; }

public :
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
};

//-----------------------------------------------------------------------------------------
// 특별활동 판매 아이템 슬롯;
class GfxItemPreviewSlot : public GfxItemBaseSlot
{
public :
	GfxItemPreviewSlot();
	virtual ~GfxItemPreviewSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_ITEM_PREVIEW; }
};

//---------------------------------------------------------------------------------------
// 개인상점 슬롯
class GfxPrivateMarketSlot : public GfxItemBaseSlot
{
public :
	GfxPrivateMarketSlot();
	virtual ~GfxPrivateMarketSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_PRIVATEMARKET; }

private :
	int m_PosX;
	int m_PosY;

public :
	virtual bool GetTooltip( GFx::Value* pTooltip );
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetPosition( int nPosX, int nPosY );
	void GetPosition( int& nPosX, int& nPosY );
};

//---------------------------------------------------------------------------------------
// 개인 상점 검색 슬롯
class GfxPrivateMarketSearchSlot : public GfxItemBaseSlot
{
public :
	GfxPrivateMarketSearchSlot();
	virtual ~GfxPrivateMarketSearchSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_PRIVATEMARKETSEARCH; }

public :
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
};

//---------------------------------------------------------------------------------------
// 락커 슬롯
class GfxLockerSlot : public GfxItemBaseSlot
{
public :
	GfxLockerSlot();
	virtual ~GfxLockerSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_LOCKER; }

private :
	int m_nIndex;
	int m_nPosX;
	int m_nPosY;

public :
	virtual void HoldStart( bool bLock );
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock );
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );

public :
	void SetIndex( int nIndex ) { m_nIndex = nIndex; }
	int GetIndex() { return m_nIndex; }

	void SetPosition( int nPosX, int nPosY );
	void GetPosition( int& nPosX, int& nPosY );
};

//-----------------------------------------------------------------------------------------
// 버프 슬롯
class GfxBuffSlot : public GfxSkillBaseSlot
{
public :
	GfxBuffSlot();
	virtual ~GfxBuffSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_BUFF; }

public :
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
};

//-----------------------------------------------------------------------------------------
// 세트 아이템 슬롯
class GfxSetItemSlot : public GfxItemBaseSlot
{
public :
	GfxSetItemSlot();
	virtual ~GfxSetItemSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_SETITEM; }

public :
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
};

//-----------------------------------------------------------------------------------------
// 소환수 슬롯;
class GfxSummonStateSlot : public GfxSlotBase
{
private:
	std::string m_strIconPath;
	INT	m_nIconIdxX;
	INT	m_nIconIdxY;

public:
	GfxSummonStateSlot(void);
	virtual ~GfxSummonStateSlot(void);

public:
	void GetValue( GFx::Movie* pMovie, GFx::Value* pValue );
	// 슬롯 데이터 리셋
	virtual void Reset() {}
	virtual bool GetTooltip( GFx::Value* pTooltip ) { return false; }
	virtual bool GetTrade( std::string& strTrade ) { return false; }

public :
	virtual void HoldStart( bool bLock ) {}
	virtual bool PreDropSlotProc( GfxSlotBase* pTargetSlot, bool bHoldLock, bool bTargetLock ) { return false; }
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock ) { return false; }
	virtual void DragSlotProc( GfxSlotBase* pTargetSlot ) {}
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) {}
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) {}
	virtual bool GetSubTooltip(GFx::Value* pTooltipInfo) { return false; }

public:
	void SetIconData(std::string strIconPath, INT nIconIdxX, INT nIconIdxY);
	virtual EMGFX_SLOT_TYPE GetType(void)	{ return EMGFX_SLOT_SUMMONSTATE; }
};

//-----------------------------------------------------------------------------------------
// 파티 분배
class GfxPartyDistributionSlot : public GfxItemBaseSlot
{
public:
	INT		m_nSlotIndex;

public :
	GfxPartyDistributionSlot();
	virtual ~GfxPartyDistributionSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_PARTYDISTRIBUTION; }
};

//-----------------------------------------------------------------------------------------
// 아이템 링크 정보
class GfxItemLinkInfoSlot : public GfxItemBaseSlot
{
public :
	GfxItemLinkInfoSlot();
	virtual ~GfxItemLinkInfoSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_ITEMLINKINFO; }

public :
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift );
};
