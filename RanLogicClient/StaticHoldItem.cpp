#include "pch.h"

#include "../RanLogic/Item/GLItemCustom.h"

#include "./StaticHoldItem.h"

#include "./GfxInterfaceBase.h"

namespace holditem
{
    SITEMCUSTOM s_HoldItem;

    HOLD_PLACE_TYPE s_HoldPlace = NONE;

    WORD s_HoldPosX, s_HoldPosY;

    DWORD s_Data0;
    DWORD s_Data1;
	bool s_ReserveDelete;

	std::string strHoldSlotBinding = "";

    void HoldItem( HOLD_PLACE_TYPE _HoldPlace, const SITEMCUSTOM& _HoldItem, WORD _PosX, WORD _PosY, bool bReserveDelete )
    {
        s_HoldPlace = _HoldPlace;

        s_HoldItem = _HoldItem;

        s_HoldPosX = _PosX;
        s_HoldPosY = _PosY;
		
		s_ReserveDelete = bReserveDelete;
    }

    const SITEMCUSTOM& GetHoldItem()
    {
        return s_HoldItem;
    }

    void ReleaseHoldItem()
    {
        s_HoldPlace = NONE;
        s_HoldItem = SITEMCUSTOM();
		s_ReserveDelete = false;
		strHoldSlotBinding = "";

		GfxInterfaceBase::GetInstance()->SlotDragStop();
    }

    bool HaveHoldItem()
    {
        return (GetHoldItem().GetNativeID() != NATIVEID_NULL());
    }

    bool IsHoldPlace( HOLD_PLACE_TYPE _HoldPlace )
    {
        return ( s_HoldPlace == _HoldPlace );
    }

    HOLD_PLACE_TYPE WhenHoldItem()
    {
        return s_HoldPlace;
    }

    WORD GetHoldPosX()
    {
        return s_HoldPosX;
    }

    WORD GetHoldPosY()
    {
        return s_HoldPosY;
    }

    void SetData( DWORD _Data0 /*= 0*/, DWORD _Data1 /*= 0 */ )
    {
        s_Data0 = _Data0;
        s_Data1 = _Data1;
    }

    void GetData( DWORD& _OutData0, DWORD& _OutData1 )
    {
        _OutData0 = s_Data0;
        _OutData1 = s_Data1;
    }

	void SetReserveDelete(bool bReserve)
	{
		s_ReserveDelete = bReserve;
	}

	bool IsReserveDelete(void)
	{
		return s_ReserveDelete;
	}

	void SetHoldSlotBinding( const std::string& strBinding )
	{
		strHoldSlotBinding = strBinding;
	}

	const std::string& GetHoldSlotBinding()
	{
		return strHoldSlotBinding;
	}
};