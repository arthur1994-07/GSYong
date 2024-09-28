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
// ���� ���̽� Ŭ����
// Ÿ�Ժ��� ��ӹ޾� ��������.
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
	// ���� ������ ����
	virtual void GetValue( GFx::Movie* pMovie, GFx::Value* pValue ) = 0;
	// ���� ������ ����
	virtual void Reset() = 0;
	// ����(HTML), ��ƿ��� �ؽ�Ʈ�� ���鵵�� ��� �Լ� ȣ��
	virtual bool GetTooltip( GFx::Value* pTooltip ) = 0;
	// ���� ����
	virtual bool GetSubTooltip(GFx::Value* pTooltipInfo) = 0;
	// ���� �߰� ���� ����
	virtual bool GetAddSlotCount( int& nCount );
	// ���� �߰� ���� ���ε� ID
	virtual bool GetAddSlotBinding( std::string& strBinding );
	// ���� ��ȯ ���� ����
	virtual bool GetTradeSlotCount( int& nCount );
	// ���� ��ȯ ���� ���ε� ID
	virtual bool GetTradeSlotBinding( std::string& strBinding );

public :
	// Ȧ�� ����
	virtual void HoldStart( bool bLock ) = 0;
	// ��� ������ ��ó��, �巡�׷� �̵����� ����(Ÿ�� ����)�� ���ƿ�. (false ��ȯ�� PostDropSlotProc�� ó������ �ʴ´�.)
	virtual bool PreDropSlotProc( GfxSlotBase* pTargetSlot, bool bHoldLock, bool bTargetLock ) = 0;
	// ��� ������ ��ó��, �巡�׷� �̵���Ų ����(Ȧ�� ����)�� ���ƿ�.
	virtual bool PostDropSlotProc( GfxSlotBase* pHoldSlot, bool bHoldLock, bool bTargetLock ) = 0;
	// �巡�� ó��
	virtual void DragSlotProc( GfxSlotBase* pTargetSlot ) = 0;
	// ���� ���콺 Ŭ��
	virtual void LeftMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) = 0;
	// ������ ���콺 Ŭ��
	virtual void RightMouseClick( bool bLock, bool bCtrl, bool bAlt, bool bShift ) = 0;
};

//-----------------------------------------------------------------------------------------
// ���� ����
class GfxTooltipSlot : public GfxSlotBase
{
public :
	GfxTooltipSlot();
	virtual ~GfxTooltipSlot();

protected :
	GfxSlotBase* m_pSlot;
	// ���Կ��� ��µ� ������ �ƴ϶�� �Ʒ� ���̵� ������;
	// ��ó�� �ָŸ�ȣ�ϴ�;
	// �ֻ��� �θ� ���� Ÿ������ �к��ϱ⵵ �ָ���;
	// ������ ���Կ��� ��ų ���� ����ؾ��ϴ� ��쵵 ����;
	// ���� ����Ѱ� ����Ÿ������ Ȯ���ϴ°� ����!;
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
// ������ ���̽� ����
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
// ��ų ���̽� ����
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
// �κ��丮 ����
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
// ĳ���� ���� ����
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
// �κ��丮 ���� ����
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
// �κ��丮 ���ø���Ʈ ����
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
// �κ��丮 ������ ����
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
// ��ų ������
class GfxSkillQuickSlot : public GfxSkillBaseSlot
{
public :
	GfxSkillQuickSlot();
	virtual ~GfxSkillQuickSlot();

protected :
	// ���� ��ϵ� ������ �ε���
	int m_nIndex;

	// ���� ��ų ID
	SNATIVEID m_sPrevNativeID;
	// ���� ��ϵ� ������ �ε���
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
// ������ ������
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
// �̴ϰ��� : �ֻ��� ���� ������ ����;
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
// ��ųâ ����
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
// Ż������â ����
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
// NPC �Ǹ� ������ ����
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
// NPC īƮ ������ ����
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
// ���� ����;
class GfxProductSlot : public GfxItemBaseSlot
{
public :
	GfxProductSlot();
	virtual ~GfxProductSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_PRODUCT; }
};

//-----------------------------------------------------------------------------------------
// ����Ʈ�� ����;
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
// ����Ʈ�� īƮ ����;
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
// �ŷ� ����
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
// ���λ����˻� ����
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
// ���� ����;
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
// �������� ����
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
// �������� ��ų ����
class GfxReferCharSkillSlot : public GfxSkillBaseSlot
{
public :
	GfxReferCharSkillSlot() {}
	virtual ~GfxReferCharSkillSlot() {}

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_REFERCHAR_SKILL; }
};

//---------------------------------------------------------------------------------------
//�� ��� ����
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
// Ŭ��â�� ����
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
// �⼮�� ���󽽷�;
class GfxAttendanceAwardSlot : public GfxItemBaseSlot
{
public :
	GfxAttendanceAwardSlot();
	virtual ~GfxAttendanceAwardSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_ATTENDANCE_AWARD; }
};

//-----------------------------------------------------------------------------------------
// Ư��Ȱ�� �Ǹ� ������ ����;
class GfxActivitySalesSlot : public GfxItemBaseSlot
{
public :
	GfxActivitySalesSlot();
	virtual ~GfxActivitySalesSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_ACTIVITY_SALES; }
};

//-----------------------------------------------------------------------------------------
// ����ī�� ������ ����
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
// // ����ī�� ������ ����
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
// Ư��Ȱ�� �Ǹ� ������ ����;
class GfxItemPreviewSlot : public GfxItemBaseSlot
{
public :
	GfxItemPreviewSlot();
	virtual ~GfxItemPreviewSlot();

public :
	virtual EMGFX_SLOT_TYPE GetType() { return EMGFX_SLOT_ITEM_PREVIEW; }
};

//---------------------------------------------------------------------------------------
// ���λ��� ����
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
// ���� ���� �˻� ����
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
// ��Ŀ ����
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
// ���� ����
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
// ��Ʈ ������ ����
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
// ��ȯ�� ����;
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
	// ���� ������ ����
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
// ��Ƽ �й�
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
// ������ ��ũ ����
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
